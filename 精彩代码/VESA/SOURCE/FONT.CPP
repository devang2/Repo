#include <stdio.h>
#include <dir.h>
#include <alloc.h>
#include <string.h>
#include <stdarg.h>

#include "\vga\xms.h"
#include "\vga\vgabase.h"
#include "\vga\vgafont.h"

#define MAXFONTHIGH 2880
#define MAXFONTWIDE 2880

int FONT::TEXT_X=0;
int FONT::TEXT_Y=0;

FONT::FONT(char *name,int whe,VGABASE *v)
{
FILE *fp;
struct ffblk fblk;

if(findfirst(name,&fblk,0)==-1)
	{
	where=inNONE;
	leng=0L;
	goto QUIT;
	}
leng=fblk.ff_fsize;

if(whe==inMEM)
	{
	long l;
	l=coreleft();
	if(leng>=l)
		whe=inXMS;
	if(leng>=0xffffL)
		whe=inXMS;
	}
if(whe==inXMS)
	{
	if(XMS::OK==0)
		whe=inHD;
	else
		{
		long l=0;
		l=(long)XMS::largestblock()-1L;
		l *= 1024L;
		if( leng>=l )
			whe=inHD;
		}
	}

if(whe==inMEM)
	{
	handle.mem=malloc((unsigned)leng);
	fp=fopen(name,"rb");
	fread(handle.mem,(unsigned)leng,1,fp);
	fclose(fp);
	}
else if(whe==inXMS)
	{
	long sagl=0x40ffL,saglc,offset,ll;
	int sagn,i;
	void *buf;

	ll=(leng+1023L)/1024L;
	handle.xms=new XMS((int)ll);
	buf=malloc((unsigned)sagl);
	sagn=(int)(leng/sagl);
	saglc=leng-sagl*(long)sagn;
	fp=fopen(name,"rb");
	offset=0L;
	for(i=0;i<sagn;i++)
		{
		fread(buf,(unsigned)sagl,1,fp);
		handle.xms->put((void *)offset,buf,sagl);
		offset+=sagl;
		}
	fread(buf,(unsigned)saglc,1,fp);
	handle.xms->put((void *)offset,buf,saglc);
	free(buf);
	fclose(fp);
	}
else
	{
	handle.file=fopen(name,"rb");
	whe=inHD;
	}
where=whe;

QUIT:
vga=v;
fonts[0]=fonts[1]=fonts[2]=0;
xi=0;
hollow=0;
if(where==inNONE)
	OK=0;
else
	OK=1;
}

FONT::~FONT()
{
if(where==inMEM)
	free(handle.mem);
else if(where==inXMS)
	delete handle.xms;
else if(where==inHD)
	fclose(handle.file);
}

void FONT::setsizeOnly(int hi,int wi,int xxi)
{
if(hi==0)
	high=org_high;
else if(hi>0&&hi<MAXFONTHIGH)
	high=hi;

if(wi==0)
	wide=org_wide;
else if( wi>0&&wi<MAXFONTWIDE )
	wide=wi;
xi=xxi;
}

void FONT::setsize(int hi,int wi,int xxi)
{
int i;
setsizeOnly(hi,wi);
for(i=0;i<3;i++)
	if(fonts[i]!=0&&fonts[i]!=this)
		fonts[i]->setsizeOnly(high,wide);
xi=xxi;
}

void FONT::join(FONT *ftp1,FONT *ftp2)
{
if(type!=ASCfont)
	{
	if(ftp1!=0)
		{
		fonts[ftp1->type]=ftp1;
		ftp1->setsizeOnly(high,wide);
		}
	if(ftp2!=0)
		{
		fonts[ftp2->type]=ftp2;
		ftp2->setsizeOnly(high,wide);
		}
	}
}

void FONT::cut(FONT *ftp)
{
int i;
for(i=0;i<3;i++)
	if(fonts[i]!=this&&fonts[i]==ftp)
		fonts[i]=0;
}

int FONT::gettexthigh()
{
if(OK==0) return(0);
return(high);
}

int FONT::gettextwide(char *string)
{
if(OK==0) return(0);

unsigned char *str;
int wide=0;
int i=0,n;

str=(unsigned char *)string;
n=strlen(str);
for(i=0;i<n;i++)
	{
	if(str[i]<0xa1)
		{
		if(fonts[0]!=0)
			wide+=fonts[0]->getcharwide(str[i]);
		}
	else if(str[i]<0xb0)
		{
		if(fonts[1]!=0)
			wide+=fonts[1]->getcharwide(str[i]);
		i++;
		}
	else
		{
		if(fonts[2]!=0)
			wide+=fonts[2]->getcharwide(str[i]);
		i++;
		}
	}
return(wide);
}

int FONT::getcharwide(unsigned char c)
{
if(OK==0) return(0);
unsigned char c1=c;
c=c1;
return(org_wide+xi);
}

