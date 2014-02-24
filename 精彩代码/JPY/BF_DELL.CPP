#include <dir.h>
#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <string.h>
#include <graphics.h>
#include <float.h>
#include <stat.h>
#include <io.h>
#include <fcntl.h>
#define Scrn_x 20
#define Scrn_y 110
#define IGNORE 0
#define RETRY 1
#define ABORT 2
#include "menu_d.h"
#include "new2.h"
#include "mouse.h"
int l_number=3;
int insert=FALSE;
float l_data[20]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
			 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
void cssz_error(int i);
struct in_struc {
  int x,y;
  char *prompt;
  char *ptr;
  int length;
  /*unsigned*/ int label_f;
  /*unsigned*/ int input_f;
  };
struct in_struc in_scrn1[21]={
 Scrn_x-4,Scrn_y+30+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+46+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+62+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+78+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+94+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+110+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+126+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+142+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+158+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+174+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+190+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+206+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+222+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+238+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+254+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+270+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+286+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+302+10,"请输入电流：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+318+10,"请输入浓度：",NULL,6,14,14,
 Scrn_x-4,Scrn_y+334+10,"请输入电流：",NULL,6,14,14,
 };
struct in_struc in_scrn2[2]={
 Scrn_x-4,Scrn_y-14,"请输入次数：",NULL,2,14,14,
 };
struct in_struc in_scrn3[2]={
 Scrn_x-4,392,"请输入电流：",NULL,6,14,14,
 };
char get_fld(char *string,int inlength,int start,int nx,int bx,int M_right,int M_left,int M_top);
void board(int left,int top,int right,int bottom,int leavel,int color);

static char *err_msg[]={
  "write protect",
  "unknown unit",
  "drive not ready",
  "unknown command",
  "data error(CRC)",
  "bad request",
  "seek error",
  "unknown media type",
  "sector not found",
  "printer out of paper",
  "write fault",
  "read fault",
  "general failure",
  "reserved",
  "reserved",
  "invalid disk change"
  };
void error_win(char *msg)
  {
  printf("%s",msg);
  }
int handler(int errval,int ax,int bp,int si)
  {
  static char msg[80];
  unsigned di;
  int drive;
  int errorno;
  di=_DI;
  if(ax<0){
    error_win("Device error");
    hardretn(ABORT);
    }
  drive=ax&0x00ff;
  errorno=di&0x00ff;
  hardretn(ABORT);
  sprintf(msg,"Error:%s on drive %c\r\n",err_msg[errorno],'A'+drive);
  error_win(msg);
  return ABORT;
  }
int getbit(unsigned char c,int n)
  {
  return((c>>n)&1);
  }
void outcc(int x,int y,int nm,int color)
  {
  unsigned int wei;
  unsigned int qu;
  int j,i1,i2,i3,rec;
  long addr;
  char font[32];
  FILE *fp;
  fp=fopen("hzk16","rb");
  if(fp==NULL)
   {
   closegraph();
   cputs("error on open hzk16");
   getch();
   exit(1);
   }
    qu=(nm&0xff)-0xa0;
    wei=((nm&0xff00)>>8)-0xa0;
    rec=(qu-1)*94+wei-1;
    addr=rec*32l;
    fseek(fp,addr,SEEK_SET);
    fread(font,sizeof(char),32,fp);
    for(i1=0;i1<16;i1++)
     for(i2=0;i2<2;i2++)
      for(i3=0;i3<8;i3++)
	if(getbit(font[i1*2+i2],7-i3))
	  putpixel(x+i2*8+i3,y+i1,color);
  fclose(fp);
  return;
  }
void outchxy(int x,int y,char ch,int color)
  {
  char b[2];
  setcolor(color);
  sprintf(b,"%c",ch);
  outtextxy(x,y,b);
  }
void disp_hz_str(int x,int y,char *s,int color)
  {
  while(*s)
  if((*s&0x80)&&(*(s+1)&0x80))
   {
   outcc(x,y,*(int*)s,color);
   s+=2;
   x=x+17;
   }
  else
   {
   outchxy(x,y+4,*s,color);s++;x=x+9;
   }
  }
void disp_shadow_str(int x,int y,char *s,int color)
  {
  int wd=1;
  while(*s)
  if((*s&0x80)&&(*(s+1)&0x80))
   {
   outcc(x+1,y+1,*(int*)s,15);
   outcc(x,y,*(int*)s,color);
   s+=2;
   x=x+wd+16;
   }
  else
   {
   outchxy(x+1,y+5,*s,15);
   outchxy(x,y+4,*s,color);
   s++;x=x+wd+8;
   }
  }
void Initialize(void)
{
   int gdriver = VGA, gmode=VGAHI, errorcode;
   struct palettetype pal;
   int i, ht, y,x,j;
   initgraph(&gdriver, &gmode, "");
   errorcode = graphresult();
   if (errorcode != grOk)
   {
      printf("Graphics error: %s\n", grapherrormsg(errorcode));
      printf("Press any key to halt:");
      getch();
      exit(1);
   }
   getpalette(&pal);
   for (i=1; i<pal.size-7; i++)
     setrgbpalette(pal.colors[i], i*4+16, i*4+16, i*4+16);
   setrgbpalette(pal.colors[10], 18, 882, 36);
 }
void copy(char *from,char *to,int first,int length)
  {
  int i;
  if(first<0) return;
  if((first+length+1)>strlen(from))
    length=strlen(from)+first+1;
  for(i=0;i<length;i++)
    to[i]=from[(first)+i];
    to[i]='\0';
 return;
  }
void chip_left(char *chip,char *block,int number)
  {
  char temp[32];
  strcpy(chip,"\0");
    if(number<1)
      return;
    if(number>strlen(block))
      number=strlen(block);
    strcpy(temp,block);
    copy(block,chip,0,number);
    copy(block,temp,number,(strlen(block)-number));
    strcpy(block,temp);
  }
