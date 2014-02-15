#include <string.h>
#include <mem.h>
#include <dir.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <alloc.h>
#include <math.h>
#include <graphics.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include <g_id.inc>

#include "feedef.h"

#define NOFRESH  0
#define REFRESH  1
#define ITBL_CLR 5

UI            UOldlen;

/* function  : set the user units (hu tou)
 * called by : set_data()
 * date      : 1993.11.8
 */
void set_user_unit(void)
{
    int   flag;
    UI    pg;
    TABLE_STRUCT  user_tbl = {100,99,20,18,16,4,{35,55,220,60}, ITBL_CLR};
    USERS_STRUCT  *tbl;
    struct        ffblk   user_fblk,maint_fblk;

    /* remember the date and time of users.dat and charge.pho  */
    flag = findfirst("gfsdata\\dept.dat",&user_fblk,FA_RDONLY);
    if(flag == -1)
    {
	user_fblk.ff_fdate = 0;
	user_fblk.ff_ftime = 0;
    }
    flag = findfirst(ChargeDataFile[PHONE_CASH],&maint_fblk,FA_RDONLY);
    if(flag == -1)                /* file does not exist */
    {
	message(PHO_FILE_ERR);
	return;
    }

    clr_DialWin(1);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&user_tbl);        /* draw the table frame */
    disp_user_head();             /* display table head   */
    disp_user_help();
    message_disp(8,"根据右上方弹出提示窗口提示操作");

    load_user_tbl();

    UOldlen = user_tbl_len();           /* calculate table length */
    UNewlen = UOldlen;
    Count = 1;
    tbl = User_top;

    disp_user_tbl(tbl,Count);          /* display 1th page of the rate table */

    if(!(UNewlen%16) && UNewlen)       /* calculate the pages and display    */
	 pg = UNewlen/16;
    else
	 pg = (UI)(UNewlen/16)+1;
    outf(542,397,11,14,"%2u",pg);

    locate_finger(0,1);
    input_user_data();
    unload_user_tbl();

    set_phone_user();              /* set the user unit of every telephone */

    set_user_index(&user_fblk,&maint_fblk);   /* construct user unit index */

    recover_screen(1);
    message_end();
    return;
}

/* function  : display the head information for user unit setting
 * called by : set_user_unit()
 *
 */
void disp_user_head(void)
{
	 hz16_disp(220,80,"户  头  设  定",0);   /* unit setting */

	 outf(103, 103, 7, 0, "序号");        /* seqence no   */
	 outf(142, 103, 7, 0, "户头号");      /* user No.     */
	 outf(260, 103, 7, 0, "户头名称");    /* user name    */
	 outf(417, 103, 7, 0, "分机数");      /* phone numb   */

	 return;
}

/* function  : display the help information
 * called by : set_user_unit()
 * date      : 1993.11.9
 */
