#include <bio.inc>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>

#include "feedef.h"

/* function  : inquire the old telephone records of some specified month
 * called by : phone_record_proc() (record.c)
 * date      : 1993.11.5
 */
void inquire_old_rec(UC isphone)
{
    UC    suc, flag;
    UL    mon=0, phone_no, input_cd;
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
                               {44, 96, 70, 175, 44, 44, 44, 52, 60},\
                               GREEN};

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
	message(RATE_FILE_ERR);
	return;
    }

    suc = input_month(INQUIRE,&mon);        /* input the month for records */
    if(suc == FALSE)                        /* unsuccessful */
    {
	unload_rate_tab();
	return;
    }

    if(isphone == PHONE_NO)
    {
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
    }
    else if(isphone == AUTH_CD)
    {
        flag = input_auth_cd(&input_cd, INQUIRE);
        if(flag == 2)
        {
            unload_rate_tab();
	    return;
        }
        else if(flag == FALSE)        /* the phone no does not exist */
        {
            unload_rate_tab();
            message(AUTHCD_NOT_EXIST);
	    return;
        }
    }

    flag  = load_authcd_tbl(0);
    if(flag != TRUE)
        Authflg = OFF;
    else
        Authflg = ON;

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();

    message_disp(8,"按任意键继续");         /* press any key */

    hz16_disp(240,80,"国 际 长 途 话 单",BLACK);  /* International long call */

    if(disp_a_old_rcd(IDD, isphone, mon, phone_no, input_cd) == FALSE)
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

    if(disp_a_old_rcd(DDD, isphone, mon, phone_no, input_cd) == FALSE)
    {
        recover_screen(2);
        message_end();
        unload_rate_tab();
        if(Authflg == ON)
            unload_authcd_tbl();
        return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"市   话   话   单",BLACK);   /* local long call */

    disp_a_old_rcd(LDD, isphone, mon, phone_no, input_cd);
    recover_screen(2);
    message_end();
    unload_rate_tab();
    if(Authflg == ON)
        unload_authcd_tbl();
    return;
}


UC  disp_a_old_rcd(UC mode, UC isphone, UL mon, UL phone_no, UL input_cd)
{
    FILE  *fp;
    UC    month[3], back_file[15], auth_name[9];
    UC    position=0;
    UI    key, num = 0;
    char *a_name="    ";
    RATE_STRUCT huge *rate_tmp;

    if(mode == IDD)
        strcpy(back_file,"idd");
    else if(mode == DDD)
        strcpy(back_file,"ddd");
    else
        strcpy(back_file,"ldd");

    ltoa(mon, month, 10);
    strcat(back_file,month);
    strcat(back_file,".dbf");
    fp = fopen(back_file,"rb");
    if(fp != NULL)
    {
	while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
	{
            if( isphone == PHONE_NO )
            {
                if(temp_list.auth_code != 0xaaaaaa)
                     continue;
	        if(temp_list.caller2 != phone_no)           /* is this phone     */
                     continue;

            }
            else if( isphone == AUTH_CD )
            {
                if(temp_list.auth_code == 0xaaaaaa)
                     continue;
	        if(temp_list.auth_code != input_cd)           /* is this phone     */
                     continue;
            }

            if(mode == IDD)
            {
	        rate_tmp = get_rate(&temp_list.callee[2],IDD);	 /* get charge rate */
            }
            else if(mode == DDD)
            {
	        rate_tmp = get_rate(&temp_list.callee[1],DDD);	 /* get charge rate */
            }
            else
            {
	        rate_tmp = get_rate(temp_list.callee,LDD);	 /* get charge rate */
            }

	    if(rate_tmp == NULL)		  /* the area No. does not exist    */
	        strcpy(rate1.area_name, a_name);
	    else
	        rate1 = *rate_tmp;

	    num++;

            auth_usr_proc(auth_name);
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

        if(mode != LDD)
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
    }         /* end of "if(fp != NULL)"        */
    else
	get_key1();

    return TRUE;
}

