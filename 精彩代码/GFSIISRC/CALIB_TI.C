#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

int SendDateTime(UC  *date, UC  *time);

/* function  : calibrating the GFS system date and time
 * called by : none
 * output    : ratify=1: ESC
 *             ratify=2: F1
 * date      : 1994.8.5
 */
UC  calib_time(void)
{
    int  i,j;
    UC   backx=5, backy=20;
    UC   ratify;
    UI   sx,sy,ex;
    TABLE_STRUCT ctime_tbl = {H_BX+10,H_BY-30,0,40,2,7,\
			       {40,40,28,40,28,40,28},7};
    UNIT_STRUCT  cur_unit;
    UC  *date;
    UC  *time;

    pop_back(H_BX-10,H_BY-65,H_BX+275,H_BY+80,7); /* big frame */
    draw_table(&ctime_tbl);
    hz16_disp(H_BX+50,H_BY-45,"校 准 计 费 系 统 时 钟",BLACK);  /* set deleting time */

    time=get_time();
    date=get_date();

    for(i=0;i<2;i++)
	for(j=0;j<7;j++)
	{
	    cur_unit.unit_x = i;
	    cur_unit.unit_y = j;
	    get_certain(&cur_unit);
	    sx = cur_unit.dot_sx;
	    sy = cur_unit.dot_sy;
	    ex = cur_unit.dot_ex;

	    switch(j)
	    {
		case 0:
		    if(i==0)      /* date */
			hz16_disp(sx+backx-4,sy+backy,"日期",BLACK);
		    if(i==1)      /* time   */
			hz16_disp(sx+backx-4,sy+backy,"时间",BLACK);
		    break;
		case 2:
		    if(i==0)      /* year */
			hz16_disp(sx+backx+1,sy+backy,"年",BLACK);
		    if(i==1)      /* hour */
			hz16_disp(sx+backx+1,sy+backy,"时",BLACK);
		    break;
		case 4:
		    if(i==0)       /* month*/
			hz16_disp(sx+backx+1,sy+backy,"月",BLACK);
		    if(i==1)       /* minute */
			hz16_disp(sx+backx+1,sy+backy,"分",BLACK);
		    break;
		case 6:
		    if(i==0)       /* day  */
			hz16_disp(sx+backx+1,sy+backy,"日",BLACK);
		    if(i==1)       /* second */
			hz16_disp(sx+backx+1,sy+backy,"秒",BLACK);
		    break;

		case 1:
		    draw_back(sx-2,sy+backy-2,ex+2,sy+backy+20,11);
		    if(i==0)
			outf(sx+11,sy+backy,11,BLACK,"%2u",*date);
		    if(i==1)
			outf(sx+11,sy+backy,11,BLACK,"%2u",*time);
		    break;
		case 3:
		    draw_back(sx-2,sy+backy-2,ex+2,sy+backy+20,11);
		    if(i==0)
			outf(sx+11,sy+backy,11,BLACK,"%2u",*(date+1));
		    if(i==1)
			outf(sx+11,sy+backy,11,BLACK,"%2u",*(time+1));
		    break;
		case 5:
		    draw_back(sx-2,sy+backy-2,ex+2,sy+backy+20,11);
		    if(i==0)
			outf(sx+11,sy+backy,11,BLACK,"%2u",*(date+2));
		    if(i==1)
			outf(sx+11,sy+backy,11,BLACK,"%2u",*(time+2));
		    break;
		default:
		    sound_alarm();
		    break;
	    }      /* end of "switch(j)"     */
	}          /* end of "for(j), for(i) */

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);
    message_disp(8,"←↓→↑ 移动  Enter 修改  F1 确认  Esc 退出");  /* move and input */

    ratify = modi_time(date,time);

    message_end();
    rid_pop();
    return(ratify);
}

/* function  : modifing the GFS system clock
 * called by : calib_time()
 * output    : date -- the GFS system date
 *             time -- the GFS system time
 *             esc=1: ESC
 *             esc=2: ratify (F1)
 * date      : 1994.8.6
 */
UC  modi_time(UC date[3],UC time[3])
{
    UC   esc=0,result=0;
    UC   backx=11,backy=20,high=20,width=32;
    UI   sx,sy;
    UI   key;
    UL   value;
    UNIT_STRUCT  cur_unit;
    UC date0[3];
    UC time0[3];

    date0[0] = date[0];	date0[1] = date[1]; date0[2] = date[2];

    time0[0] = time[0]; time0[1] = time[1]; time0[2] = time[2];

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
	    case F1:
		esc = 2;
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
		if(cur_unit.unit_y < 4)
		    move_finger(3,2);       /* right two steps  */
		break;
	    case ENTER:
		if(cur_unit.unit_x==0)      /* date    */
		{
		    switch(cur_unit.unit_y)
		    {
		    case 1:     /* year */
			do
			{
			    hide_finger();
			    result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && (value<1));

			if(result)     /* valid input */
			{
			    date[0] = value;
			    move_finger(3,2);
			}
			break;
		    case 3:       /* month */
			do
			{
			    hide_finger();
			    result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( (value<1) || (value>12) ) );

			if(result)     /* valid input */
			{
			    date[1] = value;
			    move_finger(3,2);
			}
			break;
		    case 5:       /* day */
			do
			{
			    hide_finger();
			    result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( (value<1) || (value>31) ) );

			if(result)     /* valid input */
			{
			    date[2] = value;
			    move_finger(1,1);
			    move_finger(2,4);
			}
			break;
		    default:
			sound_alarm();
			break;
		    }        /* end of "switch(cur_unit.unit_y)" */
		}            /* end of "if(cur_unit.unit_x==0)"  */

		else if(cur_unit.unit_x==1)      /* time  */
		{
		    switch(cur_unit.unit_y)
		    {
		    case 1:     /* hour */
			do
			{
			    hide_finger();
			    result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( value >= 24 ) );

			if(result)     /* valid input */
			{
			    time[0] = value;
			    move_finger(3,2);
			}
			break;
		    case 3:       /* minute */
			do
			{
			    hide_finger();
			    result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && ( value >= 60 ) );

			if(result)     /* valid input */
			{
			    time[1] = value;
			    move_finger(3,2);
			}
			break;
		    case 5:       /* second */
			do
			{
			    hide_finger();
			    result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			    echo_finger();
			}while(result && (value >= 60) );

			if(result)     /* valid input */
			    time[2] = value;
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

	if(esc != 0)
	    break;
    }
			  /* end of "for(;;)"     */
    if(esc==2)
    {
	if(set_date(date) == TRUE)
	{
	    if(set_time(time) == TRUE)
	    {
		if(SendDateTime(date, time) != 0) /* failure */
		{
		    set_date(date0);
		    set_time(time0);
		}
	    }
	    else
		set_date(date0);
	}

    }

    return(esc);
}

int SendDateTime(UC  *date, UC  *time)
{
     UC  tmp1[10], tmp2[10];
     UC  port;

    /* shake hand with host */
    if(ShakeHandAll() != 0)
	return -1;

    sprintf(tmp1, "%02d%02d%02d", date[0], date[1], date[2]);
    sprintf(tmp2, "%02d%02d%02d", time[0], time[1], time[2]);

    for(port=0; port<PORT_NUM; port++)
      if(Sys_mode.com_m[port]) {
        DownloadCode(tmp1, 0, 6, 0xC1, port);
        DownloadCode(tmp2, 0, 6, 0xC2, port);
      }

    return 0;
}