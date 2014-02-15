#include <string.h>
#include <ctype.h>
#include <mem.h>
#include <alloc.h>
#include <graphics.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include <g_id.inc>

#include "feedef.h"

#define BK_CLR    7
#define CHR_CLR   0
#define HEAD_CLR  0
#define ITBL_CLR  5

/* function  : input the month_lease, new service, manly long call,
 *             repairing, and other fee
 * calls     :
 * called by : set_data() (set_data.c)
 * output    : input the fees and save to charge.pho
 * date      : 1993.9.22
 */
void set_month_add(void)
{
    UI pg;
    TABLE_STRUCT rate_tbl = {10,99,20,18,16,7,{48,70,68,68,68,68,68,68},ITBL_CLR};
    PHONE_STRUCT *tbl;

    clr_DialWin(2);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&rate_tbl);        /* draw the table frame */
    disp_fee_head();              /* display table head   */
    disp_fee_help();              /* display the help information */

    load_phone_tbl();

    Newlen = phone_tbl_len();
    Count  = 1;
    tbl = Phone_top;

    disp_fee_tbl(tbl,Count);

    if(!(Newlen%16) && Newlen)
	 pg = Newlen/16;
    else
	 pg = (UI)(Newlen/16)+1;
    outf(542,397,11,14,"%2u",pg);

    locate_finger(0,2);
    message_disp(8," 根据右上方弹出提示窗口提示操作");

    input_fee_data();

    unload_phone_tbl();
    message_end();
}

/* function  : display the head Hanzi for the fee table
 * called by : set_mon_add()
 * date      : 1993.9.22
 */
void disp_fee_head(void)
{
	 outf(180,  80, BK_CLR, HEAD_CLR, "月附加费设定");     /* No.         */
	 outf(20,  103, BK_CLR, HEAD_CLR, "序号");     /* No.         */
	 outf(70,  103, BK_CLR, HEAD_CLR, "分机号");   /* phone No    */
	 outf(140, 103, BK_CLR, HEAD_CLR, "月租费");   /* month lease */
	 outf(202, 103, BK_CLR, HEAD_CLR, "新业务费"); /* new service */
	 outf(270, 103, BK_CLR, HEAD_CLR, "人工长话"); /* manly long  */
	 outf(340, 103, BK_CLR, HEAD_CLR, " 维修费");  /* reparing    */
	 outf(410, 103, BK_CLR, HEAD_CLR, " 其它费");  /* others      */

	 return;
}

/* function  : display the help information for input
 * called by : set_mon_add()
 * date      : 1993.9.22
 */
void disp_fee_help(void)
{
    hz16_disp(500,103,"单位: 元",HEAD_CLR);
    /*** PROMPT: ***/
    draw_back1(480, 122, 625, 357, 11);
    outf(490, 142, 11, 0, "F2   存盘");
    outf(490, 162, 11, 0, "F6   分机号定位");
    outf(490, 182, 11, 0, "F7   页定位");
    outf(490, 202, 11, 0, "F9   复制前一栏");
    outf(490, 222, 11, 0, "←↓→↑ 移动");
    outf(490, 242, 11, 0, "PgUp  向前翻页");
    outf(490, 262, 11, 0, "PgDn  向后翻页");
    outf(490, 282, 11, 0, "Enter 输入");
    outf(490, 322, 11, 0, "Esc   退出");

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(498, 365, 607, 427, 11);
    outf(518, 377, 11, 14, "第");
    outf(575, 377, 11, 14, "页");
    outf(518, 397, 11, 14, "共");
    outf(575, 397, 11, 14, "页");
}


/* function  : display a page of fee table
 * called by : set_mon_add(), pgup_fee_tbl(), pgdn_fee_tbl()
 * input     : tbl -- the first item of this page
 *             num -- the xu-hao of the first item of this page
 * date      : 1993.9.22
 */
