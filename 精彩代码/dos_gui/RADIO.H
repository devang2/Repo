#ifndef __RADIO_H
#define __RADIO_H

#include "checkbox.h"

typedef class radio_class Tradio;

class radio_class: public Tcheckbox{
public:
	radio_class(int ID,char *title_hotkey,
		int left,int top,int width,int height);

virtual	void  draw ( );
virtual int control_change_value (int new_value  );
};

#endif
