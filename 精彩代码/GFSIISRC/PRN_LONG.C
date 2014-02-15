#include <string.h>
#include <stdlib.h>
#include <bios.h>

#include <bio.inc>
#include "feedef.h"

/* function  : print the long call telephone records of a specified phone
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void print_one_long(void)
{
    UL	 phone_no;
    UC	 flag;
    USERS_STRUCT nil= {0, "", 0};

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

    flag = input_phone_no(&phone_no, PRINT);
    if(flag == 2)		  /* ESC key is pressed 	 */
    {
	unload_rate_tab();
	return;
    }
    else if(flag == FALSE)	  /* the phone No does not exist */
    {
	message(NO_NOT_EXIST);
	unload_rate_tab();
	return;
    }

    message_disp(8,"正在打印，请稍候...");   /* printing */

    if(prn_long_rcd(IDD, PHONE_NO, 99, phone_no, 99, nil) == FALSE)
    {
        message_end();
	unload_rate_tab();
	return;
    }

    prn_long_rcd(DDD, PHONE_NO, 99, phone_no, 99, nil);
    message_end();
    unload_rate_tab();
    return;
}

/* function  : print the long call telephone records of the user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void print_user_long(void)
{
    UC	 flag;
    UL	 input_no=0;
    UI	 pnum=0;
    USERS_STRUCT user;

    flag = check_prn();
    if(flag == FALSE)	     /* printer is not ready */
    {
	return;
    }

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
	message(RATE_FILE_ERR);
	return;
    }

    flag = input_user(&input_no, PRINT);
    if(flag == 2)
    {
	unload_rate_tab();
	return;
    }
    else if(flag == FALSE)	  /* the user no does not exist */
    {
	message(UNO_NOT_EXIST);
	unload_rate_tab();
	return;
    }

    search_user((UI)input_no,&pnum,&user);

    load_phone_no(pnum,&user);

    message_disp(8,"正在打印，请稍候...");   /* printing */

    if(prn_long_rcd(IDD, DEPT_NO, 99, 99, 99, user) == FALSE)
    {
	message_end();
	unload_rate_tab();
	unload_phone_no();
	return;
    }

    prn_long_rcd(DDD, DEPT_NO, 99, 99, 99, user);
    message_end();
    unload_rate_tab();
    unload_phone_no();
    return;
}



/* function  : print the long call telephone records of the user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void print_auth_long(void)
{
    UC	 flag;
    UL	 auth_cd=0;
    USERS_STRUCT nil= {0, "", 0};

    flag = check_prn();
    if(flag == FALSE)	     /* printer is not ready */
    {
	return;
    }

    flag = input_auth_cd(&auth_cd, PRINT);
    if(flag == 2)
	return;
    else if(flag == FALSE)	  /* the user no does not exist */
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

    message_disp(8,"正在打印，请稍候...");   /* printing */

    if(prn_long_rcd(IDD, AUTH_CD, 99, 99, auth_cd, nil) == FALSE)
    {
        message_end();
        unload_rate_tab();
	unload_authcd_tbl();
        return;
    }

    prn_long_rcd(DDD, AUTH_CD, 99, 99, auth_cd, nil);

    message_end();
    unload_rate_tab();
    unload_authcd_tbl();
    return;
}



/* function  : print the long call telephone records of all phones
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void print_all_long(UC isauthcd)
{
    UC	 flag;
    USERS_STRUCT nil= {0, "", 0};

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

    flag  = load_authcd_tbl(0);
    if(flag != TRUE)
	Authflg = OFF;
    else
	Authflg = ON;

    message_disp(8,"正在打印，请稍候...");   /* printing */

    if(prn_long_rcd(IDD, WHOLE, isauthcd, 99, 99, nil) == FALSE)
    {
        message_end();
        unload_rate_tab();
        if(Authflg == ON)
	    unload_authcd_tbl();
        return;
    }

    prn_long_rcd(DDD, WHOLE, isauthcd, 99, 99, nil);

    message_end();
    unload_rate_tab();
    if(Authflg == ON)
        unload_authcd_tbl();
    return;
}


UC  prn_long_rcd(UC mode, UC isphone, UC isauthcd, \
                 UL phone_no, UL auth_cd, USERS_STRUCT user)
{
    FILE *fp;
    UI    num=0, i;
    UC pflg = FALSE;
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
            pflg = TRUE;
	    if(i == 0 && mode == IDD)
	    {
		if(print_rcdrpt_head(IDD)==FALSE)
		{
		    fclose(fp);
		    return FALSE;
		}
	    }
	    else if(i == 0 && mode == DDD)
	    {
		if(print_rcdrpt_head(DDD)==FALSE)
		{
		    fclose(fp);
		    return FALSE;
		}
	    }
	    else if(i == 0 && mode == LDD)
	    {
		if(print_rcdrpt_head(LDD)==FALSE)
		{
		    fclose(fp);
		    return FALSE;
		}
	    }
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
                    else if(isauthcd == WHOLE)
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

	        if(rate_tmp == NULL)	  /* the area No. does not exist    */
	            strcpy(rate1.area_name, a_name);
	        else
	            rate1 = *rate_tmp;

                num++;
		if(print_rcdrpt_detail(auth_name, num, mode)==FALSE)
		{
		    fclose(fp);
		    return FALSE;
		}
            }          /* end of "while(fread(...fp)==1)" */
            fclose(fp);
        }
    }         /* end of "if(fp != NULL)"        */

    if(pflg == TRUE)
	if(print_rcdrpt_tail()==FALSE)
	    return FALSE;

    return TRUE;
}

