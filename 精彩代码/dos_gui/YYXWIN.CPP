// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR: XuYongYong

/* 	yyxwin.cpp
*/
#include "yyxwin.h"
#include "sysmenu.h"
#include "applicat.h"

int win_class::make_it_and_above_invisible()
{   struct pwin_list *temp;

	if ( pfirst_above  != NULL )   /* ? */
	for (temp=pfirst_above ;
			temp != NULL ; temp = temp ->pnext_list)
		temp->pthe_win->make_it_and_above_invisible ();

	if (visible == IN_VISIBLE ) return 0;
	if (visible == TEMP_IN_VISIBLE ) return 0;

	reserve_self_bitmap (  );
	restore_under_bitmap (  );
	visible =TEMP_IN_VISIBLE ;
	return 1;
}

int win_class::make_it_and_above_visible ()
{   struct pwin_list *temp;

	if (visible ==VISIBLE ) return 0;
	if (visible ==IN_VISIBLE ) return 0;

	if (pfirst_under !=NULL )
		reserve_under_bitmap (  );
	restore_self_bitmap (  );
	visible = VISIBLE;

	if ( pfirst_above  != NULL )
	for (temp=pfirst_above ;
			temp != NULL ; temp = temp ->pnext_list)
		temp->pthe_win->make_it_and_above_visible ();
	return 1;
}


int win_class::invisible_one_win ()
{
//	if (pwin_handled==NULL ) return 0;
	if (visible ==IN_VISIBLE ) return 0;
	make_it_and_above_invisible (  );        //invisible
	if (pfirst_above != NULL )
		pfirst_above->pthe_win->make_it_and_above_visible ();
	visible =IN_VISIBLE ;
	return 1;
}

int win_class::visible_one_win  ()
{
//	if (pwin_handled==NULL ) return 0;
	if (visible ==VISIBLE ) return 0;
	make_it_and_above_invisible (  );
	visible=TEMP_IN_VISIBLE;
	make_it_and_above_visible (  );
	visible = VISIBLE ;
	return 1;
}


void win_class::select ()
{	Twin  *ptemp_win;

	if (this ==pcurrent_selected_win ) return ;
	unselect_current_selected_win ();

	if (visible == IN_VISIBLE ) return ;
	pcurrent_selected_win = this ;

	if (pfirst_above !=NULL ){
		make_it_and_above_invisible (  );

		if (pfirst_above != NULL )
			pfirst_above->pthe_win->make_it_and_above_visible ();

		if (pfirst_above !=NULL )
			reserve_under_bitmap (  );
		restore_self_bitmap ( );
/*	change_selected_win_links (  );		*/
		kill_all_links (  );
		build_all_links ();
	}
	visible = VISIBLE;

	draw_win_frame ( );

	thequeue.SendMessage(ID,WinSelectedMSG,this);
}

void win_class::draw()
{
	draw_win_frame ();
	draw_win_control ();
	draw_win_contents ();
}

int win_class::unselect_current_selected_win()
{	Twin   *ptemp_win;

	ptemp_win =pcurrent_selected_win;
	pcurrent_selected_win=NULL;
	if (ptemp_win !=NULL )
	 ptemp_win->draw_win_frame ();
	return 0;
}

int win_class::restore_self_bitmap ()
{
	if ( type & UNRESERVE_WIN  ) {
		draw();
		return 1;
	}

	if (pself_bitmap != NULL ){
		setport(win_living_viewport);
		putimage (bounds.left,bounds.top
			,pself_bitmap ,COPY_PUT);
		farfree (pself_bitmap );
		pself_bitmap=NULL;
	}
	return 0;
}

BOOL win_class::above_has_this (Twin *pwin)			 //no recursion
{
  struct pwin_list *temp_list ;
	if (pwin == NULL ) return FALSE;
	if ( pwin->pfirst_above==NULL ) return FALSE;
	for (temp_list =pwin->pfirst_above; temp_list !=NULL;temp_list=temp_list->pnext_list ) {
			if ( temp_list->pthe_win == this) return TRUE;
		}
	return FALSE;
}

