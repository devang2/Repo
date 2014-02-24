// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR: XuYongYong

/* 	radio.cpp
*/
#include "radio.h"

/**************************************************************************/
radio_class::radio_class (int ID,char *title_hotkey,
	int left,int top,int width,int height )
	:checkbox_class(ID,title_hotkey,left,top,width,height)
{	type =RADIO;}

/***************************************************************************/
void radio_class::draw ()
{
   if ( status & INVISIBLE) return;    // INVISIBLE
	setcolor (BLACK);
	FrameOval (box);
	control_class::draw ();
}
/**************************************************************************/
int radio_class::control_change_value
	(int new_value )
{   if (control_class::control_change_value(new_value)==TRUE)
		return TRUE;
  Trect tmprect=box;
	InsetRect (&tmprect,-2,-2);
	switch (new_value) {
		case UNCHECKED:
			FillOval (box,WHITE);
			break;
		case CHECKED:
			FillOval (tmprect,BLACK);
			break;
	}
	current_value =new_value;
	return TRUE;
}
/**************************************************************************/
