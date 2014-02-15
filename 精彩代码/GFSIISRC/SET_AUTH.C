#include <mem.h>
#include <dir.h>
#include <dos.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include <alloc.h>
#include <graphics.h>
#include <bios.h>
#include <time.h>

#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include <g_id.inc>
#include "feedef.h"

#define UNIT_NUM	16
#define ITEM_NUM	 4

typedef struct
{
  UC setup[3];
  UC close[3];
  UC change[3];
}AUTH_HEAD_STRUCT;

void send_auth_code(void);

/* function  : creat the authority code and the authorized unit relation
 * called by : set_data()
 * date      : 1993.11.8
 */
void process_auth_code(UC mode)
{
    UI    pg;

    TABLE_STRUCT  authcd_tbl = {102,99,20,18,UNIT_NUM,ITEM_NUM,
    				{35,86,35,64}, ITBL_CLR};
    TABLE_STRUCT  authcd_tblq = {102,99,20,18,UNIT_NUM,ITEM_NUM,
    				{35,86,35,64},QTBL_CLR};
    AUTHCD_STRUCT *tbl;

    clr_DialWin(1);

    set_finger_color(Dsp_clr.fng_clr);
    if(mode == INQ_AUTH)
	draw_table(&authcd_tblq);        /* draw the table frame */
    else
	draw_table(&authcd_tbl);        /* draw the table frame */

    disp_authcd_head(mode);             /* display table head   */
    disp_authcd_help(mode);
    message_disp(8,"¸ù¾ÝÓÒÉÏ·½µ¯³öÌáÊ¾´°¿ÚÌáÊ¾²Ù×÷");

    load_authcd_tbl(1);

    ANewlen = authcd_tbl_len();          /* calculate table length */
    Count = 1;
    tbl = Auth_cd_top;

    disp_authcd_tbl(tbl,Count);          /* display 1th page of the authority code table */

    if(!(ANewlen%UNIT_NUM) && ANewlen)         /* calculate the pages and display */
	 pg = ANewlen/UNIT_NUM;
    else
	 pg = (UI)(ANewlen/UNIT_NUM)+1;
    outf(542,397,11,14,"%2u",pg);

    locate_finger(0,1);
    process_authcd_data(mode);
    unload_authcd_tbl();

    recover_screen(1);
    message_end();

    return;
}

/* function  : display the head information for authority code setting
 * called by : set_authcd_unit()
 *
 */
void disp_authcd_head(UC mode)
{
	 if(mode==SET_AUTH)
	     hz16_disp(150,80, "ÊÚ  È¨  ÓÃ  »§  Éè  ¶¨", BLACK);   /* authcd setting */
	 else
	     hz16_disp(150,80, "ÊÚ  È¨  ÓÃ  »§  ²é  Ñ¯", BLACK);   /* authcd inquiring */

	 outf(104, 103, 7, 0, "¿¨ºÅ");        /* seqence no   */
	 outf(143, 103, 7, 0, "ÊÚÈ¨ÓÃ»§Ãû");    /* authed authcd name    */
	 outf(228, 103, 7, 0, "¼¶±ð");        /* auth class */
	 outf(270, 103, 7, 0, "ÔÂ×â·Ñ");    /* authed authcd name    */

	 return;
}

/* function  : display the help information
 * called by : set_authcd_unit()
 * date      : 1993.11.9
 */
void disp_authcd_help(UC mode)
{
    /*** PROMPT: ***/
    draw_back1(490, 122, 625, 357, 11);
    if(mode==SET_AUTH)
    {
	outf(500, 142, 11, 0, "F2   ´æÅÌ");
	outf(500, 162, 11, 0, "F5   É¾³ý");
	outf(500, 182, 11, 0, "F6   ¿¨ºÅ¶¨Î»");
	outf(500, 202, 11, 0, "F7   Ò³¶¨Î»");
	outf(500, 222, 11, 0, "¡û¡ý¡ú¡ü ÒÆ¶¯");
	outf(500, 242, 11, 0, "PgUp  ÏòÇ°·­Ò³");
	outf(500, 262, 11, 0, "PgDn  Ïòºó·­Ò³");
	outf(500, 282, 11, 0, "Enter ÊäÈë");

	outf(500, 322, 11, 0, "Esc   ÍË³ö");
    }
    else
    {
	outf(500, 142, 11, 0, "F6   ¿¨ºÅ¶¨Î»");
	outf(500, 162, 11, 0, "F7   Ò³¶¨Î»");
	outf(500, 182, 11, 0, "¡û¡ý¡ú¡ü ÒÆ¶¯");
	outf(500, 202, 11, 0, "PgUp  ÏòÇ°·­Ò³");
	outf(500, 222, 11, 0, "PgDn  Ïòºó·­Ò³");

	outf(500, 322, 11, 0, "Esc   ÍË³ö");
    }

    /*** PAGE No. AND TOTAL PAGE ***/
    draw_back1(498, 365, 607, 427, 11);
    outf(518, 377, 11, 14, "µÚ");
    outf(575, 377, 11, 14, "Ò³");
    outf(518, 397, 11, 14, "¹²");
    outf(575, 397, 11, 14, "Ò³");
}

