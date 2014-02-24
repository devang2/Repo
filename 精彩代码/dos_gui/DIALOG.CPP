// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	dialog.cpp
*/
#include "dialog.h"
#include "applicat.h"

dialog_class::dialog_class(int ID,char *title,int left,int top,int width,int height)
	:win_class(ID,title,DIALOG_WIN,'2',left,top,width,height)
{ int width1=width-2*LINE_WIDTH;
  int height1=height-bar_height-2*LINE_WIDTH;
  int hbar=0.5*bar_height;
  int vbar=1.5*bar_height;

	strcpy (ok_string,"&OK");
	strcpy (cancel_string,"&Cancel");
	ok_button =new Tbutton (OK_ID,ok_string,hbar,height1-2*vbar,
		width1/2 -2*hbar, vbar);
	cancel_button =new Tbutton (CANCEL_ID,cancel_string,width1 /2+hbar,height1-2*vbar,
		width1/2 -2*hbar, vbar);
	insert_control (cancel_button);
	insert_control (ok_button);

//	pcontrol_current_selected =ok_button;
}

//dialog_class::~dialog_class()
//{
//	delete	(ok_button);
//	delete  (cancel_button);
//}

int dialog_class::msg_handler (MSG& message)
{
	switch (message.Action){
		case KeyPressedMSG:
//		if  (pcurrent_selected_win->type & DIALOG_WIN ) {   //dialog
  int ret_val;
			ret_val=win_class::msg_handler (message);
			if (ret_val!=FALSE) return ret_val;
			if (key_code==ESCKEY) {thequeue.SendMessage(CANCEL_ID,
				ButtonPushedMSG,ok_button);return TRUE;} else
			if (key_code==ENTERKEY) {thequeue.SendMessage(OK_ID,
				ButtonPushedMSG,cancel_button);	return TRUE;}
			break;
		case ButtonPushedMSG:
			switch (message.ID ) {
				case OK_ID:
//				case ok_button:
					if (! (canclose||func_canclose()) ) return FALSE;
					canclose =TRUE;
					thequeue.SendMessage(pcurrent_selected_win->ID,
						WinSysCloseMSG,pcurrent_selected_win);
					thequeue.SendMessage(pcurrent_selected_win->ID,
						DlgOkedMSG,pcurrent_selected_win);
					break;
				case CANCEL_ID:
//				case cancel_button:
					canclose =TRUE;		// must quit
					thequeue.SendMessage(pcurrent_selected_win->ID,
						WinSysCloseMSG,pcurrent_selected_win);
					thequeue.SendMessage(pcurrent_selected_win->ID,
						DlgCanceledMSG,pcurrent_selected_win);
					break;
			}
			return TRUE;
	}
	return win_class::msg_handler (message);
}

