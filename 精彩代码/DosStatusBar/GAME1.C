#include<io.h>
#include<stdio.h>
#include<dos.h>
#include<string.h>
#include<math.h>
#include<stdio.h>
#include<bios.h>
#include<mem.h>
#include<fcntl.h>
#include<stdlib.h>
#include<conio.h>
#include<process.h>

#define WINDOWS 0
#define BUTTON 1
#define BUTTON_PRESS 2
#define BUTTON_SMALL 3
#define BUTTON_SMALL_PRESS 4
#define BUTTON_CLOSE 5
#define BUTTON_CLOSE_PRESS 6
#define BUTTON_MENU 7
#define BAR 8

#define BUTTON_COLOR 24
#define BUTTON_COLOR_LIGHT 7
#define BUTTON_COLOR_DARK 8

#define BUTTON_X 35
#define BUTTON_Y 10
#define BUTTON_XY 10

#define WINDOWS_X 0
#define WINDOWS_Y 186
#define WINDOWS_X_MOVE 319
#define WINDOWS_Y_MOVE 13

#define WINDOWS_COLOR_OUT 24
#define WINDOWS_COLOR_IN 15
#define WINDOWS_COLOR_LIGHT 7
#define WINDOWS_COLOR_DARK 8
#define WINDOWS_COLOR_TITLE 12

#define MAX 0
#define MIN 1

#define ACTIVE 1
#define NOACTIVE 0

#define ACTIVE_COLOR 1
#define NOACTIVE_COLOR 8

#define VGA256 0x13
#define TEXT_MODE 0x03
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define PALETTE_MASK 0x3c6
#define PALETTE_REGISTER_RD 0x3c7
#define PALETTE_REGISTER_WR 0x3c8
#define PALETTE_DATA 0x3c9

#define CHAR_HEIGHT 8
#define CHAR_WIDTH 8

#define MOUSE_INT 0x33
#define MOUSE_RESET 0x00
#define MOUSE_SHOW 0x01
#define MOUSE_HIDE 0x02
#define MOUSE_BUTT_POS 0x03
#define MOUSE_SET_SENSITIVITY 0x1A
#define MOUSE_MOTION_REL 0x0B
#define MOUSE_LEFT_BUTTON 0x01
#define MOUSE_RIGHT_BUTTON 0x02
#define MOUSE_CENTER_BUTTON 0x04

int xa=0,ya=0,l=0;
FILE *stream="load.dat";
char flag=',';


typedef struct{
  int mak[2][16];
  int hor;
  int ver;
  }mscurstype;

mscurstype handcurs={-7681,-7681,-7681,-7681,-7681,-8192,-8192,-8192,
		      0,    0,    0,    0,    0,    0,    0,    0,
		      7680,4608,4608,4608,4608,5119,4681,4681,4681,
		      -28671,-28671,-28671,-22767,-32767,-32767,-1,4,0};

mscurstype arrow={0x3fff,0x1fff,0x0fff,0x07ff,
 0x03ff,0x01ff,0x00ff,0x007f,
 0x003f,0x00ff,0x01ff,0x10ff,
 0x30ff,0xf87f,0xf87f,0xfc3f,
 0x0000,0x4000,0x6000,0x7000,
 0x7800,0x7c00,0x7e00,0x7f00,
 0x7f80,0x7e00,0x7c00,0x4600,
 0x0600,0x0300,0x0300,0x0180,0,0};

mscurstype lin={0xe3e3,0xe3e3,0xe3e3,0xe3e3,
0x8080, 0x8080,0x8080,0xe3e3,
0xc1c1, 0x8080,0x0000,0x2323,
0xe3e3,0xe3e3,0xe3e3, 0xe3e3,
 0x0000,0x0808,0x0808,0x0808,
 0x0808,0x3e3e,0x0808,0x0808,
 0x1c1c,0x2a2a,0x4848,0x0808,
 0x0808,0x0808,0x0808,0x0000,2,0};

