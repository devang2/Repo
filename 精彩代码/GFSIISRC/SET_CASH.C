#include <alloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <graphics.h>
#include <io.h>
#include <mem.h>
#include <string.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include <g_id.inc>
#include "feedef.h"

#define BK_CLR    7
#define CHR_CLR   0
#define HEAD_CLR  0
#define ITBL_CLR  5

#define BK_COLOR        0       /* specify the background color  */
#define CHAR_COLOR      14      /* specify the characters' color */
#define CURSOR_COLOR    12      /* specify the cursor color      */

#define DRAW_KEY        draw_key(x, y, x+16, y+16, "关", 7, 0)
#define CHANG_KEY       chang_key1(x, y, x+16, y+16, "开", 12)

/* function  : set cash pledge (ya jin) for every telephone No
 * called by : set_data()
 * date      : 1993.10.18
 */
void set_cash_pledge(UC mode)
{
    UC flag;
    UI pg;
    CASH_PLEDGE   *tbl;
    UC *title = "注意:";
    UC *warn  = "应先打开开关再设定押金!";

    TABLE_STRUCT  cash_tbl = {115,99,20,18,16,5, {48, 72, 72, 72, 72}, ITBL_CLR};

    if(Cashflg[mode] == 0)         /* do not use cash pledge function      */
    {
	warn_mesg(title, warn);
	return;
    }

    flag = check_maint_cash(mode);
    if(flag == 0)
    {
	message(PHO_FILE_ERR);
	return;
    }

    flag = load_cash_tbl(mode);       /* load cash file to memory */
    Cash_len[mode] = cash_tbl_len(mode);

    clr_DialWin(1);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&cash_tbl);        /* draw the table frame */
    disp_cash_head(SET_CASH, mode);     /* display table head   */
    disp_cash_help(SET_CASH, mode);     /* display the help information */

    message_disp(8,"根据右上方弹出提示窗口提示操作");
    Count = 1;
    tbl = Cash_top[mode];
    disp_cash_tbl(tbl,Count,SET_CASH, mode);

    if(!(Cash_len[mode]%16) && Cash_len[mode])
	 pg = Cash_len[mode]/16;
    else
	 pg = (UI)(Cash_len[mode]/16)+1;
    outf(542,397,11,14,"%2u",pg);

    locate_finger(0,2);
    input_cash_data(mode);
    unload_cash_tbl(mode);
    message_end();

    return;
}

/* function  : check if the telephones in charge.pho are the same as those
 *             in cash data file. If not, add to cash data file automaticly
 * called by : set_cash()
 * input     : mode -- phone pledge or authorized code pledge
 * output    : 0 -- unsuccesful
 *             1 -- successful
 * date      : 1993.11.9
 */
UC check_maint_cash(UC mode)
{
    FILE *fpm, *fpc;
    PHONE_STRUCT  phone;
    AUTHCD_STRUCT auth;
    CASH_PLEDGE *cashe, ctmp={"",0,0,0};
    size_t psize, asize, csize;

    psize = sizeof(PHONE_STRUCT);
    asize = sizeof(AUTHCD_STRUCT);
    csize = sizeof(CASH_PLEDGE);

    /* the phones in charge.pho are not the same as those in cash data file,
     * so refresh cash data file
     */
    message_disp(8,"正在进行文件操作, 请稍候...");    /* refreshing, wait */
    load_cash_tbl(mode);
    Cash_len[mode] = cash_tbl_len(mode);

    fpm = fopen(ChargeDataFile[mode],"rb");
    fpc = fopen(CashDataFile[mode],"wb");
    if(mode == PHONE_CASH)
	while(fread(&phone,psize,1,fpm) == 1)
	{
	    cashe = binary_cash(phone.phone_no, mode);
	    if(cashe != NULL)
		fwrite(cashe,csize,1,fpc);
	    else
	    {
		strcpy(ctmp.phone_no,phone.phone_no);
		fwrite(&ctmp,csize,1,fpc);
	    }
	 }
    else /* mode == AUTH_CASH */
	while(fread(&auth,asize,1,fpm) == 1)
	{
	    if(auth.auth_class != UNUSED_AUTH)
	    {
		sprintf(phone.phone_no, "%03d", auth.auth_code);
		cashe = binary_cash(phone.phone_no, mode);
		if(cashe != NULL)
		    fwrite(cashe,csize,1,fpc);
		else
		{
		    strcpy(ctmp.phone_no,phone.phone_no);
		    fwrite(&ctmp,csize,1,fpc);
		}
	     }
	}

    fclose(fpc);
    fclose(fpm);
    unload_cash_tbl(mode);
    message_end();

    return(1);
}

