#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
    char filename[160];
	int i,num;
	double xstart,xstep,ystart,ystep,zelev;
	FILE *outfile;

	if (argc < 8 )
	{   puts("\n###############################################################################");
        puts("  Usage:  MAKE_XYZ filename num_stations xstart xstep ystart ystep zelev\n");
        puts("    Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.");
        puts("  This program creates a simple XYZ file.");
        puts("  Example: MAKE_XYZ file1.xyz 5 0.0 1.0 11.0 0.0 0.0");
        puts("  The output XYZ file would look like:");
        puts("   21");
        puts("  0.0 11.0 0.0");
        puts("  1.0 11.0 0.0");
        puts("  2.0 11.0 0.0");
        puts("  3.0 11.0 0.0");
        puts("  4.0 11.0 0.0");
        printf("  Version 1.0 - Last Revision Date: %s at %s      \n","March 13, 2000",__TIME__);
        puts("###############################################################################");
        return 1;
    }

    strcpy(filename,argv[1]);
	num    = atoi(argv[2]);
	xstart = atof(argv[3]);
	xstep  = atof(argv[4]);
	ystart = atof(argv[5]);
	ystep  = atof(argv[6]);
	zelev  = atof(argv[7]);

	outfile = fopen(filename,"wt");
	if (outfile == NULL)
	{	printf("\nUnable to open output file %s\n",filename);
		return 2;
	}

	fprintf(outfile,"%5d\n",num);
	for (i=0; i<num; i++)
	   fprintf(outfile,"%8.3f %8.3f %8.3f\n",xstart+(i*xstep),ystart+(i*ystep),zelev);
	fclose(outfile);

    return 0;
}
