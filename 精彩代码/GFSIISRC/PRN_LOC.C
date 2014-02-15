#include <bios.h>
#include <conio.h>
#include <alloc.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

#define LIST_MODE    0
#define INVOICE_MODE 1

A_PHONE 		*p_top;

/* function  : print the local call telephone records of a specified phone
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void print_one_local(void)
{
    UL	 phone_no;
    UC	 flag;
    USERS_STRUCT nil={0, "", 0};

    flag = check_prn();
    if(flag == FALSE)	     /* printer is not ready */
	return;

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

    prn_long_rcd(LDD, PHONE_NO, 99, phone_no, 99, nil);
    message_end();
    unload_rate_tab();
    return;
}

/* function  : print the local call telephone records for a user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void print_user_local(void)
{
    UC	 flag;
    UL	 input_no=0;
    UI	 pnum=0;
    USERS_STRUCT user;

    flag = check_prn();
    if(flag == FALSE)	     /* printer is not ready */
	return;

    flag = input_user(&input_no, PRINT);
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

    message_disp(8,"正在打印，请稍候...");   /* printing */
    prn_long_rcd(LDD, DEPT_NO, 99, 99, 99, user);
    message_end();

    unload_phone_no();
    unload_rate_tab();
    return;
}


/* function  : print the local call telephone records for a user unit
 * called by : phone_record_proc()
 * date      : 1993.11.15
 */
void print_auth_local(void)
{
    UC	 flag;
    UL	 auth_cd=0;
    USERS_STRUCT nil = {0, "", 0};

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
    if(flag != TRUE)
	Authflg = OFF;
    else
	Authflg = ON;

    message_disp(8,"正在打印，请稍候...");   /* printing */

    prn_long_rcd(LDD, AUTH_CD, 99, 99, auth_cd, nil);
    message_end();
    if(Authflg == ON)
        unload_authcd_tbl();
    unload_rate_tab();
    return;
}


/* function  : print the local call telephone records of all phones
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void print_all_local(UC isauthcd)
{
    UC	 flag;
    USERS_STRUCT nil={0, "", };

    flag = check_prn();
    if(flag == FALSE)
	return;

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
    prn_long_rcd(LDD, WHOLE, isauthcd, 99, 99, nil);
    message_end();

    if(Authflg == ON)
        unload_authcd_tbl();
    unload_rate_tab();
    return;
}



/* function  : load the phone no of this user unit
 * called by : print_user_loc()
 * input     : p_top -- the phone no top
 *	       pnum  -- the phones before this user unit
 *	       user  -- the user unit to be inquired
 * date      : 1993.11.15
 */
void load_phone_no(UI pnum, USERS_STRUCT *user)
{
    int      i;
    FILE     *fpp;
    A_PHONE  *phone_no;

    p_top = (A_PHONE *)farcalloc(user->phones, sizeof(A_PHONE));
    if(p_top == NULL)
    {
	exit_scr(1,"Out of memery ERROR.\n\nGFS system shutdown abnormally.\n\n\n\n");
    }

    fpp = fopen("gfsdata\\phones.idx","rb");
    fseek(fpp,sizeof(A_PHONE)*pnum,SEEK_SET);

    phone_no = p_top;
    for(i=0;i<user->phones;i++)
    {
	fread(phone_no,sizeof(A_PHONE),1,fpp);
	phone_no++;
    }
    fclose(fpp);

    return;
}

/* function  : free the space for phone no
 * date      : 1993.11.15
 */
void unload_phone_no(void)
{
    farfree(p_top);
    return;
}

/* function  : judge whether the caller2 is in this user unit or not
 * called by : a general purposed routine
 * input     : p_top -- the phone no top
 *	       user  -- the user unit to be inquired
 * output    : 0 -- in this user unit
 *	       1 -- not in this user unit
 * date      : 1993.11.15
 */
UC binary_phone(USERS_STRUCT *user)
{
    int result;
    UC	input_no[10];
    UI	tbllen;
    UI	top, bottom, middle;
    A_PHONE *tbl, *tbl_top;

    ltoa(temp_list.caller2,input_no,10);

    tbl_top = p_top;
    tbl = p_top;
    tbllen = user->phones;

    top = 0;
    bottom = tbllen-1;

    /***** Compare with the first item. *****/
    if(strcmp(	  tbl->phone_no,input_no)<0    && \
       strcmp((tbl+1)->phone_no,input_no)>0)
	    return(0);

    if(strcmp(tbl->phone_no,input_no) > 0)
	    return(0);

    /***** Compare with the last item. *****/
    if(strcmp(tbl[bottom].phone_no,input_no) < 0)
	    return(0);

    /***** Compare and locate in the fee table. *****/
    while(top <= bottom)
    {
	middle = (UI) (top+bottom)/2;
	tbl = &tbl_top[middle];
	result = strcmp(tbl->phone_no,input_no);

	if(!result)	     /* find! */
	    return(1);

	else if(result < 0)	  /* tbl->phone_no < input_no */
	    top = middle+1;
	else if(result > 0)	  /* tbl->phone_no > input_no */
	    bottom = middle-1;
    }

    return(0);		       /* not find! */
}



