/* -------------------------------------------------------------------
	FILENAME:	initdisk.c
	FUNCTION:	Floppy disk(5.25" or 3.5" HD) initiation tool.
	DATE:		25/4/93
	AUTHOR:		LIU GUANG YU
   ------------------------------------------------------------------- */
#include	<dos.h>
#include	<stdio.h>
#include	<alloc.h>
#include	<string.h>
#include	<graphics.h>

#include	<def.Inc>
#include	<func.inc>

#define WARN_WIN_X	300
#define WARN_WIN_Y	360

#define	MAX_TRACKS	80 			/*  total track number of a disk. */
#define	RE_READ		4          	/*  repeat times when error occurred. */

/* ---------------------------------------------------------------------- */
#define RECOVER_1E		in.x.ax = 0x251e;\
						seg.ds = FP_SEG(old_int1E);\
						in.x.dx = FP_OFF(old_int1E);\
						int86x (0x21, &in, &out, &seg)
/* ---------------------------------------------------------------------- */

static  UC init_track(UC driver, UC side, UC track, UC sector_num);
		/* format a track of a disk,
		   if Ok return 0, else return the error code */

static  void write_FAT(UC driver, UC disk_type, UC track_mark[2][MAX_TRACKS]);
		/*  write the disk's FAT */

static  UC err_handler(UI win_sx, UI win_sy, UC err_code);
		/*  If formatting or testing a trace error, a message window
			poped out, and get the user's selection: Abort or Retry */

static UI Disp_sx, Disp_sy;
       /* Coord. at where displaying the prompt information: the finished percent */
static UC Bk_color, Char_color;
       /* Colors in which displaying the prompt */
static UC Disp_info = TRUE;
       /* Display the prompt or not.  (TRUE or FALSE) */

/* ---------------------------------------------------------------------
	FUCTION:	Initiate a 5.25" HD or 3.5" HD floppy disk.
	CALLS:		None.
	CALLED BY:	General-purposed routine
	INPUT:		driver 	0 -- A:
  			        1 -- B:
				(x, y)	coordinate at which the prompt information echoed.
	RETURN:		0 -- formatting successfully.
				1 -- formatting failed
   --------------------------------------------------------------------- */
