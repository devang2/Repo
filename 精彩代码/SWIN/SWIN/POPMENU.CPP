/*--------------------------------------------------------------------
							POPMENU.CPP
		This file declare a popup menu class.
			Write by TanWenHong 1997.1.14 Chen Du
---------------------------------------------------------------------*/
#include "popmenu.hpp"

PopMenu::PopMenu(int x,int y,char * bar[],int items,int curbar)
	:Windows(x,y,0,0,"",curbar)
{
	Windows::ww = gr.TextWidth(bar[0])+20 ;
	Windows::wh = items * 20 + 8 ;
	for(int i = 0 ; i < items ; i ++ )
		Windows::AddButton(new MenuItem(0,4+i*20,ww,20,bar[i],i+1));
}

PopMenu::PopMenu(int x,int y,int w,int h,MenuItem * itemList[],int items)
	:Windows(x,y,w,h,"",1)
{
	for(int i = 0 ; i < items ; i ++ )
		Windows::AddButton(itemList[i]);
}

int PopMenu::Run()
{
	if(buttList.IsEmpty()) return NOCLICK ;
	Windows::Show();
	buttList.Locate(_objnum);
	int exitcode = NOCLICK ;
	while(exitcode == NOCLICK){
		while(!event.Present());
		if(event.type == KEYBD){
			switch(event.key)
			{
			case 0x5000 :buttList.SkipNext();continue ;
			case 0x4800 :buttList.SkipPrev();continue ;
			case 0x4b00 :
			case 0x4d00 :
			case 0x011b :exitcode = CANCEL ;continue ;
			}
		}else if(event.type == MBUTTON && !MouseIn()){
				exitcode = CANCEL ;
				continue ;
		}
		exitcode = buttList.CheckEvent();
		if(exitcode == CANCEL) exitcode = NOCLICK ;
		else	_objnum = buttList.ObjNum(buttList.CurrNode->element);
	}
	Windows::Hide();
	return exitcode ;
}