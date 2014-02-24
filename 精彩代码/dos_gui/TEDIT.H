#ifndef __TEDIT_H
#define __TEDIT_H

#include "yyxctrl.h"

typedef class tedit_class Ttedit;

class tedit_class: public Tcontrol {
public:
////////////////////////////////////////////////////////in pixels
	int text_pos_x;
	int text_pos_y;
////////////////////////////////////////////////////////in chars
//	max_value represents is always 0
//  min_value is always 0
//  current_value  is always 0
	int  max_text_len ;
	int  max_text_show_len; // length of 'viewport'
	int  text_begin_show;   // from text's beginning //
	int  cursor_pos		;	// from text's beginninh //

	int  cursor_pos_x	;	// in pixels
	char text[255];
	char text_show[255];
	BOOL is_first_press;
public:
	tedit_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,char *default_text,
		int max_len=0);
virtual void  setup_control();

virtual	void  draw ();
virtual	void  select ( );
virtual	void  unselect ( );
virtual	int   key_pressed_handler  (int key_scan_num );
virtual int   msg_handler	(MSG& message );

protected:
	int 	adjust_cursor(int new_cursor_pos);
	void	draw_cursor();  //counted in chars, global coordinate
};

#ifdef __YYXMAIN
 int  TEDIT_SELECTED_COLOR   =DARKGRAY;
 int  TEDIT_BACKGROUND_COLOR =LIGHTGRAY;
 int  TEDIT_CHAR_COLOR 	   =WHITE;
 int  TEDIT_CURSOR_COLOR	   =RED;
#else
extern int  TEDIT_SELECTED_COLOR   ;
extern int  TEDIT_BACKGROUND_COLOR ;
extern int  TEDIT_CHAR_COLOR 	   ;
extern int  TEDIT_CURSOR_COLOR	   ;
#endif

#endif