void disp_user_help(void)
{
    /*** PROMPT: ***/
    draw_back1(480, 122, 625, 357, 11);
    outf(490, 142, 11, 0, "F2   存盘");
    outf(490, 162, 11, 0, "F5   删除");
    outf(490, 182, 11, 0, "F6   户头号定位");
    outf(490, 202, 11, 0, "F7   页定位");
//    outf(490, 222, 11, 0, "F9   复制前一栏");
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

/* function  : load user unit from user.dat
 * called by : set_user_unit()
 * date      : 1993.11.9
 */
void load_user_tbl(void)
{
    FILE          *fp;
    USERS_STRUCT  *tbl;
    UL            ss;
    int           i,num = 0;

    User_top = (USERS_STRUCT *)farcalloc(MAX_USERS, sizeof(USERS_STRUCT));
    if(!User_top)
    {
	ss = (UL)(MAX_USERS*sizeof(USERS_STRUCT));
	exit_scr(1,"Out of memery ERROR: %ld bytes.\n\nGFS system shutdown abnormally.\n\n\n\n",ss);
    }

    tbl = User_top;
    fp = fopen("gfsdata\\dept.dat","rb");
    if(fp != NULL)
    {
	while(fread(tbl,sizeof(USERS_STRUCT),1,fp) == 1)
	{
	     tbl++;
	     num++;
	}
	fclose(fp);
    }

    for(i=num;i<MAX_USERS;i++)
    {
	(User_top+i)->user_no = 0;
	(User_top+i)->user_name[0] = '\0';
	(User_top+i)->phones  = 0;
    }

    return;
}

/* function  : free the alloced space
 * called by : set_user_unit()
 * date      : 1993.11.9
 */
void unload_user_tbl(void)
{
       farfree(User_top);
       return;
}

/* function  : calculate the length of the user table
 * called by : set_user_unit()
 * output    : length of the user table
 * date      : 1993.11.9
 */
UI user_tbl_len(void)
{
    USERS_STRUCT  *tbl;
    UI tbllen = 0;

    tbl = User_top;

    while(tbl->phones && tbllen<MAX_USERS)
    {
	tbllen++;
	tbl++;
    }

    return(tbllen);
}

/* function  : display a page of the user unit talbe
 * input     : tbl -- the first struct of this page
 *             num -- the sequence no of the first struct of this page
 * date      : 1993.11.9
 */
void disp_user_tbl(USERS_STRUCT  *tbl, int num)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;

    for(i=0;i<16;i++)        /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > MAX_USERS)
	     break;

	 outf(105, i*19+123, 7, 0, "%-3u", num+i);
    }

    pg = (UI)(num/16)+1;    /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(534, 393, 575, 377);
    outf(542, 377, 11, 14, "%2u", pg);

    if(num == UNewlen+1)      /* the table has been added, so redisplay pg */
    {
	 bar(534, 413, 575, 397);
	 outf(542, 397, 11, 14, "%2u", pg);
    }

    /***** Display a page of the rate table. *****/
    for(i=0;i<16;i++)
    {
	 if((tbl->phones==0) || (num+i > MAX_USERS))
	     break;

	 for(j=1;j<4;j++)
	 {
	      cer_unit.unit_x = i;
	      cer_unit.unit_y = j;
	      get_certain(&cer_unit);
	      x = cer_unit.dot_sx+5;
	      y = cer_unit.dot_sy+1;

	      switch(j)
	      {
		   case 1:    /* user sequence no */
		       outf(x,y,7,0,"%-4u",tbl->user_no);
		       break;
		   case 2:    /* user name */
		       outf(x, y, 7, 0, "%-s", tbl->user_name);
		       break;
		   case 3:    /* phone number */
		       outf(x, y, 7, 0, "%-3u", tbl->phones);
		       break;
	       }/* END SWITCH */
	 }/* END FOR2 */

	 tbl++;
    }/* END FOR1 */

    return;
}

/* function  : run commands user inputs
 * date      : 1993.11.9
 */
