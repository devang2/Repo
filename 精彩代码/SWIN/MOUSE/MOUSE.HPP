//-------------------------------------------------------
//                 MOUSE.HPP
//      Impelement of mouse.cpp .
//      Write by TanWenHong 1996.12.18 Chen Du
//--------------------------------------------------------
#ifndef _MOUSE_HPP
#define _MOUSE_HPP

#include <dos.h>

#define ON	1
#define OFF	0

#define LEFT	0		/* left button               */
#define RIGHT	1		/* right button              */
#define CENTER	2		/* center button             */

typedef struct  {
	int	max, mbx, mcx, mdx;
} Param;

extern Param * __Mpar ;

class Mouse
{
public :
	int got_mouse ;   /* 0 = not installed mouse ,1 = installed mouse*/
	unsigned buttons;	/* button status             */
	unsigned cury;		/* last column pos           */
	unsigned curx;		/* last row position         */
	unsigned count;	/* button event count        */
	unsigned display;	/* cursor status 1=ON 0=OFF  */
private :
	void mouse(Param * mptr);
public :
	Mouse();
	void Display(int on_off);
	void Locate(int x,int y);
	void Release(int button);
	void Status(void);
	void TextCursor(int cursor_type, int cursor_attr1, int cursor_attr2);
	void GraphCursor(int x_hot, int y_hot, void far *cmask);
	void	Conditional(int ux, int uy, int lx, int ly);
	void	Window(int left, int top, int right, int bottom);
	void	LightPen(int on_off);
	void	Motion(int& x, int& y);
	void	Event(unsigned mask, void (far * func)(void));
	void	Ratio(int x, int y);
	void	Speed(int x);
};

extern Mouse mouse;
#endif
