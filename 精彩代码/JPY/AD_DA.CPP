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
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <bios.h>
void da_v(float value)
{
 int lowby,highby,an_data;
 an_data=floor(((value+2048)/4096)*4096);
 lowby=an_data&0xff;
 highby=an_data>>8;
 outportb(0x283,lowby);
 outportb(0x284,highby);
}
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
 printf("highby %d lowby %d\n",highby,lowby);
return(in_v);
}
int main(){
  int i=0;
  da_v(0);
  for(;;){
    da_v(0);
    sleep(1);
    //if(bioskey(0)==283) break;
    ad_v();
    if(kbhit()) break;
    }
  return 0;
  }

