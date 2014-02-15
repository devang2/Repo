class Node
{
public :
	Box * item ;
	Node * next ;
public :
	~Node(){ delete item ;}
};

class List
{
	unsigned int totalNode ;
	unsigned int picStart ;
	unsigned int picEnd ;
	Node * ListHead,* CurrNode,*ListTail;
	friend Screen ;
public :
	List(){ ListHead = CurrNode = ListTail = 0 ;
				picStart = picEnd = totalNode = 0 ;}
	~List();
	void LocatePicStart();
};

void List::LocatePicStart()
{
	CurrNode = ListHead ;
	for(int i = 1 ;i < picStart ;i ++)
		CurrNode = CurrNode->next ;
}

List::~List()
{
	if(!ListHead) return ;
	while(ListHead){
		CurrNode = ListHead ;
		ListHead = ListHead->next ;
		delete CurrNode ;
	}
}

