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

UI    SpecOldlen, SpecNewlen;
char  *SpecialCallFile[] = {"GFSDATA\\LOCAL.PHO","GFSDATA\\INFO.PHO",
    			 "GFSDATA\\PROHIBIT.PHO","GFSDATA\\EMERG.PHO"};
char CodeType[4] ={0xBD, 0xBC, 0xBE, 0xBF};

UC UNIT_NUM;
UC ITEM_NUM;

void set_special_call(UC mode)
{
    TABLE_STRUCT  special_tbl = {102,149,0,18,10,6,{30,65,65,65,65,65}, ITBL_CLR};

    UI pg;
    SPECIAL_CALL  *tbl;

    UNIT_NUM = special_tbl.unit_num;
    ITEM_NUM = special_tbl.item_num;

    clr_DialWin(1);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&special_tbl);        /* draw the table frame */
    hz16_disp(special_tbl.start_x+4,125,"序号",0);               /* xu hao */
    hz16_disp(special_tbl.start_x+150,125,"号    码",0);
    disp_special_key_help();       /* display the help information */
    message_disp(8,"根据右上方弹出提示窗口提示操作");

    load_special_tbl(mode);

    SpecOldlen = special_tbl_len(mode);
    SpecNewlen = SpecOldlen;
    Count = 1;
    tbl = Call_top[mode];

    disp_special_tbl(tbl,Count);          /* display a page of phone number  */

    pg = 1;
    outf(542,397,11,14,"%2u",pg);       /* display the overall page number */

    locate_finger(0,1);

    input_special_num(mode);

    unload_special_tbl(mode);

    message_end();
    return;
}

/* function  : display the help information on the screen.
 * calls     : outf()
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
void disp_special_key_help(void)
{
    /*** PROMPT: ***/
    draw_back1(480, 122, 625, 357, 11);
    outf(490, 142, 11, 0, "F2   存盘");
    outf(490, 162, 11, 0, "F5   删除");
    outf(490, 182, 11, 0, "←↓→ ↑移动");
    outf(490, 202, 11, 0, "Enter 输入");
    outf(490, 232, 11, 0, "Esc   退出");

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(498, 365, 607, 427, 11);
    outf(518, 377, 11, 14, "第");
    outf(575, 377, 11, 14, "页");
    outf(518, 397, 11, 14, "共");
    outf(575, 397, 11, 14, "页");
}

/* function  : load charge.pho
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
void load_special_tbl(UC mode)
{
    FILE         *fp;
    SPECIAL_CALL *tbl;
    UL           ss;
    int          i, num = 0;

    Call_top[mode] = (SPECIAL_CALL *) farcalloc(MAX_SPECIAL, sizeof(SPECIAL_CALL));
    if( !Call_top[mode] )
    {
	ss = (UL)(MAX_SPECIAL*sizeof(SPECIAL_CALL));
	exit_scr(1,"Out of memery ERROR: %ld bytes.\n\nGFS system shutdown abnormally.\n\n\n\n",ss);
    }
    tbl = Call_top[mode];

    fp = fopen(SpecialCallFile[mode],"rb");
    if(fp != NULL)      /* the file exists */
    {
	while(fread(tbl, sizeof(SPECIAL_CALL), 1, fp) == 1)
	{
	     tbl++;
	     num++;
	}
	fclose(fp);
    }
    for(i=num;i<MAX_SPECIAL;i++) {
	tbl->phone_no[0] = '\0';
        tbl++;
    }

    return;
}

/* function  : free the memory for the phone number table
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
void unload_special_tbl(UC mode)
{
       farfree(Call_top[mode]);

       return;
}

/* function  : calculate the length of phone number table
 * called by : set_phone_num()
 * date      : 1993.9.21
 */
UI special_tbl_len(UC mode)
{
    SPECIAL_CALL *tbl;
    UI tbllen = 0;

    tbl = Call_top[mode];

    while(strlen(tbl->phone_no) && tbllen<MAX_SPECIAL)
    {
	tbllen++;
	tbl++;
    }

    return(tbllen);
}

/* function  :     Display a page of phone num table.
 * calls     :     None
 * called by :     rate_set,pgup_special_tbl,pgdn_special_tbl,
 *			loc_special_tbl,loc_page
 * input     :     tbl -- a pointer pointing the page
 *                 num -- the No. of the first item of
 *			  the page
 * date      :     1993.9.21
 */
