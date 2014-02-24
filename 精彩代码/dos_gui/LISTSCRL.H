#ifndef __LISTSCRL_H
#define __LISTSCRL_H

#include "listbox.h"
#include "scroll.h"
#include "group.h"

typedef class listscrl_class Tlistscrl;

class listscrl_class: public Tgroup {
public:
Tscroll *	scroll;
Tlistbox * listbox;
public:
	listscrl_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,
		int string_num, char ** thestring_list);
//	virtual	void  draw_control (  );
virtual void  setup_control(  );

//virtual	int   control_change_value ( int new_value );
virtual	void  select (  );
virtual	void  unselect (  );
virtual	int   key_pressed_handler  (int key_scan_num );
virtual int   msg_handler (MSG& message);
};


#endif

