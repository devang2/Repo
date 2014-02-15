/* -------------------------------------------------------------------
	SOURCE:	    RECOVER.C
	FUNCTION:   void recover_screen(UC mode )
---------------------------------------------------------------------- */

#include <graphics.h>

#include <def.inc>
#include <menufunc.inc>

#include "menu_def.inc"

extern UC  H_status;
extern UC  Pop_down1;
extern UC  Pop_down2;			/* draw_scr.C */
char patterns[][8] = {
      { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 },
//    { 0xAA, 0xA5, 0x55, 0xAA, 0xA5, 0x55, 0xAA, 0xA5 },
//    { 0x33, 0x33, 0xCC, 0xCC, 0x33, 0x33, 0xCC, 0xCC },
//    { 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F },
//    { 0x00, 0x10, 0x28, 0x44, 0x28, 0x10, 0x00, 0x00 },
//    { 0x00, 0x70, 0x20, 0x27, 0x24, 0x24, 0x07, 0x00 },
//    { 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00 },
//    { 0x00, 0x00, 0x3C, 0x3C, 0x3C, 0x3C, 0x00, 0x00 },
//    { 0x00, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x7E, 0x00 },
//    { 0x00, 0x00, 0x22, 0x08, 0x00, 0x22, 0x1C, 0x00 },
//    { 0xFF, 0x7E, 0x3C, 0x18, 0x18, 0x3C, 0x7E, 0xFF },
//    { 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00 },
//    { 0x00, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x00 }
  };


/* ============================ clr_win() =============================== */
void clr_DialWin(UC code)
{
	switch(code)
	{
	case 0:	/* ---- reset dialogue window without border ---- */
                setfillpattern( &patterns[0][0], 15 );
                bar(H_BX-10,H_BY-45,H_BX+H_X_M*H_XAD+7,H_BY+(H_Y_M+1)*H_YAD+7);
		break;

	case 1:	/* ---- reset dialogue window without border ---- */
//              setfillpattern( &patterns[0][0], 15 );
//		setfillstyle(SOLID_FILL, 7);
//		bar(D_LEFT, D_TOP+1, D_RIGHT, D_BOTTOM);
		draw_back(D_LEFT+2, D_TOP+2, D_RIGHT-2, D_BOTTOM-2, 7);
		break;

	case 2: /* M_BY: BY of main menu, D_BOTTOM: bottom of diag_win */
		draw_back(0,M_BY+1,MAX_X,D_BOTTOM-2,7);
		break;

	default:
		break;
	}
}

/* =========================== recover_screen() ========================= */
void recover_screen(UC mode)
{
	switch(mode)
	{
	case 0: /* ---- reset dialogue window without border ---- */
//                setfillpattern( &patterns[0][0], 15 );
//		bar(D_LEFT, D_TOP+1, D_RIGHT, D_BOTTOM);
		if (H_status)
			init_head_menu();
		else
		if(Pop_down1)
		{
			init_gen_menu();
			if(Pop_down2)
				init_zen_menu();
		}
		break;

	case 1: /* ---- reset dialogue window without border ---- */
//		clr_DialWin(1);
                setfillpattern( &patterns[0][0], 15 );
		bar(D_LEFT, D_TOP+1, D_RIGHT, D_BOTTOM);
		if (H_status)
			init_head_menu();
		else
		if(Pop_down1)
		{
			init_gen_menu();
			if(Pop_down2)
				init_zen_menu();
		}
		break;

	case 2:	/* ---- reset the dialogue window border ---- */
//		setfillstyle(SOLID_FILL, 7);
                setfillpattern( &patterns[0][0], 15 );
		bar(0, D_TOP+1, D_RIGHT, D_BOTTOM);

		init_main_menu();

//		setfillstyle(SOLID_FILL, 7);
//		bar(D_LEFT, D_TOP+1, D_RIGHT, D_BOTTOM);

		if (H_status)
			init_head_menu();
		else
		if(Pop_down1)
		{
			init_gen_menu();
			if (Pop_down2)
				init_zen_menu();
		}
		break;
/*
	case 2:
		/* ---- reset the dialogue window with border ---- */
		draw_back(D_LEFT, D_TOP, D_RIGHT, D_BOTTOM, 1);
		if (H_status)
			init_head_menu();
		else if(Pop_down1)
		{
			init_gen_menu();
			if(Pop_down2)
				init_zen_menu();
		}
		break;
*/
	default:
		break;
	}
}

