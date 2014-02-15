#include <alloc.h>
#include <math.h>

#include "\vga\vgabase.h"
#include "\vga\xms.h"
#include "\vga\fillbase.h"

static double SIN45=0.707106781186548;

void VGABASE::setfillstyle(int fst)
{
FILL::setstyle(fst);
}

int VGABASE::getfillstyle(void)
{
return FILL::getstyle();
}

void VGABASE::setfillpattern(unsigned char *s)
{
FILL::setpattern(s);
}

void VGABASE::bar(int x1,int y1,int x2,int y2)
{
int i;
if(FILL::dataptr==0)
	{
	for(i=y1;i<=y2;i++)
		scanline(x1,x2,i);
	}
else
	{
	for(i=y1;i<=y2;i++)
		FILL::scanlinestyle(x1,x2,i);
	rectangle(x1,y1,x2,y2);
	}
}

void VGABASE::polyfill(int n,int *border)
{
LFILL lfill(this);
lfill.poly(n,border);
lfill.draw();
}

void VGABASE::polyfill(int *border)
{
int n,i=0;
LFILL lf(this);
while(1) {
	n=border[i];
	if(n<=0)
		break;
	i++;
	lf.poly(n,border+i);
	i += (n*2);
	}
lf.draw();
}

void VGABASE::circlefill(int x0,int y0,int r)
{
int i;
long tn,x,y;
long xmax;
YFILL yfill(this,y0-r,y0+r);

y=r; x=0;
xmax=(double)r*SIN45;
tn=(1-r*2);
while(x<=xmax) {
	if(tn>=0)
		{
		tn += ( 6 + ((x-y)<<2) );
		yfill.inspole(x0+x,y0+y);  // 6
		yfill.inspole(x0-x,y0+y);  // 5
		yfill.inspole(x0-x,y0-y);  // 2
		yfill.inspole(x0+x,y0-y);  // 1
		y--;
		}
	else
		tn += ( (x<<2) + 2 );
	yfill.inspole(x0+y,y0+x);  // 7
	yfill.inspole(x0-y,y0+x);  // 4
	if(x>0)
		{
		yfill.inspole(x0-y,y0-x);  // 3
		yfill.inspole(x0+y,y0-x);  // 0
		}
	x++;
	}
yfill.supple();
yfill.draw();
}

