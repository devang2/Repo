#include "\vga\vgabase.h"
#include "\vga\vgahigh.h"

void VGAHIGH::putpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
int color=CUR_COLOR.word;
asm {
	mov ax,y
	mul scanleng
	shl x,1
	add ax,x
	jnc jemp1
	inc dx
	}
jemp1:
asm {
	mov di,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov ax,color
	mov es:[di],ax
	}
}

COLOR VGAHIGH::getpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
union COLOR color={0L};
asm {
	mov ax,y
	mul scanleng
	shl x,1
	add ax,x
	jnc jemp1
	inc dx
	}
jemp1:
asm	{
	mov di,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov ax,es:[di]
	}
color.word=_AX;
return(color);
}

void VGAHIGH::cls()
{
int col=CUR_COLOR.word;
int pn=PAGEN;

asm mov si,0
loop:
Select_Page(_SI);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov di,0h
	mov ax,col
	mov cx,8000h
	cld
	rep stosw

	inc si
	cmp si,pn
	jne loop
	}
}

int VGAHIGH::scanlinesize(int x1,int x2)
{
int k;
if(x1>x2)
	{ k=x1; x1=x2; x2=k; }
k=(x2-x1+1)*2;
return k;
}

void VGAHIGH::scanline(int x1,int x2,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
int k;
int color=CUR_COLOR.word;

asm {
	shl x1,1
	shl x2,1
	mov ax,x2
	sub ax,x1
	jnc jemp1
	not ax
	inc ax
	mov bx,x2
	mov x1,bx
	}
jemp1:
asm {
	push ax

	mov ax,y
	mul scanleng
	add ax,x1
	jnc jemp2
	inc dx
	}
jemp2:
asm {
	mov di,ax
	cmp dx,cur_page
	je jemp3
	mov cur_page,dx
	}
	Select_Page(cur_page);
jemp3:
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov ax,di
	pop cx
	add ax,cx
	jc jemp4
	shr cx,1
	inc cx
	mov ax,color
	cld
	rep stosw
	jmp end
	}
jemp4:
asm {
	push ax
	sub cx,ax
	shr cx,1
	mov ax,color
	cld
	rep stosw
	inc cur_page
	}
	Select_Page(cur_page);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	pop cx
	shr cx,1
	inc cx
	mov di,0h
	mov ax,color
	cld
	rep stosw
	}
end:;
}

void VGAHIGH::getscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
int k1,k2;
asm {
	mov ax,y
	mul scanleng
	shl x1,1
	add ax,x1
	jnc jemp1
	inc dx
	}
jemp1:
asm {
	mov si,ax
	cmp dx,cur_page
	je jemp2
	mov cur_page,dx
	}
	Select_Page(cur_page);
jemp2:
asm {
	mov ax,si
	mov cx,n
	shl cx,1
	dec cx
	add ax,cx
	jc jemp3
	push ds
	inc cx
	les di,buf
	mov ax,G_SEGMENT
	mov ds,ax
	cld
	rep movsb
	pop ds
	jmp end
	}
jemp3:
asm {
	push ds
	sub cx,ax
	mov k1,ax
	les di,buf
	mov ax,G_SEGMENT
	mov ds,ax
	cld
	rep movsb
	mov k2,di
	pop ds
	inc cur_page
	}
	Select_Page(cur_page);
asm {
	push ds
	mov cx,k1
	inc cx
	les di,buf
	mov di,k2
	mov ax,G_SEGMENT
	mov ds,ax
	mov si,0h
	cld
	rep movsb
	pop ds
	}
end:;
}

void VGAHIGH::putscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
int k1,k2;
asm {
	mov ax,y
	mul scanleng
	shl x1,1
	add ax,x1
	jnc jemp1
	inc dx
	}
jemp1:
asm {
	mov di,ax
	cmp dx,cur_page
	je jemp2
	mov cur_page,dx
	}
	Select_Page(cur_page);
jemp2:
asm {
	mov ax,di
	mov cx,n
	shl cx,1
	dec cx
	add ax,cx
	jc jemp3
	push ds
	inc cx
	mov ax,G_SEGMENT
	mov es,ax
	lds si,buf
	cld
	rep movsb
	pop ds
	jmp end
	}
