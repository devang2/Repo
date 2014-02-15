#include <alloc.h>

#include "\vga\vgabase.h"
#include "\vga\vgamouse.h"

MouseMapHigh::MouseMapHigh(VGABASE *v) : MouseMap(v)
{
wide=14;
high=19;
centre_X=0;
centre_Y=0;
xy=(int *)calloc(10,2);
setup();
}

MouseMapHigh::~MouseMapHigh()
{
free(xy);
}

void MouseMapHigh::draw(int x0,int y0)
{
static int xyo[]={0,0, 0,19, 6,12, 14,14, 0,0};
int i;
for(i=0;i<10; i+=2)
	{
	xy[i]=xyo[i]+x0;
	xy[i+1]=xyo[i+1]+y0;
	}
vga->setfillstyle(0);
vga->setcolor(255,255,100);
vga->polyfill(3,xy+4);
vga->setcolor(160,160,50);
vga->polyfill(3,xy+2);
vga->poly(3,xy+2);
vga->poly(3,xy+4);
vga->setcolor(120,120,30);
vga->polyfill(3,xy);
vga->poly(3,xy);
}
