#ifndef __DIALOG_H
#define __DIALOG_H

#include "yyxwin.h"
#include "button.h"

typedef class dialog_class Tdialog;

#define OK_ID 		1
#define CANCEL_ID	2

#define Dlg_OK		2
#define Dlg_CANCEL  3
#define Sys_QUIT 	0xff

class dialog_class :public Twin {
public:
	Tbutton * ok_button;
	Tbutton * cancel_button;
protected:
	char ok_string[25];
	char cancel_string[25];
public:
	dialog_class(int ID,char *title,int left,int top,int width,int height);
virtual int  msg_handler (MSG& message);
//virtual	~dialog_class ();
};


#endif