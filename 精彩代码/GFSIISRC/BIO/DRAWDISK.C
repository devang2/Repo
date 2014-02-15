/* --------------------------------------------------------------------
	FILNAME:	draw_disk.c
	FUNCTION:	draw a disk, the size is selectable:
				the bigger or smaller.
	DATE:       25/4/93
	AUTHOR:		LIU GUANG YU
   --------------------------------------------------------------------- */
#include	<stdio.h>
#include 	<graphics.h>

#include	<def.inc>
#include	<func.inc>

/* ---------------------------------------------------------------------
	FUNCTION:		in the point draw disk
	CALLS:
	CALL BY:		Function NEW_init, or other programs.
	INPUT:			(xl, yl): 	The coordinates at which the diskette drew.
					coff:       1 -- draw the smaller one.
								2 -- draw the bigger one.
	OUTPUT:			None
	RETURN:			None
   ---------------------------------------------------------------------- */
void draw_disk(UI xl, UI yl, UC coff, UC disk_color, UC scr_color)
{
UI 		disk_cx, disk_cy;	/*  diskette size */
UI		x1, y1, x2, y2;		/*  diskette size */
UI		l1 = 132;           	/*  diskette size */
UI		c1 = 3 , d1 = 40;   	/*  diskette size */
UI		d2 = 28, d4 = 13;   	/*  diskette size */
UI		l2 = 30, l3 = 7;    	/*  diskette size */
UI      l5 = 25;	        	/*  diskette size */
UC		i;						/*  loop number	  */

	setcolor(12);
	setfillstyle(1, disk_color);
	bar(xl, yl, xl+coff*l1, yl+coff*l1);

	setfillstyle(1, scr_color);
	x1 = xl+(l1-c1)*coff;
	y1 = yl+l2*coff;
	x2 = x1+coff*c1;
	y2 = y1+coff*l3;
	bar(x1, y1, x2, y2);

	disk_cx = xl+l1*coff/2;
	disk_cy = yl+l1*coff/2;
	setcolor(6);
	circle(disk_cx, disk_cy, d2*coff/2);
	floodfill(disk_cx, disk_cy, 6);			/* here the back color is scr_color */

	circle(disk_cx, disk_cy, d1*coff/2);
	setfillstyle(1, 6);
	floodfill(disk_cx+d1*coff/2-3, disk_cy, 6);

	circle(disk_cx+25*coff, disk_cy+10*coff, 6*coff/2);
	floodfill(disk_cx+25*coff, disk_cy+10*coff, 6);

	disk_cy = yl+l1*coff-c1*coff-coff*d4/2-l5*coff;
	arc(disk_cx, disk_cy, 0, 180, coff*d4/2);
	arc(disk_cx, disk_cy+l5*coff, 180, 360, coff*d4/2);
	line(disk_cx-d4*coff/2, disk_cy, disk_cx-d4*coff/2, disk_cy+25*coff);
	line(disk_cx+d4*coff/2, disk_cy, disk_cx+d4*coff/2, disk_cy+25*coff);
	floodfill(disk_cx, disk_cy, 6);

	x1 = xl+3*coff;
	y1 = yl+3*coff;
	x2 = x1+32*coff;
	y2 = y1+8*coff;
	setfillstyle(1, 7);
	bar(x1, y1, x2, y2);
	y1 = y2;
	y2 = y2+24*coff;
	setfillstyle(1, 13);
	bar(x1, y1, x2, y2);

	setfillstyle(1, 7);
	x1 = xl+38*coff;
	y1 = yl+3*coff;
	x2 = x1+85*coff;
	y2 = y1+32*coff;
	bar(x1, y1, x2, y2);
	setcolor(13);
	setlinestyle(0, 0, 3);
	line(x1, y1+coff*5, x2, y1+coff*5);
	setlinestyle(0, 0, 0);
	for (i = 0; i<3; i++)
		line(x1, 15*coff+y1+5*i*coff, x2, 15*coff+y1+5*i*coff);
	setcolor(12);
	settextstyle(2, 0, 3*coff);
	outtextxy(xl+(3+4)*coff, yl+(3+8)*coff, "Maxell");
	if (coff!= 1)
	{
		hz16_disp(xl+(3+4)*coff-8, yl+(3+8)*coff+20, "»ªÎª´ÅÅÌ", 14);
		hz16_disp(x1+4*coff, yl+10*coff, "New_init.exe", 13);
	}
}

