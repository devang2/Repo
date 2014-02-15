#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <bios.h>
#include <string.h>
#include <graphics.h>
#include <time.h>
#include <io.h>
#include <float.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dir.h>
#include "mouse.h"
#include "menu_d.h"
#include "new2.h"
struct setpara {
 int fuji_v;
 int fuji_t;
 int first_v;
 int end_v;
 int pulse;
 int p_width;
 int t_width;
 int slope;
 int brush_v;
 int brush_t;
 int stat_t;
 int sel_R;
 int sel_bl;
 } spara;
struct ivpointer{
 float current;
 float insist;
 int scan_v;
 }ivp[1802];
struct ivdiffrance{
 float current;
 int scan_v;
 }ivd[1804];
struct ivtop{
 float current[100];
 int scan_v[100];
 }ivtop;
struct wpeak{
 float current;
 int scan_v;
 }wp[100];
int pi;
struct wbottm{
 float current;
 int scan_v;
 }wb[100];
struct date d;
struct time t;
struct temp_d{
 int te_year;
 int te_day;
 int te_mon;
}te_d;
struct temp_t{
 int te_hour;
 int te_min;
 int te_sec;
}te_t;
float insist_set[16]={232,472,942,2442,4842,9942,24942,51042,
100942,200942,350942,620942,1090942,2090942,4090942,6790942};
float insist,max;
int count,con1,con2;
int max_num;
int mouse_test_sign=0;
int step;
int sub_num[7]={4,4,3,3,2,4,2};
int flag=FALSE;
int mc_flag=0;
float cur_flag=1000;
char *filename[1]={"*.lin           ",};
int rate_flag=3;
struct ssys{
  int prn;
  int IE;
  int QD;
  }sz_sys;
extern struct pd_str m_menu[MAIN_OPT]={
 "系统维护",
	      help_0,
	      "参数设置",
	      "系统时钟",
	      "ＤＯＳ命令",
	      "退出系统",
	      "",
	      "",
			   cssz,
			   jp_clock,
			   shell,
			   exit_con,
			   0,
			   0,
 "测试方法",
	   help_1,
	      "差示脉冲溶出",
	      "差示脉冲伏安",
	      "线性扫描伏安",
	      "循环扫描伏安",
	      "",
	      "",
			   scrc,
			   scmc,
			   smfa,
			   xhfa,
			   0,
			   0,
 "校准方法",
	  help_2,
	      "双标类推法",
	      "外推加入法",
	      "校准曲线法",
	      "",
	      "",
	      "",
			   sblt,
			   bzjr,
			   gzqx,
			   0,
			   0,
			   0,
 "文件调用",
	   help_3,
	      "测试曲线",
	      "校准数据",
	      "当前曲线",
	      "",
	      "",
	      "",
			   scrn_diao,
			   spara_diao,
			   just_scrn,
			   0,
			   0,
			   0,
 "文件存盘",
	  help_4,
	      "测试曲线",
	      "校准数据",
	      "",
	      "",
	      "",
	      "",
			   scrn_cun,
			   spara_cun,
			   0,
			   0,
			   0,
			   0,
 " 打  印 ",
	   help_6,
	      "测试曲线",
	      "求导曲线",
	      "校准曲线",
	      "窗口内容",
	      "",
	      "",
			   IE_line,
			   QD_line,
			   JZ_line,
			   print_win,
			   0,
			   0,
 "检测台",
	  help_5,
	      "分析卡检测",
	      "工作台检测",
	      "",
	      "",
	      "",
	      "",
			   xtzj,
			   gztzj,
			   0,
			   0,
			   0,
			   0,
 };
int m_sign;
int sub_left,sub_top,M_Pointer=0,S_Pointer=0;
unsigned int size;
void far *buffer=(void far *)0xA0000000;
void print_win()
  {
  int p_flag=0;
  int col1,col2,col3;
  col1=getpixel(206,420);
  col2=getpixel(208,460);
  col3=getpixel(208,456);
  if((col1==8)&&(col2==8)&&(col3==8)){
    file_warn(9);
    return;
    }
  p_flag=print_test();
  if(p_flag<0) return;
  if(col1!=8) print_2(12,60,630,470+25*sz_sys.prn,col1,14);
  else if(col3!=8) print_2(12,60,630,470+25*sz_sys.prn,col3,col3);
  else print_2(12,60,630,470+25*sz_sys.prn,col2,col2);
  return;
  }
void save_win(int left,int top,int right,int bottom)
  {
  sub_left=left;
  sub_top=top;
  size=imagesize(left,top,right,bottom);
  buffer=malloc(size);
  getimage(left,top,right,bottom,buffer);
  }
void gun_pop()
  {
  putimage(sub_left,sub_top,buffer,COPY_PUT);
  farfree(buffer);
  flag=FALSE;
  }
void submenu(int left,int top,int right,int bottom,int color)
  {
  int i,j;
  save_win(left,top,right,bottom);
  setfillstyle(SOLID_FILL,color-3);
  bar(left,top,right,bottom);
  setfillstyle(SOLID_FILL,color-1);
  bar(left+4,top+4,right-4,bottom-4);
  setcolor(14);
  setlinestyle(0,1,1);
  rectangle(left+2,top+2,right-2,bottom-2);
  rectangle(left+4,top+4,right-4,bottom-4);
  for(j=0;j<sub_num[M_Pointer];j++){
    setfillstyle(SOLID_FILL,15);
    for (i=3; i<5; i++)
	{
	setcolor(15);
	line(left+15-i,top+16+j*26-i,left+110+i,top+16+j*26-i);
	line(left+15-i,top+16+j*26-i,left+15-i,top+16+j*26+16+i);
	setcolor(i);
	line(left+110+i,top+16+j*26-i,left+110+i,top+16+j*26+16+i);
	line(left+15-i,top+16+j*26+16+i,left+110+i,top+16+j*26+16+i);
	}
   disp_shadow_str(left+14,top+18+j*26,m_menu[M_Pointer].sub[j],0);
   }
 flag=TRUE;
 }
void mouse_test(int c)
   {
   if(c>=8){
     if(c==8) exit_con();
     //else sys_win(2,24,80,90);
     c=M_Pointer;
     return;
     }
   else M_Pointer=c;
   pull_bar(m_sign+2,3+step,m_sign+76,31+step);
   m_sign=4+c*88;
   push_bar(m_sign+2,3+step,m_sign+76,31+step);
   while(BOY_MOUSE_BUTTON()!=0);
   if(flag==TRUE) gun_pop();
   if(S_Pointer>=sub_num[M_Pointer]) S_Pointer=sub_num[M_Pointer]-1;
   submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
   push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
   }
void mouse_sub_test(int c)
  {
  pull_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
  if(c>=8){
     if(c==8) exit_con();
     //else sys_win(2,24,80,90);
     c=S_Pointer;
     }
  else S_Pointer=c;
  push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
  while(BOY_MOUSE_BUTTON()!=0);
  gun_pop();
  m_menu[M_Pointer].fun[S_Pointer]();
  if(S_Pointer>=sub_num[M_Pointer]) S_Pointer=sub_num[M_Pointer]-1;
  submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
  push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
  return;
  }
int flag_test()
  {
  if(flag==FALSE) return(-1);
  else return(M_Pointer);
  }
int quit()
  {
  BOY_MOUSE_HIDE();
  if(flag==FALSE) m_menu[M_Pointer].m_help();
  else{
      gun_pop();
      m_menu[M_Pointer].m_help();
      submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
      push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
      }
  BOY_MOUSE_VIEW();
  return(-1);
  }
void ret_Main()
  {
  int i;
  char *str[7];
  for(i=0;i<=6;i++)
    str[i]=m_menu[i].main;
  Main_board(str);
  push_bar(m_sign+2,3+step,m_sign+76,31+step);
  return;
  }
void shell(void)
  {
  struct viewporttype vp;
  int mode;
  getviewsettings( &vp );
  mode = getgraphmode();
  restorecrtmode();
  printf("Type exit or EXIT  return \n");
  system("");
  setgraphmode( mode );
  ret_Main();
  return;
  }
