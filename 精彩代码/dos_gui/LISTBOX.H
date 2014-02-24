#ifndef __LISTBOX_H
#define __LISTBOX_H

#include "yyxctrl.h"

typedef class listbox_class Tlistbox;

/*
struct string_list_struct {
	struct Rect bounds;
	char *string;
};
*/

class listbox_class: public Tcontrol {
public:   // corrected by outer function
	char ** string_list;
protected:  //inner use
	int 	row;
	int 	column;
	int 	itemnum;
	Trect 	box;
	int 	box_width;
	int		box_height;
	int 	begin_show;
private:
	Trect get_string_rect(int k );

public:
listbox_class::listbox_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,
		int therow ,int thecolumn, int string_num, char ** thestring_list);
virtual void  setup_control(  );
virtual	int   control_change_value ( int new_value );

virtual	void  draw (  );
virtual	void  select (  );
virtual	void  unselect (  );
virtual	int   key_pressed_handler  (int key_scan_num );
virtual int   msg_handler	(MSG& message );
};

#ifdef __YYXMAIN
 int  STRING_SELECTED_COLOR  =BLUE;
 int  STRING_COLOR 		   =YELLOW;
#else
extern int  STRING_SELECTED_COLOR  	;
extern int  STRING_COLOR 		   	;
#endif

#endif

