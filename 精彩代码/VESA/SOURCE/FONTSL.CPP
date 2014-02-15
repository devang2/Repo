#include <alloc.h>

#include "\vga\xms.h"
#include "\vga\vgabase.h"
#include "\vga\vgafont.h"
#include "\vga\fillbase.h"

ASCBC::ASCBC(char *name,int whe,VGABASE *v) : FONT_ASC(name,whe,v)
{
unsigned char *p;
p=(unsigned char *)getbuf(0x88L,2);
org_high=high=p[0];
org_wide=wide=p[0];
freebuf(p);
}

int ASCBC::gettextwide(char *string)
{
if(OK==0) return(0);

unsigned char *str,*p,bc,ec,c;
int cn,wi=0,off,n=0;
long l;

p=(unsigned char *)getbuf(0x80L,1024);
bc=p[4];
ec=p[4]+p[1]-1;
off=0x10+(int)p[1]*2-(int)p[4];
str=(unsigned char *)string;
c=*str;
while(c!=0) {
	if(c<bc||c>ec)
		wi=0;
	else
		{
		wi += p[off+c]+xi;
		n++;
		}
	str++;
	c=*str;
	}
freebuf(p);
l=wi*wide+org_wide/2;
wi=l/org_wide;
return(wi+n*xi);
}

int ASCBC::getcharwide(unsigned char c)
{
if(OK==0) return(0);

unsigned char *p,bc,ec;
int cn,wi=0,off;
long l;

p=(unsigned char *)getbuf(0x80L,1024);
bc=p[4];
ec=p[4]+p[1]-1;
if(c<bc||c>ec)
	wi=0;
else
	{
	off=0x10+(int)p[1]*2-(int)p[4];
	wi = p[off+(int)c];
	}
freebuf(p);
l=wi*wide+org_wide/2;
wi=l/org_wide;
return(wi+xi);
}

void *ASCBC::getdata(int &wi,unsigned char c)
{
unsigned char bc,*p;
int offset,k1,*kp;
long l;

p=(unsigned char *)getbuf(0x80L,1024);
if( c<p[4] || c>(p[4]+p[1]-1) )
	{
	freebuf(p);
	return(0);
	}
else
	{
	wi=p[ 0x10+(int)p[1]*2+(int)c-(int)p[4] ];
	kp=(int *)(p+5);
	offset=*kp+0x80;
	k1=0x10+(int)(c-p[4])*2;
	kp=(int *)(p+k1);
	offset += (*kp);
	freebuf(p);
	p=(unsigned char *)getbuf(offset,1024);
	return(p);
	}
}

int ASCBC::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

int wi,yf,yd,yd2,xf,xd,xd2;
unsigned char *data,c1,c2;
int x,y,i;
long l;

data=(unsigned char *)getdata(wi,cp[0]);
if(data==0)
	return(0);

yf=high; yd=org_high; yd2=yd/2;
xf=wide; xd=org_wide; xd2=xd/2;
y0+=high;
i=0;
while(1) {
	c1=data[i]; i++;
	c2=data[i]; i++;
	x=c1&0x7f;
	y=c2&0x7f;
	if( y>=64 ) y-=128;
	if( x>=64 ) x-=128;
	if( xf!=xd )
		{ l=x*xf+xd2; x=l/xd; }
	if( yf!=yd )
		{ l=y*yf+yd2; y=l/yd; }
	x+=x0;
	y=y0-y;
	if( (c1&0x80) && (c2&0x80) )
		vga->lineto(x,y);
	else if( (c1&0x80) && (c2&0x80)==0 )
		vga->moveto(x,y);
	else if( (c1&0x80)==0 && (c2&0x80)==0 )
		break;
	}
freebuf(data);
l=wi*xf+xd2;
wi=l/xd;
return(wi+xi);
}


ASCSL::ASCSL(char *name,int whe,VGABASE *v) : FONT_ASC(name,whe,v)
{
org_high=high=96;
org_wide=wide=96;
}

