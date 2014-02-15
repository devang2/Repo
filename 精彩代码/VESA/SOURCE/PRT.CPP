#include <alloc.h>
#include <stdio.h>
#include <mem.h>
#include <bios.h>
#include <stdlib.h>

#include "\vga\vgabase.h"
#include "\vga\xms.h"
#include "\vga\vgaprt.h"

prt_buf::prt_buf(int wi,int hi,int whe)
{
long size,l;
int i;

WIDE=wi;
HIGH=hi;
widebyte=(wi+7)/8;
if( widebyte&0x01 )
	widebyte++;
size=(long)widebyte*(long)hi;

if(whe==inMEM)
	{
	l=coreleft();
	if(size>=(l-0x10000L))
		whe=inXMS;
	}
if(whe==inXMS)
	{
	if(XMS::OK==0)
		whe=inHD;
	else
		{
		l=(long)XMS::largestblock()-1L;
		l *= 1024L;
		if( size>=l )
			whe=inHD;
		}
	}
where=whe;

if(where==inMEM)
	{
	unsigned char **pp;
	pp=(unsigned char **)calloc(hi,sizeof(void *));
	for(i=0;i<hi;i++)
		pp[i]=(unchar *)calloc(widebyte,1);
	handle.mem=pp;
	buffer=pp[0];
	}
else if(where==inXMS)
	{
	l=(size+1023L)/1024L;
	handle.xms=new XMS((int)l);
	buffer=(unchar *)calloc(widebyte,1);
	l=0;
	for(i=0;i<hi;i++)
		{
		handle.xms->put((void *)l,buffer,widebyte);
		l += (long)widebyte;
		}
	}
else if(where==inHD)
	{
	FILE *fp;
	fp=fopen("prt_buf.img","w+b");
	buffer=(unchar *)calloc(widebyte,1);
	for(i=0;i<hi;i++)
		fwrite(buffer,1,widebyte,fp);
	handle.file=fp;
	}
else
	buffer=(unchar *)calloc(widebyte,1);
bufy=0;
setcolor(1);
VESAmodeNo=-1;
if(where==inNONE) OK=0;
else OK=1;
}

prt_buf::~prt_buf()
{
int i;
if( where==inMEM )
	{
	for(i=0;i<HIGH;i++)
		free(handle.mem[i]);
	free(handle.mem);
	}
else
	free(buffer);
if( where==inXMS )
	delete handle.xms;
if( where==inHD )
	fclose(handle.file);
}

void prt_buf::setcolor(unchar col)
{
CUR_COLOR.dword=0L;
if(col>0)
	CUR_COLOR.byte=1;
}

COLOR prt_buf::setcolorto(unchar col)
{
COLOR color={0};
if(col>0)
	color.byte=1;
return(color);
}

void prt_buf::putpixel(int x,int y)
{
if(x<0||x>=WIDE||y<0||y>=HIGH)
	return;

int byte,bit;
unsigned char c=0x80;
byte=x>>3;
bit=x&7;
c >>= bit;
getbuffer(y);
if( CUR_COLOR.byte )
	buffer[byte] |= c;
else
	buffer[byte] &= (~c);
}

COLOR prt_buf::getpixel(int x,int y)
{
COLOR color={0L};
if(x<0||x>=WIDE||y<0||y>=HIGH)
	return(color);

int byte,bit;
unsigned char c;
byte=x>>3;
bit=x&7;
c >>= bit;
getbuffer(y);
c=buffer[byte]<<bit;
color.byte=(c>>7);
return(color);
}

void prt_buf::scanline(int x1,int x2,int y)
{
if(y<0||y>=HIGH||x1>=WIDE||x2<0)
	return;
if(x1<0) x1=0;
if(x2>=WIDE) x2=WIDE-1;

int byte1,byte2,bit1,bit2,i;
unsigned char c1=0xff,c2=0xff;

if(x1>x2)
	{ i=x1; x1=x2; x2=i; }
getbuffer(y);
byte1=x1>>3;
byte2=x2>>3;
bit1=x1&7;
bit2=7-(x2&7);

if( byte2>byte1 )
	{
	c1 >>= bit1;
	if( CUR_COLOR.byte )
		{
		buffer[byte1] |= c1;
		c1=0xff;
		}
	else
		{
		buffer[byte1] &= (~c1);
		c1=0;
		}

	for(i=byte1+1;i<byte2;i++)
		buffer[i]=c1;

	c2 <<= bit2;
	if( CUR_COLOR.byte )
		buffer[byte2] |= c2;
	else
		buffer[byte2] &= (~c2);
	}
else
	{
	c1 >>= bit1;
	c2 <<= bit2;
	c1 = c1&c2;
	if( CUR_COLOR.byte )
		buffer[byte1] |= c1;
	else
		buffer[byte1] &= (~c1);
	}
}

