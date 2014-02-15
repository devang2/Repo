/******************************** S10_EHDR.C ********************************\
 * This program allows the user to edit most fields in a SIR-10A header.
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
 *  November 9, 1993  - completed.
 *  February 7, 1994  - added automatic "modification date/time" update.
 *  July 7, 1995      - recompiled using newer dzt_pgms.h and updated
 *                      PrintOneDztHdr().
 *                    - Added 2 optional command arguments to allow batch
 *                      mode operation.
 *  October 19, 1995  - Modified manifest constants and ConvertProcHist2()
 *                      to reflect change in how static stacking coded in
 *                      header.
 *                    - Changed user response to S or s to save to disk.
 *                    - Reordered search for antenna ID number to handle
 *                      new format in main(), PrintOneDztHeader0(), and
 *                      PrintOneDztHeader().
 *                    - Added another pause in PrintOneDztHeader0() and
 *                      PrintOneDztHeader().
 *  October 20, 1995  - Modified EditDztHeader() to possibly add ')' at end
 *                      of hdrPtr->rh_antname[].
 *  April 23, 2001    - Fixed bug, where "show_help" was not set to false
 *                      (initialized) when in batch mode. THis led to infinite
 *                      looping.
 *                    - Commented out PR_STS_NEW.
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm S10_EHDR.C
 *     To compile for 80486 executable :
 *       icc /F /xnovm /zmod486 S10_EHDR.C
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program on 80486
 *
 *   Borland C++ 3.1
 *     bcc -3 -a -d -f287 -ml -O2 S10_EHDR.C
 *
 *     -3 generates 80386 protected mode compatible instructions
 *     -a alignes code on (2-byte) word boundaries
 *     -d merges duplicates strings
 *     -f287 invokes 80287 instructions for floating point math
 *     -ml uses the large memory model
 *     -O2 optimizes for fastest code
 *
 *  To run: S10_EHDR dzt_filename
 *
\****************************************************************************/

/* non-ANSI headers */
#include <conio.h>    /* getch,kbhit,getche */
#include <errno.h>    /* errno */
#include <io.h>       /* access */
#include <time.h>     /* struct tm,time_t */

/* ANSI-compatible headers */
#include <stdio.h>    /* fopen,fclose,printf,puts,gets,rewind,fwrite,fseek */
#include <stdlib.h>   /* exit,atoi,atof,size_t */
#include <string.h>   /* strcpy,strstr,strncpy,strlen */

/* application headers */
#include "dzt_pgms.h" /* manifest constants, macros, data types,
						   global variables, function prototypes */