void disp_fee_tbl(PHONE_STRUCT *tbl, UI num)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;

    for(i=0; i<16; i++)        /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > MAX_USERS)
	     break;

	 outf(20, i*19+123, BK_CLR, HEAD_CLR, "%3u", num+i);
    }

    pg = (UI)(num/16)+1;    /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(534, 393, 575, 377);
    outf(542, 377, 11, 14, "%2u", pg);

    /***** Display a page of the rate table. *****/
    for(i = 0; i < 16; i++)
    {
	 if(!strlen(tbl->phone_no) || (num+i > MAX_USERS))
	     break;
	 for(j = 1; j < 7; j++)
	 {
	      cer_unit.unit_x = i;
	      cer_unit.unit_y = j;
	      get_certain(&cer_unit);
	      x = cer_unit.dot_sx;
	      y = cer_unit.dot_sy+1;

	      switch(j)
	      {
		   case 1:    /* telephone no */
		       if(!strcmp(tbl->phone_no,""))
			   break;

		       outf(x+6,y,BK_CLR,CHR_CLR,"%-s",tbl->phone_no);
		       break;

		   case 2:    /* month lease */
		       outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->month_lease)/100);
		       break;
		   case 3:    /* new service fee */
			outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[0])/100);
			break;
		   case 4:    /* manly long call fee */
			outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[1])/100);
			break;
		   case 5:    /* reparing fee */
			outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[2])/100);
			break;
		   case 6:    /* other fee    */
			outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[3])/100);
			break;
	      }    /* END SWITCH */
	 }         /* END FOR2   */

	 tbl++;
    }    /* END FOR1 */

    return;
}

/* function  : do as input keys
 * called by : set_month_add()
 * date      : 1993.9.23
 */
void input_fee_data(void)
{
    UI input;
    UC *title = "注意:";
    UC *warn = "最多只能有这么多分机号码!";
    UC result;
    UNIT_STRUCT cur_unit;


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
			 save_fee_tbl();
			 recover_screen(2);
			 return;

		     case 3:     /* QUIT AND DON'T SAVE */
			 recover_screen(2);
			 return;
		 }
		 break;

	    case UP:    /* GO TO THE LAST ROW */
		move_finger(0,1);
		break;

	    case DOWN:  /* GO TO THE NEXT ROW */
		 if(Count+cur_unit.unit_x == MAX_USERS)
		 {
	              warn_mesg(title, warn);
		      break;
		 }

		 if((Count+cur_unit.unit_x) < Newlen)
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
		 if( strlen(Phone_top[Count+cur_unit.unit_x-1].phone_no) )
		     move_finger(3,1);
		 else
		     sound_bell();
		 break;

	    case PAGEUP:
		 pgup_fee_tbl();
		 break;

	    case PAGEDOWN:
		 pgdn_fee_tbl();
		 break;

	    case F2:    /* SAVE THE fee TABLE */
		 save_fee_tbl();
		 break;

	    case F6:    /* locate according to the phone No input */
		 loc_fee_tbl();
		 break;

	    case F7:    /* PAGE No. LOCATE */
		 loc_fee_page();
		 break;

	    case F9:    /* COPY the last item */
		 copy_last_afee();
		 break;

	    case ENTER:
		 modi_fee_tbl();    /* MODIFY THE fee TABLE */
		 break;

	    default:
		 sound_bell();
		 break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  : modify the fee table
 * called by : input_fee_data()
 * date      : 1993.9.23
 */
void modi_fee_tbl(void)
{
    UI xs, ye;
    UI result;
    UI back_in=4;
    UL lease, add;
    UC cur_y;
    UNIT_STRUCT cur_unit;

    hide_finger();

    message_disp(8," 输入数字   Enter 确认");      /*number*/
    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ye = cur_unit.dot_sy;

    switch(cur_unit.unit_y)
    {
	case 2:    /* month lease */
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_money(xs, ye+1, 17, 57, back_in, 4, &lease, 0x0);

	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Phone_top[Count+cur_unit.unit_x-1].month_lease = lease;

	    move_finger(3,1);
	    break;

	case 3:    /* new service fee */
	case 4:    /* manly long call */
	case 5:    /* reparing fee    */
	case 6:    /* other fee       */
	    cur_y = cur_unit.unit_y;
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
            result = get_money(xs, ye+1, 17, 57, back_in, 4, &add, 0x0);

	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Phone_top[Count+cur_unit.unit_x-1].addfee[cur_y-3] = add;

	    if(cur_y != 6)        /* not the most right one */
		move_finger(3,1);
	    else if( (Count+cur_unit.unit_x) < Newlen )/* not the last one */
	    {
		move_finger(1,1);
		move_finger(2,4);
	     }
	    break;
	default:
	    sound_alarm();
	    break;
    }/* END OF SWITCH */

    Newlen = phone_tbl_len();

    message_end();
    return;
}