jemp3:
asm {
	push ds
	mov k1,ax
	sub cx,ax
	mov ax,G_SEGMENT
	mov es,ax
	lds si,buf
	cld
	rep movsb
	mov k2,si
	pop ds
	inc cur_page
	}
	Select_Page(cur_page);
asm {
	push ds
	mov cx,k1
	inc cx
	mov ax,G_SEGMENT
	mov es,ax
	mov di,0h
	lds si,buf
	mov si,k2
	cld
	rep movsb
	pop ds
	}
end:;
}

void VGA32k::setcolor(unchar red,unchar green,unchar blue)
{
unsigned col;
asm {
	mov bx,0

	mov al,blue
	mov cl,3
	shr ax,cl
	and ax,0000000000011111b
	or bx,ax

	mov al,green
	mov cl,2
	shl ax,cl
	and ax,0000001111100000b
	or bx,ax

	mov al,red
	mov cl,7
	shl ax,cl
	and ax,0111110000000000b
	or bx,ax
	}
col=_BX;
CUR_COLOR.word=col;
}

COLOR VGA32k::setcolorto(unchar red,unchar green,unchar blue)
{
COLOR color={0L};
asm {
	mov bx,0

	mov al,blue
	mov cl,3
	shr ax,cl
	and ax,0000000000011111b
	or bx,ax

	mov al,green
	mov cl,2
	shl ax,cl
	and ax,0000001111100000b
	or bx,ax

	mov al,red
	mov cl,7
	shl ax,cl
	and ax,0111110000000000b
	or bx,ax
	}
color.word=_BX;
return(color);
}

void VGA64k::setcolor(unchar red,unchar green,unchar blue)
{
unsigned col;
asm {
	mov bx,0

	mov al,blue
	mov cl,3
	shr ax,cl
	and ax,0000000000011111b
	or bx,ax

	mov al,green
	mov cl,3
	shl ax,cl
	and ax,0000011111100000b
	or bx,ax

	mov al,red
	mov cl,8
	shl ax,cl
	and ax,1111100000000000b
	or bx,ax
	}
col=_BX;
CUR_COLOR.word=col;
}

COLOR VGA64k::setcolorto(unchar red,unchar green,unchar blue)
{
COLOR color={0L};
asm {
	mov bx,0

	mov al,blue
	mov cl,3
	shr ax,cl
	and ax,0000000000011111b
	or bx,ax

	mov al,green
	mov cl,3
	shl ax,cl
	and ax,0000011111100000b
	or bx,ax

	mov al,red
	mov cl,8
	shl ax,cl
	and ax,1111100000000000b
	or bx,ax
	}
color.word=_BX;
return(color);
}

_320_200_32k::_320_200_32k()
{
SCANLENG=640;
WIDE=320;
HIGH=200;
PAGEN=2;
VESAmodeNo=0x010d;
}

_512_480_32k::_512_480_32k()
{
SCANLENG=1024;
WIDE=512;
HIGH=480;
PAGEN=8;
VESAmodeNo=0x0170;
}

_640_480_32k::_640_480_32k()
{
SCANLENG=1280;
WIDE=640;
HIGH=480;
PAGEN=10;
VESAmodeNo=0x0110;
}

_800_600_32k::_800_600_32k()
{
SCANLENG=1600;
WIDE=800;
HIGH=600;
PAGEN=15;
VESAmodeNo=0x0113;
}

_320_200_64k::_320_200_64k()
{
SCANLENG=640;
WIDE=320;
HIGH=200;
PAGEN=2;
VESAmodeNo=0x010e;
}

_512_480_64k::_512_480_64k()
{
SCANLENG=1024;
WIDE=512;
HIGH=480;
PAGEN=8;
VESAmodeNo=0x0171;
}

_640_480_64k::_640_480_64k()
{
SCANLENG=1280;
WIDE=640;
HIGH=480;
PAGEN=10;
VESAmodeNo=0x0111;
}

_800_600_64k::_800_600_64k()
{
SCANLENG=1600;
WIDE=800;
HIGH=600;
PAGEN=15;
VESAmodeNo=0x0114;
}
