/******************************** GPR_RHDR.C ********************************\
 *                            version 1.08.08.01
 * GPR_RHDR reads and displays header information for ground penetrating
 * radar files.  The following computer storage formats are recognized.
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
 *  May 19, 1994      - First recorded date of revision - DZT_RHDR.C.
 *  October 19, 1995  - Modified manifest constants and ConvertProcHist()
 *                      to reflect change in how static stacking coded in
 *                      header.
 *                    - Renamed PrintOneDztHeader() to PrintOneDztHeader0().
 *                    - Reordered search for antenna ID number to handle
 *                      new format in PrintOneDztHeader0().
 *                    - Added another pause in PrintOneDztHeader0().
 *  November 9, 1995  - Rename to DZT_RHDR.C.
 *                    - Let program continue if GetDztHdrs() returns "WARNING".
 *  November 20, 1995 - Relinked using updated gpr_io.lib.
 *                      Added color-coded warning/error messages.
 *  February 6, 1996  - Rename to GPR_RHDR.C v. 1.00.
 *                    - Updated DZT functions.
 *                    - jl_util1.lib must now be linked at compile time.
 *  February 7, 1996  - Add functions to read DT1/HD and SGY files.
 *  February 8, 1996  - Corrected how ANSI_there called.
 *                    - Added error reporting for return from GetGprFileType().
 *  February 14, 1996 - exit() was missing if GetGprFileType() returned error.
 *                    - Added check for trace count in DT1 and SGY sections.
 *  August 8, 2001    - Updated time and date functions
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /zfloatsync GPR_RHDR.C gpr_io.lib jl_util1.lib
 *     To compile for 80486 executable :
 *       icc /F /xnovm /zfloatsync /zmod486 GPR_RHDR.C gpr_io.lib jl_util1.lib
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /zmod486 generates specific 80486 instructions, speeds up program on 80486
 *     Jeff Lucius's GPR I/O functions (DZT, DT1, SGY) are in gpr_io.lib.
 *     Jeff Lucius's utility functions are in jl_util1.lib.
 *
 *     to remove assert() functions from code also add:
 *     /DNDEBUG
 *
 * To run: GPR_RHDR gpr_filename [/a]
 *
\****************************************************************************/

/********************** Include required header files ***********************/
/* non-ANSI headers */
#include <conio.h>     /* getch,kbhit */
#include <errno.h>     /* errno */
#include <io.h>        /* access */
#include <dos.h>       /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <stdio.h>     /* fopen,fclose,printf,puts,gets,fseek,SEEK_SET,fread */
#include <stdlib.h>    /* exit,atoi,atof,malloc,free */
#include <string.h>    /* strcpy,strstr,strncpy,strupr,strchr,strcat */

/* application headers */
#include "gpr_io.h"    /* to use gpr_io.lib functions, etc. */
#include "jl_defs.h"   /* common manifest constants and macros */
#include "jl_util1.h"  /* ANSI_there, etc. */

/*************************** Manifest constants ****************************/
#define GPR_RHDR_VER "1.08.08.01"
#define DATETIME "August 8, 2001"

/**************************** Global variables ******************************/
int Debug = FALSE;     /* if TRUE, turn debugging on */

