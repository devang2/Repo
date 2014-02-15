#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <bios.h>
#include <string.h>
#include <graphics.h>
#include "mouse.h"
#include "menu_d.h"
#include "new2.h"
void dra_screen(int color,int type)
  {
  int temp_y;
  int i=0;
  char *buf[5]={"2","4","6","8","10"};
  temp_y=floor(328/5);
  max_num=dpoint(max,0)-1;
  screen1(color);
  if(type>0) dra_x();
  else diffdra_x();
  if(color<0) screen1(0);
  else screen1(color);
  for(i=1;i<=5;i++) outtextxy(210-8*strlen(buf[i-1]),440-temp_y*i,buf[i-1]);
  }
void drascreen(float x,float y,int color)
  {
  int m0,n0;
  m0=210+floor(fabs((-1600)-x)/4.5);
  n0=440-floor(y*328/500);
  if((n0>=70)&&(color!=-1)&&(n0<=460))  putpixel(m0,n0,0);
 }
void screen1(int color)
{
  register int i;
  int temp_x,temp_y;
  temp_y=floor(328/5);
  temp_x=floor(400/9);
  setcolor(15);
  line(211,441,607,441);
  line(211,441,211,116);
  for(i=1;i<=9;i++){
   if(i<=5) line(216,440-temp_y*i,211,440-temp_y*i);
   line(211+temp_x*i,437,211+temp_x*i,440);
  }
  setcolor(color);
  line(210,440,606,440);
  line(210,440,210,115);
  outtextxy(209,436,".");
   for(i=1;i<=9;i++){
   if(i<=5) line(215,440-temp_y*i,210,440-temp_y*i);
   line(210+temp_x*i,437,210+temp_x*i,440);
  }
 }
void dra_x()
  {
  int temp_x;
  temp_x=floor(400/9);
  moveto(194,445);outtext("-1.6");
  moveto(194+temp_x*1,445);outtext("-1.4");
  moveto(194+temp_x*2,445);outtext("-1.2");
  moveto(194+temp_x*3,445);outtext("-1.0");
  moveto(194+temp_x*4,445);outtext("-0.8");
  moveto(194+temp_x*5,445);outtext("-0.6");
  moveto(194+temp_x*6,445);outtext("-0.4");
  moveto(194+temp_x*7,445);outtext("-0.2");
  moveto(194+temp_x*8,445);outtext(" 0  ");
  moveto(194+temp_x*9,445);outtext(" 0.2");
  //board(1,4,16,7,0,7);
  }
void diffdra_x()
  {
  int temp_x;
  temp_x=floor(400/9);
  moveto(194,445);outtext("+0.2");
  moveto(194+temp_x*1,445);outtext(" 0.0");
  moveto(194+temp_x*2,445);outtext("-0.2");
  moveto(194+temp_x*3,445);outtext("-0.4");
  moveto(194+temp_x*4,445);outtext("-0.6");
  moveto(194+temp_x*5,445);outtext("-0.8");
  moveto(194+temp_x*6,445);outtext("-1.0");
  moveto(194+temp_x*7,445);outtext("-1.2");
  moveto(194+temp_x*8,445);outtext("-1.4");
  moveto(194+temp_x*9,445);outtext("-1.6");
  }
void dra_y(int rate,int type,int color)
  {
  register int temp_y;
  int i=0;
  char *buf[9][5]={
      "0.1","0.2","0.3","0.4","0.5",
      "0.2","0.4","0.6","0.8","1",
      "0.4","0.8","1.2","1.6","2",
      "1","2","3","4","5",
      "2","4","6","8","10",
      "4","8","12","16","20",
      "10","20","30","40","50",
      "20","40","60","80","100",
      "100","200","300","400","500",
      };
  temp_y=floor(328/5);
  max_num=dpoint(max,0)-1;
  screen1(color);
  putpixel(208,460,color);//420
  if(type>0) dra_x();
  else diffdra_x();
  moveto(18,90);
  outtext("X*10");
  itoa_test(52,87,(int)max_num,0,7);
  for(i=0;i<5;i++){
    moveto(210-8*strlen(buf[rate][i]),440-temp_y*(i+1));
    outtext(buf[rate][i]);
    }
  }
void difdra_y(int type)
  {
  int i;
  char *buf[3][5]={
    "-10","-5","0","5","10",
    "-5","-2.5","0","2.5","5",
    "-2.5","-1.25","0","1.25","2.5",
    };
  for(i=0;i<5;i++)
    outtextxy(210-8*strlen(buf[type][i]),436-82*i,buf[type][i]);
  }
void drajpline(int x0,int y0,float x1,float y1,int rate,int type,int color)
  {
  int m0,n0;
  float rate_value[9]={0.5,1,2,5,10,20,50,100,500};
  max_num=dpoint(max,0)-1;
  if(type>0){
    m0=x0+floor(fabs((-1600)-x1)/4.5);
    n0=y0-floor((y1*328)/(rate_value[rate]*pow(10,max_num)));
    }
  else{
    m0=x0+floor(fabs((200)-x1)/4.5);
    n0=y0-floor((y1*328)/(rate_value[rate]*pow(10,max_num)));
    }
  if((n0>=70)&&(n0<=460))  putpixel(m0,n0,color);
  }
void screen2(int color)
  {
  register int i;
  int temp_x,temp_y;
  temp_y=floor(328/4);
  temp_x=floor(400/9);
  setcolor(color);
  line(210,440,210+398,440);
  line(210,440,210,112);
  setcolor(color);
  putpixel(208,456,color);
  for(i=1;i<=9;i++){
   line(210+temp_x*i,440-5,210+temp_x*i,440);
   };
  for(i=1;i<=4;i++){
   line(210+5,440-temp_y*i,210,440-temp_y*i);
   }
  }
void diffscreen(int x0,int y0,float x1,float y1,int color,float rate)
  {
  int m0,n0;
  m0=x0+floor(fabs((-1600)-x1)/4.5);
  n0=y0-floor((y1*164)/(rate*pow(10,max_num)));
  putpixel(m0,n0,color);
  max_num=dpoint(max,0)-1;
  }
void yjdrascreen(float x,float y,int color)
  {
  int m0,n0;
  m0=210+floor(fabs(200-x)/4.5);
  n0=440-floor(y*328/500);
  if((n0>=70)&&(color!=-1)&&(n0<=460)) putpixel(m0,n0,color);
  }
void difyjdrascreen(int x0,int y0,float x1,float y1,int color,float rate)
  {
  int m0,n0;
  m0=x0+floor(fabs((200)-x1)/4.5);
  n0=y0-floor((y1*164)/(rate*pow(10,max_num)));
  putpixel(m0,n0,color);
  max_num=dpoint(max,0)-1;
  }
int dpoint(double num,int n)
  {
  char p[33];
  int dec;
  double i;
  float i1;
  long int i2;
  modf(num,&i);
  i=floor(i);
  i1=i;
  i2=(long int)i1;
  ltoa(i2,p,10);
  dec=strlen(p)+n;
  return(dec);
  }
void itoa_text(int x,int y,int data,int color,int bkcolor)
  {
  char buf[8];
  itoa(data,buf,10);
  setfillstyle(1,bkcolor);
  bar(x,y-2,x+40,y+8);
  moveto(x+1,y+1);
  setcolor(15);
  outtext(buf);
  moveto(x,y);
  setcolor(color);
  outtext(buf);
  return;
  }