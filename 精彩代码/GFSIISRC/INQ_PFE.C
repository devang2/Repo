#include <dos.h>
#include <bios.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

#define BK_CLR	  7
#define CHR_CLR   0
#define HEAD_CLR  0

/* function  : print the detailed information for the phones from the input one
 *	       to the end
 * called by : statistic()
 * date      : 1993.12.11
 */
void process_fee_all(UC opflg2)
{
    UI	 i;
    PHONE_STRUCT  *phone;

    if(check_prn() == FALSE)	    /* not ready */
	return;

    load_phone_tbl();

    Newlen = phone_tbl_len();
    phone  = Phone_top;

    message_disp(8,"正在打印, 请稍候...");

    for(i=0;i<Newlen;i++)
    {
	if(inv_one(phone, phone->phone_no, PHONE_FEE, MULTY, opflg2) == FALSE)
	{
	    message_end();
	    unload_phone_tbl();
	    return;
	}
	phone++;
    }

    message_end();
    unload_phone_tbl();
    return;
}



/* function  : print the detailed information for the phones from the input one
 *	       to the end
 * called by : statistic()
 * date      : 1993.12.11
 */
void process_fee_part(UC opflg1, UC opflg2)
{
    UI	 i, idx;
    UI	 lenth;
    UC	 flag;
    UC	 phoneno[10];
    UL	 phone_no;
    PHONE_STRUCT  *phone;

    if(opflg1 == PRINT)
    {
	if(check_prn() == FALSE)	/* not ready */
	    return;
    }

    flag = inp_phid_num(&phone_no, &lenth, opflg1, PHONE_NO);
    if(flag == 1)
	return;

    load_phone_tbl();

    Newlen = phone_tbl_len();
    i = 0;
    phone  = Phone_top;

    ltoa(phone_no,phoneno,10);
    while(strcmp(phone->phone_no,phoneno) != 0)
    {
	i++;
	phone++;
    }

    idx = i;
    if( (idx+lenth) >= Newlen )
	 idx = Newlen;
    else
	 idx += lenth;

    if(opflg1 == PRINT)
	message_disp(8,"正在打印, 请稍候...");

    for(; i<idx; i++)
    {
	if(opflg1 == INQUIRE)
	{
	    if(inq_part_fee(phone, phoneno, PHONE_FEE) == FALSE) break;
	}
	else
	{
	     if(inv_one(phone, phoneno, PHONE_FEE, MULTY, opflg2) == FALSE)
	     {
		 message_end();
		 unload_phone_tbl();
		 return;
	     }
	}
	phone++;
    }

    if(opflg1 == PRINT)
	message_end();

    unload_phone_tbl();
    return;
}


/* function  :
 * called by : statistic()
 * date      : 1993.10.11
 */
void process_fee_one(UC opflg1, UC opflg2)
{
    UC	  flag, phoneno[10];
    UL	  phone_no;
    USERS_STRUCT nil = {0, "", 0};
    PHONE_STRUCT  *phone;

    if(opflg1 == PRINT)
    {
	if(check_prn() == FALSE)	/* not ready */
	    return;
    }

    flag = input_phone_no(&phone_no, opflg1);
    if(flag == 2)
	return;
    else if(flag == FALSE)	  /* the phone no does not exist */
    {
	message(NO_NOT_EXIST);
	return;
    }

    load_phone_tbl();
    Newlen = phone_tbl_len();

    ltoa(phone_no,phoneno,10);
    phone  = binary_search(phoneno);
    if(phone == NULL)
    {
	message(NO_NOT_EXIST);
	unload_phone_tbl();
	return;
    }

    if(opflg1 == INQUIRE)
    {
        if(inq_part_fee(phone, phoneno, PHONE_FEE) == TRUE)
            inquire_one_record(phone_no, nil, ACCOUNT, PHONE_NO);
    }
    else
    {
	message_disp(8,"正在打印, 请稍候...");
/*	if(Usr_typ == NORMAL_USR || opflg2 == DETL)
	{
	    if( inv_one(phone, phoneno, PHONE_FEE, SINGLE, opflg2) == FALSE )
            {
		message_end();
		unload_phone_tbl();
		return;
	    }
	}
	else
	{
	    if( htl_inv_one(phone, phone_no, PHONE_FEE, SINGLE) == FALSE )
	    {
		message_end();
		unload_phone_tbl();
		return;
	    }
	}
*/

	    if( inv_one(phone, phoneno, PHONE_FEE, SINGLE, opflg2) == FALSE )
            {
		message_end();
		unload_phone_tbl();
		return;
	    }

        message_end();
    }
    unload_phone_tbl();
    return;
}


/* function  : inquire
 * called by :
 * date      : 1993.10.18
 */