void main (int argc, char *argv[])
{
	char                 dzt_filename[MAX_PATHLEN];
	char                 ant_freq[12];
	unsigned short       us_val, rg_breaks, us_oldval1, us_oldval2, checksum;
	unsigned short       us_newval;
    int                  itemp, num_hdrs, num_scans, selection, loop, num;
    int                  i, show_help, i_val, rg_break_delta;
    int                  num_hdrflds = 32;   /* 0 - 31 */
	int                  batch = FALSE, field_number;
    int                  i_newval;
    float               *f_ptr,f_val, f_oldval1, rg_values[MAX_RG_BREAKS];
    float                f_newval;
    FILE                *IO_file;
    struct DztDateStruct timedate;
    struct DztHdrStruct  hdr1, hdr2, hdr3, hdr4, *hdrPtr;
    struct DztHdrStruct *hdrPtr_array[4];
    struct tm *newtime;
	time_t long_time;

    /* following in dzt_pgms.h */
    extern const char *month_abbr[];
	extern const char *ant_number[];
    extern const char *ant_name[];

    /* following in errno.h */
    extern int errno;

    /* static declaration stores variable in "global" area, not on stack */
	static char      str[450], c_oldval1[450], buff[1024];
    static const int bufsiz = 1024;

	/* Verify usage */
	if (argc < 2)
	{   puts("                                                                               ");
		puts("                                                                               ");
		printf("%c%c37;44m",0x1B,0x5B);
		puts("                                                                               ");
		puts("###############################################################################");
		puts("   Usage: S10_EHDR dzt_filename [field_number  new_value]                      ");
		puts("    This program allows users to edit most fields in a SIR-10A header.         ");
		puts("    The modified file header is replaced into the input file.                  ");
		puts("                                                                               ");
		puts("   Required command line arguments:                                            ");
		puts("     dzt_filename - name of the GSSI SIR-10 DZT file.                          ");
		puts("                                                                               ");
		puts("   Optional command line arguments:                                            ");
		puts("     field_number - the header field number to be modified.                    ");
		puts("     new_value - the value for the header field.                               ");
		puts("     Both must be specified and the program will operate in batch mode.        ");
		puts("     Only the following fields can be modified in batch mode:                  ");
		puts("       0, 2, 3, 10, 19           - use unsigned short values                   ");
		puts("       5, 6, 7, 8, 9, 20, 21, 22 - use floating point values                   ");
		puts("     All other fields either cannot be modified or must be modified in         ");
		puts("       interactive mode.                                                       ");
		puts("     NOTE: Batch mode will not work if the file contains multiple channels.    ");
		puts("                                                                               ");
		printf("   Last Revision Date: April 23, 2001 at %s\n",__TIME__);
		puts("###############################################################################");
		printf("%c%c37;40m",0x1B,0x5B);
		exit(1);
	}

	/* Get command line arguments */
	strcpy(dzt_filename,argv[1]);
	if (argc == 4)
	{   field_number = atoi(argv[2]);
		batch = TRUE;
		switch (field_number)
		{   case 0:  case 2:  case 3:  case 19:
				us_newval = atoi(argv[3]);
				break;
			case 5:  case  6:  case  7:  case  8:
			case 9:  case 20:  case 21:  case 22:
				f_newval = atof(argv[3]);
				break;
			case 10:
				i_newval = atoi(argv[3]);
				break;
			default:
				field_number = -1;
				batch = FALSE;
				break;
		}
	}

	/* Open file */
	if ((IO_file = fopen(dzt_filename,"rb+")) == NULL)
	{   printf("Unable to open input file %s\n",dzt_filename);
		if (access(dzt_filename,6))
		{   if      (errno == ENOENT) puts("File or path name not found.");
			else if (errno == EACCES) puts("Read and Write Access denied.");
			else                      puts("Unknown access error.");
		}
		exit(2);
	}

	/* Get the headers from the file */
	itemp = GetDztHdrs(&num_hdrs,&num_scans,&hdr1,&hdr2,&hdr3,&hdr4,IO_file);
	if (itemp > 0)
	{   printf("\n%s\n",GetDztHdrsMsg[itemp]);
		fclose(IO_file);
		exit(3);
	}

	/* If more than one header ask user to select */
	if (num_hdrs > 1)
	{   batch = FALSE;
		field_number = -1;
		puts("\n\nSelect SIR-10A header to use.");
		do
		{   printf("\r                                       \r");
			printf("Enter a number from 1 to %d --->",num_hdrs);
			selection = getche();
			selection -= '0';
		} while ((selection < 1) || (selection > num_hdrs));
		puts("\n");
	} else
		selection = 1;

	/* Assign header adresses to pointer array members, then point to selection */
	hdrPtr_array[0] = &hdr1;
	hdrPtr_array[1] = &hdr2;
	hdrPtr_array[2] = &hdr3;
	hdrPtr_array[3] = &hdr4;
	hdrPtr = hdrPtr_array[selection-1];

	/* Print the header information */
	if (!batch)
	{   itemp = PrintOneDztHeader((int)sizeof(struct DztHdrStruct),
					 (long)num_scans,"stdout",dzt_filename,hdrPtr);
		if (itemp > 0)
		{   printf("\n%s\n",PrintOneDztHeaderMsg[itemp]);
			exit(4);
		}
#if 0
		printf("Press a key to continue ...");
		if (!getch()) getch();
#endif
	}

    /* Update modification time/date */
    time( &long_time );                 /* Get time as long integer */
	newtime = localtime( &long_time );  /* Convert to local time */
    hdrPtr->rh_modif.month = newtime->tm_mon + 1;     /* 0 - 11 */
	hdrPtr->rh_modif.day   = newtime->tm_mday;        /* 1 - 31 */
    hdrPtr->rh_modif.year  = newtime->tm_year - 80;   /* year - 1900 */
    hdrPtr->rh_modif.hour  = newtime->tm_hour;        /* 0 - 24 */
	hdrPtr->rh_modif.min   = newtime->tm_min;
    hdrPtr->rh_modif.sec2  = newtime->tm_sec/2;

    /* Update checksum ! */
    hdrPtr->rh_chksum = 0;
    checksum = 0;
	for (i=0; i<512; i++)
        checksum += *((unsigned short *)hdrPtr + i);
	hdrPtr->rh_chksum = checksum;

	/* Loop till user presses Q or q, and change requested header member */
	loop = TRUE;
    show_help = FALSE;
	while (loop == TRUE)
	{   if (!batch)
		{   /* ask user which field to change */
			do
			{   printf("\nEnter a number from 0 to %d (\"h\" = help;  \"d\" = done) ===> ",num_hdrflds);
				gets(str);
				num = -1;
				show_help = FALSE;
				if (strstr(str,"H") || strstr(str,"h"))
				{   show_help = TRUE;
					break;
				} else if (strstr(str,"D") || strstr(str,"d"))
				{   loop = FALSE;
					break;
				} else if (str[0] == 0)
				{   continue;
				} else
				{	num = atoi(str);
				}
			} while ( (num < 0) || (num >= num_hdrflds) );
		}

		/* exit loop if requested */
		if (loop == FALSE) break;

		/* show help screen if requested */
		if (show_help)
		{   puts("field  field_type           field_name      field_description");
			puts("  0    unsigned short       rh_tag;         select 0 to 3, the header number");
			puts("  2    unsigned short       rh_nsamp;       samples per scan");
			puts("  3    unsigned short       rh_bits;        bits per data word");
			puts("  5    float                rh_sps;         scans per second");
			puts("  6    float                rh_spm;         scans per meter");
			puts("  7    float                rh_mpm;         meters per mark");
			puts("  8    float                rh_position;    position (ns)");
			puts("  9    float                rh_range;       range (ns)");
			puts(" 10    unsigned short       rh_npass;       scans per pass for 2D files");
			puts(" 11    struct DztDateStruct rh_create;      date/time created");
			puts(" 12    struct DztDateStruct rh_modif;       date/time modified");
			puts(" 19    unsigned short       rh_nchan;       number of channels");
			puts(" 20    float                rh_epsr;        average dielectric constant");
			puts(" 21    float                rh_top;         top position in meters");
			puts(" 22    float                rh_depth;       range in meters");
			puts(" 24    char                 rh_antname[14]; antenna number or name");
			puts(" 26    char                 rh_name[12];    the file name");
			puts(" 28    unsigned short       num_rg_breaks   number of range gain break points");
			puts(" 29    float *              rg_values[]     the range gain values in db");
			puts(" 30    char *               text            comments");
			puts(" 31    char *               proc_hist       coded processing history");
			printf("Other fields can not be modified (only viewed).\n");
			continue;
		}

		/* change the field */
		if (batch) num = field_number;
		switch (num)
		{   case 0:
				us_oldval1 = hdrPtr->rh_tag;
				printf("\nCurrent header tag = 0x%04hX\n",us_oldval1);
				if (batch)
				{   us_val = us_newval;
					printf("New header number = %hu\n",us_val);
				} else
				{   printf("ENTER the new header number (1 to 4) --->");
					us_val = atoi(gets(str));
				}
				if (us_val>=1 && us_val <= 4)
				{   us_val--;
					EditDztHeader(num,(void *)&us_val,hdrPtr);
					printf("new rh_tag = 0x%04hX\n",hdrPtr->rh_tag);
				} else
					puts("Invalid value: header number must range from 1 to 4.");
				break;
			case 1:
				puts("rh_data (offset to data) is a non-editable field.");
				printf("\tcurrent rh_data = %hu\n",hdrPtr->rh_data);
				break;
			case 2:
				us_oldval1 = hdrPtr->rh_nsamp;
				printf("\nCurrent number of samples per scan = %hu\n",us_oldval1);
				if (batch)
				{   us_val = us_newval;
					printf("New number of samples per scan = %hu\n",us_val);
				} else
				{   printf("ENTER new number of samples per scan (256, 512, or 1024) --->");
					us_val = atoi(gets(str));
				}
				if (us_val == 256 || us_val == 512 || us_val == 1024)
				{   EditDztHeader(num,(void *)&us_val,hdrPtr);
					printf("new rh_nsamp = %hu\n",hdrPtr->rh_nsamp);
                } else
					puts("Invalid valid: number of samples must be 256, 512, or 1024.");
                break;
            case 3:
                us_oldval1 = hdrPtr->rh_bits;
                us_oldval2 = hdrPtr->rh_zero;
				printf("\nCurrent number of bits per data word = %hu  (rh_zero = 0x%04hX)\n",
						us_oldval1,us_oldval2);
                if (batch)
                {   us_val = us_newval;
					printf("New number of bits per data word = %hu\n",us_val);
				} else
				{   printf("ENTER new number of bits per data word (8 or 16) --->");
                    us_val = atoi(gets(str));
                }
                if (us_val == 8 || us_val == 16)
                {   EditDztHeader(num,(void *)&us_val,hdrPtr);
                    printf("new rh_bits = %hu  (new rh_zero = 0x%04hX)\n",
							hdrPtr->rh_bits,hdrPtr->rh_zero);
				} else
                    puts("Invalid value: number of bits per data word must be 8 or 16.");
				break;
            case 4:
                puts("rh_zero (mid-point of data) is a non-editable field (is set by rh_bits).");
                printf("\tcurrent rh_zero = 0x%04hX\n",hdrPtr->rh_zero);
                break;
			case 5:
				f_oldval1 = hdrPtr->rh_sps;
				printf("\nCurrent number of scans per second = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("New number of scans per second = %f\n",f_val);
                } else
                {   printf("ENTER new number of scans per second (0.10 or 128.0) --->");
                    f_val = atof(gets(str));
                }
                if (f_val >= 0.10 && f_val <=128.0)
                {   EditDztHeader(num,(void *)&f_val,hdrPtr);
					printf("new rh_sps = %f\n",hdrPtr->rh_sps);
				} else
                    puts("Invalid value: number of scans per scan must range from 0.10 and 128.0.");
				break;
            case 6:
                f_oldval1 = hdrPtr->rh_spm;
                printf("\nCurrent number of scans per meter = %f\n",f_oldval1);
                if (batch)
				{   f_val = f_newval;
					printf("New number of scans per meter = %f\n",f_val);
				} else
				{   printf("ENTER new number of scans per meter (>= 0) --->");
					if (strlen(gets(str)) == 0) break;
					f_val = atof(str);
				}
				if (f_val >= 0.0)
				{   EditDztHeader(num,(void *)&f_val,hdrPtr);
					printf("new rh_spm = %f\n",hdrPtr->rh_spm);
				} else
					puts("Invalid value: number of scans per meter must be >= 0.");
				break;
			case 7:
				f_oldval1 = hdrPtr->rh_mpm;
				printf("\nCurrent number of meters per mark = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("New number of meters per mark = %f\n",f_val);
                } else
				{   printf("ENTER new number of meters per mark (>= 0) --->");
					if (strlen(gets(str)) == 0) break;
					f_val = atof(str);
				}
				if (f_val >= 0.0)
				{   EditDztHeader(num,(void *)&f_val,hdrPtr);
					printf("new rh_mpm = %f\n",hdrPtr->rh_mpm);
				} else
					puts("Invalid value: number of meters per mark must be >= 0.");
				break;
			case 8:
				f_oldval1 = hdrPtr->rh_position;
				printf("\nCurrent cursor position in nanoseconds = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("The new cursor position in nanoseconds = %f\n",f_val);
				} else
				{   printf("ENTER the new cursor position in nanoseconds --->");
					if (strlen(gets(str)) == 0) break;
					f_val = atof(str);
				}
				EditDztHeader(num,(void *)&f_val,hdrPtr);
				printf("new rh_position = %f\n",hdrPtr->rh_position);
				break;
			case 9:
				f_oldval1 = hdrPtr->rh_range;
				printf("\nCurrent time range in nanoseconds = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("The new time range in nanoseconds = %f\n",f_val);
				} else
				{   printf("ENTER new time range in nanoseconds (0.01 to 10000.0) --->");
					f_val = atof(gets(str));
				}
                if (f_val >= 0.01 && f_val <=10000.0)
                {   EditDztHeader(num,(void *)&f_val,hdrPtr);
                    printf("new rh_range = %f\n",hdrPtr->rh_range);
                } else
                    puts("Invalid value: time range must range from 0.01 to 10000.0 ns.");
                break;
			case 10:
				us_oldval1 = hdrPtr->rh_npass;
				printf("\nCurrent number of scans per pass = %hu\n",us_oldval1);
				if (batch)
				{   i_val = i_newval;
					printf("The new number of scans per pass = %i\n",i_val);
				} else
				{   printf("ENTER new number of scans per pass (>= 1) --->");
					i_val = atoi(gets(str));
				}
                if (i_val >= 1)
                {   us_val = i_val;
                    EditDztHeader(num,(void *)&us_val,hdrPtr);
					printf("new rh_npass = %hu\n",hdrPtr->rh_npass);
                } else
                    puts("Invalid value: number of scans per pass must >= 1.");
                break;
            case 11:
                if (hdrPtr->rh_modif.month == 0)
                    puts("\nCurrent create date  = no date   time = no time");
				else
					printf("\nCurrent create date = %s %d, %d   time = %d:%02d:%02d\n",
                        month_abbr[hdrPtr->rh_create.month],
                        hdrPtr->rh_create.day,
                        hdrPtr->rh_create.year+1980,
						hdrPtr->rh_create.hour,
                        hdrPtr->rh_create.min,
                        hdrPtr->rh_create.sec2*2);
                printf("ENTER new date - month (01-12), day (01-31), year (mm:dd:yyyy) --->");
                if (strlen(gets(str)) < 10)
                {   puts("Invalid date entry (valid examples: 01:08:1989 or 10:13:1993).");
                    break;
                }
				i_val = atoi(str+0);
                if (i_val < 1)  i_val = 1;
                if (i_val > 12) i_val = 12;
                timedate.month = i_val;
                i_val = atoi(str+3);
                if (i_val < 1)  i_val = 1;
                if (i_val > 31) i_val = 31;
				timedate.day = i_val;
				i_val = atoi(str+6);
                i_val -= 1980;
                if (i_val < 0)   i_val = 0;
                if (i_val > 127) i_val = 127;
				timedate.year = i_val;
                printf("ENTER new time - hr (00-23), min (00-59), sec (00-59) (hh:mm:ss) --->");
                if (strlen(gets(str)) < 8)
                {   puts("Invalid time entry (valid examples: 01:08:23 or 15:50:03).");
                    break;
                }
                i_val = atoi(str+0);
                if (i_val < 0)  i_val = 0;
				if (i_val > 23) i_val = 23;
                timedate.hour = i_val;
                i_val = atoi(str+3);
                if (i_val < 0)  i_val = 0;
                if (i_val > 59) i_val = 59;
                timedate.min = i_val;
                i_val = atoi(str+6);
				i_val /= 2;
				if (i_val < 0)  i_val = 0;
                if (i_val > 29) i_val = 29;
                timedate.sec2 = i_val;
                EditDztHeader(num,(void *)&timedate,hdrPtr);
				printf("new create date = %s %d, %d   time = %d:%02d:%02d\n",
                    month_abbr[hdrPtr->rh_create.month],
                    hdrPtr->rh_create.day,
                    hdrPtr->rh_create.year+1980,
                    hdrPtr->rh_create.hour,
                    hdrPtr->rh_create.min,
                    hdrPtr->rh_create.sec2*2);
                break;
			case 12:
                if (hdrPtr->rh_modif.month == 0)
                    puts("\nCurrent modification date  = no date   time = no time");
                else
                    printf("\nCurrent modification date  = %s %d, %d   time = %d:%02d:%02d\n",
                        month_abbr[hdrPtr->rh_modif.month],
                        hdrPtr->rh_modif.day,
						hdrPtr->rh_modif.year+1980,
						hdrPtr->rh_modif.hour,
                        hdrPtr->rh_modif.min,
                        hdrPtr->rh_modif.sec2*2);
                printf("ENTER new date - month (01-12), day (01-31), year (mm:dd:yyyy) --->");
				if (strlen(gets(str)) < 10)
                {   puts("Invalid date entry (valid examples: 01:08:1989 or 10:13:1993).");
                    break;
                }
                i_val = atoi(str+0);
                if (i_val < 1)  i_val = 1;
                if (i_val > 12) i_val = 12;
                timedate.month = i_val;
				i_val = atoi(str+3);
                if (i_val < 1)  i_val = 1;
                if (i_val > 31) i_val = 31;
                timedate.day = i_val;
                i_val = atoi(str+6);
                i_val -= 1980;
                if (i_val < 0)   i_val = 0;
				if (i_val > 127) i_val = 127;
				timedate.year = i_val;
                printf("ENTER new time - hr (00-23), min (00-59), sec (00-59) (hh:mm:ss) --->");
                if (strlen(gets(str)) < 8)
                {   puts("Invalid time entry (valid examples: 01:08:23 or 15:50:03).");
					break;
                }
                i_val = atoi(str+0);
                if (i_val < 0)  i_val = 0;
                if (i_val > 23) i_val = 23;
                timedate.hour = i_val;
                i_val = atoi(str+3);
                if (i_val < 0)  i_val = 0;
				if (i_val > 59) i_val = 59;
                timedate.min = i_val;
				i_val = atoi(str+6);
                i_val /= 2;
                if (i_val < 0)  i_val = 0;
                if (i_val > 29) i_val = 29;
                timedate.sec2 = i_val;
				EditDztHeader(num,(void *)&timedate,hdrPtr);
				printf("new modification date  = %s %d, %d   time = %d:%02d:%02d\n",
                    month_abbr[hdrPtr->rh_modif.month],
                    hdrPtr->rh_modif.day,
                    hdrPtr->rh_modif.year+1980,
                    hdrPtr->rh_modif.hour,
                    hdrPtr->rh_modif.min,
                    hdrPtr->rh_modif.sec2*2);
                break;
            case 13:
                puts("rh_rgain (offset to range gain function) is a non-editable field.");
                printf("\tcurrent rh_rgain = %hu\n",hdrPtr->rh_rgain);
                break;
			case 14:
                puts("rh_nrgain (size of range gain function) is a non-editable field.");
				printf("\tcurrent rh_nrgain = %hu\n",hdrPtr->rh_nrgain);
                break;
            case 15:
                puts("rh_text (offset to text) is a non-editable field.");
                printf("\tcurrent rh_text = %hu\n",hdrPtr->rh_text);
				break;
			case 16:
                puts("rh_ntext (size of text) is a non-editable field.");
                printf("\tcurrent rh_ntext = %hu\n",hdrPtr->rh_ntext);
                break;
            case 17:
                puts("rh_proc (offset to processing history) is a non-editable field.");
                printf("\tcurrent rh_proc = %hu\n",hdrPtr->rh_proc);
                break;
            case 18:
                puts("rh_nproc (size of processing history) is a non-editable field.");
                printf("\tcurrent rh_nproc = %hu\n",hdrPtr->rh_nproc);
                break;
			case 19:
                us_oldval1 = hdrPtr->rh_nchan;
				us_oldval2 = hdrPtr->rh_chanmask;
                printf("\nCurrent number of channels = %hu  (rh_chanmask = 0x%04hX).\n",
                        us_oldval1,us_oldval2);
				if (batch)
                {   us_val = us_newval;
					printf("New number of channels = %hu\n",us_val);
				} else
                {   printf("ENTER new number of channels (1 to 4) --->");
                    us_val = atoi(gets(str));
                }
                if (us_val >= 1 && us_val <= 4)
                {   EditDztHeader(num,(void *)&us_val,hdrPtr);
                    printf("new rh_nchan = %hu  (new rh_chanmask = 0x%04hX)\n",
                            hdrPtr->rh_nchan,hdrPtr->rh_chanmask);
                } else
                    puts("Invalid value: number of channels must range from 1 to 4.");
                break;
            case 20:
				f_oldval1 = hdrPtr->rh_epsr;
                printf("\nCurent average dielectric constant = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("The new average dielectric constant = %f\n",f_val);
                } else
                {   printf("ENTER new average dielectric constant (0.0 to 100.0) --->");
					if (strlen(gets(str)) == 0) break;
                    f_val = atof(str);
                }
                if (f_val >= 0.0 && f_val <=100.0)
                {   EditDztHeader(num,(void *)&f_val,hdrPtr);
                    printf("new rh_epsr = %f\n",hdrPtr->rh_epsr);
                } else
                    puts("Invalid value: average dielectric constant must range from 0.0 to 100.0.");
                break;
            case 21:
                f_oldval1 = hdrPtr->rh_top;
                printf("\nCurent top position in meters = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("the new top position in meters = %f\n",f_val);
                } else
                {   printf("ENTER new top position in meters --->");
                    if (strlen(gets(str)) == 0) break;
                    f_val = atof(str);
                }
				EditDztHeader(num,(void *)&f_val,hdrPtr);
                printf("new rh_top = %f\n",hdrPtr->rh_top);
                break;
            case 22:
                f_oldval1 = hdrPtr->rh_depth;
                printf("\nCurent depth range in meters = %f\n",f_oldval1);
				if (batch)
				{   f_val = f_newval;
					printf("The new depth range in meters = %f\n",f_val);
                } else
                {   printf("ENTER new depth range in meters --->");
                    if (strlen(gets(str)) == 0) break;
                    f_val = atof(str);
				}
                EditDztHeader(num,(void *)&f_val,hdrPtr);
                printf("new rh_depth = %f\n",hdrPtr->rh_depth);
                break;
            case 23:
                puts("rh_reserved[32] is a non-editable field.");
                break;
			case 24:
                strcpy(c_oldval1,hdrPtr->rh_antname);
                ant_freq[0] = 0;
				for (i=0; ant_number[i]; i++)
				{   if (strstr(c_oldval1,ant_number[i]))
                    {   strcpy(ant_freq,ant_name[i]);
                        break;
                    }
                }
                if (ant_freq[0] == 0) strcpy(ant_freq,"unknown antenna type");
                printf("\nCurrent antenna number = %s (%s)\n",c_oldval1,ant_freq);
				printf("ENTER new antenna number (name or 4-digit integer) --->");
				if (strlen(gets(str)) == 0) break;
				EditDztHeader(num,(void *)str,hdrPtr);
				ant_freq[0] = 0;
				for (i=0; ant_number[i]; i++)
				{   if (strstr(hdrPtr->rh_antname,ant_number[i]))
					{   strcpy(ant_freq,ant_name[i]);
						break;
					}
				}
				if (ant_freq[0] == 0) strcpy(ant_freq,"unknown antenna type");
				printf("new rh_antname = %s (%s)",hdrPtr->rh_antname,ant_freq);
				break;
			case 25:
				puts("rh_chanmask (active channels mask) is a non-editable field.");
				printf("\tcurrent rh_chanmask = 0x%04hX\n",hdrPtr->rh_chanmask);
                break;
            case 26:
                strcpy(c_oldval1,hdrPtr->rh_name);
                printf("\nCurrent file name = %s\n",c_oldval1);
                printf("ENTER new file name (first 8 characters, no extension) --->");
                if (strlen(gets(str)))
				{   EditDztHeader(num,(void *)str,hdrPtr);
                    printf("new rh_name = %s\n",hdrPtr->rh_name);
                }
                break;
            case 27:
                puts("rh_chksum (checksum for header) is a non-editable field.");
                break;
			case 28:  case 29:
                rg_breaks = *(unsigned short *)((char *)hdrPtr + hdrPtr->rh_rgain);
                printf("\nCurrent number of range gain break points = %hu\n",rg_breaks);
                printf("Current size of range gain function = %hu\n",hdrPtr->rh_nrgain);
                printf("Current range gains in db: scan sample     db\n");
                rg_break_delta = (hdrPtr->rh_nsamp-1) / (rg_breaks-1);
                f_ptr = (float *)((char *)hdrPtr + hdrPtr->rh_rgain + 2);
                for (i=0; (i<rg_breaks && i<MAX_RG_BREAKS); i++, f_ptr++)
                {   printf("                              %3d       %7.3f\n",
                            rg_break_delta*i,*f_ptr);
                }
                printf("ENTER new number of range gain break points (2 to 8) --->");
                i_val = atoi(gets(str));
				if (i_val >= 2 && i_val <= 8)
                {   us_val = i_val;
                    EditDztHeader(28,(void *)&us_val,hdrPtr);
                    for (i=0; i<MAX_RG_BREAKS; i++) rg_values[i] = 0.0;
                    for (i=0; i<us_val; i++)
                    {   printf("ENTER a floating point number for the RG in db --->");
                        gets(str);
						if (str[0] == 0) break;
                        rg_values[i] = atof(str);
                    }
                    EditDztHeader(29,(void *)rg_values,hdrPtr);
                    rg_breaks = *(unsigned short *)((char *)hdrPtr + hdrPtr->rh_rgain);
                    printf("New size of range gain function = %hu\n",hdrPtr->rh_nrgain);
                    printf("New range gains in db: scan sample     db\n");
                    rg_break_delta = (hdrPtr->rh_nsamp-1) / (rg_breaks-1);
                    f_ptr = (float *)((char *)hdrPtr + hdrPtr->rh_rgain + 2);
					for (i=0; (i<rg_breaks && i<MAX_RG_BREAKS); i++, f_ptr++)
					{   printf("                          %3d       %7.3f\n",
                                rg_break_delta*i,*f_ptr);
                    }
				} else
                    puts("Invalid value: number of range gain break puts must range from 2 to 8.");
                break;
            case 30:
                printf("Current text field is:\n%s\n",(char *)hdrPtr + hdrPtr->rh_text);
                printf("ENTER number of lines for new text (1 to 5) --->");
                i_val = atoi(gets(str));
				if (i_val >= 1 && i_val <= 5)
                {   char *c_ptr;
                    int length;

                    str[0] = 0;                   /* initialize str[] */
                    c_ptr = str;                  /* point to beginning of str[] */
                    for (i=0; i<i_val; i++)       /* loop for each line to be entered */
                    {   printf("ENTER text for line %d (up to 75 characters)\n",i+1);
                        gets(c_ptr);              /* get string from user */
						length = strlen(c_ptr);   /* get length of string */
						if (length > 75)          /* check if too long */
                            length = 75;          /* truncate */
                        if (i < i_val-1)          /* not last line */
						{   c_ptr[length]   = 0x0D;   /* add CR */
                            c_ptr[length+1] = 0x0A;   /* add LF */
                            c_ptr[length+2] = 0x00;   /* terminate string */
                            c_ptr += strlen(c_ptr);   /* move starting point along str[] */
                        } else                    /* last line */
                            c_ptr[length]   = 0x00;   /* terminate string */
                    }
					EditDztHeader(num,(void *)str,hdrPtr);
					printf("new text field is:\n%s\n",(char *)hdrPtr + hdrPtr->rh_text);
				} else
					puts("Invalid value: number of text lines must range from 1 to 5.");
				break;
			case 31:
				puts("The processing history is a non-editable field at this time.");
				printf("Current processing history is:\n%s\n",
						ConvertProcHist(bufsiz,buff,hdrPtr));
				break;
			default:
				break;
		}    /* end of "switch (num)" */
		if (batch) loop = FALSE;
	}     /* end of "while (loop == TRUE)" */

	/* Print the new header information */
#if 0
	if (!batch)
	{   puts("");
		itemp = PrintOneDztHeader((int)sizeof(struct DztHdrStruct),
					  (long)num_scans,"stdout",dzt_filename,hdrPtr);
		if (itemp > 0)
		{   printf("\n%s\n",PrintOneDztHeaderMsg[itemp]);
			exit(5);
		}
	}
#endif
	/* Save changes to disk after user OK's */
	if (!batch)
	{   while (kbhit()) if (!getch()) getch();   /* clear out buffer */
		printf("\nPress \"S\" to save file header changes to disk, any other key quit ...");
		i_val = getch();
		puts("");
	} else
		i_val = 'S';
	if (i_val == 'S' || i_val == 's')
	{   if (i_val == 0) getch();   /* clear out buffer */
		rewind(IO_file);
		if (fseek(IO_file,(long)((selection-1)*sizeof(struct DztHdrStruct)),SEEK_SET))
		{   puts("ERROR returned from fseek().  Changes not saved to disk.");
		} else
		{   if (fwrite((void *)hdrPtr,sizeof(struct DztHdrStruct),(size_t)1,IO_file) < 1)
				puts("ERROR: short count returned by fwrite().  Changes not saved to disk.");
			else
			{   printf("%c%c37;44m",0x1B,0x5B);
				printf("   Changes to %s header saved to disk.   ",dzt_filename);
				printf("%c%c37;40m",0x1B,0x5B);
			}
		}
	} else
	{   printf("%c%c37;44m",0x1B,0x5B);
		puts("   Changes NOT saved to disk.   ");
		printf("%c%c37;40m",0x1B,0x5B);
	}
	/* Close file and return success */
	fclose(IO_file);
	exit(0);
}
/******************************** GetDztHdrs() ******************************/
/* Read the (1 to 4) DZT headers.  The file must have been previously opened
 * using fopen.
 *
 * Parameters:
 *  int *num_hdrs  - address of variable, will hold number of headers on return
 *  int *num_scans - address of variable, will hold number of GPR scans on return
 *  struct DztHdrStruct *hdrPtr1 - address of header structure
 *  struct DztHdrStruct *hdrPtr2 - address of header structure
 *  struct DztHdrStruct *hdrPtr3 - address of header structure
 *  struct DztHdrStruct *hdrPtr4 - address of header structure
 *  FILE *infile   - pointer to FILE structure for the input file
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, attached header.
 * Calls: fread, rewind, fseek, ftell.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: June 28, 1993
 */