void FONT::putxy(int x,int y)
{
if(x>=0&&x<vga->WIDE)
	TEXT_X=x;
if(y>=0&&y<vga->HIGH)
	TEXT_Y=y;
}

void FONT::getxy(int &x,int &y)
{
x=TEXT_X;
y=TEXT_Y;
}

void FONT::outtextxy(int x0,int y0,char *string)
{
if(OK==0) return;

unsigned char *str;
int i,n;

str=(unsigned char *)string;
n=strlen(str);
for(i=0;i<n;i++)
	{
	if(str[i]<0xa1)
		{
		if(fonts[0]!=0)
			x0+=fonts[0]->outchar(x0,y0,str+i);
		}
	else if(str[i]<0xb0)
		{
		if(fonts[1]!=0)
			x0+=fonts[1]->outchar(x0,y0,str+i);
		i++;
		}
	else
		{
		if(fonts[2]!=0)
			x0+=fonts[2]->outchar(x0,y0,str+i);
		i++;
		}
	if(x0>=vga->WIDE)
		{
		x0=0; y0+=high;
		}
	}
putxy(x0,y0);
}

void FONT::outtext(char *string)
{
outtextxy(TEXT_X,TEXT_Y,string);
}

void FONT::printf(char *fmt, ...)
{
char *string;
va_list argptr;

string=(char *)malloc(512);
va_start(argptr, fmt);
vsprintf(string, fmt, argptr);
va_end(argptr);
outtextxy(TEXT_X,TEXT_Y,string);
free(string);
}

void *FONT::getbuf(long offset,int ll)
{
void *data;
long dl=leng-offset;

if(OK==0||offset>=leng||dl<=0)
	return(0);

if(dl<(long)ll)
	ll=(int)dl;
if(where==inMEM)
	data=(char *)handle.mem+offset;
else
	{
	data=malloc(ll);
	if(where==inXMS)
		handle.xms->get(data,(void *)offset,ll);
	else if(where==inHD)
		{
		fseek(handle.file,offset,0);
		fread(data,ll,1,handle.file);
		}
	}
return(data);
}

void FONT::freebuf(void *buf)
{
if(where!=inMEM&&buf!=0)
	free(buf);
}

long FONT::getoffsetSmall(unsigned char *cp,int isize)
{
unsigned *idx,cc;
int beg,end,i,k,n;
long offset;

idx=(unsigned *)cp;
cc=*idx;
idx=(unsigned *)getbuf(0L,2);
n=*idx;
freebuf(idx);
idx=(unsigned *)getbuf(0,2*n);
beg=1;
end=n;
i=(n+1)/2;
k=0;
while(1) {
	if(end-beg<=1)
		{
		k++;
		if(k>2)
			{
			i=0;
			break;
			}
		}
	if(cc==idx[i])
		break;
	else if(cc>idx[i])
		{
		beg=i;
		i=(beg+end+1)/2;
		}
	else
		{
		end=i;
		i=(beg+end)/2;
		}
	}
freebuf(idx);

offset=2L+(long)n*2L+(long)isize*(long)i;
return(offset);
}


FONT_ASC::FONT_ASC(char *name,int whe,VGABASE *v) : FONT(name,whe,v)
{
fonts[0]=this;
type=ASCfont;
}

int FONT_ASC::gettextwide(char *string)
{
if(OK==0) return(0);
int n=strlen(string);
return(n*(org_wide+xi));
}

void FONT_ASC::outtextxy(int x0,int y0,char *string)
{
if(OK==0) return;

unsigned char *str;
int i;

str=(unsigned char *)string;
while((*str)!=0) {
	x0+=outchar(x0,y0,str);
	str++;
	if(x0>=vga->WIDE)
		{
		x0=0;
		y0+=high;
		}
	}
putxy(x0,y0);
}



ASC16::ASC16(char *name,int whe,VGABASE *v) : FONT_ASC(name,whe,v)
{
high=org_high=16;
wide=org_wide=8;
}

int ASC16::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

unsigned char c,*p,*data;
int i,j,x;
long l;

l=(long)(cp[0])*16L;
data=(unsigned char *)getbuf(l,16);
p=data;
for(i=0;i<16;i++)
	{
	c=*p;
	p++;
	x=x0;
	for(j=0;j<8;j++)
		{
		if(c&0x80)
			vga->putpixel(x,y0);
		x++;
		c<<=1;
		}
	y0++;
	}
freebuf(data);
return(org_wide+xi);
}

ASC24::ASC24(char *name,int whe,VGABASE *v) : FONT_ASC(name,whe,v)
{
high=org_high=24;
wide=org_wide=12;
}

int ASC24::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

unsigned char c,*p,*data;
int i,j,k,x;
long l;

