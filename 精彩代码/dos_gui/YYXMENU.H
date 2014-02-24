#ifndef __YYXMENU_H
#define __YYXMENU_H

#if !defined(__YYXSYS_H)
#include "yyxsys.h"
#endif
#if !defined(__YYXSYS1_H)
#include "yyxsys1.h"
#endif
#if !defined(__YYXMSG_H)
#include "yyxmsg.h"
#endif
#if !defined(__YYXOBJ_H)
#include "yyxobj.h"
#endif

//	#define ENABLED 1
//  #define DISABLED 0       /* used for menu_status */
//  #define INVISIBLE 2

//#define SELECT_MENU 1
//#define UNSELECT_MENU 2
//#define DRAW_MENU_TITLE 3
//#define ENTER_MENU 4
//#define DRAW_SUB_MENU_RECT 5
//#define ESC_MENU 6		  /* menu OP s  */

#define MAX_IMME_KEY_NUMBERS 10

// #define patCopy 1

class menu_class:public Tobject {
public:
	char 	*help;
	int 	Imme_key_num;
	byte    sub_menu_nums;
	struct		BitMap	save_bitmap;
	Tmenu *psub_menu_selected;
	Tmenu *pfather_menu;
	Tmenu *pson_menu;
/*____________________Functions _________________________ */
public:
virtual void	select 		()	;
virtual void 	unselect 	()	;
virtual void  	draw		() 	;
virtual int 	msg_handler (MSG& message);
virtual int  	key_pressed_handler( int key_scan_num );

public:
	menu_class (int ID,char *title_hotkey,char *help,Tmenu *pfather );
	menu_class (char *init_title,char *init_help);    //init
void createmenu_class
	(Tmenu *pfather_menu,int ID,char *title,char *help,byte hotkey,int Imme_key_num,
	 byte status,int savebit_left,int savebit_top,int savebit_right,int savebit_bottom
	 );
virtual	~menu_class( void );

void dispose_all_submenus();
//	int  menu_key_trap(int key_scan_num );

void 	offsetmenu( int dx,int dy );

void  	enter_menu (void)                               ;

protected:
// Tmenu * get_next_enabled_item ( int direction )   ;
int   	esc_menu (void)                                 ;

//int  	menu_operation(int menu_OP) ;
Tmenu   * get_next_sub_item (enum direction direct);
//	int  	menu_func (int menu_ID )   ;
// void    normal_key_handler  ( int key_scan_num );

void 	esc_all_menus(void);
int 	bar_Imme_menu_key_handler( int key_scan_num);
void  	draw_help_texts (char *help_texts);

void  	draw_sub_menu_rect ( void );

//Tmenu *padd_a_new_menu


int   Left_Right_key_handler(enum direction scroll_direction);

};
/*------------------------------------------------------ */
/*------------------------------------------------------ */

#ifdef __YYXMAIN

  int  MENU_ITEM_ENABLED_COLOR     ;
  int  MENU_ITEM_SELECTED_COLOR    ;
  int  MENU_ITEM_DISABLED_COLOR    ;
  int  MENU_ITEM_SELECT_COLOR      ;
  int  MENU_HOTKEY_COLOR           ;
  int  HELP_RECT_COLOR             ;
  int  HELP_TEXT_COLOR             ;
  int  MENU_RECT_COLOR             ;

/* global variables */
Tmenu *pcurrent_menu;
struct Rect on_line_help_rect;
struct viewporttype menu_viewport;

int    	Immediate_key	[MAX_IMME_KEY_NUMBERS];
int    	Imme_Menu_ID   	[MAX_IMME_KEY_NUMBERS];
Tmenu *	Imme_Menu_fptr  [MAX_IMME_KEY_NUMBERS];
int    	Max_Imme_key_nums;
int 	in_menu_trap;

MSG menu_save_sysmsg;
class MSGQueue menu_save_queue;
#else

extern int  MENU_ITEM_ENABLED_COLOR     ;
extern int  MENU_ITEM_SELECTED_COLOR    ;
extern int  MENU_ITEM_DISABLED_COLOR    ;
extern int  MENU_ITEM_SELECT_COLOR      ;
extern int  MENU_HOTKEY_COLOR           ;
extern int  HELP_RECT_COLOR             ;
extern int  HELP_TEXT_COLOR             ;
extern int  MENU_RECT_COLOR             ;
/*  global variables */
extern Tmenu *pcurrent_menu;
extern struct Rect on_line_help_rect;
extern struct viewporttype menu_viewport;

extern int	Immediate_key	[MAX_IMME_KEY_NUMBERS];
extern int	Imme_Menu_ID     [MAX_IMME_KEY_NUMBERS];
extern Tmenu *Imme_Menu_fptr [MAX_IMME_KEY_NUMBERS];
extern int	Max_Imme_key_nums;
extern int 	in_menu_trap;

extern MSG menu_save_sysmsg;
extern class MSGQueue menu_save_queue;
#endif

//	extern void menu_func (int menu_ID );

#endif
