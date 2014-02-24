// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR:  XuYongYong

/* 	yyxctrl.cpp
*/
#include "yyxctrl.h"
#include "applicat.h"
/**************************************************************************/
void control_class::clear_control ( )
{
//	setcolor (WHITE);
//	setfillstyle (SOLID_FILL, WHITE);
//	if (title[0] !='\x0')  {  bar (title_pos_x,title_pos_y,
//		title_pos_x+textwidth(title),title_pos_y +textheight (title)); }
	FillRect (bounds, WHITE);
}

void  control_class::setup_control( )
{   // used for setting up inner variables , protected or private
	// not public---> public is defined by outer functions
}

void  control_class::update_control ( )
{
	clear_control();
	setup_control();
	draw ();
	control_change_value (current_value);
}

void control_class::draw ( )
{   if ( status & INVISIBLE) return;    // INVISIBLE
	setcolor (BLACK);
	outtextxy (title_pos_x,title_pos_y,title);
	draw_hotkey(title,hotkey,title_pos_x,title_pos_y);
}

void control_class::select ( )
{  // INVISIBLE here not detected //to save time
	setcolor (DARKGRAY);
	setlinestyle( DOTTED_LINE,1,NORM_WIDTH );
	rectangle (title_pos_x-2,title_pos_y+1,title_pos_x+textwidth (title)+2,
		title_pos_y+textheight (title)+2 );
	setlinestyle( SOLID_LINE,1,NORM_WIDTH );
}

void control_class::unselect ( )
{  // INVISIBLE here not detected //to save time
	setcolor (LIGHTGRAY);
	setlinestyle( DOTTED_LINE,1,NORM_WIDTH );
	rectangle (title_pos_x-2,title_pos_y+1,title_pos_x+textwidth (title)+2,
		title_pos_y+textheight (title)+2 );
	setlinestyle( SOLID_LINE,1,NORM_WIDTH );
}

int control_class::control_change_value
	( int new_value)
{
	if ((new_value<	min_value)||
		(new_value>	max_value))	return TRUE;
	else return FALSE;
}

int control_class:: key_pressed_handler (int key_scan_num )
{  	return FALSE;
}


int control_class::msg_handler (MSG& message)
{ // may be should exists in group key_pressed_handler
	switch (message.Action){
		case KeyPressedMSG:
			return key_pressed_handler( key_code );
	}
	return FALSE;
}


control_class::control_class
	(int ID,char *title_hotkey,byte status,byte type,
	 int left,int top,int width,int height,
	 int min_value,int max_value,int current_value
	 ):object_class (ID,title_hotkey,status,type,left,top,width,height)
{// Tcontrol  *ptemp_control, *ptemp1;

	title_pos_x =left+20;
	title_pos_y =top+2;

	this->min_value =min_value;
	this->current_value =current_value;
	this->max_value =max_value;

	pleft  =this;     /* in windows */
	pright =this;     /* user complete the last */
}

control_class::~control_class()
{
}

