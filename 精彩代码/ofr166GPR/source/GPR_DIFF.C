/******************************** GPR_DIFF.C ********************************\
 *                               version 1.00
 * GPR_DIFF subtracts or adds one digital ground penetrating radar data set
 * from/to another.  Both data sets (files) must have the same number of
 * traces, one channel (DZT files), the same number of samples per trace,
 * the same sample rate (time interval between samples), and the same storage
 * format.  The following computer storage formats are recognized.
 *
 *   GSSI SIR-10A version 3.x to 5.x binary files, 8-bit or 16-bit unsigned
 *     integers with an embedded descriptive 512- or 1024-byte header.
 *     Data files usually have the extension "DZT".
 *   Sensors and Software pulseEKKO binary files, 16-bit signed integers
 *     accompanied by a descriptive ASCII "HD" file.  Data files must have
 *     the extensiom "DT1".
 *   SEG-Y formatted files, 16-bit or 32-bit signed integers or 32-bit
 *     floating point reals.  Data files usually have the extension "SGY".
 *
 * Jeff Lucius
 * Geophysicist
 * U.S. Geological Survey
 * Geologic Division, Branch of Geophysics
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@musette.cr.usgs.gov
 *
 * Revision History:
 *  February 16, 1996  - Completed as a modification and combination of
 *                       DZT_SUB and  GPR_STAK.C.
 *  August 11, 1997    - Added support for modified DZT formats.
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /zfloatsync gpr_diff.c gpr_io.lib jl_util1.lib
 *     To compile for 80486 executable:
 *       icc /F /xnovm /zfloatsync /zmod486 gpr_diff.c gpr_io.lib jl_util1.lib
 *
 *     /F removes the floating point emulator, reduces binary size.
 *     /xnovm specifies no virtual memory manager, disk access is too slow.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     Jeff Lucius's GPR I/O functions (DZT, DT1, SGY) are in gpr_io.lib.
 *     Jeff Lucius's utility functions are in jl_util1.lib.
 *
 *     to remove assert() functions from code also add:
 *     /DNDEBUG
 *
 * To run: GPR_DIFF in_filename1 in_filename2 out_filename [/a /d /b]
 *
\****************************************************************************/

/********************** Include required header files ***********************/
/* non-ANSI headers */
#include <conio.h>     /* getch */
#include <errno.h>     /* errno */
#include <io.h>        /* access */
#include <sys\stat.h>  /* stat,struct stat  */

/* ANSI-compatible headers */
#include <float.h>     /* FLT_MAX */
#include <stdio.h>     /* puts,printf,fopen,fread,fclose,FILE,NULL,_IOFBF,
                          SEEK_SET,fseek,fwrite */
#include <stdlib.h>    /* exit,malloc,free,_splitpath */
#include <string.h>    /* strcpy,strstr,strcmp,strupr (non-ANSI) */
#include <time.h>      /* struct tm,time_t,time,localtime */

/* application headers */
#include "gpr_io.h"    /* to use gpr_io.lib functions, etc. */
#include "jl_defs.h"   /* common manifest constants and macros */
#include "jl_util1.h"  /* ANSI_there, MAX_PATHLEN, etc. */

/**************************** Global variables ******************************/
int Debug = FALSE;     /* if TRUE, turn debugging on */
int Batch = FALSE;     /* if TRUE, supress interaction with user */

/*************************** Function prototypes ****************************/
void ExitFunc1(void);

