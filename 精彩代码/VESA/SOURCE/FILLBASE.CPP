#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <math.h>

#include "\vga\xms.h"
#include "\vga\vgabase.h"
#include "\vga\fillbase.h"

VGABASE *FILL::vga;
int FILL::dataptr=0;
int FILL::STDN=12;
unsigned char FILL::STD[96]={
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,

	0xff,0xff,0x00,0x00,0xff,0xff,0x00,0x00,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,

	0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x81,
	0x1e,0x3c,0x78,0xf0,0xe1,0xc3,0x87,0x0f,

	0xc0,0x60,0x30,0x18,0x0c,0x06,0x03,0x81,
	0x78,0x3c,0x1e,0x0f,0x87,0xc3,0xe1,0xf0,

	0x41,0x22,0x14,0x08,0x14,0x22,0x41,0x80,
	0x44,0xff,0x44,0x44,0x44,0xff,0x44,0x44,

	0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
	0x30,0x30,0x03,0x03,0x30,0x30,0x03,0x03,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void FILL::scanlinestyle(int x1,int x2,int y)
{
unsigned char c,c1,b;
int x;

if(x1>x2)
	{
	x=x1; x1=x2; x2=x;
	}
c=c1=STD[dataptr+(y&7)];
b=x1&7;
c<<=b;
for(x=x1;x<=x2;x++)
	{
	if( c&0x80 )
		vga->putpixel(x,y);
	b++;
	if(b==8)
		{
		b=0;
		c=c1;
		}
	else
		c<<=1;
	}
}

void FILL::setstyle(int fst)
{
if(fst>=0&&fst<STDN)
	dataptr=fst*8;
}

int FILL::getstyle(void)
{
return(dataptr/8);
}

void FILL::setpattern(unsigned char *s)
{
dataptr=(STDN-1)*8;
memcpy(STD+dataptr,s,8);
}

void FILL::line(int x1,int y1,int x2,int y2)
{
int i,p,n,x,y,tn,my;

if( abs(y1-y2)<=1 )
	return;
if( x1==x2 )
	{
	if(y1>y2)
		{ y=y2; y2=y1; y1=y; }
	for(i=y1+1;i<y2;i++)
		inspole(x1,i);
	return;
	}

if( abs(y2-y1) <= abs(x2-x1) )
	{
	if( (y2<y1&&x2<x1) || (y1<=y2&&x1>x2) )
		{
		x=x2; y=y2; x2=x1; y2=y1; x1=x; y1=y;
		}
	if( y2>=y1 && x2>=x1 )
		{
		x=x2-x1; y=y2-y1;
		p=2*y; n=2*x-2*y; tn=x;
		my=y2;
		while(x1<=x2) {
			if(tn>=0) tn-=p;
			else
				{
				tn+=n; y1++;
				if(y1<my)
					inspole(x1,y1);
				}
			x1++;
			}
		}
	else
		{
		x=x2-x1; y=y2-y1;
		p=-2*y; n=2*x+2*y; tn=x;
		my=y2;
		while(x1<=x2) {
			if(tn>=0) tn-=p;
			else
				{
				tn+=n; y1--;
				if(y1>my)
					inspole(x1,y1);
				}
			x1++;
			}
		}
	}
else
	{
	x=x1; x1=y2; y2=x; y=y1; y1=x2; x2=y;
	if( (y2<y1&&x2<x1) || (y1<=y2&&x1>x2) )
		{
		x=x2; y=y2; x2=x1; y2=y1; x1=x; y1=y;
		}
	if( y2>=y1 && x2>=x1 )
		{
		x=x2-x1; y=y2-y1;
		p=2*y; n=2*x-2*y; tn=x;
		x1++;
		while(x1<x2) {
			if(tn>=0) tn-=p;
			else { tn+=n; y1++; }
			inspole(y1,x1);
			x1++;
			}
		}
	else
		{
		x=x2-x1; y=y2-y1;
		p=-2*y; n=2*x+2*y; tn=x;
		x1++;
		while(x1<x2) {
			if(tn>=0) tn-=p;
			else { tn+=n; y1--; }
			inspole(y1,x1);
			x1++;
			}
		}
	}
}

void LFS::insbefore(LFS *lfs)
{
next=lfs;
prev=lfs->prev;
prev->next=this;
lfs->prev=this;
}

void LFS::del()
{
if(prev!=0&&next!=0)
	{
	prev->next=next;
	next->prev=prev;
	}
}

LFILL::LFILL(VGABASE *v) : FILL(v)
{
beg=new LFS(-1,-1);
end=new LFS(8888,8888);
beg->prev=end->next=0;
beg->next=end;
end->prev=beg;
cur=end;
}

LFILL::~LFILL()
{
LFS *p,*p1;
p=beg;
while(p!=0) {
	p1=p->next;
	delete p;
	p=p1;
	}
}

void LFILL::inspole(int x,int y)
{
LFS *p;
p=cur;
cur=new LFS(x,y);

if( *cur>*p )
	{
	p=p->next;
	while(1) {
		if( *cur>*p )
			p=p->next;
		else
			{
			cur->insbefore(p);
			break;
			}
		}
	}
else
	{
	p=p->prev;
	while(1) {
		if( *cur<*p )
			p=p->prev;
		else
			{
			cur->insbefore(p->next);
			break;
			}
		}
	}
}

void LFILL::inspole(LFS *lp)
{
LFS *p;
p=cur;
cur=lp;

if( *lp>*p )
	{
	p=p->next;
	while(1) {
		if( *lp>*p )
			p=p->next;
		else
			{
			lp->insbefore(p);
			break;
			}
		}
	}
else
	{
	p=p->prev;
	while(1) {
		if( *lp<*p )
			p=p->prev;
		else
			{
			lp->insbefore(p->next);
			break;
			}
		}
	}
}

void LFILL::add(LFILL *lfp)
{
LFS *begp=lfp->beg;
LFS *lp=lfp->beg->next;
while(lp->next!=0) {
	lp->del();
	inspole(lp);
	lp=begp->next;
	}
}

int LFILL::ifincl(LFILL *lfp)
{
LFS *lp,*dlp;

lp=beg->next;
dlp=lfp->beg->next;
while(dlp->next!=0) {
	while(1) {
		if( lp==0 || lp->next==0 )
			return(0);
		if( dlp->y > lp->y )
			lp=lp->next->next;
		else if( dlp->y < lp->y )
			lp=lp->prev->prev;
		else // dlp->y==lp->y
			{
			if( dlp->x >= lp->x )
				{
				if( dlp->x >= lp->next->x )
					{
					if( dlp->y == lp->next->next->y &&
						dlp->next->x > lp->next->next->x )
						lp=lp->next->next;
					else
						return(0);
					}
				else if( dlp->next->x <= lp->next->x)
					break;
				else
					return(0);
				}
			else if( dlp->next->x <= lp->x )
				{
				if( dlp->y == lp->prev->y )
					lp=lp->prev->prev;
				else
					return(0);
				}
			else
				return(0);
			}
		}
	dlp=dlp->next->next;
	}
return(1);
}

void LFILL::poly(int n,int *border)
{
int i,j,nn;
int *x,*y;

x=(int *)calloc(n+3,2);
y=(int *)calloc(n+3,2);
j=0;
for(i=1;i<=n;i++)
	{
	x[i]=border[j]; j++;
	y[i]=border[j]; j++;
	if( i>1 && x[i]==x[i-1] && y[i]==y[i-1] )
		{ i--; n--; }
	}
for(i=2;i<n;i++)
	{
	if( y[i]==y[i-1] && y[i]==y[i+1] )
		{
		memmove(&x[i],&x[i+1],(n-i)*2);
		memmove(&y[i],&y[i+1],(n-i)*2);
		n--; i--;
		}
	}
if( y[1]==y[2] && y[1]==y[n] )
	{
	memmove(&x[1],&x[2],(n-1)*2);
	memmove(&y[1],&y[2],(n-1)*2);
	n--;
	}
if( y[n]==y[n-1] && y[n]==y[1] )
	n--;
x[0]=x[n]; y[0]=y[n];
x[n+1]=x[1]; y[n+1]=y[1];
x[n+2]=x[2]; y[n+2]=y[2];

for(i=1;i<=n;i++)
	{
	j=i+1;
	if(y[i]==y[j])
		{
		if( (y[i]<=y[i-1] && y[j]<=y[j+1]) ||
			(y[i]>=y[i-1] && y[j]>=y[j+1]) )
			{
			inspole(x[i],y[i]);
			inspole(x[j],y[j]);
			}
		else
			inspole(x[i],y[i]);
		}
	else if(y[i]!=y[i-1])
		{
		if( (y[i]<y[i-1] && y[i]<y[i+1]) ||
			(y[i]>y[i-1] && y[i]>y[i+1]) )
			{
			inspole(x[i],y[i]);
			inspole(x[i],y[i]);
			}
		else
			{
			inspole(x[i],y[i]);
			}
		}
	}
for(i=1;i<=n;i++)
	line(x[i],y[i],x[i+1],y[i+1]);

free(x); free(y);
}


void LFILL::draw(void)
{
LFS *p1,*p2;

p1=beg->next;
if(dataptr==0)
	{
	while(p1!=0) {
		p2=p1->next;
		if(p2==0)
			break;
		vga->scanline(p1->x,p2->x,p1->y);
		p1=p2->next;
		}
	}
else
	{
	while(p1!=0) {
		p2=p1->next;
		if(p2==0)
			break;
		scanlinestyle(p1->x,p2->x,p1->y);
		p1=p2->next;
		}
	}
}

void LFILL::save(void)
{
LFS *p1,*p2;
FILE *fp;

fp=fopen("LFILL.DAT","w");
fprintf(fp,"%d  %d\n\n",beg->x,beg->y);
p1=beg->next;
while(p1!=0) {
	p2=p1->next;
	fprintf(fp,"%d  %d\n",p1->x,p1->y);
	fprintf(fp,"%d  %d\n\n",p2->x,p2->y);
	p1=p2->next;
	if(p2==0)
		break;
	}
fclose(fp);
}


YFILL::YFILL(VGABASE *v,int y1,int y2) : FILL(v)
{
if(y2<y1)
	{ Y1=y2; Y2=y1; }
else
	{ Y1=y1; Y2=y2; }
yfs=(YFS *)calloc(Y2-Y1+1,sizeof(YFS));
}

YFILL::~YFILL()
{
free(yfs);
}

void YFILL::inspole(int x,int y)
{
if(y>=Y1&&y<=Y2)
	{
	int yy=y-Y1;
	yfs[yy].x[yfs[yy].n]=x;
	yfs[yy].n++;
	}
}

void YFILL::supple(void)
{
int i,n;
n=Y2-Y1+1;
for(i=1;i<n;i++)
	if(yfs[i].n<=1&&yfs[i-1].n>=2)
		{
		yfs[i].n=2;
		yfs[i].x[0]=yfs[i-1].x[0];
		yfs[i].x[1]=yfs[i-1].x[1];
		}
}

void YFILL::draw(void)
{
int i,x,n,y;
int *xx;

n=Y2-Y1+1;
y=Y1;
for(i=0;i<n;i++)
	{
	xx=yfs[i].x;
	if(yfs[i].n==4)
		{
		if(xx[0]>xx[1])
			{
			x=xx[1];
			xx[1]=xx[0];
			xx[0]=x;
			}
		if(xx[2]>xx[3])
			{
			x=xx[3];
			xx[3]=xx[2];
			xx[2]=x;
			}
		if( xx[2]<xx[1] && xx[3]>xx[0] )
			{
			x=xx[2];
			xx[2]=xx[1];
			xx[1]=x;
			}
		if(dataptr==0)
			{
			vga->scanline(xx[2],xx[3],y);
			vga->scanline(xx[0],xx[1],y);
			}
		else
			{
			scanlinestyle(xx[2],xx[3],y);
			scanlinestyle(xx[0],xx[1],y);
			}
		}
	else if(yfs[i].n==2)
		{
		if(dataptr==0)
			vga->scanline(xx[0],xx[1],y);
		else
			scanlinestyle(xx[0],xx[1],y);
		}
	y++;
	}
}

void YFILL::save(void)
{
int i,n;
FILE *fp;

fp=fopen("YFILL.DAT","w");
n=Y2-Y1+1;
for(i=0;i<n;i++)
	fprintf(fp,"%3d  %d   %d  %d  %d  %d\n",Y1+i,yfs[i].n,yfs[i].x[0],
	yfs[i].x[1],yfs[i].x[2],yfs[i].x[3]);
fclose(fp);
}
