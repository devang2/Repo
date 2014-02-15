#include <alloc.h>
#include <math.h>
#include <dos.h>
#include <graphics.h>
#include <stdio.h>
#include <conio.h>
#include <bios.h>
#include <stdlib.h>
#include "mouse.h"
#include "menu_d.h"
#include "new2.h"
int start_x=0,start_y=0;
int MOUSE_TYPE_TAB=0;
char _MOUSE;
typedef struct{
  char *type[32];
  int pix;
  int piy;
  }MOUSE_TYPE;
MOUSE_TYPE mouse_type[2]={
   {
   "1111111111111111",
   "1111111111111111",
   "1011111111111111",
   "1001111111111111",
   "1000111111111111",
   "1000011111111111",
   "1000001111111111",
   "1000000111111111",
   "1000000011111111",
   "1000001111111111",
   "1001001111111111",
   "1011101111111111",
   "1111110111111111",
   "1111110111111111",
   "1111111011111111",
   "1111111111111111",

   "1000000000000000",
   "1100000000000000",
   "1110000000000000",
   "1111000000000000",
   "1111100000000000",
   "1111110000000000",
   "1111111000000000",
   "1111111100000000",
   "1111111110000000",
   "1111111111000000",
   "1111111000000000",
   "1110111000000000",
   "1100011100000000",
   "0000011100000000",
   "0000001110000000",
   "0000001110000000",1,1},
   {
   "1111111111111111",
   "1111111111111111",
   "1110011111111111",
   "1111001111111111",
   "1111100111111111",
   "1111100001111111",
   "1110100000011111",
   "1100100000000111",
   "1000100000000111",
   "1001100000000111",
   "1000000000000111",
   "1100000000001111",
   "1110000000011111",
   "1111000000111111",
   "1111000000111111",
   "1111000000111111",

   "0000000000000000",
   "0000000000000000",
   "0001100000000000",
   "0000110000000000",
   "0000011000000000",
   "0000011110000000",
   "0001011111100000",
   "0011011111111000",
   "0111011111111000",
   "0110011111111000",
   "0111111111111000",
   "0011111111110000",
   "0001111111100000",
   "0000111111000000",
   "0000111111000000",
   "0000111111000000",4,3}
   };
int read_mouse();
int BOY_SET_MOUSE(void)/*init mouse*/
  {
  int apd;
  asm{
     mov ax,0
     int 0x33
     mov apd,ax
     }
  if(apd!=-1){
    _MOUSE=0;
    return 0;
    }
  _MOUSE=1;
  return 1;
  }
void BOY_MOUSE_VIEW(void)/*show mouse*/
  {
  asm{
    mov ax,1
    int 0x33
    }
  }
void BOY_MOUSE_HIDE(void)/*hide mouse*/
  {
  asm{
    mov ax,2
    int 0x33
    }
  }
void BOY_MOUSE_MOVE(int x,int y)/*move mouse*/
  {
  asm{
    mov ax,4
    mov cx,x
    mov dx,y
    int 0x33
    }
  }
void BOY_MOUSE_GET(int *x,int *y)/*get x,y(640,480)*/
  {
  int c,d;
  asm{
    mov ax,3
    int 0x33
    mov c,cx
    mov d,dx
    }
  *x=c;
  *y=d;
  }
void BOY_MOUSE_DGET(int *x,int *y)
  {
  int c,d;
  asm{
    mov ax,0x000b
    int 0x33
    mov c,cx
    mov d,dx
    }
  *x=c;
  *y=d;
  }
void BOY_MOUSE_GETALL(int *x,int *y,int *button)
  {
  int b,c,d;
  asm{
    mov ax,3
    int 0x33
    mov b,bx
    mov c,cx
    mov d,dx
    }
  *x=c;
  *y=d;
  *button=b;
  }
