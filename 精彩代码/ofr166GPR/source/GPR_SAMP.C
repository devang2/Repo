/******************************** GPR_SAMP.C ********************************\
 *                            version 1.08.09.01
 * GPR_SAMP v. 1.0 resamples digital ground penetrating radar data.  The number
 * of trace and/or samples can be reduced and the number of samples per trace
 * can be increased or decreased.  The following computer storage formats are 
 * automatically recognized.
 *
 *   GSSI SIR-10A version 3.x to 5.x binary files, 8-bit or 16-bit unsigned
 *     integers with an embedded descriptive 512- or 1024-byte header.
 *     Data files usually have the extension "DZT".
 *   Sensors and Software pulseEKKO binary files, 16-bit signed integers
 *     accompanied by a descriptive ASCII "HD" file.  Data files must have
 *     the extension "DT1".
 *   SEG-Y formatted files.  Data files usually have the extension "SGY".
 *
 * If the storage format does not conform to any of the above or this
 * program is having trouble reading the file correctly, there are
 * options in the CMD file for the user to specify required parameters.
 *
 * After the data have been manipulated, they are stored to disk in the same 
 * format as they were read in.  An exception is made for early versions GSSI
 * DZT files that had 512-byte headers.  A current (version 5.x) 1024-byte 
 * header is written into the output file.  File extensions are forced to
 * DZT for GSSI files and to DT1 for PulseEKKO files.
 *
 * To convert from one GPR storage format to another, use program
 * GPR_CONV.EXE.  Because some storage formats maintain information about
 * the data in each trace header, both the trace header and trace
 * data block are stored as a unit as one column in the program's internal
 * storage grid.  If the size of the trace header is not an even multiple of
 * the data element (sample) size, the program will stop and report an error.
 * This should only be a problem with user-defined storage formats.
 *
 * The input to this program is a "CMD" file, an ASCII text file containing
 * commands (or parameters) describing how to process the radar data.  The
 * CMD file specifies the data file name (and optionally the storage format).
 * Inspect the example file GPR_SAMP.CMD for usage.
 *
 * NOTES:
 *   Only 1 to 4 headers are supported in DZT files.
 *   There is no graphic display of the data.
 *   To change storage formats use GPR_CONV.EXE.
 *   To process the data use GPR_PROC.EXE.
 *   To display the processed data, use programs such as GPR_DISP.EXE or
 *     FIELDVEW.EXE.
 *
 * Jeff Lucius, Geophysicist
 * U.S. Geological Survey, Geologic Division
 * Central Mineral Resources Program
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@usgs.gov
 *
 * Revision History:
 *  September 11, 1997 - Completed as an extensive revision of GPR_PROC.C.
 *  November 18, 1997  - In SaveDztData(), SaveDt1Data(), SaveSgyData(),
 *                       total time was not being reset.
 *  August 9, 2001     - Updated time and date functions
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /zfloatsync /xregion=32M gpr_samp.c gpr_io.lib gpr_dfx.lib
 *            jl_util1.lib
 *     To compile for 80486 executable:
 *       icc /F /xnovm /zfloatsync /zmod486 /xregion=32M gpr_samp.c gpr_io.lib
 *            gpr_dfx.lib jl_util1.lib
 *
 *     /F removes the floating point emulator, reduces binary size.
 *     /xnovm specifies no virtual memory manager, disk access is too slow.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /xregion=32M sets the region size to 32 Mbytes
 *     Jeff Lucius's GPR I/O functions (DZT, DT1, SGY) are in gpr_io.lib.
 *     Jeff Lucius's GPR data manipulation functions are in gpr_dfx.lib.
 *     Jeff Lucius's utility functions are in jl_util1.lib.
 *
 *     to remove assert() functions from code also add:
 *     /DNDEBUG
 *
 * To run: GPR_SAMP cmd_filename
 *
\****************************************************************************/

/*************************** Function dependency ****************************/
/*
 * con I/O  - keyboard and/or text-mode screen used
 * disk I/O - reads from and/or writes to disk
 * DOS      - DOS/BIOS interrupt performed
 * graphics - graphics modes required, direct writes to ports/video adaptor
 * port I/O - hardware I/O ports used
 * RAM      - direct RAM memory access
 *
 * main                                                (con I/O)
 *   +-- ANSI_there                                             (DOS)
 *   |     +-- GetDosVersion                                    (DOS)
 *   +-- Sound                                                       (port I/O)
 *   +-- GetParameters                                 (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line and opening the GPR data/info files to get
 *   |     | additional information about the data sets.
 *   |     +-- PrintUsageMsg                           (con I/O)
 *   |     +-- InitParameters
 *   |     +-- GetCmdFileArgs                (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetGprFileType                (disk I/O)
 *   |     +-- ReadOneDztHeader              (disk I/O)
 *   |     +-- InitDt1Parameters
 *   |     +-- GetSsHdFile                   (disk I/O)
 *   |     +-- PrintSsHdInfo                           (con I/O)
 *   |     +-- ReadSegyReelHdr               (disk I/O)
 *   |     +-- PrintSegyReelHdr                        (con I/O)
 *   |     +-- TrimStr
 *   |     +-- GetOtherInfo                  (disk I/O)
 *   +-- DisplayParameters                             (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- GetGprDataAsGrid                    (disk I/O)(con I/O)
 *   |     +-- GetDztChSubGrid8              (disk I/O)
 *   |     +-- GetDztChSubGrid16             (disk I/O)
 *   |     +-- GetGprSubGrid                 (disk I/O)
 *   +-- ProcessGprData                      (disk I/O)(con I/O)
 *   |     +-- Spline
 *   |     +-- SlideSamples
 *   |     +-- FftFilterGridTraces
 *   |           +-- FftFilterTrace
 *   |                 +-- BandPassFFT1D
 *   |                      +-- RealFFT1D
 *   |                            +-- FFT1D
 *   +-- SaveGprData                         (disk I/O)(con I/O)
 *         +-- SaveDztData                   (disk I/O)(con I/O)
 *         |     +-- ReadOneDztHeader        (disk I/O)
 *         |     +-- SetDzt5xHeader
 *         |     +-- SaveDztFile             (disk I/O)
 *         +-- SaveDt1Data                   (disk I/O)
 *         |     +-- GetSsHdFile             (disk I/O)
 *         |     +-- SaveSsHdFile            (disk I/O)
 *         +-- SaveSgyData                   (disk I/O)
 *         |     +-- ReadSegyReelHdr         (disk I/O)
 *         |     +-- ExtractSsInfoFromSegy
 *         |     +-- SetSgyFileHeader
 *         +-- SaveUsrData                   (disk I/O)
 */

/******************* Includes all required header files *********************/
#include "gpr_samp.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The next array is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_SAMP_CMDS[] =
{   "batch","debug","dat_infilename","dat_outfilename","file_header_bytes",
    "trace_header_bytes","samples_per_trace","total_time","input_datatype","channel",
    "first_trace","last_trace","first_samp","last_samp","resample",
    "preprocFFT",NULL,
} ;

const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR:  Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",

    "GetParameters() ERROR: Problem getting information about input data file.",
    "GetParameters() ERROR: Invalid channel selection for DZT file.",
    "GetParameters() ERROR: Problem getting info from HD file.",
    "GetParameters() ERROR: Problem getting info from SGY file.",
    "GetParameters() ERROR: Problem determining user-defined parameters.",

    "GetParameters() ERROR: No recognizable data/info input files specified.",
    "GetParameters() ERROR: Invalid input data element type .",
    "GetParameters() ERROR: Less than one trace in file.",
    "GetParameters() ERROR: Less than two samples per trace.",
    "GetParameters() ERROR: nanoseconds per sample is <= 0.0.",

    "GetParameters() ERROR: trace_header_size must be a multiple of sample size.",
} ;

const char *GetGprDataAsGridMsg[] =
{   "GetGprDataAsGrid(): No errors.",
    "GetGprDataAsGrid() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetGprDataAsGrid() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "GetGprDataAsGrid() ERROR: Invalid input data element type .",
    "GetGprDataAsGrid() ERROR: No recognized data/info input files specified.",

    "GetGprDataAsGrid() ERROR: Not able to allocate storage for grid.",
    "GetGprDataAsGrid() ERROR: Problem getting data from input file.",
} ;
const char *ResampleGprDataMsg[] =
{   "ResampleGprData(): No errors.",
    "ResampleGprData() ERROR: Unable to allocate temporary storage.",
    "ResampleGprData() ERROR: Error returned from FFT filter function - see LOG file.",
    "ResampleGprData() ERROR: Error returned from Spline function - see LOG file.",
} ;

const char *SaveGprDataMsg[] =
{   "SaveGprData(): No errors.",
    "SaveGprData() ERROR: Invalid/unknown file storage format.",
    "SaveGprData() ERROR: Data/information was not saved to disk.",
} ;

const char *SaveDztDataMsg[] =
{   "SaveDztData(): No errors.",
    "SaveDztData() ERROR: Invalid datatype for output DZT file.",
    "SaveDztData() ERROR: Problem getting input file header.",
    "SaveDztData() ERROR: Problem saving data.",
} ;

const char *SaveDt1DataMsg[] =
{   "SaveDt1Data(): No errors.",
    "SaveDt1Data() ERROR: Invalid datatype for output DT1 file.",
    "SaveDt1Data() ERROR: Problem getting input file header.",
    "SaveDt1Data() ERROR: Problem saving HD file.",
    "SaveDt1Data() ERROR: Unable to open output DT1 file.",

    "SaveDt1Data() ERROR: Problem saving DT1 file.",
} ;

const char *SaveSgyDataMsg[] =
{   "SaveSgyData(): No errors.",
    "SaveSgyData() ERROR: Invalid datatype for output SGY file.",
    "SaveSgyData() ERROR: Problem getting input file header.",
    "SaveSgyData() ERROR: Unable to open output DT1 file.",
    "SaveSgyData() ERROR: Problem saving SGY file header.",

    "SaveSgyData() ERROR: Problem saving SGY file data.",
} ;

const char *SaveUsrDataMsg[] =
{   "SaveUsrData(): No errors.",
    "SaveUsrData() ERROR: Invalid datatype for output user-defined file.",
    "SaveUsrData() ERROR: Problem getting input file header.",
    "SaveUsrData() ERROR: Unable to open output GPR file.",
    "SaveUsrData() ERROR: Problem saving GPR file header.",

    "SaveUsrData() ERROR: Problem saving GPR file data.",
} ;

