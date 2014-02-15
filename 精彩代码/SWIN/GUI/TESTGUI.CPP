#include "gui.hpp"
#include <conio.h>


void main()
{
	IMG img;
	gr.Box(1,1,400,400,SOLID_FILL,GREEN);
	gr.SetColor(BLUE);
	gr.Outtext(50,200,"字体宽度 = %d & 字体高度 = %d",
	gr.TextWidth("W"),gr.TextHeight("H"));
		  gr.SetTextStyle(1);
	gr.SetColor(RED);
	gr.Outtext(10,100,"Font width = %d &Font heitgh =%d",
	gr.TextWidth("W"),gr.TextHeight("H"));
	gr.GetImage(0,0,440,400,(IMG*)&img);
	gr.PutImage(100,20,(IMG*)&img,COPY_PUT);
	gr.PutImage(10,20,(IMG*)&img,COPY_PUT);
	gr.PutImage(50,20,(IMG*)&img,COPY_PUT);
	getch();
}