void disp_special_tbl(SPECIAL_CALL *tbl, UI num)
{
    UI i, j;
    UI x, y;
    UI pg,count,flag=0;
    UNIT_STRUCT cer_unit;

    for(i=0; i<UNIT_NUM; i++)      /* DISPLAY No. IN THE TABLE */
    {
	 j = num+i*(ITEM_NUM-1);
	 if(j > MAX_SPECIAL)
	     break;
	 outf(105,i*19+153,BK_CLR,0,"%3u",j);
    }

    pg = 1;     /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(534, 393, 575, 377);
    outf(542, 377, 11, 14, "%2u", pg);

    if(num == SpecNewlen+1)      /* insert the new item, then refresh */
    {
	 bar(534, 413, 575, 397);
	 outf(542, 397, 11, 14, "%2u", pg);
    }

    /***** Display a page of the rate table. *****/
    for(i=0;i<UNIT_NUM;i++)
    {
	 for(j=1;j<ITEM_NUM;j++)
	 {
	      cer_unit.unit_x = i;
	      cer_unit.unit_y = j;
	      get_certain(&cer_unit);
	      x = cer_unit.dot_sx;
	      y = cer_unit.dot_sy+1;

	      count = i*(ITEM_NUM-1)+j-1;
	      if(!strlen((tbl+count)->phone_no) || (num+count>MAX_SPECIAL))   /* empty */
	      {
		  flag = 1;
		  break;
	      }

	      outf(x+5,y,BK_CLR,0,"%-6s",(tbl+count)->phone_no);

	 }    /* END FOR2 */

	 if(flag == 1)
	     break;
    }         /* END FOR1 */

    return;
}

/* Function :       Run commands user input.
 * calls    :          save_special_tbl,sort_special_tbl,loc_special_tbl,
 *		     loc_page,pgup_special_tbl,pgdn_special_tbl,
 *		      modi_special_tbl,del_special_tbl,set_trunk
 * called by:      rate_set()
 * date     : 1993.9.21
 */
