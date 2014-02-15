#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : inquire the long telephone records for specified phone number
 * called by : phone_record_proc()
 * date      : 1993.9.5
 */
void inquire_one_long(void)
{
    UL   phone_no;
    UC   flag;
    USERS_STRUCT nil= {0, "", 0};
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
                               {44, 96, 70, 175, 44, 44, 44, 52, 60},\
                               GREEN};

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    flag = input_phone_no(&phone_no, INQUIRE);
    if(flag == 2)
    {
        unload_rate_tab();
        return;
    }
    else if(flag == FALSE)        /* the phone no does not exist */
    {
        message(NO_NOT_EXIST);
        unload_rate_tab();
        return;
    }

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"国 际 长 途 话 单",BLACK);  /* International long call */

    if(disp_long_rcd(IDD, PHONE_NO, 99, phone_no, 99, nil) == FALSE)
    {
        recover_screen(2);
        message_end();
        unload_rate_tab();
        return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"国 内 长 途 话 单",BLACK);  /* national long call */

    disp_long_rcd(DDD, PHONE_NO, 99, phone_no, 99, nil);

    recover_screen(2);
    message_end();
    unload_rate_tab();
    return;
}

/* function  : inquire the long call telephone records for a user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void inquire_user_long(void)
{
    UC   flag;
    UL   input_no=0;
    UI   pnum=0;
    USERS_STRUCT user;
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
                               {44, 96, 70, 175, 44, 44, 44, 52, 60},\
                               GREEN};

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    flag = input_user(&input_no, INQUIRE);
    if(flag == 2)
    {
        unload_rate_tab();
        return;
    }
    else if(flag == FALSE)        /* the user no does not exist */
    {
        message(UNO_NOT_EXIST);
        unload_rate_tab();
        return;
    }

    search_user((UI)input_no,&pnum,&user);

    load_phone_no(pnum,&user);

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"国 际 长 途 话 单",BLACK);  /* International long call */

    if(disp_long_rcd(IDD, DEPT_NO, 99, 99, 99, user) == FALSE)
    {
        unload_phone_no();
        recover_screen(2);
        message_end();
        unload_rate_tab();
        return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"国 内 长 途 话 单",BLACK);  /* national long call */

    disp_long_rcd(DDD, DEPT_NO, 99, 99, 99, user);

    recover_screen(2);
    message_end();
    unload_phone_no();
    unload_rate_tab();
    return;
}


/* function  : inquire the long call telephone records for a user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void inquire_auth_long(void)
{
    UC    flag;
    UL    auth_cd=0;
    USERS_STRUCT nil= {0, "", 0};
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
                               {44, 96, 70, 175, 44, 44, 44, 52, 60},\
                               GREEN};

    flag = input_auth_cd(&auth_cd, INQUIRE);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the user no does not exist */
    {
        message(AUTHCD_NOT_EXIST);
        return;
    }

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

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"国 际 长 途 话 单",BLACK);  /* International long call */

    if(disp_long_rcd(IDD, AUTH_CD, 99, 99, auth_cd, nil) == FALSE)
    {
        if(Authflg == ON)
            unload_authcd_tbl();
        recover_screen(2);
        message_end();
        unload_rate_tab();
        return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"国 内 长 途 话 单",BLACK);  /* national long call */

    disp_long_rcd(DDD, AUTH_CD, 99, 99, auth_cd, nil);

    recover_screen(2);
    message_end();
    if(Authflg == ON)
        unload_authcd_tbl();
    unload_rate_tab();
    return;
}



/* function  : inquire the long call telephone records of all phones
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void inquire_all_long(UC isauthcd)
{
    UC   flag;
    USERS_STRUCT nil= {0, "", 0};
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
                               {44, 96, 70, 175, 44, 44, 44, 52, 60},\
                               GREEN};

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    flag  = load_authcd_tbl(0);
    if(flag != TRUE)
        Authflg =OFF;
    else
        Authflg = ON;

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"国 际 长 途 话 单",BLACK);  /* International long call */

    if(disp_long_rcd(IDD, WHOLE, isauthcd, 99, 99, nil) == FALSE)
    {
        recover_screen(2);
        message_end();
        unload_rate_tab();
        if(Authflg == ON)
            unload_authcd_tbl();
        return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"国 内 长 途 话 单",BLACK);  /* national long call */

    disp_long_rcd(DDD, WHOLE, isauthcd, 99, 99, nil);

    recover_screen(2);
    message_end();
    if(Authflg == ON )
        unload_authcd_tbl();
    unload_rate_tab();
    return;
}

UC  disp_long_rcd(UC mode, UC isphone, UC isauthcd, \
                    UL phone_no, UL auth_cd, USERS_STRUCT user)
{
    FILE *fp;
    UC   position=0;
    UI   key, num=0, i;
    char *a_name="    ";
    char  auth_name[9];
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
                if(!is_in_range())         /* not in the sorting time range */
                    continue;

                if(isphone == PHONE_NO)
                {
		    if(temp_list.caller2 != phone_no)
			continue;
		    if(temp_list.auth_code != 0xaaaaaa)
			continue;
		    strcpy(auth_name, "");
		}
		else if(isphone == DEPT_NO)
		{
		    if(!binary_phone(&user))
			continue;
		    if(temp_list.auth_code != 0xaaaaaa)
			continue;
		    strcpy(auth_name, "");
		}
                else if(isphone == AUTH_CD)
                {
                    if(temp_list.auth_code != auth_cd)
                        continue;
                    if(temp_list.auth_code == 0xaaaaaa)
                        continue;
                    find_auth_usr(auth_name);
                }
                else if(isphone == WHOLE)
                {
                    if( (isauthcd == PHONE_NO) && (temp_list.auth_code == 0xaaaaaa) )
                        strcpy(auth_name, "");
                    else if( (isauthcd == AUTH_CD) && (temp_list.auth_code != 0xaaaaaa) )
                        find_auth_usr(auth_name);
                    else
                        auth_usr_proc(auth_name);
		}

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
                    rate_tmp = get_rate(temp_list.callee,LDD);       /* get charge rate */
                }

                if(rate_tmp == NULL)                  /* the area No. does not exist    */
                    strcpy(rate1.area_name, a_name);
                else
                    rate1 = *rate_tmp;

                num++;
                disp_a_record(auth_name, position,num);
                position++;
                if( (position>=16) && (!EndofRecFile(fp)) )
                {
                    key = get_key1();
                    if(key == ESC)
                    {
                        fclose(fp);
                        return FALSE;
                    }
                    else
                    {
                        brush_tbl(9, 16, 7);
                        position = 0;
                    }
                }
            }          /* end of "while(fread(...fp)==1)" */
            fclose(fp);
        }
    }         /* end of "if(fp != NULL)"        */

    key = get_key1();
    if(mode == IDD)
    {
        if(key == ESC)
        {
            return FALSE;
        }
        else
        {
            brush_tbl(9, 16, 7);
            position = 0;
            num = 0;
        }
    }
    return TRUE;
}



