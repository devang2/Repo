

#include <dir.h>
#include <dos.h>
#include <alloc.h>
#include <conio.h>
#include <string.h>
#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"
#define  MAXNUM    50

UL          *modi_no;        /* modified/deleted record No */
TIME_STRUCT *modi_date;

/* function  : display and handle the out-of-range telephone records
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void out_of_range(void)
{
    UC           ratify;
    TABLE_STRUCT record_tbl = { 0, 99, 20, 18, 16, 9,
				{44, 85, 70, 175, 44, 44, 44, 52, 60},
				GREEN
			       };

    TIME_STRUCT  in_stime={0,0,0,0}, in_etime={0,0,0,24};

    ratify = set_in_time(&in_stime, &in_etime);       /* set in-range time */
    if(ratify == 1)          /* ESC */
	return;

    modi_no = (UL *)farcalloc(MAXNUM, sizeof(UL));
    if(modi_no == NULL)
	exit_scr(1,"Out of memory %lu bytes!\n\nGFS system shutdown abnormally\n\n\n\n",sizeof(UL)*MAXNUM);

    modi_date = (TIME_STRUCT *)farcalloc(MAXNUM, sizeof(TIME_STRUCT));
    if(modi_date == NULL)
	exit_scr(1,"Out of memory %lu bytes!\n\nGFS system shutdown abnormally\n\n\n\n", sizeof(TIME_STRUCT)*MAXNUM);

    clr_DialWin(2);

    draw_table(&record_tbl);

//    draw_table(&record_tbl);
    disp_out_head();
    disp_num();

    pop_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, 11);
    hz16_disp(MES_RIGHT+15, MES_TOP+1,"按键继续", 0);  /* press any key */

    hz16_disp(220,80,"越 界 国 际 长 途 话 单", BLACK);  /* Intern long call */
    out_range_proc(IDD, in_stime, in_etime);

    clr_scr(220,80,440,97,0,7);
    hz16_disp(220,80,"越 界 国 内 长 途 话 单", BLACK);  /* national long call */
    out_range_proc(DDD, in_stime, in_etime);

    clr_scr(220,80,440,97,0,7);
    hz16_disp(220,80,"越 界 市 内 电 话 话 单", BLACK);   /* local call */
    out_range_proc(LDD, in_stime, in_etime);

    farfree(modi_date);
    farfree(modi_no);

    rid_pop();
    recover_screen(2);
    return;
}

/* function  : judge whether the file exists or not
 * called by : a general purposed routine
 * input     : filename -- file name to be checked
 * output    : TRUE  -- exist
 *             FALSE -- not exist
 * date      : 1993.10.12
 */
UC file_exist(UC *filename)
{
    struct ffblk fblk;

    if(findfirst(filename,&fblk,FA_RDONLY) == 0)      /* find */
	return(TRUE);
    else
	return(FALSE);
}

/* function  : set in-range time
 * called by : out_of_range()
 * date      : 1993.10.8
 */