int ASCSL::gettextwide(char *string)
{
if(OK==0) return(0);

unsigned char *p,*str,c;
int wi=0,off,n=0;
long l;

p=(unsigned char *)getbuf(160L,128);
str=(unsigned char *)string;
c=*str;
while(c!=0) {
	if(c>=32&&c<127)
		{
		wi+=p[c];
		n++;
		}
	str++;
	c=*str;
	}
freebuf(p);
l=(long)wi*wide+org_wide/2;
wi=l/org_wide;
return(wi+xi*n);
}


int ASCSL::getcharwide(unsigned char c)
{
if(OK==0) return(0);

unsigned char *p;
int wi,off;
long l;

if(c<32||c>127)
	return(0);

off=192+(int)(c-32);
p=(unsigned char *)getbuf(off,2);
wi=p[0];
freebuf(p);
l=(long)wi*wide+org_wide/2;
wi=l/org_wide;
return(wi+xi);
}

int ASCSL::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

long yf,yd,yd2,xf,xd,xd2;
unsigned char *data;
int i,j,*xy,n,wi;
long l;

data=(unsigned char *)getdata(wi,cp[0]);
if(data==0)
	return(0);

xy=(int *)calloc(512,2);
yf=high; yd=org_high; yd2=yd/2;
xf=wide; xd=org_wide; xd2=xd/2;

i=0;
while(1) {
	xy[i]=n=(int)data[i];
	i++;
	if(n==0)
		break;
	for(j=0;j<n;j++)
		{
		l=(long)data[i]*xf+xd2;
		xy[i]=l/xd; xy[i]+=x0; i++;
		l=(long)data[i]*yf+yd2;
		xy[i]=l/yd; xy[i]+=y0; i++;
		}
	}
if( hollow==0 )
	vga->polyfill(xy);
vga->poly(xy);
freebuf(data);
free(xy);
l=(long)wi*(long)xf+xd2;
wi=l/xd;
return(wi+xi);
}

void *ASCSL::getdata(int &wi,unsigned char c)
{
unsigned char *p;
int off,i;
if(c<32||c>=127)
	return(0);

p=(unsigned char *)getbuf(0L,288);
i=((int)c-32)*2;
off=(int)p[i]+(int)p[i+1]*256;
i=160+(int)c;
wi=p[i];
freebuf(p);
return getbuf((long)off,512);
}

HZUC::HZUC(char *name,int whe,VGABASE *v) : FONT(name,whe,v)
{
high=org_high=96;
wide=org_wide=96;
fonts[2]=this;
type=HZfont;
}

int HZUC::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

int *xy,n,*xy1;

xy=getdata(cp);
if(xy==0)
	return(0);
enlarge(x0,y0,xy);

if( hollow==0 )
	{
	xy1=xy;
	while(1) {
		n=*xy1;
		xy1++;
		if(n==0)
			break;
		vga->polyfill(n,xy1);
		xy1 += (n+n);
		}
	}
vga->poly(xy);
free(xy);
return(wide+xi);
}


int *HZUC::getdata(unsigned char *cp)
{
long *lp,offset,doff;
int *ip,le;
unsigned char *p;
int *xy;

offset=getoffsetUC(cp);
p=(unsigned char *)getbuf(offset,6);
lp=(long *)p;
doff=lp[0];
ip=(int *)p;
le=ip[2];
freebuf(p);
p=(unsigned char *)getbuf(doff,le);
return translate(le,p);
}

long HZUC::getoffsetUC(unsigned char *cp)
{
long offset=6L*(((long)cp[0]-0xb0L)*94L+(long)cp[1]-0xa1L);
return(offset);
}

