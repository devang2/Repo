/* --------------------------------------------------------------------
	FILENAME:	gethz_2.c
	FUNCTION:	This modula offers some routines for get_hz_str().
				These routines are about the input-window.
	DATE:		1993.7.28
	AUTHOR:		Bob Kong.
   -------------------------------------------------------------------- */
#include <graphics.h>

#include <def.inc>
#include <func.inc>

extern UC   Get_bk_color;
extern UC   Get_char_color;
extern UC   Get_cursor_color;

extern UC   In_buf[81];
extern UI   Cur_hz, End_hz, Max_num;
extern UI	Char_sx, Char_sy;

/* -------------------------------------------------------------------------
	FUNCTION:	Display the flashing cursor of the input-window.
				The cursor is 16 dots width.
				The place where the cursor displayed is indicated by the
				global variables:	Char_sx, Char_sy, Cur_hz, End_hz, Max_num.
	CALLED BY:	Function get_hz_str();
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------- */
void disp_hz_cursor(void)
{
static UI count = 0;
static UC dark = TRUE;
UI cursor_x,cursor_y;

	count = ++count%6000;
	if (count==0)
	{
		if (dark)
			setcolor(Get_cursor_color);
		else
			setcolor(Get_bk_color);
		if (Cur_hz==Max_num)
		{
			cursor_x = Char_sx+(Cur_hz<<4)-16;
			cursor_y = Char_sy+16;
		}
		else
		{
			cursor_x = Char_sx+(Cur_hz<<4);
			cursor_y = Char_sy+16;
		}
		line(cursor_x, cursor_y, cursor_x+15, cursor_y);
		cursor_y++;
		line(cursor_x, cursor_y, cursor_x+15, cursor_y);
		dark = !dark;
	}
}

/* -------------------------------------------------------------------------
	FUNCTION:	Draw a cursor at the very place in the input-window.
				The place where the cursor drew is indicated by the global
				variables:	Char_sx, Char_sy, Cur_hz, End_hz, Max_num.
	CALLED BY:	Function get_hz_str();
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------- */
void draw_cursor(void)
{
UI cursor_x,cursor_y;

	setcolor(Get_cursor_color);
	if (Cur_hz==Max_num)
	{
		cursor_x = Char_sx+(Cur_hz<<4)-16;
		cursor_y = Char_sy+16;
	}
	else
	{
		cursor_x = Char_sx+(Cur_hz<<4);
		cursor_y = Char_sy+16;
	}
	line(cursor_x, cursor_y, cursor_x+15, cursor_y);
	cursor_y++;
	line(cursor_x, cursor_y, cursor_x+15, cursor_y);
}

/* -------------------------------------------------------------------------
	FUNCTION:	Clear the cursor at the very place in the input-window, which
				maybe exist.
				The place at where the cursor be cleared is indicated by the
				global variables:	Char_sx, Char_sy, Cur_hz, End_hz, Max_num.
	CALLED BY:	Function get_hz_str();
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------- */
void clr_cursor(void)
{
UI cursor_x,cursor_y;

	setcolor(Get_bk_color);
	if (Cur_hz==Max_num)
	{
		cursor_x = Char_sx+(Cur_hz<<4)-16;
		cursor_y = Char_sy+16;
	}
	else
	{
		cursor_x = Char_sx+(Cur_hz<<4);
		cursor_y = Char_sy+16;
	}
	line(cursor_x, cursor_y, cursor_x+15, cursor_y);
	cursor_y++;
	line(cursor_x, cursor_y, cursor_x+15, cursor_y);
}

/* ------------------------------------------------------------------------
	FUNCTION:	Delete the current HanZi in global variable: In_buf.
				The current HanZi means the HanZi with cursor underside.
				The display will justify accordingly.
	CALLS:		None.
	CALLED BY:	Function get_hz_str(). It is called when <Del> key pressed.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
void del_cur_hz(void)
{
UC cur_char_pos, end_char_pos, i;
UI block_sx, block_ex;

	if ( Cur_hz<End_hz )
	{
		cur_char_pos = Cur_hz<<1;
		end_char_pos = (End_hz<<1)+1;		 /* +2: include the tail zero */

		for ( i=cur_char_pos; i<end_char_pos; i++ )
			In_buf[i] = In_buf[i+2];

		block_sx = Char_sx+(Cur_hz<<4);
		block_ex = Char_sx+(End_hz<<4)-1;
		bar(block_sx, Char_sy, block_ex, Char_sy+17);
		hz16_disp(block_sx, Char_sy, In_buf+(Cur_hz<<1), Get_char_color);
		End_hz--;
		return;
	}
	if ( Cur_hz==Max_num )
	{
		clr_cursor();
		Cur_hz --;
		block_sx = Char_sx+(Cur_hz<<4);
		block_ex = block_sx+15;
		bar(block_sx, Char_sy, block_ex, Char_sy+17);
		In_buf[Cur_hz<<1] = 0;
		End_hz --;
		return;
	}
	sound_bell();	/* if (Cur_hz==End_hz && Cur_hz!=Max_num) sound_bell() */
}

