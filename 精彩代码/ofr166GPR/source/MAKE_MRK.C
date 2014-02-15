#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
	int i,num;
	double start,step;
	FILE *outfile;

	if (argc < 5 )
	{
	   printf("\nUsage: MAKE_MRK start step num filename\n");
	   return 1;
	}

	start = atof(argv[1]);
	step  = atof(argv[2]);
	num   = atoi(argv[3]);

	outfile = fopen(argv[4],"wt");
	if (outfile == NULL)
	{
		printf("\nUnable to open output file %s\n",argv[4]);
		return 2;
	}
	fprintf(outfile,"%d\n",num);
	for (i=0; i<num; i++)
	   fprintf(outfile,"%g\n",start+(i*step));
	fclose(outfile);

    return 0;
}
