//I N C L U D E -----------------------------
#ifndef _HZ_HPP
	#include "hz.hpp"
#endif
#ifndef __GRAPHICS_H
	#include <graphics.h>
#endif
#ifndef __DOS_H
	#include <dos.h>
#endif

//D E F I N E ----------------------------------

HZ hz("..\\hz\\hzk16");

void HZ::Get_asc_8x16()
{
	struct REGPACK reg;

	reg.r_ax = 0x1130;
	reg.r_bx = 0x600;
	intr(0x10, &reg);

	asc_8x16_fp=(char far *)MK_FP(reg.r_es,reg.r_bp);
}


//DispHz():Display a hz
//
void HZ::DispHz(int qh,int wh)
{
	struct linesettingstype oldlinetype;
	unsigned int buf[16];
	HZK::GetFont16(buf,qh,wh);

	unsigned int linetypedata[16];

	for(int i=0;i<16;i++)
	{
		 linetypedata[i]=(buf[i]<<8&0xff00)|(buf[i]>>8&0x00ff);
	}
	getlinesettings(&oldlinetype);
	for(i=0;i<16;i++)
	{
		  setlinestyle(USERBIT_LINE,linetypedata[i],1);
		  moveto(curx,cury+i);
		  lineto(curx+15,cury+i);
	}
	this->curx+=16+HZ::zj;
	setlinestyle(oldlinetype.linestyle,oldlinetype.upattern,oldlinetype.thickness);
}

//DispAsc():Display ASCII character
//
void HZ::DispAsc(char asc)
{
	if(asc == '\n') {
		HZ::cury+=16 + hj ;curx = 0 ;
		return ;
	}else if(asc == '\t') {
		curx += 3*8 ;
		return ;
	}
	char far *by=asc_8x16_fp+asc*16;

	for(int i=0;i<16;i++)
		for(int j=0;j<8;j++)
			if((by[i]>>(7-j))&1)
				putpixel(curx+size*j,cury+size*i,getcolor());
	this->curx+=8*size+zj;
}

void HZ::Outtext(char far *str)
{
	int ishz= FALSE;
	int qh,wh;
	unsigned int c;
	while((c=*str++)!=0)
	{
		if(c>0xa1)
			if(ishz){
				ishz=FALSE;
				wh=(c-0xa1)&0x07f;
				HZ::DispHz(qh,wh);
			}
			else{
				ishz=TRUE;
				qh=(c-0xa1)&0x07f;
			}
		else
		{
			HZ::DispAsc(*(str-1));
		}
	}
}