mscurstype qi={0xff89,0xff88,0xff8e,0x8001,
0x8001,0x8001,0x8d8f,0x8c8c,
0x8d8c,0x8d88,0x8009,0x8983,
0x9542,0x1540,0x39b0,0x7d79,
0x0000,0x0020,0x0020,0x0020,
0x3ffc,0x2020,0x2020,0x2020,
0x2020,0x2020,0x2020,0x2020,
0x0000,0x4008,0x0004,0x0000,16,0};

mscurstype arrowcurs={16383,8191,4095,2047,1023,511,225,127,63,
		       31,511,4351,12543,-1921,-1921,-961,0,16348,
		       24576,28672,30720,31744,32256,32512,32640,
		       32704,31744,17920,17920,768,384,192,-1,-1};


int Squeeze_Mouse(int command,int *x,int *y,int *buttons)
{
union REGS inregs,outregs;

switch(command)
{
case MOUSE_RESET:
{
inregs.x.ax=0x00;
int86(MOUSE_INT,&inregs,&outregs);
*buttons=outregs.x.bx;
return(outregs.x.ax);
}break;



case MOUSE_SHOW:
{
inregs.x.ax=0x01;
int86(MOUSE_INT,&inregs,&outregs);
return(1);
}break;


case MOUSE_HIDE:
{
inregs.x.ax=0x02;
int86(MOUSE_INT,&inregs,&outregs);
return(1);
}break;


case MOUSE_BUTT_POS:
{
inregs.x.ax=0x03;
int86(MOUSE_INT,&inregs,&outregs);
*x=outregs.x.cx;
*y=outregs.x.dx;
*buttons=outregs.x.bx;
return(1);
}break;


case MOUSE_MOTION_REL:
{
inregs.x.ax=0x03;
int86(MOUSE_INT,&inregs,&outregs);
*x=outregs.x.cx;
*y=outregs.x.dx;
return(1);
}break;

case MOUSE_SET_SENSITIVITY:
{
inregs.x.ax=0x04;

inregs.x.bx=*buttons;
inregs.x.cx=*x;
inregs.x.dx=*y;

int86(MOUSE_INT,&inregs,&outregs);
return(1);
}break;


default:break;
}
}

void setcurshape(mscurstype mask)
{
  union REGS r;
  struct SREGS s;
  r.x.ax=9;
  r.x.bx=mask.hor;
  r.x.cx=mask.ver;
  r.x.dx=FP_OFF(&mask);
  s.es=FP_SEG(&mask);
  int86x(0x33,&r,&r,&s);
}

void mscurson(mscurstype shape)
{
int msvisible;
 union REGS r;
 struct SREGS s;
 setcurshape(shape);
 r.x.ax=1;
 msvisible=1;
 int86x(0x33,&r,&r,&s);
 }


typedef struct point
{
	int x;
	int y;
}point;


typedef struct windows
{
	int kind;
	point top;
	point move;
	char *word;
	int color;
	int bk_color;
	int status;
//	char *ico;
//	void (far *build)();
	char *hotkey;
	void (far *windows)(struct windows *win);
	char far *background;
	int bk_flag;
	struct windows *next;
	struct windows *father;
	int active;
} windows, *windows_ptr;

windows_ptr head;
windows_ptr now;
windows_ptr pre;
windows_ptr win_now;
windows_ptr last;
int menuflag=MIN;

typedef struct RGB_color_typ
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;
}RGB_color,*RGB_color_ptr;

unsigned char far *video_buffer=(char far *)0xA0000000L;
unsigned char far *rom_char_set=(char far *)0xF000FA6EL;

float sin_look[361];
float cos_look[361];

void men(windows_ptr win);


void Delay(int clicks)
{
unsigned int far *clock=(unsigned int far *)0x0000046CL;
unsigned int now;
now=*clock;
while(abs(*clock-now)<clicks){}
}

void Create_Tables(void)
{
int index;
for(index=0;index<=360;index++)
{
cos_look[index]=(float)cos((double)(index*3.14159/180));
sin_look[index]=(float)sin((double)(index*3.14159/180));
}
}

