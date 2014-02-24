#ifndef __RADIOS_H
#define __RADIOS_H

#include "group.h"

/////////////////////////////////////3/////////////////////////////////////
typedef class radios_group_class Tradios_group;

class radios_group_class:public Tgroup {
public:

public:
	radios_group_class (int ID,char *title_hotkey,int left,int top,int width,int height );
	virtual int control_change_value	(int new_value );

	virtual int key_pressed_handler  (int key_scan_num);
	virtual	int msg_handler 	(MSG& message);
protected:
	BOOL flag;
};


#endif