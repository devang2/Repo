#ifndef __TSTATIC_H
#define __TSTATIC_H

#include "yyxctrl.h"

#define ALIGN_LEFT 		0
#define ALIGN_MIDDLE 	1
#define ALIGN_RIGHT		2

typedef class tstatic_class Tstatic;

class tstatic_class: public Tcontrol {
public:
////////////////////////////////////////////////////////in pixels
	int text_pos_x;
	int text_pos_y;

	int static_type;
////////////////////////////////////////////////////////in chars
//	max_value represents is always 0
//  min_value is always 0
//  current_value  is always 0
	char text[255];
public:
	tstatic_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,char *default_text,
		int atype=ALIGN_LEFT);

virtual	void  draw ();
};


#endif