void chip_right(char *chip,char *block,int number)
  {
  char temp[32];
  strcpy(chip,"\0");
  if(number<1||number>strlen(block)) return;
  copy(block,chip,strlen(block)-number,number);
  copy(block,temp,0,strlen(block)-number);
  strcpy(block,temp);
  return;
  }
void trim_left(char *string)
  {
  int i=0; int j=0;
  if(!strlen(string)) return;
  while(string[i]==' '&&string[i]!='\0') i++;
  while(string[i]!='\0') string[j++]=string[i++];
  string[j]='\0';
  }
void trim_right(char *string)
  {
  int pos;
  pos=strlen(string)-1;
  while(string[pos]==' ')
    {
    string[pos--]='\0';
    if(pos<0)
    break;
    }
  }
void get_key(char *ch,int *ext)
  {
  *ch=getch();
  if(!*ch) *ext=getch();
  }
void wherexy(int *x,int *y)
  {
  *x=getx(); *y=gety();
  return;
  }
/*
int val_field(char *string)
  {
  int i=0;
  while(string[i]!='\0')
    {
    if(string[i]!='e'&&string[i]!='E'&&string[i]!='-'&&string[i]!=
       '0'&&string[i]!='1'&&string[i]!='2'&&string[i]!='3'&&string[i]
       !='4'&&string[i]!='5'&&string[i]!='6'&&string[i]!=
       '7'&&string[i]!='8'&&string[i]!='9'&&string[i]!=
       '.'&&string[i]!=' '&&string[i]!='d'&&string[i]!='a'&&string[i]!=
       't'&&string[i]!='*')
       return(0);
    i++;
    }
  return(1);
  }*/
void hilight(int x,int y)
  {
  void *cursor;
  cursor=(void *)(malloc(imagesize(x,y+textheight("D"),x+textwidth("D"),
		  y+textheight("D")+1)));
  getimage(x,y+textheight("D"),x+textwidth("D"),y+textheight("D")+1,cursor);
  putimage(x,y+textheight("D"),cursor,NOT_PUT);
  free(cursor);
  cursor=NULL;
  return;
  }
int miti_fld(struct in_struc in_scrn[],int nx,int M_right,int M_left,int M_top)
  {
  int number=0;
  int i,j,x,y,savebkcolor;
  char temp_attr;
  int val_code;
  int ext;
  BOY_MOUSE_TYPE(1);
  for(;;)
    {
    if((in_scrn[number++].x==0)||(number>nx)) break;
    if(number==10000) return(1);
    }
  number--;
  for(i=0;i<number;i++)
    {
    x=in_scrn[i].x;
    y=in_scrn[i].y;
    disp_hz_str(x,y-4,in_scrn[i].prompt,in_scrn[i].label_f);
    x=x+strlen(in_scrn[i].prompt)*8+10;
    setcolor(in_scrn[i].input_f);
    outtextxy(x,y,in_scrn[i].ptr);
    }
  i=0;
  setfillstyle(1,9);
  bar(30,5,62,21);
  if(insert) disp_hz_str(30,5,"插入",15);
  else disp_hz_str(30,5,"覆盖",15);
  while(1)
    {
    setcolor(in_scrn[i].input_f);
    x=in_scrn[i].x;
    y=in_scrn[i].y;
    x=x+strlen(in_scrn[i].prompt)*8+10;
      moveto(x,y);
      ext=get_fld(in_scrn[i].ptr,in_scrn[i].length,0,nx,i,M_right,M_left,M_top);
      if(ext<=0){
	i=ext*(-1);
	}
      if(ext==ESCAPE) break;
      if((ext==DOWN)||(ext==RETURN)) i++;
      if(ext==UP) i--;
      else if(ext==ESCAPE||ext==PGUP||ext==PGDN) break;
      if(i==number&&ext==RETURN)
	{
	ext=RETURN;
	break;
	}
      if(i<0) i=number-1;
      if(i>(number-1)) i=0;
    }
  setfillstyle(1,9);
  bar(30,5,62,21);
  BOY_MOUSE_TYPE(0);
  return ext;
  }
void cssz_error(int i)
  {
  char *error_str[13]={
       "富集电位：－１６００～２００ｍｖ",
       "富集时间：０～９９９９ｓ",
       "起始电位：－１６００～２００ｍｖ",
       "终止电位：－１６００～２００ｍｖ",
       "脉冲增量：１～１００ｍｖ",
       "脉冲宽度：１～１００ｍｓ",
       "间隔时间：１～３２ｓ",
       "斜率增量：１～１６ｍｖ",
       "清洗电压：－１６００～２００ｍｖ",
       "清洗时间：０～９９９９ｓ",
       "静止时间：０～９９９９ｓ",
       "扫描档位：１～１２",
       "档位倍率：１～５",
       };
  button(error_str[i],0,0,1,0);
  return;
  }
