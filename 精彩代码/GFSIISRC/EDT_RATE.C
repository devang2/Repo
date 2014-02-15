/*********************************************************************
 *                                                                   *
 *    FILE NAME:     EDITRATE.C                                      *
 *    FUNCTION:      Edit the rate table on the screen.              *
 *                                       1. Input data on the whole screen.              *
 *                                       2. Locate according to users' input(area No.    *
 *                      or page No.)                                 *
 *                                       3. Increase, delete and modify area numbers,    *
 *                                          rates and names.                             *
 *                                       4. If save, must do:                            *
 *                                          (1) Record the length of area No..           *
 *                                          (2) Resort the rate table.                   *
 *                                          (3) Set trunk marks in the table.            *
 *    DATE:          1993.9.15                                                                       *
 *********************************************************************/

#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include <alloc.h>
#include <math.h>
#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

#define BK_CLR   7
#define CHR_CLR  0
#define HEAD_CLR 0
#define ITBL_CLR 5

#define NOFRESH  0
#define REFRESH  1

#define ITEMS	 8

/*---------------------------------------------------------
 ----------------------- rate_set() -----------------------
	       Function:       Set rate table.
	       Calls:          load_tbl, unload_tbl,disp_tbl,
				       disp_head,input_dat,out_prompt,
				       tbl_len
	       Called by:      main()
	       Input:          mode -- 1:      international rate table
									   2:      national rate table
									   3:      local rate table
	       Output:         None
	       Return:         None
 ----------------------------------------------------------*/
void set_rate(UC mode)
{
    UI pg;
    TABLE_STRUCT rate_tbl = {10,99,20,18,16,ITEMS,{37,73,80,59,45,59,45,59}, ITBL_CLR};
    RATE_STRUCT  huge *tbl;

    clr_DialWin(2);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&rate_tbl);        /* draw the table frame */
    disp_head(mode);              /* display table head   */
    out_prompt(mode);             /* display the help information */
    message_disp(8,"根据右上方弹出提示窗口提示操作");

    load_tbl(mode);

    Oldend = tbl_len();           /* calculate table length */
    Tbllen = Oldend;
    Count = 1;
    Flag = mode;
    tbl = Tbltop;

    disp_tbl(tbl,Count);          /* display 1th page of the rate table */

    if(!(Tbllen%16) && Tbllen)    /* calculate the pages and display    */
	 pg = Tbllen/16;
    else
	 pg = (UI)(Tbllen/16)+1;

    if(mode == DDD)
	 outf(546,397,11,14,"%3u",pg);
    else
	 outf(542,397,11,14,"%2u",pg);

    locate_finger(0,1);

    input_data(mode);

    unload_tbl();

    message_end();
    return;
}

/*---------------------------------------------------------
  --------------------- disp_head() ---------------------
	       Function:       Display the head of rate table
	       Calls:          None
	       Called by:      rate_set()
	       Input:          None
	       Output:         None
	       Return:         None
  -------------------------------------------------------*/
void disp_head(UC mode)
{
	 outf(380,  80, BK_CLR, HEAD_CLR, "单位:元");     /* ARAE NUMBER */
	 outf(13,  103, BK_CLR, HEAD_CLR, "序号");      /* No.         */
	 if(mode == LDD)
	 {
	    outf(180,  80, BK_CLR, HEAD_CLR, "费率设定(市话)");     /* ARAE NUMBER */
	    outf(70,  103, BK_CLR, HEAD_CLR, "局 号");     /* ARAE NUMBER */
	    outf(140, 103, BK_CLR, HEAD_CLR, " 局 名 ");   /* AREA NAME   */
	 }
	 else if(mode == DDD)
	 {
	    outf(164,  80, BK_CLR, HEAD_CLR, "费率设定(国内长途)");     /* ARAE NUMBER */
	    outf(70,  103, BK_CLR, HEAD_CLR, "区 号");     /* ARAE NUMBER */
	    outf(140, 103, BK_CLR, HEAD_CLR, " 区 名 ");   /* AREA NAME   */
	 }
	 else if(mode == IDD)
	 {
	    outf(164,  80, BK_CLR, HEAD_CLR, "费率设定(国际长途)");     /* ARAE NUMBER */
	    outf(70,  103, BK_CLR, HEAD_CLR, "区 号");     /* ARAE NUMBER */
	    outf(140, 103, BK_CLR, HEAD_CLR, " 区 名 ");   /* AREA NAME   */
	 }
	 outf(217, 103, BK_CLR, HEAD_CLR, "起价");  /* FIRST RATE  */
	 outf(278, 103, BK_CLR, HEAD_CLR, "秒");        /* FIRST TIME  */
	 outf(320, 103, BK_CLR, HEAD_CLR, "正常价");  /* FORMAL RATE */
	 outf(385, 103, BK_CLR, HEAD_CLR, "秒");        /* FORMAL TIME */
	 outf(421, 103, BK_CLR, HEAD_CLR, "附加费");        /* ADD FEE */
}

