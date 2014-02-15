#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include <alloc.h>
#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

void InnerToExt(void);

/* function  : calculate InterNational call charge
 * called by : all_record_transfer(),imm_record_transfer()
 * input     : len -- the number length of callee (including the ending fd)
 *             in_fp -- file pointer to Intern.tmp
 * output    : write charge to Intern.tmp
 * date      : 1993.9.28
 */
void Intern_charge(void)
{
    UC   flag;
    UL   second, secc;
	 RATE_STRUCT huge *rate_tmp;

    rate_tmp = get_rate(&temp_list.callee[2],IDD);   /* get charge rate */
    if(rate_tmp == NULL)              /* the area No. does not exist    */
    {
	rate.first_rate = 0;
	rate.first_time = 60;
	rate.formal_rate = 0;
	rate.formal_time = 60;
	rate.add_fee = 0;
    }
    else
	rate = *rate_tmp;

    if(rate.formal_time == 0)
		  rate.formal_time = 60;

    flag = normal_charge_hour();            /* calculate calling time   */
    if(flag == TRUE)
    {
        second = (UL)hour*60*60+min*60+sec;
        if(second >= 43200L) /* calling time is longer than 12 hours */
            second = 180;
    }
    else                        /* calling time is too long */
		  second = 180;

	 temp_list.time = second;    /* calling time */

    second += Intern_add.add_time;
    temp_list.charge = rate.first_rate;     /* calling charge*/
    if(second > rate.first_time)
    {
        secc = (second-rate.first_time)/rate.formal_time;
        if( (second-rate.first_time-secc*rate.formal_time) > 0)
            secc++;
        temp_list.charge += secc*rate.formal_rate;
	 }

	 if( ( strcmp("852", rate.area_no) == 0 ) || \
        ( strcmp("853", rate.area_no) == 0 ) )
    {
        temp_list.add = (UL) (temp_list.charge*Honkon_add.add_percent/100  \
                         +Honkon_add.add_rate*((second-1)/rate.formal_time + 1)\
			 +Honkon_add.add_record + rate.add_fee);
    }
    else
    {
	temp_list.add = (UL) (temp_list.charge*Intern_add.add_percent/100  \
								 +Intern_add.add_rate*((second-1)/rate.formal_time + 1)\
			 +Intern_add.add_record + rate.add_fee);
	 }

//    temp_list.add = ((temp_list.add+9)/10)*10;

    temp_list.charge += temp_list.add;

//    temp_list.charge = ((temp_list.charge+9)/10)*10;

    temp_list.flag = TRUE;

	 return;
}

/* function  : calculate National call charge
 * called by : all_record_transfer(),imm_record_transfer()
 * input     : len -- the number length of callee (including the ending fd)
 *             na_fp -- file pointer to Nation.tmp
 * output    : write charge to Nation.tmp
 * date      : 1993.9.28
 */
void Nation_charge(void)
{
    UC   flag;
    UL   second,sec0,semi_sec0,secc;
    RATE_STRUCT huge *rate_tmp;

    rate_tmp = get_rate(&temp_list.callee[1],DDD);   /* get charge rate */
    if(rate_tmp == NULL)              /* the area No. does not exist    */
    {
        rate.first_rate = 0;
        rate.first_time = 60;
        rate.formal_rate = 0;
        rate.formal_time = 60;
    }
	 else
        rate = *rate_tmp;

    strcpy(Distr_name, rate.area_name);
    if(rate.formal_time == 0)
        rate.formal_time = 60;

    flag = nation_charge_hour();            /* calculate calling time   */
    if(flag == TRUE)
    {
        sec0 = (UL)hour*60*60+min*60+sec;
        semi_sec0 = (UL)semi_hour*60*60+semi_min*60+semi_sec;
		  second = sec0+semi_sec0;
        if(second >= 43200L) /* calling time is longer than 12 hours */
		  {
            second = 180;
            sec0   = 180;
            semi_sec0 = 0;
        }
    }
    else                        /* calling time is too long */
    {
        second = 180;
        sec0   = 180;
		  semi_sec0 = 0;
    }

    temp_list.time = second;

    second += Nation_add.add_time;
    sec0   += Nation_add.add_time;
    temp_list.charge = rate.first_rate;

    if(second > rate.first_time)
    {
        secc = (second-rate.first_time)/rate.formal_time;
		  if((second-rate.first_time-secc*rate.formal_time) > 0)
            secc++;
		  temp_list.charge += secc*rate.formal_rate;
    }

    if(second < 1)
        second = 1;                       /* for if second=0 &&&*/
    temp_list.charge = temp_list.charge/2.0*((double)semi_sec0/second)    \
                      +temp_list.charge*((double)sec0/second);

    temp_list.add = (UL)(temp_list.charge*Nation_add.add_percent/100  \
			+Nation_add.add_rate*((second-1)/rate.formal_time + 1)\
			+Nation_add.add_record + rate.add_fee);

//    temp_list.add = ((temp_list.add+9)/10)*10;

    temp_list.charge += temp_list.add;

//    temp_list.charge = ((temp_list.charge+9)/10)*10;

    temp_list.flag = TRUE;

	 return;
}