void input_special_num(UC mode)
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
	pos = Count+cur_unit.unit_x*(ITEM_NUM-1) + cur_unit.unit_y-1;

	switch(input)
	{
	    case ESC:   /* QUIT */
		 result=select_1in3();
		 switch(result)
		 {
		     case 1:     /* DON'T QUIT */
			 break;

		     case 2:     /* SAVE AND QUIT */
			 save_special_tbl(NOFRESH, mode);
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
		 if(pos > (MAX_SPECIAL-(ITEM_NUM-1)) )
		 {
	              warn_mesg(title, warn);
		      break;
		 }

		 if((pos+ITEM_NUM-1) <= SpecNewlen )
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
		 if(pos == MAX_SPECIAL)  /* already the last one */
		 {
	              warn_mesg(title, warn);
		      break;
		 }

		 if( strlen(Call_top[mode][pos-1].phone_no) )
		 {
		     if(cur_unit.unit_y < ITEM_NUM-1)
			 move_finger(3,1);            /* right a step */
		     else if(cur_unit.unit_y == ITEM_NUM-1)
		     {
			 move_finger(1,1);            /* down a step  */
			 move_finger(2,(ITEM_NUM-2));            /* left 3 steps */
		     }
		     else
			 sound_bell();
		 }
		 else
		     sound_bell();
		 break;

	    case F2:    /* SAVE THE PHONE TABLE */
		 save_special_tbl(REFRESH, mode);
		 break;

	    case F5:    /* DELETE */
		 delete_special_num(mode);
		 break;

	    case ENTER:
		 modi_special_tbl(mode);    /* MODIFY THE PHONE TABLE */
		 break;

	    default:
		    sound_bell();
		    break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  :  Modify the phone number table.
 * calls     :  special_tbl_len,maxnum
 * called by :  input_phone_num()
 * date      :
 */
void modi_special_tbl(UC mode)
{
    int result;
    UI xs,ys,xe,ye;
    UC res;
    UI num;
    UI pos;
    UI back_in=6;
    UC phone_buf[10] = "";
    UC *title = "注意:";
    UC *warn = "该号码已存在, 请重新输入!";
    SPECIAL_CALL *tbl;
    UNIT_STRUCT  cur_unit;

    hide_finger();

    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ys = cur_unit.dot_sy;

    pos = Count+cur_unit.unit_x*(ITEM_NUM-1) + cur_unit.unit_y-1;
    if(pos <= SpecOldlen)
    {
	sound_bell();          /* add only, do not change */
	echo_finger();
	return;
    }

    message_disp(8," 输入数字   Enter 确认");      /*number*/
    do
    {
	strcpy(phone_buf,"");
	set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	res = get_account(xs, ys, 18, 65, back_in, 4, phone_buf, 0);

	if(!res)        /* ESC or not change phone_buf */
	    break;

	tbl = Call_top[mode];
	num = 0;
	do
	{
	     result = strcmp(tbl->phone_no, phone_buf);

	     /*** THE phone No. INPUTTED ALREADY EXIST, REINPUT ***/
	     /* Call_top[mode][pos-1] is the item being
	      * editted, so not judge it
	      */
	     if((!result) && (tbl != &Call_top[mode][pos-1]))
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
	}while(strlen(tbl->phone_no) && num<MAX_SPECIAL);
    }while(strlen(tbl->phone_no) && num<MAX_SPECIAL);

    /***** RECORD THE phone No. AND LENGTH INPUTTED. *****/
    if(res)
    {
	strcpy(Call_top[mode][pos-1].phone_no, phone_buf);
	if(cur_unit.unit_y%ITEM_NUM == ITEM_NUM-1)
	{
	    move_finger(1,1);
	    move_finger(2,ITEM_NUM-2);
	}
	else
	    move_finger(3,1);
    }
    else
	echo_finger();

    SpecNewlen = special_tbl_len(mode);

    message_end();
    return;
}

/* function :   Save the phone num table.
 * calls    :   None
 * called by:   input_phone_num()
 * input    :   status -- 0: no refresh, 1:refresh
 * date     :
 */
void save_special_tbl(UC status, UC mode)
{
//    UC    flag;
    FILE *fp;
    SPECIAL_CALL newitem;
//    struct       ffblk   charg_fblk;

    SpecNewlen = special_tbl_len(mode);
//    if(SpecNewlen == 0)
//    {
//	flag = findfirst(SpecialCallFile[mode],&charg_fblk,FA_RDONLY);
//	if(flag == 0) {
//	    fp = fopen(SpecialCallFile[mode], "wb");
//	    fclose(fp);
//	}
//	return;
//    }

    message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */

    /***** RESORT THE PHONE TABLE *****/
    while(SpecOldlen < SpecNewlen)
    {
	 newitem = Call_top[mode][SpecOldlen];
	 sort_special_tbl(&newitem, mode);
	 SpecOldlen += 1;
    }

    /* send phone table */
    if(send_phone_table(mode)==0) {
	/***** SAVE THE PHONE TABLE *****/
	fp = fopen(SpecialCallFile[mode], "wb");
	fwrite(Call_top[mode], sizeof(SPECIAL_CALL), SpecNewlen, fp);
	fclose(fp);
    }
    else
	warn_mesg("错误","通讯错误");

    message_end();

    if(status == REFRESH)
    {
	hide_finger();
	hide_finger();
	brush_tbl(ITEM_NUM, UNIT_NUM, 7);
	disp_special_tbl(&Call_top[mode][Count-1], Count);   /* REDISPLAY */
	echo_finger();
    }

    return;
}

/* function:       Increase and resort the rate table.
 * calls:          maxnum()
 * called by:      input_phone_num()
 * input:          newitem -- a new item of phone number
 */
void sort_special_tbl(SPECIAL_CALL *newitem, UC mode)
{
    int         result;
    UI          num;
    SPECIAL_CALL *tbl, *tblptr;

    if(!strcmp(newitem->phone_no, ""))    /* if it is null */
	return;

    tbl = Call_top[mode];
    num = 0;
    /**** If the rate table is empty, the new item is
		  the first one in the table. ****/
    if(!SpecOldlen)
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
	      tblptr = &Call_top[mode][SpecOldlen];
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
    }while(strlen(tbl->phone_no) && num<MAX_SPECIAL);

    /**** The new item is the last one in the rate table. ****/
    *tbl = *newitem;
}


/* function :      Delete a item in the phone number table.
 * calls    :      clr_special_tbl,disp_special_tbl
 * called by:      input_phone_num()
 */
void delete_special_num(UC mode)
{
    UI x, y;
    UI xs, ys, xe, ye;
    UI pos,cur_pos;
    UC result;
    UNIT_STRUCT  cur_unit;
    SPECIAL_CALL *tbl;

    get_current(&cur_unit);

    pos = Count+cur_unit.unit_x*(ITEM_NUM-1) + cur_unit.unit_y-1;
    if(pos > SpecNewlen)
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
        brush_tbl(ITEM_NUM, UNIT_NUM, 7);
	disp_special_tbl(&Call_top[mode][Count-1], Count);
	echo_finger();
	return;
     }

     /***** DELETE A ITEM *****/
     tbl = &Call_top[mode][pos-1];
     cur_pos = pos;
     while( (strlen(tbl->phone_no)) && (cur_pos<SpecNewlen) )
     {
	 *tbl= *(tbl+1);
	 tbl++;
	 cur_pos++;
     }

     memset(&Call_top[mode][SpecNewlen-1], 0, sizeof(SPECIAL_CALL));
     SpecNewlen--;

     if((pos-1) < SpecOldlen)    /* equal to "pos <= SpecOldlen" */
	 SpecOldlen--;

     hide_finger();
     brush_tbl(ITEM_NUM, UNIT_NUM, 7);
     disp_special_tbl(&Call_top[mode][Count-1], Count);   /* REDISPLAY */
     echo_finger();

     return;
}

int send_phone_table(UC mode)
{
    SPECIAL_CALL *tbl;
    char tmp[10] ="000aaa";
    int  i,j;
    UC   port;

    /* shake hand with host */
    if(ShakeHandAll() != 0)
	return -1;

    tbl = Call_top[mode];

    if(SpecNewlen == 0)
    {
	    for(port=0; port<PORT_NUM; port++)
		if(Sys_mode.com_m[port])
		    DownloadCode(tmp, 6, 0, CodeType[mode], port);
    }
    else
    {
	for(i=0; i<SpecNewlen ; i++) {
	    for(j=0; j<6-strlen(tbl->phone_no); j++)
		tmp[j] = '0';
	    strcpy(&tmp[j], tbl->phone_no);
	    for(;j<6;j++)
		if(tmp[j] == '0') tmp[j] += 0x0a;
	    for(port=0; port<PORT_NUM; port++)
		if(Sys_mode.com_m[port])
		    DownloadCode(tmp, 6, 0, CodeType[mode], port);
	    tbl++;
	}
    }

    return 0;
}