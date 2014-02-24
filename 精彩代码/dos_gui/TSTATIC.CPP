/* 	yyxtedit.cpp
*/
#include "tstatic.h"

/**************************************************************************/
tstatic_class::tstatic_class(int ID,char *title_hotkey,
		int left,int top,int width,int height,char *default_text,
		int atype)
	:  control_class(ID,title_hotkey,DISABLE ,TSTATIC,
			left,top,width,height,0,0,0 )
{
	title_pos_x =left ;
	title_pos_y =top -bar_height-1 ;

	text_pos_x  =left+LINE_WIDTH;
	text_pos_y 	=top +( height-textheight("j") )/2-2;

	strcpy (text,default_text);
	static_type =atype;
}

/**************************************************************************/
// tstatic 's text could have a lot of lines ; assuming '\n' as the seperator
void tstatic_class::draw ()
{
	if ( status & INVISIBLE) return;    // INVISIBLE
	FillRect (bounds,WHITE);

  char buf[255];
  char *p_text=text;
  char *p_buf=buf;
  int  line_num=0;

	setcolor (BLACK);
	do  {
		p_buf=buf ;
		while ( (*p_buf = *p_text ) !='\n' && (*p_buf !='\0') ) {
			p_buf ++; p_text++;
		}
		p_text++;
		*p_buf ='\0';		// get one string ,'\n' included;
		switch (static_type ) {
			case ALIGN_MIDDLE:
//				title_pos_x =bounds.left + (bounds.right-bounds.left-textwidth (buf) )/2;
				text_pos_x=bounds.left + (bounds.right-bounds.left-textwidth (buf) )/2;
		}
		outtextxy (text_pos_x,text_pos_y+line_num*bar_height, buf);
		line_num ++;
	} while (*(p_text-1) !='\0' );

	control_class::draw ();
}
