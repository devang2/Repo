#include <stdlib.h>
#include <string.h>
#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

/* function  :
 * called by :
 * date      : 1993.12.11
 */
UC inq_acc_fee(UC isacc)
{
    UI   key;
    UI   position = 0;
    FILE         *fp;
    ACC_STRUCT   acc;
    TABLE_STRUCT  accfee_tbl = { 0, 97, 40, 18, 16, 7,
				{103, 65, 110, 110, 110, 66, 68},
                                GREEN
                               };

    fp = open_acc_file(isacc);
    if(fp == NULL)
        return(FALSE);

    clr_DialWin(2);
    disp_accfee_head(isacc);
    draw_table(&accfee_tbl);

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    while(fread(&acc, sizeof(ACC_STRUCT), 1, fp) == 1)
    {
        disp_a_accfee(&acc, position, isacc);
        position++;

        if( (position >= 16) && (!EndofAccFile(fp)) )
        {
            while(1)
            {
                key = get_key1();
                if(key == ESC)
                {
                    fclose(fp);
                    recover_screen(2);
                    message_end();
                    return(FALSE);
                }
                else if(key == ENTER)
                {
                    brush_tbl(10, 16, 7);
                    position = 0;
                    break;
                }
            }
        }
    }
    fclose(fp);

    while(1)
    {
        key = get_key1();
        if(key == ESC)
        {
            recover_screen(2);
            message_end();
            return(FALSE);
        }
        else if(key == ENTER)
            break;
    }

    recover_screen(2);
    message_end();
    return(TRUE);
}

/* function  :
 * called by :
 * date      : 1993.12.11
 */
UC prn_acc_fee(UC isacc)
{
    UI   i, num;
    UC   flag;
    FILE         *fp;
    ACC_STRUCT   acc;
    double l_f = 0, d_f = 0, i_f = 0, a_f = 0, total_f = 0;
    UL     l_c = 0, d_c = 0, i_c = 0;

    flag = check_prn();
    if(flag == FALSE)
    {
        return(FALSE);
    }

    fp = open_acc_file(isacc);
    if(fp == NULL)
        return(FALSE);

    message_disp(8,"正在打印，请稍候...");   /* printing */
    if(print_accrpt_head(isacc) == FALSE)
    {
        message_end();
        return(FALSE);
    }

    num = 0;
    while(fread(&acc, sizeof(ACC_STRUCT), 1, fp) == 1)
    {
        num ++ ;
        if(print_accrpt_detail(&acc, num, isacc) == FALSE)
        {
            fclose(fp);
            message_end();
            return(FALSE);
        }

        l_c += acc.local_count;
        d_c += acc.nation_count;
        i_c += acc.intern_count;
        l_f += (double)(acc.local_charge)/100;
        d_f += (double)(acc.nation_charge)/100;
        i_f += (double)(acc.intern_charge)/100;
        for(i=0; i<5; i++)
            a_f += (double)(acc.addfee[i])/100;
        total_f += (double)(acc.local_charge)/100+\
              ((double)(acc.nation_charge)+(double)(acc.intern_charge))/100+\
              (double)(acc.addfee[0])/100+\
              (double)(acc.addfee[1])/100+\
              (double)(acc.addfee[2])/100+\
              (double)(acc.addfee[3])/100+\
              (double)(acc.addfee[4])/100;
    }
    fclose(fp);

    if(prnf("总    计:            %4u", l_c) == FALSE)
    {
         message_end();
         return(FALSE);
    }

    if(prnf(" %8.2f %4u", l_f, d_c) == FALSE)
    {
         message_end();
         return(FALSE);
    }

    if(prnf(" %8.2f %4u",d_f,i_c) == FALSE)
    {
         message_end();
         return(FALSE);
    }

    if(prnf(" %8.2f %8.2f %8.2f\n", i_f, a_f, total_f) == FALSE)
    {
         message_end();
         return(FALSE);
    }

    if(print_accrpt_tail(isacc) == FALSE)
    {
         message_end();
         return(FALSE);
    }
    message_end();
    return(TRUE);
}

/*
 * date      : 1993.10.5
 */
