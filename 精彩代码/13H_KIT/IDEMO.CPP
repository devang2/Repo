// IDEMO.CPP

// **************************************************************************
// **************************************************************************
// demonstration program for features of Mode 13h Toolkit
// **************************************************************************
// author: Mark Betz, 76605,2346
//
// developed on: July 6, 1991
//
// last update:  July 6, 1991
// **************************************************************************
// **************************************************************************


#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <mem.h>
#include <stdlib.h>
#include "images.hpp"
#include "keyboard.hpp"

font small("csfont1.fnt");
pcx rehack("rehack.pcx");
pcx celtic("celtic.pcx");
pcx title("title.pcx");

unsigned _stklen = 8192;
p_rec default_pal;

void waitkey()
	{
	char c;
	extnd s;
	while (!kbhit());
	getkey(&c,&s);
	}

void opening_screen()
	{
	win *info;

	title.display(SoftFade);
	delay(1000);
	if (!(info = new win(75,65,175,75,NULL,4,5,11,9)))
		{
		settextmode();
		reporterr(MemErr,"win init: opening_screen()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"opening_screen()");
		return;
		}
	small.setstyle(0,0,2,trans,5);
	small.show(107,75,"Game Programmer's");
	small.show(97,85,"Mode 13h Image Toolkit");
	small.setstyle(4,0,2,trans,5);
	small.show(132,101,"Version 1.0");
	small.setstyle(0,0,1,trans,5);
	small.show(90,117,"Gamer's Programming Workshop");
	small.show(100,127,"GO GAMERS, section 11, CIS ");
	delay(5000);
	delete(info);
	}

void announce(char *str)
	{
	win *info;

	if (!(info = new win(30,50,260,40,NULL,4,5,11,9)))
		{
		settextmode();
		reporterr(MemErr,"win init: announce()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: announce()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(40,60,str);
	small.setstyle(4,0,1,trans,3);
	small.show(40,75,"Press any key...");
	waitkey();
	delete(info);
	}

void about_kit()
	{
	win *info;

	if (!(info = new win(45,60,230,110,NULL,4,5,11,9)))
		{
		settextmode();
		reporterr(MemErr,"win init: about_kit()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: about_kit()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(55,70,"The toolkit provides a set of functions for");
	small.show(55,80,"basic mode 13h graphics operations, including");
	small.show(55,90,"drawing primitives, windowing, text read and");
	small.show(55,100,"write, and image handling. The window, font,");
	small.show(55,110,"and image handling are provided through easy");
	small.show(55,120,"to use class definitions. Image operations");
	small.show(55,130,"include several different types of dissolves");
	small.show(55,140,"wipes and fades, like the one you just saw.");
	small.setstyle(4,0,1,trans,3);
	small.show(55,155,"Press any key...");
	waitkey();
	delete(info);
	announce("Let's take a look at some of the dissolve types...");
	}

void demo_dissolves()
	{
	title.remove(SoftFade);
	celtic.display(SoftFade);
	delay(500);
	announce("64 pass DAC palette fade (const SoftFade)");
	celtic.remove(SoftFade);
	rehack.display(SnapWipe);
	delay(500);
	announce("Simple block memory copy (const SnapWipe)");
	rehack.remove(SnapWipe);
	title.display(SplitVerticalWipe);
	delay(500);
	announce("Split vertical image wipe (const SplitVerticalWipe)");
	title.remove(SplitVerticalWipe);
	celtic.display(SplitHorizWipe);
	delay(500);
	announce("Split horizontal image wipe (const SplitHorizWipe)");
	celtic.remove(SplitHorizWipe);
	rehack.display(SlideVerticalWipe);
	delay(500);
	announce("Sliding vertical image wipe (const SlideVerticalWipe)");
	rehack.remove(SoftFade);
	title.display(SlideHorizWipe);
	delay(500);
	announce("Sliding horizontal image wipe (const SlideHorizWipe)");
	title.remove(SoftFade);
	celtic.display(VerticalDissolve);
	delay(500);
	announce("Vertical banded dissolve (const VerticalDissolve)");
	celtic.remove(VerticalDissolve);
	rehack.display(HorizDissolve);
	delay(500);
	announce("Horizontal banded dissolve (const HorizDissolve)");
	rehack.remove(SoftFade);
	clearscr(0);
	}

void do_bm_window()
	{
	win *info;
	void *map;
	FILE *bmap;

	if ((map = new char[15744]) == NULL)
		return;
	if ((bmap = fopen("winmap.bit","rb")) == NULL)
		{
		settextmode();
		reporterr(FileOpenErr,"do_bm_window()");
		delete(map);
		return;
		}
	if (fread(map,1,15744,bmap) != 15744)
		{
		settextmode();
		reporterr(FileReadErr,"do_bm_window()");
		delete(map);
		fclose(bmap);
		return;
		}
	fclose(bmap);
	if (!(info = new win(62,71,192,82,(char *)map,0,5,0,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: do_bm_window()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: do_bm_window()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(85,91,"The windows can also have a");
	small.show(85,101,"bitmap stored in system ram.");
	small.show(85,111,"This background is stored on");
	small.show(85,121,"disk as WINMAP.BIT.");
	small.setstyle(4,0,1,trans,3);
	small.show(85,136,"Press any key...");
	waitkey();
	delete(info);
	delete(map);
	rehack.remove(SplitVerticalWipe);
	return;
	}

void demo_window()
	{
	win *info;

	clearscr(155);
	if (!(info = new win(45,60,230,95,NULL,4,5,11,9)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_window()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_window()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(55,70,"The win class is a base class for windowing");
	small.show(55,80,"operations. It provides a foundation from");
	small.show(55,90,"which new window classes can be descended. A");
	small.show(55,100,"win class object saves it's background, and");
	small.show(55,110,"has definable attributes for shadow, border,");
	small.show(55,120,"background, size and position.");
	small.setstyle(4,0,1,trans,3);
	small.show(55,135,"Press any key...");
	fadepalettein(0,256,default_pal);
	waitkey();
	delete(info);
	if (!(info = new win(50,60,210,50,NULL,3,5,11,9)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_window()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_window()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(60,70,"200 x 75 window with 3 pixel blue border,");
	small.show(60,80,"and 5 pixel shadow.");
	small.setstyle(4,0,1,trans,3);
	small.show(60,95,"Press any key...");
	waitkey();
	delete(info);
	if (!(info = new win(50,60,210,50,NULL,0,0,11,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_window()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_window()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(60,70,"The same sized window with no border,");
	small.show(60,80,"and no shadow.");
	small.setstyle(4,0,1,trans,3);
	small.show(60,95,"Press any key...");
	waitkey();
	delete(info);
	rehack.display(VerticalDissolve);
	delay(500);
	do_bm_window();
	}

void demo_font()
	{
	win *info;
	char str[20];
	char ascii_mask = (BCASE|NUMBER|PUNCT);

	celtic.display(SoftFade);
	if (!(info = new win(30,50,260,80,NULL,0,5,255,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_font()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_font()");
		return;
		}
	small.setstyle(0,0,1,trans,3);
	small.show(40,60,"The font class provides a method for displaying");
	small.show(40,70,"and reading strings of text. The setstyle()");
	small.show(40,80,"method allows you to specify character spacing,");
	small.show(40,90,"space character width, fore and background");
	small.show(40,100,"color, and transparency.");
	small.setstyle(32,0,1,trans,3);
	small.show(40,115,"Press any key...");
	waitkey();
	delete(info);
	if (!(info = new win(30,50,260,40,NULL,0,5,255,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_font()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_font()");
		return;
		}
	small.setstyle(0,0,2,trans,3);
	small.show(40,60,"Transparent text with 2 pixel spacing...");
	small.setstyle(32,0,1,trans,3);
	small.show(40,75,"Press any key...");
	waitkey();
	delete(info);

	if (!(info = new win(30,50,260,40,NULL,0,5,255,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_font()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_font()");
		return;
		}
	small.setstyle(0,35,1,opaque,3);
	small.show(40,60,"Opaque text with 1 pixel spacing...");
	small.setstyle(32,0,1,trans,3);
	small.show(40,75,"Press any key...");
	waitkey();
	delete(info);

	if (!(info = new win(30,50,260,75,NULL,0,5,255,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_font()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_font()");
		return;
		}
	small.setstyle(0,255,1,trans,3);
	small.show(40,60,"The class also provides a readstr() method");
	small.show(40,70,"with a cursor and backspace enabled...");
	small.readstr(40,85,str,20,ascii_mask);
	small.show(40,100,str);
	small.setstyle(32,0,1,trans,3);
	small.show(40,115,"Press any key...");
	waitkey();
	delete(info);

	return;
	}


void demo_drawing()
	{
	int i;
	win *info;

	if (!(info = new win(10,10,260,50,NULL,0,5,255,0)))
		{
		settextmode();
		reporterr(MemErr,"win init: demo_font()");
		return;
		}
	if (info->status != NoErr)
		{
		settextmode();
		reporterr(info->status,"win init: demo_font()");
		return;
		}

	small.setstyle(0,0,1,trans,3);
	small.show(20,20,"The images toolkit also provides some basic drawing");
	small.show(20,30,"primitives...");
	small.setstyle(4,0,1,trans,3);
	small.show(20,45,"Press any key...");
	waitkey();

	small.setstyle(0,0,1,trans,3);
	clearscr(0);
	loadpalette(0,256,default_pal);
	clearscr(15);

	randomize();
	for(i = 0; i < 100; i++)
		{
		line(160,50,random(320),random(100),random(256));
		}
	small.show(10,102,"like line drawing, and...");
	randomize();
	barfill(10,125,130,155,random(256));
	barfill(30,130,200,160,random(256));
	barfill(40,140,250,165,random(256));
	barfill(50,150,300,150,random(256));

	small.show(10,170,"barfills, as well as pixel operations, getimage(),");
	small.show(10,180,"putimage(), and copyimage().");
	small.setstyle(4,0,1,trans,3);
	small.show(10,190,"Press any key...");
	waitkey();
	clearscr(0);
	return;
	}

void closing_screen()
	{
	rehack.display(SoftFade);
	small.setstyle(0,0,1,trans,3);
	small.show(40,10,"The Mode 13h Images Toolkit is free for your use and");
	small.show(40,20,"modification, subject to the guidelines layed out");
	small.show(40,30,"in the accompanying source code and documentation.");
	small.show(40,40,"We ask only that, if you change it, make it better,");
	small.show(40,50,"and let us have a copy of your changes, so that we");
	small.show(40,60,"can keep improving it.");
	small.setstyle(14,0,1,trans,3);
	small.show(40,130,"This toolkit is provided courtesy of the Gamer's");
	small.show(40,140,"Programming Workshop, !GO GAMERS, section 11, CIS.");
	small.setstyle(4,0,1,trans,3);
	small.show(10,190,"Press any key...");
	waitkey();
	}

int main() {
	char result;

	if (!small.installed()) {
		printf("error installing font small\n");
		return(1);
	}
	setgraphmode();
	readpalette(0,256,default_pal);

	if (rehack.getstatus() != NoErr)
		{
		reporterr(rehack.getstatus(),"pcx constructor");
		return(1);
		}
	if (celtic.getstatus() != NoErr)
		{
		reporterr(celtic.getstatus(),"pcx constructor");
		return(1);
		}
	if (title.getstatus() != NoErr)
		{
		reporterr(title.getstatus(),"pcx constructor");
		return(1);
		}

	result = rehack.load(Bestfit);
	if ((result != NoErr) && (result != MemErr))
		reporterr(result,"function main()");
	result = celtic.load(Bestfit);
	if ((result != NoErr) && (result != MemErr))
		reporterr(result,"function main()");
	result = title.load(Bestfit);
	if ((result != NoErr) && (result != MemErr))
		reporterr(result,"function main()");

	opening_screen();
	about_kit();
	demo_dissolves();
	demo_window();
	demo_font();
	demo_drawing();
	closing_screen();
	settextmode();
	return(0);
}