/* function  : save the fee table
 * called by : input_fee_tbl()
 * date      : 1993.9.23
 */
void save_fee_tbl(void)
{
    FILE *fp;

    message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */
    fp = fopen(ChargeDataFile[PHONE_CASH], "wb");
    fwrite(Phone_top, sizeof(PHONE_STRUCT), Newlen, fp);
    fclose(fp);
    message_end();
    return;
}

/* function  : locate according to phone_no
 * called by : input_fee_data()
 * date      : 1993.9.23
 * note      : using the binary-search method
 */
void loc_fee_tbl(void)
{
    PHONE_STRUCT *tbl;
    UC locate_no[7] = "";
    UC result;
    UC *msg="定位分机号";       /* DING WEI hua ji HAO */
    UC *warn="没有该分机号码!";    /* WU CI QU HAO!   */

    message_disp(8," 输入分机号   Enter 确认");    /*phone no.*/
    pop_back(250, 295, 420, 335, 7);
    draw_back(345, 304, 410, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);
    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_no(350, 306, 18, 24, 0, 6, locate_no, 0x00);
    message_end();

    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return;

    /***** Locate the position in the fee table according
	       to the phone number user typed. *****/
    tbl = binary_search(locate_no);
    if(tbl == NULL)     /* can not find the phone_number, then warn */
    {
        warn_mesg("", warn);
	return;
    }

    Count = ((UI)(tbl-Phone_top)/16)*16+1;
    hide_finger();
    brush_tbl(7, 16, 7);
    disp_fee_tbl(&Phone_top[Count-1], Count);
    locate_finger(tbl-Phone_top-Count+1, 2);       /* LOCATTED */

    return;
}


/*             Function:       Change a page towards up.
	       Calls:          clr_fee_tbl,disp_fee_tbl
	       Called by:      input_fee_data
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.9.23
 */
void pgup_fee_tbl(void)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-16) < 0)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(7, 16, 7);
    Count = Count-16;
    disp_fee_tbl(&Phone_top[Count-1], Count);
    locate_finger(15,2);

    return;
}

/*             Function:       Change a page towards down.
	       Calls:          clr_fee_tbl,disp_fee_tbl
	       Called by:      input_data
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.9.23
 */
void pgdn_fee_tbl(void)
{
    UC *title = "注意:";
    UC *warn  = "最多只能有这么多分机号码!";

    /***** IF MORE THAN THE MAX LENGTH, WARN AND RETURN. *****/
    if( (Count+16) > MAX_USERS)
    {
        warn_mesg(title, warn);
	return;
    }

    /***** IF END OF TABLE, RETURN. *****/
    if((Count+16) > Newlen)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE NEXT PAGE. *****/
    Count = Count+16;
    hide_finger();
    brush_tbl(7, 16, 7);
    disp_fee_tbl(&Phone_top[Count-1], Count);
    locate_finger(0,2);

    return;
}

/*             Function:       Locate according to the page No..
	       Calls:          clr_fee_tbl,disp_fee_tbl
	       Called by:      input_fee_data()
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.9.23
 */
