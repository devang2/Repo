/*-------------------------------------------------------------
							BOX.CPP
		This file clarm Box class baseed on Object class.
			Write by TanWenHong 1996.12.27 Cheng Du
-------------------------------------------------------------*/
#ifndef	_BOX_HPP
	#include "box.hpp"
#endif
#ifndef	_GUI_HPP
	#include "gui.hpp"
#endif

int Box::MouseIn()
{
	if( (mouse.curx >= wx && mouse.curx <= wx+ww) &&
		 (mouse.cury >= wy && mouse.cury <= wy+wh)
	) return TRUE ;
	else return FALSE ;
}