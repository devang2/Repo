// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	yyxsys.cpp
*/
#include "yyxsys.h"
#include "yyxmain.h"

unsigned  far RectSize (struct Rect& therect)
{      return ( imagesize(therect.left,therect.top,
		therect.right+2,therect.bottom+2));
} // for the frame rect

void FrameRect(struct Rect& therect)
{
	rectangle (therect.left ,therect.top, therect.right,therect.bottom);
}


void FillRect(struct Rect& therect,int color)
{       int i;

	setfillstyle (SOLID_FILL, color);
	bar (therect.left,therect.top, therect.right,therect.bottom );
}

void PaintRect(struct Rect& therect, int color , int pen_mode )
{
	if (pen_mode !=COPY_PUT) setwritemode (pen_mode);
	setfillstyle (SOLID_FILL,color );
	bar (therect.left,therect.top, therect.right,therect.bottom );
	if (pen_mode !=COPY_PUT)	setwritemode (COPY_PUT);
}


void SetRect(struct Rect *r,int left,int top,int right,int bottom)
{
	r->left =left;
	r->top =top;
	r->right=right;
	r->bottom =bottom;
}

void OffsetRect(struct Rect *r,int dh,int dv)
{   r->left +=dh;
	r->right +=dh;
	r->top +=dv;
	r->bottom +=dv;
}

void InsetRect(struct Rect *r,int dh,int dv)
{   r->left -=dh;
	r->right +=dh;
	r->top -=dv;
	r->bottom +=dv;
}

void FrameOval (struct Rect& r )
{
	ellipse( ( r.left+r.right ) /2 , (r.top+r.bottom ) /2 ,
		0,360, ( r.right-r.left )/2 ,( r.bottom - r.top ) /2 );
}

void FillOval (struct Rect& r,int color)
{
	setfillstyle (SOLID_FILL, color);
	fillellipse ( ( r.left+r.right ) /2 , (r.top+r.bottom ) /2 ,
		 ( r.right-r.left )/2 ,( r.bottom - r.top ) /2  );
}

void FrameRoundRect ( struct Rect& r , int ovalWidth, int ovalHeight )
{       struct Rect 	therect;
	int 		half_width ,half_height;

	half_width =ovalWidth /2;
	half_height=ovalHeight /2;

	ellipse( ( r.left+ half_width ) , (r.top + half_height )  ,
	      90,180, half_width, half_height);
	ellipse( ( r.left+ half_width ) , (r.bottom - half_height )  ,
	     180,270, half_width, half_height);
	ellipse( ( r.right- half_width ) , (r.top + half_height )  ,
			 0,90, half_width, half_height);
	ellipse( ( r.right - half_width ) , (r.bottom - half_height )  ,
			270,360, half_width, half_height);
	line (r.left+ half_width ,r.top ,r.right- half_width ,r.top );
	line (r.left+ half_width ,r.bottom ,r.right- half_width ,r.bottom );
	line (r.left ,r.top+half_height  ,r.left  ,r.bottom- half_height  );
	line (r.right ,r.top+half_height  ,r.right,r.bottom- half_height  );
}


void FillRoundRect (struct Rect& r ,int ovalWidth,int ovalHeight,int color )
{  	int 		half_width ,half_height;

	half_width =ovalWidth /2;
	half_height=ovalHeight /2;
	setfillstyle (SOLID_FILL, color);
	setcolor (color );
	bar (r.left+half_width, r.top, r.right-half_width, r.bottom );
	bar (r.left, r.top+half_height ,
		r.left+half_width , r.bottom-half_height );
	bar (r.right-half_width , r.top+half_height ,
		r.right , r.bottom-half_height );
	sector( ( r.left+ half_width ) , (r.top + half_height )  ,
				90,180,half_width, half_height);
	sector( ( r.left+ half_width ) , (r.bottom - half_height )  ,
				180,270,half_width, half_height);
	sector( ( r.right- half_width ) , (r.top + half_height )  ,
				0,90,half_width, half_height);
	sector( ( r.right - half_width ) , (r.bottom - half_height )  ,
				270,360,half_width, half_height);
}

int SectRect (struct Rect& rect1, struct Rect& rect2 , struct Rect *pdst_rect)
{
	if ( rect1.left > rect2.left ) pdst_rect->left =rect1.left;
						   else	   pdst_rect->left =rect2.left;
	if ( rect1.right< rect2.right) pdst_rect->right=rect1.right;
						   else	   pdst_rect->right=rect2.right;
	if ( rect1.top  > rect2.top )  pdst_rect->top  =rect1.top;
						   else	   pdst_rect->top  =rect2.top;
	if ( rect1.bottom< rect2.bottom ) pdst_rect->bottom =rect1.bottom;
						   else	   pdst_rect->bottom    =rect2.bottom;
	if (( pdst_rect->top >= pdst_rect->bottom ) ||
		( pdst_rect->left >= pdst_rect->right ) )	return (FALSE_SECT );
	else return ( TRUE_SECT );
}


void far *myfarmalloc(unsigned long nbytes)
{	void far *temp;
	
	temp =farmalloc(nbytes);
	if ( temp == NULL ) {
		error ("Sorry,no room");
// No need return a Value
	}	else return (temp );
	return FALSE;
}

void far setport (struct viewporttype& viewport)
{	setviewport(viewport.left,viewport.top,
		viewport.right,viewport.bottom,viewport.clip );
}

BOOL PtInRect (int x,int y,struct Rect r)
{
	return (
		(x>=r.left )&&(x<=r.right) &&
		(y>=r.top)&&(y<=r.bottom) );
}

BOOL PtInPort (int x,int y,const struct viewporttype& port)
{
	return (
		(x>=port.left )&&(x<=port.right) &&
		(y>=port.top)&&(y<=port.bottom) );
}

BOOL Global2Local(int &x, int&y,const struct Rect& r)
{
  BOOL ret_value=TRUE;
	if ((x<r.left )||(x>r.right) ||
		(y<r.top) || (y>r.bottom) ) return FALSE;
	x=x-r.left;
	y=y-r.top;
	return ret_value;
}

BOOL Global2LocalPort(int &x, int&y,const struct viewporttype& port)
//if !in port, do nothing
//else return TRUE; && change the values;
{ BOOL ret_value=TRUE;
	if ((x<port.left )||(x>port.right) ||
		(y<port.top) || (y>port.bottom) ) return FALSE;
	x=x-port.left;
	y=y-port.top;
	return ret_value;
}



