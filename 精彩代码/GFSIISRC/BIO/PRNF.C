/* ---------------------------------------------------------------------
	FILENAME:	prnf.c
	FUNCTION:	Offers a few functions about printer.
	DATE:		1993/5/15
	AUTHOR:		Bob Kong
   --------------------------------------------------------------------- */
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include <def.inc>
#include <func.inc>

#define INIT_OK		0x10
#define PRN_BUSY	0x10
#define PRN_RDY		0x90
#define NO_PAPER	0x38
#define OFFLINE		0x18

#define WIN_BK_COLOR 	7
#define WIN_CHAR_COLOR	0
#define WIN_X		240
#define	WIN_Y		250

#define MAX_DELAY	35
#define OFFLIN_TIME	0xF000
#define ERR_TIME	30
#define BUSY_TIME	0x40000L

extern UL far 	 *Tick_cnt_ptr;
static UI Win_x = WIN_X;
static UI Win_y = WIN_Y;
static UI Win_bk_color = WIN_BK_COLOR;
static UI Win_char_color = WIN_CHAR_COLOR;

/* --------------------------------------------------------------------
	FUNCTION:	set the error-window's position to (x, y) when printing.
	CALLS:		None.
	CALLED BY:	This is an general-purposed routine.
	INPUT:		(x, y):	the left-top Coord. of the error-window.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void set_prn_mode(UI x, UI y, UC bk_color, UC char_color)
{
	Win_x = x;
	Win_y = y;
	Win_bk_color = bk_color;
	Win_char_color = char_color;
}

/* --------------------------------------------------------------------
	FUNCTION:	reset the error-window's position to (WIN_X, WIN_Y).
	CALLS:		None.
	CALLED BY:	This is an general-purposed routine.
	INPUT:		(x, y):	the left-top Coord. of the error-window.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void reset_prn_mode(void)
{
	Win_x = WIN_X;
	Win_y = WIN_Y;
	Win_bk_color = WIN_BK_COLOR;
	Win_char_color = WIN_CHAR_COLOR;
}

/* --------------------------------------------------------------------
	FUNCTION:	Initiate the printing port.
				it needs about 2 seconds
	CALLS:		None.
	CALLED BY:	This is a general-purposed routined.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   --------------------------------------------------------------------- */
UC init_prn(void)
{
UL time1;
UI time;

	_AH = 1;
	_DX = 0;
	geninterrupt(0x17);

	time1 = *Tick_cnt_ptr;
	time = (UI)(*Tick_cnt_ptr - time1);
	while (time<MAX_DELAY)
	{
		check_event_flag();
		time = (UI)(*Tick_cnt_ptr - time1);
	}

	_AH = 2;
	_DX = 0;
	geninterrupt(0x17);
	return (_AH==PRN_RDY);
}

/* ----------------------------------------------------------------------
	FUNCTION:	This function prints a character.
	CALLS:		None.
	CALLED BY:	This is a general-purposed routine.
	INPUT:		ch		The character to be printed.
	OUTPUT:		None.
	RETURN:		0 -- if prints string ok.
				1 -- if error occurred, and the user selects quit.
   ---------------------------------------------------------------------- */
