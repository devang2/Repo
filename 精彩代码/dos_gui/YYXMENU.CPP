// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	yyxmenu.cpp
*/

#ifndef __YYXMENU_H
#include "yyxmenu.h"
#endif
#ifndef __YYXWIN_H
#include "yyxwin.h"
#endif
#ifndef __APPLICAT_H
#include "applicat.h"
#endif

void menu_class::select()
{
	setport (menu_viewport);
	PaintRect ( bounds,	MENU_ITEM_SELECT_COLOR ,COPY_PUT );
//	if (->pfather_menu != NULL ) //The Only exception caused the mouse dead
	pfather_menu->psub_menu_selected  =this  ;
	draw_help_texts (help);
	draw ();
}

void menu_class::unselect ()
{
	setport (menu_viewport);
	PaintRect ( bounds,	MENU_RECT_COLOR ,COPY_PUT );
	pfather_menu->psub_menu_selected =NULL;
	draw ();
}

void menu_class::offsetmenu(int dx,int dy )
{ Tmenu   * ptmp;
  int i;

	OffsetRect (&bounds,dx,dy );
	if ((ptmp =pson_menu) == NULL ) return;
	OffsetRect (&save_bitmap.bounds,dx,dy );
	for (i=0; i<sub_menu_nums ; i++ ){
		ptmp=(Tmenu *) ptmp->pright;
		ptmp->offsetmenu(dx,dy);
	}
}

void menu_class::draw_sub_menu_rect ()
{	struct Rect therect;

	setport (menu_viewport);
	FillRect ( save_bitmap.bounds , MENU_RECT_COLOR);
	therect = save_bitmap.bounds;
//	InsetRect (&therect,-5,-5 );
	setcolor (BLACK);
	if ( pfather_menu !=NULL ) FrameRect ( therect);
}


void menu_class::enter_menu ()
{   Tobject * ptmp;
	int i;

	if ( pcurrent_menu->psub_menu_selected==NULL ) 	return ;
	if (pcurrent_menu->psub_menu_selected->status & DISABLE ) return;
	if ( pcurrent_menu->psub_menu_selected->pson_menu == NULL ) {
		if (pcurrent_menu->pfather_menu == NULL ) {
			pcurrent_menu = pcurrent_menu->psub_menu_selected;
			if (pcurrent_menu->psub_menu_selected !=NULL )
				pcurrent_menu->psub_menu_selected->select
				();
			return;
		} else {
//			thequeue.SendMessage(pcurrent_menu->psub_menu_selected->ID
//				,MenuActionMSG,pcurrent_menu->psub_menu_selected );
// Note: this is a way of communication mode
			if (in_menu_trap)
				menu_save_queue.SendMessage(pcurrent_menu->psub_menu_selected->ID
				,MenuActionMSG,pcurrent_menu->psub_menu_selected );
			else
				thequeue.SendMessage(pcurrent_menu->psub_menu_selected->ID
				,MenuActionMSG,pcurrent_menu->psub_menu_selected );
			if (pcurrent_menu !=NULL ) pcurrent_menu->esc_all_menus();
//			menu_func ( pcurrent_menu->psub_menu_selected->ID  );
			return;
		}	/* execute a command */
	}
	else   {
	pcurrent_menu = pcurrent_menu->psub_menu_selected;
	pcurrent_menu->save_bitmap.size
		=(unsigned long) RectSize (pcurrent_menu->save_bitmap.bounds);
	pcurrent_menu->save_bitmap.pbitmap
		= myfarmalloc (pcurrent_menu->save_bitmap.size );
	getimage ( pcurrent_menu->save_bitmap.bounds.left,
		pcurrent_menu->save_bitmap.bounds.top,
		pcurrent_menu->save_bitmap.bounds.right,
		pcurrent_menu->save_bitmap.bounds.bottom,
		pcurrent_menu->save_bitmap.pbitmap);
	}
/* used by main menu and those selection which have sub_menus   */
	pcurrent_menu->draw_sub_menu_rect( );

	ptmp=pcurrent_menu->pson_menu;
	for (i=0;i<pcurrent_menu->sub_menu_nums ; i++ ) {
		if (! ( ptmp->status & INVISIBLE) )
		ptmp->draw ();
		ptmp=ptmp->pright ;
	}
	if (pcurrent_menu->psub_menu_selected !=NULL )
		pcurrent_menu->psub_menu_selected->select
			();
}

