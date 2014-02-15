//-----------------------------------------------------------
//                      GUI.HPP
//      Grpaphic Unit Interfact all right reserve
//        Write by TanWenHong 1996.12.19 Chen Du
//-----------------------------------------------------------
#ifndef _GUI_HPP
#define _GUI_HPP

#ifndef	_HZ_HPP
	#include "hz.hpp"
#endif
#ifndef __STRING_H
	#include <string.h>
#endif
#ifndef  __GRAPHICS_H
	#include <graphics.h>
#endif

#ifdef BGI_Path
#else
#define BGI_Path                "C:\\BORLANDC\\BGI"
#endif

#define MAXPG	8

#define		HZ_FONT		10


class IMG
{
	int xms_hd;
	FILE * disk_hd ;
	int pages ;
	unsigned maxpagesize ;
	struct IMGPG
	{
		int rx,ry;
		unsigned long pg_off ;
		unsigned pgsize;
	}imgpg[MAXPG];
public :
	IMG(){xms_hd = 0; disk_hd = 0;}
	~IMG(){ if(xms_hd) xms.FreeXmb(xms_hd);
				else fclose(disk_hd);}
	friend GUI;
};

class GUI
{
public :
	int gdriver , gmode, errorcode ;
	int font ;
public :
	GUI(int Graphdriver);
	~GUI(){	closegraph();	}
	void InitGraph();
	void SetColor(int color);
	void SetWriteMode(int ops);
	void PutPixel(int x,int y,int color);
	void Rectangle(int x,int y,int w,int h,int color);
	void Line(int x1,int y1,int x2,int y2,int color);
	void Box(int x,int y,int w,int h,int fillsytle, int color);
	void Outtext(int x,int y,char * fmt,...);
	void Outtext(char * fmt,...);
	void SetTextStyle(int font,int direction,int charSize);
	int TextWidth(char * textstring);
	int TextHeight(char * textstring);
	void GetImage(int x,int y,int w,int h,IMG * img);
	void PutImage(int x,int y,IMG *img,int op);
   void Rect3d(int x,int y,int w,int h,int in_out);
};

inline void GUI::SetWriteMode(int ops)
{
	setwritemode(ops);
}

inline GUI::GUI(int Graphdriver=DETECT)
{
	gdriver = Graphdriver ;
	GUI::InitGraph();
	font = HZ_FONT ;
}

inline void GUI::SetColor(int color)
{
	setcolor(color);
}
inline void GUI::PutPixel(int x,int y,int color)
{
	putpixel(x,y,color);
}

inline void GUI::Rectangle(int x,int y,int w,int h,int color)
{
	setcolor(color);
	rectangle(x,y,x+w,y+h);
}

inline void GUI::Line(int x1,int y1,int x2,int y2,int color)
{
	setcolor(color);
	line(x1,y1,x2,y2);
}

inline void GUI::Box(int x,int y,int w,int h,int fillstyle,int color)
{
	setfillstyle(fillstyle,color);
	bar(x,y,x+w,y+h);
}

inline int GUI::TextWidth(char * textstring)
{
	if(GUI::font == HZ_FONT) return(strlen(textstring)*8);
	else return(textwidth(textstring));
}

inline int GUI::TextHeight(char * textstring)
{
	if(GUI::font == HZ_FONT) return(16);
	else return(textheight(textstring));
}

inline void GUI::SetTextStyle(int font,int direction=VERT_DIR,int charSize=1)
{
	GUI::font = font ;
	if(font != HZ_FONT) settextstyle(font,direction,charSize);
}

extern GUI gr;
#endif