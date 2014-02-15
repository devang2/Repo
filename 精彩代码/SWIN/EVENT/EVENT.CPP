#ifndef  _EVENT_HPP
	#include "event.hpp"
#endif
#ifndef	__BIOS_H
	#include <bios.h>
#endif

Event event ;

int Event::Present()
{
	mouse.Display(ON);
	int modifiers = bioskey(2);
	if(modifiers&ALT){
		while(modifiers&ALT){
			modifiers = bioskey(2);
			if(!(modifiers&ALT)){
				type = KEYBD ;
				alt = 1 ;
				key = 0 ;
			}
			if(bioskey(1)){
				type = KEYBD ;
				modifiers = 0 ;
				alt = 0 ;
				key = bioskey(0) ;
			}
		}
	} else if(bioskey(1)){
		type = KEYBD ;
		modifiers = 0 ;
		alt = 0 ;
		key = bioskey(0) ;
	} else {
		mouse.Status();
		if((mouse.buttons&LEFT)|(mouse.buttons&RIGHT)|(mouse.buttons&CENTER))
			type = MBUTTON ;
		else type = NOEVENT ;
	}
	return type ;
}