UC	init_disk(UC driver)
{
#include "bootdata.c"
static  UC *type_invalid = " ＃ 驱动器类型不对, 不能用于本系统。\n    按任意键返回 ．．．";
							/* error message: invalid disk driver's type. */
static  UC *track0_bad   = " ＃ 磁盘０道坏，格式化失败。\n    按任意键返回 ．．．";
							/* error message: track is bad. */
UC		track_mark[2][MAX_TRACKS];
							/* recording buffer of the resoult:
							   formatting or testing the 2 side's 80 tracks. */
void 	*old_int1E;			/* save the old int1E's interrupt vector */
UC		new1E_data[12];		/* keep the new INT 1E's data */
UC      disk_type;			/* it may be 0 (5.25 HD) or 1 (3.5 HD) */

union	REGS	in,	out;    /* Bios	interrput union varies */
struct	SREGS	seg;		/* Bios interrupt struct varies */
UC		count;				/* a counter of repeat times when error occurred */
UC		sector_num;			/* sectors/track: 15(5.25" HD) or 18 (3.5" HD) */
UC		error;				/* get return code of init_track(). */
UC		result;				/* get return code of err_handler().*/
UC		track, side;

        pop_back(210, 190, 310, 230, 7); /* big frame */
        draw_back(235, 198, 285, 222, 11);
        set_init_mode(340, 202, 11, LIGHTRED);

	if(driver>1)				/* if NOT A: or B:, return 1 */
	{
		sound_alarm();
		Disp_info = TRUE;
                rid_pop();
		return 1;
	}

	in.h.ah = 0;
	int86(0x13, &in, &out); 		/* reset the disk driver */

	in.h.ah = 0x08;
	in.h.dl = driver;
	int86x (0x13, &in, &out, &seg);	/* get the arguements of current disk */

	if ( out.h.bl==0x02 )			/* if 5.25" HD soft disk */
		disk_type = 0;
	else
	if ( out.h.bl==0x04 )			/* if 3.5" HD soft disk */
		disk_type = 1;
	else
	{
		set_msg_color(12, 0, 15);
		msg_win(210, 190, "", type_invalid, 0);
//		msg_win(WARN_WIN_X, WARN_WIN_Y, "", type_invalid, 0);
		Disp_info = TRUE;
                rid_pop();
		return 1;
	}

	sector_num = out.h.cl;
	memcpy(new1E_data, (UC *)MK_FP(seg.es, out.x.di), 12);
	*(new1E_data+8) = 0;

	in.x.ax = 0x351e;
	int86x (0x21, &in, &out, &seg);
	old_int1E = MK_FP(seg.es, out.x.bx);	/* get the old int 1E's vector */

	in.x.ax = 0x251e;
	seg.ds = FP_SEG(new1E_data);
	in.x.dx = FP_OFF(new1E_data);
	int86x (0x21, &in, &out,&seg);		/* set the new int 1E's vector */

	in.h.ah = 0x18;
	in.h.dl = driver;
	in.h.ch = out.h.ch;
	in.h.cl = out.h.cl;
	int86(0x13, &in, &out);				/* set the driver for formatting */

	if (Disp_info)
		disp_str(Disp_sx, Disp_sy, "0 ％", Bk_color, Char_color);

	for (track = 0; track<MAX_TRACKS ; track++)
	{
		for (side = 0; side <2; side++)
		{
			while (TRUE)			/* init a track */
			{
				error = init_track(driver, side, track, sector_num);

				if ( error!=0x03 && error!=0x06 && error!=0x80 )
					break;
				else
				{
					in.h.ah = 0;
					int86(0x13, &in, &out);		/* reset driver */

//					result = err_handler(WARN_WIN_X, WARN_WIN_Y, error);
					result = err_handler(210, 190, error);
					switch(result)
					{
					case	1:
						continue;
					case	2:
						RECOVER_1E;
						Disp_info = TRUE;
                                                rid_pop();
						return 1;
					default:;
					}
				}	/* end of else -- error occurred */
			}		/* end of while */

			if (track==0 && error!=0 )
			{
				set_msg_color(12, 0, 15);
//				msg_win(WARN_WIN_X, WARN_WIN_Y, "", track0_bad, 0);
				msg_win(210, 190, "", track0_bad, 0);

				RECOVER_1E;
				Disp_info = TRUE;
                                rid_pop();
				return 1;
			}

			track_mark[side][track] = error;
			if ( Disp_info )
				outf(Disp_sx, Disp_sy, Bk_color, Char_color,"%-2d", track*100/79);

		}   	/* end of for ( side=0; side<2; side++ ) */
	}           /* end of for ( track=0; track<MAX_TRACKS; track++) */

	if ( disk_type==0 )
	{
		seg.es = FP_SEG(boot5_buf);
		in.x.bx = FP_OFF(boot5_buf);
	}
	else
	{
		seg.es = FP_SEG(boot3_buf);
		in.x.bx = FP_OFF(boot3_buf);
	}
	in.x.ax = 0x0301;
	in.x.cx = 0x0001;
	in.h.dh = 0;
	in.h.dl = driver;

	count = 0;
	while (count<RE_READ)
	{
		int86x(0x13, &in, &out, &seg);
		if (out.x.cflag==0)
		{
			error = 0;
			break;
		}
		else
		{
			error = out.h.ah;
			in.h.ah = 0;
			int86(0x13, &in, &out); 		/* reset the disk driver */
			count++;
		}
	}
	if (error!=0)
	{
		set_msg_color(12, 0, 15);
//		msg_win(WARN_WIN_X, WARN_WIN_Y, "", track0_bad, 0);
		msg_win(210, 190, "", track0_bad, 0);

		RECOVER_1E;
		Disp_info = TRUE;
                rid_pop();
		return 1;
	}

	write_FAT(driver, disk_type, track_mark);

	RECOVER_1E;
	Disp_info = TRUE;
        rid_pop();
	return 0;
}

