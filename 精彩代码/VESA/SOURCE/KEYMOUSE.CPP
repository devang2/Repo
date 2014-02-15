#include <dos.h>

void interrupt (*oldkbint0x09)(void);
void interrupt mykbint0x09(void);

void interrupt (*oldmouseintr)(void);
void interrupt mymouseintr(unsigned bp,unsigned di,unsigned si,
						unsigned ds,unsigned es,unsigned dx,
						unsigned cx,unsigned bx,unsigned ax);

void setup_key_mouse();
void del_key_mouse();
void move_mouse_cur();

static int KEY_EMU_MOUSE=0;

void setup_key_mouse()
{
union REGS inregs, outregs;

inregs.x.ax=0;
int86(0x33,&inregs,&outregs);
if(outregs.x.ax==0)
	{
	oldkbint0x09=(void interrupt(*)())getvect(0x09);
	disable();
	setvect(0x09,(void interrupt(*)(...))mykbint0x09);
	enable();

	oldmouseintr=(void interrupt(*)())getvect(0x33);
	disable();
	setvect(0x33,(void interrupt(*)(...))mymouseintr);
	enable();

	KEY_EMU_MOUSE=1;
	}
}

void del_key_mouse()
{
if(KEY_EMU_MOUSE)
	{
	disable();
	setvect(0x09,(void interrupt(*)(...))oldkbint0x09);
	setvect(0x33,(void interrupt(*)(...))oldmouseintr);
	enable();
	KEY_EMU_MOUSE=0;
	}
}

#define MOUSE_KEY_N 7
static unsigned char _mouse_key_value[2][MOUSE_KEY_N]=
			{{56,29,72,80,75,77,0},{184,157,200,208,203,205,0}};
static char _mouse_key_press[MOUSE_KEY_N]={0,0,0,0,0,0,0};

void interrupt mykbint0x09()
{
static unsigned mesp,mess,invalue,i,j,k,kb,f224=0;
disable();
mesp=_SP; mess=_SS;
enable();
invalue=inportb(0x60);
if(invalue==224)
	{
	k=0;
	f224=1;
	}
else if(f224==1)
	{
	f224=0;
	k=1;
	for(i=0;i<2;i++)
		for(j=0;j<MOUSE_KEY_N-1;j++)
			if(invalue==_mouse_key_value[i][j])
				{
				_mouse_key_press[j]=1-i;
				k=0;
				break;
				}
	}
else if(invalue==54)
	{
	_mouse_key_press[6]=1;
	k=0;
	}
else if(invalue==182)
	{
	_mouse_key_press[6]=0;
	k=0;
	}
else
	k=1;
disable();
_SP=mesp; _SS=mess;
enable();
if(k)
	(*oldkbint0x09)();
else
	{
	kb=inportb(0x61); outportb(0x61,0x80);
	outportb(0x61,kb); outportb(0x20,0x20);
	}
}

static int _mouse_show=-1;
static int _mouse_x=320,_mouse_y=240,_mouse_up_x,_mouse_up_y;
static int _min_x=0,_max_x=624,_min_y=0,_max_y=464;
static int _move_xy=1;
static int _delay_time,_base_delay_time=7;

void move_mouse_cur()
{
static unsigned int f=0;
if(_mouse_key_press[2])
	{
	_mouse_y-=_move_xy;
	if(_mouse_y<_min_y)
		_mouse_y=_min_y;
	}
if(_mouse_key_press[3])
	{
	_mouse_y+=_move_xy;
	if(_mouse_y>_max_y)
		_mouse_y=_max_y;
	}
if(_mouse_key_press[4])
	{
	_mouse_x-=_move_xy;
	if(_mouse_x<_min_x)
		_mouse_x=_min_x;
	}
if(_mouse_key_press[5])
	{
	_mouse_x+=_move_xy;
	if(_mouse_x>_max_x)
		_mouse_x=_max_x;
	}
if( _mouse_x!=_mouse_up_x || _mouse_y!=_mouse_up_y )
	{
	if(f==1)
		{
		_delay_time=8;
		_move_xy=2;
		}
	else if(f>=14&&f<22)
		_delay_time--;
	else if(f==22)
		_move_xy=3;
	_mouse_up_x=_mouse_x;
	_mouse_up_y=_mouse_y;
	delay(_delay_time+_base_delay_time);
	f++;
	}
else
	{
	f=0;
	_delay_time=110;
	_move_xy=1;
	}
}

void interrupt mymouseintr(unsigned bp,unsigned di,unsigned si,
						unsigned ds,unsigned es,unsigned dx,
						unsigned cx,unsigned bx,unsigned ax)
{
static unsigned i;

i=bp; i=di; i=si; i=ds; i=es;
move_mouse_cur();
if(ax==0)
	{
	ax=-1;
	bx=2;
	}
else if(ax==3)
	{
	cx=_mouse_x;
	dx=_mouse_y;
	bx=0;
	if(_mouse_key_press[0])
		bx=1;
	if(_mouse_key_press[1])
		bx+=2;
	if(_mouse_key_press[6])
		bx+=4;
	}
else if(ax==4)
	{
	if(cx<_min_x) cx=_min_x;
	if(cx>_max_x) cx=_max_x;
	if(dx<_min_y) dx=_min_y;
	if(dx>_max_y) dx=_max_y;
	_mouse_x=cx;
	_mouse_y=dx;
	_mouse_up_x=_mouse_x;
	_mouse_up_y=_mouse_y;
	}
else if(ax==7)
	{
	if(cx>dx)
		{ i=cx; cx=dx; dx=i; }
	_min_x=cx;
	_max_x=dx;
	if(_mouse_x<_min_x)
		_mouse_x=_min_x;
	if(_mouse_x>_max_x)
		_mouse_x=_max_x;
	}
else if(ax==8)
	{
	if(cx>dx)
		{ i=cx; cx=dx; dx=i; }
	_min_y=cx;
	_max_y=dx;
	if(_mouse_y<_min_y)
		_mouse_y=_min_y;
	if(_mouse_y>_max_y)
		_mouse_y=_max_y;
	}
else if(ax==0x0f)
	{
	i=cx;
	if(i<1) i=1;
	if(i>24) i=24;
	_base_delay_time=i-1;
	}
}
