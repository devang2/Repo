#ifndef __SYSMENU_H
#define __SYSMENU_H

#include "yyxmenu.h"

typedef class sysmenu_class Tsysmenu;

class sysmenu_class :public Tmenu {
public:
	sysmenu_class (int ID,char *title_hotkey,char *help,Tmenu *pfather );
	~sysmenu_class ();
virtual	void  draw();
virtual	void  select();
virtual	void  unselect();
};

#ifdef __YYXMAIN

Tsysmenu *sysmenu;
Tmenu 	*sub_sysmenu[6];
Tsysmenu *win_sysmenu;
Tmenu	*sub_win_sysmenu[6];

#else

extern Tsysmenu *sysmenu;
extern Tmenu 	*sub_sysmenu[6];
extern Tsysmenu *win_sysmenu;
extern Tmenu	*sub_win_sysmenu[6];

#endif

#endif