/* function  : load the cash data to memory, and input the cash_pledge
 * called by : set_cash_pledge()
 * output    : 0 -- the file of cash data file does not exist
 *             1 -- successful
 * date      : 1993.10.17
 */
UC load_cash_tbl(UC mode)
{
    FILE        *fp;
    CASH_PLEDGE *tbl;
    UL           ss;
    int          i, num = 0;
    UI		 maxlen;

    if(Cash_top[mode] != NULL) return(1); /*already loaded */

    fp = fopen(CashDataFile[mode],"rb");
    if(fp == NULL)      /* the file does not exist */
	return(0);

    maxlen = (mode == PHONE_CASH)? MAX_USERS:MAX_AUTHCD;
    Cash_top[mode] = (CASH_PLEDGE *)farcalloc(maxlen, sizeof(CASH_PLEDGE));
    if(!Cash_top[mode])
    {
	ss = (UL)(maxlen*sizeof(CASH_PLEDGE));
	exit_scr(1,"Out of memery ERROR: %ld bytes.\n\nGFS system shutdown abnormally.\n\n\n\n",ss);
    }

    tbl = Cash_top[mode];
    while(fread(tbl,sizeof(CASH_PLEDGE),1,fp) == 1)
    {
	tbl++;
	num++;
    }
    fclose(fp);

    for(i=num;i<maxlen;i++)
	(Cash_top[mode]+i)->phone_no[0] = '\0';

    return(1);
}

/* function  : free the memory for the cash pledge table
 * called by : set_cash_pledge()
 * date      : 1993.9.21
 */
void unload_cash_tbl(UC mode)
{
    if( Cash_top[mode] != NULL) {
	farfree(Cash_top[mode]);
	Cash_top[mode] = NULL;
    }
    return;
}

/* function  : transfer the phone no in charge.pho to phone cash data file
 *             or transfer the authority code in charge.aus to aus cash data file
 * called by : set_cash_pledge()
 * output    : 0 -- charge.pho does not exist
 *             1 -- successful
 * date      : 1992.10.17
 */
UC maint_to_cash(UC mode)
{
    FILE         *fps,*fpd;
    PHONE_STRUCT  phone;
    AUTHCD_STRUCT auth;
    CASH_PLEDGE   cashe;
    void 	 *strp;
    size_t        size;

    fps = fopen(ChargeDataFile[mode],"rb");
    if(fps == NULL)
	return(0);
    fpd = fopen(CashDataFile[mode],"wb");

    if(mode == PHONE_CASH)
    {
	strp = &phone;
	size = sizeof(PHONE_STRUCT);
    }
    else
    {
	strp = &auth;
	size = sizeof(AUTHCD_STRUCT);
    }

    while(fread(strp,size,1,fps) == 1)
    {
	if(mode == PHONE_CASH)
	    strcpy(cashe.phone_no,phone.phone_no);
	else
	{
	    if(auth.auth_class == UNUSED_AUTH) continue;
	    sprintf(cashe.phone_no, "%03d", (UI)auth.auth_code);
	}

	cashe.cash = 0;
	cashe.limit = 0;
	fwrite(&cashe,sizeof(CASH_PLEDGE),1,fpd);
    }
    fclose(fpd);
    fclose(fps);

    return(1);
}

/* function  : calculate the length of cash pledge table
 * called by : set_cash_pledge()
 * date      : 1993.10.16
 */
