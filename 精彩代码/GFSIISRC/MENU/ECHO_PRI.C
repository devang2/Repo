/* -----------------------------------------------------------------------
	FILENAME:	ECHO_PRI.C
	FUNCTION:	Offer the function echo_privi(), which can echo the current
			priviledge.
	AUTHOR:		Bob Kong.
	DATE:		1993-08-08
   ----------------------------------------------------------------------- */
#include <graphics.h>

#include <def.inc>
#include <func.inc>
#include "menu_def.inc"

extern UC Privi;

void echo_privi(void)
{
UI x1 = T_EX+6;
UI x2 = MAX_X-5;
UI y1 = 5;
UI y2 = S_BY-6;

	switch (Privi)
	{
	case 0:
		setfillstyle(SOLID_FILL, 2);
		bar(x1, y1, x2, y2);
		hz16_disp( x1+(x2-x1-4*16)/2, 8, "操作有权", 3);
		break;

	case 1:
		setfillstyle(SOLID_FILL, 1);
		bar(x1, y1, x2, y2);
		hz16_disp( x1+(x2-x1-4*16)/2, 8, "设定有权", 14);
		break;

	case 2:
		setfillstyle(SOLID_FILL, 4);
		bar(x1, y1, x2, y2);
		hz16_disp( x1+(x2-x1-4*16)/2, 8, "管理有权", 14);
		break;

	default:
		;
	}
}

