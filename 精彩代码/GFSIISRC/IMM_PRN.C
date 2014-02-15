#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : set immediate record print selection
 * called by : phone_record_proc()
 * date      : 1993.11.5
 */
void set_imm_print(void)
{
    TABLE_STRUCT  imm_prn_tbl={H_BX+20,H_BY-40,0,30,3,2,{120,35},7};

    message_disp(8,"↓↑ 选择  Enter 切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX,H_BY-90,H_BX+200,H_BY+75,7);
    draw_table(&imm_prn_tbl);
    disp_imm_prn();

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);

    input_imm_prn();
    rid_pop();
    message_end();
    return;
}

/* function  : display all the HZ for immediate print selection
 * called by : set_imm_print()
 * date      : 1993.11.5
 */
void disp_imm_prn(void)
{
    int i,j;
    UI  x,y;
    UNIT_STRUCT  cur_unit;

    hz16_disp(H_BX+30,H_BY-70,"立 即 打 印 设 置",BLACK);

    j = 0;
    for(i=0;i<3;i++)
    {
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+10;
	y = cur_unit.dot_sy+8;

	switch(i)
	{
	    case 0:     /* international call records   */
		hz16_disp(x,y,"国际长途话单",BLACK);
		break;
	    case 1:     /* national long call records   */
		hz16_disp(x,y,"国内长途话单",BLACK);
		break;
	    case 2:     /* local call telephone records */
		hz16_disp(x,y,"市话话单",BLACK);
		break;
	}
    }

    j = 1;
    for(i=0;i<3;i++)
    {
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+5;
	y = cur_unit.dot_sy+8;

	switch(i)
	{
	    case 0:     /* international call records   */
		if(Intern_prn)
		    chang_key3(x-2, y-2, x+18, y+18, "", 15);
		else
		    draw_key(x-2, y-2, x+18, y+18, "", 7, 0);
		break;
	    case 1:     /* national long call records   */
		if(Nation_prn)
		    chang_key3(x-2, y-2, x+18, y+18, "", 15);
		else
		    draw_key(x-2, y-2, x+18, y+18, "", 7, 0);
		break;
	    case 2:     /* local call telephone records */
		if(Local_prn)
		    chang_key3(x-2, y-2, x+18, y+18, "", 15);
		else
		    draw_key(x-2, y-2, x+18, y+18, "", 7, 0);
		break;
	}
    }
    return;
}

/* function  : input immediate print selection
 * called by : set_imm_print()
 * date      : 1993.11.5
 */
void input_imm_prn(void)
{
    UI input,sx,sy;
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
		move_finger(0,1);
		break;
	    case DOWN:  /* GO TO THE NEXT ROW */
		move_finger(1,1);
		break;
	    case ENTER:
		sx = cur_unit.dot_sx;
		sy = cur_unit.dot_sy;

		hide_finger();
		switch(cur_unit.unit_x)
		{
		     case 0:      /* international long call print or not */
			if(Intern_prn == FALSE)
			{
			     Intern_prn = TRUE;
			     chang_key3(sx+3, sy+6, sx+23, sy+26, "", 15);
			}
			else
			{
			     Intern_prn = FALSE;
			     draw_key(sx+3, sy+6, sx+23, sy+26, "", 7, 0);
			}
			 break;
		     case 1:      /* national long call print or not */
			if(Nation_prn == FALSE)
			{
			     Nation_prn = TRUE;
			     chang_key3(sx+3, sy+6, sx+23, sy+26, "", 15);
			}
			else
			{
			     Nation_prn = FALSE;
			     draw_key(sx+3, sy+6, sx+23, sy+26, "", 7, 0);
			}
			 break;
		     case 2:      /* local call records print or not */
			if(Local_prn == FALSE)
			{
			     Local_prn = TRUE;
			     chang_key3(sx+3, sy+6, sx+23, sy+26, "", 15);
			}
			else
			{
			     Local_prn = FALSE;
			     draw_key(sx+3, sy+6, sx+23, sy+26, "", 7, 0);
			}
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
