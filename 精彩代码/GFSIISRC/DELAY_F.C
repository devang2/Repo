#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : set the add fee for every phone record
 * called by : set_data() (set_data.c)
 * input     : UI mode -- LDD. DDD. IDD
 * date      : 1993.9.21
 */
void set_delay(void)
{
    int  key;
    UC   res1=0,res2=0,res3=0, esc=0;
    UI   back_in=8;
    UL   delay_i, delay_d, delay_l;
    TABLE_STRUCT delay_tbl = {H_BX,H_BY-17,0,30,3,2,{150,120},7};
    UNIT_STRUCT  cur_unit;

    message_disp(8,"↓↑ 移动  Enter 输入");     /* Up,Dn: move,Enter: input */

    pop_back(H_BX-10,H_BY-55,H_BX+285,H_BY+95,7);    /* big frame */
    draw_table(&delay_tbl);

    hz16_disp(H_BX+50,H_BY-40,"计 费 延 时 长 度 设 定",BLACK);

    hz16_disp(H_BX+20,H_BY-5, "市  内  电  话              秒",BLACK);   /* add_percent */
    hz16_disp(H_BX+20,H_BY+25,"国  内  长  话              秒",BLACK); /* add_record*/
    hz16_disp(H_BX+20,H_BY+55,"国  际  长  话              秒",BLACK); /* add_record*/

    draw_back(H_BX+153,H_BY-10,H_BX+225,H_BY+13, 11);
    draw_back(H_BX+153,H_BY+22,H_BX+225,H_BY+45, 11);
    draw_back(H_BX+153,H_BY+52,H_BX+225,H_BY+75, 11);

    setcolor(BLACK);

    outf(H_BX+155+back_in,H_BY-6, 11,BLACK,"%2u",Delay.delay_ldd);
    outf(H_BX+155+back_in,H_BY+26,11,BLACK,"%2u",Delay.delay_ddd);
    outf(H_BX+155+back_in,H_BY+56,11,BLACK,"%2u",Delay.delay_idd);

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);

    for(;;)
    {
	key = get_key1();

	get_current(&cur_unit);
	switch(key)
	{
	    case ESC:
		esc = 1;
		break;
	    case UP:
		move_finger(0,1);
		break;
	    case DOWN:
		move_finger(1,1);            /* down a step */
		break;
	    case ENTER:
		hide_finger();
		switch(cur_unit.unit_x)
		{
		    case 0:     /* input the add percent */
			res1 = get_dec(H_BX+155,H_BY-7,20,60,back_in,2,\
					 &delay_l,0x00);
			if(res1)     /* valid input */
			    move_finger(1,1);
			break;
		    case 1:       /* input the add record */
		        set_get_color(11, 0, 12, 11, 0);
			res2 = get_dec(H_BX+155,H_BY+25,20,60,back_in,2,\
					 &delay_d,0x00);
			if(res2)     /* valid input */
			    move_finger(1,1);
			break;
		    case 2:       /* input the add record */
		        set_get_color(11, 0, 12, 11, 0);
			res3 = get_dec(H_BX+155,H_BY+55,20,60,back_in,2,\
					 &delay_i,0x00);
			if(res3)     /* valid input */
			    move_finger(0,2);
			break;
		    default:
			sound_alarm();
			break;
		}       /* end of "switch(cur_unit.unit_y)" */
		echo_finger();
		break;
	    default:
		sound_alarm();
		break;
	 }              /* end of "switch(key)" */
	 if(esc)
	    break;
    }                   /* end of "for(;;)"     */

    if(res1)      /* at least one input is valid */
	Delay.delay_ldd = (UI)delay_l;

    if(res2)      /* at least one input is valid */
	Delay.delay_ddd = (UI)delay_d;

    if(res3)      /* at least one input is valid */
	Delay.delay_idd = (UI)delay_i;

    if(res1 || res2 || res3)
        save_sys_config();

    rid_pop();
    message_end();
    return;
}
