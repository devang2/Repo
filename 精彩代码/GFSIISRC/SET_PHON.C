#include <string.h>
#include <stdlib.h>
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

#define NOFRESH  0
#define REFRESH  1

#define ITEM_NUM        10
#define UNUSED_CODE	2048

/* function  : input the phone number
 * calls     :
 * called by : set_data() (set_data.c)
 * output    : sort the phone number, and write to charge.pho
 * date      : 1993.9.21
 */
void set_phone_num(UC mode)
{
    UI pg;
    TABLE_STRUCT  phone_tbl = {52,99,20,18,16,ITEM_NUM, {32, 76,35,35,35,50,50,35,35,35}, ITBL_CLR};
    PHONE_STRUCT  *tbl;

    clr_DialWin(2);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&phone_tbl);        /* draw the table frame */

    if(mode == SET_PCODE)
    	outf(170, 76, BK_CLR, BLACK, "分机内码、级别及计费设定");     /* No. */
    else
   	    outf(170, 76, BK_CLR, BLACK, "立即打印分机设定");     /* No.    */

    outf(53, 103, BK_CLR, HEAD_CLR, "序号");     /* No.         */
    outf(90, 103, BK_CLR, HEAD_CLR, "话机号"); /* phone No    */

    if(mode == SET_PCODE)
    	outf(164, 103, BK_CLR, HEAD_CLR, "内码"); /* pcode now    */
    else
        outf(164, 103, BK_CLR, HEAD_CLR, "打印"); /* pcode now    */

    outf(201, 103, BK_CLR, HEAD_CLR, "级别");
    outf(236, 103, BK_CLR, HEAD_CLR, "市话");
    outf(272, 103, BK_CLR, HEAD_CLR, "农话");
    outf(324, 103, BK_CLR, HEAD_CLR, "信息台");
    outf(375, 103, BK_CLR, HEAD_CLR, "国内");
    outf(411, 103, BK_CLR, HEAD_CLR, "国际");
    outf(448, 103, BK_CLR, HEAD_CLR, "时长");

    disp_key_help();       /* display the help information */
    message_disp(8,"根据右上方弹出提示窗口提示操作");

    load_phone_tbl();

    Oldlen = phone_tbl_len();
    Newlen = Oldlen;
    Count = 1;
    tbl = Phone_top;

    disp_phone_tbl(tbl,Count, mode);          /* display a page of phone number  */

    if(!(Newlen%16) && Newlen)
	 pg = Newlen/16;
    else
	 pg = (UI)(Newlen/16)+1;
    outf(542,397,11,14,"%2u",pg);       /* display the overall page number */

    locate_finger(0,1);

    input_phone_num(mode);

    unload_phone_tbl();

    message_end();
    return;
}

/* function  : display the help information on the screen.
 * calls     : outf()
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
void disp_key_help(void)
{
    /*** PROMPT: ***/
    draw_back1(500, 102, 630, 337, 11);
    outf(505, 142, 11, 0, "F2   存盘");
    outf(505, 162, 11, 0, "F5   删除");
    outf(505, 182, 11, 0, "F9   分机号加１");
    outf(505, 202, 11, 0, "←↓→ ↑移动");
    outf(505, 222, 11, 0, "PgUp  向前翻页");
    outf(505, 242, 11, 0, "PgDn  向后翻页");
    outf(505, 262, 11, 0, "Enter 输入");
    outf(505, 302, 11, 0, "Esc   退出");

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(508, 365, 617, 427, 11);
    outf(528, 377, 11, 14, "第");
    outf(585, 377, 11, 14, "页");
    outf(528, 397, 11, 14, "共");
    outf(585, 397, 11, 14, "页");

}

