#ifndef __GROUP_H
#define __GROUP_H

#include "yyxctrl.h"

typedef class group_class Tgroup;

class group_class :public Tcontrol {
public:
	byte control_num;
	Tcontrol *pfirst_control;
	Tcontrol *pcontrol_current_selected;
protected:
	void dispose_all_controls (Tcontrol  *pcontrol_handled );
	int control_tab_key_handler 	(int key_scan_num );
public:
 void  insert_control (Tcontrol *pcontrol_handled);
	group_class(int ID,char *title_hotkey,
		int left,int top,int width,int height );
virtual	group_class::~group_class();

virtual int  control_change_value	(int new_value );
virtual void setup_control ();
virtual void update_control ();

public:
virtual	int  key_pressed_handler  (int key_scan_num );
virtual void draw ();
virtual void select ( );
virtual	void unselect ( );
virtual	int  msg_handler (MSG& message );

};




#endif