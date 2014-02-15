/*
Hi These graphics funcions are simple to use and i think it is usefull for
c gaphics lovers if u find these usefull do contact me i have total 300 more
such usefull function which will change your imagination of c graphics. I am
building a dos based web browser so that by using a single bootable floppy
u can access internet and in these browser i am using all such functions
listed below, beleive me a web browser can be built very easily with c language
if u have knowledge of tcp/ip. Problem i am facing is converting byte codes of
java to display on my browser if u know internals of byte code do help me
i will send u source code of my browser
contact: vaibhavk_@hotmail.com
		 vaibhavk@nagpur.dot.net.in

if u didnt get any of function ask me any time i will tell u how they works
and how to use them

see program test.c to see the output of these functions

// joke:
// which will be biggest ironic death of these century ?
// BILL GATES FALLS OUT OF WINDOW

After seeing output u can easily understand how Bill Gates
must have developed WINDOWS atleast interface of windows
its hardly matter of 300 lines to get a desktop like windows os have
the problem is in developing kernel
If u know kernel development plz help me out so that a bootable browser system
can be built for embeded systems on which i am working these days
i am using a 8051 microcontroller and parser developed by me to convert
c programs to instrucions of 8051 problem here is DOS cant be used on 8051
based systems so internet is not accessible

if intersted in my project contact me

lastly copyright: plz read it i know no one follows it but still read it

never use these functions to make a commericial projects
if made then send me source code of it
else
if u modify these function then also send me source code of it

and if u use these in any case the keep these message as it is
DONT REMOVE THESE MESSAGE FROM HEADER FILE FOR SAKE OF FREE FOUNDATION
DEVELOPMENT

Bye

*/
#include"mouse.h"
#include<graphics.h>
/*------------------simple yet power full graphic funtions-----------------*/

/*short cut to initiliaze graphics*/
g()
{
	union REGS i,o;
	int gd,gm;
	detectgraph(&gd,&gm);
	/* bgi driver in current dir */
	initgraph(&gd,&gm,"");
	return 0;
}

/*unpressed effect of button with coordinates*/
unpress(int x1,int y1,int x2,int y2)
{
	setlinestyle(0,1,1);
	setfillstyle(1,7);
	bar(x1,y1,x2,y2);
	setcolor(WHITE);
	line(x1,y1,x2,y1);
	line(x1,y1,x1,y2);
	setcolor(0);
	line(x1,y2,x2,y2);
	line(x2,y1,x2,y2);
	return 0;
}

/*pressed effect of button with coordinates*/
press(int x1,int y1,int x2,int y2)
{
	setlinestyle(0,1,1);
	setfillstyle(1,7);
	bar(x1,y1,x2,y2);
	setcolor(0);
	line(x1,y1,x2,y1);
	line(x1,y1,x1,y2);
	setcolor(WHITE);
	line(x1,y2,x2,y2);
	line(x2,y1,x2,y2);
	return 0;
}

/*draw a typical window of win9.x paltform*/
win(int x1,int y1,int x2,int y2)
{
	setlinestyle(0,1,1);
	hidemp();

	setfillstyle(1,7);
	bar(x1,y1,x2,y2);

	setcolor(WHITE);
	line(x1,y1,x2,y1);
	line(x1,y1,x1,y2);

	setcolor(0);
	line(x1,y2,x2,y2);
	line(x2,y1,x2,y2);

	setfillstyle(1,1);
	bar(x1+1,y1+1,x2-1,y1+20);

	showmp();
	return 0;
}

/*line down*/
lined(int x1,int y1,int x2,int y2)
{
	setlinestyle(0,1,1);
	setcolor(0);
	line(x1,y1,x2,y2);
	setcolor(WHITE);
	line(x1+1,y1+1,x2+1,y2+1);
	return 0;
}


/*draw a typical button*/
button(int x1,int y1,int x2,int y2,char s[30])
{
	int t1,t2,k=0,i;
	setlinestyle(0,1,1);
	for(i=0;i<strlen(s);i++)
		k=k+4;
	t1=(x2-x1)/2+x1-k;
	t2=(y2-y1)/2+y1;
	unpress(x1,y1,x2,y2);
	settextstyle(0,0,0);
	outtextxy(t1,t2,s);
	showmp();
	return 0;
}

/*accepts cooridinates returns 0 on success i.e on click it is a event*/
click(int x1,int y1,int x2,int y2,char s[30])
{
	int b,x,y;
	int t1,t2,k=0,i,j;
	setlinestyle(0,1,1);
	for(i=0;i<strlen(s);i++)
		k=k+4;
	t1=(x2-x1)/2+x1-k;
	t2=(y2-y1)/2+y1;
	getmstat(&b,&x,&y);
	if( (x>x1 && x<x2) && (y>y1 && y<y2) && b==1)
	{
		hidemp();
		press(x1,y1,x2,y2);
		setcolor(0);
		settextstyle(0,0,0);
		outtextxy(t1+2,t2+2,s);
		showmp();
		while((b==1))
		{
			getmstat(&b,&x,&y);
		}
		hidemp();
		unpress(x1,y1,x2,y2);
		settextstyle(0,0,0);
		outtextxy(t1,t2,s);

		for(i=50;i<500;i=i+50)
		{
			delay(10);
			sound(i+200);
		}
		nosound();
		showmp();
		return 0;
	}
	else return 1;
}
/*up effect of a button click action event return 0 if click event occurs*/
up(int x1,int y1,int x2,int y2,char s[30])
{
	int b,x,y;
	int t1,t2,k=0,i;
	setlinestyle(0,1,1);
	for(i=0;i<strlen(s);i++)
		k=k+4;
	t1=(x2-x1)/2+x1-k;
	t2=(y2-y1)/2+y1;
	getmstat(&b,&x,&y);
	if( (x>x1 && x<x2) && (y>y1 && y<y2) )
	{
		hidemp();
		unpress(x1,y1,x2,y2);
		setcolor(0);
		settextstyle(0,0,0);
		outtextxy(t1,t2,s);
		showmp();
		while( (x>x1 && x<x2) && (y>y1 && y<y2))
		{
			getmstat(&b,&x,&y);
			if(click(x1,y1,x2,y2,s)==0)
			{
				hidemp();
				normal(x1,y1,x2,y,7);
				outtextxy(t1,t2,s);
				showmp();
				return 0;
			}
		}
		hidemp();
		normal(x1,y1,x2,y2,7);
		outtextxy(t1,t2,s);
		showmp();
	}return 1;
}

/*remove drawn thing with background colors*/
normal(int x1,int y1,int x2,int y2,int c)
{
	setlinestyle(0,1,1);
	setfillstyle(1,c);
	hidemp();
	bar(x1-1,y1-1,x2+1,y2+1);
	showmp();
	return 0;
}
