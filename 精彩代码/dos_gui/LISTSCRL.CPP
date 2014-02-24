// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR:  XuYongYong

/* listscrl.cpp
   a group with a listbox & a scroll
*/

#include "listscrl.h"

/**************************************************************************/
listscrl_class::listscrl_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,
		int string_num, char ** thestring_list)
	:	group_class(ID,title_hotkey,left,top,width,height)
// gruop 's min current max value is always 0
{
	title_pos_x =left ;
	title_pos_y =top -bar_height-1;

	listbox =new Tlistbox ( ((ID<<8)|222),"",left,top,width-bar_height,height,-1,-1,
		string_num,thestring_list);
	scroll  =new Tscroll  ( ((ID<<8)|111),"",left+width-bar_height,top,bar_height,height,
		0,( (string_num>1)?string_num-1:1),0);
	insert_control (listbox);
	insert_control (scroll);
}

void listscrl_class::setup_control(  )
{  // outer changed listbox 's max_value ; that's enough
	scroll ->max_value =listbox ->max_value ;
	if (scroll->max_value<1) scroll->max_value =1; //min is 0

	scroll ->current_value =listbox ->current_value ;
	if (scroll->current_value<0 ) scroll->current_value =0;
	// can only used here
	listbox->setup_control( );
	scroll ->setup_control( );
}
/**************************************************************************/
void listscrl_class::select ( )
{
	listbox->select();
	scroll ->select();
}

/**************************************************************************/
void listscrl_class::unselect ( )
{
	listbox->unselect();
	scroll ->unselect();
}

/**************************************************************************/
int listscrl_class::key_pressed_handler   	(int key_scan_num )
{
	return listbox->key_pressed_handler(key_scan_num);
}

int listscrl_class::msg_handler (MSG& message)
{
  static int flag=0;

	switch (message.Action) {
		case ListBoxValueChangedMSG:
//			if ( (message.ID & 0xFF)==222) {
			if ( message.fptr==listbox) {
//				if (flag !=2){
					if (scroll->current_value !=listbox->current_value)
						scroll->control_change_value(listbox->current_value);
//					flag =1;
//				}
				return TRUE; //maybe FALSE
			}
		case ScrollValueChangedMSG:
			if (message.fptr==scroll) {
//				if (flag !=1){
					if (listbox->current_value !=scroll->current_value)
						listbox->control_change_value(scroll->current_value);
//					flag =2;
//				}
				return TRUE; //maybe FALSE
			}
		default:return group_class::msg_handler (message);
	}
}
