int miti_fld1(struct in_struc in_scrn[],int nx,int M_right,int M_left,int M_top)
  {
  int number=0;
  int i,j,x,y,savebkcolor;
  char temp_attr;
  int val_code;
  int ext;
  BOY_MOUSE_TYPE(1);
  for(;;)
    {
    if((in_scrn[number++].x==0)||(number>nx)) break;
    if(number==10000) return(1);
    }
  number--;
  for(i=0;i<number;i++)
    {
    x=in_scrn[i].x;
    y=in_scrn[i].y;
    val_code=atoi(in_scrn[i].ptr);
    if((val_code<in_scrn[i].label_f)||(val_code>in_scrn[i].input_f))
      disp_hz_str(x,y-4,in_scrn[i].prompt,12/*in_scrn[i].label_f*/);
    else disp_hz_str(x,y-4,in_scrn[i].prompt,15/*in_scrn[i].label_f*/);
    x=x+strlen(in_scrn[i].prompt)*8+10;
    setcolor(15/*in_scrn[i].input_f*/);
    outtextxy(x,y,in_scrn[i].ptr);
    }
  i=0;
  setfillstyle(1,9);
  bar(30,5,62,21);
  if(insert) disp_hz_str(30,5,"插入",15);
  else disp_hz_str(30,5,"覆盖",15);
  while(1)
    {
    setcolor(15/*in_scrn[i].input_f*/);
    x=in_scrn[i].x;
    y=in_scrn[i].y;
    x=x+strlen(in_scrn[i].prompt)*8+10;
      moveto(x,y);
      ext=get_fld(in_scrn[i].ptr,in_scrn[i].length,0,nx,i,M_right,M_left,M_top);
      val_code=atoi(in_scrn[i].ptr);
      if((val_code<in_scrn[i].label_f)||(val_code>in_scrn[i].input_f)){
	disp_hz_str(Scrn_x,y-4,in_scrn[i].prompt,12/*in_scrn[i].label_f*/);
	cssz_error(i);
	}
      else disp_hz_str(Scrn_x,y-4,in_scrn[i].prompt,15/*in_scrn[i].label_f*/);
      if(ext<=0){
	i=ext*(-1);
	}
      if(ext==ESCAPE) break;
      if((ext==DOWN)||(ext==RETURN)) i++;
      if(ext==UP) i--;
      else if(ext==ESCAPE||ext==PGUP||ext==PGDN) break;
      if(i==number&&ext==RETURN)
	{
	ext=RETURN;
	break;
	}
      if(i<0) i=number-1;
      if(i>(number-1)) i=0;
    }
  setfillstyle(1,9);
  bar(30,5,62,21);
  BOY_MOUSE_TYPE(0);
    return ext;
  }
char get_fld(char *string,int inlength,int start,int nx,int bx,int M_right,int M_left,int M_top)
  {
  char *bk_erase;
  int width;
  char left[32],right[32],chip[32];
  char ch;
  int ext2=1;
  int i,x,y,orig_x,orig_y;
  int letter=0,len_left=0,len_right=0;
  int p1=0,p2=0,p3=0;
  unsigned size;
  int return_code,ret_code;
  int judge=0;
  wherexy(&orig_x,&orig_y);
  x=orig_x; y=orig_y;
  hilight(x,y);
  chip[0]='\0';
  left[0]='\0';
  right[0]='\0';
  trim_right(string);
  trim_left(string);
  strcpy(right,string);
  i=strlen(right);
  if(start>i) chip_left(left,right,i);
  else chip_left(left,right,start);
  outtextxy(x,y,left);
  start:moveto(x,y);
  for(;;)
   {
   ext2=1;
   ch=0;
   letter=0;
   len_left=strlen(left);
   len_right=strlen(right);
   moveto(x,y);
   if(mouse_test_sign==1){
     BOY_MOUSE_VIEW();
     while(!kbhit()){
	 BOY_MOUSE_GETALL(&p1,&p2,&p3);
	 for(i=0;i<nx;i++){
	   if(p3!=1) break;
	   if((p2>=y)&&(p2<=y+8)) break;
	   if((p1-orig_x>=0)&&(p1-orig_x<=inlength*8)&&(p2+16*bx>y+16*i)&&(p2+16*bx<y+16*(i+1))){
	     while(BOY_MOUSE_BUTTON());
	     BOY_MOUSE_HIDE();
	     hilight(x,y);//dimlight(x,y);
	     if(len_right>0) trim_right(right);
	     if(len_left>0) trim_left(left);
	     len_left=strlen(left);
	     len_right=strlen(right);
	     ext2=(-1)*i;
	     break;
	     }
	   }
	 if(ext2<=0) break;
	 if((p3==1)&&(p1-orig_x>=0)&&(p1-orig_x<=inlength*8)&&(p2>=y)&&(p2<=y+8)){
	   judge=floor((p1-x)/8);
	   while(BOY_MOUSE_BUTTON());
	   if(judge<=0){
	     chip_right(chip,left,1-judge);
	     strcat(chip,right);
	     strcpy(right,chip);
	     }
	   else{
	     chip_left(chip,right,judge);
	     strcat(left,chip);
	     }
	   BOY_MOUSE_HIDE();
	   hilight(x,y);//dim
	   len_left+=judge;
	   len_right-=judge;
	   x=orig_x+textwidth("8")*len_left;
	   moveto(x,y);
	   break;
	   }
	 if(p3==2){
	   BOY_MOUSE_HIDE();
	   return_code=ESCAPE;
	   return(return_code);
	   }
	 if(p3==1){
	   if(BOY_MOUSE_INBOX(M_right-17,M_top+6,M_right-6,M_top+19)==1){
	     while(BOY_MOUSE_BUTTON()==1);
	     BOY_MOUSE_HIDE();
	     return_code=ESCAPE;
	     return(return_code);
	     }
	   if(BOY_MOUSE_INBOX(M_left+6,M_top+6,M_left+17,M_top+19)==1){
	     while(BOY_MOUSE_BUTTON()==1);
	     BOY_MOUSE_HIDE();
	     ext2=PGDN;
	     return_code=PGDN;
	     break;
	     }
	   if(BOY_MOUSE_INBOX(30,5,62,21)==1){
	     while(BOY_MOUSE_BUTTON());
	     BOY_MOUSE_HIDE();
	     setfillstyle(1,9);
	     bar(30,5,62,21);
	     if(!insert) disp_hz_str(30,5,"插入",15);
	     else disp_hz_str(30,5,"覆盖",15);
	     insert=!insert;
	     BOY_MOUSE_VIEW();
	     }
	   }
       }
    if(kbhit()){
      BOY_MOUSE_HIDE();
      get_key(&ch,&ext2);
      }
     }
   else get_key(&ch,&ext2);
   if(ch>=32) letter=TRUE;
   if(ext2==INSERT){
      setfillstyle(1,9);
      bar(30,5,62,21);
      if(!insert) disp_hz_str(30,5,"插入",15);
      else disp_hz_str(30,5,"覆盖",15);
      insert=!insert;
      }
   if(len_right>0) trim_right(right);
   if(len_left>0) trim_left(left);
   len_left=strlen(left);
   len_right=strlen(right);
   if(!letter)
     {
     if(ext2==HOME&&len_left>0)
       {
       strcat(left,right);
       strcpy(right,left);
       len_right=len_left+len_right;
       len_left=0;
       left[0]='\0';
       hilight(x,y);//dim
       x=orig_x;
       }
     if(ext2==END)
       {
       strcat(left,right);
       len_left=len_left+len_right;
       len_right=0;
       right[0]='\0';
       hilight(x,y);//dim
       x=orig_x+textwidth("8")*len_left;
       }
     if(ext2==LEFT&&len_left>0)
       {
       chip_right(chip,left,1);
       strcat(chip,right);
       strcpy(right,chip);
       hilight(x,y);//dim
       x=x-textwidth("8");
       len_left--;
       len_right++;
       }
     if(ext2==RIGHT&&len_right>0)
       {
       chip_left(chip,right,1);
       strcat(left,chip);
       hilight(x,y);//dim
       x=x+textwidth("8");
       len_right--;
       len_left++;
       }
     if(ext2==DELETE&&len_right>0)
       {
       chip_left(chip,right,1);
       len_right--;
       }
     }
   if(ch==BACKSPACE&&len_left>0)
     {
     chip_right(chip,left,1);
     hilight(x,y);//dim
     x=x-textwidth("8");
     len_left--;
     }
   if(letter)
     {
     hilight(x,y);//dim
     if(insert&&(len_left+len_right>=inlength))
       {
       putchar(BELL);  letter=0;
       }
     else
       {
       if(insert)
	 {
	 x=x+textwidth("8");
	 left[len_left++]=ch;
	 left[len_left]='\0';
	 }
       if(!insert)
	 {
	 if(len_left==inlength)
	   {
	   putchar(BELL);
	   letter=0;
	   }
	 else
	   {
	   chip_left(chip,right,1);
	   left[len_left++]=ch;
	   left[len_left]='\0';
	   len_right--;
	   x=x+textwidth("8");
	   }
	 }
       }
     }
   moveto(x,y);
   if(x<orig_x) x=orig_x;
   if(x>orig_x+textwidth("8")*inlength)  x=orig_x;
   if(len_left<0) len_left=0;
   if(len_right<0) len_right=0;
   size=imagesize(orig_x,orig_y,orig_x+textwidth("8")*(inlength+1),
		 orig_y+textheight("8")+2);
   bk_erase=(char *)(malloc(size));
   if(bk_erase==NULL)  return(0);
   getimage(orig_x,orig_y,orig_x+textwidth("8")*(inlength+1),
	  orig_y+textheight("8")+2,bk_erase);
   putimage(orig_x,orig_y,bk_erase,XOR_PUT);
   free(bk_erase);
   bk_erase=NULL;
   moveto(orig_x,orig_y);
   outtext(left);
   wherexy(&x,&y);
   hilight(x,y);
   outtext(right);
   if(ch==RETURN||ext2==UP||ext2==DOWN||ext2<=0)
     {
     hilight(x,y);//dim
     break;
     }
   if(ext2==PGUP||ext2==PGDN) break;
   if(ch==ESCAPE) break;
   }
   switch(ext2)
     {
     case UP: return_code=UP; break;
     case DOWN: return_code=DOWN; break;
     case PGUP: return_code=PGUP; break;
     case PGDN: return_code=PGDN; break;
     }
   if(ch==RETURN) return_code=RETURN;
   if(ch==ESCAPE) return_code=ESCAPE;
   if(ext2<=0) return_code=ext2;
   strcat(left,right);
   strcpy(string,left);
   if(ret_code==0)
     {
     putchar(BELL);
     goto start;
     }
   return(return_code);
 }
