/* -------------------------------------------------------------------
	FILENAME:	msg_win.c
	FUNCTION:	This is the source file of the message popup function.
	AUTHOR:		Bob kong.
	DATE:		1993-06-18
   ------------------------------------------------------------------- */
#include<stdlib.h>
#include <string.h>
#include <alloc.h>
#include <graphics.h>
#include <dos.h>

#include <def.inc>
#include <func.inc>
#include <menufunc.inc>

extern UC Cursor_on;

void getmsgsize(UC *msg, UI *msgW, UI *msgH);
void disp_msg(UI x, UI y, UC *msg);

extern UL far *Tick_cnt_ptr;

static UI mode_settled = 0;
static UC wbk_c, wtt_c, msg_c;

/* ---------------------------------------------------------------------
	The following macros define the default colors of the window.
------------------------------------------------------------------------ */
#define WBK_C	2 		/* background color	*/
#define WTT_C	12		/* title color		*/
#define MSG_C	15		/* message color	*/

/* ----------------------------------------------------------------------
	The following macros defined the size of characters and Chinese words
	as well as the line space. All measured in points.
------------------------------------------------------------------------- */
#define CHAR_W  8				/* width of a character    */
#define HZ_W	16		        /* width of a Chinese word */
#define CHAR_H	16				/* height of a character or Chinese word */
#define LN_SP	8				/* line space			   */
#define LN_H	(CHAR_H+LN_SP)	/* line height			   */
#define BORDER	3				/* border width			   */

/* ----------------------------------------------------------------------
	FUNCTION:	This function will pop up an error message window.
				This function will automatically adjust the window size
				to fill in the messages. If one line is too long, it will be
				seperated automatically and the window width will be ajust
				to maxium width of the screen.
	CALLS:
	CALLED BY:	This is a general-purposed routine.
	INPUT:		(x, y):		The coordinate of the left-top corner of
							the screen to pop up the window.
				title:		String pointer to the window title.
				msg:		String pointer to the messages that will be
							showed in the window.
							The message string may be multilined, and
							seperated by '\n'.
	RETURN: 	None.
   ------------------------------------------------------------ */
void msg_win(UI x, UI y, UC *title, UC *msg, UC echo_time)
{
UI left, top;
UI right, bottom;
UI msg_w, msg_h;
UI wn_w, wn_h;
UL start_time;
UC old_cursor_stat;
MOUSE_BAND_STRUCT	mouse_band;
UI old_color;
struct fillsettingstype old_fl;
struct linesettingstype old_ln;
struct textsettingstype save_text;

	old_color = getcolor();
	getfillsettings(&old_fl);
	getlinesettings(&old_ln);

	if (!mode_settled)
	{
		wbk_c = WBK_C;
		wtt_c = WTT_C;
		msg_c = MSG_C;
	}
	mode_settled = 0;

  /* -- determine the window size -- */
	getmsgsize(msg, &msg_w, &msg_h);
	wn_w = (msg_w + 8)*CHAR_W + BORDER*2;
	wn_w = min(MAX_X, max(wn_w, HZ_W*(strlen(title)+3)));
	wn_h = (msg_h + 1)*LN_H+BORDER*2;
	if (*title)
		wn_h += LN_H+8;
	wn_h = min(MAX_Y, wn_h);

  /* -- calculate the coordination of the top-left and bottom-right corner
		of the window.
  */
	left = x;
	right = left + wn_w;
	if (right > MAX_X-5)
	{
		right = MAX_X-5;
		left = right - wn_w;
	}

	top = y;
	bottom = top + wn_h;
	if (bottom > MAX_Y-5)
	{
		bottom = MAX_Y-5;
		top = bottom - wn_h;
	}

	old_cursor_stat = Cursor_on;
	get_mouse_band(&mouse_band);
	cursor_off();

	pop_back(left,top,right,bottom,wbk_c);
	if (*title)
	{
		hz16_disp( left+(wn_w-strlen(title)*CHAR_W)/2, top+CHAR_H+4, title, wtt_c);
		disp_msg(left+32, top+LN_H*2+2, msg);
	}
	else
		disp_msg(left+32, top+CHAR_H+4, msg);


	gettextsettings(&save_text);
	settextstyle(4, HORIZ_DIR, 4);
	settextjustify(CENTER_TEXT, BOTTOM_TEXT);
	setcolor(6);
/*	outtextxy(x+21, y+36, "!");
	outtextxy(x+22, y+37, "!");
	outtextxy(x+23, y+38, "!");
*/	outtextxy(x+24, y+38, "!");
	outtextxy(x+25, y+38, "!");
	setcolor(12);
	outtextxy(x+20, y+35, "!");
	outtextxy(x+21, y+35, "!");
/*	outtextxy(x+22, y+35, "!");
*/	settextstyle(save_text.font, save_text.direction, save_text.charsize);
	settextjustify(save_text.horiz,save_text.vert);

	sound(2500); delay(60); sound (600); delay(10); nosound();  /* Beep */

	set_horizontal_band(left, right-BORDER);
	set_vertical_band(top, bottom-BORDER);
	cursor_on();
	clr_keybuf();
	start_time = *Tick_cnt_ptr;
	while (1)
	{
		if ( !keybuf_nul() )
		{
			get_key0();
			break;
		}
		if ( left_pressed() )
			break;
		if (echo_time)
		{
			if (*Tick_cnt_ptr-start_time>echo_time)
				break;
		}
	};

	cursor_off();
	rid_pop();
	set_mouse_band(&mouse_band);
	if (old_cursor_stat==TRUE)
		cursor_on();

  /* -- restore previous graphical settings -- */
	setcolor(old_color);
	setfillstyle(old_fl.pattern, old_fl.color);
	setlinestyle(old_ln.linestyle, old_ln.upattern, old_ln.thickness);

}