/* function  : calculate Local call charge
 * called by : all_record_transfer(),imm_record_transfer()
 * input     : len -- the number length of callee (including the ending fd)
 *             lo_fp -- file pointer to loacl.tmp
 * output    : write charge to Local.tmp
 * date      : 1993.9.28
 */
void Local_charge(void)
{
	 UC   flag;
    UL   second,secc;
	 RATE_STRUCT huge *rate_tmp;

    rate_tmp = get_rate(temp_list.callee,LDD);       /* get charge rate */
    if(rate_tmp == NULL)              /* the area No. does not exist    */
    {
        rate.first_rate = 0;
        rate.first_time = 60;
        rate.formal_rate = 0;
        rate.formal_time = 60;
    }
	 else
        rate = *rate_tmp;

    if(rate.formal_time == 0)
        rate.formal_time = 60;

    flag = normal_charge_hour();            /* calculate calling time   */
    if(flag == TRUE)
    {
        second = (UL)hour*60*60+min*60+sec;
        if(second >= 43200L) /* calling time is longer than 12 hours */
            second = 180;
	 }
    else                        /* calling time is too long */
		  second = 180;

    temp_list.time = second;    /* calling time */

    second += Local_add.add_time;
    temp_list.charge = rate.first_rate;
    if(second > rate.first_time)
    {
        secc = (second-rate.first_time)/rate.formal_time;
        if((second-rate.first_time-secc*rate.formal_time) > 0)
				secc++;
        temp_list.charge += secc*rate.formal_rate;
	 }

    if(temp_list.charge == 0)
        temp_list.add = 0;
    else
        temp_list.add = (UL)(temp_list.charge*Local_add.add_percent/100 \
                         +Local_add.add_rate*((second-1)/rate.formal_time + 1)\
					     +Local_add.add_record + rate.add_fee);

//    temp_list.add = ((temp_list.add+9)/10)*10;

    temp_list.charge += temp_list.add;

//    temp_list.charge = ((temp_list.charge+9)/10)*10;

    temp_list.flag = TRUE;

    return;
}

/* function  : transfer telephone records to Intern.tmp, Nation.tmp and
 *             Local.tmp respectively, from pabx.dat
 * called by : take_all()
 * date      : 1993.9.28
 */
