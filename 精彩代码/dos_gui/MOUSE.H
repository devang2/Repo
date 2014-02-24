#ifndef __MOUSE_H
#define __MOUSE_H

#include "yyxsys.h"

#define 	M_RESET 		0
#define   	M_SHOW_CURS		1
#define   	M_HIDE_CURS		2
#define   	M_GET_STATUS	3
#define 	M_SET_CURS		4
#define 	M_GET_PRESS		5
#define		M_GET_REL		6
#define		M_SET_X_BOUNDS	7
#define		M_SET_Y_BOUNDS	8
#define		M_SET_G_CURS	9
#define 	M_SET_T_CURS	10

#define 	M_GET_MOVE_COUNTER 11
#define 	M_SET_EVENT_HNDL 12

#define 	LEFT_BUTTON		0
#define 	RIGHT_BUTTON	1

#define		MOUSE_UP	0
#define		MOUSE_DOWN	1

typedef class mouse_class Tmouse;

class mouse_class{
public:
	int left_button_status;
	int	right_button_status;
private:
	int m1,m2,m3,m4;
	BOOL prev_cursor_state;
public:
	mouse_class();
	~mouse_class();

	void mouse(int *m1,int *m2,int *m3,int *m4);
	BOOL reset();
	void on(int restoreflag); //0 ;regardless of state; //1 conditional
	void off(int tempflag);   //0 ;regardless of state; //1 conditional
	void get_posn();
	void get_posn_xy(int &x,int &y);
	void set_posn(int x=0,int y=0);
	int  get_press_count(int mouse_button);
	int  get_release_count(int mouse_button);
protected:
static	int  mouse_event_handler(...);
};

#ifdef __YYXMAIN
Tmouse *sysmouse=NULL;
int 	mouse_x;
int 	mouse_y;
int 	old_mouse_x;
int 	old_mouse_y;

int 	win_mouse_x;
int     win_mouse_y;
#else
extern Tmouse *sysmouse;
extern int 	mouse_x;
extern int 	mouse_y;
extern int 	old_mouse_x;
extern int 	old_mouse_y;

extern int 	win_mouse_x; //used for group_window
extern int  win_mouse_y;
#endif



#endif