void prt_buf::cls0()
{
unsigned char *buf;
int i;
buf=(unsigned char *)calloc(widebyte,1);
for(i=0;i<HIGH;i++)
	{
	getbuffer(i);
	memcpy(buffer,buf,widebyte);
	}
free(buf);
}

void prt_buf::cls()
{
unsigned char *buf;
int i;
buf=(unsigned char *)calloc(widebyte,1);
if( CUR_COLOR.byte==1 )
	for(i=0;i<widebyte;i++)
		buf[i]=0xff;
for(i=0;i<HIGH;i++)
	{
	getbuffer(i);
	memcpy(buffer,buf,widebyte);
	}
}

void prt_buf::getscanline(int x1,int y1,int n,void *buf)
{
if(y1<0||y1>=HIGH||x1>=WIDE)
	return;

int byte1,byte2,tol,tor;
int x2,i,p=0;
unsigned char c,*cp;

x2=x1+n-1;
if(x1<0) x1=0;
if(x2>=WIDE) x2=WIDE-1;

getbuffer(y1);
cp=(unsigned char *)buf;
byte1=x1>>3;
byte2=x2>>3;
tol=x1&7;
tor=8-tol;
for(i=byte1;i<=byte2;i++)
	{
	c=buffer[i];
	cp[p] |= (c>>tor);
	p++;
	cp[p]=(c<<tol);
	}
}

void prt_buf::putscanline(int x1,int y1,int n,void *buf)
{
int byte1,byte2,tol,tor,bit1,bit2,i,p=1,x2;
unsigned char c1,c2,*cp;

getbuffer(y1);
cp=(unsigned char *)buf;
x2=x1+n-1;
byte1=x1>>3;
byte2=x2>>3;
if( byte2>byte1 )
	{
	tor=x1&7;
	tol=8-tor;

	c1 = (cp[p]>>tor);
	c2 = 0xff;
	c2 <<= tol;
	buffer[byte1] &= c2;
	buffer[byte1] |= c1;

	for(i=byte1+1;i<byte2;i++)
		{
		c1=(cp[p]<<tol);
		buffer[i]=c1;
		p++;
		c1=(cp[p]>>tor);
		buffer[i]|=c1;
		}

	c1=(cp[p]<<tol);
	p++;
	c2=(cp[p]>>tor);
	c1=c1|c2;
	tor=x2&7;
	tol=7-tor;
	c2=0xff;
	c2 <<= tol;
	c1=c1&c2;
	c2=0xff;
	c2 >>=tor;
	buffer[byte2] &= c2;
	buffer[byte2] |= c1;
	}
else
	{
	c1=c2=0xff;
	tol=7-(x1&7);
	c1 <<= tol;
	tor=x2&7;
	c2 >>= tor;
	c1=c1|c2;
	buffer[byte1] &= c1;

	c1=~c1;
	tor=x1&7;
	c2=(cp[p]>>tor);
	c1=c1&c2;
	buffer[byte1] |= c1;
	}
}

int prt_buf::scanlinesize(int x1,int x2)
{
int k=(x2-x1)/8 + 4;
if( k&1 )
	k++;
return(k);
}

void prt_buf::getbuffer(int y)
{
if(y==bufy)
	return ;

long off;

if( where==inMEM )
	buffer=handle.mem[y];
else if( where==inXMS )
	{
	off=(long)bufy*(long)widebyte;
	handle.xms->put((void *)off,buffer,widebyte);
	off=(long)y*(long)widebyte;
	handle.xms->get(buffer,(void *)off,widebyte);
	}
else if( where==inHD )
	{
	off=(long)bufy*(long)widebyte;
	fseek(handle.file,off,0);
	fwrite(buffer,1,widebyte,handle.file);
	off=(long)y*(long)widebyte;
	fseek(handle.file,off,0);
	fread(buffer,1,widebyte,handle.file);
	}
bufy=y;
}


PRINTER::PRINTER(VGABASE *v)
{
vga=v;
}

unchar *PRINTER::getprtdata(int y)
{
if(y<0||y>=vga->HIGH)
	return(0);

if( vga->VESAmodeNo==-1 )
	{
	prt_buf *p;
	p=(prt_buf *)vga;
	p->getbuffer(y);
	return(p->buffer);
	}
else
	{
	static unsigned char *buf=0;
	unsigned char c;
	int bn,k,wi,x;

	wi=vga->WIDE;
	if( buf!=0 )
		free(buf);
	buf=(unsigned char *)calloc(byten,1);
	bn=k=0;
	c=0x80;
	for(x=0;x<wi;x++)
		{
		if( vga->getpixel(x,y).dword )
			buf[bn] |= c;
		k++;
		c >>= 1;
		if( k>=8 )
			{
			k=0;
			c=0x80;
			bn++;
			}
		}
	return(buf);
	}
}

