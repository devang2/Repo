#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : get default time from computer for sorting
 * called by : main()
 * date      : 1993.10.3
 */
void get_default_time(void)
{
    UC  *date;
    UC	month_day[12]={31,28,31,30,31,30,31,31,30,31,30,31};

    date = get_date();
    s_time.year  = date[0]+1900;
    s_time.month = date[1];
    s_time.day   = 1;
    s_time.hour  = 0;
    if(leap_year(s_time.year))         /* this year is leap year */
	month_day[1] = 29;

    e_time.year  = date[0]+1900;
    e_time.month = date[1];
    e_time.day   = month_day[date[1]-1];
    e_time.hour  = 24;

    return;
}

/*
/* function  : get default time from computer for sorting
 * called by : main()
 * date      : 1993.10.3
 */
void get_last_time(void)
{
    UC  *date;
    UC	month_day[12]={31,28,31,30,31,30,31,31,30,31,30,31};

    date = get_date();
    s_time.month = date[1];
    s_time.year  = date[0]+1900;
    s_time.day   = 1;
    s_time.hour  = 0;
    if(leap_year(s_time.year))         /* this year is leap year */
	month_day[1] = 29;

    e_time.year  = date[0]+1900;
    e_time.month = date[1];
    e_time.day   = month_day[date[1]-1];
    e_time.hour  = 24;

    return;
}
*/


/* function  : set time for sorting
 * called by : statis()
 * date      : 1993.10.3
 */
void set_sort_time(void)
{
    int  i,j;
    UC   backx=10, backy=20;
    UI   sx,sy;
    TABLE_STRUCT time_tbl = {H_BX-30,H_BY-20,0,40,2,9,\
			       {35,45,35,35,35,35,35,35,35},7};
    UNIT_STRUCT  cur_unit;

//    clr_DialWin(1);          /* clear small dialogue window */
    message_disp(8,"←↓→↑ 移动  Enter 输入");      /* move and input */

//    draw_back1(H_BX-40,H_BY-55,H_BX+320,H_BY+85,7); /* big frame */
    pop_back(H_BX-40,H_BY-55,H_BX+320,H_BY+85,7); /* big frame */
    draw_table(&time_tbl);
    switch(Menu_path)
    {
	case 210:
	    hz16_disp(H_BX+50-24,H_BY-35,"设 定 话 单 查 询 时 间 范 围",BLACK);  /* set querying time */
	    break;
	case 220:
	    hz16_disp(H_BX+50-24,H_BY-35,"设 定 话 单 打 印 时 间 范 围",BLACK);  /* set printing time */
	    break;
	case 330:
	    hz16_disp(H_BX+50-24,H_BY-35,"设 定 话 费 统 计 时 间 范 围",BLACK);  /* set calculating time */
	    break;
	default:
	    break;
    }

    for(i=0;i<2;i++)
	for(j=0;j<9;j++)
	{
	    cur_unit.unit_x = i;
	    cur_unit.unit_y = j;
	    get_certain(&cur_unit);
	    sx = cur_unit.dot_sx;
	    sy = cur_unit.dot_sy;

	    switch(j)
	    {
		case 0:
		    if(i==0)      /* from */
			hz16_disp(sx+backx,sy+backy,"从",BLACK);
		    if(i==1)      /* to   */
			hz16_disp(sx+backx,sy+backy,"到",BLACK);
		    break;
		case 1:           /* display year */
		    draw_back(sx+backx-10,sy+backy-2,sx+backx+38,sy+backy+20,11);
		    outf(sx+5,sy+backy,11,BLACK,"%4u",s_time.year);
		    if(i==1)
		    {
			draw_back(sx+backx-10,sy+backy-2,sx+backx+38,sy+backy+20,11);
			outf(sx+5,sy+backy,11,BLACK,"%4u",e_time.year);
		    }
		    break;
		case 2:           /* year */
		    hz16_disp(sx+backx,sy+backy,"年",BLACK);
		    break;
		case 3:           /* display month */
		    draw_back(sx+backx-10,sy+backy-2,sx+backx+30,sy+backy+20,11);
		    outf(sx+backx,sy+backy,11,BLACK,"%2u",s_time.month);
		    if(i==1)
		    {
			draw_back(sx+backx-10,sy+backy-2,sx+backx+30,sy+backy+20,11);
			outf(sx+backx,sy+backy,11,BLACK,"%2u",e_time.month);
		    }
		    break;
		case 4:           /* month*/
		    hz16_disp(sx+backx,sy+backy,"月",BLACK);
		    break;
		case 5:           /* display day */
		    draw_back(sx+backx-10,sy+backy-2,sx+backx+30,sy+backy+20,11);
		    outf(sx+backx,sy+backy,11,BLACK,"%2u",s_time.day);
		    if(i==1)
		    {
			draw_back(sx+backx-10,sy+backy-2,sx+backx+30,sy+backy+20,11);
			outf(sx+backx,sy+backy,11,BLACK,"%2u",e_time.day);
		    }
		    break;
		case 6:           /* day  */
		    hz16_disp(sx+backx,sy+backy,"日",BLACK);
		    break;
		case 7:           /* display hour */
			draw_back(sx+backx-10,sy+backy-2,sx+backx+30,sy+backy+20,11);
			outf(sx+backx,sy+backy,11,BLACK,"%2u",s_time.hour);
		    if(i==1)
		    {
			draw_back(sx+backx-10,sy+backy-2,sx+backx+30,sy+backy+20,11);
			outf(sx+backx,sy+backy,11,BLACK,"%2u",e_time.hour);
		    }		    break;
		case 8:           /* hour */
		    hz16_disp(sx+backx,sy+backy,"时",BLACK);
		    break;
		default:
		    sound_alarm();
		    break;
	    }      /* end of "switch(j)"     */
	}          /* end of "for(j), for(i) */

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 3);
//    message_disp(8,"←↓→↑ 移动  Enter 输入");      /* move and input */

    input_sort_time();
    rid_pop();
