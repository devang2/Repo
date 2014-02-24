#ifndef __YYXOBJ_H
#define __YYXOBJ_H

#if !defined(__YYXSYS_H)
#include "yyxsys.h"
#endif

#if !defined(__YYXMSG_H)
#include "yyxmsg.h"
#endif

enum direction {TO_UP,TO_DOWN};

typedef class object_class Tobject;

class object_class {
public:
	long    ID;
	char	title[40];
	char 	hotkey;

	byte 	status;
	byte 	type;
	Trect 	bounds;

	Tobject *pleft;  //all are the same kind;
	Tobject *pright;
public:
	object_class(int ID,char *title_hotkey,byte status,byte type,
	 int left,int top,int width,int height	);
virtual ~object_class();

virtual void	select		() 		;
virtual void 	unselect 	()  	;
virtual void  	draw 		()  	;

virtual int   	key_pressed_handler  ( int key_scan_num )	;
virtual int     msg_handler (MSG& message )				    ;

// LOOKING THRU LINKS; ASSUME A LOOP
public:
	int step;	//to count how many objects passed
public:
virtual	Tobject * get_object_thru_key ( int key_scan_num , BOOL useDisabled=FALSE );
virtual	Tobject * get_next_object (enum direction direct , BOOL useDisabled=FALSE );
virtual	Tobject * get_object_thru_point ( int x,int y , BOOL useDisabled=FALSE );
virtual void  change_select ( Tobject *pold_select,Tobject *pnew_select );

};


#endif














