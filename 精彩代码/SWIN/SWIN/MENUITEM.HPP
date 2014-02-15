/*--------------------------------------------------------------
						MENUITEM.HPP
		This file declare a menuitem class , it's main function
	for popup menu and pull menu .
		Write by TanWenHong 1997.1.14  Chen Du
---------------------------------------------------------------*/

#ifndef _MENUITEM_HPP
#define _MENUITEM_HPP

#include "windows.hpp"
#include "box.hpp"

class MenuItem:public Box
{
	char * prompt ;
	Windows * submenu ;
	int hotkey ;
public :
	MenuItem(int x,int y,int w,int h,char * Prompt,int exitNo,
			Windows * subMenu=0,int Key = -1);
	int Run();
	void Show();
	int Clicked();
	void Setxy(int x,int y) ;
	void Active(Bool status) ;	//Active/Deactive this object.
};
#endif