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


#include "applicat.h"

void error (char *error_message );

class theprogram_class:public application_class{
public:
  theprogram_class(int menu_num,char *title);
  virtual void get_my_menu();
  virtual BOOL echo_to_menu( long menu_ID);
  virtual int  second_class_msg_handler (MSG& message);
  virtual void anything_in_the_loop();

  virtual   void    init_ctrl();

	void newgraph();
	void close();
};

#ifdef __YYXMAIN
		class theprogram_class *theprogram;
		char  msgbuf[255];
		BOOL  nomouse=FALSE;
#else
extern      class   theprogram_class *theprogram;
extern      char  msgbuf[255];
extern      BOOL  nomouse;
#endif

