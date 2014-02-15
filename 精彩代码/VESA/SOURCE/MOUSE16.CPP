#include "\vga\vgabase.h"
#include "\vga\vga16.h"
#include "\vga\vgamouse.h"

MouseMap16::MouseMap16(VGABASE *v) : MouseMap(v)
{
wide=10;
high=15;
centre_X=0;
centre_Y=0;
setup();
}

void MouseMap16::draw(int x0,int y0)
{
int i;
vga->setcolor(15);
for(i=2;i<10;i++)
	vga->scanline(x0+1,x0+i,y0+i);
vga->scanline(x0+1,x0+5,y0+10);
vga->putpixel(x0+1,y0+1);
vga->putpixel(x0+1,y0+11);
vga->line(x0+5,y0+11,x0+7,y0+15);
vga->line(x0+6,y0+11,x0+8,y0+15);
vga->setcolor(0);
vga->putpixel(x0+1,y0+12);
vga->putpixel(x0+2,y0+11);
vga->line(x0,y0,x0,y0+12);
vga->line(x0+1,y0,x0+10,y0+9);
vga->line(x0+4,y0+11,x0+6,y0+15);
vga->line(x0+6,y0+10,x0+9,y0+15);
}

