/* ------------------------------------------------------------
	FILENAME:	GET_PASS.C
	DATE:		1993.5.5
	AUTHOR:		Bob Kong
	FUNCTION:	Offer a function: get_pass(), which gets
				a password string without echoing on screen.
   ------------------------------------------------------------ */
#include	<dos.h>
#include	<mem.h>
#include	<graphics.h>

#include	<KEY.INC>
#include	<DEF.INC>
#include	<FUNC.INC>

#define 	CHAR_DIST	18

/* ----------------------------------------------------------------------
	FUNCTION:	Read a password string without echoing.
	INPUT:      (pos_sx, pos_sy) specify the poped window's position.
				pass_len:	Maxium length that can be read in.
				password:   Used for store the input password's string.
   ---------------------------------------------------------------------- */
UC get_pass(UI char_x, UI char_y, UC bk_color, UC char_color,
			UC *inp_buf, UC pass_len)
{
UC pass_buff[21];		/*	the maxium length of a password is 20 */
UC loop;
UC input_num;
UI dist;
UI key;

	setmem(pass_buff, pass_len+1, 0);

	setfillstyle(SOLID_FILL, bk_color);
	flash_pause();
	bar(char_x-3, char_y-3, char_x+pass_len*CHAR_DIST+1, char_y+18);
	flash_continue();

	for (loop=0; loop<pass_len; loop++)
	{
		flash_pause();
		hz_16(1, 78, char_x+loop*CHAR_DIST, char_y, char_color);
		flash_continue();
	}
	input_num = 0;
	clr_keybuf();
	for( ; ; )
	{
		key = get_key1();
		switch (key)
		{
		case ENTER:
			memcpy(inp_buf, pass_buff, pass_len);
			return input_num;

		case ESC:
			return 0xFF;

		case BACKSPACE:
			if (input_num==0)
			{
				sound_bell();
				break;
			}
			input_num--;
			*(pass_buff+input_num) = 0;
			dist = input_num*CHAR_DIST;

			flash_pause();
			bar(char_x+dist, char_y, char_x+dist+CHAR_DIST-2, char_y+15);
			hz_16(01,78, char_x+input_num*CHAR_DIST, char_y, char_color);
			flash_continue();
			break;

		default:
			if (input_num==pass_len)
			{
				sound_bell();
				break;
			}
			if ( (key&0xFF) == 0 )
				key >>= 8;
			else
				key = key&0xFF;
			*(pass_buff+input_num) = (UC)key;
			flash_pause();
			hz_16(01,79, char_x+input_num*CHAR_DIST, char_y, char_color);
			flash_continue();
			input_num++;

		}	/* end of switch(key) */
	}		/* end of for()		  */

}


