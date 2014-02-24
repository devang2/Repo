#ifndef __CHECKBOX_H
#define __CHECKBOX_H

#include "button.h"

#define CHECKED 1
#define UNCHECKED 0

typedef class checkbox_class Tcheckbox;

class checkbox_class: public Tcontrol{
protected:
	Trect box;
public:
virtual	void  draw ( );
virtual void  unselect ( );
virtual	int   key_pressed_handler  (int key_scan_num );
virtual int   msg_handler( MSG & message);

public:
	checkbox_class(int ID,char *title_hotkey,
		int left,int top,int width,int height);
virtual int   control_change_value (int new_value );
};

#ifdef __YYXMAIN
 int  CHECKBOX_CHECKED_COLOR    =LIGHTGRAY;
 int  CHECKBOX_UNCHECKED_COLOR  =DARKGRAY;
#else
extern int  CHECKBOX_CHECKED_COLOR    ;
extern int  CHECKBOX_UNCHECKED_COLOR  ;
#endif

#endif