void Set_Palette_Register(int index,RGB_color_ptr color)
{
	outp(PALETTE_MASK,0xff);
	outp(PALETTE_REGISTER_WR,index);
	outp(PALETTE_DATA,color->red);
	outp(PALETTE_DATA,color->green);
	outp(PALETTE_DATA,color->blue);
}
void Get_Palette_Register(int index,RGB_color_ptr color)
{
	outp(PALETTE_MASK,0xff);
	outp(PALETTE_REGISTER_RD,index);
	color->red=inp(PALETTE_DATA);
	color->green=inp(PALETTE_DATA);
	color->blue=inp(PALETTE_DATA);

}

void Set_Video_Mode(int mode)
{
	union REGS inregs,outregs;
	inregs.h.ah=0;
	inregs.h.al=(unsigned char)mode;
	int86(0x10,&inregs,&outregs);
}

void Plot_Pixel_Fast(int x,int y,char color)
{
video_buffer[((y<<8)+(y<<6))+x]=color;
}

void BPlot_Pixel_Fast(int x,int y,char color)
{
video_buffer[((y<<8)+(y<<6))+x]=color;
video_buffer[((y<<8)+(y<<6))+x-1]=color;
video_buffer[((y<<8)+(y<<6))+x-320]=color;
video_buffer[((y<<8)+(y<<6))+x-321]=color;
}

void Bline(int x0,int y0,int x1,int y1,unsigned char color)
{
int dx,dy,x_inc,y_inc,error=0,index;
unsigned char far *vb_start=video_buffer;
vb_start=vb_start+((unsigned int )y0<<6)+((unsigned int)y0<<8)+(unsigned int)x0;
dx=x1-x0;
dy=y1-y0;
if(dx>=0)
{
x_inc=1;
}
else
{
x_inc=-1;
dx=-dx;
}
if(dy>=0)
{
y_inc=320;
}
else
{
y_inc=-320;
dy=-dy;
}
if(dx>dy)
{
for(index=0;index<=dx;index++)
{
*vb_start=color;
error+=dy;
if(error>dx)
{
error-=dx;
vb_start+=y_inc;
}
vb_start+=x_inc;
}
}
else
{
for(index=0;index<=dy;index++)
{
*vb_start=color;
error+=dx;
if(error>0)
{
error-=dy;
vb_start+=x_inc;
}
vb_start+=y_inc;
}
}
}

void H_Line(int x1,int x2,int y,unsigned int color)
{
	memset( (char far *) (video_buffer + ( (y<<8) + (y<<6) ) + x1) , color , x2-x1+1 );
}

void V_Line(int y1,int y2,int x,unsigned int color)
{
	int i;
	for(i=y1;i<y2;i++)
		video_buffer[ (i<<8) + (i<<6) + x]=color;
}


void Rectangle(int x1,int y1,int x2,int y2,int color)
{
	H_Line(x1,x2,y1,color);
	H_Line(x1,x2,y2,color);
	V_Line(y1,y2,x1,color);
	V_Line(y1,y2,x2,color);

}


void Fill_Rectangle(int x1,int y1,int x2,int y2,int color)
{
	int i;
	for(i=y1;i<=y2;i++)
	{
		H_Line(x1,x2,i,color);
	}
}


void Circle(int x,int y,int r,int color)
{
	int x0,y0,x1,y1,index;
	x0=y0=r;
	for(index=0;index<=360;index++)
	{
		x1=x0*cos_look[index]-y0*sin_look[index];
		y1=x0*sin_look[index]+y0*cos_look[index];
		Plot_Pixel_Fast(x+x1,y+y1,color);
	}

}
void BCircle(int x,int y,int r,int color)
{
	int x0,y0,x1,y1,index;
	x0=y0=r;
	for(index=0;index<=360;index++)
	{
		x1=x0*cos_look[index]-y0*sin_look[index];
		y1=x0*sin_look[index]+y0*cos_look[index];
		Plot_Pixel_Fast(x+x1,y+y1,color);
		Plot_Pixel_Fast(x+x1-1,y+y1,color);
		Plot_Pixel_Fast(x+x1,y+y1-1,color);
		Plot_Pixel_Fast(x+x1+1,y+y1,color);
		Plot_Pixel_Fast(x+x1,y+y1+1,color);
		Plot_Pixel_Fast(x+x1+1,y+y1,color);
		Plot_Pixel_Fast(x+x1-1,y+y1-1,color);
		Plot_Pixel_Fast(x+x1+1,y+y1-1,color);
		Plot_Pixel_Fast(x+x1+1,y+y1+1,color);
		Plot_Pixel_Fast(x+x1-1,y+y1+1,color);
	}

}