/* function  : load authority code from authcd.dat
 * called by : set_authcd_unit()
 * input     : mode -- 0 : if there is no charge.aus file, return false
 *                     1 : if there is no charge.aus file, alloc memory
 * date      : 1993.11.9
 */
UC load_authcd_tbl(UC Alloc)
{
    FILE          *fp;
    AUTHCD_STRUCT *tbl;
    UL            ss;
    int           i, num = 0;

    if(Auth_cd_top != NULL ) /* already load */
    	return(TRUE);

    fp = fopen(ChargeDataFile[AUTH_CASH],"rb");
    if(fp == NULL && Alloc == 0)
	return(FALSE);

    Auth_cd_top = (AUTHCD_STRUCT *) farcalloc( MAX_AUTHCD , sizeof(AUTHCD_STRUCT) );
    if(!Auth_cd_top)
    {
        ss = (UL) ( MAX_AUTHCD *sizeof(AUTHCD_STRUCT) );
	exit_scr(1,"Out of memery ERROR: %ld bytes.\n\nGFS system shutdown abnormally.\n\n\n\n",ss);
    }
    tbl = Auth_cd_top;

    if(fp != NULL) {
        while(fread(tbl,sizeof(AUTHCD_STRUCT), 1, fp) == 1)
        {
             tbl++;
             num++;
        }
	fclose(fp);
    }

    for(i=num; i<MAX_AUTHCD ; i++)
    {
	tbl->auth_code = i;
	tbl->auth_class = UNUSED_AUTH;
	tbl->auth_usr_nam[0] = '\0';
	tbl->month_lease = 0;
        tbl++;
    }

    return(TRUE);
}

/* function  : free the alloced space
 * called by : set_authcd_unit()
 * date      : 1993.11.9
 */
void unload_authcd_tbl(void)
{
       farfree(Auth_cd_top);
       Auth_cd_top = NULL;
       return;
}


/* function  : calculate the length of the authcd table
 * called by : set_authcd_unit()
 * output    : length of the authcd table
 * date      : 1993.11.9
 */
UI authcd_tbl_len(void)
{
    AUTHCD_STRUCT  *tbl;
    UI tbllen = MAX_AUTHCD;

    tbl = Auth_cd_top+MAX_AUTHCD-1;

    while( (tbl->auth_class == UNUSED_AUTH) && (tbllen>0) && tbl->auth_set_flag==0)
    {
	tbllen--;
	tbl--;
    }

    return(tbllen);
}

/* function  : display a page of the authority code talbe
 * input     : tbl -- the first struct of this page
 *             num -- the sequence no of the first struct of this page
 * date      : 1993.11.9
 */
void disp_authcd_tbl(AUTHCD_STRUCT *tbl, int num)
{
    UI i, j;
    UI x, y;
    UI pg;
    UNIT_STRUCT cer_unit;
    UC color;

    for(i=0;i<UNIT_NUM;i++)        /* DISPLAY No. IN THE TABLE */
    {
	 if(num+i > MAX_AUTHCD )
	     break;

	 outf(108, i*19+123, 7, 0, "%-3u", num+i-1);
    }

    pg = (UI)(num/UNIT_NUM)+1;    /* DISPLAY THIS PAGE No. */
    setfillstyle(1,11);
    bar(534, 393, 575, 377);
    outf(542, 377, 11, 14, "%2u", pg);

    if(num == ANewlen+1)      /* the table has been added, so redisplay pg */
    {
	 bar(534, 413, 575, 397);
	 outf(542, 397, 11, 14, "%2u", pg);
    }

    /***** Display a page of the rate table. *****/
    for(i=0;i<UNIT_NUM;i++)
    {
	 if( num+i > MAX_AUTHCD )
	     break;

	 if( tbl->auth_class != UNUSED_AUTH )
         {
   	     for(j=1;j<ITEM_NUM;j++)
	     {
	          cer_unit.unit_x = i;
	          cer_unit.unit_y = j;
	          get_certain(&cer_unit);
	          x = cer_unit.dot_sx+5;
	          y = cer_unit.dot_sy+1;

		  switch(j)
	          {
		       case 1:    /* authrized user name */
		           outf(x, y, 7, 0, "%-s", tbl->auth_usr_nam);
		           break;
		       case 2:    /* authority class */
                           if(tbl->auth_set_flag)
                               color = LIGHTRED;
                           else
                               color = 0;
		           outf(x,y,7,color,"%1d", tbl->auth_class);
		           break;
		       case 3:    /* authrized user month lease */
		           outf(x, y, 7, 0, "%6.2f", ((double)tbl->month_lease)/100);
		           break;
		       default:
			   break;
	           }/* END SWITCH */
	     }/* END FOR2 */
         }
         else if (tbl->auth_set_flag) /* have not been deleted succussfully*/
         {
	     cer_unit.unit_x = i;
	     cer_unit.unit_y = 1;
	     get_certain(&cer_unit);
	     x = cer_unit.dot_sx+5;
	     y = cer_unit.dot_sy+1;

	     outf(x, y, 7, LIGHTRED, "Î´ÏÂËÍ");
         }
	 tbl++;
    }/* END FOR1 */

    return;
}