/********************************** main() **********************************/
void main (int argc, char *argv[])
{
	char   gpr_filename[MAX_PATHLEN];
    int    itemp, discover,min_args;
    int    file_hdr_bytes, num_hdrs, trace_hdr_bytes, samples_per_trace;
    int    num_channels, input_datatype, total_time, file_type;
    long   num_traces;

	/* following in errno.h */
	extern int errno;

	/* following in gpr_io.h */
	extern const char *GetGprFileTypeMsg[];

	/* Verify usage */
    min_args = 2;
    if (argc < min_args)
	{   puts("\n");
        if (ANSI_there())  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
        puts("");
        puts("###############################################################################");
        puts("   Usage: GPR_RHDR gpr_filename [/a]");
        puts("     Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
        puts("   This program displays header information for GPR files.");
		puts("   Recognized storage formats include: GSSI DZT, S&S DT1/HD, and SEG-Y.");
		puts("\n   Required command line arguments:");
		puts("     gpr_filename - name of GPR data file");
		puts("\n   Optional command line arguments (do not include brackets):");
		puts("     /a - DZT files : display 1st two samples from each DZT trace, till Esc");
		puts("                      pressed.  Will also attempt to define tick marker ID's.");
		puts("        - DT1 and SGY files: display each trace header, till ESC pressed.");
		puts("     /d - turn debugging on.");
        printf("  Version %s - Last Revision Date: %s at %s\n",GPR_RHDR_VER,DATETIME,__TIME__);
		puts("###############################################################################");
		if (ANSI_there()) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
        exit(1);
    }

	/* Get command line arguments */
	strcpy(gpr_filename,argv[1]);
	discover = FALSE;
	for (itemp = min_args; itemp<argc; itemp++)
	{   if (strstr(strupr(argv[itemp]),"/A")) discover = TRUE;
		if (strstr(strupr(argv[itemp]),"/D")) Debug = TRUE;
	}

	/* Determine storage format */
	itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
				   &samples_per_trace,&num_channels,&num_traces,
				   &input_datatype,&total_time,&file_type,gpr_filename);
	if (itemp > 0)
	{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
		printf("%s\n",GetGprFileTypeMsg[itemp]);
		if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
		exit(2);
	}

	/* Get the info and print */
	switch (file_type)
	{   case DZT:  case MOD_DZT:
		{   int itemp,num_hdrs,header_size,channel,selection;
            long num_traces,trace;
            struct DztHdrStruct hdr;

            /* following in gpr_io.h */
			extern const char *ReadOneDztHeaderMsg[];

			/* Get first header from the file */
            channel = 0;
            itemp = ReadOneDztHeader(gpr_filename,&num_hdrs,&num_traces,channel,
                             &header_size,&hdr);
            if (itemp > 0)
			{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
				printf("%s\n",ReadOneDztHeaderMsg[itemp]);
				if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                    exit(4);
			}

            /* If more than one header ask user to select */
            if (num_hdrs > 1)
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

            /* Get header if selection not first one */
            channel = selection - 1;
			if (channel > 0)
            {   itemp = ReadOneDztHeader(gpr_filename,&num_hdrs,&num_traces,channel,
                                         &header_size,&hdr);
				if (itemp > 0)
				{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
                    printf("%s\n",ReadOneDztHeaderMsg[itemp]);
					if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                    if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                        exit(4);
				}
            }

            /* Print the header information */
			itemp = PrintOneDztHeader(header_size,num_traces,"stdout",gpr_filename,&hdr);
            if (itemp > 0)
			{   if (ANSI_there())  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
                printf("\n%s\n",PrintOneDztHeaderMsg[itemp]);
				if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                exit(4);
            }

            if (discover)
            {   unsigned char uc1,uc2,uc_norm1,uc_norm2,uc_mark1,uc_mark2,uc_temp1,uc_temp2;
                unsigned short us1,us2,us_norm1,us_norm2,us_mark1,us_mark2,us_temp1,us_temp2;
                int key,same_count,min_same,v5xmark_count;
				long disk_chunk_bytes,disk_trace_bytes;
                void *vptr;
                FILE  *IO_file;

				/* allocate space for one chunk of traces */
                disk_trace_bytes = (hdr.rh_bits/8) * hdr.rh_nsamp;
                disk_chunk_bytes = disk_trace_bytes * hdr.rh_nchan;
                if ((vptr = malloc(disk_chunk_bytes)) == NULL)
				{   puts("GPR_RHDR - ERROR: unable to allocate storage for one chunk of traces.");
                    exit(6);
                }

                /* Open file */
                if ((IO_file = fopen(gpr_filename,"rb")) == NULL)
                {   printf("GPR_RHDR - ERROR: Unable to open input file %s\n",gpr_filename);
					if (access(gpr_filename,4))
                    {   if      (errno == ENOENT) puts("File or path name not found.");
                        else if (errno == EACCES) puts("Read Access denied.");
                        else                      puts("Unknown access error.");
                    }
                    free(vptr);
                    exit(6);
                }

				/* List first 2 samples of chan 0 trace till ESC pressed and
                   auto-detect marker signature.
                 */
				printf("Press a key to start displaying first 2 samples from each trace chunk.\n");
				printf("  Then press ESC to quit displaying ...");
				key = getch();
				puts("");
				fseek(IO_file,(long)(num_hdrs * header_size),SEEK_SET);
				v5xmark_count = same_count = trace = 0;
				min_same = 10;       /* must have 10 adjacent normal traces */
				us_temp1 = us_temp2 = uc_temp1 = uc_temp2 = 0;
				us_norm1 = us_norm2 = uc_norm1 = uc_norm2 = 0;
				us_mark1 = us_mark2 = uc_mark1 = uc_mark2 = 0;
				while (key != ESC)
				{   if (fread(vptr,disk_chunk_bytes,1,IO_file) < 1)
					{   puts("GPR_RHDR - ERROR: short count from fread() getting traces.");
						free(vptr);
						fclose (IO_file);
						exit(6);
					}
					trace++;
					switch (hdr.rh_bits)
					{   case 8:
							uc1 = ((unsigned char *)(vptr))[0];
							uc2 = ((unsigned char *)(vptr))[1];
                            printf("trace = %5ld   sample 1 = %02X (%hu)   sample 2 = %02X  (%hu)\n",
                                    trace,(unsigned short)uc1,(unsigned short)uc1,
                                          (unsigned short)uc2,(unsigned short)uc2);
                            if (min_same > 0 && uc_temp1 == uc1 && uc_temp2 == uc2)
                            {   same_count++;
                                if (Debug) printf("same_count = %d  uc1 = %u  uc2 = %u\n",same_count,(unsigned)uc1,(unsigned)uc2);
								if (same_count >= min_same)
                                {   uc_norm1 = uc1;
                                    uc_norm2 = uc2;
                                    min_same = -1;
                                }
                            } else
                               same_count = 0;
                            if (min_same == -1 && (uc_temp1 != uc1 || uc_temp2 != uc2))
                            {   uc_mark1 = uc1;
								uc_mark2 = uc2;
                                min_same = -2;
                            }
							uc_temp1 = uc1;
							uc_temp2 = uc2;
                            if (uc2 == 0xE8) v5xmark_count++;
                            break;
                        case 16:
                            us1 = ((unsigned short *)(vptr))[0];
                            us2 = ((unsigned short *)(vptr))[1];
                            printf("trace = %5ld   sample 1 = %04X (%hu)   sample 2 = %04X  (%hu)\n",
									trace,us1,us1,us2,us2);
                            if (min_same > 0 && us_temp1 == us1 && us_temp2 == us2)
                            {   same_count++;
                                if (Debug) printf("same_count = %d  uc1 = %u  uc2 = %u\n",same_count,(unsigned)us1,(unsigned)us2);
                                if (same_count >= min_same)
                                {   us_norm1 = us1;
                                    us_norm2 = us2;
                                    min_same = -1;
                                }
							} else
                               same_count = 0;
                            if (min_same == -1 && (us_temp1 != us1 || us_temp2 != us2))
							{   us_mark1 = us1;
								us_mark2 = us2;
								min_same = -2;
							}
							us_temp1 = us1;
							us_temp2 = us2;
							if (us2 == 0xE800) v5xmark_count++;
							break;
					}
					if (trace >= num_traces) break;  /* out of while loop */
                    if (kbhit()) key = getch();
                }
                free(vptr);
                fclose(IO_file);
                switch (hdr.rh_bits)
                {   case 8:
                        puts("\n\n   Normal trace samples          Marker trace samples");
						printf("[0]=%02X (%05hu) [1]=%02X (%05hu)     [0]=%02X (%05hu) [1]=%02X (%05hu)\n",
                               (unsigned short)uc_norm1,(unsigned short)uc_norm1,(unsigned short)uc_norm2,(unsigned short)uc_norm2,
                               (unsigned short)uc_mark1,(unsigned short)uc_mark1,(unsigned short)uc_mark2,(unsigned short)uc_mark2);
						printf("\nNumber of DZT v.5x \"marks\" (0xE8) found = %d\n",v5xmark_count);
						puts("\n\"Normal traces\" must repeat at least 10 consecutive times.");
                        break;
                    case 16:
                        puts("\n\n        Normal trace samples                  Marker trace samples");
                        printf("[0]=%04X (%05hu) [1]=%04X (%05hu)     [0]=%04X (%05hu) [1]=%04X (%05hu)\n",
                               us_norm1,us_norm1,us_norm2,us_norm2,
                               us_mark1,us_mark1,us_mark2,us_mark2);
						printf("\nNumber of DZT v.5x \"marks\" (0xE800) found = %d\n",v5xmark_count);
						puts("\n\"Normal traces\" must repeat at least 10 consecutive times.");
                        break;
                }
            }
            break;
        }
        case DT1:
		{   char hd_filename[MAX_PATHLEN];
			char *c_ptr;
			int key,print_it;
			long trace;
			struct SsHdrInfoStruct  HdInfo;     /* buffer to hold info from HD file */

			/* following in gpr_io.h */
			extern const char *GetSsHdFileMsg[];

			/* Construct HD filename */
			hd_filename[0] = 0;
			strcpy(hd_filename,gpr_filename);
			if ((c_ptr=strchr(hd_filename,'.')) != NULL)
				*c_ptr = 0;               /* terminated string at period */
            strcat(hd_filename,".hd");

            /* Get info from HD file */
            InitDt1Parameters(&HdInfo);
            if (Debug) print_it = TRUE;
            else       print_it = FALSE;
            itemp = GetSsHdFile(print_it,&HdInfo,hd_filename);
			if (itemp > 0)
			{   printf("\n%s\n",GetSsHdFileMsg[itemp]);
				exit(2);
			}
            PrintSsHdInfo("stdout",hd_filename,&HdInfo);

            if (discover)
            {   int itemp,num_samps,data_block_bytes;
                short int *s_data;    /* pointer to buffer for 1 data trace */
				struct SsTraceHdrStruct TraceHdr;  /* buffer to hold 1 trace header */
				FILE  *IO_file;

				/* following in gpr_io.h */
                extern const char *GetSsTraceMsg[];

                /* Allocate storage for one data trace */
                num_samps = HdInfo.num_samples;
                data_block_bytes = num_samps * 2; /* data are signed short ints */
                s_data = (short *)malloc((size_t)(data_block_bytes));
                if (s_data == NULL)
				{   printf("GPR_RHDR - ERROR: Unable to allocate storage for data block\n");
					exit(6);
				}

                /* Open file */
                if ((IO_file = fopen(gpr_filename,"rb")) == NULL)
                {   printf("GPR_RHDR - ERROR: Unable to open input file %s\n",gpr_filename);
                    if (access(gpr_filename,4))
                    {   if      (errno == ENOENT) puts("File or path name not found.");
						else if (errno == EACCES) puts("Read Access denied.");
						else                      puts("Unknown access error.");
                    }
					free(s_data);
                    exit(6);
                }

                /* Get traces and print trace header */
                printf("Press a key to start displaying each trace header ...");
                key = getch();
                puts("");
				trace = 0;
				do
				{   itemp = GetSsTrace(data_block_bytes,&TraceHdr,s_data,IO_file);
					if (itemp > 0)
					{   printf("\n%s\n",GetSsTraceMsg[itemp]);
						free(s_data);
						fclose(IO_file);
						exit(6);
					}
					trace++;
					fprintf(stdout,"\n");
					PrintSsTraceHdr("stdout",&TraceHdr);
					if (trace >= HdInfo.num_traces) break;
					fprintf(stdout,"Press <Esc> to quit, other key to continue ...");
				} while (getch() != ESC) ;
				free(s_data);
				fclose(IO_file);
            }
            break;
        }
        case SGY:
        {   int itemp,key;
			int swap_bytes,num_traces;
			long trace;
			FILE  *IO_file;
			struct SegyReelHdrStruct ReelHdr; /* buffer top hold reel header */

            /* following in gpr_io.h */
            extern const char *GetSegyReelHdrMsg[];

			/* Open file */
            if ((IO_file = fopen(gpr_filename,"rb")) == NULL)
            {   printf("\nGPR_RHDR - ERROR: Unable to open input file %s\n",gpr_filename);
                if (access(gpr_filename,4))
                {   if      (errno == ENOENT) puts("File or path name not found.");
                    else if (errno == EACCES) puts("Read Access denied.");
					else                      puts("Unknown access error.");
                }
                exit(4);
            }

            /* Get the reel header from the file */
            itemp = GetSegyReelHdr(&swap_bytes,&num_traces,&ReelHdr,IO_file);
			if (itemp > 0)
			{   printf("\n%s\n",GetSegyReelHdrMsg[itemp]);
				fclose(IO_file);
                exit(4);
            }

            /* Print the reel header information */
			PrintSegyReelHdr(swap_bytes,num_traces,"stdout",gpr_filename,&ReelHdr);

			if (discover)
			{   int itemp,format,num_samps;
				int data_block_bytes;     /* bytes in data trace */
                void *v_data;             /* pointer to buffer for 1 data trace */
				struct SegyTraceHdrStruct TraceHdr;   /* buffer top hold 1 trace header */

                /* following in gpr_io.h */
                extern const char *GetSegyTraceMsg[];

                /* Allocate storage for one data trace */
                switch(ReelHdr.format)
				{   case 3:
						data_block_bytes = ReelHdr.hns * 2;
						break;
                    default:
                        data_block_bytes = ReelHdr.hns * 4;
                        break;
                }
				v_data = (void *)malloc((size_t)(data_block_bytes));
                if (v_data == NULL)
                {   printf("\nGPR_RHDR - ERROR: Unable to allocate storage for data block\n");
                    fclose(IO_file);
					exit(6);
                }

                /* Get traces and print trace header */
                printf("Press a key to start displaying each trace header ...");
                key = getch();
                puts("");
                format = ReelHdr.format;
                num_samps = ReelHdr.hns;
				trace = 0;
				do
				{   itemp = GetSegyTrace(swap_bytes,format,data_block_bytes,num_samps,
								 &TraceHdr,v_data,IO_file);
					if (itemp > 0)
					{   printf("\n%s\n",GetSegyTraceMsg[itemp]);
						free(v_data);
						fclose(IO_file);
						exit(6);
					}
					trace++;
					fprintf(stdout,"\n");
					PrintSegyTraceHdr("stdout",&TraceHdr);
					if (trace >= num_traces) break;
					fprintf(stdout,"Press <Esc> to quit, other key to continue ...");
				} while (getch() != ESC) ;
			}
			break;
		}
		default:
			puts("GPR_RHDR - ERROR: GPR file storage format is not recognized.");
			puts("           Use program GPR_CONV to convert user-defined or unrecognized");
			puts("           format to the GSSI DZT, S&S DT1/HD, or SEG-Y format.");
			exit(3);
	}
	exit(0);
}
/***************************** end of gpr_rhdr.c ****************************/

