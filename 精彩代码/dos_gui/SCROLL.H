#ifndef __SCROLL_H
#define __SCROLL_H

#include "yyxctrl.h"

#define min(x,y) ((x<y)?x:y)

typedef class scroll_class Tscroll;

class scroll_class: public Tcontrol {
public:
	bool  is_hscroll;
	int   bar_x;
	int   bar_y;

	Trect begin_box;
	Trect end_box;
	Trect begin_end_box;

	Trect thumb_box;
	Trect begin_thumb_box;
	Trect thumb_end_box;
public:
scroll_class::scroll_class
	(int ID,char *title_hotkey,	 int left,int top,int width,int height,
	 int min_value,int max_value,int current_value );

virtual	void  draw ( );
virtual	void  select ( );
virtual	void  unselect ( );
virtual	int   key_pressed_handler  (int key_scan_num );
virtual int   msg_handler	(MSG& message );

public:
virtual	int   control_change_value ( int new_value );

protected:
	void get_thumb_boxes (int thumb_value);
};

#ifdef __YYXMAIN
 int  SCROLL_BAR_COLOR 	   	=LIGHTGRAY;
 int  SCROLL_THUMB_COLOR 	=DARKGRAY;
#else
extern int  SCROLL_BAR_COLOR 	   ;
extern int  SCROLL_THUMB_COLOR 	   ;
#endif

#endif