void Fill_Screen(int value)
{
_fmemset(video_buffer,(char)value,SCREEN_WIDTH*SCREEN_HEIGHT+1);
}

void Fill_Screen_Size(int value,int x0,int y0,int x1,int y1)
{
int x,y;
long i=0,height,width;
height=y1-y0+1;
width=x1-x0+1;
x=x0;
while(i<height*width)
{
	y=y0+i/width;
	i=i+width;
	_fmemset(video_buffer+x+y*SCREEN_WIDTH,(char)value,width);
}
}



void Blit_Char(int xc,int yc,char c,int color,int trans_flag)
{
int offset,x,y;
char far *work_char;
unsigned char bit_mask=0x80;
work_char=rom_char_set+c*CHAR_HEIGHT;
offset=(yc<<8)+(yc<<6)+xc;
for(y=0;y<CHAR_HEIGHT;y++)
{
bit_mask=0x80;
for(x=0;x<CHAR_WIDTH;x++)
{
if((*work_char&bit_mask))
video_buffer[offset+x]=color;
else if(!trans_flag)
video_buffer[offset+x]=0;
bit_mask=(bit_mask>>1);
}
offset+=SCREEN_WIDTH;
work_char++;
}
}


void Blit_String(int x,int y,int color,char *string,int trans_flag)
{
int index;
for(index=0;string[index]!=0;index++)
{
Blit_Char(x+(index<<3),y,string[index],color,trans_flag);
}
}

