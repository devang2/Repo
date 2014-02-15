#include "pullmenu.hpp"

PullMenu::PullMenu(int x,int y,int w,int h,MenuItem * bar[],int items)
	:Windows(x,y,w,h,"",1)
{
	areadypull = 0 ;
	for(int i = 0 ; i < items ; i ++){
		Windows::AddButton(bar[i]);
	}
}

int PullMenu::Clicked()
{
	Node * node = buttList.ListHead ;
	do{
		if(node->element->Clicked()!=NOCLICK)
			return CLICKED ;
		node = node->next ;
	}while(node != buttList.ListHead);
	if(event.type == KEYBD)
		return((event.alt)?CLICKED:NOCLICK);
	return NOCLICK;
}

int PullMenu::Run()
{
	if(buttList.IsEmpty()) return 0 ;
	PullMenu::Show();
	int exit = 0 ;
	areadypull = 0 ;
	if(event.type== KEYBD)buttList.Locate(_objnum);
	while(!exit)
	{
		if(event.type == KEYBD){
			switch(event.key)
			{
			case 0x5000 :
			case 0x4800 :areadypull = 1 ; break;
			case 0x4b00 :buttList.SkipPrev();break;
			case 0x4d00 :buttList.SkipNext();break;
			case 0x011b :exit = 1 ; exitcode = CANCEL ; continue;
			default :
				Node * node = buttList.ListHead ;
				do{
					if(node->element->Clicked() == RUNIT){
						buttList.CurrNode->element->Active(false);
						buttList.CurrNode = node ;
						buttList.CurrNode->element->Active(true);
						areadypull = 1 ;
					}
					node = node->next ;
				}while(node != buttList.ListHead);
			}
		} else {
			if(!MouseIn()){
				exit = 1 ;
				exitcode = CANCEL ;
				continue ;
			}
			Node * node = buttList.ListHead ;
			do{
				if(node->element->MouseIn()){
					buttList.CurrNode->element->Active(false);
					buttList.CurrNode = node ;
					buttList.CurrNode->element->Active(true);
					areadypull = 1 ;
					while(event.Present()&&node->element->MouseIn());
				}
				node = node->next ;
			}while(node != buttList.ListHead);
		}
		if(areadypull){
			exitcode = buttList.CurrNode->element->Run();
			if(exitcode != CANCEL || event.key == 0x011b) exit = 1 ;
         else continue ;
		}
		if(!exit) while(!event.Present());
	}
	buttList.CurrNode->element->Active(false);
	return exitcode ;
}