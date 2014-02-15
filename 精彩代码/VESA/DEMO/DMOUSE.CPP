#include <conio.h>
#include <bios.h>
#include <alloc.h>

#include "\vga\vga.h"
#include "text.h"

void mouse16_256(int);
void mouse_ht(int ii);
void tme();

void dmouse()
{
int n=6;
char *str[6]={	" in 640*480*16 color ",
				" in 800*600*256 color ",
				" in 512*480*64k color ",
				" in 320*200*16M color ",
				" Test Mouse Event ",
				" Return "};
int i;
int x0=28,y0=7;
unsigned colh=color(0,7,0,0),coll=color(7,0,0,0);
void *buf;
buf=malloc(4200);

cclsall();
mmvprs(0,79,y0-3," Mouse Drive ",color(2,0,0,0));
mmvprs(0,79,y0-2,"อออออออออออออ",color(2,0,0,0));
i=0;
while(1) {
	i=menu(n,i,x0,y0,coll,colh,str);
	gettext(1,1,80,25,buf);
	if(i==-1||i==(n-1))
		break;
	else if(i<=1)
		mouse16_256(i);
	else if(i<=3)
		mouse_ht(i);
	else
		tme();
	puttext(1,1,80,25,buf);
	}
free(buf);
cclsall();
}

void mouse16_256(int ii)
{
int coln,xn,yn;
VGABASE *A;
if(ii==0)
	{
	coln=16; xn=8;
	A=new _640_480_16;
	}
else
	{
	coln=256; xn=16;
	A=new _800_600_256;
	}
yn=coln/xn;

if(A->init()==0)
	{
	cclsall();
	mmvprs(0,79,12,"Don't Support The Grpahics Mode",color(4,0,0,0));
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
setup_key_mouse();
MOUSE::init();
MouseMap *mmap;
if(ii==0)
	mmap=new MouseMap16(A);
else
	mmap=new MouseMap256(A);
MOUSE::putcentre();
MOUSE::show();
Event evt;
evt.getevent( (MouseDown|evKey) );
MOUSE::hide();
delete mmap;
del_key_mouse();
A->close();
delete A;
}

void mouse_ht(int ii)
{
VGABASE *A;
switch(ii) {
	case 2: A=new _512_480_64k; break;
	case 3: A=new _320_200_16M; break;
	}
if(A->init()==0)
	{
	cclsall();
	mmvprs(0,79,12,"Don't Support The Grpahics Mode",color(4,0,0,0));
	getkey();
	return;
	}

int wide=A->WIDE,high=A->HIGH;
int i,j,y,r,g,b;
static int col[3];
long l,l1;

r=0;g=1;b=2;
col[r]=64;
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

setup_key_mouse();
MOUSE::init();
MouseMap *mmap;
if(ii==2)
	mmap=new MouseMapHigh(A);
else
	mmap=new MouseMap16M(A);
MOUSE::putcentre();
MOUSE::show();
Event evt;
evt.getevent( (MouseDown|evKey) );
MOUSE::hide();
delete mmap;
del_key_mouse();
A->close();
delete A;
}

void tme()
{
int n=12;
static char *mes[12]={	"Left Button Down",
				"Left Button Up",
				"Left Button Auto-down",
				"Left Button Double Strike",
				"Right Button Down",
				"Right Button Up",
				"Right Button Auto-down",
				"Right Button Double Strike",
				"Middle Button Down",
				"Middle Button Up",
				"Middle Button Auto-down",
				"Middle Button Double Strike"};
static unsigned mev[12]={MouseDown,MouseUp,MouseAuto,MouseDouble,
				MouseDownR,MouseUpR,MouseAutoR,MouseDoubleR,
				MouseDownM,MouseUpM,MouseAutoM,MouseDoubleM};
_640_480_16 A;
A.init();
setup_key_mouse();
MOUSE::init();
MouseMap16 mmap(&A);
MOUSE::putcentre();

int x1=160,y1=30,dy=24,yd=y1+dy*n,x2=x1+15,yt=yd+n+1,xt=x1+dy;
int i,x,y;
ASC16 text("\\vga\\font\\asck16",inMEM,&A);
y=y1; x=xt;
A.setcolor(7);
for(i=0;i<n;i++)
	{
	text.outtextxy(x,y,mes[i]);
	y+=dy;
	}
Event evt;
while(1) {
	MOUSE::show();
	evt.getevent( (evMouse|evKey) );
	MOUSE::hide();
	if( evt.what&evButton )
		A.bar(x1,y1,x2,yd,A.setcolorto(0));
	A.setcolor(4);
	for(i=0;i<n;i++)
		if( evt.what&mev[i] )
			{
			y=y1+i*dy;
			A.bar(x1,y,x2,y+15);
			}
	if( evt.what&MouseMove )
		{
		A.bar(xt,yt,xt+64,yt+dy,A.setcolorto(0));
		A.setcolor(4);
		text.putxy(xt,yt);
		text.printf("%d,%d",evt.mouse.x,evt.mouse.y);
		}
	if( evt.what&evKey )
		break;
	}

MOUSE::hide();
del_key_mouse();
A.close();
}