void VGABASE::sectorfill(int x0,int y0,int r,int stangle,int endangle)
{
int i,j;
long tn,x,y;
long xmax;
int *xy;
int bx,ex,bxd,exd,bxf,exf,ly1,ly2;

bx=(stangle-1)/45;
if(bx<0) bx=7;
ex=endangle/45;
xy=(int *)calloc(4,2);
xy[0]=x0+(double)r*Lcos(stangle);
xy[1]=y0-(double)r*Lsin(stangle);
xy[2]=x0+(double)r*Lcos(endangle);
xy[3]=y0-(double)r*Lsin(endangle);

bxd=abs(y0-xy[1]);
exd=abs(y0-xy[3]);

if(bx==0||bx==1||bx==4||bx==5)
	bxf=0;
else
	bxf=1;
if(ex==0||ex==1||ex==4||ex==5)
	exf=1;
else
	exf=0;

if( (endangle>=90 && (stangle<=90||stangle>endangle)) ||
(endangle<90 && (stangle>endangle&&stangle<=90)) )
	ly1=y0-r;
else
	{
	ly1=xy[1];
	if(xy[3]<ly1)
		ly1=xy[3];
	}
if( (endangle>=270&&(stangle<=270||stangle>endangle)) ||
(endangle<270&&(stangle>endangle&&stangle<=270)) )
	ly2=y0+r;
else
	{
	ly2=xy[3];
	if(xy[1]>ly2)
		ly2=xy[1];
	}
YFILL yfill(this,ly1,ly2);

if( (xy[1]<=y0&&xy[0]>=x0) || (xy[1]>=y0&&xy[0]<=x0) )
	yfill.inspole(xy[0],xy[1]);
else
	{
	yfill.inspole(xy[0],xy[1]);
	yfill.inspole(xy[0],xy[1]);
	}

if( (xy[3]<=y0&&xy[2]<=x0) || (xy[3]>=y0&&xy[2]>=x0) )
	yfill.inspole(xy[2],xy[3]);
else
	{
	yfill.inspole(xy[2],xy[3]);
	yfill.inspole(xy[2],xy[3]);
	}

if( (xy[1]>y0&&xy[3]>y0) || (xy[1]<y0&&xy[3]<y0) )
	{
	yfill.inspole(x0,y0);
	yfill.inspole(x0,y0);
	}
else if( (xy[1]>=y0&&xy[3]<y0) || (xy[1]<y0&&xy[3]>=y0) )
	yfill.inspole(x0,y0);

if(y0!=xy[1])
	yfill.line(x0,y0,xy[0],xy[1]);
if(y0!=xy[3])
	yfill.line(xy[2],xy[3],x0,y0);

y=r; x=0;
xmax=(double)r*SIN45;
tn=(1-r*2);
while(x<=xmax) {
	if(tn>=0)
		{
		tn += ( 6 + ((x-y)<<2) );
		if( (ex>bx&&bx<6&&ex>6) || (endangle<stangle&&(bx<6||ex>6)) ||
		(bx==6&&((y>bxd)^bxf)&&y!=bxd) || (ex==6&&((y>exd)^exf)&&y!=exd) )
			yfill.inspole(x0+x,y0+y);  // 6
		if( (ex>bx&&bx<5&&ex>5) || (endangle<stangle&&(bx<5||ex>5)) ||
		(bx==5&&((y>bxd)^bxf)&&y!=bxd) || (ex==5&&((y>exd)^exf)&&y!=exd) )
			yfill.inspole(x0-x,y0+y);  // 5
		if( (ex>bx&&bx<2&&ex>2) || (endangle<stangle&&(bx<2||ex>2)) ||
		(bx==2&&((y>bxd)^bxf)&&y!=bxd) || (ex==2&&((y>exd)^exf)&&y!=exd) )
			yfill.inspole(x0-x,y0-y);  // 2
		if( (ex>bx&&bx<1&&ex>1) || (endangle<stangle&&(bx<1||ex>1)) ||
		(bx==1&&((y>bxd)^bxf)&&y!=bxd) || (ex==1&&((y>exd)^exf)&&y!=exd) )
			yfill.inspole(x0+x,y0-y);  // 1
		y--;
		}
	else
		tn += ( (x<<2) + 2 );
	if( (ex>bx&&bx<7&&ex>7) || (endangle<stangle&&(bx<7||ex>7)) ||
	(bx==7&&((x>bxd)^bxf)&&x!=bxd) || (ex==7&&((x>exd)^exf)&&x!=exd) )
		yfill.inspole(x0+y,y0+x);  // 7
	if( (ex>bx&&bx<4&&ex>4) || (endangle<stangle&&(bx<4||ex>4)) ||
	(bx==4&&((x>bxd)^bxf)&&x!=bxd) || (ex==4&&((x>exd)^exf)&&x!=exd) )
		yfill.inspole(x0-y,y0+x);  // 4
	if(x>0)
		{
		if( (ex>bx&&bx<3&&ex>3) || (endangle<stangle&&(bx<3||ex>3)) ||
		(bx==3&&((x>bxd)^bxf)&&x!=bxd) || (ex==3&&((x>exd)^exf)&&x!=exd) )
			yfill.inspole(x0-y,y0-x);  // 3
		if( (ex>bx&&bx<0&&ex>0) || (endangle<stangle&&(bx<0||ex>0)) ||
		(bx==0&&((x>bxd)^bxf)&&x!=bxd) || (ex==0&&((x>exd)^exf)&&x!=exd) )
			yfill.inspole(x0+y,y0-x);  // 0
		}
	x++;
	}
yfill.supple();
yfill.draw();
free(xy);
}

