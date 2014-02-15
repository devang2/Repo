#include "\vga\vgabase.h"
#include "\vga\vga16.h"

#define GRAPHICS_CTL 3ceh
#define SEQUENCE_CTL 3c4h

int VGA16::scanlinesize(int x1,int x2)
{
return ( (x2-x1)/8 + 3 )*4 + 2;
}

void VGA16::cls0()
{
int pn=PAGEN;
display_off();
asm {
	mov dx,SEQUENCE_CTL
	mov al,2
	out dx,al
	inc dx
	mov al,0fh
	out dx,al

	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,3h
	out dx,al

	dec dx
	mov al,3h
	out dx,al
	mov al,0h
	inc dx
	out dx,al

	dec dx
	mov al,08h
	out dx,al
	mov al,0ffh
	inc dx
	out dx,al

	dec dx
	mov al,0h
	out dx,al
	inc dx
	mov al,0
	out dx,al

	mov si,0
	}
loop:
	Select_Page(_SI);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov di,0
	mov ax,0FFFFh
	mov cx,8000h
	cld
	rep stosw

	inc si
	cmp si,pn
	jne loop
	}
display_on();
}

void VGA16::cls()
{
int pn=PAGEN;
unsigned char color=CUR_COLOR.byte;
asm {
	mov dx,SEQUENCE_CTL
	mov al,2
	out dx,al
	inc dx
	mov al,0fh
	out dx,al

	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,3h
	out dx,al

	dec dx
	mov al,3h
	out dx,al
	mov al,0h
	inc dx
	out dx,al

	dec dx
	mov al,08h
	out dx,al
	mov al,0ffh
	inc dx
	out dx,al

	dec dx
	mov al,0h
	out dx,al
	inc dx
	mov al,color
	out dx,al

	mov si,0
	}
loop:
	Select_Page(_SI);
asm {
	mov ax,G_SEGMENT
	mov es,ax
	mov di,0
	mov ax,0ffffh
	mov cx,8000h
	cld
	rep stosw

	inc si
	cmp si,pn
	jne loop
	}
}

void VGA16::putpixel(int x, int y)
{
unchar color=CUR_COLOR.byte;
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
asm {
//	计算偏移地址(DI)及页(DX)
	mov bx,x
	mov cl,3
	shr bx,cl
	mov ax,y
	mul scanleng
	add ax,bx
	mov di,ax
	cmp dx,cur_page
	je jemp1:
	}
	Select_Page(_DX);
jemp1:
asm {
	mov dx,SEQUENCE_CTL
	mov al,2
	out dx,al
	inc dx
	mov al,0fh
	out dx,al

//	置写模式 2
	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,02h
	out dx,al
//	计算并设置屏蔽位
	dec dx
	mov al,08h
	out dx,al
	mov cx,x
	and cl,7
	mov al,80h
	shr al,cl
	inc dx
	out dx,al
//	写入
	mov ax,G_SEGMENT
	mov es,ax
	mov al,color
	mov ah,es:[di]
	mov es:[di],al
	}
}

COLOR VGA16::getpixel(int x, int y)
{
COLOR color={0};
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
asm {
	mov bx,x
	shr bx,1
	shr bx,1
	shr bx,1
	mov ax,y
	mul scanleng
	add ax,bx
	mov di,ax
	cmp dx,cur_page
	je jemp1
	}
	Select_Page(_DX);
jemp1:
asm {
	mov cx,x
	and cl,7
	mov bl,80h
	shr bl,cl

	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,0
	out dx,al

	mov ax,G_SEGMENT
	mov es,ax

	dec dx
	mov al,04h
	out dx,al
	inc dx
	mov al,3
	mov ch,0
	}
loop1:
asm {
	out dx,al
	shl ch,1
	mov ah,es:[di]
	and ah,bl
	jz jemp2
	or ch,1
	}
jemp2:
asm {
	dec al
	jge loop1
	mov ah,0
	mov al,ch
	}
color.byte=_AX;
return(color);
}