/* function  : run commands authcd inputs
 * date      : 1993.11.9
 */
void process_authcd_data(UC mode)
{
    UI input;
    UC *title = "×¢Òâ:";
    UC *warn = "×î¶àÖ»ÄÜÓÐÕâÃ´¶àÊÚÈ¨ÓÃ»§!";
    UC result;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
		 if(mode==SET_AUTH)
		 {
		     result=select_1in3();
		     switch(result)
		     {
			 case 1:     /* DON'T QUIT */
			     break;

			 case 2:     /* SAVE AND QUIT */
			     message_disp(8,"ÕýÔÚ´æÅÌ£¬ÇëÉÔºò...");      /* saving, please wait... */
			     save_authcd_tbl(NOFRESH, 1);
			     message_end();
			     return;

			 case 3:     /* QUIT AND DON'T SAVE */
			     return;
		     }
		     break;
		 }
		 else
		 {
		     return;
		 }
	    case UP:    /* GO TO THE LAST ROW */
		 move_finger(0,1);
		 break;

	    case DOWN:  /* GO TO THE NEXT ROW */
		 if(Count+cur_unit.unit_x == MAX_AUTHCD )
		 {
		      warn_mesg(title, warn);
		      break;
		 }
		 move_finger(1,1);

/*		 if((Count+cur_unit.unit_x) <= ANewlen)
		 {
		     move_finger(1,1);
		     if((Count+cur_unit.unit_x) == ANewlen)
			 move_finger(2,cur_unit.unit_y-1);
		 }
		 else
		     sound_bell();
*/		 break;

	    case LEFT:
		 if(cur_unit.unit_y != 1)
		     move_finger(2,1);
		 else
		     sound_bell();
		 break;

	    case RIGHT:
//		 if(Auth_cd_top[Count+cur_unit.unit_x-1].auth_class != UNUSED_AUTH)
		     move_finger(3,1);
//		 else
//		     sound_bell();
		 break;

	    case PAGEUP:
		 pgup_authcd_tbl();
		 break;

	    case PAGEDOWN:
		 pgdn_authcd_tbl();
		 break;

	    case F2:    /* SAVE THE RATE TABLE */
		 if(mode==SET_AUTH)
		 {
		     message_disp(8,"ÕýÔÚ´æÅÌ£¬ÇëÉÔºò...");      /* saving, please wait... */
		     save_authcd_tbl(REFRESH, 1);
		     message_end();
		 }
		 break;


	    case F5:    /* DELETE */
		 if(mode==SET_AUTH)
		     delet_authcd_item();
		 break;

	    case F6:   /* AREA No. LOCATE */
		 message_disp(8," ÊäÈë¿¨ºÅ  Enter È·ÈÏ");    /*authcd no.*/
		 loc_authcd_tbl();
		 message_end();
		 break;

	    case F7:    /* PAGE No. LOCATE */
		 message_disp(8," ÊäÈëÒ³ºÅ   Enter È·ÈÏ");      /*page no.*/
		 loc_authcd_page();
		 message_end();
		 break;

	    case ENTER:
		 if(mode==SET_AUTH)
		     modi_authcd_tbl();      /* MODIFY THE RATE TABLE */
		 break;

	    default:
		 sound_bell();
		 break;
	}/* END OF SWITCH */
    }/* END OF WHILE */
}

/* function  : save the authority code setting table
 * input     : status=0 -- not refresh
 *             status=1 -- refresh
 *             send_flag=0 -- not send
 *             send_flag=1 -- send
 * date      : 1993.11.9
 */
void save_authcd_tbl(UC status, UC send_flag)
{
    FILE *authcd_file;

    ANewlen = authcd_tbl_len();
//    if(ANewlen == 0)
//        return;

    if(send_flag)
        send_auth_code();

    authcd_file = fopen(ChargeDataFile[AUTH_CASH], "wb");
    fwrite(Auth_cd_top, sizeof(AUTHCD_STRUCT), ANewlen, authcd_file);
    fclose(authcd_file);

    if(status == REFRESH)
    {
	hide_finger();
        brush_tbl(ITEM_NUM, UNIT_NUM, 7);
	disp_authcd_tbl(&Auth_cd_top[Count-1], Count);   /* REDISPLAY */
	echo_finger();
    }

    if(Cashflg[AUTH_CASH]) /* refresh the cash table */
	check_maint_cash(AUTH_CASH);

    return;
}


/* function  : locate according to the auth_code
 * date      : 1993.11.9
 */