void VGABASE::ellipsefill(int x0,int y0,long r1,long r2)
{
long r,xy,r12,r22;
int x,y,xmax,ymax;
long tn,ir12,ir22;

YFILL yfill(this,y0-r2,y0+r2);

x=0;y=r2;
r12=r1*r1;r22=r2*r2;ir12=r12;ir22=r22;
xmax=(double)r12/sqrt(r12+r22);
tn=r12-2*r2*r12;
while(x<=xmax) {
	if(tn<0||y==0)
		tn+=(4*x+2)*ir22;
	else
		{
		tn+=(4*x+2)*ir22+(1-y)*4*ir12;
		yfill.inspole(x0+x,y0+y);
		yfill.inspole(x0-x,y0+y);
		yfill.inspole(x0+x,y0-y);
		yfill.inspole(x0-x,y0-y);
		y--;
		}
	x++;
	}
x--;
yfill.inspole(x0+x,y0+y);
yfill.inspole(x0-x,y0+y);
yfill.inspole(x0+x,y0-y);
yfill.inspole(x0-x,y0-y);

r=r1;r1=r2;r2=r;
x=0;y=r2;
r12=r1*r1;r22=r2*r2;ir12=r12;ir22=r22;
xmax=(int)( (double)r12/sqrt(r12+r22)-0.1);
tn=r12-2*r2*r12;
while(x<=xmax) {
	if(tn<0||y==0)
		tn+=(4*x+2)*ir22;
	else
		{
		tn+=(4*x+2)*ir22+(1-y)*4*ir12;
		y--;
		}
	yfill.inspole(x0+y,y0+x);
	yfill.inspole(x0-y,y0+x);
	if(x>0)
		{
		yfill.inspole(x0+y,y0-x);
		yfill.inspole(x0-y,y0-x);
		}
	x++;
	}
yfill.supple();
yfill.draw();
}

void VGABASE::fillarea(int x0,int y0,union COLOR bcolor)
{
int i,j,k,f;
static int fxy[8][2]={	{1,0},{1,-1},{0,-1},{-1,-1},
						{-1,0},{-1,1},{0,1},{1,1}};
int *xx,*yy,x,y;
int ck,pk;
int n=0,maxxyn=4096;
LFILL lfill(this);

xx=(int *)calloc(maxxyn,2);
yy=(int *)calloc(maxxyn,2);

for(i=x0;i<WIDE;i++)
	if( getpixel(i,y0).dword==bcolor.dword )
		{
		xx[0]=i;
		yy[0]=y0;
		n++;
		break;
		}
if(n==0)
	goto RETU;

f=0;
k=0;
while(1) {
	j=f;
	for(i=0;i<8;i++)
		{
		if( i==0||i==2||i==4||i==6 )
			j-=i;
		else
			j+=i;
		if(j<0) j+=8;
		if(j>=8) j-=8;
		x=xx[k]+fxy[j][0];
		y=yy[k]+fxy[j][1];
		if( getpixel(x,y).dword==bcolor.dword)
			{
			putpixel(x,y);
			xx[n]=x;
			yy[n]=y;
			n++; k++; f=j;
			break;
			}
		}
	if(i>=8)
		{
		n--;k--;
		if(k<0)
			goto RETU;
		}
	if(n>=maxxyn)
		goto RETU;
	if(xx[k]==xx[0]&&yy[k]==yy[0])
		break;
	}


for(i=1;i<n;i++)
	if(yy[i]!=yy[0])
		{
		ck=i;
		break;
		}
for(i=n-2;i>0;i--)
	if(yy[i]!=yy[0])
		{
		pk=i;
		break;
		}
if((yy[pk]>yy[pk+1]&&yy[ck]>yy[ck-1])||(yy[pk]<yy[pk+1]&&yy[ck]<yy[ck-1]))
	{
	lfill.inspole(xx[pk+1],yy[pk+1]);
	lfill.inspole(xx[ck-1],yy[ck-1]);
	}
else
	lfill.inspole(xx[ck-1],yy[ck-1]);

for(i=ck+1;i<n;i++)
	{
	if(yy[i]!=yy[ck])
		{
		if((yy[i]>yy[ck]&&yy[ck-1]>yy[ck])||(yy[i]<yy[ck]&&yy[ck-1]<yy[ck]))
			{
			lfill.inspole(xx[ck],yy[ck]);
			lfill.inspole(xx[i-1],yy[i-1]);
			}
		else
			lfill.inspole(xx[ck],yy[ck]);
		ck=i;
		}
	}

lfill.draw();

RETU:
free(xx);free(yy);
}
