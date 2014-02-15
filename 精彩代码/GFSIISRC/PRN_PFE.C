#include <string.h>
#include <stdlib.h>
#include <bio.inc>
#include "feedef.h"

/* function: print the notified phone fee
 * called by:
 * caller:
 * date:
 */
UC prt_phone_fee(PHONE_STRUCT *phone, UI num)
{
    UL	   p_no;
    double total_f;
    struct date now;

    if(check_prn() == FALSE)	    /* not ready */
	return(FALSE);

    if( num == 0 )
    {
	if(print_pfehd() == FALSE)
	    return(FALSE);

	if(print_pfetbl() == FALSE)
	    return(FALSE);

	if(print_pfedtal() == FALSE)
	    return(FALSE);
    }
    else if( (num != 0) && ( (num)%Page_len == 0) )
    {
	getdate(&now);

	if(print_pfetbl() == FALSE)
	    return(FALSE);
	if(prnf("汇总起止时间：%-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时  制表时间：%-4u年%2u月%2u日\n",\
		 Ps_time.year, Ps_time.month, Ps_time.day,  Ps_time.hour,\
		 Pe_time.year, Pe_time.month, Pe_time.day,  Pe_time.hour, \
		 now.da_year, now.da_mon, now.da_day )\
		 == FALSE)
	    return(FALSE);

	/* new page */
        if(new_page() == FALSE)
	    return(FALSE);

	if(print_pfehd() == FALSE)
	    return(FALSE);

	if(print_pfetbl() == FALSE)
	    return(FALSE);

	if(print_pfedtal() == FALSE)
	    return(FALSE);
    }

    p_no = atol(phone->phone_no);
    total_f = (double)(phone->local_charge)/100+\
	      ((double)(phone->nation_charge)+(double)(phone->intern_charge))/100+\
	      (double)(phone->addfee[4])/100+\
	      (double)(phone->month_lease)/100+\
	      (double)(phone->addfee[2])/100+\
	      ((double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3]))/100;

    if(prnf("%-4u  %-7lu  %7.2f %8.2f %4u  %6.2f  %6.2f  %6.2f %7.2f %9.2f\n", \
		  num, p_no, (double)(phone->local_charge)/100,\
		  (double)(phone->nation_charge+phone->intern_charge)/100,\
		  (phone->nation_count +phone->intern_count),\
		  (double)(phone->addfee[4])/100,\
		  ((double)phone->month_lease)/100,\
		  (double)(phone->addfee[2])/100,\
		  (double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3])/100,\
		  total_f) == FALSE)
       return(FALSE);

   return(TRUE);
}

UC print_pfehd(void)
{
   UC *head = "          分 机 话 费 汇 总 表";

   if(prnfd("\n%s\n\n", head) == FALSE)
       return(FALSE);

   return(TRUE);
}

