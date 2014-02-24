// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR  BY XuYongYong

/* 	yyxsys1.cpp
*/
#include "yyxsys1.h"

void get_title_hotkey(char *title_hotkey,byte& hotkey)
{   char title[80];
	int i,j;

	for (i=0,j=0; i<=strlen (title_hotkey); i++){
		if (title_hotkey[i] !='&' ) { title[j]=title_hotkey[i]; j++;}
		else hotkey=j;
	}
	strcpy (title_hotkey,title);
}

void draw_hotkey(char *title,byte hotkey,int title_pos_x,int title_pos_y)
{
        if (hotkey ==0xff) return;
  char tmpa[50];
	    strcpy(tmpa,title);
	    tmpa[hotkey+1]=0;
int width1,width2;
        width2=textwidth (tmpa);
        tmpa[hotkey]=0;
   		width1=textwidth (tmpa);
		line(title_pos_x +width1,
			 title_pos_y +textheight("U") +2,
			 title_pos_x +width2,
			 title_pos_y +textheight("U") +2 );
}


void draw_out_button (Rect therect)
{ Rect tmprect =therect;

	FillRect (tmprect ,LIGHTGRAY);
	setlinestyle ( SOLID_LINE,1,NORM_WIDTH );
	setcolor(BLACK);	FrameRoundRect (tmprect,2,2);
	InsetRect (&tmprect,-1,-1);
	tmprect.right -=LINE_WIDTH-2;
	tmprect.bottom -=LINE_WIDTH-2;
	setcolor ( WHITE);
	moveto(tmprect.left,tmprect.bottom );
	lineto(tmprect.left,tmprect.top);
	lineto(tmprect.right,tmprect.top);
	setlinestyle ( SOLID_LINE,1,THICK_WIDTH );
	setcolor (DARKGRAY );
	lineto(tmprect.right,tmprect.bottom);
	lineto(tmprect.left,tmprect.bottom);
	setlinestyle ( SOLID_LINE,1,NORM_WIDTH );
}

void draw_in_button (Rect therect)
{
	setcolor ( DARKGRAY);
	moveto(therect.left,therect.bottom );
	lineto(therect.left,therect.top);
	lineto(therect.right,therect.top);
	setcolor (WHITE );
	lineto(therect.right,therect.bottom);
	lineto(therect.left,therect.bottom);
}

void draw_win_frame_rect (const Rect& therect )
{ Rect temp_rect =therect;

	setlinestyle ( SOLID_LINE,1,NORM_WIDTH );
	setcolor (BLACK);
	FrameRect (temp_rect );
	setlinestyle ( SOLID_LINE,1,THICK_WIDTH );
	InsetRect(&temp_rect,-2,-2);
	setcolor (LIGHTGRAY);
	FrameRect (temp_rect);
	setlinestyle ( SOLID_LINE,1,NORM_WIDTH );
    setcolor (BLACK);
	InsetRect (&temp_rect,  -1 ,-1 );
	FrameRect (temp_rect);
	line(temp_rect.left,temp_rect.top+bar_height,temp_rect.left-LINE_WIDTH,temp_rect.top+bar_height );
	line(temp_rect.left+bar_height,temp_rect.top,temp_rect.left+bar_height,temp_rect.top-LINE_WIDTH );

	line(temp_rect.right,temp_rect.top+bar_height,temp_rect.right+LINE_WIDTH,temp_rect.top+bar_height );
	line(temp_rect.right-bar_height,temp_rect.top,temp_rect.right-bar_height,temp_rect.top-LINE_WIDTH );

	line(temp_rect.right,temp_rect.bottom-bar_height,temp_rect.right+LINE_WIDTH,temp_rect.bottom-bar_height );
	line(temp_rect.right-bar_height,temp_rect.bottom,temp_rect.right-bar_height,temp_rect.bottom+LINE_WIDTH );

	line(temp_rect.left,temp_rect.bottom-bar_height,temp_rect.left-LINE_WIDTH,temp_rect.bottom-bar_height );
	line(temp_rect.left+bar_height,temp_rect.bottom,temp_rect.left+bar_height,temp_rect.bottom+LINE_WIDTH );

	line(temp_rect.left,temp_rect.top+bar_height,temp_rect.right,temp_rect.top+bar_height);
}

