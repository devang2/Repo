#include <bios.h>
#include <conio.h>
#include <dos.h>
#include <fcntl.h>
#include <string.h>
#include <graphics.h>
#include <io.h>
#include <bio.inc>
#include <key.inc>
#include <menu.inc>
#include "feedef.h"
#include <alloc.h>

UI         IMMTRCount;
UC        *a_name[16]={
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
		   "         ",
                   "         ",
		   "         ",
                   "         "
		  };

/* function  : take immediate telephone records
 * called by : phone_record_proc() (record.c)
 * date      : 1993.9.27
 */
void take_immediate_record(void)
{
    int  i, j;
    FILE *fp;
    UC   SendB2[PORT_NUM];
    UC   comm_flag[PORT_NUM];
    UC   nosendflg[PORT_NUM];
    UC   back_in = 8;
    UC   flag;
    UL   LastTick[PORT_NUM];
    UI	 rec_len=23;
    UC   port;
    UC   mode;
    UI   SpecLen[2];

    for(port=0; port<PORT_NUM; port++) {
	SIORecTail[port] = 0;
	SIORecHead[port] = 0;
	CmdCount[port] = 0;
	nosendflg[port] = 0;
	SendB2[port] = 0;
    }
    IMTRCount = 0;
    IMMTRCount = 0;

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

    for(mode=0; mode<2; mode++)
      if(Cashflg[mode])              /* want to use cash pledge function */
      {
	flag = load_cash_tbl(mode);
	if(flag != 1)        /* cash.dat does not exist          */
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

    draw_rec_tbl();          /* draw immediate phone record table */
    message_disp(back_in,"正在接收...");    /* receiving */

    outportb(SIO_IER[0],0x01);       /* enable interruption */
    outportb(SIO_IER[1],0x01);       /* enable interruption */

    for(port=0; port<PORT_NUM; port++)
	if(Sys_mode.com_m[port]) {
	    ToCPU(port, 0xb9);
	    ToCPU(port, 0xb8);
	    LastTick[port] = *Tick_cnt_ptr;
	    comm_flag[port] = 0;
	}

    for(; ;)
    {
	check_event_flag();            /* refresh the echo time */

	if(kbhit() && (get_key0()==ESC))
	{
	    for(port=0; port<PORT_NUM; port++)
		if(Sys_mode.com_m[port])
		    ToCPU(port, 0xb8); /* sure the connection is still on */

	    delay(100);

	    for(port=0; port<PORT_NUM; port++)
		if(Sys_mode.com_m[port])
		{
		    UI SioPtr;

		    CheckPort(port);
		    SioPtr = (SIORecTail[port] == 0)?SIO_BUF_SIZE-1:SIORecTail[port]-1;

		    if(SIORecBuf[port][SioPtr] != 0xb8)
			goto HostReset; /* out of connection */
		}

	    for(port=0; port<PORT_NUM; port++)
		if(Sys_mode.com_m[port]) {
		    ToCPU(port, 0xb1); ToCPU(port, 0xb1); ToCPU(port, 0xb1);
		    ToCPU(port, 0xb1); ToCPU(port, 0xb1); ToCPU(port, 0xb1);
		    ToCPU(port, 0xb1); ToCPU(port, 0xb1); ToCPU(port, 0xb1);
		}

		outportb(SIO_IER[0], 0);      /* disable interruption    */
		outportb(SIO_IER[1], 0);      /* disable interruption    */

		if(IMTRCount != 0)
		    tmp_to_dat();

		message_end();
		recover_screen(2);

		for(mode=0; mode<2; mode++)
		    if(Cashflg[mode])
		    {
			 save_cash_tbl(mode);
			 unload_cash_tbl(mode);
		    }

		    unload_special_tbl(0);
		    unload_special_tbl(1);

	    if(Authflg == ON)
	    {
		save_authcd_tbl(NOFRESH, 0);
		unload_authcd_tbl();
	    }
	    unload_rate_tab();
	    save_phone_tbl(NOFRESH, 0);
	    unload_phone_tbl();
	    return;
	}

	for(i=0; i<PORT_NUM; i++, port++) {
	    if(port >= PORT_NUM)  port = 0;
	    if(!Sys_mode.com_m[port]) continue;
	    CheckPort(port);
	    if(CmdCount[port])
	    {
		LastTick[port] = *Tick_cnt_ptr;
		break;
	    }
	    else if(SIORecBuf[port][SIORecHead[port]] == 0xF0 && (SIORecHead[port] != SIORecTail[port]) ) /* in sending */
		LastTick[port] = *Tick_cnt_ptr;

	    if(*Tick_cnt_ptr < LastTick[port]) /* another day */
		LastTick[port] = *Tick_cnt_ptr;

	    if(*Tick_cnt_ptr > LastTick[port] + TimeOut)    /* time out */
	    {
		if(comm_flag[port] != 2) /* still in connection */
		{
		     comm_flag[port] ++;
		     ToCPU(port, 0xb8);
		     LastTick[port] = *Tick_cnt_ptr;
		 }
		 else                   /* out of connection            */
		 {
		     for(j=0; j<PORT_NUM; j++)
				 if(Sys_mode.com_m[j] && comm_flag[j] == 2)
				 {
HostReset:			      recover_screen(2);
				      message_end();
				      message(REC_TIMEOUT);
				      dat_to_tmp();
				      outportb(SIO_IER[0],0);
				      outportb(SIO_IER[1],0);

				      if(Cashflg[PHONE_CASH])
					 unload_cash_tbl(PHONE_CASH);
				      if(Cashflg[AUTH_CASH])
					 unload_cash_tbl(AUTH_CASH);
				      unload_special_tbl(0);
				      unload_special_tbl(1);

				      if(Authflg == ON)
					 unload_authcd_tbl();
				      unload_rate_tab();
				      save_phone_tbl(NOFRESH, 0);
				      unload_phone_tbl();
				      return;
				}
			    }
			}
	}  /* end of "for(i=0; i<PORT_NUM; i++)"  */

	if(i == PORT_NUM) continue;

	GetCmdFromBuf(port);

	if( (CmdBuf[port][0]==0xf0) && (CmdBuf[port][rec_len-1]==0xfd) )  /* a valid record */
	{
	    if(nosendflg[port] != 1)
	    {
		CmdBuf[port][0]=0;
		for(i=1; i<rec_len-2; i++)        /* CmdBuf[rec_len-2]=check sum */
		    CmdBuf[port][0] += CmdBuf[port][i];
		CmdBuf[port][0] = CmdBuf[port][0] & 0x7f;

		if( CmdBuf[port][0] == CmdBuf[port][rec_len-2] )    /* check sum is right   */
		{
		    IMMTRCount++;
		    if(imm_record_transfer(port) == TRUE)       /* records transfer */
		    {
			TRCount++;
			IMTRCount++;
			disp_rec_tbl();
			outf(MES_LEFT+130,MES_TOP+1, 11,0,"<< %-5u>>",IMTRCount);
		    }
//                    outf(MES_LEFT+130,MES_TOP+1, 11,0,"<< %-5u>>",IMTRCount);
		    SendB2[port] = 0;
		    comm_flag[port] = 1;
		    ToCPU(port, 0xb5);

		    if((IMMTRCount%100) == 0)
		    {
			nosendflg[port] = 1;
			tmp_to_dat();

			ToCPU(port, 0xb7); ToCPU(port, 0xb7); ToCPU(port, 0xb7);
			ToCPU(port, 0xb7); ToCPU(port, 0xb7); ToCPU(port, 0xb7);
			ToCPU(port, 0xb7); ToCPU(port, 0xb7); ToCPU(port, 0xb7);
		    }
		}       /* end of "if(CmdBuf[0] == CmdBuf[rec_len-2]" */
		else
		{
		    if(SendB2[port] < 5)
		    {
			SendB2[port]++;
			ToCPU(port, 0xb2);
		    }
		    else
		    {
			fp = fopen("error.dat","ab");
			fwrite(CmdBuf[port],sizeof(UC),rec_len,fp);
                        fclose(fp);

                        SendB2[port] = 0;
                        ERRCount++;
			ToCPU(port, 0xb5);
		    }
		}       /* end of "else if(CmdBuf[0] != CmdBuf[rec_len-2]" */
	    }           /* end of "if(nosendflg != 1)"              */
	}     /* end of "if((CmdBuf[0]==0xf0) && (CmdBuf[rec_len-1]==0xfd))*/
	else if(CmdBuf[port][0] == 0xb8)
	{
	    comm_flag[port] = 1;
	}
	else if(CmdBuf[port][0] == 0xb7)
	{
	    nosendflg[port] = 0;
	    ToCPU(port, 0xb9);
	}
	else if(CmdBuf[port][0] == 0xb1) /* the host reset */
		goto HostReset;
	else
	{
	    fp = fopen("error.dat","ab");
	    fwrite(CmdBuf[port],sizeof(UC),rec_len,fp);
	    fclose(fp);

	    ERRCount++;
	    ToCPU(port, 0xb5);
	}
    }     /* end of "for(;;)" */
}

/* function  : draw table for record displaying
 * called by : take_immediate_record()
 * date      : 1993.9.29
 */
void draw_rec_tbl(void)
{
    TABLE_STRUCT rec_tbl={0, 99, 20, 18, 16, 9,\
			  {70, 73, 175, 46, 46, 46, 80, 46, 48},\
			  GREEN};

    clr_DialWin(2);

    draw_table(&rec_tbl);

    disp_rec_head();              /* display table head   */
    if(Cashflg[PHONE_CASH])
	hz16_disp(170,80,"立  即  话  单    (红色表示押金现额已低于下限)",BLACK);
    else
	hz16_disp(270,80,"立  即  话  单    金额单位: 元",BLACK);

    return;
}


/* function  : display the head Hanzi for the immdiate record table
 * called by : draw_rec_tbl()
 * date      : 1993.9.22
 */
void disp_rec_head(void)
{
	 outf(11,  103, 7,0, "分  机");   /* caller        */
	 outf(77,  103, 7,0, "授权用户"); /* authorized user */
	 outf(148, 103, 7,0, "通达地区"); /* callee        */
	 outf(222, 103, 7,0, "被    叫"); /* callee        */
	 outf(328, 103, 7,0, "日期");     /* date          */
	 outf(376, 103, 7,0, "起时");     /* starting time */
	 outf(423, 103, 7,0, "时长"); /* duration      */
	 outf(475, 103, 7,0, "费率(分)"); /* charge rate   */
	 outf(556, 103, 7,0, "附加");     /* added fee     */
	 outf(595, 103, 7,0, "总 费");   /* charge        */

	 return;
}

/* function  : display the immediate records
 * called by : take_immediate_record()
 * date      : 1993.9.22
 */
void disp_rec_tbl(void)
{
    UC     i, row, pos;
    static UC cash_flag[16]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static RATE_STRUCT rec_rate[16];
    static RECORD_STRUCT record[16];

    if( Cashflg[PHONE_CASH] == FALSE && Cashflg[AUTH_CASH] == FALSE)
	 for(i=0; i<16; i++)
	     cash_flag[i]=0;

    if(IMTRCount < 17)       /* immediate records are from 1 to 16 */
    {
	row = IMTRCount-1;
	pos = row;
	if( (Cashflg[PHONE_CASH] && temp_list.auth_code == 0xaaaaaa) ||
	    (Cashflg[AUTH_CASH] && temp_list.auth_code != 0xaaaaaa) )
	{
	    if( (temp_cash->cash >= temp_cash->limit) || (temp_cash==NULL)\
		|| (temp_cash->flag == CASH_OFF) )
		cash_flag[row] = 0;
	    else
		cash_flag[row] = 1;
	}
	record[row]   = temp_list;
	rec_rate[row] = rate;

	strcpy(rec_rate[row].area_name, rate1.area_name);

	auth_usr_proc(a_name[row]);

	disp_rec_row(cash_flag, record, rec_rate, a_name, row, pos);
    }
    else
    {
	pos = 0;
	row = (IMTRCount-1)%16;
	if( (Cashflg[PHONE_CASH] && temp_list.auth_code == 0xaaaaaa) ||
	    (Cashflg[AUTH_CASH] && temp_list.auth_code != 0xaaaaaa) )
	{
	    if((temp_cash->cash>=temp_cash->limit) || (temp_cash==NULL)\
		|| (temp_cash->flag == CASH_OFF) )
		cash_flag[row] = 0;
	    else                           /* cash pledge is overflow */
		cash_flag[row] = 1;
	}
	record[row]   = temp_list;
	rec_rate[row] = rate;

	strcpy(rec_rate[row].area_name, rate1.area_name);

	auth_usr_proc(a_name[row]);

	for(i=row+1; i<16; i++)
	{
	    disp_rec_row(cash_flag, record, rec_rate, a_name, i, pos);
	    pos++;
	}
	for(i=0; i<=row; i++)
	{
	    disp_rec_row(cash_flag, record, rec_rate, a_name, i, pos);
	    pos++;
	}
    }

    return;
}


/* function: search for autherized user
 * called by:
 * calling:
 * date:
 */
void auth_usr_proc(UC *auth_name)
{
    if(temp_list.auth_code != 0xaaaaaa)
	find_auth_usr(auth_name);
    else
        strcpy(auth_name, "");

    return;
}


/* function: search for autherized user
 * called by:
 * calling:
 * date:
 */
void find_auth_usr(UC *auth_name)
{
    AUTHCD_STRUCT *atbl;
    UI i;

    if(Authflg == ON)
    {
	atbl = binary_authcd(temp_list.auth_code);
	if(atbl == NULL)
	{
	    strcpy(auth_name , "未登记");
	}
	else
	{
	    for(i = 0; i<8; i++)
		auth_name[i] = atbl->auth_usr_nam[i];
	    auth_name[i]= '\0';
	}
    }
    else
    {
	strcpy(auth_name, "未登记");
    }

    return;
}

/* function  : display a row of the immediate record table
 * called by : disp_rec_tbl()
 * input     : cash_flag -- cash in pledge is overflow or not
 *             record    -- telephone record to be displayed
 *             rec_rate  -- charge rate to be displayed
 *             row       -- the temporary row to be displayed
 * date      : 1993.9.30
 */
void disp_rec_row(UC *cash_flag, RECORD_STRUCT *record, RATE_STRUCT *rec_rate,\
		  UC **a_name, UC row, UC pos)
{
    int i, j, k, l;
    int dh, dm, ds;
    UC tel_no[16];
    UC area_n[19];
    UI x, y;
    UC auth_name[9];
    UNIT_STRUCT  cur_unit;
    UC color;

    if(IMTRCount > 16)
	brush_one_row(pos, 9, 7);

    for(j=0; j<8; j++)
	auth_name[j]=a_name[row][j];
    auth_name[j] ='\0';

    cur_unit.unit_x = pos;
    for(j=0;j<9;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+4;
	y = cur_unit.dot_sy+1;

	switch(j)
	{
	    case 0:      /* main caller 2 */
		 if( (cash_flag[row] == 1) && (record[row].auth_code == 0xaaaaaa))
		    color = LIGHTRED;
		 else
		    color = 0;

		 if(record[row].caller2 != 999999L)
		     outf(x,y,7,color,"%-lu",record[row].caller2);
		 else
		 {
		     outf(x,y,7,0,"未定义");
		     sound_alarm();
		 }
		 if(cash_flag[row] == 1) sound_alarm();

		 break;
	    case 1:      /* autherized user */
		 if(record[row].auth_code != 0xaaaaaa)
		 {
		     if(cash_flag[row] == 0)
			 outf(x, y, 7, 0,"%-s",auth_name);
		     else
			 outf(x, y, 7, LIGHTRED,"%-s",auth_name);
		 }
		 break;
	    case 2:      /* callee[17]    */
		 i=0;
		 while( (record[row].callee[i] != 0xfd) && (i<14) )
		 {
		     tel_no[i] = record[row].callee[i]+'0';
		     i++;
		 }
		 tel_no[i] = '\0';
		 l=(21-i)/2;
		 if(l > 4) l = 4;
		 for(k=0; k<l*2; k++)
		     area_n[k]=rec_rate[row].area_name[k];
		 area_n[k] = '\0';
		 outf(x-2,y,7,0, area_n);
		 outf(x+l*16,y,7,0,"%-s",tel_no);
		 break;
	    case 3:      /* month/date */
		 outf(x,y,7,0,"%02u/%02u", \
		     record[row].mon1,record[row].day1);
		 break;
	    case 4:      /* starting time */
		 outf(x,y,7,0,"%02u:%02u", \
		     record[row].hour1,record[row].min1);
		 break;
	    case 5:      /* duration   */
		 dh = record[row].hour2 - record[row].hour1;
		 dm = record[row].min2  - record[row].min1;
		 ds = record[row].sec2  - record[row].sec1;
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
            case 6:      /* charge rate    */
                 outf(x,y,7,0,"%-u/%-u",rec_rate[row].first_rate, \
					       rec_rate[row].formal_rate);
		 break;
	    case 7:      /* add fee      */
		 outf(x,y,7,0,"%5.2f",(float)record[row].add/100);
		 break;
	    case 8:      /* charge fee   */
		 outf(x-4,y,7,0,"%6.2f",(float)record[row].charge/100);
                 break;
        }    /* END SWITCH */
    }    /* end of "for(j=0;j<9;j++)" */
    return;
}


/* function  : transfer the callee number to ASCII string
 * called by : disp_rec_tbl()
 * date      : 1993.9.30
 */
void callee_uc(UC *callee,UC *tel_no)
{
    int i=0;

    while((callee[i]!=0xfd) && (i<14))
    {
        tel_no[i] = callee[i]+'0';
        i++;
    }
    tel_no[i] = '\0';

    return;
}
