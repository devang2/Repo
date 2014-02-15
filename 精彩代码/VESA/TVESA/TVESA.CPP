#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <alloc.h>
#include <bios.h>
#include <process.h>

#include "\vga\evtcode.h"

int getkey();

int modeN;

struct SYS1 {
		char *vesa;
		char ver2,ver1;
		char *oem;
		char cap[4];
		unsigned int *mode;
		} sys1;

struct SYS2 {
	int attr;
	char wina,winb;
	int gran;
	int size;
	int aseg,bseg;
	void far *funcptr;
	int lineb;

	int xr,yr;
	char xcs,ycs;
	char planes;
	char bpp;
	char banks;
	char model;
	char banksize;
	} sys2;

void getsys2(int modeNo)
{
_ES=FP_SEG(&sys2);
_DI=FP_OFF(&sys2);
_CX=modeNo;
_AX=0x4f01;
geninterrupt(0x10);
}

void getsys1()
{
_ES=FP_SEG(&sys1);
_DI=FP_OFF(&sys1);
_AX=0x4f00;
geninterrupt(0x10);
if(_AX!=0x004f)
	{
	printf("\nYour display card don't support VESA BIOS\n");
	exit(0);
	}
int i;
for(i=0;i<1024;i++)
	if(sys1.mode[i]==0xffff)
		break;
modeN=i;
}

void dispsys(int mode)
{
char *model[7]={"Text","CGA graphics","Hercules graphics","4 Planes",
		"Compress","5","VESA retain"};
long colorn,memn,memk;
int i;

clrscr();
printf("VESA Version %d.%d\n",sys1.ver1,sys1.ver2);
printf("%s\n",sys1.oem);
printf("Number of VESA Mode : %d -- %d\n\n",modeN,mode+1);

if( (sys2.attr&0x0002)!=0 )
	{
	colorn=1;
	for(i=0;i<sys2.bpp;i++)
		colorn *= 2L;
	if( (sys2.attr&0x0010)!=0)
		memn=(long)sys2.lineb*(long)sys2.yr*(long)sys2.planes;
	else
		memn=(long)sys2.lineb*(long)sys2.yr;
	memk=(memn+512L)/1024L;
	}
printf("VESA Mode No. : %XH\n",sys1.mode[mode]);
if( (sys2.attr&0x0010)!=0 )
	printf("Graphics Mode  ");
else
	printf("Text Mode  ");
if( (sys2.attr&0x0002)!=0 )
	printf("%d * %d * %ld\n\n",sys2.xr,sys2.yr,colorn);

printf("Win A: ");
if( (sys2.wina&0x01)==0 )
	printf("None\n");
else
	{
	if( (sys2.wina&0x04)!=0 )
		printf("Write Enable    ");
	if( (sys2.wina&0x02)!=0 )
		printf("Read Enable");
	printf("\n");
	}
printf("Win A Segment : %X\n",sys2.aseg);
printf("Win B: ");
if( (sys2.winb&0x01)==0 )
	printf("None\n");
else
	{
	if( (sys2.winb&0x04)!=0 )
		printf("Write Enable    ");
	if( (sys2.winb&0x02)!=0 )
		printf("Read Enable");
	printf("\n\n");
	}
printf("Win B Segment : %X\n",sys2.bseg);
printf("Window Granularity : %dk\n",sys2.gran);
printf("Page Size : %dK\n\n",sys2.size);

printf("Bytes of scanline : %dByte\n",sys2.lineb);
if( (sys2.attr&0x0002)!=0 )
	{
	printf("Display Memory Size : %ldBytes    %ldk\n",memn,memk);
	printf("Number of Planes : %d\n",sys2.planes);
	printf("Bits of pixel : %d\n",sys2.bpp);
	printf("Display Memory model: %s\n",model[sys2.model]);
	}

printf("\nSelect Page Function Ptr. : %p\n\n",sys2.funcptr);
}

void main()
{
int i,key;

getsys1();
i=0;
do {
	getsys2(sys1.mode[i]);
	dispsys(i);
	key=getkey();
	if(key==27)
		break;
	if( key==HOME )
		i=0;
	else if( key==END )
		i=modeN-1;
	else if( (key==PGUP||key==_UP) && i>0 )
		i--;
	else if( (key==PGDN||key==_DOWN) && i<(modeN-1) )
		i++;
} while(1);
}

int getkey()
{
int k,a,b;

k=bioskey(0);
a=k>>8;
b=k&0x00ff;
if(b==0)
	k=256+a;
else
	k=b;
return(k);
}