/* ======================== set_msg_color() ============================  */
void set_msg_color(UC bk_c, UC title_c, UC char_c)
{
	wbk_c = bk_c;
	wtt_c = title_c;
/*	wbd_c = border_c;*/
	msg_c = char_c;
	mode_settled = 1;
}

/*-------------------------------------------------------------------------
	Name		:	getmsgsize
	Prototype	:
			void getmsgsize(UC *msg, UI *msg_w, UI *msg_h)
	Function	:
			This function is used to inspect the maxium width and
		lines of the message that is to be shown.
	Parameters	:
		msg		pointer to the message string.
		msg_w	pointer to a integer that will return the maxium width
				of the message. The width is count by character.
		msg_h	pointer to a integer that will return the total lines
				of the message.
	Return Value:
			None. The results are return through pointers 'msg_w' and
		'msg_h'.
-------------------------------------------------------------------------*/
void getmsgsize(UC *msg, UI *msgW, UI *msgH)
{
UC *p;
UI msg_width, msg_high;
UI curW = 0 ;

	p = msg;
	msg_width = 0 ;
	msg_high = 0 ;
	while (*p)
	{
		if (*p == '\n')
		{
			msg_width = max(msg_width, curW);
			curW = 0 ;
			(msg_high) ++ ;
		}
		else
			curW ++ ;
		p ++ ;
   } ;
   *msgW = max( msg_width , curW );
   *msgH = msg_high + 1 ;
}

/*------------------------------------------------------------------
	Name		:	disp_msg
	Prototype	:
			void show_msg(UI x, UI y, UC *msg)
	Function	:
			This is used to show messages on the screen.
	Parameters	:
		(x, y)	top-left corner where to put the message.
		msg		message that is to be shown.
	Return Value:
			None.
------------------------------------------------------------------*/
void disp_msg(UI x ,UI y ,UC *msg)
{
UI curX=x ,curY = y ;
UC *p;

	p = msg ;
	while(*p)
	{
		if (*p == '\n')
		{
			curX = x ;
			curY += LN_H ;
			p ++ ;
			continue ;
		}
		if ( (unsigned)(*p)>0xa0  && (unsigned)(*(p+1))>0xa0 )
		{
			hz_16(*p-0xa0, *(p+1)-0xa0, curX, curY, msg_c) ;
			curX += HZ_W ;
			p +=2 ;
		}
		else
		{
			char_16(*p,curX,curY,msg_c);
			curX += CHAR_W ;
			p ++ ;
		}
   }
}
