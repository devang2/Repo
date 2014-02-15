#include "event.hpp"
#include "mouse.hpp"
#include <conio.h>
#include <graphics.h>


int xmask[32]={0x07e0,0x0000,0x0180,0x700e,0x0000,0x1c38,0xc003,0x0660,
					0xf00f,0x03c0,0xc003,0x0660,0x0000,0x1c38,0x0180,0x700e,
					0x07e0,0x0000,0xffff,0x0000,0xffff,0x0000,0xffff,0x0000,
					0xffff,0x0000,0xffff,0x0000,0xffff,0x0000,0xffff,0x0000 };

void main()
{
	int gdriver = DETECT, gmode, errorcode;

	initgraph(&gdriver, &gmode, "\\borlandc\\bgi");
	if(!mouse.got_mouse){
		outtextxy(200,240,"Mouse driver not install ...");
		closegraph();
		return ;
	}
	mouse.GraphCursor(1,1,xmask);
	while(!event.Present()) ;

	/* clean up */
	getch();
	closegraph();
}
