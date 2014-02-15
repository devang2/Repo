#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */

void inquire_one_phone(void)
{
    UL   phone_no;
    UC   flag;
    USERS_STRUCT nil = {0, "", 0};

    flag = input_phone_no(&phone_no, INQUIRE);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the phone no does not exist */
    {
        message(NO_NOT_EXIST);
        return;
    }

    inquire_one_record(phone_no, nil, NOACCOUNT, PHONE_NO);

    return;
}

/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */

void inquire_one_user(void)
{
    UL   input_no=0;
    UC   flag;
    UI   pnum=0;
    USERS_STRUCT user;

    flag = input_user(&input_no, INQUIRE);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the user no does not exist */
    {
        message(UNO_NOT_EXIST);
        return;
    }

    search_user((UI)input_no,&pnum,&user);

    load_phone_no(pnum,&user);

    inquire_one_record(99, user, NOACCOUNT, DEPT_NO);

    unload_phone_no();

    return;
}


/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */
void inquire_one_authcd(void)
{
    UL   input_cd;
    UC   flag;
    USERS_STRUCT nil = {0, "", 0};

    flag = input_auth_cd(&input_cd, INQUIRE);
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
    else
        Authflg = ON;

    inquire_one_record(input_cd, nil, NOACCOUNT, AUTH_CD);

    if(Authflg == ON)
        unload_authcd_tbl();
    return;
}

/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */
void print_one_phone(void)
{
    UL   phone_no;
    UC   flag;
    USERS_STRUCT nil = {0, "", 0};

    flag = check_prn();
    if(flag == FALSE)
    {
        return;
    }

    flag = input_phone_no(&phone_no, PRINT);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the phone no does not exist */
    {
        message(NO_NOT_EXIST);
        return;
    }

    print_one_record(phone_no, nil, NOACCOUNT, PHONE_NO);

    return;
}

/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */

void print_one_user(void)
{
    UL   input_no=0;
    UC   flag;
    UI   pnum=0;
    USERS_STRUCT user;

    flag = check_prn();
    if(flag == FALSE)
    {
        return;
    }

    flag = input_user(&input_no, INQUIRE);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the user no does not exist */
    {
        message(UNO_NOT_EXIST);
        unload_rate_tab();
        return;
    }

    search_user((UI)input_no,&pnum,&user);

    load_phone_no(pnum,&user);

    print_one_record(99, user, NOACCOUNT, DEPT_NO);

    unload_phone_no();

    return;
}


/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */
void print_one_authcd(void)
{
    UL   input_cd;
    UC   flag;
    USERS_STRUCT nil = {0, "", 0};

    flag = check_prn();
    if(flag == FALSE)
    {
        return;
    }

    flag = input_auth_cd(&input_cd, INQUIRE);
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
    else
        Authflg = ON;

    print_one_record(input_cd, nil, NOACCOUNT, AUTH_CD);

    if(Authflg == ON)
        unload_authcd_tbl();
    return;
}


/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */
void inquire_one_record(UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd)
{
    UC   flag, position=0;
    UI   num=0;

    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
                               {44, 96, 70, 175, 44, 44, 44, 52, 60},\
                               GREEN};

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    hz16_disp(240, 80, "电 话 话 单 清 单",BLACK);  /* International long call */

    if(disp_a_rcd(IDD, phone_no, user, acflg, isauthcd, &position, &num)==FALSE)
    {
        recover_screen(2);
        message_end();
        unload_rate_tab();
        return;
    }

    if(disp_a_rcd(DDD, phone_no, user, acflg, isauthcd, &position, &num)==FALSE)
    {
        recover_screen(2);
        message_end();
        unload_rate_tab();
        return;
    }

    if(Usr_typ == NORMAL_USR || acflg == NOACCOUNT || Ldd_fflg == YES)
        disp_a_rcd(LDD, phone_no, user, acflg, isauthcd, &position, &num);

    recover_screen(2);
    message_end();
    unload_rate_tab();
    return;

}

/* function  : inquire the telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */

void print_one_record(UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd)
{
    UC    flag;
    UI    num;

    flag = check_prn();
    if(flag == FALSE)
    {
        return;
    }

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    message_disp(8,"正在打印，请稍候...");   /* printing */

    num=0;
    if(print_rcdrpt_head(5) == FALSE)
    {
        message_end();
        unload_rate_tab();
        return;
    }

    if(prn_a_rcd(IDD, phone_no, user, acflg, isauthcd, &num) == FALSE)
    {
        message_end();
        unload_rate_tab();
        return;
    }

    if(Usr_typ == HOTEL_USR && acflg == ACCOUNT && Ldd_fflg == NO)
    {
        if(prn_a_rcd(DDD, phone_no, user, acflg, isauthcd, &num) == TRUE)
        {
            if(print_rcdrpt_tail() == FALSE)
            {
                 message_end();
                 unload_rate_tab();
                 return;
            }
        }
    }
    else
    {
        if(prn_a_rcd(DDD, phone_no, user, acflg, isauthcd, &num) == FALSE)
        {
            message_end();
            unload_rate_tab();
            return;
        }
    }

    if(Usr_typ == NORMAL_USR || acflg == NOACCOUNT || Ldd_fflg == YES)
        if(prn_a_rcd(LDD, phone_no, user, acflg, isauthcd, &num) == TRUE)
        {
            if(print_rcdrpt_tail() == FALSE)
            {
                 message_end();
                 unload_rate_tab();
                 return;
            }
        }

    message_end();
    unload_rate_tab();
    return;
}