/* ----------------------------------------------------------------------
	FUNCTION:	format and test a track of a disk.
	CALLS:
	CALLED BY:	called by function: init_disk()
	INPUT:      driver -- the driver specified.
				side -- the cylinder specified.
				track -- the track specified.
				sector_num -- sector number per track.
	OUTPUT:		None.
	RETURN:		0 -- success.
				others -- the error code.
   ---------------------------------------------------------------------- */
static UC init_track( UC driver, UC side, UC track, UC sector_num )
{
static UC 	buff[80] = {/*Cyl, head, sector No, bytes/sector */
				00,	00,	1,	2,	00,	00,	2,	2,	00,	00,	3,	2,
				00,	00,	4,	2,	00,	00,	5,	2,	00,	00,	6,	2,
				00,	00,	7,	2,	00,	00,	8,	2,	00,	00,	9,	2,
				00,	00,	10,	2,	00,	00,	11,	2,	00,	00,	12,	2,
				00,	00,	13,	2,	00, 00,	14,	2,  00,	00,	15,	2,
				00,	00,	16,	2,	00, 00,	17,	2,  00,	00,	18,	2
				};  		/* data of a track, includes: track No, head,
							   sector No, and bytes/sector */
union	REGS	in,	out;
struct	SREGS	seg;
UC 		error;
UC		count;
UC		loop;

	for (loop = 0; loop<sector_num; loop++)
		buff[loop<<2] = track;
	for (loop = 0; loop<sector_num; loop++)
		buff[(loop<<2)+1] = side;

	seg.es = FP_SEG(buff);
	in.x.bx = FP_OFF(buff);
	in.h.al = sector_num;
	in.h.dl = driver;
	in.h.dh = side;
	in.h.ch = track;
	in.h.cl = 1;
	count = 0;
	while ( count<RE_READ )
	{
		in.h.ah = 0x05;
		int86x(0x13, &in, &out, &seg);
		if ( out.x.cflag==0 )
		{
			error = 0;
			break;
		}
		else
		{
			error = out.h.ah;
			in.h.ah = 0;
			int86(0x13, &in, &out); 	/* reset the disk driver */
			count++;
		}
	}
	if (error!=0)
		return error;

	in.h.al = sector_num;
	in.h.dl = driver;
	in.h.dh = side;
	in.h.ch = track;
	in.h.cl = 1;
	count = 0;
	while ( count<RE_READ )
	{
		in.h.ah = 0x04;
		int86(0x13, &in, &out);
		if (out.x.cflag==0)
			return 0;
		else
		{
			error = out.h.ah;
			in.h.ah = 0;
			int86(0x13, &in, &out); 	/* reset the disk driver */
			count++;
		}
	}
	return error;
}

/* ----------------------------------------------------------------------
	FUNCTION:	Write the disk's FAT table.
	CALLS:
	CALLED BY:	Called by function: init_disk()
	INPUT:		driver:			the disk driver specified (0 or 1).
				disk_type:		the floppy disk's type.
				*track_mark[2]:	records of the resoult:
								formatting or testing the 2 side's 80 tracks.
	OUTPUT:		None
	RETURN:		None
   ---------------------------------------------------------------------- */
