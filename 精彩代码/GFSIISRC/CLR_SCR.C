#include <dos.h>
#include <graphics.h>
#include "feedef.h"

/* function  : set cursor position at (x,y)
 * called by : this is a general-purposed routine
 * input     : (x,y) -- position
 * date      : 1993.9.21
 */
void GOTOxy(char x, char y)
{
    _AH=0x01; _CH=06; _CL=07;
     geninterrupt(0x10);          /* set the highth of cursor   */
    _AH=0x02; _BH=0;
    _AL=y;    _DH=_AL;
    _AL=x;    _DL=_AL;
    geninterrupt(0x10);           /* set the position of cursor */
}

/* function  : where is the X-position of cursor
 * called by : this is a general-purposed routine
 * output    : X-positon
 * date      : 1993.9.21
 */
int whereX(void)
{
    int x;
    _AH=3; _BH=0;
    geninterrupt(0x10);
    x=_DL;
    return x;
}

/* function  : where is the Y-position of cursor
 * called by : this is a general-purposed routine
 * output    : Y-position
 * date      : 1993.9.21
 */
int whereY(void)
{
    int y;
    _AH=3; _BH=0;
    geninterrupt(0x10);
    y=_DH;
    return y;
}

/* function  : clear the positioned part: (left,top)--(ritght,bottom),
 *             in graphics mode
 * called by : this is a general-purposed routine
 * input     : (left,top) -- (right,bottom)  zone to be cleared
 *             fore_c -- the foreground color
 *             back_c -- the background color
 * date      : 1993.9.21
 */
void clr_scr(UI left,UI top,UI right,UI bottom,UC fore_c,UC back_c)
{
    setfillstyle(SOLID_FILL,back_c);
    bar(left,top,right,bottom);
    setcolor(fore_c);

    return;
}