void bios_test(int c)
  {
  if(flag==FALSE){
    switch(c){
      case 19712:
		 pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		 M_Pointer++;
		 if(M_Pointer>6){
		   M_Pointer=0;
		   m_sign=4;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		 else{
		   pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		   m_sign+=88;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		 break;
     case 19200:
		pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		M_Pointer--;
		 if(M_Pointer<0){
		   M_Pointer=6;
		   m_sign=532;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		 else{
		   pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		   m_sign-=88;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		 break;
    case 7181:
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 20480:
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 20736://PgDn
		smaller();
		break;
    case 18688://PgUp
		bigger();
		break;
    case 20011:
		add_line(1,10);
		break;
    case 3389:
		add_line(1,100);
		break;
    case -32000:
		add_line(1,1);
		break;
    case 18989:
		add_line(0,10);
		break;
    case 3117:
		add_line(0,100);
		break;
    case -32256:
		add_line(0,1);
		break;
    case 15104: //F1
		m_menu[M_Pointer].m_help();
		break;
    case 15360://F2
		scrn_cun();
		break;
    case 15616://F3
		scrn_diao();
		break;
    case 26880://ALT+F2
		spara_cun();
		break;
    case 27136://ALT+F3
		spara_diao();
		break;
    case 24320://Ctrl+F2
		spara_cun();
		break;
    case 24576://Ctrl+F3
		spara_diao();
		break;
    case 15872://F4
		m_menu[0].fun[0]();
		break;
    case 16128://F5
		exit_con();
    case 16384://F6
		cls_all_scrn();
		break;
    case 16640: //F7
		just_QD();
		break;
    case 16896: //F8
		print_win();
		break;
/*    case 17152://F9
		cls_all_scrn();
		brush();
		break;*/
/*    case 17408: //F10
		sys_win(2,24,80,90);
		break;*/
    case 3849:
		tab_bigger();
		break;
     }
    }
    else{
    switch(c){
      case 19712:
		 pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		 M_Pointer++;
		 gun_pop();
		 if(M_Pointer>6){
		   M_Pointer=0;
		   m_sign=4;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		 else{
		   pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		   m_sign+=88;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		if(S_Pointer>=sub_num[M_Pointer]) S_Pointer=sub_num[M_Pointer]-1;
		  submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		  push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		 break;
     case 19200:
		pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		M_Pointer--;
		gun_pop();
		if(M_Pointer<0){
		   M_Pointer=6;
		   m_sign=532;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		 else{
		   pull_bar(m_sign+2,3+step,m_sign+76,31+step);
		   m_sign-=88;
		   push_bar(m_sign+2,3+step,m_sign+76,31+step);
		   }
		if(S_Pointer>=sub_num[M_Pointer]) S_Pointer=sub_num[M_Pointer]-1;
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		 break;
     case 18432: /*UP*/
		pull_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		if(S_Pointer<=0) S_Pointer=sub_num[M_Pointer]-1;
		else S_Pointer--;
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 20480: /*DN*/
		pull_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		if(S_Pointer>=sub_num[M_Pointer]-1) S_Pointer=0;
		else S_Pointer++;
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 18176: /*Home*/
		pull_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		S_Pointer=0;
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 20224:/*END*/
		pull_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		S_Pointer=sub_num[M_Pointer]-1;
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 7181:
		gun_pop();
		m_menu[M_Pointer].fun[S_Pointer]();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 3849: //Tab
		gun_pop();
		tab_bigger();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 20736://PgDn
		gun_pop();
		smaller();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 18688://PgUp
		gun_pop();
		bigger();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 7178://Ctrl+Enter
		gun_pop();
		cls_all_scrn();
		cont_sm();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 20011:
		gun_pop();
		add_line(1,10);
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 3389:
		gun_pop();
		add_line(1,100);
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case -32000:
		gun_pop();
		add_line(1,1);
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 18989:
		gun_pop();
		add_line(0,10);
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 3117:
		gun_pop();
		add_line(0,100);
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case -32256:
		gun_pop();
		add_line(0,1);
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
     case 15104://F1
		gun_pop();
		m_menu[M_Pointer].m_help();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 15360://F2
		gun_pop();
		scrn_cun();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 15616://F3
		gun_pop();
		scrn_diao();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 26880://ALT+F2
		gun_pop();
		spara_cun();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 27136://ALT+F3
		gun_pop();
		spara_diao();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 24320://Ctrl+F2
		gun_pop();
		spara_cun();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 24576://Ctrl+F3
		gun_pop();
		spara_diao();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 15872://F4:
		gun_pop();
		m_menu[0].fun[0]();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 16128://F5
		exit_con();
    case 16384://F6
		gun_pop();
		cls_all_scrn();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 16640: //F7
		gun_pop();
		just_QD();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 16896: //F8
		gun_pop();
		print_win();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
/*    case 17152: //F9
		gun_pop();
		cls_all_scrn();
		brush();
		submenu(m_sign-4*M_Pointer,36+step,m_sign+124-4*M_Pointer,sub_num[M_Pointer]*26+54+step,8);
		push_bar(m_sign-4*M_Pointer+9,S_Pointer*26+46+step,m_sign+116-4*M_Pointer,S_Pointer*26+74+step);
		break;
    case 17408: //F10
		sys_win(2,24,80,90);
		break;*/
    case 283:
		gun_pop();
		S_Pointer=0;
		break;
     }
    }
  }
#pragma warn + par
int main()
  {
  int i,input_key=0;
  char *str[7];
  _control87(0,0);
  step=22;
  outportb(0x220,0x00);
  da_v(spara.brush_v);
  system("viewbmp fm.bmp");
  harderr(handler);
  for(i=0;i<=6;i++)
    str[i]=m_menu[i].main;
  Main_board(str);
  m_sign=4;
  push_bar(m_sign+2,3+step,m_sign+76,31+step);
  sys_diao();
  spara.fuji_v=-1200;
  spara.fuji_t=60;
  spara.first_v=-1200;
  spara.end_v=0;
  spara.pulse=50;
  spara.p_width=50;
  spara.t_width=1;
  spara.slope=4;
  spara.brush_v=-50;
  spara.brush_t=60;
  spara.stat_t=30;
  spara.sel_R=5;
  spara.sel_bl=1;
  getdate(&d);
  gettime(&t);
  te_d.te_year=d.da_year;
  te_d.te_day=d.da_day;
  te_d.te_mon=d.da_mon;
  te_t.te_hour=t.ti_hour;
  te_t.te_min=t.ti_min;
  te_t.te_sec=t.ti_sec;
  for(i=0;i<1802;i++){ivp[i].current=0;ivp[i].scan_v=0;}
  if(BOY_SET_MOUSE()==1){
    mouse_test_sign=1;
    BOY_MOUSE_TYPE(0);
    BOY_MOUSE_VIEW();
    do{
      input_key=waitforinput();
      }while(input_key>=0);
    }
    else{
     mouse_test_sign=0;
     for(;;){
       input_key=bioskey(0);
       bios_test(input_key);
       }
    }
  exit_con();
  return(0);
  }
void jp_clock()
  {
  int second=0;
  char *string="时钟";
  saveimage("text.img",10,60,221,141);
  win_board(10,60,220,140,30,7,0);
  disp_hz_str(100,65,string,15);
  BOY_MOUSE_VIEW();
  setfillstyle(1,0);
  for(;;){
    if(kbhit()){
      if(bioskey(0)==283){
	BOY_MOUSE_HIDE();
	break;
	}
      else{
	button("按ＥＳＣ键退出！",0,0,1,0);
	setfillstyle(1,0);
	}
      }
    if((BOY_MOUSE_BUTTON()==1)&&(BOY_MOUSE_INBOX(203,66,214,79)==1)){
      while(BOY_MOUSE_BUTTON());
      BOY_MOUSE_HIDE();
      break;
      }
    if((BOY_MOUSE_BUTTON()==1)&&(BOY_MOUSE_INBOX(16,66,27,79)==1)){
      while(BOY_MOUSE_BUTTON());
      button("按ＥＳＣ键退出！",0,0,1,0);
      setfillstyle(1,0);
      }
    gettime(&t);
    if(second!=t.ti_sec){
      getdate(&d);
      MoveToXY(7,4);
      BOY_MOUSE_HIDE();
      bar(30,100,200,130);
      printf("%d-%d-%d  %d:%d:%d",d.da_year,d.da_mon,d.da_day,t.ti_hour,
	     t.ti_min,t.ti_sec);
      BOY_MOUSE_VIEW();
      second=t.ti_sec;
      }
    }
  loadimage("text.img",10,60,221,141);
  bell(400,1);
  return;
  }
void cssz()
 {
 int cs[13];
 int i;
 float pow_n,pow_10;
 float insist_R[16]={232,472,942,2442,4842,9942,24942,51042,
       100942,200942,350942,620942,1090942,2090942,4090942,6790942};
 cs[0]=spara.fuji_v;
 cs[1]=spara.fuji_t;
 cs[2]=spara.first_v;
 cs[3]=spara.end_v;
 cs[4]=spara.pulse;
 cs[5]=spara.p_width;
 cs[6]=spara.t_width;
 cs[7]=spara.slope;
 cs[8]=spara.brush_v;
 cs[9]=spara.brush_t;
 cs[10]=spara.stat_t;
 cs[11]=spara.sel_R;
 cs[12]=spara.sel_bl;
 cssz_ch(cs);
 spara.fuji_v=cs[0];
 if(cs[1]<0) cs[1]=0;
 spara.fuji_t=cs[1];
 spara.first_v=cs[2];
 spara.end_v=cs[3];
 spara.pulse=cs[4];
 spara.p_width=cs[5];
 if(cs[6]<0) cs[6]=0;
 spara.t_width=cs[6];
 if(cs[7]<1) cs[7]=1;
 spara.slope=cs[7];
 spara.brush_v=cs[8];
 if(cs[9]<0) cs[9]=0;
 spara.brush_t=cs[9];
 if(cs[10]<0) cs[10]=0;
 spara.stat_t=cs[10];
 if(cs[11]>15) cs[11]=15;
 if(cs[11]<0) cs[11]=0;
 spara.sel_R=cs[11];
 if(cs[12]>10) cs[12]=10;
 if(cs[12]<1) cs[12]=1;
 spara.sel_bl=cs[12];
 for(i=0;i<=15;i++){
   pow_n=pow(spara.sel_bl,i);
   pow_10=pow10(spara.sel_bl-1);
   insist_set[i]=insist_R[i]*pow_10/pow_n;
   }
 return;
 }
void MoveToXY(int x,int y)
   {
    union REGS reg;
    reg.h.ah=2;
    reg.x.bx=0;
    reg.h.dh=x;
    reg.h.dl=y;
    int86(0X10,&reg,&reg);
   }
float ad_v()
{
 int poll,lowby,highby;
 float in_v,ad_v;
 outportb(0x280,0x00);
 /*do {
  poll=inportb(0x285);
  //printf("poll %d",poll);
  poll=poll&128;
 } while(poll!=128);*/
 lowby=inportb(0x281);
 delay(1);
 highby=inportb(0x282)&15;
 //printf("hb %d",highby);
// highby=highby&15;
 //if(highby==255) highby-=10;
 //printf("hb& %d",highby);
 ad_v=highby*256+lowby;
 in_v=((ad_v-2047)*20/4096)*1000*(-1);
 printf("v %f ",in_v);
return(in_v);
}
/*
float adn_v(int n)
{
 int poll,lowby,highby;
 float in_v,ad_v;
 outportb(0x280,n);
 do {
  poll=inportb(0x285);
  poll=poll&128;
 } while(poll!=128);
 lowby=inportb(0x281);
 highby=inportb(0x282)&15;
 ad_v=highby*256+lowby;
 in_v=((ad_v-2047)*20/4096)*1000;
 return(in_v);
} */
void da_v(float value)
{
 int lowby,highby,an_data;
 an_data=floor(((value+2048)/4096)*4096);
 lowby=an_data&0xff;
 highby=an_data>>8;
 outportb(0x283,lowby);
 outportb(0x284,highby);
}
void add_line(int add,int scro)
  {
  int i;
  float temp=100;
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  if(ivp[10].current!=0.0)
    temp=fabs(ivp[10].current/scro);
  else if(ivp[8].current!=0.0) temp=fabs(ivp[8].current/scro);
  for(i=1;i<count;i++){
    if(add>0) ivp[i].current+=temp;
    else ivp[i].current-=temp;
    }
  cls_all_scrn();
  just_IE();
  return;
  }
void tab_bigger()
  {
  if(rate_flag>=0) rate_flag--;
  if(rate_flag<0) rate_flag=8;
  cls_all_scrn();
  just_IE();
  return;
  }
void smaller()
  {
  if(rate_flag<8) rate_flag++;
  cls_all_scrn();
  just_IE();
  return;
  }
void bigger()
  {
  if(rate_flag>0) rate_flag--;
  cls_all_scrn();
  just_IE();
  return;
  }
void drascr(int rate,int type,int color)
 {
 int i,x0,y0;
 if(spara.first_v<spara.end_v) x0=204;
 else x0=210;
 y0=400;
 dra_y(rate,type,color);
 for(i=1;i<count;i++)
    drajpline(x0,y0,ivp[i].scan_v,ivp[i].current,rate,type,color);
 }
void diffdrascr()
{
 float temp1;
 int i,max_num,x0,y0;
 int color;
 char *buf;
 color=12;
 if(spara.first_v<spara.end_v) x0=204;
 else x0=210;
 y0=236;
 max_num=dpoint(max,0)-1;
 temp1=max/pow(10,max_num);
 setcolor(color);
 screen2(color);
 dra_x();
 moveto(18,90);outtext("X*10");
 gcvt(max_num,5,buf);
 moveto(52,87);
 outtext(buf);
 if(temp1>=5){
	      difdra_y(0);
	      for(i=1;i<count-2;++i){
		diffscreen(x0,y0,ivd[i].scan_v,ivd[i].current,color,10);//1
	       }
	     }
 else{
 if(temp1<=2.5){
	      difdra_y(2);
	      for(i=1;i<count-2;++i){
		diffscreen(x0,y0,ivd[i].scan_v,ivd[i].current,color,2.5);//3
	       }
	     }
  else{
   if((temp1>2.5)&&(temp1<5)){
	      difdra_y(1);
	      for(i=1;i<count-2;++i){
		diffscreen(x0,y0,ivd[i].scan_v,ivd[i].current,color,5);//2
	    }
	}
      }
   }
  }
void diffyjdrascr()
 {
 float temp1;
 int i,max_num,x0,y0;
 int color;
 char *buf;
 color=12;
 if(spara.first_v<spara.end_v) x0=204;
 else x0=210;
 y0=236;
 max_num=dpoint(max,0)-1;
 temp1=max/pow(10,max_num);
 screen2(color);
 diffdra_x();
 moveto(18,90);outtext("X*10");
 gcvt(max_num,5,buf);
 moveto(52,87);
 outtext(buf);
 if(temp1>=5){
	     difdra_y(0);
	     for(i=1;i<count-2;++i){
		difyjdrascreen(x0,y0,ivd[i].scan_v,ivd[i].current,color,10);
	       }
	     }
 else{

  if(temp1<=2.5){
	      difdra_y(2);
	      for(i=1;i<count-2;++i){
		difyjdrascreen(x0,y0,ivd[i].scan_v,ivd[i].current,color,2.5);
	       }
	     }
  else{
   if((temp1>2.5)&&(temp1<5)){
	      difdra_y(1);
	      for(i=1;i<count-2;++i){
		difyjdrascreen(x0,y0,ivd[i].scan_v,ivd[i].current,color,5);
	       }
      }
  }
 }
}
void yjdraline(float x1,float y1,float x2,float y2)
{
  int x0,y0,m1,n1,m2,n2;
  int XWide,YHigh;
  XWide=400;
  YHigh=336-8;
  if(spara.first_v<spara.end_v) x0=192+12;
  else x0=192+18;
  y0=400;
  m1=x0+floor(fabs((200)-x1)*XWide/1800);
  if((max/pow(10,max_num))>5){
   n1=y0-floor((y1*YHigh)/pow(10,max_num+1));}
  if((max/pow(10,max_num))<2.5)
   { n1=y0-floor((y1*YHigh)/(2.5*pow(10,max_num)));}
  if(((max/pow(10,max_num))>2.5)&&((max/pow(10,max_num))<5))
   { n1=y0-floor((y1*YHigh)/(5*pow(10,max_num)));}
  m2=x0+floor(fabs((200)-x2)*XWide/1800);
  if((max/pow(10,max_num))>5){
   n2=y0-floor((y2*YHigh)/pow(10,max_num+1));}
  if((max/pow(10,max_num))<2.5)
   { n2=y0-floor((y2*YHigh)/(2.5*pow(10,max_num)));}
  if((max/pow(10,max_num))>2.5&&(max/pow(10,max_num))<5)
   { n2=y0-floor((y2*YHigh)/(5*pow(10,max_num)));}
  setcolor(0);
  line(m1,n1,m2,n2);
}
void ivpeak()
{
 int i,j,l;
 float temp;
 i=1;con1=1;con2=1;
 if(ivp[1].current>ivp[2].current){
  con1=0;
  for(;;){
   do {
    if(i<count) i=i+1;
    else goto draln;
    temp=ivp[i+1].current+0.1;
   }while(ivp[i].current>temp);
   wb[con1].current=ivp[i].current;
   wb[con1].scan_v=ivp[i].scan_v;
   con1=con1+1;
 do {
    if(i<count) i=i+1;
    else goto draln;
     temp=ivp[i+1].current-0.1;
   }while(ivp[i].current>temp);
   wb[con1].current=ivp[i].current;
   wb[con1].scan_v=ivp[i].scan_v;
    con1=con1+1;
   do{
    if(i<count) i=i+1;
    else goto draln;
   }while(ivp[i].current<ivp[i+1].current);
   wp[con2].current=ivp[i].current;
   wp[con2].scan_v=ivp[i].scan_v;
   con2=con2+1;
  }
 }
 else{con2=0;con1=0;
  for(;;){
   do{
    if(i<count) i=i+1;
    else goto draln;
     temp=ivp[i+1].current+0.1;
   }while(ivp[i].current<temp);
   wp[con2].current=ivp[i].current;
   wp[con2].scan_v=ivp[i].scan_v;
   con2=con2+1;
   do {
    if(i<count) i=i+1;
    else goto draln;
     temp=ivp[i+1].current+0.1;
   }while(ivp[i].current>temp);
   wb[con1].current=ivp[i].current;
   wb[con1].scan_v=ivp[i].scan_v;
   con1=con1+1;
    do {
    if(i<count) i=i+1;
    else goto draln;
     temp=ivp[i+1].current-0.1;
   }while(ivp[i].current>temp);
   wb[con1].current=ivp[i].current;
   wb[con1].scan_v=ivp[i].scan_v;
   con1=con1+1;
  }
 }
 draln:for(i=1;i<con1-1;i=i+2){if(ivp[1].scan_v<ivp[2].scan_v){
				draline(wb[i].scan_v,wb[i].current,wb[i+1].scan_v,wb[i+1].current);}
			       else{
				yjdraline(wb[i].scan_v,wb[i].current,wb[i+1].scan_v,wb[i+1].current);}
       }
 }
void draline(float x1,float y1,float x2,float y2)
 {
  int x0,y0,m1,n1,m2,n2;
  int XWide,YHigh;
  XWide=400;
  YHigh=328;
  if(spara.first_v<spara.end_v) x0=204;
  else x0=210;
  y0=400;
  m1=x0+floor(fabs((-1600)-x1)*XWide/1800);
  if((max/pow(10,max_num))>5){
   n1=y0-floor((y1*YHigh)/pow(10,max_num+1));}
  if((max/pow(10,max_num))<2.5)
   { n1=y0-floor((y1*YHigh)/(2.5*pow(10,max_num)));}
  if((max/pow(10,max_num))>2.5&&(max/pow(10,max_num))<5.)
   { n1=y0-floor((y1*YHigh)/(5*pow(10,max_num)));}

  m2=x0+floor(fabs((-1600)-x2)*XWide/1800);
  if(max/(pow(10,max_num))>5)
   { n2=y0-floor((y2*YHigh)/pow(10,max_num+1)); }
  if((max/pow(10,max_num))<2.5)
   { n2=y0-floor((y2*YHigh)/(2.5*pow(10,max_num)));}
  if((max/pow(10,max_num))>2.5 && (max/pow(10,max_num))<5)
   { n2=y0-floor((y2*YHigh)/(5*pow(10,max_num)));}

  setcolor(0);
  line(m1,n1,m2,n2);
  return;
 }
float diffrance()
 {
 int i;
 for(i=0;i<1800;i++){ivd[i].current=0;ivd[i].scan_v=0;}
 for(i=1;i<count-1;i++){
  ivd[i].current=(ivp[i+1].current-ivp[i].current)/(ivp[i+1].scan_v-ivp[i].scan_v);
  ivd[i].scan_v=(ivp[i+1].scan_v+ivp[i].scan_v)/2;
  }
 return 0;
 }
void maxfind()
  {
  int i;
  float min1;
  max=fabs(ivd[1].current);
  min1=fabs(ivd[1].current);
  for(i=1;i<=count-2;i++)
    {
    if((fabs(ivd[i].current))<min1) min1=fabs(ivd[i].current);
    if((fabs(ivd[i].current))>max) max=fabs(ivd[i].current);
    }
  if(max==min1) max=1;
  return;
  }
void smjp()
  {
  int i,sel_R;
  int tq_time,jb_time;
  float step_v;
  char *spara_c_hz[5]={
	    "准备好按任意键",
	    "正在清洗",
	    "正在富集",
	    "溶液静止，请稍候",
	    "正在扫描，请稍候",
			 };
  cls_all_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[1],0);
  bell(400,1);
  outportb(0x220,0x08);
  da_v(spara.brush_v);
  outportb(0x220,0x18);
  if(watch(50,169,30,15,spara.brush_t)==283){
    cls_left_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  cls_left_scrn();
  bell(400,1);
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[2],0);
  outportb(0x220,0x08);
  da_v(spara.fuji_v);
  if(watch(50,169,30,15,spara.fuji_t)==283){
    cls_left_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  outportb(0x220,0x00);
  bell(400,1);
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[3],0);
  for(i=0;i<spara.stat_t;i++){
    if(kbhit()){
      if(bioskey(0)==283){
	cls_left_scrn();
	bell(400,0);
	return;
	}
      else break;
      }
    sleep(1);
    }
  sel_R=spara.sel_R;
  if(sel_R>15) sel_R=15;
  else if(sel_R<0) sel_R=0;
  count=1;
  ivp[count].insist=insist_set[sel_R];
  selinsist(sel_R);
  step_v=spara.slope;
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[4],0);
  bell(400,1);
  ivp[count].scan_v=spara.first_v;
  ivp[count].current=0.;
 do{
  ivp[count].current=0.0;
  delay(spara.t_width);
  ivp[count].current+=(-1)*ad_v();
  ivp[count].insist=insist_set[sel_R];
  ivp[count].current=(ivp[count].current)/ivp[count].insist*1E+5;
  count++;
  ivp[count].scan_v=ivp[count-1].scan_v+step_v;
  da_v(ivp[count].scan_v);
  if(kbhit()){
    if(bioskey(0)==283) break;
    else button("按ＥＳＣ键返回！",0,0,1,0);
    }
  }while(ivp[count].scan_v < spara.end_v);
  bell(400,1);
  for(i=1;i<=10;i++)
    step_v+=ivp[i].current/10;
  for(i=1;i<=count;i++)
    ivp[i].current=ivp[i].current-step_v;
  da_v(spara.brush_v);
  outportb(0x220,0x00);
  for(i=0;i<6;i++){
    smooth();
    smooth1();
    }
  cls_all_scrn();
  maxfind1();
  board(1,4,16,7,0,7);
  if(sz_sys.IE==1) drascr(rate_flag,1,0);
  diffrance();
  maxfind();
  if(sz_sys.QD==1){
    board(1,4,16,7,0,7);
    diffdrascr();
    diffcalpeak();
    }
  return;
  }
void yjsmfa()
{
  int i,sel_R;
  int tq_time,jb_time;
  float step_v;
  char *spara_c_hz[5]={
	  "准备好按任意键",
	  "正在清洗",
	  "正在富集",
	  "溶液静止，请稍候",
	  "正在扫描，请稍候",
			 };
  cls_all_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[1],0);
  bell(400,1);
  da_v(spara.brush_v);
  outportb(0x220,0x18);
  if(watch(50,169,30,15,spara.brush_t)==283){
    cls_left_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  bell(400,1);
  outportb(0x220,0x08);
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[2],0);
  da_v(spara.fuji_v);
  if(watch(50,169,30,15,spara.fuji_t)==283){
    cls_left_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  outportb(0x220,0x00);
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[3],0);
  for(i=0;i<spara.stat_t;i++){
    if(kbhit()){
      if(bioskey(0)==283){
	cls_all_scrn();
	bell(400,0);
	return;
	}
      else break;
      }
    sleep(1);
    }
  sel_R=spara.sel_R;
  if(sel_R>15) sel_R=15;
  else if(sel_R<0) sel_R=0;
  count=1;
  step_v=spara.slope;
  ivp[count].scan_v=spara.first_v;
  ivp[count].insist=insist_set[sel_R];
  selinsist(sel_R);
  da_v(spara.first_v);
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[4],0);
  bell(400,1);
 do{
  ivp[count].current=0;
  delay(spara.t_width);
  ivp[count].current+=ad_v();
  ivp[count].insist=insist_set[sel_R];
  ivp[count].current=(ivp[count].current)/ivp[count].insist*1E+5;
  count++;
  ivp[count].scan_v=ivp[count-1].scan_v-step_v;
  da_v(ivp[count].scan_v);
  if(kbhit()){
    if(bioskey(0)==283) break;
    else button("按ＥＳＣ键返回！",0,0,1,0);
    }
}while(ivp[count].scan_v>=spara.end_v);
da_v(spara.brush_v);
outportb(0x220,0x00);
for(i=0;i<6;i++){
  smooth();
  smooth1();
  }
cls_all_scrn();
maxfind1();
board(1,4,16,7,0,7);
if(sz_sys.IE==1) drascr(rate_flag,0,0);
diffrance();
maxfind();
if(sz_sys.QD==1){
  board(1,4,16,7,0,7);
  diffyjdrascr();
  diffyjcalpeak();
  }
return;
}
void xhfa()
  {
  int i,sel_R,judge=1;
  int tq_time,jb_time;
  float step_v;
  char *spara_c_hz[5]={
	    "准备好按任意键",
	    "正在清洗",
	    "正在富集",
	    "溶液静止，请稍候",
	    "正在扫描，请稍候",
			 };
  cls_all_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[1],0);
  bell(400,1);
  outportb(0x220,0x08);
  da_v(spara.brush_v);
  outportb(0x220,0x18);
  if(watch(50,169,30,15,spara.brush_t)==283){
    cls_left_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  cls_left_scrn();
  bell(400,1);
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[2],0);
  outportb(0x220,0x08);
  da_v(spara.fuji_v);
  if(watch(50,169,30,15,spara.fuji_t)==283){
    cls_left_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  outportb(0x220,0x00);
  bell(400,1);
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[3],0);
  for(i=0;i<spara.stat_t;i++){
     if(kbhit()){
      if(bioskey(0)==283){
	cls_left_scrn();
	bell(400,0);
	return;
	}
      else break;
      }
    sleep(1);
    }
  sel_R=spara.sel_R;
  if(sel_R>15) sel_R=15;
  else if(sel_R<0) sel_R=0;
  count=1;
  ivp[count].insist=insist_set[sel_R];
  selinsist(sel_R);
  step_v=spara.slope;
  cls_left_scrn();
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[4],0);
  bell(400,1);
  ivp[count].scan_v=spara.first_v;
  ivp[count].current=0.;
  if(spara.first_v<=spara.end_v) judge=1;
  else judge=-1;
 for(;;){
  ivp[count].current=0.0;
  delay(spara.t_width);
  ivp[count].current+=(-1)*ad_v();
  ivp[count].insist=insist_set[sel_R];
  ivp[count].current=ivp[count].current/ivp[count].insist*1E+5;
  count++;
  ivp[count].scan_v=ivp[count-1].scan_v+judge*step_v;
  da_v(ivp[count].scan_v);
  if(fabs(ivp[count].scan_v-spara.end_v)<=step_v) judge=(-1)*judge;
  if(ivp[count].scan_v==spara.first_v) break;
  if(kbhit()){
    if(bioskey(0)==283) break;
    else button("按ＥＳＣ键返回！",0,0,1,0);
    }
  }
  bell(400,1);
  for(i=1;i<=10;i++)
    step_v+=ivp[i].current/10;
  for(i=1;i<=count;i++)
    ivp[i].current=ivp[i].current-step_v;
  da_v(spara.brush_v);
  outportb(0x220,0x00);
  for(i=0;i<6;i++){
    smooth();
    smooth1();
    }
  cls_all_scrn();
  maxfind1();
  board(1,4,16,7,0,7);
  if(sz_sys.IE==1) drascr(rate_flag,1,0);
  return;
  }
void xtzj()
{
  int t,i,sel_R,scanv;
  float inv[11];
  int key=0;
  char *spara_c_hz[2]={
			"系统正在工作",
			"扫描电位",
			 };
  if(spara.first_v>=spara.end_v){
    file_warn(5);
    return;
    }
  cls_all_scrn();
  dra_screen(0,1);
  board(1,4,17,10,20,7);
  disp_shadow_str(16,106,spara_c_hz[0],0);
  disp_shadow_str(16,142,spara_c_hz[1],0);
  bell(400,1);
  sel_R=spara.sel_R;
  if(sel_R>15) sel_R=15;
  else if(sel_R<0) sel_R=0;
  ivp[count].scan_v=spara.first_v;
  for(i=0;i<=11;i++) inv[i]=0;
  count=1;
  ivp[count].scan_v=spara.first_v;
  ivp[count].insist=insist_set[sel_R];
  selinsist(sel_R);
  ivp[count].current=0;
//  dra_screen(0,1);
 do{
  outportb(0x220,0x40);
  scanv=ivp[count].scan_v;
  itoa_text(90,148,scanv,0,7);
  for(t=1;t<=(spara.t_width*1000-500);t++)
   {
    delay(1);
   }
  for(t=1;t<=7;t++){
   delay(2);
   inv[t]=ad_v();
  }
   for(t=1;t<=6;t++){
    for(i=t+1;i<=7;i++){
    if(inv[t]>inv[i]){ inv[t]=inv[i]; }
   }
   }
   for(t=2;t<=6;t++){
   ivp[count].current=ivp[count].current+inv[t];
   }
   ivp[count].current=ivp[count].current/5;
   count=count+1;
   ivp[count].scan_v=ivp[count-1].scan_v+spara.pulse;
   da_v(ivp[count].scan_v);
   for(t=1;t<=(spara.p_width);t++) delay(1);
   for(t=1;t<=7;t++){
    delay(2);
    inv[t]=ad_v();
   }
   for(t=1;t<=6;t++){
    for(i=t+1;i<=7;i++){
    if(inv[t]>inv[i]){ inv[t]=inv[i]; }
   }
   }
   for(t=2;t<=6;t++){
   ivp[count].current=ivp[count].current+inv[t];
   }
    ivp[count].current=ivp[count].current/5;
    ivp[count].scan_v=ivp[count].scan_v-spara.pulse+spara.slope;
    da_v(ivp[count].scan_v);
    selinsist(sel_R);
    ivp[count].insist=insist_set[sel_R];
    ivp[count-1].current=(ivp[count-1].current-ivp[count].current);
    ivp[count-1].current=(ivp[count-1].current)/ivp[count-1].insist*1E+5;
    drascreen(ivp[count-1].scan_v,ivp[count-1].current/cur_flag,0);
    if(kbhit()){
      key=bioskey(0);
      switch(key){
	case 283:
	     bell(400,0);
	     goto BRK_ZJ;
	case 18688:
	     if(cur_flag>0.01){
	       cls_right_scrn();
	       dra_screen(0,1);
	       cur_flag=cur_flag/10;
	       for(i=0;i<count;i++)
		 drascreen(ivp[i].scan_v,ivp[i].current/cur_flag,0);
	       }
	     break;
	case 20736:
	     if(cur_flag<1000000){
	       cls_right_scrn();
	       dra_screen(0,1);
	       cur_flag=cur_flag*10;
	       for(i=0;i<count;i++)
		 drascreen(ivp[i].scan_v,ivp[i].current/cur_flag,0);
	       }
	     break;
	case 18432:
	     sel_R++;
	     if(sel_R>15) sel_R=15;
	     selinsist(sel_R);
	     break;
	case 20480:
	     sel_R--;
	     if(sel_R<0) sel_R=0;
	     selinsist(sel_R);
	     break;
	default:
	     button("ＰｇＵｐ：放大；ＰｇＤｎ：缩小；↑、↓：换档；ＥＳＣ：返回！  ",0,0,1,0);
	     break;
	}
     spara.sel_R=sel_R;
     }
}while(ivp[count].scan_v <= spara.end_v);
bell(400,1);
BRK_ZJ: da_v(spara.brush_v);
return;
}
void mcrcfa()
  {
  int t,i,sel_R,scanv,key;
  float inv[11];
  int jg_time=0;
  char *spara_c_hz[7]={
			"系统正在工作",
			"扫描电位",
			"正在清洗",
			"准备好后按任一键",
			"正在通气",
			"正在富集",
			"溶液静止，请稍候",
			 };
  cls_all_scrn();
  dra_screen(0,1);
  board(1,4,20,7,0,7);
  disp_shadow_str(16,96,spara_c_hz[2],0);
  bell(400,1);
  outportb(0x220,0x18);
  da_v(spara.brush_v);
  if(watch(50,169,30,15,spara.brush_t)==283){
    cls_all_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  bell(400,1);
  cls_left_scrn();
  if(mc_flag==0){
    board(1,4,20,7,0,7);
    disp_shadow_str(16,96,spara_c_hz[5],0);
    outportb(0x220,0x08);
    da_v(spara.fuji_v);
    if(watch(50,169,30,15,spara.fuji_t)==283){
      cls_all_scrn();
      outportb(0x220,0x00);
      bell(400,0);
      return;
      }
    bell(400,1);
    cls_left_scrn();
    }
  outportb(0x220,0x00);
  board(1,4,20,7,0,7);
  disp_shadow_str(14,92,spara_c_hz[6],0);
  for(i=0;i<spara.stat_t;i++){
    if(kbhit()){
      if(bioskey(0)==283){
	cls_all_scrn();
	bell(400,0);
	return;
	}
      else break;
      }
    sleep(1);
    }
  sel_R=spara.sel_R;
  if(sel_R>15) sel_R=15;
  else if(sel_R<0) sel_R=0;
  cls_left_scrn();
  board(1,4,17,10,20,7);
  disp_shadow_str(16,106,spara_c_hz[0],0);
  disp_shadow_str(16,142,spara_c_hz[1],0);
  bell(400,1);
  count=1;
  ivp[count].scan_v=spara.first_v;
  for(i=0;i<=11;i++){inv[i]=0.;}
  ivp[count].scan_v=spara.first_v;
  ivp[count].insist=insist_set[sel_R];
  selinsist(sel_R);
  ivp[count].current=0;
  da_v(spara.first_v);
  if(spara.t_width==0) jg_time=500;
  else jg_time=spara.t_width*1000;
//  dra_screen(0,1);
 do{
  scanv=ivp[count].scan_v;
  itoa_text(90,148,scanv,0,7);
  for(t=1;t<=(jg_time-spara.p_width-30);t++)
    {
    delay(1);
    }
  for(t=1;t<=3;t++){
   delay(4);
   //inv[t]=ad_v();
  }
/*   for(t=1;t<=6;t++){
    for(i=t+1;i<=7;i++){
    if(inv[t]>inv[i]){ inv[t]=inv[i];}
   }
   }*/
   //for(t=1;t<=3;t++){
   do{
     ivp[count].current=ad_v();//inv[t];
     if(ivp[count].current<-2000){
       bell(400,1);
       //outportb(0x280,0x00); delay(10);
       }
     }while(ivp[count].current<-2000);
   //}
   // ivp[count].current=ivp[count].current/3;
    count=count+1;
   ivp[count].scan_v=ivp[count-1].scan_v+spara.pulse;
   da_v(ivp[count].scan_v);
   for(t=1;t<=(spara.p_width);t++) delay(1);
   for(t=1;t<=3;t++){
    delay(4);
    //inv[t]=ad_v();
   }
/*   for(t=1;t<=6;t++){
    for(i=t+1;i<=7;i++){
    if(inv[t]>inv[i]){inv[t]=inv[i];}
   }
   }*/
  // for(t=1;t<=3;t++){
  do{
   ivp[count].current=ad_v();//inv[t];
   if(ivp[count].current<-2000){
     bell(400,1);
     //outportb(0x280,0x00); delay(10);
     }
   }while(ivp[count].current<-2000);
   //}
   //ivp[count].current=ivp[count].current/3;
//   printf("current: %f ",ivp[count].current);
   ivp[count].scan_v=ivp[count].scan_v-spara.pulse+spara.slope;
    da_v(ivp[count].scan_v);
    sel_R=spara.sel_R;
    selinsist(sel_R);
    ivp[count].insist=insist_set[sel_R];
    ivp[count-1].current=(ivp[count-1].current-ivp[count].current);
    ivp[count-1].current=(ivp[count-1].current)/ivp[count-1].insist*1E+6;
    drascreen(ivp[count-1].scan_v,ivp[count-1].current/cur_flag,0);
  //  printf("current0: %f insist %f\n",ivp[count-1].current,ivp[count-1].insist);
    printf("scan_v %d current %f\n",ivp[count-1].scan_v,ivp[count-1].current);
    if(kbhit()){
      key=bioskey(0);
      switch(key){
	case 283:
	     bell(400,0);
	     goto BRK_MC;
	case 18688:
	     if(cur_flag>0.01){
	       cls_right_scrn();
	       dra_screen(0,1);
	       cur_flag=cur_flag/10;
	       for(i=0;i<count;i++)
		 drascreen(ivp[i].scan_v,ivp[i].current/cur_flag,0);
	       }
	     break;
	case 20736:
	     if(cur_flag<1000000){
	       cls_right_scrn();
	       dra_screen(0,1);
	       cur_flag=cur_flag*10;
	       for(i=0;i<count;i++)
	       drascreen(ivp[i].scan_v,ivp[i].current/cur_flag,0);
	       }
	     break;
	case 18432:
	     sel_R++;
	     if(sel_R>15) sel_R=15;
	     selinsist(sel_R);
	     break;
	case 20480:
	     sel_R--;
	     if(sel_R<0) sel_R=0;
	     selinsist(sel_R);
	     break;
	default:
	     button("ＰｇＵｐ：放大；ＰｇＤｎ：缩小；↑、↓：换档；ＥＳＣ：返回！  ",0,0,1,0);
	     break;
	}
     spara.sel_R=sel_R;
     }
}while(ivp[count].scan_v <= spara.end_v);
bell(400,1);
BRK_MC: da_v(spara.brush_v);
 for(i=0;i<6;i++){
    smooth();
    smooth1();
    }
 cls_all_scrn();
 maxfind1();
 board(1,4,16,7,0,7);
 if(sz_sys.IE==1) drascr(rate_flag,1,0);
 diffrance();
 maxfind();
 if(sz_sys.QD==1){
   board(1,4,16,7,0,7);
   diffdrascr();
   diffcalpeak();
   }
 return;
}
void yjrcfa()
{
  int t,i,sel_R,scanv,key;
  int jg_time=0;
  float inv[11];
  char *spara_c_hz[7]={
			"系统正在工作",
			"扫描电位",
			"正在清洗",
			"准备好后按任一键",
			"正在通气",
			"正在富集",
			"溶液静止，请稍候",
			 };
  cls_all_scrn();
  board(1,4,20,7,0,7);
  dra_screen(0,0);
  disp_shadow_str(16,96,spara_c_hz[2],0);
  bell(400,1);
  outportb(0x220,0x18);
  da_v(spara.brush_v);
  if(watch(50,169,30,15,spara.brush_t)==283){
    cls_all_scrn();
    outportb(0x220,0x00);
    bell(400,0);
    return;
    }
  bell(400,1);
  cls_left_scrn();
  if(mc_flag==0){
    board(1,4,20,7,0,7);
    disp_shadow_str(16,96,spara_c_hz[5],0);
    outportb(0x220,0x08);
    da_v(spara.fuji_v);
    if(watch(50,169,30,15,spara.fuji_t)==283){
      cls_all_scrn();
      outportb(0x220,0x00);
      bell(400,0);
      return;
      }
    bell(400,1);
    cls_left_scrn();
    }
  outportb(0x220,0x00);
  board(1,4,20,7,0,7);
  disp_shadow_str(16,92,spara_c_hz[6],0);
  for(i=0;i<spara.stat_t;i++){
    if(kbhit()){
      if(bioskey(0)==283){
	cls_all_scrn();
	bell(400,0);
	return;
	}
      else break;
      }
    sleep(1);
    }
  sel_R=spara.sel_R;
  if(sel_R>15) sel_R=15;
  else if(sel_R<0) sel_R=0;
  cls_left_scrn();
  board(1,4,17,10,20,7);
  disp_shadow_str(16,106,spara_c_hz[0],0);
  disp_shadow_str(16,142,spara_c_hz[1],0);
  bell(400,1);
  count=1;
  ivp[count].scan_v=spara.first_v;
  for(i=0;i<=11;i++){inv[i]=0.;}
  ivp[count].scan_v=spara.first_v;
  ivp[count].insist=insist_set[sel_R];
  selinsist(sel_R);
  ivp[count].current=0;
  if(spara.t_width==0) jg_time=500;
  else jg_time=spara.t_width*1000;
//  dra_screen(0,0);
 do{
  scanv=ivp[count].scan_v;
  itoa_text(90,148,scanv,0,7);
  for(t=1;t<=(jg_time-spara.p_width-30);t++)
   {
   delay(1);
   }
  for(t=1;t<=7;t++){
   delay(2);
   inv[t]=ad_v();
   }
   for(t=1;t<=6;t++){
    for(i=t+1;i<=7;i++){
    if(inv[t]>inv[i]){inv[t]=inv[i];}
   }
   }
   for(t=2;t<=6;t++){
   ivp[count].current=ivp[count].current+inv[t];
   }
    ivp[count].current=ivp[count].current/5;
    count=count+1;
    ivp[count].scan_v=ivp[count-1].scan_v+spara.pulse;
    da_v(ivp[count].scan_v);
   for(t=1;t<=(spara.p_width);t++) delay(1);
   for(t=1;t<=7;t++){
    delay(2);
    inv[t]=ad_v();
     }
   for(t=1;t<=6;t++){
    for(i=t+1;i<=7;i++){
    if(inv[t]>inv[i]){inv[t]=inv[i];}
   }
   }
   for(t=2;t<=6;t++){
   ivp[count].current=ivp[count].current+inv[t];
   }
    ivp[count].current=ivp[count].current/5;
    ivp[count].scan_v=ivp[count].scan_v-spara.pulse-spara.slope;
    da_v(ivp[count].scan_v);
    sel_R=spara.sel_R;
    selinsist(sel_R);
    ivp[count].insist=insist_set[sel_R];
    ivp[count-1].current=(ivp[count-1].current-ivp[count].current);
    ivp[count-1].current=(ivp[count-1].current)/ivp[count-1].insist*1E+6;
    yjdrascreen(ivp[count-1].scan_v,ivp[count-1].current/cur_flag,0);
    if(kbhit()){
      key=bioskey(0);
      switch(key){
	case 283:
	     bell(400,0);
	     goto BRK_YJ;
	case 18688:
	     if(cur_flag>0.01){
	       cls_right_scrn();
	       dra_screen(0,0);
	       cur_flag=cur_flag/10;
	       for(i=0;i<count;i++)
		 yjdrascreen(ivp[i].scan_v,ivp[i].current/cur_flag,0);
	       }
	     break;
	case 20736:
	     if(cur_flag<1000000){
	       cls_right_scrn();
	       dra_screen(0,0);
	       cur_flag=cur_flag*10;
	       for(i=0;i<count;i++)
	       yjdrascreen(ivp[i].scan_v,ivp[i].current/cur_flag,0);
	       }
	     break;
	case 18432:
	     sel_R++;
	     if(sel_R>15) sel_R=15;
	     selinsist(sel_R);
	     break;
	case 20480:
	     sel_R--;
	     if(sel_R<0) sel_R=0;
	     selinsist(sel_R);
	     break;
	default:
	     button("ＰｇＵｐ：放大；ＰｇＤｎ：缩小；↑、↓：换档；ＥＳＣ：返回！  ",0,0,1,0);
	     break;
	}
     spara.sel_R=sel_R;
     }
}while(ivp[count].scan_v >= spara.end_v);
bell(400,1);
BRK_YJ: da_v(spara.brush_v);
 for(i=0;i<6;i++){
    smooth();
    smooth1();
    }
 cls_all_scrn();
 maxfind1();
 board(1,4,16,7,0,7);
 if(sz_sys.IE==1) drascr(rate_flag,0,0);
 diffrance();
 maxfind();
 if(sz_sys.QD==1){
   board(1,4,16,7,0,7);
   diffyjdrascr();
   diffyjcalpeak();
   }
 return;
}
void selinsist(int dw)
{
 int i,temp,temp_226;
 float insist;
 temp=temp_226&0x00;
 dw=temp|dw;
 outportb(0x226,dw);
}
float datasmoothweights(float *dataset,int numdat,int smoothnum,
		       float *weights,float wdivisor,float *smoothdata)
  {
  int i;
  int j;
  int k;
  int smoothdeg;
  int startconv;
  int stopconv;
  int numcpef;
  if((smoothnum>=1)&&(fmod(smoothnum,2)!=0)){
     smoothdeg=smoothnum/2;
     startconv=smoothdeg ;
     stopconv=numdat-smoothdeg-1;
     for(i=0;i<=numdat-1;++i){
	smoothdata[i]=0;
     }
   for(i=startconv;i<=stopconv;++i){
    for(j=0;j<=smoothnum-1;++j){
      smoothdata[i]=smoothdata[i]+dataset[i-smoothdeg+j]*weights[j];
    }
    smoothdata[i]=smoothdata[i]/wdivisor;
   }
   j=smoothdeg-1;
   k=numdat-smoothdeg;
   for(i=1;i<=smoothdeg;++i){
    smoothdata[j]=smoothdata[j+1];
    smoothdata[k]=smoothdata[k-1];
    j=j-1;
    k=k+1;
   }
 }
 return 0;
}
float smooth(void)
{
 float smoothdata[2000],dataset[2000];
 int i,numdat,smoothnum;
 float weights[5],wdivisor;
 numdat=count;smoothnum=5;
 weights[0]=1;weights[1]=2;weights[2]=3;weights[3]=2;weights[4]=1;
 wdivisor=9;
 for(i=0;i<=count-1;++i){
  dataset[i]=ivp[i+1].current;
 }
 datasmoothweights(dataset,numdat,smoothnum,weights,wdivisor,smoothdata);
 for(i=0;i<=count-1;++i){
  ivp[i+1].current=smoothdata[i];
 }
 return 0;
}
float smooth1(void)
{
 float smoothdata[2000],dataset[2000];
 int i,numdat,smoothnum;
 float weights[5],wdivisor;
 numdat=count-2;smoothnum=5;
 weights[0]=1;weights[1]=2;weights[2]=3;weights[3]=2;weights[4]=1;
 wdivisor=9;
 for(i=0;i<=count-3;i++){
  dataset[i]=ivd[i+1].current;
 }
 datasmoothweights(dataset,numdat,smoothnum,weights,wdivisor,smoothdata);
 for(i=0;i<=count-3;i++){
  ivd[i+1].current=smoothdata[i];
 }
 return 0;
}
void  maxfind1(void)
 {
  float min;
  int i;
  max=fabs(ivp[1].current);
  min=fabs(ivp[1].current);
  for(i=1;i<=count;i++)
   {
    if((fabs(ivp[i].current))<min) min=fabs(ivp[i].current);
    if((fabs(ivp[i].current))>max) max=fabs(ivp[i].current);
  }
  if(max==min) max=1;
  return;
 }
void disp_seat(int x,int y,int value,int color)
  {
  char *str="*****";
  itoa(value,str,10);
  disp_hz_str(x,y,str,color);
  }
int print_spara()
  {
  int l1=70,l2=140,l3=600,l4=390;
  win_board(l1-25,l2-60,l3,l4,30,7,0);
  disp_hz_str(l1+200,l2-56,"测试参数表",15);
  disp_hz_str(l1,l2-10,"实验单位:________________________ 实验人:________________",15);
  disp_hz_str(l1,l2+40,"---------------------------------------------------------",15);
  disp_hz_str(l1,l2+60,"实验日期:",15);
  disp_seat(l1+80,l2+60,te_d.te_year,15);
  disp_hz_str(l1+116,l2+60,"-",15);
  disp_seat(l1+124,l2+60,te_d.te_mon,15);
  disp_hz_str(l1+142,l2+60,"-",15);
  disp_seat(l1+150,l2+60,te_d.te_day,15);
  disp_hz_str(l1+260,l2+60,"实验时间:",15);
  disp_seat(l1+340,l2+60,te_t.te_hour,15);
  disp_hz_str(l1+360,l2+60,":",15);
  disp_seat(l1+370,l2+60,te_t.te_min,15);
  disp_hz_str(l1+390,l2+60,":",15);
  disp_seat(l1+400,l2+60,te_t.te_sec,15);
  disp_hz_str(l1,l2+80,"富集电位:",15);
  disp_seat(l1+80,l2+80,spara.fuji_v,15);
  disp_hz_str(l1+128,l2+80,"MV",15);
  disp_hz_str(l1+260,l2+80,"富集时间:",15);
  disp_seat(l1+340,l2+80,spara.fuji_t,15);
  disp_hz_str(l1+390,l2+80,"S",15);
  disp_hz_str(l1,l2+100,"起始电位:",15);
  disp_seat(l1+80,l2+100,spara.first_v,15);
  disp_hz_str(l1+128,l2+100,"MV",15);
  disp_hz_str(l1+260,l2+100,"终止电位:",15);
  disp_seat(l1+340,l2+100,spara.end_v,15);
  disp_hz_str(l1+390,l2+100,"MV",15);
  disp_hz_str(l1,l2+120,"脉冲增量:",15);
  disp_seat(l1+80,l2+120,spara.pulse,15);
  disp_hz_str(l1+128,l2+120,"MV",15);
  disp_hz_str(l1+260,l2+120,"脉冲宽度:",15);
  disp_seat(l1+340,l2+120,spara.p_width,15);
  disp_hz_str(l1+390,l2+120,"MS",15);
  disp_hz_str(l1,l2+140,"间隔时间:",15);
  disp_seat(l1+80,l2+140,spara.t_width,15);
  disp_hz_str(l1+128,l2+140,"S",15);
  disp_hz_str(l1+260,l2+140,"斜率增量:",15);
  disp_seat(l1+340,l2+140,spara.slope,15);
  disp_hz_str(l1+390,l2+140,"MV",15);
  disp_hz_str(l1,l2+160,"清洗电压:",15);
  disp_seat(l1+80,l2+160,spara.brush_v,15);
  disp_hz_str(l1+128,l2+160,"MV",15);
  disp_hz_str(l1+260,l2+160,"清洗时间:",15);
  disp_seat(l1+340,l2+160,spara.brush_t,15);
  disp_hz_str(l1+390,l2+160,"S",15);
  disp_hz_str(l1,l2+180,"静止时间:",15);
  disp_seat(l1+80,l2+180,spara.stat_t,15);
  disp_hz_str(l1+128,l2+180,"S",15);
  disp_hz_str(l1,l2+200,"扫描档位:",15);
  disp_seat(l1+80,l2+200,spara.sel_R,15);
  disp_hz_str(l1+260,l2+200,"档位倍率:",15);
  disp_seat(l1+340,l2+200,spara.sel_bl,15);
  disp_hz_str(l1,l2+220,"---------------------------------------------------------",15);
  if(print_test()<0) return(-1);
  else{
    print_2(l1-18,l2-30,l3-20,l4-10+25*sz_sys.prn,15,15);
    }
  return(0);
  }
float calpeak()
 {
 int pj,i,j,m0,n0,xx0,yy0;
 char buf[12];
  float x0,y0,x1,y1,x2,y2,b0,b1;
  int Max_x,Max_y,XWide,YHigh;
  XWide=400;
  YHigh=328;
  if(spara.first_v<spara.end_v) xx0=208;
  else xx0=206;
  yy0=400;
  j=100;pi=0;pj=0;
  for(i=1;i<(con1-1);i=i+2){ pi=pi+1;pj=pj+1;
  y1=wb[i].current;x1=wb[i].scan_v;x2=wb[i+1].scan_v;y2=wb[i+1].current;
  b1=(y1-y2)/(x1-x2);b0=(y2*x1-x2*y1)/(x1-x2);
  x0=wp[pj].scan_v;
  y0=b0+b1*x0;
  ivtop.current[pi]=wp[pj].current-y0;
  ivtop.scan_v[pi]=wp[pj].scan_v;
  if(ivtop.current[pi]<2){pi=pi-1;}
  else{
  if(ivp[1].scan_v<ivp[2].scan_v){
   m0=xx0+floor(fabs((-1600)-wp[pj].scan_v)*XWide/1800);}
  else{
   m0=xx0+floor(fabs((200)-wp[pj].scan_v)*XWide/1800);}
  if((max/pow(10,max_num))>5){
   n0=yy0-floor((wp[pj].current*YHigh)/pow(10,max_num+1));}
  if((max/pow(10,max_num))<2.5){
   n0=yy0-floor((wp[pj].current*YHigh)/(2.5*pow(10,max_num)));}
  if(((max/pow(10,max_num))>2.5)&&((max/pow(10,max_num))<5)){
    n0=yy0-floor((wp[pj].current*YHigh)/(5*pow(10,max_num)));
    }
  moveto(m0,n0-8);itoa(pi,buf,10);outtext(buf);
  j=j+20;
  }
 }
 board(1,7,16,pi+8,0,7);
 for(i=1;i<=pi;i++){
   setcolor(0);
   moveto(18,i*20+120);itoa(i,buf,10);outtext(buf);
   moveto(38,i*20+120);gcvt(ivtop.current[i],dpoint(ivtop.current[i],3),buf);outtext(buf);outtext(" NA");
   moveto(38,i*20+130);itoa(ivtop.scan_v[i],buf,10);outtext(buf);outtext(" MV");
 }
 return 0;
}
float diffcalpeak()
 {
 struct ivdtop{
 float current[20];
 int scan_v[20];
 }ivdtop;
struct wdpeak{
 float current;
 int scan_v;
 }wdp[100];
 int pj,i,j,x0,y0,m0,n0,max_num1;
 char buf[12];
 float x1,y1,y2,y3,x4,y4,y5,y6,y7;
 int XWide,YHigh;
  x1=0;y1=0;y2=0;y3=0;y4=0;y5=0;y6=0;y7=0;x4=0;
  XWide=400;
  YHigh=164;
  if(spara.first_v<spara.end_v) x0=204;
  else x0=210;
  y0=236;
  max_num1=0;
  pi=0;pj=0;
  max_num1=dpoint(max,0)-1;
  for(i=1;i<100;i++){wdp[i].current=0;wdp[i].scan_v=0;}
  for(i=1;i<20;i++){ivdtop.current[i]=0;ivdtop.scan_v[i]=0;}
  for(i=1;i<(count-12);i=i+1){
   y1=ivd[i].current;y2=ivd[i+1].current;y3=ivd[i+2].current;
   y4=ivd[i+3].current;y5=ivd[i+4].current;
   y6=ivd[i+5].current;y7=ivd[i+6].current;
   x4=ivd[i+3].scan_v;
  if(((y1<y2)&&(y2<y3)&&(y3<y4)&&(y5<y4)&&(y6<y5)&&(y7<y6))||((y2<y1)&&(y3<y2)&&(y4<y3)&&(y4<y5)&&(y5<y6)&&(y6<y7))){
    pj=pj+1; wdp[pj].current=y4;wdp[pj].scan_v=x4;}
  }
  i=1;
  do{
   do{
    do{
     y1=wdp[i].current;
     x1=wdp[i].scan_v;
     i=i+1;
     }while(y1<=0);
     if(i>pj) break;
     y2=wdp[i].current;
    }while(y2>=0);
    if(i>pj) break;
    i=i+1;
    if((y1-y2)>=0.1){
    pi=pi+1;
    ivdtop.current[pi]=(y1-y2);
    ivdtop.scan_v[pi]=x1;
   if(ivp[1].scan_v<=ivp[2].scan_v){
    m0=x0+floor(fabs((-1600)-ivdtop.scan_v[pi])*XWide/1800);}
   else{
    m0=x0+floor(fabs((200)-ivdtop.scan_v[pi])*XWide/1800);}
   if((max/pow(10,max_num1))>5){
    n0=y0-floor((y1*YHigh)/pow(10,max_num1+1));}
   if((max/pow(10,max_num1))<2.5){
    n0=y0-floor((y1*YHigh)/(2.5*pow(10,max_num1)));}
    if(((max/pow(10,max_num1))>2.5)&&((max/pow(10,max_num1))<5)){
      n0=y0-floor((y1*YHigh)/(5*pow(10,max_num1)));
      }
    moveto(m0,n0-8);itoa(pi,buf,10);outtext(buf);
 }
 }while(i<pj);
 board(1,7,16,pi+8,0,7);
 for(i=1;i<=pi;i++){
   setcolor(12);
   moveto(18,i*18+120);itoa(i,buf,10);outtext(buf);
   moveto(38,i*18+120);gcvt(ivdtop.current[i]/100,dpoint(ivdtop.current[i]/100,3),buf);outtext(buf);outtext(" NA");
   moveto(38,i*18+130);itoa(ivdtop.scan_v[i],buf,10);outtext(buf);outtext(" MV");
 }
 return 0;
}
float diffyjcalpeak()
{
struct ivdtop{
 float current[20];
 int scan_v[20];
 }ivdtop;
struct wdpeak{
 float current;
 int scan_v;
 }wdp[100];
 int pj,i,j,x0,y0,m0,n0;
 char buf[12];
 float x2,y1,y2,y3,x4,y4,y5,y6,y7;
  int XWide,YHigh;
  XWide=400;
  YHigh=164;
  if(spara.first_v<spara.end_v) x0=204;
  else x0=210;
  y0=236;
  pi=0;pj=0;
  x2=0;y1=0;y2=0;y3=0;y4=0;y5=0;y6=0;y7=0;x4=0;
  for(i=1;i<100;i++){wdp[i].current=0;wdp[i].scan_v=0;}
  for(i=1;i<20;i++){ivdtop.current[i]=0;ivdtop.scan_v[i]=0;}
  for(i=1;i<(count-12);i=i+1){
   y1=ivd[i].current;y2=ivd[i+1].current;y3=ivd[i+2].current;
   y4=ivd[i+3].current;y5=ivd[i+4].current;
   y6=ivd[i+5].current;y7=ivd[i+6].current;
   x4=ivd[i+3].scan_v;
  if(((y1<y2)&&(y2<y3)&&(y3<y4)&&(y5<y4)&&(y6<y5)&&(y7<y6))||((y2<y1)&&(y3<y2)&&(y4<y3)&&(y4<y5)&&(y5<y6)&&(y6<y7))){
    pj=pj+1; wdp[pj].current=y4;wdp[pj].scan_v=x4;}
  }
 i=0;
 do{
   do{
    do{
     y1=wdp[i].current;
     i++;
     }while(y1>=0);
     if(i>pj) break;
     y2=wdp[i].current;
     x2=wdp[i].scan_v;
    }while(y2<=0);
    if(i>pj) break;
    i=i+1;
    if((y2-y1)>=0.1){
    pi=pi+1;
    ivdtop.current[pi]=(y2-y1);
    ivdtop.scan_v[pi]=x2;
   if(ivp[1].scan_v<=ivp[2].scan_v){
    m0=x0+floor(fabs((-1600)-ivdtop.scan_v[pi])*XWide/1800);}
   else{
    m0=x0+floor(fabs((200)-ivdtop.scan_v[pi])*XWide/1800);}
   if((max/pow(10,max_num))>5){
    n0=y0-floor((y2*YHigh)/pow(10,max_num+1));}
   if((max/pow(10,max_num))<2.5){
    n0=y0-floor((y2*YHigh)/(2.5*pow(10,max_num)));}
    if(((max/pow(10,max_num))>2.5)&&((max/pow(10,max_num))<5)){
      n0=y0-floor((y2*YHigh)/(5*pow(10,max_num)));
      }
    moveto(m0,n0-8);itoa(pi,buf,10);outtext(buf);
   }
 }while(i<pj);
 board(1,7,16,pi+8,0,7);
 for(i=1;i<=pi;i++){
   setcolor(12);
   moveto(18,i*18+120);itoa(i,buf,10);outtext(buf);
   moveto(38,i*18+120);gcvt(ivdtop.current[i]/100,dpoint(ivdtop.current[i]/100,3),buf);outtext(buf);outtext(" NA");
   moveto(38,i*18+130);itoa(ivdtop.scan_v[i],buf,10);outtext(buf);outtext(" MV");
   }
 return 0;
 }
void scrn_cun(void)
  {
  char *pf;
  int i=0;
  FILE *fp;
  int number,handle;
  int press_key=0;
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  getdate(&d);
  gettime(&t);
  if(get_str(filename,0,0)==27) return;
  _fmode=O_BINARY;
  pf=searchpath(filename[0]);
  if(pf!=NULL){
       press_key=file_warn(2);
       if((press_key==5497)||(press_key==5465)){
	 fp=fopen(filename[0],"wb");
	 fprintf(fp,"%d\n",2);
	 fprintf(fp,"%d %d %d %d %d %d %d\n",0,d.da_year,d.da_mon,d.da_day,t.ti_hour,t.ti_min,t.ti_sec);
	 fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d\n",spara.fuji_v,spara.fuji_t,spara.first_v,spara.end_v,spara.pulse,
	   spara.p_width,spara.t_width,spara.slope,spara.brush_v,spara.brush_t,spara.stat_t,spara.sel_R,spara.sel_bl);
	 fprintf(fp,"%d\n",count);
	 for(i=0;i<=count-1;i++)
	   {
	   fprintf(fp," %d",ivp[i+1].scan_v);
	   fprintf(fp," %9.2f",ivp[i+1].current);
	   }
	 fclose(fp);
	 }
      }
    else{
       handle=creat(filename[0],S_IWRITE|S_IREAD);
       fp=fdopen(handle,"wb");
       if(fp==NULL){
	 file_warn(3);
	 return;
	 }
       fprintf(fp,"%d\n",2);
       fprintf(fp,"%d %d %d %d %d %d %d\n",0,d.da_year,d.da_mon,d.da_day,t.ti_hour,t.ti_min,t.ti_sec);
       fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d\n",spara.fuji_v,spara.fuji_t,spara.first_v,spara.end_v,spara.pulse,
	 spara.p_width,spara.t_width,spara.slope,spara.brush_v,spara.brush_t,spara.stat_t,spara.sel_R,spara.sel_bl);
       fprintf(fp,"%d\n",count);
       for(i=0;i<=count-1;i++)
	 {
	 fprintf(fp," %d",ivp[i+1].scan_v);
	 fprintf(fp," %9.2f",ivp[i+1].current);
	 }
       fclose(fp);
    }
  bell(400,1);
  return;
  }
