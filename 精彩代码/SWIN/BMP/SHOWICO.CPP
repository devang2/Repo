#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

typedef struct {
	unsigned icoWidth ;
	unsigned icoHeight ;
}ICOHEAD;

int main(int argc,char * argv[])
{
	FILE * icofp ;
	ICOHEAD icoHead ;
	if(argc < 2){
		printf("Usage:%s <icofile>",argv[0]);
		return 1 ;
	}
	if((icofp=fopen(argv[1],"rb"))==NULL){
		printf("[%s] open error",argv[1]);
		return 1;
	}
	fread((char *)&icoHead,1,sizeof(ICOHEAD),icofp);
	long icoSize = icoHead.icoWidth*icoHead.icoHeight;
	char * buf = new char[icoSize];
	fread(buf,1,icoSize,icofp);

	/* request auto detection */
	int gdriver = DETECT, gmode, errorcode;

	/* initialize graphics and local
		variables */
	initgraph(&gdriver, &gmode, "c:\\borlandc\\bgi");

	/* read result of initialization */
	errorcode = graphresult();
	if (errorcode != grOk)  /* an error
		 occurred */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1); /* terminate with an error
						code */
	}

	for(int i = 0 ;i<icoHead.icoHeight;i++)
		for(int j = 0 ;j<icoHead.icoWidth;j++)
			putpixel(j,i,buf[i*icoHead.icoHeight+j]);
   getch();
	/* clean up */
	closegraph();
	return 0;
}