void all_record_transfer(void)
{
    int   i, k;
    UC    caller[6], mark, flag, w_flag = 0;
    FILE  *in_fp, *na_fp, *lo_fp, *pabx_fp;
    UC     port;
    UC    mode;
    UC    user[10];
    PHONE_STRUCT  *phone;
    UI    SpecLen[2];

    flag  = load_rate_tab();
    if(flag != TRUE)              /* *.rat does not exist    */
    {
	message(RATE_FILE_ERR);
	return;
    }

    for(mode =0; mode<2; mode++)
	if(Cashflg[mode])
	{
	    flag = load_cash_tbl(mode);
	    if(flag != 1)             /* cash data does not exist */
		 Cashflg[mode] = 0;
	    else
		Cash_len[mode] = cash_tbl_len(mode);
	}

    load_special_tbl(0);  /* local call definition table */
    load_special_tbl(1);  /* information call definition table */
    SpecLen[0] = special_tbl_len(0);  /* local */
    SpecLen[1] = special_tbl_len(1);  /* information */

    load_phone_tbl();
    Newlen = phone_tbl_len();

    in_fp = fopen("idd.tmp","ab");
    na_fp = fopen("ddd.tmp","ab");
    lo_fp = fopen("ldd.tmp","ab");

    pabx_fp = fopen("pabx.chg","rb");
    if(pabx_fp != NULL )     /* the file exists */
    {
	while(fread(caller,sizeof(UC),1,pabx_fp) == 1)
	{
	    w_flag = 0;      /*  Reset the error flag */

	    port = caller[0];

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.year2 = ((caller[0] & 0xF0)>>4)*10+(caller[0] & 0x0F);
	    temp_list.year2 += 1900;
	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.mon2 = cbcd_uc(caller);
	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.day2 = cbcd_uc(caller);

	    temp_list.year1 = temp_list.year2;
	    temp_list.mon1 = temp_list.mon2;
	    temp_list.day1 = temp_list.day2;

	    fread(caller, sizeof(UC), 1, pabx_fp);
	    temp_list.caller2 = ((caller[0] & 0xF0)>>4)*10+(caller[0] & 0x0F)+PhonesPerPort*port;
	    InnerToExt();
	    temp_list.caller1 = temp_list.caller2;

	    fread(caller,sizeof(UC), 3, pabx_fp);
	    cbcd_hex(&temp_list.auth_code, caller, 6);

	    if(caller[0] != 0x0a )
		cbcd_hex(&temp_list.auth_code, caller, 6);
	    else if(caller[0] == 0x0a )
		temp_list.auth_code = 0xaaaaaa;

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.hour1 = cbcd_uc(caller);

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.min1 = cbcd_uc(caller);

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.sec1 = cbcd_uc(caller);

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.hour2 = cbcd_uc(caller);

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.min2 = cbcd_uc(caller);

	    fread(caller,sizeof(UC),1,pabx_fp);
	    temp_list.sec2 = cbcd_uc(caller);

	    /* in caller number, if it is National long call, then the
	     * first 0 is saved as A, if it is an InterNation long call,
	     * then save the first two 0 as AA. The 0s in the tail are
	     * saved as 0s
	     */
	    for(i=0, k=0; i<7; i++)       /* callee number */
	    {
		fread(caller,1,1,pabx_fp);
		if( i == 0 && (caller[0] & 0xf0) == 0)
		{
		    w_flag = 1;    /*  Set the error flag */
		    continue;
		}

                if((caller[0] & 0xf0) == 0)      /* 0 in the tail */
                    break;
                else
					 {
                    temp_list.callee[k] = (caller[0] & 0xf0)>>4;
						  if( temp_list.callee[k] > 0x0a)
                    {
                        w_flag = 1;    /*  Set the error flag */
			continue;
                    }

                    if(temp_list.callee[k] == 0x0a)   /* 0 in the head */
			temp_list.callee[k] = 0;
                    k++;

		    if((caller[0] & 0x0f) == 0)
			break;
		    else     /* if((caller[0] & 0x0f) != 0) */
		    {
			temp_list.callee[k] = caller[0] & 0x0f;

			if( temp_list.callee[k] > 0x0a)
			{
			    w_flag = 1;  /*  Set the error flag */
			    continue;
			}

			if(temp_list.callee[k] == 0x0a)
			    temp_list.callee[k] = 0;

			k++;
			if(k >= 14)    /* 14 bytes are enough for callee number */
			    break;
                    }   /* end of "if((caller[0] & 0x0f) != 0)" */
                }       /* end of "else if((caller[0] & 0xf0) != 0)" */
            }      /* end of "for(i=0,k=0;i<8;i++)" */

	    if(w_flag == 1)
            {
					 for(;;)
                {
						  fread(caller,sizeof(UC),1,pabx_fp);
                    if(caller[0] == 0xfd)           /* end of a phone record */
                        break;
		}
                continue;
            }

	    if(k > 14)
                k = 14;
	    for(;k<16;k++)
					 temp_list.callee[k] = 0xfd;

				if(temp_list.callee[0] == 0)
            {
                if(temp_list.callee[1] == 0)
		    mark = IDD;             /* InterNational */
		else
                    mark = DDD;             /* National      */
	    }
	    else
		mark = LDD;                 /* Local         */

	    for(; ;)
	    {
					 fread(caller,sizeof(UC),1,pabx_fp);
		if(caller[0] == 0xfd)       /* end of a phone record */
		    break;
	    }

	    if(delay_proc(mark) == FALSE)
		continue;

//          if((temp_list.caller1==0) && (temp_list.caller2==0))
//              continue;

	    just_date();

	    /* compute week day of day1 and day2 */
	    week1 = weekday(temp_list.year1,temp_list.mon1,temp_list.day1);
	    week2 = weekday(temp_list.year2,temp_list.mon2,temp_list.day2);

	    if(mark==IDD)
	    {
		Intern_charge();
		fwrite(&temp_list,sizeof(RECORD_STRUCT),1,in_fp);

					 /* handle cash in pledge */
		if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
		    update_cash(AUTH_CASH);
		else
		{
		    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
		    phone = binary_search(user);         /* locate the phone No */
		    if( (phone != NULL) && phone->feeflag[4])  	/* IDD fee */
			update_cash(PHONE_CASH);
		}
	    }
	    else if(mark==DDD)
	    {
		Nation_charge();
		fwrite(&temp_list,sizeof(RECORD_STRUCT),1,na_fp);
		/* handle cash in pledge */
		if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
		    update_cash(AUTH_CASH);
		else
		{
		    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
		    phone = binary_search(user);         /* locate the phone No */
		    if( (phone != NULL) && phone->feeflag[3])  	/* DDD fee */
			update_cash(PHONE_CASH);
		 }

	    }
	    else if(mark==LDD)
	    {
		Local_charge();
		fwrite(&temp_list,sizeof(RECORD_STRUCT),1,lo_fp);
		/* handle cash in pledge */
		if(Usr_typ == NORMAL_USR || Ldd_fflg == YES)
		    if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
			update_cash(AUTH_CASH);
		    else
						  {
			ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
								phone = binary_search(user);         /* locate the phone No */
			if( phone != NULL)
			{
			    UC f1, f2;

			    f1 = IsSpecialCall(temp_list.callee,SpecLen[0],0);
			    f2 = IsSpecialCall(temp_list.callee,SpecLen[1],1);

			    if(   (phone->feeflag[1] && f1)
				||(phone->feeflag[2] && f2)
										  ||(phone->feeflag[0] && !(f1 && f2) ))
				update_cash(PHONE_CASH);
							 }
		    }
	    }
	}     /* end of "while(!feof(pabx_fp))" */
	fclose(pabx_fp);
    }         /* end of "if(pabx_fp != NULL )"  */

    fclose(in_fp);
    fclose(na_fp);
    fclose(lo_fp);

    unload_special_tbl(0);
    unload_special_tbl(1);

    for(mode=0; mode<2; mode++)
	if(Cashflg[mode])
	{
	    save_cash_tbl(mode);
	    unload_cash_tbl(mode);
	}

    unload_rate_tab();
    unload_phone_tbl();

    return;
}