void sys_diao()
  {
  int handle;
  int i;
  char buf[11];
  char *buff[3];
  handle=open("jpy.sys",O_RDONLY|O_BINARY,S_IWRITE|S_IREAD);
  for(i=0;i<3;i++){
    _read(handle,buf,11);
    buff[i]=(char *)malloc(sizeof("123456789"));
    strcpy(buff[i],buf);
    }
  if(strncmp(buff[0],"Printer=1",9)==0) sz_sys.prn=1;
  else if(strncmp(buff[0],"Printer=2",9)==0) sz_sys.prn=2;
  else if(strncmp(buff[0],"Printer=3",9)==0) sz_sys.prn=3;
  else if(strncmp(buff[0],"Printer=4",9)==0) sz_sys.prn=4;
  else if(strncmp(buff[0],"Printer=5",9)==0) sz_sys.prn=5;
  else if(strncmp(buff[0],"Printer=6",9)==0) sz_sys.prn=6;
  else if(strncmp(buff[0],"Printer=7",9)==0) sz_sys.prn=7;
  else if(strncmp(buff[0],"Printer=8",9)==0) sz_sys.prn=8;
  else if(strncmp(buff[0],"Printer=9",9)==0) sz_sys.prn=9;
  else if(strncmp(buff[0],"Printer-1",9)==0) sz_sys.prn=-1;
  else if(strncmp(buff[0],"Printer-2",9)==0) sz_sys.prn=-2;
  else if(strncmp(buff[0],"Printer-3",9)==0) sz_sys.prn=-3;
  else if(strncmp(buff[0],"Printer-4",9)==0) sz_sys.prn=-4;
  else if(strncmp(buff[0],"Printer-5",9)==0) sz_sys.prn=-5;
  else sz_sys.prn=0;
  if(strncmp(buff[1],"IE_line=0",9)==0) sz_sys.IE=0;
  else sz_sys.IE=1;
  if(strncmp(buff[2],"QD_line=0",9)==0) sz_sys.QD=0;
  else sz_sys.QD=1;
  for(i=0;i<3;i++) free(buff[i]);
  return;
  }
