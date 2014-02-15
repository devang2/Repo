#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <bios.h>
#include <string.h>
#include <graphics.h>
#include <alloc.h>
#include <time.h>
#include "menu_d.h"
#include "new2.h"
#include "mouse.h"
struct dotcolor{
  unsigned dot1:4;
  unsigned dot2:4;
  };
typedef union{
  struct dotcolor color;
  char dot;
  }BYTE;
int any_key()
  {
  int key=0;
  key=bioskey(0);
  return(key);
  }
void exit_con(void)
  {
  closegraph();
  exit(0);
  }
void turn_up(int x,int y,int radius)
  {
  setlinestyle(0,1,3);
  setcolor(15);
  line(x-radius,y,x+radius,y);
  delay(10);
  setcolor(0);
  line(x-radius,y,x+radius,y);
  setcolor(15);
  line(x-radius,y-floor(radius/3),x+radius,y+floor(radius/3));
  delay(10);
  setcolor(0);
  line(x-radius,y-floor(radius/3),x+radius,y+floor(radius/3));
  setcolor(15);
  line(x,y-floor(radius/3.5),x,y+floor(radius/3.5));
  delay(10);
  setcolor(0);
  line(x,y-floor(radius/3.5),x,y+floor(radius/3.5));
  setcolor(15);
  line(x-radius,y+floor(radius/3),x+radius,y-floor(radius/3));
  delay(10);
  setcolor(0);
  line(x-radius,y+floor(radius/3),x+radius,y-floor(radius/3));
  setlinestyle(0,1,1);
  return;
  }
void gas_up(int x,int y,int radius,int xra,int type)
  {
  unsigned size;
  void far *buf;
  int forward=0;
  int i=1;
  int x1,y1;
  x1=x;
  y1=y;
  if(type==0) return;
  if(type==2) turn_up(x1,y1+xra,xra-8);
  else{
    for(i=1;i<=radius;i++){
      size=imagesize(x-i,y-i,x+i,y+i);
      buf=farmalloc(size);
      getimage(x-i,y-i,x+i,y+i,buf);
      setcolor(15);
      circle(x,y,i);
      if(type==3) turn_up(x1,y1+xra,xra-8);
      else delay(20);
      putimage(x-i,y-i,buf,COPY_PUT);
      farfree(buf);
      y-=i;
      forward++;
      if(forward>2) forward=0;
      if(forward==0) x+=i;
      else{
	if(forward==1) x-=i;
       }
     if(kbhit()) break;
     }
    }
  return;
  }
void work(int x,int y,int xra,int type)
  {
  setcolor(15);
  ellipse(x,y,0,360,xra,floor(xra/3));
  ellipse(x,y+xra,0,360,xra,floor(xra/3));
  line(x-xra,y,x-xra,y+xra);
  line(x+xra,y,x+xra,y+xra);
  y=y+xra;
  xra=floor(xra/2);
  ellipse(x,y,0,360,xra,floor(xra/3));
  ellipse(x,y+xra,0,360,xra,floor(xra/3));
  line(x-xra,y,x-xra,y+xra);
  line(x+xra,y,x+xra,y+xra);
  while((!kbhit())&&(BOY_MOUSE_BUTTON()!=1)) gas_up(x,y,7,xra,type);
  }
