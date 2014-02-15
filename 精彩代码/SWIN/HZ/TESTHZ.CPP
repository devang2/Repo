#include "hz.hpp"
#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

int main(void)
{
    /* request auto detection */
   int gdriver = DETECT, gmode, errorcode;

	/* initialize graphics and local
	variables */
	initgraph(&gdriver, &gmode, "c:\\borlandc\\bgi");

	/* read result of initialization */
	errorcode = graphresult();
	/* an error occurred */
	if (errorcode != grOk)
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();

		exit(1);    /* terminate with an error code */
	}
        FILE * fp = fopen("c:\\TAN\\NOTE\\NOTE.DOC","r") ;
	char * str = new char[2000];
	if(!str) exit(1);
        long fsize = fread(str,1,2000,fp)+1;
	fclose(fp);
        str[fsize] = 0 ;
	hz.Outtext(str);

	/* clean up */
	getch();
	closegraph();
	return 0;
}
