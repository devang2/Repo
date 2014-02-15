#include <dos.h>

#include "\vga\vgabase.h"
#include "\vga\vga16.h"
#include "\vga\vga256.h"
#include "\vga\vgahigh.h"
#include "\vga\vgaroam.h"

#define CRT_CTL 3d4h

int ROAM::begx=0,ROAM::begy=0;
int ROAM::endx=0,ROAM::endy=0;
int ROAM::minMoveY,ROAM::maxMoveY;
int ROAM::maxMoveX;
int ROAM::minMoveWide,ROAM::minSetWide;
int ROAM::VWIDE,ROAM::VHIGH;
int ROAM::orgWide,ROAM::orgHigh;
int ROAM::activHigh,ROAM::fixedHigh;
int ROAM::KEY_OK=0;
int ROAM::dxy;

void ROAM::roam(int dx,int dy)
{
setbxy(begx+dx,begy+dy);
}

void ROAM::autoroam(int x0,int y0,int xn,int yn)
{
if(x0<begx)
	setbxy(x0,begy);
if(y0<begy)
	setbxy(begx,y0);
if( (x0+xn-1)>endx )
	setexy(x0+xn,endy);
if( (y0+yn-1)>endy)
	setexy(endx,y0+yn);
}

void ROAM::setexy(int ex,int ey)
{
setbxy(ex-orgWide+1,ey-orgHigh+fixedHigh+1);
}

void ROAM::setbxy(int bx,int by)
{
long l,l1;
unsigned seat;
if(bx<0) bx=0;
if( bx > maxMoveX )
	bx=maxMoveX;
if(by<minMoveY)
	by=minMoveY;
if( by >maxMoveY  )
	by=maxMoveY;
l1=(long)VWIDE*(long)by;
l=l1+(long)bx+(long)minMoveWide/2L;
l /= minMoveWide;
seat=(unsigned)l;
l *= (long)minMoveWide;
begx=l-l1;
begy=by;
endx=begx+orgWide-1;
endy=begy+orgHigh-fixedHigh-1;
asm {
	mov bx,seat
	mov dx,CRT_CTL
	mov al,0ch
	out dx,al
	inc dx
	mov al,bh
	out dx,al

	dec dx
	mov al,0dh
	out dx,al
	inc dx
	mov al,bl
	out dx,al
	}
}

void ROAM::setScreenWide(int wi)
{
wi /= minSetWide;
asm {
	mov dx,CRT_CTL
	mov al,13h
	out dx,al
	inc dx
	mov ax,wi
	out dx,al
	}
}

void ROAM::cutScreen(int hi)
{
if(hi<0||hi>=orgHigh-1)
	return;
asm {
	mov bx,hi
	mov dx,CRT_CTL
	mov al,18h
	out dx,al
	inc dx
	mov al,bl
	out dx,al

	dec dx
	mov al,7
	out dx,al
	inc dx
	in al,dx
	mov ah,bh
	mov cl,4
	shl ah,cl
	or ah,11101111b
	and ah,al
	dec dx
	mov al,7
	out dx,al
	inc dx
	mov al,ah
	out dx,al

	dec dx
	mov al,9
	out dx,al
	inc dx
	in al,dx
	mov ah,bh
	mov cl,5
	shl ah,cl
	or ah,10111111b
	and ah,al
	dec dx
	mov al,9
	out dx,al
	inc dx
	mov al,ah
	out dx,al
	}
}

void ROAM::setmode()
{
setScreenWide(VWIDE);
cutScreen(activHigh-1);
setbxy(0,0);
}

void ROAM::checkpar(void)
{
long l;
int maxy;

activHigh=orgHigh-fixedHigh;
if( activHigh<=0 )
	activHigh=0;
if( activHigh>=orgHigh )
	activHigh=orgHigh;
if( activHigh>0 && activHigh<orgHigh )
	fixedHigh=orgHigh-activHigh;
else
	fixedHigh=0;

VWIDE=(VWIDE/minSetWide)*minSetWide;
if(VWIDE<orgWide)
	VWIDE=orgWide;
maxMoveX=VWIDE-orgWide;

l = 0xffffL*(long)minMoveWide;
l /= VWIDE;
maxy = l+activHigh;
if(VHIGH>maxy)
	VHIGH=maxy;
if(VHIGH<orgHigh)
	VHIGH=orgHigh;

maxMoveY=VHIGH-activHigh-1;
minMoveY=orgHigh-activHigh-1;
if(activHigh==0||fixedHigh==0)
	minMoveY=0;
}

static unsigned char _roam_key_value[2][4]=
			{{72,80,75,77},{200,208,203,205}};
void interrupt (*oldkbint0x0009)(void);

void interrupt ROAM::kbint0x09_roam()
{
static unsigned mesp,mess,invalue,i,k,kb,f224=0,dx,dy;
disable();
mesp=_SP; mess=_SS;
enable();
invalue=inportb(0x60);
if(invalue==224)
	{
	k=0;
	f224=1;
	}
else if(f224==1)
	{
	f224=0;
	k=1;
	dx=dy=0;
	for(i=0;i<4;i++)
		if(invalue==_roam_key_value[1][i])
			k=0;
	for(i=0;i<4;i++)
		if(invalue==_roam_key_value[0][i])
			{
			k=0;
			if(i==0) dy=-dxy;
			else if(i==1) dy=dxy;
			else if(i==2) dx=-dxy;
			else if(i==3) dx=dxy;
			roam(dx,dy);
			break;
			}
	}
else
	k=1;
disable();
_SP=mesp; _SS=mess;
enable();
if(k)
	(*oldkbint0x0009)();
else
	{
	kb=inportb(0x61); outportb(0x61,0x80);
	outportb(0x61,kb); outportb(0x20,0x20);
	}
}

void ROAM::setup_key_roam(int d)
{
dxy=d;
if(KEY_OK==0)
	{
	oldkbint0x0009=(void interrupt(*)())getvect(0x09);
	disable();
	setvect(0x09,(void interrupt(*)(...))kbint0x09_roam);
	enable();
	KEY_OK=1;
	}
}

void ROAM::del_key_roam()
{
if(KEY_OK)
	{
	disable();
	setvect(0x09,(void interrupt(*)(...))oldkbint0x0009);
	enable();
	KEY_OK=0;
	}
}

