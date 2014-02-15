#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <bios.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : show the telephone number whose cash pledge is overflow
 * called by : all_record_transfer()
 * input	 : opflg -- INQUIRE or print
 *             mode  -- authcd cash or phone cash
 *             send  -- 0: no send    1: send
 *             tout  -- 0: no timeout 1: has time out
 * date      : 1993.10.18
 */
void overflow_cash(UC opflg, UC mode, UC send, UC tout)
{
    FILE *fp;
    UI    position=0;
    UI    key;
    UC    flag = 0, dflag = FALSE;
    TABLE_STRUCT cash_tbl = {185,104,20,18,16,5,{48,72,72,72,72},GREEN};
    CASH_PLEDGE  cashe;
    struct date now;
    PHONE_STRUCT *phone;
    UI auth_no;
    UC tmp[20], err, port;
    UL GoalTick;

    if(opflg != INQUIRE)
	if(check_prn() == FALSE)        /* not ready */
	    return;

    if(mode == AUTH_CASH)
	load_authcd_tbl(0);

    if(mode == PHONE_CASH)
	load_phone_tbl();

    fp = fopen(CashDataFile[mode],"rb");
    if(fp != NULL)
    {
	if(opflg == INQUIRE)
	{
	    dflag = TRUE;
	    clr_DialWin(1);
	    draw_table(&cash_tbl);
	    disp_over_head(mode);
	    message_disp(8,"Enter 继续   Esc 终止");        /* press any key */
	}
	else
	    message_disp(8,"正在打印, 请稍候...");        /* printing */
	while(fread(&cashe,sizeof(CASH_PLEDGE),1,fp) == 1)
	{
	    if( (cashe.cash >= cashe.limit ) || (cashe.flag == CASH_OFF) ) /* not overflow */
		continue;

	    if(opflg == INQUIRE)
	    {
		disp_over_cash(position,&cashe);
		if(mode == PHONE_CASH)
                {
		    phone = binary_search(cashe.phone_no);
		    if(phone != NULL && !(phone->class == 1 && phone->set_flag == 0))
		    {
			  phone->class = 1;
			  phone->set_flag = 1;
			  sprintf(tmp, "%02d%02d%02d", phone->code%PhonesPerPort, (int)(phone->class),(int)(phone->max_min));
			  if(send && DownloadCode(tmp, 0, 6, 0xC0, phone->code/PhonesPerPort) == 0)
				 phone->set_flag = 0;
		     }
		}
		else /* AUTH_CASH */
		{
		    auth_no = atoi(cashe.phone_no);
		    if(   Auth_cd_top[auth_no].auth_class != 0 || Auth_cd_top[auth_no].auth_set_flag == 1)
		    {
			Auth_cd_top[auth_no].auth_class = 0;
				Auth_cd_top[auth_no].auth_set_flag = 1;
			if (send )
			{
			      sprintf(tmp, "099999%04d00", Auth_cd_top[auth_no].auth_code);
			      err = 0;
			      for(port=0; port<PORT_NUM; port++)
				if(Sys_mode.com_m[port])
				    if(DownloadCode(tmp, 6, 6, 0xBA, port) != 0) err=1;
					    if(!err)
						    Auth_cd_top[auth_no].auth_set_flag = 0;
			}
		    }
		}
	    }
	    else if(prt_over_cash(position, &cashe) == FALSE)
		break;

	    flag=TRUE;      /* have overcashed phone */

	    position++;
	    if( (position%16==0) && (!EndofCashFile(fp)) )
	    {
		if(opflg == INQUIRE)
		{
		    key = get_key1();
		    if(key == ESC)
		    {
			fclose(fp);
			recover_screen(1);
			message_end();
			if(mode == AUTH_CASH)
			    unload_authcd_tbl();
			if(mode == PHONE_CASH)
			    unload_phone_tbl();
			return;
		    }
		    else if(key == ENTER)
			clr_over_cash();
		}
	    }
	}          /* end of "while(!feof(fp))" */

	if( (opflg == PRINT) && (flag ==TRUE) )
	{
	    getdate(&now);

	    if(print_ocstbl() == FALSE)
	    {
		message_end();
		if(mode == AUTH_CASH)
		    unload_authcd_tbl();
		if(mode == PHONE_CASH)
		    unload_phone_tbl();
		return;
	    }
	    if(prnf("                         制表时间：%-4u年%2u月%2u日\n",\
		now.da_year, now.da_mon, now.da_day )\
		== FALSE)
	    {
		message_end();
		if(mode == AUTH_CASH)
		    unload_authcd_tbl();
		if(mode == PHONE_CASH)
		    unload_phone_tbl();
		return;
	    }

	    /* new page */
	    if(new_page() == FALSE)
	    {
		message_end();
		if(mode == AUTH_CASH)
		    unload_authcd_tbl();
		if(mode == PHONE_CASH)
		    unload_phone_tbl();
		return;
	    }
	}
	fclose(fp);
	message_end();
    }         /* end of "if(fp != NULL)"        */
    else
	message(NULL_CASH_ERR);

    if( opflg == INQUIRE && dflag == TRUE)
    {
       if(tout)
       {
	   GoalTick = *Tick_cnt_ptr + 18.2*30; /*30s */
	   while(!_bios_keybrd(_KEYBRD_READY) && ( GoalTick > *Tick_cnt_ptr) )
	       check_event_flag();

	   if(GoalTick > *Tick_cnt_ptr)
	       _bios_keybrd(_KEYBRD_READ); /* key pressed */
       }
       else
	   get_key1();

       recover_screen(1);
    }

    if(mode == AUTH_CASH)
    {
	save_authcd_tbl(NOFRESH, 0);
	unload_authcd_tbl();
    }
    if(mode == PHONE_CASH)
    {
	save_phone_tbl(NOFRESH, 0);
	unload_phone_tbl();
    }

    return;
}