int judge_print(void)
  {
  return(sz_sys.prn);
  }
void bell(int num,int type)
 {
 int i;
 for(i=0;i<num;i++){
    if(type>0) sound(940);
    else{
     if(i<200) sound(1040);
     else sound(840);
     }
   if(kbhit()) break;
   delay(1);
   }
 nosound();
 return;
 }
void scrn_diao(void)
  {
  FILE *fp;
  int i,number;
  int exxt;
  exxt=get_str(filename,0,1);
  if(exxt==ESCAPE) return;
  if(getpixel(206,420)!=8) cls_all_scrn();
  fp=fopen(filename[0],"r");
  if(fp==NULL){
    file_warn(0);
    return;
    }
  fscanf(fp,"%d",&number);
  if(number!=2){
    file_warn(1);
    return;
    }
  fscanf(fp,"%d %d %d %d %d %d %d",&number,&te_d.te_year,&te_d.te_mon,&te_d.te_day,&te_t.te_hour,&te_t.te_min,&te_t.te_sec);
  if(number==0)
    fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d",&spara.fuji_v,&spara.fuji_t,&spara.first_v,&spara.end_v,&spara.pulse,&spara.p_width,&spara.t_width,
      &spara.slope,&spara.brush_v,&spara.brush_t,&spara.stat_t,&spara.sel_R,&spara.sel_bl);
  else{
    if(number==1){
      fscanf(fp,"%d %d %d %d %d %d %d %d %d %d %d",&spara.fuji_v,&spara.fuji_t,&spara.first_v,&spara.end_v,&spara.pulse,&spara.p_width,&spara.t_width,
	&spara.slope,&spara.brush_v,&spara.brush_t,&spara.sel_R);
	}
    else{
      fscanf(fp,"%d %d %d %d %d %d %d %d %d",&spara.first_v,&spara.end_v,&spara.pulse,&spara.p_width,&spara.t_width,
	&spara.slope,&spara.brush_v,&spara.brush_t,&spara.sel_R);
	}
    }
  fscanf(fp,"%d",&count);
  for(i=0;i<=count-1;++i)
    {
    fscanf(fp," %d",&ivp[i+1].scan_v);
    fscanf(fp," %f",&ivp[i+1].current);
    }
  fclose(fp);
  just_IE();
  just_QD();
  bell(400,1);
  return;
  }