void input_user_data(void)
{
    UI input;
    UC *title = "注意:";
    UC *warn = "最多只能有这么多户头!";
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
			 message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */
			 save_user_tbl(NOFRESH);
			 message_end();
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

		 if((Count+cur_unit.unit_x) <= UNewlen)
		 {
		     move_finger(1,1);
		     if((Count+cur_unit.unit_x) == UNewlen)
			 move_finger(2,cur_unit.unit_y-1);
		 }
		 else
		     sound_bell();
		 break;

	    case LEFT:
		 if(cur_unit.unit_y != 1)
		     move_finger(2,1);
		 else
		     sound_bell();
		 break;

	    case RIGHT:
		 if(User_top[Count+cur_unit.unit_x-1].user_no > 0)
		     move_finger(3,1);
		 else
		     sound_bell();
		 break;

	    case PAGEUP:
		 pgup_user_tbl();
		 break;

	    case PAGEDOWN:
		 pgdn_user_tbl();
		 break;

	    case F2:    /* SAVE THE RATE TABLE */
		 message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */
		 save_user_tbl(REFRESH);
		 message_end();
		 break;

	    case F5:    /* DELETE */
		 delet_user_item();
		 break;

	    case F6:   /* AREA No. LOCATE */
		 message_disp(8," 输入户头号   Enter 确认");    /*user no.*/
		 loc_user_tbl();
		 message_end();
		 break;

	    case F7:    /* PAGE No. LOCATE */
		 message_disp(8," 输入页号   Enter 确认");      /*page no.*/
		 loc_user_page();
		 message_end();
		 break;

//	    case F9:
//		 copy_last_user();
//		 break;

	    case ENTER:
		 modi_user_tbl();      /* MODIFY THE RATE TABLE */
		 break;

	    default:
		 sound_bell();
		 break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  : save the user unit setting table
 * input     : status=0 -- not refresh
 *             status=1 -- refresh
 * date      : 1993.11.9
 */
void save_user_tbl(UC status)
{
    FILE *user_file;
    USERS_STRUCT newitem;

    UNewlen = user_tbl_len();
    if(UNewlen == 0)
        return;

    /***** RESORT THE RATE TABLE *****/
    while(UOldlen < UNewlen)
    {
	 newitem = User_top[UOldlen];
	 sort_user_tbl(&newitem);
	 UOldlen += 1;
    }

    user_file = fopen("gfsdata\\dept.dat", "wb");
    fwrite(User_top, sizeof(USERS_STRUCT), UNewlen, user_file);
    fclose(user_file);

    if(status == REFRESH)
    {
	hide_finger();
        brush_tbl(4, 16, 7);
	disp_user_tbl(&User_top[Count-1], Count);   /* REDISPLAY */
	echo_finger();
    }

    return;
}

/* function  : increase and resort the user unit table
 * input     : newitem -- a new item of USERS_STRUCT to be sorted
 * date      : 1993.11.9
 */
void sort_user_tbl(USERS_STRUCT  *newitem)
{
    UI            num;
    USERS_STRUCT  *tbl,  *tblptr;

    if(newitem->user_no == 0)       /* if it is null */
	return;

    tbl = User_top;
    num = 0;

    /**** If the rate table is empty, the new item is
		  the first one in the table. ****/
    if(!UOldlen)
    {
	tbl[0] = *newitem;
	return;
    }

    /**** The new item is inserted in middle of the table. ****/
    do
    {
	 if(newitem->user_no == tbl->user_no)
	     return;

	 if(tbl->user_no > newitem->user_no)     /* INSERT HERE */
	 {
	      tblptr = &User_top[UOldlen];
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
    }while(tbl->phones && num<MAX_USERS);

    /**** The new item is the last one in the rate table. ****/
    *tbl = *newitem;
    return;
}

/* function  : locate according to the user_no
 * date      : 1993.11.9
 */
void loc_user_tbl(void)
{
    USERS_STRUCT  *tbl;
    UC result;
    UL locate_no;
    UC *msg ="定位户头号";     /* DING WEI hu tou HAO */
    UC *warn="无该户头号码!";     /* WU CI QU HAO!   */

    pop_back(250, 295, 420, 335, 7);
    draw_back(345, 304, 410, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_dec(350, 306, 18, 24, 0, 4, &locate_no, 0x00);

    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return;

    /***** Locate the position in the user table according
	       to the user number user typeed. *****/
    tbl = binary_user(locate_no);

    if(tbl != NULL)
    {
	 Count = ((UI)(tbl-User_top)/16)*16+1;
         hide_finger();
         brush_tbl(4, 16, 7);
	 disp_user_tbl(&User_top[Count-1], Count);
	 locate_finger(tbl-User_top-Count+1, 1);       /* LOCATTED */
	 return;
    }

    /***** Cann't locate, warning. *****/
    warn_mesg("", warn);
    return;
}

/* function  : modify the user table
 * date      : 1993.11.9
 */
void modi_user_tbl(void)
{
    UC res;
    UI num;
    UI xs,ys,xe,ye;
    UI result;
    UI back_in=5;
    UL input=0;
    UC name_buf[25] = "";
    UC *title = "注意:";
    UC *warn = "该户头号码已有, 请重新输入!";
    USERS_STRUCT  *tbl;
    UNIT_STRUCT cur_unit;

    hide_finger();

    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ys = cur_unit.dot_sy;

    switch(cur_unit.unit_y)
    {
	case 1:    /* user unit no */
	    if(Count+cur_unit.unit_x <= UOldlen)
	    {
		sound_bell();          /* add only, do not change */
		echo_finger();
		return;
	    }

	    message_disp(8," 输入1--9999之间的整数   Enter 确认");     /* input 1--9999*/

	    do
	    {
		input = 0;
		do
		{
		    set_get_color(0, 10, 10, 7, 0);
		    res = get_dec(xs, ys+1, 17, 55, back_in, 4, &input, 0x00);
		}while(res && (input<1 || input>9999) );

		if(!res)
		    break;

		tbl = User_top;
		num = 0;
		do
		{
		    result = tbl->user_no-(UI)input;

		    /*** THE AREA No. INPUTTED ALREADY EXIST, REINPUT ***/
		    /* User_top[Count+cur_unit.unit_x-1] is the item being
		     * editted, so not judge it
		     */
		    if((result==0) && (tbl!=&User_top[Count+cur_unit.unit_x-1]))
		    {
			UNIT_STRUCT  cur;
 	                warn_mesg(title, warn);

			/* clear the current item */
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
		}while(tbl->phones && num<MAX_USERS);
	    }while(tbl->phones && num<MAX_USERS);

	    /***** RECORD THE AREA No. AND LENGTH INPUTTED. *****/
	    if(res)
	    {
		User_top[Count+cur_unit.unit_x-1].user_no = (UI)input;
		move_finger(3,1);
	    }
	    else
		echo_finger();

	    message_end();
	    break;

	case 2:    /* user unit name */
	    strcpy(name_buf, User_top[Count+cur_unit.unit_x-1].user_name);
	    message_disp(8," 输入汉字   Enter 确认");      /*chinese char*/
	    set_get_color(0, 10, 10, 7, 0);
	    result = get_hz_str(xs, ys, 18, 220, back_in, 12, name_buf);
	    message_end();
	    if(result)
		strcpy(User_top[Count+cur_unit.unit_x-1].user_name, name_buf);
	    move_finger(3,1);
	    break;

	case 3:    /* phone number of this unit */
	    input = 0;
	    do
	    {
		message_disp(8," 输入数字   Enter 确认");      /*number*/
		set_get_color(0, 10, 10, 7, 0);
		result = get_dec(xs, ys+1, 17, 60, back_in, 3, &input, 0x00);
		message_end();
	    }while(result && (input<1 || input>MAX_USERS) );

	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		User_top[Count+cur_unit.unit_x-1].phones = (UI)input;

	    if( (Count+cur_unit.unit_x) != MAX_USERS && input )
		move_finger(1,1);
	    move_finger(2,2);
	    break;
    }/* END OF SWITCH */

    UNewlen = user_tbl_len();
    return;
}

/* function  : change a page towards up
 * date      : 1993.11.9
 */
void pgup_user_tbl(void)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-16) < 0)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(4, 16, 7);
    Count = Count-16;
    disp_user_tbl(&User_top[Count-1], Count);

    locate_finger(15,1);
}

/* function  : change a page towards down
 * date      : 1993.11.9
 */
void pgdn_user_tbl(void)
{
    UC *title = "注意:";
    UC *warn  = "最多只能有这么多户头!";

    /***** IF MORE THAN THE MAX LENGTH, WARN AND RETURN. *****/
    if(Count+15 >= MAX_USERS)
    {
        warn_mesg(title, warn);
	return;
    }

    /***** IF END OF TABLE, RETURN. *****/
    if((Count+15) > UNewlen)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE NEXT PAGE. *****/
    Count = Count+16;
    hide_finger();
    brush_tbl(4, 16, 7);
    disp_user_tbl(&User_top[Count-1], Count);

    locate_finger(0,1);
}

/* function  : delete in item in the user unit table
 * date      : 1993.11.9
 */
void delet_user_item(void)
{
    UI i;
    UI x, y, cur_pos;
    UI xs, ys, xe, ye;
    UC result;
    UNIT_STRUCT cur_unit;
    USERS_STRUCT  *tbl;

    get_current(&cur_unit);

    if((Count+cur_unit.unit_x) > UNewlen)
    {
	    sound_bell();
	    return;
    }

    /***** CLEAR THE ITEM DELETED WITH "- - -". *****/
    move_finger(2, cur_unit.unit_y-1);
    for(i = 0; i < 3; i++)
    {
	hide_finger();
	get_current(&cur_unit);
	xs = cur_unit.dot_sx;
	ys = cur_unit.dot_sy;
	xe = cur_unit.dot_ex;
	ye = cur_unit.dot_ey;
	setfillstyle(1,0);
	bar(xs,ys,xe,ye);

	x = (cur_unit.dot_ex-cur_unit.dot_sx-8)/2+cur_unit.dot_sx;
	y = (cur_unit.dot_ey-cur_unit.dot_sy-16)/2+cur_unit.dot_sy;
	outf(x, y, 0, 14, "%s", "-");

	move_finger(3,1);
    }

    /***** DISPLAY THE DELETE MENU. *****/
    move_finger(2,2);
    sound_alarm();

    result = select_1in2(10);
    /***** NOT DELETE *****/
    if(!result || result==1)
    {
	hide_finger();
        brush_tbl(4, 16, 7);
	disp_user_tbl(&User_top[Count-1], Count);
	echo_finger();
	return;
     }

     /***** DELETE A ITEM *****/
     tbl = &User_top[Count+cur_unit.unit_x-1];
     cur_pos = Count+cur_unit.unit_x;
     while(tbl->phones && (cur_pos<UNewlen) )
     {
	 *tbl= *(tbl+1);
	 tbl++;
	 cur_pos++;
     }

     memset(&User_top[UNewlen-1], 0, sizeof(USERS_STRUCT));

     UNewlen--;

     if((Count+cur_unit.unit_x-1) < UOldlen)
	 UOldlen--;

     hide_finger();
     brush_tbl(4, 16, 7);
     disp_user_tbl(&User_top[Count-1], Count);   /* REDISPLAY */

     locate_finger(0,1);
     echo_finger();
}

/* function  : locate according to the page No.
 * date      : 1993.11.9
 */
void loc_user_page(void)
{
    UL locate_no;
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
    result = get_dec(335, 306, 18, 24, 0, 3, &locate_no, 0x00);

    rid_pop();

    if(!result)   /* "Esc" */
	return;

    if(!(UNewlen%16) && UNewlen)       /* calculate the pages */
	 page_no = UNewlen/16;
    else
	 page_no = (UI)(UNewlen/16)+1;

    if(!locate_no || locate_no > page_no)
    {
         warn_mesg("", warn);
	 return;
    }

    /***** DISPLAY THE PAGE. *****/
    hide_finger();
    brush_tbl(4, 16, 7);
    Count = (UI)(locate_no-1)*16+1;
    disp_user_tbl(&User_top[Count-1], Count);

    locate_finger(0,1);       /* LOCATTED */
}

/* function  :    Locate the user no using the binary search method
 * calls     :    None
 * called by :    loc_user_tbl()
 * input     :    locate_no -- user
 * return    :    The position of location. If not find, return NULL
 * note      :    using binary search method
 * date      :    1993.11.9
 */
USERS_STRUCT *binary_user(UL locate_no)
{
    USERS_STRUCT *tbl, *tbl_top;
    UI tbllen;
    UI top, bottom, middle;

    tbl_top = User_top;
    tbl = User_top;
    tbllen = user_tbl_len();

    top = 0;
    bottom = tbllen-1;

    /***** Compare with the first item. *****/
    if( ((UL)tbl->user_no<locate_no) && ((UL)(tbl+1)->user_no>locate_no) )
	return(NULL);

    if((UL)tbl->user_no > locate_no)
	return(NULL);

    /***** Compare with the last item. *****/
    if((UL)tbl[bottom].user_no < locate_no)
	return(NULL);

    /***** Compare and locate in the fee table. *****/
    while(top <= bottom)
    {
	middle = (UI) (top+bottom)/2;
	tbl = &tbl_top[middle];

	if((UL)tbl->user_no == locate_no)            /* find! */
	    return(tbl);

	else if((UL)tbl->user_no < locate_no)       /* tbl->user_no < locate_no */
	    top = middle+1;
	else if((UL)tbl->user_no > locate_no)       /* tbl->user_no > locate_no */
	    bottom = middle-1;
    }

    return(NULL);                 /* not find! */
}


