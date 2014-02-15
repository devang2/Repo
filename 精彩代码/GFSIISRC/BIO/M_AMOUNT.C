/* ------------------------------------------------------------------
	FILENAME:	M_AMOUNT.C
	FUNCTION:	Offers two function: M_amount() & M_available().
	AUTHOR:		Bob Kong.
	DATE:		1993-05-25.
   ------------------------------------------------------------------ */
#include <dos.h>

#include <def.inc>

/* ------------------------------------------------------------------
	FUNCTION:	Obtains the total conventional memory amount.
	CALLS:		None.
	CALLED BY:	This is an general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		The amount of the conventional memory.
   ------------------------------------------------------------------ */
UI M_amount(void)
{
	geninterrupt(0x12);
	return _AX;
}
/* ------------------------------------------------------------------
	FUNCTION:	Obtains the conventional memory amount which haven't
				been allocated.
	CALLS:		None.
	CALLED BY:	This is an general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		The amount of the Non-allocated conventional memory.
   ------------------------------------------------------------------ */
UL M_available(void)
{
UL tmp_value;

	_BX = 0xFFFF;
	_AH = 0x48;
	geninterrupt(0x21);
	tmp_value = _BX;

	tmp_value <<= 4;
	return tmp_value;
}
