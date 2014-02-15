#pragma  inline
#include <def.inc>

void pixel(UI where_x, UI where_y)
{
	asm push ds

/*  Calculate the start address of VRAM */
	asm mov ax, where_y    /* address: where_y*80+where_x/8	*/
	asm mov cl, 4
	asm shl ax, cl
	asm mov dx, ax
	asm shl ax, 1
	asm shl ax, 1
	asm add ax, dx
	asm mov dx, where_x
	asm mov cl, 3
	asm shr dx, cl
	asm add ax, dx
	asm push ax

/* Calculate the accordingly bit map */
	asm mov cx, where_x
	asm and cl, 0x07
	asm mov ah, 0x80
	asm shr ah, cl    /* mask bit in ah */

	asm mov dx, 0x3CE
	asm mov al, 0x08
	asm out dx, al
	asm inc dx
	asm mov al, ah
	asm out dx, al

/* dot it */
	asm mov dx, 0x0A000
	asm mov ds, dx
	asm pop di
	asm mov al, [di]
	asm mov [di], al

	asm pop ds
}

void pixels(UI where_x, UI where_y, UC *dot_buf, UC len)
{
	asm push ds
	asm mov ax, 0xA000
	asm mov ds, ax

/*  Calculate the start address of VRAM */
	asm mov ax, where_y    /* address: where_y*80+where_x/8	*/
	asm mov cl, 4
	asm shl ax, cl
	asm mov dx, ax
	asm shl ax, 1
	asm shl ax, 1
	asm add ax, dx
	asm mov dx, where_x
	asm mov cl, 3
	asm shr dx, cl
	asm add ax, dx
	asm mov di, ax

	asm mov dx, 0x3CE
	asm mov al, 8
	asm out dx, al
	asm inc dx

	asm xor cl, cl;
LOOP:
	_AL = *dot_buf;
	asm and al, al
	asm jz  NEXT

	asm out dx, al
	asm mov al, [di]
	asm mov [di], al
NEXT:
	asm inc di
	asm inc cl
	dot_buf ++;
	asm cmp cl, len
	asm jne LOOP

	asm pop ds
}

void set_dot_mode(UC color)
{
	asm mov dx, 0x3CE			/* Set register 0x3CE-00 to color */
	asm xor al, al
	asm out dx, al
	asm inc dx
	asm mov al, color
	asm out dx, al

	asm mov dx, 0x3CE			/* Set register 0x3CE-01 to 0x0F */
	asm mov al, 1
	asm out dx, al
	asm inc dx
	asm mov al, 0x0F
	asm out dx, al
}

void reset_dot_mode(void)
{
	asm mov dx, 0x3CE
	asm mov al, 0x08
	asm out dx, al
	asm inc dx
	asm mov al, 0xFF
	asm out dx, al

	asm mov dx, 0x3CE
	asm mov al, 1
	asm out dx, al
	asm inc dx
	asm xor ax, ax
	asm out dx, al
}

