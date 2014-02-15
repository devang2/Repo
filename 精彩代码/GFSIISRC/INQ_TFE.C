#include <dos.h>
#include <bios.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

#define BK_CLR    7
#define CHR_CLR   0
#define HEAD_CLR  0

FE_STAT_STRUCT Statsfee;
TIME_STRUCT    Ts_time, Te_time;

/* function  : calculating the total fee of PABX
 * date      : 1993.10.11
 */
void stats_fee(void)
{
    int    i, flag;
    FILE   *fp;
    PHONE_STRUCT  *phone;
    AUTHCD_STRUCT *authcd;
    FE_STAT_STRUCT statsfee={ 0,
			      0,
			      0,
			      0,
			      0,
			      0,
			      0,
			      0,
			      0,
			      0,
			      {0, 0, 0, 0, 0}
			    };

    load_phone_tbl();
    Newlen = phone_tbl_len();
    phone = Phone_top;

    message_disp(8,"正在统计，请稍候...");   /* statsing, please wait... */

    for(i=0; i<Newlen; i++)
    {
	statsfee.month_lease  += phone->month_lease;
	statsfee.addfee[0]    += phone->addfee[0];
	statsfee.addfee[1]    += phone->addfee[1];
	statsfee.addfee[2]    += phone->addfee[2];
	statsfee.addfee[3]    += phone->addfee[3];
	statsfee.addfee[4]    += phone->addfee[4];
	statsfee.local_charge += phone->local_charge;
	statsfee.local_time   += phone->local_time;
	statsfee.local_count  += phone->local_count;
	statsfee.intern_charge+= phone->intern_charge;
	statsfee.intern_time  += phone->intern_time;
	statsfee.intern_count += phone->intern_count;
	statsfee.nation_charge+= phone->nation_charge;
	statsfee.nation_time  += phone->nation_time;
	statsfee.nation_count += phone->nation_count;
	phone++;
    }
    unload_phone_tbl();

    flag = load_authcd_tbl(0);
    if(flag == TRUE)
    {
        ANewlen = authcd_tbl_len();
        authcd  = Auth_cd_top;

        for(i=0; i<ANewlen; i++)
        {
	    statsfee.month_lease  += authcd->month_lease;
	    statsfee.addfee[4]    += authcd->addfee;
	    statsfee.local_charge += authcd->local_charge;
	    statsfee.local_time   += authcd->local_time;
	    statsfee.local_count  += authcd->local_count;
	    statsfee.intern_charge+= authcd->intern_charge;
	    statsfee.intern_time  += authcd->intern_time;
	    statsfee.intern_count += authcd->intern_count;
	    statsfee.nation_charge+= authcd->nation_charge;
	    statsfee.nation_time  += authcd->nation_time;
	    statsfee.nation_count += authcd->nation_count;
	    authcd++;
        }
        unload_authcd_tbl();
    }

    fp = fopen("gfsdata\\totfee.dat","wb");
    fwrite(&statsfee,sizeof(FE_STAT_STRUCT),1,fp);
    fclose(fp);

    message_end();
    return;
}


/* function  : inquire total fee
 * called by : stat_prt()
 * date      : 1993.10.18
 */
void inq_total_fee(void)
{
    FILE *fp;
    UI    input;
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

    fp = fopen("gfsdata\\totfee.dat","rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return;
    }
    fread(&Statsfee, sizeof(FE_STAT_STRUCT), 1, fp);
    fclose(fp);

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return;
    }
    fread(&Ts_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Te_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);

    disp_tfee_head(TOTAL_FEE);      /* display table head   */

    draw_table(&tfee_tbl1);         /* draw the table frame */
    disp_tfee_detl1();

    draw_table(&tfee_tbl2);         /* draw the table frame */
    disp_tfee_detl2();

    draw_table(&tfee_tbl3);         /* draw the table frame */
    disp_tfee_detl3();

    message_disp(8,"Esc 终止 ");

    while(1)
    {
	input=get_key1();
	get_current(&cur_unit);
	switch(input)
	{
	    case ESC:	  /* QUIT */
		 recover_screen(2);
		 message_end();
		 return;

	    default:
		 sound_bell();
		 break;
	}
    }/* END OF WHILE */
}

