#include <dos.h>
#include <alloc.h>

#include "\vga\vgabase.h"

static void far *Select_Page_Func;

int VGABASE::init(void)
{
CUR_COLOR.dword=0L;
setmode();
if(PAGEN>0)
	Select_Page(0);
return(OK);
//getpagepar();
}

void VGABASE::setmode()
{
union REGS inregs;
OK=1;
if(VESAmodeNo>=0x100)
	{
	inregs.x.bx=VESAmodeNo;
	inregs.x.ax=0x4f02;
	int86(0x10,&inregs,&inregs);
	if(inregs.x.ax!=0x004f)
		OK=0;
	}
else if(VESAmodeNo>=0)
	{
	inregs.x.ax=VESAmodeNo;
	int86(0x10,&inregs,&inregs);
	}
}

void VGABASE::getpagepar()
{
if(VESAmodeNo<0x100)
	return;
unsigned int *info;
union REGS inregs,outregs;
struct SREGS sregs;

info=(unsigned int *)calloc(128,2);
sregs.es=FP_SEG(info);
inregs.x.di=FP_OFF(info);
inregs.x.cx=VESAmodeNo;
inregs.x.ax=0x4f01;
int86x(0x10,&inregs,&outregs,&sregs);
Select_Page_Func=MK_FP(info[7],info[6]);
PN=info[3]/info[2];
free(info);
}

void VGABASE::close(void)
{
union REGS inregs;
inregs.x.ax=0x0003;
int86(0x10,&inregs,&inregs);
}

void VGABASE::Select_Page_VESAcall(int page)
{
asm {
	mov bx,0000h
	mov dx,page
	call Select_Page_Func
	}
CUR_PAGE=page;
}

void VGABASE::Select_Page_VESAcallG(int page)
{
page *= PN;
asm {
	mov bx,0000h
	mov dx,page
	call Select_Page_Func
	}
CUR_PAGE=page;
}

void VGABASE::Select_Page_VESAint(int page)
{
asm {
	mov bx,0
	mov dx,page
	mov ax,4f05h
	int 10h
	}
CUR_PAGE=page;
}

void VGABASE::Select_Page_VESAintG(int page)
{
page *= PN;
asm {
	mov bx,0
	mov dx,page
	mov ax,4f05h
	int 10h
	}
CUR_PAGE=page;
}

void VGABASE::Select_Page_Trident(int page)
{
asm {
	mov bx,page
	xor bl,02h
	mov dx,3c4h
	mov al,0eh
	out dx,al
	inc dx
	mov al,bl
	out dx,al
	}
CUR_PAGE=page;
}

void VGABASE::display_off(void)
{
asm {
	mov bl,36h
	mov ax,1201h
	int 10h
	}
}

void VGABASE::display_on(void)
{
asm {
	mov bl,36h
	mov ax,1200h
	int 10h
	}
}

void VGABASE::cls0()
{
int pn=PAGEN;
display_off();

asm mov si,0
loop:
Select_Page(_SI);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov di,0
	mov ax,0
	mov cx,8000h
	cld
	rep stosw

	inc si
	cmp si,pn
	jne loop
	}
display_on();
}

void VGABASE::setcolor(unchar col)
{
CUR_COLOR.byte=col;
}

void VGABASE::setcolor(unchar r,unchar g,unchar b)
{
CUR_COLOR.rgb[0]=b;
CUR_COLOR.rgb[1]=g;
CUR_COLOR.rgb[2]=r;
}

COLOR VGABASE::setcolorto(unchar col)
{
COLOR A={0};
A.byte=col;
return(A);
}

COLOR VGABASE::setcolorto(unchar r,unchar g,unchar b)
{
COLOR A={0};
A.rgb[0]=b;
A.rgb[1]=g;
A.rgb[2]=r;
return A;
}

void VGAindirect::setdac(unchar idx,unchar red,unchar green,unchar blue)
{
asm {
	mov dx,3c8h
	mov al,idx
	out dx,al

	mov cl,2
	inc dx
	mov al,red
	shr al,cl
	out dx,al
	mov al,green
	shr al,cl
	out dx,al
	mov al,blue
	shr al,cl
	out dx,al
	}
}

void VGAindirect::getdac(unchar idx,unchar &red,unchar &green,unchar &blue)
{
unchar r,g,b;
asm {
	mov dx,3c7h
	mov al,idx
	out dx,al

	mov cl,2
	mov dx,3c9h
	in al,dx
	shl al,cl
	mov r,al
	in al,dx
	shl al,cl
	mov g,al
	in al,dx
	shl al,cl
	mov b,al
	}
red=r; green=g; blue=b;
}
