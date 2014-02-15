/******************************************************************/
/*   FILENAME: MBASIC.C                                           */
/*   LAST_MODI: 1993-3-2                                          */
/*   FUNCTION : draw_key(,,,,,,,)                                 */
/*              chang_key(,,,,,,,)                                */
/*              chang_key_border(,,,,,)                           */
/*              write_key_date(,,,,,,,)                           */
/*              draw_back(,,,,,,)                                 */
/******************************************************************/

#include <graphics.h>
#include <string.h>
#include <alloc.h>

#include <def.inc>
#include <func.inc>

static struct
	{
		UC *buff;
		UI back_sx;
		UI back_sy;
	}Pop_dat[4];
static UC Pop_num = 0;

void draw_key(UI left, UI top, UI right, UI bottom, UC *string, UC bc, UC cc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setfillstyle(SOLID_FILL, bc);
	bar(left+2, top+2, right-2, bottom-2);
	for(i=1;i<2;i++)
	{
		setcolor(15);
		line(left+i,top+i,right-i,top+i);
		line(left+i,top+i,left+i,bottom-i);
		setcolor(6);
		line(left+i,bottom-i,right-i,bottom-i);
		line(right-i,top+i,right-i,bottom-i);
	}
	setcolor(0);
	rectangle(left,top,right,bottom);
	hz16_disp( ( right+left+1-(strlen(string)<<3) )>>1,
			   (top+bottom-15)>>1, string, cc);
}

void chang_key(UI left, UI top, UI right, UI bottom, UC *string, UC cc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setfillstyle(SOLID_FILL, 6);
	bar(left+3, top+3, right-3, bottom-3);

	for(i=1;i<3;i++)
	{
		setcolor(0);
		line(left+i, top+i, right-i, top+i);
		line(left+i, top+i, left+i, bottom-i);
		setcolor(15);
		line(left+i, bottom-i, right-i, bottom-i);
		line(right-i, top+i, right-i, bottom-i);
	}
	setcolor(0);
	rectangle(left, top, right, bottom);
	hz16_disp( ( right+left+1-(strlen(string)<<3) )>>1,
			   (top+bottom-15)>>1, string, cc);
}

void chang_key1(UI left, UI top, UI right, UI bottom, UC *string, UC cc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setfillstyle(SOLID_FILL, 10);
	bar(left+3, top+3, right-3, bottom-3);

	for(i=1;i<3;i++)
	{
		setcolor(0);
		line(left+i, top+i, right-i, top+i);
		line(left+i, top+i, left+i, bottom-i);
		setcolor(15);
		line(left+i, bottom-i, right-i, bottom-i);
		line(right-i, top+i, right-i, bottom-i);
	}
	setcolor(0);
	rectangle(left, top, right, bottom);
	hz16_disp( ( right+left+1-(strlen(string)<<3) )>>1,
			   (top+bottom-15)>>1, string, cc);
}

void chang_key3(UI left, UI top, UI right, UI bottom, UC *string, UC cc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	setfillstyle(SOLID_FILL, 2);
	bar(left+3, top+3, right-3, bottom-3);

	for(i=1;i<3;i++)
	{
		setcolor(0);
		line(left+i, top+i, right-i, top+i);
		line(left+i, top+i, left+i, bottom-i);
		setcolor(15);
		line(left+i, bottom-i, right-i, bottom-i);
		line(right-i, top+i, right-i, bottom-i);
	}
	setcolor(0);
	rectangle(left, top, right, bottom);
	hz16_disp( ( right+left+1-(strlen(string)<<3) )>>1,
			   (top+bottom-15)>>1, string, cc);
}

/* -------------------------- draw_key2() ----------------------------- */
void draw_key2(UI left, UI top, UI right, UI bottom, UC *string, UC cc)
{

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);

	setcolor  (15);
	line (left, top, right, top);
	line (left, top, left, bottom);
	setcolor  (6);
	line (left+1, bottom, right, bottom);
	line (right, top+1, right, bottom);

	hz16_disp( ( right+left+1-(strlen(string)<<3) )>>1,
			   (top+bottom-15)>>1, string, cc);
}

/* --------------------------- change_key2() --------------------------- */
void chang_key2(UI left, UI top, UI right, UI bottom, UC *string, UC cc)
{

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);

	setcolor  (6);
	line (left, top, right, top);
	line (left, top, left, bottom);
	setcolor  (15);
	line (left+1, bottom, right, bottom);
	line (right, top+1, right, bottom);

	hz16_disp( ( right+left+1-(strlen(string)<<3) )>>1,
			   (top+bottom-15)>>1, string, cc);
}