static void write_FAT(UC driver, UC disk_type, UC track_mark[2][MAX_TRACKS])
{
UC			*buff;			/* read FAT buffer */
UI			logic_num;		/* logic sector number */
UI			cluster;        /* disk cluster */
float		cluster_p;      /* disk cluster point */
UC 			fat_num;
UC			sector_num;
UC 			data_start;
UC          medium;
UC			track, sector;			/* loop vaies */

	if (disk_type==0)
	{
		sector_num = 15;
		fat_num = 7;
		data_start = 0x1d;
		medium = 0xF9;
	}
	else
	{
		sector_num = 18;
		fat_num = 9;
		data_start = 0x21;
		medium = 0xF0;
	}
	buff = (UC *)mem_alloc(fat_num*0x200);
/*	absread(driver, fat_num, 1, buff);*/
	memset(buff, 0, fat_num*0x200);

	*(buff+0) = medium;
	*(buff+1) = 0xFF;
	*(buff+2) = 0xFF;
	for (track = 0; track<MAX_TRACKS; track++)
	{
		if ( track_mark[0][track]!=0 )
		{
			for (sector = 1; sector<=sector_num; sector++)
			{
				logic_num = 2*sector_num*track+sector-1;
				cluster = logic_num+2-data_start;
				cluster_p = cluster*1.5;
				if (cluster_p == (UI)cluster_p)
				{
					*(buff+(UI)cluster_p) = 0xF7;
					*(buff+(UI)cluster_p+1) = *(buff+(UI)cluster_p+1)|0xf;
				}
				else
				{
					*(buff+(UI)cluster_p) = *(buff+(UI)cluster_p) & 0x0f;
					*(buff+(UI)cluster_p) = *(buff+(UI)cluster_p) | 0x70;
					*(buff+(UI)cluster_p+1) = 0xff;
				}
			}
		}
	}
	for (track = 0; track<MAX_TRACKS; track++)
	{
		if ( track_mark[1][track]!=0 )
		{
			for (sector = 1; sector<=sector_num; sector++)
			{
				logic_num = sector_num*(2*track+1)+sector-1;
				cluster = logic_num+2-data_start;
				cluster_p = cluster*1.5;
				if (cluster_p == (UI)cluster_p)
				{
					*(buff+(UI)cluster_p) = 0xf7;
					*(buff+(UI)cluster_p+1) =*(buff+(UI)cluster_p+1)|0xf;
				}
				else
				{
					*(buff+(UI)cluster_p) = *(buff+(UI)cluster_p) & 0x0f;
					*(buff+(UI)cluster_p) = *(buff+(UI)cluster_p) | 0x70;
					*(buff+(UI)cluster_p+1) = 0xff;
				}
			}
		}
	}
	abswrite(driver, fat_num, 1, buff);
	farfree(buff);
}

/* ---------------------------------------------------------------------
	FUNCTION:	Pop out the error message window, and get
				the user's selection.
	CALLS:		set_menu(), hv_menu()
	CALLED BY:	called by function: init_disk()
	INPUT:      (win_sx, win_sy): the Coord. of the poped error message
								  window.
				error_type: error type code.
	OUTPUT:		None
				None
	RETURN:		1 -- if user selects retry.
				2 -- if user selects failed.
   ---------------------------------------------------------------------- */
static UC	err_handler(UI win_sx , UI win_sy, UC error_type)
{
UC	*error_msg[] ={"   磁盘被写保护! 您还想继续吗?",
                       "   驱动器末准备好! 您还想继续吗?",
                       "   驱动器响应超时! 您还想继续吗?"};
UC	*item_e[] ={"重 试", "取 消"};

		set_menu (7, 0, TRUE, FALSE);
		if (error_type == 0x3)
			return (hv_menu(win_sx+10, win_sy+10, 2, 2, error_msg[0], item_e));
		if (error_type == 0x6)
			return (hv_menu(win_sx+10, win_sy+10, 2, 2, error_msg[1], item_e));
		if (error_type == 0x80)
			return (hv_menu(win_sx+10, win_sy+10, 2, 2, error_msg[2], item_e));
                return(1);
}

void set_init_mode(UI x, UI y, UC bk_color, UC char_color)
{
	Disp_sx = x;
	Disp_sy = y;
	Bk_color = bk_color;
	Char_color = char_color;
	Disp_info = TRUE;
}