void JZ_line()
  {
  int p_flag=0;
  if(getpixel(270,420)==8){
    file_warn(8);
    return;
    }
  p_flag=print_test();
  if(p_flag<0) return;
  print_2(12,60,630,470+25*sz_sys.prn,0,14);
  return;
  }
void IE_line()
  {
  int p_flag=1;
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  cls_all_scrn();
  p_flag=print_spara();
  cls_all_scrn();
  maxfind1();
  board(1,4,16,7,0,7);
  if(ivp[1].scan_v<ivp[2].scan_v) drascr(rate_flag,1,0);
  else drascr(rate_flag,0,0);
  if(p_flag>=0) print_2(12,60,630,470+25*sz_sys.prn,0,0);
  return;
  }
void QD_line()
  {
  int p_flag=1;
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  cls_all_scrn();
  p_flag=print_spara();
  cls_all_scrn();
  maxfind1();
  board(1,4,16,7,0,7);
  diffrance();
  maxfind();
  if(ivp[1].scan_v<ivp[2].scan_v){
    diffdrascr();
    diffcalpeak();
    }
  else{
    diffyjdrascr();
    diffyjcalpeak();
    }
  if(p_flag>=0) print_2(12,60,630,470+25*sz_sys.prn,12,12);
  return;
  }
