#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : set the printer configeration
 * called by : set_data()
 * date      : 1993.9.22
 */
void set_dsp_cfg(void)
{
    TABLE_STRUCT  dsp_cfg_tbl = {H_BX+10, H_BY-40, 40, 40, 1, 1, {130}, 7};

    message_disp(8," Enter  切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX, H_BY-45, H_BX+160, H_BY+60, 7);
    draw_table(&dsp_cfg_tbl);
    disp_dsp_cfg();
    input_dsp_cfg();
    rid_pop();
    message_end();

    return;
}

/* function  : display all the HZ for print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void disp_dsp_cfg(void)
{
    UI  x,y;
    UNIT_STRUCT  cur_unit;

    hz16_disp(H_BX+32, H_BY-25,"显示设备设置",BLACK);
    cur_unit.unit_x = 0;
    cur_unit.unit_y = 0;
    get_certain(&cur_unit);
    x = cur_unit.dot_sx+28;
    y = cur_unit.dot_sy+8;

    if(Dsp_typ == MONO)
	draw_key(x, y, x+80,  y+28, "单  色", 7, 0);
    else
	chang_key(x, y, x+80, y+28, "彩  色", 15);

    return;
}

/* function  : input print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void input_dsp_cfg(void)
{
    UI input, x, y;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	cur_unit.unit_x = 0;
	cur_unit.unit_y = 0;
	get_certain(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
                save_sys_config();
		return;

	    case ENTER:
		x = cur_unit.dot_sx+28;
		y = cur_unit.dot_sy+8;

		if(Dsp_typ == COLOR)
		{
		    Dsp_typ = MONO;
		    draw_key(x, y, x+80,  y+28, "单  色", 7, 0);
                    Dsp_clr.fng_clr = BLUE;
		}
		else
		{
		    Dsp_typ = COLOR;
		    chang_key(x, y, x+80, y+28, "彩  色", 15);
                    Dsp_clr.fng_clr = LIGHTGREEN;
		}
		break;
	    default:
		sound_bell();
		break;
	}    /* end of switch(input)   */
    }        /* END OF WHILE */
}