void loc_fee_page(void)
{
    UL locate_no=500;
    UI page_no;
    UC result;
    UC *msg="定位页号";
    UC *warn="没有该页号!";

    /***** DISPLAY THE PAGE LOCATE WINDOW. *****/
    message_disp(8," 输入页号   Enter 确认");      /*page no.*/
    pop_back(250, 295, 400, 335, 7);
    draw_back(330, 304, 390, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result=get_dec(335, 306, 18, 24, 0, 3, &locate_no, 0x00);
    message_end();

    rid_pop();

    if(!result)   /* "Esc" */
	return;

    if(!(Newlen%16) && Newlen)         /* calculate the total pages */
	 page_no = Newlen/16;
    else
	 page_no = (UI)(Newlen/16)+1;

    if(!locate_no || locate_no > page_no)
    {
         warn_mesg("", warn);
	 return;
    }

    /***** DISPLAY THE PAGE. *****/
    hide_finger();
    brush_tbl(7, 16, 7);
    Count = (UI)(locate_no-1)*16+1;
    disp_fee_tbl(&Phone_top[Count-1], Count);
    locate_finger(0,2);       /* LOCATTED */

    return;
}

/* function  :    Locate the phone number using the binary search method
 * calls     :    None
 * called by :    loc_fee_tbl(), search_area_no()
 * input     :    input_no -- telephone number
 * return    :    The position of location. If not find, return NULL
 * note      :    using binary search method
 * date      :    1993.9.23
 */
PHONE_STRUCT *binary_search(UC *input_no)
{
    PHONE_STRUCT *tbl, *tbl_top;
    UI tbllen, top, bottom, middle;
    int result;

    tbl_top = Phone_top;
    tbl = Phone_top;
    if(Newlen == 0)
	tbllen = phone_tbl_len();
    else
	tbllen = Newlen;

    top = 0;
    bottom = tbllen-1;

    /***** Compare with the first item. *****/
    if(strcmp(tbl->phone_no, input_no) < 0 &&\
       strcmp((tbl+1)->phone_no, input_no) > 0)
	return(NULL);

    if(strcmp(tbl->phone_no,input_no) > 0)
	return(NULL);

    /***** Compare with the last item. *****/
    if(strcmp(tbl[bottom].phone_no,input_no) < 0)
	return(NULL);

    /***** Compare and locate in the fee table. *****/
    while(top <= bottom)
    {
	middle = (UI) (top+bottom)/2;
	tbl = &tbl_top[middle];
	result = strcmp(tbl->phone_no, input_no);

	if(!result)          /* find! */
	    return(tbl);

	else if(result < 0)       /* tbl->phone_no < input_no */
	    top = middle+1;
	else if(result > 0)       /* tbl->phone_no > input_no */
	    bottom = middle-1;
    }

    return(NULL);                 /* not find! */
}


void copy_last_afee(void)
{
    UC  i;
    UI  seq_no;
    UNIT_STRUCT cur_unit;

    get_current(&cur_unit);
    seq_no = Count+cur_unit.unit_x;    /* sequence no of item being edited */
    if(seq_no==1 || seq_no>MAX_USERS || \
       !strlen(Phone_top[seq_no-1].phone_no)  )
	return;

    Phone_top[seq_no-1].month_lease = Phone_top[seq_no-2].month_lease;
    for(i=0; i<4; i++)
	Phone_top[seq_no-1].addfee[i]  = Phone_top[seq_no-2].addfee[i];

    hide_finger();

    disp_one_afee(cur_unit.unit_x);
    if(seq_no < MAX_USERS)
    {
	move_finger(1,1);
	move_finger(2,cur_unit.unit_y-1);
    }
    echo_finger();

    Newlen = phone_tbl_len();
    return;
}

/* function  : display one row of the rate table
 * date      : 1993.11.19
 */
void disp_one_afee(UC row)
{
    UI j;
    UI x, y;
    UNIT_STRUCT  cer_unit;
    PHONE_STRUCT *tbl;

    tbl = Phone_top+Count+row-1;
    cer_unit.unit_x = row;

    for(j = 1; j < 7; j++)
    {
	cer_unit.unit_y = j;
	get_certain(&cer_unit);
	x = cer_unit.dot_sx;
	y = cer_unit.dot_sy+1;

	switch(j)
	{
	    case 1:    /* telephone no */
		 if(!strcmp(tbl->phone_no,""))
		     break;
		 outf(x+10,y,BK_CLR,CHR_CLR,"%-s",tbl->phone_no);
		 break;

	    case 2:    /* month lease */
		 outf(x+10,y,BK_CLR,CHR_CLR,"%6.2f",((double)tbl->month_lease)/100);
		 break;
	    case 3:    /* new service fee */
		 outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[0])/100);
		 break;
	    case 4:    /* manly long call fee */
		 outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[1])/100);
		 break;
	    case 5:    /* reparing fee */
		 outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[2])/100);
		 break;
	    case 6:    /* other fee    */
		 outf(x+2,y,BK_CLR,CHR_CLR,"%7.2f",((double)tbl->addfee[3])/100);
		 break;
	}     /* END SWITCH */
    }         /* END FOR2   */
    return;
}


