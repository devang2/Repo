/******************************** GPR_REV.C*********************************\
 *                               version 1.00
 * GPR_REV reverses the traces in a digital ground penetrating radar data
 * file, ie. the last trace will be the first and the first trace will be
 * the last.
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
 *   April 29, 1994     - Completed as S10_REV.
 *   February 23, 1996  - Completed as a modification and combination of
 *                        S10_REV.C, GPR_DIFF.C, and GPR_CNDS.C.
 *   February 26, 1996  - Improved I/O error reporting.
 *   March 1, 1996      - Allow program to continue if ReadOneDztHeaderMsg
 *                        contains "WARNING".
 *   August 11, 1997    - Added support for modified DZT foemats.
 *   April 11, 2000     - Updated time and date functions
 *   May 11, 2001       - Fixed bug when calling ReadOneDztHeader(). I was
 *                        terminating prog on warning as well as error.
 *                      - Also wrong error message when insufficient command
 *                        line arguments.
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /zfloatsync gpr_rev.c gpr_io.lib jl_util1.lib
 *     To compile for 80486 executable:
 *       icc /F /xnovm /zfloatsync /zmod486 gpr_rev.c gpr_io.lib jl_util1.lib
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
 *   Borland C++ 3.1
 *     bcc -3 -a -d -f287 -ml -O2 gpr_rev.c
 *
 *     -3 generates 80386 protected mode compatible instructions
 *     -a alignes code on (2-byte) word boundaries
 *     -d merges duplicates strings
 *     -f287 invokes 80287 instructions for floating point math
 *     -ml uses the large memory model
 *     -O2 optimizes for fastest code
 *
 * To run: GPR_REV in_filename out_filename
 *
\****************************************************************************/

/********************** Include required header files ***********************/
/* non-ANSI headers */
#include <conio.h>     /* getch */
#include <errno.h>     /* errno,EACCES,ENOENT */
#include <io.h>        /* access */
#include <sys\stat.h>  /* fstat,struct stat  */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <stdio.h>     /* puts,printf,fopen,fread,fclose,FILE,NULL,_IOFBF,
						  SEEK_SET,SEEK_CUR,SEEK_END,ftell,fseek,fwrite,
						  rewind,fileno,ferror,clearerr */
#include <stdlib.h>    /* exit,malloc,free,_splitpath */
#include <string.h>    /* strcpy,strcmp,strncpy,strcat */

/* application headers */
#include "gpr_io.h"    /* to use gpr_io.lib functions, etc. */
#include "jl_defs.h"   /* common manifest constants and macros */
#include "jl_util1.h"  /* ANSI_there, MAX_PATHLEN, etc. */

/*************************** Manifest constants ****************************/
#if defined(_INTELC32_)
  #define BUFSIZE 65536L      /* size of I/O and data buffers */
#else
  #define BUFSIZE 32768L      /* size of I/O and data buffers */
#endif

#define GPR_REV_VER "1.05.11.01"
#define DATETIME "May 11, 2001"

/**************************** Global variables ******************************/
int Debug = FALSE;       /* if TRUE, turn debugging on */
int Batch = FALSE;       /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;  /* TRUE if ANSI.SYS loaded */

/*************************** Function prototypes ****************************/
void ExitFunc1(void);