void WriteMode(int mode)
  {
  outportb(0x3CE,5);
  outportb(0x3CF,mode);
  }
void closescr()
  {
  unsigned char byt;
  outportb(0x3c4,1);
  byt=inportb(0x3c5);
  byt|=0x20;
  outportb(0x3c5,byt);
  return;
  }
void openscr()
  {
  unsigned char byt;
  outportb(0x3c4,1);
  byt=inportb(0x3c5);
  byt&=0xdf;
  outportb(0x3c5,byt);
  return;
  }
void board(int left,int top,int right,int bottom,int leavel,int color)
    {
    win_board(left*8,top*18,right*8,bottom*18,leavel,6,color);
    }
void cssz_ch(int cs[13])
 {
 int exxt,kk;
 struct in_struc in_scrn[14]={
   Scrn_x,Scrn_y,"富集电位ｍｖ：",NULL,5,-1600,200,
   Scrn_x,Scrn_y+16,"富集时间ｓ  ：",NULL,5,0,9999,
   Scrn_x,Scrn_y+32,"起始电位ｍｖ：",NULL,5,-1600,200,
   Scrn_x,Scrn_y+48,"终止电位ｍｖ：",NULL,5,-1600,200,
   Scrn_x,Scrn_y+64,"脉冲增量ｍｖ：",NULL,5,10,100,
   Scrn_x,Scrn_y+80,"脉冲宽度ｍｓ：",NULL,5,10,100,
   Scrn_x,Scrn_y+96,"间隔时间ｓ  ：",NULL,5,1,32,
   Scrn_x,Scrn_y+112,"斜率增量ｍｖ：",NULL,5,1,16,
   Scrn_x,Scrn_y+128,"清洗电压ｍｖ：",NULL,5,-1600,200,
   Scrn_x,Scrn_y+144,"清洗时间ｓ  ：",NULL,5,0,9999,
   Scrn_x,Scrn_y+160,"静止时间ｓ  ：",NULL,5,0,9999,
   Scrn_x,Scrn_y+176,"扫描档位    ：",NULL,5,1,12,
   Scrn_x,Scrn_y+192,"档位倍率    ：",NULL,5,1,5,
   };
 for(kk=0;kk<13;kk++){
   in_scrn[kk].ptr=(char *)malloc(sizeof("      "));
   itoa(cs[kk],in_scrn[kk].ptr,10);
   }
 saveimage("text.img",10,68,201,324);
 win_board(10,68,200,322,30,6,0);
 disp_hz_str(68,73,"参数设置",15);
 exxt=miti_fld1(in_scrn,13,200,10,68);
 loadimage("text.img",10,68,201,324);
 for(kk=0;kk<13;kk++){
    if(exxt!=27) cs[kk]=atoi(in_scrn[kk].ptr);
    if(in_scrn[kk].ptr!=NULL) free(in_scrn[kk].ptr);
    }
 if(exxt==27) bell(400,0);
 else bell(400,1);
 }