void VGA16::scanline(int x1,int x2,int y)
{
unsigned char color=CUR_COLOR.byte;
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
asm {
	mov ax,x1
	mov bx,x2
	cmp bx,ax
	jge jemp1
	mov x1,bx
	mov x2,ax
	}
jemp1:
asm {
	mov ax,y
	mul scanleng
	push ax
	mov bx,x1
	shr bx,1
	shr bx,1
	shr bx,1
	add ax,bx
	mov di,ax
	mov bx,x2
	shr bx,1
	shr bx,1
	shr bx,1
	pop ax
	add ax,bx
	mov si,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	mov dx,SEQUENCE_CTL
	mov al,2
	out dx,al
	inc dx
	mov al,0fh
	out dx,al

//	置写模式 3
	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,3h
	out dx,al

	dec dx
	mov al,0h
	out dx,al
	inc dx
	mov al,color
	out dx,al

	dec dx
	mov al,3h
	out dx,al
	inc dx
	mov al,0h
	out dx,al

//	计算并设置屏蔽位
	dec dx
	mov al,08h
	out dx,al
	inc dx
	mov al,0ffh
	out dx,al

	mov ax,G_SEGMENT
	mov es,ax
	cmp si,di
	je inonebyte
	}
asm {
// write start byte
	mov cx,x1
	and cx,7
	mov al,0ffh
	shr al,cl
	mov ah,es:[di]
	mov es:[di],al
	mov al,0ffh
	}
middle_byte_loop:
asm {
	inc di
	cmp di,si
	je write_end_byte
	mov es:[di],al
	jmp middle_byte_loop
	}
write_end_byte:
asm {
	mov cx,x2
	not cx
	and cx,7
	mov al,0ffh
	shl al,cl
	mov ah,es:[di]
	mov es:[di],al
	jmp end
	}
inonebyte:
asm {
	mov cx,x1
	and cx,7
	mov al,0ffh
	shr al,cl

	mov cx,x2
	not cx
	and cx,7
	mov ah,0ffh
	shl ah,cl
	and al,ah
//	写入
	mov ah,es:[di]
	mov es:[di],al
	}
end:
}

void VGA16::getscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unsigned byte1,byte2,x2;
char pn=4;
asm {
	mov ax,x1
	add ax,n
	dec ax
	mov x2,ax

	mov ax,y
	mul scanleng
	push ax
	mov bx,x1
	shr bx,1
	shr bx,1
	shr bx,1
	add ax,bx
	mov byte1,ax
	pop ax
	mov bx,x2
	shr bx,1
	shr bx,1
	shr bx,1
	add ax,bx
	mov byte2,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,0
	out dx,al

	dec dx
	mov al,04h
	out dx,al
	inc dx

	mov cx,x1
	and cx,7
	mov bl,cl
	mov bh,8
	sub bh,bl

	push ds
	mov ax,G_SEGMENT
	mov ds,ax
	les di,buf
	mov ax,byte2;
	sub ax,byte1;
	add ax,2
	mov es:[di],al
	inc di

	}
loop1:
asm {
	dec pn
	mov al,pn
	out dx,al
	mov si,byte1
	}
	loop2:
	asm {
		mov ah,ds:[si]
		inc si
		mov al,ah
		mov cl,bh
		shr ah,cl
		or es:[di],ah
		inc di
		mov cl,bl
		shl al,cl
		mov es:[di],al
		cmp byte2,si
		jge loop2
		inc di

	cmp pn,0
	jne loop1

	pop ds
	}
}

