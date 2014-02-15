#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : input the phone no to be inquired
 * called by : inquire_one_record()
 * input     : no -- phone number
 * output    : TRUE  -- the phone no exists
 *	       FALSE -- the phone no does not exist
 *	       2     -- does not input the number
 * date      : 1993.10.4
 */
UC input_phone_no(UL *no, UC opflg)
{
    PHONE_STRUCT *tbl;
    UC	result;
    UC	back_in=5;
    UC	phone[10]="";

    pop_back(H_BX-40,H_BY-11,H_BX+240,H_BY+35,7); /* big frame */
    draw_back(H_BX+132,H_BY,H_BX+230,H_BY+25,11);
    message_disp(8," 输入分机号码   Enter 确认");    /*phone no.*/
    if( opflg == INQUIRE )
    {
	hz16_disp(H_BX-35,H_BY+5,"请输入欲查询分机号码",BLACK);   /* input phone num  */
    }
    else if( opflg == DELETE )
    {
	hz16_disp(H_BX-35,H_BY+5,"请输入欲删除分机号码",BLACK);   /* input phone num  */
    }
    else if( opflg == PRINT )
    {
	hz16_disp(H_BX-35,H_BY+5,"请输入欲打印分机号码",BLACK);   /* input phone num  */
    }
    else
    {
	hz16_disp(H_BX-35,H_BY+5,"请输入欲结算分机号码",BLACK);   /* input phone num  */
    }
    result = get_no(H_BX+135,H_BY+5, 18, 60, back_in, 8, phone, 0x00);
    message_end();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
    {
	rid_pop();
	return(2);
    }

    load_phone_tbl();

    Newlen = phone_tbl_len();

    /*	judge whether the phone number exists or not	      */
    tbl = binary_search(phone);
    if(tbl == NULL)	     /* can not find the phone_number */
    {
	unload_phone_tbl();
	rid_pop();
	return(FALSE);
    }

    unload_phone_tbl();

    *no = atol(phone);
    rid_pop();
    return(TRUE);
}


/* function  : input the phone no to be inquired
 * called by : inquire_one_record()
 * input     : no -- phone number
 * output    : TRUE  -- the phone number exists
 *	       FALSE -- the phone number does not exist
 *	       2     -- does not input the number
 * date      : 1993.10.4
 */
