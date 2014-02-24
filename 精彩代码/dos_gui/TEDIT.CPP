// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR: XuYongYong

/* 	yyxtedit.cpp
*/
#include "tedit.h"
#include "applicat.h"
/**************************************************************************/
tedit_class::tedit_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,char *default_text,
		int max_len)
	:  control_class(ID,title_hotkey,NORMAL ,TEDIT,
			left,top,width,height,0,0,0 )
{
	title_pos_x =left ;
	title_pos_y =top -bar_height-1 ;

	text_pos_x  =left+3*LINE_WIDTH;
	text_pos_y 	=top +( height-textheight("j") )/2-2;
	strcpy (text,default_text);
	max_text_len =max(max_len,strlen (text) +1);
	max_text_show_len 	= (width -2*LINE_WIDTH ) / textwidth ("M");

	setup_control();
}

void tedit_class::setup_control()
{
	max_text_len =max(max_text_len,strlen (text) );
	cursor_pos =strlen (text)-1;
  int i;
	i = (max_text_show_len <strlen (text)) ?max_text_show_len: strlen(text) ;
	text_begin_show =cursor_pos -i+1;
	strncpy (text_show ,text+text_begin_show,i+1);
	is_first_press=TRUE;
	cursor_pos_x= textwidth(text_show)-2;
}
/**************************************************************************/
void tedit_class::draw ()
{
	if ( status & INVISIBLE) return;    // INVISIBLE

	FillRect (bounds,WHITE);
	setcolor (BLACK);
	FrameRect ( bounds );
	outtextxy (text_pos_x,text_pos_y, text_show );
	control_class::draw ();
}
/**************************************************************************/
void tedit_class::select ()
{
	setfillstyle (SOLID_FILL, BLUE);
	bar (text_pos_x,text_pos_y+1,text_pos_x + textwidth (text_show),
		text_pos_y + textheight (text_show) + 4 );
	setcolor (WHITE);
	outtextxy (text_pos_x,text_pos_y, text_show );
	is_first_press =TRUE;
}
/**************************************************************************/
void tedit_class::unselect ()
{
	setfillstyle (SOLID_FILL, WHITE);
	bar (text_pos_x,text_pos_y+1,text_pos_x + textwidth (text_show),
		text_pos_y+textheight (text_show) + 4 );
	setcolor (BLACK);
	outtextxy (text_pos_x,text_pos_y,text_show );
	is_first_press =FALSE;
	thequeue.SendMessage(ID,EditInputedMSG,this);
}
/**************************************************************************/
int tedit_class::adjust_cursor(int new_cursor_pos)
{ int ret_val=FALSE;
  int i;
	for (i=0;i<=max_text_show_len;i++) text_show[i]=0;
	strncpy (text_show , text+text_begin_show, max_text_show_len);

	setcolor (WHITE);
    draw_cursor();

	if ( ( (new_cursor_pos+1) < 0)  ||(  (new_cursor_pos+1)>=strlen(text)+1 ) )	return FALSE;		// unchanged
	if (cursor_pos==new_cursor_pos) return FALSE;
	cursor_pos= new_cursor_pos;
	if (( cursor_pos < text_begin_show )&&(cursor_pos !=-1)) {
		text_begin_show=cursor_pos;
		for (i=0;i<max_text_show_len;i++) text_show[i]=0;
		strncpy (text_show , text+text_begin_show, max_text_show_len);
		ret_val=TRUE;  	    //drawed
	}	else if ( (cursor_pos+1) >= (text_begin_show+strlen(text_show)+1) )
	{	text_begin_show=cursor_pos-max_text_show_len+1;
		for (i=0;i<max_text_show_len;i++) text_show[i]=0;
		strncpy (text_show , text+text_begin_show, max_text_show_len);
		ret_val=TRUE;		// drawed
	}
  char tmpstr[255]="";
	strncpy (tmpstr,text_show,cursor_pos - text_begin_show+1);
	tmpstr[cursor_pos - text_begin_show+2]=0;
	cursor_pos_x= textwidth(tmpstr)-2;
	return ret_val;
}

void tedit_class::draw_cursor()
{
	line (text_pos_x + cursor_pos_x +1, text_pos_y+1 ,
		text_pos_x + cursor_pos_x+1 , text_pos_y +textheight("j") + 3);
}
/**************************************************************************/
int tedit_class::key_pressed_handler  	(int key_scan_num )
{ int i;
  static char temp_string [100];

	if  (control_class::key_pressed_handler (key_scan_num)==TRUE)
		return TRUE;
	else	// MAY BE PUT HERE --> No matter at all
	switch (key_scan_num ) {
		case    LEFTKEY 	:
			if (adjust_cursor (	cursor_pos -1 ))
				draw();
			break;
		case    RIGHTKEY 	:
			if (adjust_cursor (	cursor_pos +1 ))
				draw();
			break;
		case 	HOMEKEY		:
			if (adjust_cursor ( 0 ))
				draw();
			break;
		case 	ENDKEY		:
			if	(adjust_cursor (	strlen (text)  ))
				draw();
			break;
		case	BSKEY		:
			if (!strlen(text)) return TRUE;
			if (cursor_pos >=0) {
				if (text_begin_show>0) text_begin_show--;
				strcpy ( text + cursor_pos , text + cursor_pos+1 );
				adjust_cursor ( cursor_pos -1 );
				draw();
			}
			break;
		case 	DELKEY		:
			if (!strlen(text)) return TRUE;
			if ( (cursor_pos+1) <strlen(text)) {
				strcpy ( text + cursor_pos+1 , text +cursor_pos+2 );
				adjust_cursor ( cursor_pos );
				draw();
			}
			break;
		case    ENTERKEY	:
			thequeue.SendMessage(ID,EditInputedMSG,this);
			key_code = TABKEY;
			thequeue.SendMessage(0, KeyPressedMSG,NULL);
			break;
		default				:
			if (key_scan_num % 256 ==0 ) return FALSE;
			if (key_scan_num ==ESCKEY ) return FALSE;

			if (strlen(text)>max_text_len) {
				printf("\007");
				return TRUE;
			}
			if (is_first_press) {
				text[0]=0;
				text_begin_show=0;
				adjust_cursor(-1);
				draw();
				is_first_press =FALSE;
			}
			strcpy (temp_string,text + cursor_pos+1);
			text[cursor_pos+1] = (key_scan_num % 256 ) ;
			text[cursor_pos+2] = 0;
			strcpy (text+cursor_pos+2,temp_string);

			if (cursor_pos-text_begin_show +1>=max_text_show_len)
			   text_begin_show++;
			adjust_cursor ( cursor_pos +1 );
			draw();
			break;
	}
	if (is_first_press) {
		draw();
		is_first_press =FALSE;
	}
	setcolor (BLACK);
	draw_cursor();
	return TRUE;
}

int tedit_class::msg_handler	(MSG& message )
{ static int flag;
	switch (message.Action){
		case TimerMSG:
			if (in_menu_trap) return FALSE;
			flag =!flag;
			if (flag) setcolor(BLACK);
				else setcolor(WHITE);
			draw_cursor();
			return FALSE; //others also need it
	}
	return control_class::msg_handler	(message);
}
