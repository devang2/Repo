/******************************** S10_MRKS.C*********************************\
 * This program finds the tick mark trace locations in GSSI SIR-10A files
 * and displays them to the screen.  The user is prompted to save the values
 * to disk.  If Yes then they give a filename and the values are stored to
 * disk with the MRK extension.  Other programs, such as DISPLAY1, will
 * recognize the MRK extension and ASCII format of the file.
 *
 * Jeff Lucius, Geophysicist
 * Department of Interior
 * U.S. Geological Survey
 * Geologic Division
 * Branch of Geophysics
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 *
 * Revision History:
 *   March 3, 1994     - Completed.
 *   June 5, 1994      - fixed problem where first trace not being read
 *   March 10, 2000    - Changed how markes found. Now check that 2nd
 *                       nibble is 0x8 to accomodate RADAN files as well
 *                       SIR-10 and SIR-2.
 *                     - Added diff from last to MRK file
 *                     - Updated usage message.
 *                     - Changed version to 2.0.
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm  s10_mrks.c
 *     To compile for 80486 executable :
 *       icc /F /xnovm /zmod486 s10_mrks.c
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program on 80486
 *
 *   Borland C++ 3.1
 *     bcc -3 -a -d -f287 -ml -O2 s10_mrks.c
 *
 *     -3 generates 80386 protected mode compatible instructions
 *     -a alignes code on (2-byte) word boundaries
 *     -d merges duplicates strings
 *     -f287 invokes 80287 instructions for floating point math
 *     -ml uses the large memory model
 *     -O2 optimizes for fastest code
 *
 * To run: S10_MRKS dzt_filename max_ticks [mrk_filename]
 *
\****************************************************************************/

#include <stdio.h>     /* puts,printf,fopen,fread,fclose,FILE,NULL,_IOFBF,
                          SEEK_SET,fprintf,fseek,scan,rewind */
#include <stdlib.h>    /* exit,malloc,free,atoi */
#include <string.h>    /* strcpy,strchr,strcat */

#include "dzt_pgms.h"  /* realmalloc,realfree,struct DztHdrStruct */