UC  set_in_time(TIME_STRUCT *in_stime, TIME_STRUCT *in_etime)
{
    int  i,j;
    UC   backx=10, backy=20;
    UC   ratify;
    UI   sx, sy, ex;
    TABLE_STRUCT time_tbl = {H_BX-30,H_BY-30,0,40,2,7,\
			       {40,50,40,40,40,40,40},7};
    UNIT_STRUCT  cur_unit;

    message_disp(8,"←↓→↑ 移动  Enter 输入  F1 确认");  /* move and input */
    pop_back(H_BX-40,H_BY-70,H_BX+270,H_BY+80,7); /* big frame */
    draw_table(&time_tbl);
    hz16_disp(H_BX+50,H_BY-50,"设 定 界 内 时 间",BLACK);  /* set range time */

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
		    if(i==0)      /* from */
			hz16_disp(sx+backx,sy+backy,"从",BLACK);
		    if(i==1)      /* to   */
			hz16_disp(sx+backx,sy+backy,"到",BLACK);
		    break;
		case 2:           /* year */
		    hz16_disp(sx+backx,sy+backy,"年",BLACK);
		    break;
		case 4:           /* month*/
		    hz16_disp(sx+backx,sy+backy,"月",BLACK);
		    break;
		case 6:           /* day  */
		    hz16_disp(sx+backx,sy+backy,"日",BLACK);
		    break;
		case 1:
		    draw_back(sx+backx-12, sy+backy-4, ex+2, sy+backy+20,11);
		    outf(sx+5, sy+backy , 11, BLACK, "%4u", s_time.year);
		    if(i==1)
		    {
			draw_back(sx+backx-12, sy+backy-4, ex+2, sy+backy+20, 11);
			outf(sx+5,sy+backy,11,BLACK,"%4u",e_time.year);
		    }
		    break;
		case 3:
		    draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
		    outf(sx+backx,sy+backy,11,BLACK,"%2u",s_time.month);
		    if(i==1)
		    {
                        draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
			outf(sx+backx,sy+backy,11,BLACK,"%2u",e_time.month);
		    }
		    break;
		case 5:
		    draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
		    outf(sx+backx,sy+backy,11,BLACK,"%2u",s_time.day);
		    if(i==1)
		    {
		        draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
			outf(sx+backx,sy+backy,11,BLACK,"%2u",e_time.day);
		    }
		    break;
		default:
		    sound_alarm();
		    break;
	    }      /* end of "switch(j)"     */
	}          /* end of "for(j), for(i) */

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 1);

    in_stime->year = s_time.year;
    in_stime->month= s_time.month;
    in_stime->day  = s_time.day;
    in_etime->year = e_time.year;
    in_etime->month= e_time.month;
    in_etime->day  = e_time.day;

    ratify = input_in_time(in_stime, in_etime);

    rid_pop();
    message_end();
    return(ratify);
}

/* function  : input starting time and ending time for sorting
 * called by : set_in_time()
 * output    : in_stime -- starting time for deleting
 *             in_etime -- ending   time for deleting
 *             esc=1: ESC
 *             esc=2: F1(ratify)
 * date      : 1993.10.6
 */
