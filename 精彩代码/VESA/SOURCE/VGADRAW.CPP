#include <stdio.h>
#include <string.h>
#include <alloc.h>
#include <math.h>

#include "\vga\xms.h"
#include "\vga\vgabase.h""

static long SINV[91]={0,
	17452406,	34899496,	52335956,	69756473,	87155742,	104528463,
	121869343,	139173100,	156434465,	173648177,	190808995,	207911690,
	224951054,	241921895,	258819045,	275637355,	292371704,	309016994,
	325568154,	342020143,	358367949,	374606593,	390731128,	406736643,
	422618261,	438371146,	453990499,	469471562,	484809620,	500000000,
	515038074,	529919264,	544639035,	559192903,	573576436,	587785252,
	601815023,	615661475,	629320391,	642787609,	656059028,	669130606,
	681998360,	694658370,	707106781,	719339800,	731353701,	743144825,
	754709580,	766044443,	777145961,	788010753,	798635510,	809016994,
	819152044,	829037572,	838670567,	848048096,	857167300,	866025403,
	874619707,	882947592,	891006524,	898794046,	906307787,	913545457,
	920504853,	927183854,	933580426,	939692620,	945518575,	951056516,
	956304755,	961261695,	965925826,	970295726,	974370064,	978147600,
	981627183,	984807753,	987688340,	990268068,	992546151,	994521895,
	996194698,	997564050,	998629534,	999390827,	999847695,	1000000000};

double Lsin(int angle)
{
double v,f;

if(angle>360||angle<-360)
	angle=angle-(angle/360)*360;
if(angle<0)
	angle=360+angle;

if(angle>180) f=-1;
else f=1;

if(angle>90&&angle<=180)
	angle=180-angle;
else if(angle>180&&angle<=270)
	angle=angle-180;
else if(angle>270)
	angle=360-angle;
v=f*SINV[angle]/10.0e8;
return(v);
}

double Lcos(int angle)
{
double v,f;

if(angle>360||angle<-360)
	angle=angle-(angle/360)*360;
if(angle<0)
	angle=360+angle;

if(angle<270&&angle>90) f=-1;
else f=1;

if(angle>90&&angle<=180)
	angle=180-angle;
else if(angle>180&&angle<=270)
	angle=angle-180;
else if(angle>270)
	angle=360-angle;
angle=90-angle;
v=f*SINV[angle]/10.0e8;
return(v);
}

static double SIN45=0.707106781186548;

void VGABASE::line(int x1,int y1,int x2,int y2)
{
int i,p,n,x,y,tn;

CUR_X=x2;
CUR_Y=y2;
if( y1==y2 )
	{
	if(x1>x2)
		{ x=x2; x2=x1; x1=x; }
	scanline(x1,x2,y1);
	return;
	}
if( x1==x2 )
	{
	if(y1>y2)
		{ y=y2; y2=y1; y1=y; }
	for(i=y1;i<=y2;i++)
		putpixel(x1,i);
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
		while(x1<=x2) {
			if(tn>=0) tn-=p;
			else { tn+=n; y1++; }
			putpixel(x1,y1);
			x1++;
			}
		}
	else
		{
		x=x2-x1; y=y2-y1;
		p=-2*y; n=2*x+2*y; tn=x;
		while(x1<=x2) {
			if(tn>=0) tn-=p;
			else { tn+=n; y1--; }
			putpixel(x1,y1);
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
		while(x1<=x2) {
			if(tn>=0) tn-=p;
			else { tn+=n; y1++; }
			putpixel(y1,x1);
			x1++;
			}
		}
	else
		{
		x=x2-x1; y=y2-y1;
		p=-2*y; n=2*x+2*y; tn=x;
		while(x1<=x2) {
			if(tn>=0) tn-=p;
			else { tn+=n; y1--; }
			putpixel(y1,x1);
			x1++;
			}
		}
	}
}

void VGABASE::moveto(int x,int y)
{
CUR_X=x;
CUR_Y=y;
}

void VGABASE::lineto(int x,int y)
{
line(CUR_X,CUR_Y,x,y);
}

