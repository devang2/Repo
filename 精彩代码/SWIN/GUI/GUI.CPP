//-----------------------------------------------------------
//                      GUI.CPP
//      Grpaphic Unit Interfact all right reserve
//        Write by TanWenHong 1996.12.19 Chen Du
//-----------------------------------------------------------
#ifndef	_GUI_HPP
	#include "gui.hpp"
#endif
#ifndef	__STDIO_H
	#include <stdio.h>
#endif
#ifndef	__STDLIB_H
	#include <stdlib.h>
#endif
#ifndef	__STDARG_H
	#include <stdarg.h>
#endif

#ifdef BGI_Path
	#undef BGI_Path
#endif

#define BGI_Path                "..\\gui"

GUI gr;

void GUI::InitGraph()
{
	initgraph(&gdriver, &gmode, BGI_Path);

	/* read result of initialization */
	errorcode = graphresult() ;
	if (errorcode != grOk)  /* an error occurred */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		exit(1);             /* return with error code */
	}
}

void GUI::Outtext(int x,int y,char * fmt,...)
{
	char * buffer= new char[255] ;
	va_list argptr;

	va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);
	va_end(argptr);
	if(GUI::font == HZ_FONT)
		hz.Outtext(x,y,buffer);
	else outtextxy(x,y,buffer);
	delete buffer ;
}

void GUI::Outtext(char * fmt,...)
{
	char * buffer = new char[255] ;
	va_list argptr;

	va_start(argptr, fmt);
	vsprintf(buffer, fmt, argptr);
	va_end(argptr);
	if(GUI::font == HZ_FONT)
		hz.Outtext(buffer);
	else outtextxy(hz.Getx(),hz.Gety(),buffer);
	delete buffer ;
}


void GUI::GetImage(int x,int y,int w,int h,IMG * img)
{
//	if(img->xms_hd) xms.FreeXmb(img->xms_hd);
	if((x+w)>639) w = 639 - x ;
	if((y+h)>479) h = 479 - y ;
	unsigned pgSize = 0xffff;
	unsigned resSize = 0;
	unsigned pgh = h;
	unsigned pgs = 0;
	unsigned long imgSize = 0;
	while(pgSize == 0xffff){
		pgs++ ;
		pgh = h/pgs;		                           //maxpage height
		pgSize = imagesize(x,y,x+w,y+pgh-1);			//maxpagesize
	}
	pgSize = (pgSize+1)&0xfffe ;
	if(pgs*pgh<=h)
		resSize = (imagesize(x,y+pgs*pgh,x+w,y+h)+1)&0xfffe; //Rest image size
	imgSize = (long)pgSize*pgs + resSize ;						//total image size
	char * imgBuff = new char[pgSize];
	if(!imgBuff){
		printf("No enough memory for getimage");
		abort();
	}
	img->xms_hd = xms.AllocXmb(imgSize/1024+1);
	if(!img->xms_hd){
		img->disk_hd = tmpfile();
		if(!img->disk_hd){
			printf("Can't open temp file.");
			abort();
		}
	}
	unsigned long pg_off = 0L ;					//xms offset

	img->maxpagesize = pgSize ;
	img->pages = pgs ;

	for(int i = 0;i<pgs ;i++){
		img->imgpg[i].ry = i * pgh ;
		img->imgpg[i].pg_off = pg_off ;
		img->imgpg[i].pgsize = pgSize ;
		getimage(x,y+img->imgpg[i].ry,x+w,y+img->imgpg[i].ry+pgh-1,imgBuff);
		if(img->xms_hd){
			if(!xms.CmbMovetoXmb(imgBuff,img->xms_hd,pg_off,pgSize)){
				printf("GetImage:[%d],%u,%s",i,pgSize,xms.ErrorMess());
				abort();
			}
		} else 	fwrite(imgBuff,1,pgSize,img->disk_hd);
		pg_off += pgSize ;
	}
	if(pgs*pgh <= h){
		img->pages ++ ;
		img->imgpg[i].ry = i*pgh ;
		img->imgpg[i].pg_off = pg_off ;
		img->imgpg[i].pgsize = resSize ;
		getimage(x,y+img->imgpg[i].ry,x+w,y+h,imgBuff);
		if(img->xms_hd)
			xms.CmbMovetoXmb(imgBuff,img->xms_hd,pg_off,resSize);
		else fwrite(imgBuff,1,resSize,img->disk_hd);
	}
	delete imgBuff ;
}

void GUI::PutImage(int x,int y,IMG * img,int op)
{
	char far * imgBuff = new char[img->maxpagesize];
	if(!imgBuff){
		printf("PutImage : Memory not enough\n");
		exit(0);
	}
	if(!img->xms_hd){
		fseek(img->disk_hd,0,SEEK_SET);
	}
	for(int i=0 ;i<img->pages;i++){
		if(img->xms_hd){
			if(!xms.XmbMovetoCmb(img->xms_hd,img->imgpg[i].pg_off,
									imgBuff,img->imgpg[i].pgsize))
			{
				printf("Put:imgpg[%d],ry=%d,pg_off=%lx,pgsize=%u\n",
					i,img->imgpg[i].ry,img->imgpg[i].pg_off,img->imgpg[i].pgsize);
				abort();
			}
		} else {
			fread(imgBuff,1,img->imgpg[i].pgsize,img->disk_hd);
		}
		putimage(x,y+img->imgpg[i].ry,imgBuff,op);
	}
	delete imgBuff ;
}

void GUI::Rect3d(int wx,int wy,int ww,int wh,int out3d)
{
	out3d?setcolor(BLACK):setcolor(WHITE);
	moveto(wx,wy+wh);
	lineto(wx+ww,wy+wh);					//Draw _
	lineto(wx+ww,wy);                //Draw  |

	//Draw |~
	setcolor(DARKGRAY);
	lineto(wx,wy);							//Draw  ~
	lineto(wx,wy+wh);                //Draw |

	//Draw _|
	setcolor(DARKGRAY);
	moveto(wx+ww-1,wy+1);
	lineto(wx+ww-1,wy+wh-1);
	lineto(wx+1,wy+wh-1);

	//Draw |~
	out3d?setcolor(WHITE):setcolor(BLACK);
	lineto(wx+1,wy+1);
	lineto(wx+ww-1,wy+1);
}