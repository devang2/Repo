/* ------------------------------------------------------------
	FILENAME:	GET_MON.C
	DATE:		1993.4.17
	FUNCTION:	Offers a function: get_money(), which gets money
				amount (a float value) from keyboard.
	AUTHOR:		Bob Kong
   ------------------------------------------------------------ */
#include	<mem.h>
#include	<graphics.h>
#include	<stdio.h>
#include	<stdlib.h>
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

#define		CHAR_COLOR		14	/* specify the characters color */
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
	FUNCTION:       Input money amount (a float value) from keyboard.
	CALLS:		None.
	CALLED BY:	The charging rate setting modula (by Xia Yiang).
	INPUT:          (pos_sx, pos_sy):	specify the screen postion to pop window.
				high:				specify the window's height.
				width:				specify the window width.
									it must be >=(max_length+3+1)*8.
				back_in:			specify the space between pos_sx and char_sx.
				max_len:			specify the length to read.(not include
									the point '.' and the next 2 digits
									after '.')
				money:				return the money amount
									(unsigned long integer. Unit: FEN).
				mode:   			0x0:    no default value & no first_digit.
									0x#2:	with first digit: #.
   OUTPUT:		None.
   RETURN:		return 0 if <ESC> pressed or <ENTER> with no input,
				return 1 otherwise.
   ---------------------------------------------------------------------- */
UC	get_money(UI pos_sx, UI pos_sy, UI high, UI width, UI back_in,
			  UC max_len, UL *money, UC mode)
{
UC 		input_str[81];	/* keep the input keys */
UC 		input_num;		/* number of keys in input_str */
UI 		pos_ex, pos_ey;	/* pos_sx, pos_sy, pos_ex, pos_ey
						   indicates the input window's range */
UI 		char_sx, char_sy, char_ex, char_ey;
						/* indicates the characters' echoing range. */
UC 		point_pos = 0xFF;
						/* = 0xFF if '.' isn't get, else it indicates the '.'
						   position in string input_str. */
UC 		max_charnum;	/* specify the length to read. (include the point '.'
						   and the next 2 digits after '.') */
UI		key;			/* keyboard scan_code */
UL 		temp;			/* keep the temp value of money */

UC  	*keep_scr_buf;
UI  	scr_size;
int 	old_color;         		    /* used for saving the old color setting */
struct 	fillsettingstype old_fill;  /* for saving old fill style setting */
struct 	linesettingstype old_line;	/* for ssaving old line style setting */

	if (high<15)
	{
		sound_alarm();
		return 0;
	}
	if (max_len>7)
	{
		sound_alarm();
		return 0;
	}

	max_charnum = max_len+3;
	if ( width < ((max_charnum+1)<<3)+back_in )
		width = ((max_charnum+1)<<3)+back_in;

	pos_ex = pos_sx+width-1;
	pos_ey = pos_sy+high-1;
	char_sx = pos_sx + back_in;
	char_sy = pos_sy + ((high-15)>>1);
	char_ex = char_sx + (max_charnum<<3) + 7;
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

	if (mode==0x0||mode==0x02)
	{
		memset(input_str, 0, max_charnum+1);
		input_num = 0;
	}
	else
	if ( (mode&0x0F)==0x02 )
	{
		input_str[0] = (mode>>4)+0x30;
		memset(input_str+1, 0, max_charnum);
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
	while (1)
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
			if (input_str[0]=='.')
			{
				char_16('0', char_sx, char_sy+1, Get_ret_char_color);
				hz16_disp(char_sx+8, char_sy, input_str, Get_ret_char_color);
			}
			else
				hz16_disp(char_sx, char_sy, input_str, Get_ret_char_color);


			temp = atol(input_str)*100;
			input_num--;
			if ( point_pos<input_num)
			{
				if (point_pos==input_num-1)
					temp += (input_str[point_pos+1]-'0')*10;
				else
					temp += (input_str[point_pos+1]-'0')*10 + input_str[point_pos+2]-'0';
			}
			*money = temp;

			farfree(keep_scr_buf);
			RESTORE_SAVE;
			return 1;

		case BACKSPACE:
			if (input_num==0)
			{
				sound_bell();
				break;
			}
			input_str[--input_num] = 0;
			if (input_num==point_pos)
				point_pos = 0xFF;
			DISP_STR;
			break;

		default:
			if ( (key &= 0xFF)==0 )
			{
				sound_bell();
				break;
			}
			if ( key=='.' )
			{
				if ( point_pos!=0xFF )
				{
					sound_bell();
					break;
				}
				point_pos = input_num;
				input_str[input_num++] = key;
				DISP_STR;
				break;
			}
			if ( key<'0' || key>'9' )
			{
				sound_bell();
				break;
			}
			if ( point_pos==0xFF )
			{
				if ( input_num>=max_len )
				{
					sound_bell();
					break;
				}
				if ( input_num==0 && key=='0')
				{
					break;
				}
				input_str[input_num++] = key;
			}
			else
			{
				if ( input_num>=point_pos+3 )
				{
					sound_bell();
					break;
				}
				input_str[input_num++] = key;
			}
			DISP_STR;

		}		/* end of switch(key) */
	}   	/* end of while(1) */
}		/* end of get_mon()  */