/* function  : transfer telephone records to Intern.tmp, Nation.tmp and
 *             Local.tmp respectively, for immediate method
 * called by : take_immediate_record()
 * date      : 1993.9.28
 */
UC imm_record_transfer(UC port)
{
   int   i, k, mark;
   UC    flag, phone_no[10];
   char  auth_name[9];
   char *a_name="    ";
   RATE_STRUCT huge *rate_tmp;
   PHONE_STRUCT *phone;
   UC    user[10];
   UI    SpecLen[2];
   UC    f1, f2;
   UC    tmp[20], err;

   FILE *in_fp, *na_fp, *lo_fp;

   SpecLen[0] = special_tbl_len(0);  /* local */
   SpecLen[1] = special_tbl_len(1);  /* information */

    temp_list.year1 = cbcd_uc(CmdBuf[port]+1);
    temp_list.year1 += 1900;
    temp_list.mon1  = cbcd_uc(CmdBuf[port]+2);
    temp_list.day1  = cbcd_uc(CmdBuf[port]+3);

    temp_list.year2 = temp_list.year1;
    temp_list.mon2  = temp_list.mon1;
    temp_list.day2  = temp_list.day1;

    if(CmdBuf[port][5] != 0x0a) {
	for(i=5;i<8;i++) {
	  if( (CmdBuf[port][i]&0xF0) == 0xa0) CmdBuf[port][i] = CmdBuf[port][i] & 0x0F;
	  if( (CmdBuf[port][i]&0x0F) == 0x0a) CmdBuf[port][i] = CmdBuf[port][i] & 0xF0;
		  }
	cbcd_hex(&temp_list.auth_code, CmdBuf[port]+5, 6);
	 }
    else if(CmdBuf[port][5] == 0x0a)
		  temp_list.auth_code = 0xaaaaaa;

/*    cbcd_hex(&temp_list.caller1, CmdBuf[port]+4, 2);*/
    cbcd_hex(&temp_list.caller2, CmdBuf[port]+4, 2);
    temp_list.caller2 += port*PhonesPerPort;
    InnerToExt();
    temp_list.caller1 =   temp_list.caller2;

    for(k = 0, i = 14; i < 21; i ++)
    {
	if( (CmdBuf[port][i] & 0xf0) == 0)
	    break;
	else
	{
	    temp_list.callee[k] = (CmdBuf[port][i] & 0xf0) >> 4;

	  if( temp_list.callee[k] > 0x0a)
	      return(FALSE);

	    if(temp_list.callee[k] == 0x0a)
					 temp_list.callee[k] = 0;
	    k ++;

	    if((CmdBuf[port][i] & 0x0f) == 0)
					 break;
	    else                  /* if((CmdBuf[i] & 0x0f) != 0) */
	    {
		temp_list.callee[k] = CmdBuf[port][i] & 0x0f;

	      if( temp_list.callee[k] > 0x0a)
		  return(FALSE);

	      if(temp_list.callee[k] == 0x0a)
		    temp_list.callee[k] = 0;
					 k++;

	      if(k >= 14)       /* 14 bytes are enough for callee number */
		    break;
	    }                     /* end of "if((CmdBuf[i] & 0x0f) != 0)" */
	}                         /* end of "else if((CmdBuf[i] & 0xf0) != 0)" */
    }                             /* end of "for(k=0,i=11;i<19;i++)" */

    if(k > 14)
	k = 14;
	 for(; k<16; k++)
	temp_list.callee[k] = 0xfd;

    if(temp_list.callee[0] == 0)
    {
	if(temp_list.callee[1] == 0)   /* InterNational */
	    mark = IDD;
	else
	    mark = DDD;                                                         /* National */
    }
    else
	mark = LDD;

    temp_list.hour1 = cbcd_uc(CmdBuf[port]+8);
    temp_list.min1  = cbcd_uc(CmdBuf[port]+9);
    temp_list.sec1  = cbcd_uc(CmdBuf[port]+10);

    temp_list.hour2 = cbcd_uc(CmdBuf[port]+11);
    temp_list.min2  = cbcd_uc(CmdBuf[port]+12);
    temp_list.sec2  = cbcd_uc(CmdBuf[port]+13);

    if(delay_proc(mark) == FALSE)
	return(FALSE);

    just_date();

    /* compute week day of day1 and day2 */
    week1 = weekday(temp_list.year1,temp_list.mon1,temp_list.day1);
    week2 = weekday(temp_list.year2,temp_list.mon2,temp_list.day2);

    if(mark==IDD)
    {
	Intern_charge();
	in_fp = fopen("idd.tmp","ab");
	fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, in_fp);
	fclose(in_fp);

	/* handle cash in pledge */
	if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
	    update_cash(AUTH_CASH);
	else
	{
	    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
	    phone = binary_search(user);         /* locate the phone No */
	    if( (phone != NULL) && phone->feeflag[4])  	/* IDD fee */
		update_cash(PHONE_CASH);
	}

	rate_tmp = get_rate(&temp_list.callee[2],IDD);   /* get charge rate */
	if(rate_tmp == NULL)              /* the area No. does not exist    */
		strcpy(rate1.area_name, a_name);
	else
		rate1 =*rate_tmp;

	if(Intern_prn && check_prn())        /* printer is ready */
	{
	    ltoa( temp_list.caller2, phone_no, 10 );
	    phone = binary_search( phone_no );
	    if( phone != NULL && phone->flag)
	    {
		auth_usr_proc(auth_name);
		flag=print_a_record(auth_name, IMTRCount);
		if(flag==FALSE)
		    return(TRUE);
	     }
	}
    }
    else if(mark==DDD)
    {
	Nation_charge();
	na_fp = fopen("ddd.tmp","ab");
	fwrite(&temp_list,sizeof(RECORD_STRUCT),1,na_fp);
		  fclose(na_fp);

		  /* handle cash in pledge */
		  if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
				 update_cash(AUTH_CASH);
		  else
		  {
				ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
				phone = binary_search(user);         /* locate the phone No */
				if( (phone != NULL) && phone->feeflag[3])  	/* DDD fee */
					 update_cash(PHONE_CASH);
		  }

		  rate_tmp = get_rate(&temp_list.callee[1],DDD);   /* get charge rate */
		  if(rate_tmp == NULL)                             /* the area No. does not exist    */
				strcpy(rate1.area_name, a_name);
		  else
				rate1 =*rate_tmp;

		  if(Nation_prn && check_prn())        /* printer is ready */
		  {
				ltoa( temp_list.caller2, phone_no, 10 );
				phone = binary_search( phone_no );
				if( phone != NULL && phone->flag)
				{
					 auth_usr_proc(auth_name);
					 flag=print_a_record(auth_name, IMTRCount);
					 if(flag==FALSE)
						  return(TRUE);
				}
		  }
	 }
	 else if(mark==LDD)
	 {
		  Local_charge();
		  lo_fp = fopen("ldd.tmp","ab");
		  fwrite(&temp_list,sizeof(RECORD_STRUCT),1,lo_fp);
		  fclose(lo_fp);

		  /* handle cash in pledge */
		  if(Usr_typ == NORMAL_USR || Ldd_fflg == YES)
				if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
					 update_cash(AUTH_CASH);
				else
				{
					 ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
					 phone = binary_search(user);         /* locate the phone No */
					 if( phone != NULL)
					 {
						  f1 = IsSpecialCall(temp_list.callee,SpecLen[0],0);
						  f2 = IsSpecialCall(temp_list.callee,SpecLen[1],1);

						  if(   (phone->feeflag[1] && f1)
								||(phone->feeflag[2] && f2)
								||(phone->feeflag[0] && !(f1 && f2) ))
				  update_cash(PHONE_CASH);
					 }
				}

		  rate_tmp = get_rate(temp_list.callee,LDD);   /* get charge rate */
		  if(rate_tmp == NULL)                             /* the area No. does not exist    */
				strcpy(rate1.area_name, a_name);
		  else
				rate1 =*rate_tmp;

		  if(Local_prn && check_prn())        /* printer is ready */
		  {
				ltoa( temp_list.caller2, phone_no, 10 );
				phone = binary_search( phone_no );
				if( phone != NULL && phone->flag)
				{
					 auth_usr_proc(auth_name);
					 flag=print_a_record(auth_name, IMTRCount);
					 if(flag==FALSE)
						  return(TRUE);
				}
		  }
	 }

	 // check overcash
	 if( (temp_cash != NULL) && (temp_cash->flag ==CASH_ON) &&
		  (temp_cash->cash < temp_cash->limit ) )/* overflow */
	 {
		  if(temp_list.auth_code !=  0xaaaaaa) /* auth*/
		  {
				if(   Auth_cd_top[(UI)temp_list.auth_code].auth_class != 0
				 || Auth_cd_top[(UI)temp_list.auth_code].auth_set_flag == 1)
				{
					Auth_cd_top[(UI)temp_list.auth_code].auth_class = 0;
					Auth_cd_top[(UI)temp_list.auth_code].auth_set_flag = 1;
				}
				sprintf(tmp, "099999%04d00", temp_list.auth_code);
				err = 0;
				for(port=0; port<PORT_NUM; port++)
					 if(Sys_mode.com_m[port])
						  if(DownloadCode(tmp, 6, 6, 0xBA, port) != 0) err=1;
				if(!err)
					Auth_cd_top[(UI)temp_list.auth_code].auth_set_flag = 0;
		  }
		  else
		  {
				ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
				phone = binary_search(user);         /* locate the phone No */
				if(phone != NULL)
				{
					phone->class = 1;
					phone->set_flag = 1;
				}
				sprintf(tmp, "%02d%02d%02d", phone->code%PhonesPerPort, (int)(phone->class),(int)(phone->max_min));
				if(DownloadCode(tmp, 0, 6, 0xC0, phone->code/PhonesPerPort) == 0)
					phone->set_flag = 0;
		  }
	 }

	 return(TRUE);
}

