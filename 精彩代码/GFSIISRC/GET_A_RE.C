#include <fcntl.h>
#include <conio.h>
#include <stdio.h>
#include <io.h>
#include <dos.h>
#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"
#include <bios.h>
#include <alloc.h>

void interrupt far (*old_com1)(void);
void interrupt far (*old_com2)(void);

/* function  : initialize the SIO port address, according to COM1 or COM2
 * called by : take_all_record()
 * date      : 1993.9.24
 */
void InitPortAdd(void)
{
    int i;

    SIO_INT[0]  = 0xc;      /* interrupt vector of COM1  */
    SIO_DATA[0] = 0x3f8;    /* data port address of COM1 */
    SIO_INT[1]  = 0xb;      /* interrupt vector of COM2  */
    SIO_DATA[1] = 0x2f8;    /* data port address of COM2 */

    for(i=0; i<2; i++) {
        SIO_IER[i] = SIO_DATA[i]+1;    /* interrupt enable register         */
    	SIO_IIR[i] = SIO_DATA[i]+2;    /* interrupt identification register */
    	SIO_LCR[i] = SIO_DATA[i]+3;    /* line control register             */
    	SIO_MCR[i] = SIO_DATA[i]+4;    /* modem control register            */
    	SIO_LSR[i] = SIO_DATA[i]+5;    /* line status register              */
    	SIO_MSR[i] = SIO_DATA[i]+6;    /* modem status register             */
    }
}

/* function  : new interrupt serve program
 * called by : SIOInit()
 * date      : 1993.9.25
 */
void far interrupt new_com1(void)
{
    new_com(0);
}

void far interrupt new_com2(void)
{
    new_com(1);
}

void new_com(UC port)
{
    UC  status;

    while((status = inportb(SIO_IIR[port])&0x07) != 1)   /* have interrupt */
    {
        disable();           /* disable further interrupt come in  */
        switch(status)
        {
            case 6:                    /* receiving line error     */
                inportb(SIO_LSR[port]);
                break;
            case 4:                    /* have received valid data */
                SIORecInt(port);
                break;
            case 2:                    /* sending buffer is empty  */
                inportb(SIO_IIR[port]);
                break;
            case 0:                    /* modem interrupt */
                inportb(SIO_MSR[port]);
                break;
            default:
                break;
        }
        outportb(0x20,0x20);           /* tell 8259 this is end of interupt*/
        enable();
    }
    return;
}

/* function  : data receiving handle
 * called by : new_com()
 * date      : 1993.9.25
 */
void SIORecInt(UC port)                            /*receive data*/
{
    UC ch;

    ch = SIORecBuf[port][SIORecTail[port]++] = inportb(SIO_DATA[port]);
    if(DEBUG)
    {
	int i;
	UC str[4];
	union REGS inregs, outregs;

	sprintf(str, "#%02x", ch);

	for(i=0; i<3; i++)
	{
	   // show the character
	   inregs.h.ah = 0x09;
	   inregs.h.al = str[i];
	   inregs.h.bh = 0;
	   inregs.h.bl = 7; // character color
	   inregs.x.cx = 1;
	   int86(0x10, &inregs, &outregs);

	   // get the cursor position
	   inregs.h.ah = 0x03;
	   inregs.h.bh = 0;
	   int86(0x10, &inregs, &outregs);

	   // set the curosr positon
	   inregs.h.ah = 0x02;
	   if(outregs.h.dl == 79)  // last one
	   {
	       inregs.h.dh = outregs.h.dh + 1; // the next row
	       inregs.h.dl = 0;
	   }
	   else
	   {
	       inregs.h.dh = outregs.h.dh; // the same row
	       inregs.h.dl = outregs.h.dl + 1;
	   }

	   inregs.h.bl = 0;  // page
	   int86(0x10, &inregs, &outregs);
       }
    }

    if( (ch==0xfd) || (ch==0xfc) || (ch==0xb4) || (ch==0xb7) || (ch==0xb8) )
        CmdCount[port]++;
    if(SIORecTail[port] >= SIO_BUF_SIZE)
        SIORecTail[port] -= SIO_BUF_SIZE;

    return;
}