void disp_accfee_head(UC isacc)
{
    if(isacc == FEE_ACC || isacc == HFEE_ACC)
        outf(218, 75, 7, 0, "已  结  算  话  费  报  告");
    else if(isacc == BAN_ACC || isacc == HBAN_ACC)
        outf(218, 75, 7, 0, "班  电  话  营  业  报  告");
    else if(isacc == DAY_ACC || isacc == HDAY_ACC)
        outf(218, 75, 7, 0, "日  电  话  营  业  报  告");
    else if(isacc == MON_ACC || isacc == HMON_ACC)
        outf(218, 75, 7, 0, "月  电  话  营  业  报  告");
    else if(isacc == YEAR_ACC || isacc == HYEAR_ACC)
        outf(218, 75, 7, 0, "年  电  话  营  业  报  告");

    outf(189, 102, 7,0, "市内电话");
    outf(301, 102, 7,0, "国内长话");
    outf(413, 102, 7,0, "国际长话");
    outf(512, 102, 7,0, "月租及");

    outf(11,  120, 7,0, "流  水  号");

    if(isacc == FEE_ACC || isacc == HFEE_ACC)
        outf(113, 120, 7, 0, "分  机");
    else if(isacc == BAN_ACC || isacc == HBAN_ACC)
        outf(113, 120, 7, 0, "班  次");
    else if(isacc == DAY_ACC || isacc == HDAY_ACC)
        outf(113, 120, 7, 0, "日  期");
    else if(isacc == MON_ACC || isacc == HMON_ACC)
        outf(113, 120, 7, 0, "月  份");
    else if(isacc == YEAR_ACC || isacc == YEAR_ACC)
        outf(113, 120, 7, 0, "年  份");

    outf(173, 120, 7,0, "次数");
    outf(241, 120, 7,0, "话费");
    outf(285, 120, 7,0, "次数");
    outf(353, 120, 7,0, "话费");
    outf(395, 120, 7,0, "次数");
    outf(465, 120, 7,0, "话费");
    outf(504, 120, 7,0, "附加费等");
    outf(574, 120, 7,0, "费用合计");

    return;
}

/* function  :
 * called by :
 * input     : pos -- display position in the table
 *             num -- sequence number
 * date      : 1993.10.5
 */
void disp_a_accfee(ACC_STRUCT *acc, UC pos, UC isacc)
{
    int    j;
    UI     x, y;
    double total_f;
    UC     serial_no[13];
    UNIT_STRUCT  cur_unit;

    get_s_no(acc, serial_no);

    total_f = (double)(acc->local_charge)/100+\
              ((double)(acc->nation_charge)+(double)(acc->intern_charge))/100+\
              (double)(acc->addfee[0])/100+\
              (double)(acc->addfee[1])/100+\
              (double)(acc->addfee[2])/100+\
              (double)(acc->addfee[3])/100+\
              (double)(acc->addfee[4])/100;

    cur_unit.unit_x = pos;
    for(j=0;j<10;j++)
    {
        cur_unit.unit_y = j;
        get_certain(&cur_unit);
        x = cur_unit.dot_sx+4;
        y = cur_unit.dot_sy+1;
        switch(j)
        {
            case 0:          /* sequence number */
                outf(x,y,7,0,"%s",serial_no);
                break;
            case 1:          /* sequence number */
                if(isacc == FEE_ACC || isacc == HFEE_ACC)
                    outf(x-4,y,7,0,"%7lu",acc->phone_no);
                else if(isacc == BAN_ACC || isacc == HBAN_ACC)
                    outf(x-4,y,7,0,"%4u",acc->seral_no.hour/8 + 1);
                else if(isacc == DAY_ACC || isacc == HDAY_ACC)
                    outf(x-4,y,7,0,"%4u",acc->seral_no.day);
                else if(isacc == MON_ACC || isacc == HMON_ACC)
                    outf(x-4,y,7,0,"%4u",acc->seral_no.month);
                else if(isacc == YEAR_ACC || isacc == YEAR_ACC)
                    outf(x-4,y,7,0,"%5u",acc->seral_no.year+1900);
                break;
            case 2:
                outf(x,y,7,0,"%4u",acc->local_count);
                outf(x+32,y,7,0,"%9.2f", (double)(acc->local_charge)/100);
                break;
            case 3:
                outf(x,y,7,0,"%4u",acc->nation_count);
                outf(x+32,y,7,0,"%9.2f", (double)(acc->nation_charge)/100);
                break;
            case 4:
                outf(x,y,7,0,"%4u",acc->intern_count);
                outf(x+32,y,7,0,"%9.2f", (double)(acc->intern_charge)/100);
                break;
            case 5:
                outf(x-4,y,7,0,"%8.2f",\
                    (double)(acc->addfee[0])/100+\
                    (double)(acc->addfee[1])/100+\
                    (double)(acc->addfee[2])/100+\
                    (double)(acc->addfee[3])/100+\
                    (double)(acc->addfee[4])/100);
                break;
            case 6:
                outf(x-2,y,7,0,"%8.2f",total_f);
                break;
        }     /* end of "switch(j)"        */
    }         /* end of "for(j=0;j<8;j++)" */

    return;
}

