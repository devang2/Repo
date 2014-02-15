/*Hi These graphics funcions are simple to use and i think it is usefull for
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

/* this is a test program to show power of window.h library file*/
#include"mouse.h"
#include"window.h"

int b,x,y,t,r1=0,r2=0,i=211,j=389,f=0,i1=0,j1=10,c=1,i2=0,c1=0,bs,xs,ys;
int connct=0,dc=57,hi1=0;
char s[50],*s1;

main()
{
	g();m();
	init();
	getmstat(&b,&x,&y);
	showmp();
	while(1)
	{
		if(click(200,300,300,340,"Ok")==0) break;
		if(click(320,300,420,340,"Cancel")==0){closegraph(); exit(0);}

	}

	showmp();

	getmstat(&bs,&xs,&ys);

	setfillstyle(1,WHITE);
	bar(400+1,20+1,620-1,40-1);
	outtextxy(402,32,s1);
	a:desktop();
	a1:while(!kbhit())
	{
		if(click(10,445,65,465,"START")==0)
		{
			menu();
			while(!kbhit())
			{
				getmstat(&b,&x,&y);
				if((x>150 || y<100) && b==1)
				{
					normal(10,100,150,438,dc);
					break;
				}
				if((x>35 && x<150) && (y>400 && y<440))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,410,148,435);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,420,"Shut down...");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>400 && y<440))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							hidemp();
							quitwin();
							showmp();
							while(!kbhit())
							{
								if(click(200,290,270,330,"Yes")==0)
								{
									closegraph();
									exit(0);
								}
								if(click(300,290,370,330,"No")==0)
								{
									hidemp();
									normal(160,150,450,350,dc);
									showmp();
									break;
								}
							}
						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,410,148,435);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,420,"Shut down...");
					showmp();
				}
				if((x>35 && x<150) && (y>360 && y<400))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,370,148,395);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,380,"Command1");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>360 && y<400))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							getmstat(&b,&x,&y);
							while(b==1)
							{
								getmstat(&b,&x,&y);
							}
							goto a;
						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,370,148,395);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,380,"Command1");
					showmp();
				}
				if((x>35 && x<150) && (y>320 && y<360))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,330,148,355);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,340,"Command2");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>320 && y<360))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							getmstat(&b,&x,&y);
							while(b==1)
							{
								getmstat(&b,&x,&y);
							}
						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,330,148,355);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,340,"Command2");
					showmp();
				}

				if((x>35 && x<150) && (y>280 && y<320))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,290,148,315);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,300,"Command3");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>280 && y<320))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							getmstat(&b,&x,&y);
							while(b==1)
							{
								getmstat(&b,&x,&y);
							}

						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,290,148,315);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,300,"Command3");
					showmp();
				}

				if((x>35 && x<150) && (y>240 && y<280))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,250,148,275);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,260,"Command4");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>240 && y<280))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							getmstat(&b,&x,&y);
							while(b==1)
							{
								getmstat(&b,&x,&y);
							}

						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,250,148,275);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,260,"Command4");
					showmp();
				}

				if((x>35 && x<150) && (y>200 && y<240))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,210,148,235);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,220,"Command5");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>200 && y<240))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							getmstat(&b,&x,&y);
							while(b==1)
							{
								getmstat(&b,&x,&y);
							}

						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,210,148,235);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,220,"Command5");
					showmp();
				}

				if((x>35 && x<150) && (y>160 && y<200))
				{
					hidemp();
					setfillstyle(1,1);
					bar(35,170,148,195);
					settextstyle(0,0,0);
					setcolor(WHITE);
					outtextxy(40,180,"Command6");
					showmp();
					getmstat(&b,&x,&y);
					while((x>35 && x<150) && (y>160 && y<200))
					{
						getmstat(&b,&x,&y);
						if(b==1)
						{
							getmstat(&b,&x,&y);
							while(b==1)
							{
								getmstat(&b,&x,&y);
							}
						}
					}
					hidemp();
					setfillstyle(1,7);
					bar(35,170,148,195);
					settextstyle(0,0,0);
					setcolor(0);
					outtextxy(40,180,"Command6");
					showmp();
				}
			}
		}
		if(up(80,445,200,465,"screen saver")==0){ goto a;}
		if(up(220,445,340,465,"Refresh")==0) goto a;
	}
	return 0;
}
menu()
{
	unpress(10,100,150,438);

	lined(12,400,148,400);
	lined(12,160,148,160);

	setfillstyle(1,1);
	bar(11,101,31,438);

	settextstyle(0,1,1);
	outtextxy(27,200,"MY WINDOWS 2002 HA HA..");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,420,"Shut down...");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,380,"Command1");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,340,"Command2");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,300,"Command3");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,260,"Command4");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,220,"Command5");

	settextstyle(0,0,0);
	setcolor(0);
	outtextxy(40,180,"Command6");
	return 0;
}
init()
{
	hidemp();
	unpress(10,10,630,470);
	setcolor(60);
	settextstyle(0,0,2);

	win(150,100,450,350);
	button(200,300,300,340,"OK");
	button(320,300,420,340,"Cancle");
	hidemp();
	outtextxy(160,150," This is intial screen which is  ");
	outtextxy(160,200," called as splash screen by vb ");
	outtextxy(160,250," programmers hardly 10 lines of code");

	showmp();
	return 0;
}
quitwin()
{
	hidemp();
	win(160,150,450,350);
	showmp();
	settextstyle(7,0,1);
	outtextxy(200,200,"You really want to quit");
	button(200,290,270,330,"Yes");
	button(300,290,370,330,"No");
	return 0;
}
desktop()
{
	setlinestyle(0,1,1);
	hidemp();
	setfillstyle(1,57);
	bar(10,10,630,470);

	unpress(10,50,630,80);
	setcolor(60);
	settextstyle(0,0,2);
	outtextxy(50,60,"WELCOME TO WORLD OF C GRAPHICS");

	unpress(10,440,630,470);
	button(10,445,65,465,"START");

	press(560,445,625,465);

	lined(550,445,550,465);
	lined(70,445,70,465);

	setcolor(0);
	settextstyle(0,0,0);
	outtextxy(92,455,"screen saver");
	outtextxy(230,455,"Refresh");

	showmp();
	return 0;
}