UC print_pfetbl(void)
{
    if(prnf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")  == FALSE)
        return(FALSE);
    return(TRUE);
}


UC print_pfedtal(void)
{
	if(prnf("序    分机       市话      长话 长话    附加    月租    维修              费用\n") == FALSE)
	    return(FALSE);
	if(prnf("号    号码       话费      话费 次数      费      费      费    杂费      合计\n") == FALSE)
	    return(FALSE);
	if(print_pfetbl() == FALSE)
	    return(FALSE);

        return(TRUE);
}

/* function: print the notified user fee
 * called by:
 * caller:
 * date:
 */
UC prt_usr_fee(PHONE_STRUCT *phone, USERS_STRUCT usr, UI num)
{
    double total_f;
    struct date now;
    UI	   i;
    UC	   usr_nam_buf[11];

    if(check_prn() == FALSE)	    /* not ready */
	return(FALSE);

    if( num == 0 )
    {
	if(print_ufehd() == FALSE)
	    return(FALSE);
	if(print_pfetbl() == FALSE)
	    return(FALSE);

	if(print_ufedtal() == FALSE)
	    return(FALSE);
    }
    else if( (num != 0) && ( (num)%Page_len == 0) )
    {
	getdate(&now);

	if(print_pfetbl() == FALSE)
	    return(FALSE);
	if(prnf("汇总起止时间：%-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时  制表时间：%-4u年%2u月%2u日\n",\
		 Ps_time.year, Ps_time.month, Ps_time.day,  Ps_time.hour,\
		 Pe_time.year, Pe_time.month, Pe_time.day,  Pe_time.hour, \
		 now.da_year, now.da_mon, now.da_day )\
		 == FALSE)
	    return(FALSE);

	/* new page */
        if(new_page() == FALSE)
	    return(FALSE);

	if(print_ufehd() == FALSE)
	    return(FALSE);

	if(print_pfetbl() == FALSE)
	    return(FALSE);

	if(print_ufedtal() == FALSE)
	    return(FALSE);
    }

    for(i=0; i<8; i++)
	usr_nam_buf[i] = usr.user_name[i];
    usr_nam_buf[i] = '\0';
    total_f = (double)(phone->local_charge)/100+\
	      ((double)(phone->nation_charge)+(double)(phone->intern_charge))/100+\
	      (double)(phone->addfee[4])/100+\
	      (double)(phone->month_lease)/100+\
	      (double)(phone->addfee[2])/100+\
	      ((double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3]))/100;

    if(prnf("%-4u%-10s%2u %7.2f %8.2f %4u %7.2f %6.2f %6.2f %7.2f %9.2f\n", \
		  num, usr_nam_buf, usr.phones, (double)(phone->local_charge)/100,\
		  (double)(phone->nation_charge+phone->intern_charge)/100,\
		  (phone->nation_count +phone->intern_count),\
		  (double)(phone->addfee[4])/100,\
		  (double)(phone->month_lease)/100,\
		  (double)(phone->addfee[2])/100,\
		  (double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3])/100,\
		  total_f) == FALSE)
       return(FALSE);

   return(TRUE);
}

UC print_ufehd(void)
{
   UC *head = "          分 户 话 费 汇 总 表";

   if(prnfd("\n%s\n\n", head) == FALSE)
       return(FALSE);
   return(TRUE);
}

UC print_ufedtal(void)
{
	if(prnf("序           分机   市话     长话 长话    附加   月租   维修              费用\n") == FALSE)
	    return(FALSE);
	if(prnf("号  户头名   数量   话费     话费 次数      费     费     费    杂费      合计\n") == FALSE)
	    return(FALSE);
	if(print_pfetbl() == FALSE)
	    return(FALSE);

        return(TRUE);
}

/* function  : print the detailed information for the phones from the input one
 *	       to the end
 * called by : statistic()
 * date      : 1993.12.11
 */
