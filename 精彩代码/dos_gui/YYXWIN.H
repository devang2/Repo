#ifndef __YYXWIN_H
#define __YYXWIN_H

#include "yyxsys.h"
#include "yyxsys1.h"
#include "keydef.h"
#include "yyxmsg.h"

#include "yyxctrl.h"
#include "group.h"

#include "yyxobj.h"

#define VISIBLE 1
#define IN_VISIBLE 0
#define TEMP_IN_VISIBLE 3

#define NORMAL_WIN 0x00
#define DIALOG_WIN 0x01
#define UNRESERVE_WIN 0x02

#define SELECT_WIN 1
#define UNSELECT_WIN 2
#define VISIBLE_WIN 3
#define IN_VISIBLE_WIN 4
#define DRAW_WIN_FRAME 5
#define DRAW_WIN_CONTROL 6

#define SCREEN_BACK_COLOR WHITE


//#define FALSE 0
//#define TRUE  1
#define SIZING 2	//for in_win_moving

struct pwin_list {
	Twin				*pthe_win;
	struct pwin_list	*pnext_list;
};


class win_class :public Tobject {
public:
	BOOL canclose;     // used for close
	Rect newbounds;	//used in move & size
	Rect oldbounds; //used in Zoom in & out

	byte 		visible;  	/* VISIBLE & IN_VISIBLE  */
	Tgroup 	*win_group_control;

	struct viewporttype 	viewport;   /* inner rect's points */
protected:
	void far 	*punder_bitmap;		/* used in open_win    */
	void far 	*pself_bitmap;         /* used in visible_win */
	Twin   		*pnext_win;	        /* used as a link of all windows  */
protected:
	Rect title_box;
	Rect close_box;
	Rect zoom_out_box;
	Rect zoom_in_box;
	Rect size_box;
public:
	struct pwin_list  		*pfirst_above;
	struct pwin_list  		*pfirst_under;
	void far 				*pother_info;		/* used for later */

public:static Twin *pnext; //used as CTRL_F6's link
/*-----------------------------functions------------------------------*/
/*------------------------------------------------------------------------*/
/*------------------------------------------------------------------------*/

public:	// comes from yyxobject
virtual void  select();
virtual	void  draw();

virtual int   key_pressed_handler  ( int key_scan_num );
virtual int   msg_handler (MSG& message );
// Note: No unselect--->since unselect_current_selected_win

public:
int   unselect_current_selected_win();
 win_class
	(int ID,char *title,byte type,byte hotkey,
	 int left,int top,int width,int height
	);
virtual ~win_class ();
virtual BOOL func_canclose();		// || canclose two channel

virtual void  setup_window ();

 void   draw_win_frame ();
 void   draw_win_control ();
virtual int   draw_win_contents ();

 int   invisible_one_win ();
 int   visible_one_win  ();
 int   open_one_win  ();
 int   close_one_win  ();
 void  insert_control (Tcontrol *pcontrol_handled);

 int move_size_window (int key_scan_num );
 void draw_size_rect();
protected:

 void  dispose_all_controls (Tcontrol  *pcontrol_handled );

 int   make_it_and_above_invisible();
 int   make_it_and_above_visible ();
 int   build_all_links ();
 int   kill_all_links ();

 int  restore_self_bitmap ();
 int  restore_under_bitmap ();
 int  reserve_self_bitmap ();
 int  reserve_under_bitmap();


BOOL win_class::above_has_this (Twin *tmp);
int win_class::restore();
BOOL win_class::under_all_redrawed(Twin *pwin);
int win_class::mark_need_redraw();
BOOL win_class::mark_above_win (Twin *pwin);              //no recursion

};

/* GLOBAL VARIABLES FOR WINDOWS */
#ifdef __YYXMAIN

int 	WIN_TITLEBAR_SELECTED_COLOR		;
int 	WIN_TITLEBAR_UNSELECTED_COLOR	;

int 	WIN_TITLE_SELECTED_COLOR   		;
int 	WIN_TITLE_UNSELECTED_COLOR 		;

int 	WIN_BACKGROUND_COLOR    		;

//	WIN_FRAME_COLOR   					;
//	WIN_HOTKEY_COLOR  					;

 Twin  *pfirst_win_of_all 		=NULL; /* used in search of all window */
 Twin  *pcurrent_selected_win	=NULL;

 BOOL  in_win_moving =FALSE;
#else

	extern int WIN_TITLEBAR_SELECTED_COLOR		;
	extern int WIN_TITLEBAR_UNSELECTED_COLOR	;

	extern int WIN_TITLE_SELECTED_COLOR   	;
	extern int WIN_TITLE_UNSELECTED_COLOR 	;

	extern int WIN_BACKGROUND_COLOR     ;

//	extern int WIN_FRAME_COLOR   		;
//	extern int WIN_HOTKEY_COLOR  		;

extern Twin  *pfirst_win_of_all 		; /* used in search of all window */
extern Twin  *pcurrent_selected_win		;

extern BOOL  in_win_moving ;

#endif


/*------------------------------------------------------------------------*/

#endif