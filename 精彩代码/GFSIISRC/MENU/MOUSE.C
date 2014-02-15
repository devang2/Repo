/* --------------------------------------------------------------------
	FILENAME: 	mouse.c
	FUNCTION:	Offers some basic service routines about mouse.
	AUTHOR:
	LAST MODIFY:1993-06-24
	MODIFIED BY:Bob kong.
   -------------------------------------------------------------------- */
#include <dos.h>
#include <def.inc>

#include <func.inc>
#include <menufunc.inc>

#define  LEFTB       1
#define  RIGHTB      2
#define  BOTHB       3
#define  NOT_MOVED   0
#define  LEFTMV      1
#define  RIGHTMV     2
#define  UPMV        3
#define  DOWNMV      4

UC Cursor_on	=	FALSE;

static UI Band_sx = 0;
static UI Band_sy = 0;
static UI Band_ex = MAX_X-10;
static UI Band_ey = MAX_Y-12;
static union REGS ireg,oreg;

UC mouse_reset(void)
{
	ireg.x.ax = 0;
	int86(0x33, &ireg, &oreg);
	if ( oreg.x.ax != 0xff )
	{
		return(FALSE);
	}
	if ( oreg.x.bx != 2 )
	{
		return(FALSE);
	}
	return(TRUE);
}

UC left_button_pressed(void)
{
	check_event_flag();
	ireg.x.ax = 3;
	int86(0x33, &ireg, &oreg);
	return(oreg.x.bx&0x01);
}

UC right_button_pressed(void)
{
	check_event_flag();
	ireg.x.ax = 3;
	int86(0x33, &ireg, &oreg);
	return(oreg.x.bx&0x02);
}

UC get_pressed_button(void)
{
 	ireg.x.ax=3;
	int86(0x33, &ireg, &oreg);
	if ( (oreg.x.bx&0x03)==1 ) return(LEFTB);
	if ( (oreg.x.bx&0x03)==2 ) return(RIGHTB);
	if ( (oreg.x.bx&0x03)==3 ) return(BOTHB);
	return(FALSE);
}

UC left_pressed(void)
{
	if ( left_button_pressed() )
	{
		while ( left_button_pressed() )
			;
		return(TRUE);
	}
	else
		return(FALSE);
}

UC right_pressed(void)
{
	if ( right_button_pressed() )
	{
		while ( right_button_pressed() )
			;
		return(TRUE);
	}
	else
		return(FALSE);
}

void set_mouse_position(UI x, UI y)
{
	ireg.x.ax = 4;
	ireg.x.cx = x;
	ireg.x.dx = y;
	int86(0x33, &ireg, &oreg);
}

void get_mouse_position(UI *x, UI *y)
{
	ireg.x.ax = 3;
	int86(0x33, &ireg, &oreg);
	*x = oreg.x.cx;
	*y = oreg.x.dx;
}

void cursor_on(void)
{
	if (Cursor_on)
		return;
	ireg.x.ax = 1;
	int86(0x33, &ireg, &oreg);
	Cursor_on = TRUE;
}

void cursor_off(void)
{
	if ( !Cursor_on )
		return;
	ireg.x.ax = 2;
	int86(0x33, &ireg, &oreg);
	Cursor_on = FALSE;
}

void set_mickey_rate(UI x, UI y)
{
	ireg.x.ax = 15;
	ireg.x.cx = x;
	ireg.x.dx = y;
	int86(0x33, &ireg, &oreg);
}

void set_vertical_band(UI y1, UI y2)
{
	ireg.x.ax = 8;
	ireg.x.cx = y1;
	ireg.x.dx = y2;
	int86(0x33, &ireg, &oreg);
	Band_sy = y1;
	Band_ey = y2;
}

void set_horizontal_band(UI x1, UI x2)
{
	ireg.x.ax = 7;
	ireg.x.cx = x1;
	ireg.x.dx = x2;
	int86(0x33, &ireg, &oreg);
	Band_sx = x1;
	Band_ex = x2;
}

void get_mouse_band(MOUSE_BAND_STRUCT *mouse_band)
{
	mouse_band->sx = Band_sx;
	mouse_band->sy = Band_sy;
	mouse_band->ex = Band_ex;
	mouse_band->ey = Band_ey;
}

void set_mouse_band(MOUSE_BAND_STRUCT *mouse_band)
{
	set_vertical_band(mouse_band->sy, mouse_band->ey);
	set_horizontal_band(mouse_band->sx, mouse_band->ex);
}

UC init_mouse(void)
{
	set_horizontal_band(0, MAX_X-10);
	set_vertical_band(0, MAX_Y-12);
	return(TRUE);
}