/********************************** main() **********************************/
void main(int argc, char *argv[])
{
    char   in_filename1[MAX_PATHLEN];
    char   in_filename2[MAX_PATHLEN];
    char   out_filename[MAX_PATHLEN];
    int    i,itemp,min_args,add,format1,format2;
    long   size1,size2;
    struct stat statbuf;

    /* following used by GetGprFileType() */
    int   file_hdr_bytes, num_hdrs, trace_hdr_bytes, samples_per_trace;
    int   num_channels, input_datatype, total_time, file_type;
    long  num_traces;

    /* following in errno.h */
    extern int errno;

    /* Register exit function 1 (return console colors to white on black) */
    atexit(ExitFunc1);

    /* Check if enough command line arguments */
    min_args = 4;
    if (argc < min_args)
    {   puts("\n");
        if (ANSI_there())  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
        puts("");
        puts("###############################################################################");
        puts("   Usage: GPR_DIFF in_filename1 in_filename2 out_filename [/a /b /d]");
        puts("     This program subtracts/adds in_filename2 from/to in_filename1 and stores");
        puts("       the results to disk as out_filename.");
        puts("     Recognized storage formats include: GSSI DZT, S&S DT1/HD, and SEG-Y.");
        puts("\n   Required command line arguments:");
        puts("     in_filename1 - The name of the first input GPR data file.");
        puts("     in_filename2 - The name of the second input GPR data file.");
        puts("     out_filename - The name of the output GPR data file.");
        puts("   Information from the the first file header is used for the output file");
        puts("     header - if applicable, modification date, comments, and processing");
        puts("     history are changed.");
        puts("   NOTE: Files MUST HAVE the same number of traces, one channel (DZT files),");
        puts("      same number of samples per trace, same sample rate (time interval between");
        puts("      samples), and the same storage format.");
		puts("   Optional command line arguments (do not include brackets):");
        puts("     /a - add the data sets instead of the default which is to subtract them.");
        puts("     /b - turn batch processing on.");
        puts("     /d - turn debugging on.");
        printf(" Version 1.00 - Last Revision Date: %s at %s\n",__DATE__,__TIME__);
        puts("###############################################################################");
        exit(1);
    }

    /* Get command line arguments */
    strcpy(in_filename1,strupr(argv[1]));
    strcpy(in_filename2,strupr(argv[2]));
    strcpy(out_filename,strupr(argv[3]));
    add = FALSE;
    for (i = min_args; i<argc; i++)
    {   if (strstr(strupr(argv[i]),"/A")) add   = TRUE;
        if (strstr(strupr(argv[i]),"/D")) Debug = TRUE;
        if (strstr(strupr(argv[i]),"/B")) Batch = TRUE;
    }

    /* Set console colors to red on black to report problems */
    if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */

    /* Check that filenames are different */
    if (strcmp(in_filename1,in_filename2) == 0)
    {   puts("\nGPR_DIFF: ERROR - Input filenames must be different.");
        exit(1);
    }
    if (strcmp(in_filename1,out_filename) == 0)
    {   puts("\nGPR_DIFF: ERROR - Output filename must be different from input file names.");
        exit(1);
    }
    if (strcmp(in_filename2,out_filename) == 0)
    {   puts("\nGPR_DIFF: ERROR - Output filename must be different from input file names.");
        exit(1);
    }

    /* Check if storage formats are the same */
    itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                   &samples_per_trace,&num_channels,&num_traces,
                   &input_datatype,&total_time,&file_type,in_filename1);
    if (itemp > 0)
    {   printf("%s\n",GetGprFileTypeMsg[itemp]);
        exit(2);
	}
	format1 = file_type;
    itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                   &samples_per_trace,&num_channels,&num_traces,
                   &input_datatype,&total_time,&file_type,in_filename2);
    if (itemp > 0)
    {   printf("%s\n",GetGprFileTypeMsg[itemp]);
        exit(2);
    }
    format2 = file_type;
    if (format1 != format2)
    {   printf("\nGPR_DIFF: ERROR - Both input files must have same storage format\n");
        exit(3);
    }

    /* Check that file sizes (number of traces) are the same */
    itemp = stat(in_filename1, &statbuf);
    if (itemp)
    {   printf("\nGPR_DIFF: ERROR - Unable to get status of file %s\n",in_filename1);
        if (errno == ENOENT) puts("                  File or path name not found.");
        exit(4);
    }
    size1 = statbuf.st_size;
    itemp = stat(in_filename2, &statbuf);
    if (itemp)
    {   printf("\nGPR_DIFF: ERROR - Unable to get status of file %s\n",in_filename2);
        if (errno == ENOENT) puts("                  File or path name not found.");
        exit(4);
    }
    size2 = statbuf.st_size;
    if (size1 != size2)
    {    puts("\nGPR_DIFF: ERROR - Both input files must have the same number of traces");
        exit(4);
    }

    /* Set console colors to cyan on black to report progress */
    if (ANSI_there())  printf("%c%c36;40m",0x1B,0x5B);  /* cyan on black */

    /* Get the data, subtract/add file2 from/to file 1, and save to disk */
    switch (file_type)
	{   case DZT:  case MOD_DZT:
		{   char   str[400];
			int    i;
			int    num_chan1,num_bits1,num_samps1;
			int    num_chan2,num_bits2,num_samps2;
			double dtemp1,dtemp2;
			size_t trace_num;
            size_t disk_trace_size;    /* number of bytes in one trace */
            void  *trace1      = NULL; /* hold one trace */
            void  *trace2      = NULL; /* hold one trace */
            struct tm *newtime;
            time_t long_time;
            struct DztHdrStruct DztHdr1,DztHdr2;
            FILE  *infile1     = NULL;
            FILE  *infile2     = NULL;
            FILE  *outfile     = NULL;
            #if defined(_INTELC32_)
              char *inbuffer1  = NULL;
              char *inbuffer2  = NULL;
              char *outbuffer  = NULL;
              size_t vbufsize  = 64512; /* 63 KB */
            #endif

            /* Open input DZT files */
            if ((infile1 = fopen(in_filename1,"rb")) == NULL)
            {   printf("\nGPR_DIFF: ERROR - Unable to open input DZT file %s\n",in_filename1);
                if (access(in_filename1,4))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                exit(5);
            }
            if ((infile2 = fopen(in_filename2,"rb")) == NULL)
            {   printf("\nGPR_DIFF: ERROR - Unable to open input DZT file %s\n",in_filename2);
                if (access(in_filename2,4))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                fclose(infile1);
                exit(5);
            }

            /* Open output DZT file */
			if ((outfile = fopen(out_filename,"wb")) == NULL)
            {   printf("\nGPR_DIFF: ERROR - Unable to open output DZT file %s\n",out_filename);
                if (access(out_filename,2))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                fclose(infile1);  fclose(infile2);
                exit(5);
            }

            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
              realmalloc(inbuffer1,vbufsize);
              if (inbuffer1)  setvbuf(infile1,inbuffer1,_IOFBF,vbufsize);
              else            setvbuf(infile1,NULL,_IOFBF,vbufsize);
              realmalloc(inbuffer2,vbufsize);
              if (inbuffer2)  setvbuf(infile2,inbuffer2,_IOFBF,vbufsize);
              else            setvbuf(infile2,NULL,_IOFBF,vbufsize);
              realmalloc(outbuffer,vbufsize);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
              else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
            #endif

            /* Get the first header from the first input file */
            if (fread((void *)&DztHdr1,sizeof(struct DztHdrStruct),(size_t)1,infile1) < 1)
            {   fclose(infile1); fclose(infile2); fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
                #endif
                printf("\nGPR_DIFF: ERROR - short count from fread() getting first DZT header\n");
                exit(6);
            }
            num_chan1  = DztHdr1.rh_nchan;
            num_bits1  = DztHdr1.rh_bits;
            num_samps1 = DztHdr1.rh_nsamp;

            /* Get the first header from the second input file */
            if (fread((void *)&DztHdr2,sizeof(struct DztHdrStruct),(size_t)1,infile2) < 1)
            {   fclose(infile1); fclose(infile2); fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
                #endif
                printf("\nGPR_DIFF: ERROR - short count from fread() getting second DZT header\n");
                exit(6);
			}
            num_chan2  = DztHdr2.rh_nchan;
            num_bits2  = DztHdr2.rh_bits;
            num_samps2 = DztHdr2.rh_nsamp;

            /* Check that both files the same type */
            if (num_chan1 > 1 || num_chan2 > 1)
            {   fclose(infile1); fclose(infile2); fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
                #endif
                puts("\nGPR_DIFF: ERROR - Both input DZT files must have only one channel");
                exit(7);
            }
            if (num_bits1 != num_bits2)
            {   fclose(infile1); fclose(infile2); fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
                #endif
                puts("\nGPR_DIFF: ERROR - Both input DZT files must have same sample size");
                exit(7);
            }
            if (num_samps1 != num_samps2)
            {   fclose(infile1); fclose(infile2); fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
                #endif
                puts("\nGPR_DIFF: ERROR - Both input DZT files must have same number of samples");
                exit(7);
            }


            /* Allocate storage traces */
            disk_trace_size = num_samps1 * (num_bits1/8);   /* 8 bits per byte */
            trace1 = (void *)malloc(disk_trace_size);
            trace2 = (void *)malloc(disk_trace_size);
            if (trace1 == NULL || trace2 == NULL)
            {   free(trace1);  free(trace2);
                fclose(infile1); fclose(infile2); fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
                #endif
                puts("\nGPR_DIFF: ERROR - Unable to allocate temporary storage for DZT traces.");
                exit(8);
            }

            /* Update modification time/date */
            time( &long_time );                 /* Get time as long integer */
            newtime = localtime( &long_time );  /* Convert to local time */
            DztHdr1.rh_modif.month = newtime->tm_mon + 1;     /* 0 - 11 */
            DztHdr1.rh_modif.day   = newtime->tm_mday;        /* 1 - 31 */
            DztHdr1.rh_modif.year  = newtime->tm_year - 80;   /* year - 1900 */
            DztHdr1.rh_modif.hour  = newtime->tm_hour;        /* 0 - 24 */
            DztHdr1.rh_modif.min   = newtime->tm_min;
            DztHdr1.rh_modif.sec2  = newtime->tm_sec/2;

            /* Change file name */
            {   char drive[80],dir[80],fname[80],ext[80];

                _splitpath(out_filename,drive,dir,fname,ext);
                EditDztHeader(26,(void *)fname,&DztHdr1);
            }

            /* Change comment string */
            if (!add) sprintf(str,"\nGPR_DIFF: %s subtracted from %s\n",in_filename2,in_filename1);
            else      sprintf(str,"\nGPR_DIFF: %s added to %s\n",in_filename2,in_filename1);
            EditDztHeader(30,(void *)str,&DztHdr1);

            /* Copy header to output file */
            fwrite(&DztHdr1,sizeof(struct DztHdrStruct),1,outfile);

            /* Move the file pointer to start of first trace */
            fseek(infile1,(size_t)sizeof(struct DztHdrStruct),SEEK_SET);
            fseek(infile2,(size_t)sizeof(struct DztHdrStruct),SEEK_SET);

            /* Get traces from files */
            if (add)  printf("\nGPR_DIFF: Adding file %s to %s ...",in_filename2,in_filename1);
            else      printf("\nGPR_DIFF: Subtracting file %s from %s ...",in_filename2,in_filename1);
			trace_num = 0;
            while (1)
            {   if ( !(trace_num % 100) ) printf(".");
                if (fread(trace1,(size_t)disk_trace_size,(size_t)1,infile1) < 1)
                    break;  /* out of while(1) loop */
                if (fread(trace2,(size_t)disk_trace_size,(size_t)1,infile2) < 1)
                    break;  /* out of while(1) loop */
                switch (num_bits1)
                {   case 8:
                        for (i=2; i<num_samps1; i++)
                        {   dtemp1 = ((unsigned char *)trace1)[i];
                            dtemp1 -= 128.0;
							dtemp2 = ((unsigned char *)trace2)[i];
                            dtemp2 -= 128.0;
                            if (!add) dtemp1 -= dtemp2;
                            else      dtemp1 += dtemp2;
                            dtemp1 += 128.0;
                            dtemp1 = _LIMIT(0.0,dtemp1,255.0);
                            ((unsigned char *)trace1)[i] = dtemp1;
                        }
                        break;
                    case 16:
                        for (i=2; i<num_samps1; i++)
                        {   dtemp1 = ((unsigned short *)trace1)[i];
                            dtemp1 -= 32768.0;
                            dtemp2 = ((unsigned short *)trace2)[i];
                            dtemp2 -= 32768.0;
                            if (!add) dtemp1 -= dtemp2;
                            else      dtemp1 += dtemp2;
                            dtemp1 += 32768.0;
                            dtemp1 = _LIMIT(0.0,dtemp1,65535.0);
                            ((unsigned short *)trace1)[i] = dtemp1;
                        }
                        break;
                    case 32:
                        for (i=2; i<num_samps1; i++)
                        {   dtemp1 = ((unsigned long *)trace1)[i];
                            dtemp1 -= 2147483648.0;
                            dtemp2 = ((unsigned long *)trace2)[i];
                            dtemp2 -= 2147483648.0;
                            if (!add) dtemp1 -= dtemp2;
                            else      dtemp1 += dtemp2;
                            dtemp1 += 2147483648.0;
                            dtemp1 = _LIMIT(0.0,dtemp1,4294967295.0);
                            ((unsigned long *)trace1)[i] = dtemp1;
                        }
                        break;
                }  /* end of switch (num_bits1) block */
                if (fwrite(trace1,(size_t)disk_trace_size,(size_t)1,outfile) < 1)
                    break;  /* out of while(1) loop */
                trace_num++;
            }  /* end of while (1) block */

            fclose(infile1); fclose(infile2); fclose(outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
            #endif
            free(trace1);  free(trace2);
            printf("\nNew file %s contains %u traces\n",out_filename,trace_num);
            break;
        }  /* end of case DZT: block */
        case DT1:
        {   char   hd_infilename1[MAX_PATHLEN],hd_infilename2[MAX_PATHLEN];
            char   hd_outfilename[MAX_PATHLEN];
            char  *c_ptr,*proc_hist,str[80];
            int    proc_hist_size = 240;
            int    i,itemp,print_it;
            int    num_samps1,num_samps2;
            int    data_block_bytes,hdr_block_bytes;
            short *s_ptr1,*s_ptr2;
            double dtemp1,dtemp2;
            size_t trace_num;
            size_t disk_trace_size;    /* number of bytes in one trace */
            void  *trace1      = NULL; /* hold one trace */
            void  *trace2      = NULL; /* hold one trace */
            struct SsHdrInfoStruct HdInfo;
            FILE  *infile1     = NULL;
            FILE  *infile2     = NULL;
            FILE  *outfile     = NULL;
            #if defined(_INTELC32_)
              char *inbuffer1  = NULL;
              char *inbuffer2  = NULL;
              char *outbuffer  = NULL;
              size_t vbufsize  = 64512; /* 63 KB */
            #endif

            /* following in gpr_io.h */
            extern const char *GetSsHdFileMsg[];

            /* Construct HD filenames */
            hd_infilename1[0] = 0;
            strcpy(hd_infilename1,in_filename1);
            if ((c_ptr=strchr(hd_infilename1,'.')) != NULL)
                *c_ptr = 0;               /* terminated string at period */
            strcat(hd_infilename1,".hd");

            hd_infilename2[0] = 0;
            strcpy(hd_infilename2,in_filename2);
            if ((c_ptr=strchr(hd_infilename2,'.')) != NULL)
                *c_ptr = 0;               /* terminated string at period */
            strcat(hd_infilename2,".hd");

            hd_outfilename[0] = 0;
            strcpy(hd_outfilename,out_filename);
            if ((c_ptr=strchr(hd_outfilename,'.')) != NULL)
                *c_ptr = 0;               /* terminated string at period */
            strcat(hd_outfilename,".hd");

            /* Get info from input HD files */
            InitDt1Parameters(&HdInfo);
            if (Debug) print_it = TRUE;
            else       print_it = FALSE;
            itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename2);
            if (itemp > 0)
            {   printf("\n%s\n",GetSsHdFileMsg[itemp]);
                exit(5);
            }
            num_samps2 = HdInfo.num_samples;

            InitDt1Parameters(&HdInfo);
            if (Debug) print_it = TRUE;
            else       print_it = FALSE;
            itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename1);
            if (itemp > 0)
            {   printf("\n%s\n",GetSsHdFileMsg[itemp]);
                exit(5);
            }
            num_samps1 = HdInfo.num_samples;

            if (num_samps1 != num_samps2)
            {   puts("\nGPR_DIFF: ERROR - Both input DT1 files must have same number of samples");
                exit(6);
            }

            /* Allocate space for traces */
            data_block_bytes = num_samps1 * 2; /* data are signed short ints */
            hdr_block_bytes = sizeof(struct SsTraceHdrStruct);
            disk_trace_size = data_block_bytes + hdr_block_bytes;
            trace1 = (void *)malloc(disk_trace_size);
            trace2 = (void *)malloc(disk_trace_size);
            if (trace1 == NULL || trace2 == NULL)
            {   free(trace1); free(trace2);
				printf("\nGPR_DIFF: ERROR - Unable to allocate storage for DT1 data blocks.\n");
                exit(7);
            }

            /* Open input DT1 files */
            if ((infile1 = fopen(in_filename1,"rb")) == NULL)
			{   printf("\nGPR_DIFF: ERROR - Unable to open input DT1 file %s\n",in_filename1);
                if (access(in_filename1,4))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                free(trace1); free(trace2);
                exit(7);
            }
            if ((infile2 = fopen(in_filename2,"rb")) == NULL)
			{   printf("\nGPR_DIFF: ERROR - Unable to open input DT1 file %s\n",in_filename2);
                if (access(in_filename2,4))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                free(trace1); free(trace2);
                fclose(infile1);
                exit(7);
            }

            /* Open output DT1 file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
			{   printf("\nGPR_DIFF: ERROR - Unable to open output DT1 file %s\n",out_filename);
                if (access(out_filename,2))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                free(trace1); free(trace2);
                fclose(infile1);  fclose(infile2);
                exit(7);
            }

            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
              realmalloc(inbuffer1,vbufsize);
              if (inbuffer1)  setvbuf(infile1,inbuffer1,_IOFBF,vbufsize);
              else            setvbuf(infile1,NULL,_IOFBF,vbufsize);
              realmalloc(inbuffer2,vbufsize);
              if (inbuffer2)  setvbuf(infile2,inbuffer2,_IOFBF,vbufsize);
              else            setvbuf(infile2,NULL,_IOFBF,vbufsize);
              realmalloc(outbuffer,vbufsize);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
              else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
            #endif

            /* Set/save values for output header */
            if (HdInfo.proc_hist)  /* if allocated */
                itemp = strlen(HdInfo.proc_hist) + 1;
            itemp += proc_hist_size + 1;
            proc_hist = (char *)malloc((size_t)itemp);
            if (proc_hist)
            {   if (HdInfo.proc_hist)  /* allocated */
                    strcpy(proc_hist,HdInfo.proc_hist);
				if (!add) sprintf(str,"\nGPR_DIFF: %s subtracted from %s\n",in_filename2,in_filename1);
				else      sprintf(str,"\nGPR_DIFF: %s added to %s\n",in_filename2,in_filename1);
                strcat(proc_hist,str);
            }
            itemp = SaveSsHdFile((int)HdInfo.day,(int)HdInfo.month,(int)HdInfo.year,
                                (long)HdInfo.num_traces,(long)HdInfo.num_samples,
                                (long)HdInfo.time_zero_sample,(int)HdInfo.total_time_ns,
                                (double)HdInfo.start_pos,(double)HdInfo.final_pos,
                                (double)HdInfo.step_size,HdInfo.pos_units,
                                (double)HdInfo.ant_freq,(double)HdInfo.ant_sep,
                                (double)HdInfo.pulser_voltage,
                                (int)HdInfo.num_stacks,HdInfo.survey_mode,
                                proc_hist,hd_outfilename,out_filename,
								HdInfo.job_number,HdInfo.title1,HdInfo.title2,
                                0.0,0.0,0,NULL);
            free(proc_hist);

            /* Get traces from files */
			if (add)  printf("\nGPR_DIFF: Adding file %s to %s ...",in_filename2,in_filename1);
			else      printf("\nGPR_DIFF: Subtracting file %s from %s ...",in_filename2,in_filename1);
            trace_num = 0;
            while (1)
            {   if ( !(trace_num % 100) ) printf(".");
                if (fread(trace1,(size_t)disk_trace_size,(size_t)1,infile1) < 1)
                    break;  /* out of while(1) loop */
                if (fread(trace2,(size_t)disk_trace_size,(size_t)1,infile2) < 1)
                    break;  /* out of while(1) loop */

                s_ptr1 = (short *)((char *)trace1 + sizeof(struct SsTraceHdrStruct));
                s_ptr2 = (short *)((char *)trace2 + sizeof(struct SsTraceHdrStruct));
                for (i=0; i<num_samps1; i++)
                {   dtemp1 = s_ptr1[i];
                    dtemp2 = s_ptr2[i];
                    if (!add) dtemp1 -= dtemp2;
                    else      dtemp1 += dtemp2;
                    dtemp1 = _LIMIT(-32768.0,dtemp1,32767.0);
                    s_ptr1[i] = dtemp1;
                }

                if (fwrite(trace1,(size_t)disk_trace_size,(size_t)1,outfile) < 1)
                    break;  /* out of while(1) loop */
                trace_num++;
            }  /* end of while (1) */
            fclose(infile1); fclose(infile2); fclose(outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
            #endif
            free(trace1);  free(trace2);
            printf("\nNew file %s contains %u traces\n",out_filename,trace_num);
            break;
        }  /* end of case DT1: block */
        case SGY:
        {   char  *proc_hist,str[80];
            int    proc_hist_size = 240;
            int    i,itemp;
            int    data_block_bytes,hdr_block_bytes;
            int    num_samps1,num_samps2;
            short *s_ptr1,*s_ptr2;
			long  *l_ptr1,*l_ptr2;
            float *f_ptr1,*f_ptr2;
            int    swap_bytes;
            long   num_traces;
            struct SegyReelHdrStruct ReelHdr;
            struct GprInfoStruct GprInfo;
            double dtemp1,dtemp2;
            size_t trace_num;
            size_t disk_trace_size;    /* number of bytes in one trace */
            void  *trace1      = NULL; /* hold one trace */
            void  *trace2      = NULL; /* hold one trace */
            FILE  *infile1     = NULL;
            FILE  *infile2     = NULL;
            FILE  *outfile     = NULL;
            #if defined(_INTELC32_)
              char *inbuffer1  = NULL;
              char *inbuffer2  = NULL;
              char *outbuffer  = NULL;
              size_t vbufsize  = 64512; /* 63 KB */
            #endif

            /* following in gpr_io.h */
            extern const char *ReadSegyReelHdrMsg[];

            /* Get info from input SEG-Y reel header */
            itemp = ReadSegyReelHdr(in_filename2,&swap_bytes,&num_traces,&ReelHdr);
            if (itemp > 0)
            {   printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
                exit(5);
            }
            num_samps2 = ReelHdr.hns;
            itemp = ReadSegyReelHdr(in_filename1,&swap_bytes,&num_traces,&ReelHdr);
            if (itemp > 0)
            {   printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
                exit(5);
            }
            num_samps1 = ReelHdr.hns;

            if (num_samps1 != num_samps2)
			{   puts("\nGPR_DIFF: ERROR - Both input SGY files must have same number of samples");
                exit(6);
            }

            /* Allocate space for a block of traces (include skipped ones) */
            switch (ReelHdr.format)
			{   case 1:  case 2:  case 4:
                    data_block_bytes = num_samps1 * 4;
                    break;
                case 3:
                    data_block_bytes = num_samps1 * 2;
                    break;
                default:
					printf("\nGPR_DIFF - ERROR: Unvalid SEG-Y sample format code.\n");
                    exit(4);
            }
            hdr_block_bytes = sizeof(struct SegyTraceHdrStruct);
            disk_trace_size = data_block_bytes + hdr_block_bytes;
            trace1 = (void *)malloc(disk_trace_size);
            trace2 = (void *)malloc(disk_trace_size);
            if (trace1 == NULL || trace2 == NULL)
            {   free(trace1); free(trace2);
				printf("\nGPR_DIFF: ERROR - Unable to allocate storage for SGY data blocks.\n");
				exit(7);
			}

			/* Open input SGY files */
			if ((infile1 = fopen(in_filename1,"rb")) == NULL)
			{   printf("\nGPR_DIFF: ERROR - Unable to open input SGY file %s\n",in_filename1);
				if (access(in_filename1,4))
				{   if      (errno == ENOENT) puts("                  File or path name not found.");
					else if (errno == EACCES) puts("                  Read Access denied.");
					else                      puts("                  Unknown access error.");
				}
				free(trace1); free(trace2);
				exit(7);
			}
			if ((infile2 = fopen(in_filename2,"rb")) == NULL)
			{   printf("\nGPR_DIFF: ERROR - Unable to open input SGY file %s\n",in_filename2);
				if (access(in_filename2,4))
				{   if      (errno == ENOENT) puts("                  File or path name not found.");
					else if (errno == EACCES) puts("                  Read Access denied.");
					else                      puts("                  Unknown access error.");
				}
				free(trace1); free(trace2);
				fclose(infile1);
				exit(7);
			}

			/* Open output SGY file */
			if ((outfile = fopen(out_filename,"wb")) == NULL)
			{   printf("\nGPR_DIFF: ERROR - Unable to open output SGY file %s\n",out_filename);
                if (access(out_filename,2))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                free(trace1); free(trace2);
                fclose(infile1);  fclose(infile2);
                exit(7);
            }

            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
              realmalloc(inbuffer1,vbufsize);
              if (inbuffer1)  setvbuf(infile1,inbuffer1,_IOFBF,vbufsize);
              else            setvbuf(infile1,NULL,_IOFBF,vbufsize);
              realmalloc(inbuffer2,vbufsize);
              if (inbuffer2)  setvbuf(infile2,inbuffer2,_IOFBF,vbufsize);
              else            setvbuf(infile2,NULL,_IOFBF,vbufsize);
              realmalloc(outbuffer,vbufsize);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
              else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
            #endif

            /* Set/save values for output reel header */
            ExtractSsInfoFromSegy(&GprInfo,&ReelHdr);
            itemp = 0;
            if (GprInfo.proc_hist && GprInfo.proc_hist_bytes > 0)  /* if allocated */
                itemp = GprInfo.proc_hist_bytes;
            itemp += proc_hist_size + 1;
            proc_hist = (char *)malloc((size_t)itemp);
            if (proc_hist)
            {   if (GprInfo.proc_hist && GprInfo.proc_hist_bytes > 0)
                    strncpy(proc_hist,GprInfo.proc_hist,GprInfo.proc_hist_bytes);
				if (!add) sprintf(str,"\nGPR_DIFF: %s subtracted from %s\n",in_filename2,in_filename1);
				else      sprintf(str,"\nGPR_DIFF: %s added to %s\n",in_filename2,in_filename1);
                strcat(proc_hist,str);
            }
            SetSgyFileHeader((int)GprInfo.time_date_created.day,
                             (int)GprInfo.time_date_created.month,
                             (int)GprInfo.time_date_created.year+1980,
                             GprInfo.total_traces,(long)GprInfo.in_samps_per_trace,
                             (long)GprInfo.timezero_sample,
                             GprInfo.in_time_window,
                             GprInfo.starting_position,GprInfo.final_position,
                             GprInfo.position_step_size,GprInfo.position_units,
                             (double)GprInfo.nominal_frequency,
                             GprInfo.antenna_separation,GprInfo.pulser_voltage,
                             GprInfo.number_of_stacks,GprInfo.survey_mode,
                             proc_hist,GprInfo.text,GprInfo.job_number,
                             GprInfo.title1,GprInfo.title2,out_filename,
                             0.0,0.0,0,NULL,&ReelHdr);
            fwrite((void *)&ReelHdr,sizeof(struct SegyReelHdrStruct),1,outfile);
            free(proc_hist);

            /* Move the file pointer to start of first trace */
            fseek(infile1,(size_t)sizeof(struct SegyReelHdrStruct),SEEK_SET);
            fseek(infile2,(size_t)sizeof(struct SegyReelHdrStruct),SEEK_SET);

            /* Get traces from files */
			if (add)  printf("\nGPR_DIFF: Adding file %s to %s ...",in_filename2,in_filename1);
			else      printf("\nGPR_DIFF: Subtracting file %s from %s ...",in_filename2,in_filename1);
            trace_num = 0;
            while (1)
            {   if ( !(trace_num % 100) ) printf(".");
                if (fread(trace1,(size_t)disk_trace_size,(size_t)1,infile1) < 1)
                    break;  /* out of while(1) loop */
                if (fread(trace2,(size_t)disk_trace_size,(size_t)1,infile2) < 1)
                    break;  /* out of while(1) loop */
                switch (ReelHdr.format)
                {   case 1:
                        f_ptr1 = (float *)( (char *)trace1 + sizeof(struct SegyTraceHdrStruct) );
                        f_ptr2 = (float *)( (char *)trace2 + sizeof(struct SegyTraceHdrStruct) );
                        for (i=0; i<num_samps1; i++)
                        {   dtemp1 = f_ptr1[i];
                            dtemp2 = f_ptr2[i];
                            if (!add) dtemp1 -= dtemp2;
                            else      dtemp1 += dtemp2;
                            dtemp1 = _LIMIT(-FLT_MAX,dtemp1,FLT_MAX);
                            f_ptr1[i] = dtemp1;
                        }
                        break;
                    case 2:  case 4:
                        l_ptr1 = (long *)( (char *)trace1 + sizeof(struct SegyTraceHdrStruct) );
                        l_ptr2 = (long *)( (char *)trace2 + sizeof(struct SegyTraceHdrStruct) );
                        for (i=0; i<num_samps1; i++)
                        {   dtemp1 = l_ptr1[i];
                            dtemp2 = l_ptr2[i];
                            if (!add) dtemp1 -= dtemp2;
                            else      dtemp1 += dtemp2;
                            dtemp1 = _LIMIT(-2147483648.0,dtemp1,2147483647.0);
                            l_ptr1[i] = dtemp1;
                        }
                        break;
                    case 3:
                        s_ptr1 = (short *)( (char *)trace1 + sizeof(struct SegyTraceHdrStruct) );
                        s_ptr2 = (short *)( (char *)trace2 + sizeof(struct SegyTraceHdrStruct) );
                        for (i=0; i<num_samps1; i++)
                        {   dtemp1 = s_ptr1[i];
                            dtemp2 = s_ptr2[i];
                            if (!add) dtemp1 -= dtemp2;
                            else      dtemp1 += dtemp2;
                            dtemp1 = _LIMIT(-32768.0,dtemp1,32767.0);
                            s_ptr1[i] = dtemp1;
                        }
                        break;
                }  /* end of switch (ReelHdr.format) block */
                if (fwrite(trace1,(size_t)disk_trace_size,(size_t)1,outfile) < 1)
                    break;  /* out of while(1) loop */
                trace_num++;
            }  /* end of while (1) */
            fclose(infile1); fclose(infile2); fclose(outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer1); realfree(inbuffer2); realfree(outbuffer);
            #endif
            free(trace1);  free(trace2);
            printf("\nNew file %s contains %u traces\n",out_filename,trace_num);
            break;
        }  /* end of case SGY: block */
        default:
			puts("\nGPR_DIFF - ERROR: GPR file storage format is not recognized.");
            puts("           Use program GPR_CONV to convert user-defined or unrecognized");
            puts("           format to the GSSI DZT, S&S DT1/HD, or SEG-Y format.");
            exit(99);
    }  /* end of switch (file_type) block */

	puts("\nGPR_DIFF finished.");
    exit(0);
}

/******************************** ExitFunc1() *******************************/
void ExitFunc1(void)
{
    if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}

/***************************** END OF GPR_DIFF *****************************/