int GetDztHdrs (int *num_hdrs,int *num_scans,
                struct DztHdrStruct *hdrPtr1,struct DztHdrStruct *hdrPtr2,
                struct DztHdrStruct *hdrPtr3,struct DztHdrStruct *hdrPtr4,
                FILE *infile)
{
    int i;
    unsigned short ustemp,checksum;
    long start_byte,last_byte,num_bytes;

    /* Verify parameter list */
	if (hdrPtr1 == NULL || hdrPtr2 == NULL || hdrPtr3 == NULL ||
		hdrPtr4 == NULL || infile == NULL || num_hdrs == NULL ||
        num_scans == NULL) return 1;

    /* Get first header and number of headers */
    rewind(infile);
    if (fread((void *)hdrPtr1,sizeof(struct DztHdrStruct),1,infile) < 1)
        return 2;
    *num_hdrs = hdrPtr1->rh_nchan;               /* number of headers */
    if (hdrPtr1->rh_tag != 0x00FF) return 4;     /* is tag correct? */
    ustemp = hdrPtr1->rh_chksum;                 /* calculate check sum */
    hdrPtr1->rh_chksum = 0;
    checksum = 0;
    for (i=0; i<512; i++)
        checksum += *((unsigned short *)hdrPtr1 + i);
    hdrPtr1->rh_chksum = ustemp;
    if (checksum != ustemp) return 5;            /* is check sum correct? */

    /* Get rest of headers if more than one */
    if (*num_hdrs > 1)
	{   if (fread((void *)hdrPtr2,sizeof(struct DztHdrStruct),1,infile) < 1)
			return 3;
        if (hdrPtr2->rh_tag != 0x01FF) return 4;
        ustemp = hdrPtr2->rh_chksum;                 /* calculate check sum */
        hdrPtr2->rh_chksum = 0;
        checksum = 0;
        for (i=0; i<512; i++)
            checksum += *((unsigned short *)hdrPtr2 + i);
        hdrPtr2->rh_chksum = ustemp;
        if (checksum != ustemp) return 5;            /* is check sum correct? */
    }

    if (*num_hdrs > 2)
    {   if (fread((void *)hdrPtr3,sizeof(struct DztHdrStruct),1,infile) < 1)
            return 3;
        if (hdrPtr3->rh_tag != 0x02FF) return 4;
        ustemp = hdrPtr3->rh_chksum;                 /* calculate check sum */
        hdrPtr3->rh_chksum = 0;
        checksum = 0;
        for (i=0; i<512; i++)
			checksum += *((unsigned short *)hdrPtr3 + i);
		hdrPtr3->rh_chksum = ustemp;
        if (checksum != ustemp) return 5;            /* is check sum correct? */
    }
    if (*num_hdrs > 3)
    {   if (fread((void *)hdrPtr4,sizeof(struct DztHdrStruct),1,infile) < 1)
            return 3;
        if (hdrPtr4->rh_tag != 0x03FF) return 4;
        ustemp = hdrPtr4->rh_chksum;                 /* calculate check sum */
        hdrPtr4->rh_chksum = 0;
        checksum = 0;
        for (i=0; i<512; i++)
            checksum += *((unsigned short *)hdrPtr4 + i);
        hdrPtr4->rh_chksum = ustemp;
        if (checksum != ustemp) return 5;            /* is check sum correct? */
    }

    /* Determine number of GPR scans in file */
    start_byte = ftell(infile);          /* get current location (byte) */
    fseek(infile, 0L, SEEK_END);         /* go to end of file */
	last_byte = ftell(infile);           /* get last location (byte) */
	fseek(infile, start_byte, SEEK_SET); /* return file pointer to first scan */
    num_bytes = last_byte-start_byte;
    *num_scans = num_bytes / (hdrPtr1->rh_nsamp*(hdrPtr1->rh_bits/8));

    return 0;
}
/****************************** EditDztHeader() *****************************/
/* Edit one field of a DZT header structure.
 *
 * Parameters:
 *   int                  field_id  - see below; number representing which field
 *                                    to change; fields in square brackets cannot
 *                                    be changed -- they can be specified but a
 *                                    hardwired solution is used.
 *   void                *field_val - address of variable to be type cast according
 *                                    to field_id
 *   struct DztHdrStruct *hdrPtr    - address of structure
 *
 * field_id    field_type       field_name        field_description
 *
 *    0      unsigned short       rh_tag;         select 0 to 3, the header number
 *     [1]     unsigned short       rh_data;        offset to data in file or could be
 *                                                    the size of the header structure
 *                                                  (always 1024)
 *      2      unsigned short       rh_nsamp;       samples per scan
 *      3      unsigned short       rh_bits;        bits per data word
 *     [4]     unsigned short       rh_zero;        binary offset (mid-point of data)
 *                                                  (determined from rh_bits)
 *      5      float                rh_sps;         scans per second
 *      6      float                rh_spm;         scans per meter
 *      7      float                rh_mpm;         meters per mark
 *      8      float                rh_position;    position (ns)
 *      9      float                rh_range;       range (ns)
 *     10      unsigned short       rh_npass;       scans per pass for 2D files
 *     11      struct DztDateStruct rh_create;      date created
 *     12      struct DztDateStruct rh_modif;       date modified
 *    [13]     unsigned short       rh_rgain;       offset to range gain function
 *                                                (always 128)
 *  [14]     unsigned short       rh_nrgain;      size of range gain function
 *                                                (determined from field_id 28)
 *  [15]     unsigned short       rh_text;        offset to text
 *                                                (determined from rh_rgain and rh_nrgain)
 *  [16]     unsigned short       rh_ntext;       size of text
 *                                                (determined from field_id 30)
 *  [17]     unsigned short       rh_proc;        offset to processing history
 *                                                (determined from rh_text and rh_ntext)
 *  [18]     unsigned short       rh_nproc;       size of processing history
 *                                                (determined from field_id 31)
 *   19      unsigned short       rh_nchan;       number of channels
 *   20      float                rh_epsr;        average dielectric constant
 *   21      float                rh_top;         top position in meters
 *     22      float                rh_depth;       range in meters
 *  [23]     char                 reserved[32];   (always zeroes; for now)
 *   24      char                 rh_antname[14]; antenna name (actually its number)
 *  [25]     unsigned short       rh_chanmask;    active channels mask
 *                                                (determined from rh_nchan; 0x5301, 0x5303, 0x5307, 0x530F)
 *   26      char                 rh_name[12];    this file name
 *   --      unsigned short       rh_chksum;      checksum for header (always recalculated)
 *      128 bytes to here
 *  [27]     char                 variable[896];  range gain, comments, processing history
 *      below are not part of structure except as stored in variable[]
 *   28      unsigned short       num_rg_breaks   number of range gain break points
 *   29      float *              rg_values       the range gain values in db
 *                                                (be sure to set num_rg_breaks first !!!)
 *   30      char *               text            comments
 *   31      char *               proc            this is the coded processing history
 *
 *
 * Requires: <string.h>.
 * Calls: strlen, strncpy.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: October 20, 1995
 */