l=(long)(cp[0])*48L;
data=(unsigned char *)getbuf(l,48);
p=data;
for(i=0;i<24;i++)
	{
	x=x0;
	for(j=0;j<2;j++)
		{
		c=*p;
		p++;
		for(k=0;k<8;k++)
			{
			if( c & 0x80 )
				vga->putpixel(x,y0);
			x++;
			if(x==12) break;
			c<<=1;
			}
		if(x==12) break;
		}
	y0++;
	}
freebuf(data);
return(org_wide+xi);
}

HZ16::HZ16(char *name,int whe,VGABASE *v) : FONT(name,whe,v)
{
high=org_high=16;
wide=org_wide=16;
fonts[2]=fonts[1]=this;
type=HZfont;
}

int HZ16::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

unsigned char c,*p,*data;
int i,j,k,x;

data=(unsigned char *)getdata(cp);
p=data;
for(i=0;i<16;i++)
	{
	x=x0;
	for(j=0;j<2;j++)
		{
		c=*p;
		p++;
		for(k=0;k<8;k++)
			{
			if( c & 0x80 )
				vga->putpixel(x,y0);
			x++;
			c<<=1;
			}
		}
	y0++;
	}
freebuf(data);
return(org_wide+xi);
}

void *HZ16::getdata(unsigned char *cp)
{
long offset;

offset=32L*(((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,32);
}

HZ24::HZ24(char *name,int whe,VGABASE *v) : FONT(name,whe,v)
{
high=org_high=24;
wide=org_wide=24;
fonts[2]=this;
type=HZfont;
}

int HZ24::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

unsigned char c,*p,*data;
int i,j,k,y;

data=(unsigned char *)getdata(cp);
p=data;
for(i=0;i<24;i++)
	{
	y=y0;
	for(j=0;j<3;j++)
		{
		c=*p;
		p++;
		for(k=0;k<8;k++)
			{
			if( c & 0x80 )
				vga->putpixel(x0,y);
			y++;
			c<<=1;
			}
		}
	x0++;
	}
freebuf(data);
return(org_wide+xi);
}

void *HZ24::getdata(unsigned char *cp)
{
long offset;

offset=72L*(((long)cp[0]-0xb0L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,72);
}

void *TX24::getdata(unsigned char *cp)
{
long offset;

offset=72L*(((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,72);
}


HZ32::HZ32(char *name,int whe,VGABASE *v) : FONT(name,whe,v)
{
high=org_high=32;
wide=org_wide=32;
fonts[2]=this;
type=HZfont;
}

int HZ32::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);

unsigned char c,*p,*data;
int i,j,k,x,y;

data=(unsigned char *)getdata(cp);
p=data;
x=x0;
for(j=0;j<32;j++)
	{
	c=*p;
	p++;
	y=y0;
	for(k=0;k<8;k++)
		{
		if( c & 0x80 )
			vga->putpixel(x,y);
		y++;
		c<<=1;
		}
	x++;
	}
x=x0;
for(i=0;i<32;i++)
	{
	y=y0+8;
	for(j=0;j<3;j++)
		{
		c=*p;
		p++;
		for(k=0;k<8;k++)
			{
			if( c & 0x80 )
				vga->putpixel(x,y);
			y++;
			c<<=1;
			}
		}
	x++;
	}
freebuf(data);
return(org_wide+xi);
}

void *HZ32::getdata(unsigned char *cp)
{
long offset;

offset=128L*(((long)cp[0]-0xb0L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,128);
}

void *TX32::getdata(unsigned char *cp)
{
long offset;

offset=128L*(((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,128);
}


HZ40::HZ40(char *name,int whe,VGABASE *v) : FONT(name,whe,v)
{
high=org_high=40;
wide=org_wide=36;
fonts[2]=this;
type=HZfont;
}

int HZ40::outchar(int x0,int y0,unsigned char *cp)
{
if(OK==0) return(0);
unsigned char c,*p,*data;
int i,j,k,x,y;

data=(unsigned char *)getdata(cp);
p=data;
x=x0;
for(i=0;i<36;i++)
	{
	y=y0;
	for(j=0;j<2;j++)
		{
		c=*p;
		p++;
		for(k=0;k<8;k++)
			{
			if( c & 0x80 )
				vga->putpixel(x,y);
			y++;
			c<<=1;
			}
		}
	x++;
	}
x=x0;
for(i=0;i<36;i++)
	{
	y=y0+16;
	for(j=0;j<3;j++)
		{
		c=*p;
		p++;
		for(k=0;k<8;k++)
			{
			if( c & 0x80 )
				vga->putpixel(x,y);
			y++;
			c<<=1;
			}
		}
	x++;
	}
freebuf(data);
return(org_wide+xi);
}

void *HZ40::getdata(unsigned char *cp)
{
long offset;

offset=180L*(((long)cp[0]-0xb0L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,180);
}

void *TX40::getdata(unsigned char *cp)
{
long offset;

offset=180L*(((long)cp[0]-0xa1L)*94L+(long)cp[1]-0xa1L);
return getbuf(offset,180);
}

