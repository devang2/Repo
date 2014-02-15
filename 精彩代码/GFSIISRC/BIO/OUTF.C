/**************************************************************

   Function:   outf(UI posi_x, UI posi_y, UC back_color, UC fore_color,
				  const char *format, ...)
   Note:
		This function displays a formated string in Chinese mode just like
		the printf() function in English mode.
   Parameters:
		posi_x,      x coordination for displaying the string in
					 Chinese mode (graphic mode)
		posi_y,      y coordination for displaying
		back_color,  background color for displaying
		fore_color,  foreground color for displayint
		*format,     format string, followed by a variable number of
					 arguments
   Return:
		none

   Designer:	Yu Chuan
   Date    :    Dec. 1992

 **************************************************************/

#include <stdio.h>
#include <stdarg.h>

#include <def.inc>
#include <func.inc>

void outf(UI posi_x, UI posi_y, UC back_color, UC fore_color,
		  const UC *format, ...)
{
	UC buffer[80];
	va_list argptr;

	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	disp_str(posi_x, posi_y, buffer, back_color, fore_color);
}