BOOL win_class::mark_above_win (Twin *pwin)              //no recursion
{
  struct pwin_list *temp_list ;
  BOOL ret;
	if (pwin == NULL ) return FALSE;
	if ( pwin->pfirst_above==NULL ) return FALSE;
	ret =FALSE;
	for (temp_list =pwin->pfirst_above; temp_list !=NULL;temp_list=temp_list->pnext_list ) {
			if (!(temp_list->pthe_win->type & 0x40)) {
				temp_list->pthe_win->type |= 0x40;
				ret= TRUE;
		}
	}
	return ret;
}

BOOL win_class::under_all_redrawed (Twin *pwin)     // not include itself
{ Rect dstrect;							// recurse
  struct pwin_list *temp_list ;

	if (pwin==NULL ) return TRUE;
	if ( pwin->pfirst_under ==NULL  ) return	TRUE;
	for (temp_list =pwin->pfirst_under; temp_list !=NULL;temp_list=temp_list->pnext_list ) {
		   if (	(temp_list->pthe_win->type & 0x40) &&
				(!(temp_list->pthe_win->type & 0x80))
			  ) return FALSE;
		else if (! under_all_redrawed(temp_list->pthe_win) ) return FALSE;
	}
	return TRUE;
}


int win_class::mark_need_redraw()
{ int no_more;
  Twin *temp_win;
 do{
	no_more= TRUE;
	for (temp_win=pfirst_win_of_all; temp_win !=NULL; temp_win=temp_win->pnext_win){
		if ( temp_win == this ) continue;
		if ( temp_win->visible==IN_VISIBLE ) continue;
		if ( temp_win->visible==TEMP_IN_VISIBLE ) continue;
		if ( temp_win->type & 0x40 )   {
			no_more=! mark_above_win(temp_win);
			continue;
		}
		if (above_has_this (temp_win)) {
			temp_win->type |= 0x40;
			no_more=FALSE;
		}
	}
 }while (!no_more);
 return 0;
}

int win_class::restore ()
{
  Twin *temp_win;

	mark_need_redraw();
 int no_more;
 do{
	no_more= TRUE;
	for (temp_win=pfirst_win_of_all; temp_win !=NULL; temp_win=temp_win->pnext_win){
		if (temp_win == this ) continue;
		if (!( temp_win->type & 0x40 )) continue;
		if( (under_all_redrawed (temp_win)) && (!(temp_win->type & 0x80) )) {
			temp_win->draw ();

			temp_win->type |= 0x80;
			no_more=FALSE;
			continue;
		}
	}
 }while (!no_more);

	for (temp_win=pfirst_win_of_all; temp_win !=NULL; temp_win=temp_win->pnext_win)
		temp_win ->type &= 0x3f;
  return 0;
}


int win_class::restore_under_bitmap ()
{
	if (( type & UNRESERVE_WIN  ) && (pfirst_under != NULL )){

		setport (win_living_viewport );
		FillRect(bounds,WHITE);
		restore ();
		return 1;
	}

	setport(win_living_viewport);
	if (punder_bitmap != NULL ){
		putimage (bounds.left,bounds.top
			,punder_bitmap ,COPY_PUT);
		farfree (punder_bitmap );
		punder_bitmap=NULL;
	} else  FillRect (bounds, SCREEN_BACK_COLOR );
	return 0;
}


int  win_class::reserve_self_bitmap ()
{
	if ( type & UNRESERVE_WIN  ) return 1;

	if (pself_bitmap !=NULL )
		farfree(pself_bitmap);
	pself_bitmap = myfarmalloc
	( (unsigned long) RectSize (bounds) );
	setport (win_living_viewport);
	getimage ( bounds.left,bounds.top,
		bounds.right	,bounds.bottom ,
		pself_bitmap );
	return 0;
}

int  win_class::reserve_under_bitmap()
{
	if ( type & UNRESERVE_WIN  ) return 1;

	if (punder_bitmap !=NULL )
		farfree(punder_bitmap);
	punder_bitmap = myfarmalloc
	( (unsigned long) RectSize (bounds) );
	setport(win_living_viewport);
	getimage ( bounds.left,bounds.top,
		bounds.right	,bounds.bottom ,
		punder_bitmap );
	return 0;
}
	

