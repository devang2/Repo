// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* group.cpp
*/

#include "group.h"

/*************************************************************************/
group_class::group_class(int ID,char *title_hotkey,
		int left,int top,int width,int height )
	:	control_class(ID,title_hotkey,NORMAL ,GROUP,
			left,top,width,height,0,0,0)
{
	control_num =0;
	pfirst_control =NULL;
	pcontrol_current_selected=NULL;
}

/*************************************************************************/
group_class::~group_class()
{
	dispose_all_controls(pfirst_control);
}

/*************************************************************************/
void group_class::select ( )
{
//	control_class::select();
	if (( pcontrol_current_selected !=NULL ) &&
		(!( pcontrol_current_selected->status & INVISIBLE)) )
		pcontrol_current_selected ->select();
}
/**************************************************************************/
void group_class::unselect ( )
{
//	control_class::select();
	if (( pcontrol_current_selected !=NULL ) &&
		(!( pcontrol_current_selected->status & INVISIBLE))  )
		pcontrol_current_selected ->unselect();
}


void group_class::update_control ()
{ Tcontrol *ptemp_control;
  int i;
	if ( status & INVISIBLE)  return;

//	control_class::update_control();	recursive

	if (pfirst_control !=NULL ){
		ptemp_control =pfirst_control;
		for (i=0;((i<control_num) && (ptemp_control !=NULL));i++){
			ptemp_control ->update_control ( );
			ptemp_control =(Tcontrol*)ptemp_control->pright;
		}
	}
	select();
}


/*************************************************************************/
void group_class::draw ()
{ Tcontrol *ptemp_control;
  int i;
	if ( status & INVISIBLE)  return;

	control_class::draw ();

	if (pfirst_control !=NULL ){
		ptemp_control =pfirst_control;
		for (i=0;((i<control_num) && (ptemp_control !=NULL));i++){
			ptemp_control ->draw ( );
			ptemp_control ->control_change_value
				(ptemp_control->current_value );
			ptemp_control =(Tcontrol*)ptemp_control->pright;
		}
	}
	if (pcontrol_current_selected !=NULL )
		pcontrol_current_selected->select();
}

/**************************************************************************/
void group_class::insert_control (Tcontrol *pcontrol_handled)
{
	control_num ++;
	max_value ++;
	if ( pfirst_control ==NULL ) {
		pfirst_control=pcontrol_handled;
		return ;
	}
	pcontrol_handled->pright=pfirst_control->pright;
	pcontrol_handled->pleft =pfirst_control;
	pfirst_control->pright->pleft=pcontrol_handled;
	pfirst_control->pright=pcontrol_handled;

	if ( !(pcontrol_handled->status & DISABLE) &&
		 !(pcontrol_handled->status & INVISIBLE)  )
		pcontrol_current_selected =pcontrol_handled;
}

void group_class::setup_control ()
{ Tcontrol *ptemp_control;
  int i;

	if (pfirst_control !=NULL ){
		ptemp_control =pfirst_control;
		for (i=0;((i<control_num) && (ptemp_control !=NULL));i++){
			ptemp_control ->setup_control ( );
			ptemp_control =(Tcontrol*)ptemp_control->pright;
		}
	}
}

/*************************************************************************/
void group_class::dispose_all_controls (Tcontrol  *pcontrol_handled )
{   Tcontrol *ptemp, *ptemp1;

	if (pcontrol_handled==NULL ) return;
	for (ptemp =pcontrol_handled;1;	ptemp =ptemp1) {
		ptemp1 =(Tcontrol*)ptemp->pright ;
		if (ptemp  != NULL ) delete (ptemp);
		if (ptemp1 == pcontrol_handled ) break;
	}
	pfirst_control=NULL;
	pcontrol_current_selected=NULL;
}

