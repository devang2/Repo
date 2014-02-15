#ifndef _PULLMENU_HPP
#define _PULLMENU_HPP

#include "popmenu.hpp"
#include "menuitem.hpp"

class PullMenu:public Windows
{
	int areadypull ;
public :
	PullMenu(int x,int y,int w,int h,MenuItem * bar[],int items);
	void Show();
	int Clicked();
	int Run();
	int ExitCode(){return exitcode;}
};

inline void PullMenu::Show()
{
	Windows::buttList.ShowAllObj();
}

#endif