void BOY_MOUSE_TYPE(int s)
  {
  union REGS mr;
  struct SREGS ms;
  char *last;
  int BOY_SET_MOUSE_TYPE[32];
  int BOY_SET_MOUSE_i;
  for(BOY_SET_MOUSE_i=0;BOY_SET_MOUSE_i<32;BOY_SET_MOUSE_i++)
    BOY_SET_MOUSE_TYPE[BOY_SET_MOUSE_i]=strtol(mouse_type[s].type[BOY_SET_MOUSE_i],&last,2);
  mr.x.ax=9;
  mr.x.bx=mouse_type[s].pix;
  mr.x.cx=mouse_type[s].piy;
  mr.x.dx=FP_OFF(BOY_SET_MOUSE_TYPE);
  ms.es=FP_SEG(BOY_SET_MOUSE_TYPE);
  int86x(0X33,&mr,&mr,&ms);
  MOUSE_TYPE_TAB=s;
  }
int BOY_MOUSE_INBOX(int x1,int y1,int x2,int y2)
  {
  int c,d;
  asm{
    mov ax,3
    int 0x33
    mov c,cx
    mov d,dx
    }
  if((c>=x1)&&(c<=x2)&&(d>=y1)&&(d<=y2)) return(1);
  else return(0);
  }
int BOY_MOUSE_BUTTON(void)
  {
  int b;
  asm{
    mov ax,3
    int 0x33
    mov b,bx
    }
  return(b);
  }
int getinput()
 {
 int c;
 if(kbhit()){
   BOY_MOUSE_HIDE();
   c=bioskey(0);
   bios_test(c);
   BOY_MOUSE_VIEW();
   return(0);
 }
 else{
   read_mouse();
  }
 return(0);
 }
int waitforinput()
 {
 int c;
 while((c=getinput())==0);
 return(c);
 }
int mouse_menu(int count)
 {
 int i,len[7][2];
 for(i=0;i<count;i++){
   len[i][0]=4+i*88;
   len[i][1]=len[i][0]+78;
   }
 for(i=0;i<count;i++){
   if(BOY_MOUSE_INBOX(len[i][0],24,len[i][1],55)==1){
     if(BOY_MOUSE_BUTTON()==1)  return(i);
     else{ if(BOY_MOUSE_BUTTON()==2) return(quit());}
     }
   else{ if(BOY_MOUSE_BUTTON()==2) return(quit()); }
   }
 if(BOY_MOUSE_INBOX(622,6,633,20)==1){
  if(BOY_MOUSE_BUTTON()==1)
  return(8);
  }
 return(-1);
 }
int mouse_sub(int choice,int count)
 {
 int i,len[6][2];
 if(choice<0) return(-1);
 for(i=0;i<count;i++){
   len[i][0]=12+choice*88-4*choice;
   len[i][1]=i*26+69;
   }
 for(i=0;i<count;i++){
    if(BOY_MOUSE_INBOX(len[i][0],len[i][1],len[i][0]+106,len[i][1]+26)==1){
      if(BOY_MOUSE_BUTTON()==1)  return(i);
      else{ if(BOY_MOUSE_BUTTON()==2) quit();}
      }
   }
 if(BOY_MOUSE_INBOX(622,6,633,20)==1){
   if(BOY_MOUSE_BUTTON()==1)  return(8);
   }
 return(-1);
 }
void itoa_test(int x,int y,int data,int color,int bkcolor)
  {
  char buf[8];
  itoa(data,buf,10);
  setfillstyle(1,bkcolor);
  bar(x,y-2,x+34,y+8);
  moveto(x,y);
  setcolor(color);
  outtext(buf);
  return;
  }