void PRINTER::outcode(unsigned char *code)
{
int i=0;
while( code[i]!=0 ) {
	biosprint(0,code[i],0);
	i++;
	}
}

void PRINTER::nextrow()
{
biosprint(0,0x0a,0);
biosprint(0,0x0d,0);
}

void HP_PRINTER::print()
{
int i,j,yn=vga->HIGH;
unsigned char *buf;

byten=vga->WIDE/8;
setdx();
setdpi();
setgz();
for(i=0;i<yn;i++)
	{
	buf=getprtdata(i);
	setbytenum(byten);
	for(j=0;j<byten;j++)
		biosprint(0,buf[j],0);
	}
end();
}

void HP_PRINTER::outpaper()
{
outcode("\x1b\x26\x6c\x30\x48\0");
}

void HP_PRINTER::setdx()
{
outcode("\x1b\x26\x6b\x30\x57");
}

void HP_PRINTER::setgz()
{
outcode("\x1b\x2a\x72\x32\x51");
}

void HP_PRINTER::setbytenum(int n)
{
int i,k;
unsigned char *str;
str=(unsigned char *)calloc(10,1);
itoa(n,str,10);
biosprint(0,0x1b,0);
biosprint(0,0x2a,0);
biosprint(0,0x62,0);
i=0;
while(str[i]!=0) {
	biosprint(0,str[i],0);
	i++;
	}
biosprint(0,0x57,0);
free(str);
}

void HP_PRINTER::end()
{
outcode("\x1b\x2a\x72\x62\x43");
}

void HP_600::setdpi()
{
outcode("\x1b\x2a\x74\x36\x30\x30\x52");
}

void HP_300::setdpi()
{
outcode("\x1b\x2a\x74\x33\x30\x30\x52");
}

void HP_150::setdpi()
{
outcode("\x1b\x2a\x74\x31\x35\x30\x52");
}

void HP_100::setdpi()
{
outcode("\x1b\x2a\x74\x37\x35\x52");
}

void HP_75::setdpi()
{
outcode("\x1b\x2a\x74\x33\x30\x30\x52");
}


void EPSON_PRINTER::outpaper(void)
{
biosprint(0,0x0c,0);
}

void EPSON_PRINTER::init()
{
outcode("\x1b\x40\x1b\x3c");
}

void EPSON_PRINTER::setdx()
{
outcode("\x1b\x55\x1");
}

void EPSON_PRINTER::setjg180(unsigned char c180)
{
biosprint(0,0x1b,0);
biosprint(0,0x33,0);
biosprint(0,c180,0);
}

void EPSON_PRINTER::print()
{
unsigned char **data,*buf,c1,c2;
int yi,i,j,yn,xn,y,bn,k1,k2;
int rowd=dpi*2/15;
int rowb=dpi/60;

byten=vga->WIDE/rowb;
init();
setjg180(24);
setdx();
xn=vga->WIDE;
yn=(vga->HIGH+rowd-1)/rowd;
data=(unchar **)calloc(rowb,sizeof(void *));
for(i=0;i<rowb;i++)
	data[i]=(unchar *)calloc(xn,1);
y=0;
for(yi=0;yi<yn;yi++)
	{
	c1=0x80;
	bn=k1=0;
	for(i=0;i<rowb;i++)
		for(j=0;j<xn;j++)
			data[i][j]=0;
	for(i=0;i<rowd;i++)
		{
		buf=getprtdata(y);
		y++;
		if(buf==0)
			break;
		c2=*buf;
		k2=0;
		for(j=0;j<xn;j++)
			{
			if( c2&0x80 )
				data[bn][j] |= c1;
			k2++;
			c2 <<=1;
			if( k2>=8 )
				{
				buf++;
				c2=*buf;
				k2=0;
				}
			}
		k1++;
		c1 >>= 1;
		if( k1>=8 )
			{
			c1=0x80;
			bn++;
			k1=0;
			}
		}
	setmode(xn);
	for(i=0;i<xn;i++)
		for(j=0;j<rowb;j++)
			biosprint(0,data[j][i],0);
	nextrow();
	}
setjg180(30);
for(i=0;i<rowb;i++)
	free(data[i]);
free(data);
}

void EPSON_PRINTER::setmodeb(int pxn,unchar *code)
{
int i;
i=pxn>>8;
code[4]=i;
i=pxn&0xff;
code[3]=i;
for(i=0;i<5;i++)
	biosprint(0,code[i],0);
}


void EPSON_360::setmode(int pxn)
{
unsigned char code[6]={0x1b,0x2a,72,0,0,0};
setmodeb(pxn,code);
}

void EPSON_180::setmode(int pxn)
{
unsigned char code[6]={0x1b,0x2a,39,0,0,0};
setmodeb(pxn,code);
}

void EPSON_60::setmode(int pxn)
{
unchar code[6]={0x1b,0x2a,0,0,0,0};
setmodeb(pxn,code);
}
