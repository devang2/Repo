#include <conio.h>
#include <alloc.h>

#include "\vga\vga.h"
#include "text.h"

class MMap16_roam : public MouseMap16
	{
public:
	MMap16_roam(VGABASE *v) : MouseMap16(v)
		{ activMask=MouseMove; }
protected:
	void active(Event evt)
		{ ROAM::autoroam(evt.mouse.x,evt.mouse.y,wide,high); }
	} ;

void mouse_roam();
void key_roam();

void droam()
{
int n=3;
char *str[3]={	" Mouse Auto-roam (800*600 to 1024*1204 16 color) ",
				" Key Auto-roam (800*600 to 1024*1024 16 color) ",
				" Return "};
int i;
int x0=15,y0=8;
unsigned colh=color(0,7,0,0),coll=color(7,0,0,0);
void *buf;
buf=malloc(4200);

cclsall();
mmvprs(0,79,y0-3," Screen Roam ",color(2,0,0,0));
mmvprs(0,79,y0-2,"อออออออออออออ",color(2,0,0,0));
i=0;
while(1) {
	i=menu(n,i,x0,y0,coll,colh,str);
	gettext(1,1,80,25,buf);
	if(i==-1||i==(n-1))
		break;
	else if(i==0)
		mouse_roam();
	else
		key_roam();
	puttext(1,1,80,25,buf);
	}
free(buf);
cclsall();
}

void mouse_roam()
{
Event ev;
_800_600_16_roam roam(1024,1024,0);
int wide=roam.WIDE-1,high=roam.HIGH-1;
roam.init();
setup_key_mouse();
MOUSE::init();
MMap16_roam mmap(&roam);
MOUSE::putcentre();
roam.setcolor(4);
roam.line(0,0,wide,high);
roam.line(0,high,wide,0);
roam.ellipse(wide/2,high/2,wide/2,high/2);
MOUSE::show();
ev.getevent( (MouseDown|evKey) );
MOUSE::hide();
del_key_mouse();
roam.close();
}

void key_roam()
{
_800_600_16_roam roam(1024,1024,0);
int wide=roam.WIDE-1,high=roam.HIGH-1;
roam.init();
roam.setup_key_roam(16);
roam.setcolor(4);
roam.line(0,0,wide,high);
roam.line(0,high,wide,0);
roam.ellipse(wide/2,high/2,wide/2,high/2);
getkey();
roam.del_key_roam();
roam.close();
}