void prtfee_part(UC opflg)
{
    UC	 flag, phoneno[10];
    UL	 phone_no;
    UI	 pnum=0, lenth, j, i, idx;
    double	   total_f;
    FILE	  *fp, *fpu;
    PHONE_STRUCT phoneu = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};
    USERS_STRUCT   user;
    PHONE_STRUCT  *phone;
    struct date    now;

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    if(check_prn() == FALSE)	    /* not ready */
	return;

    if(opflg == PHONE_FEE)
	flag = inp_phid_num(&phone_no, &lenth, PRINT, PHONE_NO);
    else if(opflg == DEPT_FEE)
	flag = inp_phid_num(&phone_no, &lenth, PRINT, DEPT_NO);
    else
	flag = inp_phid_num(&phone_no, &lenth, PRINT, AUTH_CD);

    if(flag == 1)
	return;

    load_phone_tbl();

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    if(opflg == PHONE_FEE)
    {
	Newlen = phone_tbl_len();
	i = 0;
	phone  = Phone_top;
	ltoa(phone_no, phoneno, 10);
	while(strcmp(phone->phone_no, phoneno) != 0)
	{
	    i++;
	    phone++;
	}

	idx = i;
	if( (idx+lenth) >= Newlen )
	     idx = Newlen;
	else
	     idx += lenth;

	j=0;
	total_f = 0;
	for(; i<idx; i++)
	{
	    if(prt_phone_fee(phone, j) == FALSE)
	    {
		unload_phone_tbl();
		message_end();
		return;
	    }
	    total_f += (double)(phone->local_charge)/100+\
		  (double)(phone->nation_charge+phone->intern_charge)/100+\
		  (double)(phone->addfee[4])/100+\
		  (double)(phone->month_lease)/100+\
		  (double)(phone->addfee[2])/100+\
		  (double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3])/100;
	    phone++;
	    j++;
	}
    }
    else if( opflg == DEPT_FEE)
    {
	fpu = fopen("gfsdata\\dept.dat","rb");
	if(fpu == NULL)
	{
	    unload_phone_tbl();
	    message(USER_FILE_ERR);
	    message_end();
	    return;
	}

	pnum = 0;
	while(fread(&user, sizeof(USERS_STRUCT), 1, fpu) == 1)
	{
	    if(user.user_no != (UI)phone_no)
		pnum += user.phones;
	    else
		break;
	}

	j=0;
	total_f = 0;
	for(i = 0; i < lenth; i++)
	{
	    clr_ufc(&phoneu, 0);
	    cal_usr_fee(pnum, &user, &phoneu);

	    if(prt_usr_fee(&phoneu, user, j) == FALSE)
	    {
	        fclose(fpu);
		message_end();
		unload_phone_tbl();
		return;
	    }

	    j++;
	    total_f += (double)(phoneu.local_charge)/100+\
		  (double)(phoneu.nation_charge+phoneu.intern_charge)/100+\
		  (double)(phoneu.addfee[4])/100+\
		  (double)(phoneu.month_lease)/100+\
		  (double)(phoneu.addfee[2])/100+\
		  (double)(phoneu.addfee[0]+phoneu.addfee[1]+phoneu.addfee[3])/100;
	    pnum += user.phones;
	    if(fread(&user, sizeof(USERS_STRUCT), 1, fpu) != 1) break;
	}

	fclose(fpu);
    }

    if(print_total(total_f) == FALSE)
    {
       unload_phone_tbl();
       message_end();
       return;
    }
    if(print_pfetbl()  == FALSE)
    {
	unload_phone_tbl();
	message_end();
	return;
    }
    getdate(&now);
    if(prnf("汇总起止时间：%-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时  制表时间：%-4u年%2u月%2u日\n",\
		 Ps_time.year, Ps_time.month, Ps_time.day,  Ps_time.hour,\
		 Pe_time.year, Pe_time.month, Pe_time.day,  Pe_time.hour, \
		 now.da_year, now.da_mon, now.da_day )\
		 == FALSE)
    {
	unload_phone_tbl();
	message_end();
	return;
    }
    /* new page */
    if(new_page() == FALSE)
    {
	unload_phone_tbl();
	message_end();
	return;
    }

    unload_phone_tbl();
    message_end();
    return;
}


/* function  : print the detailed information for all phones
 * called by : sta_rpt()
 * date      : 1993.12.11
 */
void prtfee_all(UC opflg)
{
    UI	 i, j;
    UI	 pnum=0;
    double	   total_f;
    FILE	  *fp, *fpu;
    PHONE_STRUCT  *phone;
    PHONE_STRUCT phoneu = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};
    USERS_STRUCT   user;
    struct date    now;

    if(check_prn() == FALSE)	    /* not ready */
	return;

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    load_phone_tbl();

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    if(opflg == PHONE_FEE)
    {
	Newlen = phone_tbl_len();
	phone  = Phone_top;

	total_f = 0;
	for(i=0; i<Newlen; i++)
	{
	    if(prt_phone_fee(phone, i) == FALSE)
	    {
		unload_phone_tbl();
		message_end();
		return;
	    }
	    total_f += (double)(phone->local_charge)/100+\
		  (double)(phone->nation_charge+phone->intern_charge)/100+\
		  (double)(phone->addfee[4])/100+\
		  (double)(phone->month_lease)/100+\
		  (double)(phone->addfee[2])/100+\
		  (double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3])/100;
	    phone++;
	}
    }
    else
    {
	fpu = fopen("gfsdata\\dept.dat","rb");
	if(fpu == NULL)
	{
	    unload_phone_tbl();
	    message(USER_FILE_ERR);
	    message_end();
	    return;
	}

	pnum = 0;
	j=0;
	total_f = 0;
	while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
	{
	    clr_ufc(&phoneu, 0);
	    cal_usr_fee(pnum, &user, &phoneu);

	    if(prt_usr_fee(&phoneu, user, j) == FALSE)
	    {
		message_end();
		unload_phone_tbl();
		return;
	    }

	    j++;
	    total_f += (double)(phoneu.local_charge)/100+\
		  (double)(phoneu.nation_charge+phoneu.intern_charge)/100+\
		  (double)(phoneu.addfee[4])/100+\
		  (double)(phoneu.month_lease)/100+\
		  (double)(phoneu.addfee[2])/100+\
		  (double)(phoneu.addfee[0]+phoneu.addfee[1]+phoneu.addfee[3])/100;
	    pnum += user.phones;
	}

	fclose(fpu);
    }
    if(print_total(total_f) == FALSE)
    {
       unload_phone_tbl();
       message_end();
       return;
    }
    if(print_pfetbl()  == FALSE)
    {
	unload_phone_tbl();
	message_end();
	return;
    }
    getdate(&now);
    if(prnf("汇总起止时间：%-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时  制表时间：%-4u年%2u月%2u日\n",\
		 Ps_time.year, Ps_time.month, Ps_time.day,  Ps_time.hour,\
		 Pe_time.year, Pe_time.month, Pe_time.day,  Pe_time.hour, \
		 now.da_year, now.da_mon, now.da_day )\
		 == FALSE)
    {
	unload_phone_tbl();
	message_end();
	return;
    }

    /* new page */
    if(new_page() == FALSE)
    {
	unload_phone_tbl();
	message_end();
	return;
    }

    unload_phone_tbl();
    message_end();
    return;
}

