#include <string.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>

extern void draw_prompt(UC kind);
extern write_xxm(UC *xxm);
extern disp_input_cursor(UC xxm_len);

extern void ins_hz(UC qh, UC wh);

/* -------------------------------------------------------------------------
	FUNCTION:   Read the GBM input, and insert the certain HanZi.
	CALLS:		Many.
	CALLED BY:	get_hz_str().
	INPUT:      keyin -- The first letter of the GBM input.
	OUTPUT:
	RETURN:     0..3 -- Return stat is PY. GB. English. DB.
				0xFE -- <Esc> pressed, will escape from get_hz_str after return.
				0xFF -- <Enter> pressed, will escape from get_hz_str after return.
   ------------------------------------------------------------------------- */
UC gb_input(UI keyin)
{
UC	gbm_len;
UC	gb_str[5];
UI	key;
UC	qh, wh;

	if ( keyin<'0' || keyin>'9' )
	{
		sound_bell();
		return 1;
	}

	memset(gb_str, 0, 5);
	gb_str[0] = keyin;
	gbm_len = 1;
	write_xxm(gb_str);

	while ( gbm_len<4 )
	{
		clr_keybuf();
		while ( keybuf_nul() )
			disp_input_cursor(gbm_len);

		key = get_key0();
		switch ( key )
		{
		case PAGEUP:
			draw_prompt(0);
			return 0;

		case PAGEDOWN:
			draw_prompt(2);
			return 2;

		case ENTER:
			return 0xFF;

		case ESC:
			return 0xFE;

		case BACKSPACE:
			if ( gbm_len==1 )
			{
				write_xxm("");
				return 1;
			}
			gb_str[--gbm_len] = 0;
			write_xxm(gb_str);
			break;

		default:
			key &= 0x00FF;
			if ( key<'0' || key>'9' )
			{
				sound_bell();
				break;
			}
			gb_str[gbm_len++] = key;
			write_xxm(gb_str);
			break;
		}		/* End of switch(key) */
	}		/* End of while(gbm_len<4) */

	qh = (gb_str[0]-0x30)*10 + (gb_str[1]-0x30);
	if ( qh==0 || qh>87 || (qh>9 && qh<16) )
	{
		sound_alarm();
		return 1;
	}

	wh = (gb_str[2]-0x30)*10 + (gb_str[3]-0x30);
	if ( wh==0 || wh>94 )
	{
		sound_alarm();
		return 1;
	}

	ins_hz(qh, wh);
	return 1;
}		/* End of gb_input  */
