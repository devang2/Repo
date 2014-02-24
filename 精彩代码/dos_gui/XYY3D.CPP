/*        
		About       DOS_WIN

This program is to generate a Windows - Like Graphical User Interface(GUI)
in DOS. It is mainly for users to develop their own programs in DOS. 
In DOS, the porogram has offered a lot of Windows-like facilities such as 
Multi-Windows, DIalog, Button, CheckBox, Radios, ListBox, ScrollBar, and 
PullDown Menus ( With no-limit SubMenus!) And a lot of Other Choices. 
It also offer Mouse Functions for Menu selection and Window operation.

With all SOURCE CODEs Included, You can  easily modify it for your own 
purpose especially when you want to write a Graphics Program while you 
donot like to use the Windows Programming !! Since the program is designed
as a C++ downword system, some parts like GUI are completely indepentent. 
You can easily modify the main program and insert your own codes to it.

The package include a Example of Drawing a 3D-Object and viewing it in any
way. To start it just type the exe file and the program will work. It will 
work on a PC with EGA/VGA Monitor. It is Compiled by Borland C++ 3.1

You can distribute the source code freely only together with this readme
file. Since this is a shareware, you are required to register in order 
to use it in your own program. It will cost $500. 

However, if you do not like to pay so much, you can also pay as much as you 
wish to register. Just think the time and energe you will save by this 
program !

After you register for this program, you will get permission to 
include the source code to your own program. Besides,you will also 
get more detailed comments of the source code and you can get answers 
from author about any programming problems without extra fee.

Money and Check should mail to: 
<Current Address: Valid Since Sept. 1995>

Mr. Yongyong Xu,
1940 Howard Street. Apt. 333,
Kalamazoo, MI 49008
U.S.A.

Telephone: (616) 387-7569
EMail:  yxu@cs.wmich.edu   (prefered)
			or 
		99xu1@grog.lab.cc.wmich.edu

When you cannot reach me by the above address, try the next:
-----------------------------------------------------------------------
<Permanet Home Address>

Mr. Xu Yongyong,
Building 34 Room 604,
HuayuanXinCun , Chang Zhou
Jiang Su Province 213016
P.R.China

Tel:(86)-(519)-(328-0177)

*/


// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR: XuYongYong

/*  yyxmain.cpp
*/


#define __YYXMAIN

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "mouse.h"
#include "yyxmain.h"
#include "dialog.h"
#include "filedial.h"
#include "msgbox.h"

#include "3dmenu.h"
#include "3dgbas.h"


void error (char *error_message )
{
	if (getvect(TIMER_VECTOR)==application_class::timer )
	setvect (TIMER_VECTOR,theprogram->old_timer);
	if (getvect(KEY_VECTOR)==theprogram->application_class::key )
	setvect (KEY_VECTOR,theprogram->old_key);
	if (getvect(MOUSE_VECTOR)==theprogram->application_class::mouse)
	setvect (MOUSE_VECTOR,theprogram->old_mouse);
	if (getvect(DOS_ERROR_VECTOR)==application_class::dos_error)
	setvect (DOS_ERROR_VECTOR,theprogram->old_dos_error);
	delete theprogram;

	puts(error_message);
	putch ('\007');
	exit(1);
}

#include <signal.h>

void Catcher(int *reglist);

void Catcher(int *reglist)
{
	theprogram->exec_dialog(new Tmsgbox("Message","Floating point error",MB_OK));
	error("");
//  puts("Caught it!");
// *(reglist + 8) = 3; /* make return AX = 3 */
}

int matherr (struct exception *a)
{
  if (a->type == DOMAIN) {
	  puts (a->name);
	  putch(0x07);
	  a->retval =1;
	return 1;
	}
  return 0;
}

void myexit()  // Normal Exited message after main
{
	if (bioskey(1) == 0)
//  if  (bioskey(0) == SPACEKEY)
	{
		closegraph();
	}
	puts ("OK, Exited");
}

#pragma exit myexit

///theprogram_class:////theprogram_class:////theprogram_class:////theprogram_class://

theprogram_class::theprogram_class(int menu_num,char *title)
	:application_class(menu_num,title){};