void loc_authcd_tbl(void)
{
    AUTHCD_STRUCT  *tbl;
    UC result;
    UL locate_no;
    UC *msg ="¶¨Î»¿¨ºÅ";     /* locate auth code */
    UC *warn="ÎÞ¸Ã¿¨ºÅ!";    /* auth code not exist */

    pop_back(250, 295, 420, 335, 7);
    draw_back(345, 304, 410, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. AUTHCD INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_dec(350, 306, 18, 24, 0, 6, &locate_no, 0x00);

    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return;

    /***** Locate the position in the authcd table according
	       to the authcd number authcd typeed. *****/
    tbl = binary_authcd(locate_no);

    if(tbl != NULL)
    {
	 Count = ((UI)(tbl-Auth_cd_top)/UNIT_NUM)*UNIT_NUM+1;
         hide_finger();
         brush_tbl(ITEM_NUM, UNIT_NUM, 7);
	 disp_authcd_tbl(&Auth_cd_top[Count-1], Count);
	 locate_finger(tbl-Auth_cd_top-Count+1, 1);       /* LOCATTED */
	 return;
    }

    /***** Cann't locate, warning. *****/
    warn_mesg("", warn);
    return;
}

/* function  : modify the authcd table
 * date      : 1993.11.9
 */
void modi_authcd_tbl(void)
{
    UC res;
    UI xs, ys;
    UI result;
    UI back_in=5;
    UL input=0;
    UL lease;
    UC name_buf[25] = "";
    UNIT_STRUCT cur_unit;

    hide_finger();

    get_current(&cur_unit);
    xs = cur_unit.dot_sx;
    ys = cur_unit.dot_sy;

    switch(cur_unit.unit_y)
    {
	case 1:
//	    strcpy(name_buf, Auth_cd_top[Count+cur_unit.unit_x-1].auth_usr_nam);
	    message_disp(8," ÊäÈëºº×Ö   Enter È·ÈÏ");
	    set_get_color(0, 10, 10, 7, 0);
	    result = get_hz_str(xs, ys, 18, 86, back_in, 5, name_buf);
	    message_end();
	    if(result)
		strcpy(Auth_cd_top[Count+cur_unit.unit_x-1].auth_usr_nam, name_buf);
	    move_finger(3,1);
	    break;

	case 2:
	    message_disp(8,"0-ÏÞ²¦ 1-ÊÐ»° 2-Å©»° 3-ÐÅÏ¢Ì¨ 4-¹úÄÚ 5-¹ú¼Ê Enter È·ÈÏ");     /* input 0--5*/
	    input = 0;
	    do
	    {
		set_get_color(0, 10, 10, 7, LIGHTRED);
		res = get_dec(xs, ys, 18, 35, back_in, 1, &input, 0x00);
	    }while(res && input>5 );

	    if(res)
	    {
		Auth_cd_top[Count+cur_unit.unit_x-1].auth_class = input;
		Auth_cd_top[Count+cur_unit.unit_x-1].auth_set_flag = 1;
		move_finger(3,1);
	    }
	    else
		echo_finger();

	    message_end();
	    break;

	case 3:
	    set_get_color(0, 10, 10, 7, 0);
	    result = get_money(xs, ys+1, 17, 64, back_in, 3, &lease, 0x00);

	    if(result)
		Auth_cd_top[Count+cur_unit.unit_x-1].month_lease = (UI)lease;

	    if( (Count+cur_unit.unit_x) < MAX_AUTHCD  )
		move_finger(1,1);
            move_finger(2,ITEM_NUM-2);
	    break;

    }/* END OF SWITCH */

    ANewlen = authcd_tbl_len();
    return;
}

/* function  : change a page towards up
 * date      : 1993.11.9
 */
void pgup_authcd_tbl(void)
{
    /***** IF THE FIRST PAGE, RETURN. *****/
    if((Count-UNIT_NUM) < 0)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE LAST PAGE. *****/
    hide_finger();
    brush_tbl(ITEM_NUM, UNIT_NUM, 7);
    Count = Count-UNIT_NUM;
    disp_authcd_tbl(&Auth_cd_top[Count-1], Count);

    locate_finger(UNIT_NUM-1,1);
}

/* function  : change a page towards down
 * date      : 1993.11.9
 */
void pgdn_authcd_tbl(void)
{
    UC *title = "×¢Òâ:";
    UC *warn  = "×î¶àÖ»ÄÜÓÐÕâÃ´¶àÊÚÈ¨ÓÃ»§!";

    /***** IF MORE THAN THE MAX LENGTH, WARN AND RETURN. *****/
    if(Count+UNIT_NUM-1 >= MAX_AUTHCD )
    {
        warn_mesg(title, warn);
	return;
    }

    /***** IF END OF TABLE, RETURN. *****/
    if((Count+UNIT_NUM-1) > ANewlen)
    {
	sound_bell();
	return;
    }

    /***** DISPLAY THE NEXT PAGE. *****/
    Count = Count+UNIT_NUM;
    hide_finger();
    brush_tbl(ITEM_NUM, UNIT_NUM, 7);
    disp_authcd_tbl(&Auth_cd_top[Count-1], Count);

    locate_finger(0,1);
}

/* function  : delete in item in the authority code table
 * date      : 1993.11.9
 */
void delet_authcd_item(void)
{
    UI i;
    UI x, y;
    UI xs, ys, xe, ye;
    UC result;
    UNIT_STRUCT cur_unit;

    get_current(&cur_unit);

    if((Count+cur_unit.unit_x) > ANewlen)
    {
	    sound_bell();
	    return;
    }

    /***** CLEAR THE ITEM DELETED WITH "- - - -". *****/
    move_finger(2, cur_unit.unit_y-1);
    for(i = 0; i < ITEM_NUM; i++)
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
        brush_tbl(ITEM_NUM, UNIT_NUM, 7);
	disp_authcd_tbl(&Auth_cd_top[Count-1], Count);
	echo_finger();
	return;
     }

     /***** DELETE A ITEM *****/
     Auth_cd_top[Count+cur_unit.unit_x-1].auth_class = UNUSED_AUTH;
     Auth_cd_top[Count+cur_unit.unit_x-1].auth_usr_nam[0] = '\0';
     Auth_cd_top[Count+cur_unit.unit_x-1].month_lease = 0;
     Auth_cd_top[Count+cur_unit.unit_x-1].auth_set_flag = 1;

     ANewlen = authcd_tbl_len();          /* calculate table length */

     hide_finger();
     brush_tbl(ITEM_NUM, UNIT_NUM, 7);
     disp_authcd_tbl(&Auth_cd_top[Count-1], Count);   /* REDISPLAY */

     locate_finger(0,1);
     echo_finger();
}

