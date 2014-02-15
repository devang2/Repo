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
void set_csh_swtch(void)
{
    TABLE_STRUCT  csh_swtch_tbl = {H_BX+10, H_BY-40, 40, 40, 2, 2, {130, 80}, 7};

    message_disp(8," Enter  切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX, H_BY-45, H_BX+230, H_BY+120, 7);
    draw_table(&csh_swtch_tbl);
    disp_csh_swtch();

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 1);

    input_csh_swtch();

    rid_pop();
    message_end();
    return;
}

/* function  : display all the HZ for print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void disp_csh_swtch(void)
{
    UI  x,y;
    UC  buf[2][11] = {"话机押金", "授权码押金"};
    UNIT_STRUCT  cur_unit;
    int i, j;

    hz16_disp(H_BX+45, H_BY-25,"押金功能开关",BLACK);

    for (j=0; j<2; j++)
        for(i=0; i<2; i++)
        {
            cur_unit.unit_x = i;
            cur_unit.unit_y = j;
            get_certain(&cur_unit);
            x = cur_unit.dot_sx;
            y = cur_unit.dot_sy+17;

            if(j==0) {
	        draw_key(x, y-6, x+80, y+28, buf[i], 7, 0);
            }
            else
            {
	        draw_back(x-10, y-2, x+70, y+20, 11);
                if(Cashflg[i])
	            outf(x, y, 11, BLACK,"打开");
                else
	            outf(x, y, 11, BLACK,"关闭");
            }
        }

    return;
}

/* function  : input print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void input_csh_swtch(void)
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
		move_finger(0,1);
		break;

	    case DOWN:  /* GO TO THE NEXT ROW */
		move_finger(1,1);
		break;

	    case ENTER:
		x = cur_unit.dot_sx;
		y = cur_unit.dot_sy+17;

		if(Cashflg[cur_unit.unit_x] == TRUE)
		{
		    Cashflg[cur_unit.unit_x] = FALSE;
	            outf(x, y, 11, BLACK,"关闭");
		}
		else
		{
		    Cashflg[cur_unit.unit_x] = TRUE;
	            outf(x, y, 11, BLACK,"打开");
		}
		break;
	    default:
		sound_bell();
		break;
	}    /* end of switch(input)   */
    }        /* END OF WHILE */
}
