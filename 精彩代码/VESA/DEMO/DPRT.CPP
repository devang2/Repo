#include <conio.h>
#include <bios.h>
#include <alloc.h>

#include "\vga\vga.h"
#include "text.h"

void prt(int);

void dprt()
{
int n=9;
char *str[9]={	" EPSON 360dpi (ESC/P2) ",
				" EPSON 180dpi (ESC/P2 & ESC/P) ",
				" EPSON 60dpi (ESC/P2 & ESC/P) ",
				" HP 600dpi (PCL) ",
				" HP 300dpi (PCL) ",
				" HP 150dpi (PCL) ",
				" HP 100dpi (PCL) ",
				" HP 75dpi (PCL) ",
				" Return "};
int i;
int x0=25,y0=7;
unsigned colh=color(0,7,0,0),coll=color(7,0,0,0);
void *buf;
buf=malloc(4200);

cclsall();
mmvprs(0,79,y0-3," Graphics Print ",color(2,0,0,0));
mmvprs(0,79,y0-2,"様様様様様様様様",color(2,0,0,0));
i=0;
while(1) {
	i=menu(n,i,x0,y0,coll,colh,str);
	gettext(1,1,80,25,buf);
	if(i==-1||i==(n-1))
		break;
	else
		{
		cclsall();
		int sts,abyte=0;
		sts=biosprint(2,abyte,0);
		if( (sts&0x01) || (sts&0x08) || (sts&0x20) || (sts&0x40) ||
		((sts&0x10)==0) || ((sts&0x80)==0) )
			{
			mmvprs(0,79,12,"Printer error",color(4,0,0,0));
			getkey();
			}
		else
			{
			mmvprs(0,79,12,"Being Print.",color(2,0,0,0));
			prt(i);
			}
		}
	puttext(1,1,80,25,buf);
	}
free(buf);
cclsall();
}

void prt(int ii)
{
static int dpi[8]={360,180,60,600,300,150,100,75};
int cy=5;
XMS::init();
prt_buf A(dpi[ii]*cy,dpi[ii]*cy,inMEM);
int wide=A.WIDE,high=A.HIGH;
A.setcolor(1);
A.rectangle(0,0,wide-1,high-1);
A.circle(wide/2,high/2,wide/2-1);
A.circlefill(wide/2,high/2,100);
PRINTER *p;
switch(ii) {
	case 0: p=new EPSON_360(&A); break;
	case 1: p=new EPSON_180(&A); break;
	case 2: p=new EPSON_60(&A); break;
	case 3: p=new HP_600(&A); break;
	case 4: p=new HP_300(&A); break;
	case 5: p=new HP_150(&A); break;
	case 6: p=new HP_100(&A); break;
	case 7: p=new HP_75(&A); break;
	}
p->print();
p->nextrow();
}