UI cash_tbl_len(UC mode)
{
    CASH_PLEDGE *tbl;
    UI tbllen = 0, maxlen;


    maxlen = (mode == PHONE_CASH)? MAX_USERS:MAX_AUTHCD;

    tbl = Cash_top[mode];

    while(strlen(tbl->phone_no) && tbllen<maxlen)
    {
	tbllen++;
	tbl++;
    }

    return(tbllen);
}

/* function  : display the head Hanzi for the cash table
 * called by : set_cash_pledge()
 * date      : 1993.9.22
 */
void disp_cash_head(UC opflg, UC mode)
{
    UC *msg[2]={"分机号码","授权卡号"};

    if(opflg == SET_CASH)
	outf(225, 76, BK_CLR, BLACK, "押  金  设  定");     /* No.         */
    else if(opflg == INQ_CASH)
	outf(225, 76, BK_CLR, BLACK, "押  金  查  询");     /* No.         */
    else
	outf(225, 76, BK_CLR, BLACK, "透  支  报  告");     /* No.         */

    outf(124, 103, BK_CLR, HEAD_CLR, "序号");     /* No.         */
    outf(169, 103, BK_CLR, HEAD_CLR, msg[mode]);  /* phone No or auth code */
    outf(242, 103, BK_CLR, HEAD_CLR, "押金状态"); /* cash now    */
    outf(315, 103, BK_CLR, HEAD_CLR, "押金现额"); /* cash now    */
    outf(388, 103, BK_CLR, HEAD_CLR, "押金下限"); /* cash limit  */

    return;
}


/* function  : display the help information for input
 * called by : set_cash_pledge()
 * input     : mode -- SET_CASH or INQ_CASH
 * date      : 1993.9.22
 */
void disp_cash_help(UC opflg, UC mode)
{
    UC *msg[2] = {"F6   分机号定位","F6 授权卡号定位"};

    hz16_disp(500,103,"单位: 元",0);

    /*** PROMPT: ***/
    draw_back1(480, 122, 625, 357, 11);
    if(opflg == SET_CASH)
    {
	outf(490, 142, 11, 0, "F2   存盘");
	outf(490, 162, 11, 0, msg[mode]);
	outf(490, 182, 11, 0, "F7   页定位");
	outf(490, 202, 11, 0, "←↓→↑ 移动");
	outf(490, 222, 11, 0, "PgUp  向前翻页");
	outf(490, 242, 11, 0, "PgDn  向后翻页");
	outf(490, 262, 11, 0, "Enter 输入");

	outf(490, 322, 11, 0, "Esc   退出");
    }
    else
    {
	outf(490, 142, 11, 0, msg[mode]);
	outf(490, 162, 11, 0, "F7   页定位");
	outf(490, 182, 11, 0, "↓↑ 移动");
	outf(490, 202, 11, 0, "PgUp  向前翻页");
	outf(490, 222, 11, 0, "PgDn  向后翻页");

	outf(490, 322, 11, 0, "Esc   退出");
    }

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(498, 365, 607, 427, 11);
    outf(518, 377, 11, 14, "第");
    outf(575, 377, 11, 14, "页");
    outf(518, 397, 11, 14, "共");
    outf(575, 397, 11, 14, "页");
}

/* function  : display a page of cash table
 * called by : set_cash_pledge(), pgup_cash_tbl(), pgdn_cash_tbl()
 * input     : tbl -- the first item of this page
 *             num -- the xu-hao of the first item of this page
 *             cmode -- SET_CASH or INQ_CASH
 *	       mode -- PHONE_CASH or AUTHCD_CASH
 * date      : 1993.10.16
 */