UC  EndofAccFile(FILE *fp)
{
    UC     flag;
    fpos_t filepos;
    ACC_STRUCT temp;

    fgetpos(fp, &filepos);
    if(fread(&temp, sizeof(ACC_STRUCT), 1, fp) != 1)   /* End of File*/
        flag = TRUE;
    else
        flag = FALSE;
    fsetpos(fp, &filepos);

    return(flag);
}

void get_s_no(ACC_STRUCT *acc, UC *serial_no)
{
    UC buf[3];

    serial_no[0]='\0';
    itoa( (int)acc->seral_no.year, buf, 10);
    if(buf[1]==NULL)
    {
         buf[1] = buf[0];
         buf[0] = 0x30;
         buf[2] = '\0';
    }
    strcat(serial_no, buf);
    itoa( (int)acc->seral_no.month, buf, 10);
    if(buf[1]==NULL)
    {
         buf[1] = buf[0];
         buf[0] = 0x30;
         buf[2] = '\0';
    }
    strcat(serial_no, buf);
    itoa( (int)acc->seral_no.day, buf, 10);
    if(buf[1]==NULL)
    {
         buf[1] = buf[0];
         buf[0] = 0x30;
         buf[2] = '\0';
    }
    strcat(serial_no, buf);
    itoa( (int)acc->seral_no.hour, buf, 10);
    if(buf[1]==NULL)
    {
         buf[1] = buf[0];
         buf[0] = 0x30;
         buf[2] = '\0';
    }
    strcat(serial_no, buf);
    itoa( (int)acc->seral_no.min, buf, 10);
    if(buf[1]==NULL)
    {
         buf[1] = buf[0];
         buf[0] = 0x30;
         buf[2] = '\0';
    }
    strcat(serial_no, buf);
    itoa( (int)acc->seral_no.sec, buf, 10);
    if(buf[1]==NULL)
    {
         buf[1] = buf[0];
         buf[0] = 0x30;
         buf[2] = '\0';
    }
    strcat(serial_no, buf);

    return;
}

/* function  :
 * called by :
 * input     : pos -- display position in the table
 *             num -- sequence number
 * date      : 1993.10.5
 */
UC print_a_acc(ACC_STRUCT *acc, UC isacc)
{
    double total_f;
    UC     serial_no[13];

    get_s_no(acc, serial_no);

    total_f = (double)(acc->local_charge)/100+\
              ((double)(acc->nation_charge)+(double)(acc->intern_charge))/100+\
              (double)(acc->addfee[0])/100+\
              (double)(acc->addfee[1])/100+\
              (double)(acc->addfee[2])/100+\
              (double)(acc->addfee[3])/100+\
              (double)(acc->addfee[4])/100;

    if(prnf("%12s", serial_no) == FALSE)
        return(FALSE);

    if(isacc == FEE_ACC || isacc == HFEE_ACC)
    {
        if(prnf(" %7lu",acc->phone_no) == FALSE)
            return(FALSE);
    }
    else if(isacc == BAN_ACC || isacc == HBAN_ACC)
    {
        if(prnf(" %7lu",(UL)(acc->seral_no.hour/8 + 1) ) == FALSE)
            return(FALSE);
    }
    else if(isacc == DAY_ACC || isacc == HDAY_ACC)
    {
        if(prnf(" %7lu",(UL)(acc->seral_no.day) ) == FALSE)
            return(FALSE);
    }
    else if(isacc == MON_ACC || isacc == HMON_ACC)
    {
        if(prnf(" %7lu",(UL)(acc->seral_no.month) ) == FALSE)
            return(FALSE);
    }
    else if(isacc == YEAR_ACC || isacc == YEAR_ACC)
    {
        if(prnf(" %7lu",(UL)(acc->seral_no.year) ) == FALSE)
            return(FALSE);
    }

    if(prnf(" %4u", acc->local_count) == FALSE)
        return(FALSE);

    if(prnf(" %8.2f %4u",
        (double)(acc->local_charge)/100,\
        acc->nation_count) == FALSE)
        return(FALSE);

    if(prnf(" %8.2f %4u",
        (double)(acc->nation_charge)/100,\
        acc->intern_count) == FALSE)
        return(FALSE);

    if(prnf(" %8.2f %8.2f %8.2f\n",
        (double)(acc->intern_charge)/100,\
        (double)(acc->addfee[0])/100+\
        (double)(acc->addfee[1])/100+\
        (double)(acc->addfee[2])/100+\
        (double)(acc->addfee[3])/100+\
        (double)(acc->addfee[4])/100,
        total_f) == FALSE)
        return(FALSE);

    return(TRUE);
}

