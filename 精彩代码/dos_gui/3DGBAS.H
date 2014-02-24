#ifndef __GRAPHBAS_H
#define __GRAPHBAS_H

#include "yyxwin.h"

#define POINTNUM 22
#define SURFACENUM 10
#define PI 3.1415926

typedef class graphbas_class Tgraphbas;

class graphbas_class:public Twin {
public:
  float rho, theta, phi,d;
  float X,Y,Z;	// of viewport;
  float s1,c1,s2,c2;
  int cx,cy;

  float sv[POINTNUM][2];  // Point 's Projection Point;; 2 Dimension
  int n[SURFACENUM][3];	  // Direction Vector  of every surface
  int showornot[POINTNUM];

public:
	graphbas_class::graphbas_class (int ID,char *title,byte type,byte hotkey,
		 int left, int top, int width, int height );
	virtual	~graphbas_class ();
	virtual BOOL func_canclose();
	virtual void setup_window ();

	virtual int draw_win_contents();
	virtual int key_pressed_handler  ( int key_scan_num );
	virtual int msg_handler (MSG& message );

	void  calc();
	void show_surface (int sur);
	void draw_all_surface(int color);
};


#ifdef __YYXMAIN

  float v[POINTNUM][3] = {
	{5,7,-5},{5,7,5},{5,-7,5},{5,-7,-5},{-5,7,-5},
	{-5,-7,-5},{-5,-7,5},{-5,7,5},{0,7,8},{0,-7,8},

	{5,-1,-4},{5,1,-4},{5,-1,0}, {5,1,0},
	{5,-4,0}, {5,-3,0}, {5,-4,1}, {5,-3,1},
	{5, 3,0}, {5, 4,0}, {5, 3,1}, {5, 4,1},
  };	// Point Array  3 Dimension

  int nps[SURFACENUM]={5,6,5,6,5,5, 5,5,5,5}; //How many points per surface

  int s[SURFACENUM][6]={ {1,2,3,4,1,0}, {1,5,8,9,2,1}, {5,6,7,8,5,0},
		{4,3,10,7,6,4}, {3,2,9,10,3,0}, {7,10,9,8,7,0},	{1,4,6,5,1,0},
		{11,12,14,13,11,0},
		{15,16,18,17,15,0},
		{19,20,22,21,19,0}
		};	 // Every Surface's Points -- Right Rounded, clock wised

#else
extern float v[POINTNUM][3];
extern int nps[SURFACENUM];
extern int s[SURFACENUM][6];

#endif

#endif