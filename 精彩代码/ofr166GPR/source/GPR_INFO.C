/******************************** GPR_INFO.C ********************************\
 *                               version 1.00
 * GPR_INFO reads a ground penetrating radar (GPR) file and tries to determine
 * the storage format.  It reports the results to the screen.
 *
 * Revision History:
 *  October 27, 1995   - Completed.
 *  November xx, 1995  - Modified to use gpr_io.lib
 *  August 11, 1997    - Corrected printf's in ANSI not there section.
 *                     - Added cases for modified DZT and RAMAC formats.
 *  August 8, 2001     - Updated time and date functions
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /s010000H  /zfloatsync gpr_info.c gpr_io.lib
 *     To compile for 80486 executable :
 *       icc /F /xnovm /s010000H /zfloatsync /zmod486 gpr_disp.c gpr_io.lib
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /s010000H allocates 64K of stack space (required by graphics module)
 *       NOTE: This may have to be increased to 1 MB (/s0100000H) to
 *             accomodate some imaging routines in the graphics module.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     Jeff Lucius's GPR I/O functions (DZT, DT1, SGY) are in gpr_io.lib.
 *
 * To run: GPR_INFO gpr_filename
 *
\****************************************************************************/

#include "gpr_info.h"

int main(int argc, char *argv[])
{
/***** Declare variables *****/
	int  itemp;                /* scratch variable */
	int  file_hdr_bytes;       /* bytes in a file header */
	int  num_hdrs;             /* number of file headers */
	int  trace_hdr_bytes;      /* bytes in a trace header */
	int  samples_per_trace;    /* samples per trace */
	int  num_channels;         /* number of data channels */
	long num_traces;           /* number of traces per channel */
	int  input_datatype;       /* sample data type */
	int  total_time;           /* time range per trace */
	int  file_type;            /* data stoarge type */
	char filename[80];         /* input filename */

	/* in gpr_io.h */
	extern const char *GetGprFileTypeMsg[];

/***** Usage *****/
	if (argc < 2)
	{   puts("");
		if (ANSI_there())  printf("%c%c32;40m",0x1B,0x5B);
		puts("                                                                               ");
		puts("###############################################################################");
		puts("   Usage: GPR_INFO gpr_filename                                                ");
        puts("     Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
		puts("   This program inspects a ground penetrating radar (GPR) file to determine  ");
		puts("   the storage format type and essential information for reading the file.   ");
		puts("                                                                               ");
		puts("   Required command line arguments:                                            ");
		puts("     gpr_filename - name of the GPR file.                                      ");
		puts("                                                                               ");
        printf("  Version %s - Last Revision Date: %s at %s\n",GPR_INFO_VER,DATETIME,__TIME__);
		puts("###############################################################################");
		if (ANSI_there())  printf("%c%c37;40m",0x1B,0x5B);
		exit(1);
	}

	strcpy(filename,argv[1]);

/***** Get info *****/
	itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
					&samples_per_trace,&num_channels,&num_traces,
					&input_datatype,&total_time,&file_type,filename);
	if (itemp > 0)
	{   printf("\n%s\n",GetGprFileTypeMsg[itemp]);
		exit(2);
	}

/***** Print info *****/
  if (ANSI_there())
  {
	printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
	printf("\nGPR_INFO (Lastest Program Compile Date = %s at %s):\n\n",DATETIME,__TIME__);
	if (file_type == DT1)
	{   char *cp, hd_filename[80];

		strcpy(hd_filename,filename);
		cp = strchr(hd_filename,'.');
		if (cp) *cp = 0;
		strcat(hd_filename,".HD");
		printf("%c%c33;40mFilename:                     %c%c36;40m%s (%s)\n",0x1B,0x5B,0x1B,0x5B,strupr(filename),strupr(hd_filename));
	}  else
	{    printf("%c%c33;40mFilename:                     %c%c36;40m%s\n",0x1B,0x5B,0x1B,0x5B,strupr(filename));
	}
	switch (file_type)
	{   case DZT:     printf("%c%c33;40mStorage format:               %c%c36;40mGSSI SIR-10 or SIR-2\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
		case DT1:     printf("%c%c33;40mStorage format:               %c%c36;40mS&S PulseEKKO\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
		case SGY:     printf("%c%c33;40mStorage format:               %c%c36;40mSEG-Y\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
		case MOD_DZT: printf("%c%c33;40mStorage format:               %c%c36;40mmodified GSSI DZT\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
		case RD3:     printf("%c%c33;40mStorage format:               %c%c36;40mMala RAMAC\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
		case OTHER:   printf("%c%c33;40mStorage format:               %c%c36;40mUSER DEFINED\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
		default:      printf("%c%c33;40mStorage format:               %c%c36;40mUnknown or invalid\n",
						   0x1B,0x5B,0x1B,0x5B);       break;
	}
	printf("%c%c33;40mFile header bytes:            %c%c36;40m%d\n",0x1B,0x5B,0x1B,0x5B,file_hdr_bytes);
	printf("%c%c33;40mNumber of file headers:       %c%c36;40m%d\n",0x1B,0x5B,0x1B,0x5B,num_hdrs);
	printf("%c%c33;40mTrace header bytes:           %c%c36;40m%d\n",0x1B,0x5B,0x1B,0x5B,trace_hdr_bytes);
	printf("%c%c33;40mSamples per trace:            %c%c36;40m%d\n",0x1B,0x5B,0x1B,0x5B,samples_per_trace);
	printf("%c%c33;40mNumber of channels:           %c%c36;40m%d\n",0x1B,0x5B,0x1B,0x5B,num_channels);
	printf("%c%c33;40mNumber of traces per channel: %c%c36;40m%ld\n",0x1B,0x5B,0x1B,0x5B,num_traces);
	switch (input_datatype)
	{   case -1: printf("%c%c33;40mData element type:            %c%c36;40m1-byte unsigned characters\n",0x1B,0x5B,0x1B,0x5B); break;
		case  1: printf("%c%c33;40mData element type:            %c%c36;40m1-byte signed characters\n",0x1B,0x5B,0x1B,0x5B); break;
		case -2: printf("%c%c33;40mData element type:            %c%c36;40m2-byte unsigned short integers\n",0x1B,0x5B,0x1B,0x5B); break;
		case  2: printf("%c%c33;40mData element type:            %c%c36;40m2-byte signed short integers\n",0x1B,0x5B,0x1B,0x5B); break;
		case -3: printf("%c%c33;40mData element type:            %c%c36;40m4-byte unsigned long integers\n",0x1B,0x5B,0x1B,0x5B); break;
		case  3: printf("%c%c33;40mData element type:            %c%c36;40m4-byte signed long integers\n",0x1B,0x5B,0x1B,0x5B); break;
		case -5: printf("%c%c33;40mData element type:            %c%c36;40m2-byte unsigned short integers\n",0x1B,0x5B,0x1B,0x5B); break;
		case -6: printf("%c%c33;40mData element type:            %c%c36;40m4-byte unsigned long integers\n",0x1B,0x5B,0x1B,0x5B); break;
		case  4: printf("%c%c33;40mData element type:            %c%c36;40m4-byte floating point real\n",0x1B,0x5B,0x1B,0x5B); break;
		case  8: printf("%c%c33;40mData element type:            %c%c36;40m8-byte floating point real\n",0x1B,0x5B,0x1B,0x5B); break;
        default: printf("%c%c33;40mData element type:            %c%c36;40minvalid or unrecognized type\n",0x1B,0x5B,0x1B,0x5B); break;
    }
    printf("%c%c33;40mTotal time (nanoseconds):     %c%c36;40m%d\n",0x1B,0x5B,0x1B,0x5B,total_time);
    printf("%c%c37;40m",0x1B,0x5B);
  }
  else
  {
    printf("\nGPR_INFO (Lastest Program Compile Date = %s at %s):\n\n",__DATE__,__TIME__);
    if (file_type == DT1)
    {   char *cp, hd_filename[80];

        strcpy(hd_filename,filename);
        cp = strchr(hd_filename,'.');
        if (cp) *cp = 0;
        strcat(hd_filename,".HD");
		printf("Filename:                     %s (%s)\n",strupr(filename),strupr(hd_filename));
    }  else
    {    printf("Filename:                     %s\n",strupr(filename));
    }
	switch (file_type)
	{   case DZT:     printf("Storage format:               GSSI SIR-10 or SIR-2\n");       break;
		case DT1:     printf("Storage format:               S&S PulseEKKO\n");      break;
		case SGY:     printf("Storage format:               SEG-Y\n");              break;
		case MOD_DZT: printf("Storage format:               modified GSSI DZT\n");  break;
		case RD3:     printf("Storage format:               Mala RAMAC\n");  break;
		case OTHER:   printf("Storage format:               USER DEFINED\n");       break;
		default:      printf("Storage format:               Unknown or invalid\n"); break;
	}
	printf("File header bytes:            %d\n",file_hdr_bytes);
	printf("Number of file headers:       %d\n",num_hdrs);
	printf("Trace header bytes:           %d\n",trace_hdr_bytes);
	printf("Samples per trace:            %d\n",samples_per_trace);
	printf("Number of channels:           %d\n",num_channels);
	printf("Number of traces per channel: %ld\n",num_traces);
	switch (input_datatype)
	{   case -1: printf("Data element type:            1-byte unsigned characters\n"); break;
		case  1: printf("Data element type:            1-byte signed characters\n"); break;
        case -2: printf("Data element type:            2-byte unsigned short integers\n"); break;
        case  2: printf("Data element type:            2-byte signed short integers\n"); break;
        case -3: printf("Data element type:            4-byte unsigned long integers\n"); break;
        case  3: printf("Data element type:            4-byte signed long integers\n"); break;
        case -5: printf("Data element type:            2-byte unsigned short integers\n"); break;
        case -6: printf("Data element type:            4-byte unsigned long integers\n"); break;
        case  4: printf("Data element type:            4-byte floating point real\n"); break;
        case  8: printf("Data element type:            8-byte floating point real\n"); break;
        default: printf("Data element type:            invalid or unrecognized type\n"); break;
    }
    printf("Total time (nanoseconds):     %d\n",total_time);
  }
    return 0;
}

/******************************* ANSI_there() *******************************/
/* Determine if the ANSI.SYS driver has been installed.
 * Headers: "k2_port.hpp".
 * Functions: _int86, GetDosVersion.
 * Globals: none.
 * Returns: int = 0 if ANSI.SYS not installed,
 *                1 if it is installed.
 */
int ANSI_there (void)
{
    union REGS regs;
    int mode,major_ver,minor_ver;

    GetDosVersion(&mode,&major_ver,&minor_ver);

    if (major_ver >= 4)
    {   regs.x.ax = 0x1A00;
		int86(INT_MULTIPLEX,&regs,&regs);
        if (regs.h.al == 0) return 0;  /* not installed */
        else                return 1;
	} else
        return 0;
}
/***************************** GetDosVersion() ******************************/
/* Get version number of DOS.
 *
 * Parameter list:
 *    int *mode      - 0 if DOS loaded in conventional memory
 *                   - 1 if DOS loaded into HMA
 *    int *major_ver - major version number
 *    int *minor_ver - minor version number
 *
 * Example usage:
 *    int mode,major_ver,minor_ver;
 *    GetDosVersion(&mode,&major_ver,&minor_ver);
 *
 * Headers: "k2_port.hpp".
 * Functions: _int86.
 * Globals: none.
 * Returns: void.
 */
void GetDosVersion(int *mode,int *major_ver,int *minor_ver)
{
    union REGS regs;

    *mode = 0;
    regs.x.bx = 0x0000;
    regs.x.ax = 0x3306;       /* this function not changed by SETVER */
    int86(INT_DOS,&regs,&regs);
    if (regs.x.bx == 0x0000)  /* version less than 5.0 */
    {   regs.x.ax = 0x3000;
        int86(INT_DOS,&regs,&regs);
        if (regs.h.al == 0)
		{   *major_ver = 1;
            *minor_ver = 0;   /* actually any of the 1.x versions */
        } else
        {   *major_ver = regs.h.al;  /* for version 3.3, would be 3 */
            *minor_ver = regs.h.ah;  /* for version 3.3, would be 30 */
        }
    } else
    {   *major_ver = regs.h.bl;
        *minor_ver = regs.h.bh;
		*mode = (regs.h.dh & 0x10);
    }
}