/* function  : load charge.pho
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
void load_phone_tbl(void)
{
    FILE         *fp;
    PHONE_STRUCT *tbl;
    UL           ss;
    int          i, num = 0;

    if(Phone_top != NULL) /* already loaded */
    	return;

    Phone_top = (PHONE_STRUCT *) farcalloc(MAX_USERS, sizeof(PHONE_STRUCT));
    if( !Phone_top )
    {
	ss = (UL)(MAX_USERS*sizeof(PHONE_STRUCT));
	exit_scr(1,"Out of memery ERROR: %ld bytes.\n\nGFS system shutdown abnormally.\n\n\n\n",ss);
    }
    tbl = Phone_top;

    fp = fopen(ChargeDataFile[PHONE_CASH], "rb");
    if(fp != NULL)      /* the file exists */
    {
	while(fread(tbl, sizeof(PHONE_STRUCT), 1, fp) == 1)
	{
	     tbl++;
	     num++;
	}
	fclose(fp);
    }
    for(i=num;i<MAX_USERS;i++) {
	tbl->phone_no[0] = '\0';
        tbl->code = UNUSED_CODE;
	tbl->class = 0;
	tbl->feeflag[0] = 0;
	tbl->feeflag[1] = 0;
	tbl->feeflag[2] = 0;
	tbl->feeflag[3] = 0;
	tbl->feeflag[4] = 0;
    tbl->max_min = 0;
    tbl++;
    }

    return;
}

/* function  : free the memory for the phone number table
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
void unload_phone_tbl(void)
{
       farfree(Phone_top);
       Phone_top = NULL;

       return;
}

/* function  : calculate the length of phone number table
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
UI phone_tbl_len(void)
{
    PHONE_STRUCT *tbl;
    UI tbllen = 0;

    tbl = Phone_top;

    while(strlen(tbl->phone_no) && tbllen<MAX_USERS)
    {
	tbllen++;
	tbl++;
    }

    return(tbllen);
}

/* function  :     Display a page of phone num table.
 * calls     :     None
 * called by :     rate_set,pgup_phone_tbl,pgdn_phone_tbl,
 *			loc_phone_tbl,loc_page
 * input     :     tbl -- a pointer pointing the page
 *                 num -- the No. of the first item of
 *			  the page
 * date      :     1993.9.21
 */
void disp_phone_tbl(PHONE_STRUCT *tbl, UI num, UC mode)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;
    UC color;

    for(i=0; i<16; i++)          /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > MAX_USERS)
	     break;
	 outf(55, i*19+123, BK_CLR, HEAD_CLR, "%-3u", num+i);
    }

    pg = (UI)(num/16)+1;         /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(554, 393, 580, 377);
    outf(562, 377, 11, 14, "%2u", pg);

    for(i=0; i<16; i++)
    {
	 if(!strlen(tbl->phone_no) || (num+i > MAX_USERS))
	     break;

	 for(j=1;j<ITEM_NUM;j++)
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
		       outf(x-3,y,BK_CLR,CHR_CLR,"%-s",tbl->phone_no);
		       break;
		   case 2:    /* pcode */
		       if(mode == SET_PCODE)
		       {
			   if( tbl->code != UNUSED_CODE){
				if(tbl->set_flag == 1)
					color = LIGHTRED;
				else
					color = 0;

			       outf(x-8,y,BK_CLR,color,"%3u",tbl->code);
			   }
		       }
		       else
		       {
			   if(tbl->flag == FALSE)
			       outf(x,y,BK_CLR,CHR_CLR, "否");
			   else
			       outf(x,y,BK_CLR,CHR_CLR, "打");
		       }
		       break;
		   case 3:   /* class */
			if( tbl->code != UNUSED_CODE){
			    if(tbl->set_flag == 2)
				color = LIGHTRED;
			    else
				color = 0;

			    outf(x-2,y,BK_CLR,color,"%u",tbl->class);
			}
			break;
                   case 4:
                   case 5:
                   case 6:
                   case 7:
                   case 8:
                        if(!tbl->feeflag[j-4])
                            outf(x+3,y,BK_CLR,YELLOW,"否",0);
            		else
                            outf(x+3,y,BK_CLR,CHR_CLR,"计",0);
                        break;
                   case 9:   /* maxinum minutes of a call */
                        if(tbl->max_min != 0)
	   	    	    outf(x-2,y,7,0,"%u",tbl->max_min);
                   	break;

		   default:
			break;
	      }    /* END SWITCH */
	 }         /* END FOR2   */

	 tbl++;
    }    /* END FOR1 */

    return;

}

/* Function :       Run commands user input.
 * calls    :          save_phone_tbl,sort_phone_tbl,loc_phone_tbl,
 *		     loc_page,pgup_phone_tbl,pgdn_phone_tbl,
 *		      modi_phone_tbl,del_phone_tbl,set_trunk
 * called by:      rate_set()
 * date     : 1993.9.21
 */
