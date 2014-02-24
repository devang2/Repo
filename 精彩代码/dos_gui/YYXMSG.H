#ifndef __YYXMSG_H
#define __YYXMSG_H

#include <stdlib.h>
#include "yyxsys.h"
#define MAXMSGNUM 100
#define MSGFULLNUM 20

#define TimerMSG		0x0001
#define SlowTimerMSG	0x0004
#define SysQuitMSG		0x0002
#define KeyPressedMSG	0x0003
#define CtrlBreakMSG 	0x0005
#define MouseLButtonDownMSG 0x0006


#define MenuActionMSG 	0x0101

#define WinOpenedMSG  	0x0201
#define WinSysCloseMSG 	0x0202
#define WinSysClosedMSG 	0x0204
#define WinMovedSizedMSG 0x0203
#define WinSelectedMSG   0x0205
#define WinMovingSizingMSG	0x0206
#define WinRedrawboundsMSG  0x0207

#define DlgOkedMSG		0x0301
#define DlgCanceledMSG	0x0302

#define ButtonPushedMSG 		0x0401
#define CheckBoxValueChangedMSG 0x0402
#define ListBoxValueChangedMSG  0x0403
#define ScrollValueChangedMSG	0x0404
#define ListBoxItemSelectedMSG	0x0405
#define EditInputedMSG 			0x0406
#define GroupValueChangedMSG	0x0407

typedef struct tagMSG {
	long int ID;
	long int Action;
	void *fptr;
} MSG;

class MSGQueue {
friend application_class;
public:
	MSG  msg_array[MAXMSGNUM+1];
private:
	int oldfirst;	// Last Inserted MSG
	int first;	//Not NULL    for in
	int last;	//NULL        for out
// functions
public:
	MSGQueue();
	~MSGQueue();
	void SendMessage(long int ID,long int Action,void *ptr);
	BOOL PeekMessage(MSG& message);
	BOOL GetMessage (MSG& message);
    BOOL QueueFull ();
	void Reset(void);
};

#ifdef __YYXMAIN
	MSG  sysmsg;
	class MSGQueue thequeue;
#else
	extern MSG	sysmsg;
	extern class MSGQueue thequeue;
#endif


#endif

