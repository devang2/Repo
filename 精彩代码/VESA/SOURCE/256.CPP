#include "\vga\vgabase.h"
#include "\vga\vga256.h"

void VGA256::putpixel(int x,int y)
{
unchar color=CUR_COLOR.byte;
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
asm {
	mov ax,y
	mul scanleng
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
	mov al,color
	mov es:[di],al
	}
}

union COLOR VGA256::getpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
union COLOR color={0L};
asm {
	mov ax,y
	mul scanleng
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
	mov al,es:[di]
	}
color.byte=_AL;
return(color);
}

void VGA256::cls()
{
unchar col=CUR_COLOR.byte;
int pn=PAGEN;

asm mov si,0
loop:
Select_Page(_SI);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov di,0h
	mov ah,col
	mov al,col
	mov cx,8000h
	cld
	rep stosw

	inc si
	cmp si,pn
	jne loop
	}

}

int VGA256::scanlinesize(int x1,int x2)
{
int k;
if(x1>x2)
	{ k=x1; x1=x2; x2=k; }
k=x2-x1+1;
if( (k&0x0001) )
	k++;
return(k);
}

void VGA256::scanline(int x1,int x2,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unsigned char color;
color=CUR_COLOR.byte;

asm {
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
	inc cx
	mov al,color
	cld
	rep stosb
	jmp end
	}
jemp4:
asm {
	push ax
	sub cx,ax
	mov al,color
	cld
	rep stosb
	inc cur_page
	}
	Select_Page(cur_page);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	pop cx
	inc cx
	mov di,0h
	mov al,color
	cld
	rep stosb
	}
end:
}

void VGA256::getscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
int k1,k2;
asm {
	mov ax,y
	mul scanleng
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
	mov k1,ax
	sub cx,ax
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

void VGA256::putscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
int k1,k2;
asm {
	mov ax,y
	mul scanleng
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

_320_200_256::_320_200_256()
{
SCANLENG=320;
WIDE=320;
HIGH=200;
PAGEN=1;
VESAmodeNo=0x0013;
}

_640_400_256::_640_400_256()
{
SCANLENG=640;
WIDE=640;
HIGH=400;
PAGEN=4;
VESAmodeNo=0x0100;
}

_640_480_256::_640_480_256()
{
SCANLENG=640;
WIDE=640;
HIGH=480;
PAGEN=5;
VESAmodeNo=0x0101;
}

_800_600_256::_800_600_256()
{
SCANLENG=800;
WIDE=800;
HIGH=600;
PAGEN=8;
VESAmodeNo=0x0103;
}

_1024_768_256::_1024_768_256()
{
SCANLENG=1024;
WIDE=1024;
HIGH=768;
PAGEN=12;
VESAmodeNo=0x0105;
}