void Draw_Windows(windows_ptr win)
{
	switch(win->kind)
	{
		case WINDOWS:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,WINDOWS_COLOR_DARK);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,WINDOWS_COLOR_LIGHT);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,WINDOWS_COLOR_OUT);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+10,WINDOWS_COLOR_TITLE);
			if(win->active==ACTIVE)
				Blit_String(win->top.x+2,win->top.y+2,win->color,win->word,1);
			else
				Blit_String(win->top.x+2,win->top.y+2,NOACTIVE_COLOR,win->word,1);
			Fill_Rectangle(win->top.x+2,win->top.y+22,win->top.x+win->move.x-2,win->top.y+win->move.y-2,WINDOWS_COLOR_LIGHT);
			Fill_Rectangle(win->top.x+2,win->top.y+22,win->top.x+win->move.x-3,win->top.y+win->move.y-3,WINDOWS_COLOR_DARK);
			Fill_Rectangle(win->top.x+3,win->top.y+23,win->top.x+win->move.x-3,win->top.y+win->move.y-3,WINDOWS_COLOR_IN);

			break;
		}
		case BAR:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,WINDOWS_COLOR_DARK);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,WINDOWS_COLOR_LIGHT);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,WINDOWS_COLOR_OUT);
			if(win->active==ACTIVE)
			{
				Blit_String(win->top.x+79,win->top.y+3,WINDOWS_COLOR_LIGHT,win->word,1);
				Blit_String(win->top.x+80,win->top.y+4,WINDOWS_COLOR_DARK,win->word,1);
			}
			else
			{
				Blit_String(win->top.x+79,win->top.y+3,WINDOWS_COLOR_LIGHT,win->word,1);
				Blit_String(win->top.x+80,win->top.y+4,WINDOWS_COLOR_DARK,win->word,1);
			}
			break;
		}
		case BUTTON_MENU:
		{
			if(win->active==ACTIVE)
				Blit_String(win->top.x+2,win->top.y+1,win->color,win->word,1);
			else
				Blit_String(win->top.x+2,win->top.y+1,NOACTIVE_COLOR,win->word,1);
			break;
		}
		case BUTTON:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,BUTTON_COLOR_DARK);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR_LIGHT);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR);
			if(win->active==ACTIVE)
				Blit_String(win->top.x+2,win->top.y+1,win->color,win->word,1);
			else
				Blit_String(win->top.x+2,win->top.y+1,NOACTIVE_COLOR,win->word,1);
			break;
		}
		case BUTTON_PRESS:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,BUTTON_COLOR_LIGHT);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR_DARK);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR);
			Blit_String(win->top.x+2,win->top.y+1,win->color,win->word,1);
			break;
		}
		case BUTTON_CLOSE:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,BUTTON_COLOR_DARK);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR_LIGHT);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR);
			if(win->active==ACTIVE)
			{
				Bline(win->top.x+2,win->top.y+2,win->top.x+win->move.x-2,win->top.y+win->move.y-2,win->color);
				Bline(win->top.x+win->move.x-2,win->top.y+2,win->top.x+2,win->top.y+win->move.y-2,win->color);
			}
			else
			{
				Bline(win->top.x+2,win->top.y+2,win->top.x+win->move.x-2,win->top.y+win->move.y-2,NOACTIVE_COLOR);
				Bline(win->top.x+win->move.x-2,win->top.y+2,win->top.x+2,win->top.y+win->move.y-2,NOACTIVE_COLOR);
			}
			break;
		}
		case BUTTON_CLOSE_PRESS:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,BUTTON_COLOR_LIGHT);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR_DARK);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR);
			Bline(win->top.x+2,win->top.y+2,win->top.x+win->move.x-2,win->top.y+win->move.y-2,win->color);
			Bline(win->top.x+win->move.x-2,win->top.y+2,win->top.x+2,win->top.y+win->move.y-2,win->color);
			break;
		}
		case BUTTON_SMALL:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,BUTTON_COLOR_DARK);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR_LIGHT);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR);
			if(win->active==ACTIVE)
				H_Line(win->top.x+2,win->top.x+win->move.x-2,win->top.y+win->move.y-2,win->color);
			else
				H_Line(win->top.x+2,win->top.x+win->move.x-2,win->top.y+win->move.y-2,NOACTIVE_COLOR);
			break;
		}
		case BUTTON_SMALL_PRESS:
		{
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x,win->top.y+win->move.y,BUTTON_COLOR_LIGHT);
			Fill_Rectangle(win->top.x,win->top.y,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR_DARK);
			Fill_Rectangle(win->top.x+1,win->top.y+1,win->top.x+win->move.x-1,win->top.y+win->move.y-1,BUTTON_COLOR);
			H_Line(win->top.x+2,win->top.x+win->move.x-2,win->top.y+win->move.y-2,win->color);
			break;
		}
		default:break;
	}
}

void Object_Init(windows_ptr ptr,int kind,int x,int y,int move_x,int move_y,char *word,int color,int bk_color,int status,char *hotkey,int bk_flag,void (far *windows),int active)
{
	ptr->kind=kind;
	ptr->top.x=x;
	ptr->top.y=y;
	ptr->move.x=move_x;
	ptr->move.y=move_y;
	ptr->word=word;
	ptr->color=color;
	ptr->bk_color=bk_color;
	ptr->status=status;
	ptr->hotkey=hotkey;
	ptr->windows=windows;
	ptr->bk_flag=bk_flag;
	if(bk_flag==1)
		ptr->background=(char far *)malloc( (move_x+1)* (move_y+1)+1);
	ptr->active=active;
}


void Behind_Button(windows_ptr win)
{

char far *work_back;
int work_offset=0,offset,y;


work_back = win->background;


offset = (win->top.y << 8) + (win->top.y << 6) + win->top.x;

for (y=0; y<BUTTON_Y; y++)
    {

    _fmemmove((void far *)&work_back[work_offset],
	     (void far *)&video_buffer[offset],
	     BUTTON_X);


    offset      += SCREEN_WIDTH;
    work_offset += BUTTON_X;

    }

}