void EditDztHeader (int field_id,void *field_val,struct DztHdrStruct *hdrPtr)
{
    char                 *c_ptr;
    char                 *cp_hdr = (char *)hdrPtr;
	unsigned short       *us_ptr, us_val, checksum;
    int                   i, old1, new1;
    float                *f_ptr, *f_ptr2, f_val;
    size_t                new_len;

    switch (field_id)
    {   case 0:
            us_val = *(unsigned short *)field_val;
            if (us_val <= 3)
               hdrPtr->rh_tag = (us_val << 8) | 0x00FF;
            break;
        case 1:
            hdrPtr->rh_data = 1024;
            break;
        case 2:
            us_val = *(unsigned short *)field_val;
            if (us_val == 256 || us_val == 512 || us_val == 1024)  hdrPtr->rh_nsamp = us_val;
            break;
        case 3:
            us_val = *(unsigned short *)field_val;
			if (us_val == 8 || us_val == 16)
            {   hdrPtr->rh_bits = us_val;
                hdrPtr->rh_zero = 1 << (us_val-1);
            }
            break;
        case 4:
            if (hdrPtr->rh_bits == 8 || hdrPtr->rh_nsamp == 16)
                hdrPtr->rh_zero = 1 << (hdrPtr->rh_nsamp-1);
            break;
        case 5:
            f_val = *(float *)field_val;
            if (f_val >= 0.10 && f_val <= 128.0)  hdrPtr->rh_sps = f_val;
            break;
        case 6:
            hdrPtr->rh_spm = *(float *)field_val;
            break;
        case 7:
            hdrPtr->rh_mpm = *(float *)field_val;
            break;
        case 8:
			hdrPtr->rh_position = *(float *)field_val;
            break;
        case 9:
            hdrPtr->rh_range = *(float *)field_val;
            break;
        case 10:
            us_val = *(unsigned short *)field_val;
            if (us_val >= 1)  hdrPtr->rh_npass = us_val;
            break;
        case 11:
            hdrPtr->rh_create = *(struct DztDateStruct *)field_val;
            break;
        case 12:
            hdrPtr->rh_modif = *(struct DztDateStruct *)field_val;
            break;
        case 13:
            hdrPtr->rh_rgain = 128;
            break;
        case 14:
            break;
		case 15:
            if (hdrPtr->rh_rgain > 0 && hdrPtr->rh_nrgain > 0)
               hdrPtr->rh_text = hdrPtr->rh_rgain + hdrPtr->rh_nrgain;
            break;
        case 16:
            break;
        case 17:
            if (hdrPtr->rh_text > 0 && hdrPtr->rh_ntext > 0)
               hdrPtr->rh_text = hdrPtr->rh_text + hdrPtr->rh_ntext;
            break;
        case 18:
            break;
        case 19:
            us_ptr = (unsigned short *)field_val;
            if (*us_ptr >= 1  && *us_ptr <= 4)
            {   hdrPtr->rh_nchan = *us_ptr;
                switch (hdrPtr->rh_nchan)
                {   case 1:  hdrPtr->rh_chanmask = 0x5301;  break;
                    case 2:  hdrPtr->rh_chanmask = 0x5303;  break;
                    case 3:  hdrPtr->rh_chanmask = 0x5307;  break;
					case 4:  hdrPtr->rh_chanmask = 0x530F;  break;
                }
            }
            break;
        case 20:
            hdrPtr->rh_epsr = *(float *)field_val;
            break;
        case 21:
            hdrPtr->rh_top = *(float *)field_val;
            break;
        case 22:
            hdrPtr->rh_depth = *(float *)field_val;
            break;
        case 23:
            break;
        case 24:
			strncpy(hdrPtr->rh_antname,(char *)field_val,13);
			if (hdrPtr->rh_antname[8] == '(') hdrPtr->rh_antname[13] = ')';
			else                              hdrPtr->rh_antname[13] = 0;
			break;
		case 25:
			switch (hdrPtr->rh_nchan)
			{   case 1:  hdrPtr->rh_chanmask = 0x5301;  break;
                case 2:  hdrPtr->rh_chanmask = 0x5303;  break;
                case 3:  hdrPtr->rh_chanmask = 0x5307;  break;
                case 4:  hdrPtr->rh_chanmask = 0x530F;  break;
            }
            break;
        case 26:
            strncpy(hdrPtr->rh_name,(char *)field_val,11);
            break;
        case 27:
            break;
        case 28:
            us_ptr = (unsigned short *)field_val;
            hdrPtr->rh_rgain = 128;
            /* get old value */
            us_val = *(unsigned short *)(cp_hdr + hdrPtr->rh_rgain);
			/* is new value in valid range? */
            if (*us_ptr >= 2 && *us_ptr <= 8)
            {   /* move remainder of variable[] if neccesary */
                if (*us_ptr > us_val)
                {   new1 = sizeof(struct DztHdrStruct) - 1;
                    old1 = new1 - ((*us_ptr - us_val) * sizeof(float));
                    while (old1 >= hdrPtr->rh_rgain+2+us_val*sizeof(float))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1--;  old1--;
                    }
                } else if (*us_ptr < us_val)
                {   old1 = hdrPtr->rh_rgain + 2 + (us_val * sizeof(float));
                    new1 = old1 - ((us_val - *us_ptr) * sizeof(float));
                    while (old1 < sizeof(struct DztHdrStruct))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1++;  old1++;
                    }
                }
                *(unsigned short *)(cp_hdr + hdrPtr->rh_rgain) = *us_ptr;
                hdrPtr->rh_nrgain = 2 + *us_ptr * sizeof(float);
				/* adjust offsets to remaining fields */
                hdrPtr->rh_text = hdrPtr->rh_rgain + hdrPtr->rh_nrgain;
                hdrPtr->rh_proc = hdrPtr->rh_text + hdrPtr->rh_ntext;
            }
            break;
        case 29:
            /* get number of breaks from header */
            us_val = *(unsigned short *)(cp_hdr + hdrPtr->rh_rgain);
            /* type cast array pointer */
            f_ptr = (float *)field_val;
            /* point to start of float "array" in variable[] */
            f_ptr2 = (float *)(cp_hdr + hdrPtr->rh_rgain + 2);
            /* copy values */
            for (i=0; i<us_val; i++)  f_ptr2[i] = f_ptr[i];
            break;
        case 30:
            c_ptr = (char *)field_val;
            new_len = strlen(c_ptr) + 1;
            /* is new value in valid range? */
            if (new_len > 1)
			{   /* move remainder of variable[] if neccesary */
                if (new_len > hdrPtr->rh_ntext)
                {   new1 = sizeof(struct DztHdrStruct) - 1;
                    old1 = new1 - (new_len - hdrPtr->rh_ntext);
                    while (old1 >= hdrPtr->rh_text + hdrPtr->rh_ntext)
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1--;  old1--;
                    }
                } else if (new_len < hdrPtr->rh_ntext)
                {   old1 = hdrPtr->rh_text + hdrPtr->rh_ntext;
                    new1 = old1 - (hdrPtr->rh_ntext - new_len);
                    while (old1 < sizeof(struct DztHdrStruct))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1++;  old1++;
                    }
                }
                hdrPtr->rh_ntext = new_len;
                /* adjust offsets to remaining fields */
                hdrPtr->rh_proc = hdrPtr->rh_text + hdrPtr->rh_ntext;
                /* copy replacement text string */
				strncpy(cp_hdr + hdrPtr->rh_text,c_ptr,new_len - 1);
                *(cp_hdr + hdrPtr->rh_text + hdrPtr->rh_ntext - 1) = 0;  /* be sure NULL terminated */
            }
            break;
        case 31:
            c_ptr = (char *)field_val;
            new_len = strlen(c_ptr) + 1;
            /* is new value in valid range? */
            if (new_len > 1)
            {   /* move remainder of variable[] if neccesary */
                if (new_len > hdrPtr->rh_nproc)
                {   new1 = sizeof(struct DztHdrStruct) - 1;
                    old1 = new1 - (new_len - hdrPtr->rh_nproc);
                    while (old1 >= hdrPtr->rh_proc + hdrPtr->rh_nproc)
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1--;  old1--;
                    }
                } else if (new_len < hdrPtr->rh_nproc)
                {   old1 = hdrPtr->rh_proc + hdrPtr->rh_nproc;
                    new1 = old1 - (hdrPtr->rh_nproc - new_len);
					while (old1 < sizeof(struct DztHdrStruct))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1++;  old1++;
                    }
                }
                hdrPtr->rh_nproc = new_len;
                /* copy replacement string */
                strncpy(cp_hdr + hdrPtr->rh_proc,c_ptr,new_len - 1);
                *(cp_hdr + hdrPtr->rh_proc +  - 1) = 0;  /* be sure NULL terminated */
            }
            break;
    }
    /* now update checksum ! */
    hdrPtr->rh_chksum = 0;
    checksum = 0;
    for (i=0; i<512; i++)
        checksum += *((unsigned short *)hdrPtr + i);
    hdrPtr->rh_chksum = checksum;
}
/*************************** PrintOneDztHeader0() ***************************/
/* Print out the contents of one DZT file header to an output device
 * (stdout, stderr, a disk file, etc.)..
 *
 * Parameters:
 *   int num_scans      - number of radar scans, <=0 if unknown
 *   char *out_filename - "stdout", "stderr", or a DOS filename
 *   char *dzt_filename - DOS name of the DZT file header(s) belong to
 *   struct DztHdrStruct *hdrPtr1 - addresses of the header
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, attached header.
 * Calls: fopen, fprintf, getch, strstr, ConvertProcHist.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: October 19, 1995
 */
