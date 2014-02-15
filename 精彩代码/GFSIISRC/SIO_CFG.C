#include <graphics.h>
#include <bios.h>
#include <conio.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : set the serial communication configeration
 * called by : set_data()
 * date      : 1993.9.22
 */
void set_sio_cfg(void)
{
    TABLE_STRUCT  sio_cfg_tbl = {H_BX+10, H_BY-105, 30, 30, PORT_NUM, 2, {80, 80}, 7};

    message_disp(8,"↓↑ 选择  Enter 切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX, H_BY-107, H_BX+205, H_BY+250, 7);
    draw_table(&sio_cfg_tbl);
    disp_sio_cfg();

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 1);

    input_sio_cfg();

    rid_pop();
    message_end();
    return;
}

/* function  : display all the HZ for serial communication configeration
 * called by : set_sio_cfg()
 * date      : 1993.11.5
 */
void disp_sio_cfg(void)
{
    int i,j;
    UI  x,y;
    UNIT_STRUCT  cur_unit;
    UC  buf[10] = "串口%d";

    hz16_disp(H_BX+42, H_BY-90,"串  口   设  置",BLACK);

    for(j=0; j<2; j++)
        for(i=0; i<PORT_NUM; i++)
        {
	    cur_unit.unit_x = i;
	    cur_unit.unit_y = j;
	    get_certain(&cur_unit);
	    x = cur_unit.dot_sx+5;
	    y = cur_unit.dot_sy+8;

            if(j == 0) {
	        outf(x+15, y+7, 7, 0, buf, i+1);
//	        draw_key(x, y, x+80, y+28, buf, 7, 0);
            }
            else
            {
	        draw_back(x, y, x+72, y+28, 11);
                if(Sys_mode.com_m[i])
	            outf(x+5, y+6, 11, BLACK,"用");
                else
	            outf(x+5, y+6, 11, BLACK,"否");
            }
        }

    return;
}

/* function  : input serial communication configeration
 * called by : set_sio_cfg()
 * date      : 1993.11.5
 */
void input_sio_cfg(void)
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

                SIOEnd();
                SIOInit();

		return;
	    case UP:    /* GO TO THE LAST ROW */
		move_finger(0,1);
		break;
	    case DOWN:  /* GO TO THE NEXT ROW */
		move_finger(1,1);
		break;
	    case ENTER:
		x = cur_unit.dot_sx+5;
		y = cur_unit.dot_sy+8;

		hide_finger();
                if(Sys_mode.com_m[cur_unit.unit_x]) {
		  Sys_mode.com_m[cur_unit.unit_x] = 0;
	          outf(x+5, y+6, 11, BLACK,"否");
                }
                else {
		  Sys_mode.com_m[cur_unit.unit_x] = 1;
	          outf(x+5, y+6, 11, BLACK,"用");
                }

		echo_finger();
		break;      /* ENTER */
	    default:
		sound_bell();
		break;
	}    /* end of switch(input)   */
    }        /* END OF WHILE */
}