void VGABASE::poly(int n,int *border)
{
int i,nn;
nn=n*2;
line(border[nn-2],border[nn-1],border[0],border[1]);
for(i=2;i<nn;i+=2)
	lineto(border[i],border[i+1]);
}

void VGABASE::poly(int *border)
{
int i=0,n;
while(1) {
	n=border[i];i++;
	if(n==0)
		break;
	poly(n,border+i);
	i += (2*n);
	}
}

void VGABASE::rectangle(int x1,int y1,int x2,int y2)
{
line(x1,y1,x2,y1);
line(x2,y1,x2,y2);
line(x1,y2,x2,y2);
line(x1,y1,x1,y2);
}

void VGABASE::circle(int x0,int y0,int r)
{
int i;
long tn;
int x,y;
int xmax;

y=r; x=0;
xmax=(double)r*SIN45;
tn=(1-r*2);
while(x<=xmax) {
	if(tn>=0)
		{
		tn += ( 6 + ((x-y)<<2) );
		y--;
		}
	else
		tn += ( (x<<2) + 2 );
	putpixel(x0+y,y0+x); /* 7 */
	putpixel(x0+x,y0+y); /* 6 */
	putpixel(x0-x,y0+y); /* 5 */
	putpixel(x0-y,y0+x); /* 4 */
	putpixel(x0-y,y0-x); /* 3 */
	putpixel(x0-x,y0-y); /* 2 */
	putpixel(x0+x,y0-y); /* 1 */
	putpixel(x0+y,y0-x); /* 0 */
	x++;
	}
putpixel(x0+y,y0+x); /* 7 */
putpixel(x0+x,y0+y); /* 6 */
putpixel(x0-x,y0+y); /* 5 */
putpixel(x0-y,y0+x); /* 4 */
putpixel(x0-y,y0-x); /* 3 */
putpixel(x0-x,y0-y); /* 2 */
putpixel(x0+x,y0-y); /* 1 */
putpixel(x0+y,y0-x); /* 0 */
}

void VGABASE::sector(int x0,int y0,int r,int stangle,int endangle)
{
int i,j;
int *xy;
int bx,ex,bxd,exd,bxf,exf,ben;
long tn,x,y;
long xmax;

y=r; x=0;
xmax=(double)r*SIN45;
tn=(1-r*2);

xy=(int *)calloc(20,2);
xy[0]=x0+r; xy[1]=y0;
xy[2]=x0; xy[3]=y0-r;
xy[4]=x0; xy[5]=y0-r;
xy[6]=x0-r; xy[7]=y0;
xy[8]=x0-r; xy[9]=y0;
xy[10]=x0; xy[11]=y0+r;
xy[12]=x0; xy[13]=y0+r;
xy[14]=x0+r; xy[15]=y0;

bx=stangle/45;
ex=endangle/45;
ben=ex-bx-1;
xy[16]=(double)r*Lcos(stangle);
xy[17]=(double)r*Lsin(stangle);
xy[18]=(double)r*Lcos(endangle);
xy[19]=(double)r*Lsin(endangle);
line(x0+xy[16],y0-xy[17],x0,y0);
line(x0+xy[18],y0-xy[19],x0,y0);
if(bx==1||bx==2||bx==5||bx==6)
	bxd=abs(xy[16]);
else
	bxd=abs(xy[17]);
if(ex==1||ex==2||ex==5||ex==6)
	exd=abs(xy[18]);
else
	exd=abs(xy[19]);
if(bx==0||bx==2||bx==4||bx==6)
	bxf=0;
else
	bxf=1;
if(ex==0||ex==2||ex==4||ex==6)
	exf=1;
else
	exf=0;

while(x<=xmax) {
	if(tn>=0)
		{
		tn += ( 6 + ((x-y)*4) );
		y--;
		xy[0]--;
		xy[3]++;
		xy[5]++;
		xy[6]++;
		xy[8]++;
		xy[11]--;
		xy[13]--;
		xy[14]--;
		}
	else
		tn += ( (x*4) + 2 );
	if(stangle<endangle)
		{
		j=(bx+1)*2;
		for(i=0;i<ben;i++)
			{
			putpixel(xy[j],xy[j+1]);
			j+=2;
			}
		}
	else if(stangle>endangle)
		{
		j=(bx+1)*2;
		for(i=bx+1;i<8;i++)
			{
			putpixel(xy[j],xy[j+1]);
			j+=2;
			}
		j=0;
		for(i=0;i<ex;i++)
			{
			putpixel(xy[j],xy[j+1]);
			j+=2;
			}
		}
	i=bx*2;
	if( (x>bxd)^bxf )
		putpixel(xy[i],xy[i+1]);
	i=ex*2;
	if( (x>exd)^exf )
		putpixel(xy[i],xy[i+1]);
	x++;
	xy[1]--;
	xy[2]++;
	xy[4]--;
	xy[7]--;
	xy[9]++;
	xy[10]--;
	xy[12]++;
	xy[15]++;
	}
free(xy);
}

