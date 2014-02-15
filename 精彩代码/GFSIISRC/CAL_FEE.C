#include <string.h>
#include <stdlib.h>
#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

ACC_STRUCT   Account = { 0,0,0, 0,0,0, 0,0,0, 0,
                         { 0, 0, 0, 0, 0 },
                         { 0, 0, 0, 0, 0, 0 },
                           0, 0
                       };

/* function  : calculate the total dialing fee for all the phone
 * called by : statistics()
 * date      : 1993.10.9
 */
void cal_total_fee(void)
{
    UI    i, j=0;
    UC    flag, user[10];
    FILE  *fp;
    PHONE_STRUCT  *phone;
    AUTHCD_STRUCT *authcd;
    UI    SpecLen[2];

    load_phone_tbl();

    flag = load_authcd_tbl(0);
    if(flag == TRUE)
    {
        Authflg=ON;
        ANewlen = authcd_tbl_len();
    }
    else
    {
        Authflg=OFF;
        message(AUTHCD_FILE_ERR);
    }

    load_special_tbl(0);  /* local call definition table */
    load_special_tbl(1);  /* information call definition table */
    SpecLen[0] = special_tbl_len(0);  /* local */
    SpecLen[1] = special_tbl_len(1);  /* information */

    message_disp(8,"正在计算, 请稍候...");  /* calculating, please wait... */

    Newlen = phone_tbl_len();
    for(i=0; i<Newlen; i++)
    {
        (Phone_top+i)->intern_time   = 0;
        (Phone_top+i)->intern_charge = 0;
        (Phone_top+i)->intern_count  = 0;

        (Phone_top+i)->nation_time   = 0;
        (Phone_top+i)->nation_charge = 0;
        (Phone_top+i)->nation_count  = 0;

        (Phone_top+i)->local_time    = 0;
        (Phone_top+i)->local_charge  = 0;
        (Phone_top+i)->local_count   = 0;

        (Phone_top+i)->addfee[4] = 0;
    }

    if(Authflg == ON)
    {
        for(i=0; i<ANewlen; i++)
        {
            (Auth_cd_top+i)->intern_time   = 0;
            (Auth_cd_top+i)->intern_charge = 0;
            (Auth_cd_top+i)->intern_count  = 0;

            (Auth_cd_top+i)->nation_time   = 0;
            (Auth_cd_top+i)->nation_charge = 0;
            (Auth_cd_top+i)->nation_count  = 0;

            (Auth_cd_top+i)->local_time    = 0;
            (Auth_cd_top+i)->local_charge  = 0;
            (Auth_cd_top+i)->local_count   = 0;

            (Auth_cd_top+i)->addfee        = 0;
        }
    }

    for(i=0; i<2; i++)
    {
        if(i == 0)
            fp = fopen("idd.dbf","rb");
        else
            fp = fopen("idd.hst","rb");

        if(fp != NULL)
        {
            while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
            {
                if(!is_in_range())             /* not in the sorting time range */
                    continue;

                if(temp_list.auth_code == 0xaaaaaa)
                {
                    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
                    phone = binary_search(user);         /* locate the phone No */
                    if(phone == NULL)                    /* can't find the No   */
                        continue;

                    if(temp_list.time%60 != 0 )
                        temp_list.time    = (temp_list.time/60 + 1)*60;
                    phone->intern_time   += temp_list.time;
                    phone->intern_count  += 1;
                    if(phone->feeflag[4])  {		/* IDD fee */
                        phone->intern_charge += temp_list.charge-temp_list.add;
                        phone->addfee[4]     += (UL)temp_list.add;
                    }
                }
                else if(Authflg == ON)
                {
                    authcd = binary_authcd(temp_list.auth_code);          /* locate the phone No */
                    if(authcd == NULL)                     /* can't find the No   */
                        continue;

                    if(temp_list.time%60 != 0 )
                        temp_list.time = (temp_list.time/60 + 1)*60;
                    authcd->intern_time   += temp_list.time;
                    authcd->intern_charge += temp_list.charge-temp_list.add;
                    authcd->intern_count  += 1;
                    authcd->addfee        += (UL)temp_list.add;
                }
            }
            fclose(fp);
        }
    }

    for(i=0; i<2; i++)
    {
        if(i == 0)
            fp = fopen("ddd.dbf","rb");
        else
            fp = fopen("ddd.hst","rb");

        if(fp != NULL)
        {
            while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
            {
                if(!is_in_range())             /* not in the sorting time range */
                    continue;

                if(temp_list.auth_code == 0xaaaaaa)
                {
                    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
                    phone = binary_search(user);         /* locate the phone No */
                    if(phone == NULL)                    /* can't find the No   */
                        continue;

                    if(temp_list.time%60 != 0 )
                        temp_list.time = (temp_list.time/60 + 1)*60;
                    phone->nation_time   += temp_list.time;
                    phone->nation_count  += 1;
                    if(phone->feeflag[3])  {		/* DDD fee */
                        phone->nation_charge += temp_list.charge-temp_list.add;
                        phone->addfee[4]     += (UL)temp_list.add;
                    }
                }
                else if(Authflg == ON)
                {
                    authcd = binary_authcd(temp_list.auth_code);          /* locate the phone No */
                    if(authcd == NULL)                     /* can't find the No   */
                        continue;

                    if(temp_list.time%60 != 0 )
                        temp_list.time = (temp_list.time/60 + 1)*60;
                    authcd->nation_time   += temp_list.time;
                    authcd->nation_count  += 1;
                    authcd->nation_charge += temp_list.charge-temp_list.add;
                    authcd->addfee        += (UL)temp_list.add;
                }
            }
            fclose(fp);
        }
    }

    for(i=0; i<2; i++)
    {
        if(i == 0)
            fp = fopen("ldd.dbf","rb");
        else
            fp = fopen("ldd.hst","rb");

        if(fp != NULL)
        {
            while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
            {
                if(!is_in_range())         /* not in the sorting time range */
                    continue;
                j++;

                if(temp_list.auth_code == 0xaaaaaa)
                {
                    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
                    phone = binary_search(user);         /* locate the phone No */
                    if(phone == NULL)                    /* can't find the No   */
                        continue;

                    if(temp_list.time%60 != 0 )
                        temp_list.time   = (temp_list.time/60 + 1)*60;
                    phone->local_time   += temp_list.time;
                    phone->local_count  += 1;

                    if( phone->feeflag[0] && phone->feeflag[1] && phone->feeflag[2])
		    {
                        phone->local_charge += temp_list.charge-temp_list.add;
                        phone->addfee[4]    += (UL)temp_list.add;
                    }
		    else {
                        UC f1, f2;
                        f1 = IsSpecialCall(temp_list.callee,SpecLen[0],0);
                        f2 = IsSpecialCall(temp_list.callee,SpecLen[1],1);

                        if(   (phone->feeflag[1] && f1)
                            ||(phone->feeflag[2] && f2)
                            || (phone->feeflag[0] && !(f1 && f2) ))
		        {
                            phone->local_charge += temp_list.charge-temp_list.add;
                            phone->addfee[4]    += (UL)temp_list.add;
                        }
                    }
                }
                else if(Authflg == ON)
                {
                    authcd = binary_authcd(temp_list.auth_code);          /* locate the phone No */
                    if(authcd == NULL)                     /* can't find the No   */
                        continue;

                    if(temp_list.time%60 != 0 )
                        temp_list.time = (temp_list.time/60 + 1)*60;
                    authcd->local_time   += temp_list.time;
                    authcd->local_charge += temp_list.charge-temp_list.add;
                    authcd->local_count  += 1;
                    authcd->addfee       += (UL)temp_list.add;
                }
            }
            fclose(fp);
        }
    }

    unload_special_tbl(0);
    unload_special_tbl(1);

    save_phone_tbl(0, 0);
    unload_phone_tbl();

    if(Authflg == ON)
    {
        save_authcd_tbl(NOFRESH, 0);
        unload_authcd_tbl();
    }

    fp = fopen(TotTimeFileName,"wb");
    fwrite(&s_time, sizeof(TIME_STRUCT), 1, fp);
    fwrite(&e_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    message_end();

    return;
}


/* function  : calculate the total dialing fee for all the phone
 * called by : statistics()
 * date      : 1993.10.9
 */
UC cal_acc_fee(UC isacc)
{
    UI    i;
    UC    sflag=0;
    TIME_STRUCT stime, etime;
    ACC_STRUCT acc;
    FILE *fp;

    if(isacc == BAN_ACC)
        fp = fopen("gfsdata\\feeacc.dat","rb");
    else if(isacc == DAY_ACC)
        fp = fopen("gfsdata\\banacc.dat","rb");
    else if(isacc == MON_ACC)
        fp = fopen("gfsdata\\dayacc.dat","rb");
    else if(isacc == YEAR_ACC)
        fp = fopen("gfsdata\\monacc.dat","rb");
    else
        return(FALSE);

    if(fp != NULL)
    {
        message_disp(8,"正在计算, 请稍候...");  /* calculating, please wait... */

        Account.intern_time     = 0;
        Account.intern_charge   = 0;
        Account.intern_count    = 0;

        Account.nation_time     = 0;
        Account.nation_charge   = 0;
        Account.nation_count    = 0;

        Account.local_time      = 0;
        Account.local_charge    = 0;
        Account.local_count     = 0;

        Account.month_lease     = 0;

        for(i=0; i<5; i++)
            Account.addfee[i] = 0;

        while(fread(&acc, sizeof(ACC_STRUCT), 1, fp) == 1)
        {
            Account.intern_time  += acc.intern_time;
            Account.intern_count += acc.intern_count;
            Account.intern_charge+= acc.intern_charge;

            Account.nation_time  += acc.nation_time;
            Account.nation_count += acc.nation_count;
            Account.nation_charge+= acc.nation_charge;

            Account.local_time   += acc.local_time;
            Account.local_count  += acc.local_count;
            Account.local_charge += acc.local_charge;

            Account.month_lease  += acc.month_lease;

            for(i=0; i<5; i++)
                Account.addfee[i]+= acc.addfee[i];

            if(sflag == 0)
            {
                sflag = 1;
                stime.year      = acc.seral_no.year+1900;
                stime.month     = acc.seral_no.month;
                stime.day       = acc.seral_no.day;
                stime.hour      = acc.seral_no.hour;
            }
            etime.year  = acc.seral_no.year+1900;
            etime.month = acc.seral_no.month;
            etime.day   = acc.seral_no.day;
            etime.hour  = acc.seral_no.hour;
            etime.hour++;
        }
        fclose(fp);

        if(isacc == BAN_ACC)
            fp = fopen("gfsdata\\bantime.dat","wb");
        else if(isacc == DAY_ACC)
            fp = fopen("gfsdata\\daytime.dat","wb");
        else if(isacc == MON_ACC)
            fp = fopen("gfsdata\\montime.dat","wb");
        else if(isacc == YEAR_ACC)
            fp = fopen("gfsdata\\yeartime.dat","wb");
        else
        {
            message_end();
            return(FALSE);
        }

        fwrite(&stime, sizeof(TIME_STRUCT), 1, fp);
        fwrite(&etime, sizeof(TIME_STRUCT), 1, fp);
        fclose(fp);

        message_end();
    }
    else
        return(FALSE);

    return(TRUE);
}

/* function  : calculate the total dialing fee for all the phone
 * called by : statistics()
 * date      : 1993.10.9
 */
void clr_acc_rcd(UC isacc)
{
    UI     i;
    FILE  *fp, *acc_fp;
    struct time tt;
    struct date dd;
    ACC_STRUCT  acc;

    getdate(&dd);
    gettime(&tt);

    acc.seral_no.year  = (UC) (dd.da_year-1900);
    acc.seral_no.month = (UC) dd.da_mon;
    acc.seral_no.day   = (UC) dd.da_day;
    acc.seral_no.hour  = tt.ti_hour;
    acc.seral_no.min   = tt.ti_min;
    acc.seral_no.sec   = tt.ti_sec;

    acc.flag = isacc;
    acc.phone_no = 0;

    acc.intern_time   = Account.intern_time;
    acc.intern_charge = Account.intern_charge;
    acc.intern_count  = Account.intern_count;

    acc.nation_time   = Account.nation_time ;
    acc.nation_charge = Account.nation_charge;
    acc.nation_count  = Account.nation_count;

    acc.local_time   = Account.local_time;
    acc.local_charge = Account.local_charge;
    acc.local_count  = Account.local_count;

    acc.month_lease  = Account.month_lease;

    for(i=0; i<5; i++)
        acc.addfee[i] = Account.addfee[i];

    message_disp(8,"正在进行扎帐处理, 请稍候...");  /* calculating, please wait... */

    if(isacc == BAN_ACC)
        fp = fopen("gfsdata\\banacc.dat","ab");
    else if(isacc == DAY_ACC)
        fp = fopen("gfsdata\\dayacc.dat","ab");
    else if(isacc == MON_ACC)
        fp = fopen("gfsdata\\monacc.dat","ab");
    else
        fp = fopen("gfsdata\\yearacc.dat","ab");

    fwrite(&acc, sizeof(ACC_STRUCT), 1, fp);
    fclose(fp);

    if(isacc == BAN_ACC)
        acc_fp = fopen("gfsdata\\feeacc.hst","ab");
    else if(isacc == DAY_ACC)
        acc_fp = fopen("gfsdata\\banacc.hst","ab");
    else if(isacc == MON_ACC)
        acc_fp = fopen("gfsdata\\dayacc.hst","ab");
    else
        acc_fp = fopen("gfsdata\\monacc.hst","ab");

    if(isacc == BAN_ACC)
        fp = fopen("gfsdata\\feeacc.dat","rb");
    else if(isacc == DAY_ACC)
        fp = fopen("gfsdata\\banacc.dat","rb");
    else if(isacc == MON_ACC)
        fp = fopen("gfsdata\\dayacc.dat","rb");
    else
        fp = fopen("gfsdata\\monacc.dat","rb");

    if(fp != NULL)
    {
        while(fread(&acc, sizeof(ACC_STRUCT), 1, fp) == 1)
            fwrite(&acc, sizeof(ACC_STRUCT), 1, acc_fp);
        fclose(fp);
    }
    fclose(acc_fp);

    if(isacc == BAN_ACC)
        remove("gfsdata\\feeacc.dat");
    else if(isacc == DAY_ACC)
        remove("gfsdata\\banacc.dat");
    else if(isacc == MON_ACC)
        remove("gfsdata\\dayacc.dat");
    else
        remove("gfsdata\\monacc.dat");

    message_end();
    return;
}

/* function  : calculate the total dialing fee for all the phone
 * called by : statistics()
 * date      : 1993.10.9
 */
void cal_one_fee(UL phone_no, UC isauthcd)
{
    UI    i;
    UC    user[10], sflg1, sflg2, sflg3;
    TIME_STRUCT    stime1, etime1, stime2, etime2, stime3, etime3;
    PHONE_STRUCT  *phone;
    AUTHCD_STRUCT *authcd;
    FILE *fp;

    sflg1 = 0;
    sflg2 = 0;
    sflg3 = 0;

    if(isauthcd == PHONE_FEE)
    {
        ltoa(phone_no, user, 10);            /* int to ASCII string */
        phone = binary_search(user);         /* locate the phone No */
        if(phone == NULL)                    /* can't find the No   */
            return;
    }
    else if(isauthcd == AUTH_CD)
    {
        authcd = binary_authcd(phone_no);          /* locate the phone No */
        if(authcd == NULL)                     /* can't find the No   */
            return;
    }

    message_disp(8,"正在计算, 请稍候...");  /* calculating, please wait... */

    Account.intern_time   = 0;
    Account.intern_charge = 0;
    Account.intern_count  = 0;

    Account.nation_time   = 0;
    Account.nation_charge = 0;
    Account.nation_count  = 0;

    Account.local_time   = 0;
    Account.local_charge = 0;
    Account.local_count  = 0;

    if(isauthcd == PHONE_FEE)
    {
        Account.month_lease = phone->month_lease;
        Account.addfee[4] = 0;
        for(i=0; i<4; i++)
            Account.addfee[i] = phone->addfee[i];
    }
    else if(isauthcd == AUTH_CD)
    {
        Account.month_lease = authcd->month_lease;
        Account.addfee[0] = authcd->addfee;
        for(i=1; i<5; i++)
            Account.addfee[i] = 0;
    }

    stime1.year = 9999;
    etime1.year = 0000;
    stime1.month = 99;
    etime1.month = 00;
    stime1.day = 99;
    etime1.day = 00;
    stime1.hour = 99;
    etime1.hour = 00;

    stime2.year = 8888;
    etime2.year = 1111;
    stime2.month = 88;
    etime2.month = 11;
    stime2.day = 88;
    etime2.day = 11;
    stime2.hour = 88;
    etime2.hour = 11;

    stime3.year = 7777;
    etime3.year = 1111;
    stime3.month = 77;
    etime3.month = 11;
    stime3.day = 77;
    etime3.day = 11;
    stime3.hour = 77;
    etime3.hour = 11;

    fp = fopen("idd.dbf","rb");
    if(fp != NULL)
    {
        while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
        {
            if( ( (isauthcd == PHONE_NO) && (temp_list.auth_code != 0xaaaaaa) ) ||\
                ( (isauthcd == AUTH_CD)  && (temp_list.auth_code == 0xaaaaaa) ) )
                continue;

            if( (isauthcd == PHONE_NO) && (temp_list.caller2 != phone_no) )
                continue;

            if( (isauthcd == AUTH_CD) && (temp_list.auth_code != phone_no) )
                continue;

            if(temp_list.flag == FALSE)
                continue;

            if(temp_list.time%60 != 0 )
                temp_list.time   = (temp_list.time/60 + 1)*60;

            Account.intern_time   += temp_list.time;

//            Account.intern_charge += temp_list.charge-temp_list.add;
            Account.intern_charge += temp_list.charge;
            Account.intern_count  += 1;

//            Account.addfee[4]     += (UL)temp_list.add;

            if(sflg1 == 0)
            {
                sflg1 = 1;
                stime1.year = temp_list.year1;
                stime1.month = temp_list.mon1;
                stime1.day = temp_list.day1;
                stime1.hour = temp_list.hour1;
            }
            etime1.year = temp_list.year2;
            etime1.month = temp_list.mon2;
            etime1.day = temp_list.day2;
            etime1.hour = temp_list.hour2;
        }
        fclose(fp);
    }

    fp = fopen("ddd.dbf","rb");
    if(fp != NULL)
    {
        while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
        {
            if( ( (isauthcd == PHONE_NO) && (temp_list.auth_code != 0xaaaaaa) ) ||\
                ( (isauthcd == AUTH_CD)  && (temp_list.auth_code == 0xaaaaaa) ) )
                continue;

            if( (isauthcd == PHONE_NO) && (temp_list.caller2 != phone_no) )
                continue;

            if( (isauthcd == AUTH_CD) && (temp_list.auth_code != phone_no) )
                continue;

            if(temp_list.flag == FALSE)
                continue;

            if(temp_list.time%60 != 0 )
                temp_list.time = (temp_list.time/60 + 1)*60;

            Account.nation_time += temp_list.time;
//            Account.nation_charge += temp_list.charge-temp_list.add;
            Account.nation_charge += temp_list.charge;
            Account.nation_count  += 1;
//            Account.addfee[4]   += (UL)temp_list.add;

            if(sflg2 == 0)
            {
                sflg2 = 1;
                stime2.year = temp_list.year1;
                stime2.month = temp_list.mon1;
                stime2.day = temp_list.day1;
                stime2.hour = temp_list.hour1;
            }
            etime2.year = temp_list.year2;
            etime2.month = temp_list.mon2;
            etime2.day = temp_list.day2;
            etime2.hour = temp_list.hour2;
        }
        fclose(fp);
    }

    if(Usr_typ == HOTEL_USR && Ldd_fflg == YES)
    {
        fp = fopen("ldd.dbf","rb");
        if(fp != NULL)
        {
            while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
            {
                if( ( (isauthcd == PHONE_NO) && (temp_list.auth_code != 0xaaaaaa) ) ||\
                    ( (isauthcd == AUTH_CD)  && (temp_list.auth_code == 0xaaaaaa) ) )
                    continue;

                if( (isauthcd == PHONE_NO) && (temp_list.caller2 != phone_no) )
                    continue;

                if( (isauthcd == AUTH_CD) && (temp_list.auth_code != phone_no) )
                    continue;

                if(temp_list.flag == FALSE)
                    continue;

                if(temp_list.time%60 != 0 )
                    temp_list.time = (temp_list.time/60 + 1)*60;

                Account.local_time   += temp_list.time;
//                Account.local_charge += temp_list.charge-temp_list.add;
                Account.local_charge += temp_list.charge;
                Account.local_count  += 1;
//                Account.addfee[4]    += (UL)temp_list.add;

                if(sflg3 == 0)
                {
                    sflg3 = 1;
                    stime3.year = temp_list.year1;
                    stime3.month = temp_list.mon1;
                    stime3.day = temp_list.day1;
                    stime3.hour = temp_list.hour1;
                }
                etime3.year = temp_list.year2;
                etime3.month = temp_list.mon2;
                etime3.day = temp_list.day2;
                etime3.hour = temp_list.hour2;
            }
            fclose(fp);
        }
    }

    fnd_stime(&stime1, &stime2);
    fnd_stime(&stime1, &stime3);
    fnd_etime(&etime1, &etime2);
    fnd_etime(&etime1, &etime3);

    etime1.hour++;

    fp = fopen("gfsdata\\acctime.dat","wb");
    fwrite(&stime1, sizeof(TIME_STRUCT), 1, fp);
    fwrite(&etime1, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    message_end();
    return;
}


void fnd_stime(TIME_STRUCT *time1, TIME_STRUCT *time2)
{
    UC flag=0;

    if(time1->year > time2->year)
        flag = 1;
    else if(time1->year == time2->year)
    {
        if(time1->month > time2->month)
            flag = 1;
        else if(time1->month == time2->month)
        {
            if(time1->day > time2->day)
                flag = 1;
            else if(time1->day == time2->day)
            {
                if(time1->hour > time2->hour)
                    flag = 1;
            }
        }
    }
    if(flag == 1)
    {
        time1->year = time2->year;
        time1->month = time2->month;
        time1->day = time2->day;
        time1->hour = time2->hour;
    }
    return;
}

void fnd_etime(TIME_STRUCT *time1, TIME_STRUCT *time2)
{
    UC flag=0;

    if(time1->year > time2->year)
        flag = 1;
    else if(time1->year == time2->year)
    {
        if(time1->month > time2->month)
            flag = 1;
        else if(time1->month == time2->month)
        {
            if(time1->day > time2->day)
                flag = 1;
            else if(time1->day == time2->day)
            {
                if(time1->hour > time2->hour)
                    flag = 1;
            }
        }
    }
    if(flag == 0)
    {
        time1->year = time2->year;
        time1->month = time2->month;
        time1->day = time2->day;
        time1->hour = time2->hour;
    }
    return;
}


/* function  : calculate the total dialing fee for all the phone
 * called by : statistics()
 * date      : 1993.10.9
 */
void clr_one_rcd(UL phone_no, UC isauthcd)
{
    UI    i;
    UC    phone[10], flag;
    FILE  *acc_fp;
    struct time tt;
    struct date dd;
    ACC_STRUCT  acc;

    getdate(&dd);
    gettime(&tt);

    acc.seral_no.year  = (UC) (dd.da_year-1900);
    acc.seral_no.month = (UC) dd.da_mon;
    acc.seral_no.day   = (UC) dd.da_day;
    acc.seral_no.hour  = tt.ti_hour;
    acc.seral_no.min   = tt.ti_min;
    acc.seral_no.sec   = tt.ti_sec;

    acc.flag = FEE_ACC;
    acc.phone_no = phone_no;

    acc.intern_time   = Account.intern_time;
    acc.intern_charge = Account.intern_charge;
    acc.intern_count  = Account.intern_count;

    acc.nation_time   = Account.nation_time ;
    acc.nation_charge = Account.nation_charge;
    acc.nation_count  = Account.nation_count;

    acc.local_time   = Account.local_time;
    acc.local_charge = Account.local_charge;
    acc.local_count  = Account.local_count;

    acc.month_lease  = Account.month_lease;

    for(i=0; i<5; i++)
        acc.addfee[i] = Account.addfee[i];

    message_disp(8,"正在对结算话单作已结标记, 请稍候...");  /* calculating, please wait... */

    stamp_rcd(IDD, phone_no, isauthcd);
    stamp_rcd(DDD, phone_no, isauthcd);
    stamp_rcd(LDD, phone_no, isauthcd);

    message_end();

    message_disp(8,"正在记流水帐, 请稍候...");  /* calculating, please wait... */
    acc_fp = fopen("gfsdata\\feeacc.dat", "ab");
    fwrite(&acc, sizeof(ACC_STRUCT), 1, acc_fp);
    fclose(acc_fp);
    message_end();

    message_disp(8,"正在清理押金记录, 请稍候...");  /* calculating, please wait... */

    flag = load_cash_tbl(PHONE_CASH);
    if(flag != 1)        /* cash.dat does not exist          */
        Cashflg[PHONE_CASH] = 0;
    else
        Cash_len[PHONE_CASH] = cash_tbl_len(PHONE_CASH);

    /* handle cash in pledge */
    if(Cashflg[PHONE_CASH])
    {
        ltoa(phone_no, phone, 10);
        temp_cash = binary_cash(phone, PHONE_CASH);
        if( (temp_cash != NULL) && (temp_cash->flag ==CASH_ON) \
        && (temp_list.auth_code == 0xaaaaaa) )
        {
            temp_cash->cash = 0;
            temp_cash->limit = 0;
            temp_cash->flag = CASH_OFF;
        }
        save_cash_tbl(PHONE_CASH);
        unload_cash_tbl(PHONE_CASH);
    }
    message_end();

    return;
}


void acount_one(UC isauthcd)
{
    UL     phone_no;
    UC     flag, ret_val;
    UI     key;
    USERS_STRUCT nil = {0, "", 0};
    PHONE_STRUCT phone;
    PHONE_STRUCT *phonep;
    AUTHCD_STRUCT *authcd;
    UC     phoneno[9];

    flag = take_all_record();
    if(flag != 1) return;

    if(isauthcd == AUTH_CD)
    {
	flag = input_auth_cd(&phone_no, 5);
	if(flag == 2)
	    return;
	else if(flag == FALSE)        /* the user no does not exist */
	{
	    message(AUTHCD_NOT_EXIST);
	    return;
	}

	flag = load_authcd_tbl(0);
	if(flag != TRUE)
	    Authflg = OFF;
	else {
	    Authflg = ON;
	    authcd = binary_authcd(phone_no);          /* locate the phone No */
	}
    }
    else
    {
	flag = input_phone_no(&phone_no, 5);
	if(flag == 2)
	    return;
	else if(flag == FALSE)        /* the phone no does not exist */
	{
	    message(NO_NOT_EXIST);
	    return;
	}
	ltoa(phone_no, phoneno, 10);

	load_phone_tbl();
	Newlen = phone_tbl_len();
	phonep = binary_search(phoneno);
    }

    cal_one_fee(phone_no, isauthcd);

    if( (Account.local_charge+  Account.nation_charge+\
	 Account.intern_charge ) == 0)
    {
	message(NULL_ACCOUNT_ERR);
	if(Authflg == ON && isauthcd == AUTH_CD)
	{
	    authcd->auth_class = UNUSED_AUTH;
	    authcd->auth_set_flag = 1;
	    save_authcd_tbl(NOFRESH, 1);
	    unload_authcd_tbl();
	}
	if(isauthcd == PHONE_FEE)
	{
	    phonep->class = 1;
	    phonep->set_flag = 1;
	    save_phone_tbl(NOFRESH, 1);
	    unload_phone_tbl();
	}
	return;
    }

    phone.intern_time   = Account.intern_time;
    phone.intern_charge = Account.intern_charge;
    phone.intern_count  = (UI)Account.intern_count;

    phone.nation_time   = Account.nation_time;
    phone.nation_charge = Account.nation_charge;
    phone.nation_count  = (UI)Account.nation_count;

    phone.local_time    = Account.local_time;
    phone.local_charge  = Account.local_charge;
    phone.local_count   = (UI)Account.local_count;

    phone.addfee[4] = Account.addfee[4];

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */
    clr_DialWin(2);

    htl_dsp_one(&phone, phone_no, isauthcd);

    while(1)
    {
	key = get_key1();
	if(key == ESC)
	{
	    recover_screen(2);
	    message_end();
	    if(Authflg == ON && isauthcd == AUTH_CD)
		unload_authcd_tbl();
	    if(isauthcd == PHONE_FEE)
		unload_phone_tbl();
	    return;
	}
        else if(key == ENTER)
            break;
    }
    recover_screen(2);
    message_end();

    inquire_one_record(phone_no, nil, ACCOUNT, isauthcd);

    ret_val = select_1in2(11);
    if( ret_val == 1 )
    {
        message_disp(8,"正在打印, 请稍候...");
	if( htl_inv_one(&phone, phone_no, isauthcd, MULTY) == FALSE )
        {
            message_end();
            if(Authflg == ON && isauthcd == AUTH_CD)
                unload_authcd_tbl();
            if(isauthcd == PHONE_FEE)
		unload_phone_tbl();
	    return;
	}
//        print_one_record(phone_no, nil, ACCOUNT, isauthcd);
	message_end();
    }

    message_disp(8,"正在打印, 请稍候...");
    ret_val = select_1in2(4);
    if( ret_val == 2 )
	print_one_record(phone_no, nil, ACCOUNT, isauthcd);
    message_end();

    ret_val = select_1in2(5);
    if( ret_val == 1 )
	clr_one_rcd(phone_no, isauthcd);

    if(Authflg == ON && isauthcd == AUTH_CD)
    {
       if( ret_val == 1)
       {
	   authcd->auth_class = UNUSED_AUTH;
	   authcd->auth_set_flag = 1;
	   save_authcd_tbl(NOFRESH, 1);
       }
       unload_authcd_tbl();
    }

    if(isauthcd == PHONE_FEE)
    {
	if(ret_val == 1) /* success */
	{
	    phonep->class = 1;
	    phonep->set_flag = 1;
	    save_phone_tbl(NOFRESH, 1);
	}
	unload_phone_tbl();
    }
    return;
}


void stamp_rcd(UC mode, UL phone_no, UC isauthcd)
{
    FILE *fps, *fpd1, *fpd2;

    if(mode == IDD)
        fps = fopen("idd.dbf","rb");
    else if(mode == DDD)
        fps = fopen("ddd.dbf","rb");
    else
        fps = fopen("ldd.dbf","rb");

    if(fps != NULL)
    {
        if(mode == IDD)
        {
            fpd1 = fopen("idd.del","wb");
            fpd2 = fopen("idd.hst","ab");
        }
        else if(mode == DDD)
        {
            fpd1 = fopen("ddd.del","wb");
            fpd2 = fopen("ddd.hst","ab");
        }
        else
        {
            fpd1 = fopen("ldd.del","wb");
            fpd2 = fopen("ldd.hst","ab");
        }
        while(fread(&temp_list, sizeof(RECORD_STRUCT), 1, fps) == 1)
        {
            if( ( (isauthcd == PHONE_NO) && (temp_list.auth_code != 0xaaaaaa) ) ||\
                ( (isauthcd == AUTH_CD)  && (temp_list.auth_code == 0xaaaaaa) ) )
            {
                fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd1);
                continue;
            }

            if( (isauthcd == PHONE_NO) && (temp_list.caller2 != phone_no) )
            {
                fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd1);
                continue;
            }

            if( (isauthcd == AUTH_CD) && (temp_list.auth_code != phone_no) )
            {
                fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd1);
                continue;
            }


            if(temp_list.flag == FALSE)
            {
                fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd2);
                continue;
            }

            temp_list.flag = FALSE;
            fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd2);
        }
        fclose(fps);
        fclose(fpd1);
        fclose(fpd2);

        if(mode == IDD)
        {
            fcopy("idd.del","idd.dbf");        /* .del to .dat */
            fcopy("idd.del","idd.tmp");        /* .del to .tmp */
            remove("idd.del");
        }
        else if(mode == DDD)
        {
            fcopy("ddd.del","ddd.dbf");        /* .del to .dat */
            fcopy("ddd.del","ddd.tmp");        /* .del to .tmp */
            remove("ddd.del");
        }
        else
        {
            fcopy("ldd.del","ldd.dbf");        /* .del to .dat */
            fcopy("ldd.del","ldd.tmp");        /* .del to .tmp */
            remove("ldd.del");
        }
    }
}

/* function  : to Check if the call is local one or information one
 * called by : cal_total_fee()
 * date      : 1996.03.9
 */
UC IsSpecialCall(UC *callee, UI len, UC mode)
{
    SPECIAL_CALL *tbl;
    UI		 i;
    UC 		 flag;
    UC           CalleeAsc[11];

    flag = 0;
    tbl = Call_top[mode];

    for(i=0; i<10; i++)
        CalleeAsc[i] = callee[i]+'0';

    for(i=0; i<len; i++)
    {
        if(strncmp(CalleeAsc, tbl->phone_no, strlen(tbl->phone_no))==0)
        {
            flag = 1;
            break;
        }
        tbl++;
    }

    return flag;
}