/********************************** main() **********************************/
void main (int argc,char *argv[])
{
/***** Declare variables *****/
    /* following are scratch variables used by main */
    char log_filename[80];
    int  itemp;
    double dtemp;

    /* following is the information structure passed between functions */
    struct SampParamInfoStruct ParamInfo;

    /* these variables found near beginning of this source */
    extern int Debug,Batch,ANSI_THERE;
    extern FILE *log_file;
    extern const char *GetParametersMsg[];
    extern const char *GetGprDataAsGridMsg[];
    extern const char *ProcessGprDataMsg[];
    extern const char *SaveGprDataMsg[];

/***** Initialize variables *****/
    /* Open error message file */
    strcpy(log_filename,"gpr_samp.log");
    log_file = fopen(log_filename,"a+t");
    if (log_file == NULL)
    {    strcpy(log_filename,"c:\\gpr_samp.log");
        log_file = fopen(log_filename,"a+t");
    }
    if (log_file)
    {   char str[10];
        struct dosdate_t dosdate;
        struct dostime_t dostime;
        char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;

        fprintf(log_file,"\n**************************************************************************\n");
        sprintf(str,"%g",GPR_SAMP_VER);
        fprintf(log_file,"Messages from program GPR_SAMP v. %s: %s %d, %d at %d:%02d:%02d\n",
                        str,month[dosdate.month],
                        dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute,dostime.second);
    }

    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Zero-out information structure */
    memset((void *)&ParamInfo,0x0000,sizeof(struct SampParamInfoStruct));

/***** Get information used to process data *****/
    if (log_file && argv[1])
    {   fprintf(log_file,"Processing command file: %s\n",argv[1]);
    }
    printf("Getting user parameters ...");
    itemp =  GetParameters(argc,argv,&ParamInfo);
    printf("\r                                          \r");
    if (itemp > 0)
    {   printf("\n%s\n",GetParametersMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",GetParametersMsg[itemp]);
        if (log_file) fclose(log_file);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("\nGPR_SAMP finished.\nFile %s on disk contains messages.\n",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(1);
    }
    if (strcmp(ParamInfo.dat_infilename,ParamInfo.dat_outfilename) == 0 ||
        ( ParamInfo.hd_infilename[0] &&
          (strcmp(ParamInfo.hd_infilename,ParamInfo.hd_outfilename) == 0)
        )
       )
    {   printf("\nERROR: Input filename is duplicated in output list.\n");
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"ERROR: Input filename is duplicated in output list.\n");
            fclose(log_file);
            printf("\nFile %s on disk contains messages.\n",log_filename);
        }
        puts("GPR_SAMP finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(2);
    }
    if (log_file)
    {   if (Batch) fprintf(log_file,"Program in Batch mode.\n");
        if (Debug) fprintf(log_file,"Program in Debug mode.\n");
    }

/***** Display parameters and ask if user wants to continue *****/
    DisplayParameters(&ParamInfo);
    if (!Batch)
    {   printf("\nPress <Esc> to quit or other key to continue ... ");
        if ((itemp = getch()) == ESC)
        {   puts("\nProgram terminated by user.");
            if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
            if (log_file)
            {   fprintf(log_file,"Program terminated by user.\n");
                fclose(log_file);
                printf("\nFile %s on disk contains messages.\n",log_filename);
            }
            puts("GPR_SAMP finished.");
            if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
            exit(3);
        } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
            getch();
        printf("\r                                                                    \r");
    }

/***** Get the data *****/
    printf("Getting Data ...");
    itemp = GetGprDataAsGrid(FUNC_CREATE,&ParamInfo);
    printf("\r                            \r");
    if (itemp > 0)
    {   printf("\n%s\n",GetGprDataAsGridMsg[itemp]);
        if (log_file) fprintf(log_file,"\n%s\n",GetGprDataAsGridMsg[itemp]);
        if (itemp == 5)  /* data set too large for memory */
        {   unsigned long mem,stack,bytes,requested;

            mem = _memavl();
            switch (ParamInfo.input_datatype)
            {   case -1:  case 1:                    bytes = 1;  break;
                case -2:  case 2:  case 5:           bytes = 2;  break;
                case -3:  case 3:  case 6:  case 4:  bytes = 4;  break;
                case  8:                             bytes = 8;  break;
            }
            requested = ParamInfo.num_cols * ParamInfo.num_rows * bytes;
            stack = requested/mem;
            printf("\nSize of data storage grid (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
            printf("Try using GPR_CNDS first with skip_traces set to %lu or higher.\n",stack+2);
            printf("Try reducing the output file size by reducing the number of samples per trace.\n");
            printf("Also, try increasing the region size of GPR_SAMP.EXE using MODXCONF.\n");
            if (log_file) fprintf(log_file,"Size of data storage grid (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
            if (log_file) fprintf(log_file,"Try using GPR_CNDS first with skip_traces set %lu or higher.\n",stack+2);
            if (log_file) fprintf(log_file,"Try reducing the output file size by reducing the number of samples per trace.\n");
            if (log_file) fprintf(log_file,"Also, try increasing the region size of GPR_SAMP.EXE using MODXCONF.\n");
        }
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("\nFile %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPR_SAMP finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(4);
    }

/***** Resample the data *****/
    printf("Resampling Data.  Please wait ...");
    itemp = ResampleGprData(&ParamInfo);
    printf("\r                                                                           \r");
    if (itemp > 0)
    {   printf("\n%s\n",ResampleGprDataMsg[itemp]);
        GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"%s\n",ResampleGprDataMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("\nFile %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPR_SAMP finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(5);
    }

/***** Save grid to disk *****/
    printf("Saving data to disk ...");
    itemp = SaveGprData(&ParamInfo);
    printf("\r                                \r");
    if (itemp > 0)
    {   printf("\n%s\n",SaveGprDataMsg[itemp]);
        GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"%s\n",SaveGprDataMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("\nFile %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPR_SAMP finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(6);
    }

/***** Free storage and terminate program *****/
    itemp = GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
    if (itemp > 0)
    {   printf("\n%s\n",GetGprDataAsGridMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"%s\n",GetGprDataAsGridMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("\nFile %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPR_SAMP finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }
        exit(7);
    }
    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
    if (log_file)
    {   fprintf(log_file,"End of error messages.  Program terminated normally.\n");
        fprintf(log_file,"GPR_SAMP finished.  Resampled data saved to disk in %s.\n",ParamInfo.dat_outfilename);
        fclose(log_file);
        printf("\nFile %s on disk contains messages.\n",log_filename);
    }
    printf("GPR_SAMP finished.  Resampled data saved to disk in %s.\n",ParamInfo.dat_outfilename);
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
    exit(0);
}
/****************************** GetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 * Determine GPR file storage format.
 * Get information from the appropriate "information" files.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct SampParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <math.h>, <stdio.h>, <stdlib.h>, <string.h>, "gpr_samp.h".
 * Calls: strcpy, printf, fprintf, getch, strstr, strupr, puts, strncat,
 *        atoi, atof, pow,
 *        PrintUsageMsg, GetCmdFileArgs, InitParameters.
 *        ReadOneDztHeader, DeallocInfoStruct,
 *        GetSsHdFile, PrintSsHdInfo, ReadSegyReelHdr, PrintSegyReelHdr,
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR:  Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",

    "GetParameters() ERROR: Problem getting information about input data file.",
    "GetParameters() ERROR: Invalid channel selection for DZT file.",
    "GetParameters() ERROR: Problem getting info from HD file.",
    "GetParameters() ERROR: Problem getting info from SGY file.",
    "GetParameters() ERROR: Problem determining user-defined parameters.",

    "GetParameters() ERROR: No recognizable data/info input files specified.",
    "GetParameters() ERROR: Invalid input data element type .",
    "GetParameters() ERROR: Less than one trace in file.",
    "GetParameters() ERROR: Less than two samples per trace.",
    "GetParameters() ERROR: nanoseconds per sample is <= 0.0.",

    "GetParameters() ERROR: trace_header_size must be a multiple of sample size.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: Sepember 11, 1997
 */
int GetParameters (int argc,char *argv[],struct SampParamInfoStruct *InfoPtr)
{

/***** Declare variables *****/
    char *cptr;
    int  req_args = 2;         /* exe_name and cmd_filename */
    int  i,itemp;              /* scratch variables and counters */
    long samp_bytes;           /* number of bytes in one trace sample */
    long ltemp;                /* scratch variables and counters */
    double fund_freq,Nyq_freq; /* fundamental and Nyquist freq. of a trace */

    /* this group used by GetGprFileType() */
    int  file_hdr_bytes,num_hdrs,trace_hdr_bytes,samples_per_trace;
    int  num_channels,input_datatype,total_time,file_type;
    long num_traces;

    extern int Debug;                        /* beginning of this source */
    extern FILE *log_file;                   /* beginning of this source */
    extern const char *GetGprFileTypeMsg[];  /* from gpr_io.h */

/***** Verify usage *****/
    if (argc < req_args)
    {   PrintUsageMsg();
        return 1;
    }

/***** Initialize information structure *****/
    InitParameters(InfoPtr);
    strncpy(InfoPtr->cmd_filename,argv[1],sizeof(InfoPtr->cmd_filename)-1);

/***** Get user-defined parameters from CMD file *****/
    itemp = GetCmdFileArgs(InfoPtr);
    if (itemp > 0) return itemp;      /* 2 to 4 */

/***** Determine GPR file storage format and essential info *****/
    itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                &samples_per_trace,&num_channels,&num_traces,
                &input_datatype,&total_time,&file_type,
                InfoPtr->dat_infilename);
    if (itemp > 0)
    {   printf("%s\n",GetGprFileTypeMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
        return 5;
    }
    if (file_type == MOD_DZT)  file_type = DZT;
    InfoPtr->storage_format     = file_type;
    InfoPtr->file_header_bytes  = file_hdr_bytes;
    InfoPtr->trace_header_bytes = trace_hdr_bytes;
    InfoPtr->total_traces       = num_traces;
    InfoPtr->total_samps        = samples_per_trace;
    InfoPtr->total_time         = total_time;
    InfoPtr->ns_per_samp        = (double)InfoPtr->total_time / (InfoPtr->total_samps - 1);
    InfoPtr->input_datatype     = input_datatype;

/***** Get necessary information from info file/header *****/
    if (InfoPtr->storage_format == DZT)
    {   int channel,header_size;
        struct DztHdrStruct DztHdr;                /* from gpr_io.h */
        extern const char *ReadOneDztHeaderMsg[];  /* from gpr_io.h */

        /* get first DZT header in file */
        channel = 0;
        itemp = ReadOneDztHeader(InfoPtr->dat_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),channel,
                                 &header_size,&DztHdr);
        if (itemp > 0)
        {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
            if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
            {   puts("Fatal error.");
                return 5;
            }
        }
        if (header_size < 1024)
        {   puts("This DZT header is not consistent with version 5.x SIR-10A software.");
            if (log_file) fprintf(log_file,"This DZT header is not consistent with version 5.x SIR-10A software.\n");
        }
        if (InfoPtr->channel >= num_hdrs) return 6;

        /* get selected header (if different than first) */
        if (InfoPtr->channel > 0)
        {   itemp = ReadOneDztHeader(InfoPtr->dat_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),InfoPtr->channel,
                                 &header_size,&DztHdr);
            if (itemp > 0)
            {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
                if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                {   puts("Fatal error.");
                    return 5;
                }
            }
        }

        /* assign values to info structure */
        InfoPtr->total_samps    = DztHdr.rh_nsamp;
        if      (DztHdr.rh_bits ==  8) InfoPtr->input_datatype = -1;
        else if (DztHdr.rh_bits == 16) InfoPtr->input_datatype = -2;
        else if (DztHdr.rh_bits == 32) InfoPtr->input_datatype = -3;
        InfoPtr->total_time       = DztHdr.rh_range;
        InfoPtr->ns_per_samp      = DztHdr.rh_range/(DztHdr.rh_nsamp-1);
        InfoPtr->first_proc_samp  = 2;
        InfoPtr->header_samps = 0;
        if (Debug)
        {   printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            printf("num_hdrs = %d\n",num_hdrs);
            getch();
        }
        
        /* Force output extension to DZT */
        cptr = strchr(InfoPtr->dat_outfilename,'.');
        if (cptr != NULL)
            *cptr = 0;      /* truncate string at . */
        strcat(InfoPtr->dat_outfilename,".DZT");

    } else if (InfoPtr->storage_format == DT1)
    {   int print_it;
        struct SsHdrInfoStruct  HdInfo;      /* buffer to hold info from HD file */
        extern const char *GetSsHdFileMsg[]; /* from gpr_io.h */

        /* Get info from HD file */
        InitDt1Parameters(&HdInfo);
        if (Debug)  print_it = 1;
        else        print_it = 0;
        itemp = GetSsHdFile(print_it,&HdInfo,InfoPtr->hd_infilename);
        if (itemp > 0)
        {   printf("%s\n",GetSsHdFileMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetSsHdFileMsg[itemp]);
            return 7;
        }
        if (Debug)
        {   PrintSsHdInfo("stdout",InfoPtr->hd_infilename,&HdInfo);
            if (!getch()) getch();
        }

        InfoPtr->input_datatype = 2;
        InfoPtr->total_traces   = HdInfo.num_traces;
        InfoPtr->total_samps    = HdInfo.num_samples;
        InfoPtr->total_time     = HdInfo.total_time_ns;
        InfoPtr->ns_per_samp    = (double)HdInfo.total_time_ns/(HdInfo.num_samples-1);
        InfoPtr->first_proc_samp = 0;
        InfoPtr->header_samps = InfoPtr->trace_header_bytes/2;    /* 128 / 2-byte shorts */
        
        if (Debug)
        {   printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            getch();
        }

        /* Force output extension to DT1 */
        cptr = strchr(InfoPtr->dat_outfilename,'.');
        if (cptr != NULL)
            *cptr = 0;      /* truncate string at . */
        strcat(InfoPtr->dat_outfilename,".DT1");

    } else if (InfoPtr->storage_format == SGY)
    {   char  *cp,cmdstr[30];
        char string[40][80];    /* should have 3200 continuous bytes */
        int swap_bytes,str_num,max_records,num,found;
        int comment_size,record_length,samp_bytes;  /* bytes */
        long num_traces;
        struct SegyReelHdrStruct hdr;            /* from gpr_io.h */
        extern const char *ReadSegyReelHdrMsg[]; /* from gpr_io.h */

        itemp = ReadSegyReelHdr(InfoPtr->dat_infilename,&swap_bytes,
                                &num_traces,&hdr);
        if (itemp > 0)
        {   printf("%s\n",ReadSegyReelHdrMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadSegyReelHdrMsg[itemp]);
            return 8;
        }
        if (Debug) PrintSegyReelHdr(swap_bytes,num_traces,"stdout",
                                    InfoPtr->dat_infilename,&hdr);

        switch(hdr.format)
        {   case 1:          
                InfoPtr->input_datatype = 4;  
                samp_bytes = 4;
                break;
            case 2:  case 4: 
                InfoPtr->input_datatype = 3;  
                samp_bytes = 4;
                break;
            case 3:          
                InfoPtr->input_datatype = 2;  
                samp_bytes = 2;
                break;
            default: break;
        }
        InfoPtr->total_traces   = num_traces;
        InfoPtr->total_samps    = hdr.hns;
        InfoPtr->ns_per_samp    =
                 (hdr.hns - 1) * ((double)hdr.hdt / 1000.); /* was picosec */

        /* Extract S&S info from SEG-Y reel header */
        /* copy ASCII part of header and terminate strings */
        comment_size = sizeof(hdr.comment);
        record_length = 80;                          /* bytes */
        max_records = comment_size / record_length;  /* should be 40 */
        memcpy((void*)string,(void*)hdr.comment,
                (size_t)_MINN(comment_size,3200));
        for (i=0; i<max_records; i++)  string[i][record_length-1] = 0;
        if (Debug)
        {   for (i=0; i<40; i++)
            {   printf("string[%d]=%s\n",i,string[i]);
                getch();
            }
        }
        for (str_num=0; str_num<40; str_num++)
        {   cmdstr[0] = 0;                      /* set to 0 so strncat works right */
            if (Debug)
            {   printf("string[%d]=%s\n",str_num,string[str_num]);
                getch();
            }
            cp = strchr(string[str_num],'=');   /* look for equal sign */
            if (cp == NULL) continue;           /* invalid command line so ignore */
            num = (int)(cp-string[str_num]);    /* number of chars before equal sign */
            strncat(cmdstr,string[str_num],(size_t)num);  /* copy to working string */
            TrimStr(cmdstr);                    /* remove leading and trailing blanks */
            if (Debug) printf("cmdstr = %s\n",cmdstr);
            found = -1;
            i = 0;
            while (SEGY_ASCII_CMDS[i])     /* requires last member to be NULL */
            {   if (strstr(strupr(cmdstr),SEGY_ASCII_CMDS[i]) != NULL)
                {   found = i;             /* store index into SEGY_ASCII_CMDS[] */
                    break;
                }
                i++;
            }
            if (found == -1) continue;    /* bad or missing command word */
            cp++;                         /* step 1 past equal sign */
            TrimStr(cp);                  /* remove leading and trailing blanks */
            if (Debug)  printf("cp=%s\n",cp);
            switch (found)  /* cases depend on same order in SEGY_ASCII_CMDS[] */
            {   case 0:
                    itemp = atoi(cp);
                    if (itemp > 0)  InfoPtr->total_traces = itemp;
                    break;
                case 1:
                    itemp = atoi(cp);
                    if (itemp > 0)  InfoPtr->total_samps = itemp;
                    break;
                case 3:
                    itemp = atoi(cp);
                    if (itemp > 0) InfoPtr->total_time = atoi(cp);
                    break;
                default:
                    break;
                }
        }
        if (InfoPtr->total_samps > 1 && InfoPtr->total_time > 0.0)
            InfoPtr->ns_per_samp = InfoPtr->total_time / (InfoPtr->total_samps - 1);
        else
            InfoPtr->ns_per_samp = 0;
        InfoPtr->first_proc_samp  = 0;
        InfoPtr->header_samps = InfoPtr->trace_header_bytes/samp_bytes;

        if (Debug)
        {   printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            getch();
        }

    } else if (InfoPtr->storage_format == USR)  /* USR must equal 4 */
    {   itemp = GetOtherInfo(InfoPtr);
        if (Debug)
        {   puts("After GetOtherInfo():");
            printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            getch();
        }
        if (itemp) return 9;
    } else  /* unknown or unspecified storage format */
    {   return 10;
    }

/***** Finish initializing variables and verifying valid ranges *****/
    if (Debug) puts("Start of initialize and check section");

    /* Check for valid ranges in required parameters */
    switch (InfoPtr->input_datatype)
    {   case 1: case -1:                    samp_bytes = 1;  break;
        case 2: case -2: case -5:           samp_bytes = 2;  break;
        case 3: case -3: case  4: case -6:  samp_bytes = 4;  break;
        case 8:                             samp_bytes = 8;  break;
        default:
            return 11;
            /* break;   ** unreachable code */
    }
    if (InfoPtr->total_traces < 1)   return 12;
    if (InfoPtr->total_samps < 3)    return 13;
    if (InfoPtr->ns_per_samp <= 0.0) return 14;

    /* Limit first and last traces used to valid ranges */
    if (InfoPtr->first_trace < 0)
        InfoPtr->first_trace = 0;
    if (InfoPtr->last_trace < 0)
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->last_trace < InfoPtr->first_trace)
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->last_trace > InfoPtr->total_traces - 1)
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->first_trace >= InfoPtr->last_trace)
        InfoPtr->first_trace = 0;

    /* Limit first and last samples used to valid ranges */
    if (InfoPtr->first_samp < 0)
        InfoPtr->first_samp = 0;
    if (InfoPtr->last_samp < 0)
        InfoPtr->last_samp = InfoPtr->total_samps - 1;
    if (InfoPtr->last_samp < InfoPtr->first_samp)
        InfoPtr->last_samp = InfoPtr->total_samps - 1;
    if (InfoPtr->last_samp < InfoPtr->first_samp + 2)
        InfoPtr->last_samp = InfoPtr->first_samp + 2;
    if (InfoPtr->last_samp > InfoPtr->total_samps - 1)
        InfoPtr->last_samp = InfoPtr->total_samps - 1;
    if (InfoPtr->first_samp >= InfoPtr->last_samp)
        InfoPtr->first_samp = 0;
    if (InfoPtr->first_samp > InfoPtr->last_samp - 2)
        InfoPtr->first_samp = InfoPtr->last_samp - 2;
    if (InfoPtr->first_samp < 0)
        InfoPtr->first_samp = 0;

    /* Calculate new total samples and new total time */
	if (InfoPtr->first_samp > 0 || InfoPtr->last_samp <  (InfoPtr->total_samps - 1))
    {   InfoPtr->new_total_samps = InfoPtr->last_samp - InfoPtr->first_samp + 1;
        InfoPtr->new_total_time  = (InfoPtr->new_total_samps - 1) * InfoPtr->ns_per_samp;
    } else
    {   InfoPtr->new_total_samps = InfoPtr->total_samps;
        InfoPtr->new_total_time  = InfoPtr->total_time;
    }

    /* Verify whole number of samples in each trace header */
    if (InfoPtr->trace_header_bytes > 0)
    {   itemp = InfoPtr->trace_header_bytes/samp_bytes;  /* truncation expected */
        if ((itemp * samp_bytes) != InfoPtr->trace_header_bytes)  return 15;
    }
    
    /* Check for number of new (re)samples and new sample rate */
    if (InfoPtr->resample > 0 && InfoPtr->resample < 3)  
		InfoPtr->resample = 0;
    if (InfoPtr->resample > 65536L) InfoPtr->resample = 65536L;
    if (InfoPtr->resample >= 3)
    {   if (InfoPtr->storage_format == DZT)
        {   /* check that ->resample is a power of 2 */
            assert(InfoPtr->resample <= 65536L);
            for (i=1; i<16; i++)
            {   if (InfoPtr->resample > pow(2,(double)i) &&    InfoPtr->resample < pow(2,(double)(i+1)))
                {   InfoPtr->resample = pow(2,(double)(i+1));  /* force value */
                    break;     /* out of for loop */
                }
            }
        }
        InfoPtr->new_ns_per_samp = InfoPtr->new_total_time / (InfoPtr->resample - 1);
    } else
        InfoPtr->new_ns_per_samp = InfoPtr->ns_per_samp;

    /* Assign number of columns and rows in the grid */
    ltemp = _MAXX(InfoPtr->total_samps,InfoPtr->resample);
    InfoPtr->num_rows = InfoPtr->header_samps + ltemp;
    InfoPtr->num_cols = InfoPtr->last_trace - InfoPtr->first_trace + 1;

    /* Assign FFT filter frequencies */
    fund_freq = 1.0E09 / ( (InfoPtr->total_samps-1) * (InfoPtr->ns_per_samp) );
    Nyq_freq  = 1.0E09 / (InfoPtr->ns_per_samp*2.0);  
    InfoPtr->lo_freq_cutoff = fund_freq;   /* no high-pass filter */
    InfoPtr->hi_freq_cutoff = Nyq_freq;    /* no low-pass filter */
    if (InfoPtr->new_total_time != InfoPtr->total_time)
        InfoPtr->lo_freq_cutoff = 1.0E09 / ( (InfoPtr->new_total_samps - 1) * InfoPtr->ns_per_samp );
    if (InfoPtr->new_ns_per_samp != InfoPtr->ns_per_samp)
        InfoPtr->hi_freq_cutoff = 1.0E09 / ( 2 * _MAXX(InfoPtr->new_ns_per_samp,InfoPtr->ns_per_samp) );

    return 0;
}
/****************************** GetOtherInfo() ******************************/
/* Verify info read from command file and calculate remaining minimum
 * required values.
 *
 * Parameters:
 *  struct SampParamInfoStruct *InfoPtr - adress of information structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, "gpr_disp.h"
 * Calls: fopen, fclose, ftell, fseek.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: 
 */
int GetOtherInfo (struct SampParamInfoStruct *InfoPtr)
{
    int error = 0;
    long samp_size;
    FILE *infile;

    /* Make sure info read correctly from command file */
    if (InfoPtr->input_datatype == 0)    error = 1;
    if (InfoPtr->total_samps == 0)       error = 2;
    if (InfoPtr->file_header_bytes < 0)  error = 3;
    if (InfoPtr->trace_header_bytes < 0) error = 4;
    if (InfoPtr->total_samps < 1)        error = 6;

    /* Calculate number of bytes in 1 trace sample */
    switch (InfoPtr->input_datatype)
    {   case 1:  case -1:                    samp_size = 1;  break;
        case 2:  case -2:  case -5:          samp_size = 2;  break;
        case 3:  case -3:  case 4: case -6:  samp_size = 4;  break;
        case 8:                              samp_size = 8;  break;
        default:  error = 7;  break;
    }

    /* Calculate the rest of minimal required info */
    if (!error)
    {   InfoPtr->ns_per_samp = InfoPtr->total_time/(InfoPtr->total_samps-1);
        InfoPtr->first_proc_samp = 0;
        InfoPtr->header_samps = InfoPtr->trace_header_bytes/samp_size;

        if ((infile = fopen(InfoPtr->dat_infilename,"rb")) == NULL)
        {   error = 8;
            printf("ERROR occurred attempting to open file %s\n",InfoPtr->dat_infilename);
            if (log_file) fprintf(log_file,"ERROR occurred attempting to open file %s\n",InfoPtr->dat_infilename);
        } else
        {   /* determine number of GPR traces in file */
            long start_byte,last_byte,num_bytes;

            fseek(infile,(long)InfoPtr->file_header_bytes,SEEK_SET);
            start_byte = ftell(infile);        /* get current location (byte) */
            fseek(infile, 0L, SEEK_END);       /* go to end of file */
            last_byte = ftell(infile);         /* get last location (byte) */
            num_bytes = last_byte - start_byte;
            InfoPtr->total_traces = num_bytes /
                ((InfoPtr->total_samps * samp_size) + InfoPtr->trace_header_bytes);
            fclose(infile);
        }
    }

    return error;
}
/****************************** PrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>.
 * Calls: puts.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: September 11, 1997
 */
void PrintUsageMsg (void)
{
    char str[10];
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("                                                                               ");
    puts("###############################################################################");
    puts("   Usage: GPR_SAMP cmd_filename");
    puts("     Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
    puts("   This program resamples radar data and saves the resulting data to disk.");
	puts("	   The number of trace and/or samples can be reduced and the number of");
    puts("     samples per trace can be increased or decreased.");
    puts("\n   Required command line arguments:");
    puts("     cmd_filename - name of ASCII command file that follows the \"CMD\" format.");
    puts("\n   Inspect \"GPR_SAMP.CMD\" for command file format and valid parameters.");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_SAMP_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** InitParameters() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "gpr_samp.h".
 * Calls: none.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions:  
 */
void InitParameters (struct SampParamInfoStruct *InfoPtr)
{
    int i;

    InfoPtr->cmd_filename[0]    = 0;
    InfoPtr->dat_infilename[0]  = 0;
    InfoPtr->hd_infilename[0]   = 0;
    InfoPtr->dat_outfilename[0] = 0;
    InfoPtr->hd_outfilename[0]  = 0;
    InfoPtr->storage_format     = 0;
    InfoPtr->input_datatype     = 0;
    InfoPtr->file_header_bytes  = 0;
    InfoPtr->trace_header_bytes = 0;
    InfoPtr->channel            = 0;
    InfoPtr->first_proc_samp    = 0L;
    InfoPtr->header_samps       = 0;
    InfoPtr->total_samps        = 0L;
    InfoPtr->total_traces       = 0L;
    InfoPtr->total_time         = 0.0;
    InfoPtr->ns_per_samp        = 0.0;
    InfoPtr->first_trace        = -1L;
    InfoPtr->last_trace         = -1L;
    InfoPtr->first_samp         = -1L;
    InfoPtr->last_samp          = -1L;
    InfoPtr->new_total_samps    = 0L;
    InfoPtr->new_total_time     = 0.0;
    InfoPtr->new_ns_per_samp    = 0.0;
    InfoPtr->resample           = 0L;
    InfoPtr->lo_freq_cutoff     = -1.0;   /* MHz */
    InfoPtr->hi_freq_cutoff     = -1.0;   /* MHz */
    InfoPtr->preprocFFT         = TRUE;
    for (i=0; i<sizeof(InfoPtr->proc_hist); i++) InfoPtr->proc_hist[i] = 0;
    InfoPtr->created            = FALSE;
    InfoPtr->grid               = NULL;
    InfoPtr->num_cols           = 0L;
    InfoPtr->num_rows           = 0L;
}
/***************************** GetCmdFileArgs() *****************************/
/* Get processing parameters from user from the DOS command line.
 * Valid file commands:
const char *GPR_SAMP_CMDS[] =
{   "batch","debug","dat_infilename","dat_outfilename","file_header_bytes",
    "trace_header_bytes","samples_per_trace","total_time","input_datatype","channel",
    "first_trace","last_trace","first_samp","last_samp","resample",
    "preprocFFT",NULL,
} ;
 * Parameter list:
 *  struct SampParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <stdio.h>, <stdlib.h>, <string.h>, "gpr_samp.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, strlwr (non-ANSI), strupr (non-ANSI), Trimstr, strncpy,
 *        assert, atol.
 * Returns: 0 on success,
 *         >0 on error (offsets 2, 3, and 4 into GetParametersMsg[] strings).
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: 
 */
int GetCmdFileArgs (struct SampParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];

    int found,i,num,itemp,have_string;
    int dat_in_found      = FALSE;
    int dat_out_found     = FALSE;
    long ltemp;
    double dtemp;
    FILE *infile          = NULL;
    extern int Debug;
    extern const char *GPR_SAMP_CMDS[];

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
        while (GPR_SAMP_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_SAMP_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_SAMP_CMDS[] */
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
        {   printf("cp = %s\n",cp);
            getch();
        }
#endif
        switch (found)  /* cases depend on same order in GPR_SAMP_CMDS[] */
        {   case 0:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0) Batch = FALSE;
                else            Batch = TRUE;
                break;
            case 1:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0) Debug = FALSE;
                else            Debug = TRUE;
                break;
            case 2:
                if (Debug) printf("case 11: cp = %s\n",cp);
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dat_infilename,cp,sizeof(InfoPtr->dat_infilename)-1);
                strcat(InfoPtr->dat_infilename,"\0");
                strupr(InfoPtr->dat_infilename);
                if (strstr(InfoPtr->dat_infilename,".DT1") != NULL)
                {   strcpy(InfoPtr->hd_infilename,InfoPtr->dat_infilename);
                    cp2 =  strstr(InfoPtr->hd_infilename,".");
                    *cp2 = 0;
                    strcat(InfoPtr->hd_infilename,".HD");
                    if (Debug) printf("info file = %s\n",InfoPtr->hd_infilename);
                }
                if (InfoPtr->dat_infilename[0]) dat_in_found = TRUE;
                if (Debug) printf("data=%s info=%s fmt=%d\n",InfoPtr->dat_infilename,
                                  InfoPtr->hd_infilename,InfoPtr->storage_format);
                break;
            case 3:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dat_outfilename,cp,sizeof(InfoPtr->dat_outfilename)-1);
                strcat(InfoPtr->dat_outfilename,"\0");
                strupr(InfoPtr->dat_outfilename);
                if (InfoPtr->dat_outfilename[0]) dat_out_found = TRUE;
                cp2 = strstr(InfoPtr->dat_outfilename,".DT1");
                if (cp2 != NULL)
                {   strncpy(InfoPtr->hd_outfilename,InfoPtr->dat_outfilename,
                            (size_t)(cp2-InfoPtr->dat_outfilename));
                    strcat(InfoPtr->hd_outfilename,".HD\0");
                }
                break;
            case 4:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->file_header_bytes = itemp;
                if (Debug) printf("file_header_bytes = %d\n",InfoPtr->file_header_bytes);
                break;
            case 5:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->trace_header_bytes = itemp;
                if (Debug) printf("trace_header_bytes = %d\n",InfoPtr->trace_header_bytes);
                break;
            case 6:    /* samples_per_trace */
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->total_samps = itemp;
                if (Debug) printf("total_samps = %d\n",InfoPtr->total_samps);
                break;
            case 7:
                dtemp = atof(cp);
                if (dtemp > 0.0)  InfoPtr->total_time = dtemp;
                if (Debug) printf("total_time = %g\n",InfoPtr->total_time);
                break;
            case 8:
                itemp = atoi(cp);
                if (Debug) printf("itemp (input_datatype) = %d\n",itemp);
                switch (itemp)
                {   case -6:  case -5:  case -3:  case -2:  case -1:
                    case  1:  case  2:  case  3:  case  4:  case  8:
                        InfoPtr->input_datatype = itemp;
                        break;
                    default:
                        InfoPtr->input_datatype = 0;
                        break;
                }
                if (Debug) printf("input_datatype = %d\n",InfoPtr->input_datatype);
                break;
            case 9:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->channel = itemp;
                break;
            case 10:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->first_trace = itemp;
                break;
            case 11:
                itemp = atoi(cp);
                if (itemp > 0)   InfoPtr->last_trace = itemp;
                break;
            case 12:
                ltemp = atol(cp);
                if (ltemp >= 0)  InfoPtr->first_samp = ltemp;
                break;
            case 13:
                ltemp = atol(cp);
                if (ltemp > 2)   InfoPtr->last_samp = ltemp;
                break;
            case 14:
                itemp = atoi(cp);
                if (itemp >= 3)  InfoPtr->resample = itemp;
                break;
            case 15:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0) InfoPtr->preprocFFT = FALSE;
                else            InfoPtr->preprocFFT = TRUE;
                break;
            default:
                break;
        }
    }
    fclose(infile);
    if (!dat_out_found)  return 3;
    if (!dat_in_found)   return 4;
    return 0;
}
/**************************** DisplayParameters() ***************************/
/* Display parameters to CRT.  Also calculates approximate storage requirements.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, "gpr_samp.h".
 * Calls: printf, puts.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: November 20, 1997
 */
