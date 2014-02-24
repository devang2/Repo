// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR:  XuYongYong

/* 	listbox.cpp
*/
#include "listbox.h"

/**************************************************************************/
listbox_class::listbox_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,
		int therow ,int thecolumn, int string_num, char ** thestring_list)
	:	control_class(ID,title_hotkey,NORMAL ,LISTBOX,
			left,top,width,height,0, string_num-1, -1 )
			//current_value=-1::because no more blue at first
{
	title_pos_x =left ;
	title_pos_y =top -bar_height;
// max_value =string_num +1 ;
	row=therow;
	column=thecolumn;
	if ((row== -1)||(column==-1) ) {
		column=1;
		row =( height -2) / bar_height;
	}
	string_list =thestring_list;
	box.left  =left+1;
	box.top   =top +1;
	box_width =( width - 2  ) /column;
	box_height=( height- 2  ) / row ;
	box.right =box.left+box_width ;
	box.bottom=box.top +box_height;

	setup_control();
}

void listbox_class::setup_control()  //used for every update controls
// max_value changed  // setup inner variables
{
	itemnum =max_value+1;
	begin_show =0;
}

/**************************************************************************/
void listbox_class::draw ( )
{
   if ( status & INVISIBLE) return;    // INVISIBLE

	control_class::draw();
  int k;
  Trect tmprect;

	FillRect (bounds,WHITE);
	setcolor (BLACK);
	FrameRect (bounds);
	setcolor (BLACK);
	for (k=begin_show;(k<begin_show+column*row)&&(k<itemnum) ;k++) {
		tmprect =get_string_rect (k);
		outtextxy (tmprect.left+2, tmprect.top +2, string_list[k] );
	}
}

/**************************************************************************/
Trect listbox_class::get_string_rect(int k)
{ Trect tmprect =box;
  int 	i,j;

	k=k-begin_show;
	j =k % row;
	i =k / row;
	InsetRect (&tmprect,-2,-2);
	OffsetRect (&tmprect,i* box_width , j* box_height);

	return tmprect;
}
/**************************************************************************/
void listbox_class::select ( )
{ Rect tmprect;

	if ((current_value<min_value )||(current_value>max_value)) return ;
	tmprect =get_string_rect (current_value);
	FillRect (tmprect,BLUE );
	setcolor (WHITE);
	outtextxy (tmprect.left+2, tmprect.top +2, string_list[current_value]);

	setcolor (WHITE);
	setlinestyle( DOTTED_LINE,1,NORM_WIDTH );
	FrameRect (get_string_rect(current_value));
	setlinestyle( SOLID_LINE,1,NORM_WIDTH );
}

/**************************************************************************/
void listbox_class::unselect ( )
{ Rect tmprect;

	if ((current_value<min_value )||(current_value>max_value)) return ;
	tmprect =get_string_rect (current_value);
	FillRect (tmprect,WHITE );
	setcolor (BLACK);
	outtextxy (tmprect.left+2, tmprect.top +2, string_list[current_value]);

	setcolor (WHITE);
	setlinestyle( DOTTED_LINE,1,NORM_WIDTH );
	FrameRect (get_string_rect(current_value));
	setlinestyle( SOLID_LINE,1,NORM_WIDTH );
}
/**************************************************************************/
int listbox_class::control_change_value	(int new_value  )
{  	if (control_class::control_change_value(new_value)==TRUE)
		return TRUE;
  Trect tmprect;
	if (new_value < begin_show) {
		begin_show =new_value;
		draw();
	} else if ( (new_value >=begin_show+column*row) ) {
		begin_show= new_value -column*row+1;
		draw();
	}  else {
		if (!( status & INVISIBLE))  unselect ();
	}
	current_value =new_value;
	if (!( status & INVISIBLE)) select();
	thequeue.SendMessage(ID,ListBoxValueChangedMSG,this);
	return TRUE;
}

/**************************************************************************/
int listbox_class::key_pressed_handler
	(int key_scan_num )
{ int new_value;

	switch (key_scan_num )	{
	case LEFTKEY:
		new_value =current_value -row;
		if (new_value<0 ) new_value =0;
		break;
	case RIGHTKEY:
		new_value =current_value +row;
		if (new_value>= max_value ) new_value =max_value;
		break;
	case UPKEY:
		new_value =current_value -1;
		if (new_value<0 )  new_value = 0;
		break;
	case DOWNKEY:
		new_value =current_value +1;
		if (new_value>= max_value) new_value =max_value;
		break;
	case HOMEKEY:
		new_value =0;
		break;
	case ENDKEY:
		new_value =max_value;
		break;
	case PGUPKEY:
		new_value =current_value -row;
		if (new_value<0 )  new_value = 0;
		break;
	case PGDNKEY:
		new_value =current_value +row;
		if (new_value>= max_value) new_value =max_value;
		break;
	case SPACEKEY:
		thequeue.SendMessage(ID,
			ListBoxItemSelectedMSG,this);
		return TRUE;
//		break;
//	default : return FALSE;	//this is another way to do below
	default : return control_class::key_pressed_handler (key_scan_num) ;
	}
//	return control_change_value(new_value );
	control_change_value(new_value );
	return TRUE;
}


int listbox_class::msg_handler	(MSG& message )
{ int x,y ,k;
	switch (message.Action){
		case MouseLButtonDownMSG:
			x=win_mouse_x - bounds.left;
			y=win_mouse_y - bounds.top;
			x=x / box_width ;
			y=y / box_height;
			k=begin_show  + x*row + y;
			if (k==current_value) {
					thequeue.SendMessage(ID,
				ListBoxItemSelectedMSG,this);
				return TRUE;
			}
			if ( k <= max_value ) {
				control_change_value( k );
				return TRUE;
			}
	}
	return	control_class::msg_handler(message);
}




















