#ifndef __YYXCTRL_H
#define __YYXCTRL_H

#include "yyxobj.h"


#define CONTROL 	0

#define BUTTON		1
#define CHECKBOX 	2
#define RADIO		3
#define SCROLL  	4
//#define H_SCROLLBAR 5
//#define V_SCROLLBAR 6
#define TSTATIC 	7
#define TEDIT 		8
#define LISTBOX		9
#define GROUP 		10
//#define STRING_LIST	10
//#define RADIOS 		11

#define DRAW_CONTROL 		1
#define SELECT_CONTROL 		2
#define UNSELECT_CONTROL 	3

#define NORMAL		0x00

#define DISABLE 	0x01		// status & DISBLE ---TRUE is non-zero
#define INVISIBLE   0x02


class control_class :public Tobject{
public:
	int 		title_pos_x;
	int 		title_pos_y;
	int			min_value;
	int         max_value;
	int			current_value;
/////////////////////////////////////////////////////////////////////////
public:
virtual	void  draw ( );
virtual	void  select (  );
virtual	void  unselect (  );

virtual	int  control_change_value ( int new_value );
virtual	int  key_pressed_handler  (int key_scan_num );
virtual	int  msg_handler (MSG& message);

public:
virtual	void  clear_control ( );
virtual	void  update_control ( );
virtual	void  setup_control ( );

	control_class::control_class
	(int ID,char *title_hotkey,byte status,byte type,
	 int left,int top,int width,int height,
	 int min_value,int max_value,int current_value
	 );
virtual	~control_class();
};

#ifdef __YYXMAIN

 int  CONTROL_FRAME_COLOR=RED;
/* Note :enable & disable ---> show the text */
 int  CONTROL_DISABLED_COLOR =YELLOW;
 int  CONTROL_ENABLED_COLOR  =BROWN;
 int  CONTROL_SELECTED_COLOR =GREEN;
 int  CONTROL_TITLE_COLOR    =YELLOW;

#else
extern int  CONTROL_FRAME_COLOR	   ;
/* Note :enable & disable ---> show the text */
extern int  CONTROL_DISABLED_COLOR ;
extern int  CONTROL_ENABLED_COLOR  ;
extern int  CONTROL_SELECTED_COLOR ;
extern int  CONTROL_TITLE_COLOR    ;

#endif
/*------------------------------------------------------------------------*/
#endif