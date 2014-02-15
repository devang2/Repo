/*--------------------------------------------------------------
						MENUITEM.CPP
		This file declare a menuitem class , its main function
	for popup menu and pull menu .
		Write by TanWenHong 1997.1.14  Chen Du
---------------------------------------------------------------*/
#include "gui.hpp"
#include "menuitem.hpp"

MenuItem::MenuItem(int x,int y,int w,int h,char * Prompt,int exitNo,
			Windows * subMenu,int hotKey)
	:Box(x,y,w,h)
{
	MenuItem::prompt = Prompt ;
	Box::exitcode = exitNo ;
	MenuItem::submenu = subMenu ;
	MenuItem::hotkey = hotKey ;
}

void MenuItem::Setxy(int x,int y)
{
	int dx = x - wx ;
	int dy = y - wy ;
	Box::Setxy(x,y);
	if(submenu) submenu->Setxy(dx+submenu->x(),dy+submenu->y());
}

int MenuItem::Run()
{
	if(submenu)	return submenu->Run();
	else return exitcode ;
}

void MenuItem::Show()
{
	int on_off = mouse.display ;
	mouse.Display(OFF);
	if(exitcode == COMMENT){
		Box::Disable(true) ;
		gr.Line(wx+3,wy+wh/2+1,wx+ww-2,wy+wh/2+1,WHITE);
		gr.Line(wx+2,wy+wh/2,wx+ww-3,wy+wh/2,DARKGRAY);
	}
	if(!disable) gr.SetColor(BLACK);
	else gr.SetColor(DARKGRAY);
	gr.Outtext(wx+(ww-gr.TextWidth(prompt))/2,
		wy+(wh-gr.TextHeight("H"))/2,prompt);
	mouse.Display(on_off);
}

void MenuItem::Active(Bool status)
{
	if(active == status||disable) return;
	int on_off = mouse.display ;
	mouse.Display(OFF);
	active = status ;
	if(active){
		gr.Box(wx+2,wy,ww-4,wh,SOLID_FILL,LIGHTBLUE);
		gr.SetColor(WHITE);
		gr.Outtext(wx+(ww-gr.TextWidth(prompt))/2,
			wy+(wh-gr.TextHeight("H"))/2,prompt);
	}	else	{
		gr.Box(wx+2,wy,ww-4,wh,SOLID_FILL,LIGHTGRAY);
		gr.SetColor(BLACK);
		gr.Outtext(wx+(ww-gr.TextWidth(prompt))/2,
			wy+(wh-gr.TextHeight("H"))/2,prompt);
	}
	mouse.Display(on_off);
}

int MenuItem::Clicked()
{
	int clicked = NOCLICK ;
	if(event.type == KEYBD ){
		if((event.key == hotkey)||(event.key == 0x1c0d && active))
			clicked = CLICKED;
	} else {
		while(event.Present()&&Box::MouseIn());
		if(Box::MouseIn())
			clicked = CLICKED ;
	}
	if(disable&&(!active)) clicked = NOCLICK ;
	return clicked ;
}