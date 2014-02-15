/******************************************************************/
/*   FILENAME: DRAW_M.C                                          */
/*   FUNCTION : draw_stat_window(UC WNO)                          */
/*              draw_one_status(UC ANO,UC BC,UC CC,UC *STR)       */
/*              draw_sub_menu()                                   */
/*              draw_main_menu()                                  */
/*              draw_head_menu()                                  */
/*              draw_menu()                                       */
/*              disp_ht_title()                                   */
/*   AUTHER :  ZHANG_RUI_JUN                                      */
/*   OTHERS :  Modified by Bob Kong, 1993-03-26                   */
/******************************************************************/

#include <stdio.h>
#include <dos.h>
#include <graphics.h>
#include <string.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>
#include <menufunc.inc>
#include <menu_def.inc>

#define   HEAD_BK_CLR   9
#define   BOTM_BK_CLR   11

extern UC Cursor_on;
extern char patterns[][8];

static UC Stat_num;

void draw_status_window(UC wno)
/*      Draw the blank status window.
        Called only by draw_screen() in draw_screen()
*/
{
UI i;
UI bx = M_EX+2;
UI by = D_BOTTOM+1;
UI ex = MAX_X-S_XAD-2;
UI ey = MAX_Y;

        for(i=0;i<wno;i++)
        {
                draw_back(bx+i*(ex-bx)/wno-1, by,
                                  bx+(i+1)*(ex-bx)/wno+1, ey, 11);
        }
}


void draw_one_status(UC ano, UC bc, UC cc, UC *str)
/* ---------------------------------------------------
                ano:    specify one status window.
                bc:             background color.
                cc:             characters' color.
                str:    string to be echoed.
------------------------------------------------------ */
{
UI              bx = M_EX+2;
UI              by = D_BOTTOM+1;
UI              ex = MAX_X-S_XAD-2;
UI              ey = MAX_Y;

UC              old_cursor_stat;
UI              old_color;
struct  fillsettingstype old_fl;
struct  linesettingstype old_ln;


        old_color = getcolor();
        getfillsettings( &old_fl );
        getlinesettings( &old_ln );

        flash_pause();
        old_cursor_stat = Cursor_on;
        cursor_off();

        draw_back(bx+ano*(ex-bx)/Stat_num+1, by,
                          bx+(ano+1)*(ex-bx)/Stat_num, ey, bc);
        disp_str(bx+ano*(ex-bx)/Stat_num+((ex-bx)/Stat_num-1-8*strlen(str))/2,
                          (by+ey)/2-8, str, bc, cc);

        if ( old_cursor_stat==TRUE )
                cursor_on();

        flash_continue();
        setcolor(old_color);
        setfillstyle(old_fl.pattern, old_fl.color);
        setlinestyle(old_ln.linestyle, old_ln.upattern, old_ln.thickness);
}

void draw_sub_menu()
/* Echo the 2th menu. The horizon one. */
{
UI i;

        for(i=0;i<S_MENU;i++)
        {
                draw_key(S_BX+i*S_XAD+1, S_BY, S_BX+(i+1)*S_XAD, S_EY, "", 7, 0);
        }
}

void draw_main_menu(void)
/* Echo the 1th menu. The vertical one. */
{
UI i;

        for(i=0;i<M_MENU;i++)
        {
                draw_key(M_BX, M_BY+i*M_YAD+1, M_EX, M_BY+(i+1)*M_YAD, "", 7, 0);
        }
}

