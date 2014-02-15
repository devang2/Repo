#include <graphics.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>

UC get_YN()
{
UI key;

	while (1==1)
	{
		key = get_key1();
		switch(key)
		{
			case ESC:	return('E');    /* ESC */

			case 0x1579:					/*  y  */
			case 0x1559:	return('Y');	/*  Y  */

			case 0x316e:                    /*  n  */
			case 0x314e:	return('N');    /*  N  */

			default    :	sound_alarm();
		}
	}
}


UC get_yn(UI x, UI y, UC bk_color, UC char_color, UC cursor_color)
{
UI key;

	while (TRUE)
	{
		clr_keybuf();
		while ( keybuf_nul() )
			disp_cursor(x, y+14, bk_color, cursor_color);

		key = get_key0();
		switch(key)
		{
			case ESC:
				return('E');    /* ESC */

			case 0x1579:					/*  y  */
			case 0x1559:
				setfillstyle(SOLID_FILL, bk_color);
				bar(x, y, x+7, y+15);
				char_16('Y', x, y+2, char_color);
				return('Y');

			case 0x316e:                    /*  n  */
			case 0x314e:
				setfillstyle(SOLID_FILL, bk_color);
				bar(x, y, x+7, y+15);
				char_16('N', x, y+2, char_color);
				return('N');

			default    :
				sound_alarm();
		}
	}
}
