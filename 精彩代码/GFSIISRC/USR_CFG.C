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
void set_usr_cfg(void)
{
//    TABLE_STRUCT  usr_cfg_tbl = {H_BX-50, H_BY-40, 40, 40, 2, 3, {80, 90, 160}, 9};
    TABLE_STRUCT  usr_cfg_tbl = {H_BX-50, H_BY-40, 40, 40, 2, 3, {80, 250}, 7};

    message_disp(8,"↓↑ 选择  Enter 切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX-65, H_BY-45, H_BX+300, H_BY+100, 7);
    draw_table(&usr_cfg_tbl);
    disp_usr_cfg();

    outf(H_BX+50, H_BY-30, 7, 0, "用 户 常 量 设 置");

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 1);

    input_usr_cfg();

    rid_pop();
    message_end();
    return;
}

/* function  : display all the HZ for print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void disp_usr_cfg(void)
{
    int i,j;
    UI  x,y;
    UNIT_STRUCT  cur_unit;

    j = 0;
    for(i=0; i<2; i++)
//    for(i=0; i<3; i++)
    {
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx;
	y = cur_unit.dot_sy+8;

	switch(i)
	{
	    case 0:
		outf(x+4, y,  7, BLACK, "用户名称", 7, 0);
		break;
	    case 1:
		draw_key(x, y-6, x+72, y+22, "用户类型", 7, 0);
		break;
	}
    }

    j = 1;
    for(i=0; i<2; i++)
    {
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx;
	y = cur_unit.dot_sy+8;

	switch(i)
	{
	    case 0:
		draw_back(x, y-6, x+250, y+22, 11);
		outf(x+5, y, 11, BLACK, "%-30s", Usr_nam);
		break;
	    case 1:
		draw_back(x, y-6, x+80, y+22, 11);
		if(Usr_typ == HOTEL_USR)
		    outf(x+5, y, 11, BLACK,"酒店/宾馆");
		else
		    outf(x+5, y, 11, BLACK,"一般用户");
		break;
	}
    }
    return;
}

/* function  : input print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void input_usr_cfg(void)
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
	    case UP:    /* GO TO THE LAST ROW */
                hide_finger();
		locate_finger(0,1);
                echo_finger();
		break;
	    case DOWN:  /* GO TO THE NEXT ROW */
                hide_finger();
		locate_finger(1,0);
                echo_finger();
		break;
	    case ENTER:
		x = cur_unit.dot_sx;
		y = cur_unit.dot_sy+8;

		hide_finger();
		switch(cur_unit.unit_x)
		{
		     case 0:      /* printer device */
	                message_disp(8," 输入汉字   Enter 确认");      /*chinese char*/
	                set_get_color(11, 12, 12, 11, 0);
	                get_hz_str(x+5, y, 18, 90, 0, 15, Usr_nam);
	                message_end();
                        hide_finger();
                        locate_finger(1, 0);
                        echo_finger();
                        break;
		     case 1:      /* paper width */
			chang_key(x, y-6, x+72, y+22, "用户类型", 15);
			delay(200);
			draw_key( x, y-6, x+72, y+22, "用户类型", 7, 0);
			if( Usr_typ == HOTEL_USR )
			{
			     draw_back(x+81, y-6, x+161, y+22, 11);
			     Usr_typ = NORMAL_USR;
			     outf(x+85, y, 11, BLACK,"一般用户");
			}
			else
			{
			     Usr_typ = HOTEL_USR;
			     draw_back(x+81, y-6, x+161, y+22, 11);
			     outf(x+85, y, 11, BLACK,"酒店/宾馆");
			}
                         hide_finger();
                         locate_finger(0, 1);
                         echo_finger();
			 break;
		     default:
			 sound_alarm();
			 break;
		}      /* end of "switch(cur_unit.unit_x)" */

		echo_finger();
		break;      /* ENTER */
	    default:
		sound_bell();
		break;
	}    /* end of switch(input)   */
    }        /* END OF WHILE */
}