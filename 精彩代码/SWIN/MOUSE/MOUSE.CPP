//---------------------------------------------------
//                    MOUSE.CPP
//      This program offer the interface of mouse
//   driver .It muse complie with C++ .
//      Write by TanWenHong 1996.12.18 Cheng Du
//---------------------------------------------------
#ifndef _MOUSE_HPP
	#include "mouse.hpp"
#endif

Mouse mouse;

Param * __Mpar ;

Mouse::Mouse()
{
	union	REGS	mreg;
	struct	SREGS	segs;

	/* User needs DOS 2 or higher to use these routines */

	if (_osmajor < 2) {
		got_mouse = 0 ;
		return;
	}

	__Mpar = (Param *)new char[sizeof(Param)];

	/* initialize variables to 0 */

	__Mpar->max = __Mpar->mbx = __Mpar->mcx = __Mpar->mdx = 0;

	/* status returned in __Mpar->max, if 0 then not installed */

	if (_osmajor >= 3)
		mouse(__Mpar);
	else  {				/* it's version 2 */
		mreg.h.ah = 0x35;	/* Function to get interrupt vector */
		mreg.h.al = 0x33;	/* mouse interrupt number */
		intdosx(&mreg, &mreg, &segs);

		if (segs.es == 0 && mreg.x.bx == 0){
			got_mouse = 0;	/* if vector points to 0000:0000,
								mouse not in */
			return ;
		}	else
			mouse(__Mpar);	/* initialize mouse */
	}
	got_mouse=__Mpar->max ;
}

void Mouse::mouse(Param *mptr)
{
	union	REGS	mousreg;

	mousreg.x.ax = mptr->max;
	mousreg.x.bx = mptr->mbx;
	mousreg.x.cx = mptr->mcx;
	mousreg.x.dx = mptr->mdx;
	int86(0x33, &mousreg, &mousreg);	/* mouse interrupt */

	mptr->max = mousreg.x.ax;
	mptr->mbx = mousreg.x.bx;
	mptr->mcx = mousreg.x.cx;
	mptr->mdx = mousreg.x.dx;
}

void Mouse::Display(int on_off)
{
	if(!got_mouse) return ;

	if (on_off == ON)  {
		__Mpar->max = 1;
		display = ON;
		mouse(__Mpar);
	} else  {
		__Mpar->max = 2;
		display = OFF;
		mouse(__Mpar);
	}
}

void Mouse::Release(int button)
/* Get button release */
{
	if(!got_mouse) return ;

	__Mpar->max = 6;
	__Mpar->mbx = button;	/* 0 = left, 1 = right, 2 = center */
	mouse(__Mpar);

	buttons = __Mpar->max;	/* status of button now */
	count   = __Mpar->mbx;	/* number of button releases */
	curx    = __Mpar->mcx;	/* horizontal pos. of last release */
	cury    = __Mpar->mdx;	/* vertical pos. of last release */
}

void Mouse::Status(void)
/*
 *	Return mouse status
 */
{
	if(!got_mouse) return ;

	__Mpar->max = 3;
	mouse(__Mpar);

	buttons = __Mpar->mbx;
	curx    = __Mpar->mcx;
	cury    = __Mpar->mdx;
}

void Mouse::TextCursor(int cursor_type, int cursor_attr1, int cursor_attr2)
/*
 *	Set cursor mode.  Set cursor to text or software
 */
{
	if(!got_mouse) return ;
	__Mpar->max = 10;
	__Mpar->mbx = cursor_type; /* if 0, use software cursor; 1, use hardware cursor */
	__Mpar->mcx = cursor_attr1;/* if software cursor, defines screen mask */
				  /* if hardware cursor, defines scan line start */
	__Mpar->mdx = cursor_attr2;/* if software cursor, defines cursor mask */
				  /* if hardware cursor, defines scan line stop */
	mouse(__Mpar);
}