void DisplayParameters (struct SampParamInfoStruct *InfoPtr)
{
    char str[10];
    int lines;
    long grid_size;
    double fund_freq,Nyq_freq; /* fundamental and Nyquist freq. of a trace */
    extern int Debug,Batch,ANSI_THERE;

    if (Debug) puts("DisplayParameters()");

    grid_size = InfoPtr->num_cols * InfoPtr->num_rows;
    switch (InfoPtr->input_datatype)
    {   case 2: case -2: case -5:         grid_size *= 2; break;
        case 3: case -3: case 4: case -6: grid_size *= 4; break;
        case 8:                           grid_size *= 8; break;
    }

    /* if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); */ /* red on black */
    if (ANSI_THERE) printf("%c%c33;40m",0x1B,0x5B); /* yellow on black */
    sprintf(str,"%4.2f",GPR_SAMP_VER);
    printf("\nGPR_SAMP v. %s (Latest Program Compile Date = %s at %s):\n",str,__DATE__,__TIME__);
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

    printf("cmd_filename          = %s\n",strupr(InfoPtr->cmd_filename));
    lines = 2;

    printf("dat_infilename        = %s",strupr(InfoPtr->dat_infilename));
    if (InfoPtr->storage_format == DT1)
    {   printf("  hd_infilename = %s\n",strupr(InfoPtr->hd_infilename));
    } else
        puts("");
    lines ++;

    printf("dat_outfilename       = %s",strupr(InfoPtr->dat_outfilename));
    if (InfoPtr->storage_format == DT1)
    {   printf("  inf_outfilename = %s\n",strupr(InfoPtr->hd_outfilename));
    } else
        puts("");
    lines ++;

    switch (InfoPtr->storage_format)
    {   case DZT:   puts("data storage format is GSSI SIR-10, SIR-2, or modified DZT");  break;
        case DT1:   puts("data storage format is S&S PulseEKKO"); break;
        case SGY:   puts("data storage format is SEG-Y");         break;
        case USR:
            puts("data storage format is USER DEFINED");
            printf("file_header_bytes = %d  trace_header_bytes = %d\n",
                    InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes);
            break;
        default:  puts("unrecognized data storage format");    break;
    }
    lines ++;

    switch (InfoPtr->input_datatype)
    {   case -1: puts("data element type is 8-bit unsigned integer");  break;
        case  1: puts("data element type is 8-bit signed integer");    break;
        case -2: puts("data element type is 16-bit unsigned integer"); break;
        case  2: puts("data element type is 16-bit signed integer");   break;
        case -3: puts("data element type is 32-bit unsigned integer"); break;
        case  3: puts("data element type is 32-bit signed integer");   break;
        case -5: puts("data element type is 12-bit unsigned integer"); break;
        case -6: puts("data element type is 24-bit unsigned integer"); break;
        case  4: puts("data element type is 32-bit floating point");    break;
        case  8: puts("data element type is 64-bit floating point");    break;
        default: puts("unknown data element type"); break;
    }
    lines ++;
    if (InfoPtr->storage_format == DZT) 
    {   printf("DZT channel = %d \n",InfoPtr->channel);
        lines ++;
    }

    printf("File header bytes = %d  Trace header bytes = %d\n",
        InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes);
    lines ++;

    printf("total_samps = %ld  total_traces = %ld\n",
        InfoPtr->total_samps,InfoPtr->total_traces);
    lines ++;
    printf("first_trace = %ld  last_trace = %ld\n",
        InfoPtr->first_trace,InfoPtr->last_trace);
    lines ++;

    printf("first_samp = %ld  last_samp = %ld new_total_samps = %ld\n",
        InfoPtr->first_samp,InfoPtr->last_samp,InfoPtr->new_total_samps);
    lines ++;

    printf("new_total_time = %g\n",InfoPtr->new_total_time);
	lines ++;
	    
    printf("first_proc_samp = %ld  ns_per_samp = %g\n",
        InfoPtr->first_proc_samp,InfoPtr->ns_per_samp);
    lines ++;

    printf("num_cols = %ld  num_rows = %ld  est. grid size = %ld bytes (%g MB)\n",
        InfoPtr->num_cols,InfoPtr->num_rows,grid_size,(double)grid_size/(1024.0*1024.0));
    lines ++;

    printf("resample = %d  new_ns_per_samp = %g\n",
        InfoPtr->resample,InfoPtr->new_ns_per_samp);
    lines ++;

    Nyq_freq  = 1.0E09 / (InfoPtr->ns_per_samp*2.0);
    fund_freq = 1.0E09 / ( (InfoPtr->total_samps-1) * (InfoPtr->ns_per_samp) );
    printf("lo_freq_cutoff  = %9.3f MHz    Fundamental = %9.3f MHz\n",
        InfoPtr->lo_freq_cutoff/1.E06,fund_freq/1.0E06);
    lines++;
    printf("hi_freq_cutoff  = %9.3f MHz    Nyquist     = %9.3f MHz\n",
        InfoPtr->hi_freq_cutoff/1.E06,Nyq_freq/1.E06);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("preprocFFT      = %s (if frequency filtering required)\n",
        InfoPtr->preprocFFT ? "TRUE" : "FALSE");
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    /* Issue messages */
    if (Debug)       puts("In TESTing mode.");
    else if (Batch)  puts("In Batch processing mode.");
}
/*************************** GetGprDataAsGrid() *****************************/
/* Get the data/info from the appropriate files.
 * Allocate/De-allocate storage for grid[][].
 * Verify valid parameters and ranges.
 *
 * NOTE: The "created" member of struct SampParamInfoStruct restricts the
 *       application of the struct to one data set and one grid at a time.
 *       To read multiple data sets and allocate multiple grids, either
 *       de-allocate the first one OR declare multiple structs to hold the
 *       information and pointers.
 *
 * Method:
 *   1. Declare variables
 *   2. De-allocate storage if command==FUNC_DESTROY and return, else
 *   3. Test for balanced calls
 *   4. Allocate storage for grid[][]
 *   5. Copy disk file data into grid[][]
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct SampParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "gpr_samp.h".
 * Calls: strcpy, malloc, free, printf, puts,
 *        GetDztChSubGrid8, GetDztChSubGrid16, GetGprSubGrid.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *GetGprDataAsGridMsg[] =
{   "GetGprDataAsGrid(): No errors.",
    "GetGprDataAsGrid() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetGprDataAsGrid() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "GetGprDataAsGrid() ERROR: Invalid input data element type .",
    "GetGprDataAsGrid() ERROR: No recognized data/info input files specified.",

    "GetGprDataAsGrid() ERROR: Not able to allocate storage for grid.",
    "GetGprDataAsGrid() ERROR: Problem getting data from input file.",
} ;
 NOTE: message 5 is checked by calling routine (main).
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: January 30, 1996; September 5, 1997
 */
