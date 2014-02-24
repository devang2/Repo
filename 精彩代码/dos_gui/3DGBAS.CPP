#include "msgbox.h"
#include "yyxmain.h"
#include "editdlg.h"
#include "filedial.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include "3dgbas.h"
#include "3dmenu.h"

graphbas_class::graphbas_class (int ID,char *title,byte type,byte hotkey,
	 int left, int top, int width, int height )
	:win_class (ID,title,type,hotkey,left,top,width,height)
{
	rho =40; theta =0; phi = PI/2 ; d =450;
	viewport.clip =1 ; // CLIP_ON;
	setup_window();  // may result in a duplicate
}

void graphbas_class::setup_window ()
{	win_class::setup_window();

	cx 	=( bounds.right-bounds.left-25) /2 ;
	cy 	=( bounds.bottom-bounds.top-25) /2;
//	ix1=25, iy1=25, ix2=bounds.right-bounds.left-25, iy2=bounds.bottom-bounds.top-90;   // bounds.top
	calc();
}

graphbas_class::~graphbas_class ()
{
}

int graphbas_class::key_pressed_handler  ( int key_scan_num )
{

  int ret_value;
	if ((ret_value=win_class::key_pressed_handler (key_scan_num )) !=FALSE)
		return ret_value;

	draw_all_surface (BLACK);

	switch(key_scan_num )	{
		case F1KEY:rho *=2;  break;
		case F2KEY:rho /=2;  break;
		case LEFTKEY:theta +=PI/8;break;
		case RIGHTKEY:theta -=PI/8;break;
		case UPKEY:phi +=PI/8;break;
		case DOWNKEY:phi -=PI/8;break;
		case '7':d   *=2;break;
		case '8':d   /=2;break;
	}

	calc();
	draw_all_surface (GREEN);
	return FALSE;
}


int graphbas_class::msg_handler (MSG& message )
{
	if (win_class::msg_handler (message) !=FALSE ) return TRUE;
	switch ( message .Action){
		case MenuActionMSG:
			switch (message.ID ){
				case LEFT: key_pressed_handler  ( LEFTKEY);break;
				case RIGHT:key_pressed_handler  ( RIGHTKEY);break;
				case UP:   key_pressed_handler  (   UPKEY);break;
				case DOWN: key_pressed_handler  ( DOWNKEY);break;
				case NEAR: key_pressed_handler  (   F2KEY);break;
				case FAR : key_pressed_handler  (   F1KEY);break;

				case DISTANCE: break;
  Teditdlg * modifydlg;
  char buf[30];
				case X_VALUE:
					sprintf (buf,"%4.1f",X);
					modifydlg=new Teditdlg(0,"INPUT X VALUE",150,100,300,200,buf);
					if (theprogram->exec_dialog(modifydlg) ==Dlg_OK )  {
						X = atof ( buf );

						rho = sqrt (X*X+Y*Y+Z*Z);
						theta = acos ( Z/ rho);
						phi   = acos ( X/ sqrt(X*X+Y*Y));
						calc ();
						draw_win_contents();
					};
					break;
				case Y_VALUE:
					sprintf (buf,"%4.1f",Y);
					modifydlg=new Teditdlg(0,"INPUT Y VALUE",150,100,300,200,buf);
					if (theprogram->exec_dialog(modifydlg) ==Dlg_OK )  {
						Y = atof ( buf );

						rho = sqrt (X*X+Y*Y+Z*Z);
						theta = acos ( Z/ rho);
						phi   = acos ( X/ sqrt(X*X+Y*Y));
						calc ();
						draw_win_contents();
					};
					break;
				case Z_VALUE:
					sprintf (buf,"%4.1f",Z);
					modifydlg=new Teditdlg(0,"INPUT Z VALUE",150,100,300,200,buf);
					if (theprogram->exec_dialog(modifydlg) ==Dlg_OK )  {
						Z = atof ( buf );

						rho = sqrt (X*X+Y*Y+Z*Z);
						theta = acos ( Z/ rho);
						phi   = acos ( X/ sqrt(X*X+Y*Y));
						calc ();
						draw_win_contents();
					};
					break;
			};
			break;
		default: return FALSE;
	}

	return TRUE;
}