/* function  : display the DBCS head for the total fee table
 * called by : inq_total_fee()
 * date      : 1993.9.22
 */
void disp_tfee_head(UC opflg)
{
     if(opflg == TOTAL_FEE)
	 outf(232,  78+5, BK_CLR, HEAD_CLR, "话  费  汇  总  报  告");
     else if(opflg == PHONE_FEE)
	 outf(232,  78+5, BK_CLR, HEAD_CLR, "分  机  话  费  报  告");
     else if(opflg == DEPT_FEE)
	 outf(232,  78+5, BK_CLR, HEAD_CLR, "分  户  话  费  报  告");
     else
	 outf(232,  78+5, BK_CLR, HEAD_CLR, "授 权 用 户 话 费 报 告");

	 outf(46,  116+10, BK_CLR, HEAD_CLR, "项  目");
	 outf(146, 116+10, BK_CLR, HEAD_CLR, "国际长途");
	 outf(281, 116+10, BK_CLR, HEAD_CLR, "国内长途");
	 outf(414, 116+10, BK_CLR, HEAD_CLR, "市    话");
	 outf(542, 116+10, BK_CLR, HEAD_CLR, "合  计");
	 outf(46,  162,    BK_CLR, HEAD_CLR, "话  费");
	 outf(46,  208-10, BK_CLR, HEAD_CLR, "时  长");
	 outf(46,  254-20, BK_CLR, HEAD_CLR, "次  数");
	 outf(46,  300-29, BK_CLR, HEAD_CLR, "月租费");
	 outf(119, 300-29, BK_CLR, HEAD_CLR, "新业务费");
	 outf(200, 300-29, BK_CLR, HEAD_CLR, "人工长途");
	 outf(288, 300-29, BK_CLR, HEAD_CLR, "维修费");
	 outf(370, 300-29, BK_CLR, HEAD_CLR, "其它费");
	 outf(451, 300-29, BK_CLR, HEAD_CLR, "附加费");
	 outf(542, 300-29, BK_CLR, HEAD_CLR, "合  计");

	 if(opflg == TOTAL_FEE)
	 {
	     outf(77-16,  395-49, BK_CLR, HEAD_CLR, "汇总货币单位");
	     outf(240-16, 395-49, BK_CLR, HEAD_CLR, "汇总起始时间");
	     outf(404-16, 395-49, BK_CLR, HEAD_CLR, "汇总终止时间");
	 }
	 else
	 {
	     outf(77-16,  395-49, BK_CLR, HEAD_CLR, "结算货币单位");
	     outf(240-16, 395-49, BK_CLR, HEAD_CLR, "结算起始时间");
	     outf(404-16, 395-49, BK_CLR, HEAD_CLR, "结算终止时间");
	 }
	 outf(542, 395-49, BK_CLR, HEAD_CLR, "总  费");
	 return;
}


/* function  : display the detail for the total fee table
 * called by : inq_total_fee()
 * date      : 1993.9.22
 */
