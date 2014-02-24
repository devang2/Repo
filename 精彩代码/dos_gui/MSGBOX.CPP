// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR:  XuYongYong

/* --------------------------------------------------------
  MSGBOX.CPP
  -------------------------------------------------------- */

#include "msgbox.h"

msgbox_class::msgbox_class(char *title,char *msg_contents,int atype )
	:dialog_class(0xAAAA,title,
		(win_living_viewport.right-win_living_viewport.left-360)/2,
		(win_living_viewport.bottom-win_living_viewport.top-160)/2,
		360,160)
{ int x,y;
	x=( bounds.right-bounds.left -textwidth (msg_contents ) ) / 2;
	if (x<0) x=0;
//	insert_control(msg_tstatic=new Tstatic (11,"",x,20,380-x,20,
//		msg_contents,ALIGN_MIDDLE) );
	insert_control(msg_tstatic=new Tstatic (11,"",LINE_WIDTH,20,
		bounds.right-bounds.left-2*LINE_WIDTH,20,msg_contents,ALIGN_MIDDLE) );

	if ( atype & MB_ERROR ) puts ("\007\007");
	else if (atype & MB_WARING ) puts ("\007");

	if (atype &	MB_RETRYCANCEL){
			strcpy (ok_button->title,"RETRY");
			ok_button->setup_control();
	} else if (atype & 	MB_OK ) {
			OffsetRect (&ok_button->bounds,(bounds.right-bounds.left)/4,0);
			ok_button->setup_control();
			cancel_button->status |= DISABLE;
			cancel_button->status |= INVISIBLE;
// ugly things
// one button -----the other is hidden
	}
}