int GetGprDataAsGrid (int command,struct SampParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/
    long i,j,itemp;             /* scratch variables and counters */
    long ptr_size;              /* size of element in first dimension of grid */
    long grid_col_size;         /* number of bytes in one trace */
    long samp_bytes;            /* number of bytes in one trace sample */

/***** Deallocate storage *****/
    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->created) return 1;
        for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return 0;
    }

/***** Test for balanced calls *****/
    if (InfoPtr->created) return 2;

/***** Allocate storage for grid[][] with same type as input data sets *****/
    /* get row pointer size */
    switch (InfoPtr->input_datatype)
    {   case 1: case -1:
            ptr_size = sizeof(char *);
            samp_bytes = 1;
            break;
        case 2: case -2: case -5:
            ptr_size = sizeof(short *);
            samp_bytes = 2;
            break;
        case 3: case -3: case -6:
            ptr_size = sizeof(long *);
            samp_bytes = 4;
            break;
        case 4:
            ptr_size = sizeof(float *);
            samp_bytes = 4;
            break;
        case 8:
            ptr_size = sizeof(double *);
            samp_bytes = 8;
            break;
        default:
            return 3;
    }
    grid_col_size = InfoPtr->num_rows * samp_bytes;
    InfoPtr->grid = (void **)malloc((size_t)(InfoPtr->num_cols * ptr_size));
    if (InfoPtr->grid)
    {   for (i=0; i<InfoPtr->num_cols; i++)
        {   InfoPtr->grid[i] = (void *)malloc((size_t)grid_col_size);
            if (InfoPtr->grid[i] == NULL)
            {   for (j=0; j<i; j++) free(InfoPtr->grid[j]);
                free(InfoPtr->grid);
                InfoPtr->grid = NULL;
                return 5;
            }
        }
    } else
    {   return 5;
    }