int win_class::open_one_win  ()
{   Twin  *temp1;

	unselect_current_selected_win();
	if ( pfirst_win_of_all == NULL ) pfirst_win_of_all =this;
	else {
		for(temp1=pfirst_win_of_all;temp1->pnext_win!= NULL;temp1 =temp1 ->pnext_win);
		temp1->pnext_win=this ;
	}
	build_all_links (  );
	if (pfirst_under !=NULL )
		reserve_under_bitmap();

	pcurrent_selected_win =this ;
	draw();

	thequeue.SendMessage(pcurrent_selected_win->ID
			,WinOpenedMSG,pcurrent_selected_win );
	thequeue.SendMessage(ID,WinSelectedMSG,this);
	return 0;
}

/* note : after a close it's users duty to NULL the ptr   */
int win_class::close_one_win  ()
{	Twin  *temp_win, *front_temp_win;
	
//	if (pwin_handled==NULL ) return 0;
	if ( pfirst_above  == NULL ) {
		setport (win_living_viewport);
		restore_under_bitmap (  );
	} else 				// only for reserve space
	invisible_one_win ( );
	kill_all_links ( );

	for (temp_win=pfirst_win_of_all ; 
		(( temp_win != NULL ) && (temp_win !=this ));
		temp_win=temp_win ->pnext_win ) front_temp_win =temp_win;
	if (pfirst_win_of_all == this )
		pfirst_win_of_all =pnext_win;
	else if (temp_win == this )
		front_temp_win->pnext_win = pnext_win ;

    this->pnext_win =NULL;

	if (punder_bitmap != NULL )
		farfree (punder_bitmap);
	if (pself_bitmap != NULL )
		farfree (pself_bitmap);
//????????????????????????????????
//	farfree (pwin_handled );

/* now select the next window   */
	if (pfirst_win_of_all !=NULL ) {
		for (temp_win=pfirst_win_of_all ; 
		( temp_win->pnext_win!= NULL ) && (temp_win->pfirst_above !=NULL);
		temp_win=temp_win ->pnext_win ) ;
		if (temp_win->pfirst_above ==NULL ) {
			pcurrent_selected_win =temp_win;
			temp_win-> visible = VISIBLE;
				temp_win->draw_win_frame ();
		}
	} else pcurrent_selected_win =NULL;

	if ( pcurrent_selected_win !=NULL )
		thequeue.SendMessage(pcurrent_selected_win->ID,WinSelectedMSG,pcurrent_selected_win);
	return 0;
}

int win_class::build_all_links ()
{
	Twin  *temp_win;
	struct pwin_list *temp_list, *temp_under_list ;
	struct Rect dstrect ;
/* condition pwin_handled is front-most in the screen   */

		for (temp_win=pfirst_win_of_all ; temp_win != NULL;
		temp_win=temp_win ->pnext_win ){
			if (( SectRect ( bounds ,
			temp_win->bounds,&dstrect ) == TRUE_SECT) &&
			( temp_win !=this ) ) {
				if (temp_win->pfirst_above == NULL ) {
					temp_win->pfirst_above =(struct pwin_list *) myfarmalloc ( sizeof(struct pwin_list));
					temp_win->pfirst_above->pthe_win =this;
					temp_win->pfirst_above->pnext_list=NULL ;
				}	else {
					for ( temp_list= temp_win->pfirst_above;
						temp_list->pnext_list!= NULL;temp_list =temp_list->pnext_list);
					temp_list->pnext_list= (struct pwin_list *)myfarmalloc ( sizeof(struct pwin_list));
					temp_list->pnext_list->pthe_win =this;
					temp_list->pnext_list->pnext_list =NULL ;
				};
				/* above is to set up a win's above_link */
				if (pfirst_under==NULL ) {
					pfirst_under= (struct pwin_list *)myfarmalloc ( sizeof(struct pwin_list));
					pfirst_under->pthe_win=temp_win;
					pfirst_under->pnext_list=NULL;
					temp_under_list =pfirst_under;
				}	else {
					temp_under_list->pnext_list = (struct pwin_list *)myfarmalloc ( sizeof(struct pwin_list));
					temp_under_list->pnext_list->pthe_win=temp_win;
					temp_under_list->pnext_list->pnext_list=NULL;
					temp_under_list =temp_under_list->pnext_list;
				}
				/* above is set up its own under win_list */
			}
		}
	return 0;
}


