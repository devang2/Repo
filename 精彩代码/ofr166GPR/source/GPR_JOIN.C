/******************************** GPR_JOIN.C ********************************\
 *                            version 1.08.08.01
 * GPR_JOIN joins or concatenates two or more digital ground penetrating
 * radar data sets.  All data sets (files) must have one channel (DZT files),
 * the same number of samples per trace, the same sample rate (time interval
 * between samples), and the same storage format.  The following computer
 * storage formats are recognized.
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
 *  November 25, 1996  - Completed as a modification and combination of
 *                       GPR_DIFF.C and GPR_REV.C.
 *  August 11, 1997    - Added support for modified DZT formats.
 *  August 22, 2000    - Corrected docs above.
 *  August 8, 2001     - Updated time and date functions
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /zfloatsync gpr_join.c gpr_io.lib jl_util1.lib
 *     To compile for 80486 executable:
 *       icc /F /xnovm /zfloatsync /zmod486 gpr_join.c gpr_io.lib jl_util1.lib
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
 *     bcc -3 -a -d -f287 -ml -O2 gpr_join.c
 *
 *     -3 generates 80386 protected mode compatible instructions
 *     -a alignes code on (2-byte) word boundaries
 *     -d merges duplicates strings
 *     -f287 invokes 80287 instructions for floating point math
 *     -ml uses the large memory model
 *     -O2 optimizes for fastest code
 *
 * To run: GPR_JOIN filename.cmd [/b /d]
 *
\****************************************************************************/

/********************** Include required header files ***********************/
/* non-ANSI headers */
#include <conio.h>     /* getch */
#include <errno.h>     /* errno */
#include <io.h>        /* access */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <stdio.h>     /* puts,printf,fopen,fread,fclose,FILE,NULL,_IOFBF,
                          SEEK_SET,fseek,fwrite,fgets,fileno */
#include <stdlib.h>    /* exit,malloc,free,_splitpath,atoi,atol */
#include <string.h>    /* strcpy,strstr,strcmp,strchr,strlen,strcat,strncpy,
                          strupr (non-ANSI) */

/* application headers */
#include "gpr_io.h"    /* to use gpr_io.lib functions, etc. */
#include "jl_defs.h"   /* common manifest constants and macros */
#include "jl_util1.h"  /* ANSI_there(), MAX_PATHLEN, TrimStr() etc. */

/*************************** Manifest constants ****************************/
#if defined(_INTELC32_)
  #define BUFSIZE 65536L      /* size of I/O and data buffers */
#else
  #define BUFSIZE 32768L      /* size of I/O and data buffers */
#endif

#define GPR_JOIN_VER "1.08.08.01"
#define DATETIME "August 8, 2001"

/**************************** Global variables ******************************/
int Debug = FALSE;     /* if TRUE, turn debugging on */
int Batch = FALSE;     /* if TRUE, supress interaction with user */

/*************************** Function prototypes ****************************/
void ExitFunc1(void);

