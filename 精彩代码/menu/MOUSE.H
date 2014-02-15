/* mouse header file required by window header file*/

/* In Case of any problem contact vaibhavk_@hotmail.com*/

/* to avoid ...........*/
#ifndef __MOUSE_H__
#define __MOUSE_H__
/* to avoid ...........*/

#include<dos.h>
/*initiliaze mouse it is used to check mouse driver*/
m()
{
	union REGS i,o;
	i.x.ax=0;
	int86(51,&i,&o);
	return (o.h.al);
}
/*show mouse cursor*/
showmp()
{
	union REGS i,o;
	i.x.ax=1;
	int86(51,&i,&o);
	return 0;
}
/*hide mouse cursor*/
hidemp()
{
	union REGS i,o;
	i.x.ax=2; int86(51,&i,&o);
	return 0;
}
/*restrict mouse to area x1,y1,x2,y2 */
restrictm(int x1,int y1,int x2,int y2)
{
	union REGS i,o;
	i.x.ax=7;
	i.x.cx=x1;
	i.x.dx=x2;
	int86(51,&i,&o);
	i.x.ax=8;
	i.x.cx=y1;
	i.x.dx=y2;
	int86(51,&i,&o);
	return 0;
}
/*get mouse status i.e b->button click & x,y->position*/
getmstat(int *b,int *x,int *y)
{
	union REGS i,o;
	i.x.ax=3;
	int86(51,&i,&o);
	*b=o.x.bx;
	*x=o.x.cx;
	*y=o.x.dx;
	return 0;
}
/*right click check*/
int rclick(void)
{
	int b,x,y;
	getmstat(&b,&x,&y);
	if(b==2) return 0;
	else return 1;
}
/*left click check*/
int lclick(void)
{
	int b,x,y;
	getmstat(&b,&x,&y);
	if(b==1) return 0;
	else return 1;
}
/*middle click check*/
int mclick(void)
{
	int b,x,y;
	getmstat(&b,&x,&y);
	if(b==3) return 0;
	else return 1;
}
#endif

/*main()
{
}*/