void VGABASE::ellipse(int x0,int y0,long r1,long r2)
{
long r,r12,r22;
int x,y,xmax;
long tn;

x=0;y=r2;
r12=r1*r1;r22=r2*r2;
xmax=(double)r12/sqrt(r12+r22);
tn=r12-2*r2*r12;
while(x<=xmax) {
	if(tn<0||y==0)
		tn+=(4*x+2)*r22;
	else
		{
		tn+=(4*x+2)*r22+(1-y)*4*r12;
		y--;
		}
	putpixel(x0+x,y0+y);
	putpixel(x0-x,y0+y);
	putpixel(x0+x,y0-y);
	putpixel(x0-x,y0-y);
	x++;
	}
putpixel(x0+x,y0+y);
putpixel(x0-x,y0+y);
putpixel(x0+x,y0-y);
putpixel(x0-x,y0-y);

r=r1;r1=r2;r2=r;
x=0;y=r2;
r12=r1*r1;r22=r2*r2;
xmax=r12/sqrt(r12+r22);
tn=r12-2*r2*r12;
while(x<=xmax) {
	if(tn<0||y==0)
		tn+=(4*x+2)*r22;
	else
		{
		tn+=(4*x+2)*r22+(1-y)*4*r12;
		y--;
		}
	putpixel(x0+y,y0+x);
	putpixel(x0+y,y0-x);
	putpixel(x0-y,y0+x);
	putpixel(x0-y,y0-x);
	x++;
	}
putpixel(x0+y,y0+x);
putpixel(x0+y,y0-x);
putpixel(x0-y,y0+x);
putpixel(x0-y,y0-x);
}


IMAGE::~IMAGE()
{
if(where==inMEM)
	free(handle.mem);
else if(where==inXMS)
	delete handle.xms;
else if(where==inHD)
	{
	remove(handle.filename);
	free(handle.filename);
	}
where=inNONE;
}

long VGABASE::imagesize(int x1,int y1,int x2,int y2)
{
long size;
int k;
if(y1>y2)
	{ k=y1; y1=y2; y2=k; }
size=(long)scanlinesize(x1,x2)*((long)y2-(long)y1+1L);
return(size);
}

IMAGE *VGABASE::getimage(int x1,int y1,int x2,int y2,int where)
{
int i;
long size;
IMAGE *img;

img=new IMAGE;
if(x1>x2)
	{ i=x1; x1=x2; x2=i; }
if(y1>y2)
	{ i=y1; y1=y2; y2=i; }
img->x1=x1;
img->y1=y1;
img->xn=x2-x1+1;
img->yn=y2-y1+1;
size=imagesize(x1,y1,x2,y2);

if(where==inMEM)
	{
	long l;
	l=coreleft();
	if(size>=l || size>=0xffffL)
		where=inXMS;
	else
		{
		img->handle.mem=malloc((size_t)size);
		getimageMEM(x1,y1,x2,y2,img->handle.mem);
		}
	}
if(where==inXMS)
	{
	if(XMS::OK==0)
		where=inHD;
	else
		{
		long l=0;
		l=(long)XMS::largestblock()-1L;
		l *= 1024L;
		if( size>=l )
			where=inHD;
		}
	if( where==inXMS )
		img->handle.xms=getimageXMS(x1,y1,x2,y2);
	}
if(where==inHD)
	img->handle.filename=getimageHD(x1,y1,x2,y2);
img->where=where;
return(img);
}

