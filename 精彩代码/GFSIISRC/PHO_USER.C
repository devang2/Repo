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
#define  ITBL_CLR     5

/* function  : input the user unit for every telephone
 * calls     :
 * called by : set_user_unit() (set_user.c)
 * output    : input the user unit and save to charge.pho
 * date      : 1993.11.9
 */
void set_phone_user(void)
{
    UI pg;
    TABLE_STRUCT pho_user_tbl = {140,99,20,18,16,3,{75,150,75},ITBL_CLR};
    PHONE_STRUCT  *tbl;

    clr_DialWin(1);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&pho_user_tbl);    /* draw the table frame */
    disp_pho_user_head();         /* display table head   */
    disp_pho_user_help();         /* display the help information */

    load_phone_tbl();

    Newlen = phone_tbl_len();
    Count = 1;
    tbl = Phone_top;

    disp_pho_user_tbl(tbl,Count);

    if(!(Newlen%16) && Newlen)
	 pg = Newlen/16;
    else
	 pg = (UI)(Newlen/16)+1;
    outf(542,397,11,14,"%2u",pg);

    locate_finger(0,2);

    input_pho_user_data();

    unload_phone_tbl();
}

/* function  : display the head Hanzi for the phone user setting table
 * called by : set_phone_user()
 * date      : 1993.11.9
 */
void disp_pho_user_head(void)
{
	 hz16_disp(215,80,"设定分机所属户头号码",0);

	 outf(160, 103, 7, 0, "序号");     /* No.         */
	 outf(265, 103, 7, 0, "分机号");   /* phone No    */
	 outf(370, 103, 7, 0, "所属户头"); /* user unit no*/

	 return;
}

/* function  : display the help information for input
 * called by : set_mon_add()
 * date      : 1993.11.9
 */
void disp_pho_user_help(void)
{
    /*** PROMPT: ***/
    draw_back1(480, 122, 625, 357, 11);
    outf(490, 142, 11, 0, "F2    存盘");
    outf(490, 162, 11, 0, "F6    分机号定位");
    outf(490, 182, 11, 0, "F7    页定位");
    outf(490, 202, 11, 0, "↓↑  移动");
    outf(490, 222, 11, 0, "PgUp  向前翻页");
    outf(490, 242, 11, 0, "PgDn  向后翻页");
    outf(490, 262, 11, 0, "Enter 输入");
    outf(490, 322, 11, 0, "Esc   退出");

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(498, 365, 607, 427, 11);
    outf(518, 377, 11, 14, "第");
    outf(575, 377, 11, 14, "页");
    outf(518, 397, 11, 14, "共");
    outf(575, 397, 11, 14, "页");
}

/* function  : display a page of user unit
 * called by : set_mon_add(), pgup_pho_user_tbl(), pgdn_pho_user_tbl()
 * input     : tbl -- the first item of this page
 *             num -- the xu-hao of the first item of this page
 * date      : 1993.11.9
 */
void disp_pho_user_tbl(PHONE_STRUCT *tbl, UI num)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;

    for(i=0; i<16; i++)        /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > MAX_USERS)
	     break;

	 outf(160, i*19+123, 7, 0, "%3u", num+i);
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

	 for(j = 1; j < 3; j++)
         {
              cer_unit.unit_x = i;
              cer_unit.unit_y = j;
              get_certain(&cer_unit);
	      x = cer_unit.dot_sx+15;
	      y = cer_unit.dot_sy+1;

	      switch(j)
	      {
		   case 1:    /* telephone no */
		       if(!strcmp(tbl->phone_no,""))
			   break;

		       outf(x,y,7,0,"%-7s",tbl->phone_no);
		       break;

		   case 2:    /* user unit */
		       outf(x,y,7,0,"%-4u",tbl->user_no);
		       break;
	      }    /* END SWITCH */
	 }         /* END FOR2   */

	 tbl++;
    }    /* END FOR1 */

    return;
}

/* function  : do as input keys
 * called by : set_month_add()
 * date      : 1993.11.9
 */