/* function:  invoice
 * called by:
 * caller:    none
 * date:      1994.12.5
 */
UC inv_one(PHONE_STRUCT *phone, UC *name, UC opflg1, UC opflg2, UC opflg3)
{
    FILE *fp;
    double tch;
    UI	   k;
    UL	  th[4], tm[4], ttm, tcu;
    UC *head[4] = { "      用   户   话   费   收   据",
	            "      分   机   话   费   汇   总",
	            "      分   户   话   费   汇   总",
	            "      授  权  户  话  费  汇  总"};
    struct date now;

    if(check_prn() == FALSE)	    /* not ready */
	return(FALSE);
    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return(FALSE);
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

	if(opflg3 != DETL)
	{
	    if(prnfd("\n%s\n\n", head[0])==FALSE)
		return(FALSE);
	}
	else
	{
	    if(opflg1 == PHONE_FEE)
	    {
		if(prnfd("\n%s\n\n", head[1])==FALSE)
		    return(FALSE);
	    }
	    else if(opflg1 == DEPT_FEE)
	    {
		if(prnfd("\n%s\n\n", head[2])==FALSE)
		    return(FALSE);
	    }
	    else
	    {
		if(prnfd("\n%s\n\n", head[3])==FALSE)
		    return(FALSE);
	    }
	}

    if(opflg1 == PHONE_FEE)
    {
	if(prnf(" 分机号码: %-s\n", phone->phone_no) == FALSE)
	    return(FALSE);
    }
    else if(opflg1 == DEPT_FEE)
    {
	if(prnf(" 户头名: %-s\n", name) == FALSE)
	     return(FALSE);
    }
    else  if(opflg1 == AUTH_FEE)
    {
	if(prnf(" 授权户名: %-s\n", name) == FALSE)
	     return(FALSE);
    }
    else
    {
	if(prnf(" 分机号码: %-s\n", name) == FALSE)
	     return(FALSE);
    }

    if(print_invtbl1() == FALSE)
	return(FALSE);
    if(print_invdtl1() == FALSE)
	return(FALSE);
    if(print_invtbl2() == FALSE)
	return(FALSE);

    tch = (double)(phone->intern_charge + phone->nation_charge +\
	   phone->local_charge)/100;

    if(  prnf("┃  话  费  │     %9.2f  │   %9.2f  │     %9.2f  │   %9.2f┃\n",\
	      (double)(phone->intern_charge)/100, (double)(phone->nation_charge)/100,\
	      (double)(phone->local_charge)/100,\
	      tch)==FALSE)
    {
	return(FALSE);
    }

    if(print_invtbl2() == FALSE)
	return(FALSE);

    ttm    = phone->intern_time;

    if( ttm%60 != 0 )
	tm[0]  = (ttm/60 + 1) % 60;
    else
	tm[0]  = (ttm/60) % 60;
    th[0]  = (ttm/60 + 1) / 60;

    ttm    = phone->nation_time;
    if( ttm%60 != 0 )
	tm[1]  = (ttm/60 + 1) % 60;
    else
	tm[1]  = (ttm/60) % 60;
    th[1]  = (ttm/60 + 1) / 60;

    ttm    = phone->local_time;
    if( ttm%60 != 0 )
	tm[2]  = (ttm/60 + 1) % 60;
    else
	tm[2]  = (ttm/60) % 60;
    th[2]  = (ttm/60 + 1) / 60;

    ttm = phone->intern_time + phone->nation_time + phone->local_time;
    if( ttm%60 != 0 )
	tm[3]  = (ttm/60 + 1) % 60;
    else
	tm[3]  = (ttm/60) % 60;
    th[3]  = (ttm/60 + 1) / 60;

    if(prnf("┃  时  长  │  %6lu时%2u分  │%6lu时%2u分  │  %6lu时%2u分  │%6lu时%2u分┃\n",\
	    th[0],(UI)tm[0],th[1],(UI)tm[1],th[2],(UI)tm[2],th[3],(UI)tm[3])==FALSE)
	return(FALSE);

    if(print_invtbl2() == FALSE)
	return(FALSE);

    tcu = (UL)(phone->intern_count + phone->nation_count +\
	  phone->local_count);
    if(prnf("┃  次  数  │    %10u  │  %10u  │    %10u  │  %10lu┃\n",\
	      phone->intern_count, phone->nation_count,\
	      phone->local_count,\
	      tcu)==FALSE)
	return(FALSE);

    if(print_invtbl3() == FALSE)
	return(FALSE);

    if(print_invdtl2() == FALSE)
	return(FALSE);

    if(print_invtbl4() == FALSE)
	return(FALSE);

    tch = (double)(phone->month_lease)/100+\
	  (double)(phone->addfee[0])/100+\
	  (double)(phone->addfee[1])/100+\
	  (double)(phone->addfee[2])/100+\
	  (double)(phone->addfee[3])/100+\
	  (double)(phone->addfee[4])/100;
    if(prnf("┃ %8.2f │ %7.2f│ %7.2f│ %7.2f│ %7.2f│ %8.2f │   %9.2f┃\n",\
	     (double)(phone->month_lease)/100,\
	     (double)(phone->addfee[0])/100,\
	     (double)(phone->addfee[1])/100,\
	     (double)(phone->addfee[2])/100,\
	     (double)(phone->addfee[3])/100,\
	     (double)(phone->addfee[4])/100,tch) == FALSE)
	return(FALSE);

    if(print_invtbl5() == FALSE)
	return(FALSE);

    if(opflg3 != DETL)
    {
        if(print_invdtl3() == FALSE)
	    return(FALSE);
    }
    else
    {
        if(print_invdtl4() == FALSE)
	    return(FALSE);
    }
        if(print_invtbl6() == FALSE)
	return(FALSE);

    tch  = (double)phone->intern_charge + (double)phone->nation_charge +\
	   (double)phone->local_charge;
    tch += (double)phone->month_lease;
    for(k=0; k<5; k++)
	tch += (double)phone->addfee[k];

    if(prnf("┃    人 民 币 元     │%-4u年%2u月%2u日%2u时│%-4u年%2u月%2u日%2u时  │   %9.2f┃\n",\
					  Ps_time.year, Ps_time.month,\
					  Ps_time.day,	Ps_time.hour,\
					  Pe_time.year, Pe_time.month,\
					  Pe_time.day,	Pe_time.hour, tch/100\
					  )==FALSE)
	return(FALSE);

    if(print_invtbl7() == FALSE)
	return(FALSE);

    getdate(&now);
    if(prnf(" 制表时间：%-4u年%-2u月%-2u日\n",now.da_year, now.da_mon, now.da_day ) == FALSE)
	return(FALSE);

    if(opflg2 == SINGLE)
    {
	/* new page */
        if(new_page() == FALSE)
	    return(FALSE);
    }
    else
    {
	if(prnf("\n\n\n\n") == FALSE)
	    return(FALSE);
    }
    return(TRUE);
}

