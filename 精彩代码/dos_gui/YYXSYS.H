#ifndef __YYXSYS_H
#define __YYXSYS_H

#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <alloc.h>
#include <string.h>
#include <ctype.h>
#include <bios.h>
#include <dos.h>
#include <conio.h>
//#include <process.h>
//#include <math.h>
#include "n_graphi.h"

#include "keydef.h"

#define PI 3.1415926
#define TRUE_SECT	0
#define FALSE_SECT  1
#define CLIP_ON 1

#define TRUE 1
#define FALSE 0

#define NORMAL		0x00

#define DISABLE 	0x01		// status & DISBLE ---TRUE is non-zero
#define INVISIBLE   0x02

#define LINE_WIDTH 3

#define LOW(f) ((f)&0xff)
#define HIGH(f) (LOW(f>>8))
#define max(x,y) ((x>y)?x:y)
#define min(x,y) ((x<y)?x:y)

#define PEN_XOR (outp(0x3ce,3));(outp(0x3cf,0x18))
#define PEN_COPY (outp(0x3ce,3));(outp(0x3cf,0))

typedef unsigned char		BYTE;
typedef unsigned char 		BOOL;
typedef unsigned char 		bool;
typedef unsigned char 		byte;
typedef char far * 			LPSTR;
typedef char *		 		PSTR;

typedef struct Rect 		Trect;
typedef class win_class 	Twin;
typedef class menu_class 	Tmenu;
typedef class control_class	Tcontrol;
typedef class application_class Tapplication;


#include "mouse.h"

struct Rect {
	int		left;
	int 	top;
	int 	right;
	int 	bottom;
};
struct BitMap  {
	unsigned long int  size;
	struct	Rect bounds;
	void far *pbitmap;
};


unsigned  far RectSize (struct Rect& therect);
void FrameRect(struct Rect& therect);
void FillRect(struct Rect& therect,int color);
void PaintRect(struct Rect& therect, int color , int pen_mode );
void SetRect(struct Rect *r,int left,int top,int right,int bottom);
void OffsetRect(struct Rect *r,int dh,int dv);
void InsetRect(struct Rect *r,int dh,int dv);
void FrameOval (struct Rect& r );
void FillOval (struct Rect& r,int color);
void FrameRoundRect ( struct Rect& r , int ovalWidth, int ovalHeight );
void FillRoundRect (struct Rect& r ,int ovalWidth,int ovalHeight,int color );
int SectRect (struct Rect& rect1, struct Rect& rect2 , struct Rect *pdst_rect);
void far *myfarmalloc(unsigned long nbytes);
void far setport ( struct viewporttype& viewport);

BOOL PtInRect (int x,int y,struct Rect r);
BOOL Global2Local(int &x, int&y,const struct Rect& r);
BOOL Global2LocalPort(int &x, int&y,const struct viewporttype& port);
BOOL PtInPort (int x,int y,const struct viewporttype& port);

#ifdef __YYXMAIN
	struct viewporttype screen_viewport	 ;
	struct viewporttype win_living_viewport;
	int font_height;
	int bar_height;
	int alt_key[26]={
ALT_A   // 0x1e00
,ALT_B   //0x3000
,ALT_C   //0x2e00
,ALT_D   //0x2000
,ALT_E   //0x1200
,ALT_F   //0x2100
,ALT_G   //0x2200
,ALT_H   //0x2300
,ALT_I   //0x1700
,ALT_J   //0x2400
,ALT_K   //0x2500
,ALT_L   //0x2600
,ALT_M   //0x3200
,ALT_N   //0x3100
,ALT_O   //0x1800
,ALT_P   //0x1900
,ALT_Q   //0x1000
,ALT_R   //0x1300
,ALT_S   //0x1f00
,ALT_T   //0x1400
,ALT_U   //0x1600
,ALT_V   //0x2f00
,ALT_W   //0x1100
,ALT_X   //0x2d00
,ALT_Y   //0x1500
,ALT_Z   //0x2c00
};

#else
extern struct viewporttype screen_viewport  ;
extern struct viewporttype win_living_viewport;
extern int font_height;
extern int bar_height;
extern int alt_key[26];

#endif

#endif