void just_IE()
  {
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  maxfind1();
  board(1,4,16,7,0,7);
  if(ivp[1].scan_v<ivp[2].scan_v) drascr(rate_flag,1,0);
  else drascr(rate_flag,0,0);
  return;
  }
void just_QD()
  {
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  maxfind1();
  board(1,4,16,7,0,7);
  diffrance();
  maxfind();
  if(ivp[1].scan_v<ivp[2].scan_v){
    diffdrascr();
    diffcalpeak();
    }
  else{
    diffyjdrascr();
    diffyjcalpeak();
    }
  return;
  }
void just_scrn()
  {
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  cls_all_scrn();
  maxfind1();
  board(1,4,16,7,0,7);
  if(ivp[1].scan_v<ivp[2].scan_v) drascr(rate_flag,1,0);
  else drascr(rate_flag,0,0);
  diffrance();
  maxfind();
  if(ivp[1].scan_v<ivp[2].scan_v){
    diffdrascr();
    diffcalpeak();
    }
  else{
    diffyjdrascr();
    diffyjcalpeak();
    }
  return;
  }
void read_site(int mouse_x,int mouse_y)
 {
 int button;
 int tx,ty;
 BOY_MOUSE_GET(&mouse_x,&mouse_y);
 just_site(mouse_x,mouse_y,spara.first_v,spara.end_v);
 return;
 }