/* function  : send a char to SIO port, that is,send data to Exchanger
 * date      : 1993.9.25
 */
void ToCPU(UC port, UC ch)
{
    if(DEBUG)
	printf("%02x",ch);

    if(port == 0 || port == 1) {
	while(!(inportb(SIO_LSR[port]) & 0x20));      /* THR is not empty */
        outportb(SIO_DATA[port],ch);
    }
    else
        comtxch(IRQNO,port-2,ch);

    delay(15);		/* delay 15 ms */

    return;
}

/* function  : initialize Port address, set Baud rate, preserve the
 *             old INT vector, and set the new one
 * date      : 1993.9.25
 */
void SIOInit(void)
{
    int i,j;

    InitPortAdd();

    for(j=0; j<2; j++) {
        outportb(SIO_LCR[j],0x83);       /* Access Divisor Latch      */
        outportb(SIO_DATA[j],0x30);      /* 2400 Baud Rate            */
        outportb(SIO_IER[j],0);          /* disable interrupt of UART */
        outportb(SIO_LCR[j],0x03);       /* set 3-bits data len       */
    }

    disable();
    old_com1 = getvect(SIO_INT[0]);   /* preserve the old interrupt vector */
    setvect(SIO_INT[0],new_com1);     /* and set the new one               */

    old_com2 = getvect(SIO_INT[1]);   /* preserve the old interrupt vector */
    setvect(SIO_INT[1],new_com2);     /* and set the new one               */

    for(j=0; j<2; j++) {
        outportb(SIO_IER[j],0x1);        /* Enable Receive Int */
        outportb(SIO_MCR[j],0xb);        /* must be set if want interruption  */
    }
    /* in 8259A interrupt mask regiter(0x21), if bit is set, then the
     * corresponding interrupt is disable. COM1(IRQ4) and COM2(IRQ3)
     * interrupts are bit 4 and bit 3, so set them to 0 to enable
     * interrupt
     */
    outportb(0x21, inportb(0x21)&0xe7);

    for(j=0; j<2; j++) {
        for(i = 0; i < 100; i++)
        {
            inportb(SIO_DATA[j]);
            inportb(SIO_LSR[j]);
            inportb(SIO_MSR[j]);
            inportb(SIO_IIR[j]);
        }
    }

    for(j=0; j<PORT_NUM; j++) {
        SIORecTail[j] = SIORecHead[j] = 0;
        CmdCount[j] = 0;
    }

    TRCount = ERRCount = ErrCount = 0;
    enable();

    return;
}

/* function  : set back the interrupt vector to the original one
 * called by : take_all_record()
 * date      : 1993.9.25
 */
void SIOEnd(void)
{
    setvect(SIO_INT[0],old_com1);     /* and set the new one               */
    setvect(SIO_INT[1],old_com2);     /* and set the new one               */
    return;
}

/* function  : take all telephone records from Exchanger
 * called by : phone_record_proc()   (record.c)
 * output    : 0 -- unsuccessful in receiving
 *             1 -- successful in receiving
 * procedure : (1) connect with Exchanger
 *             (2) take all the telephone records to pabx.dat, and inform
 *                 Exchanger to clear the record buffer
 *             (3) transfer the records to idd.dbf, ddd.dbf and
 *                 ldd.dbf
 *             (4) remove pabx.dat
 * date      : 1993.9.25
 */