void Mouse::GraphCursor(int x_hot, int y_hot, void far *cmask)
/*
 *	Create a graphic cursor
 */
{
	if(!got_mouse) return ;

	struct	SREGS	msregs;
	union	REGS	mousreg;
	unsigned  maskseg, maskoff;

	maskseg = FP_SEG(cmask);
	maskoff = FP_OFF(cmask);

	mousreg.x.ax = 9;
	mousreg.x.bx = x_hot;		/* horizontal hot spot of cursor */
	mousreg.x.cx = y_hot;		/* vertical hot spot of cursor */
	mousreg.x.dx = maskoff;		/* offset of mask  */

	segread(&msregs);		/* copy the current segment registers to msregs */
	msregs.es = maskseg;		/* segment address of cursor mask */
	int86x(0x33, &mousreg, &mousreg, &msregs);
}

void	Mouse::Conditional(int ux, int uy, int lx, int ly)
/*
 *	Function to hide the mouse if it is in the region
 *	when this function is called.  Your program must call
 *	m_display(ON) to enable the cursor again.
 */
{
	if(!got_mouse) return ;

	_AX = 16;		/* mouse function 16          */
	_CX = ux;		/* upper x screen coordinates */
	_DX = uy;		/* upper y screen coordinates */
	_SI = lx;		/* lower x screen coordinates */
	_DI = ly;		/* lower y screen coordinates */

	geninterrupt(0x33);	/* mouse interrupt */
}

void	Mouse::Window(int left, int top, int right, int bottom)
/* Enable an active window */
{
	if(!got_mouse) return ;

	__Mpar->max = 7;
	__Mpar->mcx = left;
	__Mpar->mdx = right;
	mouse(__Mpar);

	__Mpar->max = 8;
	__Mpar->mcx = top;
	__Mpar->mdx = bottom;
	mouse(__Mpar);
}

void	Mouse::LightPen(int on_off)
/*
 *	Function to switch the light pen emulation mode on or off.
 *	On by default, after initialization.
 *
 *	0 turns it on, any other value turns it off.
 */
{
	if(!got_mouse) return ;
	__Mpar->max = (on_off ? 13 : 14);
	mouse(__Mpar);
}

void	Mouse::Motion(int& x, int& y)
/* reads the motion of the mouse */
{
	if(!got_mouse) return ;

	__Mpar->max = 11;
	mouse(__Mpar);

	x = __Mpar->mcx;	/* horizontal distance since last call */
	y = __Mpar->mdx;	/* vertical distance since last call   */
}


void	Mouse::Event(unsigned mask, void (far * func)(void))
/*
 *	Define a subroutine to be executed conditionally
 *	by the mouse software
 */
{
	if(!got_mouse) return ;

	_ES = FP_SEG(func);	/* segment of function */
	_DX = FP_OFF(func);	/* offset of function  */
	_CX = mask;		/* condition call mask */
	_AX = 12;		/* mouse function 12   */

	geninterrupt(0x33);	/* mouse function call */

	/*
	 *	NOTE: The subroutine called must do a far return.
	 *      on entry to the subroutine:
	 *
	 *	ax contains the condition mask
	 *	bx contains the button state
	 *	cx contains the horizontal cursor coordinate
	 *	dx contains the vertical cursor coordinate
	 */
}

void	Mouse::Ratio(int x, int y)
/* Set the mickey to pixel ratio */
{
	if(!got_mouse) return ;

	__Mpar->max = 15;
	__Mpar->mcx = x;		/* horizontal mickey/pixel ratio */
	__Mpar->mdx = y;		/* vertical mickey/pixel ratio */
	mouse(__Mpar);
}

void	Mouse::Speed(int x)
/*
 *	Set the threshold speed for doubling the cursor's
 *	movements
 */
{
	if(!got_mouse) return ;

	__Mpar->max = 19;
	__Mpar->mdx = x;		/* speed threshold */
	mouse(__Mpar);
}
