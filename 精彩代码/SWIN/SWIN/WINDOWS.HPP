#ifndef _WINDOWS_HPP
#define _WINDOWS_HPP

#ifndef _GUI_HPP
	#include "gui.hpp"
#endif
#ifndef	_DBLIST_HPP
	#include "dblist.hpp"
#endif
#ifndef _BOX_HPP
	#include "box.hpp"
#endif
#ifndef _XMS_HPP
	#include "xms.hpp"
#endif

class Windows:public Box
{
protected :
	char * title;
	IMG * bk_img ;
	DbList buttList ;
	int _objnum ;
public :
	Windows(int x,int y,int w,int h,char * Title,int objnum);
	~Windows();
	void AddButton(Box * butt);
	int Run();
	void Show();
	int Clicked(){return(MouseIn()?RUNIT:NOCLICK);}
	void Hide();
	void Setxy(int x,int y);
};

inline Windows::Windows(int x,int y,int w,int h,char * Title,int objnum=1)
		:Box(x,y,w,h)
{
	Windows::bk_img = 0;
	Windows::_objnum = objnum ;
	title = Title ;
}

inline Windows::~Windows()
{
	Windows::Hide();
}

inline void Windows::AddButton(Box * butt)
{
	butt->Setxy(wx+butt->x(),wy+butt->y());
	Windows::buttList.AtachToTail(butt);
}

#endif