UC  disp_a_rcd(UC mode, UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd, UC *position, UI *num)
{
    FILE *fp;
    UC   auth_name[9];
    UI   key, i;
    char *a_name="    ";
    RATE_STRUCT huge *rate_tmp;

    for(i=0; i<2; i++)
    {
        if(i == 0)
        {
            if(mode == IDD)
                fp = fopen("idd.dbf","rb");
            else if(mode == DDD)
                fp = fopen("ddd.dbf","rb");
            else
                fp = fopen("ldd.dbf","rb");
        }
        else
        {
            if(mode == IDD)
                fp = fopen("idd.hst","rb");
            else if(mode == DDD)
                fp = fopen("ddd.hst","rb");
            else
                fp = fopen("ldd.hst","rb");
        }

        if(fp != NULL)
        {
            while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
            {
                if(temp_list.auth_code != 0xaaaaaa && isauthcd == PHONE_NO)
                    continue;

                if(temp_list.caller2 != phone_no && isauthcd == PHONE_NO)
                    continue;

                if( ( !binary_phone(&user) ) && (isauthcd == DEPT_NO) )
                    continue;

                if(temp_list.auth_code != 0xaaaaaa && isauthcd == DEPT_NO)
                    continue;

                if(temp_list.auth_code != phone_no && isauthcd == AUTH_CD)
                    continue;

                if(acflg != ACCOUNT && is_in_range() == FALSE )
                    continue;

                if(acflg == ACCOUNT && temp_list.flag == FALSE)
                    continue;

                if(mode == IDD)
                {
                    rate_tmp = get_rate(&temp_list.callee[2],IDD);       /* get charge rate */
                }
                else if(mode == DDD)
                {
                    rate_tmp = get_rate(&temp_list.callee[1],DDD);       /* get charge rate */
                }
                else
                {
                    rate_tmp = get_rate(temp_list.callee,LDD);   /* get charge rate */
                }

                if(rate_tmp == NULL)              /* the area No. does not exist    */
                    strcpy(rate1.area_name, a_name);
                else
                    rate1 = *rate_tmp;

                (*num)++;

                auth_usr_proc(auth_name);
                disp_a_record(auth_name, *position, *num);

                (*position)++;

                if( (*position>=16) && (!EndofRecFile(fp)) )
                {
                    while(1)
                    {
                        key = get_key1();
                        if(key == ESC)
                        {
                            fclose(fp);
                            return FALSE;
                        }
                        else if(key == ENTER)
                        {
                            brush_tbl(9, 16, 7);
                            *position = 0;
                            break;
                        }
                    }
                }
            }          /* end of "while(fread(...fp)==1)" */
            fclose(fp);
        }
    }         /* end of "if(fp != NULL)"        */

    if( (mode == LDD) || (mode == DDD && Usr_typ == HOTEL_USR && \
         acflg== ACCOUNT && Ldd_fflg == NO) )
    {
        while(1)
        {
            key = get_key1();
            if(key == ESC)
                return FALSE;
            else if(key == ENTER)
                break;
        }
    }
    return TRUE;
}


UC  prn_a_rcd(UC mode, UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd, UI *num)
{
    FILE *fp;
    UI    i;
    UC    auth_name[9];
    char *a_name="    ";
    RATE_STRUCT huge *rate_tmp;

    for(i=0; i<2; i++)
    {
        if(i == 0)
        {
            if(mode == IDD)
                fp = fopen("idd.dbf","rb");
            else if(mode == DDD)
                fp = fopen("ddd.dbf","rb");
            else
                fp = fopen("ldd.dbf","rb");
        }
        else
        {
            if(mode == IDD)
                fp = fopen("idd.hst","rb");
            else if(mode == DDD)
                fp = fopen("ddd.hst","rb");
            else
                fp = fopen("ldd.hst","rb");
        }

        if(fp != NULL)
        {
            while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
            {
                if(temp_list.auth_code != 0xaaaaaa && isauthcd == PHONE_NO)
                    continue;

                if(temp_list.caller2 != phone_no && isauthcd == PHONE_NO)
                    continue;

                if( ( !binary_phone(&user) ) && (isauthcd == DEPT_NO) )
                    continue;

                if(temp_list.auth_code != 0xaaaaaa && isauthcd == DEPT_NO)
                    continue;

                if(temp_list.auth_code != phone_no && isauthcd == AUTH_CD)
                    continue;

                if(acflg != ACCOUNT && is_in_range() == FALSE )
                    continue;

                if(acflg == ACCOUNT && temp_list.flag == FALSE)
                    continue;

                if(mode == IDD)
                {
                    rate_tmp = get_rate(&temp_list.callee[2],IDD);       /* get charge rate */
                }
                else if(mode == DDD)
                {
                    rate_tmp = get_rate(&temp_list.callee[1],DDD);       /* get charge rate */
                }
                else
                {
                    rate_tmp = get_rate(temp_list.callee,LDD);   /* get charge rate */
                }

                if(rate_tmp == NULL)              /* the area No. does not exist    */
                    strcpy(rate1.area_name, a_name);
                else
                    rate1 = *rate_tmp;

                (*num)++;

                auth_usr_proc(auth_name);
                if(print_rcdrpt_detail(auth_name, *num, 5)==FALSE)
//              if(print_rcdrpt_detail("", *num, 5)==FALSE)
                {
                    fclose(fp);
                    return FALSE;
                }
            }          /* end of "while(fread(...fp)==1)" */
            fclose(fp);
        }
    }         /* end of "if(fp != NULL)"        */

    return TRUE;
}