/* function  : locate according to the page No.
 * date      : 1993.11.9
 */
void loc_authcd_page(void)
{
    UL locate_no;
    UI page_no;
    UC result;
    UC *msg="¶¨Î»Ò³ºÅ";
    UC *warn="Ã»ÓÐ¸ÃÒ³ºÅ!";

    /***** DISPLAY THE PAGE LOCATE WINDOW. *****/
    pop_back(250, 295, 400, 335, 7);
    draw_back(330, 304, 390, 326, 11);
    outf(260, 308, 7, 0, "%s", msg);

    /***** LOCATE ACCODING TO THE PAGE No. AUTHCD INPUT. *****/
    set_get_color(11, 0, 12, 11, 0);
    result = get_dec(335, 306, 18, 24, 0, 3, &locate_no, 0x00);

    rid_pop();

    if(!result)   /* "Esc" */
	return;

    if(!(ANewlen%UNIT_NUM) && ANewlen)       /* calculate the pages */
	 page_no = ANewlen/UNIT_NUM;
    else
	 page_no = (UI)(ANewlen/UNIT_NUM)+1;

    if(!locate_no || locate_no > page_no)
    {
         warn_mesg("", warn);
	 return;
    }

    /***** DISPLAY THE PAGE. *****/

    hide_finger();
    brush_tbl(ITEM_NUM, UNIT_NUM, 7);
    Count = (UI)(locate_no-1)*UNIT_NUM+1;
    disp_authcd_tbl(&Auth_cd_top[Count-1], Count);

    locate_finger(0,1);       /* LOCATTED */
}

/* function  :    Locate the authcd no using the binary search method
 * calls     :    None
 * called by :    loc_authcd_tbl()
 * input     :    locate_no -- authcd
 * return    :    The position of location. If not find, return NULL
 * note      :    using binary search method
 * date      :    1993.11.9
 */
AUTHCD_STRUCT *binary_authcd(UL locate_no)
{
   if(locate_no < MAX_AUTHCD)
    {
        if(Auth_cd_top[(UI)locate_no].auth_class != UNUSED_AUTH)
            return &Auth_cd_top[(UI)locate_no];
    }

    return(NULL);                 /* not find! */
}

/* function  :    Send authority code
 * calls     :    ToCPU()
 * called by :    save_authcd_tbl(UC status)
* input     :     none
 * return    :    none
 * note      :
 * date      :    1996.01.21
 */
