#include <graphics.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

struct RGB
{
	unsigned char red ;
	unsigned char green ;
	unsigned char blue ;
};

void GetRgb(int colorPal,RGB& rgb)
{
	union REGS regs ;
	struct SREGS sregs ;

	regs.x.ax = 0x1007 ;
	regs.h.bl = (char)colorPal ;
	int86(0x10,&regs,&regs);

	regs.x.ax = 0x1015 ;
	regs.h.bl = regs.h.bh ;
	regs.h.bh = 0 ;
	int86(0x10,&regs,&regs);

//	rgb.red = regs.h.dh << 2;
//	rgb.green = regs.h.ch << 2 ;
//	rgb.blue = regs.h.cl << 2;

	rgb.red = regs.h.dh;
	rgb.green = regs.h.ch ;
	rgb.blue = regs.h.cl;
}

RGB rgb[16] ;

int main(void)
{
/* request auto detection */
	int gdriver = DETECT, gmode, errorcode;

/* initialize graphics and local variables */
	initgraph(&gdriver, &gmode, "c:\\borlandc\\bgi");

/* read result of initialization */
	errorcode = graphresult();
/* an error occurred */
	if (errorcode != grOk)
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
/* terminate with an error code */
		exit(1);
	}

/* grab a copy of the palette */
	printf("default palette\n");
	for(int i = 0 ; i < 16 ; i++){
		GetRgb(i,rgb[i]);
		printf("%d %d %d\n",rgb[i].red,rgb[i].green,rgb[i].blue);
	}

	FILE * palfp = fopen("swin.pal","wb");
	if(palfp == NULL){
		printf("Opend swin.pal error");
		exit(1);
	}
	fwrite((char *)&rgb,1,48,palfp);
	fclose(palfp);

/* clean up */
	getch();
	closegraph();
	return 0;
}