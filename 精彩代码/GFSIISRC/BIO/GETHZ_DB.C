#include <stdio.h>
#include <string.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>

extern void draw_prompt(UC kind);
extern write_xxm(UC *xxm);
extern disp_input_cursor(UC xxm_len);

extern void ins_hz(UC qh, UC wh);

extern FILE *Fp_lsdb;

/* -------------------------------------------------------------------------
	FUNCTION:   Read the DBM input, and insert the certain HanZi.
	CALLS:		Many.
	CALLED BY:	get_hz_str().
	INPUT:      keyin -- The first letter of the DBM input.
	OUTPUT:
	RETURN:     0..3 -- Return stat is PY. GB. English. DB.
				0xFE -- <Esc> pressed, will escape from get_hz_str after return.
				0xFF -- <Enter> pressed, will escape from get_hz_str after return.
   ------------------------------------------------------------------------- */
UC db_input(UI keyin)
{
UC	dbm_len;
UC	db_str[5];
UI	key;
UC	qh, wh;
UI	db_no;
UI	hz_pos;

	if ( keyin<'0' || keyin>'9' )
	{
		sound_bell();
		return 3;
	}

	memset(db_str, 0, 5);
	db_str[0] = keyin;
	dbm_len = 1;
	write_xxm(db_str);

	while ( dbm_len<4 )
	{
		clr_keybuf();
		while (keybuf_nul())
			disp_input_cursor(dbm_len);

		key = get_key0();
		switch ( key )
		{
		case PAGEUP:
			draw_prompt(2);
			return 2;

		case PAGEDOWN:
			draw_prompt(0);
			return 0;

		case ENTER:
			return 0xFF;

		case ESC:
			return 0xFE;

		case BACKSPACE:
			if (dbm_len==1 )
			{
				write_xxm("");
				return 3;
			}
			db_str[--dbm_len] = 0;
			write_xxm(db_str);
			break;

		default:
			key &= 0x00FF;
			if ( key<'0' || key>'9' )
			{
				sound_bell();
				break;
			}
			db_str[dbm_len++] = key;
			write_xxm(db_str);
			break;
		}		/* End of switch(key) */
	}		/* End of while(dbm_len<4) */

	db_no = (db_str[0]-0x30)*1000 + (db_str[1]-0x30)*100 +
			(db_str[2]-0x30)*10 + (db_str[3]-0x30);

	if ( db_no==0 || db_no>9800 )
	{
		sound_alarm();
		return 3;
	}

	hz_pos = ((--db_no)<<1) + ((db_no/20)<<1);
	fseek(Fp_lsdb, hz_pos, SEEK_SET);
	fread(&qh, 1, 1, Fp_lsdb);
	fread(&wh, 1, 1, Fp_lsdb);

	if ( qh<161 || wh<161 )
	{
		sound_alarm();
		return(3);
	}
	qh -=160;
	wh -=160;

	ins_hz(qh, wh);
	return 3;
}		/* End of db_input  */