void draw_head_menu(void)
/* Echo the 0th menu -- the main menu */
{
UI i,j;
UC *mstr = "主 菜 单";          /* the title: main menu */

        draw_back1(H_BX-5,H_BY-41,H_BX+H_X_M*H_XAD+6,H_BY+(H_Y_M+1)*H_YAD+6,7);
        disp_str(H_BX+H_XAD+((H_X_M-2)*H_XAD-8*8)/2,H_BY-27,mstr,7,0);

        for(i=0; i<H_Y_M; i++)
                for(j=0;j<H_X_M;j++)
                {
                        draw_key(H_BX+j*H_XAD,H_BY+i*H_YAD,
                                 H_BX+(j+1)*H_XAD,H_BY+(i+1)*H_YAD ,"",7,0);
		}
}
/*
void pop_head(void)
{
UC *mstr1 = " GFS通用程控交换机计费系统 ";
UC *mstr2 = "         版本 2.5";
//UC *mstr3 = "  深圳泰康信工业有限公司";
UC *mstr3 = "深圳泰康信工业有限公司版权所有";
//UC *mstr3 = "深圳瑞信佳通信设备有限公司提供";
UC *mstr4 = "      1994年, 1995年";
//UC *mstr5 = "    版权所有, 不得拷贝";
UC *mstr5 = "                      ";
UC *mstr6 = "拥有合法许可才可获得技术支持!";
UC *mstr7 = "热线电话: 0755-6404502/6646081";

//        pop_back(H_BX-5,H_BY-41, H_BX+H_X_M*H_XAD+6,H_BY+(H_Y_M+1)*H_YAD+6,9);
	pop_back(H_BX-5,H_BY-41, H_BX+H_X_M*H_XAD+6,H_BY+(H_Y_M+1)*H_YAD+6+40,9);
//        pop_back(H_BX-5,H_BY-41, H_BX+H_X_M*H_XAD+6,H_BY+(H_Y_M+1)*H_YAD+6-10,9);
	message_disp(8,"按任意键继续");        // press any key

	disp_str(H_BX+46,H_BY-27,mstr1,9,14);
	disp_str(H_BX+50,H_BY-7, mstr2,9,14);
	disp_str(H_BX+32,H_BY+17,mstr3,9,14);
	disp_str(H_BX+50,H_BY+37,mstr4,9,14);
	disp_str(H_BX+50,H_BY+57,mstr5,9,14);
	disp_str(H_BX+36,H_BY+77,mstr6,9,14);
	disp_str(H_BX+32,H_BY+97,mstr7,9,14);
	get_key1();
//        delay(2000);
	message_end();
	rid_pop();
}
*/

void draw_menu(UC wno)
/* draw the whole screen */
{
        Stat_num = wno;

//        setfillstyle(SOLID_FILL,7);
	setfillpattern( &patterns[0][0], 15);
        bar(0,0,639,479);       /* reset the clock face */
        setfillstyle(SOLID_FILL,0);
        rectangle(3,3,S_BX-2,M_BY-2);       /* reset the clock face */
        bar(3,3,S_BX-3,M_BY-3);       /* reset the clock face */
//        bar(0,0,S_BX-1,M_BY-2);       /* reset the clock face */
//        draw_key(-1,-1,S_BX,M_BY, "", 9, 0);       /* reset the clock face */
        setfillstyle(SOLID_FILL,HEAD_BK_CLR);
//        bar(S_BX-1,0,T_EX,S_BY-2);    /* reset the whole menu title square */
//        bar(T_EX,0,639,S_BY-2);       /* reset the current 0th choice */

        draw_key(S_BX+1,2,T_EX-1,S_BY-2,"", 9,0);    /* reset the whole menu title square */
        setcolor(7);
        rectangle(S_BX+1,2,T_EX-1,S_BY-2);    /* reset the whole menu title square */
//        draw_key(T_EX+1,2,639-2,S_BY-1,"", 9,0);       /* reset the current 0th choice */
        draw_back(T_EX+1,3,639-2,S_BY-3, 11);       /* reset the current 0th choice */

        setcolor(15);
//        rectangle(2,2,S_BX-2,M_BY-2);       /* reset the clock face */
//        rectangle(0,0,S_BX-1,M_BY-2);       /* reset the clock face */
//        rectangle(S_BX-1,0,T_EX,S_BY-2);    /* reset the whole menu title square */
//        rectangle(T_EX,0,639,S_BY-2);       /* reset the current 0th choice */

        draw_sub_menu();                        /* reset the 2th menu -- horizon */
        draw_main_menu();                       /* reset the 1th menu -- vertical */

        /* ---- Draw Help window ---- */
        draw_back(0,D_BOTTOM+1,M_EX,479, BOTM_BK_CLR);
        disp_esc();
//        hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);

        /* ---- draw Exit window ---- */
        draw_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, BOTM_BK_CLR);
//        hz16_disp(MAX_X-S_XAD+2, D_BOTTOM+8, "07556404502", 0);
        draw_status_window(wno);                /* reset the status' windows */
}
