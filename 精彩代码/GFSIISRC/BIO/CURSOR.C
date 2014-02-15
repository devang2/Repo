#include <alloc.h>
#include <graphics.h>

#include 	<DEF.INC>
#include 	<KEY.INC>
#include	<FUNC.INC>

/**/
/* CREATE A CURSOR */
void cursor(UI x1,UI y1,UI x2,UI y2)
{
void *buf;
UL size;

	size=imagesize(x1,y1,x2,y2);
    if (size!=-1)
	{
		buf = mem_alloc(size);
		getimage(x1,y1,x2,y2,buf);
		putimage(x1,y1,buf,NOT_PUT);
		farfree(buf);
	}
}
/*  END OF CURSOR  */