void send_auth_code()
{
    int           i, j;
    UC            tmp[14];
    AUTHCD_STRUCT *tbl;
    int 	  err=0;
    UC            port;
    UL		  passwd;
    char          msg[60], buf[150], *bufp;
    char *fname ="gfsdata\\authhead.dat";
    FILE *fp;
    AUTH_HEAD_STRUCT auth_head;
    struct date d;

    /* get date */
    getdate(&d);

    /* read the authority code head */
    fp = fopen(fname,"rb");
    if(fp != NULL) {
        fread(&auth_head, sizeof(AUTH_HEAD_STRUCT), 1, fp);
        fclose(fp);
    }
    else {
        message_disp(8, "ÏÈÉè¶¨ÃÜÂë×ÖÍ·");
        delay_10ms(100);
        message_end();
    }

    /* shake hand with host */
    if(ShakeHandAll() != 0) return;

    randomize(); /* initializes the random number generator */

    /* download the deleted authority codes */
    tbl = Auth_cd_top;
    for(i=0; i<ANewlen ; i++) {
    	if (tbl->auth_set_flag) {
            err = 0;
            if(tbl->auth_class == UNUSED_AUTH || tbl->auth_class == 0)
                sprintf(tmp, "099999%04d00", tbl->auth_code);
            else
	    {
                passwd = rand()*2.74+10000L; /* passwd between 10000--99999*/
                sprintf(tmp, "%06ld%04d%02d", passwd, (int)tbl->auth_code, (int)tbl->auth_class);
            }
            for(port=0; port<PORT_NUM; port++)
                if(Sys_mode.com_m[port])
                    if(DownloadCode(tmp, 6, 6, 0xBA, port) != 0) err=1;
            if(!err)
	    {
	        tbl->auth_set_flag = 0;
                if(tbl->auth_class != UNUSED_AUTH && tbl->auth_class != 0)
                {
		    if(check_prn() == FALSE)
		    {
                        sprintf(msg, "¿¨ºÅ:%03d ÊÚÈ¨Âë: %lu", (int)tbl->auth_code, passwd);
                        message_disp(8, msg);
                        while(!kbhit());
                        message_end();
                    }
                    else
                    {
//                        message_disp(8,"ÕýÔÚ´òÓ¡£¬ÇëÉÔºò...");   /* printing */

                        /* print the client notification */
                        fp = fopen("gfsdata\\client.txt", "r");
                        if(fp == NULL)
                            prnf("¿¨ºÅ:%03d ÊÚÈ¨Âë: %lu\n", (int)tbl->auth_code, passwd);
                        else
                        {
                            while( fgets(buf, 130, fp) != NULL)
			    {
                                for(j=strlen(buf);j<150;j++)
                            	    buf[j] = 0;

                                bufp = strstr(buf, "%");
                                while( bufp != NULL)
                                {
                                    switch(*(bufp+1))
                                    {
                                        case 'G':
					     if(tbl->auth_usr_nam) {
					         sprintf(msg, "%10s",tbl->auth_usr_nam);
                                                 strncpy(bufp, msg, strlen(msg));
                                             }
                                             else
					         strncpy(bufp, "      ¿ÍÈË", 10);
                                             break;
                                         case 'C':
                                             sprintf(msg, "%03d", (int)tbl->auth_code);
                                             strncpy(bufp, msg, strlen(msg));
                                             break;
                                         case 'P':
                                             sprintf(msg, "%lu", passwd);
                                             strncpy(bufp, msg, strlen(msg));
                                             break;
                                         case 'O':
                                             sprintf(msg, "%2s",auth_head.setup);
                                             strncpy(bufp, msg, strlen(msg));
                                             break;
					 case 'S':
                                             sprintf(msg, "%2s",auth_head.close);
                                             strncpy(bufp, msg, strlen(msg));
                                             break;
                                         case 'X':
                                             sprintf(msg, "%2s",auth_head.change);
                                             strncpy(bufp, msg, strlen(msg));
                                             break;
                                         case 'U':
                                             strncpy(bufp, Usr_nam, strlen(Usr_nam));
                                             break;
                                         case 'D':
                                             sprintf(msg,"%dÄê%02dÔÂ%02dÈÕ",d.da_year,d.da_mon,d.da_day);
                                             strncpy(bufp, msg, strlen(msg));
                                             break;
                                         default:
                                             break;
                                    }
                                    bufp = strstr(bufp+1, "%");
				}
				j = strlen(buf);
				if(buf[j-1] != '\n')
				    buf[j] ='\n';
				prnf(buf);
			    }
			    fclose(fp);
			}

//                        message_end();
                    } /* end of if(check_prn() */
                }  /* end of if(tbl->auth_class) */
            }/* end of if(!err) */
        }
        tbl++;
    }

}

int ShakeHand(UC port)
{
    char *title = "´íÎó";
    char *shake = "°YWHQCSNü";
    int i;
    UL  GoalTick;

    SIORecHead[port] = SIORecTail[port] = 0;

    if(port == 0 || port == 1)
        outportb(SIO_IER[port],0x1);        /* Enable Receive Int */

    for (i=0; i<9;i++)		  /* send shaking hand string */
    	ToCPU(port, 0xB0);

    GoalTick = *Tick_cnt_ptr+TimeOut;
    while(*Tick_cnt_ptr < GoalTick)
    {
        CheckPort(port);
        SIORecBuf[port][SIORecTail[port]] = 0;
        if(strcmp(SIORecBuf[port]+SIORecTail[port]-strlen(shake), shake) == 0)
        {
            SIORecHead[port] = SIORecTail[port] = 0;
            return 0;
        }
        if(_bios_keybrd(_KEYBRD_READY) )
       	    if(_bios_keybrd(_KEYBRD_READ) == ESC) return -1;
    }

    warn_mesg(title,"Áª»ú³¬Ê±");
    return -1;

}

int ShakeHandAll()
{
    UC port;

    for(port=0; port<PORT_NUM; port++)
        if(Sys_mode.com_m[port])
            if(ShakeHand(port)) return -1;

    return 0;
}

int  CheckPort(UC port)
{
    int len;
    int portm;
    UC  ch;

    if(port >1 )			/* Multi Card port */
    {
        portm = port - 2;
        while(comrxch(IRQNO, portm, &ch) == 0) {
            if( (ch==0xfd) || (ch==0xfc) || (ch==0xb4) || (ch==0xb7) || (ch==0xb8) )
                CmdCount[port]++;
	    SIORecBuf[port][SIORecTail[port]] = ch;
	    if(DEBUG)
		printf("#%02x", ch);
	    SIORecTail[port]++;
	    if(SIORecTail[port] >= SIO_BUF_SIZE)
		SIORecTail[port] -= SIO_BUF_SIZE;
        }
    }

    len = SIORecTail[port] - SIORecHead[port];
    if(len < 0) len += SIO_BUF_SIZE;

    return len;
}