/***** Get the data *****/
    itemp = 0;
    switch (InfoPtr->storage_format)
    {   case DZT:
        {   extern const char *GetDztChSubGrid8Msg[];   /* from gpr_io.h */
            extern const char *GetDztChSubGrid16Msg[];  /* from gpr_io.h */
            long num_rows;

            num_rows = (InfoPtr->trace_header_bytes/samp_bytes) + InfoPtr->total_samps;
            if (InfoPtr->input_datatype == -1)
            {   itemp = GetDztChSubGrid8(InfoPtr->dat_infilename,InfoPtr->channel,
                        InfoPtr->first_trace,InfoPtr->num_cols,num_rows,
                        (unsigned char **)(InfoPtr->grid));
                if (itemp > 0)
                {   printf("\n%s\n",GetDztChSubGrid8Msg[itemp]);
                    if (log_file) fprintf(log_file,"%s\n",GetDztChSubGrid8Msg[itemp]);
                }
            } else if (InfoPtr->input_datatype == -2)
            {   itemp = GetDztChSubGrid16(InfoPtr->dat_infilename,InfoPtr->channel,
                        InfoPtr->first_trace,InfoPtr->num_cols,num_rows,
                        (unsigned short **)(InfoPtr->grid));
                if (itemp > 0)
                {   printf("\n%s\n",GetDztChSubGrid16Msg[itemp]);
                    if (log_file) fprintf(log_file,"%s\n",GetDztChSubGrid16Msg[itemp]);
                }
            } else
            {   puts("\nSorry, only 8-bit and 16-bit GSSI data supported at this time.");
                if (log_file) fprintf(log_file,"Sorry, only 8-bit and 16-bit GSSI data supported at this time.");
                itemp = 1;
            }
            break;
        }
        case DT1:  case SGY:  case USR:
        {   extern const char *GetGprSubGridMsg[];

            itemp = GetGprSubGrid(InfoPtr->dat_infilename,InfoPtr->input_datatype,
                         InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes,
                         InfoPtr->first_trace,InfoPtr->total_samps,
                         InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->grid);
            if (itemp > 0)
            {   printf("\n%s\n",GetGprSubGridMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetGprSubGridMsg[itemp]);
            }
            break;
        }
        default:
            return 4;
    }
    if (itemp > 0)  /* problem getting data */
    {   /* deallocate storage and return error code */
        for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return 6;
    } else
    {   /* flag structure and return success */
        InfoPtr->created = TRUE;
        return 0;
    }
}
/***************************** ResampleGprData() *****************************/
/* This is the driver routine for processing the GPR data.
 *
 * NOTES: Maximum trace length = 2 exp 16 = 65536 samples (64 K).
 *
 * Parameter list:
 *  struct SampParamInfoStruct *ParamInfoPtr - address of information structure
 *
 * Requires: <float.h>, <math.h>, <stdlib.h>, <string.h>, "jl_util1.h", "gpr_samp.h".
 * Calls: fprintf,
 *         FftFilterGridTraces, RemGridGlobBckgrnd, RemGrdWindBckgrnd,
 *         SmoothGridHorizontally, SmoothGridVertically, AdjustMeanGridTraces,
 *         RescaleGrid, StackGrid, ApplyGridTempMedFilter,
 *         ChangeGridRangeGain, ApplyGridSpatMedFilter, Spline.
 * Returns:  0 on success, or
 *           >0 if an error has occurred.
 *
const char *ResampleGprDataMsg[] =
{   "ResampleGprData(): No errors.",
    "ResampleGprData() ERROR: Unable to allocate temporary storage.",
    "ResampleGprData() ERROR: Error returned from FFT filter function - see LOG file.",
    "ResampleGprData() ERROR: Error returned from Spline function - see LOG file.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: September 10, 1997, September 11, 1997
 */
