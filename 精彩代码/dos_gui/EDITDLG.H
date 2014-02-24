#ifndef __EDITDLG_H
#define __EDITDLG_H

#include "dialog.h"
#include "tedit.h"

typedef class editdlg_class Teditdlg;

class editdlg_class :public Tdialog {
public:
	Ttedit * 	content;
	char * buf;
public:
		editdlg_class(int ID,char *title,int left,int top,int width,int height,
			char * old_content );
	virtual BOOL func_canclose();
};


#endif