void input_phone_num(UC mode)
{
    UI  input,pos;
    UC  *title = "注意:";
    UC  *warn = "最多只能有这么多分机号码!";
    UC  result;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);
	pos = Count+cur_unit.unit_x;

	switch(input)
	{
	    case ESC:   /* QUIT */
		 result=select_1in3();
		 switch(result)
		 {
		     case 1:     /* DON'T QUIT */
			 break;

		     case 2:     /* SAVE AND QUIT */
			 save_phone_tbl(NOFRESH, mode);
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
		 if(pos == MAX_USERS )
		 {
	              warn_mesg(title, warn);
		      break;
		 }

		 if(pos <= Newlen )
		     move_finger(1,1);
		 else
		     sound_bell();
		 break;

	    case LEFT:
		 if(cur_unit.unit_y > 1)        /* left a step   */
		     move_finger(2,1);
		 else
		     sound_bell();
		 break;

	    case RIGHT:
	         if( strlen(Phone_top[pos-1].phone_no) )
		      move_finger(3,1);            /* right a step */
		 else
		      sound_bell();
		 break;

	    case PAGEUP:
		 pgup_phone_tbl(mode);
		 break;

	    case PAGEDOWN:
		 pgdn_phone_tbl(mode);
		 break;

	    case F2:    /* SAVE THE PHONE TABLE */
		 save_phone_tbl(REFRESH, mode);
		 break;

	    case F5:    /* DELETE */
		 delete_num(mode);
		 break;

	    case F9:
		 auto_add_one(mode);
		 break;

	    case ENTER:
		 modi_phone_tbl(mode);    /* MODIFY THE PHONE TABLE */
		 break;

	    default:
		    sound_bell();
		    break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  :  Modify the phone number table.
 * calls     :  phone_tbl_len,maxnum
 * called by :  input_phone_num()
 * date      :
 */
void modi_phone_tbl(UC mode)
{
    int result;
    UI xs,ys,xe,ye;
    UC res;
    UI num;
    UI pos;
    UI back_in=3;
    UC phone_buf[10] = "";
    UC *title = "注意:";
    UC *warn = "该分机号码已有, 请重新输入!";
    PHONE_STRUCT *tbl;
    UNIT_STRUCT  cur_unit;
    UL value;

    get_current(&cur_unit);
    if(mode == SET_IMPRN && cur_unit.unit_y != 2)
        return;

    xs = cur_unit.dot_sx;
    ys = cur_unit.dot_sy;

    pos = Count+cur_unit.unit_x;

    hide_finger();

    switch(cur_unit.unit_y)
    {
        case 1:
            if(pos <= Oldlen)
            {
	        sound_bell();          /* add only, do not change */
	        echo_finger();
	        break;
            }

	    message_disp(8," 输入数字   Enter 确认");      /*number*/
	    do
	    {
		strcpy(phone_buf,"");
		set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
		res = get_account(xs, ys, 18, 60, 4, 8, phone_buf, 0);

	        if(!res)        /* ESC or not change phone_buf */
	            break;

	        tbl = Phone_top;
	        num = 0;
	        do
	        {
	            result = strcmp(tbl->phone_no, phone_buf);

	            /*** THE phone No. INPUTTED ALREADY EXIST, REINPUT ***/
	            /* Phone_top[pos-1] is the item being
	             * editted, so not judge it
	            */
	            if((!result) && (tbl != &Phone_top[pos-1]))
	            {
		         UNIT_STRUCT  cur;

			 warn_mesg(title, warn);

			 /* clear the current item */
		         get_current(&cur);
		         xs = cur.dot_sx;
		         ys = cur.dot_sy;
		         xe = cur.dot_ex;
		         ye = cur.dot_ey;
		         setfillstyle(1, BK_CLR);
		         bar(xs, ys, xe, ye);

		         break;
	             }

	             tbl++;
	             num++;
	        }while(strlen(tbl->phone_no) && num<MAX_USERS);
            }while(strlen(tbl->phone_no) && num<MAX_USERS);

            /***** RECORD THE phone No. AND LENGTH INPUTTED. *****/
	    if(res)
	    {
		strcpy(Phone_top[pos-1].phone_no, phone_buf);
                move_finger(3,1);
            }
            else
	        echo_finger();

            Newlen = phone_tbl_len();

            message_end();

            break;
	case 2:
            if(mode == SET_PCODE)
            {
//                message_disp(8,"请输入内码");     /* input inner code*/

		do {
		    do {
			value = -1;
				set_get_color(0, 10, 10, BK_CLR, LIGHTRED);
				result = get_dec(xs,ys,18,24,back_in,3,&value,0x0);
                    }while(result &&(value > MAX_USERS-1 || !Sys_mode.com_m[(UC)(value/PhonesPerPort)]));

                    tbl = Phone_top;
		    num = 0;
		    do {
		      if(tbl->code == value && tbl!=&Phone_top[Count+cur_unit.unit_x-1])
		      {
			UNIT_STRUCT  cur;
			UC *title = "注意:";
			UC *warn = "该码已有, 请重新输入!";

			warn_mesg(title, warn);

			get_current(&cur);
			xs = cur.dot_sx;
			ys = cur.dot_sy;
			xe = cur.dot_ex;
			ye = cur.dot_ey;
			setfillstyle(1, 7);
			bar(xs, ys, xe, ye);

			break;
		      }

		      tbl++;
		      num++;
		    }while (tbl->phone_no[0] && num <  MAX_USERS);
		  }while (tbl->phone_no[0] && num <  MAX_USERS);

		  if(result) {
		       Phone_top[pos-1].code = (UI)value;
		       Phone_top[pos-1].set_flag = 1;
		    }
	     }
	     else
	     {
		if(Phone_top[Count+cur_unit.unit_x-1].flag == FALSE)
		{
		    Phone_top[Count+cur_unit.unit_x-1].flag = TRUE;
		    outf(xs+8,ys+1,BK_CLR,CHR_CLR, "打");
		}
		else
		{
		    Phone_top[Count+cur_unit.unit_x-1].flag = FALSE;
		    outf(xs+8,ys+1,BK_CLR,CHR_CLR, "否");
		}
	     }

	     move_finger(3,1);
//                  echo_finger();
//                  message_end();
	    break;

	case 3:
	    message_disp(8,"0-限拨 1-市话 2-农话 3-信息台 4-国内 5-国际 Enter 确认");     /* input 0--5*/
	    value = 0;
	    do
	    {
		set_get_color(0, 10, 10, 7, LIGHTRED);
		res = get_dec(xs, ys, 18, 35, 6, 1, &value, 0x00);
	    }while(res && value>5 );

	    if(res)
	    {
		Phone_top[pos-1].class = value;
		Phone_top[pos-1].set_flag  = 2;
		move_finger(3,1);
	    }

	    echo_finger();

	    message_end();
	    break;

	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	    Phone_top[pos-1].feeflag[cur_unit.unit_y-4] =
		!(Phone_top[pos-1].feeflag[cur_unit.unit_y-4]);
	    if (!Phone_top[pos-1].feeflag[cur_unit.unit_y-4])
		outf(xs+11, ys+1, BK_CLR, YELLOW, "否", 0);
	    else
		outf(xs+11, ys+1, BK_CLR, CHR_CLR, "计", 0);

	    move_finger(3,1);
	    break;
	case 9:
		message_disp(8,"每次通话最长时间(0-不限 1--99分钟) Enter 确认");     /* input 0--5*/
		value = 0;

		set_get_color(0, 10, 10, 7, 0);
		res = get_dec(xs, ys, 18, 35, 2, 2, &value, 0x00);

	    if(res)
		{
		    Phone_top[pos-1].max_min = value;
		    Phone_top[pos-1].set_flag = 3;
		    move_finger(3,1);
		}

		move_finger(1,1);
		move_finger(2,ITEM_NUM-2);

		echo_finger();
		message_end();
	    break;

	default:
	    sound_alarm();
	    break;
    }/* END OF SWITCH */

    return;
}

/* function :      page up
 * calls    :      clr_phone_tbl,disp_phone_tbl
 * called by:      input_data()
 * date     :
 */
void pgup_phone_tbl(UC mode)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-16) < 0)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(ITEM_NUM, 16, 7);
    Count = Count-16;
    disp_phone_tbl(&Phone_top[Count-1], Count, mode);

    locate_finger(15,1);
}

