/********************************* GPR_CMPG.C ********************************\
                               version 1.01.23.02
-----------------------------------------------------------------------------
INTRODUCTION

GPR_CMPG.EXE performs a CMP (common midpoint) gather of GPR traces along one
single radar line. Each GPR file contains one set of traces with a fixed
offset (common offset gathers). All GPR files for the single line must contain
the same number of traces. Once all the files are read and the data are
re-sorted into CMP gathers, a constant-velocity normal moveout (NMO)
correction is performed. A mute is applied. Then the gather is stacked. The
CMP stacked section (for the radar line) is saved to disk as the first "grid"
in a DZT radar file. The remaining "grids" will be the individual CMP gathers
after muting and then the individual CMP gathers after NMO correction.

The input to this program is a "CMD" file, an ASCII text file containing
keywords (or commands) which are discussed in a section below. There is no
graphic display of the data. To display the output data, use programs such as
GPR_DISP.EXE or FIELDVEW.EXE. If you need to select a subset of traces from a
file then use GPR_SAMP.EXE.

-----------------------------------------------------------------------------
GENERATING THE CMP STACKED SECTION

For a subsurface model that consists of constant-velocity, horizontal layers,
the travel-time curve (from the transmitting antenna to a layer and back to
the receiving antenna) as a function of offset (which is the distance between
antennas) is a hyperbola. The difference in travel time between zero offset
and some other offset is called the normal moveout (NMO). The NMO correction
is the subtraction of the appropriate NMO time from every sample in a trace.

A common midpoint (CMP) GPR record is required to perform the NMO correction.
A GPR CMP record is one in which the each trace in the file is from two
antennas that are positioned equally and oppositely from a central point at
uniformly increasing distances. When the correct NMO velocity is used, the
hyperbolic shape of a horizontal reflector in a CMP record turns into a flat
reflector. See Yilmaz (1987) for details.

The GPR files used as input for this program must be common offset files.
That means that the antennas have the same separation in each file. In
addition, the stations must be spaced the same (for example, 0.05 m between
each station in every file) and there must be the same number of stations in
each file. The antenna separation (the offset) must increase the same amount
from one file to the next, with the first file read in having the smallest
offset.

After all files are read in, the traces are sorted into CMP gathers, where
all traces have the same midpoint. An NMO correction for one velocity is
applied to all the gathers with a mute.

Here is the general algorithm used to perform the NMO correction.

- For the selected velocity (m/ns)
    - Square the velocity; [V2]
    - For every trace in a file
        - Calculate the offset (meters) of that trace; [X]
        - Square the offset and divide by the square of the velocity; [X2/V2]
        - For every sample in a trace
            - Calculate the travel time (ns) at the sample and square the
              value; T2
            - Calculate the NMO time, Tnmo = SQRT(T2 + X2/V2)
            - Get the amplitude values for the samples on either side of the
              NMO time
            - Linearly interpolate to the get the amplitude for the NMO time;
              AMPnmo
            - Change AMPnmo to the median value if > the mute limit =
              [(Tnmo-T) / T]
            - Assign the AMPnmo to the current sample
        - If every sample has been muted then give a special mark to trace

-----------------------------------------------------------------------------
GPR DATA STORAGE FORMATS

Only the GSSI SIR-10A, SIR-2, SIR-2000, or RADAN binary "DZT" format. is
supported at this time.
    NOTE: Only ONE channel will be used from multiple-channel DZT files.

-----------------------------------------------------------------------------
OPERATING SYSTEM LIMITATIONS

GPR_CMPG.EXE is a protected-mode MS-DOS program that executes within the DOS
operating system or within a DOS (or console) window in Microsoft Windows
3.x/95/98. It probably will not execute within a console window in Windows NT.
This program will force the computer CPU to work in "protected mode" so that all
memory can be utilized, not just the 1 MB that DOS normally has access to. If an
error message occurs when trying to run GPR_CMPG.EXE it may be because the
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
January 23, 2001   - First version completed as revision of GPR_VELA.
                   - Only DZT as input.
                   - No graphics.
                   - All output is in one DZT file.

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /zfloatsync gpr_cmpg.c gpr_io.lib jl_util1.lib
     To compile for 80486 executable :
       icc /F /xnovm /zfloatsync /zmod486 gpr_cmpg.c gpr_io.lib jl_util1.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions, speeds up program.
     gpr_io.lib   - Jeff Lucius's GPR I/O functions (DZT, DT1, SGY).
     jl_util1.lib - Jeff Lucius's utility functions.

     to remove assert() functions from code also add:
     /DNDEBUG

 To run: GPR_CMPG cmd_filename
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
 *   +-- CmpgGetParameters                        (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line and opening the GPR data/info files to get additional
 *   |     | information about the data sets.
 *   |     +-- CmpgPrintUsageMsg                  (con I/O)
 *   |     +-- CmpgInitParameters
 *   |     +-- CmpgGetCmdFileArgs      (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetGprFileType          (disk I/O)
 *   |     +-- ReadOneDztHeader        (disk I/O)
 *   |     +-- CmpgDeallocGrids
 *   +-- CmpgDisplayParameters                    (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- CmpgAllocGrids
 *   |       This function allocates storage for data and gathers
 *   +-- CmpgDeallocGrids
 *   |       This function deallocates storage for data and gathers
 *   +-- CmpgGetDataAsGrid16u          (disk I/O) (con I/O)
 *   |     +-- GetDztChSubGrid16       (disk I/O)
 *   +-- CmpgSortData
 *   |       This function re-orders the data into CMP gathers. vgrid[][][] is
 *   |       is initialized to 32768U.
 *   +-- CmpgPerformNMO
 *   |     | This function performs the NMOs on the grids in the first half of
 *   |     | vgrid[][][] and saves the results to second half vgrid[][][].
 *   |     +-- ChangeRangeGain16u
 *   +-- CmpgCalcCmpStackedSection
 *   |     | This function calculates the CMP stacked section.
 *   +-- CmpgSaveGprData                     (disk I/O)(con I/O)
 *   |     | This function sets the proc_hist and calls the data save function.
 *   |     +-- CmpgSaveDztData               (disk I/O)(con I/O)
 *   |     |     +-- ReadOneDztHeader        (disk I/O)
 *   |     |     +-- SetDzt5xHeader
 *   |     |     +-- SaveDztFile             (disk I/O)
 *   +-- CmpgSaveInfo
 *   +-- ANSI_there                                              (DOS)
 *   |     +-- GetDosVersion                                     (DOS)
 *   +-- Sound                                                        (port I/O)
 */
/******************* Includes all required header files *********************/
#include "gpr_cmpg.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_CMPG_CMDS[]=
{   "debug","batch","display_none","num_input_files","input_filelist[]",
    "dzt_outfilename","offset_first","offset_incr","pos_start","pos_step",
/* 10 */
    "velocity","channel","samp_first","mute","rg_start_trace",
    "rg_start_samp","rg_stop_samp","rg_step","rg_num_on","rg_on[0]",
/* 20 */
    "rg_on[1]",NULL,
} ;

