#ifndef __BUTTON_H
#define __BUTTON_H

#include "yyxsys.h"
#include "yyxctrl.h"

#define PUSHED 1
#define UNPUSHED 0

typedef class button_class Tbutton;

class button_class: public Tcontrol {
public:
	button_class(int ID,char *title_hotkey,
		int left,int top,int width,int height);

	virtual void  setup_control( );
	virtual	void  draw ( );
	virtual void  select ( );
	virtual void  unselect ( );
	virtual int  key_pressed_handler  (int key_scan_num );
	virtual int  msg_handler(MSG& message);

	virtual int  control_change_value ( int new_value);

};

#ifdef __YYXMAIN
 int  BUTTON_PUSHED_COLOR    =LIGHTGRAY;
 int  BUTTON_UNPUSHED_COLOR  =DARKGRAY;
#else
extern int  BUTTON_PUSHED_COLOR    ;
extern int  BUTTON_UNPUSHED_COLOR  ;
#endif

#endif