/* function :      page down
 * calls    :      clr_phone_tbl,disp_phone_tbl
 * called by:      input_data()
 * date     :
 */
void pgdn_phone_tbl(UC mode)
{
    UC *title = "注意:";
    UC *warn  = "最多只能有这么多分机号码!";

    /***** IF MORE THAN THE MAX LENGTH, WARN AND RETURN. *****/
    if(Count+15 >= MAX_USERS)
    {
        warn_mesg(title, warn);
	return;
    }

    /***** IF END OF TABLE, RETURN. *****/
    if((Count+15) > Newlen)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE NEXT PAGE. *****/
    Count = Count+16;
    hide_finger();
    brush_tbl(ITEM_NUM, 16, 7);
    disp_phone_tbl(&Phone_top[Count-1], Count, mode);

    locate_finger(0,1);
}

/* function :   Save the phone num table.
 * calls    :   None
 * called by:   input_phone_num()
 * input    :   status -- 0: no refresh, 1:refresh
 *				mode   -- 0: no send     1:send
 * date     :
 */
void save_phone_tbl(UC status, UC mode)
{
    UC    flag;
    FILE *fp;
    PHONE_STRUCT newitem;
    struct       ffblk   charg_fblk;

    Newlen = phone_tbl_len();
    if(Newlen == 0)
    {
        flag = findfirst(ChargeDataFile[PHONE_CASH],&charg_fblk,FA_RDONLY);
        if(flag == 0)
            remove(ChargeDataFile[PHONE_CASH]);
        return;
    }

    message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */

    /***** RESORT THE PHONE TABLE *****/
    while(Oldlen < Newlen)
    {
	 newitem = Phone_top[Oldlen];
	 sort_phone_tbl(&newitem);
	 Oldlen += 1;
    }

    /* Download the phones */
    if(mode == SET_PCODE)
    	send_pcode_tbl();

    /***** SAVE THE PHONE TABLE *****/
    fp = fopen(ChargeDataFile[PHONE_CASH], "wb");
    fwrite(Phone_top, sizeof(PHONE_STRUCT), Newlen, fp);
    fclose(fp);

    message_end();

    if(status == REFRESH)
    {
	hide_finger();
        hide_finger();
        brush_tbl(ITEM_NUM, 16, 7);
	disp_phone_tbl(&Phone_top[Count-1], Count, mode);   /* REDISPLAY */
	echo_finger();
    }

    if(Cashflg[PHONE_CASH]) /* refresh the cash table */
	check_maint_cash(PHONE_CASH);


    return;
}

