#include <graphics.h>
#include <bios.h>
#include <conio.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : set the printer configeration
 * called by : set_data()
 * date      : 1993.9.22
 */
void set_ldd_cfg(void)
{
    TABLE_STRUCT  ldd_cfg_tbl = {H_BX+10, H_BY-40, 40, 40, 1, 2, {130, 100}, 7};

    message_disp(8,"↓↑ 选择  Enter 切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX, H_BY-45, H_BX+250, H_BY+65, 7);
    draw_table(&ldd_cfg_tbl);
    disp_ldd_cfg();

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 0);

    input_ldd_cfg();

    rid_pop();
    message_end();
    return;
}

/* function  : display all the HZ for print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void disp_ldd_cfg(void)
{
    int i,j;
    UI  x,y;
    UNIT_STRUCT  cur_unit;

        hz16_disp(H_BX+58, H_BY-25,"市 话 是 否 结 算",BLACK);

        j = 0;
        i=0;
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+5;
	y = cur_unit.dot_sy+8;

	draw_key(x+8, y, x+100, y+28, "市话结算", 7, 0);

        j = 1;
        i=0;
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+5;
	y = cur_unit.dot_sy+8;

	draw_back(x, y, x+90, y+28, 11);
	if(Ldd_fflg == YES )
	    outf(x+5, y+6, 11, BLACK,"参与结算");
	else
	    outf(x+5, y+6, 11, BLACK,"不参与结算");

        return;
}

/* function  : input print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void input_ldd_cfg(void)
{
    UI input, x, y;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
                save_sys_config();
		return;
	    case ENTER:
		x = cur_unit.dot_sx+5;
		y = cur_unit.dot_sy+8;

		hide_finger();

		chang_key(x+8, y, x+100, y+28, "市话结算", 15);
		delay(200);
		draw_key(x+8, y, x+100, y+28,  "市话结算", 7, 0);
		if(Ldd_fflg == YES)
		{
		     draw_back(x+131, y, x+221, y+28, 11);
		     Ldd_fflg = NO;
		     outf(x+136, y+6, 11, BLACK,"不参与结算");
		}
		else
		{
		     draw_back(x+131, y, x+221, y+28, 11);
		     Ldd_fflg = YES;
		     outf(x+136, y+6, 11, BLACK,"参与结算");
		}
		echo_finger();
		break;      /* ENTER */
	    default:
		sound_bell();
		break;
	}    /* end of switch(input)   */
    }        /* END OF WHILE */
}
