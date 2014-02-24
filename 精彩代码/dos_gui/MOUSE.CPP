#include "mouse.h"
#include "yyxsys.h"

mouse_class::mouse_class()
{
	reset();
	mouse_x=0; mouse_y=0;
	set_posn();
//	setup event handler

	m1=M_SET_EVENT_HNDL;
//	m3=0x2 | 0x4  	; // press & release LEFT BUTTON
	m3=1			; // press & release LEFT BUTTON
	_ES =FP_SEG ((void far *) mouse_event_handler);
	m4 =FP_OFF ((void far*)  mouse_event_handler);
//	mouse(&m1,&m2,&m3,&m4);
	prev_cursor_state=FALSE;
	on(0);
}

mouse_class::~mouse_class()
{
	m1=M_SET_EVENT_HNDL;
//	m3=0x2 | 0x4  	; // press & release LEFT BUTTON
	m3=0			; // press & release LEFT BUTTON
	_ES =FP_SEG ((void far *) mouse_event_handler);
	m4 =FP_OFF ((void far*)  mouse_event_handler);
	mouse(&m1,&m2,&m3,&m4);
	reset();
}

void mouse_class::mouse(int *m1,int *m2,int *m3,int *m4)
{
	union REGS inregs,outregs;
	inregs.x.ax=*m1;
	inregs.x.bx=*m2;
	inregs.x.cx=*m3;
	inregs.x.dx=*m4;
	int86 (0x33,&inregs,&outregs);
	*m1=outregs.x.ax;
	*m2=outregs.x.bx;
	*m3=outregs.x.cx;
	*m4=outregs.x.dx;
}

BOOL mouse_class::reset()
{   m1=M_RESET;
	mouse(&m1,&m2,&m3,&m4);
	return (m1==-1);
}

void mouse_class::on(int restoreflag) //0 ;regardless of state; //1 conditional
//first 0; later 1;only if previous is on
{   if (!restoreflag||prev_cursor_state) {
		m1=M_SHOW_CURS;
		mouse (&m1,&m2,&m3,&m4);
		prev_cursor_state=TRUE;
	}
}

void mouse_class::off(int tempflag) //0 ;regardless of state; //1 conditional
//first 0; later 1;only if previous is on, set to off
//usually use (1)
{   if (prev_cursor_state) {
		m1=M_HIDE_CURS;
		mouse (&m1,&m2,&m3,&m4);
		if (!tempflag) prev_cursor_state=FALSE;
	}
}

void mouse_class::get_posn()
{
	m1=M_GET_STATUS;
	mouse (&m1,&m2,&mouse_x,&mouse_y);
}

void mouse_class::get_posn_xy(int &x,int &y)
{
	m1=M_GET_STATUS;
	mouse (&m1,&m2,&x,&y);
}

void mouse_class::set_posn(int x,int y)
{
	m1=M_SET_CURS;
	if (!x||!y)	mouse (&m1,&m2,&x,&y);
	else	mouse (&m1,&m2,&mouse_x,&mouse_y);
}

int mouse_class::get_press_count(int mouse_button)
{
	m1=M_GET_PRESS;
	m2=mouse_button;
	mouse (&m1,&m2,&m3,&m4);
	if (mouse_button==LEFT_BUTTON) left_button_status =m1; else
	if (mouse_button==RIGHT_BUTTON) right_button_status =m2;

	return (m2);	//0,1,2,3,4......since last call
}


int mouse_class::get_release_count(int mouse_button)
{
	m1=M_GET_REL;
	m2=mouse_button;
	mouse (&m1,&m2,&m3,&m4);
	if (mouse_button==LEFT_BUTTON) left_button_status =m1; else
	if (mouse_button==RIGHT_BUTTON) right_button_status =m2;

	return (m2);	//0,1,2,3,4......since last call
}

int mouse_class::mouse_event_handler(...)
{
	putch('\007');
}

