#ifndef _BUTTON_HPP
#define _BUTTON_HPP

#ifndef _BOX_HPP
	#include "box.hpp"
#endif
#ifndef _GUI_HPP
	#include "gui.hpp"
#endif

class Button:public Box
{
protected :
	int hotkey ;
	char * caption ;
	int (* Proc)();
public :
	Button(	int x,int y,int w,int h,char * Caption,
				int exitNo,int hotkey,int (*proc)());
	void Show();
	int Run();
	void Disable(Bool status);
	void Active(Bool status);
	int Clicked();
};

inline Button::Button(int x,int y,int w,int h,char * Caption,
	int exitNo = 1,int hotKey = -1 ,int (*proc)() = 0 )
	:Box(x,y,w,h)
{
	caption = Caption ;
	exitcode = exitNo ;
	Proc = proc ;
	hotkey = hotKey ;
}

#endif