/* function  : convert a compacted BCD string to unsinged char
 * called by : all_record_transfer(), imm_record_transfer()
 * input     : *c_bcd -- the compacted BCD string
 * output    : the converted unsinged char
 * date      : 1993.9.28
 */
UC cbcd_uc(UC *c_bcd)
{
	 UC tmp;

    tmp = 0;
	 tmp = ((*c_bcd) & 0xf0)>>4;
	 tmp = tmp*10+((*c_bcd) & 0x0f);

    return(tmp);
}


UC delay_proc(UC mode)
{
    int   dh, dm, ds;
    UL    dl;

    dh = temp_list.hour2 - temp_list.hour1;
    dm = temp_list.min2  - temp_list.min1;
    ds = temp_list.sec2  - temp_list.sec1;
    if(ds < 0)
    {
	ds += 60;
	dm --;
    }

    if(dm < 0)
    {
	dm += 60;
	dh --;
    }

    if(dh < 0)
    {
	dh += 24;
    }

    dl = ds + 60*dm + 60*60*dh;

    if(mode == IDD)
    {
	if(dl <= (UL)Delay.delay_idd)
	    return(FALSE);
	else
	    temp_list.sec1 += (UC)Delay.delay_idd;
    }
    else if(mode == DDD)
    {
	if(dl <= (UL)Delay.delay_ddd)
	    return(FALSE);
	else
	    temp_list.sec1 += (UC)Delay.delay_ddd;
    }
    else
    {
	if(dl <= (UL)Delay.delay_ldd)
	    return(FALSE);
	else
	    temp_list.sec1 += (UC)Delay.delay_ldd;
    }

    if(temp_list.sec1 > 60)
    {
	temp_list.sec1 -= 60;
	temp_list.min1 ++;
    }
    if(temp_list.min1 > 60)
    {
	temp_list.min1 -= 60;
	temp_list.hour1 ++;
    }
    if(temp_list.hour1 > 24)
    {
	temp_list.hour1 -= 24;
	temp_list.day1 ++;
    }
    switch(temp_list.mon1)
    {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	     if(temp_list.day1 > 31)
	     {
		  temp_list.day1 = 1;
		  temp_list.mon1 ++;
	     }
	     break;
	case 4:
	case 6:
	case 9:
	case 11:
	     if(temp_list.day1 > 30)
	     {
		 temp_list.day1 = 1;
		 temp_list.mon1 ++;
	      }
	      break;
	case 2:
	     if(leap_year(temp_list.year1))
	     {
		  if(temp_list.day1 > 29)
		  {
		     temp_list.day1 = 1;
		     temp_list.mon1 = 3;
		  }
	     }
	     else
	     {
		 if(temp_list.day1 > 28)
		 {
		     temp_list.day1 = 1;
		     temp_list.mon1 = 3;
		  }
	     }
	     break;
	case 12:
	     if(temp_list.day1 > 31)
	     {
		 temp_list.day1  = 1;
		 temp_list.mon1  = 1;
		 temp_list.year1 ++;
	     }
	     break;
	default:
	     break;
	 }
/*
    if(dl < 225)
	return TRUE;
    else if(dl < 300)
	temp_list.sec2 += 20;
    else if(dl < 400)
		  temp_list.sec2 += 30;
    else if(dl < 500)
	temp_list.sec2 += 40;
    else if(dl < 600)
		  temp_list.sec2 += 50;
    else if(dl < 1200)
	temp_list.min2 += 1;
    else
	temp_list.min2 += 2;

    if(temp_list.sec2 > 60)
	 {
		  temp_list.sec2 -= 60;
	temp_list.min2 ++;
    }
    if(temp_list.min2 > 60)
	 {
		  temp_list.min2 -= 60;
	temp_list.hour2 ++;
    }
	 if(temp_list.hour2 > 24)
    {
	temp_list.hour2 -= 24;
		  temp_list.day2 ++;
	 }
    switch(temp_list.mon2)
    {
	case 1:
		  case 3:
		  case 5:
	case 7:
	case 8:
		  case 10:
	     if(temp_list.day2 > 31)
	     {
					  temp_list.day2 = 1;
					  temp_list.mon2 ++;
	     }
	     break;
	case 4:
		  case 6:
		  case 9:
	case 11:
	     if(temp_list.day2 > 30)
				 {
		 temp_list.day2 = 1;
		 temp_list.mon2 ++;
				 }
				 break;
	case 2:
	     if(leap_year(temp_list.year2))
	     {
					  if(temp_list.day2 > 29)
					  {
		     temp_list.day2 = 1;
		     temp_list.mon2 = 3;
					  }
	     }
	     else
				 {
					  if(temp_list.day2 > 28)
		 {
                     temp_list.day2 = 1;
                     temp_list.mon2 = 3;
					  }
				 }
             break;
        case 12:
				 if(temp_list.day2 > 31)
             {
                 temp_list.day2  = 1;
					  temp_list.mon2  = 1;
					  temp_list.year2 ++;
	     }
	     break;
	default:
				 break;
	 }
*/
    return(TRUE);
}

