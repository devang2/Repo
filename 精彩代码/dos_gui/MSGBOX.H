#ifndef __MSGBOX_H
#define __MSGBOX_H

#include "dialog.h"
#include "tstatic.h"

#define MB_OKCANCEL 	0x01
#define MB_RETRYCANCEL	0x02
#define MB_OK 			0x04

#define MB_ERROR		0x08
#define MB_WARING		0x10

typedef class msgbox_class Tmsgbox;

class msgbox_class :public Tdialog {
public:
	Tstatic * 	msg_tstatic;
public:
	msgbox_class (char *title,char *msg_contents,int atype);
/*   Usage::
	Result = MessageBox(GetFocus(),"Could not Create Timer", "GDIDemo", MB_RETRYCANCEL);
  };
  if (Result == IDCANCEL)  PostQuitMessage(0);
*/

};

#endif