// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	button.cpp
*/
#include "button.h"
#include "yyxsys1.h"

/**************************************************************************/
button_class::button_class (int ID,char *title_hotkey,
	int left,int top,int width,int height )
	:	control_class(ID,title_hotkey,NORMAL ,BUTTON,
			left,top,width,height,UNPUSHED,PUSHED,UNPUSHED )
{
	setup_control();
}

void button_class::setup_control()
{
  int text_width =textwidth (title);
  int text_height=textheight(title);
	title_pos_x =bounds.left + (bounds.right-bounds.left-text_width )/2;
	title_pos_y =bounds.top  + (bounds.bottom-bounds.top-text_height)/2-2;
}
void button_class::draw ()
{   if ( status & INVISIBLE) return;    // INVISIBLE
	draw_out_button (bounds);
	control_class::draw ();
}
/**************************************************************************/
void button_class::select ( )
{   control_class::select ();

  Trect Arect =bounds;
	setcolor (BLACK);
	FrameRoundRect (Arect,2,2);
	InsetRect (&Arect,1,1);
	FrameRoundRect (Arect,2,2);
}
/**************************************************************************/
void button_class::unselect ( )
{   control_class::unselect ( );

  Trect Arect =bounds;
	setcolor (WHITE);
	InsetRect (&Arect,1,1);
	FrameRoundRect (Arect,2,2);
}
/**************************************************************************/
int button_class::control_change_value
	(int new_value )
{   if (control_class::control_change_value(new_value)==TRUE)
		return TRUE;
	switch (new_value) {
		case UNPUSHED:
			draw ();
			break;
		case PUSHED:
			FillRoundRect (bounds,2,2,LIGHTGRAY);
			setcolor (BLACK);
			outtextxy (title_pos_x,title_pos_y,title);
			break;
	}
	current_value =new_value;
	return TRUE;
}
/**************************************************************************/
int button_class::key_pressed_handler	(int key_scan_num)
{   switch (key_scan_num ) {
		case ENTERKEY:
		case SPACEKEY:
			control_change_value
				(PUSHED);
			delay(200);
			control_change_value
				(UNPUSHED);
			thequeue.SendMessage(ID,
				ButtonPushedMSG,this);
			return TRUE;
		default:return control_class::key_pressed_handler (key_scan_num);

	}
}

int button_class::msg_handler(MSG& message)
{
	switch (message.Action){
		case MouseLButtonDownMSG:
			control_change_value
				(PUSHED);
			delay(200);
			control_change_value
				(UNPUSHED);
			thequeue.SendMessage(ID,
				ButtonPushedMSG,this);
			return TRUE;
		default:return control_class::msg_handler (message);

	}
}
