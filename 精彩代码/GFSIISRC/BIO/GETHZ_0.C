/* ----------------------------------------------------------------------
	FILENAME:	GETHZ_0.C
	FUNCTION:	Offers function: get_hz_str(), which edits an HanZi
				characters' string.
	DATE:		1993.4.18
	MODIFIED:	1993.7.28
	AUTHOR:		Bob Kong
   ---------------------------------------------------------------------- */
#include <stdio.h>
#include <graphics.h>
#include <alloc.h>
#include <string.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>
/* GETHZ_1.C */
extern UC   init_pym_table(void);
extern void init_gethz_para(void);
extern void keep_prompt(void);
extern void restore_prompt(void);
extern void draw_init_prompt(void);
extern void draw_prompt(UC code);
/* GETHZ_2.C */
extern void disp_hz_cursor(void);
extern void draw_cursor(void);
extern void clr_cursor(void);
extern void del_cur_hz(void);
extern void del_back_hz(void);
extern void ins_hz(UC qh, UC wh);
extern void ins_char(int mkey);
/* GETHZ_PY.C */
extern void select_hz(UC key);
extern void fore_page(void);
extern void next_page(void);
extern UC 	py_input(UI first_key);
/* GETHZ_DB.C */
extern UC 	db_input(UI first_key);
/* GETHZ_GB.C */
extern UC 	gb_input(UI first_key);

#define		CHAR_COLOR		15	/* specify the characters color */
#define		BK_COLOR		0	/* specify the background color */
#define		CURSOR_COLOR 	12	/* specify the cursor color */
#define		RET_BK_COLOR	7	/* specify the return back color */
#define		RET_CHAR_COLOR  0   /* specify the return char color */
/* SET_GET.C */
extern UC 	Get_mode_settled;
extern UC   Get_bk_color;
extern UC   Get_char_color;
extern UC   Get_cursor_color;
extern UC	Get_ret_bk_color;
extern UC	Get_ret_char_color;

UC   *Pyhz_buf;
FILE *Fp_lspy;
FILE *Fp_lsdb;

/*typedef struct
	{
		UC py[7];
		UC hz_num;
	} pym_type;
pym_type *Pym;
*/
typedef struct
	{
		UI hz_pos;
		UC py[6];
		UC len;
		UC hz_num;
	}  PYM_STRUCT;
PYM_STRUCT *Pym;

UC In_buf[81];
UI Cur_hz, End_hz, Max_num;
UI Char_sx, Char_sy;

/* ------------------------------------------------------------------------ */
#define ENTER_PROCESS	\
{\
	farfree(Pym);\
	farfree(Pyhz_buf);\
	restore_prompt();\
	fclose(Fp_lspy);\
	fclose(Fp_lsdb);\
\
	if ( strcmp(hz_buf, In_buf)!=0 )\
	{\
		strcpy(hz_buf, In_buf);\
\
		setfillstyle(SOLID_FILL, Get_ret_bk_color);\
		bar(pos_sx, pos_sy, pos_ex, pos_ey);\
		hz16_disp(Char_sx, Char_sy, hz_buf, Get_ret_char_color);\
		farfree(keep_scr_buf);\
\
		setfillstyle(old_fill.pattern, old_fill.color);\
		setlinestyle(old_line.linestyle, old_line.upattern, old_line.thickness);\
		setcolor(old_color);\
\
		return 1;\
	}\
	else\
	{\
		putimage(pos_sx, pos_sy, keep_scr_buf, COPY_PUT);\
		farfree(keep_scr_buf);\
\
		setfillstyle(old_fill.pattern, old_fill.color);\
		setlinestyle(old_line.linestyle, old_line.upattern, old_line.thickness);\
		setcolor(old_color);\
\
		return 0;\
	}\
}

#define ESC_PROCESS	\
{\
	farfree(Pym);\
	farfree(Pyhz_buf);\
	restore_prompt();\
	fclose(Fp_lspy);\
	fclose(Fp_lsdb);\
	putimage(pos_sx, pos_sy, keep_scr_buf, COPY_PUT);\
	farfree(keep_scr_buf);\
	return 0;\
}
/* ------------------------------------------------------------------------ */