void VGA16::putscanline(int x1,int y,int n,void *buf)
{
int scanleng=SCANLENG;
int cur_page=CUR_PAGE;
unsigned x2,byte1,byte2;
unsigned char pn=4,pbn;
asm {
	mov ax,x1
	add ax,n
	dec ax
	mov x2,ax

	mov ax,y
	mul scanleng
	push ax
	mov bx,x1
	shr bx,1
	shr bx,1
	shr bx,1
	add ax,bx
	mov byte1,ax
	mov bx,x2
	shr bx,1
	shr bx,1
	shr bx,1
	pop ax
	add ax,bx
	mov byte2,ax
	cmp dx,cur_page
	je jemp2
	}
	Select_Page(_DX);
jemp2:
asm {
//	置写模式 0
	mov dx,GRAPHICS_CTL
	mov al,05h
	out dx,al
	inc dx
	mov al,0h
	out dx,al

	dec dx
	mov al,01h
	out dx,al
	inc dx
	mov al,0h
	out dx,al

	dec dx
	mov al,03h
	out dx,al
	inc dx
	mov al,0h
	out dx,al
//	计算并设置屏蔽位
	push ds

	mov ax,G_SEGMENT
	mov es,ax
	lds si,buf;
	mov al,ds:[si]
	mov pbn,al

	mov cx,x1
	and cx,7
	mov bl,cl
	mov bh,8
	sub bh,bl
	}

loop1:
asm {
	dec pn
	lds si,buf
	mov al,3
	sub al,pn
	mul pbn
	add ax,2
	add si,ax
	mov di,byte1

	mov dx,SEQUENCE_CTL
	mov al,2
	out dx,al
	inc dx
	mov cl,pn
	mov al,1
	shl al,cl
	out dx,al

	mov dx,GRAPHICS_CTL
	mov al,08h
	out dx,al
	inc dx

	mov ax,byte1
	cmp ax,byte2
	je inonebyte

// write start byte
	mov al,0ffh
	mov cl,bl
	shr al,cl
	out dx,al

	mov al,ds:[si]
	mov cl,bl
	shr al,cl
	mov ah,es:[di]
	mov es:[di],al

// write middle byte
	mov al,0ffh
	out dx,al
	}
	middle_byte_loop:
	asm {
		inc di
		cmp di,byte2
		je write_end_byte
		mov al,ds:[si]
		mov cl,bh
		shl al,cl
		inc si
		mov ah,ds:[si]
		mov cl,bl
		shr ah,cl
		or al,ah
		mov es:[di],al
		jmp middle_byte_loop
		}
write_end_byte:
asm {
	mov cx,x2
	not cx
	and cx,7
	mov al,0ffh
	shl al,cl
	out dx,al

	mov al,ds:[si]
	mov cl,bh
	shl al,cl
	inc si
	mov ah,ds:[si]
	mov cl,bl
	shr ah,cl
	or al,ah
	mov ah,es:[di]
	mov es:[di],al

	jmp jemp3
	}
inonebyte:
asm {
	mov cx,x1
	and cx,7
	mov al,0ffh
	shr al,cl
	mov cx,x2
	not cx
	and cx,7
	mov ah,0ffh
	shl ah,cl
	and al,ah
	out dx,al

	mov al,ds:[si]
	mov cl,bl
	shr al,cl
//	写入
	mov ah,es:[di]
	mov es:[di],al
	}
jemp3:
if(pn>0)
	goto loop1;

asm	pop ds
}

void VGA16::setpalette(unchar idx16,unchar idx256)
{
asm {
	mov bl,idx16
	and bl,0fh
	mov bh,idx256
	and bh,3fh
	mov ax,1000h
	int 10h
	}
}

unchar VGA16::getpalette(unchar idx16)
{
asm {
	mov bl,idx16
	and bl,0fh
	mov ax,1007h
	int 10h
	}
return _BH;
}

_640_480_16::_640_480_16()
{
SCANLENG=80;
WIDE=640;
HIGH=480;
PAGEN=1;
VESAmodeNo=0x0012;
}

_800_600_16::_800_600_16()
{
SCANLENG=100;
WIDE=800;
HIGH=600;
PAGEN=1;
VESAmodeNo=0x0102;
}

_1024_768_16::_1024_768_16()
{
SCANLENG=128;
WIDE=1024;
HIGH=768;
PAGEN=2;
VESAmodeNo=0x0104;
}
