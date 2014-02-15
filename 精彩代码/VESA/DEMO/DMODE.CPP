#include <conio.h>
#include <bios.h>
#include <alloc.h>

#include "\vga\vga.h"
#include "text.h"

void d16_256(int);
void dhigh_true(int);

void dmode()
{
int n=19;
char *str[19]={	" 640 * 480 * 16 color         ",
				" 800 * 600 * 16 color         ",
				"1024 * 768 * 16 color         ",
				" 320 * 200 * 256 color        ",
				" 640 * 400 * 256 color        ",
				" 640 * 480 * 256 color        ",
				" 800 * 600 * 256 color        ",
				"1024 * 768 * 256 color        ",
				" 320 * 200 * 15bit high color ",
				" 512 * 480 * 15bit high color ",
				" 640 * 480 * 15bit high color ",
				" 800 * 600 * 15bit high color ",
				" 320 * 200 * 16bit high color ",
				" 512 * 480 * 16bit high color ",
				" 640 * 480 * 16bit high color ",
				" 800 * 600 * 16bit high color ",
				" 320 * 200 * 24bit true color ",
				" 640 * 480 * 24bit true color ",
				" Return "};
int i;
int x0=25,y0=4;
unsigned colh=color(0,7,0,0),coll=color(7,0,0,0);
void *buf;
buf=malloc(4200);

cclsall();
mmvprs(0,79,0,"VGA Graphics Mode",color(2,0,0,0));
mmvprs(0,79,1,"อออออออออออออออออ",color(2,0,0,0));
i=0;
while(1) {
	i=menu(n,i,x0,y0,coll,colh,str);
	gettext(1,1,80,25,buf);
	if(i==-1||i==n-1)
		break;
	else if(i<8)
		d16_256(i);
	else
		dhigh_true(i);
	puttext(1,1,80,25,buf);
	}
free(buf);
cclsall();
}

void d16_256(int ii)
{
int coln,xn,yn;
VGABASE *A;
if(ii<3)
	{ coln=16; xn=8; }
else
	{ coln=256; xn=16; }
yn=coln/xn;
switch(ii) {
	case 0: A=new _640_480_16; break;
	case 1: A=new _800_600_16; break;
	case 2: A=new _1024_768_16; break;
	case 3: A=new _320_200_256; break;
	case 4: A=new _640_400_256; break;
	case 5: A=new _640_480_256; break;
	case 6: A=new _800_600_256; break;
	case 7: A=new _1024_768_256; break;
	}
if(A->init()==0)
	{
	cclsall();
	mmvprs(0,79,12,"Don't Support The Mode",color(4,0,0,0));
	getkey();
	return;
	}
int x0,y0,dw,dh,bw,bh,i,j,x1,y1;
dw=A->WIDE/xn;
bw=dw*7/8;
x0=(dw-bw)/2;
dh=A->HIGH/yn;
bh=dh*7/8;
y0=(dh-bh)/2;

x1=x0; y1=y0;
j=0;
for(i=0;i<coln;i++)
	{
	if(j>=xn)
		{
		j=0;
		x1=x0; y1+=dh;
		}
	A->bar(x1,y1,x1+bw,y1+bh,A->setcolorto(i));
	x1+=dw;
	j++;
	}
getkey();

long l;
x0=A->WIDE/2;
y0=A->HIGH/2;
y1=y0-1;
l=(long)y1*(long)A->WIDE*3L/(4L*(long)A->HIGH);
x1=l;
A->cls0();
A->setcolor(4);
A->line(0,0,A->WIDE-1,A->HIGH-1);
A->line(0,A->HIGH-1,A->WIDE-1,0);
A->ellipse(x0,y0,x1,y1);
A->setcolor(7);
for(i=1;i<A->WIDE;i+=2)
	A->putpixel(i,y0);
for(i=1;i<A->HIGH;i+=2)
	A->putpixel(x0,i);
A->circle(x0,y0,99);
getkey();
A->close();
delete A;
}

void dhigh_true(int ii)
{
VGABASE *A;
switch(ii) {
	case 8: A=new _320_200_32k; break;
	case 9: A=new _512_480_32k; break;
	case 10: A=new _640_480_32k; break;
	case 11: A=new _800_600_32k; break;
	case 12: A=new _320_200_64k; break;
	case 13: A=new _512_480_64k; break;
	case 14: A=new _640_480_64k; break;
	case 15: A=new _800_600_64k; break;
	case 16: A=new _320_200_16M; break;
	case 17: A=new _640_480_16M; break;
	}
if(A->init()==0)
	{
	cclsall();
	mmvprs(0,79,12,"Don't Support The Mode",color(4,0,0,0));
	getkey();
	return;
	}

int dy=A->HIGH/7,wide=A->WIDE,high=A->HIGH;
int i,j,k,y,r,g,b,c;
long l,l1;
static col[4];
y=0;
for(i=1;i<=8;i++)
	{
	r=g=b=0;
	col[1]=col[2]=col[3]=0;
	if(i&0x01) b=3;
	if(i&0x02) g=2;
	if(i&0x04) r=1;
	for(j=0;j<wide;j++)
		{
		l=(long)j*256L/(long)wide;
		c=l;
		col[r]=c; col[g]=c; col[b]=c;
		A->setcolor(col[1],col[2],col[3]);
		A->line(j,y,j,y+dy);
		}
	y+=dy;
	}
getkey();

A->cls0();
k=0;
r=0;g=1;b=2;
col[r]=k;
do {
	for(i=0;i<high;i++)
		{
		l=(long)i*256L/(long)high;
		col[g]=(int)l;
		for(j=0;j<wide;j++)
			{
			l=(long)j*256L/(long)wide;
			col[b]=(int)l;
			A->putpixel(j,i,A->setcolorto(col[0],col[1],col[2]));
			}
		}
	if(bioskey(1)!=0)
		{
		getkey();
		break;
		}
	r++;g++;b++;
	if(r>2) r=0;
	if(g>2) g=0;
	if(b>2) b=0;
	k+=8;
	col[r]=k;
} while(1);
getkey();
int x0,y0,x1,y1;
x0=A->WIDE/2;
y0=A->HIGH/2;
y1=y0-1;
l=(long)y1*(long)A->WIDE*3L/(4L*(long)A->HIGH);
x1=l;
A->cls0();
A->setcolor(128,0,0);
A->line(0,0,A->WIDE-1,A->HIGH-1);
A->line(0,A->HIGH-1,A->WIDE-1,0);
A->ellipse(x0,y0,x1,y1);
A->setcolor(128,128,128);
for(i=1;i<A->WIDE;i+=2)
	A->putpixel(i,y0);
for(i=1;i<A->HIGH;i+=2)
	A->putpixel(x0,i);
A->circle(x0,y0,99);
getkey();
A->close();
delete A;
}