int auto_sz(char *in_str1,char *in_str2,int s_flag)
  {
  int exxt,i;
  char *sign_str[3]={"差示脉冲溶出","差示脉冲伏安","线性扫描伏安"};
  struct in_struc in_scrn4[3]={
   20,112,"间隔时间ｓ：",NULL,2,15,15,
   20,132,"测试次数  ：",NULL,2,15,15,
   };
  in_scrn4[0].ptr=(char *)malloc(sizeof("0"));
  in_scrn4[1].ptr=(char *)malloc(sizeof("0 "));
  strcpy(in_scrn4[0].ptr,in_str1);
  strcpy(in_scrn4[1].ptr,in_str2);
  saveimage("filename.img",8,80,182,160);
  win_board(8,80,180,158,20,6,0);
  disp_hz_str(44,85,sign_str[s_flag],15);
  exxt=miti_fld(in_scrn4,2,180,8,80);
  loadimage("filename.img",8,80,182,160);
  if(exxt!=27){
    strcpy(in_str1,in_scrn4[0].ptr);
    strcpy(in_str2,in_scrn4[1].ptr);
    }
  free(in_scrn4[0].ptr);
  free(in_scrn4[1].ptr);
  return exxt;
  }
int get_str(char *in_str[1],int flag,int type)
  {
  int exxt;
  char *type_str[4]={"保存曲线文件","打开曲线文件","保存数据文件","打开数据文件"};
  struct in_struc in_scrn[2]={
   12,100,"请输入文件名：",NULL,16,15,15,
   };
  in_scrn[0].ptr=(char *)malloc(sizeof("               "));
  if(flag==1)  in_str[0]="*.dat          ";
  strcpy(in_scrn[0].ptr,in_str[0]);
  saveimage("filename.img",2,60,278,122);
  win_board(2,60,276,120,20,6,0);
  disp_hz_str(84,65,type_str[type],15);
  exxt=miti_fld(in_scrn,1,276,2,60);
  loadimage("filename.img",2,60,278,122);
  if(exxt!=27) in_str[0]=strdup(in_scrn[0].ptr);
  free(in_scrn[0].ptr);
  return exxt;
  }
