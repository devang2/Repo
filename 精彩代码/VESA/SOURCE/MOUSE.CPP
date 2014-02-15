#include <dos.h>
#include <bios.h>
#include <alloc.h>

#include "\vga\vgabase.h"
#include "\vga\vgamouse.h"
#include "\vga\evtcode.h"

void init_key_mouse(void);
void del_key_mouse(void);

void Event::getKeyEvent()
{
int k,a,b;

if(bioskey(1)==0)
	return;

k=bioskey(0);
a=k>>8;
b=k&0x00ff;
if(b==0)
	k=256+a;
else
	k=b;
what=evKey;
key=k;
}

void Event::getMouseEvent()
{
MOUSE::getevent( *this );
}

struct PostBox {
	static Event post[8];
	static int beg;
	static int num,maxNum;
	} ;
Event PostBox::post[8];
int PostBox::beg=0;
int PostBox::maxNum=8;
int PostBox::num=0;

void Event::getMessageEvent()
{
if( PostBox::num==0 )
	what = evNothing;
else
	{
	*this=PostBox::post[PostBox::beg];
	PostBox::beg++;
	PostBox::num--;
	if( PostBox::beg>=PostBox::maxNum )
		PostBox::beg = 0;
	}
}

void Event::postmessage(void)
{
if(PostBox::num<PostBox::maxNum)
	{
	int i=PostBox::beg+PostBox::num;
	if( i>=PostBox::maxNum )
		i -= PostBox::maxNum;
	PostBox::post[i]=*this;
	PostBox::num++;
	}
}

void Event::clearpostbox()
{
PostBox::beg=PostBox::num=0;
}

void Event::getevent()
{
what=evNothing;
getMessageEvent();
if( what == evNothing )
	{
	if( MOUSE::OK )
		getMouseEvent();
	if( what == evNothing )
		getKeyEvent();
	}
}

void Event::getevent(unsigned mask)
{
if(mask==0)
	return;
do {
	getevent();
} while( (what&mask)==0 );
}

void Event::clear(void)
{
what=evNothing;
}

MouseMap *MOUSE::mmap=0;
int MOUSE::OK=0;
int MOUSE::showState=1;
long MOUSE::lastDownTime=0L;
long MOUSE::lastDownTimeR=0L;
long MOUSE::lastDownTimeM=0L;
int MOUSE::max_X=639;
int MOUSE::max_Y=479;
int MOUSE::centre_X=0;
int MOUSE::centre_Y=0;
int MOUSE::doubleInterval=7;
int MOUSE::autoInterval=7;

struct MouseState {
	unchar buttons;
	int x,y;
	long time;
	} ;
static MouseState lastMouse={0,0,0};

void MOUSE::getevent( Event& ev )
{
static int flag=1;
MouseState state;

getstate(state);
if(flag)
	{
	flag=0;
	lastMouse=state;
	}

if( state.buttons&mbLeftButton )
	{
	if( lastMouse.buttons&mbLeftButton )
		{
		if( state.time-lastDownTime>autoInterval )
			ev.what |= MouseAuto;
		}
	else
		{
		if( (state.time-lastDownTime)<doubleInterval &&
		state.time>lastDownTime )
			ev.what |= MouseDouble;
		else
			ev.what |= MouseDown;
		lastDownTime=state.time;
		}
	}
else if( lastMouse.buttons&mbLeftButton )
		ev.what |= MouseUp;

if( state.buttons&mbRightButton )
	{
	if( lastMouse.buttons&mbRightButton )
		{
		if( state.time-lastDownTimeR>autoInterval )
			ev.what |= MouseAutoR;
		}
	else
		{
		if( (state.time-lastDownTimeR)<doubleInterval &&
		state.time>lastDownTimeR )
			ev.what |= MouseDoubleR;
		else
			ev.what |= MouseDownR;
		lastDownTimeR=state.time;
		}
	}
else if( lastMouse.buttons&mbRightButton )
		ev.what |= MouseUpR;

if( state.buttons&mbMiddleButton )
	{
	if( lastMouse.buttons&mbMiddleButton )
		{
		if( state.time-lastDownTimeM>autoInterval )
			ev.what |= MouseAutoM;
		}
	else
		{
		if( (state.time-lastDownTimeM)<doubleInterval &&
		state.time>lastDownTimeM )
			ev.what |= MouseDoubleM;
		else
			ev.what |= MouseDownM;
		lastDownTimeM=state.time;
		}
	}
else if( lastMouse.buttons&mbMiddleButton )
		ev.what |= MouseUpM;

if( state.x!=lastMouse.x || state.y!=lastMouse.y )
	{
	ev.what |= MouseMove;
	if( mmap!=0 && showState==0 )
		{
		mmap->putback();
		mmap->show(state.x,state.y);
		}
	}
ev.mouse.buttons=state.buttons;
ev.mouse.x=state.x+centre_X;
ev.mouse.y=state.y+centre_Y;
ev.mouse.keystate=bioskey(2);
lastMouse = state;
if( mmap!=0 && (ev.what&mmap->activMask) )
	mmap->active(ev);
}