/* function  : print a telephone record
 * called by : print_one_local(),print_all_local(),print_one_long(),
 *	       print_all_long(),print_all()
 * input     : num -- sequence number
 * date      : 1993.10.5
 */
UC print_a_record(char *auth_name, UI num)
{
    UI	  i, l, k;
    int   dh, dm, ds;
    UC	  flag, tel_no[16], area_n[16], space[3]="  ";
    char  au_name[9];
    UC    mode;

    mode = LIST_MODE;
    /* if it is a LQ1600 printer, set two space between every two items,
     * otherwise, if it is a LX800 printer, set one space between every
     * two items
     */
    if(Sys_mode.prt_w == N_WIDTH) space[1] = '\0';
    space[1] = '\0';

    for(i=0; i<8; i++)
    {
	if(auth_name[i] == '\0') break;
	au_name[i]=auth_name[i];
    }
    for(; i<8; i++)
	au_name[i]=' ';
    au_name[i]='\0';

    if(mode == INVOICE_MODE) {
	flag=prnf("\n\n\n\n%40s\n\n","电  话  费  用  单");
    	if(flag==FALSE)  return(FALSE);
        flag=prnf("序号   分机  授权用户 通达地区 被叫号码     日期  起时  时长           话费\n\n");
    	if(flag==FALSE)  return(FALSE);
    }

    flag=prnf("%-4u%s",num,space);
    if(flag==FALSE)  return(FALSE);

    flag=prnf("%-8lu",temp_list.caller2);
    if(flag==FALSE) return(FALSE);

    flag=prnf("%s", au_name);
    if(flag==FALSE) return(FALSE);

//    flag=prnf("%s", au_name);
//    if(flag==FALSE) return(FALSE);

//    if(Sys_mode.prt_w == N_WIDTH)
//    {
//	flag=prnf("%4u%s",temp_list.trunk_no,space);
//	if(flag==FALSE) return(FALSE);
//    }
//    else
//    {
//	flag=prnf("%-4u%s",temp_list.trunk_no,space);
//	if(flag==FALSE) return(FALSE);
//    }

    i=0;
    while( (temp_list.callee[i] != 0xfd) && (i<14) )
    {
	tel_no[i] = temp_list.callee[i]+'0';
	i++;
    }
    tel_no[i] = '\0';
    l=(22-i)/2;
    if(l > 4 ) l=4;
    for(k=0; k<l*2; k++)
        area_n[k] = rate1.area_name[k];
    area_n[k] = '\0';
    flag=prnf("%-8s",area_n);
    if(flag==FALSE) return(FALSE);
    flag=prnf("%-14s%s",tel_no,space);
    if(flag==FALSE) return(FALSE);

    if(Sys_mode.prt_w == W_WIDTH)
    {
	flag=prnf("%02u%02u/%02u%s", temp_list.year1,\
		   temp_list.mon1, temp_list.day1, space);
	if(flag==FALSE) return(FALSE);
    }
    else
    {
	flag=prnf("%02u/%02u%s", temp_list.mon1, temp_list.day1, space);
	if(flag==FALSE) return(FALSE);
    }
    flag=prnf("%02u:%02u%s",temp_list.hour1,temp_list.min1,space);
    if(flag==FALSE) return(FALSE);

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

    if( Sys_mode.prt_w == W_WIDTH)
    {
	flag=prnf("%02u:%02u:%02u%s", dh, dm, ds, space);
	if(flag==FALSE) return(FALSE);
    }
    else
    {
	 if( ds != 0 )
       {
	  dm ++;
	  if( dm == 60 )
	  {
	     dm=0;
	     dh++;
	  }
       }
	flag=prnf("%02u:%02u", dh, dm);
	if(flag==FALSE) return(FALSE);
    }

    if( Sys_mode.prt_w == W_WIDTH)
    {
	flag=prnf("%8.2f%s", (double)(temp_list.charge - temp_list.add)/100, space);
	if(flag==FALSE) return(FALSE);
    }

//    flag=prnf("%7.2f", (double)(temp_list.add)/100);
    flag=prnf("       ");
    if(flag==FALSE) return(FALSE);

    flag=prnf("%8.2f\n", (double)(temp_list.charge)/100);
    if(flag==FALSE) return(FALSE);

    if(mode == INVOICE_MODE) {
    	flag = prnf("\n\n\n\n\n\n\n");
        if(flag == FALSE) return (FALSE);
    }

    return(TRUE);
}