void disp_cash_tbl(CASH_PLEDGE *tbl, int num, UC cmode, UC mode)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;
    UI maxlen;

    maxlen = (mode == PHONE_CASH)? MAX_USERS:MAX_AUTHCD;

    for(i=0; i<16; i++)          /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > maxlen)
	     break;
	 outf(124, i*19+123, BK_CLR, HEAD_CLR, "%-3u", num+i);
    }

    pg = (UI)(num/16)+1;         /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(534, 393, 575, 377);
    outf(542, 377, 11, 14, "%2u", pg);

    for(i=0; i<16; i++)
    {
	 if(!strlen(tbl->phone_no) || (num+i > maxlen))
	     break;

	 if((cmode == INQ_CASH) || (cmode == INQ_OVCASH))
	 {
	     if(tbl->flag == CASH_ON)
	     {
		 if( tbl->cash < tbl->limit)
		     indicator( 284+4, i*19+123+8, LIGHTRED );
		 else
		     indicator( 284+4, i*19+123+8, LIGHTGREEN );
	     }
	 }
	 for(j=1;j<5;j++)
	 {
	      cer_unit.unit_x = i;
	      cer_unit.unit_y = j;
	      get_certain(&cer_unit);
	      x = cer_unit.dot_sx+8;
	      y = cer_unit.dot_sy+1;

	      switch(j)
	      {
		   case 1:    /* telephone no */
		       if(!strcmp(tbl->phone_no,""))
			   break;
		       outf(x,y,BK_CLR,CHR_CLR,"%-s",tbl->phone_no);
		       break;
		   case 2:    /* cash switch */
		       if(tbl->flag == CASH_ON)
			   CHANG_KEY;
		       else
			   DRAW_KEY;
		       break;
		   case 3:    /* cash preserved now */
		       outf(x,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->cash)/100);
		       break;
		   case 4:    /* cash bottom  limit */
			outf(x,y,BK_CLR,CHR_CLR,"%6.2f",((double)tbl->limit)/100);
			break;
		   default:
			break;
	      }    /* END SWITCH */
	 }         /* END FOR2   */

	 tbl++;
    }    /* END FOR1 */

    return;
}

/* function  : do as input keys
 * called by : set_cash_pledge()
 * date      : 1993.9.23
 */
void input_cash_data(UC mode)
{
    UI input;
    UC *title = "注意:";
    UC *warn = "最多只能有这么多!";
    UC result;
    UNIT_STRUCT cur_unit;
    UI maxlen;

    maxlen = (mode == PHONE_CASH)? MAX_USERS:MAX_AUTHCD;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */

		 result=select_1in3();
                 switch(result)
                 {
                     case 1:     /* DON'T QUIT */
                         break;

                     case 2:     /* SAVE AND QUIT */
                         save_cash_tbl(mode);
                         recover_screen(1);
                         return;

                     case 3:     /* QUIT AND DON'T SAVE */
                         recover_screen(1);
			 return;
                 }
                 break;

            case UP:    /* GO TO THE LAST ROW */
                move_finger(0,1);
                break;

            case DOWN:  /* GO TO THE NEXT ROW */
                 if(Count+cur_unit.unit_x == maxlen)
                 {
                      warn_mesg(title, warn);
                      break;
                 }

                 if((Count+cur_unit.unit_x) < Cash_len[mode])
                     move_finger(1,1);
                 else
                     sound_bell();
                 break;

            case LEFT:       /* go to the left colomn */
                 if(cur_unit.unit_y > 2)
                     move_finger(2,1);
                 else
                     sound_bell();
                 break;

            case RIGHT:      /* go to the right column */
                 if( strlen(Cash_top[mode][Count+cur_unit.unit_x-1].phone_no) )
                     move_finger(3,1);
                 else
                     sound_bell();
                 break;

            case PAGEUP:
		 pgup_cash_tbl(SET_CASH, mode);
                 break;

            case PAGEDOWN:
		 pgdn_cash_tbl(SET_CASH, mode);
		 break;

	    case F2:    /* SAVE THE cash TABLE */
		 message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */
		 save_cash_tbl(mode);
		 message_end();
		 break;

	    case F6:    /* locate according to the phone No input */
		 if(mode == PHONE_CASH)
		     message_disp(8," 输入分机号   Enter 确认");    /*phone no.*/
		 else
		     message_disp(8," 输入卡号   Enter 确认");    /*phone no.*/

		 loc_cash_tbl(SET_CASH);
		 message_end();
		 break;

	    case F7:    /* PAGE No. LOCATE */
                 message_disp(8," 输入页号   Enter 确认");      /*page no.*/
		 loc_cash_page(SET_CASH, mode);
                 message_end();
                 break;

            case ENTER:
                 message_disp(8," 输入数字   Enter 确认");      /*number*/
		 modi_cash_tbl(mode);    /* MODIFY THE cash TABLE */
                 message_end();
                 break;

            default:
                 sound_bell();
                 break;
        }/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  : modify the cash table
 * called by : input_cash_data()
 * date      : 1993.9.23
 */