void menu_class::draw ()
{
	setport (menu_viewport);
	moveto
	(bounds.left+5,bounds.top);

	if (status & DISABLE ) {
		setcolor ( MENU_ITEM_DISABLED_COLOR );
		outtext  (title);
	}else if (hotkey == 0xff ) {
		if (this !=pfather_menu->psub_menu_selected )
		setcolor (MENU_ITEM_ENABLED_COLOR );
		else setcolor ( MENU_ITEM_SELECTED_COLOR );
		outtext (title );
	} else 	{
		if (this !=pfather_menu->psub_menu_selected )
		setcolor (MENU_ITEM_ENABLED_COLOR );
		else setcolor ( MENU_ITEM_SELECTED_COLOR );

		outtext (title );
		draw_hotkey(title,hotkey,bounds.left+5,bounds.top);
	}
}

void  menu_class::draw_help_texts (char *help_texts)
{
	setport (menu_viewport);
	FillRect ( on_line_help_rect,HELP_RECT_COLOR );
	moveto(on_line_help_rect.left+10,on_line_help_rect.top);
	setcolor (HELP_TEXT_COLOR);
	outtext (help_texts);
}


int  menu_class::esc_menu ()
{ // pcurrent_menu---<this
	if ( this->pfather_menu ==  NULL ) {
		if ( this->psub_menu_selected != NULL ) {
		this->psub_menu_selected->unselect
			();
//		this->psub_menu_selected =NULL;
///		if ( this->psub_menu_selected == NULL )		///5/2/1994
			draw_help_texts(this-> help); ///	else
///		draw_help_texts(this->psub_menu_selected->help);
		}
		if (in_menu_trap){
			in_menu_trap =FALSE;  ////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			sysmsg=menu_save_sysmsg;
			thequeue=menu_save_queue;
		}
		if ( pcurrent_selected_win !=NULL ) setport ( pcurrent_selected_win->viewport);
/////////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!
		return (0);
	}
/* above is for main menu */
	if (this->save_bitmap.pbitmap != NULL ) {
		putimage ( this->save_bitmap.bounds.left ,
			this->save_bitmap.bounds.top,
			this->save_bitmap.pbitmap ,COPY_PUT  );
		farfree ( this->save_bitmap.pbitmap  );
	}
	pcurrent_menu= this ->pfather_menu ;
///	if ( pcurrent_menu->psub_menu_selected == NULL )  ///5/2/1994
		draw_help_texts(pcurrent_menu-> help); ///	else
///	 draw_help_texts(pcurrent_menu->psub_menu_selected->help);
	return (1);
}

Tmenu * menu_class::get_next_sub_item (enum direction direct)
{ Tmenu *ptmp;
	ptmp =pcurrent_menu->psub_menu_selected;
	if (ptmp==NULL ) { if ( pcurrent_menu->pson_menu == NULL ) return NULL;
		else  ptmp =(Tmenu*)pcurrent_menu->pson_menu->pleft;
	}
	return (Tmenu*)ptmp->get_next_object( direct, TRUE);
	//determine whether to go to disabled item
}


int menu_class::Left_Right_key_handler(enum direction scroll_direction)
{
	if (pcurrent_menu->pfather_menu->pfather_menu ==NULL)
	{
		pcurrent_menu->esc_menu ();

		change_select (pcurrent_menu->psub_menu_selected,
			pcurrent_menu->get_next_sub_item ( scroll_direction));
		pcurrent_menu->psub_menu_selected->enter_menu ();
		return TRUE;
	 }
	 else if ((pcurrent_menu->pfather_menu == NULL )&&
			(pcurrent_menu->psub_menu_selected !=NULL )) {
		change_select (pcurrent_menu->psub_menu_selected,
			pcurrent_menu->get_next_sub_item ( scroll_direction));

		return TRUE;
	 }
	 return FALSE;
}

