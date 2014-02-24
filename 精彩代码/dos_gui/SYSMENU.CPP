// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	sysmenu.cpp
*/
#include "sysmenu.h"

sysmenu_class::sysmenu_class(int ID,char *title_hotkey,char *help,Tmenu *pfather):
	menu_class (ID,title_hotkey,help,pfather ){};

void sysmenu_class::unselect()
{ //	if (this== NULL )return;
	setport (menu_viewport);
	PaintRect ( bounds,LIGHTGRAY, COPY_PUT ) ;
	pfather_menu->psub_menu_selected = this;
	draw ();
}

sysmenu_class::~sysmenu_class()
{}

void sysmenu_class::draw( )
{  //	if (pmenu_handled == NULL )return;
	setport (menu_viewport);
		if (this !=pfather_menu->psub_menu_selected )
		setcolor (DARKGRAY );
		else setcolor ( WHITE );

	moveto
	(bounds.left+5,bounds.top);
  char minus[]="-";
	outtext (minus);
}

void sysmenu_class::select()
{
//	if (pmenu_select== NULL )return;

	setport (menu_viewport);
	PaintRect ( bounds, DARKGRAY ,COPY_PUT );
	if (pfather_menu != NULL )
	pfather_menu->psub_menu_selected  = this;
	draw ();
	draw_help_texts (help);
}