void MOUSE::getstate( MouseState& ms )
{
int m1=0x03,m2,m3,m4;
mouse_intr(m1,m2,m3,m4);
ms.buttons=m2;
ms.x=m3;
ms.y=m4;
ms.time=biostime(0,ms.time);
}

void MOUSE::mouse_intr(int &m1,int &m2,int &m3,int &m4)
{
union REGS inregs, outregs;

inregs.x.ax=m1; inregs.x.bx=m2;
inregs.x.cx=m3; inregs.x.dx=m4;
int86(0x33,&inregs,&outregs);
m1=outregs.x.ax; m2=outregs.x.bx;
m3=outregs.x.cx; m4=outregs.x.dx;
}

void MOUSE::mouse_intr(unsigned ax,unsigned bx,unsigned cx,unsigned dx,unsigned es)
{
struct REGPACK reg;
reg.r_ax=ax; reg.r_bx=bx;
reg.r_cx=cx; reg.r_dx=dx;
reg.r_es=es;
intr(0x33,&reg);
}

int MOUSE::init(void)
{
int ax=0,bx,cx,dx;
mouse_intr(ax,bx,cx,dx);
showState=1;
if(ax!=0) OK=1;
else OK=0;
return(OK);
}

void MOUSE::hide(void)
{
if( showState==0 && mmap!=0 )
	mmap->putback();
showState++;
}

void MOUSE::show(void)
{
if( showState>0 )
	{
	showState--;
	if( showState==0 && mmap!=0 )
		{
		int m1=3,m2,x,y;
		mouse_intr(m1,m2,x,y);
		mmap->show(x,y);
		}
	}
}

void MOUSE::putxy(int x,int y)
{
hide();
mouse_intr(4,0,x,y,0);
show();
}

void MOUSE::putcentre(void)
{
putxy(max_X/2-centre_X,max_Y/2-centre_Y);
}

void MOUSE::getxy(int &x,int &y)
{
int m1=3,m2;
mouse_intr(m1,m2,x,y);
x+=centre_X;
y+=centre_Y;
}

int MOUSE::inarea(int left,int top,int right,int bottom)
{
int x,y;
getxy(x,y);
if(x<left||x>right||y<top||y>bottom)
	return(0);
else
	return(1);
}

void MOUSE::setregion(int left,int top,int right,int bottom)
{
mouse_intr(7,0,left-centre_X,right-centre_X,0);
mouse_intr(8,0,top-centre_Y,bottom-centre_Y,0);
}

void MOUSE::restoreregion(void)
{
mouse_intr(7,0,0,max_X,0);
mouse_intr(8,0,0,max_Y,0);
}

void MOUSE::setspeed(int n)
{
mouse_intr(0x0f,0,n,n<<1,0);
}


MouseMap::~MouseMap()
{
MOUSE::hide();
free(buf);
MOUSE::mmap=0;
MOUSE::show();
}

void MouseMap::setup()
{
MOUSE::centre_X=centre_X;
MOUSE::centre_Y=centre_Y;
MOUSE::max_X=vga->WIDE-wide-1;
MOUSE::max_Y=vga->HIGH-high-1;
MOUSE::restoreregion();
MOUSE::hide();
size_t size;
size=(size_t)vga->imagesize(0,0,wide,high);
buf=malloc(size);
init();
MOUSE::mmap=this;
MOUSE::show();
}

void MouseMap::putback(void)
{
vga->putimageMEM(x1,y1,wide+1,high+1,buf);
}

void MouseMap::show(int x,int y)
{
COLOR col=vga->CUR_COLOR;
int fst=vga->getfillstyle();
x1=x; y1=y;
vga->getimageMEM(x1,y1,x1+wide,y1+high,buf);
draw(x,y);
vga->setfillstyle(fst);
vga->CUR_COLOR=col;
}