UC inq_part_fee(PHONE_STRUCT *phone, UC *name, UC opflg)
{
    FILE *fp;
    UI	  input;
    UNIT_STRUCT cur_unit;

    TABLE_STRUCT  tfee_tbl1 = { 28, 115, 35, 35, 3, 5,
				{80, 135, 135, 135, 96},
				GREEN
			      };
    TABLE_STRUCT  tfee_tbl2 = { 28, 261, 35, 35, 1, 7,
				{80, 81, 80, 81, 80, 81, 96},
				GREEN
			      };
    TABLE_STRUCT  tfee_tbl3 = { 28, 335, 35, 35, 1, 4,
				{162, 162, 162, 96},
				GREEN
			      };

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return(FALSE);
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_tfee_head(opflg);	/* display table head	*/
    draw_table(&tfee_tbl1);	    /* draw the table frame */
    disp_pfee_detl1(phone, name, opflg);

    draw_table(&tfee_tbl2);	    /* draw the table frame */
    disp_pfee_detl2(phone);

    draw_table(&tfee_tbl3);	    /* draw the table frame */
    disp_pfee_detl3(phone);

    message_disp(8,"Enter 继续   Esc 终止");

    while(1)
    {
	input=get_key1();
	get_current(&cur_unit);
	switch(input)
	{
	    case ESC:	  /* QUIT */
		 recover_screen(2);
		 message_end();
		 return(FALSE);

	    case ENTER:   /* CONTINUE */
		 recover_screen(2);
		 message_end();
		 return(TRUE);
	    default:
		 sound_bell();
		 break;
	}
    }/* END OF WHILE */
}


/* function  : display the detail for the total fee table
 * called by : inq_total_fee()
 * date      : 1993.9.22
 */