/* function:       Increase and resort the rate table.
 * calls:          maxnum()
 * called by:      input_phone_num()
 * input:          newitem -- a new item of phone number
 */
void sort_phone_tbl(PHONE_STRUCT *newitem)
{
    int         result;
    UI          num;
    PHONE_STRUCT *tbl, *tblptr;

    if(!strcmp(newitem->phone_no, ""))    /* if it is null */
	return;

    tbl = Phone_top;
    num = 0;
    /**** If the rate table is empty, the new item is
		  the first one in the table. ****/
    if(!Oldlen)
    {
	tbl[0] = *newitem;
	return;
    }

    /**** The new item is inserted in middle of the table. ****/
    do
    {
	 result = strcmp(tbl->phone_no, newitem->phone_no);

	 if(!result)    /* result=0 : the two strings are equal */
	     return;

	 if(result > 0)       /* INSERT HERE */
	 {
	      tblptr = &Phone_top[Oldlen];
	      while(tblptr > tbl)
	      {
		 *tblptr = *(tblptr-1);
		 tblptr--;
	      }

	      *tblptr = *newitem;
	      return;
	 }

	 tbl++;
	 num++;
    }while(strlen(tbl->phone_no) && num<MAX_USERS);

    /**** The new item is the last one in the rate table. ****/
    *tbl = *newitem;
}


/* function :      Delete a item in the phone number table.
 * calls    :      clr_phone_tbl,disp_phone_tbl
 * called by:      input_phone_num()
 */