UC input_auth_cd(UL *auth_cd, UC opflg)
{
    AUTHCD_STRUCT *atbl;
    UC	result, flag;
    UC	back_in=5;
    UC	authcd[7]="";

    pop_back(H_BX-40,H_BY-11,H_BX+240,H_BY+35,7); /* big frame */
    draw_back(H_BX+132,H_BY,H_BX+230,H_BY+25,11);
    message_disp(8," 输入授权卡号   Enter 确认");    /*phone no.*/
    if( opflg == INQUIRE )
    {
	hz16_disp(H_BX-22,H_BY+5,"输入欲查询授权卡号",BLACK);   /* input phone num  */
    }
    else if( opflg == DELETE )
    {
	hz16_disp(H_BX-22,H_BY+5,"输入欲删除授权卡号",BLACK);   /* input phone num  */
    }
    else if( opflg == PRINT )
    {
	hz16_disp(H_BX-22,H_BY+5,"输入欲打印授权卡号",BLACK);   /* input phone num  */
    }
    else
    {
	hz16_disp(H_BX-22,H_BY+5,"输入欲结算授权卡号",BLACK);   /* input phone num  */
    }
    result = get_no(H_BX+135, H_BY+5, 18, 60, back_in, 6, authcd, 0x00);
    message_end();

    rid_pop();
    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return(2);

    flag = load_authcd_tbl(0);
    if(flag == FALSE)		      /* charge.pho does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return(2);
    }
    ANewlen = authcd_tbl_len();

    *auth_cd = atol(authcd);
    atbl = binary_authcd(*auth_cd);
    if(atbl == NULL)
    {
	unload_authcd_tbl();
	return(FALSE);
    }

    unload_authcd_tbl();
    return(TRUE);
}


/* function  : display head for inquring record table
 * called by : inquire_one_record(),inquire_all_record()
 * date      : 1993.10.5
 */
void disp_record_head(void)
{
    outf(7,   103, 7,0, "序号");      /* sequence num  */
    outf(52,   103, 7,0, "分  机");   /* caller        */
    outf(146,  103, 7,0, "授权用户"); /* authorized user */
//  outf(180, 103, 7,0, "中继");      /* trunk No      */
    outf(217, 103, 7,0, "通达地区");  /* callee        */
    outf(291, 103, 7,0, "被    叫");  /* callee        */
    outf(396, 103, 7,0, "日期");      /* date          */
    outf(443, 103, 7,0, "起时");      /* starting time */
    outf(488, 103, 7,0, "时长");      /* duration      */
    outf(528, 103, 7,0, "附  加");    /* main caller 2 */
    outf(590-6, 103, 7,0, "话  费");    /* charge fee    */

    return;
}

/* function  : judge whether the record is in the sorting time range or not
 * called by : inquire_one_record(),inquire_all_record()
 * output    : TRUE  -- is  in the range
 *	       FALSE -- not in the range
 * date      : 1993.10.5
 * note      : only judge whether the ending time of the record is in the
 *	       range or not
 */
UC is_in_range(void)
{
    UC	ok1=0, ok2=0;

    if(temp_list.year2 > s_time.year)
	ok1 = 1;
    else if(temp_list.year2 == s_time.year)
	if(temp_list.mon2 > s_time.month)
	    ok1 = 1;
	else if(temp_list.mon2 == s_time.month)
	    if(temp_list.day2 > s_time.day)
		ok1 = 1;
	    else if(temp_list.day2 == s_time.day)
		if(temp_list.hour2 >= s_time.hour)
		    ok1 = 1;

    if(ok1 == 1)
    {
	if(temp_list.year2 < e_time.year)
	    ok2 = 1;
	else if(temp_list.year2 == e_time.year)
	    if(temp_list.mon2 < e_time.month)
		ok2 = 1;
	    else if(temp_list.mon2 == e_time.month)
		if(temp_list.day2 < e_time.day)
		    ok2 = 1;
		else if(temp_list.day2 == e_time.day)
		    if(temp_list.hour2 <= e_time.hour)
			ok2 = 1;
    }  /* if(ok1==1) */

    if((ok1==1) && (ok2==1))
	return(TRUE);
    else
	return(FALSE);
}

/* function  : display a record in the table
 * called by : inquire_one_record(),inquire_all_record()
 * input     : pos -- display position in the table
 *	       num -- sequence number
 * date      : 1993.10.5
 */
void disp_a_record(char *auth_name, UC pos, UI num)
{
    int  i, j, k, l;
    int  dh, dm, ds;
    UC	 area_n[19];
    UC	 tel_no[16];
    UI	 x, y;
    UNIT_STRUCT  cur_unit;

    cur_unit.unit_x = pos;
    for(j=0;j<10;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+4;
	y = cur_unit.dot_sy+1;
	switch(j)
	{
	    case 0:	     /* sequence number */
		outf(x,y,7,0,"%-5u",num);
		break;
	    case 1:
		outf(x,y,7,0,"%-lu",temp_list.caller2);
                if(temp_list.caller2 != 999999L)
         	    outf(x,y,7,0,"%-lu",temp_list.caller2);
                else
		    outf(x,y,7,0,"未定义");
		break;
	    case 2:
     	        if(temp_list.auth_code != 0xaaaaaa)
                    outf(x, y, 7, 0,"%s",auth_name);
		break;
//	    case 3:
//		outf(x,y,7,0,"%-u", temp_list.trunk_no);
//		break;
	    case 3:
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
	    case 7:	 /* add fee	 */
		 outf(x,y,7,0,"%6.2f",(double)(temp_list.add)/100);
		 break;
	    case 8:
		outf(x,y,7,0,"%7.2f",(double)(temp_list.charge)/100);
		break;
	}     /* end of "switch(j)"        */
    }	      /* end of "for(j=0;j<8;j++)" */

    return;
}

/* function  : judge whether is end of record file(*.dat) or not
 * called by : inqire_one(),inqire_all(),out_rang_proc()
 * input     : fp -- the file pointer to *.dat
 * date      : 1993.10.8
 * note      : why we edit this module is that, the lib function
 *	       "feof()" sometimes gets error, or to say, not very accurate,
 *	       if we use it, we often read one more item from the file than
 *	       the real items in the file
 */
UC  EndofRecFile(FILE *fp)
{
    UC	   flag;
    fpos_t filepos;
    RECORD_STRUCT temp;

    fgetpos(fp, &filepos);
    if(fread(&temp,sizeof(RECORD_STRUCT),1,fp) != 1)   /* End of File*/
	flag = TRUE;
    else
	flag = FALSE;
    fsetpos(fp, &filepos);

    return(flag);
}


/* function  : inquire the local call telephone records for specified phone no
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void inquire_one_local(void)
{
    UL	 phone_no;
    UC	 flag;
    USERS_STRUCT nil= {0, "", 0};
    TABLE_STRUCT record_tbl = { 0, 99, 20, 18, 16, 9,
				{44, 96, 70, 175, 44, 44, 44, 52, 60},
				GREEN
			       };

    flag = input_phone_no(&phone_no, INQUIRE);
    if(flag == 2)
    {
	return;
    }
    else if(flag == FALSE)	  /* the phone no does not exist */
    {
	message(NO_NOT_EXIST);
	return;
    }

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"市   话   话   单",BLACK);   /* local long call */

    disp_long_rcd(LDD, PHONE_NO, 99, phone_no, 99, nil);

    recover_screen(2);
    message_end();
    unload_rate_tab();

    return;
}


