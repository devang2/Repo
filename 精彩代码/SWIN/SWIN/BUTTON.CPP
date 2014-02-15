#include "button.hpp"
#include <stdlib.h>

void Button::Disable(Bool status)
{
	if(status == disable) return ;
	disable = status ;
	Button::Show();
}

void Button::Show()
{
	gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
	gr.Rect3d(wx,wy,ww,wh,TRUE);
	if(disable){
		gr.SetColor(WHITE);
		gr.Outtext(wx+(ww-gr.TextWidth(caption))/2+1,
			wy+(wh-gr.TextHeight("H"))/2+1,caption);
	}
	gr.SetColor(BLACK);
	gr.Outtext(wx+(ww-gr.TextWidth(caption))/2,
		wy+(wh-gr.TextHeight("H"))/2,caption);
}

void Button::Active(Bool status)
{
	if(active == status) return ;
	int on_off = mouse.display;
	mouse.Display(OFF);
	active = status ;
	gr.SetWriteMode(XOR_PUT);
	gr.Rectangle(wx-1,wy-1,ww+2,wh+2,LIGHTGRAY);
	gr.SetWriteMode(COPY_PUT);
	mouse.Display(on_off);
}

int Button::Run()
{
	if(disable) return NOCLICK ;
	if(Proc)	return (*Proc)();
	else return exitcode;
}

int Button::Clicked()
{
	int on_off = mouse.display ;
	int clicked = NOCLICK ;
	char * buff = new char[imagesize(wx+2,wy+2,wx+ww-3,wy+wh-3)];
	if(!buff){
		printf("Memory not enough\n");
		abort();
	}
	if(event.type == KEYBD){
		if((event.key == hotkey)||(event.key == 0x1c0d && active == true)){
			mouse.Display(OFF);
			getimage(wx+2,wy+2,wx+ww-3,wy+wh-3,buff);
			gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
			gr.Rect3d(wx,wy,ww,wh,FALSE);
			putimage(wx+3,wy+3,buff,COPY_PUT);
			delay(200);
			gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
			gr.Rect3d(wx,wy,ww,wh,TRUE);
			putimage(wx+2,wy+2,buff,COPY_PUT);
			clicked = disable?CLICK_ONLY:RUNIT ;
		}
	}
	else
	{
		if( Box::MouseIn() )
		{
			mouse.Display(OFF);
			getimage(wx+2,wy+2,wx+ww-3,wy+wh-3,buff);
			gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
			gr.Rect3d(wx,wy,ww,wh,FALSE);
			putimage(wx+3,wy+3,buff,COPY_PUT);
			while(event.Present());
			if(Box::MouseIn())
			{	clicked = disable?CLICK_ONLY:RUNIT ;}
			else clicked = CLICK_ONLY ;
			mouse.Display(OFF);
			gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
			gr.Rect3d(wx,wy,ww,wh,TRUE);
			putimage(wx+2,wy+2,buff,COPY_PUT);
		}
	}
	delete buff ;
	mouse.Display(on_off);
	return clicked;
}