/*************************************************************************/
int group_class::key_pressed_handler(int key_scan_num )
{
// for tab and alt && etc.
	if (pfirst_control ==NULL ) return FALSE;
	if ( (key_scan_num == TABKEY ) || (key_scan_num ==SHIFT_TABKEY) )
		return control_tab_key_handler ( key_scan_num );
  Tcontrol *ptmp, *ptmp1;
  int i;
//	ptmp=pright;
	ptmp=(Tcontrol*)pcontrol_current_selected->pright;
	ptmp1=pcontrol_current_selected;
	i=current_value;
	for(;ptmp !=ptmp1;ptmp=(Tcontrol*)ptmp->pright, i=((i==max_value)?0 : (i+1))  ){
		if ( (alt_key[ toupper(ptmp->title[ptmp->hotkey])-'A'] ==key_scan_num)
			&& (!(ptmp->status & DISABLE )) )  {
			if ((pcontrol_current_selected !=NULL) &&
				(!( pcontrol_current_selected->status & INVISIBLE)) )
				pcontrol_current_selected ->unselect();
			if (!( ptmp->status & INVISIBLE))
				ptmp->select();
			this->control_change_value (i);
			pcontrol_current_selected =ptmp;
			return TRUE;
		}
	}
// normal key
	if (pcontrol_current_selected !=NULL )
		if (pcontrol_current_selected->key_pressed_handler(key_scan_num) ==TRUE )
			return TRUE;
	switch (key_scan_num) {
		case SHIFT_LEFT:
		case SHIFT_UP:
		case LEFTKEY:
		case UPKEY:
		case CTRL_F7:
			return control_tab_key_handler ( SHIFT_TABKEY );
		case SHIFT_RIGHT:
		case SHIFT_DOWN:
		case RIGHTKEY:
		case DOWNKEY:
		case CTRL_F8:
			return control_tab_key_handler ( TABKEY );
		default:	return FALSE;
	}
}
/*************************************************************************/
int group_class::control_tab_key_handler 	(int key_scan_num )
{
  Tcontrol   *ptemp1;
  int i;

	if (pfirst_control ==NULL ) return FALSE;
	if (pcontrol_current_selected ==NULL) {
		pcontrol_current_selected =(Tcontrol*)pfirst_control->pleft;
	}
	if ((pcontrol_current_selected !=NULL) &&
			(!( pcontrol_current_selected->status & INVISIBLE)) )
			pcontrol_current_selected ->unselect();
	i=((key_scan_num==TABKEY)?current_value+1:current_value-1);
	if (i<0)	 i=max_value-1;
	if (i>max_value-1) i=0;

	for(ptemp1 =(Tcontrol*) (	(key_scan_num==TABKEY) ? pcontrol_current_selected-> pright
									  : pcontrol_current_selected-> pleft
				 );
		((ptemp1 !=this) && (ptemp1->status & DISABLE ));
		ptemp1 =(Tcontrol*) (	(key_scan_num==TABKEY) ? ptemp1 ->pright
									  : ptemp1 ->pleft
				 )
	   )  {
			i=((key_scan_num ==TABKEY)?(i+1):(i-1));
			if (i<0)	 i=max_value-1;
			if (i>max_value-1) i=0;
	   }
	if ( (ptemp1 !=this ) && (!( ptemp1->status & INVISIBLE)) )
		ptemp1->select ( );
	this->control_change_value (i);
	pcontrol_current_selected =ptemp1;
	return TRUE;
}
/*************************************************************************/
int group_class::msg_handler (MSG& message )
{
  int ret_val;
//	if ( (ret_val= control_class::msg_handler(message))!=FALSE)
//			return ret_val;
	switch (message.Action){
		case MouseLButtonDownMSG:

  Tcontrol *ptmp,*ptmp1;
			if (pfirst_control==NULL) return FALSE;
			if (pcontrol_current_selected==NULL) ptmp1=pfirst_control;
			else ptmp1=pcontrol_current_selected;
  int i;	i=current_value;
			if ( (ptmp=(Tcontrol*)ptmp1->get_object_thru_point (win_mouse_x,win_mouse_y))==NULL) break;
			if (!PtInRect (win_mouse_x,win_mouse_y, ptmp->bounds)) return FALSE;
			i=i+ptmp1->step;

			if (ptmp==pcontrol_current_selected) break; //to current control

			if ((pcontrol_current_selected !=NULL) &&
				(!( pcontrol_current_selected->status & INVISIBLE)) )
				pcontrol_current_selected ->unselect();
			if (!( ptmp->status & INVISIBLE))
				ptmp->select();
			if (i>=max_value) i =i-max_value;
			this->control_change_value (i);	//Note !!!! seem i;
			pcontrol_current_selected =ptmp;

			return TRUE;
//		default:return  control_class::msg_handler(message);
	}
	if (pcontrol_current_selected !=NULL )
			return pcontrol_current_selected->msg_handler(message);
	else return  control_class::msg_handler(message);
// I don't know whether this is valid
}
/*************************************************************************/
int group_class::control_change_value	(int new_value )
{   if (control_class::control_change_value(new_value)==TRUE)
		return TRUE;
	current_value =new_value;
	thequeue.SendMessage(ID,GroupValueChangedMSG,this);
	return TRUE;
}

