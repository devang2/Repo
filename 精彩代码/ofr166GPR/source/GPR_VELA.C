/********************************* GPR_VELA.C ********************************\
                               version 1.04.12.01
-----------------------------------------------------------------------------
INTRODUCTION

GPR_VELA.EXE calculates constant-velocity normal moveout (NMO) for a single GPR
common-midpoint (CMP) file. The velocity spectrum is calculated from the NMOs. 
The input to this program is a "CMD" file, an ASCII text file containing 
keywords (or commands) which are discussed in a section below. There is no 
graphic display of the data. To display the output data, use programs such as
GPR_DISP.EXE or FIELDVEW.EXE. If you need to select a subset of traces from a 
file then use GPR_SAMP.EXE.

-----------------------------------------------------------------------------
NMO CORRECTION AND THE VELOCITY SPECTRUM

For a subsurface model that consists of constant-velocity, horizontal layers, 
the travel-time curve (from the transmitting antenna to a layer and back to the 
receiving antenna) as a function of offset (which is the distance between 
antennas) is a hyperbola. The difference in travel time between zero offset and 
some other offset is called the normal moveout (NMO). The NMO correction is the 
subtraction of the appropriate NMO time from every sample in a trace. 

A common midpoint (CMP) GPR record is required to perform the NMO correction. A 
GPR CMP record is one in which the each trace in the file is from two antennas 
that are positioned equally and oppositely from a central point at uniformly 
increasing distances.

Usually a range of velocities is selected in order to determine the best NMO 
time (or velocity) for a particular layer. When the correct NMO velocity is 
used, the hyperbolic shape of a horizontal reflector in a CMP record turns into 
a flat reflector. See "Seismic Data Processing" by Ozdogan Yilmaz (1987; Society 
of Exploration Geophysicists) for details. 

The velocity spectrum consists of a set of traces where each trace is the result 
of stacking (or averaging) the results of one NMO. The number of traces equals 
the number of velocities selected for NMO corrections. The highest amplitudes in 
the velocity spectrum occur for the best velocity selections for each layer 
represented in the CMP.

Here is the general algorithm used to perform the NMO correction.

For every velocity (m/ns)
  Square the velocity; [V^2]
  For every trace in a file
    Calculate the offset (meters) of that trace; [X]
    Square the offset and divide by the square of the velocity; [X^2/V^2]
    For every sample in a trace
      Calculate the travel time (ns) at the sample and square the value; T^2
      Calculate the NMO time, Tnmo = SQRT(T^2 + X^2/V^2)
      Get the amplitude values for the samples on either side of the NMO time
      Linearly interpolate to the get the amplitude for the NMO time; AMPnmo
      Change AMPnmo to the median value if > the mute limit = [(Tnmo-T) / T]
      Assign the AMPnmo to the current sample
    If every sample has been muted then give special mark to trace

-----------------------------------------------------------------------------
GPR DATA STORAGE FORMATS

Only the GSSI SIR-10A, SIR-2, SIR-2000, or RADAN binary "DZT" format. is
supported at this time.
    NOTE: Only ONE channel will be used from multiple-channel DZT files.

-----------------------------------------------------------------------------
OPERATING SYSTEM LIMITATIONS

GPR_VELA.EXE is a protected-mode MS-DOS program that executes within the DOS 
operating system or within a DOS (or console) window in Microsoft Windows 
3.x/95/98. It probably will not execute within a console window in Windows NT. 
This program will force the computer CPU to work in "protected mode" so that all 
memory can be utilized, not just the 1 MB that DOS normally has access to. If an 
error message occurs when trying to run GPR_VELA.EXE it may be because the 
requested amount of "protected mode" RAM memory (the region size) is set too low 
for the program, or the requested region size is more than what is available on 
the computer. Use the program MODXCONF.EXE, which should have been distributed 
with this software, to adjust the region size.

NOTE: Files used by this program must follow the naming conventions required by 
early versions of MS-DOS. Filenames can be only eight (8) characters long, with 
no embedded spaces, with an optional extension of up to three characters. This 
program will run in a Windows 95/98 console where filenames do not have to 
follow this convention. Unexpected results may occur if input file names, 
including the CMD file, are longer than 8 characters. For example, both command 
files procfile.cmd and procfile1.cmd will be seen as procfile.cmd to a DOS 
program.

-----------------------------------------------------------------------------
RULES FOR CONSTRUCTING "GPRSCONV.CMD" FILES:
  1. All valid keywords and their defaults are shown below in the next
       section. Only one keyword can be used per line.
  2. Keywords can be upper, lower, or mixed case.
  3. Only lines with an equal sign and the valid (correctly spelled) keyword to
       the left of the equal sign will be used.  All OTHERS ARE IGNORED (except
       for sets of numbers, see 11, 12, and 13 below).  Blank lines are
       ignored everywhere (also see 6, 7, and 10 below).
  4. Numeric values assigned to keywords must be a single number.
       Mathematical operations, such as 5+2, are not recognized. DO NOT
       insert a comma in a number. For example, use 1234 not 1,234.
       DO NOT place quotes on either side of a numeric entry.
  5. Equal signs do not have to line up.
  6. Spaces are ignored (and removed) except those within strings and sets of
       numbers.
  7. A semicolon begins a comment, except within strings. Text and numbers
       after a semicolon are ignored.
  8. Strings must be enclosed in double quotation marks.
       Examples: use_mark_file = "TRUE"
                 use_xyz_file = "FALSE"
     NOTE: DO NOT enclose the file names in the input and output lists in
           quotes.
  9. "TRUE" equals 1. "FALSE" equals 0.  "INVALID_VALUE" equals 1.0E19.
 10. Lines should be less than 160 characters long (characters past 159 are
      ignored).
 11. Keywords can be given in any order, except the num_... keywords which
       must precede the arrays, or sets of numbers, they are describing.
 12. Keywords that expect sets of data end in square brackets.
 13. Sets of numbers or strings must be separated by a space or a
       "new line" characters (i.e. a carriage return/line feed pair, CR/LF,
        generated by pressing <Enter>).
     Example: num_input_files = 8
              input_filelist[] = file1.dzt file2.dzt file3.dzt file4.dzt
                                 file5.dzt file6.dzt file7.dzt file8.dzt
     Example: num_input_files = 8
              input_filelist[] = 
                file1.dzt file2.dzt file3.dzt file4.dzt
                file5.dzt file6.dzt file7.dzt file8.dzt
     Example: num_input_files = 8
              input_filelist[] = file1.dzt file2.dzt
                          file3.dzt file4.dzt
                          file5.dzt file6.dzt
                          file7.dzt file8.dzt

 14. Almost all values have defaults except for the input and output data
       filenames. At least one filename must be specified in each list.
 15. Parameters or commands may appear more than once, but usually only the
       last instance is preserved (ie., earlier values are lost or written
       over).
-----------------------------------------------------------------------------
WRITTEN BY:
Jeff Lucius
U.S. Geological Survey
Box 25046  Denver Federal Center  MS 964
Denver, CO  80225-0046
voice: 303-236-1413
fax:   303-236-1425
email: lucius@usgs.gov
-----------------------------------------------------------------------------
Revision History:
April 12, 2001     - First version completed.
                   - Only DZT as input.
                   - No graphics.
                   - All output is in one DZT file.

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /zfloatsync gpr_vela.c gpr_io.lib jl_util1.lib
     To compile for 80486 executable :
       icc /F /xnovm /zfloatsync /zmod486 gpr_vela.c gpr_io.lib jl_util1.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions, speeds up program.
     gpr_io.lib   - Jeff Lucius's GPR I/O functions (DZT, DT1, SGY).
     jl_util1.lib - Jeff Lucius's utility functions.

     to remove assert() functions from code also add:
     /DNDEBUG

 To run: GPR_VELA cmd_filename
\****************************************************************************/

/******************* Function dependency and resource usage *****************/
/*
 * con I/O  - console I/O - keyboard and/or text-mode screen used
 * disk I/O - reads from and/or writes to disk
 * DOS      - DOS/BIOS interrupt performed
 * graphics - graphics modes required, direct writes to ports/video adaptor
 * port I/O - hardware I/O ports used
 * RAM      - direct access of CPU RAM memory
 *
 * main                                           (con I/O)
 *   +-- VelaGetParameters                            (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line and opening the GPR data/info files to get additional
 *   |     | information about the data sets.
 *   |     +-- VelaPrintUsageMsg                      (con I/O)
 *   |     +-- VelaInitParameters
 *   |     +-- VelaGetCmdFileArgs          (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetGprFileType          (disk I/O)
 *   |     +-- ReadOneDztHeader        (disk I/O)
 *   |     +-- VelaDeallocInfoStruct
 *   +-- VelaDisplayParameters                        (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- GetGprDataAsGrid              (disk I/O) (con I/O)
 *   |     +-- GetDztChSubGrid16       (disk I/O)
 *   +---VelaPerformNMO
 *   |     | This function performs the NMOs and saves the results to
 *   |     | grid[][][].
 *   |     +-- ChangeRangeGain16u
 *   +---VelaCalcVelSpectrum
 *   |
 *   +-- VelaSaveGprData                     (disk I/O)(con I/O)
 *   |     | This function sets the proc_hist and calls the data save function.
 *   |     +-- VelaSaveDztData               (disk I/O)(con I/O)
 *   |     |     +-- ReadOneDztHeader        (disk I/O)
 *   |     |     +-- SetDzt5xHeader
 *   |     |     +-- SaveDztFile             (disk I/O)
 *   +-- VelaSaveInfo
 *   +-- ANSI_there                                              (DOS)
 *   |     +-- GetDosVersion                                     (DOS)
 *   +-- Sound                                                        (port I/O)
 */
/******************* Includes all required header files *********************/
#include "gpr_vela.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_VELA_CMDS[]=
{   "debug","batch","display_none","dzt_infilename","dzt_outfilename",
    "channel","samp_first","trace_first","trace_last","pos_start",
/* 10 */
    "pos_step","vel_start","vel_step","vel_num","mute",
    "rg_start_trace","rg_start_samp","rg_stop_samp","rg_step","rg_num_on",
/* 20 */
    "rg_on[0]","rg_on[1]",NULL,
} ;

