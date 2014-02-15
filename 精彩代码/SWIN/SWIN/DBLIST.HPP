/*----------------------------------------------------
						DBLIST.HPP
		This file clarm a cycle double directional
	link list class.Its impelement in DBLIST.CPP.
		Write by TanWenHong 1996.12.27 Cheng Du
----------------------------------------------------*/
#ifndef _DBLIST_HPP
#define _DBLIST_HPP

#ifndef	__OBJECT_HPP
	#include "box.hpp"
#endif

class Node
{
public :
	Box * element;
	Node * prev;
	Node * next;
public :
	Node(){	element = 0 ; prev = 0 ; next = 0 ; }
	~Node(){	if(element) delete element; }
};

class DbList
{
public :
	Node * ListHead,* ListTail, * CurrNode;
public :
	DbList(){ ListHead = 0 ;
				 ListTail = 0 ;
				 CurrNode = 0 ;}
	~DbList();
	void AtachToHead(Box *);
	void AtachToTail(Box *);
	void DetachFromHead();
	void DetachFromTail();
	Box * Locate(int objnum);
	int Locate(Box * object);
	Bool Delete();
	Bool IsEmpty(){ return(ListHead?false:true); }
	Box * ObjPtr(int objnum) ;
	int ObjNum(Box * object) ;
	int SkipNext() ;
	int SkipPrev() ;
//-------------------------------------------------
	void ShowAllObj();
	int CheckEvent();
};

#endif