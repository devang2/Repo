#ifndef _EVENT_HPP
#define _EVENT_HPP

#ifndef	_MOUSE_HPP
	#include "mouse.hpp"
#endif

#define	NOEVENT		0
#define	KEYBD			1
#define	MBUTTON		2

#define CTRL   0x04
#define ALT    0x08

class Event
{
public :
	int alt ;
	int type ;
	int key ;
public :
	int Present();
};

extern Event event ;
#endif