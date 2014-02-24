#ifndef __APPLICAT_H
#define __APPLICAT_H

#include "yyxsys.h"
#include "yyxmenu.h"
#include "yyxwin.h"
#include "sysmenu.h"
#include "dialog.h"
#include "dos.h"

#define  SYS_STATUS 1   //in show status 's augument as kind
#define  STATUS 2
#define  HELP_STATUS 3

#define SYS_RESTOREID 	1000
#define SYS_MOVEID      1001
#define SYS_SIZEID		1002
#define SYS_MINIMIZEID  1003
#define SYS_MAXIMIZEID  1004
#define SYS_CLOSEID		1005

#define WIN_SYS_RESTOREID 	2000
#define WIN_SYS_MOVEID      2001
#define WIN_SYS_SIZEID		2002
#define WIN_SYS_MINIMIZEID  2003
#define WIN_SYS_MAXIMIZEID  2004
#define WIN_SYS_CLOSEID		2005

#define DOSSHELL 3000
#define QUIT	 10

#define TIMER_VECTOR 0x08
#define KEY_VECTOR   0x09
////#define MOUSE_VECTOR 0x0b 	//COM2, setup by mouse driver after int33h,0
////#define MOUSE_VECTOR 0x0c 	//COM1, setup by mouse driver after int33h,0

#define DOS_ERROR_VECTOR 0x24

class application_class {
	friend void error (char *error_message );

public:
	int menu_num; //Added in 1993.10.6

	int status;
	Tmenu *start_menu;
//	int RE_START ;
	Tmenu **my_menu;
	char *title;

	Rect 	sys_status_rect;
	Rect	status_rect;
	long 	sys_mem;
	char	sys_status_string[40];

public:
	application_class(int menu_num,char *title);
  virtual	~application_class();
		  void run();
		void DosShell();
	int _screen2Disk ( const char * swpfile );
	int _disk2Screen ( const char * swpfile );

  virtual void get_my_menu();
  virtual BOOL echo_to_menu (long);
  virtual int  first_class_msg_handler (MSG& message);
  //MUST BE HANDLED FIRST
  virtual int  second_class_msg_handler (MSG& message);
  //MAY BE HANDLED AFTER OTHERS HAVE HANDLED
  virtual   void 	anything_in_the_loop();

  int 	exec_dialog (Tdialog *pdialog_handled );
  void  show_status(int kind);

public:
			void    init_screen();
	static		void 	interrupt (*old_timer)(...);
	static		void	interrupt 		timer(...);

protected:
			void 	init();
			void 	draw_application_win();
  virtual 	int  	key_pressed_handler (int key_scan_num);
  virtual 	int  	main_message_loop();
  virtual 	void 	init_msg();
  virtual	void	init_menu();
  virtual	void	init_win();
  virtual	void	init_ctrl();
//////////////interrupt interrupt interrupt interrupt///////////////////
	static	BOOL	timer_active;	//=FALSE;
	static	int    	time_counter;
	static	int		slow_time_counter;

	static		void	interrupt (*old_mouse)(...);
	static		void	interrupt 		mouse(...);
	static		BOOL	mouse_active; //=FALSE;

	static		void 	interrupt (*old_key  )(...);
	static		void	interrupt 		key  (...);
	static		BOOL	kb_active; //=FALSE;

	static		void 	interrupt (*old_dos_error  )(...);
	static		void	interrupt 		dos_error  (...);
	static		BOOL	dos_error_active; //=FALSE;


	static		int TIMER_COUNT;	  //=10;
	static		int SLOW_TIMER_COUNT;	//=20;

};

#define SYS_RESTOREID 	1000
#define SYS_MOVEID      1001
#define SYS_SIZEID		1002
#define SYS_MINIMIZEID  1003
#define SYS_MAXIMIZEID  1004
#define SYS_CLOSEID		1005

#define WIN_SYS_RESTOREID 	2000
#define WIN_SYS_MOVEID      2001
#define WIN_SYS_SIZEID		2002
#define WIN_SYS_MINIMIZEID  2003
#define WIN_SYS_MAXIMIZEID  2004
#define WIN_SYS_CLOSEID		2005

#ifdef __YYXMAIN
//		BOOL	timer_active=FALSE;
//		int    	time_counter;
//		int		slow_time_counter;
//		void 	interrupt (*old_timer)(...);
//		void	interrupt 		timer(...);

//		void	interrupt (*old_mouse)(...);
//		void	interrupt 		mouse(...);

//		void 	interrupt (*old_key  )(...);
//		void	interrupt 		key  (...);

//		int     key_code;
//		BOOL	kb_active =FALSE;

//		int TIMER_COUNT=10;
//		int SLOW_TIMER_COUNT=20;
		int     key_code;
		struct viewporttype current_viewport;
		char	status_string[40];
		char    main_help_text[40]="WELCOME ANY USERS";
		int 	diskerrorno=0;

		int  	timer_click=0;		 // used in timermsg of applicat
		int 	slow_timer_click =0; // used in timermsg of applicat

		int MOUSE_VECTOR	=0x0b;

#else
//extern 		BOOL	timer_active;
//extern		int    	time_counter;
//extern		int		slow_time_counter;
//extern		void 	interrupt (*old_timer)(...);
//extern		void	interrupt 		timer(...);

//extern		void	interrupt (*old_mouse)(...);
//extern		void	interrupt 		mouse(...);

//extern 		int     key_code;
//extern 		BOOL	kb_active;
//extern		void interrupt (*old_key  )(...);
//extern 		void interrupt 		key  (...);

//extern		int TIMER_COUNT;
//extern		int SLOW_TIMER_COUNT;
extern 		int     key_code;
extern 		struct viewporttype current_viewport;
extern		char	status_string[40];
extern 		char    main_help_text[40];
extern		int 	diskerrorno;

extern		int  	timer_click;		 // used in timermsg of applicat
extern		int 	slow_timer_click ; // used in timermsg of applicat

extern 		int MOUSE_VECTOR	;

#endif


#endif