void gztzj()
  {
  char *str[4]={"停止动作！","正在通气！","正在搅拌！","正在搅拌、通气！"};
  int key,i=0;
  int out_data[4]={0x00,0x10,0x08,0x18};
  outportb(0x220,0x00);
  saveimage("filename.img",2,60,212,221);
  win_board(2,60,210,220,20,6,0);
  disp_hz_str(60,65,"工作台检测",15);
  disp_hz_str(20,94,str[0],15);
  BOY_MOUSE_VIEW();
  work(100,150,30,0);
  outportb(0x220,0x00);
  for(;;){
    if(kbhit()) key=bioskey(0);
    else{
      if(BOY_MOUSE_BUTTON()==1){
	if(BOY_MOUSE_INBOX(193,66,204,79)==1) key=283;
	else if(BOY_MOUSE_INBOX(8,66,19,79)==1) key=0;
	else key=20480;
	while(BOY_MOUSE_BUTTON());
	}
      }
    switch(key){
      case 18432:
	   i--;
	   if(i<0) i=3;
	   setfillstyle(1,0);
	   bar(16,88,200,110);
	   disp_hz_str(20,94,str[i],15);
	   outportb(0x220,out_data[i]);
	   break;
      case 20480:
	   i++;
	   if(i>3) i=0;
	   setfillstyle(1,0);
	   bar(16,88,200,110);
	   disp_hz_str(20,94,str[i],15);
	   outportb(0x220,out_data[i]);
	   break;
      case 19200:
	   i--;
	   if(i<0) i=3;
	   setfillstyle(1,0);
	   bar(16,88,200,110);
	   disp_hz_str(20,94,str[i],15);
	   outportb(0x220,out_data[i]);
	   break;
      case 19712:
	   i++;
	   if(i>3) i=0;
	   setfillstyle(1,0);
	   bar(16,88,200,110);
	   disp_hz_str(20,94,str[i],15);
	   outportb(0x220,out_data[i]);
	   break;
      case 283:
	   outportb(0x220,0x00);
	   BOY_MOUSE_HIDE();
	   loadimage("filename.img",2,60,212,221);
	   return;
      default:
	   button("按→↓←↑改变自检对象，按ＥＳＣ键返回！  ",0,0,1,0);
	   break;
      }
     work(100,150,30,i);
     }
  }
char p_out(char byte)
  {
  union REGS inregs,outregs;
  inregs.h.ah=0;
  inregs.h.al=byte;
  inregs.x.dx=0;
  int86(0x17,&inregs,&inregs);
  return(outregs.h.ah);
  }
int print_test()
  {
  int status,abyte=0;
  status=biosprint(2,abyte,0);
  if(status&0x80) return(0);
  else
    {
    button("提示：打印机未准备好！按任一键返回···",0,0,1,0);
    return(-1);
    }
  }
void print_2(int x1,int y1,int x2,int y2,int color1,int color2)
  {
    int i1,l1,j1,k1,ip;
    int n1;
    int m2,n2;
    int status,abyte=0;
    int buffer[3]={0,0,0};
    int bit[8]={128,64,32,16,8,4,2,1};
    int i,pbyte1,pbyte2,pbyte3;
    char *ts_str="正在打印，请稍候．．．";
    n1=(y2-y1)%8;
    m2=((x2-x1+1)*2)/256;n2=((x2-x1+1)*2)%256;
    saveimage("filenam1.img",400,0,616,33);
    win_board(400,0,614,32,0,6,7);
    disp_hz_str(412,8,ts_str,0);
    p_out(27);
    p_out(64);
    p_out(27);
    p_out(43);
    p_out(48);
    for(i1=y1;i1<=y2;i1++)
      {
      p_out(27);
      p_out(42);
      p_out(39);
      p_out(n2);
      p_out(m2);
      for(j1=x1;j1<=x2;j1++)
	{
	pbyte1=pbyte2=pbyte3=0;
	for(ip=0;ip<=3;ip++){
	  if(j1<=188){
	   if(getpixel(j1,i1+ip)==color2)
	     pbyte1=pbyte1+bit[ip*2]+bit[ip*2+1];
	   if(getpixel(j1,i1+ip+4)==color2)
	     pbyte2=pbyte2+bit[ip*2]+bit[ip*2+1];
	   if(getpixel(j1,i1+ip+8)==color2)
	       pbyte3=pbyte3+bit[ip*2]+bit[ip*2+1];
	   }
	 else{
	   if(getpixel(j1,i1+ip)==color1)
	     pbyte1=pbyte1+bit[ip*2]+bit[ip*2+1];
	   if(getpixel(j1,i1+ip+4)==color1)
	     pbyte2=pbyte2+bit[ip*2]+bit[ip*2+1];
	   if(getpixel(j1,i1+ip+8)==color1)
	       pbyte3=pbyte3+bit[ip*2]+bit[ip*2+1];
	   }
	 }
      buffer[0]=pbyte1;
      buffer[1]=pbyte2;
      buffer[2]=pbyte3;
      p_out(buffer[0]);
      p_out(buffer[1]);
      p_out(buffer[2]);
      p_out(buffer[0]);
      p_out(buffer[1]);
      p_out(buffer[2]);
      }
    p_out(13);
    p_out(10);
    i1+=11;
    i=i1;
    }
 if(i1>y2)
   {
   p_out(27);
   p_out(42);
   p_out(39);
   p_out(n2);
   p_out(m2);
   for(j1=x1;j1<=x2;j1++){
     pbyte1=pbyte2=pbyte3=0;
     for(i1=(i-8);i1<=(i-8+n1);i1++)
     pbyte1=pbyte1+bit[i1-i-8];
     p_out(pbyte1);
     p_out(pbyte2);
     p_out(pbyte3);
   }
 }
 biosprint(1,abyte,0);
 p_out(27);
 p_out(64);
 loadimage("filenam1.img",400,0,616,33);
 return;
 }