UC print_accrpt_head(UC isacc)
{
    UC  *head[5] = {"       已  结  算  话  费  清  单",
                    "       班  电  话  营  业  报  表",
                    "       日  电  话  营  业  报  表",
                    "       月  电  话  营  业  报  表",
                    "       年  电  话  营  业  报  表"};

    if(isacc == FEE_ACC || isacc == HFEE_ACC)
    {
        if(prnfd("\n%s\n\n", head[0]) == FALSE)
            return(FALSE);
    }
    else if(isacc == BAN_ACC || isacc == HBAN_ACC)
    {
        if(prnfd("\n%s\n\n", head[1]) == FALSE)
            return(FALSE);
    }
    else if(isacc == DAY_ACC || isacc == HDAY_ACC)
    {
        if(prnfd("\n%s\n\n", head[2]) == FALSE)
            return(FALSE);
    }
    else if(isacc == MON_ACC || isacc == HMON_ACC)
    {
        if(prnfd("\n%s\n\n", head[3]) == FALSE)
            return(FALSE);
    }
    else if(isacc == YEAR_ACC || isacc == HYEAR_ACC)
    {
        if(prnfd("\n%s\n\n", head[4]) == FALSE)
            return(FALSE);
    }

    if(print_acctbl() == FALSE)
        return(FALSE);

    if(prnf("                      市 内 电 话   国 内 长 话   国 际 长 话    月租及\n") == FALSE)
        return(FALSE);
    if(isacc == FEE_ACC || isacc == HFEE_ACC)
    {
        if(print_accdtal("    分  机 ") == FALSE)
            return(FALSE);
    }
    else if(isacc == BAN_ACC || isacc == HBAN_ACC)
    {
        if(print_accdtal("    班  次 ") == FALSE)
            return(FALSE);
    }
    else if(isacc == DAY_ACC || isacc == HDAY_ACC)
    {
        if(print_accdtal("    日  期 ") == FALSE)
            return(FALSE);
    }
    else if(isacc == MON_ACC || isacc == HMON_ACC)
    {
        if(print_accdtal("    月  份 ") == FALSE)
            return(FALSE);
    }
    else if(isacc == YEAR_ACC || isacc == HYEAR_ACC)
    {
        if(print_accdtal("    年  份 ") == FALSE)
            return(FALSE);
    }
    if(print_acctbl() == FALSE)
        return(FALSE);

    return(TRUE);
}


UC print_accdtal(UC *name)
{
    if(prnf("流  水  号%s次数     话费 次数     话费 次数     话费   附加费 费用合计\n", name) == FALSE)
        return(FALSE);
    return(TRUE);
}