/* function  : inquire the local call telephone records for a user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void inquire_user_local(void)
{
    UC	 flag;
    UL	 input_no=0;
    UI	 pnum=0;
    USERS_STRUCT user;
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
			       {44, 96, 70, 175, 44, 44, 44, 52, 60},\
			       GREEN};

    flag = input_user(&input_no, INQUIRE);
    if(flag == 2)
	return;
    else if(flag == FALSE)	  /* the user no does not exist */
    {
	message(UNO_NOT_EXIST);
	return;
    }

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    search_user((UI)input_no,&pnum,&user);

    load_phone_no(pnum,&user);

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"市   话   话   单",BLACK);   /* local long call */

    disp_long_rcd(LDD, DEPT_NO, 99, 99, 99, user);

    recover_screen(2);
    message_end();
    unload_phone_no();
    unload_rate_tab();
    return;
}

/* function  : inquire the local call telephone records for a user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void inquire_auth_local(void)
{
    UC	 flag;
    UL	 auth_cd=0;
    USERS_STRUCT nil = {0, "", 0};
    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
			       {44, 96, 70, 175, 44, 44, 44, 52, 60},\
			       GREEN};

    flag = input_auth_cd(&auth_cd, INQUIRE);
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
    if(flag != TRUE)
	Authflg = OFF;
    else
	Authflg = ON;

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"市   话   话   单",BLACK);   /* local long call */

    disp_long_rcd(LDD, AUTH_CD, 99, 99, auth_cd, nil);

    recover_screen(2);
    message_end();
    if(Authflg == ON)
	unload_authcd_tbl();
    unload_rate_tab();
    recover_screen(2);
}


/* function  : inquire the local call telephone records of all phones
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void inquire_all_local(UC isauthcd)
{
    UC	 flag;
    USERS_STRUCT nil = {0, "", 0};
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
    if(flag == TRUE)
	Authflg=ON;
    else
	Authflg=OFF;

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"市   话   话   单",BLACK);   /* local long call */

    disp_long_rcd(LDD, WHOLE, isauthcd, 99, 99, nil);

    recover_screen(2);
    message_end();
    if(Authflg == ON)
	unload_authcd_tbl();
    unload_rate_tab();
    return;
}


