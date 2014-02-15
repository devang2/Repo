void screen_off(void)
{
	asm mov dx, 0x3C4
	asm xor ax, ax
	asm out dx, al
	asm inc dx
	asm inc al
	asm out dx, al
	asm dec dx
	asm out dx, al
	asm inc dx
	asm mov al, 0x23
	asm out dx, al
	asm dec dx
	asm xor ax, ax
	asm out dx, al
	asm inc dx
	asm mov al, 3
	asm out dx, al
}

void screen_on(void)
{
	asm mov dx, 0x3C4
	asm xor ax, ax
	asm out dx, al
	asm inc dx
	asm inc al
	asm out dx, al
	asm dec dx
	asm out dx, al
	asm inc dx
	asm mov al, 0x3
	asm out dx, al
	asm dec dx
	asm xor ax, ax
	asm out dx, al
	asm inc dx
	asm mov al, 3
	asm out dx, al
}