int PrintOneDztHeader0 (int num_scans,char *out_filename,
					   char *dzt_filename,struct DztHdrStruct *hdrPtr)
{
	char ant_freq[10];
    unsigned short rg_breaks = 0;
    unsigned short checksum;
    unsigned short ustemp;
    int i,rg_break_delta;
    FILE *stream = NULL;

    static char big_buff[1024];           /* removes from stack and places in data segment */
	extern const char *ant_number[];      /* in "dzt_pgms.h" */
    extern const char *ant_name[];        /* in "dzt_pgms.h" */
    extern const char *month_abbr[];      /* in "dzt_pgms.h" */

    /* Assign or open stream */
    if      (strstr(out_filename,"stdout"))  stream = stdout;
    else if (strstr(out_filename,"stderr"))  stream = stderr;
    else                                     stream = fopen(out_filename,"wt");
    if (stream == NULL) return 1;

    /* Print the header information */
    fprintf(stream,"SIR-10A header for file %s:\n",dzt_filename);
	fprintf(stream,"number of headers    = %d\n",hdrPtr->rh_nchan);
    if (num_scans > 0)  fprintf(stream,"number of GPR scans  = %d\n",num_scans);
    fprintf(stream,"SIR-10A header ID    = 0x%04hX\n",hdrPtr->rh_tag);
    fprintf(stream,"offset to data       = %hu\n",hdrPtr->rh_data);
    fprintf(stream,"samples per scan     = %hu\n",hdrPtr->rh_nsamp);
    fprintf(stream,"bits per data word   = %hu\n",hdrPtr->rh_bits);
    fprintf(stream,"binary offset        = %hu (0x%04hX)\n",hdrPtr->rh_zero,hdrPtr->rh_zero);
    fprintf(stream,"scans per second     = %f\n",hdrPtr->rh_sps);
    fprintf(stream,"scans per meter      = %f\n",hdrPtr->rh_spm);
    fprintf(stream,"meters per mark      = %f\n",hdrPtr->rh_mpm);
    fprintf(stream,"position (ns)        = %f\n",hdrPtr->rh_position);
    fprintf(stream,"range (ns)           = %f\n",hdrPtr->rh_range);
    fprintf(stream,"scans per pass       = %hu\n",hdrPtr->rh_npass);
    fprintf(stream,"create date          = %s %d, %d (%d:%02d:%02d)\n",
        month_abbr[hdrPtr->rh_create.month],hdrPtr->rh_create.day,hdrPtr->rh_create.year+1980,
        hdrPtr->rh_create.hour,hdrPtr->rh_create.min,hdrPtr->rh_create.sec2*2);
    if (hdrPtr->rh_modif.month == 0)
        fprintf(stream,"modification date    = no date\n");
    else
        fprintf(stream,"modification date    = %s %d, %d (%d:%02d:%02d)\n",
			month_abbr[hdrPtr->rh_modif.month],hdrPtr->rh_modif.day,hdrPtr->rh_modif.year+1980,
            hdrPtr->rh_modif.hour,hdrPtr->rh_modif.min,hdrPtr->rh_modif.sec2*2);
    fprintf(stream,"offset to range gain = %hu\n",hdrPtr->rh_rgain);
    fprintf(stream,"size of range gain   = %hu\n",hdrPtr->rh_nrgain);
    fprintf(stream,"offset to text       = %hu\n",hdrPtr->rh_text);
    fprintf(stream,"size of text         = %hu\n",hdrPtr->rh_ntext);
    fprintf(stream,"offset to proc.hist. = %hu\n",hdrPtr->rh_proc);
    fprintf(stream,"size of proc.hist.   = %hu\n",hdrPtr->rh_nproc);
    if (stream==stdout || stream==stderr)
    {   fprintf(stream,"Press a key to continue ...");
        getch();
        fprintf(stream,"\r                                \r");
    }
    fprintf(stream,"number of channels   = %hu\n",hdrPtr->rh_nchan);
    fprintf(stream,"ave. diel. constant  = %f\n",hdrPtr->rh_epsr);
    fprintf(stream,"top pos. in meters   = %f\n",hdrPtr->rh_top);
    fprintf(stream,"range in meters      = %f\n",hdrPtr->rh_depth);
    fprintf(stream,"reserved             = %s\n",hdrPtr->reserved);
    ant_freq[0] = 0;
	for (i=0; ant_number[i]; i++)
	{   if (strstr(hdrPtr->rh_antname,ant_number[i]))
        {   strcpy(ant_freq,ant_name[i]);
            break;
        }
    }
    fprintf(stream,"antenna name         = %s (%s)\n",hdrPtr->rh_antname,ant_freq);
    fprintf(stream,"active channels mask = 0x%04hX\n",hdrPtr->rh_chanmask);
    fprintf(stream,"this file name       = %s\n",hdrPtr->rh_name);
    fprintf(stream,"checksum for header  = %hu (0x%04hX)\n",hdrPtr->rh_chksum,hdrPtr->rh_chksum);
    ustemp = hdrPtr->rh_chksum;
    hdrPtr->rh_chksum = 0;
    checksum = 0;
    for (i=0; i<512; i++)
        checksum += *((unsigned short *)hdrPtr + i);
    fprintf(stream,"calculated checksum  = %hu (0x%04hX) [header %s corrupted]\n",
        checksum,checksum,(checksum==ustemp) ? "is not" : "is");
    hdrPtr->rh_chksum = ustemp;
	if (stream==stdout || stream==stderr)
	{   fprintf(stream,"Press a key to continue ...");
		getch();
		fprintf(stream,"\r                                \r");
	}
	rg_breaks = *(unsigned short *) ((char *)hdrPtr + hdrPtr->rh_rgain);
    fprintf(stream,"number of rg breaks  = %hd\n",rg_breaks);
    fprintf(stream,"range gain: scan sample     db\n");
    rg_break_delta = (hdrPtr->rh_nsamp-1) / (rg_breaks-1);
    for (i=0; i<rg_breaks; i++)
    {   fprintf(stream,"               %3d       %7.3f\n",
            rg_break_delta*i,*(float *)((char *)hdrPtr + hdrPtr->rh_rgain + 2 + 4*i));
    }
    if (hdrPtr->rh_text)
        fprintf(stream,"comments =\n%s\n",(char *)hdrPtr + hdrPtr->rh_text);
    ConvertProcHist((int)sizeof(big_buff),big_buff,hdrPtr);
    fprintf(stream,"Processing History:\n");
    if (strlen(big_buff)) fprintf(stream,"%s\n",big_buff);
    else                  fprintf(stream,"\tnone\n\n");

    if      (strstr(out_filename,"stdout"))     ;
    else if (strstr(out_filename,"stderr"))     ;
    else                                        fclose(stream);
	return 0;
}
/**************************** ConvertProcHist() *****************************/
/* This function converts the coded processing history into a long ASCII
 * string.  This code is modified from that written by GSSI, contact Leo
 * Galinovsky.
 *
 * Parameters:
 *   int bufsiz - size of buffer pointed to by "buff"
 *   char *buff - pointer to string (1024-byte length recommended)
 *   struct DztHdrStruct *hdrPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, attached header.
 * Calls: strcpy, sprintf, strlen.
 * Returns: char *buff.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: October 19, 1995
 */