void modi_cash_tbl(UC mode)
{
    UI xs, ye, x, y;
    UI result;
    UI back_in=8;
    UL value;
    UNIT_STRUCT cur_unit;

    hide_finger();

    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ye = cur_unit.dot_sy;
    x = xs+8;
    y = ye+1;

    switch(cur_unit.unit_y)
    {
        case 2:
            if(Cash_top[mode][Count+cur_unit.unit_x-1].flag == CASH_ON)
            {
                 Cash_top[mode][Count+cur_unit.unit_x-1].flag = CASH_OFF;
                 DRAW_KEY;
            }
            else
	    {
                 Cash_top[mode][Count+cur_unit.unit_x-1].flag = CASH_ON;
                 CHANG_KEY;
            }
            move_finger(3,1);
            break;
        case 3:    /* cash preserved */
            set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_money(xs,ye,18,72,back_in,4,&value,0x0);

	    if(result)
		Cash_top[mode][Count+cur_unit.unit_x-1].cash = value;

	    move_finger(3,1);
	    break;

	case 4:    /* cash bottom limit */
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_money(xs,ye,18,72,back_in,3,&value,0x0);

	    if(result)
                Cash_top[mode][Count+cur_unit.unit_x-1].limit = (UI)value;

            if( (Count+cur_unit.unit_x) < Cash_len[mode] )  /* not the last one */
            {
                move_finger(1,1);
                move_finger(2,2);
             }
            else
                echo_finger();
            break;
        default:
            sound_alarm();
            break;
    }/* END OF SWITCH */

    return;
}

/* function  : save the cash table
 * called by : input_cash_tbl()
 * date      : 1993.9.23
 */
void save_cash_tbl(UC mode)
{
    FILE *fp;

    fp = fopen(CashDataFile[mode], "wb");
    fwrite(Cash_top[mode], sizeof(CASH_PLEDGE), Cash_len[mode], fp);
    fclose(fp);

    return;
}

/* function  : locate according to phone_no
 * called by : input_cash_data()
 * date      : 1993.9.23
 * note      : using the binary-search method
 */
void loc_cash_tbl(UC mode)
{
    CASH_PLEDGE *tbl;
    UC locate_no[7] = "";
    UC result;
    UC *msg="定位号";
    UC *warn="没有号码!";

    pop_back(250, 295, 420, 335, 7);
    draw_back(345, 304, 410, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_no(350, 306, 18, 24, 0, 6, locate_no, 0x00);

    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
        return;

    /***** Locate the position in the cash table according
               to the phone number user typed. *****/
    tbl = binary_cash(locate_no, mode);
    if(tbl == NULL)     /* can not find the phone_number, then warn */
    {
        warn_mesg("", warn);
        return;
    }

    Count = ((UI)(tbl-Cash_top[mode])/16)*16+1;
    hide_finger();
    brush_tbl(5, 16, 7);
    disp_cash_tbl(&Cash_top[mode][Count-1], Count,SET_CASH, mode);
    locate_finger(tbl-Cash_top[mode]-Count+1, 2);       /* LOCATTED */

    return;
}

/*             Function:       Change a page towards up.
	       Calls:          clr_cash_tbl,disp_cash_tbl
               Called by:      input_cash_data
               Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.9.23
 */
void pgup_cash_tbl(UC cmode, UC mode)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-16) < 0)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(5, 16, 7);
    Count = Count-16;
    disp_cash_tbl(&Cash_top[mode][Count-1], Count, cmode, mode);
    locate_finger(15,2);

    return;
}

/*             Function:       Change a page towards down.
	       Calls:          clr_cash_tbl,disp_cash_tbl
	       Called by:      input_data
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.9.23
 */