void input_pho_user_data(void)
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
			 save_pho_user_tbl();
			 return;

		     case 3:     /* QUIT AND DON'T SAVE */
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

	    case PAGEUP:
		 pgup_pho_user_tbl();
		 break;

	    case PAGEDOWN:
		 pgdn_pho_user_tbl();
		 break;

	    case F2:    /* SAVE THE user unit TABLE */
		 save_pho_user_tbl();
		 break;

	    case F6:    /* locate according to the phone No input */
		 message_disp(8," 输入分机号   Enter 确认");    /*phone no.*/
		 loc_pho_user_tbl();
		 message_end();
		 break;

	    case F7:    /* PAGE No. LOCATE */
		 message_disp(8," 输入页号   Enter 确认");      /*page no.*/
		 loc_pho_user_page();
		 message_end();
		 break;

	    case ENTER:
		 message_disp(8," 输入1--9999之间的整数  Enter 确认");     /* input 1--9999*/
		 modi_pho_user_tbl();    /* MODIFY THE user unit TABLE */
		 message_end();
		 break;

	    default:
		 sound_bell();
		 break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  : modify the user unit table
 * called by : input_pho_user_data()
 * date      : 1993.11.9
 */
void modi_pho_user_tbl(void)
{
    UI xs, ye;
    UI result;
    UI back_in=15;
    UL input_no=0;
    UNIT_STRUCT cur_unit;

    hide_finger();

    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ye = cur_unit.dot_sy;

    switch(cur_unit.unit_y)
    {
	case 2:    /* user unit */
	    set_get_color(0, 10, 10, 7, 0);
	    result = get_dec(xs,ye+1,17,75,back_in,4,&input_no,0x0);

	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
	    {
		Phone_top[Count+cur_unit.unit_x-1].user_no = (UI)input_no;
		if((Count+cur_unit.unit_x) < Newlen ) /* not the last one */
		    move_finger(1,1);
	    }
	    break;
	default:
	    sound_alarm();
	    break;
    }/* END OF SWITCH */
    echo_finger();

    return;
}

/* function  : save the user unit table
 * called by : input_pho_user_tbl()
 * date      : 1993.11.9
 */
void save_pho_user_tbl(void)
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
 * called by : input_pho_user_data()
 * date      : 1993.11.9
 * note      : using the binary-search method
 */
void loc_pho_user_tbl(void)
{
    PHONE_STRUCT *tbl;
    UC locate_no[10] = "";
    UC result;
    UC *msg="定位分机号";       /* DING WEI hua ji HAO */
    UC *warn="没有该分机号码!";    /* WU CI QU HAO!   */

    pop_back(250, 295, 420, 335, 7);
    draw_back(345, 304, 410, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_no(350, 306, 18, 24, 0, 6, locate_no, 0x00);

    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return;

    /***** Locate the position in the user unit table according
	       to the phone number user typed. *****/
    tbl = binary_search(locate_no);
    if(tbl == NULL)     /* can not find the phone_number, then warn */
    {
        warn_mesg("", warn);
	return;
    }

    Count = ((UI)(tbl-Phone_top)/16)*16+1;
    hide_finger();
    brush_tbl(3, 16, 7);
    disp_pho_user_tbl(&Phone_top[Count-1], Count);
    locate_finger(tbl-Phone_top-Count+1, 2);       /* LOCATTED */

    return;
}

/*	       Function:       Change a page towards up.
	       Calls:          clr_pho_user_tbl,disp_pho_user_tbl
	       Called by:      input_pho_user_data
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.11.9
 */
void pgup_pho_user_tbl(void)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-16) < 0)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(3, 16, 7);
    Count = Count-16;
    disp_pho_user_tbl(&Phone_top[Count-1], Count);
    locate_finger(15,2);

    return;
}

/*	       Function:       Change a page towards down.
	       Calls:          clr_pho_user_tbl,disp_pho_user_tbl
	       Called by:      input_data
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.11.9
 */
void pgdn_pho_user_tbl(void)
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
    brush_tbl(3, 16, 7);
    disp_pho_user_tbl(&Phone_top[Count-1], Count);
    locate_finger(0,2);

    return;
}

/*	       Function:       Locate according to the page No..
	       Calls:          clr_pho_user_tbl,disp_pho_user_tbl
	       Called by:      input_pho_user_data()
	       Input:          None
	       Output:         None
	       Return:         None
	       Date  :         1993.11.9
*/
void loc_pho_user_page(void)
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

    if(!(Newlen%16) && Newlen)              /* calculate the total pages */
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
    brush_tbl(3, 16, 7);
    Count = (UI)(locate_no-1)*16+1;
    disp_pho_user_tbl(&Phone_top[Count-1], Count);
    locate_finger(0,2);       /* LOCATTED */

    return;
}