UC take_all_record(void)
{
    UC    suc;
    int   handle;
    UL    len1,len2;
    FILE  *fp;
    UC    port;

    check_dat_tmp();
    handle = open("pabx.chg",O_RDONLY|O_BINARY);
    if(handle != -1)         /* the file exists, then transfer again   */
    {
	len1 = filelength(handle);
	close(handle);
	if(len1 != -1)
	{
	    fp = fopen("pabx.lth","rb");    /* length file of pabx.chg */
	    if(fp != NULL)
	    {
		if(fread(&len2,sizeof(UL),1,fp)==1)
		{
		    fclose(fp);
		    if(len1 == len2)
		    {
			message_disp(8,"正在分拣, 请稍候...");
			all_record_transfer();
			message_end();

			tmp_to_dat();
			remove("pabx.chg");
			remove("pabx.lth");

			if(Cashflg[PHONE_CASH])
			    overflow_cash(INQUIRE, PHONE_CASH,1,1); /* in overcash.c */
			if(Cashflg[AUTH_CASH])
			    overflow_cash(INQUIRE, AUTH_CASH,1,1);  /* in overcash.c */
		    }   /* if(len1 == len2)               */
		}  /* if(fread(&len2,sizeof(UL),1,fp)==1) */
	    } /* if(fp != NULL) */
	    remove("pabx.chg");
	}     /* if(len1 != -1) */
    }    /* if(handle != -1)    */

    pop_back(H_BX,H_BY,H_BX+3*H_XAD,H_BY+2*H_YAD,LIGHTGRAY);

    for(port=0; port<PORT_NUM; port++) {
	if(!Sys_mode.com_m[port]) continue;

	suc = ConnTermAndCPU(port);       /* connect with Exchanger */
	if(suc != 1)                  /* not successful */
	{
	    rid_pop();
	    return(suc);
	}
    }

    suc = take_record();
    if(suc == 1)                  /* successful           */
    {
	if(TRCount != 0)          /* backup               */
	    tmp_to_dat();
	remove("pabx.chg");
	remove("pabx.lth");

	if(Cashflg[PHONE_CASH])
	    overflow_cash(INQUIRE, PHONE_CASH, 1, 1); /* in overcash.c */
	if(Cashflg[AUTH_CASH])
	    overflow_cash(INQUIRE, AUTH_CASH, 1, 1);  /* in overcash.c */
    }

    rid_pop();

    return(suc);
}

/* function  : check whether *.dat and *.tmp are the same, if not the same,
 *             copy *.dat to *.tmp
 * called by : take_all_record()
 * date      : 1993.10.18
 */
void check_dat_tmp(void)
{
    int   k;
    long  buff1,buff2;

    if ((k=open("idd.tmp",O_RDONLY|O_BINARY)) != -1)
    {
        buff1 = filelength(k);
        close(k);
	if((k=open("idd.dbf",O_RDONLY|O_BINARY)) != -1)
        {
            buff2 = filelength(k);
            close(k);
            if((buff1 != buff2) && (buff2 != 0))
                fcopy("idd.dbf","idd.tmp");
        }
    }

    if ((k=open("ddd.tmp",O_RDONLY|O_BINARY)) != -1)
    {
        buff1 = filelength(k);
        close(k);
        if((k=open("ddd.dat",O_RDONLY|O_BINARY)) != -1)
	{
            buff2 = filelength(k);
            close(k);
            if((buff1 != buff2) && (buff2 != 0))
                fcopy("ddd.dat","ddd.tmp");
        }
    }

    if ((k=open("ldd.tmp",O_RDONLY|O_BINARY)) != -1)
    {
        buff1 = filelength(k);
        close(k);
        if((k=open("ldd.dbf",O_RDONLY|O_BINARY)) != -1)
        {
            buff2 = filelength(k);
            close(k);
            if((buff1 != buff2) && (buff2 != 0))
                fcopy("ldd.dbf","ldd.tmp");
        }
    }
    return;
}


/* function  : connect with Exchanger
 * called by : take_all_record()
 * output    : 1 -- successful in connection
 *             0 -- unsuccessful
 * date      : 1993.9.25
 */
