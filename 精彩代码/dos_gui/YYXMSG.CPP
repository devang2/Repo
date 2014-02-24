// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	yyxmsg.cpp
*/
#include "yyxmsg.h"
#include "yyxmain.h"

MSGQueue::MSGQueue()
{
	this->Reset();
}

void MSGQueue::SendMessage (long int ID,long int Action,void *fptr)
{   asm cli;
	msg_array[first].ID =ID;
	msg_array[first].Action =Action;
	msg_array[first].fptr =fptr;

    oldfirst =first;
	first -=1;
	if ( first <0 )  first=MAXMSGNUM;
	if ( last==first ) {
//		puts("\007\007\007MSGQueue Overflow");
//		exit (100);
		error("\007\007\007MSGQueue Overflow");
	}
	asm sti;
}

BOOL MSGQueue::GetMessage (MSG& message)
{
	if ( first ==last ) return FALSE;
	message=msg_array[last];
	last -=1;
	if ( last <0)  last =MAXMSGNUM;
	return TRUE;
}

BOOL MSGQueue::PeekMessage (MSG& message)
{
	if ( first ==last ) return FALSE;
	message=msg_array[last];
	return TRUE;
}

BOOL MSGQueue::QueueFull ()
{ register int i;
	i = last - first;
	if ( i<0 ) i +=MAXMSGNUM;
	if ( i >= MSGFULLNUM ) return TRUE;
	return FALSE;
}

void MSGQueue::Reset ()
{
	first=last;
}

MSGQueue::~MSGQueue()
{
}