/*------------------------------------------------------------
  ---------------------- out_prompt() ----------------------
	       Function:       Display the prompt on the screen.
	       Calls:          None
	       Called by:      rate_set
	       Input:          None
	       Output:         None
	       Return:         None
  ----------------------------------------------------------*/
void out_prompt(UC mode)
{
    /*** PROMPT: ***/
    draw_back1(480, 102, 625, 357, 11);
    outf(490, 122, 11, 0, "F2   存盘");
    outf(490, 142, 11, 0, "F5   删除");
    outf(490, 162, 11, 0, "F6   区号定位");
    outf(490, 182, 11, 0, "F7   页定位");

    outf(490, 222, 11, 0, "F9   费率复制");
    outf(490, 242, 11, 0, "←↓→↑ 移动");
    outf(490, 262, 11, 0, "PgUp  向前翻页");
    outf(490, 282, 11, 0, "PgDn  向后翻页");
    outf(490, 302, 11, 0, "Enter 输入");
    outf(490, 322, 11, 0, "Esc   退出");

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(498, 365, 607, 427, 11);
    outf(518, 377, 11, 14, "第");
    outf(575, 377, 11, 14, "页");
    outf(518, 397, 11, 14, "共");
    outf(575, 397, 11, 14, "页");
}

/*----------------------------------------------------------
  -------------------- load_tbl() ------------------------
	       Function:       Load rate table from "intern.rat" or
			       "nation.rat" or "local.rat".
	       Calls:          tbl_len
	       Called by:      rate_set()
	       Input:          mode -- 1: international rate table
				       2: national rate table
				       3: local rate table
	       Output:         None
	       Return:         None
  --------------------------------------------------------*/
 void load_tbl(UC mode)
 {
    FILE        *fptr;
    RATE_STRUCT huge *tbl;
    UL          ss;
    int         i, num = 0;

    if(mode == IDD)
    {
	 fptr = fopen(IDDRateFileName, "rb");
	 if(!fptr)
	 {
	      fptr = fopen("gfsdata\\idd.std", "rb");
	 }
	 Max_len = MAX_INTERN;
    }
    else  if(mode == DDD)
    {
	 fptr = fopen(DDDRateFileName, "rb");
	 if(!fptr)
	 {
	     fptr = fopen("gfsdata\\ddd.std", "rb");
	 }
	 Max_len = MAX_NATION;
    }
    else  if(mode == LDD)
    {
	 fptr = fopen(LDDRateFileName, "rb");
	 if(!fptr)
	 {
	     fptr = fopen("gfsdata\\ldd.std", "rb");
	 }
	 Max_len = MAX_LOCAL;
    }

    Tbltop = (RATE_STRUCT huge *)farcalloc(Max_len, sizeof(RATE_STRUCT));
    if(!Tbltop)
    {
	ss = (UL)(Max_len*sizeof(RATE_STRUCT));
	exit_scr(1,"Out of memery ERROR: %ld bytes.\n\nGFS system shutdown abnormally.\n\n\n\n",ss);
    }

    tbl = Tbltop;

    if(fptr != NULL)
    {
	while(fread(tbl,sizeof(RATE_STRUCT),1,fptr) == 1)
	{
	    tbl++;
	    num++;
	}
	fclose(fptr);
    }

    for(i=num; i<Max_len; i++)
    {
	(Tbltop+i)->area_no[0] = '\0';
	(Tbltop+i)->length = 0;
	(Tbltop+i)->first_rate  = 0;
	(Tbltop+i)->first_time  = 60;
	(Tbltop+i)->formal_rate = 0;
	(Tbltop+i)->formal_time = 60;
	(Tbltop+i)->add_fee = 0;
    }

    Tbltop->trunk_mark = 1;

    return;
}

 /*-----------------------------------------------------
  -------------------- unload_tbl() ------------------
	       Function:       Free the space used by rate
				       table.
	       Calls:          None
	       Called by:      rate_set()
	       Input:          None
	       Output:         None
	       Return:         None
  ----------------------------------------------------*/
void unload_tbl(void)
 {
       farfree(Tbltop);
       return;
 }

/*-------------------------------------------------------
  --------------------- tbl_len() ---------------------
	       Function:       Calculate the length of rate
				       table.
	       Calls:          Nnoe
	       Called by:      rate_set,load_tbl,
				       modi_tbl
	       Input:          None
	       Output:         None
	       Return:         The length of rate table
  -----------------------------------------------------*/
UI tbl_len()
{
    RATE_STRUCT huge *tbl;
    UI tbllen = 0;

    tbl = Tbltop;

    while(tbl->length && tbllen<Max_len)
    {
	tbllen++;
	tbl++;
    }

    return(tbllen);
}

/*----------------------------------------------------------
  ------------------------ maxnum() ----------------------
	       Function:       Compare with two numbers.
	       Calls:          None
	       Called by:      modi_tbl,sort_tbl
	       Input:          num1,num2 -- two numbers
	       Output:         None
	       Return:         The bigger between the two numbers
  ----------------------------------------------------------*/