int win_class::kill_all_links ()
{
	Twin  *temp_win;
	struct pwin_list *temp_list, *front_temp_list ;
	struct Rect dstrect ;

	for (temp_win=pfirst_win_of_all ; temp_win!=NULL;
	temp_win=temp_win ->pnext_win ){
		if ( (temp_win->pfirst_above != NULL) && (temp_win !=this)){
			for ( temp_list= temp_win->pfirst_above;
			 ((temp_list->pnext_list!= NULL)&& (temp_list->pthe_win!=this)) ;
				temp_list =temp_list->pnext_list) 
					front_temp_list =temp_list;
			if (temp_list->pthe_win==this){
				if (temp_list==temp_win->pfirst_above) temp_win->pfirst_above=temp_list->pnext_list;
				else front_temp_list->pnext_list =temp_list->pnext_list;
				farfree (temp_list );
			}
		};
		/* above is to kill a win's above_link */
		if ( (temp_win->pfirst_under != NULL) && (temp_win !=this)){
			for ( temp_list= temp_win->pfirst_under;
			 ((temp_list->pnext_list!= NULL)&& (temp_list->pthe_win!=this)) ;
				temp_list =temp_list->pnext_list)
					front_temp_list =temp_list;
			if (temp_list->pthe_win==this ) {
				if (temp_list==temp_win->pfirst_under) temp_win->pfirst_under=temp_list->pnext_list;
				else front_temp_list->pnext_list =temp_list->pnext_list;
				farfree (temp_list );
			}
		};
		/* above is to kill a win's under_link */
	}
	if (pfirst_above !=NULL)  {
			temp_list =pfirst_above;
			while ( temp_list != NULL ) {
				front_temp_list =temp_list->pnext_list;
				farfree ( temp_list );
				temp_list=front_temp_list;
			}
	}
	if (pfirst_under !=NULL)  {
			temp_list =pfirst_under;
			while ( temp_list != NULL ) {
				front_temp_list =temp_list->pnext_list;
				farfree ( temp_list );
				temp_list=front_temp_list;
			}
	}
	pfirst_above =NULL;
	pfirst_under =NULL;
	return 0;
}

void win_class::draw_win_frame ()
{	struct Rect title_rect;

//	if (pwin_handled->type == DIALOG_WIN ) return 0;
	setport(win_living_viewport);
	if (pcurrent_selected_win == this ) {
		draw_win_frame_rect_and_others( bounds,
		title,WIN_TITLEBAR_SELECTED_COLOR,
		WIN_TITLE_SELECTED_COLOR );
	} else {
		draw_win_frame_rect_and_others( bounds,
		title,WIN_TITLEBAR_UNSELECTED_COLOR,
		WIN_TITLE_UNSELECTED_COLOR );
	}
}

void win_class::draw_win_control ()
{	struct Rect bound_rect;
	Tcontrol *ptemp_control;
	int i;

	setport(viewport);

//	SetRect (&bound_rect,1,1,viewport.right-viewport.left-LINE_WIDTH-1,
//		viewport.bottom-viewport.top-bar_height-LINE_WIDTH-1);
	SetRect (&bound_rect,1,1,viewport.right-viewport.left-1,
		viewport.bottom-viewport.top-bar_height-1);
	FillRect (bound_rect,WHITE);
	win_group_control ->draw ();
}

int win_class::draw_win_contents ()
{
	getviewsettings (&current_viewport );
	setport(viewport);
	return 0;
	setport (current_viewport );
}

win_class::win_class
	(int ID,char *title,byte type,byte hotkey,
	 int left,int top,int width,int height
	) :object_class (ID,title,NORMAL, type ,left,top,width,height )

{	Twin /*  *ptemp_win, */ *temp1;

	canclose =TRUE;

//	this->title =title;
	this->hotkey =hotkey;
	this->visible =VISIBLE ;
// insert_control later !! OK !!

	this->punder_bitmap =NULL;
	this->pself_bitmap  =NULL;

	this->viewport.clip =CLIP_ON  ;
	this->win_group_control =new Tgroup(0,"",viewport.left,viewport.top,
		viewport.right-viewport.left,viewport.bottom-viewport.top);
//======================================================================
	this->pnext_win=NULL;
	this->pfirst_above =NULL;
	this->pfirst_under =NULL;
	this->pother_info		  = NULL	 ;
/*  now draw win frames & build needed links */
//	open_one_win (this );			used in the call func
	setup_window();
}