void pgdn_cash_tbl(UC cmode, UC mode)
{
    UC *title = "注意:";
    UC *warn  = "最多只能有这么多号码!";
    UI maxlen;

    maxlen = (mode == PHONE_CASH)? MAX_USERS:MAX_AUTHCD;

    /***** IF MORE THAN THE MAX LENGTH, WARN AND RETURN. *****/
    if( (Count+16) > maxlen)
    {
	warn_mesg(title, warn);
	return;
    }

    /***** IF END OF TABLE, RETURN. *****/
    if((Count+16) > Cash_len[mode])
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE NEXT PAGE. *****/
    Count = Count+16;
    hide_finger();
    brush_tbl(5, 16, 7);
    disp_cash_tbl(&Cash_top[mode][Count-1], Count, cmode, mode);
    locate_finger(0,2);

    return;
}

/*             Function:       Locate according to the page No..
	       Calls:          clr_cash_tbl,disp_cash_tbl
	       Called by:      input_cash_data()
	       Input:          None
               Output:         None
               Return:         None
               Date  :         1993.9.23
 */
void loc_cash_page(UC cmode, UC mode)
{
    UL locate_no=500;
    UI page_no;
    UC result;
    UC *msg="定位页号";
    UC *warn="没有该页号!";

    /***** DISPLAY THE PAGE LOCATE WINDOW. *****/
    pop_back(250, 295, 400, 335, 7);
    draw_back(330, 304, 390, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result=get_dec(335, 306, 18, 24, 0, 3, &locate_no, 0x00);

    rid_pop();

    if(!result)   /* "Esc" */
	return;

    if(!(Cash_len[mode]%16) && Cash_len[mode])
	 page_no = Cash_len[mode]/16;
    else
	 page_no = (UI)(Cash_len[mode]/16)+1;

    if(!locate_no || locate_no > page_no)
    {
	 warn_mesg("", warn);
	 return;
    }

    /***** DISPLAY THE PAGE. *****/
    hide_finger();
    brush_tbl(5, 16, 7);
    Count = (UI)(locate_no-1)*16+1;
    disp_cash_tbl(&Cash_top[mode][Count-1], Count, cmode, mode);
    locate_finger(0,2);       /* LOCATTED */

    return;
}

/* function  :    Locate the phone number using the binary search method
 * calls     :    None
 * called by :    loc_cash_tbl(), search_area_no()
 * input     :    input_no -- telephone number
 * return    :    The position of location. If not find, return NULL
 * note      :    using binary search method
 * date      :    1993.9.23
 */
CASH_PLEDGE *binary_cash(UC *input_no, UC mode)
{
    CASH_PLEDGE *tbl, *tbl_top;
    UI  tbllen;
    UI  top, bottom, middle;
    int result;

    tbl_top = Cash_top[mode];
    tbl = Cash_top[mode];
    if(Cash_len[mode] == 0)
        tbllen = cash_tbl_len(mode);
    else
        tbllen = Cash_len[mode];

    top = 0;
    bottom = tbllen-1;

    /***** Compare with the first item. *****/
    if(strcmp(    tbl->phone_no,input_no)<0    && \
       strcmp((tbl+1)->phone_no,input_no)>0)
            return(NULL);

    if(strcmp(tbl->phone_no,input_no) > 0)
            return(NULL);

    /***** Compare with the last item. *****/
    if(strcmp(tbl[bottom].phone_no,input_no) < 0)
            return(NULL);

    /***** Compare and locate in the cash table. *****/
    while(top <= bottom)
    {
        middle = (UI) (top+bottom)/2;
        tbl = &tbl_top[middle];
        result = strcmp(tbl->phone_no,input_no);

        if(!result)          /* find! */
            return(tbl);

        else if(result < 0)       /* tbl->phone_no < input_no */
            top = middle+1;
        else if(result > 0)       /* tbl->phone_no > input_no */
            bottom = middle-1;
    }

    return(NULL);                 /* not find! */
}


void indicator(UI x, UI y, UC ind_color)
{
    setcolor(ind_color);
    setfillstyle( SOLID_FILL, ind_color );
    pieslice( x, y, 0, 360, 2 );
    setcolor(8);
    arc( x, y, 45, 235, 5 );
    setcolor(15);
    arc( x, y, 236,44,  5 );
}