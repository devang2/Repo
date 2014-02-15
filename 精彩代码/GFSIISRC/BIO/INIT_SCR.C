#include <alloc.h>
#include <stdio.h>
#include <stdarg.h>
#include <process.h>
#include <graphics.h>

#include <def.inc>
#include <func.inc>
#include <menu_def.inc>

static UC graphics_stat = FALSE;
extern HMENU *Menu_stru;
extern UC    *Menu[MAX_NODE];

void init_scr(void)
{

int     graphdriver = DETECT;
int     graphmode;
int     errorcode;
UI      max_x, max_y;

  initgraph( &graphdriver, &graphmode, "" );
  errorcode = graphresult();
  if( errorcode != grOk )
		exit_scr(1,"\n\rINIT_SCR.C/init_scr(): Graphics System Error: %s", grapherrormsg(errorcode));

  max_x = getmaxx();
  max_y = getmaxy();
  if ( max_x<639 || max_y<479)
		exit_scr(1,"\n\rINIT_SCR.C/init_scr(): Graphics System Error: NOT A EGA/VGA ADAPTOR.");

  graphics_stat = TRUE;
/*
  setpalette(2,17);
  setpalette(3,33);
  setpalette(5,49);
  setpalette(6,8);
  setpalette(8,0);
  setpalette(11,35);
  setpalette(12,36);
  setpalette(13,24);
  setpalette(14,54);
*/
		setpalette(2,24);
		setpalette(3,46);
		setpalette(5,49);
		setpalette(6,56);
		setpalette(8,0);
		setpalette(11,35);
		setpalette(12,36);
		setpalette(13,17);
		setpalette(14,54);

}

void exit_scr(UI code, const UC *format, ...)
{
int i;
UC buffer[80];
va_list argptr;

		if(Menu_stru != 0)
		    mem_free(Menu_stru);

		for(i=0;i<MAX_NODE;i++)
		{
		    if(Menu[i] != 0)
			mem_free(Menu[i]);
		    else
			break;
		}

		if (graphics_stat)
		    closegraph();

		va_start(argptr, format);
		vsprintf(buffer, format, argptr);
		va_end(argptr);

		puts(buffer);

		unload_timer();
/*      exit_mail();*/
		exit(code);
}

void exit_scrm(UI code, const UC *format, ...)
{
int i;
UC buffer[80];
va_list argptr;

		if(Menu_stru != 0)
		    mem_free(Menu_stru);

		for(i=0;i<MAX_NODE;i++)
		{
		    if(Menu[i] != 0)
			mem_free(Menu[i]);
		    else
			break;
		}

		if (graphics_stat)
		    closegraph();

		va_start(argptr, format);
		vsprintf(buffer, format, argptr);
		va_end(argptr);

		puts(buffer);

		unload_timer();
/*      exit_mail();*/
		return;
}