void scrc()
  {
  if(spara.first_v==spara.end_v){
    file_warn(5);
    return;
    }
  mc_flag=0;
  if(spara.first_v<spara.end_v) mcrcfa();
  else yjrcfa();
  return;
  }
void scmc()
  {
  if(spara.first_v==spara.end_v){
    file_warn(5);
    return;
    }
  mc_flag=1;
  if(spara.first_v<spara.end_v) mcrcfa();
  else yjrcfa();
  return;
  }
void smfa()
  {
  if((spara.first_v==spara.end_v)||(spara.t_width<1)){
    file_warn(5);
    return;
    }
  if(spara.first_v<spara.end_v) smjp();
  else yjsmfa();
  return;
  }/*
int brush()
  {
  int i,sel_R;
  float inv[3],brush;
  int temp=0,step_volt=0;
  char buf[8];
  int ext,number,key;
  char *spara_c_hz[5]={
	    "准备好按任意键",
	    "快速清洗．．．",
	    "清洗完毕！",
	    "溶液静止，请稍候",
	    "正在扫描，请稍候",
	    };
  cls_all_scrn();
  if(spara.brush_v<=-1200){
    button("参数设置错误！！！",0,0,1,0);
    da_v(spara.brush_v);
    outportb(0x220,0x00);
    return(283);
    }
  board(1,4,20,6,0,7);
  disp_shadow_str(18,84,spara_c_hz[1],1);
  outportb(0x220,0x18);
  number=1;
  sel_R=1;
  selinsist(sel_R);
  for(i=0;i<3;i++){inv[i]=0.;}
  step_volt=-1200;
  ext=7+(1200+spara.brush_v)/64;
  board(3,7,6,7+(1200+spara.brush_v)/64,0,7);
  if(spara.brush_v>100) spara.brush_v=100;
 do{
  delay(10);
  brush=ad_v();
  insist_set[sel_R];
  brush=brush/insist_set[sel_R];;
  setfillstyle(1,10);
  if(136+floor(number/2)<=ext*18){
     bar(30,133+floor(number/2),42,133+floor(number/2));
     }
  number++;
  step_volt+=2;
  temp=floor(100*fabs(1200+step_volt)/fabs(1200+spara.brush_v));
  itoa_text(94,134,temp,0,8);
  outtextxy(118,134,"%");
  da_v(step_volt);
  if(kbhit()){
    key=bioskey(0);
    if(key==283) break;
    else button("按ＥＳＣ键返回！",0,0,1,0);
    }
}while(step_volt<spara.brush_v);
bell(400,1);
board(1,4,20,6,0,7);
disp_shadow_str(18,84,spara_c_hz[2],1);
da_v(spara.brush_v);
outportb(0x220,0x00);
return(key);
}    */
void scrn_save(int name)
  {
  char *file_name[100]={
  "z000","z001","z002","z003","z004","z005","z006","z007","z008","z009",
  "z010","z011","z012","z013","z014","z015","z016","z017","z018","z019",
  "z020","z021","z022","z023","z024","z025","z026","z027","z028","z029",
  "z030","z031","z032","z033","z034","z035","z036","z037","z038","z039",
  "z040","z041","z042","z043","z044","z045","z046","z047","z048","z049",
  "z050","z051","z052","z053","z054","z055","z056","z057","z058","z059",
  "z060","z061","z062","z063","z064","z065","z066","z067","z068","z069",
  "z070","z071","z072","z073","z074","z075","z076","z077","z078","z079",
  "z080","z081","z082","z083","z084","z085","z086","z087","z088","z089",
  "z090","z091","z092","z093","z094","z095","z096","z097","z098","z099",
  };
  char *pf;
  int i=0;
  FILE *fp;
  int number,handle;
  if(count<5){
    button("没有扫描数据！",0,0,1,0);
    return;
    }
  getdate(&d);
  gettime(&t);
  _fmode=O_BINARY;
  handle=creat(file_name[name],S_IWRITE|S_IREAD);
  fp=fdopen(handle,"wb");
  if(fp==NULL){
    file_warn(3);
    return;
    }
  fprintf(fp,"%d\n",2);
  fprintf(fp,"%d %d %d %d %d %d %d\n",0,d.da_year,d.da_mon,d.da_day,t.ti_hour,t.ti_min,t.ti_sec);
  fprintf(fp,"%d %d %d %d %d %d %d %d %d %d %d %d %d\n",spara.fuji_v,spara.fuji_t,spara.first_v,spara.end_v,spara.pulse,
    spara.p_width,spara.t_width,spara.slope,spara.brush_v,spara.brush_t,spara.stat_t,spara.sel_R,spara.sel_bl);
  fprintf(fp,"%d\n",count);
  for(i=0;i<=count-1;i++)
    {
    fprintf(fp," %d",ivp[i+1].scan_v);
    fprintf(fp," %9.2f",ivp[i+1].current);
    }
  fclose(fp);
  bell(400,1);
  return;
  }
void cont_sm()
 {
 int k=0,j=0,r=0;
 int cs=0,zc=1,key=0;
 char *in_str1=" 0";
 char *in_str2="10";
 if(M_Pointer==1) key=auto_sz(in_str1,in_str2,S_Pointer);
 else{ file_warn(7); key=27;}
 if(key==27){
   bell(400,0);
   return;
   }
 cs=atoi(in_str1);
 zc=atoi(in_str2);
 for(r=0;r<zc;r++){
 /*  if(cs<0){
     key=brush();
     if(key==283) break;
     } */
   if(M_Pointer==1){
     switch(S_Pointer){
       case 0:
	 scrc();
	 break;
       case 1:
	 scmc();
	 break;
       case 2:
	 smfa();
	 break;
       case 3:
	 xhfa();
	 break;
       }
     }
   if(kbhit()) key=bioskey(0);
   if(key==283) break;
   else scrn_save(k);
   for(j=0;j<fabs(60*cs);j++){
     if(kbhit()) key=bioskey(0);
     delay(1);
     if(key==283){
       bell(400,0);
       return;
      }
     }
   k++;
   }
 return;
 }