UI maxnum(UI num1, UI num2)
{
    if(num1 >= num2)
	return(num1);
    else
	return(num2);
}

/*------------------------------------------------------
  ------------------- disp_tbl() ---------------------
	       Function:       Display a page of rate table.
	       Calls:          None
	       Called by:      rate_set,pgup_tbl,pgdn_tbl,
			       loc_tbl,loc_page
	       Input:          tbl -- a pointer pointing the page
			       num -- the No. of the first item of
			              the page
	       Output:         None
	       Return:         None
  ------------------------------------------------------*/
void disp_tbl(RATE_STRUCT huge *tbl, int num)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;

    for(i=0; i<16; i++)        /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > Max_len)
	     break;

	 if(Flag == DDD)
	     outf(12, i*19+123, BK_CLR, HEAD_CLR, "%4u", num+i);
	 else
	     outf(12, i*19+123, BK_CLR, HEAD_CLR, "%3u", num+i);
    }

    pg = (UI)(num/16)+1;    /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(534, 393, 575, 377);
    if(Flag == DDD)
	outf(546, 377, 11, 14, "%3u", pg);
    else
	outf(542, 377, 11, 14, "%2u", pg);

    if(num == Tbllen+1)      /* the table has been added, so redisplay pg */
    {
	 bar(534, 413, 575, 397);
	 if(Flag == DDD)
	     outf(546, 397, 11, 14, "%3u", pg);
	 else
	     outf(542, 397, 11, 14, "%2u", pg);
    }

    /***** Display a page of the rate table. *****/
    for(i = 0; i < 16; i++)
    {
	 if(!tbl->length || (num+i > Max_len))
	     break;

	 for(j = 1; j < ITEMS; j++)
	 {
	      cer_unit.unit_x = i;
	      cer_unit.unit_y = j;
	      get_certain(&cer_unit);
	      x = cer_unit.dot_sx;
	      y = cer_unit.dot_sy+1;

	      switch(j)
	      {
		   case 1:    /* AREA No. */
		       if(!strcmp(tbl->area_no,""))
			   break;

		       outf(x+3,y,BK_CLR,CHR_CLR,"%-8s",tbl->area_no);
		       break;

		   case 2:    /* AREA NAME */
		       outf(x+2, y, BK_CLR, CHR_CLR, "%s", tbl->area_name);
		       break;
		   case 3:    /* first_RATE */
			outf(x+2, y, BK_CLR, CHR_CLR, "%6.2f", ((double)tbl->first_rate)/100);
			break;
		   case 4:    /* first time */
			outf(x+12,y,BK_CLR, CHR_CLR, "%-3u", tbl->first_time);
			break;
		   case 5:    /* formal_RATE */
			outf(x+2, y, BK_CLR, CHR_CLR, "%6.2f", ((double)tbl->formal_rate)/100);
			break;
		   case 6:    /* formal time */
			outf(x+12,y,BK_CLR, CHR_CLR, "%-3u", tbl->formal_time);
			break;
		   case 7:    /* addtional fee */
			outf(x+12, y, BK_CLR, CHR_CLR, "%4.2f", ((double)tbl->add_fee)/100);
			break;
	       }/* END SWITCH */
	 }/* END FOR2 */

	 tbl++;
    }/* END FOR1 */

    return;
}


/*-------------------------------------------------------
  ------------------- input_data() --------------------
	       Function:       Run commands user input.
	       Calls:          save_tbl,sort_tbl,loc_tbl,
				       loc_page,pgup_tbl,pgdn_tbl,
				       modi_tbl,del_tbl,set_trunk
	       Called by:      rate_set()
	       Input:          None
	       Output:         None
	       Return:         None
  -------------------------------------------------------*/