/********************************** main() **********************************/
void main(int argc, char *argv[])
{
    char   cmd_filename[MAX_PATHLEN];
    char   out_filename[MAX_PATHLEN];
    char   str[MAX_CMDLINELEN];
    int    i,itemp,min_args,num_files;
    int    format,channels,samps,datatype,ttime;
    FILE  *infile;
    struct info
    {   char in_filename[MAX_PATHLEN];
        long first_trace,last_trace;
    } ;
    struct info *info_array = NULL;

    /* following used by GetGprFileType() */
    int   file_hdr_bytes, num_hdrs, trace_hdr_bytes, samples_per_trace;
    int   num_channels, input_datatype, total_time, file_type;
    long  num_traces;

    /* following in errno.h */
    extern int errno;

    /* Register exit function 1 (return console colors to white on black) */
    atexit(ExitFunc1);

	/* Check if enough command line arguments */
    min_args = 2;
    if (argc < min_args)
    {   puts("\n");
        if (ANSI_there())  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
        puts("");
        puts("###############################################################################");
        puts("   Usage: GPR_JOIN filename.cmd [/b /d]");
        puts("     Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
        puts("   This program joins two or more GPR data fles together and stores the");
		puts("      results to disk as as a new file.");
		puts("   Recognized storage formats include: GSSI DZT, S&S DT1/HD, and SEG-Y.");
	    puts("\n   Required command line arguments:");
		puts("     filename.cmd - a command or parameter file containing the output filename");
		puts("                    and the input filenames with first and last traces to use");
		puts("                    from each file.  See GPR_JOIN.CMD for an example file.");
		puts("   Information from the the first file header is used for the output file");
		puts("     header - if applicable, modification date, comments, and processing");
		puts("     history are changed.");
		puts("   NOTE: Files MUST HAVE the same number of samples per trace, same sample rate");
		puts("      (time interval between samples), and the same storage format.");
        puts("   Optional command line arguments (do not include brackets):");
        puts("     /b - turn batch processing on.");
        puts("     /d - turn debugging on.");
        printf("  Version %s - Last Revision Date: %s at %s\n",GPR_JOIN_VER,DATETIME,__TIME__);
        puts("###############################################################################");
        exit(1);
    }

    /* Get command line arguments */
    strcpy(cmd_filename,strupr(argv[1]));
    for (i = min_args; i<argc; i++)
    {   if (strstr(strupr(argv[i]),"/D")) Debug = TRUE;
        if (strstr(strupr(argv[i]),"/B")) Batch = TRUE;
    }

    /* Set console colors to red on black to report problems */
    if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */

    /* Open command file */
    if ((infile = fopen(cmd_filename,"rt")) == NULL)
    {   char *cptr;

        if ((cptr = strchr(cmd_filename,'.')) == NULL)
        {   strcat(cmd_filename,".");
            if ((infile = fopen(cmd_filename,"rt")) == NULL)
            {   strcat(cmd_filename,"cmd");
                if ((infile = fopen(cmd_filename,"rt")) == NULL)
                {   printf("\nGPR_JOIN: ERROR - problem opening command file %s.\n",cmd_filename);
                    if (access(cmd_filename,4))
                    {   if      (errno == ENOENT) puts("                  File or path name not found.");
                        else if (errno == EACCES) puts("                  Read Access denied.");
                        else                      puts("                  Unknown access error.");
                    }
					exit(2);
				}
            }
        } else
        {   *cptr = 0;
            strcat(cmd_filename,".cmd");
            if ((infile = fopen(cmd_filename,"rt")) == NULL)
            {   printf("\nGPR_JOIN: ERROR - problem opening command file %s.\n",cmd_filename);
                if (access(cmd_filename,4))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Read Access denied.");
					else                      puts("                  Unknown access error.");
                }
                exit(2);
            }
        }
    }

    /* Get info from command file.  NOTE: this file is structured but allows
       blank lines and truncates lines at semicolons. */
    itemp = num_files = 0;
    out_filename[0] = 0;
    while (1)            /* read till EOF or error */
    {   char *cp,*cp2;

        if (fgets(str,MAX_CMDLINELEN,infile) == NULL) /* get a record */
            break;   /* out of while(1) loop */
        cp = strchr(str,';');         /* see if semicolon present */
        if (cp != NULL)               /* if present ... */
            *cp = 0;                  /* ... truncate at semicolon */
        TrimStr(str);                 /* remove leading and trailing blanks */
        if (str[0] == 0) continue;    /* if string empty go to end of loop */
        if (num_files == 0)
        {   num_files = atoi(str);    /* get number of files first */
            continue;                 /* go to end of loop */
        }
        if (num_files >= 2 && out_filename[0] == 0)
        {   strcpy(out_filename,str); /* get output filename second */
            continue;                 /* go to end of loop */
        }
        if (num_files >= 2 && out_filename[0] != 0 && info_array == NULL)
        {   info_array = (struct info *)malloc(num_files * sizeof(struct info));
            if (info_array == NULL)
            {   printf("\nGPR_JOIN: ERROR - unable to allocate temporary storage.\n");
                exit(3);
			}
			for (i=0; i<num_files; i++)
            {   info_array[i].in_filename[0] = 0;
                info_array[i].first_trace = -1;
                info_array[i].last_trace = -1;
            }
        }
        if (info_array != NULL)
        {   cp = strchr(str,' ');     /* look for space after filename */
            if (cp != NULL)
                *cp = 0;              /* truncate at first space ... */
			else
				cp = str;             /* ... or point to start of string */
			if (strlen(str))
			{   strcpy(info_array[itemp].in_filename,str);  /* copy filename */
				if (cp != str)
				{   cp++;  /* step past space */
				} else
				{    if (info_array[itemp].in_filename[0] != 0)
						itemp++;
					continue; /* go to end of loop */
				}
				TrimStr(cp);          /* remove leading and trailing blanks */
				info_array[itemp].first_trace = atol(cp); /* conversion stops at next space */
				cp2 = strchr(cp,' '); /* look for space after 1st number */
				if (cp2 != NULL)
					info_array[itemp].last_trace = atol(cp2);
				if (info_array[itemp].in_filename[0] != 0)
					itemp++;
			}
		}
	}   /*end of while (1) loop */
	fclose(infile);

	/* Check values */
	if (Debug)
	{	printf("\nnum_files    = %d\n",num_files);
		printf("out_filename = %s\n",out_filename);
		for (i=0; i<num_files; i++)
		{   printf("in_filename  = %s  first_trace = %ld  last_trace = %ld\n",
			   info_array[i].in_filename,info_array[i].first_trace,info_array[i].last_trace);
		}

	}
	if (num_files < 2)
	{   printf("\nGPR_JOIN: ERROR - number of input file must be at least 2.\n");
		exit(4);
	}
	if (out_filename[0] == 0)
	{   printf("\nGPR_JOIN: ERROR - no output file specified.");
		exit(4);
	}
	for (i=0; i<num_files; i++)
	{   if (strcmp(info_array[i].in_filename,out_filename) == 0)
		{   puts("\nGPR_JOIN: ERROR - Output filename must be different from input file names.");
			exit(4);
		}
	}

	/* Check that all files are the same type */
	itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
				   &samples_per_trace,&num_channels,&num_traces,
				   &input_datatype,&total_time,&file_type,info_array[0].in_filename);
	if (itemp > 0)
	{   printf("%s\n",GetGprFileTypeMsg[itemp]);
		exit(5);
	}
	format = file_type;
	channels = num_channels;
	samps = samples_per_trace;
	datatype = input_datatype;
	ttime = total_time;

	if (info_array[0].first_trace < 0)
		info_array[0].first_trace = 0;
	if (info_array[0].last_trace  < 0)
        info_array[0].last_trace = num_traces - 1;
    if (info_array[0].last_trace < info_array[0].first_trace)
        info_array[0].last_trace = num_traces - 1;
    if (info_array[0].first_trace > info_array[0].last_trace)
		info_array[0].first_trace = 0;

	for (i=1; i<num_files; i++)
	{   itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
				   &samples_per_trace,&num_channels,&num_traces,
				   &input_datatype,&total_time,&file_type,info_array[i].in_filename);
		if (itemp > 0)
		{   printf("%s\n",GetGprFileTypeMsg[itemp]);
			exit(5);
		}
		if (format != file_type)
		{   printf("\nGPR_JOIN: ERROR - All input files must have the same storage format.\n");
			exit(4);
		}
		if (channels != num_channels)
		{   printf("\nGPR_JOIN: ERROR - All input files must have the same number of channels.\n");
			exit(4);
		}
		if (samps != samples_per_trace)
		{   printf("\nGPR_JOIN: ERROR - All input files must have the same number of samples.\n");
			exit(4);
		}
		if (datatype != input_datatype)
		{   printf("\nGPR_JOIN: ERROR - All input files must have the same data type.\n");
			exit(4);
		}
		if (ttime != total_time)
		{   printf("\nGPR_JOIN: ERROR - All input files must have the same sample rate.\n");
			exit(4);
		}
		if (info_array[i].first_trace < 0)
			info_array[i].first_trace = 0;
		if (info_array[i].last_trace  < 0)
			info_array[i].last_trace = num_traces - 1;
		if (info_array[i].last_trace < info_array[i].first_trace)
			info_array[i].last_trace = num_traces - 1;
		if (info_array[i].first_trace > info_array[i].last_trace)
			info_array[i].first_trace = 0;
	}
	printf("\nnum_files    = %d\n",num_files);
	printf("out_filename = %s\n",out_filename);
	for (i=0; i<num_files; i++)
	{   printf("in_filename  = %s  first_trace = %ld  last_trace = %ld\n",
			info_array[i].in_filename,info_array[i].first_trace,info_array[i].last_trace);
	}

    /* Set console colors to cyan on black to report progress */
    if (ANSI_there())  printf("%c%c36;40m",0x1B,0x5B);  /* cyan on black */

    /* Get the data and concatenate */
    switch (file_type)
	{   case DZT:  case MOD_DZT:
		{   char   str[400],text[400];
            int    i,itemp,ifile,error;
            int    num_chan,num_bits,num_samps;
            int    num_hdrs,channel,header_size,disk_chunk_size;
			long   num_traces,offset,chunks,filebytes,last_byte,current_byte;
            size_t buf_size,disk_trace_size;
            size_t vbuf_size   = BUFSIZE;
            void  *v_ptr       = NULL;
            struct dosdate_t dosdate;
            struct dostime_t dostime;
            char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
            struct DztHdrStruct DztHdr;
            FILE  *infile      = NULL;
            FILE  *outfile     = NULL;
            #if defined(_INTELC32_)
              char *inbuffer   = NULL;
              char *outbuffer  = NULL;
            #endif

			/* Open output DZT file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
			{   printf("\nGPR_JOIN: ERROR - Unable to open output DZT file %s\n",out_filename);
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
              realmalloc(outbuffer,vbuf_size);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbuf_size);
              else            setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #else
              setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #endif

            /* Copy header(s) to disk */
            for (i=0; i<channels; i++)
			{   /* get header from input file */
                channel = i;
                itemp = ReadOneDztHeader(info_array[i].in_filename,&num_hdrs,&num_traces,
                                         channel,&header_size,&DztHdr);
				if (itemp > 0)
                {   printf("\nGPR_JOIN: ERROR - short count from fread() of file %s\n",
                            info_array[i].in_filename);
                    fclose(infile);  fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(outbuffer);
                    #endif
                    free(v_ptr);
					exit(8);
                }
                if (i == 0)
                {   num_chan  = DztHdr.rh_nchan;
                    num_bits  = DztHdr.rh_bits;
                    num_samps = DztHdr.rh_nsamp;
                }

                /* update modification time/date */
                _dos_getdate(&dosdate);
                _dos_gettime(&dostime);
                DztHdr.rh_modif.month = dosdate.month - 1;    /* 0 - 11 */
                DztHdr.rh_modif.day   = dosdate.day;          /* 1 - 31 */
                DztHdr.rh_modif.year  = dosdate.year - 1900;  /* year - 1900 */
                DztHdr.rh_modif.hour  = dostime.hour;         /* 0 - 24 */
                DztHdr.rh_modif.min   = dostime.minute;
                DztHdr.rh_modif.sec2  = (dostime.second+1)/2;

                /* change file name */
                {   char drive[80],dir[80],fname[80],ext[80];

                    _splitpath(out_filename,drive,dir,fname,ext);
                    EditDztHeader(26,(void *)fname,&DztHdr);
                }

                /* change comment string */
                if (i == 0)
                {   char str2[MAX_PATHLEN];

                    sprintf(str,"\nGPR_JOIN:");
                    for (itemp=0; itemp<num_files; itemp++)
					{   if ((strlen(str) + strlen(info_array[itemp].in_filename) + 20) > 78)
							strcat(str,"\n");
						sprintf(str2," %s trace %d thru %d;",
							info_array[itemp].in_filename,
							info_array[itemp].first_trace,
							info_array[itemp].last_trace);
						strcat(str,str2);
					}
					strncpy(text,(char *)&DztHdr + DztHdr.rh_text,sizeof(text)-1);
					if (strlen(text) < sizeof(text)-strlen((char *)str)-2)
						strcat(text,(char *)str);
					EditDztHeader(30,(void *)text,&DztHdr);
				}

				/* write header to output file */
				if (fwrite((void *)&DztHdr,sizeof(struct DztHdrStruct),(size_t)1,outfile) < 1)
				{   printf("\nGPR_JOIN: ERROR - short count from fwrite() to file %s\n",out_filename);
					fclose(infile); fclose(outfile);
					#if defined(_INTELC32_)
					  realfree(inbuffer);  realfree(outbuffer);
					#endif
					free(v_ptr);
					exit(9);
				}
			}   /* end of for (i=0; i<num_chan; i++) loop */

			for (ifile=0; ifile<num_files; ifile++)
			{    /* Open input DZT file */
				if ((infile = fopen(info_array[ifile].in_filename,"rb")) == NULL)
				{   printf("\nGPR_JOIN: ERROR - Unable to open input DZT file %s\n",
							info_array[ifile].in_filename);
					if (access(info_array[ifile].in_filename,4))
					{   if      (errno == ENOENT) puts("                  File or path name not found.");
						else if (errno == EACCES) puts("                  Read Access denied.");
                        else                      puts("                  Unknown access error.");
                    }
                    exit(5);
                }

                /* Speed up disk access by using large (conventional memory)
				   buffer. NOTE: stream errors are not reported to fwrite() or
                   fread() until the buffer setup by setvbuf() is emptied!
                   If vbuf_size is greater than the filesize ferror() will
				   not report errors! */
                #if defined(_INTELC32_)
				  realmalloc(inbuffer,vbuf_size);
                  if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbuf_size);
				  else           setvbuf(infile,NULL,_IOFBF,vbuf_size);
				#else
                  setvbuf(infile,NULL,_IOFBF,vbuf_size);
                #endif

                /* Setup/allocate RAM buffer to speed transfer */
                buf_size = vbuf_size;
                disk_trace_size = num_samps * (num_bits/8);     /* 8 bits per byte */
                disk_chunk_size = num_chan * disk_trace_size;
                last_byte = (header_size * num_chan) +
							((info_array[ifile].last_trace+1) * disk_trace_size);
				filebytes = (info_array[ifile].last_trace - info_array[ifile].first_trace + 1)
							* disk_trace_size;
                if (filebytes < buf_size)  buf_size = filebytes;
                else                       buf_size = (buf_size/disk_chunk_size) * disk_chunk_size;
                while ((buf_size>=disk_chunk_size) && ((v_ptr=malloc(buf_size))==NULL))
                    buf_size -= disk_chunk_size;
                if (v_ptr == NULL || buf_size < disk_chunk_size)
                {   puts("\nGPR_JOIN: ERROR - Unable to allocate temporary storage.");
                    fclose(infile); fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(outbuffer);
                    #endif
                    free(v_ptr);
                    exit(7);
                }
                if (filebytes % buf_size)  chunks = filebytes/buf_size + 1;
                else                       chunks = filebytes/buf_size;

                /* Move the file pointer to start of first trace */
				offset = (header_size * num_chan) +
						 (info_array[ifile].first_trace * disk_trace_size);
				fseek(infile,(size_t)offset,SEEK_SET);

				/* Copy traces */
				error = FALSE;
				printf("GPR_JOIN: Copying %s to %s ...                       ",
					   info_array[ifile].in_filename,out_filename);
				for (i=0; i<chunks; i++)
				{   if (fread(v_ptr,buf_size,(size_t)1,infile) < 1)
					{   error = TRUE;
						break;                /* out of for (i...) loop */
					}
					if (fwrite(v_ptr,buf_size,(size_t)1,outfile) < 1)
					{   error = TRUE;
						break;                /* out of for (i...) loop */
					}
					if (ferror(infile) || ferror(outfile))
					{   error = TRUE;
						break;                /* out of for (i...) loop */
					}
					current_byte = ftell(infile);
					if (buf_size > (last_byte - current_byte))
					{   buf_size = last_byte - current_byte;
					}
				}
				puts("");
				free(v_ptr);
				fclose(infile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);
				#endif
				if (error == TRUE)
				{   printf("\nGPR_JOIN - ERROR: I/O error reported at chunk %d\n",i);
					puts("     WARNING: Joining operation may not be complete!");
					clearerr(infile);  clearerr(outfile);
					fclose(outfile);
					#if defined(_INTELC32_)
					  realfree(outbuffer);
					#endif
					exit(10);
				}
			}   /* end of for (ifile=0; ifile<num_files; ifile++) */

			fclose(outfile);
			#if defined(_INTELC32_)
			  realfree(outbuffer);
			#endif

			break;
		}  /* end of case DZT: block */
		case DT1:
		{   char   hd_infilename[MAX_PATHLEN],hd_outfilename[MAX_PATHLEN];
			char  *c_ptr,str[400];
			char  *proc_hist   = NULL;
			int    proc_hist_size = 240;
			int    i,j,itemp,print_it,ifile,count,error;
			int    num_samps;
			int    data_block_bytes,hdr_block_bytes;
			long   offset,chunks,filebytes,last_byte,current_byte;
			double start_pos,final_pos;
            size_t buf_size,disk_trace_size,disk_chunk_size;
            size_t vbuf_size   = BUFSIZE;
            void  *v_ptr       = NULL; /* hold one trace */
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

            /* Open output DT1 file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
            {   printf("\nGPR_JOIN: ERROR - Unable to open output DT1 file %s\n",out_filename);
                if (access(out_filename,2))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                exit(7);
            }

            /* Speed up disk access by using large (conventional memory)
               buffer. NOTE: stream errors are not reported to fwrite() or
               fread() until the buffer setup by setvbuf() is emptied!
               If vbuf_size is greater than the filesize ferror() will
			   not report errors! */
            #if defined(_INTELC32_)
              realmalloc(outbuffer,vbuf_size);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbuf_size);
              else            setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #else
              setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #endif

            /* Construct output HD filename */
			hd_outfilename[0] = 0;
            strcpy(hd_outfilename,out_filename);
			if ((c_ptr=strchr(hd_outfilename,'.')) != NULL)
                *c_ptr = 0;               /* terminated string at period */
            strcat(hd_outfilename,".hd");

            count = 0;
            for (ifile=0; ifile<num_files; ifile++)
            {    /* Open input DT1 file */
                if ((infile = fopen(info_array[ifile].in_filename,"rb")) == NULL)
                {   printf("\nGPR_JOIN: ERROR - Unable to open input DT1 file %s\n",
                            info_array[ifile].in_filename);
                    if (access(info_array[ifile].in_filename,4))
                    {   if      (errno == ENOENT) puts("                  File or path name not found.");
                        else if (errno == EACCES) puts("                  Read Access denied.");
                        else                      puts("                  Unknown access error.");
					}
                    exit(5);
                }

                /* Speed up disk access by using large (conventional memory)
                   buffer. NOTE: stream errors are not reported to fwrite() or
                   fread() until the buffer setup by setvbuf() is emptied!
                   If vbuf_size is greater than the filesize ferror() will
                   not report errors! */
                #if defined(_INTELC32_)
                  realmalloc(inbuffer,vbuf_size);
                  if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbuf_size);
                  else           setvbuf(infile,NULL,_IOFBF,vbuf_size);
                #else
                  setvbuf(infile,NULL,_IOFBF,vbuf_size);
                #endif

                /* Construct input HD filename */
                hd_infilename[0] = 0;
				strcpy(hd_infilename,info_array[ifile].in_filename);
                if ((c_ptr=strchr(hd_infilename,'.')) != NULL)
                    *c_ptr = 0;               /* terminated string at period */
                strcat(hd_infilename,".hd");

                /* Get info from input HD files */
                if (ifile == 0)
                {   InitDt1Parameters(&HdInfo);
                    if (Debug) print_it = TRUE;
                    else       print_it = FALSE;
					itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename);
					if (itemp > 0)
					{   printf("\n%s\n",GetSsHdFileMsg[itemp]);
						exit(6);
					}
					num_samps = HdInfo.num_samples;
					start_pos = HdInfo.start_pos;
				}
				if (ifile == num_files - 1)
				{   InitDt1Parameters(&HdInfo);
					if (Debug) print_it = TRUE;
					else       print_it = FALSE;
					itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename);
					if (itemp > 0)
					{   printf("\n%s\n",GetSsHdFileMsg[itemp]);
						exit(6);
					}
					final_pos = HdInfo.final_pos;
				}

				/* Setup/allocate RAM buffer to speed transfer */
				buf_size = vbuf_size;
				data_block_bytes = num_samps * 2; /* data are signed short ints */
				hdr_block_bytes = sizeof(struct SsTraceHdrStruct);
                disk_trace_size = data_block_bytes + hdr_block_bytes;
                disk_chunk_size = disk_trace_size;
				last_byte = ((info_array[ifile].last_trace+1) * disk_trace_size);
				filebytes = (info_array[ifile].last_trace - info_array[ifile].first_trace + 1)
                            * disk_trace_size;
                if (filebytes < buf_size)  buf_size = filebytes;
                else                       buf_size = (buf_size/disk_chunk_size) * disk_chunk_size;
                while ((buf_size>=disk_chunk_size) && ((v_ptr=malloc(buf_size))==NULL))
                    buf_size -= disk_chunk_size;
                if (v_ptr == NULL || buf_size < disk_chunk_size)
                {   puts("\nGPR_JOIN: ERROR - Unable to allocate temporary storage.");
					fclose(infile); fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(outbuffer);
                    #endif
                    free(v_ptr);
                    exit(7);
                }
                if (filebytes % buf_size)  chunks = filebytes/buf_size + 1;
                else                       chunks = filebytes/buf_size;

				/* Move the file pointer to start of first trace */
				offset = (info_array[ifile].first_trace * disk_trace_size);
				fseek(infile,(size_t)offset,SEEK_SET);

                /* Copy traces */
                error = FALSE;
                printf("GPR_JOIN: Copying %s to %s ...                       ",
                       info_array[ifile].in_filename,out_filename);
                for (i=0; i<chunks; i++)
                {   if (fread(v_ptr,buf_size,(size_t)1,infile) < 1)
                    {   error = TRUE;
                        break;                /* out of for (i...) loop */
                    }
                    for (j=0; j<buf_size; j+=disk_trace_size, count++)
                    {    /* update trace header */
                        TraceHdr = (struct SsTraceHdrStruct *)((char *)v_ptr+j);
						TraceHdr->aux_trace_num = TraceHdr->trace_num = count;
                    }
                    if (fwrite(v_ptr,buf_size,(size_t)1,outfile) < 1)
                    {   error = TRUE;
                        break;                /* out of for (i...) loop */
                    }
					if (ferror(infile) || ferror(outfile))
                    {   error = TRUE;
                        break;                /* out of for (i...) loop */
                    }
                    current_byte = ftell(infile);
					if (buf_size > (last_byte - current_byte))
					{   buf_size = last_byte - current_byte;
                    }
                }
                puts("");
                free(v_ptr);
                fclose(infile);
                #if defined(_INTELC32_)
				  realfree(inbuffer);
                #endif
                if (error == TRUE)
                {   printf("\nGPR_JOIN - ERROR: I/O error reported at chunk %d\n",i);
                    puts("     WARNING: Joining operation may not be complete!");
                    clearerr(infile);  clearerr(outfile);
                    fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(outbuffer);
                    #endif
					exit(10);
                }
			}   /* end of for (ifile=0; ifile<num_files; ifile++) */

			fclose(outfile);
			#if defined(_INTELC32_)
			  realfree(outbuffer);
			#endif

			/* Set/save values for output header */
			hd_infilename[0] = 0;
			strcpy(hd_infilename,info_array[0].in_filename);
			if ((c_ptr=strchr(hd_infilename,'.')) != NULL)
				*c_ptr = 0;               /* terminated string at period */
			strcat(hd_infilename,".hd");
			InitDt1Parameters(&HdInfo);
			if (Debug) print_it = TRUE;
			else       print_it = FALSE;
			itemp = GetSsHdFile(print_it,&HdInfo,hd_infilename);
			if (itemp > 0)
			{   printf("\n%s\n",GetSsHdFileMsg[itemp]);
				exit(6);
			}
			if (HdInfo.proc_hist)  /* if allocated */
				itemp = strlen(HdInfo.proc_hist) + 1;
			itemp += proc_hist_size + 1;
			proc_hist = (char *)malloc((size_t)itemp);
			if (proc_hist)
			{   char str2[MAX_PATHLEN];

				if (HdInfo.proc_hist)  /* allocated */
					strcpy(proc_hist,HdInfo.proc_hist);

				sprintf(str,"\nGPR_JOIN:");
				for (itemp=0; itemp<num_files; itemp++)
				{   if ((strlen(str) + strlen(info_array[itemp].in_filename) + 20) > 78)
						strcat(str,"\n");
					sprintf(str2," %s trace %d thru %d;",
						info_array[itemp].in_filename,
						info_array[itemp].first_trace,
						info_array[itemp].last_trace);
					strcat(str,str2);
				}
				strcat(proc_hist,str);
			}
			itemp = SaveSsHdFile((int)HdInfo.day,(int)HdInfo.month,(int)HdInfo.year,
								(long)count,(long)HdInfo.num_samples,
								(long)HdInfo.time_zero_sample,(int)HdInfo.total_time_ns,
								start_pos,final_pos,
								(double)HdInfo.step_size,HdInfo.pos_units,
								(double)HdInfo.ant_freq,(double)HdInfo.ant_sep,
								(double)HdInfo.pulser_voltage,
                                (int)HdInfo.num_stacks,HdInfo.survey_mode,
                                proc_hist,hd_outfilename,out_filename,
                                HdInfo.job_number,HdInfo.title1,HdInfo.title2,
                                0.0,0.0,0,NULL);
            free(proc_hist);


            break;
        }  /* end of case DT1: block */
		case SGY:
        {   char  *proc_hist   = NULL;
            char   str[400];
            int    proc_hist_size = 240;
            int    i,j,itemp,ifile,count,error;
            int    data_block_bytes,hdr_block_bytes;
            int    num_samps;
            int    swap_bytes;
			long   offset,chunks,filebytes,last_byte,current_byte;
            double start_pos,final_pos;
            size_t buf_size,disk_trace_size,disk_chunk_size;
            struct SegyReelHdrStruct ReelHdr;
            struct SegyTraceHdrStruct *TraceHdr;
            struct GprInfoStruct GprInfo;
            size_t vbuf_size   = BUFSIZE;
            void  *v_ptr       = NULL; /* hold one trace */
            FILE  *infile      = NULL;
            FILE  *outfile     = NULL;
            #if defined(_INTELC32_)
			  char *inbuffer   = NULL;
              char *outbuffer  = NULL;
			#endif

            /* following in gpr_io.h */
            extern const char *ReadSegyReelHdrMsg[];

            /* Open output SGY file */
            if ((outfile = fopen(out_filename,"wb")) == NULL)
            {   printf("\nGPR_JOIN: ERROR - Unable to open output SGY file %s\n",out_filename);
                if (access(out_filename,2))
                {   if      (errno == ENOENT) puts("                  File or path name not found.");
                    else if (errno == EACCES) puts("                  Write Access denied.");
                    else                      puts("                  Unknown access error.");
                }
                exit(7);
			}

            /* Speed up disk access by using large (conventional memory)
               buffer. NOTE: stream errors are not reported to fwrite() or
               fread() until the buffer setup by setvbuf() is emptied!
               If vbuf_size is greater than the filesize ferror() will
               not report errors! */
            #if defined(_INTELC32_)
              realmalloc(outbuffer,vbuf_size);
              if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbuf_size);
			  else            setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #else
              setvbuf(outfile,NULL,_IOFBF,vbuf_size);
            #endif

            /* Set/save values for output reel header */
            itemp = ReadSegyReelHdr(info_array[0].in_filename,&swap_bytes,&num_traces,&ReelHdr);
            if (itemp > 0)
            {   printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
                exit(6);
            }
            ExtractSsInfoFromSegy(&GprInfo,&ReelHdr);
            num_samps = GprInfo.in_samps_per_trace;
            start_pos = GprInfo.starting_position;
            itemp = ReadSegyReelHdr(info_array[num_files-1].in_filename,&swap_bytes,&num_traces,&ReelHdr);
            if (itemp > 0)
            {   printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
                exit(6);
            }
			ExtractSsInfoFromSegy(&GprInfo,&ReelHdr);
			final_pos = GprInfo.final_position;
			itemp = 0;
			if (GprInfo.proc_hist && GprInfo.proc_hist_bytes > 0)  /* if allocated */
				itemp = GprInfo.proc_hist_bytes;
			itemp += proc_hist_size + 1;
			proc_hist = (char *)malloc((size_t)itemp);
			if (proc_hist)
			{   char str2[MAX_PATHLEN];

				if (GprInfo.proc_hist && GprInfo.proc_hist_bytes > 0)
					strncpy(proc_hist,GprInfo.proc_hist,GprInfo.proc_hist_bytes);
				sprintf(str,"\nGPR_JOIN:");
				for (i=0; i<num_files; i++)
				{   if ((strlen(str) + strlen(info_array[i].in_filename) + 20) > 78)
						strcat(str,"\n");
					sprintf(str2," %s trace %d thru %d;",
						info_array[i].in_filename,
						info_array[i].first_trace,
						info_array[i].last_trace);
					strcat(str,str2);
				}
				strcat(proc_hist,str);
			}
			count = 0;
			for (i=0; i<num_files; i++)
				count += (info_array[i].last_trace - info_array[i].first_trace + 1);
			SetSgyFileHeader((int)GprInfo.time_date_created.day,
							 (int)GprInfo.time_date_created.month,
							 (int)GprInfo.time_date_created.year+1980,
							 (long)count,(long)GprInfo.in_samps_per_trace,
							 (long)GprInfo.timezero_sample,
							 GprInfo.in_time_window,
							 start_pos,final_pos,
							 GprInfo.position_step_size,GprInfo.position_units,
							 (double)GprInfo.nominal_frequency,
							 GprInfo.antenna_separation,GprInfo.pulser_voltage,
							 GprInfo.number_of_stacks,GprInfo.survey_mode,
							 proc_hist,GprInfo.text,GprInfo.job_number,
							 GprInfo.title1,GprInfo.title2,out_filename,
							 0.0,0.0,0,NULL,&ReelHdr);
			fwrite((void *)&ReelHdr,sizeof(struct SegyReelHdrStruct),1,outfile);
			free(proc_hist);
			count = 0;
			for (ifile=0; ifile<num_files; ifile++)
			{    /* Open input SGY file */
                if ((infile = fopen(info_array[ifile].in_filename,"rb")) == NULL)
                {   printf("\nGPR_JOIN: ERROR - Unable to open input SGY file %s\n",
                            info_array[ifile].in_filename);
                    if (access(info_array[ifile].in_filename,4))
					{   if      (errno == ENOENT) puts("                  File or path name not found.");
                        else if (errno == EACCES) puts("                  Read Access denied.");
                        else                      puts("                  Unknown access error.");
                    }
                    exit(5);
                }

                /* Speed up disk access by using large (conventional memory)
                   buffer. NOTE: stream errors are not reported to fwrite() or
                   fread() until the buffer setup by setvbuf() is emptied!
                   If vbuf_size is greater than the filesize ferror() will
                   not report errors! */
                #if defined(_INTELC32_)
                  realmalloc(inbuffer,vbuf_size);
                  if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbuf_size);
                  else           setvbuf(infile,NULL,_IOFBF,vbuf_size);
                #else
                  setvbuf(infile,NULL,_IOFBF,vbuf_size);
                #endif

                /* Setup/allocate RAM buffer to speed transfer */
                buf_size = vbuf_size;
                switch (ReelHdr.format)
                {   case 1:  case 2:  case 4:
                        data_block_bytes = num_samps * 4;
                        break;
                    case 3:
                        data_block_bytes = num_samps * 2;
                        break;
                    default:
                        printf("\nGPR_JOIN - ERROR: Unvalid SEG-Y sample format code.\n");
                        exit(7);
                }
                hdr_block_bytes = sizeof(struct SegyTraceHdrStruct);
                disk_trace_size = data_block_bytes + hdr_block_bytes;
                disk_chunk_size = disk_trace_size;
				last_byte = sizeof(struct SegyReelHdrStruct) +
							((info_array[ifile].last_trace+1) * disk_trace_size);
				filebytes = (info_array[ifile].last_trace - info_array[ifile].first_trace + 1)
                            * disk_trace_size;
				if (filebytes < buf_size)  buf_size = filebytes;
                else                       buf_size = (buf_size/disk_chunk_size) * disk_chunk_size;
                while ((buf_size>=disk_chunk_size) && ((v_ptr=malloc(buf_size))==NULL))
                    buf_size -= disk_chunk_size;
                if (v_ptr == NULL || buf_size < disk_chunk_size)
				{   puts("\nGPR_JOIN: ERROR - Unable to allocate temporary storage.");
                    fclose(infile); fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(outbuffer);
                    #endif
                    free(v_ptr);
                    exit(7);
                }
                if (filebytes % buf_size)  chunks = filebytes/buf_size + 1;
                else                       chunks = filebytes/buf_size;

                /* Move the file pointer to start of first trace */
                offset = sizeof(struct SegyReelHdrStruct) +
                         (info_array[ifile].first_trace * disk_trace_size);
                fseek(infile,(size_t)offset,SEEK_SET);

                /* Copy traces */
                error = FALSE;
                printf("GPR_JOIN: Copying %s to %s ...                       ",
                       info_array[ifile].in_filename,out_filename);
                for (i=0; i<chunks; i++)
                {   if (fread(v_ptr,buf_size,(size_t)1,infile) < 1)
                    {   error = TRUE;
                        break;                /* out of for (i...) loop */
                    }
                    for (j=0; j<buf_size; j+=disk_trace_size, count++)
                    {    /* update trace header */
                        TraceHdr = (struct SegyTraceHdrStruct *)((char *)v_ptr+j);
                        TraceHdr->tracl = TraceHdr->tracr = TraceHdr->fldr = count;
                    }
                    if (fwrite(v_ptr,buf_size,(size_t)1,outfile) < 1)
                    {   error = TRUE;
                        break;                /* out of for (i...) loop */
                    }
                    if (ferror(infile) || ferror(outfile))
                    {   error = TRUE;
                        break;                /* out of for (i...) loop */
                    }
                    current_byte = ftell(infile);
					if (buf_size > (last_byte - current_byte))
					{   buf_size = last_byte - current_byte;
                    }
                }
                puts("");
                free(v_ptr);
				fclose(infile);
                #if defined(_INTELC32_)
                  realfree(inbuffer);
                #endif
                if (error == TRUE)
                {   printf("\nGPR_JOIN - ERROR: I/O error reported at chunk %d\n",i);
                    puts("     WARNING: Joining operation may not be complete!");
                    clearerr(infile);  clearerr(outfile);
                    fclose(outfile);
                    #if defined(_INTELC32_)
                      realfree(outbuffer);
                    #endif
                    exit(10);
                }
            }   /* end of for (ifile=0; ifile<num_files; ifile++) */

            fclose(outfile);
            #if defined(_INTELC32_)
              realfree(outbuffer);
            #endif

            break;
        }  /* end of case SGY: block */
        default:
            puts("\nGPR_JOIN - ERROR: GPR file storage format is not recognized.");
            puts("           Use program GPR_CONV to convert user-defined or unrecognized");
			puts("           format to the GSSI DZT, S&S DT1/HD, or SEG-Y format.");
            exit(99);
    }  /* end of switch (file_type) block */

    puts("\nGPR_JOIN finished.");
    exit(0);
}
/******************************** ExitFunc1() *******************************/
void ExitFunc1(void)
{
    if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}

/***************************** END OF GPR_JOIN *****************************/