UC  input_in_time(TIME_STRUCT *in_stime,TIME_STRUCT *in_etime)
{
    UC   esc=0, result=0;
    UC   backx=10, backy=20, high=18, width=40;
    UI   sx, sy;
    UI   key;
    UL   value;
    UNIT_STRUCT  cur_unit;

    for(; ;)             /* input data */
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
		if(cur_unit.unit_x==0)      /* starting time    */
		{
		    switch(cur_unit.unit_y)
		    {
		    case 1:     /* year */
			hide_finger();
			result = get_dec(sx,sy+backy,high,width,5,4,\
							    &value,0x00);
			echo_finger();
			if(result)     /* valid input */
			{
			    in_stime->year = (UI)value;
			    move_finger(3,2);
			}
			break;
		    case 3:       /* month */
			hide_finger();
			result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			echo_finger();
			if(result)     /* valid input */
			{
			    in_stime->month = (UC)value;
			    move_finger(3,2);
			}
			break;
		    case 5:       /* day */
			hide_finger();
			result = get_dec(sx,sy+backy,high,width,backx,2,\
							    &value,0x00);
			echo_finger();
			if(result)     /* valid input */
			{
			    in_stime->day = (UC)value;
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
			    result = get_dec(sx,sy+backy,high,width,5,4,\
							    &value,0x00);
			    echo_finger();
			}while(result && (value<in_stime->year));

			if(result)     /* valid input */
			{
			    in_etime->year = (UI)value;
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
			}while(result && (in_stime->year==in_etime->year) \
				      && (value<in_stime->month) );

			if(result)     /* valid input */
			{
			    in_etime->month = (UC)value;
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
			}while(result && (in_stime->year==in_etime->year)   \
				      && (in_stime->year==in_etime->year)   \
				      && (value<in_stime->day) );

			if(result)     /* valid input */
			    in_etime->day = (UC)value;
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
    }                        /* end of "for(;;)"     */
    return(esc);
}


/* function  : display head for out-of-range records
 * called by : out_of_range()
 * date      : 1993.10.8
 */
void disp_out_head(void)
{
    outf(7,   103, 7,0, "序号");      /* sequence num  */

    outf(52,  103, 7,0, "分  机");   /* caller        */
    outf(133, 103, 7,0, "授权用户"); /* authorized user */
    outf(207, 103, 7,0, "通达地区");  /* callee        */
    outf(281, 103, 7,0, "被    叫");  /* callee        */
    outf(386, 103, 7,0, "日期");      /* date          */
    outf(433, 103, 7,0, "起时");      /* starting time */
    outf(478, 103, 7,0, "时长");      /* duration      */
    outf(518, 103, 7,0, "附加费");    /* main caller 2 */
    outf(576, 103, 7,0, "话  费");    /* charge fee    */

    return;
}


/* function  : display head for out-of-range records
 * called by : out_of_range()
 * date      : 1993.10.8
 */
void disp_num(void)
{
    int i;

    for(i=0; i<16; i++)     /* DISPLAY No. IN THE TABLE     */
	    outf(20, i*19+123, 7, 0, "%x", i);

    return;
}

/* function  : handle the out-of-range records
 * called by : out_of_range()
 * input     : mode -- IDD,DDD,LDD
 *             in_stime -- in_range starting time
 *             in_etime -- in_range ending   time
 * date      : 1993.10.8
 * procedure : first display the out-of-range records, when full of 16,
 *             ask whether to delete or modify some items. If not, go
 *             to display the next page, otherwise, remember the
 *             record No(the sequence No in *.dat) to modi_no[], and
 *             set the corresponding modi_date[]. The max number of
 *             records that can be deleted or modified is 50.
 *                 In file_refresh(), compare the record No with modi_no[],
 *             if equal, reset the date or not copy it, according to
 *             modifying or deleting
 * data      : modi_no[] -- remember the record No of modified/deleted records
 *                          in file (bei shan chu hua dan de ji lu hao)
 *             modi_date[] -- remember the modified date of modified/deleted
 *                            records
 *             modi -- remember all the number of records modified or deleted,
 *                     used for modi_no[] and modi_date[]
 *             tbl_no -- the row number in the table, transformed from
 *                       '0'-'9' or 'a'-'f'
 *             out_no[] -- remember the record No in the file for the 16
 *                         out-of-range records in the table
 */
void out_range_proc(UC mode, TIME_STRUCT in_stime, TIME_STRUCT in_etime)
{
    FILE *fp;
    char *a_name="不明";
    UC   auth_name[9];
    UC   position=0;              /* for display in table */
    UC   re_do=0, ch[2]="\0\0";
    UC   modi=0, tbl_no;
    UC   res, in_flag, modi_flag, pass_flag, flag;
    UI   i, key;
    UL   rec_no;
    UL   out_no[16], value;
    RATE_STRUCT huge *rate_tmp;
    UNIT_STRUCT   cur_unit;

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
	message(RATE_FILE_ERR);
	return;
    }

    flag  = load_authcd_tbl(0);
    if(flag != TRUE)
        Authflg = OFF;
    else
        Authflg = ON;

    if(mode == IDD)
    {
	fp = fopen("idd.dbf","rb");
	if(fp==NULL)
	{
	    get_key1();
	    unload_rate_tab();
            if(Authflg == ON)
                unload_authcd_tbl();
	    return;
	}
    }
    else if(mode == DDD)
    {
	fp = fopen("ddd.dbf","rb");
	if(fp==NULL)
	{
	    get_key1();
	    unload_rate_tab();
            if(Authflg == ON)
                unload_authcd_tbl();
	    return;
	}
    }
    else if(mode == LDD)
    {
	fp = fopen("ldd.dbf","rb");
	if(fp==NULL)
	{
	    get_key1();
	    unload_rate_tab();
            if(Authflg == ON)
                unload_authcd_tbl();
	    return;
	}
    }

    modi_flag = 0;
    rec_no = 0;
    for(i=0;i<MAXNUM;i++)
    {
	modi_no[i] = 0;
	modi_date[i].year = 0;
	modi_date[i].month= 0;
	modi_date[i].day  = 0;
	modi_date[i].hour = 0;
    }

    while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
    {
	rec_no++;                               /* record no in file    */
	in_flag = in_time_range(in_stime,in_etime);
	if(in_flag && !EndofRecFile(fp))        /* in the in-time range */
	    continue;

	if(in_flag == FALSE)
	{
            if(mode == IDD)
            {
	        rate_tmp = get_rate(&temp_list.callee[2],IDD);   /* get charge rate */
            }
            else if(mode == DDD)
            {
	        rate_tmp = get_rate(&temp_list.callee[1],DDD);   /* get charge rate */
            }
            else
	        rate_tmp = get_rate(temp_list.callee,LDD);   /* get charge rate */

            if(rate_tmp == NULL)              /* the area No. does not exist    */
	        strcpy(rate1.area_name, a_name);
	    else
	        rate1 = *rate_tmp;

            auth_usr_proc(auth_name);

	    disp_out_record(auth_name, position); /* display the out-of-range rec. */
	    out_no[position] = rec_no;
	    position++;
	}

	if( (position>=16) || (EndofRecFile(fp) && position>=1) )
	{
	    mes_disp(0,"选择序号: " );           /* input the row number */
	    GOTOxy(MES_LEFT+90,MES_TOP+1);

	    do
	    {
		re_do = 0;
		clr_scr(MES_LEFT+90,MES_TOP+1,MES_RIGHT-20,MES_BOTTOM-5,WHITE,11);

		key = get_key1();

		/* modify the date or delete some items in the tbl   */
		if( (modi<MAXNUM) &&                       \
		    ( ((key&0xff)>='0' && (key&0xff)<='9') ||
		      ((key&0xff)>='a' && (key&0xff)<='f') ))
		{
		    if(modi_flag == 0)      /* set the modifing flag */
			modi_flag = 1;

		    if(modi < MAXNUM)       /* if bigger than MAXNUM */
			re_do = 1;          /* do not handle it      */

		    ch[0] = (UC)(key&0xff);      /* NO in the table  */
		    outf(MES_LEFT+90,MES_TOP+1,11,WHITE,"%s",ch);
		    if((ch[0]>='0') && (ch[0]<='9'))
			tbl_no = ch[0]-'0';
		    else
			tbl_no = ch[0]-'a'+10;
		    if(tbl_no >= position)  /* out of the pos in tbl now */
			continue;

		    /* check whether the record No bas been modified/deleted
		     * or not. If has been, neglect it. That is to say,
		     * a record No can't be handled twice
		     */
		    res = 0;
		    for(i=0;i<modi;i++)
			if(out_no[tbl_no] == modi_no[i])
			{
			    res = 1;
			    break;
			}
		    if(res == 1)
			continue;

		    modi_no[modi] = out_no[tbl_no];

		    mes_disp(120,"1.修改日期  2.删除"); /* modify or del */
		    value = 0;
		    do
		    {
			res = get_dec(MES_LEFT+300,MES_TOP+1,18,20,0,1,\
					  &value,0x00);
		    }while((value!=1) && (value!=2));

		    if(value==2)       /* delete an item */
		    {
//			pass_flag = del_one_pass(); //out_del_pass();
			pass_flag = valid_pass(DEL_PWD); //out_del_pass();
			if(pass_flag == FALSE)           /* not right  */
			{
			    mes_disp(230,"口令错误!");
			    delay_10ms(80);
			    continue;
			}

			setcolor(12);  /* MAGENTA        */
			cur_unit.unit_x = tbl_no;
			cur_unit.unit_y = 1;
			get_certain(&cur_unit);
			line(cur_unit.dot_sx+2,cur_unit.dot_sy+9, \
					   620,cur_unit.dot_sy+9);
			modi_date[modi].year = 8000;
		    }
		    else               /* modify the date */
		    {
			mes_disp(140," 年    月    日");    /* year,mon,day */

			value = 0;
			do
			{
			    res = get_dec(MES_LEFT+120,MES_TOP+1,18,20,0,2,\
					      &value,0x00);
			}while((value<1) || (value>=100));
			modi_date[modi].year = (UI)value+1900;

			value = 0;
			do
			{
			    res = get_dec(MES_LEFT+170,MES_TOP+1,18,20,0,2,\
					      &value,0x00);
			}while((value<1) || (value>12));
			modi_date[modi].month = (UC)value;

			value = 0;
			do
			{
			    res = get_dec(MES_LEFT+220,MES_TOP+1,18,20,0,2,\
					      &value,0x00);
			}while((value<1) || (value>31));
			modi_date[modi].day = (UC)value;

			cur_unit.unit_x = tbl_no;
			cur_unit.unit_y = 4;
			get_certain(&cur_unit);
			outf(cur_unit.dot_sx+4,cur_unit.dot_sy+1,7,0, \
			     "%02u/%02u",\
			      modi_date[modi].month,modi_date[modi].day );
		    }        /* end of "else if (key=='1')" */

		    modi++;
		}  /* end of "if( ((key&0xff)>='0' && (key&0xff)<='9').." */

		else if((key==ESC) || EndofRecFile(fp))
		{
		    fclose(fp);

		    if(modi_flag)           /* modify or delete some items  */
		    {
			mes_disp(0,"正在存盘,请稍候...");  /* saving,wait...*/
			file_refresh(mode,modi);        /* refresh the file */
		    }

		    clr_scr(MES_LEFT,MES_TOP+1,MES_RIGHT-20,MES_BOTTOM-5,\
			    WHITE,11);
//		    message(MAIN_MES);
                    brush_tbl(9, 16, 7);
                    disp_num();
                    unload_rate_tab();
                    if(Authflg == ON)
                        unload_authcd_tbl();
		    return;
		}       /* end of "if(key == ESC)" */

		else
		{
                    brush_tbl(9, 16, 7);
                    disp_num();
		    position = 0;
		}

	    }while(re_do);
	}      /* end of "if( (position>=16) || (EndofRecFile(fp)) )" */
    }          /* end of "while(fread(...fp)==1)" */
    fclose(fp);

    if(modi_flag)           /* modify or delete some items  */
    {
	mes_disp(0,"正在存盘,请稍候...");  /* saving,wait...*/
	file_refresh(mode,modi);        /* refresh the file */
    }

    clr_scr(MES_LEFT,MES_TOP+1,MES_RIGHT-20,MES_BOTTOM-5,WHITE,11);
//    message(MAIN_MES);
    brush_tbl(9, 16, 7);
    disp_num();

    unload_rate_tab();
    if(Authflg == ON)
        unload_authcd_tbl();
    return;
}