UC ConnTermAndCPU(UC port)
{
    int  i;
    char hand_shake[9] = "\xb0YWHQCSN\xfc";
    UC   msg[20];
    UC   suc, flag = 0;
    UL   GoalTick;

    sprintf(msg, "正在联机(%d)...", port+1);
    hz16_disp(H_BX+H_XAD, H_BY+H_YAD/2, msg, BLACK); /* connecting */

    if(port == 0 || port == 1)
        outportb(SIO_IER[port],0x1);        /* Enable Receive Int */

    CheckPort(port);
    SIORecHead[port] = SIORecTail[port] = CmdCount[port] = 0;
    for(i=0; i<9; i++)
        ToCPU(port, 0xb0);

    GoalTick = *Tick_cnt_ptr + TimeOut;
    for(;;)
    {
        if(kbhit() && (get_key0()==ESC))
        {
           flag = 0;
           break;
        }

        CheckPort(port);
	if(!CmdCount[port])             /* receiving buffer is still empty     */
        {
	    if(*Tick_cnt_ptr > GoalTick)      /* time out and no valid data received */
            {
                message(CONN_ERR);          /* error in connection       */
                flag = 0;
		break;
            }
            else
                continue;
        }

        suc = GetCmdFromBuf(port);    /* data have been received */
        if(suc != 1)              /* not the valid data      */
        {
            message(CONN_ERR);
            flag = 0;
            break;
        }

	for(i = 0; i < 9; i++)          /* compare with the hand_shake infor*/
            if(CmdBuf[port][i] != hand_shake[i])       /* not the same         */
                break;
        if(i<9)
        {
            message(CONN_ERR);
	    flag = 0;
            break;
        }

        ToCPU(port,0xb6);              /* successful in connection */
        clr_scr(H_BX+H_XAD,H_BY+H_YAD/2,H_BX+2*H_XAD,H_BY+H_YAD,BLACK,LIGHTGRAY);
        flag = 1;
        break;
    }    /* end of "for(;;)" */

    return(flag);
}

/* function  : receive data and command from SIORecBuf
 * called by : ConnTermAndCPU(),take_record(),take_immediate_record()
 * output    : 0 -- not the valid data received
 *             1 -- successful
 * date      : 1993.9.25
 */
UC GetCmdFromBuf(UC port)
{
    int i, j, k;

    CheckPort(port);

    if(SIORecHead[port] == SIORecTail[port])
    {
        for(i = 0; i < CMD_BUF_SIZE; i++)
            CmdBuf[port][i] = 0;
        CmdCount[port]--;
        return(0);
    }

    i = 0;
    while(SIORecHead[port] != SIORecTail[port])
    {
        CmdBuf[port][i] = SIORecBuf[port][SIORecHead[port]++];     /* move data to CmdBuf */
        if( SIORecHead[port] >= SIO_BUF_SIZE)
            SIORecHead[port] -= SIO_BUF_SIZE;
        i++;
    }

    k = 0;
    for( ; ; )
    {
        if((CmdBuf[port][0] != 0xf0) && (CmdBuf[port][0] != 0xb0) &&
           (CmdBuf[port][0] != 0xb4) && (CmdBuf[port][0] != 0xb7) &&
//	   (CmdBuf[port][0] != 0xb5) && (CmdBuf[port][0] != 0xb2) &&
	   (CmdBuf[port][0] != 0xb8))          /* not the valid command head */
        {
            for(j=0;j<i;j++)           /* move forward */
                CmdBuf[port][j]=CmdBuf[port][j+1];
            k++;
            if(k==i)
		return(0);             /* error in received data */
        }
        else
            break;
    }

    for(; i < CMD_BUF_SIZE; i++)
        CmdBuf[port][i]=0;
    CmdCount[port]--;

    return(1);
}

/* function  : take records from Exchanger
 * called by : take_all_record()
 * output    : 0 -- unsuccessful in taking data
 *             1 -- successful
 * date      : 1993.9.25
 */