int *HZUC::translate(int n,unsigned char *bh)
{
unsigned char fc,kc;
int i,j,k,fn;
int pn,*xy;

if(bh==0)
	return(0);

xy=(int *)calloc((n*2+32),2);
i=j=0;
pn=0;
while(i<n) {
	fc=bh[i]>>4;
	if(fc>=12)
		{
		pn=j;
		j++;
		xy[j]=(bh[i]-0xC0)*2+(bh[i+1]>>7);
		i++; j++;
		xy[j]=bh[i]&0x7f;
		i++; j++;
		xy[pn]=1;
		}
	else if(fc==11)
		{
		i++;
		if( bh[i]>=0x80 )
			xy[j]=xy[j-2]-bh[i]+0x80;
		else
			xy[j]=xy[j-2]+bh[i];
		i++; j++;
		if( bh[i]>=0x80 )
			xy[j]=xy[j-2]-bh[i]+0x80;
		else
			xy[j]=xy[j-2]+bh[i];
		i++; j++;
		xy[pn]++;
		}
	else if(fc==9)
		{
		kc=bh[i]&0x0F;
		if( kc>=0x08 )
			xy[j+1]=xy[j-1]-kc+0x08;
		else
			xy[j+1]=xy[j-1]+kc;
		i++;
		if( bh[i]>=0x80 )
			xy[j]=xy[j-2]-bh[i]+0x80;
		else
			xy[j]=xy[j-2]+bh[i];
		i++; j+=2;
		xy[pn]++;
		}
	else if(fc==8)
		{
		kc=bh[i]&0x0F;
		if( kc>=8 )
			xy[j]=xy[j-2]-kc+8;
		else
			xy[j]=xy[j-2]+kc;
		i++; j++;
		if( bh[i]>=0x80 )
			xy[j]=xy[j-2]-bh[i]+0x80;
		else
			xy[j]=xy[j-2]+bh[i];
		i++; j++;
		xy[pn]++;
		}
	else if(fc<=3)
		{
		fn=bh[i];
		i++;
		for(k=0;k<fn;k++)
			{
			kc=bh[i]>>4;
			if(kc>=8)
				xy[j]=xy[j-2]-kc+8;
			else
				xy[j]=xy[j-2]+kc;
			j++;
			kc=bh[i]&0x0F;
			if(kc>=8)
				xy[j]=xy[j-2]-kc+8;
			else
				xy[j]=xy[j-2]+kc;
			i++; j++;
			xy[pn]++;
			}
		}
	else if(fc==4)
		{
		fn=bh[i]&0x0F;
		i++;
		for(k=0;k<fn;k++)
			{
			xy[j]=xy[j-2]+(bh[i]>>4);
			j++;
			xy[j]=xy[j-2]+(bh[i]&0x0F);
			i++; j++;
			xy[pn]++;
			}
		}
	else if(fc==5)
		{
		fn=bh[i]&0x0F;
		i++;
		for(k=0;k<fn;k++)
			{
			xy[j]=xy[j-2]-(bh[i]>>4);
			j++;
			xy[j]=xy[j-2]+(bh[i]&0x0F);
			i++; j++;
			xy[pn]++;
			}
		}
	else if(fc==6)
		{
		fn=bh[i]&0x0F;
		i++;
		for(k=0;k<fn;k++)
			{
			xy[j]=xy[j-2]-(bh[i]>>4);
			j++;
			xy[j]=xy[j-2]-(bh[i]&0x0F);
			i++; j++;
			xy[pn]++;
			}
		}
	else if(fc==7)
		{
		fn=bh[i]&0x0F;
		i++;
		for(k=0;k<fn;k++)
			{
			xy[j]=xy[j-2]+(bh[i]>>4);
			j++;
			xy[j]=xy[j-2]-(bh[i]&0x0F);
			i++; j++;
			xy[pn]++;
			}
		}
	else
		i++;
	}
freebuf(bh);
return(xy);
}

void HZUC::enlarge(int x0,int y0,int *xy)
{
int i=0,j,n;
long yf,yd,yd2,xf,xd,xd2,l;

yf=high; yd=org_high; yd2=yd/2;
xf=wide; xd=org_wide; xd2=xd/2;
while(1) {
	n=xy[i];
	i++;
	if(n<=0)
		break;
	for(j=0;j<n;j++)
		{
		l=(long)xy[i]*xf+xd2;
		xy[i]=l/xd; xy[i]+=x0; i++;
		l=(long)xy[i]*yf+yd2;
		xy[i]=l/yd; xy[i]+=y0; i++;
		}
	}
}