void  draw_win_frame_rect_and_others(Rect& Arect,char *title,
		int color1,int color2 )
{
	draw_win_frame_rect (Arect);
Rect tmprect,tmp2rect;
//	SetRect (&tmp2rect,Arect.left,Arect.top,Arect.right,Arect.bottom);
	tmp2rect =Arect;
	InsetRect (&tmp2rect,-LINE_WIDTH,-LINE_WIDTH );
// draw sysmenu
	SetRect (&tmprect,tmp2rect.left,tmp2rect.top,tmp2rect.left+bar_height-1,
		tmp2rect.top+bar_height-1);
	FrameRect (tmprect);
	InsetRect (&tmprect ,-1,-1);
	FillRect (tmprect,LIGHTGRAY );
	setcolor (DARKGRAY);
	moveto
	(tmprect.left+5,tmprect.top);
  char minus[]="-";
	outtext (minus);

//  draw up & down rect;
  Rect up_rect,down_rect;
	down_rect =tmp2rect;
	down_rect.left =down_rect.right -bar_height+1;
	down_rect.right -=1;
	down_rect.bottom =down_rect.top+bar_height-1;
	OffsetRect (&down_rect,LINE_WIDTH-1,0 );
	up_rect=down_rect;
	OffsetRect (&up_rect, -bar_height+1,0 );

	draw_out_button (down_rect);
	draw_out_button (up_rect);
	draw_arrow (down_rect.left,down_rect.top,down_rect.right-LINE_WIDTH,
		down_rect.bottom,DOWN,BLACK);
	draw_arrow (up_rect.left,up_rect.top,up_rect.right-LINE_WIDTH,
		up_rect.bottom,UP,BLACK);

//draw text
    tmprect.top -=1;
	tmprect.left +=bar_height-1;
	tmprect.right =tmp2rect.right-2*bar_height+2;
	tmprect.bottom +=1;

	FillRect (tmprect,color1);
	setcolor (color2 );
	moveto ( tmprect.left +  ( tmprect.right-tmprect.left ) /2 -
				textwidth (title ) /2 ,
			 tmprect.top);
	outtext (title);
}

void draw_arrow ( int left, int top, int right, int bottom, int direct,int color )
{

	int xx [ 6 ];
	int le, to, ri, bo;

	int mini_size;
	int hlen, vlen;

	hlen = right - left;
	vlen = bottom - top;
	mini_size = ( hlen < vlen ) ? hlen : vlen;

	int base = mini_size / 6;

	int ox, oy;
	xx [ 0 ] = xx [ 2 ] = xx [ 4 ] = ox = ( right + left ) / 2;
	xx [ 1 ] = xx [ 3 ] = xx [ 5 ] = oy = ( top + bottom ) / 2;

	switch ( direct )
		{
		case 1:
			xx [ 1 ] -= base;
			xx [ 3 ] += base;
			xx [ 5 ] += base;
			xx [ 2 ] -= base;
			xx [ 2 ] -= base;
			xx [ 4 ] += base;
			xx [ 4 ] += base;
			break;
		case 2:
			xx [ 0 ] += base;
			xx [ 2 ] -= base;
			xx [ 4 ] -= base;
			xx [ 3 ] -= base;
			xx [ 3 ] -= base;
			xx [ 5 ] += base;
			xx [ 5 ] += base;
			break;
		case 3:
			xx [ 1 ] += base;
			xx [ 3 ] -= base;
			xx [ 5 ] -= base;
			xx [ 2 ] -= base;
			xx [ 2 ] -= base;
			xx [ 4 ] += base;
			xx [ 4 ] += base;
			break;
		default:
			xx [ 0 ] -= base;
			xx [ 2 ] += base;
			xx [ 4 ] += base;
			xx [ 3 ] -= base;
			xx [ 3 ] -= base;
			xx [ 5 ] += base;
			xx [ 5 ] += base;
		}

	setfillstyle ( SOLID_FILL, color );
	setlinestyle ( SOLID_LINE, 0x1, NORM_WIDTH );

	rectangle ( left, top, right, bottom );
	fillpoly ( 3, xx );
}