void Erase_Button(windows_ptr win)
{

char far *work_back;
int work_offset=0,offset,y;


work_back = win->background;


offset = (win->top.y << 8) + (win->top.y << 6) + win->top.x;

for (y=0; y<BUTTON_Y; y++)
    {

    _fmemmove((void far *)&video_buffer[offset],
	     (void far *)&work_back[work_offset],
	     BUTTON_X);


    offset      += SCREEN_WIDTH;
    work_offset += BUTTON_X;

    }

}
void Behind_Object(windows_ptr win)
{

char far *work_back;
int work_offset=0,offset,y;


work_back = win->background;


offset = (win->top.y << 8) + (win->top.y << 6) + win->top.x;

for (y=0; y<win->move.y+1; y++)
    {

    _fmemmove((void far *)&work_back[work_offset],
	     (void far *)&video_buffer[offset],
	     win->move.x+1);


    offset      += SCREEN_WIDTH;
    work_offset += win->move.x+1;

    }

}

void Erase_Object(windows_ptr win)
{

char far *work_back;
int work_offset=0,offset,y;


work_back = win->background;


offset = (win->top.y << 8) + (win->top.y << 6) + win->top.x;

for (y=0; y<win->move.y+1; y++)
    {

    _fmemmove((void far *)&video_buffer[offset],
	     (void far *)&work_back[work_offset],
	     win->move.x+1);


    offset      += SCREEN_WIDTH;
    work_offset += win->move.x+1;

    }

}

void Object_Delete(windows_ptr win)
{

	int index;

	if(win->bk_flag==1)
		free(win->background);
	free(&win);
}

void Free_Object(void)
{
	windows_ptr restore;
	restore=head;
	if(restore->bk_flag==1)
	{
		Erase_Object(restore);
		}
	Object_Delete(restore);
	do{
		restore=restore->next;
		if(restore->bk_flag==1)
		{
			Erase_Object(restore);
		}
		Object_Delete(restore);
	}while(restore->next!=NULL);
	head=NULL;

}


void win(windows_ptr win)
{
//	printf("\n\n\n\n\nwindows fuction is in use\n");
}

void clo(windows_ptr win)
{
	win->kind=BUTTON_CLOSE_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON_CLOSE;
	Draw_Windows(win);
	Delay(5);
	Free_Object();
	Set_Video_Mode(TEXT_MODE);
	exit(1);
}

void game()
{
	int color=YELLOW,key;
	Blit_String(0,30,4,"Press arrow key to move and 'q' to quit",1);
	Delay(40);
	Fill_Screen(0);
	while(key!='q')
	{
		Fill_Rectangle(xa*16,ya*10,xa*16+15,ya*10+9,color);
		switch(key=getch())
		{
			case 75:
			if(xa>0)
			{
				Fill_Rectangle(xa*16,ya*10,xa*16+15,ya*10+9,BLACK);
				xa=xa-1;
			}
			break;
			case 77:
			if(xa<19)
			{
				Fill_Rectangle(xa*16,ya*10,xa*16+15,ya*10+9,BLACK);
				xa=xa+1;
			}
			break;
			case 72:
			if(ya>0)
			{
				Fill_Rectangle(xa*16,ya*10,xa*16+15,ya*10+9,BLACK);
				ya=ya-1;
			}
			break;
			case 80:
			if(ya<19)
			{
				Fill_Rectangle(xa*16,ya*10,xa*16+15,ya*10+9,BLACK);
				ya=ya+1;
			}
			break;
		}
	}
}


void bt1(windows_ptr win)
{
	int x,y,fcolor;
	long i;
	win->kind=BUTTON_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON;
	Draw_Windows(win);
	Free_Object();
	xa=0;
	ya=0;
	game();
}

void Read_From_File(char flag,FILE *stream,int *word)
{
	int i,j=0,k=0;
	char *buf;
	if ((stream = fopen(stream, "r+b")) == NULL)
	{
		fprintf(stderr, "Cannot open output file.\n");
		return 1;
	}
	for(i=0;!feof(stream);i++)
	{
		fread(buf,1, 1, stream);
		if(buf[0]==flag)
		{
			fseek(stream,k, SEEK_SET);
			fread(buf,i-k+1,1,stream);
			buf[i-k]='\x0';
			word[j]=atoi(buf);
			j++;
			k=i+1;
		}
	}
	fclose(stream);
}



