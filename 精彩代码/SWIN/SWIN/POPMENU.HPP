/*--------------------------------------------------------------------
							POPMENU.HPP
		This file declare a popup menu class.
			Write by TanWenHong 1997.1.14 Chen Du
---------------------------------------------------------------------*/
#ifndef _POPMENU_HPP
#define _POPMENU_HPP

#include "windows.hpp"
#include "menuitem.hpp"

class PopMenu:public Windows
{
public :
	PopMenu(int x,int y,char * bar[],int items,int curbar=1);
	PopMenu(int x,int y,int w,int h,MenuItem * itemList[],int items);
	int Run();
};

#endif