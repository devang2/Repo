# include <dos.h>		/* geninterrupt MK_FP en/disable */

# include <def.inc>
# include <func.inc>

/* ------------------------------- kb_n() -------------------------------- */
UC kb_n(void)
{
	asm mov ah,1;
	asm int 0x16;
	asm jz no_key;
	return FALSE;

no_key:
	return TRUE;
}

/* ------------------------------ clr_kb() ------------------------------ */
void clr_kb(void)
{
UI far *kb_head = (UI far *)0x0040001aL;
UI far *kb_tail = (UI far *)0x0040001cL;

	*kb_head = *kb_tail;

}
/* ----------------------------- get_k0() ------------------------------ */
UI get_k0(void)
{
	_AH = 0;
	geninterrupt(0x16);
	return(_AX);
}

/* ----------------------------- get_k1() ------------------------------ */
UI get_k1(void)
{
UI far *kb_head = (UI far *)0x0040001aL;
UI far *kb_tail = (UI far *)0x0040001cL;

	*kb_head = *kb_tail;
	while (kb_n())
		;
	_AH = 0;
	geninterrupt(0x16);
	return(_AX);
}

