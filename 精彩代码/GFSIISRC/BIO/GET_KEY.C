#include <dos.h>

#include <def.inc>
#include <func.inc>

/* --------------------------- keybuf_nul() -------------------------- */
UC keybuf_nul()
{
	check_event_flag();

	return (kb_n());
}

/* ---------------------------- clr_keybuf() ---------------------------- */
void clr_keybuf()
{
UI far *kb_head = (UI far *)0x0040001aL;
UI far *kb_tail = (UI far *)0x0040001cL;

	check_event_flag();

	*kb_head = *kb_tail;
}
/* ----------------------------- get_key0() ----------------------------- */
UI get_key0()
{

	check_event_flag();

	_AH = 0;
	geninterrupt(0x16);
	return (_AX);

}

/* ----------------------------- get_key1() ----------------------------- */
UI get_key1()
{
UI far *kb_head = (UI far *)0x0040001aL;
UI far *kb_tail = (UI far *)0x0040001cL;

	*kb_head = *kb_tail;
	while (kb_n())
		check_event_flag();

	_AH = 0;
	geninterrupt(0x16);
	return(_AX);
}