/* function  : display a out-of-range record in the table
 * called by : out_range_proc()
 * input     : pos -- display position in the table
 * date      : 1993.10.5
 */
void disp_out_record(char *auth_name, UC pos)
{
    int  i, j, k, l;
    int  dh, dm, ds;
    UC   area_n[19];
    UC   tel_no[16];
    UI   x, y;
    UNIT_STRUCT cur_unit;

    cur_unit.unit_x = pos;
    for(j=1;j<10;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+4;
	y = cur_unit.dot_sy+1;

	switch(j)
	{
	    case 1:
		outf(x,y,7,0,"%-lu",temp_list.caller2);
		break;
	    case 2:
	        if(temp_list.auth_code != 0xaaaaaa)
                    outf(x, y, 7, 0,"%-s",auth_name);
		break;
	    case 3:
/*		if( temp_list.callee[0] != 0 )    /* LDD */
		{
		     outf(x-2,y,7,0,"本地");
		     i=0;
		     while( (temp_list.callee[i] != 0xfd) && (i<14) )
		     {
			 tel_no[i] = temp_list.callee[i]+'0';
			 i++;
		     }
		     tel_no[i] = '\0';
		     outf(x+32,y,7,0,"%-s",tel_no);
		}
		else                                /* DDD IDD */
		{
*/
		     i=0;
		     while( (temp_list.callee[i] != 0xfd) && (i<14) )
		     {
			 tel_no[i] = temp_list.callee[i]+'0';
			 i++;
		     }
		     tel_no[i] = '\0';
		     l=(21-i)/2;
                     if(l > 4) l = 4;
		     for(k=0; k<l*2; k++)
			 area_n[k]=rate1.area_name[k];
		     area_n[k] = '\0';
		     outf(x-2,y,7,0, area_n);
		     outf(x+l*16,y,7,0,"%-s",tel_no);
//		}
		break;
	    case 4:
		outf(x,y,7,0,"%02u/%02u", temp_list.mon1,temp_list.day1);
		break;
	    case 5:
		outf(x,y,7,0,"%02u:%02u",temp_list.hour1, temp_list.min1);
		break;
	    case 6:
		 dh = temp_list.hour2 - temp_list.hour1;
		 dm = temp_list.min2  - temp_list.min1;
		 ds = temp_list.sec2  - temp_list.sec1;
		 if( ds < 0 )
		 {
		     ds += 60;
		     dm --;
		 }
		 if( dm < 0 )
		 {
		     dm += 60;
		     dh --;
		 }
		 if( dh < 0 )
		 {
		     dh += 24;
		 }

		 if(ds != 0)
		     dm++;
		 if(dm == 60)
		 {
		    dm = 0;
		    dh ++;
		 }

		 outf(x,y,7,0,"%02d:%02d", dh, dm);
		 break;
	    case 7:      /* add fee      */
		 outf(x,y,7,0,"%6.2f",(double)(temp_list.add)/100);
		 break;
	    case 8:
		outf(x,y,7,0,"%7.2f",(double)(temp_list.charge)/100);
		break;
	}     /* end of "switch(j)"        */
    }         /* end of "for(j=0;j<8;j++)" */
    return;
}


/* function  : refresh the *.dat according to the modified or deleted item
 * called by : out_range_proc()
 * input     : mode -- IDD,DDD,LDD
 *             modi -- all the number of records that have been modified
 *                     or deleted
 * date      : 1993.10.8
 */
void file_refresh(UC mode, UC modi)
{
    int  i;
    UC   num=0;
    FILE *fps,*fpd;
    UC   skip=FALSE;
    UL   counter=0;

    if(mode == IDD)
    {
	fps = fopen("idd.dbf","rb");
	fpd = fopen("idd.out","wb");
    }
    else if(mode == DDD)
    {
	fps = fopen("ddd.dbf","rb");
	fpd = fopen("ddd.out","wb");
    }
    else if(mode == LDD)
    {
	fps = fopen("ldd.dbf","rb");
	fpd = fopen("ldd.out","wb");
    }

    while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fps) == 1)
    {
	skip = FALSE;
	counter++;

	if(num < modi)
	{
	    for(i=0;i<modi;i++)        /* compare */
	    {
		if(counter == modi_no[i])
		{
		    num++;
		    if(modi_date[i].year == 8000)     /* deleted  */
			skip = TRUE;
		    else                              /* modified */
		    {
			temp_list.year2 = modi_date[i].year;
			temp_list.mon2  = modi_date[i].month;
			temp_list.day2  = modi_date[i].day;
			temp_list.year1 = modi_date[i].year;
			temp_list.mon1  = modi_date[i].month;
			temp_list.day1  = modi_date[i].day;
		    }
		    break;
		}       /* end of "if(counter == modi_no[i])" */
	    }      /* end of "for(i=0;i<modi;i++)" */
	}     /* end of "if(num < modi)"  */

	if(skip != TRUE)
	    fwrite(&temp_list,sizeof(RECORD_STRUCT),1,fpd);
    }    /* end of "while(fread(...fps)==1)" */

    fclose(fpd);
    fclose(fps);

    if(mode == IDD)
    {
	fcopy("idd.out","idd.dbf");
	fcopy("idd.out","idd.tmp");
	remove("idd.out");
    }
    else if(mode == DDD)
    {
	fcopy("ddd.out","ddd.dbf");
	fcopy("ddd.out","ddd.tmp");
	remove("ddd.out");
    }
    else if(mode == LDD)
    {
	fcopy("ldd.out","ldd.dbf");
	fcopy("ldd.out","ldd.tmp");
	remove("ldd.out");
    }

    return;
}

/* function  : display  "msg" in the status window. The only method to
 *             retrieve the old message is to re_display the message
 * called by : out_rang_proc()
 * input     : back_in -- distance to MES_LEFT
 *             msg -- the message to be displayed
 * date      : 1993.10.8
 */
void mes_disp(UC back_in, UC *msg)
{
    clr_scr(MES_LEFT+back_in,MES_TOP+1,MES_RIGHT-20,MES_BOTTOM-5,WHITE,11);
    hz16_disp(MES_LEFT+back_in,MES_TOP+1, msg,0);

    return;
}
