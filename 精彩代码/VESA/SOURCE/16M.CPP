#include "\vga\vgabase.h"
#include "\vga\vga16M.h"

void VGA16M::putpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unchar *rgb=CUR_COLOR.rgb;
asm {
	mov ax,y
	mul scanleng
	add ax,x
	shl x,1
	add ax,x
	mov di,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	push ds
	mov ax,G_SEGMENT
	mov es,ax
	mov cx,3
	lds si,rgb
	cld
	rep movsb
	pop ds
	}
}

COLOR VGA16M::getpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
COLOR color={0};
unsigned char *rgb=color.rgb;

asm {
	mov ax,y
	mul scanleng
	add ax,x
	shl x,1
	add ax,x
	mov si,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	push ds
	mov ax,G_SEGMENT
	mov ds,ax
	mov cx,3
	les di,rgb
	cld
	rep movsb
	pop ds
	}
return(color);
}

void VGA16M::scanline(int x1,int x2,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unsigned char *rgb=CUR_COLOR.rgb;

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
	inc ax
	push ax
	mov ax,y
	mul scanleng
	add ax,x1
	shl x1,1
	add ax,x1
	mov di,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	pop bx
	push ds
	mov ax,G_SEGMENT
	mov es,ax
	cld
	}
loop1:
asm	{
	lds si,rgb
	mov cx,3
	rep movsb
	dec bx
	jnz loop1
	pop ds
	}
}

void VGA16M::cls()
{
int i,x2=WIDE-1;
for(i=0;i<HIGH;i++)
	scanline(0,x2,i);
}

void VGA16M::getscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
asm {
	mov ax,y
	mul scanleng
	add ax,x1
	shl x1,1
	add ax,x1
	mov si,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	push ds
	mov cx,n
	shl cx,1
	add cx,n
	les di,buf
	mov ax,G_SEGMENT
	mov ds,ax
	cld
	rep movsb
	pop ds
	}

}

void VGA16M::putscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
asm {
	mov ax,y
	mul scanleng
	add ax,x1
	shl x1,1
	add ax,x1
	mov di,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	push ds
	mov cx,n
	shl cx,1
	add cx,n
	mov ax,G_SEGMENT
	mov es,ax
	lds si,buf
	cld
	rep movsb
	pop ds
	}
}

int VGA16M::scanlinesize(int x1,int x2)
{
int size;
size=(x2-x1+1)*3;
if( size&0x0001 )
	size++;
return(size);
}

void VGA16M::en_putpixel(int x,int y,unchar enl,unchar redu)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unchar *cc;
asm {
	mov ax,y
	mul scanleng
	add ax,x
	shl x,1
	add ax,x
	mov di,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov cx,3h
	}
loop1:
asm {
	mov al,es:[di]
	mul enl
	mov bl,0ffh
	cmp redu,ah
	jc jemp2
	div redu
	mov bl,al
	}
jemp2:
asm {
	mov es:[di],bl
	inc di
	dec cx
	jnz loop1
	}
}

void VGA16M::en_scanline(int x1,int x2,int y,unchar enl,unchar redu)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unsigned char *cc;
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
	inc ax
	mov bx,ax
	shl ax,1
	add ax,bx
	push ax

	mov ax,y
	mul scanleng
	add ax,x1
	shl x1,1
	add ax,x1
	mov di,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	mov ax,G_SEGMENT
	mov es,ax
	pop cx
	}
loop1:
asm {
	mov al,es:[di]
	mul enl
	mov bl,0ffh
	cmp redu,ah
	jc jemp3
	div redu
	mov bl,al
	}
jemp3:
asm {
	mov es:[di],bl
	inc di
	dec cx
	jnz loop1
	}
}

void VGA16M::en_bar(int x1,int y1,int x2,int y2,unchar enl,unchar redu)
{
int i;
for(i=y1;i<=y2;i++)
	en_scanline(x1,x2,i,enl,redu);
}

void VGA16M::re_putpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unchar *rgb=CUR_COLOR.rgb;
asm {
	mov ax,y
	mul scanleng
	add ax,x
	shl x,1
	add ax,x
	mov di,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	push ds
	mov ax,G_SEGMENT
	mov es,ax
	lds si,rgb
	mov cx,3h
	}
loop1:
asm {
	mov al,es:[di]
	add al,ds:[si]
	jnc jemp2
	mov al,0ffh
	}
jemp2:
asm {
	mov es:[di],al
	inc di
	inc si
	dec cx
	jnz loop1
	pop ds
	}
}

void VGA16M::re_scanline(int x1,int x2,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unchar *rgb=CUR_COLOR.rgb;

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
	inc ax
	push ax
	mov ax,y
	mul scanleng
	add ax,x1
	shl x1,1
	add ax,x1
	mov di,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	pop bx
	push ds
	mov ax,G_SEGMENT
	mov es,ax
	lds si,rgb
	mov cx,3h
	}
loop1:
asm {
	mov al,es:[di]
	add al,ds:[si]
	jnc jemp3
	mov al,0ffh
	}
jemp3:
asm {
	mov es:[di],al
	inc di
	inc si
	dec cx
	jnz loop1
	mov cx,3h
	sub si,3h
	dec bx
	jnz loop1
	pop ds
	}
}

void VGA16M::re_bar(int x1,int y1,int x2,int y2)
{
int i;
for(i=y1;i<=y2;i++)
	re_scanline(x1,x2,i);
}

void VGA16M::de_putpixel(int x,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unchar *rgb=CUR_COLOR.rgb;
asm {
	mov ax,y
	mul scanleng
	add ax,x
	shl x,1
	add ax,x
	mov di,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	push ds
	mov ax,G_SEGMENT
	mov es,ax
	lds si,rgb
	mov cx,3h
	}
loop1:
asm {
	mov al,es:[di]
	sub al,ds:[si]
	jnc jemp2
	mov al,0h
	}
jemp2:
asm {
	mov es:[di],al
	inc di
	inc si
	dec cx
	jnz loop1
	pop ds
	}
}

void VGA16M::de_scanline(int x1,int x2,int y)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unchar *rgb=CUR_COLOR.rgb;

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
	inc ax
	push ax
	mov ax,y
	mul scanleng
	add ax,x1
	shl x1,1
	add ax,x1
	mov di,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	pop bx
	push ds
	mov ax,G_SEGMENT
	mov es,ax
	lds si,rgb
	mov cx,3h
	}
loop1:
asm {
	mov al,es:[di]
	sub al,ds:[si]
	jnc jemp3
	mov al,0h
	}
jemp3:
asm {
	mov es:[di],al
	inc di
	inc si
	dec cx
	jnz loop1
	mov cx,3h
	sub si,3h
	dec bx
	jnz loop1
	pop ds
	}
}

void VGA16M::de_bar(int x1,int y1,int x2,int y2)
{
int i;
for(i=y1;i<=y2;i++)
	de_scanline(x1,x2,i);
}

_320_200_16M::_320_200_16M()
{
SCANLENG=1024;
WIDE=320;
HIGH=200;
PAGEN=4;
VESAmodeNo=0x010f;
}

_640_480_16M::_640_480_16M()
{
SCANLENG=2048;
WIDE=640;
HIGH=480;
PAGEN=15;
VESAmodeNo=0x0112;
}