void menu_class::esc_all_menus(void)
{
	while ( pcurrent_menu->pfather_menu != NULL )
		pcurrent_menu->esc_menu ( );
	if (pcurrent_menu->psub_menu_selected !=NULL)
		pcurrent_menu->psub_menu_selected->unselect
			();
		if (in_menu_trap){
			in_menu_trap =FALSE;  ////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			sysmsg=menu_save_sysmsg;
			thequeue=menu_save_queue;
		}

		if ( pcurrent_selected_win !=NULL ) setport ( pcurrent_selected_win->viewport);
//	pcurrent_menu->psub_menu_selected=NULL;
}

int menu_class::bar_Imme_menu_key_handler( int key_scan_num)
{ Tobject * ptmp;
  int i;

  ptmp =pcurrent_menu->pson_menu;
  for(i=0;i<pcurrent_menu->sub_menu_nums;i++ ){
	ptmp=ptmp->pright;

	if ( (    (pcurrent_menu->pfather_menu ==NULL)
		   && (alt_key[ toupper(ptmp->title[ptmp->hotkey])-'A'] ==key_scan_num)
		 )
		 ||
		 (    (pcurrent_menu->pfather_menu !=NULL)
		   && (  (toupper( ptmp->title[ptmp->hotkey])) ==
				 (toupper( lo (key_scan_num))
			  )
		 )
		 ||
		 (    (toupper( ptmp->title[ptmp->hotkey])) ==' ' )
		   && ( key_scan_num == ALT_SPACE )
		 )
	   ) {
		change_select (
		pcurrent_menu->psub_menu_selected, ptmp );
		  if (pcurrent_menu->psub_menu_selected != NULL ) {
			if (!in_menu_trap){
				in_menu_trap =TRUE;	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				menu_save_sysmsg=sysmsg;
				menu_save_queue=thequeue;
			}

			pcurrent_menu->psub_menu_selected->enter_menu ();
		  }
		return TRUE;
	}
  }
  for (i=0;i<Max_Imme_key_nums ;i++) {
	if (Immediate_key[i] ==key_scan_num ) {
		if (! Imme_Menu_fptr[i] ->status & DISABLE )
//			thequeue.SendMessage(Imme_Menu_ID[i],MenuActionMSG,NULL);
			if (in_menu_trap)
				menu_save_queue.SendMessage(Imme_Menu_ID[i],MenuActionMSG,NULL);
			else
				thequeue.SendMessage(Imme_Menu_ID[i],MenuActionMSG,NULL);
		if (pcurrent_menu !=NULL ) pcurrent_menu->esc_all_menus();
		return TRUE;
	}
  }
  return FALSE;
}

int  menu_class::key_pressed_handler( int key_scan_num )
{
	switch (key_scan_num) {
	case LEFTKEY	: return Left_Right_key_handler(TO_UP);
	case RIGHTKEY	: return Left_Right_key_handler(TO_DOWN);
	case UPKEY		:
	if (pcurrent_menu->pfather_menu !=NULL) {
		change_select (pcurrent_menu->psub_menu_selected,
			pcurrent_menu->get_next_sub_item (TO_UP));
		return TRUE;
	} else return FALSE;
	case DOWNKEY		:
	if (pcurrent_menu->pfather_menu !=NULL)  {
		change_select (pcurrent_menu->psub_menu_selected,
			pcurrent_menu->get_next_sub_item (TO_DOWN));
		return TRUE;
	}  /* !!!!!! */
	case ENTERKEY	:
	if (pcurrent_menu->psub_menu_selected != NULL ) {
		pcurrent_menu->psub_menu_selected->enter_menu ();
		return TRUE;
	} else return FALSE;
	case ESCKEY	:   /* note: isn't same as ENTERKEY */
		if (in_menu_trap) {
			pcurrent_menu->esc_menu ();
			return TRUE;
		} else return FALSE;
	}
	if (   bar_Imme_menu_key_handler(key_scan_num)  == TRUE ) return TRUE;
/*	normal_key_handler (key_scan_num );			*/
	esc_all_menus();
	return FALSE;
}