/* The rest of these are message strings that match codes returned by functions */
const char *VelaVelaGetParametersMsg[] =
{   "VelaGetParameters(): No errors.",
    "VelaGetParameters() ERROR: Insufficient command line arguments.",
    "VelaGetCmdFileArgs() ERROR: Unable to open input command file.",
    "VelaGetCmdFileArgs() ERROR: Input file(s) not specified.",
    "VelaGetParameters() ERROR: Problem getting information about input data file.",

    "VelaGetParameters() ERROR: Storage format not GSSI DZT.",
    "VelaGetParameters() ERROR: Problem getting data from GSSI DZT file.",
    "VelaGetParameters() ERROR: Invalid channel selection for DZT file.",
    "VelaGetParameters() ERROR: Problem getting info from S&S HD file.",
    "VelaGetParameters() ERROR: Problem getting info from SGY file.",

    "VelaGetParameters() ERROR: Problem getting info from user-defined storage-format file.",
    "VelaGetParameters() ERROR: No recognizable data/info input files specified.",
    "VelaGetParameters() ERROR: Max and min values must be given for floating point data.",
    "VelaGetParameters() ERROR: Value for ns_per_samp is invalid.",
    "VelaGetParameters() ERROR: Value for vel_num is less than 1.",

    "VelaGetParameters() ERROR: Velocities must be between 0.01 and 0.30 (m/ns).",
    "VelaGetParameters() ERROR: Mute value must be greater than 0.",
} ;
const char *GetDataAsGrid16uMsg[] =
{   "GetDataAsGrid16u(): No errors.",
    "GetDataAsGrid16u() ERROR: Not able to allocate storage for GPR grid[][][].",
    "GetDataAsGrid16u() ERROR: Problem getting data from input file.",
    "GetDataAsGrid16u() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetDataAsGrid16u() ERROR: Attemp made to re-allocate storage.  De-allocate first.",

    "GetDataAsGrid16u() ERROR: Unsupported data storage format.",
} ;
const char *VelaPerformNMOMsg[] =
{   "VelaPerformNMO(): No errors.",
	"VelaPerformNMO() ERROR: Divide by zero: ns_per_samp",
	"VelaPerformNMO() ERROR: Divide by zero: V2",
	"VelaPerformNMO() ERROR: Divide by zero: deltime",
	"VelaPerformNMO() ERROR: Unable to allocate temporary storage.",
} ;
const char *VelaCalcVelSpectrumMsg[] =
{   "VelaCalcVelSpectrum(): No errors.",
    "VelaCalcVelSpectrum() ERROR: Attemp made to de-allocate storage before allocating.",
    "VelaCalcVelSpectrum() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "VelaCalcVelSpectrum() ERROR: Not able to allocate storage for GPR grid[][][].",
    "VelaCalcVelSpectrum() ERROR: Not able to allocate temporary storage.",
} ;
const char *ChangeRangeGain16uMsg[] =
{   "ChangeRangeGain16u(): No errors.",
    "ChangeRangeGain16u() ERROR: Number of samps is less than 2.",
    "ChangeRangeGain16u() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
    "ChangeRangeGain16u() ERROR: NULL pointer passed to function.",
    "ChangeRangeGain16u() ERROR: start_samp value is invalid.",
    "ChangeRangeGain16u() ERROR: stop_samp value is invalid.",
} ;
const char *VelaSaveGprDataMsg[] =
{   "VelaSaveGprData(): No errors.",
    "VelaSaveGprData() ERROR: Invalid file storage format.",
    "VelaSaveGprData() ERROR: Data/information was not saved to disk.",
} ;
const char *VelaSaveDztDataMsg[] =
{   "VelaSaveDztData(): No errors.",
	"VelaSaveDztData() ERROR: Invalid datatype for output DZT file.",
    "VelaSaveDztData() ERROR: Problem getting input file header.",
    "VelaSaveDztData() ERROR: Unable to open output NMO file.",
    "VelaSaveDztData() ERROR: Problem saving header to disk.",
    "VelaSaveDztData() ERROR: Problem saving data to disk.",
} ;
const char *VelaSaveInfoMsg[] =
{   "VelaSaveInfo(): No errors.",
    "VelaSaveInfo() ERROR: Unable to open output TXT file.",
    "VelaSaveInfo() ERROR: Problem saving TXT data.",
} ;