/* -------------------------------------------------------------------------
	FUNCTION:	Edit the HanZi string.
	CALLS:          Many.
	CALLED BY:	This is a general-purposed routine.
	INPUT:		(x, y)	The position of the input-window specified.
				high	The heigth of input-window.
				width	The	width of the input-window.
				back_in	The distance from pos_sx to Char_sx.
				max_num The maxium number of Hanzi.
				*hz_buf Buffer of the HanZi string.
	OUTPUT:     None.
	RETURN:		0 -- The HanZi string NOT changed, because of error, <Esc>,
					 or <Enter> with out change.
					 If error occurred, it will sound alarm before return.
				1 -- The Hanzi string changed.
   ------------------------------------------------------------------------- */
UC get_hz_str(UI x, UI y, UI high, UI width, UI back_in,
			  UC max_number, UC *hz_buf)
{
UC stat = 0;	   /* stat: 0--PY, 1--GB, 2--English, 3--DB */
UI pos_sx, pos_sy;
UI pos_ex, pos_ey;

UC *keep_scr_buf;
UC can_select = FALSE;				/* TRUE if can select HanZi continuously */
UI key = 0;

UC old_color;
struct linesettingstype old_line;
struct fillsettingstype old_fill;  /* for saving old fill style setting */

	if (high<18)
	{
		sound_alarm ();
		return 0;
	}
	if ( width < (max_number<<4)+back_in )
		width = (max_number<<4) + back_in;
	pos_sx = x;
	pos_sy = y;
	pos_ex = pos_sx + width - 1;
	pos_ey = pos_sy + high - 1;

	Char_sx = pos_sx + back_in;
	Char_sy = pos_sy + ((high-18)>>1);

	Max_num = max_number;
	strcpy (In_buf, hz_buf);

	End_hz = strlen (In_buf);
	if ( End_hz&0x01 )        /* the length of hz_buf must be even */
	{
		printf ("\nThe input string is mistaken.");
		sound_alarm ();
		return 0;
	}

	End_hz = End_hz>>1;       /* number of Hanzi in the string     */
	if ( End_hz>Max_num )
	{
		printf ("\nThe input string is too long.");
		sound_alarm ();
		return 0;
	}

	Fp_lspy = fopen ("gfsdata\\lspy.dat","rb");
	if ( Fp_lspy==NULL )
	{
		printf ("\nCan't open file: LSPY.DAT !");
		sound_alarm ();
		return 0;
	}

	Fp_lsdb = fopen ("gfsdata\\lsdb.dat","rb");
	if ( Fp_lsdb==NULL )
	{
		printf ("\nCan't open file: LSDB.DAT !");
		sound_alarm ();
		return 0;
	}

	if ( init_pym_table()==1 )
	{
		printf ("\nCan't open file: PYNO.DAT !");
		sound_alarm ();
		return 0;
	}

	init_gethz_para ();

	old_color = getcolor ();
	getlinesettings (&old_line);
	getfillsettings (&old_fill);

	if ( !Get_mode_settled )
	{
		Get_bk_color = BK_COLOR;
		Get_char_color = CHAR_COLOR;
		Get_cursor_color = CURSOR_COLOR;
		Get_ret_bk_color = RET_BK_COLOR;
		Get_ret_char_color = RET_CHAR_COLOR;
	}
	Get_mode_settled = 0;

	keep_scr_buf = (UC *)mem_alloc( imagesize(pos_sx, pos_sy, pos_ex, pos_ey) );
	getimage(pos_sx, pos_sy, pos_ex, pos_ey, keep_scr_buf);

	setfillstyle (SOLID_FILL, Get_bk_color);
	setlinestyle (SOLID_LINE, 0, NORM_WIDTH);
	bar (pos_sx, pos_sy, pos_ex, pos_ey);
	hz16_disp (Char_sx, Char_sy, hz_buf, Get_char_color);

	keep_prompt ();
	draw_init_prompt ();

	Pyhz_buf = (UC *)mem_alloc(256);
	Cur_hz = 0;

	while ( TRUE )
	{
		clr_keybuf();
		while (keybuf_nul())
			disp_hz_cursor ();

		key = get_key0 ();
		switch ( key )
		{
		case PAGEUP:
			can_select = FALSE;
			if ( stat==0 )
				stat = 3;
			else
				stat --;
			draw_prompt (stat);
			break;

		case PAGEDOWN:
			can_select = FALSE;
			if ( stat==3 )
				stat = 0;
			else
				stat ++;
			draw_prompt (stat);
			break;

		case LEFT:
			clr_cursor ();
			if ( Cur_hz==Max_num )
				Cur_hz -= 2;
			else if ( Cur_hz )
				Cur_hz--;
			else
				sound_bell ();
			draw_cursor ();
			break;

		case RIGHT:
			clr_cursor ();
			if ( Cur_hz<End_hz )
				Cur_hz ++;
			else
				sound_bell ();
			draw_cursor ();
			break;

		case HOME:
			clr_cursor ();
			if ( Cur_hz==0 )
				sound_bell ();
			else
				Cur_hz = 0;
			draw_cursor ();
			break;

		case END:
			clr_cursor ();
			if ( Cur_hz==End_hz )
				sound_bell ();
			else
				Cur_hz = End_hz;
			draw_cursor();
			break;

		case BACKSPACE:
			del_back_hz ();
			draw_cursor ();
			break;

		case DEL:
			del_cur_hz ();
			draw_cursor ();
			break;

		case ENTER:
			ENTER_PROCESS

		case ESC:
			ESC_PROCESS

		default:
			if  ( can_select )
			{
				if ( (key&0x00FF)==0 )     /* ALT-key   */
				{
					key >>= 8;
					if ( key<0x78 || key>0x83 )
					{
						sound_bell();
						break;
					}
					if ( key==0x82 )   /* ALT-MINUS */
					{
						fore_page();
						break;
					}
					if ( key == 0x83 ) /* ALT-PLUS  */
					{
						next_page();
						break;
					}
					key -= 0x78;
					select_hz (key);
					break;
				}
/*				else if((key==0x332c) || (key==0x333c))       //<
				{
				    fore_page();
				    break;
				}
				else if((key==0x342e) || (key==0x343e))      // >
				{
				    next_page();
				    break;
				 }
*/			}
			key &= 0x00FF;
			switch ( stat )
			{
			case 0:        /* Pin yin input */
				if ( key>='a' && key<='z' )
				{
					draw_cursor ();
					stat = py_input (key);
					switch ( stat )
					{
					case 0:
					case 1:
					case 2:
					case 3:
						can_select = FALSE;
						break;
					case 0xFD:
						stat = 0;
						can_select = TRUE;
						break;
					case 0xFE:
						ESC_PROCESS
					case 0xFF:
						ENTER_PROCESS
					}
				}
				else if ( key>32 && key<128 )
					ins_char (key);
				else
					sound_bell ();
				break;

			case 1:        /* GB input */
				if ( key>='0' && key<='9' )
				{
					draw_cursor ();
					stat = gb_input (key);
					if ( stat==0xFE )
						ENTER_PROCESS
					if ( stat==0xFF )
						ESC_PROCESS
				}
				else if ( key>32 && key<128 )
					ins_char (key);
				else
					sound_bell ();
				break;

			case 2:        /* English */
				if ( key>32 && key<128 )
					ins_char (key);
				else
					sound_bell ();
				break;

			case 3:        /* DB input */
				if ( key>='0' && key<='9' )
				{
					draw_cursor ();
					stat = db_input (key);
					if ( stat==0xFE )
						ENTER_PROCESS
					if ( stat==0xFF )
						ESC_PROCESS
				}
				else if ( key>32 && key<128 )
					ins_char (key);
				else
					sound_bell ();
				break;

			default: sound_bell ();
			}		/*  end of switch(stat)  */
		}   	   	/*  End of switch(key)	*/
	}				/*  End of while ( TRUE ) */
}		 			/*  End of get_hz_str()  */
