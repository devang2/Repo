#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : delete all the telephone records in the sorting time range
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void delete_all_record(UC isauthcd)
{
    UC   ratify, ret_val, flag;
    TIME_STRUCT  del_stime={0,0,0,0}, del_etime={0,0,0,24};

    flag = valid_pass(DEL_PWD);
    if(flag == 2)                 /* ESC */
        return;
    if(flag == FALSE)
    {
        message(PASS_ERR);
        return;
    }

    ratify = set_del_time(&del_stime,&del_etime);
    if((ratify==1) || (del_etime.year==0) || (del_etime.month==0) || (del_etime.day==0))
        return;

    /* Conform for deleting*/
    if(isauthcd == AUTH_CD)
        ret_val = select_1in2(9);
    else if(isauthcd == PHONE_NO)
        ret_val = select_1in2(8);
    else
        ret_val = select_1in2(7);

    if (ret_val!=2) return;

    message_disp(8,"正在进行删除, 请稍候...");   /* deleting, please wait */

    del_a_kind_file(IDD, CRNT_F, isauthcd, del_stime, del_etime);
    del_a_kind_file(IDD, HIST_F, isauthcd, del_stime, del_etime);
    del_a_kind_file(DDD, CRNT_F, isauthcd, del_stime, del_etime);
    del_a_kind_file(DDD, HIST_F, isauthcd, del_stime, del_etime);
    del_a_kind_file(LDD, CRNT_F, isauthcd, del_stime, del_etime);
    del_a_kind_file(LDD, HIST_F, isauthcd, del_stime, del_etime);

    message_end();

    return;
}

/* function  : set time for deleting
 * called by : delete_record()
 * output    : ratify=1: ESC
 *             ratify=2: F1
 * date      : 1993.10.5
 */
UC  set_del_time(TIME_STRUCT *del_stime,TIME_STRUCT *del_etime)
{
    int  i,j;
    UC   backx=10,backy=20;
    UC   ratify;
    UI   sx,sy,ex;
    TABLE_STRUCT time_tbl = {H_BX-30,H_BY-30,0,40,2,7,\
                               {40,50,40,40,40,40,40},7};
    UNIT_STRUCT  cur_unit;

    message_disp(8,"←↓→↑ 移动  Enter 输入  F1 确认");  /* move and input */
    pop_back(H_BX-40,H_BY-70,H_BX+270,H_BY+80,7); /* big frame */
    draw_table(&time_tbl);
    hz16_disp(H_BX+25,H_BY-45,"设 定 删 除 时 间 区 间",BLACK);  /* set deleting time */

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
                    draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
                    break;
                case 3:
                    draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
                    break;
                case 5:
                    draw_back(sx-2,sy+backy-4,ex+2,sy+backy+20,11);
                    break;
                default:
                    sound_alarm();
                    break;
            }      /* end of "switch(j)"     */
        }          /* end of "for(j), for(i) */

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);

    ratify = input_del_time(del_stime,del_etime);

    rid_pop();
    message_end();
    return(ratify);
}

/* function  : input starting time and ending time for sorting
 * called by : set_del_time()
 * output    : del_stime -- starting time for deleting
 *             del_etime -- ending   time for deleting
 *             esc=1: ESC
 *             esc=2: ratify (F1)
 * date      : 1993.10.6
 */