void just_site(int mouse_x,int mouse_y,int first_v,int end_v)
  {
  int tx,ty;
  int color=0,col1,col2,col3;
  if((start_x!=mouse_x)||(start_y!=479-mouse_y)){
    start_x=mouse_x;
    start_y=479-mouse_y;
    col1=getpixel(206,420);
    col2=getpixel(208,460);
    col3=getpixel(208,456);
    if(col1!=8) color=1;
    else{
      if(col2!=8) color=2;
      if(col3!=8) color=3;
      }
    switch(color){
      case 1:
	tx=floor((mouse_x-273)/3);
	ty=floor((420-mouse_y)/2.97)+1;
	itoa_test(542,8,tx,15,9);
	outtextxy(576,8,":");
	itoa_test(584,8,ty,15,9);
	break;
      case 2:
	if(first_v>end_v)  tx=254-mouse_x;
	else tx=mouse_x-562;
	tx=floor(tx*20/44);
	ty=floor((440-mouse_y)*1.5385);
	itoa_test(542,8,tx,15,9);
	itoa_test(542,8,tx,15,9);
	outtextxy(576,8,":");
	itoa_test(584,8,ty,15,9);
	break;
      case 3:
	if(first_v>end_v)  tx=254-mouse_x;
	else tx=mouse_x-562;
	tx=floor(tx*5/11);
	ty=floor((276-mouse_y)*1.5244);
	itoa_test(542,8,tx,15,9);
	itoa_test(542,8,tx,15,9);
	outtextxy(576,8,":");
	itoa_test(584,8,ty,15,9);
	break;
      default:
	tx=start_x;
	ty=start_y;
	itoa_test(542,8,tx,15,9);
	outtextxy(576,8,":");
	itoa_test(584,8,ty,15,9);
	break;
      }
   }
 return;
 }
void drw_line(int mouse_x,int mouse_y)
  {
  void far *buffer[2];
  unsigned size_x,size_y;
  int m_x=0,m_y=0;
  if(BOY_MOUSE_BUTTON()!=1) return;
  else{
    BOY_MOUSE_HIDE();
    size_x=imagesize(211,68,610,72);
    size_y=imagesize(208,70,212,450);
    buffer[0]=farmalloc(size_x);
    buffer[1]=farmalloc(size_y);
    }
  BOY_MOUSE_GET(&mouse_x,&mouse_y);
  getimage(mouse_x-2,70,mouse_x+2,450,buffer[1]);
  getimage(211,mouse_y-2,610,mouse_y+2,buffer[0]);
  setcolor(15);
  line(mouse_x,70,mouse_x,450);
  line(211,mouse_y,610,mouse_y);
  for(;;){
    if(BOY_MOUSE_BUTTON()!=1) break;
    BOY_MOUSE_GET(&m_x,&m_y);
    if((mouse_x<=610)&&(mouse_x>=211)&&(mouse_y<=450)&&(mouse_y>=70)){
      if((mouse_x!=m_x)||(mouse_y!=m_y)){
	putimage(mouse_x-2,70,buffer[1],COPY_PUT);
	putimage(211,mouse_y-2,buffer[0],COPY_PUT);
	getimage(m_x-2,70,m_x+2,450,buffer[1]);
	getimage(211,m_y-2,610,m_y+2,buffer[0]);
	setcolor(15);
	line(m_x,70,m_x,450);
	line(211,m_y,610,m_y);
	mouse_x=m_x;
	mouse_y=m_y;
	}
      if((mouse_x!=start_x)||(mouse_y!=479-start_y)) read_site(mouse_x,mouse_y);
      }
    else break;
    }
  putimage(mouse_x-2,70,buffer[1],COPY_PUT);
  putimage(211,mouse_y-2,buffer[0],COPY_PUT);
  farfree(buffer[0]);
  farfree(buffer[1]);
  BOY_MOUSE_VIEW();
  return;
  }
int read_mouse()
 {
 int oldx,oldy;
 int choice=0,select;
 int flag;
 int sub_num[7]={4,4,3,3,2,4,2};
 int sub_number=0;
 int button;
 int mouse_x=0,mouse_y=0;
 flag=flag_test();
 choice=mouse_menu(7);
 if(choice>=0){
   BOY_MOUSE_HIDE();
   mouse_test(choice);
   BOY_MOUSE_VIEW();
   }
  else{
    sub_number=sub_num[flag];
    select=mouse_sub(flag,sub_number);
    if((select>=0)&&(flag>=0)){
      BOY_MOUSE_HIDE();
      mouse_sub_test(select);
      BOY_MOUSE_VIEW();
      }
    else{
      read_site(mouse_x,mouse_y);
      if(BOY_MOUSE_INBOX(210,70,610,450)==1)
	if((getpixel(270,420)!=8)||(getpixel(210,440)!=8))
	    drw_line(start_x,479-start_y);
     }
   }
 return(0);
 }