UC take_record(void)
{
    int   i, k;
    UC    SendB2=0;
    UL    LastTick;
    long  len;
    FILE  *fp, *fperror;
    UI	  rec_len=23;
    UC    port, B4Rtn, UsedPorts;

    B4Rtn = 0;
    UsedPorts = 0;
    for(port=0; port<PORT_NUM; port++)
    	if(Sys_mode.com_m[port])
        	UsedPorts++;

    TRCount = 0;

    hz16_disp(H_BX+H_XAD,H_BY+H_YAD/2,"正在接收...",BLACK);  /* receiving */

    fp = fopen("pabx.chg","wb");       /* original record file */

    LastTick = *Tick_cnt_ptr;

    for(; ;)
    {
        if(kbhit() && (get_key0()==ESC))
        {
            fclose(fp);
            outportb(SIO_IER[0],0);
            outportb(SIO_IER[1],0);
            return(0);
        }

        for(port=0; port<PORT_NUM; port++) {
            if(Sys_mode.com_m[port]) {
                CheckPort(port);
                if(CmdCount[port])
		{
		    LastTick = *Tick_cnt_ptr;
		    break;
		}
            }
        }
        if(port == PORT_NUM)
        {
	    if(*Tick_cnt_ptr > LastTick + TimeOut)
            {
                fclose(fp);
                outportb(SIO_IER[0],0);
                outportb(SIO_IER[1],0);
                message(REC_TIMEOUT);  /* TimeOut */
                return(0);
            }
            else
                continue;
        }
        else {
          GetCmdFromBuf(port);
          if((CmdBuf[port][0]==0xf0) && (CmdBuf[port][rec_len-1]==0xfd))   /* a telephone record*/
          {
	    CmdBuf[port][0] = 0;
            for(i = 1; i < rec_len-2; i++)                /* CmdBuf[25]=check sum */
                CmdBuf[port][0] += CmdBuf[port][i];
            CmdBuf[port][0] = CmdBuf[port][0] & 0x7f;

            if(CmdBuf[port][0] == CmdBuf[port][rec_len-2])     /* a valid phone record */
            {
                TRCount++;
                outf(H_BX+3*H_XAD/2,H_BY+H_YAD,7,BLACK,"<< %-5u>>",TRCount);
                CmdBuf[port][rec_len-2] = 0xfd;
                CmdBuf[port][0] = port;
                fwrite(CmdBuf[port],rec_len-1,1,fp);
                SendB2 = 0;
                ToCPU(port, 0xb5);                /* a verify information */
            }           /* end of "if(CmdBuf[0] == CmfBuf[25])"     */
            else
            {
                if(SendB2 < 5)
                {
                    SendB2++;
		    ToCPU(port, 0xb2);
                }
                else
                {
                    fperror = fopen("error.dat","ab");
                    fwrite(CmdBuf[port],sizeof(UC),rec_len,fperror);
                    fclose(fperror);

                    SendB2 = 0;
                    ERRCount++;
                    ToCPU(port, 0xb5);
                }
            }           /* end of "else if(CmdBuf[0] != CmfBuf[25])" */
          }     /* end of "if((CmdBuf[0]==0xf0) && (CmdBuf[26]==0xfd))"*/

          else if(CmdBuf[port][0] == 0xb4)          /* no more records       */
          {
            if(Menu_path == 300)             /* all record method */
            {
                ToCPU(port, 0xb1); ToCPU(port, 0xb1); ToCPU(port, 0xb1);
		ToCPU(port, 0xb1); ToCPU(port, 0xb1); ToCPU(port, 0xb1);
                ToCPU(port, 0xb1); ToCPU(port, 0xb1); ToCPU(port, 0xb1);
            }
            else                             /* go to immediate method */
            {
                ToCPU(port, 0xb7); ToCPU(port, 0xb7); ToCPU(port, 0xb7);
                ToCPU(port, 0xb7); ToCPU(port, 0xb7); ToCPU(port, 0xb7);
                ToCPU(port, 0xb7); ToCPU(port, 0xb7); ToCPU(port, 0xb7);

                /* read returned B7 */
		while(SIORecHead[port] == SIORecTail[port] )
		{
			CheckPort(port);
			if(_bios_keybrd(_KEYBRD_READY) )
			if(_bios_keybrd(_KEYBRD_READ) == ESC) break;
		}

		if(SIORecHead[port] == SIORecTail[port])
		    warn_mesg("错误","通讯超时");
		else if(SIORecBuf[port][SIORecHead[port]] == 0xB7) { /* success */
		}
		else
		    warn_mesg("错误","设置错误");
	    }
	    if(port <2)
		    outportb(SIO_IER[port],0);             /* disable interruption  */

	    B4Rtn++;
	    if(B4Rtn < UsedPorts ) continue;

	    fclose(fp);

	    k = open("pabx.chg",O_RDONLY|O_BINARY);
	    len = filelength(k);             /* take file length */
	    close(k);
	    fp = fopen("pabx.lth","wb");
	    fwrite(&len,sizeof(long),1,fp);
	    fclose(fp);

	    clr_scr(H_BX+H_XAD,  H_BY+H_YAD/2,\
		       H_BX+3*H_XAD-20,H_BY+2*H_YAD-20,BLACK,LIGHTGRAY);
	    if(TRCount == 0)                 /* No record */
	    {
		hz16_disp(H_BX+H_XAD,H_BY+H_YAD/2,"无  信  息!",BLACK);
		delay_10ms(100);
	    }
	    else
	    {
		hz16_disp(H_BX+H_XAD,H_BY+H_YAD/2,"接 收 完 毕!",BLACK);
		message_disp(8,"正在分拣, 请稍候...");
		all_record_transfer();
		message_end();
	    }
	    return(1);
	}
	else if(CmdBuf[port][0] == 0xb7)          /* returned B7       */
	{
	}
	else
	{
	    fperror = fopen("error.dat","ab");
	    fwrite(CmdBuf[port],sizeof(UC),rec_len,fperror);
	    fclose(fperror);

	    ERRCount++;
	    ToCPU(port, 0xb5);
	}
      }  /* end of "port"*/
    }    /* end of "for(;;)" */
}