void disp_pfee_detl1(PHONE_STRUCT *phone, UC *name, UC opflg)
{
     char *traned;
     UI    i, j;
     UI    x, y;
     UL    th, tm, ttm, tcu;
     double tch;
     UNIT_STRUCT cer_unit;

     if(opflg == PHONE_FEE)
	 outf(480,  78+16, BK_CLR, HEAD_CLR, "分机号码: %-s", phone->phone_no);
     else if(opflg == DEPT_FEE)
	 outf(450,  78+16, BK_CLR, HEAD_CLR, "户头名: %-s", name);
     else if(opflg == AUTH_FEE)
	 outf(450,  78+16, BK_CLR, HEAD_CLR, "授权人名: %-s", name);
     else
	 outf(480,  78+16, BK_CLR, HEAD_CLR, "分机号码: %-s", name);

     for(i=0; i<3; i++)
     {
       for(j=1; j<5; j++)
       {
	 cer_unit.unit_x = i;
	 cer_unit.unit_y = j;
	 get_certain(&cer_unit);
	 x = cer_unit.dot_sx+27;
	 y = cer_unit.dot_sy+9;

	 switch(j)
	 {
	    case 1:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(phone->intern_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    if(phone->intern_time % 60 != 0)
			tm = (phone->intern_time/60 + 1) % 60;
		    else
			tm = (phone->intern_time/60) % 60;
		    th = (phone->intern_time/60 +1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran( (UL)phone->intern_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 2:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(phone->nation_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    if(phone->nation_time % 60 != 0)
			tm = (phone->nation_time/60 + 1) % 60;
		    else
			tm = (phone->nation_time/60) % 60;
		    th = (phone->nation_time/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran( (UL)phone->nation_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 3:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(phone->local_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    if(phone->local_time % 60 != 0)
			tm = (phone->local_time/60 + 1) % 60;
		    else
			tm = (phone->local_time/60) % 60;
		    th = (phone->local_time/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran( (UL)phone->local_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 4:
		 if(i == 0 )
		 {
		    tch = ((double)(phone->intern_charge + phone->nation_charge +\
			   phone->local_charge))/100;
		    traned=fdigt_tran(tch);
		    outf(x-19, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    ttm = phone->intern_time + phone->nation_time +\
			  phone->local_time;
		    if(ttm % 60 != 0)
		       tm = (ttm/60 + 1) % 60;
		    else
		       tm = (ttm/60) % 60;
		    th = (ttm/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x-19, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    tcu = (UL)phone->intern_count + (UL)phone->nation_count +\
			  (UL)phone->local_count;
		    traned=ldigt_tran( tcu);
		    outf(x-19, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    default:
		 break;
	 }    /* END SWITCH */
       }
     }
     return;
}


/* function  : display the detail for the total fee table
 * called by : inq_total_fee()
 * date      : 1993.9.22
 */
void disp_pfee_detl2(PHONE_STRUCT *phone)
{
     char *traned;
     UI    i, j, k;
     UI    x, y;
     double tch;
     UNIT_STRUCT cer_unit;

     i=0;
     for(j=0; j<7; j++)
     {
	 cer_unit.unit_x = i;
	 cer_unit.unit_y = j;
	 get_certain(&cer_unit);
	 x = cer_unit.dot_sx+8;
	 y = cer_unit.dot_sy+9;

	 switch(j)
	 {
	    case 0:
		 traned=fdigt_tran((double)(phone->month_lease)/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
		 traned=fdigt_tran((double)(phone->addfee[j-1])/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    case 6:
		 tch = phone->month_lease;
		 for(k=0; k<5; k++)
		     tch += phone->addfee[k];
		 traned=fdigt_tran(tch/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    default:
		 break;
	 }    /* END SWITCH */
     }
     return;
}


/* function  : display the detail for the total fee table
 * called by : inq_total_fee()
 * date      : 1993.9.22
 */
void disp_pfee_detl3(PHONE_STRUCT *phone)
{
     char *traned;
     UI    i, j, k;
     UI    x, y;
     double tch;
     UNIT_STRUCT cer_unit;

     i=0;
     for(j=0; j<4; j++)
     {
	 cer_unit.unit_x = i;
	 cer_unit.unit_y = j;
	 get_certain(&cer_unit);
	 x = cer_unit.dot_sx+8;
	 y = cer_unit.dot_sy+9;

	 switch(j)
	 {
	    case 0:
		 outf(77, y, BK_CLR, CHR_CLR, "人民币元");
		 break;
	    case 1:
		 outf(x,y,BK_CLR,CHR_CLR,"%-4u年%-2u月%-2u日%-2u时",\
					  Ps_time.year, Ps_time.month,\
					  Ps_time.day,	Ps_time.hour);
		 break;
	    case 2:
		 outf(x,y,BK_CLR,CHR_CLR,"%-4u年%-2u月%-2u日%-2u时",\
					  Pe_time.year, Pe_time.month,\
					  Pe_time.day,	Pe_time.hour);
		 break;
	    case 3:
		 tch  = phone->intern_charge + phone->nation_charge +\
			phone->local_charge;
		 tch += phone->month_lease;
		 for(k=0; k<5; k++)
		     tch += phone->addfee[k];
		 traned=fdigt_tran(tch/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    default:
		 break;
	 }    /* END SWITCH */
     }
     return;
}

/* function:  invoice
 * called by:
 * caller:    none
 * date:      1994.12.5
 */
void htl_dsp_one(PHONE_STRUCT *phone, UL phone_no, UC isauthcd)
{
    double tch;
    UL	   th[4], tm[4], ttm, tcu;
    FILE  *fp;
    struct date now;
    TABLE_STRUCT  hfee_tbl = { 40, 154, 32, 31, 3, 6,
			       {78, 95, 95, 95, 95, 95},
			       GREEN
			     };
    TIME_STRUCT ps_time, pe_time;
    AUTHCD_STRUCT *authcd;

    fp = fopen("gfsdata\\acctime.dat","rb");
    if(fp == NULL)
    {
	message(ACCTIME_FILE_ERR);
	return;
    }
    fread(&ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);


    if(isauthcd == PHONE_FEE)
    {
        outf(32, 100, 7, 0, "                     分  机  电  话  费  用  清  单");
        outf(40, 132, 7, 0, "分机号码: %-7lu", phone_no);
    }
    else
    {
        outf(32, 100, 7, 0, "                     授  权  用  户  费  用  清  单");
        authcd = binary_authcd(phone_no);          /* locate the phone No */
        outf(40, 132, 7, 0, "授权用户: %-s", authcd->auth_usr_nam);
    }

    outf(32, 164, 7, 0, "   项  目    国际长途    国内长途    市内电话   附加服务费   合    计   ");
    tch = (double)(phone->intern_charge + phone->nation_charge +\
	      phone->local_charge + phone->addfee[4])/100;
    outf(32, 196, 7, 0, "   话  费   %8.2f元  %8.2f元  %8.2f元  %8.2f元 %9.2f元  ",\
	      (double)(phone->intern_charge)/100, (double)(phone->nation_charge)/100,\
	      (double)(phone->local_charge)/100, (double)(phone->addfee[4])/100,\
	      tch);

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

    outf(32, 228, 7, 0, "   时  长   %4lu时%2u分  %4lu时%2u分  %4lu时%2u分              %4lu时%2u分  ",\
	    th[0],(UI)tm[0],th[1],(UI)tm[1],th[2],(UI)tm[2],th[3],(UI)tm[3]);
    tcu = (UL)(phone->intern_count + phone->nation_count +\
	  phone->local_count);

    outf(32, 260, 7, 0, "   次  数   %10u  %10u  %10u              %10u  ",\
	      phone->intern_count, phone->nation_count,\
	      phone->local_count, tcu);
    getdate(&now);

    outf(32, 292, 7, 0, " 结算时段: %-4u年%2u月%2u日%2u时～%-4u年%2u月%2u日%2u时  制单: %-4u年%2u月%2u日",\
	 ps_time.year, ps_time.month, ps_time.day,  ps_time.hour,\
	 pe_time.year, pe_time.month, pe_time.day,  pe_time.hour, \
	 now.da_year,  now.da_mon,    now.da_day );

    draw_table(&hfee_tbl);	    /* draw the table frame */
    return;

}

