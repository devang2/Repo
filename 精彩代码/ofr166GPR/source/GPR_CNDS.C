/******************************** GPR_CNDS.C ********************************\
 *                               version 1.00
 * GPR_CNDS reduces the size of digital ground penetrating radar data by
 * eliminating traces.  A beginning trace and end trace must be specified
 * and, optionally, the number of traces to skip for every one read.  The
 * following computer storage formats are recognized.
 *
 *   GSSI SIR-10A version 3.x to 5.x binary files, 8-bit or 16-bit unsigned
 *     integers with an embedded descriptive 512- or 1024-byte header.
 *     Data files usually have the extension "DZT".
 *   Sensors and Software pulseEKKO binary files, 16-bit signed integers
 *     accompanied by a descriptive ASCII "HD" file.  Data files must have
 *     the extensiom "DT1".
 *   SEG-Y formatted files.  Data files usually have the extension "SGY".
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
 *  February 14, 1996  - Completed as a modification and combination of
 *                       GPR_PROC.C and GPR_RHDR.C
 *  May 23, 1996       - ...in_time_window changed from int to double.
 *  September 17, 1996 - corrected way markers preserved for DZT files.
 *  August 11, 1997    - Added support for modified DZT formats.
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /zfloatsync gpr_cnds.c gpr_io.lib jl_util1.lib
 *     To compile for 80486 executable:
 *       icc /F /xnovm /zfloatsync /zmod486 gpr_cnds.c gpr_io.lib jl_util1.lib
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
 * To run: GPR_CNDS in_filename out_filename start end [skip /d /b]
 *
\****************************************************************************/

/********************** Include required header files ***********************/
/* non-ANSI headers */
#include <conio.h>     /* getch */
#include <errno.h>     /* errno */
#include <io.h>        /* access */

/* ANSI-compatible headers */
#include <ctype.h>     /* isdigit */
#include <stdio.h>     /* puts,printf,ferror,clearerr,sprintf,fopen,fclose,
                          fseek,setvbuf */
#include <stdlib.h>    /* atoi */
#include <string.h>    /* strcpy,strupr,strstr,strcat,strchr,strncpy */
#include <time.h>      /* struct tm,time_t,localtime */

/* application headers */
#include "gpr_io.h"    /* to use gpr_io.lib functions, etc. */
#include "jl_defs.h"   /* common manifest constants and macros */
#include "jl_util1.h"  /* ANSI_there, etc. */

/**************************** Global variables ******************************/
int Debug = FALSE;     /* if TRUE, turn debugging on */
int Batch = FALSE;     /* if TRUE, supress interaction with user */