win_class::~win_class ()
{
	delete (win_group_control);
	close_one_win ();
	if (pcurrent_selected_win!=NULL ) setport (pcurrent_selected_win->viewport);
	thequeue.SendMessage(ID,WinSysClosedMSG,this);
}

void win_class::draw_size_rect()
{
//	disable();
	getviewsettings (&current_viewport );
	setwritemode (XOR_PUT);
	setcolor (GREEN);
	setport (win_living_viewport);
	setlinestyle ( DASHED_LINE,1,THICK_WIDTH );
	FrameRect(newbounds);
/*
#ifdef rectangle //(left,top,right,bottom)
#undef rectangle
//	FrameRect(newbounds);
	mouse_off;
	disable();
	rectangle(newbounds.left,newbounds.top,newbounds.right,newbounds.bottom);
	mouse_on;
#define rectangle(left,top,right,bottom)  mouse_off;rectangle((left),(top),(right),(bottom));mouse_on
#endif
	disable();
*/	setlinestyle ( SOLID_LINE,1,NORM_WIDTH );
	setwritemode (COPY_PUT);
	setport (current_viewport );
//	enable();
}

int win_class::move_size_window (int key_scan_num )
{
	draw_size_rect();
	switch (key_scan_num ) {
// move
		case LEFTKEY :
			if (newbounds.left > 0) {newbounds.left--;newbounds.right--;}
			break;
		case RIGHTKEY :
			if (newbounds.right< win_living_viewport.right-win_living_viewport.left){newbounds.left++;newbounds.right++;}
			break;
		case UPKEY :
			if (newbounds.top  > 0) {newbounds.top--;newbounds.bottom--;}
			break;
		case DOWNKEY :
			if (newbounds.bottom  < win_living_viewport.bottom-win_living_viewport.top) {newbounds.top++;newbounds.bottom++;}
			break;
// size
		case SHIFT_LEFT :
			if (newbounds.right>newbounds.left +100) {newbounds.right--;}//min_width
			break;
		case SHIFT_RIGHT:
			if (newbounds.right< win_living_viewport.right-win_living_viewport.left){newbounds.right++;}
			break;
		case SHIFT_UP :
			if (newbounds.bottom>newbounds.top +100) {newbounds.bottom--;}
			break;
		case SHIFT_DOWN :
			if (newbounds.bottom  < win_living_viewport.bottom-win_living_viewport.top) {newbounds.bottom++;}
			break;
// move
		case HOMEKEY :
			if (newbounds.left > 10) {newbounds.left -=10;newbounds.right-=10;}
			if (newbounds.top  > 10) {newbounds.top-=10;newbounds.bottom-=10;}
			break;
		case PGUPKEY :
			if (newbounds.right< win_living_viewport.right-win_living_viewport.left-10){newbounds.left+=10;newbounds.right+=10;}
			if (newbounds.top  > 10) {newbounds.top-=10;newbounds.bottom-=10;}
			break;
		case ENDKEY :
			if (newbounds.left > 10) {newbounds.left -=10;newbounds.right-=10;}
			if (newbounds.bottom  < win_living_viewport.bottom-win_living_viewport.top-10) {newbounds.top+=10;newbounds.bottom+=10;}
			break;
		case PGDNKEY :
			if (newbounds.right< win_living_viewport.right-win_living_viewport.left-10){newbounds.left+=10;newbounds.right+=10;}
			if (newbounds.bottom  < win_living_viewport.bottom-win_living_viewport.top-10) {newbounds.top+=10;newbounds.bottom+=10;}
			break;
// size
		case SHIFT_HOME :
			if (newbounds.right>newbounds.left +100 +10) {newbounds.right-=10;}//min_width
			if (newbounds.bottom>newbounds.top +100 +10) {newbounds.bottom-=10;}
			break;
		case SHIFT_PGUP:
			if (newbounds.right< win_living_viewport.right-win_living_viewport.left-10){newbounds.right+=10;}
			if (newbounds.bottom>newbounds.top +100 +10) {newbounds.bottom-=10;}
			break;
		case SHIFT_END:
			if (newbounds.right>newbounds.left +100 +10) {newbounds.right-=10;}//min_width
			if (newbounds.bottom  < win_living_viewport.bottom-win_living_viewport.top-10) {newbounds.bottom+=10;}
			break;
		case SHIFT_PGDN:
			if (newbounds.right< win_living_viewport.right-win_living_viewport.left-10){newbounds.right+=10;}
			if (newbounds.bottom  < win_living_viewport.bottom-win_living_viewport.top-10) {newbounds.bottom+=10;}
			break;

		case ENTERKEY:
			thequeue.SendMessage(pcurrent_selected_win->ID
				,WinMovedSizedMSG,pcurrent_selected_win );
			draw_size_rect();
			in_win_moving =FALSE;
			return TRUE;
		case ESCKEY:
			in_win_moving =FALSE;

			getviewsettings (&current_viewport );
			setport (menu_viewport);
			FillRect ( on_line_help_rect,HELP_RECT_COLOR );
			setcolor (HELP_TEXT_COLOR);
			outtextxy( on_line_help_rect.left+10,on_line_help_rect.top,pcurrent_menu->help);
			setport (current_viewport );
			return TRUE;
	};
	draw_size_rect();
	return TRUE;
}

