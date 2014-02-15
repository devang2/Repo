#include <dos.h>
#include <conio.h>
#include <bios.h>
#include <alloc.h>
#include <string.h>

#include "\vga\vga.h"
#include "text.h"

void dmode();
void dfont();
void dprt();
void dmouse();
void droam();

void main()
{
int n=6;
char *str[6]={	" Graphics Modes ",
				" ASCII & CC Fonts ",
				" Graphics Print ",
				" Mouse Drive ",
				" Screen Roam ",
				" Exit "};
int i;
int x0=30,y0=7;
unsigned colh=color(0,7,0,0),coll=color(7,0,0,0);
void *buf;
buf=malloc(4200);

cclsall();
mmvprs(0,79,y0-3," VGA Demo ",color(2,0,0,0));
mmvprs(0,79,y0-2,"屯屯屯屯屯",color(2,0,0,0));
i=0;
while(1) {
	i=menu(n,i,x0,y0,coll,colh,str);
	gettext(1,1,80,25,buf);
	if( i==-1 || i==(n-1) )
		break;
	else if(i==0)
		dmode();
	else if(i==1)
		dfont();
	else if(i==2)
		dprt();
	else if(i==3)
		dmouse();
	else if(i==4)
		droam();
	puttext(1,1,80,25,buf);
	}
free(buf);
cclsall();
}

void xyvprs(int x,int y,char *s,int color)
{
int n,v,kk,cc,i;
unsigned char c1;

n=strlen((char *)s);
v=160*y+2*x;
c1=color&0xff;
for(i=0;i<n;i++)
	{
	kk=v+i*2;
	pokeb(VB800,kk,s[i]);
	pokeb(VB800,kk+1,c1);
	}
}

void mmvprs(int x1,int x2,int y,char *s,int cold)
{
int x,leng,i,n;

leng=strlen(s);
n=x2-x1+1;
if(leng>=n)
	x=x1;
else
	x=x1+(n-leng-1)/2;
xyvprs(x,y,s,cold);
}

void putxy(int x,int y)
{
_BH=0;
_DH=y;
_DL=x;
_AH=2;
geninterrupt(0x10);
}

/* 设置区域的颜色 */
void putcolor(int x1,int y1,int x2,int y2,int color)
{
register int i,j;
int kk,m1,m2;
m1=2*x1;
m2=2*x2;
for(i=y1;i<=y2;i++)
	{
	kk=i*160+1;
	for(j=m1;j<=m2;j+=2)
		pokeb(VB800,kk+j,color);
	}
}

void cclsall()
{
int i,j;
j=0x0700;
for(i=0;i<4000;i+=2)
	poke(VB800,i,j);
}

int strslen(int n,char *s[])
{
register int i;
int l,max=0;
for(i=0;i<n;i++)
	{
	l=strlen((char *)s[i]);
	if(l>max)
		max=l;
	}
return (max);
}
/* 通用菜单函数                                                      */
/* 返回参数: 返回用户的选择,选择第一项返回0,选择第二项返回1,等等;    */
/*           按Esc返回27;                                            */
/* 输入参数: n 菜单项数;                                             */
/*           m 亮条的初始位置(第一项为0,...);                        */
/*           x,y 菜单体左上角坐标;                                   */
/*           col1 属性;                                              */
/*           col2 亮条属性;                                          */
/*           s[] 菜单内容字符串;                                     */
int menu(int n,int m,int x1,int y1,int col1,int col2,char *s[])
{
int i,j,key,x2,v;
static char *str="A: ";

x2=strslen(n,s)+x1+2;
str[0]='A';
for(i=0;i<n;i++)
	{
	xyvprs(x1,y1+i,str,col1);
	xyvprs(x1+3,y1+i,s[i],col1);
	str[0]++;
	}
clearkey();
do {
	putxy(x1,y1+m);
	putcolor(x1,y1+m,x2,y1+m,col2);
	key=getkey();
	putcolor(x1,y1+m,x2,y1+m,col1);
	if(key==_UP)
		{
		m--;
		if(m<0)
			m=n-1;
		}
	else if(key==_DOWN)
		{
		m++;
		if(m>=n)
			m=0;
		}
	else if(key==HOME||key==PGUP)
		m=0;
	else if(key==END||key==PGDN)
		m=n-1;
	else if(key==27)
		{
		v=-1; break;
		}
	else if(key==13)
		{
		v=m; break;
		}
	else if(key>='A'&&key<'A'+n)
		{
		m=key-'A';
		v=m;
		break;
		}
	else if(key>='a'&&key<'a'+n)
		{
		m=key-'a';
		v=m;
		break;
		}
} while(1);
if(v>=0)
	putcolor(x1,y1+m,x2,y1+m,col2);
return(v);
}

void clearkey()
{
while(bioskey(1)!=0) {
	getkey();
	};
}

int getkey()
{
int a,x,y;
a=bioskey(0);
x=a>>8;
y=a&0xff;
if(y==0)
	return(256+x);
else
	return(y);
}