void saveimage(char *fn,int left,int top,int right,int bottom)
  {
  int x,y;
  BYTE store;
  FILE *fp;
  fp=fopen(fn,"wb");
  for(y=top;y<bottom;y++){
    for(x=left;x<right;){
      store.color.dot1=getpixel(x++,y);
      store.color.dot2=getpixel(x++,y);
      fwrite(&store.dot,sizeof(BYTE),1,fp);
      }
    }
  fclose(fp);
  return;
  }
int loadimage(char *fn,int left,int top,int right,int bottom)
  {
  int x,y;
  char ch;
  BYTE store;
  FILE *fp;
  fp=fopen(fn,"rb");
  if(!fp) return 0;
  for(y=top;y<bottom;y++){
    for(x=left;x<right;){
      fread(&store.dot,sizeof(BYTE),1,fp);
      putpixel(x++,y,store.color.dot1);
      putpixel(x++,y,store.color.dot2);
      }
    }
  fclose(fp);
  return(1);
  }
void push_bar(int left,int top,int right,int bottom)
 {
 int i;
 setfillstyle(SOLID_FILL,15);
 setlinestyle(SOLID_LINE,1,1);
 for (i=3; i<6; i++)
   {
   setcolor(i);
   line(left-2+i,top-2+i,left-2+i,bottom+2-i);
   line(left-2+i,top-2+i,right+2-i,top-2+i);
   setcolor(15);
   line(right+2-i,bottom+2-i,left-2+i,bottom+2-i);
   line(right+2-i,bottom+2-i,right+2-i,top-2+i);
   }
 return;
 }
void pull_bar(int left,int top,int right,int bottom)
 {
 int i;
 setfillstyle(SOLID_FILL,15);
 setlinestyle(SOLID_LINE,1,1);
 for (i=3; i<6; i++)
   {
   setcolor(15);
   line(left-2+i,top-2+i,left-2+i,bottom+2-i);
   line(left-2+i,top-2+i,right+2-i,top-2+i);
   setcolor(i);
   line(right+2-i,bottom+2-i,left-2+i,bottom+2-i);
   line(right+2-i,bottom+2-i,right+2-i,top-2+i);
   }
 return;
 }
int Main_board(char *str[7])
  {
  int x,y,i,j;
  x=12;
  Initialize();
  closescr();
  y=30;
  win_board(0,0,639,479,54,8,8);
  disp_hz_str(232,4,"ＪＰＹ 微机极谱仪",15);
  for(j=0;j<=6;j++){
   disp_shadow_str(x-2,y,str[j],0);
   for (i=4; i<6; i++)
     {
     setcolor(15);
     line(x-i,y+1-i,x-i,y+17+i);
     line(x-i,y+1-i,x+62+i,y+1-i);
     setcolor(i);
     line(x-i,y+17+i,x+62+i,y+17+i);
     line(x+62+i,y+1-i,x+62+i,y+17+i);
     }
   x=x+88;
   }
   openscr();
   return 0;
  }