void spara_cun(void)
  {
  char *filename[1];
  char *pf;
  FILE *fp;
  int number,handle;
  int press_key=0;
  int i=0;
  int s_flag=0;
  for(i=0;i<l_number-1;i++){
    if(l_data[i]!=l_data[i+1]){
       s_flag=1;
       break;
       }
    else s_flag=0;
    }
  if(s_flag==0){
    button("没有校准数据！",0,0,1,0);
    return;
    }
  if(get_str(filename,1,2)==27) return;
  _fmode=O_BINARY;
  pf=searchpath(filename[0]);
  if(pf!=NULL){
       press_key=file_warn(2);
       if((press_key==5497)||(press_key==5465)){
	   fp=fopen(filename[0],"wb");
	   fprintf(fp,"%d\n",1);
	   fprintf(fp,"%d\n",l_number);
	   for(i=0;i<2*l_number;i++)  fprintf(fp,"%f\n",l_data[i]);
	   bell(400,1);
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
       fprintf(fp,"%d\n",1);
       fprintf(fp,"%d\n",l_number);
       for(i=0;i<2*l_number;i++) fprintf(fp,"%f\n",l_data[i]);
       fclose(fp);
    }
  bell(400,1);
  return;
  }
void spara_diao(void)
  {
  char *filename[1];
  FILE *fp;
  int exxt,number;
  int i=0;
  exxt=get_str(filename,1,3);
  if(exxt==ESCAPE) return;
  fp=fopen(filename[0],"r");
  if(fp==NULL){
    file_warn(0);
    return;
    }
  fscanf(fp,"%d\n",&number);
  fscanf(fp,"%d\n",&l_number);
  if(number!=1){
      file_warn(1);
      return;
      }
  else{
      for(i=0;i<2*l_number;i++)	fscanf(fp,"%f\n",&l_data[i]);
      }
  fclose(fp);
  bell(400,1);
  return;
  }
void point_line()
  {
  int x0,y0;
  int i;
  char *buf[10]={"1","2","3","4","5","6","7","8","9","10"};
  x0=270;y0=420;
  setcolor(15);
  line(200,421,600,421);
  line(274,120,274,421);
  setcolor(0);
  line(200,420,600,420);
  line(273,120,273,421);
  for(i=-2;i<=10;i++){
   line(x0+30*i+3,421,x0+30*i+3,425);
   itoa_test(x0+30*i,427,i,0,8);
  };
  for(i=1;i<=10;i++){
   outtextxy(x0-2,y0-30*i,"-");
   outtextxy(x0-2-8*strlen(buf[i-1]),y0-30*i,buf[i-1]);
  };
  return;
  }
void sblt()
  {
  char *ch;
  int exxt,kk,temp;
  float flag1=0.0;
  float x1[10],y1[10];
  float x2,y2;
  float maxx,maxy;
  char buf[12];
  int maxx_num,maxy_num;
  int x,y,x0,y0,m0,n0;
  register int i,j;
  char *spara_c_hz="待测浓度";
  cls_all_scrn();
  x0=270;y0=420;
  maxx=0;maxy=0;maxx_num=0;maxy_num=0;
  for(i=0;i<=10;i++){x1[i]=0;y1[i]=0;}
  point_line();
  l_number=2;
  for(kk=0;kk<2*l_number;kk++) flag1+=fabs(l_data[kk]);
  for(kk=0;kk<4;kk++)
   {
   in_scrn1[kk].ptr=(char *)malloc(sizeof("      "));
   if(flag1==0){
     ch="      ";
     strcpy(in_scrn1[kk].ptr,ch);
     }
   else
     gcvt(l_data[kk],6,in_scrn1[kk].ptr);
   }
 win_board(8,118,184,in_scrn1[3].y+18,20,6,0);
 disp_hz_str(54,123,"双标类推法",15);
 exxt=miti_fld(in_scrn1,4,184,8,118);
  if(exxt==27)
  {
  cls_all_scrn();
  for(kk=0;kk<4;kk++)
    if(in_scrn1[kk].ptr!=NULL) free(in_scrn1[kk].ptr);
  setcolor(0);
  return;
  }
 for(temp=0;temp<2;temp++)
 {
  if((strlen(in_scrn1[2*temp].ptr)!=0)&&(strlen(in_scrn1[2*temp+1].ptr)!=0))
   {
     if((in_scrn1[2*temp].ptr!="      ")&&(in_scrn1[2*temp+1].ptr!="      "))
      {
      x1[temp]=atof(in_scrn1[2*temp].ptr);
      y1[temp]=atof(in_scrn1[2*temp+1].ptr);
      }
   }
 }
 for(kk=0;kk<4;kk++)
    if(in_scrn1[kk].ptr!=NULL){
      l_data[kk]=atof(in_scrn1[kk].ptr);
      free(in_scrn1[kk].ptr);
      }
 for(i=1;i<2;i++){
    flag1=y1[i]-y1[i-1];
    if(flag1!=0){ flag1=0; break;}
    flag1=1;
    }
  if(flag1!=0){
   file_warn(4);
   cls_all_scrn();
   return;
   }
  setcolor(0);
  maxx=x1[0];
  maxy=y1[0];
  for(i=0;i<2;i++)
   {
   if(x1[i]>maxx) maxx=x1[i];
   if((y1[i])>maxy) maxy=y1[i];
   }
  maxx_num=dpoint(maxx,0)-1;
  maxy_num=dpoint(maxy,0)-1;
  for(i=0;i<2;i++)
  {
   x=(int)floor(x0+x1[i]*300/(10*pow(10,maxx_num)));
   y=(int)floor(y0-(y1[i])*300/(10*pow(10,maxy_num)));
   if((y>=64)&&(y<=460)) outtextxy(x,y,"+");
   }
  outtextxy(594,436,"X ");
  maxx_num=dpoint(maxx,0)-1;
  gcvt(pow(10,maxx_num),maxx_num,buf);
  outtext(buf);
  outtextxy(230,104,"X ");
  maxy_num=dpoint(maxy,0)-1;
  gcvt(pow(10,maxy_num),maxy_num,buf);outtext(buf);
  x2=(int)floor(x0+x1[0]*300/(10*pow(10,maxx_num)));
  y2=(int)floor(y0-(y1[0])*300/(10*pow(10,maxy_num)));
  m0=(int)floor(x0+x1[1]*300/(10*pow(10,maxx_num)));
  n0=(int)floor(y0-(y1[1])*300/(10*pow(10,maxy_num)));
  setcolor(15);
  line(x2+3,y2+3,m0+3,n0+3);
  setcolor(0);
  line(x2+2,y2+2,m0+2,n0+2);
  in_scrn3[0].ptr=(char *)malloc(sizeof("      "));
  ch="      ";
  strcpy(in_scrn3[0].ptr,ch);
  board(1,20,23,24,20,0);
  disp_hz_str(60,365,"待测浓度",15);
  for(;;){
     exxt=miti_fld(in_scrn3,1,184,8,360);
     if(exxt==27) break;
     y2=atof(in_scrn3[0].ptr);
     x2=(x1[0]*(y1[1]-y2)-x1[1]*(y1[0]-y2))/(y1[1]-y1[0]);
     disp_hz_str(16,406,spara_c_hz,YELLOW);
     gcvt(x2,dpoint(x2,3),buf);
     setfillstyle(1,0);
     bar(98,404,178,424);
     disp_hz_str(100,410,buf,YELLOW);
     x2=(int)floor(x0+x2*300/(10*pow(10,maxx_num)));
     y2=(int)floor(y0-(y2)*300/(10*pow(10,maxy_num)));
     setcolor(0);
     if((y2>=64)&&(y2<=460)) outtextxy(x2,y2,"+");
     if((exxt==PGDN)||(exxt==PGUP)) break;
    }
  return;
 }
void bzjr()
  {
  char *ch;
  int exxt,kk,temp;
  float flag1=0.0;
  float b0,b1,r,x1[10],y1[10],_x,_y,xx,xy,_xy,yy;
  float c,con,x2,y2;
  float maxx,maxy;
  float Sxx,Syy,Sxy;
  char buf[12];
  int maxx_num,maxy_num;
  int x,y,x0,y0,m0,n0,n;
  register int i,j;
  char *spara_c_hz="待测浓度";
  cls_all_scrn();
  x0=270;y0=420;
  maxx=0;maxy=0;maxx_num=0;maxy_num=0;
  b0=0;b1=0;_x=0;_y=0;_xy=0;xx=0;xy=0;
  yy=0;Sxx=0;Syy=0;Sxy=0;r=0;
  for(i=0;i<10;i++){x1[i]=0;y1[i]=0;}
  point_line();
 win_board(8,62,184,116,20,6,0);
 disp_hz_str(54,67,"外推加入法",15);
 in_scrn2[0].ptr=(char *)malloc(sizeof("  "));
 itoa(l_number,ch,10);
 strcpy(in_scrn2[0].ptr,ch);
 exxt=miti_fld(in_scrn2,1,184,8,62);
 if(exxt==27){
  cls_all_scrn();
  return;
  }
 n=atoi(in_scrn2[0].ptr);
 free(in_scrn2[0].ptr);
 if(n<3){
   file_warn(4);
   cls_all_scrn();
   return;
   }
 if(n>10) n=10;
 l_number=n;
 for(kk=0;kk<2*n;kk++)
   {
   in_scrn1[kk].ptr=(char *)malloc(sizeof("      "));
   if(l_data[kk]==0){
     ch="      ";
     strcpy(in_scrn1[kk].ptr,ch);
     }
   else gcvt(l_data[kk],6,in_scrn1[kk].ptr);
   }
 win_board(8,118,184,in_scrn1[2*n-1].y+18,20,6,0);
 exxt=miti_fld(in_scrn1,2*n,184,8,118);
 if(exxt==27)
  {
  cls_all_scrn();
  for(kk=0;kk<2*n;kk++)
    if(in_scrn1[kk].ptr!=NULL) free(in_scrn1[kk].ptr);
  setcolor(0);
  return;
  }
 for(temp=0;temp<n;temp++)
   {
   if((strlen(in_scrn1[2*temp].ptr)!=0)&&(strlen(in_scrn1[2*temp+1].ptr)!=0))
     {
     if((in_scrn1[2*temp].ptr!="      ")&&(in_scrn1[2*temp+1].ptr!="      "))
      {
      x1[temp]=atof(in_scrn1[2*temp].ptr);
      y1[temp]=atof(in_scrn1[2*temp+1].ptr);
      }
    }
   }
 for(kk=0;kk<2*n;kk++)
    if(in_scrn1[kk].ptr!=NULL){
      l_data[kk]=atof(in_scrn1[kk].ptr);
      free(in_scrn1[kk].ptr);
      }
 for(i=1;i<temp;i++){
    flag1=x1[i]-x1[i-1];
    if(flag1!=0){ flag1=0; break;}
    flag1=y1[i]-y1[i-1];
    if(flag1!=0){ flag1=0; break;}
    flag1=1;
    }
  if(flag1!=0){
   file_warn(4);
   cls_all_scrn();
   return;
   }
  setcolor(0);
  maxx=x1[0];
  maxy=y1[0];
  for(i=0;i<n;i++)
  {
    if(x1[i]>maxx) maxx=x1[i];
    if((y1[i])>maxy) maxy=y1[i];
  }
  maxx_num=dpoint(maxx,0)-1;
  maxy_num=dpoint(maxy,0)-1;
  for(i=0;i<n;i++)
  {
   x=(int)floor(x0+x1[i]*300/(10*pow(10,maxx_num)));
   y=(int)floor(y0-(y1[i])*300/(10*pow(10,maxy_num)));
   if((y>=64)&&(y<=460))  outtextxy(x,y,"+");
   }
  outtextxy(594,436,"X");
  maxx_num=dpoint(maxx,0)-1;
  gcvt(pow(10,maxx_num),maxx_num,buf);outtext(buf);
  outtextxy(230,104,"X");
  maxy_num=dpoint(maxy,0)-1;
  gcvt(pow(10,maxy_num),maxy_num,buf);outtext(buf);
  for(i=0;i<n;i++){
  _x=_x+x1[i];_y=_y+y1[i];xy=xy+x1[i]*y1[i];
  xx=xx+x1[i]*x1[i];
  yy=yy+y1[i]*y1[i]; };
  Sxx=xx-_x*_x/n;
  Syy=yy-_y*_y/n;
  Sxy=xy-_x*_y/n;
  _x=_x/n;_y=_y/n;
  _xy=_x*_y;
  b1=(float)(xy-n*_xy)/(xx-n*_x*_x);
  b0=(float)(_y-_x*b1);
  m0=(int)floor(x0+x1[n-1]*300/(10*pow(10,maxx_num)));
  n0=420-floor((b0+b1*x1[n-1])*300/(10*pow(10,maxy_num)));
  x2=(int)floor(x0-(b0/b1)*300/(10*pow(10,maxx_num)));
  y2=(int)floor(y0);
  if(Sxx*Syy<=0)
    r=1;
  else
    r=(float)(Sxy/sqrt(Sxx*Syy));
  setcolor(15);
  line(x2+3,y2+3,m0+3,n0+3);
  setcolor(0);
  line(x2+2,y2+2,m0+2,n0+2);
  x2=-b0/b1;
  board(1,21,23,24,20,0);
  disp_hz_str(60,365+18,"待测浓度",15);
  disp_hz_str(20,406,spara_c_hz,14);
  gcvt(fabs(x2),dpoint(fabs(x2),3),buf);
  disp_hz_str(96,408,buf,14);
  board(61,14,72,16,0,7);
  setcolor(0);
  outtextxy(496,258,"B0=");
  gcvt(fabs(b0),dpoint(fabs(b0),3),buf);
  outtextxy(520,258,buf);
  outtextxy(496,266,"B1=");
  gcvt(fabs(b1),dpoint(fabs(b1),3),buf);
  outtextxy(520,266,buf);
  outtextxy(496,274,"R =");
  gcvt(fabs(r),dpoint(fabs(r),3),buf);
  outtextxy(520,274,buf);
  return;
 }
void gzqx()
 {
  char *ch;
  int exxt,kk,temp;
  float flag1=0.0;
  float b0,b1,r,x1[10],y1[10],_x,_y,xx,xy,_xy,yy;
  float x2,y2;
  float maxx,maxy;
  float Sxx=0,Sxy=0,Syy=0;
  char buf[12];
  int maxx_num,maxy_num;
  int x,y,x0,y0,m0,n0,n;
  register int i,j;
  char *spara_c_hz="待测浓度：";
  maxx=0;maxy=0;maxx_num=0;maxy_num=0;
  b0=0;b1=0;_x=0;_y=0;_xy=0;xx=0;xy=0;
  yy=0;r=0;
  x0=270;y0=420;
  cls_all_scrn();
  for(i=0;i<10;i++){ x1[i]=0;y1[i]=0;}
  point_line();
  in_scrn2[0].ptr=(char *)malloc(sizeof("  "));
  itoa(l_number,ch,10);
  strcpy(in_scrn2[0].ptr,ch);
  win_board(8,62,184,116,20,6,0);
  disp_hz_str(54,67,"校准曲线法",15);
  exxt=miti_fld(in_scrn2,1,184,8,62);
  n=atoi(in_scrn2[0].ptr);
  free(in_scrn2[0].ptr);
  if(exxt==27){
    cls_all_scrn();
    return;
    }
  if(n<3){
   file_warn(4);
   cls_all_scrn();
   return;
   }
  if(n>10) n=10;
  l_number=n;
  for(kk=0;kk<2*n;kk++)
   {
   in_scrn1[kk].ptr=(char *)malloc(sizeof("      "));
   if(l_data[kk]==0){
     ch="      ";
     strcpy(in_scrn1[kk].ptr,ch);
     }
   else gcvt(l_data[kk],6,in_scrn1[kk].ptr);
   }
  win_board(8,118,184,in_scrn1[2*n-1].y+18,20,6,0);
  exxt=miti_fld(in_scrn1,2*n,184,8,118);
 if(exxt==27)
  {
  cls_all_scrn();
  for(kk=0;kk<2*n;kk++)
    if(in_scrn1[kk].ptr!=NULL) free(in_scrn1[kk].ptr);
  setcolor(0);
  return;
  }
 for(temp=0;temp<n;temp++)
 {
  if((strlen(in_scrn1[2*temp].ptr)!=0)&&(strlen(in_scrn1[2*temp+1].ptr)!=0))
   {
     if((in_scrn1[2*temp].ptr!="      ")&&(in_scrn1[2*temp+1].ptr!="      "))
      {
      x1[temp]=atof(in_scrn1[2*temp].ptr);
      y1[temp]=atof(in_scrn1[2*temp+1].ptr);
      }
   }
 }
  for(kk=0;kk<2*n;kk++)
    if(in_scrn1[kk].ptr!=NULL){
      l_data[kk]=atof(in_scrn1[kk].ptr);
      free(in_scrn1[kk].ptr);
      }
  for(i=1;i<temp;i++){
    flag1=x1[i]-x1[i-1];
    if(flag1!=0){ flag1=0; break;}
    flag1=y1[i]-y1[i-1];
    if(flag1!=0){ flag1=0; break;}
    flag1=1;
    }
  if(flag1!=0){
   file_warn(4);
   cls_all_scrn();
   return;
   }
  setcolor(0);
 maxx=x1[0];
 maxy=y1[0];
 for(i=0;i<n;i++)
  {
    if(x1[i]>maxx) maxx=x1[i];
    if((y1[i])>maxy) maxy=y1[i];
  }
  maxx_num=dpoint(maxx,0)-1;
  maxy_num=dpoint(maxy,0)-1;
  for(i=0;i<n;i++)
  {
   x=(int)floor(x0+x1[i]*300/(10*pow(10,maxx_num)));
   y=(int)floor(y0-(y1[i])*300/(10*pow(10,maxy_num)));
   if((y>=64)&&(y<=460)) outtextxy(x,y,"+");
  }
  outtextxy(594,426,"X");
  maxx_num=dpoint(maxx,0)-1;
  gcvt(pow(10,maxx_num),maxx_num,buf);outtext(buf);
  outtextxy(230,104,"X");
  maxy_num=dpoint(maxy,0)-1;
  gcvt(pow(10,maxy_num),maxy_num,buf);outtext(buf);
  for(i=0;i<n;i++){
    _x=_x+x1[i];
    _y=_y+y1[i];
    xy=xy+x1[i]*y1[i];
    xx=xx+x1[i]*x1[i];
    yy=yy+y1[i]*y1[i];
    };
  Sxx=xx-_x*_x/n;
  Syy=yy-_y*_y/n;
  Sxy=xy-_x*_y/n;
  _x=_x/n;_y=_y/n;
  _xy=_x*_y;
  b1=(float)(xy-n*_xy)/(xx-n*_x*_x);
  b0=(float)(_y-_x*b1);
  m0=(int)floor(x0+x1[n-1]*300/(10*pow(10,maxx_num)));
  n0=420-floor((b0+b1*x1[n-1])*300/(10*pow(10,maxy_num)));
  x2=(int)floor(x0-(b0/b1)*300/(10*pow(10,maxx_num)));
  y2=(int)floor(y0);
  if(Sxx*Syy<=0)
    r=1;
  else
    r=(float)(Sxy/sqrt(Sxx*Syy));
  setcolor(15);
  line(x2+3,y2+3,m0+3,n0+3);
  setcolor(0);
  line(x2+2,y2+2,m0+2,n0+2);
  board(61,14,72,16,0,7);
   setcolor(0);
   outtextxy(496,258,"B0=");
   gcvt(b0,dpoint(fabs(b0),3),buf);
   outtextxy(520,258,buf);
   outtextxy(496,266,"B1=");
   gcvt(b1,dpoint(fabs(b1),3),buf);
   outtextxy(520,266,buf);
   outtextxy(496,274,"R =");
   gcvt(fabs(r),dpoint(fabs(r),3),buf);
   outtextxy(520,274,buf);
   in_scrn3[0].ptr=(char *)malloc(sizeof("      "));
   ch="      ";
   strcpy(in_scrn3[0].ptr,ch);
   board(1,20,23,24,20,0);
   disp_hz_str(60,365,"待测浓度",15);
  for(;;){
     exxt=miti_fld(in_scrn3,1,184,8,360);
     if(exxt==27) break;
     y2=atof(in_scrn3[0].ptr);
     x2=(y2-b0)/b1;
     disp_hz_str(16,406,spara_c_hz,YELLOW);
     gcvt(x2,dpoint(x2,3),buf);
     setfillstyle(1,0);
     bar(98,404,178,424);
     disp_hz_str(100,410,buf,YELLOW);
     x2=(int)floor(x0+x2*300/(10*pow(10,maxx_num)));
     y2=(int)floor(y0-(y2)*300/(10*pow(10,maxy_num)));
     setcolor(0);
     if((y2>=64)&&(y2<=460)) outtextxy(x2,y2,"+");
     if((exxt==PGDN)||(exxt==PGUP)) break;
    }
  return;
  }