/* function  :    down load code
 * calls     :    ToCPU()
 * called by :    send_auth_code()
 * input     :    tmp -- string to be sent
 *		  len1 -- phone code length
 * 		  len2 -- other code
 *		  ch -- code type
 *			0xBA -- authority code
 *			0xBB -- authority code first character
 *			0xBC -- information phone call
 *			0xBD -- local call
 *			0xBE -- prohibit phone call
 *			0xBF -- emergcy free call
 *			0xC0 -- phone inner code and class
 *			0xC1 -- date
 *			0xC2 -- time
 * return    :    none
 * note      :
 * date      :    1996.01.21
 */
int DownloadCode(UC *src, UI len1, UI len2, UC ch, UC port)
{
    char *title = "´íÎó";
    int k, l;
    UL  GoalTick;
    int flag;
    UC  tmp[80];
    UI  Tail;

    outportb(SIO_IER[0],0x01);       /* enable interruption */
    outportb(SIO_IER[1],0x01);       /* enable interruption */

    for(k=0; k<len1+len2; k++)
	tmp[k] = src[k];

    for(k=0; k<len1-1; k++) /* find first non '0' */
	if(tmp[k] != '0') break;

    for(l=k+1; l<len1; l++) /* change '0' to 'a' */
	if(tmp[l] == '0') tmp[l] += 0x0a;

    for(k=0; k<(len1+len2)/2; k++)  /* convert to bcd */
	tmp[k] = ((tmp[2*k]-'0')<<4) | ((tmp[2*k+1]-'0'));
    tmp[k] = ch;

    // reseve the tail
//    SIORecHead[port] = SIORecTail[port] =0;
    Tail = SIORecTail[port];
    for(l=0;l<(k+1); l++) /* send code */
	ToCPU(port, tmp[l]);

    GoalTick = *Tick_cnt_ptr + TimeOut;
    while(SIORecTail[port]==Tail && *Tick_cnt_ptr < GoalTick)
    {
	CheckPort(port);
	if(_bios_keybrd(_KEYBRD_READY) )
	    if(_bios_keybrd(_KEYBRD_READ) == ESC) break;
    }

    if(SIORecTail[port] == Tail) {
	warn_mesg(title,"Í¨Ñ¶³¬Ê±");
	flag = -1;
    }
    else if(SIORecBuf[port][Tail] == 0xB5) { /* success */
	flag = 0;
    }
    else  {
	warn_mesg(title,"ÉèÖÃ´íÎó");
	flag = -2;
    }

    // restore the SIORecBuf
    SIORecTail[port] = Tail;

    return flag;
}

/* function  : set the authority code setup and cancel head
 * called by : set_auth_head() (set_data.c)
 * input     :
 * date      : 1996.1.26
 */