void disp_tfee_detl1(void)
{
     char *traned;
     UI    i, j;
     UI    x, y;
     UL    th, tm, ttm, tcu;
     double tch;
     UNIT_STRUCT cer_unit;

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
		    traned=fdigt_tran((double)(Statsfee.intern_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
                    if( Statsfee.intern_time % 60 != 0)
		        tm = ((UL)(Statsfee.intern_time)/60 + 1) % 60;
                    else
	                tm = ((UL)(Statsfee.intern_time)/60) % 60;
		    th = ((UL)(Statsfee.intern_time)/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran((UL)Statsfee.intern_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 2:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(Statsfee.nation_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
                    if( Statsfee.nation_time % 60 != 0)
		        tm = ((UL)(Statsfee.nation_time)/60 + 1) % 60;
                    else
	                tm = ((UL)(Statsfee.nation_time)/60) % 60;
		    th = ((UL)(Statsfee.nation_time)/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran((UL)Statsfee.nation_count);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 3:
		 if(i == 0 )
		 {
		    traned=fdigt_tran((double)(Statsfee.local_charge)/100);
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
                    if( Statsfee.local_time % 60 != 0)
		        tm = ((UL)(Statsfee.local_time)/60 + 1) % 60;
                    else
	                tm = ((UL)(Statsfee.local_time)/60) % 60;
		    th = ((UL)Statsfee.local_time/60 + 1) / 60;
		    traned=ldigt_tran(th);
		    outf(x, y, BK_CLR, CHR_CLR, "%-sh%2um", traned, tm);
		 }
		 else
		 {
		    traned=ldigt_tran((UL)(Statsfee.local_count));
		    outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 break;
	    case 4:
		 if(i == 0 )
		 {
		    tch = ((double)(Statsfee.intern_charge + Statsfee.nation_charge +\
			   Statsfee.local_charge))/100;
		    traned=fdigt_tran(tch);
		    outf(x-19, y, BK_CLR, CHR_CLR, "%-s", traned);
		 }
		 else if(i == 1)
		 {
		    ttm = (UL)Statsfee.intern_time + (UL)Statsfee.nation_time +\
			  (UL)Statsfee.local_time;
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
		    tcu = (UL)(Statsfee.intern_count + Statsfee.nation_count +\
			  Statsfee.local_count);
		    traned=ldigt_tran(tcu);
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
void disp_tfee_detl2(void)
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
		 traned=fdigt_tran((double)(Statsfee.month_lease)/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5:
		 traned=fdigt_tran((double)(Statsfee.addfee[j-1])/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    case 6:
		 tch = Statsfee.month_lease;
		 for(k=0; k<5; k++)
		     tch += Statsfee.addfee[k];
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
void disp_tfee_detl3(void)
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
					  Ts_time.year, Ts_time.month,\
					  Ts_time.day,  Ts_time.hour);
		 break;
	    case 2:
		 outf(x,y,BK_CLR,CHR_CLR,"%-4u年%-2u月%-2u日%-2u时",\
					  Te_time.year, Te_time.month,\
					  Te_time.day,  Te_time.hour);
		 break;
	    case 3:
		 tch  = Statsfee.intern_charge + Statsfee.nation_charge +\
			Statsfee.local_charge;
		 tch += Statsfee.month_lease;
		 for(k=0; k<5; k++)
		     tch += Statsfee.addfee[k];
		 traned=fdigt_tran(tch/100);
		 outf(x, y, BK_CLR, CHR_CLR, "%-s", traned);
		 break;
	    default:
		 break;
	 }    /* END SWITCH */
     }
     return;
}


/* function:  change the floating point to character for commercial use
 * called by: tfee_disp_tbl
 * caller:    none
 * date:      1994.12.5
 */

char *fdigt_tran(double betran)
{
    char *tbuf;
    char  traned[20];
    int   i, sign, dec, ndig=11;

    if( betran == 0 )
    {
	traned[0]=0x30;
	traned[1]=0x00;
	return traned;
    }

    tbuf = fcvt(betran, ndig, &dec, &sign);
    if(dec <= 3)
    {
	for(i=0; i<dec; i++)
	    traned[i] = tbuf[i];
	traned[i]   = 0x2e;
	traned[i+1] = tbuf[i];
	traned[i+2] = tbuf[i+1];
	traned[i+3] = 0x00;
    }
    else if(dec <=6)
    {
	for(i=0; i<dec-3; i++)
	    traned[i] = tbuf[i];
	traned[i] = 0x2c;
	for(; i<dec; i++)
	    traned[i+1] = tbuf[i];
	traned[i+1]   = 0x2e;
	traned[i+2] = tbuf[i];
	traned[i+3] = tbuf[i+1];
	traned[i+4] = 0x00;
    }
    else if(dec <=9)
    {
	for(i=0; i<dec-6; i++)
	    traned[i] = tbuf[i];
	traned[i] = 0x2c;
	for(; i<dec-3; i++)
	    traned[i+1] = tbuf[i];
	traned[i+1] = 0x2c;
	for(; i<dec; i++)
	    traned[i+2] = tbuf[i];
	traned[i+2]   = 0x2e;
	traned[i+3] = tbuf[i];
	traned[i+4] = tbuf[i+1];
	traned[i+5] = 0x00;
    }
    else
	return NULL;

    return traned;
}

/* function:  change the long integer to character for commercial use
 * called by: tfee_disp_tbl
 * caller:    none
 * date:      1994.12.5
 */

char *ldigt_tran(UL betran)
{
    char tbuf[15];
    char traned[20];
    int  i, len;

    if( betran == 0 )
    {
	traned[0]=0x30;
	traned[1]=0x00;
	return traned;
    }

    ltoa(betran, tbuf, 10);
    len = strlen(tbuf);
    if(len <= 3)
    {
	for(i=0; i<len; i++)
	    traned[i] = tbuf[i];
	traned[i] = 0x00;
    }
    else if(len <= 6)
    {
	for(i=0; i<len-3; i++)
	    traned[i] = tbuf[i];
	traned[i] = 0x2c;
	for(; i<len; i++)
	    traned[i+1] = tbuf[i];
	traned[i+1] = 0x00;
    }
    else if(len <=9)
    {
	for(i=0; i<len-6; i++)
	    traned[i] = tbuf[i];
	traned[i] = 0x2c;
	for(; i<len-3; i++)
	    traned[i+1] = tbuf[i];
	traned[i+1] = 0x2c;
	for(; i<len; i++)
	    traned[i+2] = tbuf[i];
	traned[i+2] = 0x00;
    }
    else if(len <=12)
    {
	for(i=0; i<len-9; i++)
	    traned[i] = tbuf[i];
	traned[i] = 0x2c;
	for(; i<len-6; i++)
	    traned[i+1] = tbuf[i];
	traned[i+1] = 0x2c;
	for(; i<len-3; i++)
	    traned[i+2] = tbuf[i];
	traned[i+2] = 0x2c;
	for(; i<len-3; i++)
	    traned[i+3] = tbuf[i];
	traned[i+3] = 0x00;
    }
    else
	return NULL;

    return traned;
}


/* function:  print the total fee in some period
 * called by: inq_total_fee() and statsrics()
 * caller:    none
 * date:      1994.12.5
 */
void prt_total_fee(void)
{
    FILE *fp;
    UI    k;
    UL    th[4], tm[4], ttm, tcu;
    double tch;
    struct date now;
    UC *head = "         话  费  汇  总  报  表";

    if(check_prn() == FALSE)        /* not ready */
        return;
    fp = fopen("gfsdata\\totfee.dat","rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return;
    }
    fread(&Statsfee, sizeof(FE_STAT_STRUCT), 1, fp);
    fclose(fp);

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
	return;
    }
    fread(&Ts_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Te_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    message_disp(8,"正在打印，请稍候...");   /* printing */

	if(prnfd("\n%s\n\n", head)==FALSE)
	{
	    message_end();
                return;
	}

    if(print_invtbl1() == FALSE)
    {
	message_end();
	return;
    }

    if(print_invdtl1() == FALSE)
    {
	message_end();
	return;
    }

    if(print_invtbl2() == FALSE)
    {
	message_end();
	return;
    }

    tch = (double)(Statsfee.intern_charge + Statsfee.nation_charge +\
	   Statsfee.local_charge)/100;

    if(  prnf("┃  话  费  │     %9.2f  │   %9.2f  │     %9.2f  │   %9.2f┃\n",\
	      (double)(Statsfee.intern_charge)/100, (double)(Statsfee.nation_charge)/100,\
	      (double)(Statsfee.local_charge)/100,\
	      tch)==FALSE)
    {
	message_end();
	return;
    }

    if(print_invtbl2() == FALSE)
    {
	message_end();
	return;
    }

    ttm    = Statsfee.intern_time;
    if( ttm%60 != 0 )
        tm[0]  = (ttm/60 + 1) % 60;
    else
        tm[0]  = (ttm/60) % 60;
    th[0]  = (ttm/60 + 1) / 60;

    ttm    = Statsfee.nation_time;
    if( ttm%60 != 0 )
        tm[1]  = (ttm/60 + 1) % 60;
    else
        tm[1]  = (ttm/60) % 60;
    th[1]  = (ttm/60 + 1) / 60;

    ttm    = Statsfee.local_time;
    if( ttm%60 != 0 )
        tm[2]  = (ttm/60 + 1) % 60;
    else
        tm[2]  = (ttm/60) % 60;
    th[2]  = (ttm/60 + 1) / 60;

    ttm = Statsfee.intern_time + Statsfee.nation_time + Statsfee.local_time;
    if( ttm%60 != 0 )
        tm[3]  = (ttm/60 + 1) % 60;
    else
        tm[3]  = (ttm/60) % 60;
    th[3]  = (ttm/60 + 1) / 60;

    if(prnf("┃  时  长  │  %6lu时%2u分  │%6lu时%2u分  │  %6lu时%2u分  │%6lu时%2u分┃\n",\
	    th[0],(UI)tm[0],th[1],(UI)tm[1],th[2],(UI)tm[2],th[3],(UI)tm[3])==FALSE)
    {
	message_end();
	return;
    }

    if(print_invtbl2() == FALSE)
    {
	message_end();
	return;
    }
    tcu = Statsfee.intern_count + Statsfee.nation_count +\
	  Statsfee.local_count;
    if(prnf("┃  次  数  │    %10lu  │  %10lu  │    %10lu  │  %10lu┃\n",\
	      Statsfee.intern_count, Statsfee.nation_count,\
	      Statsfee.local_count,\
	      tcu)==FALSE)
    {
	message_end();
	return;
    }

    if(print_invtbl3() == FALSE)
    {
	message_end();
	return;
    }
    if(print_invdtl2() == FALSE)
    {
	message_end();
	return;
    }
    if(print_invtbl4() == FALSE)
    {
	message_end();
	return;
    }
    tch = (double)(Statsfee.month_lease)/100+\
	  (double)(Statsfee.addfee[0])/100+\
	  (double)(Statsfee.addfee[1])/100+\
	  (double)(Statsfee.addfee[2])/100+\
	  (double)(Statsfee.addfee[3])/100+\
	  (double)(Statsfee.addfee[4])/100;
    if(prnf("┃ %8.2f │%8.2f│%8.2f│%8.2f│%8.2f│ %8.2f │   %9.2f┃\n",\
	     (double)(Statsfee.month_lease)/100,\
	     (double)(Statsfee.addfee[0])/100,\
	     (double)(Statsfee.addfee[1])/100,\
	     (double)(Statsfee.addfee[2])/100,\
	     (double)(Statsfee.addfee[3])/100,\
	     (double)(Statsfee.addfee[4])/100,tch) == FALSE)
    {
	message_end();
	return;
    }
    if(print_invtbl5() == FALSE)
    {
	message_end();
	return;
    }
    if(print_invdtl4() == FALSE)
    {
	message_end();
	return;
    }
    if(print_invtbl6() == FALSE)
    {
	message_end();
	return;
    }
    tch  = (double)Statsfee.intern_charge + (double)Statsfee.nation_charge +\
	   (double)Statsfee.local_charge;
    tch += (double)Statsfee.month_lease;
    for(k=0; k<5; k++)
	tch += (double)Statsfee.addfee[k];
    if(prnf("┃    人 民 币 元     │%-4u年%2u月%2u日%2u时│%-4u年%2u月%2u日%2u时  │   %9.2f┃\n",\
					  Ts_time.year, Ts_time.month,\
					  Ts_time.day,  Ts_time.hour,\
					  Te_time.year, Te_time.month,\
					  Te_time.day,  Te_time.hour, tch/100\
					  )==FALSE)
    {
	message_end();
	return;
    }

    if(print_invtbl7() == FALSE)
    {
	message_end();
	return;
    }

    getdate(&now);

    if(prnf(" 制表时间：%-4u年%-2u月%-2u日\n",now.da_year, now.da_mon, now.da_day ) == FALSE)
	return;

	/* new page */
    if(new_page() == FALSE)
	return;

    message_end();
    return;
}

/*
			      话  费  汇  总  报  表

┏━━━━━┯━━━━━━━━┯━━━━━━━┯━━━━━━━━┳━━━━━━┓
┃  项  目  │  国 际 长 途   │ 国 内 长 途  │    市    话    ┃   合  计   ┃
┠─────┼────────┼───────┼────────╂──────┨
┃  话  费  │   %-9.2f   │  %-9.2f  │   %-9.2f   ┃%-9.2f ┃
┠─────┼────────┼───────┼────────╂──────┨
┃  时  长  │   %-6luh%2um   │  %-6luh%2um  │   %-6luh%2um   ┃%-6luh%2um ┃
┠─────┼────────┼───────┼────────╂──────┨
┃  次  数  │   %-10lu   │  %-10lu  │   %-10lu   ┃%-10lu  ┃
┠─────┼────┬───┴┬────┬─┴──┬─────╂──────┨
┃ 月 租 费 │新业务费│人工长途│维 修 费│其 它 费│ 附 加 费 ┃   合  计   ┃
┠─────┼────┼────┼────┼────┼─────╂──────┨
┃%-8.2f │%-7.2f│%-7.2f│%-7.2f│%-7.2f│%-8.2f ┃%-9.2f  ┃
┠─────┴────┼────┴────┼────┴─────╂──────┨
┃      单   位       │    起 始 时 间   │    终 止 时 间     ┃   总  费   ┃
┠──────────┼─────────┼──────────╂──────┨
┃    人 民 币 元     │%-4u年%2u月%2u日%2u时│%-4u年%2u月%2u日%2u时  ┃%-9.2f  ┃
┗━━━━━━━━━━┷━━━━━━━━━┷━━━━━━━━━━┻━━━━━━┛


			      话  费  汇  总  报  表

┏━━━━━┯━━━━━━━━┯━━━━━━━┯━━━━━━━━┳━━━━━━┓
┃  项  目  │  国 际 长 途   │ 国 内 长 途  │    市    话    ┃   合  计   ┃
┠─────┼────────┼───────┼────────╂──────┨
┃  话  费  │    000000.00   │   000000.00  │    000000.00   ┃ 0000000.00 ┃
┠─────┼────────┼───────┼────────╂──────┨
┃  时  长  │   000000h00m   │  000000h00m  │   000000h00m   ┃000000时00秒┃
┠─────┼────────┼───────┼────────╂──────┨
┃  次  数  │    000000000   │   000000000  │    000000000   ┃ 0000000000 ┃
┠─────┼────┬───┴┬────┬─┴──┬─────╂──────┨
┃ 月 租 费 │新业务费│人工长途│维 修 费│其 它 费│ 附 加 费 ┃   合  计   ┃
┠─────┼────┼────┼────┼────┼─────╂──────┨
┃000000.00 │00000.00│00000.00│00000.00│00000.00│000000.00 ┃ 0000000.00 ┃
┠─────┴────┼────┴────┼────┴─────╂──────┨
┃      单   位       │    起 始 时 间   │    终 止 时 间     ┃   总  费   ┃
┠──────────┼─────────┼──────────╂──────┨
┃    人 民 币 元     │0000--00--00--00--│ 0000--00--00--00-- ┃00000000.00 ┃
┗━━━━━━━━━━┷━━━━━━━━━┷━━━━━━━━━━┻━━━━━━┛


                        用 户 话 费 收 据
 分机号码: 0123456
┏━━━━┯━━━━━┯━━━━━┯━━━━━┯━━━━━┯━━━━━┓
┃ 项  目 │ 国际长途 │ 国内长途 │ 市内电话 │  服务费  │ 合    计 ┃
┠────┼─────┼─────┼─────┼─────┼─────┨
┃ 话  费 │01234.67元│01234.67元│01234.67元│01234.67元│01234.67元┃
┠────┼─────┼─────┼─────┼─────┼─────┨
┃ 时  长 │0123时01分│0123时01分│0123时01分│          │0123时01分┃
┠────┼─────┼─────┼─────┼─────┼─────┨
┃ 次  数 │0123456789│0123456789│0123456789│          │0123456789┃
┗━━━━┷━━━━━┷━━━━━┷━━━━━┷━━━━━┷━━━━━┛
 结算时段: 0123年01月01日01时至0123年01月01日01时   制单: 0123年01月01日
*/