void win_board(int left,int top,int right,int bottom,int leavel,int bkcolor,int color)
  {
   int i,j;
   setfillstyle(SOLID_FILL,bkcolor);
   bar(left,top,right,bottom);
   if(bkcolor!=color){
      setcolor(1);
      rectangle(left,top,right,bottom);
      setcolor(15);
      line(left,top,left,bottom);
      line(left,top,right,top);
      }
   if(leavel>0){
     setfillstyle(SOLID_FILL,9);
     bar(left+5,top+4,right-4,top+21);
     setfillstyle(SOLID_FILL,15);
     bar(left+6,top+6,left+16,top+19);
     bar(right-17,top+6,right-6,top+19);
     for(i=0;i<2;i++){
     setcolor(4+i);
     rectangle(left+4+i,top+4+i,left+18-i,top+21-i);
     rectangle(right-19+i,top+4+i,right-4-i,top+21-i);
     rectangle(left+7+i,top+11+i,left+15-i,top+13-i);
     }
    setcolor(12);
    setlinestyle(0,0,3);
    line(right-15,top+8,right-8,top+17);
    line(right-15,top+17,right-8,top+8);
    setlinestyle(0,0,1);
    }
  for(i=4;i<6;i++)
    {
    setcolor(15);
    line(left+i,bottom-i,right-i,bottom-i);
    line(right-i,top+leavel+i,right-i,bottom-i);
    setcolor(i);
    line(left+i,top+leavel+i,left+i,bottom-i);
    line(left+i,top+leavel+i,right-i,top+leavel+i);
    }
  setfillstyle(SOLID_FILL,color);
  bar(left+6,top+leavel+6,right-6,bottom-i);
  return;
 }
int file_warn(int type)
  {
  int key=0;
  char *warn[10]={"文件不存在！","文件格式错误！","文件重写？[Y/N]",
    "文件名非法！","数据输入错误！","参数设置错误！","请按回车！",
    "请把光标移到“测试方法”！","请确定您要打印的校准曲线！",
    "窗口内没有需要打印的内容！"};
  if(type==2) key=button(warn[type],0,0,1,1);
  else key=button(warn[type],0,0,1,0);
  return(key);
  }
void cls_left_scrn()
  {
  setfillstyle(1,8);
  bar(7,62,188,472);
  }
void cls_right_scrn()
  {
  setfillstyle(1,8);
  bar(188,62,630,472);
  }
void cls_all_scrn()
  {
  setfillstyle(1,8);
  bar(7,62,630,472);
  }
int watch(int cx,int cy,int cx1,int cy1,int tt)
   {
   double result, x=1,time,rex,rey;
   int ii;
   win_board(cx+cx1,cy-cy1-5,cx+3*cx1-1,cy+cy1+5,0,6,7);
   win_board(cx-cx1+3,cy-cy1-5,cx+cx1-1,cy+cy1+5,0,6,7);
   setcolor(0);
   setfillstyle(1,0);
   outtextxy(cx+cx1-6,cy-2,"3");
   outtextxy(cx-4,cy+cy1,"6");
   outtextxy(cx-cx1+1,cy-2,"9");
   outtextxy(cx-4,cy-cy1-6,"12");
   for(ii=tt;ii>=1;ii--){
   x=x+6;
   time=(x/180)*PI+1.5*PI;
   rey=floor(sin(time)*cy1+cy);
   rex=floor(cos(time)*cy1+cx);
   setcolor(LIGHTBLUE);
   line(cx,cy,rex,rey);
   textcolor(BLINK);
   setcolor(LIGHTBLUE);
   setfillstyle(1,LIGHTBLUE);
   setcolor(0);
   itoa_text(cx+cx1+14,cy-cy1+13,ii,0,7);
   sleep(1);
   setcolor(7);
   line(cx,cy,rex,rey);
   if(kbhit()) return(any_key());
   }
  return(0);
  }
int button(unsigned char *p1,int x,int y,int color,int belong)
  {
  int length,locate_x=0,locate_y=0;
  int key=0,num=0;
  length=strlen(p1)+4;
  if(x!=0){
   locate_x=x;
   locate_y=y;
   }
  else{
   locate_x=640-length*9;
   locate_y=10;
   }
  saveimage("filenam1.img",locate_x-6,locate_y-10,locate_x-4+length*8,locate_y+22);
  win_board(locate_x-6,locate_y-10,locate_x-6+length*8,locate_y+20,0,6,7);
  disp_hz_str(locate_x,locate_y-4,p1,color);
  if(belong==0){
    bell(400,0);
    for(;;){
     if(!kbhit()){
	delay(1);
	num++;
	if(num>=400) break;
	}
      else{
	if(kbhit()) key=any_key();
	break;
	}
      }
    }
  else{
    bell(400,1);
    key=any_key();
    }
  loadimage("filenam1.img",locate_x-6,locate_y-10,locate_x-4+length*8,locate_y+22);
  return(key);
  }