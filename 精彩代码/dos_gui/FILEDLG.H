#ifndef __FILEDLG_H
#define __FILEDLG_H

#include "dialog.h"
#include "checkbox.h"
#include "radio.h"
#include "scroll.h"
#include "tedit.h"
#include "listbox.h"

typedef class filedlg_class Tfiledlg;

class filedlg_class :public Tdialog {
public:
	Tcheckbox * acheckbox;
	Tradio	*aradio;
	Tscroll *hscroll;
	Tscroll *vscroll;
	Ttedit  *tedit;
	Ttedit  *tedit2;
	Tlistbox *listbox;
public:
	filedlg_class(int ID,char *title,int left,int top,int width,int height);
	~filedlg_class ();
};

#endif
