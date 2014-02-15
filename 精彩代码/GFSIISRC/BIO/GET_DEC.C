/* ------------------------------------------------------------
	FILENAME:	GET_DEC.C
	DATE:		1993.4.18
	AUTHOR:		Bob Kong
	FUNCTION:	Offer a function: get_dec(), which gets a unsigned long
				integer value.
   ------------------------------------------------------------ */
#include	<string.h>
#include	<graphics.h>
#include	<stdlib.h>
#include	<alloc.h>

#include	<DEF.INC>
#include	<KEY.INC>
#include	<FUNC.INC>

extern UC   Get_mode_settled;
extern UC   Get_bk_color;
extern UC   Get_char_color;
extern UC   Get_cursor_color;
extern UC   Get_ret_bk_color;
extern UC   Get_ret_char_color;

#define		CHAR_COLOR	0	/* specify the characters' color */
#define		BK_COLOR        11	/* specify the background color */
#define		CURSOR_COLOR 	12	/* specify the cursor color */
#define		RET_BK_COLOR	11	/* specify the return back color */
#define		RET_CHAR_COLOR  0   /* specify the return char color */

/*--------------------------- MACRO defines ------------------------------*/
#define RESTORE_SAVE	setcolor(old_color);\
						setfillstyle(old_fill.pattern, old_fill.color);\
						setlinestyle(old_line.linestyle, old_line.upattern, old_line.thickness)

#define DISP_STR		bar(char_sx, char_sy, char_ex, char_ey);\
						hz16_disp(char_sx, char_sy, input_str, Get_char_color)

/* ============================== get_dec =============================== */
UC	get_dec(UI pos_sx, UI pos_sy, UI high, UI width, UI back_in,
		UC max_len, UL *value, UC mode)
{
UC	input_str[11];		/* store the inputed digits in ASCII mode */
UC	digit_num;  		/* indicates the digit(s) number inputed */
UC      i;
UI 	pos_ex, pos_ey;		/* pos_sx, pos_sy, pos_ex, pos_ey
					   indicates the input window */
UI	char_sx, char_sy, char_ex, char_ey;
				/* Indicates the characters' echoing range. */
UI	key;			/* keyboard scan_code */
UC     *keep_scr_buf;
UI      scr_size;

int 	old_color;     		    /* used for saving the old color setting */
struct 	fillsettingstype old_fill;  /* for saving old fill style setting */
struct 	linesettingstype old_line;  /* for ssaving old line style setting */

	if (max_len>9 || max_len==0)
		return(0);

	if ( high<15 )
		return(0);

	if ( width < ((max_len+1)<<3)+back_in )
		width = ((max_len+1)<<3)+back_in;

	pos_ex = pos_sx+width-1;
	pos_ey = pos_sy+high-1;
	char_sx = pos_sx + back_in;
	char_sy = pos_sy+((high-15)>>1);
	char_ex = char_sx+(max_len<<3)+7;
	char_ey = char_sy + 14;

	memset(input_str,0,11);
	digit_num = 0;
	switch ( mode&0xF )
	{
	case 0:
		break;

	case 1:
		ultoa(*value, input_str, 10);
		memset(input_str+max_len,0, 11-max_len);
		digit_num = strlen(input_str);
		break;

	case 2:
		if ( (mode>>4) > 0x9 )
		{
			sound_alarm();
			return 0;
		}
		input_str[0] = ( mode>>4 ) + 0x30;
		digit_num = 1;
		break;
	default:
		sound_alarm();
		return 0;
	}

	old_color = getcolor();
	getfillsettings(&old_fill);
	getlinesettings(&old_line);

	if (!Get_mode_settled)
	{
		Get_bk_color = BK_COLOR;
		Get_char_color = CHAR_COLOR;
		Get_cursor_color = CURSOR_COLOR;
		Get_ret_bk_color = RET_BK_COLOR;
		Get_ret_char_color = RET_CHAR_COLOR;
	}
	Get_mode_settled = 0;

	scr_size = imagesize(pos_sx, pos_sy, pos_ex, pos_ey);
	keep_scr_buf = (UC *)mem_alloc(scr_size);
	getimage(pos_sx, pos_sy, pos_ex, pos_ey, keep_scr_buf);
	setfillstyle(SOLID_FILL, Get_bk_color);
	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	bar(pos_sx, pos_sy, pos_ex, pos_ey);
	DISP_STR;

	while (1)
	{
		clr_keybuf();
		while(keybuf_nul())
			disp_cursor(char_sx+(digit_num<<3), char_sy+13, Get_bk_color, Get_cursor_color);

		key = get_key0();
		switch (key)
		{
		case ESC:
			putimage(pos_sx, pos_sy, keep_scr_buf, COPY_PUT);
			farfree(keep_scr_buf);
			RESTORE_SAVE; return 0;

		case ENTER:
			if(mode!=1 && digit_num==0)
			{
				putimage(pos_sx, pos_sy, keep_scr_buf, COPY_PUT);
				farfree(keep_scr_buf);
				RESTORE_SAVE;
				return 0;
			}

			*value = 0;
			for (i=0; i<digit_num; i++)
				*value = (*value)*10+input_str[i]-'0';

			setfillstyle(SOLID_FILL, Get_ret_bk_color);
			bar(pos_sx, pos_sy, pos_ex, pos_ey);
			hz16_disp(char_sx, char_sy, input_str, Get_ret_char_color);

			farfree(keep_scr_buf);
			RESTORE_SAVE;
			return 1;

		case BACKSPACE:
			if (digit_num==0)
			{
				sound_bell();
				break;
			}
			input_str[--digit_num] = 0;
			DISP_STR;
			break;

		default:
			if (digit_num>=max_len)
			{
				sound_bell();
				break;
			}

			key &= 0xFF;
			if(key>='0' && key<='9')
			{
				input_str[digit_num++] = key;
				DISP_STR;
			}
			else
				sound_bell();
		}		/* end of switch(key) */
	}   	/* end of while(1) */
}		/* end of get_dec()  */