//    recover_screen(1);
    message_end();
    return;
}

/* function  : input starting time and ending time for sorting
 * called by : set_time()
 * date      : 1993.10.4
 */
void input_sort_time(void)
{
    UC   esc=0,result=0;
    UC   backx=10,backy=20,high=16,width=35;
    UI   sx,sy;
    UI   key;
    UL   value;
    UNIT_STRUCT  cur_unit;

    for(;;)             /* input data */
    {
	key = get_key1();

	get_current(&cur_unit);
	sx = cur_unit.dot_sx;
	sy = cur_unit.dot_sy;

	switch(key)
	{
	    case ESC:
		esc = 1;
		break;
	    case UP:
	    case DOWN:
		if(cur_unit.unit_x == 0)
		    move_finger(1,1);            /* down a step */
		else
		    move_finger(0,1);            /* up   a step */
		break;
	    case LEFT:
		if(cur_unit.unit_y > 2)
		    move_finger(2,2);       /* left two steps   */
		break;
	    case RIGHT:
		if(cur_unit.unit_y < 6)
		    move_finger(3,2);       /* right two steps  */
		break;
            case ENTER:

/*	    case KEY_0:
	    case KEY_1:
	    case KEY_2:
	    case KEY_3:
	    case KEY_4:
	    case KEY_5:
	    case KEY_6:
	    case KEY_7:
	    case KEY_8:
	    case KEY_9:
*/
		if(cur_unit.unit_x==0)      /* starting time    */
		{
		    switch(cur_unit.unit_y)
		    {
		    case 1:     /* year */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,5,4,\
							    &value,0x00);
			    echo_finger();
			}while(result && (value < 1900));

			if(result)     /* valid input */
			{
			    s_time.year = (UI)value;
			    move_finger(3,2);
			}
			break;
		    case 3:       /* month */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result &&  (value>12) );

			if(result)     /* valid input */
			{
			    s_time.month = (UI)value;
			    move_finger(3,2);
			}
                        break;
		    case 5:       /* day */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( value>31) );

			if(result)     /* valid input */
			{
			    s_time.day = (UI)value;
			    move_finger(3,2);
			}
			break;
		    case 7:       /* hour */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && (value > 24) );

			if(result)     /* valid input */
			{
			    s_time.hour = (UI)value;
			    move_finger(1,1);
			    move_finger(2,4);
			}
			break;
		    default:
			sound_alarm();
			break;
		    }        /* end of "switch(cur_unit.unit_y)" */
		}            /* end of "if(cur_unit.unit_x==0)"  */

		else if(cur_unit.unit_x==1)      /* ending time  */
		{
		    switch(cur_unit.unit_y)
		    {
		    case 1:     /* year */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,5,4,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( (value<1993) || (value<s_time.year) ));

			if(result)     /* valid input */
			{
			    e_time.year = (UI)value;
			    move_finger(3,2);
			}
			break;
		    case 3:       /* month */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( (value<1) || (value>12) ||   \
					   ((s_time.year==e_time.year) && \
					    (value<s_time.month) ) ));

			if(result)     /* valid input */
			{
			    e_time.month = (UI)value;
			    move_finger(3,2);
			}
			break;
		    case 5:       /* day */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( (value<1) || (value>31) || \
					   ((s_time.year ==e_time.year)  && \
					    (s_time.month==e_time.month) && \
					    (value<s_time.day) )));

			if(result)     /* valid input */
			{
			    e_time.day = (UI)value;
			    move_finger(3,2);
			}
			break;
		    case 7:       /* hour */
			do
			{
			    hide_finger();
			    result = get_dec(sx+2,sy+backy,high,width,backx,2,\
							    &value,0x00);
			     echo_finger();
			}while(result && ( (value>24) ||       \
					   ((s_time.year ==e_time.year)  && \
					    (s_time.month==e_time.month) && \
					    (s_time.day  ==e_time.day)   && \
					    (value<s_time.hour) )));

			if(result)     /* valid input */
			    e_time.hour = (UI)value;
			break;
		    default:
			sound_alarm();
			break;
		    }        /* end of "switch(cur_unit.unit_y)"     */
		}            /* end of "else if(cur_unit.unit_x==1)" */

		break;       /* for "case ENTER:"    */

	    default:
		sound_alarm();
		break;
	}                    /* end of "switch(key)" */

	if(esc)
	    break;
    }                        /* end of "for(;;)"     */

    return;
}
