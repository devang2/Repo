// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR: XuYongYong

/* 	filedlg.cpp
	only for test
*/
#include "filedlg.h"

filedlg_class::filedlg_class(int ID,char *title,int left,int top,int width,int height)
	:dialog_class(ID,title,left,top,width,height)
{
	acheckbox 	=new Tcheckbox (3,"&checkbox",10,5,70,20);
	aradio	  	=new Tradio	 (4,"&radio",120,5,100,20 );
	hscroll     =new Tscroll (5,"",10,40,100,30,0,100,1);
	vscroll		=new Tscroll (6,"",150,60,20,80,0,10,4);
	tedit 		=new Ttedit  (7,"ted&it",10,90,90,30,"Sam",10);
	tedit2		=new Ttedit  (8,"2t&edit",200,90,70,30,"0123456789",15) ;
static  char *cc[8]={"11","22","33","44","55","6","7","8"};
	listbox		=new Tlistbox(9,"H&HJH",200,20,80,50,-1,-1,8,cc);
//  insert_control //
	insert_control (acheckbox);
	insert_control (aradio);
	insert_control (hscroll);
	insert_control (vscroll);
	insert_control (tedit);
	insert_control (tedit2);
	insert_control (listbox);
}

filedlg_class::~filedlg_class()
{
}


