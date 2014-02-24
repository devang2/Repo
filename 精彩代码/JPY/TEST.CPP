#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <bios.h>
#include <string.h>
#include <time.h>
#include <io.h>
//#include <float.h>
#include <stat.h>
#include <fcntl.h>
#include <dir.h>
#define lowbyte 0x283
#define highbyte 0x284
float ad_v()
{
 int poll,lowby,highby;
 float in_v,ad_v;
 outportb(0x280,0x00);
 do {
  poll=inportb(0x285);
  poll=poll&128;
 } while(poll!=128);
 lowby=inportb(0x281);
 highby=inportb(0x282)&15;
 ad_v=highby*256+lowby;
 in_v=((ad_v-2047)*20/4096)*1000*(-1);
return(in_v);
}
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
}
void da_v(float value)
{
 int lowby,highby,an_data;
 an_data=floor(((value+2048)/4096)*4096);
 lowby=an_data&0xff;
 highby=an_data>>8;
 outportb(0x283,lowby);
 outportb(0x284,highby);
}
int main()
  {
//  for(;;){
   // outportb(0x220,0xff);
    outportb(lowbyte,0xf0);
    outportb(highbyte,0x00);
  //  }

   /*  outportb(0x283,);
  outportb(0x284,);
  outportb(0x280,);
  outportb(0x283,);
  outportb(0x284,);*/
  return 0;
  }
