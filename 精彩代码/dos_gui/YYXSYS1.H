#ifndef __YYXSYS1_H
#define __YYXSYS1_H

#include "yyxsys.h"

#include "yyxmsg.h"

#define RIGHT 2
#define LEFT  0
#define UP    1
#define DOWN  3

void get_title_hotkey(char *title_hotkey,byte& hotkey);
void draw_hotkey(char *title,byte hotkey,int title_pos_x,int title_pos_y);
void draw_out_button (Rect therect);
void draw_in_button (Rect therect);
void draw_win_frame_rect (const Rect& therect );
void  draw_win_frame_rect_and_others(Rect& Arect,char *title,
		int color1,int color2 );
void draw_arrow( int left, int top, int right, int bottom, int direct,int color );


#endif

