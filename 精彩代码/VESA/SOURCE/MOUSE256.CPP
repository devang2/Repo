#include "\vga\vgabase.h"
#include "\vga\vga256.h"
#include "\vga\vgamouse.h"
#include "\vga\evtcode.h"

MouseMap256::MouseMap256(VGABASE *v) : MouseMap(v)
{
wide=19;
high=23;
centre_X=0;
centre_Y=0;
activMask=evMouse;
v256=(VGA256 *)v;
setup();
}

void MouseMap256::init(void)
{
Event ev;
active(ev);
}

void MouseMap256::active(Event evt)
{
static unchar rgb[8]={0x60,0x70,0x80,0x90,0xa7,0xbf,0xdf,0xff};
static k=0;
int i,j;
j=k;
for(i=0;i<8;i++)
	{
	v256->setdac(0xf8+i,rgb[j],rgb[j],rgb[j]);
	j++;
	if(j>=8) j=0;
	}
k++;
if(k>=8) k=0;
}

void MouseMap256::draw(int x0,int y0)
{
int x1=x0+1;
vga->setcolor(0xf8);
vga->putpixel(x0,y0); y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->setcolor(0xf9);
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->setcolor(0xfa);
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->setcolor(0xfb);
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->setcolor(0xfc);
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->setcolor(0xfd);
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1++; y0++;
vga->scanline(x0,x1,y0); x1=x0+11; y0++;
vga->setcolor(0xfe);
vga->scanline(x0,x1,y0); y0++;
vga->scanline(x0,x1,y0); y0++; x1=x0+12;
vga->scanline(x0,x0+1,y0);
vga->scanline(x0+6,x1,y0); y0++;
vga->setcolor(0xff);
vga->scanline(x0+6,x1,y0); y0++; x1=x0+13;
vga->scanline(x0+7,x1,y0); y0++;
vga->scanline(x0+7,x1,y0);
}