Twin* win_class::pnext=NULL;

int win_class::key_pressed_handler  ( int key_scan_num )
{   Twin  *ptemp_win;
  // pnext  !  // used as CTRL_F6's link procedure

	if (pfirst_win_of_all == NULL ) return FALSE;
	if (pcurrent_selected_win->visible == IN_VISIBLE ) return FALSE;

	if (in_win_moving) {
		move_size_window(key_scan_num);
		return TRUE;
	}
	if (!(pcurrent_selected_win->type & DIALOG_WIN )) //not dialog
	switch (key_scan_num ) {
		case CTRL_F4 :
			thequeue.SendMessage(pcurrent_selected_win->ID
				,WinSysCloseMSG,pcurrent_selected_win );
			pnext=NULL;
			return TRUE;
		case CTRL_F6 :
			if (pnext==NULL ) pnext =pfirst_win_of_all;
			if ( (pnext != NULL)&& (pnext !=pcurrent_selected_win)){
				 pnext->select ();
				 pnext =pnext->pnext_win;
				 return TRUE;
			}
			return FALSE;
		case CTRL_F5 :
			newbounds=bounds;
			in_win_moving =TRUE;

			getviewsettings (&current_viewport );
			setport (menu_viewport);
			FillRect ( on_line_help_rect,HELP_RECT_COLOR );
			setcolor (HELP_TEXT_COLOR);
			outtextxy( on_line_help_rect.left+10,on_line_help_rect.top,"MOVING WIN");
			setport (current_viewport );

			draw_size_rect();
			old_mouse_x=mouse_x;
			old_mouse_y=mouse_y;
			return TRUE;
	};
	if (pcurrent_selected_win->win_group_control != NULL)
		return pcurrent_selected_win->win_group_control->key_pressed_handler
			(key_scan_num );
	else return FALSE;
}

