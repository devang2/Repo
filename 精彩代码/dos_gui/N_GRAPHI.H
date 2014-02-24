#ifndef __N_GRAPHI_H
#define __N_GRAPHI_H

#define mouse_on  if(sysmouse)sysmouse->on(1)
#define mouse_off if(sysmouse)sysmouse->off(1)

//void mouse_on(int restoreflagflag);
//void mouse_off(int tmpflag);

#define line(x1,y1,x2,y2) mouse_off;line(x1,y1,x2,y2);mouse_on
#define linerel(dx,dy) mouse_off;linerel(dx,dy);mouse_on
#define lineto(x,y) mouse_off;lineto(x,y);mouse_on

#define ellipse(x,y,stangle,endangle,xradius,yradius)  mouse_off;ellipse(x,y,stangle,endangle,xradius,yradius);mouse_on
#define fillellipse(x,y,xradius,yradius)	mouse_off;fillellipse(x,y,xradius,yradius);mouse_on
#define sector(x,y,stangle,endangle,xradius,yradius)  mouse_off;sector(x,y,stangle,endangle,xradius,yradius);mouse_on

#define putimage(left,top,bitmap,op) mouse_off;putimage(left,top,bitmap,op);mouse_on
#define getimage(left,top,right,bottom,bitmap) mouse_off;getimage(left,top,right,bottom,bitmap);mouse_on
#define bar(left,top,right,bottom)  mouse_off;bar((left),(top),(right),(bottom));mouse_on
#define rectangle(left,top,right,bottom)  mouse_off;rectangle((left),(top),(right),(bottom));mouse_on

#define outtext(textstring)  	mouse_off;outtext(textstring);mouse_on
#define outtextxy(x,y,textstring) 	mouse_off;outtextxy((x),(y),(textstring));mouse_on

#define fillpoly(numpoints,polypoints) mouse_off;fillpoly(numpoints,polypoints);mouse_on

#endif