void update_cash(UC mode)
{
	 UC phone_no[10];

	 if( mode == PHONE_CASH )
		  ltoa(temp_list.caller2, phone_no, 10);
	 else
		  sprintf(phone_no, "%03ld",temp_list.auth_code);

	 if(Cashflg[mode])
	 {
		  temp_cash = binary_cash(phone_no,mode);
		  if( (temp_cash != NULL) && (temp_cash->flag ==CASH_ON) )
				temp_cash->cash -= temp_list.charge;
	 }

}

void InnerToExt()
{
	 UC    pcflg;
    PHONE_STRUCT *pcode;
	 UI    pclen;

           pcode = Phone_top;
            pcflg = 0;
            for(pclen = 0; pclen < Newlen; pclen++)
            {
                if(pcode->code == temp_list.caller2)
                {
                    pcflg = 1;
                    break;
					 }
                pcode ++;
				}

            if( pcflg == 1)
		temp_list.caller1 = temp_list.caller2 = atol(pcode->phone_no);
            else
            {
            	message_disp(8, "存在未设定分机内码!");
                outf(MES_LEFT+168, MES_TOP+1, LIGHTRED, YELLOW, "%-7lu", temp_list.caller2);
                sound_alarm();
                message_end();
		temp_list.caller1 = temp_list.caller2 = 999999;
            }
}