int win_class::msg_handler ( MSG& message )
{   int x,y;
	int ret_val;

	switch (message.Action){
		case KeyPressedMSG:
			return key_pressed_handler( key_code );
		case WinOpenedMSG:
			return TRUE;
		case WinSelectedMSG:
//			if ((win_sysmenu != NULL ) && (  ((Twin*)message.fptr)->visible !=INVISIBLE ) ) {
			if ((win_sysmenu != NULL ) && (  ((Twin*)message.fptr)->visible !=2         ) ) {
//	x=((Twin*)message.fptr)->bounds.left+win_living_viewport.left-menu_viewport.left+LINE_WIDTH;
//	y=((Twin*)message.fptr)->bounds.top+win_living_viewport.top -menu_viewport.top+LINE_WIDTH;
	x=((Twin*)message.fptr)->bounds.left+win_living_viewport.left-menu_viewport.left+LINE_WIDTH-win_sysmenu->bounds.left+1;
	y=((Twin*)message.fptr)->bounds.top+win_living_viewport.top -menu_viewport.top+LINE_WIDTH-win_sysmenu->bounds.top+1;
				win_sysmenu->offsetmenu(x,y);
				win_sysmenu->status =NORMAL;
			}
			return TRUE;
/*		case WinMovingSizingMSG:	//for speed
			pcurrent_selected_win->	draw_size_rect();
			OffsetRect (&pcurrent_selected_win->newbounds,
				 mouse_x-old_mouse_x,mouse_y-old_mouse_y );
			if (pcurrent_selected_win->newbounds.right>
				win_living_viewport.right-win_living_viewport.left-1)
				pcurrent_selected_win->newbounds.right
					=win_living_viewport.right-win_living_viewport.left-1;
			if (pcurrent_selected_win->newbounds.bottom>
				win_living_viewport.bottom-win_living_viewport.top-1)
				pcurrent_selected_win->newbounds.bottom
					=win_living_viewport.bottom-win_living_viewport.top-1;
			if (pcurrent_selected_win->newbounds.left<1)
				pcurrent_selected_win->newbounds.left=1;
			if (pcurrent_selected_win->newbounds.top<1)
				pcurrent_selected_win->newbounds.top=1;

			old_mouse_x=mouse_x;
			old_mouse_y=mouse_y;
			pcurrent_selected_win->	draw_size_rect();
			return TRUE;
*/
		case WinMovedSizedMSG:
                in_win_moving =FALSE;
				pcurrent_selected_win->	draw_size_rect();
				getviewsettings (&current_viewport );
				setport (menu_viewport);
				FillRect ( on_line_help_rect,HELP_RECT_COLOR );
				setcolor (HELP_TEXT_COLOR);
				outtextxy( on_line_help_rect.left+10,on_line_help_rect.top,pcurrent_menu->help);
				setport (current_viewport );
		case WinRedrawboundsMSG:
			((Twin*)message.fptr)->close_one_win();
			bounds=newbounds;
			((Twin*)message.fptr)->setup_window();
			((Twin*)message.fptr)->open_one_win ();
			printf ("\007");
			return TRUE;
/*		case WinSysCloseMSG:
			if (! (canclose|| ( func_canclose()) ))  return FALSE;
	x=((Twin*)message.fptr)->bounds.left+win_living_viewport.left-menu_viewport.left+LINE_WIDTH;
	y=((Twin*)message.fptr)->bounds.top+win_living_viewport.top -menu_viewport.top+LINE_WIDTH;
				win_sysmenu->offsetmenu(-x,-y);
				win_sysmenu->menu_status |=INVISIBLE;
			delete ((Twin*)message.fptr );
			return TRUE ;
*///		case ControlSelectedMSG: --->ListBoxControlSelectedMSG
//			return TRUE;     //NOW IT IN APPLICATION
		case MouseLButtonDownMSG:
//  int win_mouse_x,win_mouse_y;
			sysmouse->get_posn();
			win_mouse_x=mouse_x; win_mouse_y=mouse_y;
			if (!Global2LocalPort(win_mouse_x,win_mouse_y,win_living_viewport)) return FALSE;
			if (!(pcurrent_selected_win->type & DIALOG_WIN )) { //not dialog
				if (!Global2Local(win_mouse_x,win_mouse_y,bounds)) {
				// see if in other windows's bounds;
					if (pfirst_win_of_all == NULL) return FALSE;
  Twin *ptmp;
					ptmp =pfirst_win_of_all;
					while ((ptmp!=NULL) && !(PtInRect(win_mouse_x,win_mouse_y,ptmp->bounds)) ) ptmp=ptmp->pnext_win;
					if (ptmp==NULL) return FALSE;
					ptmp->select();
					return TRUE;
				} else {  // in title_box;
					if((PtInRect(win_mouse_x,win_mouse_y,title_box)) ||
					   (PtInRect(win_mouse_x,win_mouse_y,size_box))
					  )	{
//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win
			newbounds=bounds;
			if (PtInRect(win_mouse_x,win_mouse_y,size_box))
				in_win_moving =SIZING;
			else	in_win_moving =TRUE;

			getviewsettings (&current_viewport );
			setport (menu_viewport);
			FillRect ( on_line_help_rect,HELP_RECT_COLOR );
			setcolor (HELP_TEXT_COLOR);
			outtextxy( on_line_help_rect.left+10,on_line_help_rect.top,"MOVING WIN");
			setport (current_viewport );

			draw_size_rect();
			sysmouse->get_release_count(LEFT_BUTTON);
			old_mouse_x=mouse_x;
			old_mouse_y=mouse_y;
			return TRUE;

//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win//MOVE win
						return TRUE;
					} else
					if (PtInRect(win_mouse_x,win_mouse_y,close_box)) {
						thequeue.SendMessage(pcurrent_selected_win->ID
						,WinSysCloseMSG,pcurrent_selected_win );
						pnext=NULL;
						return TRUE;
					} else
					if (PtInRect(win_mouse_x,win_mouse_y,zoom_out_box)) {
						SetRect (&newbounds,
							10,10,
							win_living_viewport.right-win_living_viewport.left-10,
							win_living_viewport.bottom-win_living_viewport.top-10);
						thequeue.SendMessage(this->ID
						,WinRedrawboundsMSG, this );
						oldbounds=bounds;
						return TRUE;
					} else
					if (PtInRect(win_mouse_x,win_mouse_y,zoom_in_box)) {
						newbounds=oldbounds;
						thequeue.SendMessage(this->ID
						,WinRedrawboundsMSG, this );
						return TRUE;
					}
				} //in this && other bounds
			} else { //is a dialog
				win_mouse_x=mouse_x; win_mouse_y=mouse_y;
				if (!Global2LocalPort(win_mouse_x,win_mouse_y,viewport)) {
					putch('\007');
					return FALSE;
				} else break;
			}
			return FALSE;	// unreachable
//		default:; //; // return FALSE;
	}
	if (win_group_control != NULL)   //Always true
			return win_group_control ->msg_handler(message);

}