int graphbas_class::draw_win_contents()
{
	getviewsettings (&current_viewport );
	setport (viewport);
/*	setviewport (viewport.left,
		viewport.top,
		viewport.right,
		viewport.bottom-2*bar_height, 1);
*/
	setfillstyle(SOLID_FILL,BLACK);
	bar (0,0,viewport.right-viewport.left,
		viewport.bottom-viewport.top-bar_height);

	putch (0x007);
//	calc ();
	draw_all_surface(GREEN);

	setport (current_viewport );
	return 0;
}

void graphbas_class::draw_all_surface(int color)
{
  int i;
	setcolor (color);
	for (i=0;i<SURFACENUM;i++) {
	   if (	showornot[i]==1)	show_surface (i);
	}
  char buf[100];

	sprintf (buf,"X=%4.1f,Y=%4.1f,Z=%4.1f,rho=%4.1f,theta=%4.1f,phi=%4.1f",
		X,Y,Z,rho,theta,phi);
	outtextxy ( bounds.left +10,bounds.bottom-bar_height-30, buf );
}

void graphbas_class::show_surface (int sur)
{
  int x1,y1,x2,y2;
  int i;
//	setcolor ( GREEN );
	for (i=0;i<nps[sur]-1;i++) {
		x1=sv[s[sur][i]-1][0];
		y1=sv[s[sur][i]-1][1];
		x2=sv[s[sur][i+1]-1][0];
		y2=sv[s[sur][i+1]-1][1];
		line (x1,y1,x2,y2);
	}
}

void graphbas_class::calc()
{ int i;

//	rho =40; theta =0.7; phi = 1; d =450;
//	cx = 320; cy=240;
	s1 =sin (theta ); c1 =cos (theta );
	s2 =sin (phi  );  c2 =cos (phi );

	X=rho * s1 * c2;
	Y=rho* s1 * s2;
	Z=rho* c1;

	for (i=0;i<POINTNUM;i++ ) {
	  float x,y,z;
	  float x1,y1,z1;
		x=v[i][0]; y=v[i][1]; z=v[i][2];
		x1=-x*s1+y*c1;
		y1=-x*c1*c2-y*s1*c2+z*s2;
		z1=-x*s2*c1-y*s1*s2-z*c2+ rho;
		sv[i][0]=d*(x1/z1)+cx;
		sv[i][1]=-d*(y1/z1)+cy;

	}

  float u1,u2,u3,v1,v2,v3;
  float xe,ye,ze;
	for (i=0;i<SURFACENUM;i++){
		u1=v[s[i][1]-1][0]-v[s[i][0]-1][0];
		u2=v[s[i][1]-1][1]-v[s[i][0]-1][1];
		u3=v[s[i][1]-1][2]-v[s[i][0]-1][2];
		v1=v[s[i][2]-1][0]-v[s[i][0]-1][0];
		v2=v[s[i][2]-1][1]-v[s[i][0]-1][1];
		v3=v[s[i][2]-1][2]-v[s[i][0]-1][2];
		n[i][0]=u2*v3-v2*u3;
		n[i][1]=u3*v1-v3*u1;
		n[i][2]=u1*v2-v1*u2;
	}

	xe=rho*s2*c1;
	ye=rho*s2*s1;
	ze=rho*c2;

  float e2,wx,wy,wz;
	for (i=0;i<SURFACENUM;i++) {
		e2=s[i][0]-1;
		wx =xe-v[e2][0];
		wy =ye-v[e2][1];
		wz =ze-v[e2][2];
		if ( n[i][0]*wx + n[i][1]*wy+n[i][2]*wz <=0 ) {
			showornot[i]=0;
			// Do not show it;
		} else {
			showornot[i]=1;
//			show_surface (i);
			// Show the surface;
		}
	}
}


BOOL graphbas_class::func_canclose()
{
	return TRUE;
}