void VGABASE::putimage(int x1,int y1,IMAGE *img)
{
if( img->where==inMEM )
	putimageMEM(x1,y1,img->xn,img->yn,img->handle.mem);
else if( img->where==inXMS )
	putimageXMS(x1,y1,img->xn,img->yn,img->handle.xms);
else if( img->where==inHD )
	putimageHD(x1,y1,img->xn,img->yn,img->handle.filename);
}

void VGABASE::putimage(IMAGE *img)
{
if( img->where==inMEM )
	putimageMEM(img->x1,img->y1,img->xn,img->yn,img->handle.mem);
else if( img->where==inXMS )
	putimageXMS(img->x1,img->y1,img->xn,img->yn,img->handle.xms);
else if( img->where==inHD )
	putimageHD(img->x1,img->y1,img->xn,img->yn,img->handle.filename);
}

void VGABASE::getimageMEM(int x1,int y1,int x2,int y2,void *buf)
{
char *cp;
int xn,ssize,y;

xn=x2-x1+1;
ssize=scanlinesize(x1,x2);
cp=(char *)buf;
for(y=y1;y<=y2;y++)
	{
	getscanline(x1,y,xn,cp);
	cp += ssize;
	}
}

XMS *VGABASE::getimageXMS(int x1,int y1,int x2,int y2)
{
int ksize,ssize,xn,y;
void *buf;
long offset=0L;
XMS *xms;

xn=x2-x1+1;
ksize=(int)((imagesize(x1,y1,x2,y2)+1023L)/1024L );
xms=new XMS(ksize);
ssize=scanlinesize(x1,x2);
buf=malloc(ssize);
for(y=y1;y<=y2;y++)
	{
	getscanline(x1,y,xn,buf);
	xms->put((void *)offset,buf,ssize);
	offset += ssize;
	}
free(buf);
return(xms);
}

char *VGABASE::getimageHD(int x1,int y1,int x2,int y2)
{
static int No=0;
char *filename;
FILE *fp;
void *buf;
int ssize,xn,y;

filename=(char *)calloc(13,1);
strcpy(filename,"image_a.swp");
filename[6]+=No;
No++;

xn=x2-x1+1;
ssize=scanlinesize(x1,x2);
buf=malloc(ssize);
fp=fopen(filename,"wb");
for(y=y1;y<=y2;y++)
	{
	getscanline(x1,y,xn,buf);
	fwrite(buf,ssize,1,fp);
	}
free(buf);
fclose(fp);
return(filename);
}

void VGABASE::putimageMEM(int x1,int y1,int xn,int yn,void *buf)
{
int ssize,y,i;
char *cp;

cp=(char *)buf;
ssize=scanlinesize(x1,x1+xn-1);
y=y1;
for(i=0;i<yn;i++)
	{
	putscanline(x1,y,xn,cp);
	y++;
	cp += ssize;
	}
}

void VGABASE::putimageXMS(int x1,int y1,int xn,int yn,XMS *xms)
{
int ssize,y,i;
void *buf;
long offset=0L;

ssize=scanlinesize(x1,x1+xn-1);
buf=malloc(ssize);
y=y1;
for(i=0;i<yn;i++)
	{
	xms->get(buf,(void *)offset,ssize);
	putscanline(x1,y,xn,buf);
	y++;
	offset += ssize;
	}
free(buf);
}

void VGABASE::putimageHD(int x1,int y1,int xn,int yn,char *filename)
{
FILE *fp;
void *buf;
int ssize,y,i;

ssize=scanlinesize(x1,x1+xn-1);
buf=malloc(ssize);
fp=fopen(filename,"rb");
y=y1;
for(i=0;i<yn;i++)
	{
	fread(buf,ssize,1,fp);
	putscanline(x1,y,xn,buf);
	y++;
	}
fclose(fp);
free(buf);
}