/* The rest of these are message strings that match codes returned by functions */
const char *CmpgCmpgGetParametersMsg[] =
{   "CmpgGetParameters(): No errors.",
    "CmpgGetParameters() ERROR: Insufficient command line arguments.",
    "CmpgGetCmdFileArgs() ERROR: Unable to open input command file.",
    "CmpgGetCmdFileArgs() ERROR: Input file(s) not specified.",
    "CmpgGetParameters() ERROR: Problem getting information about input data file.",

    "CmpgGetParameters() ERROR: Storage format not GSSI DZT.",
    "CmpgGetParameters() ERROR: Problem getting data from GSSI DZT file.",
    "CmpgGetParameters() ERROR: Invalid channel selection for DZT file.",
    "CmpgGetParameters() ERROR: All files must have the same number of traces.",
    "CmpgGetParameters() ERROR: All files must have the same time window.",

    "CmpgGetParameters() ERROR: All files must have the same number of samples/trace.",
    "CmpgGetParameters() ERROR: GPR file storage format must be GSSI DZT.",
    "CmpgGetParameters() ERROR: Not used.",
    "CmpgGetParameters() ERROR: Value for ns_per_samp is invalid.",
    "CmpgGetParameters() ERROR: There must be at least 3 input files.",

    "CmpgGetParameters() ERROR: Velocities must be between 0.01 and 0.30 (m/ns).",
    "CmpgGetParameters() ERROR: Mute value must be greater than 0.",
    "CmpgGetParameters() ERROR: pos_step must not be 0.",
    "CmpgGetParameters() ERROR: offset_incr must not be 0.",
} ;
const char *CmpgAllocGridsMsg[] =
{   "CmpgAllocGrids(): No errors.",
    "CmpgAllocGrids() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "CmpgAllocGrids() ERROR: Unable to allocate storage for dgrid[][][].",
    "CmpgAllocGrids() ERROR: Unable to allocate storage for vgrid[][][].",
    "CmpgAllocGrids() ERROR: Unable to allocate storage for grid[][].",
} ;
const char *CmpgGetDataAsGrid16uMsg[] =
{   "CmpgGetDataAsGrid16u(): No errors.",
    "CmpgGetDataAsGrid16u() ERROR: dgrid[][][] is NULL.",
    "CmpgGetDataAsGrid16u() ERROR: Problem getting data from input file.",
    "CmpgGetDataAsGrid16u() ERROR: Unsupported data storage format.",
} ;
const char *CmpgSortDataMsg[] =
{   "CmpgSortData(): No errors.",
    "CmpgSortData() ERROR: vgrid[][][] is NULL.",
} ;
const char *CmpgPerformNMOMsg[] =
{   "CmpgPerformNMO(): No errors.",
	"CmpgPerformNMO() ERROR: Divide by zero: ns_per_samp",
	"CmpgPerformNMO() ERROR: Divide by zero: V2",
	"CmpgPerformNMO() ERROR: Divide by zero: deltime",
	"CmpgPerformNMO() ERROR: Unable to allocate temporary storage.",
} ;
const char *ChangeRangeGain16uMsg[] =
{   "ChangeRangeGain16u(): No errors.",
    "ChangeRangeGain16u() ERROR: Number of samps is less than 2.",
    "ChangeRangeGain16u() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
    "ChangeRangeGain16u() ERROR: NULL pointer passed to function.",
    "ChangeRangeGain16u() ERROR: start_samp value is invalid.",
    "ChangeRangeGain16u() ERROR: stop_samp value is invalid.",
} ;
const char *CmpgCalcCmpStackedSectionMsg[] =
{   "CmpgCalcCmpStackedSection(): No errors.",
    "CmpgCalcCmpStackedSection() ERROR: Not able to allocate temporary storage.",
} ;
const char *CmpgSaveGprDataMsg[] =
{   "CmpgSaveGprData(): No errors.",
    "CmpgSaveGprData() ERROR: Invalid file storage format.",
    "CmpgSaveGprData() ERROR: Data/information was not saved to disk.",
} ;
const char *CmpgSaveDztDataMsg[] =
{   "CmpgSaveDztData(): No errors.",
	"CmpgSaveDztData() ERROR: Invalid datatype for output DZT file.",
    "CmpgSaveDztData() ERROR: Problem getting input file header.",
    "CmpgSaveDztData() ERROR: Unable to open output NMO file.",
    "CmpgSaveDztData() ERROR: Problem saving header to disk.",
    "CmpgSaveDztData() ERROR: Problem saving data to disk.",
} ;
const char *CmpgSaveInfoMsg[] =
{   "CmpgSaveInfo(): No errors.",
    "CmpgSaveInfo() ERROR: Unable to open output TXT file.",
    "CmpgSaveInfo() ERROR: Problem saving TXT data.",
} ;