/********************************** main() **********************************/
void main(int argc, char *argv[])
{
    char   in_filename[MAX_PATHLEN];
    char   out_filename[MAX_PATHLEN];
	int    itemp,min_args;

    /* following used by GetGprFileType() */
    int   file_hdr_bytes, num_hdrs, trace_hdr_bytes, samples_per_trace;
    int   num_channels, input_datatype, total_time, file_type;
    long  num_traces;

    extern int ANSI_THERE;
    extern const char *ReadOneDztHeaderMsg[]; /* gpr_io.h */

    /* Register exit function 1 (return console colors to white on black) */
    atexit(ExitFunc1);

    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Check if enough command line arguments */
    min_args = 3;
    if (argc < min_args)
    {   puts("");
        if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
        puts("###############################################################################");
        puts("  Usage: GPR_REV in_filename out_filename");
        puts("    Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
        puts("  This program reverses the traces in a GPR file.");
        puts("     Recognized storage formats include: GSSI DZT, S&S DT1/HD, and SEG-Y.");
        puts("  Required command line arguments:");
        puts("     in_filename  - The name of the input GPR data file.");
        puts("     out_filename - The name of the output (reversed) GPR data file.");
        puts("  Information from the the input file header is used for the output file");
        puts("     header.  If applicable, modification date, comments, and processing");
        puts("     history are changed.");
        puts("  Example call: GPR_REV file1.dzt file1r.dzt");
        printf("  Version %s - Last Revision Date: %s at %s\n",GPR_REV_VER,DATETIME,__TIME__);
        puts("###############################################################################");
        if (ANSI_THERE)  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
        puts("\nGPR_REV: ERROR - Insufficient command line arguments.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
        exit(1);
        }

    /* Get command line arguments */
    strcpy(in_filename,argv[1]);
    strcpy(out_filename,argv[2]);

    /* Set console colors to red on black to report problems */
    if (ANSI_THERE)  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */

    /* Check that filenames are different */
    if (strcmp(in_filename,out_filename) == 0)
    {   puts("\nGPR_REV: ERROR - Output filename must be different from input file name.");
        exit(1);
    }

    /* Determine storage format */
	itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                   &samples_per_trace,&num_channels,&num_traces,
                   &input_datatype,&total_time,&file_type,in_filename);
    if (itemp > 0)
    {   printf("\n%s\n",GetGprFileTypeMsg[itemp]);
        exit(2);
    }

    /* Set console colors to cyan on black to report progress */
    if (ANSI_there())  printf("%c%c36;40m",0x1B,0x5B);  /* cyan on black */

    /* Get the data, reverse, and save to disk */
    switch (file_type)
	{   case DZT: case MOD_DZT:
        {   char   str[400],text[400];
			int    i,j,itemp,error;
			int    num_chan,num_bits,num_samps;
			int    num_hdrs,channel,header_size,disk_chunk_size;
			long   num_traces,offset,chunks,tenth,count,filebytes1,filebytes2;
			size_t buf_size,disk_trace_size;
			size_t vbuf_size   = BUFSIZE;
			void  *v_ptr       = NULL;
			struct stat statbuf;
            struct dosdate_t dosdate;
            struct dostime_t dostime;
			struct DztHdrStruct DztHdr;
			FILE  *infile      = NULL;
			FILE  *outfile     = NULL;
			#if defined(_INTELC32_)
              char *inbuffer     = NULL;
              char *outbuffer    = NULL;
            #endif

            /* following in errno.h */
            extern int errno;

			/* Open input DZT files */
            if ((infile = fopen(in_filename,"rb")) == NULL)
			{   printf("\nGPR_REV: ERROR - Unable to open input DZT file %s\n",in_filename);
                if (access(in_filename,4))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                exit(5);
			}

			/* Open output DZT file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
			{   printf("\nGPR_REV: ERROR - Unable to open output DZT file %s\n",out_filename);
                if (access(out_filename,2))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
				}
				fclose(infile);
				exit(5);
			}

			/* Speed up disk access by using large (conventional memory)
			   buffer. NOTE: stream errors are not reported to fwrite() or
			   fread() until the buffer setup by setvbuf() is emptied!
			   If vbuf_size is greater than the filesize ferror() will
			   not report errors! */
			#if defined(_INTELC32_)
			  realmalloc(inbuffer,vbuf_size);
			  realmalloc(outbuffer,vbuf_size);
              if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbuf_size);
              else           setvbuf(infile,NULL,_IOFBF,vbuf_size);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbuf_size);
              else            setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #else
              setvbuf(infile,NULL,_IOFBF,vbuf_size);
              setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #endif

            /* Get the first header from the input file */
			channel = 0;
            itemp = ReadOneDztHeader(in_filename,&num_hdrs,&num_traces,channel,
                                     &header_size,&DztHdr);
            if (itemp > 0)
            {   printf("\n%s\n",ReadOneDztHeaderMsg[itemp]);
                if (!strstr(strupr(ReadOneDztHeaderMsg[itemp]),"WARNING"))
                {   fclose(infile); fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer); realfree(outbuffer);
                    #endif
				    exit(6);
                }
            }
            num_chan  = DztHdr.rh_nchan;
            num_bits  = DztHdr.rh_bits;
			num_samps = DztHdr.rh_nsamp;

			/* Setup/allocate RAM buffer to speed reversal */
			buf_size = vbuf_size;
			fstat(fileno(infile),&statbuf);
			filebytes1 = statbuf.st_size;
			filebytes1 -= num_chan * header_size;
			disk_trace_size = num_samps * (num_bits/8);     /* 8 bits per byte */
			disk_chunk_size = num_chan * disk_trace_size;
			if (filebytes1 < buf_size)  buf_size = filebytes1;
			else                       buf_size = (buf_size/disk_chunk_size) * disk_chunk_size;
			while ((buf_size>=disk_chunk_size) && ((v_ptr=malloc(buf_size))==NULL))
				buf_size -= disk_chunk_size;
			if (v_ptr == NULL || buf_size < disk_chunk_size)
			{   puts("\nGPR_REV: ERROR - Unable to allocate temporary storage.");
				fclose(infile); fclose(outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);  realfree(outbuffer);
				#endif
				free(v_ptr);
				exit(6);
			}
			offset = -2 * buf_size;
			if (filebytes1 % buf_size)  chunks = filebytes1/buf_size + 1;
			else                        chunks = filebytes1/buf_size;

            /* Copy header(s) to disk */
            rewind(outfile);
            for (i=0; i<num_chan; i++)
            {   /* get header from input file */
                channel = i;
                itemp = ReadOneDztHeader(in_filename,&num_hdrs,&num_traces,channel,
									 &header_size,&DztHdr);
                if (itemp > 0)
                {   printf("\n%s\n",ReadOneDztHeaderMsg[itemp]);
                    if (!strstr(strupr(ReadOneDztHeaderMsg[itemp]),"WARNING"))
                    {   fclose(infile); fclose(outfile);
                        #if defined(_INTELC32_)
                          realfree(inbuffer); realfree(outbuffer);
                        #endif
                        free(v_ptr);
	    			    exit(6);
                    }
                }

                /* update modification time/date */
                _dos_getdate(&dosdate);
                _dos_gettime(&dostime);
                DztHdr.rh_modif.month = dosdate.month;       /* 1 - 12 */
                DztHdr.rh_modif.day   = dosdate.day;         /* 1 - 31 */
                DztHdr.rh_modif.year  = dosdate.year - 1980; /* year */
                DztHdr.rh_modif.hour  = dostime.hour;        /* 0 - 24 */
                DztHdr.rh_modif.min   = dostime.minute;
                DztHdr.rh_modif.sec2  = dostime.second/2;

                /* change file name */
                {   char drive[80],dir[80],fname[80],ext[80];

                    _splitpath(out_filename,drive,dir,fname,ext);
					EditDztHeader(26,(void *)fname,&DztHdr);
                }

                /* change comment string */
                if (i == 0)
                {   sprintf(str,"\nGPR_REV: traces in %s reversed\n",in_filename);
                    strncpy(text,(char *)&DztHdr + DztHdr.rh_text,sizeof(text)-1);
					if (strlen(text) < sizeof(text)-strlen((char *)str)-2)
						strcat(text,(char *)str);
                    EditDztHeader(30,(void *)text,&DztHdr);
				}

				/* write header to output file */
				if (fwrite((void *)&DztHdr,sizeof(struct DztHdrStruct),(size_t)1,outfile) < 1)
				{   printf("\nGPR_REV: ERROR - short count from fwrite() to file %s\n",out_filename);
					fclose(infile); fclose(outfile);
					#if defined(_INTELC32_)
					  realfree(inbuffer);  realfree(outbuffer);
					#endif
					free(v_ptr);
					exit(6);
				}
			}

			/* Read in traces and transfer in reverse order */
			if ((tenth=chunks/10) < 1)  tenth = 1;
			count = 0;
			error = FALSE;
			printf("GPR_REV: Reversing scan order ");
			fseek(infile,-buf_size,SEEK_END);
			for (i=0; i<chunks; i++)
			{   if ((i%tenth) == 0) printf(".");      /* indicate progress */
				if (fread(v_ptr,buf_size,(size_t)1,infile) < 1)
				{   error = TRUE;
					break;                /* out of for (i...) loop */
				}
				for (j=buf_size-disk_chunk_size; j>=0; j-=disk_chunk_size, count++)
					if (fwrite((void *)((char *)v_ptr+j),disk_chunk_size,(size_t)1,outfile) < 1)
					{   error = TRUE;
						break;            /* out of for (j...) loop */
					}
				if (error == TRUE) break;  /* out of for (i...) loop */
				if (ferror(infile) || ferror(outfile))
				{   error = TRUE;
					break;
				}
				if (-offset > ftell(infile))
				{   fseek(infile,-buf_size,SEEK_CUR);
					buf_size = ftell(infile);
					buf_size -= num_chan * header_size;
					fseek(infile,num_chan * header_size,SEEK_SET);
				} else
					fseek(infile,offset,SEEK_CUR);
			}
			if (error == TRUE)
			{   printf("\nGPR_REV - ERROR: I/O error reported at chunk %d, trace %d\n",i,count);
				puts("    WARNING: Reverse operation may not be complete!");
				clearerr(infile);  clearerr(outfile);
				fclose(infile); fclose(outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);  realfree(outbuffer);
				#endif
				exit(7);
			}

			printf("\nGPR_REV: %ld trace chunks transferred to %s\n",count,out_filename);
            fclose(infile); fclose(outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);  realfree(outbuffer);
            #endif
            free(v_ptr);

			stat(in_filename,&statbuf);
			filebytes1 = statbuf.st_size;
			stat(out_filename,&statbuf);
			filebytes2 = statbuf.st_size;
			if (filebytes1 != filebytes2)
			{   puts("\nGPR_REV - ERROR: Sizes of input and output files do not match.");
				puts("    WARNING: Reverse operation may not be complete!");
				exit(10);
			} else
				printf("\nGPR_REV: %ld traces transferred to %s\n",count,out_filename);

			break;
		}  /* end of case DZT: block */
		case DT1:
		{   char   hd_infilename[MAX_PATHLEN],hd_outfilename[MAX_PATHLEN];
			char  *c_ptr,*proc_hist,str[160];
			int    proc_hist_size = 240;
			int    i,j,itemp,print_it,error;
			int    num_samps;
			int    data_block_bytes,hdr_block_bytes;
			long   offset,chunks,tenth,count,filebytes1,filebytes2;
			size_t buf_size,disk_trace_size;
			size_t vbuf_size   = BUFSIZE;
			void  *v_ptr       = NULL;
			struct stat statbuf;
			struct SsTraceHdrStruct *TraceHdr;
			struct SsHdrInfoStruct HdInfo;
            FILE  *infile      = NULL;
			FILE  *outfile     = NULL;
			#if defined(_INTELC32_)
			  char *inbuffer   = NULL;
			  char *outbuffer  = NULL;
            #endif

            /* following in gpr_io.h */
            extern const char *GetSsHdFileMsg[];

            /* Construct HD filenames */
			hd_infilename[0] = 0;
			strcpy(hd_infilename,in_filename);
            if ((c_ptr=strchr(hd_infilename,'.')) != NULL)
                *c_ptr = 0;               /* terminated string at period */
            strcat(hd_infilename,".hd");

            hd_outfilename[0] = 0;
            strcpy(hd_outfilename,out_filename);
            if ((c_ptr=strchr(hd_outfilename,'.')) != NULL)
                *c_ptr = 0;               /* terminated string at period */
			strcat(hd_outfilename,".hd");

            /* Get info from input HD files */
			InitDt1Parameters(&HdInfo);
            if (Debug) print_it = TRUE;
            else       print_it = FALSE;
            itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename);
            if (itemp > 0)
            {   printf("\n%s\n",GetSsHdFileMsg[itemp]);
                exit(5);
            }
            num_samps = HdInfo.num_samples;

            /* Open input DT1 file */
            if ((infile = fopen(in_filename,"rb")) == NULL)
            {   printf("\nGPR_REV: ERROR - Unable to open input DT1 file %s\n",in_filename);
				if (access(in_filename,4))
				{   if      (errno == ENOENT) puts("                  File or path name not found.");
					else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                exit(7);
			}

            /* Open output DT1 file */
			if ((outfile = fopen(out_filename,"wb")) == NULL)
			{   printf("\nGPR_REV: ERROR - Unable to open output DT1 file %s\n",out_filename);
				if (access(out_filename,2))
				{   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
					else                      puts("                  Unknown access error.");
				}
				fclose(infile);
				exit(7);
			}

			/* Speed up disk access by using large (conventional memory)
			   buffer. NOTE: stream errors are not reported to fwrite() or
			   fread() until the buffer setup by setvbuf() is emptied!
			   If vbuf_size is greater than the filesize ferror() will
			   not report errors! */

			#if defined(_INTELC32_)
			  realmalloc(inbuffer,vbuf_size);
			  realmalloc(outbuffer,vbuf_size);
			  if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbuf_size);
			  else           setvbuf(infile,NULL,_IOFBF,vbuf_size);
			  if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbuf_size);
			  else            setvbuf(outfile,NULL,_IOFBF,vbuf_size);
			#else
			  setvbuf(infile,NULL,_IOFBF,vbuf_size);
			  setvbuf(outfile,NULL,_IOFBF,vbuf_size);
			#endif

			/* Setup/allocate RAM buffer to speed reversal */
			buf_size = vbuf_size;
			fstat(fileno(infile),&statbuf);
			filebytes1 = statbuf.st_size;
			data_block_bytes = num_samps * 2; /* data are signed short ints */
			hdr_block_bytes = sizeof(struct SsTraceHdrStruct);
			disk_trace_size = data_block_bytes + hdr_block_bytes;
			if (filebytes1 < buf_size)  buf_size = filebytes1;
			else                       buf_size = (buf_size/disk_trace_size) * disk_trace_size;
			while ((buf_size>=disk_trace_size) && ((v_ptr=malloc(buf_size))==NULL))
				buf_size -= disk_trace_size;
			if (v_ptr == NULL || buf_size < disk_trace_size)
			{   puts("\nGPR_REV: ERROR - Unable to allocate temporary storage.");
				fclose(infile); fclose(outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);  realfree(outbuffer);
				#endif
				free(v_ptr);
				exit(6);
			}
			offset = -2 * buf_size;
			if (filebytes1 % buf_size)  chunks = filebytes1/buf_size + 1;
			else                        chunks = filebytes1/buf_size;

			/* Set/save values for output header */
			if (HdInfo.proc_hist)  /* if allocated */
				itemp = strlen(HdInfo.proc_hist) + 1;
			itemp += proc_hist_size + 1;
			proc_hist = (char *)malloc((size_t)itemp);
			if (proc_hist)
			{   if (HdInfo.proc_hist)  /* allocated */
					strcpy(proc_hist,HdInfo.proc_hist);
                sprintf(str,"\nGPR_REV: traces in %s reversed\n",in_filename);
                strcat(proc_hist,str);
            }
            itemp = SaveSsHdFile((int)HdInfo.day,(int)HdInfo.month,(int)HdInfo.year,
								(long)HdInfo.num_traces,(long)HdInfo.num_samples,
                                (long)HdInfo.time_zero_sample,(int)HdInfo.total_time_ns,
                                (double)HdInfo.final_pos,(double)HdInfo.start_pos, /* reversed */
								(-(double)HdInfo.step_size),HdInfo.pos_units,
                                (double)HdInfo.ant_freq,(double)HdInfo.ant_sep,
								(double)HdInfo.pulser_voltage,
								(int)HdInfo.num_stacks,HdInfo.survey_mode,
								proc_hist,hd_outfilename,out_filename,
								HdInfo.job_number,HdInfo.title1,HdInfo.title2,
								0.0,0.0,0,NULL);
			free(proc_hist);

			/* Read in traces and transfer in reverse order */
			if ((tenth=chunks/10) < 1)  tenth = 1;
			count = 0;
			error = FALSE;
			printf("GPR_REV: Reversing scan order ");
			fseek(infile,-buf_size,SEEK_END);
			for (i=0; i<chunks; i++)
			{   if ((i%tenth) == 0) printf(".");      /* indicate progress */
				if (fread(v_ptr,buf_size,(size_t)1,infile) < 1)
				{   error = TRUE;
					break;                /* out of for (i...) loop */
				}
				for (j=buf_size-disk_trace_size; j>=0; j-=disk_trace_size, count++)
				{	/* update trace header */
					TraceHdr = (struct SsTraceHdrStruct *)((char *)v_ptr+j);
					TraceHdr->aux_trace_num = TraceHdr->trace_num = count;
					if (fwrite((void *)((char *)v_ptr+j),disk_trace_size,(size_t)1,outfile) < 1)
					{   error = TRUE;
						break;            /* out of for (j...) loop */
					}
				}
				if (error == TRUE) break;  /* out of for (i...) loop */
				if (ferror(infile) || ferror(outfile))
				{   error = TRUE;
					break;
				}
				if (-offset > ftell(infile))
				{   fseek(infile,-buf_size,SEEK_CUR);
					buf_size = ftell(infile);
					fseek(infile,0L,SEEK_SET);
				} else
					fseek(infile,offset,SEEK_CUR);
			}
			if (error == TRUE)
			{   printf("\nGPR_REV - ERROR: I/O error reported at chunk %d, trace %d\n",i,count);
				puts("    WARNING: Reverse operation may not be complete!");
				clearerr(infile);  clearerr(outfile);
				fclose(infile); fclose(outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);  realfree(outbuffer);
				#endif
				exit(7);
			}

			fclose(infile); fclose(outfile);
			#if defined(_INTELC32_)
			  realfree(inbuffer);  realfree(outbuffer);
			#endif
			free(v_ptr);

			stat(in_filename,&statbuf);
			filebytes1 = statbuf.st_size;
			stat(out_filename,&statbuf);
			filebytes2 = statbuf.st_size;
			if (filebytes1 != filebytes2)
			{   puts("\nGPR_REV - ERROR: Sizes of input and output files do not match.");
				puts("    WARNING: Reverse operation may not be complete!");
				exit(10);
			} else
				printf("\nGPR_REV: %ld traces transferred to %s\n",count,out_filename);

			break;
		}  /* end of case DT1: block */
		case SGY:
		{   char  *proc_hist,str[80];
			int    proc_hist_size = 240;
			int    i,j,itemp,error;
			int    data_block_bytes,hdr_block_bytes,num_samps,swap_bytes;
			long   num_traces;
			long   offset,chunks,tenth,count,filebytes1,filebytes2;
			struct SegyReelHdrStruct ReelHdr;
			struct SegyTraceHdrStruct *TraceHdr;
			struct GprInfoStruct GprInfo;
			struct stat statbuf;
			size_t buf_size,disk_trace_size;
			size_t vbuf_size   = BUFSIZE;
			void  *v_ptr       = NULL;
			FILE  *infile      = NULL;
			FILE  *outfile     = NULL;
			#if defined(_INTELC32_)
			  char *inbuffer   = NULL;
			  char *outbuffer  = NULL;
			#endif

			/* following in gpr_io.h */
			extern const char *ReadSegyReelHdrMsg[];

			/* Get info from input SEG-Y reel header */
			itemp = ReadSegyReelHdr(in_filename,&swap_bytes,&num_traces,&ReelHdr);
			if (itemp > 0)
			{   printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
				exit(5);
			}
			num_samps = ReelHdr.hns;

			/* Open input SGY file */
			if ((infile = fopen(in_filename,"rb")) == NULL)
			{   printf("\nGPR_REV: ERROR - Unable to open input SGY file %s\n",in_filename);
				if (access(in_filename,4))
				{   if      (errno == ENOENT) puts("                  File or path name not found.");
					else if (errno == EACCES) puts("                  Read Access denied.");
                    else                      puts("                  Unknown access error.");
                }
				exit(7);
            }

			/* Open output SGY file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
            {   printf("\nGPR_REV: ERROR - Unable to open output SGY file %s\n",out_filename);
                if (access(out_filename,2))
				{   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
				}
				fclose(infile);
				exit(7);
			}

			/* Speed up disk access by using large (conventional memory)
			   buffer. NOTE: stream errors are not reported to fwrite() or
			   fread() until the buffer setup by setvbuf() is emptied!
			   If vbuf_size is greater than the filesize ferror() will
			   not report errors! */
			#if defined(_INTELC32_)
			  realmalloc(inbuffer,vbuf_size);
			  realmalloc(outbuffer,vbuf_size);
			  if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbuf_size);
			  else           setvbuf(infile,NULL,_IOFBF,vbuf_size);
			  if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbuf_size);
			  else            setvbuf(outfile,NULL,_IOFBF,vbuf_size);
			#else
			  setvbuf(infile,NULL,_IOFBF,vbuf_size);
			  setvbuf(outfile,NULL,_IOFBF,vbuf_size);
			#endif

			/* Setup/allocate RAM buffer to speed reversal */
			buf_size = vbuf_size;
			hdr_block_bytes = sizeof(struct SegyTraceHdrStruct);
			fstat(fileno(infile),&statbuf);
			filebytes1 = statbuf.st_size;
			filebytes1 -= hdr_block_bytes;
			switch (ReelHdr.format)
			{   case 1:  case 2:  case 4:
					data_block_bytes = num_samps * 4;
					break;
				case 3:
					data_block_bytes = num_samps * 2;
					break;
				default:
					printf("\nGPR_REV - ERROR: Unvalid SEG-Y sample format code.\n");
					exit(4);
			}
			disk_trace_size = data_block_bytes + hdr_block_bytes;
			if (filebytes1 < buf_size)  buf_size = filebytes1;
			else                       buf_size = (buf_size/disk_trace_size) * disk_trace_size;
			while ((buf_size>=disk_trace_size) && ((v_ptr=malloc(buf_size))==NULL))
				buf_size -= disk_trace_size;
			if (v_ptr == NULL || buf_size < disk_trace_size)
			{   puts("\nGPR_REV: ERROR - Unable to allocate temporary storage.");
				fclose(infile); fclose(outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);  realfree(outbuffer);
				#endif
				free(v_ptr);
				exit(6);
			}
			offset = -2 * buf_size;
			if (filebytes1 % buf_size)  chunks = filebytes1/buf_size + 1;
			else                        chunks = filebytes1/buf_size;

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
				sprintf(str,"\nGPR_REV: traces in %s reversed\n",in_filename);
				strcat(proc_hist,str);
			}
			SetSgyFileHeader((int)GprInfo.time_date_created.day,
							 (int)GprInfo.time_date_created.month,
							 (int)GprInfo.time_date_created.year+1980,
							 GprInfo.total_traces,(long)GprInfo.in_samps_per_trace,
							 (long)GprInfo.timezero_sample,
							 GprInfo.in_time_window,
							 GprInfo.final_position,GprInfo.starting_position,  /* reversed */
							 (-GprInfo.position_step_size),GprInfo.position_units,
							 (double)GprInfo.nominal_frequency,
							 GprInfo.antenna_separation,GprInfo.pulser_voltage,
							 GprInfo.number_of_stacks,GprInfo.survey_mode,
							 proc_hist,GprInfo.text,GprInfo.job_number,
							 GprInfo.title1,GprInfo.title2,out_filename,
							 0.0,0.0,0,NULL,&ReelHdr);
			fwrite((void *)&ReelHdr,sizeof(struct SegyReelHdrStruct),1,outfile);
			free(proc_hist);

			/* Read in traces and transfer in reverse order */
			if ((tenth=chunks/10) < 1)  tenth = 1;
			count = 0;
			error = FALSE;
			printf("GPR_REV: Reversing scan order ");
			fseek(infile,-buf_size,SEEK_END);
			for (i=0; i<chunks; i++)
			{   if ((i%tenth) == 0) printf(".");      /* indicate progress */
				if (fread(v_ptr,buf_size,(size_t)1,infile) < 1)
				{   error = TRUE;
					break;                /* out of for (i...) loop */
				}
				for (j=buf_size-disk_trace_size; j>=0; j-=disk_trace_size, count++)
				{	/* update trace header */
					TraceHdr = (struct SegyTraceHdrStruct *)((char *)v_ptr+j);
					TraceHdr->tracl = TraceHdr->tracr = TraceHdr->fldr = count;
					if (fwrite((void *)((char *)v_ptr+j),disk_trace_size,(size_t)1,outfile) < 1)
					{   error = TRUE;
						break;            /* out of for (j...) loop */
					}
				}
				if (error == TRUE) break;  /* out of for (i...) loop */
				if (ferror(infile) || ferror(outfile))
				{   error = TRUE;
					break;
				}
				if (-offset > ftell(infile))
				{   fseek(infile,-buf_size,SEEK_CUR);
					buf_size = ftell(infile);
					buf_size -= sizeof(struct SegyReelHdrStruct);
					fseek(infile,(long)sizeof(struct SegyReelHdrStruct),SEEK_SET);
				} else
					fseek(infile,offset,SEEK_CUR);
			}
			if (error == TRUE)
			{   printf("\nGPR_REV - ERROR: I/O error reported at chunk %d, trace %d\n",i,count);
				puts("    WARNING: Reverse operation may not be complete!");
				clearerr(infile);  clearerr(outfile);
				fclose(infile); fclose(outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);  realfree(outbuffer);
				#endif
				exit(7);
			}

			fclose(infile); fclose(outfile);
			#if defined(_INTELC32_)
			  realfree(inbuffer);  realfree(outbuffer);
			#endif
			free(v_ptr);

			stat(in_filename,&statbuf);
			filebytes1 = statbuf.st_size;
			stat(out_filename,&statbuf);
			filebytes2 = statbuf.st_size;
			if (filebytes1 != filebytes2)
			{   puts("\nGPR_REV - ERROR: Sizes of input and output files do not match.");
				puts("    WARNING: Reverse operation may not be complete!");
				exit(10);
			} else
				printf("\nGPR_REV: %ld traces transferred to %s\n",count,out_filename);

			break;
		}  /* end of case SGY: block */
		default:
			puts("\nGPR_REV - ERROR: GPR file storage format is not recognized.");
			puts("           Use program GPR_CONV to convert user-defined or unrecognized");
			puts("           format to the GSSI DZT, S&S DT1/HD, or SEG-Y format.");
			exit(99);
	}  /* end of switch (file_type) block */

	puts("\nGPR_REV finished.");
	exit(0);
}
/******************************** ExitFunc1() *******************************/
void ExitFunc1(void)
{
	if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}

/****************************** END OF GPR_REV *****************************/
