
					  /* win_20.c */
/* ------------------------------------------------------------
	FILENAME:	win_20.c
	FUNCTION:	This modula offers a set of functions about
				Window management .
	DATE:		1993.4.6
	AUTHOR:		Fei min
	OTHER:
--------------------------------------------------------------*/

#include <stdio.h>
#include <graphics.h>
#include <stdarg.h>

#include <def.inc>
#include <func.inc>

static UI posi_x  =0;
static UI posi_y  =0;  /* current position of cursor in window. */
static UI start_x =0;
static UI start_y =0;  /* start position of the window in screen. */
static UI col     =80; /* the number of column in a row. */
static UI row     =24; /* the number of row in the window. */
static UC Color   =7;  /* the back color of the window. */
static UI win_data[7]=
	   {0,0,0,0,80,24,7}; /* backup of the parameter of the first window */
/* --------------------------------------------------------------------
	FUNCTION:	roll up the screen within window.
	   CALLS:	No
   CALLED BY:	scr_up ();
				crlf   ();
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------------- */
static void scrollup (void)
{
UC	mode;
UI	start;
UI	colum;
UI	now_dest;
UI	line_num;

	colum	 =col;
	start	 =(start_x>>3)+start_y*80;
	now_dest =start;
	line_num =(row-1)*20;
	_BX      =line_num;
	_DI		 =now_dest;
	_SI		 =now_dest+1600;

			asm mov		al,5
			asm mov		dx,3ceh
			asm out		dx,al		/*  select MODE_REG. of  */
									/*	Graph.Control.Reg    */
			asm mov		dx,3cfh
			asm in		al,dx
			asm mov		mode,al
			asm or		al,09
			asm	out		dx,al		/*  set mode_register W & R mode=1  */

			asm cld
			asm mov		ax,0a000h
			asm mov		es,ax
			asm mov		ds,ax

	bgn:
			asm mov		cx,colum
			asm rep		movsb		/*  a line of 4 page be moved	*/

			now_dest+=80;
			asm	mov		di,now_dest
			asm	mov		si,now_dest
			asm add		si,1600		/*  next line           */

			asm	dec		bx
			asm	jnz		bgn			/*  if last line be moved? */

			asm mov		al,5
			asm mov		dx,3ceh
			asm out		dx,al

			asm mov		dx,3cfh
			asm mov		al,mode
			asm	out		dx,al		/*	restore the dispaly mode	*/
}
/* -------------------------------------------------------------
	FUNCTION:	roll down the screen within window.
	   CALLS:	No
   CALLED BY:	scr_down ();
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------- */
static void scrolldown (void)
{
UC	mode;
UI	start;
UI	colum;
UI	now_dest;
UI	line_num;

	colum	 =col;
	start	 =(start_x>>3) +start_y*80 +(20*row-1)*80;
	now_dest =start;
	line_num =(row-1)*20;
	_BX      =line_num;
	_DI		 =now_dest;
	_SI		 =now_dest-1600;

			asm mov		al,5
			asm mov		dx,3ceh
			asm out		dx,al		/*  select MODE_REG. of  */
									/*	Graph.Control.Reg    */
			asm mov		dx,3cfh
			asm in		al,dx
			asm mov		mode,al
			asm or		al,09
			asm	out		dx,al		/*  set mode_register W & R mode=1  */

			asm cld
			asm mov		ax,0a000h
			asm mov		es,ax
			asm mov		ds,ax

	bgn:
			asm mov		cx,colum
			asm rep		movsb		/*  a line of 4 page be moved	*/

			now_dest-=80;
			asm	mov		di,now_dest
			asm	mov		si,now_dest
			asm sub		si,1600		/*  next line           */

			asm	dec		bx
			asm	jnz		bgn			/*  if last line be moved? */

			asm mov		al,5
			asm mov		dx,3ceh
			asm out		dx,al

			asm mov		dx,3cfh
			asm mov		al,mode
			asm	out		dx,al		/*	restore the dispaly mode	*/
}
/* -------------------------------------------------------------
	FUNCTION:	roll up the screen within window & clear last row.
	   CALLS:	scrollup ()
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------- */
void scr_up (void)
{
	scrollup ();
	setfillstyle (SOLID_FILL,Color);
	bar (start_x,start_y+(row-1)*20,start_x+col*8,start_y+row*20);
}
/* -------------------------------------------------------------
	FUNCTION:	roll down the screen within window & clear first row.
	   CALLS:	scrolldown ()
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------- */
void scr_down (void)
{
	scrolldown ();
	setfillstyle (SOLID_FILL,Color);
	bar (start_x,start_y,start_x+col*8,start_y+19);
}
/* --------------------------------------------------------------------
	FUNCTION:	display char_string & data at current cursor position
			in window according to format of printf (); if it is
			the last row screen will be roll up.
        CALLS:	        scrollup ()
        CALLED BY:      this function belong to the set of basic function.
	INPUT:	        back_color,  background color for displaying
			fore_color,  foreground color for displaying
			*format, format string, followed by a variable number of
			arguments
	OUTPUT:	        No
	RETURN:	        No
---------------------------------------------------------------------- */
void woutf(UC back_color, UC fore_color, const UC *format, ...)
{
static UC buffer[80];
va_list argptr;

	va_start (argptr, format);
	vsprintf (buffer, format, argptr);
	va_end   (argptr);
	if (posi_y<(start_y+1+(row-1)*20))
	{
		disp_str (posi_x, posi_y, buffer, back_color, fore_color);
		posi_y+=20;
	}
	else
	{
		scrollup();
		setfillstyle (SOLID_FILL,Color);
		bar (start_x,start_y+(row-1)*20,start_x+col*8,start_y+row*20);
		disp_str (posi_x, posi_y-20, buffer, back_color, fore_color);
	}
	posi_x =start_x;
}
/* --------------------------------------------------------------------
	FUNCTION:	display char_string & data at current cursor position
				in window according to format of printf ().
	   CALLS:	No
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	back_color,  background color for displaying
				fore_color,  foreground color for displaying
				*format,     format string, followed by a variable number of
							 arguments
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------------- */
void coutf(UC back_color, UC fore_color, const UC *format, ...)
{
	static UC buffer[80];
	va_list argptr;

	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	if (posi_y<(start_y+1+(row-1)*20))
		disp_str (posi_x, posi_y, buffer, back_color, fore_color);
	else
		disp_str (posi_x, posi_y-20, buffer, back_color, fore_color);

}
/* --------------------------------------------------------------------
	FUNCTION:	locate position of cursor in window
	   CALLS:	No
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	x ---  X coordinate of cursor in window.
				y ---  Y coordinate of cursor in window.
	  OUTPUT:	No
	  RETURN:	0 -- x, y are invalid.
				1 -- x, y are correct.
---------------------------------------------------------------------- */
UC locate (UI x,UI y)
{
	if	(((start_x +x*8)>(start_x+col*8-16))
		||((start_y+y*20)>(start_y+(row-1)*20)))
		return	0;
	posi_x =start_x+x*8;
	posi_y =start_y+y*20;
	return	1;
}
/* --------------------------------------------------------------------
	FUNCTION:	locate position of cursor in window, roll up screen of window
				if it is last row, locate cursor at beginning of next row if
				it's not last row.
	   CALLS:	scrollup ();
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------------- */
void crlf(void)
{
	posi_x=start_x;
	if (posi_y>=(start_y+(row-1)*20))
	{
		scrollup();
		setfillstyle (SOLID_FILL,Color);
		bar (start_x,start_y+(row-1)*20,start_x+col*8,start_y+row*20);
	}
	else
		posi_y+=20;
}
/* --------------------------------------------------------------------
	FUNCTION:	set window's size, position, back color
	   CALLS:	No
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	x      --- X coordinate of window in screen.
				y      --- Y coordinate of window in screen.
				win_col -- the number of column in a row.
				win_row -- the number of row in the window.
	  OUTPUT:	No
	  RETURN:	No
---------------------------------------------------------------------- */
void set_window (UI x,UI y,UC win_col,UC win_row,UC color)
{
	if	(x%8 != 0)
		x=(x/8)*8+8;
	if	(x >(640-16))
		x=0;
	if	(y >(480-20))
		y=0;
	if	((x+win_col*8)>639)
		win_col=(640-x)/8;
	if	((y+win_row*20)>480)
		win_row=(480-y)/20;

	start_x =x;
	start_y =y;
	posi_x  =start_x;
	posi_y  =start_y;
	col     =win_col;
	row     =win_row;
	Color   =color;
}
/* -------------------------------------------------------------
	FUNCTION:	save window's size, position, back color, etc.
	   CALLS:	No
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
--------------------------------------------------------------- */
void save_window (void)
{
	win_data[0] =posi_x;
	win_data[1] =posi_y;
	win_data[2] =start_x;
	win_data[3] =start_y;
	win_data[4] =col;
	win_data[5] =row;
	win_data[6] =Color;
}
/* ------------------------------------------------------------------
	FUNCTION:	restore last window's size, position, back color, etc.
	   CALLS:	No
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
-------------------------------------------------------------------- */
void restore_window (void)
{
	posi_x   =win_data[0];
	posi_y   =win_data[1];
	start_x  =win_data[2];
	start_y  =win_data[3];
	col      =win_data[4];
	row      =win_data[5];
	Color    =win_data[6];
}
/* ------------------------------------------------------------------
	FUNCTION:	clear window with back color.
	   CALLS:	No
   CALLED BY:   this function belong to the set of basic function.
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
-------------------------------------------------------------------- */
void cls_window (UC color)
{
	posi_x=start_x;
	posi_y=start_y;
	if	(color==0xff)
		setfillstyle (SOLID_FILL, Color);
	else
		setfillstyle (SOLID_FILL, color);
	bar (start_x,start_y,start_x+col*8,start_y+row*20);
}
/* ----------------------- 4 page mode  -----------------------------
	FUNCTION:	roll up the screen within window.
	   CALLS:	No
   CALLED BY:	scr_up ();
				crlf   ();
	   INPUT:	No
	  OUTPUT:	No
	  RETURN:	No
------------------------------------------------------------------- */
/*
void scrollup (void)
{
UI	start;
UI	now_dest;
UI	line_num;
UI	colum;
UI	w_r_page;

	colum	=col>>1;
	line_num=(row-1)*20;
	start	=(start_x>>3)+start_y*80;
	now_dest=start;
	w_r_page=0x80ff;

			asm	mov		bx,line_num

			asm	mov		al,2
			asm	mov		dx,3c4h
			asm	out		dx,al
			asm	mov		al,4
			asm	mov		dx,3ceh
			asm	out		dx,al				init. index regester

			asm cld
			asm mov		ax,0a000h
			asm mov		es,ax
			asm mov		ds,ax

	bgn:	asm	mov		ax,w_r_page
			asm	inc		al
			asm	rol		ah,1
			asm	mov		w_r_page,ax
			asm mov		dx,3cfh
			asm out		dx,al
			asm mov		dx,3c5h
			asm	mov		al,ah
			asm out		dx,al	       	open next page be written and open next page be read

			asm	mov		di,now_dest
			asm	mov		si,now_dest
			asm	add		si,1600
			asm mov		cx,colum
			asm rep		movsw			read and write a line in a page

			asm	cmp		al,08		  if a line of the 4th page be moved?
			asm	jne		bgn

			now_dest+=80;
			asm	mov		di,now_dest
			asm	mov		si,now_dest
			asm add		si,1600
			w_r_page=0x80ff;			next line

			asm	dec		bx
			asm	jnz		bgn			  if last line be moved?

			asm	mov		ax,0f00h
			asm mov		dx,3cfh
			asm out		dx,al
			asm mov		dx,3c5h
			asm	mov		al,ah
			asm out		dx,al			restore the dispaly mode
}
*/