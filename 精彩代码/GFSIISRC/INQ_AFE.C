#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

#define BK_CLR	  7
#define CHR_CLR   0
#define HEAD_CLR  0

/* function  : print the detailed information for the authcds from the input one
 *	       to the end
 * called by : statistic()
 * date      : 1993.12.11
 */
void process_afee_all(UC opflg2)
{
    UI	 i;
    UC	 flag;
    AUTHCD_STRUCT  *authcd;

    if(check_prn() == FALSE)	    /* not ready */
	return;

    flag = load_authcd_tbl(0);
    if(flag == 0)		  /* charge.pho does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return;
    }

    ANewlen = authcd_tbl_len();
    authcd  = Auth_cd_top;

    message_disp(8,"正在打印, 请稍候...");

    for(i=0;i<ANewlen;i++)
    {
	if(inv_aone(authcd, MULTY, opflg2) == FALSE)
	{
 	    message_end();
	    unload_authcd_tbl();
            return;
	}
	authcd++;
    }

    message_end();
    unload_authcd_tbl();
    return;
}


/* function  : print the detailed information for the authcds from the input one
 *	       to the end
 * called by : statistic()
 * date      : 1993.12.11
 */
void process_afee_part(UC opflg1, UC opflg2)
{
    UI	 i, idx;
    UI	 lenth;
    UC	 flag;
    UL	 auth_cd;
    AUTHCD_STRUCT  *authcd;

    if(opflg1 == PRINT)
    {
	if(check_prn() == FALSE)	/* not ready */
	    return;
    }

    flag = inp_phid_num(&auth_cd, &lenth, opflg1, AUTH_CD);
    if(flag == 1)
	return;

    flag = load_authcd_tbl(0);
    if(flag == FALSE)		  /* charge.pho does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return;
    }

    ANewlen = authcd_tbl_len();
    i = 0;
    authcd  = Auth_cd_top;
    while(authcd->auth_code != auth_cd)
    {
	i++;
	authcd++;
    }

    idx = i;
    if( (idx+lenth) >= ANewlen )
	 idx = ANewlen;
    else
	 idx += lenth;

    if(opflg1 == PRINT)
	message_disp(8,"正在打印, 请稍候...");

    for(; i<idx; i++)
    {
	if(opflg1 == INQUIRE)
	{
	    if(inq_apart_fee(authcd, AUTH_FEE) == FALSE) break;
	}
	else
	{
	    if(inv_aone(authcd, MULTY, opflg2) == FALSE)
	    {
		 message_end();
		 unload_authcd_tbl();
		 return;
	    }
	}
	authcd++;
    }

    if(opflg1 == PRINT)
	message_end();

    unload_authcd_tbl();
    return;
}


/* function  :
 * called by : statistic()
 * date      : 1993.10.11
 */
void process_afee_one(UC opflg1, UC opflg2)
{
    UC	  flag;
    UL	  auth_cd;
    USERS_STRUCT nil={0, "", 0};
    AUTHCD_STRUCT  *authcd;

    if( opflg1 == PRINT)
    {
	if(check_prn() == FALSE)	/* not ready */
	    return;
    }

    flag = input_auth_cd(&auth_cd, opflg1);
    if(flag == 2)
	return;
    else if(flag == FALSE)	  /* the authcd no does not exist */
    {
	message(AUTHCD_NOT_EXIST);
	return;
    }

    flag = load_authcd_tbl(0);
    if(flag == FALSE)		  /* charge.pho does not exist */
    {
	message(AUTHCD_FILE_ERR);
	return;
    }

    ANewlen = authcd_tbl_len();

    authcd  = binary_authcd(auth_cd);
    if(authcd == NULL)
    {
	message(AUTHCD_NOT_EXIST);
	unload_authcd_tbl();
	return;
    }

    if(opflg1 == INQUIRE)
    {
	if(inq_apart_fee(authcd, AUTH_FEE) == TRUE)
            inquire_one_record(auth_cd, nil, ACCOUNT, AUTH_CD);
    }
    else
    {
	message_disp(8,"正在打印, 请稍候...");
	if( inv_aone(authcd, SINGLE, opflg2) == FALSE )
	{
	    message_end();
	    unload_authcd_tbl();
	    return;
	}
        message_end();
    }
    unload_authcd_tbl();
    return;
}



/* function  : inquire
 * called by :
 * date      : 1993.10.18
 */
UC inq_apart_fee(AUTHCD_STRUCT *authcd, UC opflg)
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
    disp_afee_detl1(authcd);

    draw_table(&tfee_tbl2);	    /* draw the table frame */
    disp_afee_detl2(authcd);

    draw_table(&tfee_tbl3);	    /* draw the table frame */
    disp_afee_detl3(authcd);

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
void disp_afee_detl1(AUTHCD_STRUCT *authcd)
{
     char *traned;
     UI    i, j;
     UI    x, y;
     UL    th, tm, ttm, tcu;
     double tch;
     UNIT_STRUCT cer_unit;

     outf(450,  78+16, BK_CLR, HEAD_CLR, "授权人名: %-s", authcd->auth_usr_nam);

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
		    traned=fdigt_tran((double)(authcd->intern_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    if(authcd->intern_time % 60 != 0)
			tm = (authcd->intern_time/60 + 1) % 60;
		    else
			tm = (authcd->intern_time/60) % 60;
		    th = (authcd->intern_time/60 +1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran( (UL)authcd->intern_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 2:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(authcd->nation_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    if(authcd->nation_time % 60 != 0)
			tm = (authcd->nation_time/60 + 1) % 60;
		    else
			tm = (authcd->nation_time/60) % 60;
		    th = (authcd->nation_time/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran( (UL)authcd->nation_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 3:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(authcd->local_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    if(authcd->local_time % 60 != 0)
			tm = (authcd->local_time/60 + 1) % 60;
		    else
			tm = (authcd->local_time/60) % 60;
		    th = (authcd->local_time/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran( (UL)authcd->local_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 4:
		 if(i == 0 )
		 {
		    tch = ((double)(authcd->intern_charge + authcd->nation_charge +\
			   authcd->local_charge))/100;
		    traned=fdigt_tran(tch);
		    outf(x-19, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    ttm = authcd->intern_time + authcd->nation_time +\
			  authcd->local_time;
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
		    tcu = (UL)authcd->intern_count + (UL)authcd->nation_count +\
			  (UL)authcd->local_count;
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
void disp_afee_detl2(AUTHCD_STRUCT *authcd)
{
     char *traned;
     UI    i, j;
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
		 traned=fdigt_tran((double)(authcd->month_lease)/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    case 5:
		 traned=fdigt_tran((double)(authcd->addfee)/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    case 6:
		 tch = authcd->month_lease;
		 tch += authcd->addfee;
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
void disp_afee_detl3(AUTHCD_STRUCT *authcd)
{
     char *traned;
     UI    i, j;
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
		 tch  = authcd->intern_charge + authcd->nation_charge +\
			authcd->local_charge;
		 tch += authcd->month_lease;
		 tch += authcd->addfee;
		 traned=fdigt_tran(tch/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    default:
		 break;
	 }    /* END SWITCH */
     }
     return;
}


/* function: print the notified authcd fee
 * called by:
 * caller:
 * date:
 */
UC prt_authcd_fee(AUTHCD_STRUCT *authcd, UI num)
{
    UI     i;
    double total_f, serv_fee = 0;
    UC	   aus_nam_buf[13];
    struct date now;

    if(check_prn() == FALSE)	    /* not ready */
	return(FALSE);

    if( num == 0 )
    {
	if(print_afehd() == FALSE)
	    return(FALSE);

	if(print_afetbl() == FALSE)
	    return(FALSE);
	if(print_afedtal() == FALSE)
	    return(FALSE);
    }
    else if( (num != 0) && ( (num)%Page_len == 0) /*&& Sys_mode.prt_m == LQ1600*/ )
    {
	getdate(&now);

	if(print_afetbl() == FALSE)
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

	if(print_afehd() == FALSE)
	    return(FALSE);

	if(print_afetbl() == FALSE)
	    return(FALSE);
	if(print_afedtal() == FALSE)
	    return(FALSE);
    }

    for(i=0; i<10; i++)
	aus_nam_buf[i] = authcd->auth_usr_nam[i];
    aus_nam_buf[i] = '\0';

    total_f = (double)(authcd->local_charge)/100+\
	      ((double)(authcd->nation_charge)+(double)(authcd->intern_charge))/100+\
	      (double)(authcd->addfee)/100+\
	      (double)(authcd->month_lease)/100;

    if(prnf("%-4u %-12s %7.2f %4u %8.2f %4u  %7.2f %6.2f %7.2f %9.2f\n",\
		  num, aus_nam_buf, (double)(authcd->local_charge)/100,\
                  authcd->local_count,\
		  (double)(authcd->nation_charge+authcd->intern_charge)/100,\
		  (authcd->nation_count + authcd->intern_count),\
		  (double)(authcd->addfee)/100,\
		  (double)(authcd->month_lease)/100,\
		  serv_fee, total_f) == FALSE)
        return(FALSE);

    return(TRUE);
}


UC print_afehd(void)
{
    UC *head = "      授 权 用 户 话 费 汇 总 表" ;

    if(prnfd("\n%s\n\n", head) == FALSE)
        return(FALSE);
    return(TRUE);
}

UC print_afetbl(void)
{
    if(prnf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")\
        == FALSE)
        return(FALSE);
    return(TRUE);
}

UC print_afedtal(void)
{
    if(prnf("     授权用户        市话 市话     长话 长话     附加   月租    服务\n")\
        == FALSE)
        return(FALSE);
    if(prnf("序号 户名称          话费 次数     话费 次数       费     费      费  费用合计\n")\
        == FALSE)
        return(FALSE);
    if(print_afetbl() == FALSE)
        return(FALSE);

    return(TRUE);
}



/* function  : print the detailed information for the authcds from the input one
 *	       to the end
 * called by : statistic()
 * date      : 1993.12.11
 */
void prtafee_part(void)
{
    UI	 i, idx;
    UI	 lenth, j;
    UC	 flag;
    UL	 auth_cd;
    double	   total_f;
    FILE	  *fp;
    AUTHCD_STRUCT *authcd;
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

    flag = inp_phid_num(&auth_cd, &lenth, PRINT, AUTH_CD);
    if(flag == 1)
	return;

    flag = load_authcd_tbl(0);
    if(flag == 0)		  /* charge.pho does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return;
    }

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    ANewlen = authcd_tbl_len();
    i = 0;

    authcd = binary_authcd(auth_cd);

    idx = i;
    if( (idx+lenth) >= ANewlen )
         idx = ANewlen;
    else
         idx += lenth;

    j=0;
    total_f = 0;
    for(; i<idx; i++)
    {
        if(prt_authcd_fee(authcd, j) == FALSE)
        {
	    unload_authcd_tbl();
	    message_end();
	    return;
	}
	total_f += (double)(authcd->local_charge)/100+\
	  (double)(authcd->nation_charge+authcd->intern_charge)/100+\
	  (double)(authcd->addfee)/100+\
	  (double)(authcd->month_lease)/100;
	authcd++;
	j++;
    }

    if( print_total( total_f ) == FALSE )
    {
       unload_authcd_tbl();
       message_end();
       return;
    }

    if(print_afetbl() == FALSE)
    {
	unload_authcd_tbl();
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
	unload_authcd_tbl();
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

    unload_authcd_tbl();
    message_end();
    return;
}


/* function  : print the detailed information for all authcds
 * called by : sta_rpt()
 * date      : 1993.12.11
 */
void prtafee_all(void)
{
    UI	 i;
    UC	 flag;
    double	   total_f;
    FILE	  *fp;
    AUTHCD_STRUCT  *authcd;
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

    flag = load_authcd_tbl(0);
    if(flag == 0)		  /* charge.pho does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return;
    }

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */
    ANewlen = authcd_tbl_len();
    authcd = Auth_cd_top;

    total_f = 0;
    for(i=0; i<ANewlen; i++)
    {
        if(prt_authcd_fee(authcd, i) == FALSE)
        {
	    unload_authcd_tbl();
	    message_end();
	    return;
	}
	total_f += (double)(authcd->local_charge)/100+\
		  (double)(authcd->nation_charge+authcd->intern_charge)/100+\
		  (double)(authcd->addfee)/100+\
		  (double)(authcd->month_lease)/100;
	authcd++;
    }
    if(print_total(total_f) == FALSE)
    {
       unload_authcd_tbl();
       message_end();
       return;
    }
    if(print_afetbl() == FALSE)
    {
	unload_authcd_tbl();
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
	unload_authcd_tbl();
	message_end();
	return;
    }

    /* new page */
    if(new_page() == FALSE)
    {
	unload_authcd_tbl();
	message_end();
	return;
    }

    unload_authcd_tbl();
    message_end();
    return;
}


/* function:  invoice
 * called by:
 * caller:    none
 * date:      1994.12.5
 */
UC inv_aone(AUTHCD_STRUCT *authcd, UC opflg2, UC opflg3)
{
    FILE *fp;
    double tch;
    UL	  th[4], tm[4], ttm, tcu;
    struct date now;
    UC *head[2] = {
	          "        用  户  话  费  收  据",
	          "        授  权  户  话  费  汇  总" };

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
	    if(prnfd("\n%s\n\n", head[1])==FALSE)
		return(FALSE);
	}

    if(prnf(" 授权户名: %-s\n", authcd->auth_usr_nam) == FALSE)
	return(FALSE);

    if(print_invtbl1() == FALSE)
	return(FALSE);

    if(print_invdtl1() == FALSE)
	return(FALSE);

    if(print_invtbl2() == FALSE)
	return(FALSE);

    tch = (double)(authcd->intern_charge + authcd->nation_charge +\
	   authcd->local_charge)/100;

    if(  prnf("┃  话  费  │     %9.2f  │   %9.2f  │     %9.2f  │   %9.2f┃\n",\
	      (double)(authcd->intern_charge)/100, (double)(authcd->nation_charge)/100,\
	      (double)(authcd->local_charge)/100,\
	      tch)==FALSE)
	return(FALSE);

    if(print_invtbl2() == FALSE)
	return(FALSE);

    ttm    = authcd->intern_time;

    if( ttm%60 != 0 )
	tm[0]  = (ttm/60 + 1) % 60;
    else
	tm[0]  = (ttm/60) % 60;
    th[0]  = (ttm/60 + 1) / 60;

    ttm    = authcd->nation_time;
    if( ttm%60 != 0 )
	tm[1]  = (ttm/60 + 1) % 60;
    else
	tm[1]  = (ttm/60) % 60;
    th[1]  = (ttm/60 + 1) / 60;

    ttm    = authcd->local_time;
    if( ttm%60 != 0 )
	tm[2]  = (ttm/60 + 1) % 60;
    else
	tm[2]  = (ttm/60) % 60;
    th[2]  = (ttm/60 + 1) / 60;

    ttm = authcd->intern_time + authcd->nation_time + authcd->local_time;
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

    tcu = (UL)(authcd->intern_count + authcd->nation_count +\
	  authcd->local_count);
    if(prnf("┃  次  数  │    %10u  │  %10u  │    %10u  │  %10lu┃\n",\
	      authcd->intern_count, authcd->nation_count,\
	      authcd->local_count, tcu)==FALSE)
	return(FALSE);

    if(print_invtbl3() == FALSE)
	return(FALSE);

    if(print_invdtl2() == FALSE)
	return(FALSE);

    if(print_invtbl4() == FALSE)
	return(FALSE);

    tch = (double)(authcd->month_lease)/100+\
	  (double)(authcd->addfee)/100;
    if(prnf("┃ %8.2f │        │        │        │        │ %8.2f │   %9.2f┃\n",\
	     (double)(authcd->month_lease)/100,\
	     (double)(authcd->addfee)/100,tch) == FALSE)
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

    tch  = (double)authcd->intern_charge + (double)authcd->nation_charge +\
	   (double)authcd->local_charge;
    tch += (double)authcd->month_lease;
    tch += (double)authcd->addfee;

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

UC print_total(double total_f)
{
    if(prnf("                                                         费用总计:   %9.2f\n", \
	     total_f) == FALSE)
        return FALSE;
    else
        return TRUE;
}