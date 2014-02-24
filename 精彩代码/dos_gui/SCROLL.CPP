// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	scroll.cpp
*/
#include "scroll.h"
#include "applicat.h"
/**************************************************************************/
scroll_class::scroll_class
	(int ID,char *title_hotkey,	 int left,int top,int width,int height,
	 int min_value,int max_value,int current_value )
	 :control_class
	(ID,title_hotkey,NORMAL ,SCROLL,left,top,width,height,
		min_value,max_value,current_value	 )
{
	title_pos_x =left ;
	title_pos_y =top -bar_height-1 ;

	is_hscroll = ((width >height) ? TRUE:FALSE );
	if (is_hscroll) bar_x =min(height-1,bar_height);
		else bar_x=width-1;
	if (is_hscroll) bar_y =height-1;
		else bar_y=min(width-1,bar_height);

	SetRect (&begin_box,left,top,left+bar_x,top+bar_y);
	end_box =begin_box;
	if (is_hscroll) OffsetRect (&end_box,width-bar_x,0);
		else OffsetRect (&end_box,0,height-bar_y);

	begin_end_box =bounds;
	if (is_hscroll) InsetRect (&begin_end_box,-bar_x,0);
		else InsetRect (&begin_end_box,0,-bar_y);

	thumb_box =begin_end_box;
	if (is_hscroll) thumb_box.right=thumb_box.left+bar_x;
		else  thumb_box.bottom=thumb_box.top+bar_y;

	begin_thumb_box =begin_end_box;
	thumb_end_box =begin_end_box;
}

/**************************************************************************/
void scroll_class::draw ( )
{
	if ( status & INVISIBLE) return;    // INVISIBLE

	FillRect (bounds, LIGHTGRAY);
	setcolor (BLACK);
	control_class::draw();

	draw_out_button (begin_box);
	draw_out_button (end_box  );
	if (is_hscroll) {
		draw_arrow (begin_box.left,begin_box.top,begin_box.right-LINE_WIDTH,
			begin_box.bottom,LEFT,BLACK);
		draw_arrow (end_box.left,end_box.top,end_box.right-LINE_WIDTH,
			end_box.bottom,RIGHT,BLACK);
	}  else {
		draw_arrow (begin_box.left,begin_box.top,begin_box.right-LINE_WIDTH,
			begin_box.bottom,UP,BLACK);
		draw_arrow (end_box.left,end_box.top,end_box.right-LINE_WIDTH,
			end_box.bottom,DOWN,BLACK);
	}
}
/**************************************************************************/

void scroll_class::select( )
{ Trect tmprect =thumb_box;
	InsetRect (&tmprect,-2,-2);
	tmprect.right -=3; tmprect.bottom-=3;
	FillRect (tmprect, DARKGRAY);
}

void scroll_class::unselect( )
{ Trect tmprect =thumb_box;
	InsetRect (&tmprect,-2,-2);
	tmprect.right -=3; tmprect.bottom-=3;
	FillRect (tmprect, LIGHTGRAY);
}

/**************************************************************************/
void scroll_class::get_thumb_boxes (int thumb_value)
{ int i,j;
	if (is_hscroll) {
		i=(begin_end_box.right-begin_end_box.left-bar_x)
			 * thumb_value / (max_value-min_value);
		j=thumb_box.left-begin_end_box.left;
		OffsetRect(&thumb_box,i-j,0);
		begin_thumb_box.right =begin_thumb_box.left+i;
		thumb_end_box.left =i+bar_x;
	}  	//HSCROLL
	else {	//bar ==width -2
		i=(begin_end_box.bottom-begin_end_box.top-bar_y)
			 * thumb_value / (max_value-min_value);
		j=thumb_box.top-begin_end_box.top;
		OffsetRect(&thumb_box,0,i-j);
		begin_thumb_box.bottom =begin_thumb_box.top+i;
		thumb_end_box.top =i+bar_y;
	}	//VSCROLL
}
/**************************************************************************/
int scroll_class::control_change_value 	(int new_value )
{  	if (control_class::control_change_value(new_value)==TRUE)
		return TRUE;

	FillRect (thumb_box,LIGHTGRAY);
	get_thumb_boxes (new_value);
	draw_out_button (thumb_box);
	setcolor(BLACK);

	FrameRect (bounds);
	current_value =new_value;
	thequeue.SendMessage(ID,ScrollValueChangedMSG,this);
	return TRUE;
}

/**************************************************************************/
int scroll_class::key_pressed_handler	(int key_scan_num )
{ int i;
	switch (key_scan_num ) {
		case 	UPKEY:
		case    LEFTKEY 	:
			control_change_value (current_value -1 );
			return TRUE;
		case 	DOWNKEY:
		case    RIGHTKEY 		:
			control_change_value (current_value +1 );
			return TRUE;
		case 	HOMEKEY:
			control_change_value (min_value);
			return TRUE;
		case 	ENDKEY:
			control_change_value (max_value);
			return TRUE;
		case	PGDNKEY:
			i=current_value + ((max_value-min_value)/10+1) ;
			i=(i>max_value)? max_value:i;
			control_change_value (i);
			return TRUE;
		case 	PGUPKEY:
			i=current_value - ((max_value-min_value)/10+1) ;
			i=(i<min_value)? min_value:i;
			control_change_value (i);
			return TRUE;
	}
	return control_class::key_pressed_handler (key_scan_num);
}
/**************************************************************************/
int scroll_class::msg_handler	(MSG& message )
{ static int flag;
  int i;

	switch (message.Action){
		case TimerMSG:
			if (in_menu_trap) break;
			flag =!flag;
			if ( status & INVISIBLE) return TRUE;    // INVISIBLE
			if (flag) unselect();
				else select();
			break;
		case MouseLButtonDownMSG:
			if (PtInRect (win_mouse_x,win_mouse_y,begin_box)) {
				control_change_value (current_value -1 );
			}else
			if (PtInRect (win_mouse_x,win_mouse_y,end_box)) {
				control_change_value (current_value +1 );
			}else
			if (PtInRect (win_mouse_x,win_mouse_y,begin_thumb_box)) {
				i=current_value - ((max_value-min_value)/10+1) ;
				i=(i<min_value)? min_value:i;
				control_change_value (i);
			}else
			if (PtInRect (win_mouse_x,win_mouse_y,thumb_end_box)) {
				i=current_value + ((max_value-min_value)/10+1) ;
				i=(i>max_value)? max_value:i;
				control_change_value (i);
			}else break;
			return TRUE;
	}

	return	control_class::msg_handler(message);
}