/********************************** main() **********************************/
void main(int argc, char *argv[])
{
/***** Declare variables *****/
    /* following are variables used by main */
    char   log_filename[80];
    int    itemp;             /* scratch variable */
    int    error;             /* flag indicating an error has occured */
    double dtemp;             /* scratch variable */

    /* following is the information structure passed between functions */
    struct VelaParamInfoStruct ParamInfo;

    /* these variables found near beginning of this source */
    extern int         Debug,Batch,ANSI_THERE;
    extern FILE       *log_file;
    extern const char *VelaVelaGetParametersMsg[];
    extern const char *GetDataAsGrid16uMsg[];

/***** Initialize variables *****/

    /* open error message file */
    strcpy(log_filename,"GPR_VELA.LOG");
    log_file = fopen(log_filename,"a+t");  /* open text file for appending */
    if (log_file == NULL)
    {   strcpy(log_filename,"C:\\GPR_VELA.LOG");
        log_file = fopen(log_filename,"a+t");
    }
    if (log_file)
    {   struct dosdate_t dosdate;
        struct dostime_t dostime;
        char *month[] = { "","January","February","March","April","May","June",
                            "July","August","September","October",
                            "November","December" } ;

        _dos_getdate(&dosdate);
        _dos_gettime(&dostime);
        fprintf(log_file,"\n**************************************************************************\n");
        fprintf(log_file,"Messages from program GPR_VELA v. %s: %s %d, %d at %d:%02d:%02d\n",
                        GPR_VELA_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute,dostime.second);
        fprintf(log_file,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.\n");
    }

    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Initialize rest of main() variables */
    error = FALSE;

/***** Get information from the CMD file *****/
    if (log_file && argv[1])
    {   fprintf(log_file,"Processing command file: %s\n",argv[1]);
    }
    printf("Getting user parameters ...");
    itemp =  VelaGetParameters(argc,argv,&ParamInfo);
    printf("\r                                                              \r");
    if (itemp > 0)
    {   printf("\n%s\n",VelaVelaGetParametersMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",VelaVelaGetParametersMsg[itemp]);
        if (log_file) fclose(log_file);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("\nGPR_VELA finished.\nFile %s on disk contains messages.\n",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(1);
    }
    if ( (strcmp(ParamInfo.dzt_infilename,ParamInfo.dzt_outfilename) == 0) )
    {   printf("\nERROR: Input filename is duplicated in output list.\n");
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"ERROR: Input filename is duplicated in output list.\n");
            fclose(log_file);
            printf("\nFile %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_VELA finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(2);
    }
    if (log_file)
    {   if (Batch) fprintf(log_file,"Program in Batch mode.\n");
        if (Debug) fprintf(log_file,"Program in Debug mode.\n");
    }

/***** Display parameters and ask if user wants to continue *****/
    if (ParamInfo.display_none == TRUE)
    {   ; /* no parameter display */
    } else
    {   VelaDisplayParameters(&ParamInfo);
        if (!Batch)
        {   printf("Press <Esc> to quit or other key to continue ... ");
            if ((itemp = getch()) == ESC)
            {   VelaDeallocInfoStruct(&ParamInfo);
                puts("\nProgram terminated by user.\n");
                if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                if (log_file)
                {   fprintf(log_file,"End of messages.  Program terminated by user.\n");
                    fclose(log_file);
                    printf("File %s on disk contains messages.\n",log_filename);
                }
                puts("GPR_VELA finished.");
                if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                exit(3);
            } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
                getch();
            printf("\r                                                           \r");
        }
    }

/***** Get the data *****/
    printf("Getting Data ...");
    itemp = GetDataAsGrid16u(FUNC_CREATE,&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   VelaDeallocInfoStruct(&ParamInfo);
        printf("\n%s\n",GetDataAsGrid16uMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",GetDataAsGrid16uMsg[itemp]);
        if (itemp == 1)  /* data set too large for memory */
        {   unsigned long mem,bytes,requested;

            mem = _memavl();
            requested = (ParamInfo.trace_last - ParamInfo.trace_first + 1)
                         * ParamInfo.total_samps * 2;
            requested *= ParamInfo.num_grids;
            printf("\nSize of GPR data storage (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
            if (log_file) fprintf(log_file,"Size of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
        }
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_VELA finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(4);
    }

/***** Perform NMO corrections *****/
    printf("Performing NMOs ...");
    itemp = VelaPerformNMO(&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   VelaDeallocInfoStruct(&ParamInfo);
        printf("\n%s\n",VelaPerformNMOMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",VelaPerformNMOMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_VELA finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(5);
    }

/***** Calculate velocity spectrum *****/
    printf("Calculating velocity spectrum ...");
    itemp = VelaCalcVelSpectrum(FUNC_CREATE,&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   VelaDeallocInfoStruct(&ParamInfo);
        printf("\n%s\n",VelaCalcVelSpectrumMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",VelaCalcVelSpectrumMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_VELA finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(6);
    }

/***** Save input data and NMOs to disk together *****/
	printf("Saving data and NMOs to disk ...");
	itemp = VelaSaveGprData(&ParamInfo);
	printf("\r                                                 \r");
	if (itemp > 0)
	{   printf("\n%s\n",VelaSaveGprDataMsg[itemp]);
		/* GetDataAsGrid16u(FUNC_DESTROY,&ParamInfo); */
		VelaDeallocInfoStruct(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		if (log_file)
		{   fprintf(log_file,"%s\n",VelaSaveGprDataMsg[itemp]);
			fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("\nGPR_VELA finished.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		if (!Batch)
		{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
				Sound(dtemp,0.055);
		}
		exit(7);
	}

    itemp = VelaSaveInfo(&ParamInfo);
	if (itemp > 0)
	{   printf("\n%s\n",VelaSaveInfoMsg[itemp]);
		/* GetDataAsGrid16u(FUNC_DESTROY,&ParamInfo); */
		VelaDeallocInfoStruct(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		puts("\nWARNING: GPR_VELA did not write the information file to disk.");
		if (log_file)
		{   fprintf(log_file,"%s\n",VelaSaveInfoMsg[itemp]);
			fprintf(log_file,"WARNING: GPR_VELA did not write the information file to disk.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
	}

    if (log_file)
    {   fprintf(log_file,"End of messages.  Program terminated normally.\n");
        fclose(log_file);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("GPR_VELA finished.\nFile %s on disk contains messages.",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
    }
    exit(0);
}
/****************************** VelaGetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 * Determine GPR file storage format.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <math.h>, <stdio.h>, <stdlib.h>, <string.h>,
 *           "gpr_vela.h".
 * Calls: printf, VelaPrintUsageMsg, VelaInitParameters, VelaGetCmdFileArgs.
 *        strcpy, puts, strstr, atoi, atof, strchr, strcat, memcpy, strupr.
 *        ReadOneDztHeader, VelaDeallocInfoStruct,
 *
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *VelaVelaGetParametersMsg[] =
{   "VelaGetParameters(): No errors.",
    "VelaGetParameters() ERROR: Insufficient command line arguments.",
    "VelaGetCmdFileArgs() ERROR: Unable to open input command file.",
    "VelaGetCmdFileArgs() ERROR: Input file(s) not specified.",
    "VelaGetParameters() ERROR: Problem getting information about input data file.",

    "VelaGetParameters() ERROR: Storage format not GSSI DZT..",
    "VelaGetParameters() ERROR: Problem getting data from GSSI DZT file.",
    "VelaGetParameters() ERROR: Invalid channel selection for DZT file.",
    "VelaGetParameters() ERROR: Problem getting info from S&S HD file.",
    "VelaGetParameters() ERROR: Problem getting info from SGY file.",

    "VelaGetParameters() ERROR: Problem getting info from user-defined storage-format file.",
    "VelaGetParameters() ERROR: No recognizable data/info input files specified.",
    "VelaGetParameters() ERROR: Max and min values must be given for floating point data.",
    "VelaGetParameters() ERROR: Value for ns_per_samp is invalid.",
    "VelaGetParameters() ERROR: Value for vel_num is less than 1.",

    "VelaGetParameters() ERROR: Velocities must be between 0.01 and 0.30 (m/ns).",
    "VelaGetParameters() ERROR: Mute value must be greater than 0.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 11, 2001;
 */
int VelaGetParameters (int argc, char *argv[],struct VelaParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/
    int    req_args = 2;   /* exe_name, and cmd_filename */
    int    i,itemp;        /* scratch variables and counters */
    int    header_size;    /* number of bytes in DZT header */
    long   row,col;        /* scratch variables and counters */
    int  file_hdr_bytes,num_hdrs,trace_hdr_bytes,samples_per_trace;
    int  num_channels,input_datatype,total_time,file_type;
    long total_traces;
    double dtemp;

    extern int Debug;
    extern FILE *log_file;
    extern const char *GetDataLimitsMsg[];
    extern const char *GetGprFileTypeMsg[];

/***** Verify usage *****/
    if (argc < req_args)
    {   VelaPrintUsageMsg();
        return 1;
    }

/***** Initialize information structure *****/
    VelaInitParameters(InfoPtr);
    strcpy(InfoPtr->cmd_filename,argv[1]);

/***** Get user-defined parameters from CMD file *****/
    itemp = VelaGetCmdFileArgs(InfoPtr);
    if (itemp) return itemp;   /* 2 to 3 */

/***** Determine GPR file storage format and essential info *****/
    if (InfoPtr->input_format == 0)
    {   itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                    &samples_per_trace,&num_channels,&total_traces,
                    &input_datatype,&total_time,&file_type,
                    InfoPtr->dzt_infilename);
        if (itemp > 0)
        {   printf("%s\n",GetGprFileTypeMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
            return 4;
        }
        switch (file_type)
        {   case DZT:  case MOD_DZT:
               break;
            default:
                return 5;
        }
        InfoPtr->input_format       = file_type;
        InfoPtr->file_header_bytes  = file_hdr_bytes;
        InfoPtr->trace_header_bytes = trace_hdr_bytes;
        InfoPtr->total_traces       = total_traces;
        InfoPtr->total_samps        = samples_per_trace;
        InfoPtr->total_time         = total_time;
        InfoPtr->ns_per_samp        = (double)InfoPtr->total_time / (InfoPtr->total_samps - 1);
        InfoPtr->input_datatype     = input_datatype;
    }

/***** Get necessary information from info file/header *****/
    if (InfoPtr->input_format == DZT || InfoPtr->input_format == MOD_DZT)
    {   int num_hdrs,channel;
        struct DztHdrStruct DztHdr;                /* from gpr_io.h */
        extern const char *ReadOneDztHeaderMsg[];  /* from gpr_io.h */
        extern const char *ant_number[];           /* from gpr_io.h */
        extern const char *ant_name[];             /* from gpr_io.h */

        /* get first DZT header in file */
        channel = 0;
        itemp = ReadOneDztHeader(InfoPtr->dzt_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),channel,
                                 &header_size,&DztHdr);
        if (itemp > 0)
        {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
            if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
            {   puts("Fatal error.");
                return 6;
            }
        }
        if (header_size < 1024)
        {   puts("This DZT header is not consistent with version 5.x SIR-10A software.");
            if (log_file) fprintf(log_file,"This DZT header is not consistent with version 5.x SIR-10A software.\n");
        }
        if (InfoPtr->channel >= num_hdrs) return 7;


        /* get selected header (if different than first) */
        if (InfoPtr->channel > 0)
        {   itemp = ReadOneDztHeader(InfoPtr->dzt_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),InfoPtr->channel,
                                 &header_size,&DztHdr);
            if (itemp > 0)
            {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
                if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                {   puts("Fatal error.");
                    return 6;
                }
            }
        }

        /* assign values to info structure */
        InfoPtr->total_samps    = DztHdr.rh_nsamp;
        /* if (InfoPtr->samp_first < 0)   InfoPtr->samp_first = 0; */
        if      (DztHdr.rh_bits ==  8) InfoPtr->input_datatype = UCHAR;
        else if (DztHdr.rh_bits == 16) InfoPtr->input_datatype = USHORT;
        else if (DztHdr.rh_bits == 32) InfoPtr->input_datatype = ULONG;
        InfoPtr->total_time       = DztHdr.rh_range;
        InfoPtr->ns_per_samp      = DztHdr.rh_range/(DztHdr.rh_nsamp-1);
        for (itemp=0; ant_number[itemp]; itemp++)
        {   if (strstr(DztHdr.rh_antname,ant_number[itemp]))
            {   InfoPtr->ant_freq = atoi(ant_name[itemp]);
                break;
            }
        }
        if (Debug)
        {   printf("total_traces = %ld\n",InfoPtr->total_traces);
            printf("total_samps = %ld   total_time = %g\n",InfoPtr->total_samps,InfoPtr->total_time);
            printf("samp_first = %ld\n",InfoPtr->samp_first);
            printf("ns_per_samp = %g\n",InfoPtr->ns_per_samp);
            printf("datatype = %d\n",InfoPtr->input_datatype);
            printf("trace_per_m=%g pos_start=%g pos_final=%g pos_step=%g\n",
                InfoPtr->pos_start,InfoPtr->pos_final,InfoPtr->pos_step);
            printf("num_hdrs = %d\n",num_hdrs);
            getch();
        }

    } else /* if (InfoPtr->input_format == DZT) */
        return 11;

/***** Finish initializing variables and verifying valid ranges *****/
    /* limit first and last traces to valid ranges */
    if ( (InfoPtr->trace_first >= InfoPtr->total_traces) ||
         (InfoPtr->trace_first < 0) )
        InfoPtr->trace_first = 0;
    if ( (InfoPtr->trace_last == 0 ) ||
         (InfoPtr->trace_last < InfoPtr->trace_first) ||
         (InfoPtr->trace_last >= InfoPtr->total_traces) )
        InfoPtr->trace_last = InfoPtr->total_traces - 1;
    InfoPtr->trace_num = InfoPtr->trace_last - InfoPtr->trace_first + 1;

    /* limit first sample to valid range */
    if (InfoPtr->samp_first >= InfoPtr->total_samps)
        InfoPtr->samp_first = 0;

    /* Check sample rate */
    if ( InfoPtr->ns_per_samp == INVALID_VALUE || InfoPtr->ns_per_samp <= 0.0 )
    {   VelaDeallocInfoStruct(InfoPtr);
        return 13;
    }

    /* calculate pos_final */
    InfoPtr->pos_final = InfoPtr->pos_start + (InfoPtr->trace_last - InfoPtr->trace_first)* InfoPtr->pos_step;

    /* calculate grid size info */
    if (InfoPtr->vel_num < 1) return 14;
    InfoPtr->num_grids  = InfoPtr->vel_num + 2;
    InfoPtr->num_traces = InfoPtr->trace_num;
    InfoPtr->num_samps  = InfoPtr->total_samps;
    InfoPtr->num_nmos   = InfoPtr->vel_num;

    /* Be sure velocities are in range */
    if ((InfoPtr->vel_start < 0.01) || (InfoPtr->vel_start > 0.30) )
        return 15;
    InfoPtr->vel_last = InfoPtr->vel_start + (InfoPtr->vel_step * (InfoPtr->vel_num-1));
    if ((InfoPtr->vel_last < 0.01) || (InfoPtr->vel_last > 0.30) )
        return 15;

    /* check mute */
    if (InfoPtr->mute <=0.0) return 16;

    return 0;
}
/****************************** VelaPrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>, <dos.h>.
 * Calls: puts, sprintf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 10, 2001;
 */
void VelaPrintUsageMsg (void)
{
    char str[10];
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("\n###############################################################################");
    puts("  Usage: GPR_VELA cmd_filename");
    puts("    Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.");
    puts("  This program performs constant velocity NMO corrections and calculates the");
    puts("    velocity spectrum for a single GPR CMP file. The results are stored to ");
    puts("    disk in a single DZT file. A text INF file is also written.");
    puts("    Only DZT files can be read in.");
    puts("  Required command line arguments:");
    puts("    cmd_filename - name of text file that follows the \"CMD\" format.");
    puts("  Look at \"GPR_VELA.CMD\" and \"GPR_VELA.TXT\" for keyword file format, valid");
    puts("    keywords, and documentation.");
    puts("  Example call: GPR_VELA vfile1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_VELA_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** VelaInitParameters() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "gpr_vela.h".
 * Calls:
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 11, 2001;
 */
void VelaInitParameters (struct VelaParamInfoStruct *InfoPtr)
{
    int i;

    extern int Debug;

    InfoPtr->cmd_filename[0]         = 0;
    InfoPtr->dzt_infilename[0]       = 0;
    InfoPtr->input_format            = 0;
    InfoPtr->input_datatype          = 0;
    InfoPtr->dzt_outfilename[0]      = 0;
    InfoPtr->inf_outfilename[0]      = 0;
    InfoPtr->display_none            = FALSE;
    InfoPtr->file_header_bytes       = 0;
    InfoPtr->trace_header_bytes      = 0;
    InfoPtr->channel                 = 0;
    InfoPtr->ant_freq                = 0;
    InfoPtr->total_traces            = 0;
    InfoPtr->samp_first              = 0;
    InfoPtr->total_samps             = 0;
    InfoPtr->total_time              = 0.0;
    InfoPtr->ns_per_samp             = 0.0;
    InfoPtr->pos_start               = 0.0;
    InfoPtr->pos_final               = 0.0;
    InfoPtr->pos_step                = 0.0;
    InfoPtr->trace_first             = 0;
    InfoPtr->trace_last              = 0;
    InfoPtr->trace_num               = 0;
    for (i=0; i<sizeof(InfoPtr->proc_hist); i++) InfoPtr->proc_hist[i] = 0;
    InfoPtr->vel_start               = 0.0;
    InfoPtr->vel_last                = 0.0;
    InfoPtr->vel_step                = 0.0;
    InfoPtr->vel_num                 = 0;
    InfoPtr->mute                    = 50.0;
    InfoPtr->rg_start_trace          = 0;
    InfoPtr->rg_start_samp           = 0;
    InfoPtr->rg_stop_samp            = 0;
    InfoPtr->rg_step                 = 0;
    InfoPtr->rg_num_on               = 0;
    InfoPtr->rg_on[0]                = 0.0;
    InfoPtr->rg_on[1]                = 0.0;
    InfoPtr->rg_num_off              = 0;
    InfoPtr->rg_off[0]               = 0.0;
    InfoPtr->rg_off[1]               = 0.0;
    InfoPtr->grid_created            = FALSE;
    InfoPtr->grid                    = NULL;
    InfoPtr->num_grids               = 0;
    InfoPtr->num_traces              = 0;
    InfoPtr->num_samps               = 0;
    InfoPtr->vgrid_created           = FALSE;
    InfoPtr->vgrid                   = NULL;
    InfoPtr->num_nmos                = 0;

}
/***************************** VelaGetCmdFileArgs() *****************************/
/* Get processing parameters from user from a command file.
 *
const char *GPR_VELA_CMDS[]=
{   "debug","batch","display_none","dzt_infilename","dzt_outfilename",
    "channel","samp_first","trace_first","trace_last","pos_start",
* 10 *
    "pos_step","vel_start","vel_step","vel_num","mute",
    "rg_start_trace","rg_start_samp","rg_stop_samp","rg_step","rg_num_on",
* 20 *
    "rg_on[0]","rg_on[1]",NULL,
} ;
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <stdio.h>, <stdlib.h>, <string.h>, "gpr_vela.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strcat,
 *        strcmp, strupr, strlwr, strncpy, atol, malloc, free,
 *        Trimstr, strlen, getch.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 9, 2001;
 */
int VelaGetCmdFileArgs (struct VelaParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];
    int found,i,num,itemp,have_string;
    int dat_in_found = FALSE;
    long ltemp;
    double dtemp;
    FILE *infile = NULL;
    extern int Debug,Batch,Batchset,Debugset;

    if (Debug) puts("");
    if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
    {   char *cptr;

        if ((cptr = strchr(InfoPtr->cmd_filename,'.')) == NULL)
        {   strcat(InfoPtr->cmd_filename,".");
            if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
            {   strcat(InfoPtr->cmd_filename,"cmd");
                if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
                    return 2;
            }
        } else
        {   *cptr = 0;
            strcat(InfoPtr->cmd_filename,".cmd");
            if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
                return 2;
        }
    }
    have_string = FALSE;
    while (1)            /* read till EOF or error */
    {   if (have_string == FALSE) /* if have not already retrieved file line */
        {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                break;   /* out of while(1) loop */
        } else
        {   have_string = FALSE;
        }
        cmdstr[0] = 0;            /* set to 0 so strncat() works right */
        cp = strchr(str,'=');     /* look for equal sign */
        if (cp == NULL) continue; /* invalid command line so ignore */
        num = (int)(cp-str);      /* number of chars before equal sign */
        strncat(cmdstr,str,(size_t)num);  /* copy to working string */
        TrimStr(cmdstr);          /* remove leading and trailing blanks */
        found = -1;
        i = 0;
        while (GPR_VELA_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_VELA_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_VELA_CMDS[] */
                break;
            }
            i++;
        }
        if (found == -1) continue;    /* bad or missing command word */
        cp++;                         /* step 1 past equal sign */
        /* truncate string at semicolon if present outside of double quotes */
        cp2 = cp3 = cp4 = NULL;       /* initialize pointers */
        cp2 = strchr(cp,'"');         /* see if first quote present */
        if (cp2 != NULL)
            cp3 = strchr(cp2+1,'"');  /* see if second quote present */
        cp4 = strchr(cp,';');         /* see if semicolon present */
        if (cp4 != NULL)              /* if present ... */
        {   if (cp3 != NULL)
            {   if (cp4 < cp2 || cp4 > cp3)
                    *cp4 = 0;         /* truncate at semicolon */
            }
        }
        TrimStr(cp);                  /* remove leading and trailing blanks */
#if 1
        if (Debug)
        {    printf("cp = %s\n",cp);
            getch();
        }
#endif
        switch (found)       /* cases depend on same order in GPR_VELA_CMD[] */
        {   case 0:
                itemp = 0;
                cp2 = strchr(cp,'"');        /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  Debug = FALSE;
                else             Debug = TRUE;
                break;
            case 1:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  Batch = FALSE;
                else             Batch = TRUE;
                if (Debug) printf("Batch = %d\n",Batch);
                break;
            case 2:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->display_none = FALSE;
                else             InfoPtr->display_none = TRUE;
                if (Debug) printf("display_none = %d\n",InfoPtr->display_none);
                break;
            case 3:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past first quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dzt_infilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->dzt_infilename);
                if (Debug)  printf("data file = %s\n",InfoPtr->dzt_infilename);
                if (InfoPtr->dzt_infilename[0]) dat_in_found = TRUE;
                break;
            case 4:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dzt_outfilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->dzt_outfilename);
                if (strstr(InfoPtr->dzt_outfilename,".DZT") == NULL)
                {   cp2 =  strstr(InfoPtr->dzt_outfilename,".");
                    if (cp2!= NULL) *cp2 = 0;
                    strcat(InfoPtr->dzt_outfilename,".DZT");
                }
                strcpy(InfoPtr->inf_outfilename,InfoPtr->dzt_outfilename);
                cp2 =  strstr(InfoPtr->inf_outfilename,".");
                if (cp2!= NULL) *cp2 = 0;
                strcat(InfoPtr->inf_outfilename,".INF");

                if (Debug) printf("dzt_outfilename = %s\n",InfoPtr->dzt_outfilename);
                if (Debug) printf("inf_outfilename = %s\n",InfoPtr->inf_outfilename);
                break;
            case 5:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->channel = itemp;
                if (Debug) printf("channel = %d\n",InfoPtr->channel);
                break;
            case 6:
                InfoPtr->samp_first = atol(cp);
                if (Debug) printf("samp_first = %s\n",InfoPtr->samp_first);
                break;
            case 7:
                ltemp = atol(cp);
                if (ltemp >= 0)  InfoPtr->trace_first = ltemp;
                if (Debug) printf("trace_first = %s\n",InfoPtr->trace_first);
                break;
            case 8:
                ltemp = atol(cp);
                if (ltemp > 0)  InfoPtr->trace_last = ltemp;
                if (Debug) printf("trace_last = %s\n",InfoPtr->trace_last);
                break;
            case 9:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->pos_start = dtemp;
                if (Debug) printf("pos_start = %g\n",InfoPtr->pos_start);
                break;
            case 10:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->pos_step = dtemp;
                if (Debug) printf("pos_step = %g\n",InfoPtr->pos_step);
                break;
            case 11:
                dtemp = atof(cp);
                if ((dtemp >= 0.01) && (dtemp <= 0.30)) InfoPtr->vel_start = dtemp;
                if (Debug) printf("vel_start = %g\n",InfoPtr->vel_start);
                break;
            case 12:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->vel_step = dtemp;
                if (Debug) printf("vel_step = %g\n",InfoPtr->vel_step);
                break;
            case 13:
                ltemp = atol(cp);
                if (ltemp > 0) InfoPtr->vel_num = ltemp;
                if (Debug) printf("vel_num = %g\n",InfoPtr->vel_num);
                break;
            case 14:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->mute = dtemp; /* percentage */
                if (Debug) printf("mute = %g\n",InfoPtr->mute);
                break;
            case 15:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_start_trace = itemp;
                if (Debug) printf("rg_start_trace = %d\n",InfoPtr->rg_start_trace);
                break;
            case 16:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_start_samp = itemp;
                if (Debug) printf("rg_start_samp = %d\n",InfoPtr->rg_start_samp);
                break;
            case 17:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_stop_samp = itemp;
                if (Debug) printf("rg_stop_samp = %d\n",InfoPtr->rg_stop_samp);
                break;
            case 18:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_step = itemp;
                if (Debug) printf("rg_step = %d\n",InfoPtr->rg_step);
                break;
            case 19:
                itemp = atoi(cp);
                if (itemp == 2)  InfoPtr->rg_num_on = itemp;
                if (Debug) printf("rg_num_on = %d\n",InfoPtr->rg_num_on);
                break;
            case 20:
                itemp = atoi(cp);
                InfoPtr->rg_on[0] = itemp;
                if (Debug) printf("rg_on[0] = %d\n",InfoPtr->rg_on[0]);
                break;
            case 21:
                itemp = atoi(cp);
                InfoPtr->rg_on[1] = itemp;
                if (Debug) printf("rg_on[1] = %d\n",InfoPtr->rg_on[1]);
                break;
            default:
                break;
        }   /* end switch(found) block */
    }   /* end of while(1) block */
    fclose(infile);
    if (Debug)  Batch = FALSE;
    if (!dat_in_found) return 3;

    return 0;
}
/**************************** VelaDisplayParameters() ***************************/
/* Display parameters to CRT.
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, "jl_defs.h", "gpr_vela.h".
 * Calls: strupr, printf, puts, getch, kbhit, _ABS, strchr, sprintf.
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 11, 2001;
 */
void VelaDisplayParameters (struct VelaParamInfoStruct *InfoPtr)
{
    int i,lines,key;
    char str[10];
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    extern int Debug,Batch,ANSI_THERE,save_eps,save_pcx;

    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); /* red on black */
    printf("\nGPR_VELA version %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_VELA_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    lines = 3;
    printf("cmd_filename          = %s\n",strupr(InfoPtr->cmd_filename));
    lines++;
    printf("dzt_infilename        = %s\n",strupr(InfoPtr->dzt_infilename));
    lines++;
    printf("dzt_outfilename       = %s\n",strupr(InfoPtr->dzt_outfilename));
    lines++;
    printf("inf_outfilename       = %s\n",strupr(InfoPtr->inf_outfilename));
    lines++;
    printf("traces/channel = %ld  samples/trace = %ld  time/trace = %g (ns)\n",
        InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
    lines++;
    printf("ns_per_samp  = %g\n",InfoPtr->ns_per_samp);
    lines++;
    printf("channel     = %d    ant_freq   = %d MHz\n",
        InfoPtr->channel,InfoPtr->ant_freq);
    lines++;
    printf("samp_first = %ld\n",InfoPtr->samp_first);
    lines++;
    printf("trace_first = %ld  trace_last = %ld  trace_num = %ld\n",
        InfoPtr->trace_first,InfoPtr->trace_last,InfoPtr->trace_num);
    lines++;
    printf("pos_start = %g  pos_final = %g  pos_step = %g\n",
            InfoPtr->pos_start,InfoPtr->pos_final,InfoPtr->pos_step);
    lines++;
    printf("vel_start = %g  vel_final= %g  vel_step = %g  vel_num = %d\n",
        InfoPtr->vel_start,InfoPtr->vel_last,
        InfoPtr->vel_step,InfoPtr->vel_num);
    lines ++;
    printf("mute = %g percent  num_nmos = %d\n",
        InfoPtr->mute,InfoPtr->num_nmos);
    lines ++;
    printf("Range Gain: num_on = %d [%g][%g] dB\n",
        InfoPtr->rg_num_on,InfoPtr->rg_on[0],InfoPtr->rg_on[1]);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("Grid info: num_grids=%d num_traces=%d num_samps=%d size=%d bytes\n",
        InfoPtr->num_grids,InfoPtr->num_traces,InfoPtr->num_samps,
        InfoPtr->num_grids*InfoPtr->num_traces*InfoPtr->num_samps*2);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    /*  Display warning */
    if (Debug)       puts("In TESTing mode.  No graphics");
    else if (Batch)  puts("In Batch processing mode.");
}

/***************************** GetDataAsGrid16u() ****************************/
/* Get the data/info from the appropriate files and store as unsigned 16-bit
 * data col by col (ie. as a "grid").
 *
 * This function allocates storage for InfoPtr->grid[][].
 *
 * NOTE: The "created" member of struct VelaParamInfoStruct restricts the
 *       application of the struct to one data set and one grid at a time.
 *       To read multiple data sets and allocate multiple grids, either
 *       de-allocate the first one OR declare multiple structs to hold the
 *       information and pointers.
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>,
 *           "gpr_vela.h", "gpg_io.h".
 * Calls: malloc, free, fprintf, GetDztChSubGrid16
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *GetDataAsGrid16uMsg[] =
{   "GetDataAsGrid16u(): No errors.",
    "GetDataAsGrid16u() ERROR: Not able to allocate storage for GPR grid[][][].",
    "GetDataAsGrid16u() ERROR: Problem getting data from input file.",
    "GetDataAsGrid16u() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetDataAsGrid16u() ERROR: Attemp made to re-allocate storage.  De-allocate first.",

    "GetDataAsGrid16u() ERROR: Unsupported data storage format.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 10, 2001
 */
int GetDataAsGrid16u (int command,struct VelaParamInfoStruct *InfoPtr)
{
    int  i,j,itemp;       /* scratch variables and counters */
    long num,trace,samp;  /* scratch variables and counters */
    long grid_trace_size; /* number of bytes in one column of the grid */
    extern FILE *log_file;

    /* for GSSI SIR-10 files (see dzt_pgms.h) */
    extern const char *GetDztChSubGrid16Msg[];

/***** Deallocate storage *****/
    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->grid_created) return 3;
        if (InfoPtr->grid)
        {   for (i=0; i<InfoPtr->num_grids; i++)
            {   for (j=0; j<InfoPtr->num_traces; j++)  free(InfoPtr->grid[i][j]);
                free(InfoPtr->grid[i]);
            }
            free(InfoPtr->grid);
            InfoPtr->grid = NULL;
        }
        InfoPtr->grid_created = FALSE;
        return 0;
    }

/***** Test for balanced calls *****/
    if (InfoPtr->grid_created) return 4;
    InfoPtr->grid_created = FALSE;

/***** Allocate storage for grid[][][] *****/
    if (InfoPtr->grid_created) return 3;
    grid_trace_size = InfoPtr->num_samps * sizeof(unsigned short);
    InfoPtr->grid = (unsigned short ***)malloc((size_t)InfoPtr->num_grids * sizeof(unsigned short **));
    if (InfoPtr->grid)
    {   for (num=0; num<InfoPtr->num_grids; num++)
        {   InfoPtr->grid[num] = (unsigned short **)malloc((size_t)InfoPtr->num_traces * sizeof(unsigned short *));
            if (InfoPtr->grid[num] == NULL)
            {   for (trace=0; trace<num; trace++) free(InfoPtr->grid[trace]);
                InfoPtr->grid_created = FALSE;
                return 1;
            } else
            {   for (trace=0; trace<InfoPtr->num_traces; trace++)
                {   InfoPtr->grid[num][trace] = (unsigned short *)malloc((size_t)grid_trace_size);
                    if (InfoPtr->grid[num][trace] == NULL)
                    {   for (samp=0; samp<trace; samp++) free(InfoPtr->grid[num][samp]);
                        InfoPtr->grid_created = FALSE;
                        return 1;
                    }
                }
            }
        }
    } else
    {   InfoPtr->grid_created = FALSE;
        return 1;
    }
    InfoPtr->grid_created = TRUE;

/***** Initialize grid[][][] *****/
	for (num=0; num<InfoPtr->num_grids; num++)
	{   for (trace=0; trace<InfoPtr->num_traces; trace++)
        {   for (samp=0; samp<InfoPtr->num_samps; samp++)
            {   InfoPtr->grid[num][trace][samp] = 32768U;
            }
		}
    }

/***** Get the data and store as 16-bit unsigned GPR grid *****/
    itemp = 0;
    switch (InfoPtr->input_format)
    {   case DZT:  case MOD_DZT:
            itemp = GetDztChSubGrid16(InfoPtr->dzt_infilename,InfoPtr->channel,
                        (long)InfoPtr->trace_first,(long)InfoPtr->num_traces,
                        (long)InfoPtr->num_samps,InfoPtr->grid[0]);
            if (itemp > 0)
            {   printf("\n%s\n",GetDztChSubGrid16Msg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetDztChSubGrid16Msg[itemp]);
                itemp = 2;
            }
            break;
        default:      /* unsupported storage format */
            itemp = 5;
            break;
    }

    if (itemp > 0)
    {   if (!InfoPtr->grid_created) return 3;
        if (InfoPtr->grid) /***** Deallocate storage *****/
        {   for (i=0; i<InfoPtr->num_grids; i++)
            {   for (j=0; j<InfoPtr->num_traces; j++)  free(InfoPtr->grid[i][j]);
                free(InfoPtr->grid[i]);
            }
            free(InfoPtr->grid);
            InfoPtr->grid = NULL;
        }
        InfoPtr->grid_created = FALSE;
        return itemp;
    }


    /* Copy CMP from grid[0] to grid[1] */
	for (trace=0; trace<InfoPtr->num_traces; trace++)
    {   for (samp=0; samp<InfoPtr->num_samps; samp++)
        {   InfoPtr->grid[1][trace][samp] = InfoPtr->grid[0][trace][samp];
        }
	}

    return 0;
}

/**************************** VelaDeallocInfoStruct() ***************************/
/* Deallocate the buffer space in the information structure.  This function
 * will work correctly only if the num_... variables accurately reflect
 * the successful allocation of storage.
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_vela.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 10, 2001;
 */
void VelaDeallocInfoStruct (struct VelaParamInfoStruct *InfoPtr)
{
    int i,j;
    extern int Debug;

    if (InfoPtr->grid_created)
    {   if (InfoPtr->grid)
        {   for (i=0; i<InfoPtr->num_grids; i++)
            {   for (j=0; j<InfoPtr->num_traces; j++)  free(InfoPtr->grid[i][j]);
                free(InfoPtr->grid[i]);
            }
            free(InfoPtr->grid);
            InfoPtr->grid = NULL;
        }
        InfoPtr->grid_created = FALSE;
    }
    if (InfoPtr->vgrid_created)
    {   if (InfoPtr->vgrid)
        {   for (i=0; i<InfoPtr->num_nmos; i++)  free(InfoPtr->vgrid[i]);
            free(InfoPtr->vgrid);
            InfoPtr->vgrid = NULL;
        }
        InfoPtr->vgrid_created = FALSE;
    }

}
/************************** VelaSaveGprData() *******************************/
/* This is the driver routine for saving the GPR data to disk.
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of allocated structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <time.h>, "gpr_vela.h".
 * Calls: printf, fprintf, sprintf, strcat, _dos_gettime, _dos_getdate, strlen,
 *        VelaSaveDztData.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *VelaSaveGprDataMsg[] =
{   "VelaSaveGprData(): No errors.",
    "VelaSaveGprData() ERROR: Invalid file storage format.",
    "VelaSaveGprData() ERROR: Data/information was not saved to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: April 10, 2001;
 */
int VelaSaveGprData (struct VelaParamInfoStruct *InfoPtr)
{
    char str[240];
    int itemp;
    int file_saved = FALSE;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","Jan","Feb","Mar","Apr","May","Jun",
                         "Jul","Aug","Sep","Oct",
                         "Nov","Dec" } ;
    extern FILE *log_file;                   /* beginning of this source */
    extern const char *VelaSaveDztDataMsg[]; /* beginning of this source */

    /* Fill processing history string */
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
    sprintf(InfoPtr->proc_hist,"\nGPR_VELA v%s (USGS-JL) on %s %d, %d at %d:%02d\n",
                        GPR_VELA_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute);
    sprintf(str," group=(CMP)+(CMP gained)+(%d NMOs)+(vel.spectrum)\n",InfoPtr->vel_num);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," %d traces in CMP & NMO; Tzero samp=%d\n",
            InfoPtr->num_traces,InfoPtr->samp_first);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," %d traces in vel.spect.\n",InfoPtr->vel_num);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," CMP=%s 1st trace=%d offset(m.):first=%g last=%g step=%g\n",
            InfoPtr->dzt_infilename,InfoPtr->trace_first,InfoPtr->pos_start,
            InfoPtr->pos_final,InfoPtr->pos_step);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," vel(m/ns): %g to %g in %g steps; mute=%g%%\n",
            InfoPtr->vel_start,InfoPtr->vel_last,InfoPtr->vel_step,InfoPtr->mute);
    strcat(InfoPtr->proc_hist,str);
    if (InfoPtr->rg_num_on == 2)
    {   sprintf(str," RG[%g][%g]: 1st-trace=%d 1st-samp=%d stopsamp=%d step=%d \n",
            InfoPtr->rg_on[0],InfoPtr->rg_on[1],InfoPtr->rg_start_trace,
            InfoPtr->rg_start_samp,InfoPtr->rg_stop_samp,InfoPtr->rg_step);
        strcat(InfoPtr->proc_hist,str);
    }

    switch (InfoPtr->input_format)
	{   case DZT:  case MOD_DZT:
        {   itemp = VelaSaveDztData(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",VelaSaveDztDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",VelaSaveDztDataMsg[itemp]);
            } else
                file_saved = TRUE;
            break;
        }
        default:
            return 1;

    }
    if (file_saved) return 0;
    else            return 2;
}

/******************************* VelaSaveDztData() ******************************/
/* This function saves the GPR data to disk using the DZT format.
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_vela.h".
 * Calls: SetDzt5xHeader, ReadOneDztHeader, SaveDztFile, _splitpath (non-ANSI),
 *        strcpy, strncpy, strcat, sprintf, puts, printf, strlen.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *VelaSaveDztDataMsg[] =
{   "VelaSaveDztData(): No errors.",
	"VelaSaveDztData() ERROR: Invalid datatype for output DZT file.",
    "VelaSaveDztData() ERROR: Problem getting input file header.",
    "VelaSaveDztData() ERROR: Unable to open output NMO file.",
    "VelaSaveDztData() ERROR: Problem saving header to disk.",
    "VelaSaveDztData() ERROR: Problem saving data to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 12, 2001;
 */
int VelaSaveDztData (struct VelaParamInfoStruct *InfoPtr)
{
    char name[12],antname[14], drive[_MAX_DRIVE], dir[_MAX_DIR], text[600];
    char fname[_MAX_FNAME], fext[_MAX_EXT];
    unsigned char proc_hist[600];  /* hopefully 600 is big enough! */
    int i,j,itemp,error,input_datatype;
    int max_proc_hist_size = 600;
    int num,trace,trace_bytes;
    int hdr_num,num_hdrs,header_size,rh_dtype;
    unsigned short rh_nchan,rg_breaks,rh_nproc;
    long num_traces;
    float rg_values[8];    /* this limit may change in future versions */
    struct DztHdrStruct hdr;              /* struct in gpr_io.h */
    struct DztDateStruct create_date;

    FILE *outfile;
    #if defined(_INTELC32_)
      char *buffer = NULL;
      size_t vbufsize = 65536;
    #endif

    extern FILE *log_file;                /* beginning of this source */

    switch (InfoPtr->input_datatype)
    {   case -1:  case -2:    break;
        default:              return 1;
    }

	/* Read input DZT header again */
	itemp = ReadOneDztHeader(InfoPtr->dzt_infilename,&num_hdrs,
							 &num_traces,InfoPtr->channel,&header_size,&hdr);
	if (itemp > 0)
	{   if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
		return 2;
	}

	/* Set/copy values for output header */
    input_datatype = -2;
	if (hdr.rh_dtype == 0 && input_datatype != -1)
	{   if (input_datatype == -2) rh_dtype = 0x0002;
		else if (input_datatype == -3) rh_dtype = 0x0004;
		else rh_dtype = 0x0000;
	}
	rg_breaks = *(unsigned short *) ((char *)&hdr + hdr.rh_rgain);
	for (i=0; i<rg_breaks && i<8; i++)
		rg_values[i] = *(float *) ((char *)&hdr + hdr.rh_rgain + 2 + 4*i);
	strncpy(text,(char *)&hdr + hdr.rh_text,sizeof(text)-1);
	if ( strlen(text) < (sizeof(text)-strlen(InfoPtr->proc_hist)-2) )
		strcat(text,InfoPtr->proc_hist);
	rh_nproc = hdr.rh_nproc;
	if (rh_nproc > max_proc_hist_size-1) rh_nproc = max_proc_hist_size-1;
	for (i=0; i<rh_nproc; i++)
		proc_hist[i] = *((char *)&hdr + hdr.rh_proc + i);
	proc_hist[i] = 0;  /* counts on i being past for-loop limits */
	hdr_num = 1;
	rh_nchan = 1;
	_splitpath(InfoPtr->dzt_outfilename,drive,dir,fname,fext);
	strcpy(name,fname);
	strncpy(antname,hdr.rh_antname,sizeof(antname)-1);
	antname[sizeof(antname)-1] = 0;
	create_date = hdr.rh_create;
	hdr.rh_spm = 0.0;
	hdr.rh_mpm = InfoPtr->pos_step;
	hdr.rh_sps = 0.0;
	/* Note: Do not send pointers from hdr through this function!
			 SetDzt5xHeader() clears structure arrays before writing to
			 them! Other members are OK as they are sent by value.
	 */
	SetDzt5xHeader(hdr_num,hdr.rh_nsamp,hdr.rh_bits,hdr.rh_zero,hdr.rh_sps,
				 hdr.rh_spm,hdr.rh_mpm,hdr.rh_position,hdr.rh_range,
				 hdr.rh_npass,rh_nchan,hdr.rh_epsr,hdr.rh_top,hdr.rh_depth,
				 rh_dtype,antname,name,text,rg_breaks,rg_values,rh_nproc,
				 proc_hist,&create_date,&hdr);

	/* Set "trace header" values (1st 2 samps of each trace) */
	for (i=0; i<InfoPtr->num_grids; i++)
	{   for (j=0; j<InfoPtr->num_traces; j++)
        {   InfoPtr->grid[i][j][0] = 0xFFFFU;
		    if (InfoPtr->grid[i][j][1] != 0x4D00U)
                InfoPtr->grid[i][j][1] = 0xE800U;
		}
    }

    /* Open output file */
    if ((outfile = fopen(InfoPtr->dzt_outfilename,"wb")) == NULL)
    {   return 3;
    }

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(buffer,vbufsize);
      if (buffer)  setvbuf(outfile,buffer,_IOFBF,vbufsize);
      else         setvbuf(outfile,NULL,_IOFBF,vbufsize);
    #endif

    /* Write header to disk */
    if (fwrite((void *)&hdr,sizeof(struct DztHdrStruct),(size_t)1,outfile) < 1)
    {   fclose(outfile);
        #if defined(_INTELC32_)
          realfree(buffer);
        #endif
        return 4;
    }

    /* Write original and grained data and all NMOs to disk */
    trace_bytes = InfoPtr->num_samps * 2; /* 2 bytes per samp */
    for (num=0; num<InfoPtr->num_grids; num++)
    {   for (trace=0; trace<InfoPtr->num_traces; trace++)
        {   if (fwrite(InfoPtr->grid[num][trace],(size_t)trace_bytes,(size_t)1,outfile) < 1)
            {   fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(buffer);
                #endif
                return 5;
            }
        }
    }

    /* Write velocity spectrum to disk */
    for (num=0; num<InfoPtr->num_nmos; num++)
    {   if (fwrite(InfoPtr->vgrid[num],(size_t)trace_bytes,(size_t)1,outfile) < 1)
        {   fclose(outfile);
            #if defined(_INTELC32_)
              realfree(buffer);
            #endif
            return 5;
        }
    }

    /* Close file, release buffer and return */
    fclose(outfile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    return 0;
}

/******************************* VelaPerformNMO() *****************************/
/* This function performs the normal moveout (NMO) correction to the radar data
 * stored in grid[0] for every velocity requested. Results are stored in
 * grid[2] through grid[vel_num+1]. The NMO time is calculated for every sample
 * using the equation Tnmo = sqrt(Tsamp^2 + X^2/vel^2)). Linear interpolation
 * is used to find the trace amplitude at that time and the values is assigned
 * to grid[vel+1][trace][samp]. NMO times before or after the trace are not
 * used and the initialization (median) value is used. Also, if the mute
 * exceeds the user-defined limit, the initialization value is used.
 *
 * Parameter list:
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <math.h>, "gpr_vela.h".
 * Calls: sqrt, ChangeRangeGain16u.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *VelaPerformNMOMsg[] =
{   "VelaPerformNMO(): No errors.",
	"VelaPerformNMO() ERROR: Divide by zero: ns_per_samp.",
	"VelaPerformNMO() ERROR: Divide by zero: V2.",
	"VelaPerformNMO() ERROR: Divide by zero: deltime".,
	"VelaPerformNMO() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: April 10, 2001;
 */
int VelaPerformNMO (struct VelaParamInfoStruct *InfoPtr)
{
    int i, vel, trace, samp;     /* counters */
    int samp1, samp2, test, itemp;
    double V, V2, X, X2, T, T2, X2byV2;
    double dtemp;
    double nmo_time, nmo_samp, nmo_amp;
    double amp1, amp2, time1, time2, deltime, delamp;

    if (InfoPtr->ns_per_samp == 0.0) return 1;

    /* Gain CMP stored in grid[1] if requested */
    if (InfoPtr->rg_num_on == 2)
    {   int start_samp, stop_samp;
        double *gain_off_func, *gain_on_func, rg_on[2];

        /* Allocate storage for and initialize gain functions */
        gain_off_func = (double *)malloc((size_t)InfoPtr->num_samps * sizeof(double));
        gain_on_func =  (double *)malloc((size_t)InfoPtr->num_samps * sizeof(double));
        if (gain_off_func == NULL || gain_on_func == NULL)
        {   free(gain_off_func); free(gain_on_func);
            return 4;
        }

        /* Initialize gain functions */
        for (i=0; i<InfoPtr->num_samps; i++)
            gain_off_func[i] = gain_on_func[i] = 0.0;

        /* Change gain */
        rg_on[0] = InfoPtr->rg_on[0];
        rg_on[1] = InfoPtr->rg_on[1];
        for (trace=InfoPtr->rg_start_trace; trace<InfoPtr->num_traces; trace++)
        {   start_samp = InfoPtr->rg_start_samp + (trace * InfoPtr->rg_step);
            stop_samp = InfoPtr->rg_stop_samp + (trace * InfoPtr->rg_step);
            if (start_samp >= InfoPtr->num_samps) break;  /* out of for loop */
            if (stop_samp  >= InfoPtr->num_samps)
            {   itemp = (InfoPtr->num_samps-1) - start_samp;
                dtemp = InfoPtr->rg_on[1] - InfoPtr->rg_on[0];
                rg_on[1] = InfoPtr->rg_on[0] + (dtemp * (itemp/(stop_samp-start_samp)));
                stop_samp = InfoPtr->num_samps - 1;
            }
            itemp = ChangeRangeGain16u(InfoPtr->num_samps,start_samp,stop_samp,
                        InfoPtr->rg_num_off, InfoPtr->rg_off, InfoPtr->rg_num_on,
                        rg_on, gain_off_func, gain_on_func,
                        InfoPtr->grid[1][trace]);
            if (itemp > 0)
            {   printf("\n%s\n",ChangeRangeGain16uMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",ChangeRangeGain16uMsg[itemp]);
                break;   /* out of for loop */
            }
        }
    }

    /* Perform NMO */
    for (vel=0; vel<InfoPtr->vel_num; vel ++)
    {   /* V (m/ns) = the velocity for NMO correction */
        V = InfoPtr->vel_start + (InfoPtr->vel_step * vel);
        V2 = V * V;
        if (V2 == 0.0) return 2;

        for (trace=0; trace<InfoPtr->num_traces; trace++)
        {   /* X (m) = offset between antennas */
            X = InfoPtr->pos_start + (InfoPtr->pos_step * trace);
            X2 = X * X;
            X2byV2 = X2 / V2;

            for (samp=InfoPtr->samp_first+1; samp<InfoPtr->num_samps; samp++)
            {   /* T (ns) = travel time (T0) at the sample to be assigned */
                T = (samp - InfoPtr->samp_first) * InfoPtr->ns_per_samp;
                T2 = T * T;

                /* nmo_time (ns) = the NMO time later for that offset (X) */
                nmo_time = sqrt(T2 + X2byV2);

                /* get the sample numbers on either side of nmo_time */
                nmo_samp = InfoPtr->samp_first + (nmo_time / InfoPtr->ns_per_samp);
                samp1 = (int)nmo_samp; /* samp number "above/before" target */
                samp2 = samp1 + 1;     /* samp number "below/after" target */

                /* check that we are still on the trace */
                if (samp1 < 0)
                    break;             /* out of for loop; use default value */
                if (samp2 > InfoPtr->num_samps)
                    break;             /* out of for loop; use default value */

                /* get the trace amplitudes on either side of nmo_time.
                   grid[1] is copy of input CMP that may or may not be gained.
                */
                amp1 = InfoPtr->grid[1][trace][samp1];
                amp2 = InfoPtr->grid[1][trace][samp2];
                delamp = amp2 - amp1;
                time1 = (samp1 - InfoPtr->samp_first) * InfoPtr->ns_per_samp;
                time2 = (samp2 - InfoPtr->samp_first) * InfoPtr->ns_per_samp;
                deltime = time2 - time1;
                if (deltime == 0.0) return 3;
                dtemp = (nmo_time-time1) / deltime;

                /* use linear interpolation to get the NMO amp */
                nmo_amp = amp1 + (delamp * dtemp);

                /* check to see if stretch exceeds mute value */
                if (T > 0)
                {   if ( ((nmo_time-T) / T) >= InfoPtr->mute )
                        break;             /* out of for loop; use default value */
                }

                /* assign the value; remember grid[0] and grid[1] are the CMP */
                InfoPtr->grid[vel+2][trace][samp] = _LIMIT(0.0,nmo_amp,65535.0);
            }

            /* check to see if all values "muted" to median */
            test = 1;
            for (samp=InfoPtr->samp_first+1; samp<InfoPtr->num_samps; samp++)
            {   /* remember grid[0] and grid[1] are the CMP */
                if (InfoPtr->grid[vel+2][trace][samp] != 32768)
                {   test = 0;
                    break;    /* out of for loop */
                }
            }
            if (test)                          /* 'M' = 0x4D */
                InfoPtr->grid[vel+2][trace][1] = 0x4D00U; /* mark trace as all muted */
        }
    }
    return 0;
}

/**************************** VelaCalcVelSpectrum() **************************/
/* This function generates the velocity spectrum.
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct VelaParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, <math.h>, "gpr_vela.h".
 * Calls: malloc, fabs.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *VelaCalcVelSpectrumMsg[] =
{   "VelaCalcVelSpectrum(): No errors.",
    "VelaCalcVelSpectrum() ERROR: Attemp made to de-allocate storage before allocating.",
    "VelaCalcVelSpectrum() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "VelaCalcVelSpectrum() ERROR: Not able to allocate storage for GPR grid[][][].",
    "VelaCalcVelSpectrum() ERROR: Not able to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 5, 2001;
 */
int VelaCalcVelSpectrum (int command,struct VelaParamInfoStruct *InfoPtr)
{
    int i, j, itemp, num, trace, samp, count;
    int firstsamp, sign, tracecount;
    int binsize, numbins, samp1, samp2;
    unsigned long ultemp;
    double *dtrace, *etrace, dtemp, dtemp2, Z_max, Z_min, Z;

/***** Deallocate storage *****/
    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->vgrid_created) return 1;
        if (InfoPtr->vgrid)
        {   for (i=0; i<InfoPtr->num_nmos; i++)  free(InfoPtr->vgrid[i]);
            free(InfoPtr->vgrid);
            InfoPtr->vgrid = NULL;
        }
        InfoPtr->vgrid_created = FALSE;
        return 0;
    }

/***** Test for balanced calls *****/
    if (InfoPtr->vgrid_created) return 2;
    InfoPtr->vgrid_created = FALSE;

    /* Allocate storage for the velocity vgrid[][] */
    InfoPtr->vgrid = (unsigned short **)malloc((size_t)InfoPtr->num_nmos * sizeof(unsigned short *));
    if (InfoPtr->vgrid)
    {   for (num=0; num<InfoPtr->num_nmos; num++)
        {   InfoPtr->vgrid[num] = (unsigned short *)malloc((size_t)InfoPtr->num_samps * sizeof(unsigned short));
            if (InfoPtr->vgrid[num] == NULL)
            {   for (i=0; i<num; i++) free(InfoPtr->vgrid[i]);
                free(InfoPtr->vgrid);
                InfoPtr->vgrid_created = FALSE;
                return 3;
            }
        }
    } else
    {   InfoPtr->vgrid_created = FALSE;
        return 3;
    }
    InfoPtr->vgrid_created = TRUE;

    /* initialize vgrid[][] */
    for (num=0; num<InfoPtr->num_nmos; num++)
    {   for (samp=0; samp<InfoPtr->num_samps; samp++)
        {   InfoPtr->vgrid[num][samp] = 0;
        }
    }

/***** Allocate storage for the stack trace *****/
    dtrace = (double *)malloc((size_t)InfoPtr->num_samps * sizeof(double));
    etrace = (double *)malloc((size_t)InfoPtr->num_samps * sizeof(double));
    if (dtrace == NULL || etrace == NULL)
    {   for (i=0; i<InfoPtr->num_nmos; i++)  free(InfoPtr->vgrid[i]);
        free(InfoPtr->vgrid);
        InfoPtr->vgrid = NULL;
        InfoPtr->vgrid_created = FALSE;
        return 4;
    }

/***** Stack the NMOs and copy to corresponding column in vgrid[][] *****/
    Z_max = -1000000.0;
    Z_min =  1000000.0;
    numbins =  102;  /* 255; 170; 102; */
    binsize =    5;  /*   2;  3;    5; */
    firstsamp = 0;
    if (InfoPtr->samp_first > 0) firstsamp = InfoPtr->samp_first;
    sign = -1;

    for (num=0; num<InfoPtr->num_nmos; num++)
    {   /* Initialize stack trace and counter */
        for (i=0; i<InfoPtr->num_samps; i++) dtrace[i] = 0.0;
        tracecount = 0;

        /* Stack the traces for this NMO.
           remember grid[0] and grid[1] are the CMP
        */
        for (trace=0; trace<InfoPtr->num_traces; trace++)
        {
            if (InfoPtr->grid[num+2][trace][1] == 0x4D00U)
                continue;   /* skip to end of for loop and repeat */
            for (i=0; i<numbins; i++)
            {   samp1 = binsize * i;
                samp2 = samp1 + binsize;
                count = 0;
                dtemp2 = 0.0;
                for (samp=samp1; samp<samp2; samp++)
                {   if (samp<0 || samp>=InfoPtr->num_samps) continue; /* skip */
                    dtemp2 += fabs((double)InfoPtr->grid[num+2][trace][samp] - 32768.0);
                    count++;
                }
                if (count > 0) dtemp2 /= count;
                for (samp=samp1; samp<samp2; samp++)
                {   if (samp<0 || samp>=InfoPtr->num_samps) continue; /* skip */
                    dtrace[samp] += dtemp2;
                }
            }
            tracecount++;
        }
        for (samp=0; samp<InfoPtr->num_samps; samp++)
        {   dtrace[samp] /= tracecount; /* InfoPtr->num_traces; */
        }

        /* Assign the stacked value to vgrid[][] */
        for (samp=firstsamp; samp<InfoPtr->num_samps; samp++)
        {   dtemp = dtrace[samp];
            InfoPtr->vgrid[num][samp] = _LIMIT(0.0,dtemp,65535.0);
            if (InfoPtr->vgrid[num][samp] < Z_min)
                Z_min = InfoPtr->vgrid[num][samp];
            if (InfoPtr->vgrid[num][samp] > Z_max)
                Z_max = InfoPtr->vgrid[num][samp];
        }
    }

/***** Expand dynamic range, change sign *****/
    dtemp = 65535.0 / (Z_max - Z_min);
    for (num=0; num<InfoPtr->num_nmos; num++)
    {   for (samp=0; samp<InfoPtr->num_samps; samp++)
        {   Z  = InfoPtr->vgrid[num][samp];
            Z -= Z_min;
            Z *= dtemp;
            if (sign >= 0)
            {   InfoPtr->vgrid[num][samp] = (unsigned short)(_LIMIT(0.0,Z,65535.0));
            } else
            {   InfoPtr->vgrid[num][samp] = (unsigned short)(65535.0-_LIMIT(0.0,Z,65535.0));
            }
        }
    }

    /* Dealloc temp storage and return */
    free(dtrace); free(etrace);
    return 0;
}
/**************************** ChangeRangeGain16u() ***************************/
/* This function changes the gain applied to radar traces.
 *
 * Parameters:
 *  int  num_samps    - number of samples in trace
 *  int  start_samp   - samp to start gain function at
 *  int  stop_samp    - samp to stop gain function at
 *  int  num_gain_off - size of gain_off[]; 0 or >=2
 *  double *gain_off  - set of gain to be removed in decibels
 *  int  num_gain_on  - size of gain_on[]; 0 or >=2
 *  double *gain_on   - set of gain to be added in decibels
 *  double *gain_off_func
 *  double *gain_on_func
 *  unsigned short* ustrace - [num_samps]; pointer to trace
 *
 * NOTE: All pointers in the parameter list must point to allocated storage
 *       areas.
 *
 * Requires: <math.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h".
 * Calls: malloc, free, pow, printf, puts.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
const char *ChangeRangeGain16uMsg[] =
{   "ChangeRangeGain16u(): No errors.",
    "ChangeRangeGain16u() ERROR: Number of samps is less than 2.",
    "ChangeRangeGain16u() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
    "ChangeRangeGain16u() ERROR: NULL pointer passed to function.",
    "ChangeRangeGain16u() ERROR: start_samp value is invalid.",
    "ChangeRangeGain16u() ERROR: stop_samp value is invalid.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Team   Lakewood, CO
 * Date: February 8, 2001
 */
int ChangeRangeGain16u (int num_samps,int start_samp,int stop_samp,
                        int num_gain_off,double *gain_off,
                        int num_gain_on,double *gain_on,
                        double *gain_off_func,double *gain_on_func,
                        unsigned short *ustrace)
{
    int i,gain1,gain2,first_pt,last_pt,samp;
    double dtemp,gain_off_inc,gain_on_inc;
    extern int Debug;

    /* Check function parameters */
    if (num_samps < 2)                         return 1;
    if (num_gain_off < 0 || num_gain_on < 0)   return 2;
    if (num_gain_off == 1 || num_gain_on == 1) return 2;
    if (ustrace == NULL)                       return 3;
    if (num_gain_off >=2 && gain_off == NULL)  return 3;
    if (num_gain_on >=2 && gain_on == NULL)    return 3;
    if (start_samp < 0 || start_samp >= stop_samp || start_samp >= num_samps)
        return 4;
    if (stop_samp < 0 || stop_samp <= start_samp || stop_samp >= num_samps)
        return 5;
    if (Debug)
    {   puts("ChangeRangeGain16u() Debug)");
        printf("num_samps=%ld num_gain_off=%d  num_gain_on=%d start_samp=%d stop_samp=%d\n",
                 num_samps,num_gain_off,num_gain_on,start_samp,stop_samp);
    }

    /* Calculate number of samples between break points */
    gain_off_inc = gain_on_inc = 0.0;
    if (num_gain_off >= 2)
        gain_off_inc = (double)(stop_samp - start_samp - 1) / (num_gain_off - 1);
    if (num_gain_on >= 2)
        gain_on_inc  = (double)(stop_samp - start_samp - 1) / (num_gain_on - 1);
    if (Debug)
    {   printf("gain_off_inc = %g  gain_on_inc = %g\n",gain_off_inc,gain_on_inc);
        getch();
    }

    /* Calculate gain functions (linearly iterpolate between break points) */
    if (gain_off_inc > 0.0)
    {   for (gain1=0; gain1<num_gain_off-1; gain1++)
        {   gain2    = gain1 + 1;
            first_pt = start_samp + (gain1 * gain_off_inc);  /* truncation expected */
            last_pt  = start_samp + (gain2 * gain_off_inc);  /* truncation expected */
            dtemp    = gain_off[gain2] - gain_off[gain1];
            for (i=first_pt; i<last_pt && i<num_samps; i++)
            {   gain_off_func[i] = gain_off[gain1] + ((dtemp*(i-first_pt)) / gain_off_inc);
            }
        }
        gain_off_func[start_samp] = gain_off[0];
        gain_off_func[stop_samp]  = gain_off[num_gain_off-1];
    }
    if (gain_on_inc > 0.0)
    {   for (gain1=0; gain1<num_gain_on-1; gain1++)
        {   gain2    = gain1 + 1;
            first_pt = start_samp + (gain1 * gain_on_inc);  /* truncation expected */
            last_pt  = start_samp + (gain2 * gain_on_inc);  /* truncation expected */
            dtemp    = gain_on[gain2] - gain_on[gain1];
            for (i=first_pt; i<last_pt && i<=num_samps; i++)
            {   gain_on_func[i] = gain_on[gain1] + ((dtemp*(i-first_pt)) / gain_on_inc);
            }
        }
        gain_on_func[start_samp] = gain_on[0];
        gain_on_func[stop_samp]  = gain_on[num_gain_on-1];
    }

    /* Change gain */
    for (samp=start_samp; samp<=stop_samp; samp++)
    {   dtemp  = ustrace[samp];
        dtemp -= 32768.0;
        dtemp *= pow(10.,(.05*(gain_on_func[samp]-gain_off_func[samp])));
        dtemp += 32768.0;
        dtemp  = _LIMIT(0.0,dtemp,65535.0);
        ustrace[samp] = dtemp;
    }

    return 0;
}

/*************************** VelaSaveInfo() ****************************/
/* Save the information to disk that shows where each set of traces is in
 * the output DZT file.
 *
 * Parameters:
 *  struct VelaParamInfoStruct *InfoPtr - adress of information structure
 *
 * Requires: <process.h>, <stdio.h>, <stdlib>, <string.h>, "gpr_vela.h",
 * Calls: strcpy, _chdrive, chdir fopen, fclose, fprintf, sprintf, strupr,
 *        malloc.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *VelaSaveInfoMsg[] =
{   "VelaSaveInfo(): No errors.",
    "VelaSaveInfo() ERROR: Unable to open output TXT file.",
    "SaveGprVelaData() ERROR: Problem saving TXT data.",
} ;
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: April 9, 2001;
 *
 */
int VelaSaveInfo(struct VelaParamInfoStruct *InfoPtr)
{
    int     num, trace1, trace2;
    double  vel;
    FILE   *outfile;
    struct  dosdate_t dosdate;
    struct  dostime_t dostime;
    char    *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;

    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    /* Open text file */
    outfile = fopen(InfoPtr->inf_outfilename,"wt");
    if (outfile == NULL)  return 1;

    /* Write information */
    fprintf(outfile,"GPR_VELA v. %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_VELA_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    fprintf(outfile,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n\n");
    fprintf(outfile,"cmd_filename      = %s\n",strupr(InfoPtr->cmd_filename));
    fprintf(outfile,"input CMP file    = %s\n",InfoPtr->dzt_infilename);
    fprintf(outfile,"dzt_outfilename   = %s\n",strupr(InfoPtr->dzt_outfilename));
    fprintf(outfile,"inf_outfilename   = %s\n",strupr(InfoPtr->inf_outfilename));

    fprintf(outfile,"\nInput CMP file information:\n");
    fprintf(outfile,"traces/channel = %ld\n",InfoPtr->total_traces);
    fprintf(outfile,"samples/trace  = %ld\n",InfoPtr->total_samps);
    fprintf(outfile,"time/trace     = %g (ns)\n",InfoPtr->total_time);
    fprintf(outfile,"ns_per_samp    = %g\n",InfoPtr->ns_per_samp);
    fprintf(outfile,"channel        = %d\n",InfoPtr->channel);
    fprintf(outfile,"ant_freq       = %d MHz\n",InfoPtr->ant_freq);

    fprintf(outfile,"\nVelocity analysis information:\n");
    fprintf(outfile,"samp_first (T0)= %ld\n",InfoPtr->samp_first);
    fprintf(outfile,"trace_first    = %ld\n",InfoPtr->trace_first);
    fprintf(outfile,"trace_last     = %ld\n",InfoPtr->trace_last);
    fprintf(outfile,"trace_num      = %ld\n",InfoPtr->trace_num);
    fprintf(outfile,"pos_start      = %g\n",InfoPtr->pos_start);
    fprintf(outfile,"pos_final      = %g\n",InfoPtr->pos_final);
    fprintf(outfile,"pos_step       = %g\n",InfoPtr->pos_step);
    fprintf(outfile,"vel_start      = %g\n",InfoPtr->vel_start);
    fprintf(outfile,"vel_final      = %g\n",InfoPtr->vel_last);
    fprintf(outfile,"vel_step       = %g\n",InfoPtr->vel_step);
    fprintf(outfile,"vel_num        = %d\n",InfoPtr->vel_num);
    fprintf(outfile,"mute           = %g percent\n",InfoPtr->mute);
    fprintf(outfile,"num_nmos       = %d\n",InfoPtr->num_nmos);
    fprintf(outfile,"\nRange Gain:\n");
    fprintf(outfile,"num_on         = %d [%g][%g] dB\n",
        InfoPtr->rg_num_on,InfoPtr->rg_on[0],InfoPtr->rg_on[1]);
    fprintf(outfile,"rg_start_trace = %d\n",InfoPtr->rg_start_trace);
    fprintf(outfile,"rg_start_samp  = %d\n",InfoPtr->rg_start_samp);
    fprintf(outfile,"rg_stop_samp   = %d\n",InfoPtr->rg_stop_samp);
    fprintf(outfile,"rg_step        = %d\n",InfoPtr->rg_step);

    fprintf(outfile,"\nData set trace location in output DZT file:\n");
    trace1 = 0;
    trace2 = InfoPtr->num_traces-1;
    fprintf(outfile,"Original CMP data:   first trace=%6d  last trace=%6d\n",
              trace1,trace2);
    trace1 = InfoPtr->num_traces;
    trace2 = (2*InfoPtr->num_traces)-1;
    fprintf(outfile,"Gained CMP data:     first trace=%6d  last trace=%6d\n",
              trace1,trace2);
    fprintf(outfile,"\nFor NMO files, good traces are 'marked'; Muted traces are not marked.\n");
    for (num=2; num<InfoPtr->num_grids; num++)
    {   trace1 = (num)*InfoPtr->num_traces;
        trace2 = ((num+1)*InfoPtr->num_traces) -1;
        vel = InfoPtr->vel_start + ((num-2)*InfoPtr->vel_step);
        fprintf(outfile,"NMO vel=%.4f m/ns: first trace=%6d  last trace=%6d\n",
              vel,trace1,trace2);
    }
    trace1 = InfoPtr->num_grids * InfoPtr->num_traces;
    trace2 = trace1 + InfoPtr->num_nmos - 1;
    fprintf(outfile,"\nVelocity spectrum:   first trace=%6d  last trace=%6d\n",
              trace1,trace2);

    /* Close file */
    fclose(outfile);

    return 0;
}
/***************************** End of GPR_VELA.C ******************************/