UC print_invtbl1(void)
{
    if(prnf("┏━━━━━┯━━━━━━━━┯━━━━━━━┯━━━━━━━━┯━━━━━━┓\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invdtl1(void)
{
    if(prnf("┃  项  目  │  国 际 长 途   │ 国 内 长 途  │    市    话    │   合  计   ┃\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invtbl2(void)
{
    if(prnf("┠─────┼────────┼───────┼────────┼──────┨\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invtbl3(void)
{
    if(prnf("┣━━━━━┿━━━━┯━━━┷┯━━━━┯━┷━━┯━━━━━┿━━━━━━┫\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invdtl2(void)
{
    if(prnf("┃ 月 租 费 │新业务费│人工长途│维 修 费│其 它 费│ 附 加 费 │   合  计   ┃\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invtbl4(void)
{
    if(prnf("┠─────┼────┼────┼────┼────┼─────┼──────┨\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invtbl5(void)
{
    if(prnf("┣━━━━━┷━━━━┿━━━━┷━━━━┿━━━━┷━━━━━┿━━━━━━┫\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invdtl3(void)
{
    if(prnf("┃    结算货币单位    │   结算起始时间   │   结算终止时间     │   总  费   ┃\n")==FALSE)
        return(FALSE);
    return(TRUE);
}

UC print_invdtl4(void)
{
    if(prnf("┃    汇总货币单位    │   汇总起始时间   │   汇总终止时间     │   总  费   ┃\n")==FALSE)
        return(FALSE);
    return(TRUE);
}

UC print_invtbl6(void)
{
    if(prnf("┠──────────┼─────────┼──────────┼──────┨\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_invtbl7(void)
{
    if(prnf("┗━━━━━━━━━━┷━━━━━━━━━┷━━━━━━━━━━┷━━━━━━┛\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

/* function:  invoice
 * called by:
 * caller:    none
 * date:      1994.12.5
 */
UC htl_inv_one(PHONE_STRUCT *phone, UL phone_no, UC opflg1, UC opflg2)
{
    FILE  *fp;
    double tch;
    UL	   th[4], tm[4], ttm, tcu;
    TIME_STRUCT ps_time, pe_time;
    struct date now;
    UC *head = "       电  话  费  用  收  据";
    AUTHCD_STRUCT *authcd;

    if(check_prn() == FALSE)	    /* not ready */
	return(FALSE);

    fp = fopen("gfsdata\\acctime.dat","rb");
    if(fp == NULL)
    {
	message(ACCTIME_FILE_ERR);
	return(FALSE);
    }
    fread(&ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    if(prnfd("\n%s\n\n", head)==FALSE)
        return(FALSE);

    if(opflg1 == PHONE_FEE)
    {
	if(prnf(" 分机号码: %-7lu\n", phone_no) == FALSE)
	    return(FALSE);
    }
    else if(opflg1 == AUTH_CD)
    {
        authcd = binary_authcd(phone_no);          /* locate the phone No */
	if(prnf(" 授权用户: %-s\n", authcd->auth_usr_nam) == FALSE)
	    return(FALSE);
    }
    if(prnf("┏━━━━┯━━━━━┯━━━━━┯━━━━━┯━━━━━┯━━━━━┓\n")==FALSE)
	return(FALSE);

    if(prnf("┃ 项  目 │ 国际长途 │ 国内长途 │ 市内电话 │  附加费  │ 合    计 ┃\n")==FALSE)
	return(FALSE);

    if(print_invtbl8()==FALSE)
	return(FALSE);

    tch = (double)(phone->intern_charge + phone->nation_charge +\
	   phone->local_charge + phone->addfee[4])/100;

/*    if(  prnf("┃ 话  费 │%8.2f元│%8.2f元│%8.2f元│%8.2f元│%8.2f元┃\n",\
	      (double)(phone->intern_charge)/100, (double)(phone->nation_charge)/100,\
	      (double)(phone->local_charge)/100, (double)(phone->addfee[4])/100,\
	      tch)==FALSE)
*/
    if(  prnf("┃ 话  费 │%8.2f元│%8.2f元│%8.2f元│          │%8.2f元┃\n",\
	      (double)(phone->intern_charge)/100, (double)(phone->nation_charge)/100,\
	      (double)(phone->local_charge)/100, \
	      tch)==FALSE)
	return(FALSE);

    if(print_invtbl8()==FALSE)
	return(FALSE);

    ttm    = phone->intern_time;

    if( ttm%60 != 0 )
	tm[0]  = (ttm/60 + 1) % 60;
    else
	tm[0]  = (ttm/60) % 60;
    th[0]  = (ttm/60 + 1) / 60;

    ttm    = phone->nation_time;
    if( ttm%60 != 0 )
	tm[1]  = (ttm/60 + 1) % 60;
    else
	tm[1]  = (ttm/60) % 60;
    th[1]  = (ttm/60 + 1) / 60;

    ttm    = phone->local_time;
    if( ttm%60 != 0 )
	tm[2]  = (ttm/60 + 1) % 60;
    else
	tm[2]  = (ttm/60) % 60;
    th[2]  = (ttm/60 + 1) / 60;

    ttm = phone->intern_time + phone->nation_time + phone->local_time;
    if( ttm%60 != 0 )
	tm[3]  = (ttm/60 + 1) % 60;
    else
	tm[3]  = (ttm/60) % 60;
    th[3]  = (ttm/60 + 1) / 60;

    if(prnf("┃ 时  长 │%4lu时%2u分│%4lu时%2u分│%4lu时%2u分│          │%4lu时%2u分┃\n",\
	    th[0],(UI)tm[0],th[1],(UI)tm[1],th[2],(UI)tm[2],th[3],(UI)tm[3])==FALSE)
	return(FALSE);

    if(print_invtbl8()==FALSE)
	return(FALSE);
    tcu = (UL)(phone->intern_count + phone->nation_count +\
	  phone->local_count);

    if(prnf("┃ 次  数 │%10u│%10u│%10u│          │%10lu┃\n",\
	      phone->intern_count, phone->nation_count,\
	      phone->local_count,\
	      tcu)==FALSE)
	return(FALSE);

    if(prnf("┗━━━━┷━━━━━┷━━━━━┷━━━━━┷━━━━━┷━━━━━┛\n")==FALSE)
	return(FALSE);

    getdate(&now);
    if(prnf(" 结算时段：%-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时  制单：%-4u年%2u月%2u日\n",\
	 ps_time.year, ps_time.month, ps_time.day,  ps_time.hour,\
	 pe_time.year, pe_time.month, pe_time.day,  pe_time.hour, \
	 now.da_year, now.da_mon, now.da_day )\
	 == FALSE)
        return(FALSE);

    if(opflg2 == SINGLE)
    {
	/* new page */
        if(new_page() == FALSE)
	    return(FALSE);
    }
    else
    {
	if(prnf("\n\n\n\n") == FALSE)
	    return(FALSE);
    }
    return(TRUE);
}

UC print_invtbl8(void)
{
    if(prnf("┠────┼─────┼─────┼─────┼─────┼─────┨\n")==FALSE)
	return(FALSE);
    return(TRUE);
}

/* function:  print the doubled width DBCS
 * called by:
 * caller:    none
 * date:      1994.12.11
 */

UC set_dbl_char(void)
{
    if(prn_ch(0x1b) == FALSE)
	return(FALSE);
    if(prn_ch(0x21) == FALSE)
	return(FALSE);
    if(prn_ch(0xb0) == FALSE)
	return(FALSE);
    return(TRUE);
}

/* function:  print the normal width DBCS
 * called by:
 * caller:    none
 * date:      1994.12.11
 */

UC rst_nomal_char(void)
{
    if(prn_ch(0x1b) == FALSE)
	return(FALSE);
    if(prn_ch(0x21) == FALSE)
	return(FALSE);
    if(prn_ch(0x00) == FALSE)
	return(FALSE);
    return(TRUE);
}

/* function:  print the enclosed tabel with DBCS
 * called by:
 * caller:    none
 * date:      1994.12.11
 */

UC set_clos_tbl(void)
{
    if(prn_ch(0x1c) == FALSE)
	return(FALSE);
    if(prn_ch(0x76) == FALSE)
	return(FALSE);
    if(prn_ch(0x01) == FALSE)
	return(FALSE);
    return(TRUE);
}