UC print_acctbl(void)
{
    if(prnf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") == FALSE)
        return(FALSE);
    return(TRUE);
}


UC print_accrpt_tail(UC isacc)
{
    FILE  *fp;
    struct date now;
    TIME_STRUCT stime, etime;

    if(isacc == FEE_ACC || isacc == HFEE_ACC)
        fp = fopen("gfsdata\\acctime.dat","rb");
    else if(isacc == BAN_ACC || isacc == HBAN_ACC)
        fp = fopen("gfsdata\\bantime.dat","rb");
    else if(isacc == DAY_ACC || isacc == HDAY_ACC)
        fp = fopen("gfsdata\\daytime.dat","rb");
    else if(isacc == MON_ACC || isacc == HMON_ACC)
        fp = fopen("gfsdata\\montime.dat","rb");
    else if(isacc == YEAR_ACC || isacc == HYEAR_ACC)
        fp = fopen("gfsdata\\yeartime.dat","rb");
    if(fp == NULL)
    {
        message(ACCTIME_FILE_ERR);
        return(FALSE);
    }

    fread(&stime, sizeof(TIME_STRUCT), 1, fp);
    fread(&etime, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);
    getdate(&now);

    if(print_acctbl() == FALSE)
        return(FALSE);
    if(prnf("起止时间：%-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时    制单时间：%-4u年%2u月%2u日\n",\
         stime.year, stime.month, stime.day,  stime.hour,\
         etime.year, etime.month, etime.day,  etime.hour, \
         now.da_year, now.da_mon, now.da_day )\
         == FALSE)
        return(FALSE);

    if(new_page() == FALSE)
        return(FALSE);

    return(TRUE);
}

UC print_accrpt_detail(ACC_STRUCT *acc, UI num, UC isacc)
{
    if(num%Page_len !=0 )
    {
       if(print_a_acc(acc, isacc)==FALSE)
           return(FALSE);
    }
    else
    {
       if(print_accrpt_tail(isacc) == FALSE)
           return(FALSE);
       if(print_accrpt_head(isacc) == FALSE)
           return(FALSE);
       if(print_a_acc(acc, isacc)==FALSE)
           return(FALSE);
    }
    return(TRUE);
}


FILE *open_acc_file(UC mode)
{
    FILE *fp;

    switch(mode)
    {
        case FEE_ACC:
            fp = fopen("gfsdata\\feeacc.dat","rb");
            if(fp == NULL)
            {
                message(ACCFEE_FILE_ERR);
                return NULL;
            }
            break;
        case BAN_ACC:
            fp = fopen("gfsdata\\banacc.dat","rb");
            if(fp == NULL)
            {
                message(BANACC_FILE_ERR);
                return NULL;
            }
            break;
        case DAY_ACC:
            fp = fopen("gfsdata\\dayacc.dat","rb");
            if(fp == NULL)
            {
                message(DAYACC_FILE_ERR);
                return NULL;
            }
            break;
        case MON_ACC:
            fp = fopen("gfsdata\\monacc.dat","rb");
            if(fp == NULL)
            {
                message(MONACC_FILE_ERR);
                return NULL;
            }
            break;
        case YEAR_ACC:
            fp = fopen("gfsdata\\yearacc.dat","rb");
            if(fp == NULL)
            {
                message(YEARACC_FILE_ERR);
                return NULL;
            }
            break;
        case HFEE_ACC:
            fp = fopen("gfsdata\\feeacc.hst","rb");
            if(fp == NULL)
            {
                message(HACCFEE_FILE_ERR);
                return NULL;
            }
            break;
        case HBAN_ACC:
            fp = fopen("gfsdata\\banacc.hst","rb");
            if(fp == NULL)
            {
                message(HBANACC_FILE_ERR);
                return NULL;
            }
            break;
        case HDAY_ACC:
            fp = fopen("gfsdata\\dayacc.hst","rb");
            if(fp == NULL)
            {
                message(HDAYACC_FILE_ERR);
                return NULL;
            }
            break;
        case HMON_ACC:
            fp = fopen("gfsdata\\monacc.hst","rb");
            if(fp == NULL)
            {
                message(HMONACC_FILE_ERR);
                return NULL;
            }
            break;
        case HYEAR_ACC:
            fp = fopen("gfsdata\\yearacc.hst","rb");
            if(fp == NULL)
            {
                message(HYEARACC_FILE_ERR);
                return NULL;
            }
            break;
        default:
            break;
    }
    return fp;
}

void acc_proc(UC isacc)
{
    UC *title1 = "注意:";
    UC *warn1  = "因打印机出错导致扎帐不成功, 请检查打印机后重新扎帐!";

    if(cal_acc_fee(isacc) == TRUE)
    {
        if(inq_acc_fee(isacc-1) == TRUE)
        {
            if(prn_acc_fee(isacc-1) == TRUE)
            {
                clr_acc_rcd(isacc);
            }
            else
            {
                sound_alarm();
                set_msg_color(7, 0, 0);
                msg_win(120, 160, title1, warn1, 200);
            }
        }
    }
    else
    {
        if(isacc == BAN_ACC)
            message(ACCFEE_FILE_ERR);
        else if(isacc == DAY_ACC)
            message(BANACC_FILE_ERR);
        else if(isacc == MON_ACC)
            message(DAYACC_FILE_ERR);
        else if(isacc == YEAR_ACC)
            message(MONACC_FILE_ERR);
        else
            ;
    }
    return;
}



/*                             已  结  算  话  费  清  单
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                市内电话       国内长话       国际长话
--------------------------------------------------------------------------------
 流  水  号   次数    话费   次数    话费   次数    话费    附加费    费用合计
012345678901  0123 01234567  0123 01234567  0123 01234567  01234567   0123456789
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
01234567890123456789012345678901234567890123456789012345678901234567890123456789
1    5    10   5    20   5    30   5    40   5    50   5    60   5    70   5
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
                       市内电话      国内长话      国际长话
--------------------------------------------------------------------------------
 流  水  号  分   机 次数     话费 次数     话费 次数     话费  附加费  费用合计
012345678901 0123456 0123 01234567 0123 01234567 0123 01234567 01234567 01234567
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
01234567890123456789012345678901234567890123456789012345678901234567890123456789
1    5    10   5    20   5    30   5    40   5    50   5    60   5    70   5
*/