void theprogram_class::get_my_menu()
{
	application_class::get_my_menu();
	my_menu[SPECIAL_MENU] =new Tmenu (SPECIAL_MENU," ",
		"Special Function",my_menu[MAIN_MENU] );

	my_menu[GRAPH] =new Tmenu (GRAPH,"&Graph",
		"",my_menu[MAIN_MENU] );
	my_menu[ROTATE] =new Tmenu (ROTATE,"&Rotate",
		"    ",my_menu[MAIN_MENU] );
	my_menu[HELP] =new Tmenu (HELP,"&Helps",
		"Helps ",my_menu[MAIN_MENU] );

	my_menu[ABOUT_MENU_ITEM] =new Tmenu (ABOUT_MENU_ITEM,"About... ",
		"About the Program",my_menu[HELP] );

// the order is important
//  win_sysmenu =new Tsysmenu (100,"","",my_menu[0]);

	my_menu[NEW] =new Tmenu (NEW,"&New ",
		"new graph ",   my_menu[GRAPH] );
	my_menu[CLOSE] =new Tmenu (CLOSE,"&Close   ",
		"close current graph",my_menu[GRAPH] );
	my_menu[MYDOSSHELL] =new Tmenu (DOSSHELL,"&Dosshell",
		"return to dos",my_menu[GRAPH] );
	my_menu[QUIT] =new Tmenu (QUIT,"&Quit         ALT_X",
		"Quit the system",my_menu[GRAPH] );

	my_menu[LEFT] =new Tmenu (LEFT,"&Left",
		"Left", my_menu[ROTATE] );
	my_menu[RIGHT] =new Tmenu (RIGHT,"&Right",
		"Right",    my_menu[ROTATE] );
	my_menu[UP] =new Tmenu (UP,"&Up",
		"Up",   my_menu[ROTATE] );
	my_menu[DOWN] =new Tmenu (DOWN,"&Down",
		"Down", my_menu[ROTATE] );
	my_menu[NEAR] =new Tmenu (NEAR,"&Near  ...F2KEY",
		"Near  ....F2KEY",  my_menu[ROTATE] );
	my_menu[FAR ] =new Tmenu (FAR ,"&Far   ...F1KEY",
		"Far   ....F1KEY",  my_menu[ROTATE] );

//  my_menu[DISTANCE ] =new Tmenu (DISTANCE,"D&istance",
//      "Distance   ....F3KEY", my_menu[ROTATE] );
	my_menu[VIEWPOINT ] =new Tmenu (DISTANCE,"V&iewpoint",
		"VIEWPOINT  ....F4KEY", my_menu[ROTATE] );

	my_menu[X_VALUE ] =new Tmenu (X_VALUE ,"X&value   ",
		"X_Value   ....",   my_menu[VIEWPOINT] );
	my_menu[Y_VALUE ] =new Tmenu (Y_VALUE ,"Y&value   ",
		"Y_Value   ....",   my_menu[VIEWPOINT] );
	my_menu[Z_VALUE ] =new Tmenu (Z_VALUE ,"Zvalue   ",
		"Z_Value   ....",   my_menu[VIEWPOINT] );

	start_menu->psub_menu_selected =my_menu[MAIN_MENU];
	start_menu->pson_menu    =my_menu[MAIN_MENU];

	Imme_Menu_ID[0]= QUIT;   Immediate_key[0]=ALT_X;  Imme_Menu_fptr[0]=my_menu[QUIT];
	Max_Imme_key_nums=1; //
}

////////////////////////////////////////////////////////////////////////
#include <stdio.h>
void theprogram_class::newgraph ()
{
  FILE *in;
  char fileopen[100]="NONAME00.dat";
  int izero;    // 0xff ===> Nrmgraph ; 0xfe ===>  Modgraph

	strcpy (msgbuf,"OK,You Opened a New Graph");
	exec_dialog(new Tmsgbox("Message",msgbuf,MB_OK));

  Tgraphbas *agraph;

		if (pcurrent_selected_win ==NULL ) {
			agraph=new Tgraphbas(1,fileopen,UNRESERVE_WIN,ALT_1,5,5,540,340  );
			agraph->open_one_win ();
		}  else if ((pcurrent_selected_win->bounds.right+25<600) &&
				   (pcurrent_selected_win->bounds.bottom+25<400)){
			agraph=new Tgraphbas(1,fileopen,UNRESERVE_WIN,ALT_1,
			pcurrent_selected_win->bounds.left+25,
			pcurrent_selected_win->bounds.top +25,
			540,340   );
			agraph->open_one_win ();
		}  else {
			agraph=new Tgraphbas(1,fileopen,UNRESERVE_WIN,ALT_1,5,5,550,340   );
			agraph->open_one_win ();
		}

}


void theprogram_class::close()
{
	if ( pcurrent_selected_win  != NULL )
			thequeue.SendMessage(pcurrent_selected_win->ID,
			WinSysCloseMSG,pcurrent_selected_win);
}


////////////////////////////////////////////////////////////////////////
BOOL theprogram_class::echo_to_menu(long menu_ID)
{
	if (application_class::echo_to_menu(menu_ID)) return TRUE;
	switch (menu_ID ){
		case MAIN_MENU      : ; break;
		case SPECIAL_MENU   : ; break;
		case ABOUT_MENU_ITEM    :
			strcpy (msgbuf,"NEW VERSION OF XYY 3D SYSTEM");
			exec_dialog(new Tmsgbox("HELP",msgbuf,MB_OK));
			break;

		case NEW:   newgraph(); break;
		case CLOSE: close(); break;

		case QUIT: thequeue.SendMessage(0,SysQuitMSG,NULL );break;
		default: return FALSE;
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
int  theprogram_class::second_class_msg_handler (MSG& message)
{ int ret_value;

	ret_value =application_class::second_class_msg_handler (message);
	return ret_value;
}

void theprogram_class::anything_in_the_loop()
{
}

void theprogram_class::init_ctrl()
{
}

//MAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAINMAIN /
//extern unsigned _stklen = 543210U;
//extern unsigned _stklen = 64000U;
extern unsigned _stklen = 54000U;

	typedef void (*fptr)(int);

int main(int argc,char *argv[])
{ int status;

	signal(SIGFPE, (fptr)Catcher);  /* cast Catcher to appropriate type */
	if (strcmpi (argv[1],"/NOMOUSE")==0 ) nomouse =TRUE;
	if (strcmpi (argv[1],"/MOUSECOM1")==0 ) MOUSE_VECTOR=0x0c;
	if (strcmpi (argv[1],"/MOUSECOM2")==0 ) MOUSE_VECTOR=0x0b;
	if (    (strcmpi (argv[1],"/?")==0 ) ||
		(strcmpi (argv[1],"/H")==0 ) ||
		(strcmpi (argv[1],"/HELP")==0 )
	   ) {
		printf ("\n Usage: %s [/NOMOUSE /MOUSECOM1 /MOUSECOM2]", argv[0]);
		puts ("\n Programed By Xu Yongyong 1994");
		return 0;
	}

	theprogram =new theprogram_class
		(99,"                    XYY 3-D SYSTEM             1994");

	theprogram->run();
	status =theprogram->status;

	delete ( theprogram );
	return status;
}