void set_auth_head(void)
{
    int  key;
    UC   res1=0, esc=0;
    UI   back_in=8;
    char *fname ="gfsdata\\authhead.dat";
    FILE *fp;
    AUTH_HEAD_STRUCT auth_head;
    TABLE_STRUCT auth_head_tbl = {H_BX,H_BY-17,0,30,3,2,{150,120},7};
    UNIT_STRUCT  cur_unit;
    UC tmp[10];
    UC port, err=0;

    clr_DialWin(1);
    message_disp(8,"¡ý¡ü ÒÆ¶¯  Enter ÊäÈë");     /* Up,Dn: move,Enter: input */

    pop_back(H_BX-20,H_BY-55,H_BX+275,H_BY+95,7);    /* big frame */
    draw_table(&auth_head_tbl);

    hz16_disp(H_BX+40,H_BY-40,"ÊÚ È¨ Âë ×Ö Í· Éè ¶¨",BLACK);

    hz16_disp(H_BX+14,H_BY-5, "¿ª  Æô  ×Ö  Í·",BLACK);  /* setup */
    hz16_disp(H_BX+14,H_BY+27,"¹Ø  ±Õ  ×Ö  Í·",BLACK);  /* clsoe */
    hz16_disp(H_BX+14,H_BY+59,"¸ü  ¸Ä  ×Ö  Í·",BLACK);  /* change */

    draw_back1(490, 122, 625, 357, 11);
    outf(500, 142, 11, 0, "F2   ´æÅÌ");
    outf(500, 162, 11, 0, "F10  È±Ê¡ÉèÖÃ");
    outf(500, 182, 11, 0, "¡û¡ý¡ú¡ü ÒÆ¶¯");
    outf(500, 202, 11, 0, "Enter ÊäÈë");

    outf(500, 232, 11, 0, "Esc   ÍË³ö");

    draw_back(H_BX+153,H_BY-10,H_BX+225,H_BY+13, 11);
    draw_back(H_BX+153,H_BY+22,H_BX+225,H_BY+45, 11);
    draw_back(H_BX+153,H_BY+54,H_BX+225,H_BY+77, 11);

    setcolor(BLACK);

    fp = fopen(fname,"rb");
    if(fp != NULL) {
      fread(&auth_head, sizeof(AUTH_HEAD_STRUCT), 1, fp);
      outf(H_BX+155+back_in,H_BY-6, 11,BLACK,"%2s",auth_head.setup);
      outf(H_BX+155+back_in,H_BY+26,11,BLACK,"%2s",auth_head.close);
      outf(H_BX+155+back_in,H_BY+58,11,BLACK,"%2s",auth_head.change);
      fclose(fp);
    }

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);

    for(;;)
    {
	key = get_key1();

	get_current(&cur_unit);
	switch(key)
	{
	    case ESC:
            	esc = 2;
                break;
            case F2:
                esc = 0;
                if(auth_head.setup[0] == '*' && auth_head.setup[1] == 0 &&
		   auth_head.close[0] == '#' && auth_head.close[1] == 0 &&
		   auth_head.change[0] == '1' && auth_head.change[1] == '#' &&
                   auth_head.change[2] == 0   )
                     esc = 1;
	      	else {
                    if(auth_head.setup[0]>='1' && auth_head.setup[0]<='9' &&
                       auth_head.close[0]>='1' && auth_head.close[0]<='9' &&
                       auth_head.change[0]>='1' && auth_head.change[0]<='9' &&
                       auth_head.change[1]>='0' && auth_head.change[1]<='9' &&
                       auth_head.change[2] == 0 )
                    {
                      if(auth_head.setup[1] == 0 && auth_head.close[1] == 0 &&
	                      auth_head.change[0] != auth_head.setup[0] &&
         	              auth_head.change[0] != auth_head.close[0] )
                        esc = 1;
                      else if(auth_head.setup[1]>='0' && auth_head.setup[1]<='9' &&
                              auth_head.close[1]>='0' && auth_head.close[1]<='9' )
                        esc = 1;
                    }
                }

                if(!esc)
		  warn_mesg("´íÎó","ÊÚÈ¨Âë×ÖÍ·´íÎó");
		break;
            case F10:
                auth_head.setup[0] = '*';
                auth_head.setup[1] = 0;
                auth_head.close[0] = '#';
                auth_head.close[1] = 0;
                auth_head.change[0] = '1';
                auth_head.change[1] = '#';
                auth_head.change[2] = 0;
		outf(H_BX+155+back_in,H_BY-6, 11,BLACK,"* ");
      		outf(H_BX+155+back_in,H_BY+26,11,BLACK,"# ");
      		outf(H_BX+155+back_in,H_BY+58,11,BLACK,"1#");
                break;
            case UP:
                move_finger(0,1);
                break;
            case DOWN:
            	move_finger(1,1);
                break;
	    case ENTER:
		hide_finger();
		switch(cur_unit.unit_x)
		{
		    case 0:     /* input the setup head */
		        set_get_color(0, 11, 11, 7, 0);
			res1 = get_account(H_BX+155,H_BY-7,20,60,back_in,2,\
					 auth_head.setup,0x00);
			if(res1)     /* valid input */
			    move_finger(1,1);
			break;
		    case 1:       /* input the close head */
		        set_get_color(0, 11, 11, 7, 0);
			get_account(H_BX+155,H_BY+25,20,60,back_in,2,\
					 auth_head.close,0x00);
			if(res1)     /* valid input */
			    move_finger(1,1);
			break;
		    case 2:       /* input the change head */
		        set_get_color(0, 11, 11, 7, 0);
			get_account(H_BX+155,H_BY+57,20,60,back_in,2,\
					 auth_head.change,0x00);
			break;
		    default:
			sound_alarm();
			break;
		}       /* end of "switch(cur_unit.unit_y)" */
		echo_finger();
		break;
	    default:
		sound_alarm();
		break;
	 }              /* end of "switch(key)" */
	 if(esc)
	    break;
    }                   /* end of "for(;;)"     */

    if(esc !=2 && ShakeHandAll() == 0) {
        if(auth_head.setup[1]==0) /* one char */
        {
          tmp[0] = 0;
    	  tmp[1] = (auth_head.setup[0]=='*')?0x0B+'0':auth_head.setup[0];
          tmp[2] = 0;
          tmp[3] = (auth_head.close[0]=='#')?0x0C+'0':auth_head.close[0];
        }
        else
        {
          tmp[0] = auth_head.setup[0];
    	  tmp[1] = (auth_head.setup[1] == '0')?0x0a+'0':auth_head.setup[1];
          tmp[2] = auth_head.close[0];
    	  tmp[3] = (auth_head.close[1] == '0')?0x0a+'0':auth_head.close[1];
        }

        tmp[4] = 0;
        tmp[5] = (auth_head.change[0]=='*')?0x0B+'0':auth_head.change[0];
        tmp[6] = 0;
        if(auth_head.change[1] == '#')
            tmp[7] = 0x0C+'0';
        else
            tmp[7] = (auth_head.change[1] == '0')?0x0a+'0':auth_head.change[1];

        err = 0;
        for(port=0; port<PORT_NUM; port++)
          if(Sys_mode.com_m[port])
            if(DownloadCode(tmp, 0, 8, 0xBB, port) != 0) err =1;
        if(!err) {
            fp = fopen(fname, "wb");
            fwrite(&auth_head, sizeof(AUTH_HEAD_STRUCT), 1, fp);
            fclose(fp);
        }
    }

    rid_pop();
    recover_screen(1);
    message_end();
    return;
}