UC prn_ch(UC ch)
{
static UC *msg1 = "    打印机错误，请检查打印机。";
static UC *msg2 = "    打印机未联机，请检查打印机。";
static UC *msg3 = "    打印机无纸，请检查打印机。";
static UC *selections[2] = {"重    试", "中止打印"};

UC  *prompt;
UC  stat;
UC  result;
UI  err_count;
UI  offlin_count;
UL  busy_count;

	for ( ; ; )
	{
		stat = 0;
		err_count = 0;
		offlin_count = 0;
		busy_count = 0;
		while ( (err_count<ERR_TIME) && (offlin_count<OFFLIN_TIME)
				&& (busy_count<BUSY_TIME) )
		{
			check_event_flag();

			_AH = 2;
			_DX = 0;
			geninterrupt(0x17);
			stat = _AH;
			if (stat==PRN_RDY)
				break;
			if (stat==OFFLINE)
			{
				offlin_count++;
				continue;
			}
			if (stat==PRN_BUSY)
			{
				busy_count++;
				continue;
			}
			err_count++;
		}		/* end of while(...) */

		if (stat==PRN_RDY)
			break;
		switch (stat)
		{
		case NO_PAPER:
			prompt = msg3;
//			prompt = msg1;
			break;
		case OFFLINE:
			prompt = msg2;
//                        prompt = msg1;
			break;
		default:
			prompt = msg1;
		}

		message_disp(8," ← → 选择   Enter 选中");    /*left right*/
		set_menu(Win_bk_color, Win_char_color, TRUE, FALSE);
		result=hv_menu(Win_x, Win_y, 2, 2, prompt, selections);
		message_end();

		if ( result==2 )
		{
			return FALSE;
		}
	}		/* end of for( ; ; ) */

	_AH = 0;
	_DX = 0;
	_AL = ch;
	geninterrupt(0x17);

	return TRUE;
}


/* ----------------------------------------------------------------------
	FUNCTION:	This function prints a string with a tail '\0'.
	CALLS:		None.
	CALLED BY:	This is a general-purposed routine.
	INPUT:		*buffer		The string's buffer.
	OUTPUT:		None.
	RETURN:		0 -- if prints string ok.
				1 -- if error occurred when printing string, and the user
					 selects quit.
   ---------------------------------------------------------------------- */
UC prns(UC *buffer)
{
UI  char_pos;
UC  prn_stat = TRUE;

	char_pos = 0;
	while ( buffer[char_pos]!=0 && prn_stat==TRUE )
		prn_stat = prn_ch( buffer[char_pos++] );

	return prn_stat;
}

/* ----------------------------------------------------------------------
	FUNCTION:	This function prints a fixed-length string.
	CALLS:		None.
	CALLED BY:	This is a general-purposed routine.
	INPUT:		*buffer		The string's buffer.
	OUTPUT:		None.
	RETURN:		0 -- if prints string ok.
				1 -- if error occurred when printing string, and the user
					 selects quit.
   ---------------------------------------------------------------------- */
UC prn_fs(UC *buffer, UI length)
{
UI  char_pos;
UC  prn_stat = TRUE;

	char_pos = 0;
	while ( char_pos<length && prn_stat==TRUE )
		prn_stat = prn_ch( buffer[char_pos++] );

	return prn_stat;
}

/* -------------------------------------------------------------------
	FUNCTION:	Clear the buffer of printer.
	CALLS:		None.
	CALLED:		This is a general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------- */
UC clr_prnbuf(void)
{
	return prn_ch(0x18);
}

/* -------------------------------------------------------------------
	FUNCTION:	Cause the printer carriage to return and feed line.
	CALLS:		None.
	CALLED:		This is a general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------- */
UC prn_crlf(void)
{
	if ( prn_ch(0x0D)==FALSE )
		return FALSE;
	if ( prn_ch(0x0A)==FALSE )
		return FALSE;
	return TRUE;
}

/* ----------------------------------------------------------------------
	FUNCTION:	This function print a formated string.
	CALLS:		None.
	CALLED BY:	This is a general purposed routine.
	INPUT:		*format, ...	Format string, followed by a variable
								number of arguments.
	OUTPUT:		None.
	RETURN:		0 -- if prints string ok.
				1 -- if error occurred when printing string, and the user
					 selects quit.
   ---------------------------------------------------------------------- */
/*UC   prnf(const UC *format, ...)
{
static  UC buffer[255];
va_list argptr;
UI length;

	va_start(argptr, format);
	length = vsprintf(buffer, format, argptr);
	va_end(argptr);

	if (length==(UI)EOF)
		return FALSE;
	return prn_fs(buffer, length);
}
*/