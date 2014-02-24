#include "editdlg.h"
//#include <string.h>

editdlg_class::editdlg_class(int ID,char *title,int left,int top,int width,int height,
	char * old_content )
	:dialog_class(ID,title,left,top,width,height)
{
	content =new Ttedit(3,"",20,20,width -40,25, old_content );
	buf =old_content;

	insert_control (content );
	canclose =FALSE;
}

BOOL editdlg_class::func_canclose()
{
	if ( strlen (content->text) == 0 ) return FALSE;
	else {
		strcpy (buf ,content->text );
		return TRUE;
	}
}