void main(int argc, char *argv[])
{
    char in_filename[80],out_filename[80];
    int max_ticks,num_ticks,i,isave,batch,chr;
    size_t chunk_num;
    size_t disk_trace_size;   /* number of bytes in one trace */
    size_t disk_chunk_size;   /* number of bytes in one chunk of traces */
    size_t *ticks     = NULL;
    void *trace_chunk = NULL; /* hold one set of scans in multichannel files */
    FILE *infile      = NULL;
    struct DztHdrStruct DztHdr;
    #if defined(_INTELC32_)
      char *buffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
    #endif

    /* Get command line arguments */
    if (argc < 3)
    {   puts("\n\n#############################################################################");
		puts("  Usage: S10_MRKS dzt_filename max_ticks [mrk_filename]");
        puts("    Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.\n");
        puts("  This program finds up to \"max_ticks\" tick mark locations in the GSSI");
		puts("    SIR-10, SIR-2, or RADAN file \"dzt_filename\".\n");
        puts("  If the optional argument \"mrk_filename\" is given, then tick marker");
		puts("    trace locations are automatically saved to disk.\n");
        printf("  Version %s - Last Revision Date: March 10, 2000 at %s      \n","2.0",__TIME__);
	    puts("#############################################################################\n\n");
        exit(0);
    }
    batch = isave = 0;
    strcpy(in_filename,argv[1]);
    max_ticks = atoi(argv[2]);
    if (argc > 3)
	{   strcpy(out_filename,argv[3]);
        isave = 1;
        batch = 1;
    }

    /* Open DZT file */
    if ((infile = fopen(in_filename,"rb")) == NULL)
	{   printf("\nERROR: Unable to open file %s\n",in_filename);
		exit(1);
    }

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(buffer,vbufsize);
      if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
      else         setvbuf(infile,NULL,_IOFBF,vbufsize);
    #endif

    /* Get the first header from the file */
    if (fread((void *)&DztHdr,sizeof(struct DztHdrStruct),(size_t)1,infile) < 1)
    {   fclose(infile);
        #if defined(_INTELC32_)
          realfree(buffer);
        #endif
        printf("\nERROR: short count from fread() of file %s\n",in_filename);
        exit(2);
    }

    /* Allocate storage for one chunk of scans and for maximum number of ticks */
    disk_trace_size = DztHdr.rh_nsamp * (DztHdr.rh_bits/8);   /* 8 bits per byte */
    disk_chunk_size = DztHdr.rh_nchan * disk_trace_size;
    trace_chunk = (void *)malloc(disk_chunk_size);
    ticks = (size_t *)malloc((size_t)max_ticks * sizeof(size_t));
    if (trace_chunk == NULL || ticks == NULL)
    {   free(trace_chunk); free(ticks);
		fclose(infile);
        #if defined(_INTELC32_)
          realfree(buffer);
        #endif
        puts("\nERROR: Unable to allocate temporary storage.");
        exit(3);
    }

    /* Move the file pointer to start of first trace */
    rewind(infile);
    fseek(infile,(size_t)DztHdr.rh_nchan * sizeof(struct DztHdrStruct),SEEK_SET);

    /* Get chunk of traces from file; only first channel has tick marks */
    printf("\nS10_MRKS is finding tick marks in %s ...",argv[1]);
    chunk_num = num_ticks = 0;
    while (1)
	{   if (fread(trace_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
            break;
        switch (DztHdr.rh_bits)
        {   case 8:
                /* if (((unsigned char *)trace_chunk)[1] == 0xE8U) */
                if (((((unsigned char *)trace_chunk)[1])&0x0FU)==0x08U)
                {   ticks[num_ticks] = chunk_num;
                    num_ticks++;
                }
    	        break;
	        case 16:
                /* if (((unsigned short *)trace_chunk)[1] == 0xE800U) */
                if (((((unsigned short *)trace_chunk)[1])&0x0F00U) == 0x0800U)
                {   ticks[num_ticks] = chunk_num;
                    num_ticks++;
                }
        	    break;
	        case 32:
                if (((unsigned long *)trace_chunk)[1] == 0xE8000000UL)
                {   ticks[num_ticks] = chunk_num;
                    num_ticks++;
                }
	            break;
        }
        chunk_num++;
        if (num_ticks >= max_ticks) break;
    }
    printf("\r                                                                         \r");
    fclose(infile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    free(trace_chunk);
    if (num_ticks >= max_ticks)
    	puts("\nWARNING: Maximum number of tick marks found, possibly before EOF.");

    /* Print values to screen */
    if (num_ticks == 0)
    {   printf("\nNO tick markers found in file %s\n",in_filename);
    } else
    {   printf("\n%d tick markers found in file %s (%d traces)\n",num_ticks,in_filename,chunk_num);
        puts("   #   location");
        printf("%4d    %6d\n",0,ticks[0]);
	    for (i=1; i<num_ticks; i++)
    	{   printf("%4d    %6d   diff from previous = %6d\n",i,ticks[i],ticks[i] - ticks[i-1]);
            if (i>0 && !(i%22)) getch();
        }
    }

    /* Save to disk */
    if (!batch)
	{   printf("\nDo you want to save tick mark locations to disk (Y/N)? ==>");
    	chr = getche();
	    if (chr == 'Y' || chr == 'y')
    	{   printf("\nEnter 8-character filename (no extension) ==>");
        	scanf("%s",out_filename);
	        isave = 1;
    	} else
    		puts("");
    }
    if (isave)
    {   char *cp;
		FILE *outfile;

        if ((cp = strchr(out_filename,'.')) != NULL)  *cp = 0;
        strcat(out_filename,".MRK");
        outfile = fopen(out_filename,"wt");
        if (outfile)
        {   if (num_ticks == 0)
            {   printf("\nNO tick markers found in file %s\n",in_filename);
            } else
            {   fprintf(outfile,"%d ; number of markers found\n",num_ticks);
                fprintf(outfile,"%6d ;diff from previous =      0\n",ticks[0]);
                for (i=1; i<num_ticks; i++)  fprintf(outfile,"%6d ;diff from previous = %6d\n",
                        ticks[i],ticks[i]-ticks[i-1]);
            }
            fclose(outfile);
            printf("Tick marker locations saved to %s\n",out_filename);
        } else
            printf("ERROR: Unable to open %s to save tick locations.\n",out_filename);
    }

    /* Free storage and return */
    free(ticks);
    exit(0);
}
