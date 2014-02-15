#include "\vga\vgabase.h"
#include "\vga\vgamouse.h"
#include "\vga\vga16M.h"

MouseMap16M::MouseMap16M(VGABASE *v) : MouseMap(v)
{
wide=15;
high=15;
centre_X=7;
centre_Y=7;
setup();
vt=(VGA16M *)v;
}

void MouseMap16M::draw(int x0,int y0)
{
int i,x,y;
x=x0+2; y=y0+9;
vt->en_scanline(x0+2,x0+15,y0+9,1,2);
y=y0+2; x=x0+9;
for(i=2;i<=15;i++)
	{
	vt->en_putpixel(x,y,1,2);
	y++;
	}
vga->setcolor(255,255,255);
vga->scanline(x0,x0+13,y0+7);
vga->line(x0+7,y0,x0+7,y0+13);
}