UC  input_del_time(TIME_STRUCT *del_stime,TIME_STRUCT *del_etime)
{
    UC   esc=0,result=0;
    UC   backx=10,backy=20,high=20,width=40;
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
                        do
                        {
                            hide_finger();
                            result = get_dec(sx,sy+backy,high,width,5,4,\
                                                            &value,0x00);
                            echo_finger();
                        }while(result  && (value < 1900) );

                        if(result)     /* valid input */
                        {
                            del_stime->year = (UI)value;
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
                        }while(result &&  (value > 12) );

                        if(result)     /* valid input */
                        {
                            del_stime->month = (UI)value;
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
                        }while(result && (value > 31) );

                        if(result)     /* valid input */
                        {
                            del_stime->day = (UI)value;
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
                        }while(result && ( (value<1993) || (value<del_stime->year) ));

                        if(result)     /* valid input */
                        {
                            del_etime->year = (UI)value;
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
                        }while(result && ((value<1) || (value>12) || \
                                          ((del_stime->year==del_etime->year) && \
                                           (value<del_stime->month)) ));

                        if(result)     /* valid input */
                        {
                            del_etime->month = (UI)value;
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
                        }while(result && ((value<1) || (value>31) || \
                                          ((del_stime->year ==del_etime->year) && \
                                           (del_stime->month==del_etime->month)&& \
                                           (value<del_stime->day)) ));

                        if(result)     /* valid input */
                            del_etime->day = (UI)value;
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

/* function  : judge whether the record is in the deleting time range or not
 * called by : delete_record()
 * input     : del_stime -- starting time for deleting
 *             del_etime -- ending   time for deleting
 * output    : TRUE  -- in the deleting range
 *             FALSE -- not in the deleting range
 * date      : 1993.10.5
 * note      : only judge whether the ending time of the record is in the
 *             range or not
 */
UC in_time_range(TIME_STRUCT del_stime,TIME_STRUCT del_etime)
{
    UC  ok1=0, ok2=0;

    if(temp_list.year2 > del_stime.year)
        ok1 = 1;
    else if(temp_list.year2 == del_stime.year)
        if(temp_list.mon2 > del_stime.month)
            ok1 = 1;
        else if(temp_list.mon2 == del_stime.month)
            if(temp_list.day2 > del_stime.day)
                ok1 = 1;
            else if(temp_list.day2 == del_stime.day)
                if(temp_list.hour2 >= del_stime.hour)
                    ok1 = 1;

    if(ok1 == 1)
    {
        if(temp_list.year2 < del_etime.year)
            ok2 = 1;
        else if(temp_list.year2 == del_etime.year)
            if(temp_list.mon2 < del_etime.month)
                ok2 = 1;
            else if(temp_list.mon2 == del_etime.month)
                if(temp_list.day2 < del_etime.day)
                    ok2 = 1;
                else if(temp_list.day2 == del_etime.day)
                    if(temp_list.hour2 <= del_etime.hour)
                        ok2 = 1;
    }  /* if(ok1==1) */

    if((ok1==1) && (ok2==1))
        return(TRUE);
    else
       return(FALSE);
}

UC valid_pass(UC mode)
{
    int  i,cmp;
    FILE *fp;
    UC   password0[7]="",password1[7]="";
    UC   input_num=0,flag;

    if(mode == CLR_PWD)
    {
        fp = fopen("gfsdata\\clrpwd.dat","rb");        /* read the password */
    }
    else if(mode == DEL_PWD)
    {
        fp = fopen("gfsdata\\delpwd.dat","rb");        /* read the password */
    }
    else
    {
        fp = fopen("gfsdata\\cashpwd.dat","rb");        /* read the password */
    }

    if(fp == NULL)
        flag = 0;
    else
    {
        flag = 1;
        fread(password0,sizeof(UC),7,fp);
        fclose(fp);
    }

    if(mode == DEL_PWD)
        message_disp(8," 输入话单备份/删除操作口令   Enter 输入");
    else if(mode == CLR_PWD)
        message_disp(8," 输入清除话单存储器操作口令   Enter 输入");
    else
        message_disp(8," 输入用户管理口令   Enter 输入");

    pop_back(H_BX-40,H_BY-10,H_BX+260,H_BY+35,7); /* big frame */
    draw_back(H_BX+132,H_BY,H_BX+250,H_BY+25,11);
    if(mode == DEL_PWD)
        hz16_disp(H_BX-20,H_BY+5,"备份/删除操作口令",BLACK);  /* input password   */
    else if(mode == CLR_PWD)
        hz16_disp(H_BX-25,H_BY+5,"清除缓存器操作口令",BLACK);  /* input password   */
    else
        hz16_disp(H_BX-17,H_BY+5,"用户管理操作口令",BLACK);  /* input password   */
    input_num = get_pass(H_BX+138,H_BY+5,11,0,password1,6);
    rid_pop();
    message_end();

    if(input_num == 0xFF)         /* ESC */
        return(2);

    if(input_num != 6)
        return(FALSE);

    for(i=0;i<6;i++)
        password1[i] += 0x10;
    password1[6] = '\0';

    if(flag == 0)       /* the password is input the first time */
    {
        if(mode == DEL_PWD)
        {
            fp = fopen("gfsdata\\delpwd.dat","wb");
        }
        else if(mode == CLR_PWD)
        {
            fp = fopen("gfsdata\\clrpwd.dat","wb");
        }
        else
        {
            fp = fopen("gfsdata\\cashpwd.dat","wb");
        }
        fwrite(password1,sizeof(UC),7,fp);
        fclose(fp);

        return(TRUE);
    }
    else
    {
        cmp = strcmp(password0,password1);
        if(cmp == 0)        /* the same     */
            return(TRUE);
        else
            return(FALSE);
    }
}

void del_a_kind_file(UC mode, UC is_hst, UC isauthcd, TIME_STRUCT del_stime,\
                     TIME_STRUCT del_etime)
{
    FILE *fps, *fpd;
    UC deled_file[15]="";

    if(mode == IDD)
        strcpy(deled_file, "idd");
    else if(mode == DDD)
        strcpy(deled_file, "ddd");
    else
        strcpy(deled_file, "ldd");

    if(is_hst == HIST_F)
        strcat(deled_file, ".hst");
    else
        strcat(deled_file, ".dbf");

    fps = fopen(deled_file,"rb");

    if(fps != NULL)
    {
        if(mode == IDD)
            fpd = fopen("idd.del","wb");
        else if(mode == DDD)
            fpd = fopen("ddd.del","wb");
        else
            fpd = fopen("ldd.del","wb");

        while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fps) == 1)
        {
            if(in_time_range(del_stime, del_etime) /*&& temp_list.flag == FALSE*/) /* in deleting range */
            {
                if( (isauthcd == PHONE_NO) && (temp_list.auth_code != 0xaaaaaa) )
                    fwrite(&temp_list,sizeof(RECORD_STRUCT),1,fpd);
                else if( (isauthcd == AUTH_CD) && (temp_list.auth_code == 0xaaaaaa) )
                    fwrite(&temp_list,sizeof(RECORD_STRUCT),1,fpd);
            }
            else
                fwrite(&temp_list,sizeof(RECORD_STRUCT),1,fpd);
        }          /* end of "while(!feof(fps))" */
        fclose(fpd);
        fclose(fps);

        if( mode == IDD && is_hst != HIST_F)
        {
            fcopy("idd.del","idd.dbf");        /* .del to .dat */
            fcopy("idd.del","idd.tmp");        /* .del to .tmp */
            remove("idd.del");
        }
        else if(mode == IDD && is_hst == HIST_F)
        {
            fcopy("idd.del","idd.hst");        /* .del to .dat */
            remove("idd.del");
        }
        else if(mode == DDD && is_hst != HIST_F)
        {
            fcopy("ddd.del","ddd.dbf");        /* .del to .dat */
            fcopy("ddd.del","ddd.tmp");        /* .del to .tmp */
            remove("ddd.del");
        }
        else if(mode == DDD && is_hst == HIST_F)
        {
            fcopy("ddd.del","ddd.hst");        /* .del to .dat */
            remove("ddd.del");
        }
        else if(mode == LDD && is_hst != HIST_F)
        {
            fcopy("ldd.del","ldd.dbf");        /* .del to .dat */
            fcopy("ldd.del","ldd.tmp");        /* .del to .tmp */
            remove("ldd.del");
        }
        else if(mode == LDD && is_hst == HIST_F)
        {
            fcopy("ldd.del","ldd.hst");        /* .del to .dat */
            remove("ldd.del");
        }
    }
}
