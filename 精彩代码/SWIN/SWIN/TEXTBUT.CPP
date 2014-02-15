#include "textbut.hpp"

Text::Text(int x,int y,int maxChar)
	:Box(x,y,8*maxChar+4,20)
{
	max_char = maxChar ;
	Text::text = new char[maxChar+1] ;
	cursor_position = display_start = 0 ;
	Text::text[0] = 0 ;
}

Text::~Text()
{
	delete Text::text ;
}

void Text::Show()
{
	unsigned on_off = mouse.display ;
	mouse.Display(OFF);
	gr.Box(wx,wy,ww,wh,SOLID_FILL,WHITE);
	gr.Rect3d(wx,wy,ww,wh,FALSE);
	gr.SetColor(BLACK);
	gr.Outtext(wx+2,wy+2,Text::text);
	mouse.Display(on_off);
}

int Text::Clicked()
{
	int clicked = NOCLICK ;
	if(event.type == KEYBD){
		if(!active) return NOCLICK ;
		if((unsigned char)event.key >= ' ') return CLICKED ;
		switch(event.key)
		{
		case 0x0e08 : return CLICKED ;
		}
	} else if(Box::MouseIn()) {
		clicked = CLICKED ;
	}
   return clicked ;
}

int Text::Run()
{
	unsigned on_off = mouse.display ;
	mouse.Display(OFF);
	if(event.type == KEYBD){
		if(event.key == 0x0e08){
			if(cursor_position == 0){
				sound(1000);
				delay(10);
				nosound();
				return OK ;
			}
			cursor_position -- ;
			Text::text[cursor_position] = 0 ;
			gr.Box(wx+2+cursor_position*gr.TextWidth("H"),wy+2,
				8,gr.TextHeight("H"),SOLID_FILL,WHITE);
			gr.Line(wx+2+cursor_position*gr.TextWidth("H"),wy+2,
				wx+2+cursor_position*gr.TextWidth("H"),wy+2+gr.TextHeight("H"),BLACK);
		}
		else
		{
			if(cursor_position<max_char){
				gr.Box(wx+2+cursor_position*gr.TextWidth("H"),wy+2,
					8,gr.TextHeight("H"),SOLID_FILL,WHITE);
				Text::text[cursor_position] = (char)event.key ;
				cursor_position++ ;
				Text::text[cursor_position] = 0 ;
				gr.SetColor(BLACK);
				gr.Outtext(wx+2+(cursor_position-1)*gr.TextWidth("H"),wy+2,
					"%s",Text::text+cursor_position-1);
				gr.Line(hz.Getx(),hz.Gety(),hz.Getx(),hz.Gety()+15,BLACK);
			} else {
				sound(1000);
				delay(10);
				nosound();
			}
		}
	}
	mouse.Display(on_off);
	return OK ;
}

void Text::Active(Bool status)
{
	if(active == status) return ;
	active = status ;
	unsigned on_off = mouse.display ;
	mouse.Display(OFF);
	if(active) {
		gr.Line(wx+2+cursor_position*gr.TextWidth("H"),wy+2,
			wx+2+cursor_position*gr.TextWidth("H"),wy+2+gr.TextHeight("H"),BLACK);
	} else {
		gr.Line(wx+2+cursor_position*gr.TextWidth("H"),wy+2,
			wx+2+cursor_position*gr.TextWidth("H"),wy+2+gr.TextHeight("H"),WHITE);
	}
	mouse.Display(on_off);
}