UC print_rcdrpt_head(UC isidd)
{
    UC *head[4] = {
	           "      国   际   长   话   话   单",
	           "      国   内   长   话   话   单",
	           "      市   内   电   话   话   单",
	           "      电   话   话   单   清   单"};

//    if(Sys_mode.prt_w == N_WIDTH || Sys_mode.prt_w == W_WIDTH)
//    {
//	if(Sys_mode.prt_m == LQ1600)
//	    if(set_dbl_char() == FALSE)
//                return(FALSE);

	if(isidd == IDD)
	{
//	if(Sys_mode.prt_m == LQ1600)
//        {
//	    if(prnf("\n%s\n\n", head[0]) == FALSE)
//		return(FALSE);
//        }
//        else
//        {
	    if(prnfd("\n%s\n\n", head[0]) == FALSE)
		return(FALSE);
//        }
	}
	else if(isidd == DDD)
	{
//	if(Sys_mode.prt_m == LQ1600)
//        {
//	    if(prnf("\n%s\n\n", head[1]) == FALSE)
//		return(FALSE);
//        }
//        else
//        {
	    if(prnfd("\n%s\n\n", head[1]) == FALSE)
		return(FALSE);
//        }
	}
	else if(isidd == LDD)
	{
//	if(Sys_mode.prt_m == LQ1600)
//        {
//	    if(prnf("\n%s\n\n", head[2]) == FALSE)
//		return(FALSE);
//        }
//        else
//        {
	    if(prnfd("\n%s\n\n", head[2]) == FALSE)
		return(FALSE);
//        }
	}
	else
	{
//	if(Sys_mode.prt_m == LQ1600)
//        {
//	    if(prnf("\n%s\n\n", head[3]) == FALSE)
//		return(FALSE);
//        }
//        else
//        {
	    if(prnfd("\n%s\n\n", head[3]) == FALSE)
		return(FALSE);
//        }
	}

//	if(Sys_mode.prt_m == LQ1600)
//	{
//	    if(rst_nomal_char() == FALSE)
//                return(FALSE);
//	    if(set_clos_tbl() == FALSE)
//                return(FALSE);
//	}

        if(print_rcdtbl() == FALSE)
	    return(FALSE);

	if(prnf("序号 分机   授权人  通达地区  被    叫      日期  起时  时长 附加费 总   费\n") == FALSE)
	    return(FALSE);

	if(print_rcdtbl() == FALSE)
	    return(FALSE);
//    }
    return(TRUE);
}

UC print_rcdtbl(void)
{
    if(prnf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") == FALSE)
        return(FALSE);
    return(TRUE);
}


UC print_rcdrpt_tail(void)
{
    struct date now;

    getdate(&now);

    if(Sys_mode.prt_w == N_WIDTH)
    {
        if(print_rcdtbl() == FALSE)
	    return(FALSE);
	if(prnf(" 制表时间：%-4u年%2u月%2u日\n",\
		 now.da_year, now.da_mon, now.da_day )\
		 == FALSE)
	    return(FALSE);
    }
    else
    {
        if(print_rcdtbl() == FALSE)
	    return(FALSE);

	if(prnf(" 制表时间：%-4u年%2u月%2u日\n",\
		 now.da_year, now.da_mon, now.da_day )\
		 == FALSE)
	    return(FALSE);
    }
    if(new_page() == FALSE)
        return(FALSE);

    return(TRUE);
}


UC new_page(void)
{
//    if(prn_ch(0x0C) == FALSE)
//        return(FALSE);
    if(prnf("\n\n\n\n") == FALSE)
        return(FALSE);
    return(TRUE);
}


UC print_rcdrpt_detail(char *auth_name, UI num, UC isidd)
{
    if(num%Page_len != 0 )
    {
       if(print_a_record(auth_name, num)==FALSE)
	   return(FALSE);
    }
    else
    {
       if(print_rcdrpt_tail() == FALSE)
	   return(FALSE);
       if(print_rcdrpt_head(isidd) == FALSE)
	   return(FALSE);
       if(print_a_record(auth_name, num)==FALSE)
	   return(FALSE);
    }
    return(TRUE);
}

