/* ------------------------------------------------------------
	FILENAME:	GET_DSTR.C
	DATE:		1993.4.17
	AUTHOR:		Bob Kong
	FUNCTION:	Offer a function: get_decstr(), which gets the
				a decimal digit's string.
   ------------------------------------------------------------ */
#include	<mem.h>
#include	<graphics.h>
#include	<stdio.h>
#include	<alloc.h>

#include	<KEY.INC>
#include	<DEF.INC>
#include	<FUNC.INC>

extern UC 	Get_mode_settled;
extern UC   Get_bk_color;
extern UC   Get_char_color;
extern UC   Get_cursor_color;
extern UC	Get_ret_bk_color;
extern UC	Get_ret_char_color;

#define		CHAR_COLOR		14	/* specify the characters' color */
#define		BK_COLOR		0	/* specify the background color */
#define		CURSOR_COLOR 	12	/* specify the cursor color */
#define		RET_BK_COLOR	7	/* specify the return back color */
#define		RET_CHAR_COLOR  0   /* specify the return char color */

/*---------------------------------------------------*/
#define RESTORE_SAVE	setcolor(old_color);\
						setfillstyle(old_fill.pattern, old_fill.color);\
						setlinestyle(old_line.linestyle, old_line.upattern, old_line.thickness)

#define DISP_STR		bar(char_sx, char_sy, char_ex, char_ey);\
						hz16_disp(char_sx, char_sy, input_str, Get_char_color)

/* ----------------------------------------------------------------------
	FUNCTION:	Read a decimal digits' string.
	INPUT:      (pos_sx, pos_sy) specify the poped window's position.
				high:		The heigth of the window, must be>=15.
				width:	 	The width of the window, >= (max_len+1)*8+back_in.
				back_in:	indicates the space between pos_sx and char_sx
				max_len:	Maxium digits that can be read in.
				get_buf:    Used for store the input string.
							The destination's code is in ASCII code when return.
				mode:		0x0:	No default value or first digit.
							0x#2:	With the first digit user stroked.
   ---------------------------------------------------------------------- */
UC	get_decstr(UI pos_sx, UI pos_sy, UI high, UI width, UI back_in,
			   UC max_len, UC get_buf[], UC mode)
{
UC 		input_str[81];	/* keep the input keys */
UC 		input_num;		/* number of keys in input_str */
UI      pos_ex;			/* indicates the right of the input window */
UI 		pos_ey;			/* indicates the bottom of the input window */
UI 		char_sx, char_sy, char_ex, char_ey;
						/* indicates the characters' echoing range. */
UI		key;			/* keyboard scan_code */

UC  	*keep_scr_buf;
UI  	scr_size;
UC		old_color;         		    /* used for saving the old color setting */
struct 	fillsettingstype old_fill;  /* for saving old fill style setting */
struct 	linesettingstype old_line;	/* for ssaving old line style setting */

	if (high<15)
	{
		sound_alarm();
		return 0x0;
	}

	if ( width < ((max_len+1)<<3)+back_in )
		width = ((max_len+1)<<3)+back_in;

	pos_ex = pos_sx + width - 1;
	pos_ey = pos_sy + high - 1;
	char_sx = pos_sx + back_in;
	char_sy = pos_sy + ((high-15)>>1);
	char_ex = char_sx + ((max_len+1)<<3) - 1;
	char_ey = char_sy + 14;

	if (!Get_mode_settled)
	{
		Get_bk_color = BK_COLOR;
		Get_char_color = CHAR_COLOR;
		Get_cursor_color = CURSOR_COLOR;
		Get_ret_bk_color = RET_BK_COLOR;
		Get_ret_char_color = RET_CHAR_COLOR;
	}
	Get_mode_settled = 0;

	if (mode==0x0)
	{
		memset(input_str, 0, max_len+1);
		input_num = 0;
	}
	else
	if ( (mode&0x0F)==0x02)
	{
		input_str[0] = (mode>>4) + '0';
		input_num = 1;
		memset(input_str+1, 0, max_len);
	}
	else
	{
		sound_alarm();
		return(0);
	}

	scr_size = imagesize(pos_sx, pos_sy, pos_ex, pos_ey);
	keep_scr_buf = (UC *)mem_alloc(scr_size);
	getimage(pos_sx, pos_sy, pos_ex, pos_ey, keep_scr_buf);

	old_color = getcolor();
	getfillsettings(&old_fill);
	getlinesettings(&old_line);

	setfillstyle(1,Get_bk_color);
	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	bar(pos_sx, pos_sy, pos_ex, pos_ey);

	DISP_STR;
	while (TRUE)
	{
		clr_keybuf();
		while(keybuf_nul())
			disp_cursor(char_sx+(input_num<<3), char_sy+13, Get_bk_color, Get_cursor_color);
		key = get_key0();

		switch (key)
		{
		case ESC:
			putimage(pos_sx, pos_sy, keep_scr_buf, COPY_PUT);
			farfree(keep_scr_buf);
			RESTORE_SAVE;
			return 0;

		case ENTER:
			if( input_num==0 )
			{
				putimage(pos_sx, pos_sy, keep_scr_buf, COPY_PUT);
				farfree(keep_scr_buf);
				RESTORE_SAVE;
				return 0;
			}

			setfillstyle(SOLID_FILL, Get_ret_bk_color);
			bar(pos_sx, pos_sy, pos_ex, pos_ey);
			hz16_disp(char_sx, char_sy, input_str, Get_ret_char_color);

			memcpy(get_buf, input_str, input_num+1);

			farfree(keep_scr_buf);
			RESTORE_SAVE;
			return input_num;

		case BACKSPACE:
			if (input_num==0)
			{
				sound_bell();
				break;
			}
			input_str[--input_num] = 0;
			DISP_STR;
			break;

		default:
			if (input_num>=max_len)
			{
				sound_bell();
				break;
			}
			if ( (key &= 0xFF)==0 )
			{
				sound_bell();
				break;
			}
			if (key<'0'||key>'9')
			{
				sound_bell();
				break;
			}
			input_str[input_num++] = key;
			DISP_STR;
		}		/* end of switch(key) */
	}   	/* end of while(1) */
}		/* end of get_decstr()  */
