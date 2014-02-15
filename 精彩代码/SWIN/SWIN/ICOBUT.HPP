//icobut.hpp

#include "button.hpp"
#include "ico.hpp"

class IcoButton:public Button
{
public :
	IcoButton(int,int,int,int,char*,int exitCode,int hotkey,int (*proc)());
	void Show();
};

inline IcoButton::IcoButton(int x,int y,int w,int h,char * icoFile,int exitCode,
	int hotkey = -1,int (*proc)()=0):Button(x,y,w,h,icoFile,exitCode,hotkey,proc)
{}

void IcoButton::Show()
{
	gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
	gr.Rect3d(wx,wy,ww,wh,TRUE);
	ShowIco(wx+(ww-32)/2,wy+(wh-32)/2,Button::caption);
}