BOOL win_class::func_canclose ()
{
	return TRUE;
}

void win_class::insert_control (Tcontrol *pcontrol_handled)
{
	win_group_control ->insert_control (pcontrol_handled);
}

void win_class::setup_window ()
{
	if (bounds.right>win_living_viewport.right-win_living_viewport.left-2)
		 bounds.right=win_living_viewport.right-win_living_viewport.left-2;
	if (bounds.bottom>win_living_viewport.bottom-win_living_viewport.top-2)
		 bounds.bottom=win_living_viewport.bottom-win_living_viewport.top-2;
	if (bounds.left<2)bounds.left=2;
	if (bounds.top<2) bounds.top=2;
	if (bounds.right <=bounds.left ) bounds.right =bounds.left +1;
	if (bounds.bottom<=bounds.top  ) bounds.right =bounds.top +1;

	viewport.left 	=bounds.left+win_living_viewport.left+LINE_WIDTH;
	viewport.top 	=bounds.top +win_living_viewport.top +bar_height+LINE_WIDTH;
	viewport.right 	=bounds.right-bounds.left+viewport.left-2*LINE_WIDTH;
	viewport.bottom =bounds.bottom-bounds.top+viewport.top-2*LINE_WIDTH;

	SetRect (&title_box,LINE_WIDTH+bar_height,LINE_WIDTH,
		bounds.right-bounds.left-2*LINE_WIDTH-2*bar_height,LINE_WIDTH+bar_height);
	SetRect (&close_box,LINE_WIDTH,LINE_WIDTH,
		LINE_WIDTH+bar_height,LINE_WIDTH+bar_height);
	//zoom_out_box;
	SetRect (&zoom_out_box,bounds.right-bounds.left-2*LINE_WIDTH-2*bar_height,
		LINE_WIDTH,
		bounds.right-bounds.left- 2*LINE_WIDTH- bar_height,
		LINE_WIDTH+bar_height);
	//zoom_in_box
	SetRect (&zoom_in_box,bounds.right-bounds.left-2*LINE_WIDTH-bar_height,
		LINE_WIDTH,
		bounds.right-bounds.left- 2*LINE_WIDTH,
		LINE_WIDTH+bar_height);

	SetRect (&size_box,bounds.right-bounds.left-2*LINE_WIDTH,
		bounds.bottom-bounds.top-2*LINE_WIDTH,
		bounds.right-bounds.left ,
		bounds.bottom-bounds.top );

	if (pcurrent_selected_win !=NULL)
			SetRect (&oldbounds,
					pcurrent_selected_win->bounds.left+20,
					pcurrent_selected_win->bounds.top +20,
					pcurrent_selected_win->bounds.left+310,
					pcurrent_selected_win->bounds.top +200);
	else SetRect (&oldbounds,20,20,310,210);
};