/********************************** main() **********************************/
void main (int argc,char *argv[])
{
	char   in_filename[MAX_PATHLEN],out_filename[MAX_PATHLEN];
	int    itemp, min_args;
	int    start, end, skip;
	int    file_hdr_bytes, num_hdrs, trace_hdr_bytes, samples_per_trace;
	int    num_channels, input_datatype, total_time, file_type;
	long   num_traces;

	/* following in errno.h */
	extern int errno;

	/* Verify usage */
	min_args = 5;
	if (argc < min_args)
	{   puts("\n");
		if (ANSI_there())  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
		puts("");
		puts("###############################################################################");
		puts("   Usage: GPR_CNDS in_filename out_filename start end [skip /d /b]");
		puts("     This program reduces the size of GPR files.");
		puts("     Recognized storage formats include: GSSI DZT, S&S DT1/HD, and SEG-Y.");
		puts("\n   Required command line arguments:");
		puts("     in_filename  - The name of the input GPR data file.");
		puts("     out_filename - The name of the output GPR data file.");
		puts("     start        - The first trace to use from the file (indexed from 0).");
		puts("                    If -1, then start with the first trace in the file.");
		puts("     end          - The last trace to use from the file (indexed from 0).");
		puts("                    If -1, then end with the last trace in the file.");
		puts("   Optional command line arguments (do not include brackets):");
		puts("     skip         - The number of traces to skip for every one saved.");
		puts("                    Markers are preserved for DZT files.");
		puts("                    Trace numbers are updated for DT1 and SGY files.");
		puts("     /d - turn debugging on.");
		puts("     /b - turn batch processing on (only channel 0 available for DZT files).");
		puts(" Examples: gpr_cnds file1.dzt newfile1.dzt -1 -1 2");
		puts("           gpr_cnds file1.dzt newfile1.dzt 53 870");
		printf(" Version 1.00 - Last Revision Date: %s at %s\n",__DATE__,__TIME__);
		puts("###############################################################################");
		if (ANSI_there()) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
		exit(1);
	}

	/* Get command line arguments */
	strcpy(in_filename,argv[1]);
	strcpy(out_filename,argv[2]);
	start = atoi(argv[3]);
	end   = atoi(argv[4]);
	skip  = 0;
	for (itemp = min_args; itemp<argc; itemp++)
	{   if (strstr(strupr(argv[itemp]),"/D"))  Debug = TRUE;
		if (strstr(strupr(argv[itemp]),"/B"))  Batch = TRUE;
		if (isdigit(argv[itemp][0]))           skip = atoi(argv[itemp]);
	}

	/* Check input and output filenames different */
	if (strcmp(strupr(in_filename),strupr(out_filename)) == 0)
	{   printf("\nGPR_CNDS - ERROR: input and output filenames are the same.\n");
		exit(2);
	}

	/* Determine storage format */
	itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
				   &samples_per_trace,&num_channels,&num_traces,
				   &input_datatype,&total_time,&file_type,in_filename);
	if (itemp > 0)
	{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
		printf("%s\n",GetGprFileTypeMsg[itemp]);
		if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
	}

	/* Get the data, condense, and save to disk */
	printf("start = %d  end = %d  skip = %d\n",start,end,skip);
	printf("Condensing file %s to %s ...",in_filename,out_filename);
	switch (file_type)
	{   case DZT:  case MOD_DZT:
		{   char text[600], str[300];
			int  max_proc_hist_size = 600;
			unsigned char proc_hist[600];
			char name[12], antname[14], drive[_MAX_DRIVE], dir[_MAX_DIR];
			char fname[_MAX_FNAME], fext[_MAX_EXT];
			int  i,itemp,num_hdrs,header_size,channel,selection,dot,icount;
			int mark_set;
			unsigned short rh_nchan,rg_breaks,rh_nproc;
			long num_traces,trace,offset,trace_offset,rh_dtype;
			long disk_chunk_bytes,disk_trace_bytes,block_size;
			float rg_values[8];  /* this limit may change in future versions */
			void *vptr;
			FILE  *infile,*outfile;
			struct DztDateStruct create_date;
			struct DztHdrStruct hdr;
			struct tm *newtime;
			time_t long_time;
			#if defined(_INTELC32_)
			  char *inbuffer = NULL;
			  char *outbuffer = NULL;
			  size_t vbufsize = 64512;  /* 63 KB */
			#endif
			char *month[12] = { "January","February","March","April","May",
								"June","July","August","September","October",
								"November","December" } ;

			/* following in gpr_io.h */
			extern const char *ReadOneDztHeaderMsg[];

			/* Get first header from the input file */
			channel = 0;
			itemp = ReadOneDztHeader(in_filename,&num_hdrs,&num_traces,channel,
							 &header_size,&hdr);
			if (itemp > 0)
			{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
				printf("%s\n",ReadOneDztHeaderMsg[itemp]);
				if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
				if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
					exit(3);
			}

			/* If more than one header ask user to select */
			if (num_hdrs > 1 && !Batch)
			{   puts("\n\nSelect SIR-10A header to use.");
				do
				{   printf("\r                                    \r");
					printf("Enter a number from 1 to %d --->",num_hdrs);
					selection = getch();
					selection -= '0';
				} while ((selection < 1) || (selection > num_hdrs));
				puts("");
			} else
				selection = 1;

			/* Get header again if selection not first one */
			channel = selection - 1;
			if (channel > 0)
			{   itemp = ReadOneDztHeader(in_filename,&num_hdrs,&num_traces,channel,
										 &header_size,&hdr);
				if (itemp > 0)
				{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
					printf("%s\n",ReadOneDztHeaderMsg[itemp]);
					if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
					if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
						exit(3);
				}
			}

			/* Check limits of arguments */
			if (start <= 0) start = 0;
			if (end <= 0 || end <= start || end >= num_traces)
				end = num_traces - 1;
			if ((end - start) < 1)
			{   printf("GPR_CNDS - ERROR: (end - start) is less than 1.\n");
				exit(4);
			}
			if (skip <= 0)  skip = 0;
			if (skip >= (end-start)) skip = end - start - 1;
			skip++;

			/* Allocate space for a block of traces (include skipped ones) */
			disk_trace_bytes = (hdr.rh_bits/8) * hdr.rh_nsamp;
			disk_chunk_bytes = disk_trace_bytes * hdr.rh_nchan;
			block_size = disk_chunk_bytes * skip;
			if ((vptr = (void *)malloc(block_size)) == NULL)
			{   puts("GPR_CNDS - ERROR: unable to allocate storage for one chunk of DZT traces.");
				exit(4);
			}

			/* Open input file */
			if ((infile = fopen(in_filename,"rb")) == NULL)
			{   printf("GPR_CNDS - ERROR: Unable to open input file %s\n",in_filename);
				if (access(in_filename,4))
				{   if      (errno == ENOENT) puts("File or path name not found.");
					else if (errno == EACCES) puts("Read Access denied.");
					else                      puts("Unknown access error.");
				}
				free(vptr);
				exit(5);
			}
			/* Speed up disk access by using large (conventional memory) buffer */
			#if defined(_INTELC32_)
			  realmalloc(inbuffer,vbufsize);
			  if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbufsize);
			  else           setvbuf(infile,NULL,_IOFBF,vbufsize);
			#endif

			/* Open output file */
			if ((outfile = fopen(out_filename,"wb")) == NULL)
			{   printf("GPR_CNDS - ERROR: Unable to open output file %s\n",out_filename);
				if (access(out_filename,4))
				{   if      (errno == ENOENT) puts("File or path name not found.");
					else if (errno == EACCES) puts("Read Access denied.");
					else                      puts("Unknown access error.");
				}
				fclose(infile);
				free(vptr);
				exit(5);
			}
			/* Speed up disk access by using large (conventional memory) buffer */
			#if defined(_INTELC32_)
			  realmalloc(outbuffer,vbufsize);
			  if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
			  else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
			#endif

			/* Set/copy values for output header */
			if (hdr.rh_dtype == 0 && hdr.rh_bits != 8)
			{   if (hdr.rh_bits == 16)      rh_dtype = 0x0002;
				else if (hdr.rh_bits == 32) rh_dtype = 0x0004;
				else                        rh_dtype = 0x0000;
			}
			rg_breaks = *(unsigned short *) ((char *)&hdr + hdr.rh_rgain);
			for (i=0; i<rg_breaks && i<8; i++)
				rg_values[i] = *(float *) ((char *)&hdr + hdr.rh_rgain + 2 + 4*i);
			/* copy/update text/comment string */
			strncpy(text,(char *)&hdr + hdr.rh_text,sizeof(text)-1);
			time( &long_time );                 /* get time as long integer */
			newtime = localtime( &long_time );  /* convert to local time */
			sprintf(str,"  file=%s start=%d end=%d skip=%d\n",
				in_filename,start,end,skip-1);
			sprintf((char *)proc_hist,"\nGPR_CNDS v.1.00 (USGS-JL) on %s %d, %d at %d:%02d ->\n",
					month[newtime->tm_mon],newtime->tm_mday,newtime->tm_year+1900,
					newtime->tm_hour,newtime->tm_min);
			strcat((char *)proc_hist,str);
			if (strlen(text) < sizeof(text)-strlen((char *)proc_hist)-2)
				strcat(text,(char *)proc_hist);

			/* copy proc hist */
			rh_nproc = hdr.rh_nproc;
			if (rh_nproc > max_proc_hist_size-1) rh_nproc = max_proc_hist_size-1;
			for (i=0; i<rh_nproc; i++)
				proc_hist[i] = *((char *)&hdr + hdr.rh_proc + i);
			proc_hist[i] = 0;  /* counts on i being past for-loop limits */
			/* set rest */
			rh_nchan = 1;
			_splitpath(out_filename,drive,dir,fname,fext);
			strcpy(name,fname);
			strncpy(antname,hdr.rh_antname,sizeof(antname)-1);
			antname[sizeof(antname)-1] = 0;
			create_date = hdr.rh_create;
			/* Note: Do not send pointers from hdr through this function!
					 SetDzt5xHeader() clears structure arrays before writing to
					 them! Other members are OK as they are sent by value.
			 */
			SetDzt5xHeader(selection,hdr.rh_nsamp,hdr.rh_bits,hdr.rh_zero,hdr.rh_sps,
				 hdr.rh_spm,hdr.rh_mpm,hdr.rh_position,hdr.rh_range,
				 hdr.rh_npass,rh_nchan,hdr.rh_epsr,hdr.rh_top,hdr.rh_depth,
				 rh_dtype,antname,name,text,rg_breaks,rg_values,rh_nproc,
				 proc_hist,&create_date,&hdr);

			/* Copy header to output file */
			fwrite(&hdr,sizeof(struct DztHdrStruct),1,outfile);

			/* Move input file pointer to first trace chunk to retrieve */
			offset = (num_hdrs * header_size) +
					 (start * disk_chunk_bytes);
			fseek(infile,offset,SEEK_SET);
			trace_offset = disk_trace_bytes * channel;

			/* Condense file.  File pointer must be at start of first chunk
			   to get.  Read in desired trace chunk plus skipped ones. */
			dot = (end-start)/skip/10;
			if (dot < 1) dot = 1;
			icount = 0;
			trace = start;
			for (trace=start; trace<=end; trace+=skip,icount++)
			{   if (!(icount % dot)) printf(".");
				if (end - trace > skip)    /* far enough from end? */
				{   fread(vptr,block_size,1,infile);
				} else                     /* get end trace */
				{   offset = (num_hdrs * header_size) +
							  (end * disk_chunk_bytes) + trace_offset ;
					fseek(infile,offset,SEEK_SET);
					fread(vptr,disk_trace_bytes,1,infile);
				}

				/* check for markers */
				mark_set = 0;
				switch (hdr.rh_bits)
				{   case 8:
					{   unsigned char *uc_ptr;

						for (i=0; i<skip; i++)
						{   /* point to start of each chunk of traces */
							uc_ptr = (unsigned char *)vptr + (i * disk_chunk_bytes);
							if (uc_ptr[1] == 0xE8) /* check second sample */
							{   mark_set = 1;
								break;  /* out of for loop */
							}
						}
						/* point to start of trace to save */
						uc_ptr = (unsigned char *)vptr + trace_offset;
						uc_ptr[0] = 0xFF;
						if (mark_set == 1) uc_ptr[1] = 0xE8;
						else               uc_ptr[1] = 0xF1;
						break;
					}
					case 16:
					{   unsigned short *us_ptr;

						for (i=0; i<skip; i++)
						{   /* point to start of each chunk of traces */
							us_ptr = (unsigned short *)((char *)vptr + (i * disk_chunk_bytes));
							if (us_ptr[1] == 0xE800) /* check second sample */
							{   mark_set = 1;
								break;  /* out of for loop */
							}
						}
						/* point to start of trace to save */
						us_ptr = (unsigned short *)((char *)vptr + trace_offset);
						us_ptr[0] = 0xFFFF;
						if (mark_set == 1) us_ptr[1] = 0xE800;
						else               us_ptr[1] = 0xF100;
						break;
					}
					case 32:
					{   unsigned long *ul_ptr;

						for (i=0; i<skip; i++)
						{   /* point to start of each chunk of traces */
							ul_ptr = (unsigned long *)((char *)vptr + (i * disk_chunk_bytes));
							if (ul_ptr[1] == 0xE8000000L) /* check second sample */
							{   mark_set = TRUE;
								break;  /* out of for loop */
							}
						}
						/* point to start of trace to save */
						ul_ptr = (unsigned long *)((char *)vptr + trace_offset);
						ul_ptr[0] = 0xFFFF;
						if (mark_set == TRUE) ul_ptr[1] = 0xE8000000L;
						else                  ul_ptr[1] = 0xF1000000L;
						break;
					}
				}
				fwrite((void *)(((char *)vptr) + trace_offset),(long)(disk_trace_bytes),1,outfile);
				if (ferror(infile) || ferror(outfile)) break;
			}
			if (ferror(infile) || ferror(outfile))
			{   printf("GPR_CNDS - ERROR: ferror reported at trace %d\n",trace);
				puts("    WARNING: Condense operation may not be complete!");
				clearerr(infile);  clearerr(outfile);
			}

			#if defined(_INTELC32_)
			  realfree(inbuffer);
			  realfree(outbuffer);
			#endif
			fclose(infile);  fclose(outfile);
			free(vptr);
			break;
		}
		case DT1:
		{   char hd_infilename[MAX_PATHLEN],hd_outfilename[MAX_PATHLEN];
			char *c_ptr,*proc_hist,str[80];
			int  proc_hist_size = 160;
			int  itemp,print_it,dot,icount;
			int num_samps,data_block_bytes,hdr_block_bytes,num_gain_pts;
            void *v_ptr;          /* pointer to buffer for chunk of trace */
            double start_pos,final_pos,step_size,*gain_pts,traces_per_sec,meters_per_mark;
            long num_traces,offset,trace;
            size_t block_size,trace_size;
            struct tm *newtime;
            time_t long_time;
            struct SsTraceHdrStruct *TraceHdr;
            struct SsHdrInfoStruct HdInfo;
            FILE  *infile,*outfile;
            #if defined(_INTELC32_)
              char *inbuffer = NULL;
              char *outbuffer = NULL;
              size_t vbufsize = 64512;  /* 63 KB */
            #endif
			char *month[12] = { "January","February","March","April","May",
                                "June","July","August","September","October",
                                "November","December" } ;

			/* following in gpr_io.h */
			extern const char *GetSsHdFileMsg[];
            extern const char *GetSsTraceMsg[];

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

            /* Get info from input HD file */
            InitDt1Parameters(&HdInfo);
            if (Debug) print_it = TRUE;
            else       print_it = FALSE;
			itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename);
            if (itemp > 0)
            {   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
                printf("\n%s\n",GetSsHdFileMsg[itemp]);
                if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                exit(3);
            }
            if (Debug) PrintSsHdInfo("stdout",hd_outfilename,&HdInfo);

            /* Check limits of arguments */
            if (start <= 0) start = 0;
            if (end <= 0 || end <= start || end >= HdInfo.num_traces)
                end = HdInfo.num_traces - 1;
            if ((end - start) < 1)
            {   printf("GPR_CNDS - ERROR: (end - start) is less than 1.\n");
                exit(4);
            }
            if (skip <= 0)  skip = 0;
            if (skip >= (end-start)) skip = end - start - 1;
            skip++;

            /* Allocate space for a block of traces (include skipped ones) */
            num_samps = HdInfo.num_samples;
            data_block_bytes = num_samps * 2; /* data are signed short ints */
			hdr_block_bytes = sizeof(struct SsTraceHdrStruct);
            trace_size = data_block_bytes + hdr_block_bytes;
            block_size = trace_size * skip;
            v_ptr = (void *)malloc(block_size);
            if (v_ptr == NULL)
            {   printf("GPR_CNDS - ERROR: Unable to allocate storage for DT1 data block.\n");
                exit(4);
            }

            /* Open input file */
			if ((infile = fopen(in_filename,"rb")) == NULL)
            {   printf("GPR_CNDS - ERROR: Unable to open input file %s\n",in_filename);
                if (access(in_filename,4))
                {   if      (errno == ENOENT) puts("File or path name not found.");
                    else if (errno == EACCES) puts("Read Access denied.");
                    else                      puts("Unknown access error.");
				}
                free(v_ptr);
                exit(5);
            }
            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
              realmalloc(inbuffer,vbufsize);
              if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbufsize);
              else           setvbuf(infile,NULL,_IOFBF,vbufsize);
            #endif

            /* Open output file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
            {   printf("GPR_CNDS - ERROR: Unable to open output file %s\n",out_filename);
                if (access(out_filename,4))
                {   if      (errno == ENOENT) puts("File or path name not found.");
                    else if (errno == EACCES) puts("Read Access denied.");
                    else                      puts("Unknown access error.");
                }
                fclose(infile);
				free(v_ptr);
                exit(5);
            }
            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
			  realmalloc(outbuffer,vbufsize);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
              else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
            #endif

            /* Set/save values for output header */
            start_pos  = HdInfo.start_pos + (HdInfo.step_size * start);
            final_pos  = HdInfo.start_pos + (HdInfo.step_size * end);
            step_size  = HdInfo.step_size * skip;
            num_traces = 2 + ((end - 1 - start)/skip);
            traces_per_sec = 0.0;
            meters_per_mark = 0.0;
            num_gain_pts = 0;
            gain_pts = NULL;
			itemp = 0;
			if (HdInfo.proc_hist)  /* if allocated */
                itemp = strlen(HdInfo.proc_hist) + 1;
            itemp += proc_hist_size + 1;
            proc_hist = (char *)malloc((size_t)itemp);
            if (proc_hist)
            {   if (HdInfo.proc_hist)  /* allocated */
                    strcpy(proc_hist,HdInfo.proc_hist);
                time( &long_time );                 /* get time as long integer */
                newtime = localtime( &long_time );  /* convert to local time */
                sprintf(str,"\nGPR_CNDS v. 1.00 (USGS-JL) on %s %d, %d at %d:%02d ->\n",
                        month[newtime->tm_mon],newtime->tm_mday,newtime->tm_year+1900,
                        newtime->tm_hour,newtime->tm_min);
                strcat(proc_hist,str);
                sprintf(str,"   file = %s  start = %d  end = %d  skip = %d\n",
                in_filename,start,end,skip-1);
                strcat(proc_hist,str);
            }
            itemp = SaveSsHdFile((int)HdInfo.day,(int)HdInfo.month,(int)HdInfo.year,
                                num_traces,(long)HdInfo.num_samples,
                                (long)HdInfo.time_zero_sample,(int)HdInfo.total_time_ns,
								start_pos,final_pos,step_size,HdInfo.pos_units,
                                (double)HdInfo.ant_freq,(double)HdInfo.ant_sep,
                                (double)HdInfo.pulser_voltage,
                                (int)HdInfo.num_stacks,HdInfo.survey_mode,
                                proc_hist,hd_outfilename,out_filename,
                                HdInfo.job_number,HdInfo.title1,HdInfo.title2,
                                traces_per_sec,meters_per_mark,
                                num_gain_pts,gain_pts);
            free(proc_hist);

			/* Move input file pointer to first trace chunk to retrieve */
            offset = start * trace_size;
            fseek(infile,offset,SEEK_SET);

			/* Condense file.  File pointer must be at start of first trace
               to get.  Read in desired trace plus skipped ones. */
            dot = (end-start)/skip/10;
            if (dot < 1) dot = 1;
            icount = 0;
            trace = start;
			for (trace=start; trace<=end; trace+=skip,icount++)
            {   if (!(icount % dot)) printf(".");
                if (end - trace > skip)    /* far enough from end? */
                {    fread(v_ptr,block_size,1,infile);
				} else                     /* get end trace */
                {    offset = end * trace_size;
                    fseek(infile,offset,SEEK_SET);
                    fread(v_ptr,trace_size,1,infile);
                }
                /* update trace header */
                TraceHdr = (struct SsTraceHdrStruct *)v_ptr;
                TraceHdr->aux_trace_num = TraceHdr->trace_num = icount;

                fwrite((void *)v_ptr,(long)trace_size,1,outfile);
                if (ferror(infile) || ferror(outfile)) break;
            }
            if (ferror(infile) || ferror(outfile))
            {   printf("GPR_CNDS - ERROR: ferror reported at trace %d\n",trace);
                puts("    WARNING: Condense operation may not be complete!");
                clearerr(infile);  clearerr(outfile);
			}

            #if defined(_INTELC32_)
              realfree(inbuffer);
              realfree(outbuffer);
            #endif
            fclose(infile);  fclose(outfile);
            free(v_ptr);
            break;
        }
        case SGY:
		{   char *proc_hist,str[80];
            int  proc_hist_size = 160;
            int  itemp,dot,icount;
            int num_samps,data_block_bytes,hdr_block_bytes,num_gain_pts;
			void *v_ptr;          /* pointer to buffer for chunk of trace */
            double start_pos,final_pos,step_size,*gain_pts,traces_per_sec,meters_per_mark;
            int swap_bytes;
            long num_traces,offset,trace;
			size_t block_size,trace_size;
			struct tm *newtime;
            time_t long_time;
            struct SegyReelHdrStruct ReelHdr;
            struct SegyTraceHdrStruct *TraceHdr;
            struct GprInfoStruct GprInfo;
            FILE  *infile,*outfile;
            #if defined(_INTELC32_)
              char *inbuffer = NULL;
              char *outbuffer = NULL;
			  size_t vbufsize = 64512;  /* 63 KB */
            #endif
            char *month[12] = { "January","February","March","April","May",
                                "June","July","August","September","October",
                                "November","December" } ;

            /* following in gpr_io.h */
            extern const char *ReadSegyReelHdrMsg[];
            extern const char *GetSegyTraceMsg[];

            /* Get SEG-Y reel header file */
			itemp = ReadSegyReelHdr(in_filename,&swap_bytes,&num_traces,&ReelHdr);
            if (itemp > 0)
            {   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
                printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
                if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                exit(3);
            }
            if (Debug) PrintSegyReelHdr(swap_bytes,num_traces,"stdout",
                                        in_filename,&ReelHdr);

            /* Check limits of arguments */
            if (start <= 0) start = 0;
            if (end <= 0 || end <= start || end >= num_traces)
                end = num_traces - 1;
            if ((end - start) < 1)
            {   printf("GPR_CNDS - ERROR: (end - start) is less than 1.\n");
                exit(4);
            }
            if (skip <= 0)  skip = 0;
            if (skip >= (end-start)) skip = end - start - 1;
			skip++;

            /* Allocate space for a block of traces (include skipped ones) */
            num_samps = ReelHdr.hns;
			switch (ReelHdr.format)
            {   case 1:  case 2:  case 4:
                    data_block_bytes = num_samps * 4;
                    break;
                case 3:
                    data_block_bytes = num_samps * 2;
                    break;
                default:
                    printf("GPR_CNDS - ERROR: Unvalid SEG-Y sample format code.\n");
                    exit(4);
			}
            hdr_block_bytes = sizeof(struct SegyTraceHdrStruct);
            trace_size = data_block_bytes + hdr_block_bytes;
            block_size = trace_size * skip;
            v_ptr = (void *)malloc(block_size);
            if (v_ptr == NULL)
			{   printf("GPR_CNDS - ERROR: Unable to allocate storage for SEG-Y data block.\n");
                exit(4);
            }

            /* Open input file */
            if ((infile = fopen(in_filename,"rb")) == NULL)
            {   printf("GPR_CNDS - ERROR: Unable to open input file %s\n",in_filename);
                if (access(in_filename,4))
                {   if      (errno == ENOENT) puts("File or path name not found.");
                    else if (errno == EACCES) puts("Read Access denied.");
                    else                      puts("Unknown access error.");
                }
                free(v_ptr);
                exit(5);
            }
            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
              realmalloc(inbuffer,vbufsize);
              if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbufsize);
              else           setvbuf(infile,NULL,_IOFBF,vbufsize);
			#endif

            /* Open output file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
            {   printf("GPR_CNDS - ERROR: Unable to open output file %s\n",out_filename);
				if (access(out_filename,4))
                {   if      (errno == ENOENT) puts("File or path name not found.");
                    else if (errno == EACCES) puts("Read Access denied.");
                    else                      puts("Unknown access error.");
				}
                fclose(infile);
                free(v_ptr);
                exit(5);
            }
            /* Speed up disk access by using large (conventional memory) buffer */
            #if defined(_INTELC32_)
              realmalloc(outbuffer,vbufsize);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
              else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
			#endif

            /* Set/save values for output reel header */
            ExtractSsInfoFromSegy(&GprInfo,&ReelHdr);
            start_pos  = GprInfo.starting_position + (GprInfo.position_step_size * start);
            final_pos  = GprInfo.starting_position + (GprInfo.position_step_size * end);
            step_size  = GprInfo.position_step_size * skip;
            num_traces = 2 + ((end - 1 - start)/skip);
            traces_per_sec = GprInfo.traces_per_sec;
            meters_per_mark = GprInfo.meters_per_mark;
            num_gain_pts = GprInfo.num_gain_pts;
            gain_pts = GprInfo.gain_pts;

            itemp = 0;
			if (GprInfo.proc_hist && GprInfo.proc_hist_bytes > 0)  /* if allocated */
                itemp = GprInfo.proc_hist_bytes;
            itemp += proc_hist_size + 1;
			proc_hist = (char *)malloc((size_t)itemp);
			if (proc_hist)
			{   if (GprInfo.proc_hist && GprInfo.proc_hist_bytes > 0)
					strncpy(proc_hist,GprInfo.proc_hist,GprInfo.proc_hist_bytes);
				time( &long_time );                 /* get time as long integer */
                newtime = localtime( &long_time );  /* convert to local time */
                sprintf(str,"\nGPR_CNDS v. 1.00 (USGS-JL) on %s %d, %d at %d:%02d ->\n",
                        month[newtime->tm_mon],newtime->tm_mday,newtime->tm_year+1900,
                        newtime->tm_hour,newtime->tm_min);
				strcat(proc_hist,str);
                sprintf(str,"   file=%s  start=%d  end=%d  skip=%d\n",
                in_filename,start,end,skip-1);
				strcat(proc_hist,str);
            }
			SetSgyFileHeader((int)GprInfo.time_date_created.day,
                             (int)GprInfo.time_date_created.month,
                             (int)GprInfo.time_date_created.year+1980,
                             num_traces,num_samps,(long)GprInfo.timezero_sample,
							 (int)GprInfo.in_time_window,
                             start_pos,final_pos,step_size,GprInfo.position_units,
                             (double)GprInfo.nominal_frequency,
                             GprInfo.antenna_separation,GprInfo.pulser_voltage,
                             GprInfo.number_of_stacks,GprInfo.survey_mode,
							 proc_hist,GprInfo.text,GprInfo.job_number,
							 GprInfo.title1,GprInfo.title2,out_filename,
                             traces_per_sec,meters_per_mark,
                             num_gain_pts,gain_pts,&ReelHdr);
            fwrite((void *)&ReelHdr,sizeof(struct SegyReelHdrStruct),1,outfile);
			free(proc_hist);

            /* Move input file pointer to first trace chunk to retrieve */
            offset = sizeof(struct SegyReelHdrStruct) + (start * trace_size);
            fseek(infile,offset,SEEK_SET);

            /* Condense file.  File pointer must be at start of first trace
               to get.  Read in desired trace plus skipped ones. */
            dot = (end-start)/skip/10;
            if (dot < 1) dot = 1;
            icount = 0;
            trace = start;
            for (trace=start; trace<=end; trace+=skip,icount++)
            {   if (!(icount % dot)) printf(".");
                if (end - trace > skip)    /* far enough from end? */
                {    fread(v_ptr,block_size,1,infile);
				} else                     /* get end trace */
                {    offset = sizeof(struct SegyReelHdrStruct) + (end * trace_size);
                    fseek(infile,offset,SEEK_SET);
                    fread(v_ptr,trace_size,1,infile);
                }
                /* update trace header */
                TraceHdr = (struct SegyTraceHdrStruct *)v_ptr;
                TraceHdr->tracl = TraceHdr->tracr = TraceHdr->fldr = icount;

                fwrite((void *)v_ptr,(long)trace_size,1,outfile);
                if (ferror(infile) || ferror(outfile)) break;
            }
            if (ferror(infile) || ferror(outfile))
            {   printf("GPR_CNDS - ERROR: ferror reported at trace %d\n",trace);
                puts("    WARNING: Condense operation may not be complete!");
                clearerr(infile);  clearerr(outfile);
            }

            #if defined(_INTELC32_)
              realfree(inbuffer);
			  realfree(outbuffer);
            #endif
            fclose(infile);  fclose(outfile);
            free(v_ptr);
            break;
        }
        default:
            puts("\nGPR_CNDS - ERROR: GPR file storage format is not recognized.");
            puts("           Use program GPR_CONV to convert user-defined or unrecognized");
			puts("           format to the GSSI DZT, S&S DT1/HD, or SEG-Y format.");
            exit(3);
    }
    puts("\nGPR_CNDS finished");
    exit(0);
}

/**************************** end of GPR_CNDS.C *****************************/