void loa(windows_ptr win)
{
	int x,y,fcolor;
	long i;
	int *word=malloc(sizeof(int)*2);
	win->kind=BUTTON_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON;
	Draw_Windows(win);
	Free_Object();
	Read_From_File(flag,stream,word);
	xa=word[0];
	ya=word[1];
	game();
}

void Write_To_File(char flag,FILE *stream,int *word)
{
	const char *xy;
	int i=0;
	if ((stream = fopen(stream, "w+b")) == NULL)
	{
		fprintf(stderr, "Cannot open output file.\n");
		return 1;
	}
	while(word[i]!=-1)
	{
	itoa(word[i],xy,10);
	fwrite(xy,strlen(xy), 1, stream);
	fwrite(",",1, 1, stream);
	i++;
	}
	fclose(stream);
}


void sav(windows_ptr win)
{
	int x,y,fcolor;
	long i;
	int *word=malloc(sizeof(int)*2);
	win->kind=BUTTON_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON;
	Draw_Windows(win);
	Free_Object();
	word[0]=xa;
	word[1]=ya;
	word[2]=-1;
	Write_To_File(flag,stream,word);
}

void bt2(windows_ptr win)
{
	win->kind=BUTTON_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON;
	Draw_Windows(win);
	Delay(5);
	Free_Object();
	Set_Video_Mode(TEXT_MODE);
	exit(1);
}

void sma(windows_ptr win)
{
	win->kind=BUTTON_SMALL_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON_SMALL;
	Draw_Windows(win);
	Free_Object();
}

void men(windows_ptr win)
{
	windows_ptr restore;
	win->kind=BUTTON_PRESS;
	Draw_Windows(win);
	Delay(5);
	win->kind=BUTTON;
	Draw_Windows(win);
	if(win->status==MIN)
	{
		now=(struct windows *)malloc(sizeof(struct windows));
		Object_Init(now,BUTTON,WINDOWS_X+2,WINDOWS_Y-BUTTON_Y*4,BUTTON_X-1,BUTTON_Y-1,"new",0,24,0,"",1,bt1,ACTIVE);
		now->father=win;
		Behind_Object(now);
		Draw_Windows(now);
		last->next=now;
		pre=now;
		now=(struct windows *)malloc(sizeof(struct windows));
		Object_Init(now,BUTTON,WINDOWS_X+2,WINDOWS_Y-BUTTON_Y*3,BUTTON_X-1,BUTTON_Y-1,"save",0,24,0,"",1,sav,ACTIVE);
		now->father=win;
		Behind_Object(now);
		Draw_Windows(now);
		pre->next=now;
		pre=now;
		now=(struct windows *)malloc(sizeof(struct windows));
		Object_Init(now,BUTTON,WINDOWS_X+2,WINDOWS_Y-BUTTON_Y*2,BUTTON_X-1,BUTTON_Y-1,"load",0,24,0,"",1,loa,ACTIVE);
		now->father=win;
		Behind_Object(now);
		Draw_Windows(now);
		pre->next=now;
		pre=now;
		now=(struct windows *)malloc(sizeof(struct windows));
		Object_Init(now,BUTTON,WINDOWS_X+2,WINDOWS_Y-BUTTON_Y,BUTTON_X-1,BUTTON_Y-1,"exit",0,24,0,"",1,bt2,ACTIVE);
		now->father=win;
		Behind_Object(now);
		Draw_Windows(now);
//		getch();
//		Erase_Button(now);
//		getch();
		pre->next=now;
		pre=now;
		pre->next=NULL;

		win->status=MAX;
		menuflag=MAX;
	}
	else if(win->status==MAX)
	{
		restore=head;
		do{
			restore=restore->next;
			if(restore->bk_flag==1)
			{
				Erase_Object(restore);
				Object_Delete(restore);
			}
		}while(restore->next!=NULL);
		last->next=NULL;
		win->status=MIN;
		menuflag=MIN;
	}
}

