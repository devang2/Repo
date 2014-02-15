#include "windows.hpp"

void Windows::Show()
{
	if(bk_img) return ;
	unsigned on_off = mouse.display ;
	if(on_off == ON) mouse.Display(OFF);

	bk_img = new IMG ;
	gr.GetImage(wx,wy,ww,wh,bk_img);
	gr.Box(wx,wy,ww,wh,SOLID_FILL,LIGHTGRAY);
	gr.Rect3d(wx,wy,ww,wh,TRUE);
	buttList.ShowAllObj();
	mouse.Display(on_off);
}

int Windows::Run()
{
	while(!event.Present());
	return 1 ;
}

void Windows::Setxy(int x,int y)
{
	int dx = x - wx;
	int dy = y - wy;
	Box::Setxy(x,y);
	Node * node = buttList.ListHead ;
	do{
		node->element->Setxy(dx+node->element->x(),dy+node->element->y());
		node = node->next ;
	}while(node != buttList.ListHead);
}

void Windows::Hide()
{
	if(!bk_img) return ;
	unsigned on_off = mouse.display ;
	if(on_off == ON) mouse.Display(OFF);
	gr.PutImage(wx,wy,bk_img,COPY_PUT);
	delete bk_img ;
	bk_img = 0;
	mouse.Display(on_off);
}