/* ----------------------------------------------------------------------
	FILENAME:	GET_CPAS.C
	DATE:		1993.8.5
	AUTHOR:		Bob Kong
	FUNCTION:	Offer a function: get_cpas(), which gets the four-digit's
				call-barray password.
   --------------------------------------------------------------------- */
#include	<mem.h>
#include	<graphics.h>
#include	<alloc.h>

#include	<KEY.INC>
#include	<DEF.INC>
#include	<FUNC.INC>

extern UC 	Get_mode_settled;
extern UC       Get_bk_color;
extern UC       Get_char_color;
extern UC       Get_cursor_color;
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
	FUNCTION:	Read call-barray password: a four-digit's decimal integer.
	INPUT:          (pos_sx, pos_sy) specify the poped window's position.
			high:		The heigth of the window, must be>=15.
			width:	 	The width of the window, >= (max_len+1)*8+back_in.
			back_in:	indicates the space between pos_sx and char_sx
			*passwd:        Used for keeping the input password.
			mode:		0x0:	No default value or first digit.
					0x1:	With the default value.
					0x#2:	With the first digit user stroked.
   ---------------------------------------------------------------------- */
UC	get_cpas(UI pos_sx, UI pos_sy, UI high, UI width, UI back_in,
			 UI *passwd, UC mode)
{
UC 		input_str[5];	/* keep the input keys */
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

	if ( width < ((4+1)<<3)+back_in )
		width = ((4+1)<<3)+back_in;

	pos_ex = pos_sx + width - 1;
	pos_ey = pos_sy + high - 1;
	char_sx = pos_sx + back_in;
	char_sy = pos_sy + ((high-15)>>1);
	char_ex = char_sx + ((4+1)<<3) - 1;
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

	memset(input_str, 0, 5);
	if (mode==0x0)
		input_num = 0;
	else
	if (mode==0x1)
	{
		if (*passwd>9999)
			input_num = 0;
		else
		{
			input_str[0] = (*passwd/1000) + '0';
			input_str[1] = ((*passwd%1000)/100) + '0';
			input_str[2] = ((*passwd%100)/10) + '0';
			input_str[3] = (*passwd%10) + '0';

			input_num = 4;
		}
	}
	else
	if ( (mode&0x0F)==0x02)
	{
		input_str[0] = (mode>>4) + '0';
		input_num = 1;
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
		while ( keybuf_nul() )
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
			if( input_num==0 || input_num==4 )
			{
				setfillstyle(SOLID_FILL, Get_ret_bk_color);
				bar(pos_sx, pos_sy, pos_ex, pos_ey);
				farfree(keep_scr_buf);
				RESTORE_SAVE;
				if ( input_num==0 )
					*passwd = 0xFFFF;
				else
				{
					*passwd = (input_str[0]-'0')*1000 + (input_str[1]-'0')*100
							  + (input_str[2]-'0')*10 + (input_str[3]-'0');
					hz16_disp(char_sx, char_sy, input_str, Get_ret_char_color);
				}
				return 1;
			}
			else
				sound_bell();
			break;

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
			if ( input_num==4 )
			{
				sound_bell();
				break;
			}
			if ( (key &= 0xFF)==0 )
			{
				sound_bell();
				break;
			}
			if ( key<'0' || key>'9' )
			{
				sound_bell();
				break;
			}
			input_str[input_num++] = key;
			DISP_STR;
		}		/* end of switch(key) */
	}   	/* end of while(1) */
}		/* end of get_decstr()  */