void Build_Object()
{
	now=pre=(struct windows *)malloc(sizeof(struct windows));
	Object_Init(now,BAR,WINDOWS_X,WINDOWS_Y,WINDOWS_X_MOVE,WINDOWS_Y_MOVE,"Move Rectangle",0,24,0,"",1,win,ACTIVE);
	Behind_Object(now);
	win_now=now;
	Draw_Windows(now);
	head=now;
	now=(struct windows *)malloc(sizeof(struct windows));
	Object_Init(now,BUTTON,WINDOWS_X+2,WINDOWS_Y+2,BUTTON_X-1,BUTTON_Y-1,"Star",0,24,MIN,"",0,men,ACTIVE);
	Draw_Windows(now);
	pre->next=now;
	pre=now;
	now=(struct windows *)malloc(sizeof(struct windows));
	Object_Init(now,BUTTON_SMALL,WINDOWS_X+WINDOWS_X_MOVE-(BUTTON_XY+2)*2,WINDOWS_Y+2,BUTTON_XY-1,BUTTON_XY-1,"",0,24,MAX,"",0,sma,ACTIVE);
	Draw_Windows(now);
	pre->next=now;
	pre=now;
	now=(struct windows *)malloc(sizeof(struct windows));
	Object_Init(now,BUTTON_CLOSE,WINDOWS_X+WINDOWS_X_MOVE-BUTTON_XY-2,WINDOWS_Y+2,BUTTON_XY-1,BUTTON_XY-1,"",0,24,0,"",0,clo,ACTIVE);
	Draw_Windows(now);
	pre->next=now;
	pre=now;
	pre->next=NULL;
	last=pre;

}



void Judge_Object(void)
{
	int xmouse,ymouse,btnmouse;
	static int btn=-1;
	windows_ptr test;
	Squeeze_Mouse(MOUSE_MOTION_REL,&xmouse,&ymouse,&btnmouse);
	if(ymouse==199&&head==NULL)
	{
		Squeeze_Mouse(MOUSE_HIDE,0,0,0);
		Build_Object();
		Squeeze_Mouse(MOUSE_SHOW,0,0,0);
	}
	if(ymouse<=185&&menuflag!=MAX&&head!=NULL)
	{
		Squeeze_Mouse(MOUSE_HIDE,0,0,0);
		Free_Object();
		Squeeze_Mouse(MOUSE_SHOW,0,0,0);
	}
	Squeeze_Mouse(MOUSE_BUTT_POS,&xmouse,&ymouse,&btnmouse);
	if(btnmouse==btn)
		return;
	else
		btn=btnmouse;
	xmouse=xmouse/2;
	ymouse=ymouse;
	if(btnmouse==1)
	{
		if(head!=NULL)
		{
		test=head;
		if((xmouse>=test->top.x&&xmouse<=test->top.x+test->move.x)&&
		(ymouse>=test->top.y&&ymouse<=test->top.y+test->move.y))
		{
			Squeeze_Mouse(MOUSE_HIDE,0,0,0);
			if(test->active==ACTIVE)
				test->windows(test);
			Squeeze_Mouse(MOUSE_SHOW,0,0,0);
		}
		do{
			test=test->next;
			if((xmouse>=test->top.x&&xmouse<=test->top.x+test->move.x)&&
			(ymouse>=test->top.y&&ymouse<=test->top.y+test->move.y))
			{
				Squeeze_Mouse(MOUSE_HIDE,0,0,0);
				if(test->active==ACTIVE)
					test->windows(test);
				Squeeze_Mouse(MOUSE_SHOW,0,0,0);
			}
		}while(test->next!=NULL);
		}
	}
}

void main(void)
{
	windows_ptr test;
	Set_Video_Mode(VGA256);
	Build_Object();
	Squeeze_Mouse(MOUSE_RESET,0,0,0);
	mscurson(handcurs);

	while(1){
		Judge_Object();
	}
}