/* function  : copy *.tmp to *.dat
 * called by : take_all_record(), take_immediate_record()
 * output    : 0 -- successful, others -- unsuccessful
 * date      : 1993.9.25
 */
UC tmp_to_dat(void)
{
    int    flag;
    struct ffblk  fblk;

    message_disp(8,"正在存盘, 请稍候...");       /* saving, please wait...*/

    flag = findfirst("idd.tmp",&fblk,FA_ARCH);
    if(flag == 0)
        fcopy("idd.tmp","idd.dbf");

    flag = findfirst("ddd.tmp",&fblk,FA_ARCH);
    if(flag == 0)
        fcopy("ddd.tmp","ddd.dbf");

    flag = findfirst("ldd.tmp",&fblk,FA_ARCH);
    if(flag == 0)
        fcopy("ldd.tmp","ldd.dbf");

    message_end();

    return(flag);
}

/* function  : copy *.dat to *.tmp
 * called by : take_all_record(), take_immediate_record()
 * output    : 0 -- successful, others -- unsuccessful
 * date      : 1993.9.25
 */
UC dat_to_tmp(void)
{
    int    flag;
    struct ffblk fblk;

    message_disp(8,"正在存盘, 请稍候...");       /* saving, please wait...*/

    flag = findfirst("idd.dbf",&fblk,FA_ARCH);
    if(flag == 0)
        fcopy("idd.dbf","idd.tmp");
    else
        remove("idd.tmp");

    flag = findfirst("ddd.dbf",&fblk,FA_ARCH);
    if(flag == 0)
        fcopy("ddd.dbf","ddd.tmp");
    else
        remove("ddd.tmp");

    flag = findfirst("ldd.dbf",&fblk,FA_ARCH);
    if(flag == 0)
        fcopy("ldd.dbf","ldd.tmp");
    else
        remove("ldd.tmp");

    message_end();

    return(flag);
}