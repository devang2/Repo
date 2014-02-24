// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	checkbox.cpp
*/
#include "checkbox.h"

////////////////////////////////////////////////////////////////////////////
checkbox_class::checkbox_class (int ID,char *title_hotkey,
	int left,int top,int width,int height )
	:	control_class(ID,title_hotkey,NORMAL,CHECKBOX,
			left,top,width,height,UNPUSHED,PUSHED,UNPUSHED )
{
  int text_height=textheight(title);
  int bar =bar_height *0.7;
	title_pos_y =top  + (height-text_height)/2-2;
	title_pos_x =left + (height);

	SetRect (&box,left,top,left+bar,top+bar);
	OffsetRect (&box,(height-bar)/2,(height-bar)/2 );
}

/**************************************************************************/
void checkbox_class::draw ()
{   if ( status & INVISIBLE) return;    // INVISIBLE
	setcolor (BLACK);
	FrameRect(box);
	control_class::draw ();
}
/**************************************************************************/
void checkbox_class::unselect ( )
{   setcolor (WHITE);
	setlinestyle( DOTTED_LINE,1,NORM_WIDTH );
	rectangle (title_pos_x-2,title_pos_y+1,title_pos_x+textwidth (title)+2,
		title_pos_y+textheight (title)+2 );
	setlinestyle( SOLID_LINE,1,NORM_WIDTH );
}
/**************************************************************************/

int checkbox_class::control_change_value
	(int new_value )
{   if (control_class::control_change_value(new_value)==TRUE)
		return TRUE;
	switch (new_value) {
		case UNCHECKED:
			setcolor (WHITE);
			break;
		case CHECKED:
			setcolor (BLACK);
			break;
	}
	line ( box.left,box.top,box.right,box.bottom );
	line ( box.left,box.bottom,box.right,box.top );

	current_value =new_value;
	return TRUE;
}

/**************************************************************************/
int checkbox_class::key_pressed_handler	(int key_scan_num )
{
	switch (key_scan_num ) {
		case SPACEKEY:
			control_change_value
				(1-current_value);
			thequeue.SendMessage(ID,
				CheckBoxValueChangedMSG,this);
			return TRUE;
		default:return control_class::key_pressed_handler (key_scan_num);

	}
}
/**************************************************************************/

int checkbox_class::msg_handler(MSG& message)
{
	switch (message.Action){
		case MouseLButtonDownMSG:
			control_change_value
				(1-current_value);
			thequeue.SendMessage(ID,
				CheckBoxValueChangedMSG,this);
			return TRUE;
		default:return control_class::msg_handler (message);
	}
}