/* function  : display the head Hanzi for the overflow cash table
 * called by : take_all_record()
 * date      : 1993.10.18
 */
void disp_over_head(UC mode)
{
	 outf(290, 80, 7,  0, "透支及超限报告");     /* No.         */

	 outf(194, 108, 7, 0, "序号");     /* No.         */
         if(mode == PHONE_CASH)
	     outf(239, 108, 7, 0, "分机号码"); /* phone No    */
         else
	     outf(239, 108, 7, 0, "授权卡号"); /* phone No    */
	 outf(312, 108, 7, 0, "押金状态"); /* cash now    */
	 outf(385, 108, 7, 0, "押金现额"); /* cash now    */
	 outf(458, 108, 7, 0, "押金下限"); /* cash limit  */
	 return;
}


/* function  : display the cash-overflow phones
 * called by : overflow_cash()
 * input     : pos   -- position in the table
 *             cashe -- cash pledge to be displayed
 * date      : 1993.10.18
 */
void disp_over_cash(UI pos,CASH_PLEDGE *cashe)
{
    int  j;
    UI   x,y;
    UNIT_STRUCT  cur_unit;

    cur_unit.unit_x = pos%16;
    for(j=0;j<5;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+8;
	y = cur_unit.dot_sy+1;

	switch(j)
	{
	    case 0:
		outf(x,y,7,0,"%-u",pos+1);
		break;
	    case 1:
		outf(x,y,7,0,"%-s",cashe->phone_no);
		break;
	    case 2:    /* cash switch */
		if(cashe->cash < 0)
		    outf(x+12, y, 7, LIGHTRED, "透支");
		else
		    outf(x+12, y, 7, YELLOW,   "超限");
		break;
	    case 3:
		outf(x,y,7,0,"%7.2f", (double)(cashe->cash)/100);
		break;
	    case 4:
		outf(x,y,7,0,"%7.2f", (double)(cashe->limit)/100);
		break;
	}     /* end of "switch(j)"        */
    }         /* end of "for(j=0;j<4;j++)" */

    return;
}