void delete_num(UC mode)
{
    UI x, y;
    UI xs, ys, xe, ye;
    UI pos,cur_pos;
    UC result;
    UNIT_STRUCT  cur_unit;
    PHONE_STRUCT *tbl;

    get_current(&cur_unit);

    pos = Count+ cur_unit.unit_x;
    if(pos > Newlen)
    {
	    sound_bell();
	    return;
    }

    /***** CLEAR THE ITEM DELETED WITH "- - -". *****/
    hide_finger();
    xs = cur_unit.dot_sx;
    ys = cur_unit.dot_sy;
    xe = cur_unit.dot_ex;
    ye = cur_unit.dot_ey;
    setfillstyle(1,0);
    bar(xs,ys,xe,ye);

    x = (cur_unit.dot_ex-cur_unit.dot_sx-8)/2+cur_unit.dot_sx;
    y = (cur_unit.dot_ey-cur_unit.dot_sy-16)/2+cur_unit.dot_sy;
    outf(x-20, y, 0, 14, "%s", "- - - -");
    echo_finger();

    /***** DISPLAY THE DELETE MENU. *****/
    sound_alarm();

    result = select_1in2(10);
    /***** NOT DELETE *****/
    if(!result || result==1)
    {
	hide_finger();
        brush_tbl(ITEM_NUM, 16, 7);
	disp_phone_tbl(&Phone_top[Count-1], Count, mode);
	echo_finger();
	return;
     }

     /***** DELETE A ITEM *****/
     tbl = &Phone_top[pos-1];
     cur_pos = pos;
     while( (strlen(tbl->phone_no)) && (cur_pos<Newlen) )
     {
	 *tbl= *(tbl+1);
	 tbl++;
	 cur_pos++;
     }

     memset(&Phone_top[Newlen-1], 0, sizeof(PHONE_STRUCT));
     Newlen--;

     if((pos-1) < Oldlen)    /* equal to "pos <= Oldlen" */
	 Oldlen--;

     hide_finger();
     brush_tbl(ITEM_NUM, 16, 7);
     disp_phone_tbl(&Phone_top[Count-1], Count, mode);   /* REDISPLAY */
     echo_finger();

     return;
}

/* function  : let phone no add one automaticaly
 * date      : 1993.11.19
 */
void auto_add_one(UC mode)
{
    int result;
    UC phone_buf[10] = "";
    UC *title = "注意:";
    UC *warn = "分机号码或内码已存在，内码越界!";
    UI pos;
    UI num;
    UI code;
    UL phone;
    PHONE_STRUCT *tbl;
    UNIT_STRUCT  cur_unit;

    hide_finger();

    get_current(&cur_unit);
    pos = Count+cur_unit.unit_x;
    if((pos==1) || (pos<=Oldlen) || pos>MAX_USERS)
    {
	sound_bell();          /* add only, do not change */
	echo_finger();
	return;
    }

    /* let phone no add one automaticaly */
    phone = atol(Phone_top[pos-2].phone_no)+1;
    ltoa(phone,phone_buf,10);
    if(Phone_top[pos-2].code != UNUSED_CODE)
    {
	code = Phone_top[pos-2].code+1;
	if(!Sys_mode.com_m[code/PhonesPerPort])
            code = UNUSED_CODE;
    }
    else
        code = UNUSED_CODE;

    tbl = Phone_top;
    num = 0;
    do
    {
	result = (   strcmp(tbl->phone_no, phone_buf)
	          || code==tbl->code || !Sys_mode.com_m[code/PhonesPerPort]);

	/*** THE phone No. INPUTTED ALREADY EXIST, REINPUT ***/
	/* Phone_top[pos-1] is the item being
	 * editted, so not judge it
	 */
	if((!result) && (tbl!=&Phone_top[pos-1]))
	{
            warn_mesg(title, warn);
	    echo_finger();
	    return;
	}

	tbl++;
	num++;
    }while(strlen(tbl->phone_no) && num<MAX_USERS);

    ltoa(phone,Phone_top[pos-1].phone_no,10);
    Phone_top[pos-1].code = code;
    Phone_top[pos-1].class = Phone_top[pos-2].class;
    Phone_top[pos-1].feeflag[0] = Phone_top[pos-2].feeflag[0];
    Phone_top[pos-1].feeflag[1] = Phone_top[pos-2].feeflag[1];
    Phone_top[pos-1].feeflag[2] = Phone_top[pos-2].feeflag[2];
    Phone_top[pos-1].feeflag[3] = Phone_top[pos-2].feeflag[3];
    Phone_top[pos-1].feeflag[4] = Phone_top[pos-2].feeflag[4];
    Phone_top[pos-1].max_min = Phone_top[pos-2].max_min;
    Phone_top[pos-1].set_flag = 2;

    /* display the phone no */
    disp_phone_tbl(&Phone_top[Count-1], Count, mode);

    move_finger(1,1);

    echo_finger();

    Newlen = phone_tbl_len();

    return;
}