/* ------------------------------------------------------------------------
	FUNCTION:	Delete the HanZi before the current HanZi.
				The display will justify accordingly.
	CALLS:		None.
	CALLED BY:	Function get_hz_str().
				It is called when the <BackSpace> key be pressed.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
void del_back_hz(void)
{
UC cur_char_pos, end_char_pos, i;
UI block_sx, block_ex;

	if (Cur_hz==0)
	{
		sound_bell();
		return;
	}
	if (Cur_hz==Max_num)
	{
		if (Max_num==1) return;

		clr_cursor();
		Cur_hz --;
		cur_char_pos = Cur_hz<<1;
		In_buf[cur_char_pos-2] = In_buf[cur_char_pos];
		In_buf[cur_char_pos-1] = In_buf[cur_char_pos+1];
		In_buf[cur_char_pos] = In_buf[cur_char_pos+2];

		block_sx = Char_sx+(Cur_hz<<4)-16;
		block_ex = Char_sx+(End_hz<<4)-1;
		bar(block_sx, Char_sy, block_ex, Char_sy+17);
		cur_char_pos -=2;
		hz_16(In_buf[cur_char_pos]-160, In_buf[cur_char_pos+1]-160,
			  block_sx, Char_sy, Get_char_color);
		End_hz--;
		Cur_hz--;
	}
	else
	{
		cur_char_pos = (Cur_hz<<1); 			/* if Cur_hz==Max_num, special */
		end_char_pos = (End_hz<<1)+1;			/* +1: include the tail zero */
		for (i=cur_char_pos;i<end_char_pos;i++)
			In_buf[i-2] = In_buf[i];

		block_sx = Char_sx+(Cur_hz<<4)-16;
		block_ex = Char_sx+(End_hz<<4)-1;
		bar(block_sx, Char_sy, block_ex, Char_sy+17);
		clr_cursor();
		Cur_hz --;
		hz16_disp(block_sx, Char_sy, In_buf+(Cur_hz<<1), Get_char_color);
		End_hz--;
	}
}

/* ------------------------------------------------------------------------
	FUNCTION:	Insert one HanZi at the current position before the current
				HanZi in buffer: In_buf.
				The display will justify accordingly.
	CALLS:		None.
	CALLED BY:	Function: ins_char(), get_hz_str(), py_input(), db_input(),
						  gb_input().
	INPUT:		qh, wh -- Indicate the HanZi to be insert.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
void ins_hz(UC qh, UC wh)
{
UC cur_char_pos, end_char_pos, i;
UI block_sx;

	if ( End_hz>=Max_num )
	{
		sound_alarm();
		return;
	}

	if (Cur_hz==End_hz)
	{
		cur_char_pos = Cur_hz<<1;
		In_buf[cur_char_pos] = qh+160;
		In_buf[cur_char_pos+1] = wh+160;
		In_buf[cur_char_pos+2] = 0;
		block_sx = Char_sx+(Cur_hz<<4);
		hz_16(qh, wh, block_sx, Char_sy, Get_char_color);
		clr_cursor();
		Cur_hz++;
		End_hz++;
	}
	else
	{
		cur_char_pos = Cur_hz<<1;
		end_char_pos = (End_hz<<1)+1;			/* +1: include the tail zero */

/* copy back 1 hz position, can not do it use for(;;) Statement */
		i = end_char_pos-1;
		while (i>cur_char_pos)
		{
			In_buf[i+2] = In_buf[i];
			i--;
		}
		In_buf[i+2] = In_buf[i];

		In_buf[cur_char_pos] = qh+160;
		In_buf[cur_char_pos+1] = wh+160;

		block_sx = Char_sx+(Cur_hz<<4);
		disp_str(block_sx, Char_sy, In_buf+(Cur_hz<<1), Get_bk_color, Get_char_color);
		End_hz++;
		clr_cursor();
		Cur_hz++;
	}
}

/* ------------------------------------------------------------------------
	FUNCTION:	Insert one 16*16 character at the current position before
				the current HanZi in buffer: In_buf.
				The display will justify accordingly.
	CALLS:		None.
	CALLED BY:	Function get_hz_str().
	INPUT:		mkey -- Indicate the character to be insert.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
void ins_char(int mkey)
{
UC qh, wh;

	if (mkey==' ')
	{	qh = 1;
		wh = 1;
	}
	else
	{
		qh = 03;
		wh = mkey-0x20;
	}
	ins_hz(qh,wh);
}
