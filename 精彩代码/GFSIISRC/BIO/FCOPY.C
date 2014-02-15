/* --------------------------------------------------------------------
	FILENAME:	fcopy.c
	UPDATE:		1993.4.21
	AUTHOR:         Ma Shiqing
	MODIFIED:	Bob Kong.
	USED BY:	This is a general-purposed routine.
	FUNCTION:	Offer a function for copying source file to target file.
   -------------------------------------------------------------------- */

#include <io.h>
#include <dir.h>
#include <dos.h>
#include <alloc.h>
#include <stdlib.h>
#include <ctype.h>

#include <def.inc>
#include <func.inc>

#define BLOCK 4096				/* BLOCK must be >=0x200 */

static UI Disp_sx, Disp_sy;
static UC Bk_color, Char_color;
static UC Disp_info = FALSE;	/* If calling set_fcopy_mode(), this flag
								   will be setted. The following calling
								   of fcopy() will reset it.			 */

/* ----------------------------------------------------------------------
	FUNCTION:       Obtained the disk driver No from filepath
	CALLS:		None.
	CALLED BY:      General-purpose function for universal use.
	INPUT:		filename
	OUTPUT:		None.
	RETURN:		0xFF:	Wrong driver specified.
				0 -- A: driver.
				1 -- B: driver.
				2 -- C: driver.
				...
   ----------------------------------------------------------------------- */
static UC get_diskspec(UC filename[64])
{
UC   *tmp_p;
UC   a_char;

	tmp_p = filename;
	while ( *tmp_p==' ')
		tmp_p++;
	a_char = *tmp_p;

	if ( *(tmp_p+1) != ':' )
	  return (getdisk());

	a_char = toupper(a_char);
	if (a_char<'A' || a_char>'Z')
		return (0xFF);
	return (a_char-'A');
}
/* --------------------------------------------------------------------
	FUNCTION:	Set the copying information to be displayed, and
				the position at which the information be displayed.
	INPUT:		(x, y) -- The position coordinate.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void set_fcopy_mode(UI x, UI y, UC bk_color, UC char_color)
{
	Disp_sx = x;
	Disp_sy = y;
	Bk_color = bk_color;
	Char_color = char_color;
	Disp_info = TRUE;
}

/* ---------------------------------------------------------------------
	FUNCTION:	Copy the target file <tfile> to source file <sfile>.
	INPUT:		sfile:	The source file specification.
			tfile:  The target file specification.
	OUTPUT:		None.
	RETURN:		0 -- copying succfully.
				1 -- source error.
					 (source driver invalid or not ready, source file error.)
				2 -- target error.
					 (target filepath incorrect, driver not ready,
				3 -- target disk space not enough.
				4 -- reading source file error, copying failed.
				5 -- writting target file error, copying failed.
   ----------------------------------------------------------------------- */
UC fcopy(UC sfile[64], UC tfile[64])
{
union  REGS inregs, outregs;
struct SREGS segregs;
UI     inhandle;
UI     outhandle;
UC     *buf;

struct dfree dkfree;
UL     file_len, free_len;
UI     loop;
UC     drv;

	drv = get_diskspec(tfile);
	if (drv == 0xFF)
	{
		Disp_info = FALSE;
		return 2; 	            /* target file path is not correct */
	}

	inregs.x.dx = FP_OFF(sfile);
	segregs.ds = FP_SEG(sfile);
	inregs.h.al = 0;
	inregs.h.ah = 0x3D;
	intdosx(&inregs, &outregs, &segregs);	/* open source file */
	if ( outregs.x.cflag )
	{
		Disp_info = FALSE;
		return 1;       		/* open source file failed */
	}
	inhandle = outregs.x.ax;

	file_len = (UL)filelength(inhandle);

	getdfree(drv+1, &dkfree);
	if (dkfree.df_sclus == 0xFFFF)
	{
		inregs.x.bx = inhandle;
		inregs.h.ah = 0x3E;
		intdos(&inregs, &outregs); 	/* close source file */

		Disp_info = FALSE;
		return 2;                  /* get target disk space failed */
	}

	free_len  = (UL)dkfree.df_avail*(UL)dkfree.df_sclus;
	free_len *= (UL)dkfree.df_bsec;

	if ( free_len <= (file_len+32 ))
	{
		inregs.x.bx = inhandle;
		inregs.h.ah = 0x3E;
		intdos(&inregs, &outregs); 	/* close source file */

		Disp_info = FALSE;
		return 3;    			/* target disk space is not enough */
	}

	inregs.x.dx = FP_OFF(tfile);
	segregs.ds = FP_SEG(tfile);
	inregs.x.cx = 0;
	inregs.h.ah = 0x3C;
	intdosx(&inregs, &outregs, &segregs);	/* open target file */
	if ( outregs.x.cflag!=0 )
	{
		inregs.x.bx = inhandle;
		inregs.h.ah = 0x3E;
		intdos(&inregs, &outregs); 	/* close source file */

		Disp_info = FALSE;
		return 2;       		/* open target file error !  */
	}
	outhandle = outregs.x.ax;

	if (Disp_info)
		disp_str(Disp_sx, Disp_sy, "0  ге", Bk_color, Char_color);

	buf = (UC *)mem_alloc(BLOCK);
	segregs.ds = FP_SEG(buf);
	inregs.x.dx = FP_OFF(buf);

	loop = 0;
	while (TRUE)
	{
		inregs.x.bx = inhandle;
		inregs.x.cx = BLOCK;
		inregs.h.ah = 0x3F;
		intdosx(&inregs, &outregs, &segregs);
		if ( outregs.x.cflag!=0 )
		{
			inregs.x.bx = inhandle;
			inregs.h.ah = 0x3E;
			intdos(&inregs, &outregs); 	/* close source file */

			inregs.x.bx = outhandle;
			intdos(&inregs, &outregs);	/* close target file */

			farfree(buf);
			Disp_info = FALSE;
			return 4;   			/* read source file error. */
		}
		else
		{
			inregs.x.cx = outregs.x.ax;
			inregs.x.bx = outhandle;
			inregs.h.ah = 0x40;
			intdosx(&inregs, &outregs, &segregs);
			if ( outregs.x.cflag!=0 )
			{
				inregs.x.bx = inhandle;
				inregs.h.ah = 0x3E;
				intdos(&inregs, &outregs); 	/* close source file */

				inregs.x.bx = outhandle;
				intdos(&inregs, &outregs);	/* close target file */

				farfree(buf);
				Disp_info = FALSE;
				return 5;       	/* write target file error. */
			}
			if (Disp_info)
			{
				loop++;
				outf(Disp_sx, Disp_sy, Bk_color, Char_color,
					 "%-3d", min( (loop*(UL)BLOCK*100L)/file_len, 100) );
			}
		}
		if (outregs.x.ax < BLOCK)
			break;
	}

	farfree(buf);
	Disp_info = FALSE;

	inregs.x.bx = inhandle;
	inregs.h.ah = 0x3E;
	intdos(&inregs, &outregs);  		/* close source file */

	inregs.x.bx = outhandle;
	intdos(&inregs, &outregs);          /* close target file */
	if ( outregs.x.cflag!=0 )
		return 2;
	return 0;
}
