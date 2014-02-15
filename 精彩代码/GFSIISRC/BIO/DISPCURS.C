#include	<graphics.h>

#include	<DEF.INC>
#include	<KEY.INC>
#include	<FUNC.INC>

#define		BASE_DATA 			5000

/* --------------------------------------------------------------------
	FUNCTION:	Display a flash cursor at (start_x, start_y);
				the cursor color is specified by bk_color and cursor_color.
				the cursor size is 2 lines thick and 8 dots long.
	CALLS:
	CALLED BY:	This is an general-purposed routine.
	INPUT:		(start_x, start_y) -- the position where the cursor displayed.
				bk_color -- the background color.
				cursor_color -- the cursor_color.
	OUTPUT:		None.
	RETURN:		None.
   --------------------------------------------------------------------- */
void disp_cursor(UI start_x, UI start_y, UC bk_color, UC cursor_color)
{
UC color;
static UI count = 0;
static UC dark = 0;

	count = ++count%BASE_DATA;
	if (count==0)
	{
		if (dark)
			color = cursor_color;
		else
			color = bk_color;
		setcolor(color);
		line(start_x,   start_y, start_x+7, start_y);
		line(start_x, start_y+1, start_x+7, start_y+1);
		dark = !dark;
	}
}

