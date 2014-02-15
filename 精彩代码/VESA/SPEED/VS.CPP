#include <graphics.h>
#include <stdio.h>
#include <bios.h>

#include "\vga\vga.h"

#define ND 2
#define NL 50

class Speed {
	long start_dida,end_dida,dida_num;
	double dot_s,scr_s,dat_s;
	int wide,high,dot_bit;
public:
	Speed(int *p);
	void start();
	void end();
	void calu(int n);
	void out();
	} ;

Speed::Speed(int *p)
{
wide=p[0]; high=p[1],dot_bit=p[2];
start_dida=end_dida=dida_num=0L;
dot_s=scr_s=dat_s=0.0;
}

void Speed::start()
{
start_dida=biostime(0,start_dida);
}

void Speed::end()
{
end_dida=biostime(0,end_dida);
dida_num=end_dida-start_dida;
}

void Speed::calu(int scr_n)
{
double dida=18.2;
double d1,d2,d3;
d1=(double)wide*(double)high;
d2=dida_num; d2 /= dida;
if(d2==0)
	d1=0;
else
	d1 /= d2;
d1 *= (double)scr_n;
dot_s=d1/1000.0;
scr_s=(double)scr_n/d2;
d3=(double)dot_bit/8.0;
dat_s=(d1/1024.0)*d3;
}

void Speed::out()
{
printf("%4d*%3d*%2d %5.0f %5.2f %5.0f   ",wide,high,dot_bit,dot_s,scr_s,dat_s);
}

VGABASE *vga;

void run1(Speed *sw)
{
int i,j,k;
int wide=vga->WIDE;
int high=vga->HIGH;
sw->start();
for(i=0;i<ND;i++)
	for(j=0;j<high;j++)
		for(k=0;k<wide;k++)
			vga->putpixel(k,j);
sw->end();
}

void run2(Speed *sw)
{
int i,j,k;
int wide=vga->WIDE-1;
int high=vga->HIGH-1;
sw->start();
for(i=0;i<NL;i++)
	for(j=0;j<=high;j++)
		vga->scanline(0,wide,j);
sw->end();
}

#define N 15

void main()
{
int p[N][3]={{640,480,4},{800,600,4},{1024,768,4},
			{320,200,8},{640,400,8},{640,480,8},{800,600,8},{1024,768,8},
			{320,200,16},{512,480,16},{640,480,16},{800,600,16},
			{320,200,24},{640,480,24},
			{640,480,4}};
Speed sp1[N]={p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],
			p[11],p[12],p[13],p[14]};
Speed sp2[N]={p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],
			p[11],p[12],p[13],p[14]};
int i,j,k;
Event evt;

for(i=0;i<N-1;i++)
	{
	switch(i) {
		case 0: vga=new _640_480_16; break;
		case 1: vga=new _800_600_16; break;
		case 2: vga=new _1024_768_16; break;

		case 3: vga=new _320_200_256; break;
		case 4: vga=new _640_400_256; break;
		case 5: vga=new _640_480_256; break;
		case 6: vga=new _800_600_256; break;
		case 7: vga=new _1024_768_256; break;

		case 8: vga=new _320_200_64k; break;
		case 9: vga=new _512_480_64k; break;
		case 10: vga=new _640_480_64k; break;
		case 11: vga=new _800_600_64k; break;

		case 12: vga=new _320_200_16M; break;
		case 13: vga=new _640_480_16M; break;
		}
	if(vga->init()==0)
		continue;
	if(i<=7) vga->setcolor(7);
	else vga->setcolor(127,127,127);
	run1(sp1+i);
	sp1[i].calu(ND);
	if(i<=7) vga->setcolor(1);
	else vga->setcolor(63,63,191);
	run2(sp2+i);
	sp2[i].calu(NL);
	vga->close();
	delete vga;
	evt.getevent();
	if( (evt.what&evKey) && evt.key==27 )
		break;
	}

int drv=VGA,mode=VGAHI;
registerbgidriver(EGAVGA_driver);
initgraph(&drv,&mode,"d:\\bc31\\bgi");
setcolor(1);

sp1[N-1].start();
for(i=0;i<ND;i++)
	for(j=0;j<480;j++)
		for(k=0;k<640;k++)
			putpixel(k,j,1);
sp1[N-1].end();
sp1[N-1].calu(ND);

sp2[N-1].start();
for(i=0;i<NL;i++)
	bar(0,0,639,479);
sp2[N-1].end();
sp2[N-1].calu(NL);

restorecrtmode();

printf("\n             DS1   DS2   DS3                 LS1   LS2   LS3\n");
for(i=0;i<N;i++)
	{
	sp1[i].out(); sp2[i].out();
	printf("\n");
	}
printf("Note:\n");
printf("DS1: Write Dot.  Single Dot Speed (kDot/s)\n");
printf("DS2: Wirte Dot.  Screen Speed (Screen/s)\n");
printf("DS3: Wirte Dot.  Data Transfer Rate (kB/s)\n");
printf("LS1: Draw Scanline.  Single Dot Speed (kDot/s)\n");
printf("LS2: Draw Scanline.  Screen Speed (Screen/s)\n");
printf("LS3: Draw Scanline.  Data Transfer Rate (kB/s)\n");


}
