#include <dos.h>
#include <stdlib.h>
#include <string.h>

#include "\vga\vga.h"

void qquit()
{
printf("\n PLC printer_type size C_chart HZK_filename\n");
printf("Example:\n");
printf("plc epson 36 ¿¬ hzkslktj\n");
printf("plc hp 8 ºÚ hzkslhtj\n\n");
exit(1);
}

void main()
{
if(_argc<5)
	{
	printf("\nParaemter error\n");
	qquit();
	}

int type;
float size=0;
char *str="  ";
unsigned char *cstr;

if(stricmp(_argv[1],"EPSON")==0)
	type=0;
else if(stricmp(_argv[1],"HP")==0)
	type=1;
else
	{
	printf("\nprinter type error!\n");
	qquit();
	}

size=atof(_argv[2]);
if(size==0)
	{
	printf("\nsize error\n");
	qquit();
	}
int wide;
if(type==0)
	size *= 60.0;
else
	size *= 75;
size /= 2.54;
wide=size;

cstr=_argv[3];
if( cstr[0]>=0xa0 && cstr[1]>=0xa0 )
	{
	str[0]=_argv[3][0];
	str[1]=_argv[3][1];
	cstr=str;
	}
else
	{
	printf("\nChinese Chart error\n");
	qquit();
	}

XMS::init();
prt_buf pb(wide+8,wide+8,inMEM);
FONT *t;
if(cstr[0]<0xb0)
	t=new TXUC(_argv[4],inHD,&pb);
else
	t=new HZUC(_argv[4],inHD,&pb);
if(t->OK==0)
	{
	delete t;
	printf("\n%s no exist\n",_argv[4]);
	qquit();
	}
t->setsize(wide,wide,0);
t->sethollow(1);
t->outtextxy(0,0,cstr);
delete t;

PRINTER *p;
if(type==0)
	p=new EPSON_60(&pb);
else
	p=new HP_75(&pb);
p->print();
delete p;
}