int ResampleGprData (struct SampParamInfoStruct *InfoPtr)
{
    int    itemp,istep;
    int    samp_slide;
    long   trace,samp,grid_samp,n,nnew,last_samp;
    long   num_cols,num_rows,header_samps,first_proc_samp;
    double *x,*y,*xnew,*ynew;
    void **grid = InfoPtr->grid;
    extern FILE *log_file;
    extern const char *ResampleGprDataMsg[];
    extern const char *FftFilterGridTracesMsg[];
    extern const char *SplineMsg[];

/***** Select subset first if requested *****/
    /* Slide samples first */
    last_samp = InfoPtr->last_samp;
    if (InfoPtr->first_samp > 0 && InfoPtr->first_samp > InfoPtr->first_proc_samp)
    {   samp_slide = InfoPtr->first_proc_samp - InfoPtr->first_samp;
        last_samp += samp_slide;
        SlideSamples(InfoPtr->num_cols,InfoPtr->num_rows,
            InfoPtr->first_proc_samp,(long)InfoPtr->header_samps,InfoPtr->input_datatype,
            samp_slide,InfoPtr->grid);
    }

    /* Fill bottom of trace with median value if last_samp requested */
	istep = InfoPtr->num_cols/10;
    if (istep < 1) istep = 1;
    if (last_samp < InfoPtr->total_samps-1 )
    {   grid = InfoPtr->grid;
        num_cols = InfoPtr->num_cols;
        num_rows = InfoPtr->num_rows;
        header_samps = InfoPtr->header_samps;

        switch (InfoPtr->input_datatype)
        {   case -1:
            {   unsigned char **uc_pp = (unsigned char **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        uc_pp[trace][samp] = 128;
                }
                break;
            }
            case 1:
            {   char **c_pp = (char **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        c_pp[trace][samp] = 0;
                }
                break;
            }
            case -2:
            {   unsigned short **us_pp = (unsigned short **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        us_pp[trace][samp] = 32768L;
                }
                break;
            }
            case -5:
            {   unsigned short **us_pp = (unsigned short **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        us_pp[trace][samp] = 2048;
                }
                break;
            }
            case 2:
            {   short **s_pp = (short **)grid;
        
                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        s_pp[trace][samp] = 0;
                }
                break;
            }
            case -3:
            {   unsigned long **ul_pp = (unsigned long **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        ul_pp[trace][samp] = 2147483647L;
                }
                break;
            }
            case -6:
            {   unsigned long **ul_pp = (unsigned long **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        ul_pp[trace][samp] = 8388608L;
                }
                break;
            }
            case 3:
            {   long **l_pp = (long **)grid;

                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        l_pp[trace][samp] = 0;
                }
                break;
            }
            case 4:
            {   float **f_pp = (float **)grid;

                /* Warning: Assumes centered about 0 */
                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        f_pp[trace][samp] = 0;
                }
                break;
            }
            case 8:
            {   double **d_pp = (double **)grid;

                /* Warning: Assumes centered about 0 */
                for (trace=0; trace<num_cols; trace++)
	            {   if (!trace%istep) printf(".");
                    for (samp=header_samps+last_samp; samp<num_rows; samp++)
                        d_pp[trace][samp] = 0;
                }
                break;
            }
        }
    }