void input_data(UC mode)
{
    UI input;
    UC *title = "注意:";
    UC *warn = "最多只能有这么多区号!";
    UC result;
    UC mode1;
//    TABLE_STRUCT rate_tbl = {10,99,20,18,16,ITEMS,{37,73,80,59,45,59,45,59}, ITBL_CLR};
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
			 save_tbl(Flag,NOFRESH);
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
		 if(Count+cur_unit.unit_x == Max_len)
		 {
 	             warn_mesg(title, warn);
		     break;
		 }

		 if((Count+cur_unit.unit_x) <= Tbllen)
		 {
		     move_finger(1,1);
		     if((Count+cur_unit.unit_x) == Tbllen)
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
		 if(Tbltop[Count+cur_unit.unit_x-1].length)
		     move_finger(3,1);
		 else
		     sound_bell();
		 break;

	    case PAGEUP:
		 pgup_tbl();
		 break;

	    case PAGEDOWN:
		 pgdn_tbl();
		 break;

	    case F2:    /* SAVE THE RATE TABLE */
		 save_tbl(Flag,REFRESH);
		 break;

	    case F5:    /* DELETE */
		 delet_item();
		 break;

	    case F6:   /* AREA No. LOCATE */
		 loc_tbl();
		 break;

	    case F7:    /* PAGE No. LOCATE */
		 loc_page();
		 break;

	    case F9:
		 copy_last_item();
		 break;

	    case ENTER:
		 mode1 = 0x0;
		 modi_tbl(mode1);    /* MODIFY THE RATE TABLE */
		 break;

	    default:
		 sound_bell();
		 break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/*-----------------------------------------------------
  ------------------- save_tbl() --------------------
	       Function:       Save the rate table.
	       Calls:          None
	       Called by:      input_data()
	       Input:          mode -- IDD,DDD,LDD
			       status -- 0: not refresh
					 1: refresh
	       Output:         None
	       Return:         None
  ---------------------------------------------------*/
void save_tbl(UC mode,UC status)
{
    int  i;
    FILE *rate_file;
    UC   tbl_name[35];
    RATE_STRUCT newitem;
    RATE_STRUCT huge *tbl;

    Tbllen = tbl_len();

    message_disp(8,"正在存盘，请稍候...");      /* saving, please wait... */

    /***** RESORT THE RATE TABLE *****/
    while(Oldend < Tbllen)
    {
	 newitem = Tbltop[Oldend];
	 sort_tbl(&newitem);
	 Oldend += 1;
    }

    /***** SET TRUNK MARK IN THE TABLE *****/
    set_trunk();

    /***** SAVE THE RATE TABLE *****/
    if(mode == IDD)
	strcpy(tbl_name, IDDRateFileName);
    else  if(mode == DDD)
	strcpy(tbl_name, DDDRateFileName);
    else  if(mode == LDD)
	strcpy(tbl_name, LDDRateFileName);

    tbl = Tbltop;
    rate_file = fopen(tbl_name, "wb");
    for(i=0;i<Tbllen;i++)
    {
	fwrite(tbl, sizeof(RATE_STRUCT), 1, rate_file);
	tbl++;
    }
    fclose(rate_file);

    message_end();

    if(status == REFRESH)
    {
	hide_finger();
	brush_tbl(ITEMS, 16, 7);
	disp_tbl(&Tbltop[Count-1], Count);   /* REDISPLAY */
	echo_finger();
    }

    return;
}

/*-------------------------------------------------------------
  ------------------------ sort_tbl() -----------------------
                       Function:       Increase and resort the rate table.
		       Calls:          maxnum
                       Called by:      input_data()
                       Input:          newitem -- a new item of rate table
		       Output:         None
		       Return:         None
  -------------------------------------------------------------*/
void sort_tbl(RATE_STRUCT huge *newitem)
{
    int         result;
    UI          comp_len;
    UI          num;
    RATE_STRUCT huge *tbl, huge *tblptr;

    if(!memicmp(newitem->area_no, "", newitem->length))    /* if it is null */
        return;

    tbl = Tbltop;
    num = 0;

    /**** If the rate table is empty, the new item is
                  the first one in the table. ****/
    if(!Oldend)
    {
	tbl[0] = *newitem;
	return;
    }

    /**** The new item is inserted in middle of the table. ****/
    do
    {
         comp_len = maxnum(tbl->length, newitem->length);
	 result = memicmp(tbl->area_no, newitem->area_no, comp_len);

         if(!result)    /* result=0 : the two strings are equal */
	     return;

	 if(result > 0)       /* INSERT HERE */
	 {
	      tblptr = &Tbltop[Oldend];
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
    }while(tbl->length && num<Max_len);

    /**** The new item is the last one in the rate table. ****/
    *tbl = *newitem;
}

/*-------------------------------------------------------------
 ------------------------ set_trunk() -----------------------
               Function:       Set trunk mark in the rate table.
               Calls:      maxnum
               Called by:      input_data()
               Input:          None
	       Output:         None
	       Return:         None
 ------------------------------------------------------------*/
void set_trunk(void)
{
    RATE_STRUCT huge *tbl, huge *l_trunk;
    int result;
    UI  count,num;

    Tbllen = tbl_len();
    tbl = Tbltop;
    num = 0;
    tbl->trunk_mark = 1;      /* THE FIRST ITEM IN TABLE IS A TRUNK. */
    l_trunk = tbl;            /* THE LAST TRUNK POSITION */
    tbl++;
    num++;

    while(tbl->length && num<Max_len)       /* NOT END OF RATE TABLE */
    {
         count = l_trunk->length;
	 result = memicmp(tbl->area_no, l_trunk->area_no, count);

	 if(result)
         {
             tbl->trunk_mark = 1;    /* SET TRUNK MARK */
	     l_trunk = tbl;
         }
         else
	     tbl->trunk_mark = 0;     /* NOT TRUNK */

	 tbl++;
	 num++;
    }/* END OF WHILE */
}

/*------------------------------------------------------------
  ---------------------- loc_tbl() --------------------------
	       Function:       Locate according to the area No..
	       Calls:      maxnum,clr_tbl,disp_tbl
	       Called by:      input_data
	       Input:          None
	       Output:         None
	       Return:         None
  -----------------------------------------------------------*/
void loc_tbl(void)
{
    int  res;
    RATE_STRUCT huge *tbl;
    UC locate_no[6] = "";
    UC result;
    UI comp_len;
    UC *msg="定位区号";       /* DING WEI QU HAO */
    UC *warn="没有该区号!";    /* WU CI QU HAO!   */

    message_disp(8," 输入区号   Enter 确认");      /*district no.*/
    pop_back(250, 295, 400, 335, 7);
    draw_back(330, 304, 390, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_no(335, 306, 18, 30, 0, 6, locate_no, 0x00);

    rid_pop();
    message_end();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return;

    /***** Locate the position in the rate table according
	       to the area number user typeed. *****/
    tbl = blocate_area(locate_no);

    do
    {
	 comp_len = maxnum(tbl->length, result);
	 res = memicmp(tbl->area_no, locate_no, comp_len);

         if(!res)       /* find! */
         {
	      Count = ((UI)(tbl-Tbltop)/16)*16+1;
	      hide_finger();
	      brush_tbl(ITEMS, 16, 7);
              disp_tbl(&Tbltop[Count-1], Count);
              locate_finger(tbl-Tbltop-Count+1, 1);       /* LOCATTED */
              return;
         }

	 if(tbl > Tbltop)
	     tbl--;
    }while(!tbl->trunk_mark);

    comp_len = maxnum(tbl->length, result);
    res = memicmp(tbl->area_no, locate_no, comp_len);

    if(!res)
    {
         Count = ((UI)(tbl-Tbltop)/16)*16+1;
         hide_finger();
	 brush_tbl(ITEMS, 16, 7);
	 disp_tbl(&Tbltop[Count-1], Count);
	 locate_finger(tbl-Tbltop-Count+1, 1);       /* LOCATTED */
	 return;
    }

    /***** Cann't locate, warning. *****/
      warn_mesg("", warn);
}

/*------------------------------------------------------
  ------------------- modi_tbl() ---------------------
	       Function:       Modify the rate table.
	       Calls:          tbl_len,maxnum
	       Called by:      input_data()
	       Input:          mode -- mode of inputting data
	       Output:         None
	       Return:         None
  ----------------------------------------------------*/
void modi_tbl(UC mode)
{
    int result;
    UC  res;
    UI  num;
    UI  xs, ys, xe, ye;
    UI  comp_len;
    UI  back_in;
    UL  rate_value = 0, rate_time = 60;
    UC  areano_buf[9] = "";
    UC  name_buf[9] = "";
    UC  *title = "注意:";
    UC  *warn = "该区号已有, 请重新输入!";
    RATE_STRUCT huge *tbl;
    UNIT_STRUCT cur_unit;

    hide_finger();

    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ys = cur_unit.dot_sy;

    switch(cur_unit.unit_y)
    {
	case 1:    /* AREA NUMBER */
	    if(Count+cur_unit.unit_x <= Oldend)
	    {
		sound_bell();          /* add only, do not change */
		echo_finger();
		return;
	    }

	    do
	    {
		back_in = 1;
		strcpy(areano_buf,"");
		message_disp(8," 输入数字   Enter 确认");      /*number*/
		set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
		res = get_no(xs, ys, 18, 60, back_in, 8, areano_buf, mode);
		message_end();
		if(!res)
		    break;

		tbl = Tbltop;
		num = 0;
		do
		{
		    comp_len = maxnum(res, tbl->length);
		    result = memicmp(tbl->area_no, areano_buf, comp_len);

		    /*** THE AREA No. INPUTTED ALREADY EXIST, REINPUT ***/
		    /* Tbltop[Count+cur_unit.unit_x-1] is the item being
		     * editted, so not judge it
		     */
		    if(!result && (tbl!=&Tbltop[Count+cur_unit.unit_x-1]))
		    {
			UNIT_STRUCT   cur;
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
		}while(tbl->length && num<Max_len);
	    }while(tbl->length && num<Max_len);

	    /***** RECORD THE AREA No. AND LENGTH INPUTTED. *****/
	    if(res)
	    {
		strcpy(Tbltop[Count+cur_unit.unit_x-1].area_no, areano_buf);
		Tbltop[Count+cur_unit.unit_x-1].length = res;
		move_finger(3,1);
	    }
	    else
		echo_finger();
	    break;

	case 2:    /* RECORD AREA NAME INPUTTED */
	    strcpy(name_buf, Tbltop[Count+cur_unit.unit_x-1].area_name);

	    message_disp(8," 输入汉字   Enter 确认");      /*chinese char*/
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_hz_str(xs, ys, 18, 80, 2, 4, name_buf);
	    message_end();

	    if(result)
		strcpy(Tbltop[Count+cur_unit.unit_x-1].area_name, name_buf);

	    move_finger(3,1);
	    break;

	case 3:    /* first_RATE */
	    message_disp(8," 输入数字   Enter 确认");      /*number*/
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_money(xs, ys, 18, 59, 2, 3, &rate_value, mode);

	    message_end();
	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Tbltop[Count+cur_unit.unit_x-1].first_rate = (UI)rate_value;

	    move_finger(3,1);
	    break;

	case 4:    /* first_time */
	    message_disp(8," 输入数字   Enter 确认");      /*number*/
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_dec(xs, ys, 18, 45, 12, 3, &rate_time, mode);
	    message_end();

	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Tbltop[Count+cur_unit.unit_x-1].first_time = (UI)rate_time;

	    move_finger(3,1);
	    break;

	case 5:    /* normal_RATE */
	    message_disp(8," 输入数字   Enter 确认");      /*number*/
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_money(xs, ys, 18, 59, 2, 3, &rate_value, mode);
	    message_end();
	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Tbltop[Count+cur_unit.unit_x-1].formal_rate = (UI)rate_value;

	    move_finger(3,1);
	    break;

	case 6:    /* normal_time */
	    message_disp(8," 输入数字   Enter 确认");      /*number*/
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_dec(xs, ys, 18, 45, 12, 3, &rate_time, mode);
	    message_end();
	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Tbltop[Count+cur_unit.unit_x-1].formal_time = (UI)rate_time;

	    move_finger(3,1);
	    break;
	case 7:    /* add fee */
	    message_disp(8," 输入数字   Enter 确认");      /*number*/
	    set_get_color(0, 10, 10, BK_CLR, CHR_CLR);
	    result = get_money(xs, ys, 18, 59, 2, 1, &rate_value, mode);
	    message_end();
	    /***** IF NOT "Esc", RECORD THE RATE INPUTTED. *****/
	    if(result)
		Tbltop[Count+cur_unit.unit_x-1].add_fee = (UI)rate_value;

	    if(Count+cur_unit.unit_x!=Max_len && Count+cur_unit.unit_x!=Tbllen+1)
		move_finger(1,1);

	    move_finger(2,ITEMS-2);
	    break;

    }/* END OF SWITCH */

    Tbllen = tbl_len();
}

/*-----------------------------------------------------
  --------------------- pgup_tbl() ------------------
               Function:       Change a page towards up.
               Calls:          clr_tbl,disp_tbl
               Called by:      input_data
               Input:          None
               Output:         None
               Return:         None
  ---------------------------------------------------*/
void pgup_tbl(void)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-16) < 0)
    {
        sound_bell();
        return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(ITEMS, 16, 7);
    Count = Count-16;
    disp_tbl(&Tbltop[Count-1], Count);

    locate_finger(15,1);
}

/*------------------------------------------------------
  --------------------- pgdn_tbl() -------------------
               Function:       Change a page towards down.
               Calls:          clr_tbl,disp_tbl
               Called by:      input_data
               Input:          None
               Output:         None
	       Return:         None
  ----------------------------------------------------*/
void pgdn_tbl(void)
{
    UC *title = "注意:";
    UC *warn  = "最多只能有这么多区号!";

    /***** IF MORE THAN THE MAX LENGTH, WARN AND RETURN. *****/
    if(Count+15 >= Max_len)
    {
        warn_mesg(title, warn);
        return;
    }

    /***** IF END OF TABLE, RETURN. *****/
    if((Count+15) > Tbllen)
    {
        sound_bell();
	return;
    }

    /***** DISPLAY THE NEXT PAGE. *****/
    Count = Count+16;
    hide_finger();
    brush_tbl(ITEMS, 16, 7);
    disp_tbl(&Tbltop[Count-1], Count);

    locate_finger(0,1);
}

/*-------------------------------------------------------
  --------------------- delet_item() ------------------
               Function:       Delete a item in the rate table.
               Calls:      clr_tbl,disp_tbl
               Called by:      input_data()
               Input:          None
               Output:         None
               Return:         None
  -----------------------------------------------------*/
void delet_item(void)
{
    UI i;
    UI x, y, cur_pos;
    UI xs, ys, xe, ye;
    UC result;
    UNIT_STRUCT cur_unit;
    RATE_STRUCT huge *tbl;

    get_current(&cur_unit);

    if((Count+cur_unit.unit_x)>Tbllen)
    {
            sound_bell();
	    return;
    }

    /***** CLEAR THE ITEM DELETED WITH "- - -". *****/
    move_finger(2, cur_unit.unit_y-1);
    for(i = 0; i <= 5; i++)
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
	brush_tbl(ITEMS, 16, 7);
        disp_tbl(&Tbltop[Count-1], Count);
        echo_finger();
        return;
     }

     /***** DELETE A ITEM *****/
     tbl = &Tbltop[Count+cur_unit.unit_x-1];
     cur_pos = Count+cur_unit.unit_x;
     while((tbl->length) && (cur_pos<Tbllen) )
     {
	 *tbl= *(tbl+1);
         tbl++;
         cur_pos++;
     }

     memset(&Tbltop[Tbllen-1], 0, sizeof(RATE_STRUCT));

     Tbllen--;

     if((Count+cur_unit.unit_x-1) < Oldend)
         Oldend--;

     hide_finger();
     brush_tbl(ITEMS, 16, 7);
     disp_tbl(&Tbltop[Count-1], Count);   /* REDISPLAY */

     locate_finger(cur_unit.unit_x,1);
     echo_finger();
}

/*---------------------------------------------------------
  --------------------- loc_page() ----------------------
	       Function:       Locate according to the page No..
	       Calls:          clr_tbl,disp_tbl
	       Called by:      input_data()
	       Input:          None
	       Output:         None
	       Return:         None
  -------------------------------------------------------*/
void loc_page(void)
{
    UL locate_no;
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

    rid_pop();
    message_end();

    if(!result)   /* "Esc" */
	return;

    if(!(Tbllen%16) && Tbllen)         /* calculate the pages */
	 page_no = Tbllen/16;
    else
	 page_no = (UI)(Tbllen/16)+1;

    if(!locate_no || locate_no > page_no)
    {
         warn_mesg("", warn);
         return;
    }

    /***** DISPLAY THE PAGE. *****/
    hide_finger();
    brush_tbl(ITEMS, 16, 7);
    Count = (UI)(locate_no-1)*16+1;
    disp_tbl(&Tbltop[Count-1], Count);

    locate_finger(0,1);       /* LOCATTED */
}

/*------------------------------------------------------------
  ---------------------- search_area_no() --------------------
               Function:       Search according to the inputing No..
	       Calls:      maxnum,clr_tbl,disp_tbl
               Called by:      input_data()
	       Input:          None
	       Output:         None
	       Return:         None
  -----------------------------------------------------------*/
void search_area_no(void)
{
    RATE_STRUCT huge *tbl;
    UC locate_no[6] = "";
    UC result;
    UC *msg="查找区号";       /* DING WEI QU HAO */

    pop_back(250, 295, 400, 335, 7);
    draw_back(330, 304, 390, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. USERS INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_no(335, 306, 18, 24, 0, 5, locate_no, 0x00);

    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return;

    /***** Locate the position in the rate table according
	       to the area number user typeed. *****/
    tbl = blocate_area(locate_no);
    tbl++;
    Count = ((UI)(tbl-Tbltop)/16)*16+1;
    hide_finger();
    brush_tbl(ITEMS, 16, 7);
    disp_tbl(&Tbltop[Count-1], Count);
    locate_finger(tbl-Tbltop-Count+1, 1);       /* LOCATTED */
}

 /*------------------------------------------------------
  ------------------- blocate_area() -------------------
  Function:       Locate last position smaller than input_no.
  Calls:          None
  Called by:      loc_tbl(), search_area_no()
  Input:          input_no -- telephone number
  Output:         None
  Return:         The position of location.
  Note:           the first position could not be returned, otherwise
		  in loc_tbl(), there could be error when tbl--
  ------------------------------------------------------*/
RATE_STRUCT huge *blocate_area(UC *input_no)
{
    RATE_STRUCT huge *tbl, huge *tbl_top;
    UI tbllen,comp_len,comp_len1,input_len;
    UI position;
    UI top, bottom;
    int result;

    tbl_top = Tbltop;
    tbl = Tbltop;
    tbllen = tbl_len();
    input_len = strlen(input_no);

    top = 0;
    if(tbllen > 0)
	bottom = tbllen-1;
    else
	bottom = 0;

    /***** Compare with the first item. *****/
    comp_len  = maxnum(tbl->length, input_len);
    comp_len1 = maxnum((tbl+1)->length, input_len);
    if(memicmp(tbl->area_no,input_no,comp_len)<0       &&    \
       memicmp((tbl+1)->area_no,input_no,comp_len1)>0)
            return(tbl+1);

    if(memicmp(tbl->area_no,input_no,comp_len) > 0)
            return(tbl+1);

    /***** Compare with the last item. *****/
    comp_len = maxnum(tbl[bottom].length, input_len);
    if(memicmp(tbl[bottom].area_no,input_no,comp_len) < 0)
            return(&tbl[bottom]);

    /***** Compare and locate in the rate table. *****/
    for(;;)
    {
        position = (UI) (top+bottom)/2;
        tbl = &tbl_top[position];
        comp_len = maxnum(tbl->length, input_len);
        result = memicmp(tbl->area_no,input_no,comp_len);

        if(!result)
            return(tbl);

        if(result < 0)
	{
            tbl++;

            comp_len = maxnum(tbl->length, input_len);
            result = memicmp(tbl->area_no, input_no, comp_len);

            if(!result)
                return(tbl);

            if(result > 0)
                return(tbl-1);

            top = position;           /* SEARCH DOWN */
	}
        else
        {
            tbl--;

	    comp_len = maxnum(tbl->length, input_len);
	    result = memicmp(tbl->area_no, input_no, comp_len);

            if(result <= 0)
                return(tbl);

            bottom = position;        /* SEARCH UP */
        }
    }/* END OF FOR */
}

/* function  : copy the first_rate, first_time, formal_rate and formal_time
 *             of the last item
 * called by : input_data()
 * date      : 1993.11.19
 */
void copy_last_item(void)
{
    int result;
    UC  res;
    UI  seq_no,num;
    UI  comp_len;
    UL  no;
    UC  areano_buf[9] = "";
    UC  *title = "注意:";
    UC  *warn = "该区号已有, 请重新输入!";
    RATE_STRUCT huge *tbl;
    UNIT_STRUCT cur_unit;

    get_current(&cur_unit);
    seq_no = Count+cur_unit.unit_x;    /* sequence no of item being edited */
    if(seq_no==1 || seq_no>Max_len)
        return;

    if(Tbltop[seq_no-1].length == 0)   /* have not area no yet */
    {
        no = atol(Tbltop[seq_no-2].area_no);
        no++;
        ltoa(no,areano_buf,10);
        res = strlen(areano_buf);

        /* compare with all the area no before, to see whether there is one
         * that is the same
         */
        tbl = Tbltop;
	num = 0;
	do
	{
            comp_len = maxnum(res, tbl->length);
            result = memicmp(tbl->area_no, areano_buf, comp_len);

            /*** THE AREA No. INPUTTED ALREADY EXIST, REINPUT ***/
            /* Tbltop[Count+cur_unit.unit_x-1] is the item being
             * editted, so not judge it
             */
	    if(!result && (tbl!=&Tbltop[seq_no-1]))
            {
                warn_mesg(title, warn);
		return;
            }

	    tbl++;
	    num++;
	}while(tbl->length && num<Max_len);

	strcpy(Tbltop[seq_no-1].area_no,areano_buf);
        Tbltop[seq_no-1].length = strlen(Tbltop[seq_no-1].area_no);
    }
    Tbltop[seq_no-1].first_rate  = Tbltop[seq_no-2].first_rate;
    Tbltop[seq_no-1].first_time  = Tbltop[seq_no-2].first_time;
    Tbltop[seq_no-1].formal_rate = Tbltop[seq_no-2].formal_rate;
    Tbltop[seq_no-1].formal_time = Tbltop[seq_no-2].formal_time;
    Tbltop[seq_no-1].add_fee     = Tbltop[seq_no-2].add_fee;

    hide_finger();

    clr_one_row(cur_unit.unit_x);
    disp_one_row(cur_unit.unit_x);
    if(seq_no < Max_len)
    {
	move_finger(1,1);
	move_finger(2,cur_unit.unit_y-1);
    }

    echo_finger();

    Tbllen = tbl_len();
    return;
}

/* function  : display one row of the rate table
 * date      : 1993.11.19
 */
void disp_one_row(UC row)
{
    UI j;
    UI x, y;
    UNIT_STRUCT cer_unit;
    RATE_STRUCT huge *tbl;

    tbl = Tbltop+Count+row-1;
    cer_unit.unit_x = row;
    for(j = 1; j < ITEMS; j++)
    {
        cer_unit.unit_y = j;
        get_certain(&cer_unit);
	x = cer_unit.dot_sx;
	y = cer_unit.dot_sy+1;

	switch(j)
	{
		   case 1:    /* AREA No. */
		       if(!strcmp(tbl->area_no,""))
			   break;

		       outf(x+3,y,BK_CLR,CHR_CLR,"%-8s",tbl->area_no);
		       break;

		   case 2:    /* AREA NAME */
		       outf(x+2, y, BK_CLR, CHR_CLR, "%s", tbl->area_name);
		       break;
		   case 3:    /* first_RATE */
			outf(x+2, y, BK_CLR, CHR_CLR, "%6.2f", ((double)tbl->first_rate)/100);
			break;
		   case 4:    /* first time */
			outf(x+12,y,BK_CLR, CHR_CLR, "%-3u", tbl->first_time);
			break;
		   case 5:    /* formal_RATE */
			outf(x+2, y, BK_CLR, CHR_CLR, "%6.2f", ((double)tbl->formal_rate)/100);
			break;
		   case 6:    /* formal time */
			outf(x+12,y,BK_CLR, CHR_CLR, "%-3u", tbl->formal_time);
			break;
		   case 7:    /* addtional fee */
			outf(x+12, y, BK_CLR, CHR_CLR, "%4.2f", ((double)tbl->add_fee)/100);
			break;
	 }/* END SWITCH */
    }/* END FOR2 */

    return;
}

/* function  : clear one row the rate table
 * date      : 1993.11.19
 */
void clr_one_row(UC row)
{
    UI j;
    UI xs, ys, xe, ye;
    UNIT_STRUCT cur_unit;

    cur_unit.unit_x = row;
    for(j = 1; j < ITEMS; j++)
    {
         cur_unit.unit_y = j;
         get_certain(&cur_unit);
         xs = cur_unit.dot_sx;
         ys = cur_unit.dot_sy;
         xe = cur_unit.dot_ex;
         ye = cur_unit.dot_ey;
         setfillstyle(1, BK_CLR);
         bar(xs, ys, xe, ye);
        }

    return;
}