/* function  : clear the inquired record table
 * called by : inquire_one_record(),inquire_all_record()
 * date      : 1993.10.5
 */
void clr_over_cash(void)
{
    UI i, j;
    UI xs, ys, xe, ye;
    UNIT_STRUCT cur_unit;

    for(i=0;i<16;i++)
	for(j=0;j<5;j++)
	{
	   cur_unit.unit_x = i;
	   cur_unit.unit_y = j;
	   get_certain(&cur_unit);
	   xs = cur_unit.dot_sx;
	   ys = cur_unit.dot_sy;
	   xe = cur_unit.dot_ex;
	   ye = cur_unit.dot_ey;
	   setfillstyle(1, 7);
	   bar(xs, ys, xe, ye);
	}

    return;
}

/* function  : judge whether is end of cash.dat or not
 * called by : disp_over_cash()
 * input     : fp -- the file pointer to cash.dat
 * date      : 1993.10.18
 * note      : why we edit this module is that, the lib function
 *             "feof()" sometimes gets error, or to say, not very accurate,
 *             if we use it, we often read one more item from the file than
 *             the real items in the file
 */
UC  EndofCashFile(FILE *fp)
{
    UC     flag;
    fpos_t filepos;
    CASH_PLEDGE temp;

    fgetpos(fp, &filepos);
    if(fread(&temp,sizeof(CASH_PLEDGE),1,fp) != 1)   /* End of File*/
	flag = TRUE;
    else
	flag = FALSE;
    fsetpos(fp, &filepos);

    return(flag);
}


/* function:  printing the overcashed phone user
 * called by:
 * caller:
 * date:
 */
UC prt_over_cash(UI pos, CASH_PLEDGE *cashe)
{
    UL     phoneno;
    struct date now;
    UC *head ="   透支及超限分机清单";

    if( pos == 0 )
    {
        if(prnfd("\n%s\n\n", head) == FALSE)
            return(FALSE);
	if(print_ocstbl() == FALSE)
	    return(FALSE);
	if(print_ocsdtl() == FALSE)
	    return(FALSE);
	if(print_ocstbl() == FALSE)
	    return(FALSE);
    }
    else if( (pos != 0) && ( (pos)%Page_len == 0) )
    {
	getdate(&now);

	if(print_ocstbl() == FALSE)
	    return(FALSE);
	if(prnf("                         制表时间：%-4u年%2u月%2u日\n",\
	    now.da_year, now.da_mon, now.da_day ) == FALSE)
	    return(FALSE);

	/* new page */
        if(new_page() == FALSE)
	    return(FALSE);

	if(prnf("\n%s\n\n", head) == FALSE)
	    return(FALSE);
	if(print_ocstbl() == FALSE)
	    return(FALSE);
	if(print_ocsdtl() == FALSE)
	    return(FALSE);
	if(print_ocstbl() == FALSE)
	    return(FALSE);
    }
    phoneno = atol(cashe->phone_no);

    if(cashe->cash < 0)
    {
	if(prnf("%-4u    %-7lu     透支      %9.2f  %7.2f\n", pos, phoneno,\
	   (double)(cashe->cash)/100, (double)(cashe->limit)/100 )== FALSE)
	   return(FALSE);
    }
    else
    {
	if(prnf("%-4u    %-7lu     超限      %9.2f  %7.2f\n", pos, phoneno,\
	   (double)(cashe->cash)/100, (double)(cashe->limit)/100 )== FALSE)
	   return(FALSE);
    }

    return(TRUE);
}

UC print_ocstbl(void)
{
    if(prnf("─────────────────────────\n") == FALSE)
        return(FALSE);
    return(TRUE);
}

UC print_ocsdtl(void)
{
    if(prnf(" 序号   分机号码   押金状态   押金现额   押金下限\n") == FALSE)
        return(FALSE);
    return(TRUE);
}



/*
	   透支及潜在透支分机报表
─────────────────────────
 序号   分机号码   信用状态   押金现额   押金下限
 0123    0123456     0123      0123456    0123456
─────────────────────────

*/