void chang_key_border(UI left, UI top, UI right, UI bottom, UC pressed)
{
UI i;
UC color1, color2;


	if(pressed)
	{
		color1 = 6;
		color2 = 15;
	}
	else
	{
		color1 = 15;
		color2 = 6;
	}

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	for(i=1;i<3;i++)
	{
		setcolor(color1);
		line(left+i,top+i,right-i,top+i);
		line(left+i,top+i,left+i,bottom-i);
		setcolor(color2);
		line(left+i,bottom-i,right-i,bottom-i);
		line(right-i,top+i,right-i,bottom-i);
	}
	setcolor(0);
	rectangle(left,top,right,bottom);
}


void write_key_data(UI left, UI top, UI right, UI bottom, UC string[80], UC cc)
{
	hz16_disp( ( right+left+1-(strlen(string)<<3))>>1,
				(top+bottom-15)>>1, string, cc);
}

void draw_back(UI left, UI top, UI right, UI bottom, UC bkc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	for(i=0; i<2; i++)
	{
		setcolor(15);
		line(left+i,bottom-i,right-i,bottom-i);
		line(right-i,top+i,right-i,bottom-i);
		setcolor(6);
		line(left+i,top+i,right-i,top+i);
		line(left+i,top+i,left+i,bottom-i);
	}
	setfillstyle(SOLID_FILL,bkc);
	bar(left+1, top+1, right-1, bottom-1);
}

void draw_back1(UI left, UI top, UI right, UI bottom, UC bkc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	for(i=1; i<2; i++)
	{
		setcolor(15);
		line(left+i,top+i,right-i,top+i);
		line(left+i,top+i,left+i,bottom-i);
		setcolor(6);
		line(left+i,bottom-i,right-i,bottom-i);
		line(right-i,top+i,right-i,bottom-i);
	}
	rectangle(left,top,right,bottom);

	setfillstyle(SOLID_FILL,bkc);
	bar(left+2, top+2, right-2, bottom-2);
}


/* --------------------------- draw_back2() ----------------------------- */
void draw_back2(UI left, UI top, UI right, UI bottom, UC bkc)
{
	rectangle(left, top, right, bottom);

	setfillstyle(SOLID_FILL, bkc);
	bar(left, top, right, bottom);
	setcolor  (6);
	rectangle (left+1, top+1, right, bottom);
	setcolor  (15);
	rectangle (left, top, right-1, bottom-1);
}
/*
void draw_back3(UI left, UI top, UI right, UI bottom, UC bkc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);

	setcolor(6);
	line(left,top,right,top);
	line(left,top,left,bottom);
	line(left-1,bottom-1,right-1,bottom-1);
	line(right-1,top-1,right-1,bottom-1);
	setcolor(15);
	line(left,bottom,right,bottom);
	line(right,top,right,bottom);
	line(left+1,top+1,right-1,top+1);
	line(left+1,top+1,left+1,bottom-1);

	setfillstyle(SOLID_FILL,bkc);
	bar(left+2, top+2, right-2, bottom-2);
}
*/
/*void draw_back3(UI left, UI top, UI right, UI bottom, UC bkc)
{
UI i;

	setlinestyle(SOLID_LINE, 0, NORM_WIDTH);
	for(i=0; i<2; i++)
	{
		setcolor(6);
		line(left+i,bottom-i,right-i,bottom-i);
		line(right-i,top+i,right-i,bottom-i);
		setcolor(15);
		line(left+i,top+i,right-i,top+i);
		line(left+i,top+i,left+i,bottom-i);
	}
	setfillstyle(SOLID_FILL,bkc);
	bar(left+1, top+1, right-1, bottom-1);
}
*/

void pop_back(UI left, UI top, UI right, UI bottom, UC bkc)
{
        UI size;

	if (Pop_num>=4)
		exit_scr(1,"\n\rMBASIC.C/pop_back(): Can't pop more than 4 back window.");

	size = imagesize(left, top, right, bottom);
	if (size == 0xFFFF)
		exit_scr(1, "\n\rMBASIC.C/pop_back(): Window size must less than 64Kb.");
	Pop_dat[Pop_num].buff = (UC *)mem_alloc( size );
	getimage(left,top,right,bottom,Pop_dat[Pop_num].buff);
	draw_back1(left, top, right, bottom, bkc);
	Pop_dat[Pop_num].back_sx = left;
	Pop_dat[Pop_num].back_sy = top;
	Pop_num++;
}

void rid_pop(void)
{
	if (Pop_num!=0)
	{
		Pop_num--;
		putimage(Pop_dat[Pop_num].back_sx, Pop_dat[Pop_num].back_sy,
				 Pop_dat[Pop_num].buff, COPY_PUT);
		farfree(Pop_dat[Pop_num].buff);
	}
}