UC select_1in3(void)
{
    UC result;

    UC *topic = "   您想退出吗?";
    UC *select[3] = {"不退出","存盘退出","不存盘退出"};

    message_disp(8,"←↓→↑ 选择   Enter 选中"); /*left up right down*/
    set_menu(7, 0, FALSE, TRUE);
    sound_alarm();
    result = hv_menu(219, 257, 1, 3, topic, select);
    message_end();

    return result;
}

UC select_1in2(UC mode)
{
    UC result;

    UC *topic1 = "   您需要从话单库中删除已备份话单吗?";
    UC *topic2 = "   您确实需要删除已备份话单话单吗?";
    UC *topic3 = "   您需要打印电话费用单据吗?";
    UC *topic4 = "   您需要打印电话话单吗?";
    UC *topic5 = "   话费结算是否成功?";
    UC *topic6 = "   您确实需要清除话单存储器吗?";
    UC *topic7 = "   您确实需要删除全部话单吗?";
    UC *topic8 = "   您确实需要删除所有分机话单吗?";
    UC *topic9 = "   您确实需要删除所有授权人话单吗?";
    UC *topic10= "   您想删除此项吗?";

    UC *select1[2] = {"确认","取消"};
    UC *select2[2] = {"取消","确认"};
    UC *select3[2] = {"取 消", "打 印"};
    UC *select5[2] = {"打 印", "取 消"};
    UC *select4[2] = {"成  功", "不成功"};

    message_disp(8,"← → 移动   Enetr 确认");
    set_menu(7, 0, TRUE, TRUE);
    if(mode == 1)
        result = hv_menu(H_BX, H_BY, 2, 2, topic1, select1);
    else if(mode == 2)
        result = hv_menu(H_BX, H_BY, 2, 2, topic2, select2);
    else if(mode == 3)
        result = hv_menu(H_BX, H_BY, 2, 2, topic3, select3);
    else if(mode == 4)
        result = hv_menu(H_BX, H_BY, 2, 2, topic4, select3);
    else if(mode == 5)
        result = hv_menu(H_BX, H_BY, 2, 2, topic5, select4);
    else if(mode == 6)
        result = hv_menu(H_BX, H_BY, 2, 2, topic6, select2);
    else if(mode == 7)
        result = hv_menu(H_BX, H_BY, 2, 2, topic7, select2);
    else if(mode == 8)
        result = hv_menu(H_BX, H_BY, 2, 2, topic8, select2);
    else if(mode == 9)
        result = hv_menu(H_BX, H_BY, 2, 2, topic9, select2);
    else if(mode == 10)
        result = hv_menu(H_BX, H_BY, 2, 2, topic10,select2);
    else if(mode == 11)
        result = hv_menu(H_BX, H_BY, 2, 2, topic3, select5);
    message_end();

    return result;
}

void send_pcode_tbl()
{
    PHONE_STRUCT *tbl;
    UC        tmp[10];
    int i;

    /* shake hand with host */
    if(ShakeHandAll() != 0)
    	return ;

    tbl = Phone_top;
    for(i=0; i<Newlen ; i++) {
	if(tbl->set_flag && tbl->code != UNUSED_CODE) {
	    sprintf(tmp, "%02d%02d%02d", tbl->code%PhonesPerPort, (int)(tbl->class),(int)(tbl->max_min));
	    if(DownloadCode(tmp, 0, 6, 0xC0, tbl->code/PhonesPerPort) != 0) break;
	    tbl->set_flag = 0;
        }
        tbl++;
    }

    return;

}