menu_class::menu_class (char *init_title,char *init_help)
:object_class (ID,init_title,NORMAL, NORMAL,0,0,0,0 )
{
//	title =init_title;
	help  =init_help;
	hotkey=1;
	Imme_key_num =0;
//	status =NORMAL;
	sub_menu_nums =1;
//	SetRect (&bounds ,0,0,0,0 );
	SetRect (&save_bitmap.bounds ,0,0,0,0);
	psub_menu_selected  =NULL;
	pfather_menu =NULL;
	pson_menu    =NULL;
	pright =NULL;
	pleft =NULL;
//	pmenu_operation = std_menu_operation;
	save_bitmap.pbitmap =NULL;

	pcurrent_menu =this;
}


void menu_class::createmenu_class
	(Tmenu *pfather,int ID,char *title,char *help,byte hotkey,int Imme_key_num,
	 byte status,int savebit_left,int savebit_top,int savebit_right,int savebit_bottom
	 )
{
int text_width=textwidth(title)+5;

	if ( pfather !=NULL )
		SetRect (&(this->bounds),0,0,0,0);	//used for later
	this->ID = ID;
//	strcpy (this->title,title);
	this->help =help;
//	this->hotkey=hotkey;
	this->Imme_key_num =Imme_key_num;
	this->status =status;
	SetRect (&(this->save_bitmap.bounds),
		 savebit_left,savebit_top,savebit_right,savebit_bottom );
	this->pfather_menu =pfather;

//	this->pmenu_operation = std_menu_operation;
	this->save_bitmap.pbitmap =NULL;
	this->pson_menu    		=NULL;
	this->psub_menu_selected 	=NULL;
	this->sub_menu_nums 		=0;
	this->pleft  		=this;
	this->pright  		=this;

	if (pfather !=NULL ){
		pfather->sub_menu_nums++;
Tmenu *ptemp1;
		ptemp1=pfather->pson_menu;
		if (ptemp1==NULL ) {
			pfather->pson_menu=this;
		} else {
			this->pright			=ptemp1;
			this->pleft 			=ptemp1->pleft;
			ptemp1->pleft->pright =this;
			ptemp1->pleft 				=this;
		}
		if (pfather->pfather_menu == NULL )
// use 1st line of the procedure of clearing , rely on above
		SetRect (&(this->bounds),
			pleft->bounds.right,
			pfather->save_bitmap.bounds.top,
			pleft->bounds.right + text_width,
			pfather->save_bitmap.bounds.bottom
		);
		else {
		SetRect (&(this->bounds),
			pfather->save_bitmap.bounds.left,
			pfather->save_bitmap.bounds.bottom -bar_height,
			pfather->save_bitmap.bounds.right,
			pfather->save_bitmap.bounds.bottom
		);
		InsetRect (&(this->bounds),-2,-1);
		}
	}
//	return (this);
}

