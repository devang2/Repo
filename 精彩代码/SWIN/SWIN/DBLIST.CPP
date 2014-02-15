/*----------------------------------------------------
						DBLIST.CPP
		This file clarm a cycle double directional
	link list class.
			Write by TanWenHong 1996.12.27 Cheng Du
----------------------------------------------------*/
#ifndef	_DBLIST_HPP
	#include "dblist.hpp"
#endif

DbList :: ~DbList()
{
	if(!ListHead)
		return ;
	while(ListHead != ListTail){
		ListHead = ListHead->next ;
		delete ListHead->prev ;
	}
	delete ListTail ;
}

void DbList :: AtachToHead(Box * newElement)
{
	Node * node = new Node;
	node->element = newElement ;
	if(!ListHead){
		ListHead = node ;
		ListTail = node ;
	}
	else {
		node->next = ListHead ;
		ListHead->prev = node ;
		ListHead = node ;
	}
	CurrNode = node ;
	ListHead->prev = ListTail ;
	ListTail->next = ListHead ;
	ListHead->element->objno = ListHead->next->element->objno+1 ;
}


void DbList :: AtachToTail(Box * newElement)
{
	Node * node = new Node ;
	node->element = newElement ;
	if(!ListHead){
		ListHead = node ;
		ListTail = node ;
		CurrNode = node ;
	}
	else {
		node->prev = ListTail ;
		ListTail->next = node ;
		ListTail = node ;
	}
	ListHead->prev = ListTail ;
	ListTail->next = ListHead ;
	ListTail->element->objno = ListTail->prev->element->objno+1 ;
}

void DbList::DetachFromHead()
{
	if(!ListHead)	return ;
	if(ListHead == ListTail){
		delete CurrNode ;
		ListHead = ListTail = CurrNode = 0;
	} else {
		if(CurrNode == ListHead)
			CurrNode = CurrNode->next ;
		Node * deleteNode = ListHead ;
		ListHead = ListHead->next ;
		ListHead->prev = ListTail ;
		ListTail->next = ListHead ;
		delete deleteNode ;
	}
}

void DbList::DetachFromTail()
{
	if(!ListHead)	return ;
	if(ListHead == ListTail){
		delete CurrNode ;
		ListHead = ListTail = CurrNode = 0;
	} else {
		if(CurrNode == ListTail)
			CurrNode = CurrNode->prev ;
		Node * deleteNode = ListTail ;
		ListTail = ListTail->prev ;
		ListHead->prev = ListTail ;
		ListTail->next = ListHead ;
		delete deleteNode ;
	}
}

int DbList::Locate(Box * object)
{
	if(!ListHead)	return false;
	Node * skipNode = ListHead ;
	do {
		if(skipNode->element==object){
			CurrNode->element->Active(false);
			CurrNode = skipNode ;
			if(CurrNode->element->disable) DbList::SkipNext();
			else CurrNode->element->Active(true);
			return CurrNode->element->objno ;
		}
		skipNode = skipNode->next ;
	}while(skipNode != ListHead);
	return false;
}

Box * DbList::Locate(int objnum)
{
	if(!ListHead)	return false;
	Node * skipNode = ListHead ;
	do {
		if(skipNode->element->objno==objnum){
			CurrNode->element->Active(false);
			CurrNode = skipNode ;
			if(CurrNode->element->disable) DbList::SkipNext();
			else CurrNode->element->Active(true);
			return CurrNode->element ;
		}
		skipNode = skipNode->next ;
	}while(skipNode != ListHead);
	return false;
}

int DbList::SkipNext(){
	int curno = CurrNode->element->objno ;
	CurrNode->element->Active(false);
	do {
		CurrNode = CurrNode->next ;
		if(!CurrNode->element->disable){
			CurrNode->element->Active(true);
			break ;
		}
	}while(CurrNode->element->objno != curno);
	return CurrNode->element->objno ;
}

int DbList::SkipPrev(){
	int curno = CurrNode->element->objno ;
	CurrNode->element->Active(false);
	do {
		CurrNode = CurrNode->prev ;
		if(!CurrNode->element->disable){
			CurrNode->element->Active(true);
			break ;
		}
	}while(CurrNode->element->objno != curno);
	return CurrNode->element->objno ;
}

Box * DbList::ObjPtr(int objnum)
{
	if(!ListHead)	return 0;
	Node * skipNode = ListHead ;
	for(int i=1;i<=objnum;i++)
		skipNode=skipNode->next;
	return skipNode->element;
}

int DbList::ObjNum(Box * object)
{
	return object->objno ;
}

void DbList::ShowAllObj()
{
	if(!ListHead)	return ;
	Node * skipNode = ListHead ;
	do {
		skipNode->element->Show();
		skipNode = skipNode->next ;
	}while(skipNode != ListHead);
}

int DbList::CheckEvent()
{
	if(!ListHead)	return 0;
	int clicked = NOCLICK;
	Node * skipNode = ListHead ;
	if(event.type == KEYBD){
		do {
			clicked = skipNode->element->Clicked();
			if(clicked == CLICKED){
				CurrNode->element->Active(false);
				CurrNode = skipNode ;
				CurrNode->element->Active(true);
				break ;
			}
			skipNode = skipNode->next ;
		}while(skipNode != ListHead);
	} else {
		do{
			if(skipNode->element->MouseIn()){
				CurrNode->element->Active(false);
				CurrNode = skipNode ;
				CurrNode->element->Active(true);
				clicked = skipNode->element->Clicked();
				if(clicked != NOCLICK) break ;
			}
			skipNode = skipNode->next ;
		}while(skipNode != ListHead);
	}
	return(clicked == CLICKED?CurrNode->element->Run():NOCLICK);
}



/*
Bool DbList::Delete()
{
	if(!CurrNode)	return false;
	if(CurrNode->prev == CurrNode->next){
		delete CurrNode ;
		CurrNode = ListHead = ListTail = 0 ;
	}
	else if(ListHead == CurrNode) {
		ListHead = CurrNode->next ;
		ListHead->prev = ListTail ;
		ListTail->next = ListHead ;
		delete CurrNode ;
		CurrNode = ListHead ;
	}
	else if(ListTail == CurrNode ) {
		ListTail = CurrNode->prev ;
		ListTail->next = ListHead ;
		ListHead->prev = ListTail ;
		delete CurrNode ;
		CurrNode = ListTail ;
	}
	else {
		Node * node = CurrNode ;
		CurrNode->next->prev = CurrNode->prev ;
		CurrNode->prev->next = CurrNode->next ;
		CurrNode = CurrNode->next ;
		delete node ;
	}
	return true ;
}

*/