/********************************** main() **********************************/
void main(int argc, char *argv[])
{
/***** Declare variables *****/
    /* following are variables used by main */
    char   log_filename[80];
    int    itemp, i;          /* scratch variables */
    int    error;             /* flag indicating an error has occured */
    double dtemp;             /* scratch variable */

    /* following is the information structure passed between functions */
    struct CmpgParamInfoStruct ParamInfo;

    /* these variables found near beginning of this source */
    extern int         Debug,Batch,ANSI_THERE;
    extern FILE       *log_file;
    extern const char *CmpgCmpgGetParametersMsg[];
    extern const char *CmpgGetDataAsGrid16uMsg[];

/***** Initialize variables *****/

    /* open error message file */
    strcpy(log_filename,"GPR_CMPG.LOG");
    log_file = fopen(log_filename,"a+t");  /* open text file for appending */
    if (log_file == NULL)                  /* if doesn't work try root on C: */
    {   strcpy(log_filename,"C:\\GPR_CMPG.LOG");
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
        fprintf(log_file,"Messages from program GPR_CMPG v. %s: %s %d, %d at %d:%02d:%02d\n",
                        GPR_CMPG_VER,month[dosdate.month],dosdate.day,dosdate.year,
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
    itemp =  CmpgGetParameters(argc,argv,&ParamInfo);
    printf("\r                                                              \r");
    if (itemp > 0)
    {   printf("\n%s\n",CmpgCmpgGetParametersMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",CmpgCmpgGetParametersMsg[itemp]);
        if (log_file) fclose(log_file);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("\nGPR_CMPG finished.\nFile %s on disk contains messages.\n",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(1);
    }
    for (i=0; i<ParamInfo.num_input_files; i++)
    {  if ( (strcmp(ParamInfo.dat_infilename[i],ParamInfo.dzt_outfilename) == 0) )
        {   printf("\nERROR: Output filename is duplicated in input list.\n");
            if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
            if (log_file)
            {   fprintf(log_file,"ERROR: Output filename is duplicated in input list.\n");
                fclose(log_file);
                printf("\nFile %s on disk contains messages.\n",log_filename);
            }
            puts("GPR_CMPG finished.");
            if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
            exit(2);
        }
    }
    if (log_file)
    {   if (Batch) fprintf(log_file,"Program in Batch mode.\n");
        if (Debug) fprintf(log_file,"Program in Debug mode.\n");
    }

/***** Display parameters and ask if user wants to continue *****/
    if (ParamInfo.display_none == TRUE)
    {   ; /* no parameter display */
    } else
    {   CmpgDisplayParameters(&ParamInfo);
        if (!Batch)
        {   printf("Press <Esc> to quit or other key to continue ... ");
            if ((itemp = getch()) == ESC)
            {   CmpgDeallocGrids(&ParamInfo);
                puts("\nProgram terminated by user.\n");
                if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                if (log_file)
                {   fprintf(log_file,"End of messages.  Program terminated by user.\n");
                    fclose(log_file);
                    printf("File %s on disk contains messages.\n",log_filename);
                }
                puts("GPR_CMPG finished.");
                if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                exit(3);
            } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
                getch();
            printf("\r                                                           \r");
        }
    }
/***** Allocate storage *****/
    printf("Allocating storage ...");
    itemp = CmpgAllocGrids(&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   CmpgDeallocGrids(&ParamInfo);
        printf("\n%s\n",CmpgAllocGridsMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",CmpgAllocGridsMsg[itemp]);
        if (itemp > 1)  /* data set too large for memory */
        {   unsigned long mem,bytes,requested;

            mem = _memavl();
            mem /= (1024*1024);
            requested = (ParamInfo.num_vgrids*ParamInfo.num_files*ParamInfo.num_samps*2)/(1024*1024);
            requested += (ParamInfo.num_traces*ParamInfo.num_samps*2)/(1024*1024);
            requested += (ParamInfo.num_files*ParamInfo.num_traces*ParamInfo.num_samps*2)/(1024*1024);
            printf("\nSize of required storage (%lu MB) exceeds available memory (%lu MB).\n",
                                  requested,mem);
            if (log_file)
                fprintf(log_file,"Size of required storage (%lu MB) exceeds available memory (%lu MB).\n",
                        requested,mem);
        }
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_CMPG finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(4);
    }

/***** Get the data *****/
    printf("Getting Data ...");
    itemp = CmpgGetDataAsGrid16u(&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   CmpgDeallocGrids(&ParamInfo);
        printf("\n%s\n",CmpgGetDataAsGrid16uMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",CmpgGetDataAsGrid16uMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_CMPG finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(5);
    }

/***** Re-order data into CMP gathers *****/
    printf("Sorting into CMP gathers ...");
    itemp = CmpgSortData(&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   CmpgDeallocGrids(&ParamInfo);
        printf("\n%s\n",CmpgSortDataMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",CmpgSortDataMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_CMPG finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(6);
    }

/***** Perform NMO corrections *****/
    printf("Performing NMOs ...");
    itemp = CmpgPerformNMO(&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   CmpgDeallocGrids(&ParamInfo);
        printf("\n%s\n",CmpgPerformNMOMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",CmpgPerformNMOMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_CMPG finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(7);
    }

/***** CMP stacked section *****/
    printf("Calculating CMP stacked section ...");
    itemp = CmpgCalcCmpStackedSection(&ParamInfo);
    printf("\r                                                               \r");
    if (itemp > 0)
    {   CmpgDeallocGrids(&ParamInfo);
        printf("\n%s\n",CmpgCalcCmpStackedSectionMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",CmpgCalcCmpStackedSectionMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_CMPG finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(6);
    }

/***** Save input data and NMOs to disk together *****/
	printf("Saving CMP stacked section and all gathers to disk ...");
	itemp = CmpgSaveGprData(&ParamInfo);
	printf("\r                                                                      \r");
	if (itemp > 0)
	{   printf("\n%s\n",CmpgSaveGprDataMsg[itemp]);
		/* CmpgGetDataAsGrid16u(FUNC_DESTROY,&ParamInfo); */
		CmpgDeallocGrids(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		if (log_file)
		{   fprintf(log_file,"%s\n",CmpgSaveGprDataMsg[itemp]);
			fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("\nGPR_CMPG finished.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		if (!Batch)
		{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
				Sound(dtemp,0.055);
		}
		exit(7);
	}

    itemp = CmpgSaveInfo(&ParamInfo);
	if (itemp > 0)
	{   printf("\n%s\n",CmpgSaveInfoMsg[itemp]);
		/* CmpgGetDataAsGrid16u(FUNC_DESTROY,&ParamInfo); */
		CmpgDeallocGrids(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		puts("\nWARNING: GPR_CMPG did not write the information file to disk.");
		if (log_file)
		{   fprintf(log_file,"%s\n",CmpgSaveInfoMsg[itemp]);
			fprintf(log_file,"WARNING: GPR_CMPG did not write the information file to disk.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
	}

    CmpgDeallocGrids(&ParamInfo);

    if (log_file)
    {   fprintf(log_file,"End of messages.  Program terminated normally.\n");
        fclose(log_file);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("GPR_CMPG finished.\nFile %s on disk contains messages.",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
    }
    exit(0);
}
/****************************** CmpgGetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 * Determine GPR file storage format.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <math.h>, <stdio.h>, <stdlib.h>, <string.h>,
 *           "gpr_cmpg.h".
 * Calls: printf, CmpgPrintUsageMsg, CmpgInitParameters, CmpgGetCmdFileArgs.
 *        strcpy, puts, strstr, atoi, atof, strchr, strcat, memcpy, strupr.
 *        ReadOneDztHeader, CmpgDeallocGrids,
 *
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *CmpgCmpgGetParametersMsg[] =
{   "CmpgGetParameters(): No errors.",
    "CmpgGetParameters() ERROR: Insufficient command line arguments.",
    "CmpgGetCmdFileArgs() ERROR: Unable to open input command file.",
    "CmpgGetCmdFileArgs() ERROR: Input file(s) not specified.",
    "CmpgGetParameters() ERROR: Problem getting information about input data file.",

    "CmpgGetParameters() ERROR: Storage format not GSSI DZT.",
    "CmpgGetParameters() ERROR: Problem getting data from GSSI DZT file.",
    "CmpgGetParameters() ERROR: Invalid channel selection for DZT file.",
    "CmpgGetParameters() ERROR: All files must have the same number of traces.",
    "CmpgGetParameters() ERROR: All files must have the same time window.",

    "CmpgGetParameters() ERROR: All files must have the same number of samples/trace.",
    "CmpgGetParameters() ERROR: GPR file storage format must be GSSI DZT.",
    "CmpgGetParameters() ERROR: Not used.",
    "CmpgGetParameters() ERROR: Value for ns_per_samp is invalid.",
    "CmpgGetParameters() ERROR: There must be at least 3 input files.",

    "CmpgGetParameters() ERROR: Velocities must be between 0.01 and 0.30 (m/ns).",
    "CmpgGetParameters() ERROR: Mute value must be greater than 0.",
    "CmpgGetParameters() ERROR: pos_step must not be 0.",
    "CmpgGetParameters() ERROR: offset_incr must not be 0.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 7, 2002;
 */
int CmpgGetParameters (int argc, char *argv[],struct CmpgParamInfoStruct *InfoPtr)
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
    extern const char *GetGprFileTypeMsg[];

/***** Verify usage *****/
    if (argc < req_args)
    {   CmpgPrintUsageMsg();
        return 1;
    }

/***** Initialize information structure *****/
    CmpgInitParameters(InfoPtr);
    strcpy(InfoPtr->cmd_filename,argv[1]);

/***** Get user-defined parameters from CMD file *****/
    itemp = CmpgGetCmdFileArgs(InfoPtr);
    if (itemp) return itemp;   /* 2 to 3 */

/***** Determine GPR file storage format and essential info *****/
    if (InfoPtr->storage_format == 0)
    {   itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                    &samples_per_trace,&num_channels,&total_traces,
                    &input_datatype,&total_time,&file_type,
                    InfoPtr->dat_infilename[0]);
        if (itemp > 0)
        {   printf("%s\n",GetGprFileTypeMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
            return 4;
        }
        switch (file_type)               /* only DZT files accepted */
        {   case DZT:  case MOD_DZT:
               break;
            default:
                return 5;
        }
        /* set program values from first file */
        InfoPtr->storage_format     = file_type;
        InfoPtr->file_header_bytes  = file_hdr_bytes;
        InfoPtr->trace_header_bytes = trace_hdr_bytes;
        InfoPtr->total_traces       = total_traces;
        InfoPtr->total_samps        = samples_per_trace;
        InfoPtr->total_time         = total_time;
        InfoPtr->ns_per_samp        = (double)InfoPtr->total_time / (InfoPtr->total_samps - 1);
        InfoPtr->input_datatype     = input_datatype;

        for (i=1; i<InfoPtr->num_input_files; i++)
        {   itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                    &samples_per_trace,&num_channels,&total_traces,
                    &input_datatype,&total_time,&file_type,
                    InfoPtr->dat_infilename[i]);
            if (itemp > 0)
            {   printf("%s\n",GetGprFileTypeMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
                return 4;
            }
            switch (file_type)           /* only DZT files accepted */
            {   case DZT:  case MOD_DZT:
                   break;
                default:
                    return 5;
            }
            /* verify all files the same size */
            if (InfoPtr->total_traces != total_traces)
                return 8;
            if (InfoPtr->total_time != total_time)
                return 9;
            if (InfoPtr->total_samps != samples_per_trace)
                return 10;
        }
    }

/***** Get necessary information from info file/header *****/
    if (InfoPtr->storage_format == DZT || InfoPtr->storage_format == MOD_DZT)
    {   int num_hdrs,channel;
        struct DztHdrStruct DztHdr;                /* from gpr_io.h */
        extern const char *ReadOneDztHeaderMsg[];  /* from gpr_io.h */
        extern const char *ant_number[];           /* from gpr_io.h */
        extern const char *ant_name[];             /* from gpr_io.h */

        /* get first DZT header in file */
        channel = 0;
        itemp = ReadOneDztHeader(InfoPtr->dat_infilename[0],&num_hdrs,
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
        {   itemp = ReadOneDztHeader(InfoPtr->dat_infilename[0],&num_hdrs,
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
            printf("num_hdrs = %d\n",num_hdrs);
            getch();
        }
    } else /* if (InfoPtr->storage_format == DZT) */
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
    {   CmpgDeallocGrids(InfoPtr);
        return 13;
    }

    /* calculate pos_final */
    if (InfoPtr->pos_step==0.0) return 17;
    InfoPtr->pos_final = InfoPtr->pos_start + (InfoPtr->trace_last - InfoPtr->trace_first)* InfoPtr->pos_step;

    /* calculate grid sizes info */
    if (InfoPtr->num_input_files < 3) return 14;
    InfoPtr->num_traces = InfoPtr->trace_num;
    InfoPtr->num_samps  = InfoPtr->total_samps;
    InfoPtr->num_vgrids = 2 * InfoPtr->num_traces;
    InfoPtr->num_files  = InfoPtr->num_input_files;

    /* calculate offset_final */
    if (InfoPtr->offset_incr==0.0) return 18;
    InfoPtr->offset_final = InfoPtr->offset_first + (InfoPtr->num_files - 1)* InfoPtr->offset_incr;

    /* Be sure velocities are in range */
    if ((InfoPtr->velocity < 0.01) || (InfoPtr->velocity > 0.30) )
        return 15;

    /* check mute */
    if (InfoPtr->mute <=0.0) return 16;

    return 0;
}
/****************************** CmpgPrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>, <dos.h>.
 * Calls: puts, sprintf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: December 20, 2001;
 */
void CmpgPrintUsageMsg (void)
{
    char str[10];
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("\n###############################################################################");
    puts("  Usage: GPR_CMPG cmd_filename");
    puts("    Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.");
    puts("  This program constructs a stacked CMP section from a series of DZT-formatted");
    puts("    GPR data files. Each file must have the same number of traces. The first");
    puts("    file has the smallest antenna seperation. Each successive file has a larger");
    puts("    antenna seperation (uniform increment). A single velocity is used for the");
    puts("    NMO correction. A mute can be applied. Results are stored to disk in a");
    puts("    single DZT file. A text INF file is also written.");
    puts("  Required command line arguments:");
    puts("    cmd_filename - name of text file that follows the \"CMD\" format.");
    puts("  Look at \"GPR_CMPG.CMD\" and \"GPR_CMPG.TXT\" for keyword file format, valid");
    puts("    keywords, and documentation.");
    puts("  Example call: GPR_CMPG vfile1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_CMPG_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** CmpgInitParameters() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "gpr_cmpg.h".
 * Calls:
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: December 20, 2001;
 */
void CmpgInitParameters (struct CmpgParamInfoStruct *InfoPtr)
{
    int i;

    extern int Debug;

    InfoPtr->cmd_filename[0]         = 0;
    InfoPtr->num_input_files         = 0;
    InfoPtr->file_num                = 0;
    InfoPtr->dat_infilename          = NULL;
    InfoPtr->dzt_outfilename[0]      = 0;
    InfoPtr->inf_outfilename[0]      = 0;
    InfoPtr->storage_format          = 0;
    InfoPtr->input_datatype          = 0;
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
    InfoPtr->offset_first            = -1.0;
    InfoPtr->offset_incr             = -1.0;
    InfoPtr->offset_final            = -1.0;
    InfoPtr->pos_start               = 0.0;
    InfoPtr->pos_step                = 0.0;
    InfoPtr->pos_final               = 0.0;
    InfoPtr->trace_first             = 0;
    InfoPtr->trace_last              = 0;
    InfoPtr->trace_num               = 0;
    for (i=0; i<sizeof(InfoPtr->proc_hist); i++) InfoPtr->proc_hist[i] = 0;
    InfoPtr->velocity                = 0.0;
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
    InfoPtr->num_traces              = 0;
    InfoPtr->num_samps               = 0;
    InfoPtr->vgrid_created           = FALSE;
    InfoPtr->vgrid                   = NULL;
    InfoPtr->num_vgrids              = 0;
    InfoPtr->num_files               = 0;
    InfoPtr->dgrid_created           = FALSE;
    InfoPtr->dgrid                   = NULL;
}
/***************************** CmpgGetCmdFileArgs() *****************************/
/* Get processing parameters from user from a command file.
 *
const char *GPR_CMPG_CMDS[]=
{   "debug","batch","display_none","num_input_files","input_filelist[]",
    "dzt_outfilename","offset_first","offset_incr","pos_start","pos_step",
 10
    "velocity","channel","samp_first","mute","rg_start_trace",
    "rg_start_samp","rg_stop_samp","rg_step","rg_num_on","rg_on[0]",
 20
    "rg_on[1]",NULL,
} ;
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <stdio.h>, <stdlib.h>, <string.h>, "gpr_cmpg.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strcat,
 *        strcmp, strupr, strlwr, strncpy, atol, malloc, free,
 *        Trimstr, strlen, getch.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 8, 2002;
 */
int CmpgGetCmdFileArgs (struct CmpgParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];
    int found,i,j,num,itemp,have_string;
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
        while (GPR_CMPG_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_CMPG_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_CMPG_CMDS[] */
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
        switch (found)       /* cases depend on same order in GPR_CMPG_CMD[] */
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
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->num_input_files = itemp;
                if (Debug) printf("num_input_files=%d\n",InfoPtr->num_input_files);
                break;
            case 4:                         /* input_filelist[] */
            {   char *cp_next = (char *)TrimStr(cp);
                int file_found = 0;

                /* number of files must be specified first */
                if (InfoPtr->num_input_files <= 0) break;

                /* allocate storage for filenames */
                InfoPtr->dat_infilename = (char **)malloc(InfoPtr->num_input_files * sizeof(char *));
                if (InfoPtr->dat_infilename == NULL)
                {   InfoPtr->num_input_files = -2;
                    if (Debug) puts("\nmalloc failed for dat_infilename");
                    if (log_file) fprintf(log_file,"malloc failed for dat_infilename\n");
                    break;      /* break out of case */
                }
                for (i=0; i<InfoPtr->num_input_files; i++)
                {   InfoPtr->dat_infilename[i] = (char *)malloc(MAX_PATHLEN * sizeof(char));
                    if (InfoPtr->dat_infilename[i] == NULL)
                    {   for (j=0; j<i; j++) free(InfoPtr->dat_infilename[j]);
                        free(InfoPtr->dat_infilename);
                        InfoPtr->dat_infilename = NULL;
                        InfoPtr->num_input_files = -2;
                        if (Debug) puts("\nmalloc failed for dat_infilename[]");
                        if (log_file) fprintf(log_file,"malloc failed for dat_infilename[]\n");
                        break;  /* out of for loop */
                    }
                }

                if (InfoPtr->num_input_files <= 0) break; /* break out of case */

                /* get the file names */
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (file_found < InfoPtr->num_input_files)
                        {   cp2 = strchr(cp_next,' '); /* look for seperator */
                            if (cp2 != NULL)
                            {   *cp2 = 0;    /* truncate */
                                strcpy(InfoPtr->dat_infilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,InfoPtr->dat_infilename[file_found]);
                                file_found++;
                                *cp2 = ' ';  /* reset separator */
                                /* TrimStr(cp_next); */
                            } else
                            {   strcpy(InfoPtr->dat_infilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,InfoPtr->dat_infilename[file_found]);
                                file_found++;
                            }
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (file_found < InfoPtr->num_input_files)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {
                            break;      /* break out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            if (strchr(str,'='))
                            {   have_string = TRUE;
                                break;  /* break out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = (char *)TrimStr(str);
                            }
                        }
                    } else
                        break;      /* break out of while (1) */
                }
                dat_in_found = TRUE;
                if (file_found < InfoPtr->num_input_files)
                {   if (Debug) puts("\nfile_found < num_input_files");
                    if (log_file) fprintf(log_file,"GetCmdFileArgs(): files found < num_input_files\n");
                    InfoPtr->num_input_files = -3;
                    for (j=0; j<InfoPtr->num_input_files; j++)
                        free(InfoPtr->dat_infilename[j]);
                    free(InfoPtr->dat_infilename);
                    InfoPtr->dat_infilename = NULL;
                    InfoPtr->num_input_files = -4;
                    dat_in_found = FALSE;
                }
                if (Debug)
                {    for (i=0; i<InfoPtr->num_input_files; i++)
                        printf("File %3d: %s\n",i,InfoPtr->dat_infilename[i]);
                }
                break;
            }
            case 5:
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
            case 6:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->offset_first = dtemp;
                if (Debug) printf("offset_first = %g\n",InfoPtr->offset_first);
                break;
            case 7:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->offset_incr = dtemp;
                if (Debug) printf("offset_incr = %g\n",InfoPtr->offset_incr);
                break;
            case 8:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->pos_start = dtemp;
                if (Debug) printf("pos_start = %g\n",InfoPtr->pos_start);
                break;
            case 9:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->pos_step = dtemp;
                if (Debug) printf("pos_step = %g\n",InfoPtr->pos_step);
                break;
            case 10:
                dtemp = atof(cp);
                if ((dtemp >= 0.01) && (dtemp <= 0.30)) InfoPtr->velocity = dtemp;
                if (Debug) printf("velocity = %g\n",InfoPtr->velocity);
                break;
            case 11:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->channel = itemp;
                if (Debug) printf("channel = %d\n",InfoPtr->channel);
                break;
            case 12:
                InfoPtr->samp_first = atol(cp);
                if (Debug) printf("samp_first = %s\n",InfoPtr->samp_first);
                break;
            case 13:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->mute = dtemp; /* percentage */
                if (Debug) printf("mute = %g\n",InfoPtr->mute);
                break;
            case 14:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_start_trace = itemp;
                if (Debug) printf("rg_start_trace = %d\n",InfoPtr->rg_start_trace);
                break;
            case 15:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_start_samp = itemp;
                if (Debug) printf("rg_start_samp = %d\n",InfoPtr->rg_start_samp);
                break;
            case 16:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_stop_samp = itemp;
                if (Debug) printf("rg_stop_samp = %d\n",InfoPtr->rg_stop_samp);
                break;
            case 17:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->rg_step = itemp;
                if (Debug) printf("rg_step = %d\n",InfoPtr->rg_step);
                break;
            case 18:
                itemp = atoi(cp);
                if (itemp == 2)  InfoPtr->rg_num_on = itemp;
                if (Debug) printf("rg_num_on = %d\n",InfoPtr->rg_num_on);
                break;
            case 19:
                itemp = atoi(cp);
                InfoPtr->rg_on[0] = itemp;
                if (Debug) printf("rg_on[0] = %d\n",InfoPtr->rg_on[0]);
                break;
            case 20:
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
/**************************** CmpgDisplayParameters() ***************************/
/* Display parameters to CRT.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, "jl_defs.h", "gpr_cmpg.h".
 * Calls: strupr, printf, puts, getch, kbhit, _ABS, strchr, sprintf.
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 7, 2002;
 */
void CmpgDisplayParameters (struct CmpgParamInfoStruct *InfoPtr)
{
    int i,key,len,num;
    int lines, maxlines = 21;
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
    printf("\nGPR_CMPG version %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_CMPG_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    printf("cmd_filename = %s\n",strupr(InfoPtr->cmd_filename));
    printf("input data files [%d]:\n",InfoPtr->num_input_files);
    lines = 5;
    len = 0;
    num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {  if (strlen(InfoPtr->dat_infilename[i]) > len)
           len = strlen(InfoPtr->dat_infilename[i])+1;
    }
    num = 80 / len;
    if (num < 1) num = 1;
    if (num > 5) num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   printf("%s ",InfoPtr->dat_infilename[i]);
        if ( (i>0) && !((i+1)%num) )
        {   puts("");
            lines ++;
            if (!Batch && lines >= maxlines)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
        }
    }
    puts("\n");
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("dzt_outfilename = %s\n",strupr(InfoPtr->dzt_outfilename));
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("inf_outfilename = %s\n",strupr(InfoPtr->inf_outfilename));
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("traces/channel = %ld  samples/trace = %ld  time/trace = %g (ns)\n",
        InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("ns_per_samp  = %g\n",InfoPtr->ns_per_samp);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("channel     = %d    ant_freq   = %d MHz\n",
        InfoPtr->channel,InfoPtr->ant_freq);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("samp_first = %ld\n",InfoPtr->samp_first);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("trace_first = %ld  trace_last = %ld  trace_num = %ld\n",
        InfoPtr->trace_first,InfoPtr->trace_last,InfoPtr->trace_num);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("pos_start = %g  pos_final = %g  pos_step = %g\n",
            InfoPtr->pos_start,InfoPtr->pos_final,InfoPtr->pos_step);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("offset_first = %g  offset_final = %g  offset_incr = %g\n",
            InfoPtr->offset_first,InfoPtr->offset_final,InfoPtr->offset_incr);
    lines++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("velocity = %g   mute = %g percent\n",InfoPtr->velocity,InfoPtr->mute);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("Range Gain: num_on = %d [%g][%g] dB\n",
        InfoPtr->rg_num_on,InfoPtr->rg_on[0],InfoPtr->rg_on[1]);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    puts("Storage info:");
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("stacked CMP section: num_traces=%d num_samps=%d size=%7.2f MB\n",
        InfoPtr->num_traces,InfoPtr->num_samps,
        (InfoPtr->num_traces*InfoPtr->num_samps*2)/(1024.0*1024.0));
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("CMP gathers: num_vgrids=%d num_files=%d num_samps=%d size=%7.2f MB\n",
        InfoPtr->num_vgrids,InfoPtr->num_files,InfoPtr->num_samps,
        (InfoPtr->num_vgrids*InfoPtr->num_files*InfoPtr->num_samps*2)/(1024.0*1024.0));
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("Input data: num_files=%d num_traces=%d num_samps=%d size=%7.2f MB\n",
        InfoPtr->num_files,InfoPtr->num_traces,InfoPtr->num_samps,
        (InfoPtr->num_files*InfoPtr->num_traces*InfoPtr->num_samps*2)/(1024.0*1024.0));
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
/****************************** CmpgAllocGrids() *****************************/
/* Allocate the space for the grid arrays declared in the info struct.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_cmpg.h".
 * Calls: malloc, free.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *CmpgAllocGridsMsg[] =
{   "CmpgAllocGrids(): No errors.",
    "CmpgAllocGrids() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "CmpgAllocGrids() ERROR: Unable to allocate storage for dgrid[][][].",
    "CmpgAllocGrids() ERROR: Unable to allocate storage for vgrid[][][].",
    "CmpgAllocGrids() ERROR: Unable to allocate storage for grid[][].",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 4, 2002;
 */
int CmpgAllocGrids (struct CmpgParamInfoStruct *InfoPtr)
{
    int  i,j,itemp;       /* scratch variables and counters */
    long num,trace,samp;  /* scratch variables and counters */

    extern int Debug;

/***** Allocate storage for dgrid[num_files][num_traces][num_samps] *****/
    if (InfoPtr->dgrid_created) return 1;
    InfoPtr->dgrid = (unsigned short ***)malloc((size_t)InfoPtr->num_files * sizeof(unsigned short **));
    if (InfoPtr->dgrid == NULL)
    {   InfoPtr->dgrid_created = FALSE;
        return 2;
    }
    for (num=0; num<InfoPtr->num_files; num++)
    {   InfoPtr->dgrid[num] = (unsigned short **)malloc((size_t)InfoPtr->num_traces * sizeof(unsigned short *));
        if (InfoPtr->dgrid[num] == NULL)
        /* problem so free storage and return error */
        {   for (i=0; i<num; i++)
                free(InfoPtr->dgrid[i]);
            free(InfoPtr->dgrid);
            InfoPtr->dgrid = NULL;
            InfoPtr->dgrid_created = FALSE;
            return 2;
        }
        for (trace=0; trace<InfoPtr->num_traces; trace++)
        {   InfoPtr->dgrid[num][trace] = (unsigned short *)malloc((size_t)InfoPtr->num_samps * sizeof(unsigned short));
            if (InfoPtr->dgrid[num][trace] == NULL)
            /* problem so free storage and return error */
            {   /* free ones for this [num] */
                for (i=0; i<trace; i++)
                    free(InfoPtr->dgrid[num][i]);
                /* free ones for previous [num]s */
                for (i=0; i<num; i++)
                {   for (j=0; j<InfoPtr->num_traces; j++)
                        free(InfoPtr->dgrid[i][j]);
                    /* free previous [num]s */
                    free(InfoPtr->dgrid[i]);
                }
                free(InfoPtr->dgrid);
                InfoPtr->dgrid = NULL;
                InfoPtr->dgrid_created = FALSE;
                return 2;
            }
        }
    }
    InfoPtr->dgrid_created = TRUE;

/***** Allocate storage for vgrid[num_vgrids][num_files][num_samps] *****/
    if (InfoPtr->vgrid_created) return 1;
    InfoPtr->vgrid = (unsigned short ***)malloc((size_t)InfoPtr->num_vgrids * sizeof(unsigned short **));
    if (InfoPtr->vgrid == NULL)
    {   InfoPtr->vgrid_created = FALSE;
        return 3;
    }
    for (num=0; num<InfoPtr->num_vgrids; num++)
    {   InfoPtr->vgrid[num] = (unsigned short **)malloc((size_t)InfoPtr->num_files * sizeof(unsigned short *));
        if (InfoPtr->vgrid[num] == NULL)
        /* problem so free storage and return error */
        {   for (i=0; i<num; i++)
                free(InfoPtr->vgrid[i]);
            free(InfoPtr->vgrid);
            InfoPtr->vgrid = NULL;
            InfoPtr->vgrid_created = FALSE;
            return 3;
        }
        for (trace=0; trace<InfoPtr->num_files; trace++)
        {   InfoPtr->vgrid[num][trace] = (unsigned short *)malloc((size_t)InfoPtr->num_samps * sizeof(unsigned short));
            if (InfoPtr->vgrid[num][trace] == NULL)
            /* problem so free storage and return error */
            {   /* free ones for this [num] */
                for (i=0; i<trace; i++)
                    free(InfoPtr->vgrid[num][i]);
                /* free ones for previous [num]s */
                for (i=0; i<num; i++)
                {   for (j=0; j<InfoPtr->num_files; j++)
                        free(InfoPtr->vgrid[i][j]);
                    /* free previous [num]s */
                    free(InfoPtr->vgrid[i]);
                }
                free(InfoPtr->vgrid);
                InfoPtr->vgrid = NULL;
                InfoPtr->vgrid_created = FALSE;
                return 3;
            }
        }
    }
    InfoPtr->vgrid_created = TRUE;

/***** Allocate storage for grid[num_traces][num_samps] *****/
    if (InfoPtr->grid_created) return 1;
    InfoPtr->grid = (unsigned short **)malloc((size_t)InfoPtr->num_traces * sizeof(unsigned short *));
    if (InfoPtr->grid == NULL)
    {   InfoPtr->grid_created = FALSE;
        return 4;
    }
    for (trace=0; trace<InfoPtr->num_traces; trace++)
    {   InfoPtr->grid[trace] = (unsigned short *)malloc((size_t)InfoPtr->num_samps * sizeof(unsigned short));
        if (InfoPtr->grid[trace] == NULL)
        /* problem so free storage and return error */
        {   /* free previous [num]s */
            for (i=0; i<trace; i++) free(InfoPtr->grid[i]);
            free(InfoPtr->grid);
            InfoPtr->grid = NULL;
            InfoPtr->grid_created = FALSE;
            return 4;
        }
    }
    InfoPtr->grid_created = TRUE;

    return 0;
}
/***************************** CmpgDeallocGrids() ****************************/
/* Deallocate the buffer space in the information structure.  This function
 * will work correctly only if the num_... variables accurately reflect
 * the successful allocation of storage.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_cmpg.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 4, 2002;
 */
void CmpgDeallocGrids (struct CmpgParamInfoStruct *InfoPtr)
{
    int i,j;
    extern int Debug;

    if (InfoPtr->grid_created)
    {   if (InfoPtr->grid)
        {   for (i=0; i<InfoPtr->num_traces; i++)
                free(InfoPtr->grid[i]);
            free(InfoPtr->grid);
            InfoPtr->grid = NULL;
        }
        InfoPtr->grid_created = FALSE;
    }
    if (InfoPtr->vgrid_created)
    {   if (InfoPtr->vgrid)
        {   for (i=0; i<InfoPtr->num_vgrids; i++)
            {   for (j=0; j<InfoPtr->num_files; j++)
                    free(InfoPtr->vgrid[i][j]);
                free(InfoPtr->vgrid[i]);
            }
            free(InfoPtr->vgrid);
            InfoPtr->vgrid = NULL;
        }
        InfoPtr->vgrid_created = FALSE;
    }
    if (InfoPtr->dgrid_created)
    {   if (InfoPtr->dgrid)
        {   for (i=0; i<InfoPtr->num_files; i++)
            {   for (j=0; j<InfoPtr->num_traces; j++)
                    free(InfoPtr->dgrid[i][j]);
                free(InfoPtr->dgrid[i]);
            }
            free(InfoPtr->dgrid);
            InfoPtr->dgrid = NULL;
        }
        InfoPtr->dgrid_created = FALSE;
    }

}
/***************************** CmpgGetDataAsGrid16u() ****************************/
/* Get the data/info from the appropriate files and store as unsigned 16-bit
 * data col by col (ie. as a "grid"). The grid must have already been allocated
 *
 * This function initializes InfoPtr->dgrid[num_files][num_traces][num_samps]
 * to 32768U before getting the data.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>,
 *           "gpr_cmpg.h", "gpg_io.h".
 * Calls: malloc, free, fprintf, GetDztChSubGrid16
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *CmpgGetDataAsGrid16uMsg[] =
{   "CmpgGetDataAsGrid16u(): No errors.",
    "CmpgGetDataAsGrid16u() ERROR: dgrid[][][] is NULL.",
    "CmpgGetDataAsGrid16u() ERROR: Problem getting data from input file.",
    "CmpgGetDataAsGrid16u() ERROR: Unsupported data storage format.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 4, 2002
 */
int CmpgGetDataAsGrid16u (struct CmpgParamInfoStruct *InfoPtr)
{
    int  i,j,itemp;       /* scratch variables and counters */
    long num,trace,samp;  /* scratch variables and counters */
    extern FILE *log_file;

    /* for GSSI SIR-10 files (see dzt_pgms.h) */
    extern const char *GetDztChSubGrid16Msg[];

    /* Reality check */
    if (InfoPtr->dgrid == NULL) return 1;

/***** Initialize dgrid[][][] *****/
	for (num=0; num<InfoPtr->num_files; num++)
	{   for (trace=0; trace<InfoPtr->num_traces; trace++)
        {   for (samp=0; samp<InfoPtr->num_samps; samp++)
            {   InfoPtr->dgrid[num][trace][samp] = 32768U;
            }
		}
    }

/***** Get the data files and store as 16-bit unsigned GPR grids *****/
    itemp = 0;
    switch (InfoPtr->storage_format)
    {   case DZT:  case MOD_DZT:
            for (num=0; num<InfoPtr->num_files; num++)
            {   itemp = 0;
                itemp = GetDztChSubGrid16(InfoPtr->dat_infilename[num],InfoPtr->channel,
                        (long)InfoPtr->trace_first,(long)InfoPtr->num_traces,
                        (long)(InfoPtr->num_samps),InfoPtr->dgrid[num]);
                if (itemp > 0)
                {   printf("\nfile #%d: %s\n",num,GetDztChSubGrid16Msg[itemp]);
                    if (log_file) fprintf(log_file,"file #%d: %s\n",num,GetDztChSubGrid16Msg[itemp]);
                    itemp = 2;
                }
                if (itemp > 0) break; /* out for loop */
            }
            break;
        default:      /* unsupported storage format */
            itemp = 3;
            break;
    }

    if (itemp > 0) return itemp;
    else return 0;
}
/******************************* CmpgSortData() ******************************/
/* Re-order the data so that traces are sorted by position then offset.
 * The first half of vgrid[][][] is used for this. In dgrid (input data),
 * each file (or grid) has common offset for all traces. This function will
 * take the first trace (for example) from each grid in dgrid and place it
 * into the first grid in vgrid. Each grid in vgrid (first half filled in this
 * function) is a CMP gather (no NMO correction yet).
 *
 * Note: vgrid[][][] is initialized to 32768.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>,
 *           "gpr_cmpg.h", "gpg_io.h".
 * Calls: malloc, free, fprintf, GetDztChSubGrid16
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *CmpgSortDataMsg[] =
{   "CmpgSortData(): No errors.",
    "CmpgSortData() ERROR: vgrid[][][] is NULL.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 9, 2002
 */
int CmpgSortData (struct CmpgParamInfoStruct *InfoPtr)
{
    int  i,j,itemp;       /* scratch variables and counters */
    long num,trace,samp;  /* scratch variables and counters */
    extern FILE *log_file;


    /* Reality check */
    if (InfoPtr->vgrid == NULL) return 1;

    /* Initialize vgrid[][][] */
	for (num=0; num<(2*InfoPtr->num_traces); num++)
	{   /* For every trace in the grid (vgrid) */
        for (trace=0; trace<InfoPtr->num_files; trace++)
        {   for (samp=0; samp<InfoPtr->num_samps; samp++)
            {    InfoPtr->vgrid[num][trace][samp] = 32768U;
            }
        }
    }

    /* For every grid in the first half of vgrid */
	for (num=0; num<InfoPtr->num_traces; num++)
	{   /* For every trace in the grid (vgrid) */
        for (trace=0; trace<InfoPtr->num_files; trace++)
        {   for (samp=0; samp<InfoPtr->num_samps; samp++)
            {   InfoPtr->vgrid[num][trace][samp] = InfoPtr->dgrid[trace][num][samp];
            }
		}
    }

    return 0;
}

/******************************* CmpgPerformNMO() *****************************/
/* This function performs the normal moveout (NMO) correction for a single
 * velocity to the radar data stored in the first half vgrid[][][]. Results
 * are stored in the second half of vgrid[][][]. The NMO time is calculated
 * for every sample using the equation Tnmo = sqrt(Tsamp^2 + X^2/vel^2)).
 * Linear interpolation is used to find the trace amplitude at that time and
 * the values is assigned to vgrid[xxx][trace][samp]. NMO times before or
 * after the trace are not used and the initialization (median) value is used.
 * Also, if the mute exceeds the user-defined limit, the initialization
 * value is used.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <math.h>, "gpr_cmpg.h".
 * Calls: sqrt, ChangeRangeGain16u.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *CmpgPerformNMOMsg[] =
{   "CmpgPerformNMO(): No errors.",
	"CmpgPerformNMO() ERROR: Divide by zero: ns_per_samp.",
	"CmpgPerformNMO() ERROR: Divide by zero: V2.",
	"CmpgPerformNMO() ERROR: Divide by zero: deltime".,
	"CmpgPerformNMO() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: January 8, 2002;
 */
int CmpgPerformNMO (struct CmpgParamInfoStruct *InfoPtr)
{
    int i, vel, num, trace, samp;     /* counters */
    int samp1, samp2, test, itemp;
    double V, V2, X, X2, T, T2, X2byV2;
    double dtemp;
    double nmo_time, nmo_samp, nmo_amp;
    double amp1, amp2, time1, time2, deltime, delamp;

    if (InfoPtr->ns_per_samp == 0.0) return 1;

    /* Gain CMPs stored in vgrid[0 to num_traces-1] if requested */
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
        for (num=0; num<InfoPtr->num_traces; num++)
        {   for (trace=InfoPtr->rg_start_trace; trace<InfoPtr->num_files; trace++)
            {   start_samp = InfoPtr->rg_start_samp + (trace * InfoPtr->rg_step);
                stop_samp = InfoPtr->rg_stop_samp + (trace * InfoPtr->rg_step);
                if (start_samp >= InfoPtr->num_samps) break;  /* out of for inner loop */
                if (stop_samp  >= InfoPtr->num_samps)
                {   itemp = (InfoPtr->num_samps-1) - start_samp;
                    dtemp = InfoPtr->rg_on[1] - InfoPtr->rg_on[0];
                    rg_on[1] = InfoPtr->rg_on[0] + (dtemp * (itemp/(stop_samp-start_samp)));
                    stop_samp = InfoPtr->num_samps - 1;
                }
                itemp = ChangeRangeGain16u(InfoPtr->num_samps, start_samp, stop_samp,
                            InfoPtr->rg_num_off, InfoPtr->rg_off, InfoPtr->rg_num_on,
                            rg_on, gain_off_func, gain_on_func,
                            InfoPtr->vgrid[num][trace]);
                if (itemp > 0)
                {   printf("\n%s\n",ChangeRangeGain16uMsg[itemp]);
                    if (log_file) fprintf(log_file,"%s\n",ChangeRangeGain16uMsg[itemp]);
                    break;   /* out of for loop */
                }
            }
        }
    }

    /* V (m/ns) = the velocity for NMO correction */
    V = InfoPtr->velocity;
    V2 = V * V;
    if (V2 == 0.0) return 2;

    /* Perform NMO on vgrid[0 to num_traces-1]
       and assign to  vgrid[num_traces to 2*num_traces-1] */
    for (num=0; num<InfoPtr->num_traces; num++)
    {   for (trace=0; trace<InfoPtr->num_files; trace++)
        {   /* X (m) = offset between antennas */
            X = InfoPtr->offset_first + (InfoPtr->offset_incr * trace);
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
                amp1 = InfoPtr->vgrid[num][trace][samp1];
                amp2 = InfoPtr->vgrid[num][trace][samp2];
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

                /* assign the value to second half of vgrid[][][] */
                InfoPtr->vgrid[InfoPtr->num_traces+num][trace][samp] = _LIMIT(0.0,nmo_amp,65535.0);
            }

            /* check to see if all values "muted" to median */
            test = 1;
            for (samp=InfoPtr->samp_first+1; samp<InfoPtr->num_samps; samp++)
            {   if (InfoPtr->vgrid[InfoPtr->num_traces+num][trace][samp] != 32768)
                {   test = 0;
                    break;    /* out of inner for loop */
                }
            }
            if (test)                          /* 'M' = 0x4D */
                 /* mark trace as all muted */
                InfoPtr->vgrid[InfoPtr->num_traces+num][trace][1] = 0x4D00U;
        }
    }
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

/************************* CmpgCalcCmpStackedSection() ***********************/
/* This function calculates the CMP stacked section.
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, <math.h>, "gpr_cmpg.h".
 * Calls: malloc, fabs.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *CmpgCalcCmpStackedSectionMsg[] =
{   "CmpgCalcCmpStackedSection(): No errors.",
    "CmpgCalcCmpStackedSection() ERROR: Not able to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: January 9, 2002;
 */
int CmpgCalcCmpStackedSection (struct CmpgParamInfoStruct *InfoPtr)
{
    int i, num, trace, samp, count, dsamp, gtrace, test;
    int firstsamp;
    double *dtrace, dtemp, Z_max, Z_min, Z;

/***** Allocate storage for the stack trace *****/
    dtrace = (double *)malloc((size_t)InfoPtr->num_samps * sizeof(double));
    if (dtrace == NULL)
        return 1;

/***** Initialize grid[][] - to be the stacked cmp section*****/
    for (num=0; num<InfoPtr->num_traces; num++)
    {   for (samp=0; samp<InfoPtr->num_samps; samp++)
        {   InfoPtr->grid[num][samp] = 32768U;
        }
    }

/***** Stack the NMOs and copy to corresponding column in grid[][] *****/
    Z_max = -1000000.0;
    Z_min =  1000000.0;
    firstsamp = 0;
    if (InfoPtr->samp_first > 0) firstsamp = InfoPtr->samp_first;

    /* For every CMP gather in the second half of vgrid[][][] */
    for (num=InfoPtr->num_traces,gtrace=0;
         num<(2*InfoPtr->num_traces),gtrace<InfoPtr->num_traces;
         num++,gtrace++)
    {
        /* Initialize stack trace and counter */
        for (i=0; i<InfoPtr->num_samps; i++) dtrace[i] = 0.0;

        /* Stack the traces for this NMO */
        count = InfoPtr->num_files;
        dsamp = 0;
        for (samp=firstsamp; samp<InfoPtr->num_samps,dsamp<InfoPtr->num_samps; samp++)
        {   if (samp<0) continue; /* skip */
            for (trace=0; trace<InfoPtr->num_files; trace++)
            {   dtrace[dsamp] += InfoPtr->vgrid[num][trace][samp];
            }
            dtrace[dsamp] /= count;
            dsamp++;
        }
#if 0
        count = 0;
        dsamp = 0;
        for (samp=firstsamp; samp<InfoPtr->num_samps,dsamp<InfoPtr->num_samps; samp++)
        {   if (samp<0) continue; /* skip */
            for (trace=0; trace<InfoPtr->num_files; trace++)
            {   if (InfoPtr->vgrid[num][trace][1] == 0x4D00U) /* all muted */
                    break; /* out of this for loop */
                dtrace[dsamp] += InfoPtr->vgrid[num][trace][samp];
                count++;
            }
            if (count>0)
            {   dtrace[dsamp] /= count;
            }
            dsamp++;
        }
#endif
        /* Assign the stacked value to grid[][] */
        for (samp=0; samp<InfoPtr->num_samps; samp++)
        {
            /* dtrace[dsamp] += 32768.0; */
            InfoPtr->grid[gtrace][samp] = _LIMIT(0.0,dtrace[samp],65535.0);
            /* keep track of max and min */
            if (InfoPtr->grid[gtrace][samp] < Z_min) Z_min = InfoPtr->grid[gtrace][samp];
            if (InfoPtr->grid[gtrace][samp] > Z_max) Z_max = InfoPtr->grid[gtrace][samp];
        }
    }

/***** Expand dynamic range *****/
    if (Z_max > Z_min)
    {   dtemp = 65535.0 / (Z_max - Z_min);
        for (trace=0; trace<InfoPtr->num_traces; trace++)
        {   for (samp=0; samp<InfoPtr->num_samps; samp++)
            {   Z  = InfoPtr->grid[trace][samp];
                Z -= Z_min;
                Z *= dtemp;
                InfoPtr->grid[trace][samp] = (unsigned short)(_LIMIT(0.0,Z,65535.0));
            }
        }
    }

    /* Dealloc temp storage and return */
    free(dtrace);
    return 0;
}

/************************** CmpgSaveGprData() *******************************/
/* This is the driver routine for saving the GPR data to disk.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of allocated structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <time.h>, "gpr_cmpg.h".
 * Calls: printf, fprintf, sprintf, strcat, _dos_gettime, _dos_getdate, strlen,
 *        CmpgSaveDztData.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *CmpgSaveGprDataMsg[] =
{   "CmpgSaveGprData(): No errors.",
    "CmpgSaveGprData() ERROR: Invalid file storage format.",
    "CmpgSaveGprData() ERROR: Data/information was not saved to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: January 7, 2002;
 */
int CmpgSaveGprData (struct CmpgParamInfoStruct *InfoPtr)
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
    extern const char *CmpgSaveDztDataMsg[]; /* beginning of this source */


    /* Fill processing history string */
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
    sprintf(InfoPtr->proc_hist,"\nGPR_CMPG v%s (USGS-JL) on %s %d, %d at %d:%02d\n",
                        GPR_CMPG_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute);
    sprintf(str," CMP stckd sctn+%d CMP gthrs mutd+%d CMP gthrs NMO\n",
            InfoPtr->num_traces,InfoPtr->num_traces);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," %d traces in CMP section\n",InfoPtr->num_traces);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," %d traces in each CMP gather\n",InfoPtr->num_files);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," velocity=%g mute=%g position(m):first=%g last=%g step=%g\n",
            InfoPtr->velocity,InfoPtr->mute,InfoPtr->pos_start,
            InfoPtr->pos_final,InfoPtr->pos_step);
    strcat(InfoPtr->proc_hist,str);
    sprintf(str," offset(m):first=%g last=%g step=%g\n",
            InfoPtr->offset_first,InfoPtr->offset_final,InfoPtr->offset_incr);
    strcat(InfoPtr->proc_hist,str);
    if (InfoPtr->rg_num_on == 2)
    {   sprintf(str," RG[%g][%g]: 1st-trace=%d 1st-samp=%d stopsamp=%d step=%d \n",
            InfoPtr->rg_on[0],InfoPtr->rg_on[1],InfoPtr->rg_start_trace,
            InfoPtr->rg_start_samp,InfoPtr->rg_stop_samp,InfoPtr->rg_step);
        strcat(InfoPtr->proc_hist,str);
    }

    switch (InfoPtr->storage_format)
	{   case DZT:  case MOD_DZT:
        {   itemp = CmpgSaveDztData(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",CmpgSaveDztDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",CmpgSaveDztDataMsg[itemp]);
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

/******************************* CmpgSaveDztData() ******************************/
/* This function saves the GPR data to disk using the DZT format.
 *
 * Parameter list:
 *  struct CmpgParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_cmpg.h".
 * Calls: SetDzt5xHeader, ReadOneDztHeader, SaveDztFile, _splitpath (non-ANSI),
 *        strcpy, strncpy, strcat, sprintf, puts, printf, strlen.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *CmpgSaveDztDataMsg[] =
{   "CmpgSaveDztData(): No errors.",
	"CmpgSaveDztData() ERROR: Invalid datatype for output DZT file.",
    "CmpgSaveDztData() ERROR: Problem getting input file header.",
    "CmpgSaveDztData() ERROR: Unable to open output NMO file.",
    "CmpgSaveDztData() ERROR: Problem saving header to disk.",
    "CmpgSaveDztData() ERROR: Problem saving data to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: January 7, 2002;
 */
int CmpgSaveDztData (struct CmpgParamInfoStruct *InfoPtr)
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
	itemp = ReadOneDztHeader(InfoPtr->dat_infilename[0],&num_hdrs,
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
	for (i=0; i<InfoPtr->num_traces; i++)
	{   InfoPtr->grid[i][0] = 0xFFFFU;
        if (InfoPtr->grid[i][1] != 0x4D00U)
            InfoPtr->grid[i][1] = 0xE800U;
    }
	for (i=0; i<InfoPtr->num_vgrids; i++)
	{   for (j=0; j<InfoPtr->num_files; j++)
        {   InfoPtr->vgrid[i][j][0] = 0xFFFFU;
		    if (InfoPtr->vgrid[i][j][1] != 0x4D00U)
                InfoPtr->vgrid[i][j][1] = 0xE800U;
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

    /* Write CMP stacked section to disk */
    trace_bytes = InfoPtr->num_samps * 2; /* 2 bytes per samp (unsigned short) */
    for (trace=0; trace<InfoPtr->num_traces; trace++)
    {   if (fwrite(InfoPtr->grid[trace],(size_t)trace_bytes,(size_t)1,outfile) < 1)
        {   fclose(outfile);
            #if defined(_INTELC32_)
              realfree(buffer);
            #endif
            return 5;
        }
    }

    /* Write CMP gathers to disk */
    for (num=0; num<InfoPtr->num_vgrids; num++)
    {   for (trace=0; trace<InfoPtr->num_files; trace++)
        {   if (fwrite(InfoPtr->vgrid[num][trace],(size_t)trace_bytes,(size_t)1,outfile) < 1)
            {   fclose(outfile);
                #if defined(_INTELC32_)
                  realfree(buffer);
                #endif
                return 5;
            }
        }
    }


    /* Close file, release buffer and return */
    fclose(outfile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    return 0;
}

/*************************** CmpgSaveInfo() ****************************/
/* Save the information to disk that shows where each set of traces is in
 * the output DZT file.
 *
 * Parameters:
 *  struct CmpgParamInfoStruct *InfoPtr - adress of information structure
 *
 * Requires: <process.h>, <stdio.h>, <stdlib>, <string.h>, "gpr_cmpg.h",
 * Calls: strcpy, _chdrive, chdir fopen, fclose, fprintf, sprintf, strupr,
 *        malloc.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *CmpgSaveInfoMsg[] =
{   "CmpgSaveInfo(): No errors.",
    "CmpgSaveInfo() ERROR: Unable to open output TXT file.",
    "SaveGprCmpgData() ERROR: Problem saving TXT data.",
} ;
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: January 8, 2002;
 *
 */
int CmpgSaveInfo(struct CmpgParamInfoStruct *InfoPtr)
{
    int     i, num, trace1, trace2, len, trace3;
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
    fprintf(outfile,"GPR_CMPG v. %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_CMPG_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    fprintf(outfile,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n\n");
    fprintf(outfile,"cmd_filename      = %s\n",strupr(InfoPtr->cmd_filename));
    fprintf(outfile,"input files       =\n",InfoPtr->dat_infilename);
    for (i=0; i<InfoPtr->num_input_files; i++)
    {  if (strlen(InfoPtr->dat_infilename[i]) > len)
           len = strlen(InfoPtr->dat_infilename[i])+1;
    }
    num = 80 / len;
    if (num < 1) num = 1;
    if (num > 5) num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   fprintf(outfile,"%s ",InfoPtr->dat_infilename[i]);
        if ( (i>0) && !((i+1)%num) )
            fprintf(outfile,"/n");
    }
    fprintf(outfile,"dzt_outfilename   = %s\n",strupr(InfoPtr->dzt_outfilename));
    fprintf(outfile,"inf_outfilename   = %s\n",strupr(InfoPtr->inf_outfilename));

    fprintf(outfile,"\nInput CMP file information:\n");
    fprintf(outfile,"traces/channel = %ld\n",InfoPtr->total_traces);
    fprintf(outfile,"samples/trace  = %ld\n",InfoPtr->total_samps);
    fprintf(outfile,"time/trace     = %g (ns)\n",InfoPtr->total_time);
    fprintf(outfile,"ns_per_samp    = %g\n",InfoPtr->ns_per_samp);
    fprintf(outfile,"channel        = %d\n",InfoPtr->channel);
    fprintf(outfile,"ant_freq       = %d MHz\n",InfoPtr->ant_freq);

    fprintf(outfile,"\nCMP stacked section information:\n");
    fprintf(outfile,"offset_first   = %g\n",InfoPtr->offset_first);
    fprintf(outfile,"offset_final   = %g\n",InfoPtr->offset_final);
    fprintf(outfile,"offset_incr    = %g\n",InfoPtr->offset_incr);
    fprintf(outfile,"samp_first (T0)= %ld\n",InfoPtr->samp_first);
    fprintf(outfile,"trace_first    = %ld\n",InfoPtr->trace_first);
    fprintf(outfile,"trace_last     = %ld\n",InfoPtr->trace_last);
    fprintf(outfile,"trace_num      = %ld\n",InfoPtr->trace_num);
    fprintf(outfile,"pos_start      = %g\n",InfoPtr->pos_start);
    fprintf(outfile,"pos_final      = %g\n",InfoPtr->pos_final);
    fprintf(outfile,"pos_step       = %g\n",InfoPtr->pos_step);
    fprintf(outfile,"velocity       = %g\n",InfoPtr->velocity);
    fprintf(outfile,"mute           = %g percent\n",InfoPtr->mute);
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
    fprintf(outfile,"\nCMP stacked section: first trace=%6d  last trace=%6d\n",
              trace1,trace2);
    fprintf(outfile,"\nFor CMP gathers, good traces are 'marked'; Muted traces are not marked.\n");
    fprintf(outfile,"There are %d CMP gathers with mute applied.\n",InfoPtr->num_traces);
    fprintf(outfile,"Followed %d CMP gathers with the NMO correction.\n",InfoPtr->num_traces);
    fprintf(outfile,"Each CMP gathers has %d traces in it.\n",InfoPtr->num_files);
    trace3 = InfoPtr->num_traces;
    for (num=0; num<InfoPtr->num_traces; num++)
    {   trace1 = trace3 + (num)*InfoPtr->num_files;
        trace2 = trace3 + ((num+1)*InfoPtr->num_files) -1;
        fprintf(outfile,"CMP gather muted #%d: first trace=%6d  last trace=%6d\n",
              num,trace1,trace2);
    }
    trace3 += InfoPtr->num_traces * InfoPtr->num_files;
    for (num=0; num<InfoPtr->num_traces; num++)
    {   trace1 = trace3 + (num)*InfoPtr->num_files;
        trace2 = trace3 + ((num+1)*InfoPtr->num_files) -1;
        fprintf(outfile,"CMP gather muted #%d: first trace=%6d  last trace=%6d\n",
              num,trace1,trace2);
    }

    /* Close file */
    fclose(outfile);

    return 0;
}
/***************************** End of GPR_CMPG.C ******************************/

