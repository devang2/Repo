#include "yyxobj.h"

#if !defined(__YYXSYS1_H)
#include "yyxsys1.h"
#endif

void object_class::select(){}
void object_class::unselect(){}
void object_class::draw(){}
int  object_class::key_pressed_handler( int key_scan_num ){return FALSE;}
int  object_class::msg_handler(MSG& message){return FALSE;}

object_class::object_class(int ID,char *title_hotkey,byte status,byte type,
	 int left,int top,int width,int height	)
{
  byte hotkey=0xff;
	this->ID =ID;
	strcpy (title,title_hotkey);

	get_title_hotkey (title,hotkey);
	this->hotkey =hotkey;
	this->status =status;
	this->type =type ;
	SetRect (&bounds,left,top,left+width,top+height);

	pleft  =this;	//Don't Link into; Who do you know where to link???!!!
	pright =this;
}

object_class::~object_class(){};


Tobject * object_class::get_object_thru_key ( int key_scan_num, BOOL useDisabled )
{
// Now it turns into Enabled_item // from this,to this's next
  Tobject * ptmp;

	step =0;ptmp =this;
	if (ptmp->pright !=this)
	do {
		step ++;
		ptmp = ptmp->pright;
	}
	while ((!useDisabled)&&(ptmp !=this)&&((ptmp->status & DISABLE)||(ptmp->status&INVISIBLE)||
		  (alt_key[ toupper(ptmp->title[ptmp->hotkey])-'A'] !=key_scan_num )) );
	if (ptmp ==this) step =0;
	return ptmp;
}

Tobject * object_class::get_next_object ( enum direction direct, BOOL useDisabled )
{
// Now it turns into Enabled_item // from this,to this's next
  Tobject * ptmp;

	step =0;ptmp =this;
	if (ptmp->pright !=this)
	do {
		step += ( (direct == TO_DOWN ) ? +1 : -1 );
		ptmp = (direct == TO_DOWN ) ?  ptmp->pright:ptmp->pleft;
	}
	while ((!useDisabled)&&(ptmp!=this)&& ((ptmp->status & DISABLE)||(ptmp->status&INVISIBLE)));
	if (ptmp ==this) step =0;
	return ptmp;
}

Tobject * object_class::get_object_thru_point (int x,int y , BOOL useDisabled )
{
  Tobject * ptmp;

	step =0;ptmp =this;
	if (ptmp->pright !=this)
	do {
		step ++;
		ptmp = ptmp->pright;
	}
//	while ((!useDisabled)&&(ptmp !=this)&&((ptmp->status & DISABLE)||(ptmp->status&INVISIBLE)||
//		  (! PtInRect(x,y,ptmp->bounds )))  );
	while ((  (! PtInRect(x,y,ptmp->bounds ))|| ( ((ptmp->status & DISABLE)||(ptmp->status&INVISIBLE)) && (!useDisabled) )  )
			&& (ptmp !=this)
		  );
	if (ptmp ==this) step =0;
	return ptmp;
}

void object_class::change_select ( Tobject *pold_select,Tobject *pnew_select )
{ if (pold_select != NULL )
	pold_select->unselect();
  if (pnew_select != NULL )
	pnew_select->select();
}