char *ConvertProcHist (int bufsiz,char *buff,struct DztHdrStruct *hdrPtr)
{
    char *b,*p,*proc;
    int nproc;
    extern const char *ph_viirl,*ph_viirh,*ph_vfirl,*ph_vfirh,*ph_vboxl;
	extern const char *ph_vboxh,*ph_vtril,*ph_vtrih,*ph_vtcl,*ph_vtch;
    extern const char *ph_hiirl,*ph_hiirh,*ph_hfirl,*ph_hfirh,*ph_hboxl;
    extern const char *ph_htril,*ph_htcl,*ph_hboxh,*ph_htrih,*ph_htch,*ph_hsts;

    b = buff;
    b[0] = 0;
	p = proc = (char *)hdrPtr + hdrPtr->rh_proc;
    nproc = hdrPtr->rh_nproc;
    while (p-proc < nproc)
    {   switch (*p)
        {   /* first two case to make header compatible with old tapes */
            case PR_LPF:
                p += 2 + sizeof(float);
				break;
            case PR_HPF:
                p += 2 + sizeof(float);
				break;
            case PR_VIIRL:
                sprintf(b,ph_viirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
				break;
            case PR_VIIRH:
                sprintf(b,ph_viirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VFIRL:
				sprintf(b,ph_vfirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VFIRH:
                sprintf(b,ph_vfirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
			case PR_VBOXL:
                sprintf(b,ph_vboxl,(int)*(p+1));
                p += 2;
				break;
            case PR_VBOXH:
                sprintf(b,ph_vboxh,(int)*(p+1));
                p += 2;
				break;
            case PR_VTRIL:
                sprintf(b,ph_vtril,(int)*(p+1));
                p += 2;
                break;
            case PR_VTRIH:
				sprintf(b,ph_vtrih,(int)*(p+1));
                p += 2;
                break;
            case PR_VTCL:
                sprintf(b,ph_vtcl,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
			case PR_VTCH:
                sprintf(b,ph_vtch,*(float *)(p+2));
                p += 2 + sizeof(float);
				break;
            case PR_HIIRL:
                sprintf(b,ph_hiirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
				break;
            case PR_HIIRH:
                sprintf(b,ph_hiirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HFIRL:
				sprintf(b,ph_hfirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HFIRH:
                sprintf(b,ph_hfirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
			case PR_HBOXL:
                sprintf(b,ph_hboxl,(int)*(p+1));
                p += 2;
				break;
            case PR_HTRIL:
                sprintf(b,ph_htril,(int)*(p+1));
                p += 2;
				break;
            case PR_HTCL:
                sprintf(b,ph_htcl,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HBOXH:
				sprintf(b,ph_hboxh,(int)*(p+1));
                p += 2;
                break;
            case PR_HTRIH:
                sprintf(b,ph_htrih,(int)*(p+1));
                p += 2;
                break;
			case PR_HTCH:
                sprintf(b,ph_htch,*(float *)(p+2));
                p += 2 + sizeof(float);
				break;
			case PR_STS:
                sprintf(b,ph_hsts,(int)*(p+1));
                p += 2;
				break;
#if 0
			case PR_STS_NEW:
				sprintf(b,ph_hsts,(int)*(p+1));
				p += 2;
				break;
#endif
			default:
				return buff;
        }
        if (strlen(buff) + strlen(b) > bufsiz) break;
        b += strlen(b);
    }
    return buff;
}
/**************************** PrintOneDztHeader() ***************************/
/* Print out the contents of one DZT file header to an output device
 * (stdout, stderr, a disk file, etc.)..
 *
 * Parameters:
 *   int header_bytes    - number of actual bytes in file header, 512 or 1024
 *   int num_traces     - number of radar scans, <=0 if unknown
 *   char *out_filename - "stdout", "stderr", or a DOS filename
 *   char *dzt_filename - DOS name of the DZT file header(s) belong to
 *   struct DztHdrStruct *hdrPtr1 - addresses of the header
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, attached header.
 * Calls: fopen, fprintf, getch, strstr, ConvertProcHist2.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: October 19, 1995
 */
int PrintOneDztHeader (int header_bytes,long num_traces,char *out_filename,
					   char *dzt_filename,struct DztHdrStruct *hdrPtr)
{
    char ant_freq[10],newstr[32];
    unsigned short rg_breaks = 0;
    unsigned short checksum;
    unsigned short ustemp;
    int i,rg_break_delta;
	FILE *stream = NULL;

    static char big_buff[1024];           /* removes from stack and places in data segment */
	extern const char *ant_number[];      /* in "dzt_pgms.h" */
	extern const char *ant_name[];        /* in "dzt_pgms.h" */
    extern const char *month_abbr[];      /* in "dzt_pgms.h" */

    /* Assign or open stream */
    if      (strstr(out_filename,"stdout"))  stream = stdout;
    else if (strstr(out_filename,"stderr"))  stream = stderr;
    else                                     stream = fopen(out_filename,"wt");
    if (stream == NULL) return 1;

	/* Print the header information */
    fprintf(stream,"SIR-10A header for file %s:\n",dzt_filename);
    fprintf(stream,"number of headers    = %d\n",hdrPtr->rh_nchan);
    fprintf(stream,"file header size     = %d\n",header_bytes);
    if (num_traces > 0)  fprintf(stream,"number of GPR traces = %d\n",num_traces);
    fprintf(stream,"SIR-10A header ID    = 0x%04x\n",hdrPtr->rh_tag);
    fprintf(stream,"offset to data       = %hu\n",hdrPtr->rh_data);
    fprintf(stream,"samples per scan     = %hu\n",hdrPtr->rh_nsamp);
	fprintf(stream,"bits per data word   = %hu\n",hdrPtr->rh_bits);
    fprintf(stream,"binary offset        = %hd (0x%04X)\n",hdrPtr->rh_zero,(unsigned short)hdrPtr->rh_zero);
	fprintf(stream,"scans per second     = %f\n",hdrPtr->rh_sps);
	fprintf(stream,"scans per meter      = %f\n",hdrPtr->rh_spm);
    fprintf(stream,"meters per mark      = %f\n",hdrPtr->rh_mpm);
    fprintf(stream,"position (ns)        = %f\n",hdrPtr->rh_position);
    fprintf(stream,"range (ns)           = %f\n",hdrPtr->rh_range);
    fprintf(stream,"scans per pass       = %hu\n",hdrPtr->rh_npass);
    if (hdrPtr->rh_create.month == 0)
        fprintf(stream,"create date          = no date\n");
    else
		fprintf(stream,"create date          = %s %d, %d (%d:%02d:%02d)\n",
			month_abbr[hdrPtr->rh_create.month],hdrPtr->rh_create.day,hdrPtr->rh_create.year+1980,
            hdrPtr->rh_create.hour,hdrPtr->rh_create.min,hdrPtr->rh_create.sec2*2);
    if (hdrPtr->rh_modif.month == 0)
        fprintf(stream,"modification date    = no date\n");
    else
        fprintf(stream,"modification date    = %s %d, %d (%d:%02d:%02d)\n",
            month_abbr[hdrPtr->rh_modif.month],hdrPtr->rh_modif.day,hdrPtr->rh_modif.year+1980,
            hdrPtr->rh_modif.hour,hdrPtr->rh_modif.min,hdrPtr->rh_modif.sec2*2);
	fprintf(stream,"offset to range gain = %hu\n",hdrPtr->rh_rgain);
    fprintf(stream,"size of range gain   = %hu\n",hdrPtr->rh_nrgain);
	fprintf(stream,"offset to text       = %hu\n",hdrPtr->rh_text);
	fprintf(stream,"size of text         = %hu\n",hdrPtr->rh_ntext);
    fprintf(stream,"offset to proc.hist. = %hu\n",hdrPtr->rh_proc);
    fprintf(stream,"size of proc.hist.   = %hu\n",hdrPtr->rh_nproc);
    if (stream==stdout || stream==stderr)
    {   fprintf(stream,"Press a key to continue ...");
        getch();
        /* fprintf(stream,"\r                                \r");*/
        puts("");
	}
	fprintf(stream,"number of channels   = %hu\n",hdrPtr->rh_nchan);
    fprintf(stream,"ave. diel. constant  = %f\n",hdrPtr->rh_epsr);
    fprintf(stream,"top pos. in meters   = %f\n",hdrPtr->rh_top);
    fprintf(stream,"range in meters      = %f\n",hdrPtr->rh_depth);
    if (header_bytes == 512)
    {   if (stream == stdout || stream == stderr)
        {   fprintf(stream,"Press a key to continue ...");
            getch();
			 /* fprintf(stream,"\r                                \r");*/
             puts("");
		}
		if      (strstr(out_filename,"stdout"))     ;
        else if (strstr(out_filename,"stderr"))     ;
        else                                        fclose(stream);
        return 0;
    }
    fprintf(stream,"reserved             = %s\n",hdrPtr->reserved);
    ant_freq[0] = 0;
#if 0
	for (i=0; ant_number[i]; i++)
	{   if (strstr(ant_number[i],hdrPtr->rh_antname))
        {   strcpy(ant_freq,ant_name[i]);
            break;
        }
    }
#endif
	for (i=0; ant_number[i]; i++)
    {   strcpy(newstr,hdrPtr->rh_antname);
		/* GSSI changed format to include frequency; old version
           just has 4-digit model number */
		if (strlen(hdrPtr->rh_antname) > 4)  newstr[4] = 0;
		if (strstr(newstr,ant_number[i]))
        {   strcpy(ant_freq,ant_name[i]);
            break; /* out of for loop */
        }
    }
    fprintf(stream,"antenna name         = %s",hdrPtr->rh_antname);
    if (ant_freq[0]) fprintf(stream," (%s)",ant_freq);
    puts("");
    fprintf(stream,"active channels mask = 0x%04X\n",hdrPtr->rh_chanmask);
	fprintf(stream,"this file name       = %s\n",hdrPtr->rh_name);
    fprintf(stream,"checksum for header  = %hu (0x%04X)\n",hdrPtr->rh_chksum,hdrPtr->rh_chksum);
    ustemp = hdrPtr->rh_chksum;
    hdrPtr->rh_chksum = 0;
    checksum = 0;
    for (i=0; i<512; i++)
        checksum += *((unsigned short *)hdrPtr + i);
    fprintf(stream,"calculated checksum  = %hu (0x%04X) [header %s corrupted]\n",
		checksum,checksum,(checksum==ustemp) ? "is not" : "is");
    hdrPtr->rh_chksum = ustemp;
	if (stream==stdout || stream==stderr)
	{   fprintf(stream,"Press a key to continue ...");
		getch();
		fprintf(stream,"\r                                \r");
	}
	if (hdrPtr->rh_rgain != 0 && hdrPtr->rh_nrgain != 0)
       {   rg_breaks = *(unsigned short *) ((char *)hdrPtr + hdrPtr->rh_rgain);
        fprintf(stream,"number of rg breaks  = %hd\n",rg_breaks);
        fprintf(stream,"range gain: scan sample     db\n");
        rg_break_delta = (hdrPtr->rh_nsamp-1) / (rg_breaks-1);
		for (i=0; i<rg_breaks; i++)
        {   fprintf(stream,"               %3d       %7.3f\n",
                rg_break_delta*i,*(float *)((char *)hdrPtr + hdrPtr->rh_rgain + 2 + 4*i));
        }
    }
    if (stream==stdout || stream==stderr)
    {   fprintf(stream,"Press a key to continue ...");
        getch();
		/* fprintf(stream,"\r                                \r");*/
        puts("");
    }
	if (hdrPtr->rh_text)
        fprintf(stream,"comments =\n%s\n",(char *)hdrPtr + hdrPtr->rh_text);
    if (hdrPtr->rh_nproc)
    {   ConvertProcHist2((int)sizeof(big_buff),big_buff,(int)hdrPtr->rh_nproc,
                                 ((char *)hdrPtr + hdrPtr->rh_proc));
        fprintf(stream,"Processing History:\n");
        if (strlen(big_buff)) fprintf(stream,"%s\n",big_buff);
        else                  fprintf(stream,"\tnone\n\n");
    }
	if      (strstr(out_filename,"stdout"))     ;
    else if (strstr(out_filename,"stderr"))     ;
    else                                        fclose(stream);
    return 0;
}
/**************************** ConvertProcHist2() ****************************/
/* This function converts the coded processing history into a long ASCII
 * string.  This code is modified from that written by GSSI, contact Leo
 * Galinovsky.  This function differs from ConvertProcHist() in that the
 * function arguments do not contain the structure pointer but the
 * processing history buffer and its size.
 *
 * Parameters:
 *   int bufsiz      - size of buffer pointed to by "buff"
 *   char *buff      - pointer to string (1024-byte length recommended)
 *   int hist_size   - bytes in processign history buffer
 *   char *proc_hist - pointer to string containing coded processing history
 *
 * Requires: <stdio.h>, <string.h>, attached header.
 * Calls: strcpy, sprintf, strlen.
 * Returns: char *buff.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: October 19, 1995
 */
char *ConvertProcHist2 (int bufsiz,char *buff,int hist_size,char *proc_hist)
{
	char *b,*p,*proc;
	int nproc;
	extern const char *ph_viirl,*ph_viirh,*ph_vfirl,*ph_vfirh,*ph_vboxl;
	extern const char *ph_vboxh,*ph_vtril,*ph_vtrih,*ph_vtcl,*ph_vtch;
	extern const char *ph_hiirl,*ph_hiirh,*ph_hfirl,*ph_hfirh,*ph_hboxl;
    extern const char *ph_htril,*ph_htcl,*ph_hboxh,*ph_htrih,*ph_htch,*ph_hsts;

    b = buff;
    b[0] = 0;
    p = proc = proc_hist;
    nproc = hist_size;
    while (p-proc < nproc)
    {   switch (*p)
		{   /* first two case to make header compatible with old tapes */
            case PR_LPF:
                p += 2 + sizeof(float);
                break;
            case PR_HPF:
                p += 2 + sizeof(float);
                break;
            case PR_VIIRL:
				sprintf(b,ph_viirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
			case PR_VIIRH:
                sprintf(b,ph_viirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VFIRL:
                sprintf(b,ph_vfirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VFIRH:
				sprintf(b,ph_vfirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VBOXL:
                sprintf(b,ph_vboxl,(int)*(p+1));
                p += 2;
                break;
            case PR_VBOXH:
				sprintf(b,ph_vboxh,(int)*(p+1));
                p += 2;
                break;
			case PR_VTRIL:
                sprintf(b,ph_vtril,(int)*(p+1));
                p += 2;
                break;
            case PR_VTRIH:
                sprintf(b,ph_vtrih,(int)*(p+1));
                p += 2;
                break;
            case PR_VTCL:
                sprintf(b,ph_vtcl,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VTCH:
                sprintf(b,ph_vtch,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HIIRL:
				sprintf(b,ph_hiirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
			case PR_HIIRH:
                sprintf(b,ph_hiirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HFIRL:
                sprintf(b,ph_hfirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HFIRH:
                sprintf(b,ph_hfirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HBOXL:
                sprintf(b,ph_hboxl,(int)*(p+1));
				p += 2;
                break;
            case PR_HTRIL:
				sprintf(b,ph_htril,(int)*(p+1));
                p += 2;
                break;
			case PR_HTCL:
                sprintf(b,ph_htcl,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HBOXH:
                sprintf(b,ph_hboxh,(int)*(p+1));
                p += 2;
                break;
            case PR_HTRIH:
                sprintf(b,ph_htrih,(int)*(p+1));
                p += 2;
                break;
            case PR_HTCH:
                sprintf(b,ph_htch,*(float *)(p+2));
				p += 2 + sizeof(float);
				break;
			case PR_STS:
				sprintf(b,ph_hsts,(int)*(p+1));
				p += 2;
				break;
#if 0
			case PR_STS_NEW:
                sprintf(b,ph_hsts,(int)*(p+1));
                p += 2;
                break;
#endif
			default:
				return buff;
		}
		if (strlen(buff) + strlen(b) > bufsiz) break;
		b += strlen(b);
	}
	return buff;
}
/***************************** end of s10_ehdr.c ****************************/

