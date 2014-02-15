#include<fcntl.h>
#include<dos.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<process.h>
#define VGA256 0x13
#define TEXT_MODE 0x03
#ifndef TOTEM_EMS

#define TOTEM_EMS
#include <dos.h>
#include <string.h>
char EMS=0;

char test_ems()
{ int i;
  union REGS inregs,outregs;
  struct SREGS segs;
  struct DeviceHeader {
    struct DeviceHeader far *link;
    unsigned attributes;
    unsigned strategy_offset;
    unsigned interrupt_offset;
    char name_of_number_of_units[8];
  } far *dev;
  unsigned char major,minor,c[13];
  inregs.x.ax=0x3001;
  intdos(&inregs,&outregs);
  major=outregs.h.al;
  minor=outregs.h.ah;
  if (major<2) return(0);
  else
  {
     inregs.h.ah=0x52;
     intdosx(&inregs,&outregs,&segs);
     if (major==2)
       dev=(struct DeviceHeader far *)MK_FP(segs.es+1,outregs.x.bx+7);
     else
       if ((major==3) && (minor==0))
	 dev=(struct DeviceHeader far *)MK_FP(segs.es+2,outregs.x.bx+8);
       else
	 dev=(struct DeviceHeader far *)MK_FP(segs.es+2,outregs.x.bx+2);
     c[8]=NULL;
     while (FP_OFF(dev)!=0xffff)
     {
       if (dev->attributes & 0x8000)
       {
	 for (i=0;i<8;i++)
	   c[i]=dev->name_of_number_of_units[i];
       }
       if (!strcmp(c,"EMMXXXX0"))
       {
	 EMS=1;
	 return(1);
       }
       dev=dev->link;
     }
  }
  return(0);
}

char get_EMS_status()
{
  char tmp=0xff;
  if (EMS)
  {
    asm {
      mov ah,0x40
      int 0x67
      mov tmp,ah
    }
  }
  return(tmp);
  }

char get_page_frame_segment(unsigned *segment)
{
  char tmp=0xff;
  if (EMS)
  {
    asm {
      mov ah,0x41
      int 0x67
      mov tmp,ah
    }
    *segment=_BX;
  }
  return(tmp);
}

char get_number_of_pages(unsigned *avail,unsigned *total)
{
  char tmp=0xff;
  if (EMS)
  {
    asm {
      mov ah,0x42
      int 0x67
      mov tmp,ah
    }
   *avail=_BX;
    *total=_DX;
  }
  return(tmp);
}

char allocate_memory(unsigned *handle,unsigned page_numbers)
{
  char tmp=0xff;
  if (EMS)
  {
    asm {
      mov ah,0x43
      mov bx,page_numbers
      int 0x67
      mov tmp,ah
    }
    *handle=_DX;
  }
  return(tmp);
}

char map_memory(char physical_page,unsigned logical_page,unsigned handle)
{
  char tmp=0xff;
  if (EMS)
  {
    asm {
      mov ah,0x44
      mov al,physical_page
      mov bx,logical_page
      mov dx,handle
      int 0x67
      mov tmp,ah
    }
  }
  return(tmp);
}

char release_memory(unsigned handle)
{
  char tmp=0xff;
  if (EMS)
  {
    asm {
      mov ah,0x45
      mov dx,handle
      int 0x67
      mov tmp,ah
    }
  }
  return(tmp);
}

#endif

unsigned hzhandle;
unsigned char far *fc;

char Load_Hzk16_Ems(void)
{
  int e,hzk,page,i,j;
  char flag;
  unsigned a,pageaddr;
  unsigned char far *tmp,buffer[4096];
  unsigned max_block,total;
  unsigned totalpage;
  hzk=open("hzk16",O_RDONLY|O_BINARY);
  if (hzk!=-1)
  {
    if (test_ems())
    {
      if (!get_EMS_status())
      {
	flag=get_number_of_pages(&a,&totalpage);
	page=28;
	if (a>=page)
	{
	  flag=get_page_frame_segment(&pageaddr);
	}
	if ((!flag) && (!allocate_memory(&hzhandle,page)))
	{
	  fc=(char far *)MK_FP(pageaddr,0);
	  tmp=fc; a=0;
	  for (e=1;e!=0;)
	  {
	    if (tmp==fc+16384)
	    {
	      map_memory(0,a,hzhandle);
	      a++; tmp=fc;
	    }
	    e=read(hzk,buffer,4096);
	    for (i=0;i<e;i+=32)
	    {
	      for (j=0;j<32;j++) *tmp++=buffer[i+j];
	    }
	  }
	  while (tmp-fc<16384) *tmp++=0;
	  close(hzk);
	return(0);
	}
      }
    }
  }
  return(1);
}

void Read_Hzk16_Ems(int ch0,int ch1,unsigned char *buffer)
{
      int i;
      unsigned p1,p2;
      unsigned char far *dot;
      p1=ch0-161;
      p1=p1*94+ch1-161;

	p2=p1>>9;
	map_memory(0,p2,hzhandle);
	p2=(p1 & 511)<<5;
	dot=fc+p2;
	for(i=0;i<32;i++)
		buffer[i]=dot[i];
}

void Free_Hzk16_Ems(void)
{
release_memory(hzhandle);
}

unsigned char bit[8]={128,64,32,16,8,4,2,1};
unsigned char far *video_buffer=(char far *)0xA0000000L;

void Plot_Pixel_Fast(int x,int y,char color)
{
video_buffer[((y<<8)+(y<<6))+x]=color;
}


void Write_Hzk_Ems(char ch0,char ch1,int x,int y,int color)
{
register int i,j,k;
unsigned vpos;
char bitdata[32];
Read_Hzk16_Ems(ch0,ch1,bitdata);
for(i=0;i<16;i++)
for(j=0;j<8;j++){
if(bitdata[2*i]&bit[j])
Plot_Pixel_Fast(x+j,i+y,color);
if(bitdata[2*i+1]&bit[j])
Plot_Pixel_Fast(x+8+j,i+y,color);
}
}
void Write_Hzk_String_Ems(char *str,int x,int y,int color)
{
int num,i,j,xx;
unsigned char s0,s1;
num=strlen(str);
xx=x;
for(i=0;i<num;i+=2){
Write_Hzk_Ems(str[i],str[i+1],xx,y,color);
xx+=16;
}
}

void Set_Video_Mode(int mode)
{
	union REGS inregs,outregs;
	inregs.h.ah=0;
	inregs.h.al=(unsigned char)mode;
	int86(0x10,&inregs,&outregs);
}

void main(void)
{
if(Load_Hzk16_Ems()==0)
{
Set_Video_Mode(VGA256);
Write_Hzk_String_Ems("现在文字处于渐显状态。",10,100,WHITE);
getch();
Free_Hzk16_Ems();
Set_Video_Mode(TEXT_MODE);
}
else
	printf("Ems is not active.");
}