/***** Resample if requested *****/
    if (InfoPtr->resample < 3 || InfoPtr->resample > 65536L)  
    return 0;  /* nothing more to do */
            
    /* Allocate temporary storage */
    n    = InfoPtr->new_total_samps;
    nnew = InfoPtr->resample;
    x    = (double *)malloc(n * sizeof(double));
    y    = (double *)malloc(n * sizeof(double));
    xnew = (double *)malloc(nnew * sizeof(double));
    ynew = (double *)malloc(nnew * sizeof(double));
    if (x==NULL || y==NULL || xnew==NULL || ynew==NULL)
    {    free(x);  free(y);  free(xnew);  free(ynew);
        if (log_file)
             fprintf(log_file,"RESAMPLE: unable to allocate temporary storage.\n");
        return 1;
    }
                
    /* Build old sample time list - old indepedent X */
    for (samp=0; samp<n; samp++)
        x[samp] = samp * InfoPtr->ns_per_samp;
    /* Build new sample time list - new indepedent X */
    for (samp=0; samp<nnew; samp++)
        xnew[samp] = samp * InfoPtr->new_ns_per_samp;

    first_proc_samp = InfoPtr->first_proc_samp;

    /* Work thru grid */
	printf(".");
    switch (InfoPtr->input_datatype)
    {   case -1:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((unsigned char **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case -1: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= 1.0) 
                            ((unsigned char **)(InfoPtr->grid))[trace][grid_samp] = 0;
                        else if (ynew[samp] >= 255.0)
                            ((unsigned char **)(InfoPtr->grid))[trace][grid_samp] = 255;
                        else
                            ((unsigned char **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case 1:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((char **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case 1: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= -127.0) 
                            ((char **)(InfoPtr->grid))[trace][grid_samp] = -128;
                        else if (ynew[samp] >= 127.0)
                            ((char **)(InfoPtr->grid))[trace][grid_samp] = 127;
                        else
                            ((char **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case -2:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((unsigned short **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case -2: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= 1.0) 
                            ((unsigned short **)(InfoPtr->grid))[trace][grid_samp] = 0;
                        else if (ynew[samp] >= 65535.0)
                            ((unsigned short **)(InfoPtr->grid))[trace][grid_samp] = 65535L;
                        else
                            ((unsigned short **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case -5:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((unsigned short **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case -5: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= 1.0) 
                            ((unsigned short **)(InfoPtr->grid))[trace][grid_samp] = 0;
                        else if (ynew[samp] >= 4095.0)
                            ((unsigned short **)(InfoPtr->grid))[trace][grid_samp] = 4095;
                        else
                            ((unsigned short **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case 2:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((short **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case 2: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= -32767.0) 
                            ((short **)(InfoPtr->grid))[trace][grid_samp] = -32768L;
                        else if (ynew[samp] >= 32767.0)
                            ((short **)(InfoPtr->grid))[trace][grid_samp] = 32767;
                        else
                            ((short **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case -3:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((unsigned long **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case -3: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= 1.0) 
                            ((unsigned long **)(InfoPtr->grid))[trace][grid_samp] = 0;
                        else if (ynew[samp] >= 4294967295.0)
                            ((unsigned long **)(InfoPtr->grid))[trace][grid_samp] = 4294967295L;
                        else
                            ((unsigned long **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case -6:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((unsigned long **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case -6: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= 1.0) 
                            ((unsigned long **)(InfoPtr->grid))[trace][grid_samp] = 0;
                        else if (ynew[samp] >= 8388608.0)
                            ((unsigned long **)(InfoPtr->grid))[trace][grid_samp] = 8388608L;
                        else
                            ((unsigned long **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case 3:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((long **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case 3: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= -214783647.0) 
                            ((long **)(InfoPtr->grid))[trace][grid_samp] = -214783648L;
                        else if (ynew[samp] >= 214783647.0)
                            ((long **)(InfoPtr->grid))[trace][grid_samp] = 214783647L;
                        else
                            ((long **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case 4:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((float **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case 4: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                    {   if (ynew[samp] <= -FLT_MAX) 
                            ((float **)(InfoPtr->grid))[trace][grid_samp] = -FLT_MAX;
                        else if (ynew[samp] >= FLT_MAX)
                            ((float **)(InfoPtr->grid))[trace][grid_samp] = FLT_MAX;
                        else
                            ((float **)(InfoPtr->grid))[trace][grid_samp] = ynew[samp];
                    }
                }
            }
            break;
        case 8:
            for (trace=0; trace <InfoPtr->num_cols; trace++)
            {   if (!trace%istep) printf(".");
				/* Build old sample amp list - old dependent Y*/
                for (samp=0,   grid_samp=InfoPtr->header_samps; 
                     samp<n && grid_samp<InfoPtr->num_rows; 
                     samp++,   grid_samp++)
                    y[samp] = ((double **)(InfoPtr->grid))[trace][grid_samp];

                /* Spline for new sample amp values - new dependent Y */
                itemp = Spline(n,nnew,x,y,xnew,ynew);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"ResampleGprData case 8: %s\n",SplineMsg[itemp]);
                    itemp = 1;
                    break;  /* out of for trace loop */
                }
        
                /* Copy values into grid */
                if (itemp == 0)
                {   for (samp=first_proc_samp, grid_samp=InfoPtr->header_samps + first_proc_samp;
                         samp<nnew  &&         grid_samp<InfoPtr->num_rows; 
                         samp++,               grid_samp++)
                        ((double **)(InfoPtr->grid))[trace][grid_samp] = y[samp];
                }
            }
        break;
    }  /* end of switch (InfoPtr->input_datatype) block */
    if (itemp) return 3;

/***** FFT filter *****/
    itemp = FftFilterGridTraces(InfoPtr->num_cols, InfoPtr->num_rows,
                InfoPtr->first_proc_samp, InfoPtr->header_samps, 
                InfoPtr->input_datatype, InfoPtr->preprocFFT, 
                InfoPtr->new_ns_per_samp,
                InfoPtr->lo_freq_cutoff, InfoPtr->hi_freq_cutoff,
                InfoPtr->grid);
    if (itemp > 0)
    {   if (log_file)
        {   fprintf(log_file,"FFT_FILTER: %s\n",FftFilterGridTracesMsg[itemp]);
            fprintf(log_file,"%s\n",ResampleGprDataMsg[itemp]);
        }
        return 2;
    }                                                            
    return 0;
}
/******************************** SaveGprData() ********************************/
/* This is the driver routine for saving the GPR data to disk.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <dos.h>, "gpr_samp.h".
 * Calls: printf, fprintf, sprintf, strcat, time, localtime, strlen,
 *        SaveDztData, SaveDt1Data, SaveSgyData, SaveUsrData.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SaveGprDataMsg[] =
{   "SaveGprData(): No errors.",
    "SaveGprData() ERROR: Invalid/unknown file storage format.",
    "SaveGprData() ERROR: Data/information was not saved to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: September 5, 1997
 * Revisions: September 10, 1997; August 9, 2001;
 */
int SaveGprData (struct SampParamInfoStruct *InfoPtr)
{
    char str[80];
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    int itemp;
    int bytes;              /* length of current proc_hist line */
    int file_saved = FALSE;
    int change = FALSE;
    double fund_freq, Nyq_freq;

    extern FILE *log_file;               /* beginning of this source */
    extern const char *SaveDztDataMsg[]; /* beginning of this source */
    extern const char *SaveDt1DataMsg[]; /* beginning of this source */
    extern const char *SaveSgyDataMsg[]; /* beginning of this source */
    extern const char *SaveUsrDataMsg[]; /* beginning of this source */

    /* Fill processing history string */
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
    sprintf(InfoPtr->proc_hist,"\nGPR_SAMP v. %s (USGS-JL) on %s %d, %d at %d:%02d\nGPR_SAMP Methods:",
            GPR_SAMP_VER,month[dosdate.month],dosdate.day,dosdate.year,dostime.hour,dostime.minute);
    bytes = strlen(InfoPtr->proc_hist);

    if (InfoPtr->first_trace > 0 || InfoPtr->last_trace < InfoPtr->total_traces-1)
    {   sprintf(str," TRACES_SELECTED %d to %d;",InfoPtr->first_trace,InfoPtr->last_trace);
        if ((bytes + strlen(str)) >= 79)
        {   strcat(InfoPtr->proc_hist,"\n");
            bytes = 0;
        }
        bytes += strlen(str);
        if ( (strlen(InfoPtr->proc_hist)+bytes) < (sizeof(InfoPtr->proc_hist)-1))
            strcat(InfoPtr->proc_hist,str);
    }

    if (InfoPtr->first_samp > 0 || InfoPtr->last_samp < InfoPtr->total_samps-1)
    {   change = TRUE;
        sprintf(str," SAMPS_SELECTED %d to %d;",InfoPtr->first_samp,InfoPtr->last_samp);
        if ((bytes + strlen(str)) >= 79)
        {   strcat(InfoPtr->proc_hist,"\n");
            bytes = 0;
        }
        bytes += strlen(str);
        if ( (strlen(InfoPtr->proc_hist)+bytes) < (sizeof(InfoPtr->proc_hist)-1))
            strcat(InfoPtr->proc_hist,str);
    }

    fund_freq = 1.0E09 / ( (InfoPtr->total_samps-1) * (InfoPtr->ns_per_samp) );
    Nyq_freq  = 1.0E09 / (InfoPtr->ns_per_samp * 2.0);
    if (InfoPtr->lo_freq_cutoff != fund_freq || InfoPtr->hi_freq_cutoff != Nyq_freq)
    {   change = TRUE;
        sprintf(str," FFT_FILTER lo_cut=%g hi_cut=%g MHz;",
                InfoPtr->lo_freq_cutoff/1.0e+6,InfoPtr->hi_freq_cutoff/1.0e+6);
        if ((bytes + strlen(str)) >= 79)
        {   strcat(InfoPtr->proc_hist,"\n");
            bytes = 0;
        }
        bytes += strlen(str);
        if ( (strlen(InfoPtr->proc_hist)+bytes) < (sizeof(InfoPtr->proc_hist)-1))
            strcat(InfoPtr->proc_hist,str);
    }

    if (InfoPtr->resample >= 3)
    {   change = TRUE;
        sprintf(str," RESAMPLE from %ld to %ld;",InfoPtr->total_samps,InfoPtr->resample);
        if ((bytes + strlen(str)) >= 79)
        {   strcat(InfoPtr->proc_hist,"\n");
            bytes = 0;
        }
        bytes += strlen(str);
        if ( (strlen(InfoPtr->proc_hist)+bytes) < (sizeof(InfoPtr->proc_hist)-1))
            strcat(InfoPtr->proc_hist,str);
    }
    if (change==TRUE && InfoPtr->storage_format==DZT)
    {   sprintf(str,"WARNING: GAIN recorded in header may be inaccurate;");
        if ((bytes + strlen(str)) >= 79)
        {   strcat(InfoPtr->proc_hist,"\n");
            bytes = 0;
        }
        bytes += strlen(str);
        if ( (strlen(InfoPtr->proc_hist)+bytes) < (sizeof(InfoPtr->proc_hist)-1))
            strcat(InfoPtr->proc_hist,str);
    }


    strcat(InfoPtr->proc_hist,"\n");

    switch (InfoPtr->storage_format)
    {   case DZT:
        {   itemp = SaveDztData(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",SaveDztDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SaveDztDataMsg[itemp]);
            } else
                file_saved = TRUE;
            break;
        }
        case DT1:
        {   itemp = SaveDt1Data(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",SaveDt1DataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SaveDt1DataMsg[itemp]);
            } else
                file_saved = TRUE;
            break;
        }
        case SGY:
        {   itemp = SaveSgyData(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",SaveSgyDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SaveSgyDataMsg[itemp]);
            } else
                file_saved = TRUE;
            break;
        }
        case USR:
        {   itemp = SaveUsrData(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",SaveUsrDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SaveUsrDataMsg[itemp]);
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
/******************************* SaveDztData() ******************************/
/* This function saves the GPR data to disk using the DZT format.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_samp.h".
 * Calls: SetDzt5xHeader, ReadOneDztHeader, SaveDztFile, _splitpath (non-ANSI),
 *        strcpy, strncpy, strcat, sprintf, puts, printf, strlen.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SaveDztDataMsg[] =
{   "SaveDztData(): No errors.",
	"SaveDztData() ERROR: Invalid datatype for output DZT file.",
	"SaveDztData() ERROR: Problem getting input file header.",
	"SaveDztData() ERROR: Problem saving data.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 1, 1996
 * Revisions: August 25, 1997; September 10, 1997; November 18, 1997
 */
int SaveDztData (struct SampParamInfoStruct *InfoPtr)
{
	int i,itemp;
	char name[12],antname[14], drive[_MAX_DRIVE], dir[_MAX_DIR];
	char text[800];
	char fname[_MAX_FNAME], fext[_MAX_EXT];
	int max_proc_hist_size = MAX_PROC_HIST_SIZE;
	unsigned char proc_hist[MAX_PROC_HIST_SIZE];  /* hopefully 400 is big enough! */
	int hdr_num,num_hdrs,header_size,rh_dtype;
	unsigned short rh_nchan,rg_breaks,rh_nproc;
	float rg_values[8];    /* this limit may change in future versions */
	long num_traces;
	struct DztHdrStruct hdr;              /* struct in gpr_io.h */
	struct DztHdrStruct *hdrPtrArray[1];
	struct DztDateStruct create_date;
	extern const char *SaveDztFileMsg[];  /* in gpr_io.h, dzt_io.c */
    extern FILE *log_file;                /* beginning of this source */

    switch (InfoPtr->input_datatype)
    {   case -1:  case -2:    break;
        default:              return 1;
    }

    /* Read input DZT header again */
    itemp = ReadOneDztHeader(InfoPtr->dat_infilename,&num_hdrs,
                             &num_traces,InfoPtr->channel,&header_size,&hdr);
	if (itemp > 0)
	{   if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
		return 2;
	}

	/* Set/copy values for output header */
	if (InfoPtr->resample >= 3)              /* reset number of samples */
		hdr.rh_nsamp = InfoPtr->resample;
	else
		hdr.rh_nsamp = InfoPtr->total_samps;
	hdr.rh_range = InfoPtr->new_total_time;  /* reset total time */
	if (hdr.rh_dtype == 0 && InfoPtr->input_datatype != -1)
	{   if (InfoPtr->input_datatype == -2)      rh_dtype = 0x0002;
		else if (InfoPtr->input_datatype == -3) rh_dtype = 0x0004;
		else                                    rh_dtype = 0x0000;
	}
	rg_breaks = *(unsigned short *) ((char *)&hdr + hdr.rh_rgain);
	for (i=0; i<rg_breaks && i<8; i++)
		rg_values[i] = *(float *) ((char *)&hdr + hdr.rh_rgain + 2 + 4*i);
	strncpy(text,(char *)&hdr + hdr.rh_text,sizeof(text)-1);
	if (strlen(text) < sizeof(text)-strlen(InfoPtr->proc_hist)-2)
		strcat(text,InfoPtr->proc_hist);
	rh_nproc = hdr.rh_nproc;
	if (rh_nproc > max_proc_hist_size-1) rh_nproc = max_proc_hist_size-1;
	for (i=0; i<rh_nproc; i++)
		proc_hist[i] = *((char *)&hdr + hdr.rh_proc + i);
	proc_hist[i] = 0;  /* counts on i being past for-loop limits */
	hdr_num = 1;
	rh_nchan = 1;
	_splitpath(InfoPtr->dat_outfilename,drive,dir,fname,fext);
	strcpy(name,fname);
	strncpy(antname,hdr.rh_antname,sizeof(antname)-1);
	antname[sizeof(antname)-1] = 0;
	create_date = hdr.rh_create;
	/* Note: Do not send pointers from hdr through this function!
			 SetDzt5xHeader() clears structure arrays before writing to
			 them! Other members are OK as they are sent by value.
	 */
    SetDzt5xHeader(hdr_num,hdr.rh_nsamp,hdr.rh_bits,hdr.rh_zero,hdr.rh_sps,
                 hdr.rh_spm,hdr.rh_mpm,hdr.rh_position,hdr.rh_range,
                 hdr.rh_npass,rh_nchan,hdr.rh_epsr,hdr.rh_top,hdr.rh_depth,
                 rh_dtype,antname,name,text,rg_breaks,rg_values,rh_nproc,
                 proc_hist,&create_date,&hdr);

    /* Save to disk */
    hdrPtrArray[0] = &hdr;
    itemp = SaveDztFile(InfoPtr->dat_outfilename,InfoPtr->num_cols,
                (long)hdr.rh_nsamp,(int)hdr.rh_bits,hdr_num,
                hdrPtrArray,InfoPtr->grid);
    if (itemp > 0)
    {   if (log_file) fprintf(log_file,"%s\n",SaveDztFileMsg[itemp]);
        return 3;
    }

    return 0;
}
/******************************* SaveDt1Data() ******************************/
/* This function saves the GPR data to disk using the DT1 format.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, "gpr_samp.h".
 * Calls: memset, fprintf, fopen, fclose, strcat, realloc, ferror, fwrite,
 *        clearerr, GetSsHdFile, SaveSsHdFile.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SaveDt1DataMsg[] =
{   "SaveDt1Data(): No errors.",
	"SaveDt1Data() ERROR: Invalid datatype for output DT1 file.",
    "SaveDt1Data() ERROR: Problem getting input file header.",
    "SaveDt1Data() ERROR: Problem saving HD file.",
    "SaveDt1Data() ERROR: Unable to open output DT1 file.",

    "SaveDt1Data() ERROR: Problem saving DT1 file.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: January 31, 1996
 * Revisions: September 10, 1997
 */
int SaveDt1Data (struct SampParamInfoStruct *InfoPtr)
{
    int itemp,print_it,num_gain_pts,proc_hist_size,trace_size;
    long trace,num_samps;
    double traces_per_sec,meters_per_mark,gain_pts[8];
    struct SsHdrInfoStruct HdInfo;              /* struct in gpr_io.h */
    FILE *outfile;
    extern const char *GetSsHdFileMsg[];        /* in gpr_io.h */
    extern const char *SaveSsHdFileMsg[];       /* in gpr_io.h */
    extern int Debug;                           /* beginning of this source */
    extern FILE *log_file;                      /* beginning of this source */

    #if defined(_INTELC32_)
      char *outbuffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
	#endif

	switch (InfoPtr->input_datatype)
	{   case 2:      break;
		default:  return 1;
	}

	/* Read input HD file again */
	if (Debug) print_it = TRUE;
	else       print_it = FALSE;
	itemp = GetSsHdFile(print_it,&HdInfo,InfoPtr->hd_infilename);
	if (itemp > 0)
	{   if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
		return 2;
	}

	/* Set/save values for output header file */
	if (InfoPtr->resample >= 3)
		num_samps = InfoPtr->resample;
	else
		num_samps = InfoPtr->total_samps;
	meters_per_mark = traces_per_sec = 0.0;
	num_gain_pts = 0;
	proc_hist_size = strlen(HdInfo.proc_hist) + 1;
	proc_hist_size += strlen(InfoPtr->proc_hist) + 2;
	realloc(HdInfo.proc_hist,proc_hist_size);
	strcat(HdInfo.proc_hist,InfoPtr->proc_hist);
	itemp = SaveSsHdFile((int)HdInfo.day,(int)HdInfo.month,(int)HdInfo.year,
						 InfoPtr->num_cols,
						 num_samps,
						 (long)HdInfo.time_zero_sample,
						 (int)InfoPtr->new_total_time,
						 (double)HdInfo.start_pos,(double)HdInfo.final_pos,
						 (double)HdInfo.step_size,HdInfo.pos_units,
						 (double)HdInfo.ant_freq,(double)HdInfo.ant_sep,
						 (double)HdInfo.pulser_voltage,(int)HdInfo.num_stacks,
						 HdInfo.survey_mode,HdInfo.proc_hist,
						 InfoPtr->hd_outfilename,InfoPtr->dat_outfilename,
						 HdInfo.job_number,HdInfo.title1,HdInfo.title2,
						 traces_per_sec,meters_per_mark,num_gain_pts,
						 gain_pts);
	if (itemp > 0)
	{   if (log_file) fprintf(log_file,"%s\n",SaveSsHdFileMsg[itemp]);
		return 3;
	}

    /* Open Output DT1 file */
    if ((outfile = fopen(InfoPtr->dat_outfilename,"wb")) == NULL)  return 4;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(outbuffer,vbufsize);
      if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
	  else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
    #endif

    /* Save data to disk */
    itemp = 0;
    trace_size = (num_samps + InfoPtr->header_samps) * 2; /* data bytes + header bytes */
	for (trace = 0; trace < InfoPtr->num_cols; trace++)
    {   fwrite(InfoPtr->grid[trace],(size_t)trace_size,(size_t)1,outfile);
    }
    if (ferror(outfile))
	{   clearerr(outfile);
        itemp = 6;
    }
    #if defined(_INTELC32_)
      realfree(outbuffer);
    #endif
    fclose(outfile);
    return itemp;
}
/******************************* SaveSgyData() ******************************/
/* This function saves the GPR data to disk using the SEG-Y format.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, "gpr_samp.h".
 * Calls: fprintf, fopen, fclose, strcat, malloc, free, ferror, fwrite,
 *        clearerr, ReadSegyReelHdr, ExtractSsInfoFromSegy, SetSgyFileHeader.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SaveSgyDataMsg[] =
{   "SaveSgyData(): No errors.",
    "SaveSgyData() ERROR: Invalid datatype for output SGY file.",
    "SaveSgyData() ERROR: Problem getting input file header.",
    "SaveSgyData() ERROR: Unable to open output DT1 file.",
    "SaveSgyData() ERROR: Problem saving SGY file header.",

    "SaveSgyData() ERROR: Problem saving SGY file data.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: May 23, 1996
 * Revisions: September 10, 1997
 */
int SaveSgyData (struct SampParamInfoStruct *InfoPtr)
{
    char *pos_units,*survey_mode,*proc_hist,dummy_text[1],*text;
    int day,month,year,num_stacks,num_gain_pts ;
    int itemp,swap_bytes,proc_hist_size;
    long trace,trace_size,num_traces,num_samples,time_zero_sample,total_time_ns;
	double start_pos,final_pos,step_size,ant_freq,ant_sep,pulser_voltage;
    double traces_per_sec,meters_per_mark,dummy_gain_pts[1],*gain_pts ;
    struct GprInfoStruct Info;                  /* struct in gpr_io.h */
    struct SegyReelHdrStruct hdr;               /* struct in gpr_io.h */
	FILE *outfile;
    extern const char *GetSegyReelHdrMsg[];     /* in gpr_io.h */
    extern FILE *log_file;                      /* beginning of this source */

    #if defined(_INTELC32_)
      char *outbuffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
    #endif

    switch (InfoPtr->input_datatype)
    {   case 2:    case 3:    case 4:     break;
        default:  return 1;
    }

	/* Read input SEG-Y reel header again */
	itemp = ReadSegyReelHdr(InfoPtr->dat_infilename,&swap_bytes,&num_traces,
							&hdr);
	if (itemp > 0)
	{   if (log_file) fprintf(log_file,"%s\n",ReadSegyReelHdrMsg[itemp]);
		return 2;
	}
	ExtractSsInfoFromSegy(&Info,&hdr);

	/* Set/save values for output header file */
	day              = Info.time_date_created.day;
	month            = Info.time_date_created.month;
	year             = Info.time_date_created.year + 1980 - 1900;
	num_traces       = Info.total_traces;
	if (InfoPtr->resample >= 3)
		num_samples = InfoPtr->resample;
	else
		num_samples = InfoPtr->total_samps;
	time_zero_sample = Info.timezero_sample;
	total_time_ns    = InfoPtr->new_total_time;
	start_pos        = Info.starting_position;
	final_pos        = Info.final_position;
	step_size        = Info.position_step_size;
	pos_units        = Info.position_units;
	ant_freq         = Info.nominal_frequency;
	ant_sep          = Info.antenna_separation;
	pulser_voltage   = Info.pulser_voltage;
	num_stacks       = Info.number_of_stacks;
	survey_mode      = Info.survey_mode;
	if (Info.proc_hist == NULL)
	{   proc_hist = InfoPtr->proc_hist;
	} else
	{   proc_hist_size = strlen(Info.proc_hist) + 1;
		proc_hist_size += strlen(InfoPtr->proc_hist) + 2;
		proc_hist = (char *)malloc(proc_hist_size);
		if (proc_hist != NULL) strcat(proc_hist,InfoPtr->proc_hist);
	}
	text            = Info.text;
	if (text == NULL)
	{   dummy_text[0] = 0;
		text = dummy_text;
	}
	num_gain_pts      = Info.num_gain_pts;
	gain_pts          = Info.gain_pts;
	if (gain_pts == NULL)
	{   dummy_gain_pts[0] = 0.0;
		gain_pts = dummy_gain_pts;
		num_gain_pts = 0;
	}
	meters_per_mark = traces_per_sec = 0.0;

	SetSgyFileHeader(day,month,year,num_traces,num_samples,time_zero_sample,
					 (int)total_time_ns,start_pos,final_pos,step_size,pos_units,
					 ant_freq,ant_sep,pulser_voltage,num_stacks,survey_mode,
					 proc_hist,text,Info.job_number,Info.title1,Info.title2,
					 InfoPtr->dat_outfilename,traces_per_sec,meters_per_mark,
					 num_gain_pts,gain_pts,&hdr);
	free(proc_hist);

    /* Open output SGY file */
    if ((outfile = fopen(InfoPtr->dat_outfilename,"wb")) == NULL)  return 3;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(outbuffer,vbufsize);
      if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
      else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
    #endif

    /* Save reel header to disk */
    if (fwrite((void *)&hdr,sizeof(struct SegyReelHdrStruct),(size_t)1,outfile) < 1)
    {   fclose(outfile);
		#if defined(_INTELC32_)
          realfree(outbuffer);
        #endif
        return 4;
	}

    /* Save data to disk */
    itemp = 0;
    switch (InfoPtr->input_datatype)
    {   case 2:           trace_size = 2;  break;
        case 3:  case 4:  trace_size = 4;  break;
    }
    trace_size *= num_samples + InfoPtr->header_samps; /* data bytes + header bytes */

    for (trace = 0; trace < InfoPtr->num_cols; trace++)
    {   fwrite(InfoPtr->grid[trace],(size_t)trace_size,(size_t)1,outfile);
    }
    if (ferror(outfile))
    {   clearerr(outfile);
        itemp = 5;
    }
    #if defined(_INTELC32_)
      realfree(outbuffer);
    #endif
    fclose(outfile);
    return itemp;
}
/******************************* SaveUsrData() ******************************/
/* This function saves the GPR data to disk using the user-defined format.
 *
 * Parameter list:
 *  struct SampParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, "gpr_samp.h".
 * Calls: fprintf, fopen, fclose, ferror, fread, fwrite, clearerr,
 *        malloc, free.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SaveUsrDataMsg[] =
{   "SaveUsrData(): No errors.",
    "SaveUsrData() ERROR: Invalid datatype for output user-defined file.",
    "SaveUsrData() ERROR: Problem getting input file header.",
    "SaveUsrData() ERROR: Unable to open output GPR file.",
    "SaveUsrData() ERROR: Problem saving GPR file header.",

    "SaveSgyData() ERROR: Problem saving GPR file data.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: January 31, 1996
 * Revisions: September 10, 1997
 */
int SaveUsrData (struct SampParamInfoStruct *InfoPtr)
{
    char *file_header;
    int itemp;
    long trace,trace_size;
    FILE *infile,*outfile;
    extern FILE *log_file;                      /* beginning of this source */

    #if defined(_INTELC32_)
      char *outbuffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
    #endif

    switch (InfoPtr->input_datatype)
    {   case 1:  case -1:  case 2:    case -2:  case 3:  case -3:
        case 4:     case 8:  case -5:  case -6: break;
        default:  return 1;
    }

    /* Read input user-defined header again */
    if (InfoPtr->file_header_bytes > 0)
    {   file_header = (char *)malloc(InfoPtr->file_header_bytes);
        if (file_header == NULL)
        {   if (log_file) fprintf(log_file,"SaveUsrData() ERROR: unable to allocate temporary storage.\n");
            return 2;
        }
        infile = fopen(InfoPtr->dat_infilename,"rb");
        if (infile == NULL)
        {   if (log_file) fprintf(log_file,"SaveUsrData() ERROR: unable to open input GPR file.\n");
            return 2;
        }
        if (fread((void *)file_header,InfoPtr->file_header_bytes,(size_t)1,infile) < 1)
        {   if (log_file) fprintf(log_file,"SaveUsrData() ERROR: unable to allocate temporary storage.\n");
            return 2;
        }
        fclose(infile);
	}

	/* Open output GPR file */
	if ((outfile = fopen(InfoPtr->dat_outfilename,"wb")) == NULL)  return 3;

	/* Speed up disk access by using large (conventional memory) buffer */
	#if defined(_INTELC32_)
	  realmalloc(outbuffer,vbufsize);
	  if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
	  else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
	#endif

	/* Save file header to disk */
	if (InfoPtr->file_header_bytes > 0)
	{   if (fwrite((void *)file_header,InfoPtr->file_header_bytes,(size_t)1,outfile) < 1)
		{   fclose(outfile);
			#if defined(_INTELC32_)
			  realfree(outbuffer);
			#endif
			return 4;
		}
	}

	/* Save data to disk */
	itemp = 0;
	switch (InfoPtr->input_datatype)
	{   case 1:  case -1:                    trace_size = 1;  break;
		case 2:  case -2:  case -5:          trace_size = 2;  break;
		case 3:  case -3:  case -6: case 4:  trace_size = 4;  break;
		case 8:                              trace_size = 8;  break;
	}
	if (InfoPtr->resample >= 3)
		trace_size *= InfoPtr->resample + InfoPtr->header_samps; /* data bytes + header bytes */
	else
		trace_size *= InfoPtr->total_samps + InfoPtr->header_samps; /* data bytes + header bytes */
	for (trace = 0; trace < InfoPtr->num_cols; trace++)
	{   fwrite(InfoPtr->grid[trace],(size_t)trace_size,(size_t)1,outfile);
	}
	if (ferror(outfile))
	{   clearerr(outfile);
		itemp = 5;
	}
	#if defined(_INTELC32_)
	  realfree(outbuffer);
	#endif
	fclose(outfile);
	return itemp;
}

/**************************** end of GPR_SAMP.C *****************************/