int inclu(int *xy,int *bxy)
{
if(xy[0]<=bxy[0]&&xy[1]<=bxy[1] && xy[2]>=bxy[2]&&xy[3]>=bxy[3] )
	return(1);
if( xy[0]>=bxy[0]&&xy[1]>=bxy[1] && xy[2]<=bxy[2]&&xy[3]<=bxy[3] )
	return(1);
return(0);
}

int HZUCK::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

int i,j,*xy,n;

xy=getdata(cp);
if(xy==0)
	return(0);
enlarge(x0,y0,xy);

if( hollow==0 )
	{
	int **bxy,*xy1,nn=0;
	bxy=(int **)calloc(100,sizeof(void *));
	for(i=0;i<100;i++)
		{
		bxy[i]=(int *)calloc(4,2);
		bxy[i][0]=bxy[i][1]=10000;
		}
	xy1=xy;
	while(1) {
		n=*xy1;
		xy1++;
		if(n==0)
			break;
		j=0;
		for(i=0;i<n;i++)
			{
			if(xy1[j]<bxy[nn][0]) bxy[nn][0]=xy1[j];
			if(xy1[j]>bxy[nn][2]) bxy[nn][2]=xy1[j];
			j++;
			if(xy1[j]<bxy[nn][1]) bxy[nn][1]=xy1[j];
			if(xy1[j]>bxy[nn][3]) bxy[nn][3]=xy1[j];
			j++;
			}
		nn++;
		xy1 += (n+n);
		}

	xy1=xy;
	i=0;
	while(1) {
		n=*xy1;
		xy1++;
		if(n==0)
			break;
		for(j=i+1;j<nn;j++)
			if( inclu(bxy[i],bxy[j]) )
				break;
		if( j==nn )
			vga->polyfill(n,xy1);
		else
			{
			xy1--;
			vga->polyfill(xy1);
			break;
			}
		xy1 += (n+n);
		i++;
		}
	for(i=0;i<100;i++)
		free(bxy[i]);
	free(bxy);
	}
vga->poly(xy);
free(xy);
return(wide+xi);
}

TXUC::TXUC(char *name,int whe,VGABASE *v) : HZUCK(name,whe,v)
{
fonts[1]=this;
high=org_high=128;
wide=org_wide=128;
type=TXfont;
}

long TXUC::getoffsetUC(unsigned char *cp)
{
long offset=6L*(((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L);
return(offset);
}

/**************************************
int HZUCK::outchar(int x0,int y0,unsigned char *cp)
{
int *xy;

xy=getdata(cp);
if(xy==0)
	return(0);
enlarge(x0,y0,xy);
vga->poly(xy);
if( hollow==0 )
	vga->polyfill(xy);
free(xy);
return(wide+xi);
}

int HZUCKoutchar(int x0,int y0,unsigned char *cp)
{
int i,j,*xy,n;
LFILL *lfp1,*lfp2;

xy=getdata(cp);
if(xy==0)
	return(0);
enlarge(x0,y0,xy);

vga->poly(xy);

i=0;
lfp1=new LFILL(vga);
n=xy[i]; i++;
lfp1->poly(n,xy+i);
i += (n*2);
while(1) {
	n=xy[i]; i++;
	if(n<=0)
		break;
	lfp2=new LFILL(vga);
	lfp2->poly(n,xy+i);
	i += (n*2);
	if( lfp1->ifincl(lfp2) || lfp2->ifincl(lfp1) )
		{
		lfp1->add(lfp2);
		delete lfp2;
		}
	else
		{
		lfp1->draw();
		delete lfp1;
		lfp1=lfp2;
		}
	}
lfp1->draw();
delete lfp1;

free(xy);
return(wide+xi);
}
*********************************************/