menu_class::menu_class (int ID,char *title_hotkey,char *help,Tmenu *pfather )
:object_class (ID,title_hotkey,NORMAL, NORMAL,0,0,0,0 )
{ int i,j;
//byte hotkey=0xff, status =NORMAL;
  int Imme_key_num;
  int savebit_left=0,savebit_right=0,savebit_top=0,savebit_bottom=0;

//  get_title_hotkey(title_hotkey,hotkey);
  int text_width =textwidth (title_hotkey) +5;

// my_menu[0], i.e. the screen Menu
	if (pfather==NULL ) {
		SetRect(&bounds,0,0,menu_viewport.right-menu_viewport.left,bar_height);
		savebit_left=0;
		savebit_top =bar_height;
		savebit_right=menu_viewport.right-menu_viewport.left;
		savebit_bottom=bar_height+bar_height;
	}
	else if (pfather->pfather_menu ==NULL );
//  menu in the menu_bar
	else if (pfather->pfather_menu->pfather_menu == NULL ){
//  menu below the menu_bar
		if ( pfather->pson_menu==NULL) {
			pfather->save_bitmap.bounds =pfather->bounds;
			OffsetRect( &pfather->save_bitmap.bounds,0,bar_height );
//			pfather->save_bitmap.bounds.top -=1;
		} else pfather->save_bitmap.bounds.bottom +=bar_height;
		if ( pfather->save_bitmap.bounds.right-pfather->save_bitmap.bounds.left
			< text_width )
			 pfather->save_bitmap.bounds.right =
			 pfather->save_bitmap.bounds.left+text_width;
	}
	else {      // other case
//  menu of the other case
		if ( pfather->pson_menu==NULL) {
			pfather->save_bitmap.bounds =pfather->bounds;
			OffsetRect( &pfather->save_bitmap.bounds,
				pfather->bounds.right-pfather->bounds.left ,0);
			pfather->save_bitmap.bounds.top -=2;
		} else pfather->save_bitmap.bounds.bottom +=bar_height;
		if ( pfather->save_bitmap.bounds.right-pfather->save_bitmap.bounds.left
			< text_width )
			 pfather->save_bitmap.bounds.right =
			 pfather->save_bitmap.bounds.left+text_width;
	}

	if (  (pfather != NULL ) && (pfather->pfather_menu !=NULL ) ) //down_menu
	if (pfather->pson_menu !=NULL)  {
Tobject *ptemp=pfather->pson_menu;
		for (i=1;i<=pfather->sub_menu_nums;i++, ptemp=ptemp->pright)
		if (ptemp->bounds.right<save_bitmap.bounds.right)
			ptemp->bounds.right=pfather->save_bitmap.bounds.right-1;
	}

	createmenu_class
	(pfather,ID,title_hotkey,help,hotkey,Imme_key_num,status,savebit_left,
	savebit_top,savebit_right,savebit_bottom);
}

menu_class::~menu_class (void)
{
	if (save_bitmap.pbitmap != NULL )
		farfree (save_bitmap.pbitmap );
}


int  menu_class::msg_handler( MSG& message )
{
	switch ( message .Action){
		case KeyPressedMSG:
			return key_pressed_handler( key_code );
		case MouseLButtonDownMSG:
  int x,y;
  Tmenu *ptmp;
			sysmouse->get_posn();
			x=mouse_x; y=mouse_y;
			if (Global2LocalPort(x,y,menu_viewport)) {
			//  in menu_viewport
	Tmenu * ptmp1;

				ptmp =pcurrent_menu;
				ptmp1=NULL;

//				if ((ptmp!=NULL) && (PtInRect(x,y,pcurrent_menu->save_bitmap.bounds)) )
				if ( (ptmp!=NULL) )
				while (ptmp1==NULL)
				{
					if ((ptmp->save_bitmap.pbitmap !=NULL) &&
						(PtInRect (x,y,ptmp->save_bitmap.bounds))
					   )
						ptmp1=(Tmenu*)ptmp->pson_menu->get_object_thru_point (x,y,TRUE);
					else ptmp ->esc_menu();
					if (ptmp->pfather_menu ==NULL) break;
					ptmp =ptmp->pfather_menu;
				}
				if (ptmp1 !=NULL ) {
					if (ptmp1==pcurrent_menu->psub_menu_selected) ptmp1->enter_menu();
					else { change_select(pcurrent_menu->psub_menu_selected,ptmp1);
						if (!in_menu_trap) {
							in_menu_trap=TRUE;
							menu_save_sysmsg=sysmsg;
							menu_save_queue=thequeue;
						}
					}
					return TRUE;
				}
				return FALSE;
			} // in menu_viewport
			break;
	}
	return FALSE;
}


void menu_class::dispose_all_submenus()
{   Tmenu *ptemp, *ptemp1;

	if (pson_menu==NULL ) return ;
	for (ptemp =pson_menu;1;	ptemp =ptemp1) {
		ptemp1 =(Tmenu*) ( ((Tobject *)ptemp)->pright) ;
		if (ptemp  != NULL ) {
			ptemp->dispose_all_submenus();
			delete (ptemp);
		}
		if (ptemp1 ==pson_menu) break;
	}
	pson_menu=NULL;
	psub_menu_selected=NULL;
}


