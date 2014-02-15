#include<fcntl.h>
#include<dos.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<process.h>
#define VGA256 0x13
#define TEXT_MODE 0x03
#ifndef TOTEM_XMS

#define TOTEM_XMS

#include <dos.h>

char XMS=0,HMA=0,UMB=0;

void (far *xms)(void);

char test_xms()
{
	asm {
		mov ax,0x4300
			int 0x2f
	}
	if (_AL==0x80) XMS=1;
	return(XMS);
}

void get_driver_address()
{
	if (XMS)
	{
		asm {
			mov ax,0x4310
				int 0x2f
		}
		xms=(void (far *)())(((long)(_ES)<<16)+_BX);
	}
}

char query_free_xms(unsigned *max_block,unsigned *total)
{
	char error_code=0xff;
	if (XMS)
	{
		asm mov ah,0x08
			xms();
		*max_block=_AX;
		*total=_DX;
		error_code=_BL;
	}
	return(error_code);
}

char allocate_xms(unsigned size,unsigned *handle)
{
	char error_code=0xff;
	if (XMS)
	{
		asm {
			mov ah,0x09
				mov dx,size
		}
		xms();
		*handle=_DX;
		error_code=_BL;
	}
	return(error_code);
}

char free_xms(unsigned handle)
{
	char error_code=0xff;
	if (XMS)
	{
		asm {
			mov ah,0x0a
				mov dx,handle
		}
		xms();
		error_code=_BL;
	}
	return(error_code);
}

typedef struct xms_mov {
	unsigned long byte_count;             //移动的字节数
	unsigned source_handle;     		//源句柄
	unsigned long source_offset;          //源偏移量
	unsigned destination_handle;          //目的句柄
	unsigned long destination_offset;     //目的偏移量
} xmm;
xmm xmove;

char move_xms(xmm *xmm_ptr)
{
	char error_code=0xff;
	if (XMS)
	{
		unsigned xseg=FP_SEG(xmm_ptr),xoff=FP_OFF(xmm_ptr);
		asm {
			mov ah,0x0b
				mov si,xoff
				mov ds,xseg
		}
		xms();
		error_code=_BL;
	}
	return(error_code);
}

#endif

unsigned hzhandle;

char Load_Hzk16_Xms(void)
{
	int e,hzk;
	char flag;
	unsigned a,pageaddr;
	unsigned char far *tmp,buffer[4096];
	unsigned max_block,total;
	hzk=open("hzk16",O_RDONLY|O_BINARY);
	if (hzk!=-1)
	{
		if (test_xms())
		{
			get_driver_address();
			query_free_xms(&max_block,&total);
			if (max_block>300)
			{
				if (!allocate_xms(300,&hzhandle))
				{
					unsigned long s=0;
					xmove.byte_count=4096;
					xmove.source_handle=0;
					xmove.source_offset=(unsigned long)buffer;
					xmove.destination_handle=hzhandle;
					for (;e>0;)
					{
						e=read(hzk,buffer,4096);
						xmove.destination_offset=s;
						s+=e;
						move_xms(&xmove);
					}
					close(hzk);
					xmove.byte_count=32;
					xmove.source_handle=hzhandle;
					xmove.destination_handle=0;
					return(0);
				}
			}
		}
	}
	return(1);
}


void Read_Hzk16_Xms(int ch0,int ch1,unsigned char *buffer)
{
	unsigned p1,p2;
	p1=ch0-161;
	p1=p1*94+ch1-161;
	
	xmove.source_offset=p1;
	xmove.source_offset*=32;
	xmove.destination_offset=(unsigned long)buffer;
	move_xms(&xmove);
}

void Free_Hzk16_Xms(void)
{
	free_xms(hzhandle);
}

unsigned char bit[8]={128,64,32,16,8,4,2,1};
unsigned char far *video_buffer=(char far *)0xA0000000L;

void Plot_Pixel_Fast(int x,int y,char color)
{
	video_buffer[((y<<8)+(y<<6))+x]=color;
}


void Write_Hzk_Xms(char ch0,char ch1,int x,int y,int color)
{
	register int i,j,k;
	unsigned vpos;
	char bitdata[32];
	Read_Hzk16_Xms(ch0,ch1,bitdata);
	for(i=0;i<16;i++)
		for(j=0;j<8;j++){
			if(bitdata[2*i]&bit[j])
				Plot_Pixel_Fast(x+j,i+y,color);
			if(bitdata[2*i+1]&bit[j])
				Plot_Pixel_Fast(x+8+j,i+y,color);
		}
}
void Write_Hzk_String_Xms(char *str,int x,int y,int color)
{
	int num,i,j,xx;
	unsigned char s0,s1;
	num=strlen(str);
	xx=x;
	for(i=0;i<num;i+=2){
		Write_Hzk_Xms(str[i],str[i+1],xx,y,color);
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
	if(Load_Hzk16_Xms()==0)
	{
		Set_Video_Mode(VGA256);
		Write_Hzk_String_Xms("现在文字处于渐显状态。",10,100,WHITE);
		getch();
		Free_Hzk16_Xms();
		Set_Video_Mode(TEXT_MODE);
	}
	else
		printf("xms is not active.");
}