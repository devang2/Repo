/******************************** GPR_CONV.C ********************************\
                              version 2.08.15.01
-----------------------------------------------------------------------------
INTRODUCTION

GPR_CONV.EXE coverts digital radar data from one storage format to another.
The following storage formats are recognized by their filename extension.
   *.DZT    GSSI SIR-10A version 5.x binary files, 8-bit or 16-bit
              unsigned data with an embedded descriptive 1024-byte header
              (if a format error occurs, then the program will attempt to
              read the file using the old 512-byte header format).
   *.DT1    Sensors and Software pulseEKKO binary files, 16-bit signed
              data, accompanied by a descriptive ASCII "HD" file.
   *.SGY    SEG-Y formatted files, 16-bit or 32-bit signed data or 32-bit
              or 64-bit floating point (IEEE) data.
   *.SG2    SEG-2 formatted files, 16-bit or 32-bit signed data or 32-bit
              or 64-bit floating point (IEEE) data.
              NOTE: NOT SUPPORTED AT THIS TIME.
   *.RD3    Ramac binary files, 16-bit signed data with no headers,
              accompanied by a descriptive ASCII "RAD" file.
              NOTE: NOT SUPPORTED AT THIS TIME.

The user may also specify the storage format or override the format of
files that have one of the above extensions but may not fully comply
with the specification or have missing information.

The input to this program is a "CMD" file, an ASCII text file containing
commands (or parameters) describing the radar data.  Inspect the example
file "gpr_conv.cmd" for usage.

NOTES:
  Only 1 to 4 headers are supported in DZT files.
  There is no graphic display of the data.
  To display the converted data, use programs such as GPR_DISP.EXE or
    FIELDVEW.EXE.
  User-defined formats assume only one data channel.

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
May 2, 1995         - Completed.
November 18, 1995   - Modified to use GPR_IO.LIB.
November 20, 1995   - Added color-coding to PrintUsageMsg.
                    - Improved error reporting for OTHER format in
                      GetInfo().
January 23, 1996    - Moved global declarations to gpr_conv.c from
                      gpr_conv.h and with replaced extern references.
May 21, 1996        - Added info from MRK file to DZT output files.
                    - Corrected implementation of ANSI_there().
                    - Update "Function Dependency" comments.
                    - Fixed input of XYZ and MRK files.
May 23, 1996        - ...in_time_window changed from int to double.
June 13, 1996       - Corrected way out_time_window calculated for DZT
                      files in ConvertGprData().
January 29, 1997    - Modify to allow limited selection of output data type
                      for output GSSI DZT files.  Required additions to
                      struct GprInfoStruct in GPR_IO.H.
                    - Added version number as 1.1 and manifest constant.
                    - Removed ANSI_there(), TrimStr(), and GetDosVersion()
                      from source and now must link jl_util1.lib.
                    - Updated messages from GetInfo().
                    - Added provisions to select Ramac files for I/O but
                      not fully implemented yet.
June 25, 1997       - Fixed bug in GetCmdFileArgs() item 13 in_time_window.
                      Was wrongly checking if itemp > 0, nw checks dtemp
August 11, 1997     - added support for modified DZT formats.
March 31, 2000      - Convert from gpr_conv to gpr_conv.
                    - Add struct FilesInfoStruct to handle file lists.
April 4, 2000       - Fix minor glitches so compiles.
                    - Had to add flag to use mrk and/or xyz files since these
                      names are automatically assigned.
                    - Added use_mark_file and use_xyz_file so can select
                      between using marks in input DZT file or the MRK file.
April 11, 2000      - Recompiled to include fix in gpr_io - DZT text field.
May 3, 2001         - Change name to GPR_CONV. versions is 2.05.03.01.
August 1, 2001      - Change DT1 output files to start with trace 1 not 0.
August 15, 2001     - Corrected problems in converting DZT -> DT1.
                    - Changed way DT1 "poroc hist" info saved in DZT files.

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /zfloatsync GPR_CONV.C gpr_io.lib jl_util1.lib
     To compile for 80486 executable :
       icc /F /xnovm /zfloatsync /zmod486 GPR_CONV.C gpr_io.lib jl_util1.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions

     gpr_io.lib - Jeff Lucius's GPR I/O functions (DZT, DT1, SGY, general)
     jl_util1.lib - Jeff Lucius's utility functions.

     to remove assert() functions from code also add:  /DNDEBUG

To run: GPR_CONV cmd_filename
\****************************************************************************/

/*************************** Function dependency ****************************/
/*
 * con I/O  - keyboard or screen used
 * disk I/O - disk read/writes
 * DOS      - DOS/BIOS interrupt performed
 * port I/O - hardware ports used
 * RAM      - direct CPU RAM access
 *
 * main                                                (con I/O)
 *   +-- ANSI_there                                             (DOS)
 *   |     +-- GetDosVersion                                    (DOS)
 *   +-- GetParameters                                 (con I/O)
 *   |     +-- PrintUsageMsg                           (con I/O)
 *   |     +-- InitGprInfoStruct
 *   |     +-- TrimStr
 *   |     +-- GetCmdFileArgs                (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetGprFileType                (disk I/O)
 *   +-- GetInfo                             (disk I/O)(con I/O)
 *   |     +-- DeallocInfoStruct
 *   |     +-- GetMrkData                    (disk I/O)
 *   |     +-- GetXyzData                    (disk I/O)
 *   |     +-- ReadOneDztHeader              (disk I/O)
 *   |     +-- PrintOneDztHeader                       (con I/O)
 *   |     |     +-- ConvertProcHist2
 *   |     +-- InitDt1Parameters
 *   |     +-- GetSsHdFile                   (disk I/O)(con I/O)
 *   |     |     +-- TrimStr
 *   |     +-- PrintSsHdInfo                           (con I/O)
 *   |     +-- ReadSegyReelHdr               (disk I/O)
 *   |     +-- PrintSegyReelHdr                        (con I/O)
 *   |     +-- ExtractSsInfoFromSegy
 *   |     |     +-- TrimStr
 *   +-- DisplayParameters                             (con I/O)
 *   +-- ConvertGprData                      (disk I/O)(con I/O)
 *         +-- SetDztHeader
 *         +-- SaveSsHdFile                  (disk I/O)
 *         +-- SetSgyFileHeader
 *         +-- SetDt1TraceHeader
 *         +-- SetSgyTraceHeader
 *         +-- ConvertProcHist2
 *
\****************************************************************************/
#include "gpr_conv.h"    /* includes all required header files, etc. */

/**************************** Global variables ******************************/
int Debug      = FALSE;          /* if TRUE, turn debugging on */
int Batch      = FALSE;          /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_CONV_CMDS[] =
{   "debug","batch","num_input_files","input_filelist[]","output_filelist[]",
    "channel","other_format","input_datatype","file_header_bytes","trace_header_bytes",

    "in_samps_per_trace","in_time_window","output_datatype","timezero_sample","traces_per_sec",
    "number_of_stacks","nominal_frequency","pulser_voltage","antenna_separation","antenna_name",

    "traces_per_meter","meters_per_mark","starting_position","final_position","position_step_size",
    "position_units","year_created","month_created","day_created","hour_created",

    "minute_created","num_gain_pts","gain_pts[]","text","survey_mode",
    "nominal_rdp","proc_hist","use_mark_file","use_xyz_file",NULL,
} ;
const char *month_names[] =
{   "no date",    /* 0 */
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", /* 1-12 */
    "error",NULL, /* 13-14 */
} ;
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file(s) not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",
    "GetParameters() ERROR: Insufficient command line arguments.",

    "GetParameters() ERROR: Input filename found in output list.",
    "GetParameters() ERROR: Input filename repeated in input list.",
    "GetParameters() ERROR: Output filename repeated in output list.",
} ;

const char *GetInfoMsg[] =
{   "GetInfo(): No errors.",
    "GetInfo() Error: Attemp made to de-allocate storage before allocating.",
    "GetInfo() Error: Attemp made to re-allocate storage.  De-allocate first.",
    "GetInfo() Error: No recognized data/info input files specified.",
    "GetInfo() Error: Problem de-allocating storage.",

    "GetInfo() Error: Problem getting information about input data file.",
    "GetInfo() Error: Not able to allocate sufficient temporary storage.",
    "GetInfo() Error: Problem getting data from input file.",
    "GetInfo() Error: Invalid channel selection for DZT file.",
    "GetInfo() Error: Invalid type for input data.",

    "GetInfo() Error: Less than one trace in file.",
    "GetInfo() Error: Less than two samples per trace.",
    "GetInfo() Error: Less than one byte per sample.",
    "GetInfo() Error: Less than 0 or 0 ns per sample.",
    "GetInfo() Error: Input Data type not recognized.",

    "GetInfo() Error: Problem getting data from input MRK file.",
    "GetInfo() Error: Problem getting data from input XYZ file.",
    "GetInfo() Error: Number of records in MRK and XYZ disagree.",
    "GetInfo() Error: SEG-Y data type (format) not recognized.",
    "GetInfo() Error: Output Data type not recognized.",

    "GetInfo() Error: Unable to determine input file storage format.",
} ;
const char *ConvertGprDataMsg[] =
{   "ConvertGprData() No Errors.",
    "ConvertGprData() Error: Unable to allocate temporary storage.",
    "ConvertGprData() Error: Opening input file, file or path name not found.",
    "ConvertGprData() Error: Opening input file, read and write access denied.",
    "ConvertGprData() Error: Opening input file, unknown access error.",

    "ConvertGprData() Error: Opening output file, file or path name not found.",
    "ConvertGprData() Error: Opening output file, read and write access denied.",
    "ConvertGprData() Error: Opening output file, unknown access error.",
    "ConvertGprData() Error: Unrecognized input storage format.",
    "ConvertGprData() Error: Short count from fread().",

    "ConvertGprData() Error: Short count from fwrite().",
	"ConvertGprData() Error: Problem saving HD file.",
    "ConvertGprData() Error: Problem saving SGY file header file..",
    "ConvertGprData() Error: Floating point input data not supported.",
} ;

/********************************** main() **********************************/
void main (int argc,char *argv[])
{
/***** Declare variables *****/
    /* following are scratch variables used by main */
    char *cp;
    int  itemp,num;

    /* following is the information structure passed between functions */
    struct FilesInfoStruct FilesInfo;  /* defined in gpr_conv.h */
    struct GprInfoStruct ParamInfo;    /* defined in gpr_io.h */

    /* these variables found near beginning of this source */
    extern int Batch, ANSI_THERE;
    extern const char *GetParametersMsg[];
    extern const char *GetInfoMsg[];
    extern const char *ConvertGprDataMsg[];

/***** Initialize variables *****/
    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Zero-out information structure */
    memset((void *)&FilesInfo,0x00,sizeof(struct FilesInfoStruct));

/***** Get information about the data *****/
    /* Get command file parameters */
    printf("Getting user parameters ...");
    itemp =  GetParameters(argc,argv,&ParamInfo,&FilesInfo);
    printf("\r                                          \r");
    if (itemp > 0)
    {   printf("\n%s\n",GetParametersMsg[itemp]);
        exit(1);
    }

/***** Display parameters and ask if user wants to continue *****/
    DisplayParameters(&ParamInfo,&FilesInfo);
    if (!Batch)
    {   printf("Press <Esc> to quit or other key to continue ... ");
        if ((itemp = getch()) == ESC)
        {   puts("\nProgram terminated by user.\n");
            if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
            itemp = GetInfo(FUNC_DESTROY,&ParamInfo);
            if (itemp > 0)
            {   printf("\n%s\n",GetInfoMsg[itemp]);
                exit(6);
            }
            puts("GPR_CONV finished.");
            if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
            exit(3);
        } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
            getch();
        printf("\r                                                         \r");
    }

/***** Start of LOOP *****/
    for (num=0; num<FilesInfo.num_input_files; num++)
    {
        /***** Copy filename info first *****/
        strcpy(ParamInfo.data_filename,FilesInfo.dat_infilename[num]);
        strupr(ParamInfo.data_filename);

        if (FilesInfo.use_mark_file)
        {   strcpy(ParamInfo.mrk_filename,ParamInfo.data_filename);
            cp = strstr(ParamInfo.mrk_filename,".");
            if (cp != NULL) *cp = 0;
            strcat(ParamInfo.mrk_filename,".MRK\0");
        }
        if (FilesInfo.use_xyz_file)
        {   strcpy(ParamInfo.xyz_filename,ParamInfo.data_filename);
            cp = strstr(ParamInfo.xyz_filename,".");
            if (cp != NULL) *cp = 0;
            strcat(ParamInfo.xyz_filename,".XYZ\0");
        }

        if (strstr(ParamInfo.data_filename,".DT1"))
        {   strcpy(ParamInfo.info_filename,ParamInfo.data_filename);
            cp = strstr(ParamInfo.info_filename,".DT1");
            if (cp != NULL) *cp = 0;
            strcat(ParamInfo.info_filename,".HD\0");
        }

        if (strstr(ParamInfo.data_filename,".RD3"))
        {   strcpy(ParamInfo.info_filename,ParamInfo.data_filename);
            cp = strstr(ParamInfo.info_filename,".RD3");
            if (cp != NULL) *cp = 0;
            strcat(ParamInfo.info_filename,".RAD\0");
        }

        strupr(FilesInfo.dat_outfilename[num]);
        cp = strstr(FilesInfo.dat_outfilename[num],".DZT");
        if (cp != NULL)
            strcpy(ParamInfo.dzt_outfilename,FilesInfo.dat_outfilename[num]);

        cp = strstr(FilesInfo.dat_outfilename[num],".DT1");
        if (cp != NULL)
        {   strcpy(ParamInfo.dt1_outfilename,FilesInfo.dat_outfilename[num]);
            strncpy(ParamInfo.hd_outfilename,FilesInfo.dat_outfilename[num],
                            (size_t)(cp-FilesInfo.dat_outfilename[num]));
            strcat(ParamInfo.hd_outfilename,".HD\0");
        }

        cp = strstr(FilesInfo.dat_outfilename[num],".SGY");
        if (cp != NULL)
            strcpy(ParamInfo.sgy_outfilename,FilesInfo.dat_outfilename[num]);

        cp = strstr(FilesInfo.dat_outfilename[num],".SG2");
        if (cp != NULL)
            strcpy(ParamInfo.sg2_outfilename,FilesInfo.dat_outfilename[num]);

        cp = strstr(FilesInfo.dat_outfilename[num],".RD3");
        if (cp != NULL)
        {   strcpy(ParamInfo.rd3_outfilename,FilesInfo.dat_outfilename[num]);
            strncpy(ParamInfo.rad_outfilename,FilesInfo.dat_outfilename[num],
                            (size_t)(cp-FilesInfo.dat_outfilename[num]));
            strcat(ParamInfo.rad_outfilename,".RAD\0");
        }

        /***** Get the info about the data from headers/info files *****/
        printf("Getting Information ...");
        itemp = GetInfo(FUNC_CREATE,&ParamInfo);
        if (itemp > 0)
        {   printf("\n%s\n",GetInfoMsg[itemp]);
            exit(2);
        }
        printf("\r                            \r");

        /***** Get GPR data, convert format, and save to disk *****/
        printf("Converting %s and saving data",ParamInfo.data_filename);
        itemp = ConvertGprData(&ParamInfo);
        if (itemp > 0)
        {   printf("\n%s\n",ConvertGprDataMsg[itemp]);
            exit(6);
        }
        printf("\r                                                                \r");

        /***** Free storage *****/
        itemp = GetInfo(FUNC_DESTROY,&ParamInfo);
        if (itemp > 0)
        {   printf("\n%s\n",GetInfoMsg[itemp]);
            exit(7);
        }
    } /* end of for (num=0; num<ParamInfo.num_input_files; num++) */
/***** End of LOOP *****/

/***** Terminate program *****/
    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
    puts("GPR_CONV finished.  GPR data saved to disk.");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
    exit(0);
}
/****************************** GetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct GprInfoStruct *InfoPtr - address of structure
 *  struct FilesInfoStruct *FilesInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_conv.h".
 * Calls: PrintUsageMsg, GetCmdFileArgs, InitGprInfoStruct, strchr, strcat,
 *        strcpy, TrimStr.
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file(s) not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",
    "GetParameters() ERROR: Insufficient command line arguments.",

    "GetParameters() ERROR: Input filename found in output list.",
    "GetParameters() ERROR: Input filename repeated in input list.",
    "GetParameters() ERROR: Output filename repeated in output list.",
} ;
 *
 * Author: Jeff Lucius   USGS
 * Date: May 1, 1995
 * Revisions: March 31, 2000;
 */
int GetParameters (int argc,char *argv[],struct GprInfoStruct *InfoPtr,
                   struct FilesInfoStruct *FilesInfoPtr)
{
/***** Declare variables *****/
    int  itemp,i,j;
    int  req_args = 2;        /* exe_name and cmd_filename */
    extern int Debug;

/***** Verify usage *****/
    if (argc < req_args)
    {   PrintUsageMsg();
        return 4;
    }

/***** Initialize parameters *****/
    InitGprInfoStruct(InfoPtr);
    strcpy(InfoPtr->cmd_filename,argv[1]);
    strncpy(InfoPtr->cmd_filename,argv[1],sizeof(InfoPtr->cmd_filename)-1);

/***** Get user-defined parameters from CMD file *****/
    itemp = GetCmdFileArgs(InfoPtr,FilesInfoPtr);
    if (itemp > 0) return itemp;      /* 1 to 3 */

/**** Check for duplicate filenames *****/
    /* IN names cannot be in OUT list*/
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {   for (j=0; j<FilesInfoPtr->num_input_files; j++)
            if (strcmp(FilesInfoPtr->dat_infilename[i],FilesInfoPtr->dat_outfilename[j])==0)
                return 5;
    }
    /* names in IN list cannot repeat */
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {   for (j=i+1; j<FilesInfoPtr->num_input_files; j++)
            if (strcmp(FilesInfoPtr->dat_infilename[i],FilesInfoPtr->dat_infilename[j])==0)
                return 6;
    }
    /* names in OUT list cannot repeat */
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {   for (j=i+1; j<FilesInfoPtr->num_input_files; j++)
            if (strcmp(FilesInfoPtr->dat_outfilename[i],FilesInfoPtr->dat_outfilename[j])==0)
                return 7;
    }

    return 0;
}
/****************************** PrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>.
 * Calls: puts.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS
 * Date: January 28, 1997
 * Revisions: March 31, 2000;
 */
void PrintUsageMsg (void)
{
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("###############################################################################");
    puts("  Usage: GPR_CONV cmd_filename");
    puts("    Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
    puts("  This program converts radar data from one storage format to another.");
    puts("  Required command line arguments:");
    puts("     cmd_filename - name of a text keyword file that follows the \"CMD\" format.");
	puts("  Look at \"GPR_CONV.CMD\" and \"GPR_CONV.TXT\" for keyword file format, valid");
    puts("    keywords, and documentation.");
    puts("  Example call: GPR_CONV cfile1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_CONV_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */

}
/***************************** GetCmdFileArgs() *****************************/
/* Get processing parameters from user from the DOS command line.
 * Valid file commands:
const char *GPR_CONV_CMDS[] =
{   "debug","batch","num_input_files","input_filelist[]","output_filelist[]",
    "channel","other_format","input_datatype","file_header_bytes","trace_header_bytes",

    "in_samps_per_trace","in_time_window","output_datatype","timezero_sample","traces_per_sec",
    "number_of_stacks","nominal_frequency","pulser_voltage","antenna_separation","antenna_name",

    "traces_per_meter","meters_per_mark","starting_position","final_position","position_step_size",
    "position_units","year_created","month_created","day_created","hour_created",

    "minute_created","num_gain_pts","gain_pts[]","text","survey_mode",
    "nominal_rdp","proc_hist","use_mark_file","use_xyz_file",NULL,
} ;
 * Parameter list:
 *  struct GprInfoStruct *InfoPtr - address of structure
 *  struct FilesInfoStruct *FilesInfoPtr - address of structure
 *
 * Requires: "assertjl.h", <stdio.h>, <stdlib.h>, <string.h>, "gpr_conv.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, strlwr (non-ANSI), strupr (non-ANSI), Trimstr, strncpy,
 *        assert.
 * Returns: 0 on success,
 *         >0 on error (offset into GetParametersMsg[] strings).
 *
 * Author: Jeff Lucius   USGS
 * Date: January 28, 1997
 * Revisions: April 4, 2000; August 14, 2001;
 */
int GetCmdFileArgs (struct GprInfoStruct *InfoPtr,
                    struct FilesInfoStruct *FilesInfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];

    int found,i,num,itemp,have_string;
    int dat_in_found      = FALSE;
    int dat_out_found     = FALSE;
    int dt1_hd_out_found  = FALSE;
	double dtemp;
    FILE *infile          = NULL;
    extern int Debug;

    if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL) return 1;
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
    while (1)                     /* read till EOF or error */
    {   if (have_string == FALSE) /* if have not already retrieved file line */
        {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                break;            /* out of while(1) */
        } else
        {    have_string = FALSE;
        }
        cmdstr[0] = 0;            /* set to 0 so strncat works right */
        cp = strchr(str,'=');     /* look for equal sign */
        if (cp == NULL) continue; /* invalid command line so ignore */
        num = (int)(cp-str);      /* number of chars before equal sign */
        strncat(cmdstr,str,(size_t)num);  /* copy to working string */
        TrimStr(cmdstr);          /* remove leading and trailing blanks */
        found = -1;
        i = 0;
        while (GPR_CONV_CMDS[i])      /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_CONV_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_CONV_CMDS[] */
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
		if (Debug)
		{   printf("cmdstr=%s ; cp = %s\n",cmdstr,cp);
			getch();
		}
		switch (found)  /* cases depend on same order in GPR_CONV_CMD[] */
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
                if (itemp == 0) Debug = FALSE;
                else            Debug = TRUE;
                if (Debug) puts("Debug mode active");
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
                if (itemp == 0) Batch = FALSE;
                else            Batch = TRUE;
                break;
			case 2:
                itemp = atoi(cp);
                if (itemp >0) FilesInfoPtr->num_input_files = itemp;
                if (Debug) printf("num_input_files = %d\n",FilesInfoPtr->num_input_files);
                break;
            case 3:                         /* input_filelist[] */
            {   char *cp_next = (char *)TrimStr(cp);
                int file_found = 0,j;

                /* number of files must be specified first */
                if (FilesInfoPtr->num_input_files <= 0) break;

                /* allocate storage for filenames */
                FilesInfoPtr->dat_infilename = (char **)malloc(FilesInfoPtr->num_input_files * sizeof(char *));
                if (FilesInfoPtr->dat_infilename == NULL)
                {   FilesInfoPtr->num_input_files = 0;
                    if (Debug) puts("\nmalloc failed for dat_infilename");
                    break;      /* break out of case */
                }
                for (i=0; i<FilesInfoPtr->num_input_files; i++)
                {   FilesInfoPtr->dat_infilename[i] = (char *)malloc(MAX_PATHLEN * sizeof(char));
                    if (FilesInfoPtr->dat_infilename[i] == NULL)
                    {   for (j=0; j<i; j++) free(FilesInfoPtr->dat_infilename[j]);
                        free(FilesInfoPtr->dat_infilename);
                        FilesInfoPtr->dat_infilename = NULL;
                        FilesInfoPtr->num_input_files = 0;
                        if (Debug) puts("\nmalloc failed for dat_infilename[]");
                        break;  /* out of for loop */
                    }
                }

                if (FilesInfoPtr->num_input_files == 0) break; /* break out of case */

                /* get the file names */
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (file_found < FilesInfoPtr->num_input_files)
                        {   cp2 = strchr(cp_next,' '); /* look for seperator */
                            if (cp2 != NULL)
                            {   *cp2 = 0;    /* truncate */
                                strcpy(FilesInfoPtr->dat_infilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,FilesInfoPtr->dat_infilename[file_found]);
                                file_found++;
                                *cp2 = ' ';  /* reset separator */
                                /* TrimStr(cp_next); */
                            } else
                            {   strcpy(FilesInfoPtr->dat_infilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,FilesInfoPtr->dat_infilename[file_found]);
                                file_found++;
                            }
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (file_found < FilesInfoPtr->num_input_files)
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
                if (file_found < FilesInfoPtr->num_input_files)
                {   if (Debug) puts("\nfile_found < num_input_files");
                    FilesInfoPtr->num_input_files = 0;
                    for (j=0; j<FilesInfoPtr->num_input_files; j++)
                        free(FilesInfoPtr->dat_infilename[j]);
                    free(FilesInfoPtr->dat_infilename);
                    FilesInfoPtr->dat_infilename = NULL;
                    FilesInfoPtr->num_input_files = 0;
                    dat_in_found = FALSE;
                }
                if (Debug)
                {    for (i=0; i<FilesInfoPtr->num_input_files; i++)
                        printf("File %3d: %s\n",i,FilesInfoPtr->dat_infilename[i]);
                }
                break;
            }
            case 4:
            {   char *cp_next = (char *)TrimStr(cp);
                int file_found = 0,j;

                /* number of files must be specified first */
                if (FilesInfoPtr->num_input_files <= 0) break;

                /* allocate storage for filenames */
                FilesInfoPtr->dat_outfilename = (char **)malloc(FilesInfoPtr->num_input_files * sizeof(char *));
                if (FilesInfoPtr->dat_outfilename == NULL)
                {   FilesInfoPtr->num_input_files = 0;
                    if (Debug) puts("\nmalloc failed for dat_outfilename[][]");
                    break;      /* break out of case */
                }
                for (i=0; i<FilesInfoPtr->num_input_files; i++)
                {   FilesInfoPtr->dat_outfilename[i] = (char *)malloc(MAX_PATHLEN * sizeof(char));
                    if (FilesInfoPtr->dat_outfilename[i] == NULL)
                    {   for (j=0; j<i; j++) free(FilesInfoPtr->dat_outfilename[j]);
                        free(FilesInfoPtr->dat_outfilename);
                        FilesInfoPtr->dat_outfilename = NULL;
                        FilesInfoPtr->num_input_files = 0;
                        if (Debug) puts("\nmalloc failed for dat_outfilename[]");
                        break;  /* out of for loop */
                    }
                }

                if (FilesInfoPtr->num_input_files == 0) break; /* break out of case */

                /* get the file names */
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (file_found < FilesInfoPtr->num_input_files)
                        {   cp2 = strchr(cp_next,' '); /* look for seperator */
                            if (cp2 != NULL)
                            {   *cp2 = 0;    /* truncate */
                                strcpy(FilesInfoPtr->dat_outfilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,FilesInfoPtr->dat_outfilename[file_found]);
                                file_found++;
                                *cp2 = ' ';  /* reset separator */
                                /* TrimStr(cp_next); */
                            } else
                            {   strcpy(FilesInfoPtr->dat_outfilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,FilesInfoPtr->dat_outfilename[file_found]);
                                file_found++;
                            }
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (file_found < FilesInfoPtr->num_input_files)
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
                dat_out_found = TRUE;
                if (file_found < FilesInfoPtr->num_input_files)
                {   if (Debug) puts("\nfile_found < num_input_files");
                    FilesInfoPtr->num_input_files = 0;
                    for (j=0; j<FilesInfoPtr->num_input_files; j++)
                        free(FilesInfoPtr->dat_outfilename[j]);
                    free(FilesInfoPtr->dat_outfilename);
                    FilesInfoPtr->dat_outfilename = NULL;
                    FilesInfoPtr->num_input_files = 0;
                    dat_out_found = FALSE;
                }
                if (Debug)
                {    for (i=0; i<FilesInfoPtr->num_input_files; i++)
                        printf("File %3d: %s\n",i,FilesInfoPtr->dat_outfilename[i]);
                }
                break;
            }
            case 5:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->selected_channel = itemp-1;
                if (Debug) printf("channel=%d\n",InfoPtr->selected_channel);
                break;
            case 6:                      /* other_format */
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
                {   itemp = atoi(cp);
                }
				if (itemp != 0)  InfoPtr->input_format = OTHER;
                if (Debug) printf("input_format = %d\n",InfoPtr->input_format);
                break;
            case 7:
                itemp = atoi(cp);
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
            case 8:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->file_header_bytes = itemp;
                if (Debug) printf("file_header_bytes = %d\n",InfoPtr->file_header_bytes);
                break;
            case 9:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->trace_header_bytes = itemp;
                if (Debug) printf("trace_header_bytes = %d\n",InfoPtr->trace_header_bytes);
                break;
            case 10:
                itemp = atoi(cp);
                if (itemp > 0) InfoPtr->in_samps_per_trace = itemp;
                if (Debug) printf("in_samps_per_trace = %d\n",InfoPtr->in_samps_per_trace);
                break;
            case 11:
				dtemp = atof(cp);
				if (dtemp > 0.0) InfoPtr->in_time_window = dtemp;
				if (Debug) printf("in_time_window = %g\n",InfoPtr->in_time_window);
				break;
            case 12:
                itemp = atoi(cp);
                switch (itemp)
                {   case -1:  case -2:  case -3:  case -5:  case -6:
                    case  1:  case  2:  case  3:  case  4:  case  8:
                        InfoPtr->output_datatype = itemp;
                        break;
                    default:
                        InfoPtr->output_datatype = 0;
                        break;
                }
                if (Debug) printf("output_datatype = %d\n",InfoPtr->output_datatype);
                break;
			case 13:
				itemp = atoi(cp);
                if (itemp > 0) InfoPtr->timezero_sample = itemp;
                if (Debug) printf("timezero_sample = %d\n",InfoPtr->timezero_sample);
                break;
            case 14:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->traces_per_sec = dtemp;
                if (Debug) printf("traces_per_sec = %g\n",InfoPtr->traces_per_sec);
                break;
            case 15:
                itemp = atoi(cp);
                if (itemp > 0) InfoPtr->number_of_stacks = itemp;
                if (Debug) printf("number_of_stacks = %d\n",InfoPtr->number_of_stacks);
                break;
            case 16:
                itemp = atoi(cp);
                if (itemp > 0) InfoPtr->nominal_frequency = itemp;
                if (Debug) printf("nominal_frequency = %d\n",InfoPtr->nominal_frequency);
                break;
            case 17:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->pulser_voltage = dtemp;
                if (Debug) printf("pulser_voltage = %g\n",InfoPtr->pulser_voltage);
                break;
            case 18:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->antenna_separation = dtemp;
                if (Debug) printf("antenna_separation = %g\n",InfoPtr->antenna_separation);
                break;
            case 19:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->antenna_name,cp,sizeof(InfoPtr->antenna_name)-1);
                strcat(InfoPtr->antenna_name,"\0");
                strupr(InfoPtr->antenna_name);
                if (Debug) printf("antenna_name = %s\n",InfoPtr->antenna_name);
				break;
            case 20:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->traces_per_meter = dtemp;
                if (Debug) printf("traces_per_meter = %g\n",InfoPtr->traces_per_meter);
                break;
            case 21:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->meters_per_mark = dtemp;
                if (Debug) printf("meters_per_mark = %g\n",InfoPtr->meters_per_mark);
                break;
            case 22:
                dtemp = atof(cp);
                InfoPtr->starting_position = dtemp;
                if (Debug) printf("starting_position = %g\n",InfoPtr->starting_position);
                break;
            case 23:
                dtemp = atof(cp);
                InfoPtr->final_position = dtemp;
                if (Debug) printf("final_position = %g\n",InfoPtr->final_position);
                break;
            case 24:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->position_step_size = dtemp;
                if (Debug) printf("position_step_size = %g\n",InfoPtr->position_step_size);
                break;
            case 25:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->position_units,cp,sizeof(InfoPtr->position_units)-1);
                strcat(InfoPtr->position_units,"\0");
                /* strupr(InfoPtr->position_units); */
                if (Debug) printf("position_units = %s\n",InfoPtr->position_units);
                break;
            case 26:
                itemp = atoi(cp);
                if (itemp > 0) InfoPtr->time_date_created.year = itemp - 1980;
                if (Debug) printf("year_created = %d\n",InfoPtr->time_date_created.year);
                break;
			case 27:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <= 12) InfoPtr->time_date_created.month = itemp;
                if (Debug) printf("month_created = %d\n",InfoPtr->time_date_created.month);
                break;
            case 28:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <= 31) InfoPtr->time_date_created.day = itemp;
                if (Debug) printf("day_created = %d\n",InfoPtr->time_date_created.day);
                break;
            case 29:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <= 24) InfoPtr->time_date_created.hour = itemp;
                if (Debug) printf("hour_created = %d\n",InfoPtr->time_date_created.hour);
                break;
            case 30:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->time_date_created.min = itemp;
                if (Debug) printf("minute_created = %d\n",InfoPtr->time_date_created.min);
                break;
            case 31:
                itemp = atoi(cp);
                if (itemp > 1) InfoPtr->num_gain_pts = itemp;
                if (Debug) printf("num_gain_pts = %d\n",InfoPtr->num_gain_pts);
                break;
            case 32:
            {   char *cp_next = TrimStr(cp);
                int pts_found = 0;

                /* number of gain points must be specified first */
                if (InfoPtr->num_gain_pts <= 0) break;

                /* allocate storage for gain points */
                InfoPtr->gain_pts = (double *)malloc(InfoPtr->num_gain_pts * sizeof(double));
                if (InfoPtr->gain_pts == NULL)
                {   InfoPtr->num_gain_pts = 0;
                    if (Debug) puts("\nmalloc failed for gain_pts[]");
                    break; /* out of case */
                }

                while (1)
                {   while (*cp_next)    /* work through string from 1 line */
                    {   TrimStr(cp_next);
                        if (pts_found < InfoPtr->num_gain_pts)
						{   InfoPtr->gain_pts[pts_found] = atof(cp_next);
                            pts_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break; /* out of while (*cp_next) */
                        } else
                            break; /* out of while (*cp_next) */
                    }

                    if (pts_found < InfoPtr->num_gain_pts)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {   break; /* out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            if (strchr(str,'='))
                            {   have_string = TRUE;
                                break; /* out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = TrimStr(str);
                            }
                        }
                    } else
                        break; /*  out of while (1) */
                }  /* end of while (1) block */
                if (pts_found < InfoPtr->num_gain_pts)
                {   if (Debug) puts("\npts_found < num_gain_pts");
                    InfoPtr->num_gain_pts = 0;
                    free(InfoPtr->gain_pts);
                    InfoPtr->gain_pts = NULL;
                }
                if (Debug) printf("gain_pts = %0X (pointer)\n",InfoPtr->gain_pts);
                break;
            }
            case 33:
            {   char *cp_next = TrimStr(cp);
                char *cp1;
                char buffer[MAX_TEXT];
                int bytes,num_bytes,max_bytes;
#if 1
                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp3 = cp2 + 1;             /* step past quote */
					cp3 = strchr(cp3,'"');     /* look for second quote */
                    if (cp3 != NULL)
                    {   if (cp3 == cp2+1) break; /* out of case */
                    }
                } else
                    break; /* out of case */
#endif
                num_bytes = 0;
                max_bytes = sizeof(buffer);
                buffer[0] = 0;
                cp1 = buffer;                 /* point to start of buffer */
                while (1)
                {   cp2 = strchr(cp_next,'"'); /* look for first quote */
                    if (cp2 != NULL)
                    {   cp_next = cp2 + 1;     /* step past quote */
                        cp2 = strchr(cp_next,'"');  /* look for second quote */
                        if (cp2 != NULL)
                        {   *cp2 = '\n';       /* change quote to \n */
                            *(cp2 + 1) = 0;    /* truncate string */
                            bytes = strlen(cp_next); /* doesn't count \0 */
                        } else
                            bytes = 0;
                    } else
                        bytes = 0;
                    if (bytes > 0)
                    {   if (bytes > (max_bytes - num_bytes - 1))
                            bytes = max_bytes - num_bytes - 1;
                        strncat(cp1,cp_next,bytes);
                        cp1 += bytes;        /* point to \0 */
                        num_bytes += bytes;
                    }
                    if (num_bytes < max_bytes - 1)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {   break; /* out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            cmdstr[0] = 0;            /* set to 0 so strncat works right */
                            cp = strchr(str,'=');     /* look for equal sign */
                            if (cp == NULL)
                            {   have_string = FALSE;
                                cp_next = str;
                                continue; /* at end of while (1) */
                            }
                            num = (int)(cp-str);      /* number of chars before equal sign */
                            strncat(cmdstr,str,(size_t)num);  /* copy to working string */
                            TrimStr(cmdstr);          /* remove leading and trailing blanks */
                            found = FALSE;
                            i = 0;
                            while (GPR_CONV_CMDS[i])  /* requires last member to be NULL */
                            {   if (strcmp(strlwr(cmdstr),GPR_CONV_CMDS[i]) == 0)
                                {   found = TRUE;
                                    break; /* out of while (GPR_CONV_CMDS[i]) */
                                }
                                i++;
                            }
                            if (found == TRUE)
                            {   have_string = TRUE;
                                break; /* out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = str;
                            }
                        }
                    } else
                        break; /*  out of while (1) */
                }  /* end of while (1) block */
                if (num_bytes > 0)
                {   num_bytes ++;    /* add in terminating \0 */
                    InfoPtr->text_bytes = num_bytes;
                    /* allocate storage for text[] */
                    InfoPtr->text = (char *)malloc(InfoPtr->text_bytes * sizeof(char));
                    if (InfoPtr->text == NULL)
                    {   InfoPtr->text_bytes = 0;
                        if (Debug) puts("\nmalloc failed for text[]");
                        break; /* out of case */
                    }
                    for (i=0; i<InfoPtr->text_bytes; i++)
                        InfoPtr->text[i] = buffer[i];
                    InfoPtr->text[InfoPtr->text_bytes-1] = 0;
                    if (Debug) printf("text = %s\n",InfoPtr->text);
                }
                if (Debug)
                {   if (InfoPtr->text) printf("text = %s\n",InfoPtr->text);
                    else puts("no text");
                }
                break;
            }
            case 34:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->survey_mode,cp,sizeof(InfoPtr->survey_mode)-1);
                strcat(InfoPtr->survey_mode,"\0");
                /* strupr(InfoPtr->survey_mode); */
                if (Debug) printf("survey_mode = %s\n",InfoPtr->survey_mode);
                break;
            case 35:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->nominal_RDP = dtemp;
                if (Debug) printf("nominal_RDP = %g\n",InfoPtr->nominal_RDP);
                break;
            case 36:
            {   char *cp_next = TrimStr(cp);
                char *cp1;
                char buffer[MAX_TEXT];
                int bytes,num_bytes,max_bytes;

                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp3 = cp2 + 1;             /* step past quote */
                    cp3 = strchr(cp3,'"');     /* look for second quote */
                    if (cp3 != NULL)
                    {   if (cp3 == cp2+1) break; /* out of case */
                    } else
                    {   break; /* out of case */
                    }
                } else
                    break; /* out of case */

                num_bytes = 0;
                max_bytes = sizeof(buffer);
                buffer[0] = 0;
                cp1 = buffer;                 /* point to start of buffer */
                while (1)
                {   cp2 = strchr(cp_next,'"'); /* look for first quote */
                    if (cp2 != NULL)
                    {   cp_next = cp2 + 1;     /* step past quote */
                        cp2 = strchr(cp_next,'"');  /* look for second quote */
                        if (cp2 != NULL)
                        {   *cp2 = '\n';       /* change quote to \n */
                            *(cp2 + 1) = 0;    /* truncate string */
                            bytes = strlen(cp_next); /* doesn't count \0 */
                        } else
                            bytes = 0;
                    } else
                        bytes = 0;
                    if (bytes > 0)
                    {   if (bytes > (max_bytes - num_bytes - 1))
                            bytes = max_bytes - num_bytes - 1;
                        strncat(cp1,cp_next,bytes);
                        cp1 += bytes;        /* point to \0 */
                        num_bytes += bytes;
                    }
                    if (num_bytes < max_bytes - 1)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {   break; /* out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            cmdstr[0] = 0;            /* set to 0 so strncat works right */
                            cp = strchr(str,'=');     /* look for equal sign */
                            if (cp == NULL)
                            {   have_string = FALSE;
                                cp_next = str;
                                continue; /* at end of while (1) */
                            }
                            num = (int)(cp-str);      /* number of chars before equal sign */
                            strncat(cmdstr,str,(size_t)num);  /* copy to working string */
                            TrimStr(cmdstr);          /* remove leading and trailing blanks */
                            found = FALSE;
                            i = 0;
                            while (GPR_CONV_CMDS[i])  /* requires last member to be NULL */
                            {   if (strcmp(strlwr(cmdstr),GPR_CONV_CMDS[i]) == 0)
                                {   found = TRUE;
                                    break; /* out of while (GPR_CONV_CMDS[i]) */
                                }
                                i++;
                            }
                            if (found == TRUE)
                            {   have_string = TRUE;
                                break; /* out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = str;
                            }
                        }
                    } else
                        break; /* out of while (1) */
                }  /* end of while (1) block */
                if (num_bytes > 0)
                {   num_bytes ++;    /* add in terminating \0 */
                    InfoPtr->proc_hist_bytes = num_bytes;
                    /* allocate storage for proc_hist[] */
                    InfoPtr->proc_hist = (char *)malloc(InfoPtr->proc_hist_bytes * sizeof(char));
                    if (InfoPtr->proc_hist == NULL)
                    {   InfoPtr->proc_hist_bytes = 0;
                        if (Debug) puts("\nmalloc failed for proc_hist[]");
                        break; /* out of case */
                    }
                    for (i=0; i<InfoPtr->proc_hist_bytes; i++)
                        InfoPtr->proc_hist[i] = buffer[i];
                    InfoPtr->proc_hist[InfoPtr->proc_hist_bytes-1] = 0;
                    if (Debug) printf("proc_hist = %s\n",InfoPtr->proc_hist);
                }
                if (Debug)
                {   if (InfoPtr->proc_hist) printf("proc_hist = %s\n",InfoPtr->proc_hist);
                    else puts("no proc_hist");
                }
                break;
            }
            case 37:
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
                if (itemp == 0) FilesInfoPtr->use_mark_file = FALSE;
                else            FilesInfoPtr->use_mark_file = TRUE;
                if (Debug) printf("use_mark_file = %s\n",FilesInfoPtr->use_mark_file);
                break;
            case 38:
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
                if (itemp == 0) FilesInfoPtr->use_xyz_file = FALSE;
                else            FilesInfoPtr->use_xyz_file = TRUE;
                if (Debug) printf("use_xyz_file = %s\n",FilesInfoPtr->use_xyz_file);
                break;
            default:
                break;
        }
    }
    fclose(infile);
    if (!dat_out_found)  return 2;
    if (!dat_in_found)   return 3;
    return 0;
}
/**************************** DisplayParameters() ***************************/
/* Display parameters to CRT.  Also calculates approximate storage requirements.
 *
 * Parameter list:
 *  struct GprInfoStruct *InfoPtr - address of structure
 *  struct FilesInfoStruct *FilesInfoPtr - address of structure
 *
 * Requires: <stdio.h>, "gpr_conv.h".
 * Calls: printf, puts.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS
 * Date: January 28, 1997, Aug 11, 1997
 * Revisions: March 31, 2000; April 4, 2000;
 */
void DisplayParameters (struct GprInfoStruct *InfoPtr,
                        struct FilesInfoStruct *FilesInfoPtr)
{
	int i,num,len;
    int lines = 0, maxlines = 21;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
	extern int Batch,Debug,ANSI_THERE;

	if (Debug) puts("DisplayParameters()");
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    /* if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); */ /* red on black */
    if (ANSI_THERE) printf("%c%c33;40m",0x1B,0x5B); /* yellow on black */
    printf("\nGPR_CONV v. %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_CONV_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

    printf("cmd_filename = %s\n",strupr(InfoPtr->cmd_filename));
    printf("input data files [%d]:\n",FilesInfoPtr->num_input_files);
    lines = 4;
    len = 0;
    num = 5;
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {  if (strlen(FilesInfoPtr->dat_infilename[i]) > len)
           len = strlen(FilesInfoPtr->dat_infilename[i])+1;
    }
    num = 80 / len;
    if (num < 1) num = 1;
    if (num > 5) num = 5;
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {   printf("%s ",FilesInfoPtr->dat_infilename[i]);
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

    printf("output data files [%d]:\n",FilesInfoPtr->num_input_files);
    len = 0;
    num = 5;
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {  if (strlen(FilesInfoPtr->dat_outfilename[i]) > len)
           len = strlen(FilesInfoPtr->dat_outfilename[i])+1;
    }
    num = 80 / len;
    if (num < 1) num = 1;
    if (num > 5) num = 5;
    for (i=0; i<FilesInfoPtr->num_input_files; i++)
    {   printf("%s ",FilesInfoPtr->dat_outfilename[i]);
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

    printf("For DZT files only: channel = %d \n",InfoPtr->selected_channel+1);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    printf("Marker files %s be used\n",FilesInfoPtr->use_mark_file ? "will" : "will not");
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    printf("XYZ files %s be used\n",FilesInfoPtr->use_xyz_file ? "will" : "will not");
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    puts("");
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    if (ANSI_THERE) printf("%c%c36;40m",0x1B,0x5B); /* cyan on black */
    puts("USER INPUT FOR CONVERSIONS:");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->input_format == OTHER)
    {   printf("file header bytes = %d       trace header bytes = %d\n",
            InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("samples per trace = %d       bits per sample = %d\n",
            InfoPtr->in_samps_per_trace,InfoPtr->in_bits_per_samp);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("total time window = %g ns    timezero at sample %d\n",
            InfoPtr->in_time_window,InfoPtr->timezero_sample);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("Input data type = %d ",InfoPtr->input_datatype);
        switch (InfoPtr->output_datatype)
        {   case  0:  printf("(use output format default)\n");       break;
            case -1:  printf("(1-byte unsigned characters)\n");      break;
            case  1:  printf("(1-byte signed characters)\n");        break;
            case -2:  printf("(2-byte unsigned short integers)\n");  break;
            case  2:  printf("(2-byte signed short integers)\n");    break;
            case -3:  printf("(4-byte unsigned long integers)\n");   break;
            case  3:  printf("(4-byte signed long integers)\n");     break;
            case  4:  printf("(4-byte floating point real)\n");      break;
            case -5:  printf("(2-byte unsigned short integers)\n");  break;
            case -6:  printf("(4-byte unsigned long integers)\n");   break;
            case  8:  printf("(8-byte floating point real)\n");      break;
        }
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }

    if (InfoPtr->output_datatype != 0)
    {   printf("Requested output data type = %d ",InfoPtr->output_datatype);
        switch (InfoPtr->output_datatype)
        {   case  0:  printf("(use output format default)\n");         break;
            case -1:  printf("(1-byte unsigned characters)\n");        break;
            case  1:  printf("(1-byte signed characters)\n");          break;
            case -2:  printf("(2-byte unsigned short integers)\n");    break;
            case  2:  printf("(2-byte signed short integers)\n");      break;
            case -3:  printf("(4-byte unsigned long integers)\n");     break;
            case  3:  printf("(4-byte signed long integers)\n");       break;
            case  4:  printf("(4-byte floating point real)\n");        break;
            case -5:  printf("(2-byte unsigned short integers)\n");    break;
            case -6:  printf("(4-byte unsigned long integers)\n");     break;
            case  8:  printf("(8-byte floating point real)\n");        break;
        }
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->traces_per_sec > 0.0)
    {   printf("traces per sec = %g\n",InfoPtr->traces_per_sec);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->number_of_stacks > 1)
    {   printf("number of stacks = %d\n",InfoPtr->number_of_stacks);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->nominal_frequency > 0)
    {   printf("nominal frequency = %d\n",InfoPtr->nominal_frequency);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->pulser_voltage > 0.0)
    {   printf("pulser voltage = %g\n",InfoPtr->pulser_voltage);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->antenna_separation > 0.0)
    {   printf("antenna separation = %g\n",InfoPtr->antenna_separation);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->antenna_name[0])
    {   printf("antenna name = %s\n",InfoPtr->antenna_name);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->traces_per_meter > 0.0)
    {   printf("traces per meter = %g\n",InfoPtr->traces_per_meter);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->meters_per_mark > 0.0)
    {   printf("meters_per_mark = %g\n",InfoPtr->meters_per_mark);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->starting_position != 0.0)
    {   printf("starting position = %g\n",InfoPtr->starting_position);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->final_position != 0.0)
    {   printf("final position = %g\n",InfoPtr->final_position);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->position_step_size > 0.0)
    {   printf("position step size = %g\n",InfoPtr->position_step_size);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->position_units[0])
    {   printf("position units are in %s\n",InfoPtr->position_units);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->time_date_created.month > 0)
    {   printf("create date = %s %d, %d at %02d:%02d:%02d\n",
               month_names[InfoPtr->time_date_created.month],
               InfoPtr->time_date_created.day,
               InfoPtr->time_date_created.year+1980,
               InfoPtr->time_date_created.hour,InfoPtr->time_date_created.min,
               2 * InfoPtr->time_date_created.sec2);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->time_date_modified.month > 0)
    {   printf("modified date = %s %d, %d at %02d:%02d:%02d\n",
               month_names[InfoPtr->time_date_modified.month],
               InfoPtr->time_date_modified.day,
               1980 + InfoPtr->time_date_modified.year,
               InfoPtr->time_date_modified.hour,InfoPtr->time_date_modified.min,
               2 * InfoPtr->time_date_modified.sec2);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->num_gain_pts > 0)
    {   printf("num_gain_pts = %g\n",InfoPtr->num_gain_pts);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("Sample Number    Gain (db)\n");
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        for (i=0; i<InfoPtr->num_gain_pts; i++)
        {   printf("%8d      %8.2f\n",
                (int)(i * ( (double)(InfoPtr->in_samps_per_trace-1) /
                      (InfoPtr->num_gain_pts-1) ) ),
                InfoPtr->gain_pts[i]);
            lines++;
            if (!Batch && lines >= maxlines)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
        }
    }
    if (InfoPtr->proc_hist_bytes > 0)
    {   printf("size of processing history = %d bytes\n",InfoPtr->proc_hist_bytes);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("    processing history =\n");
        if (InfoPtr->input_format == DZT)
        {   static char big_buff[1024]; /* removes from stack and places in data segment */
            ConvertProcHist2((int)sizeof(big_buff),big_buff,
                             InfoPtr->proc_hist_bytes,InfoPtr->proc_hist);
            i = 0;
            while (big_buff[i])   /* anticipate */
            {   if (big_buff[i] == '\n') lines++;
                i ++;
            }
            if (!Batch && lines >= maxlines)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
            printf("%s\n",big_buff);
        } else   /* only DZT files have binary-coded history */
        {   i = 0;
            while (InfoPtr->proc_hist[i])
            {   if (InfoPtr->proc_hist[i] == '\n') lines++;
                i ++;
            }
            if (!Batch && lines >= maxlines)      /* anticipate */
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
            printf("%s\n",InfoPtr->proc_hist);
        }
    }
    if (InfoPtr->text_bytes > 0)
    {   printf("size of text = %d bytes\n",InfoPtr->text_bytes);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        if (InfoPtr->text_bytes && InfoPtr->text)
        {   printf("    text =\n%s\n",InfoPtr->text);
            if (!Batch && lines >= maxlines)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
        }
    }
    if (InfoPtr->survey_mode[0])
    {   printf("survey_mode = %s\n",InfoPtr->survey_mode);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->nominal_RDP > 0)
    {   printf("nominal RDP = %g\n",InfoPtr->nominal_RDP);
        lines++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }

    /* Issue messages */
    if (Debug)       puts("In TESTing mode.");
    else if (Batch)  puts("In Batch processing mode.");
}
/********************************* GetInfo() ********************************/
/* Get the data/info from the appropriate files.
 * Allocate/De-allocate storage for grid[][].
            Verify valid parameters and ranges.
 *
 * NOTE: The "created" member of struct GprInfoStruct restricts the
 *       application of the struct to one data set at a time.
 *       To read multiple data sets, either de-allocate the first one OR
 *       declare multiple structs to hold the information and pointers.
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct GprInfoStruct *InfoPtr - address of structure
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "jl_defs.h", "assertjl.h",
 *           <time.h>, "gpr_conv.h", "gpr_io.h".
 * Calls: strcpy, strstr, strlen, malloc, free, printf, puts, strncpy, time,
 *        strncpy, strupr,
 *        ReadOneDztHeader, GetDztChSubGrid8, GetDztChSubGrid16,
 *        PrintOneDztHeader, InitDt1Parameters, GetSsHdFile, PrintSsHdInfo,
 *        ReadSegyReelHdr, PrintSegyReelHdr, ExtractSsInfoFromSegy.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *GetInfoMsg[] =
{   "GetInfo(): No errors.",
    "GetInfo() Error: Attemp made to de-allocate storage before allocating.",
    "GetInfo() Error: Attemp made to re-allocate storage.  De-allocate first.",
    "GetInfo() Error: No recognized data/info input files specified.",
    "GetInfo() Error: Problem de-allocating storage.",

    "GetInfo() Error: Problem getting information about input data file.",
    "GetInfo() Error: Not able to allocate sufficient temporary storage.",
    "GetInfo() Error: Problem getting data from input file.",
    "GetInfo() Error: Invalid channel selection for DZT file.",
    "GetInfo() Error: Invalid type for input data.",

    "GetInfo() Error: Less than one trace in file.",
    "GetInfo() Error: Less than two samples per trace.",
    "GetInfo() Error: Less than one byte per sample.",
    "GetInfo() Error: Less than 0 or 0 ns per sample.",
    "GetInfo() Error: Input Data type not recognized.",

    "GetInfo() Error: Problem getting data from input MRK file.",
    "GetInfo() Error: Problem getting data from input XYZ file.",
    "GetInfo() Error: Number of records in MRK and XYZ disagree.",
    "GetInfo() Error: SEG-Y data type (format) not recognized.",
    "GetInfo() Error: Output Data type not recognized.",

    "GetInfo() Error: Unable to determine input file storage format.",
} ;
 *
 * Author: Jeff Lucius   USGS
 * Date: January 28, 1997, Aug 11, 1997
 * Revisions: March 31, 2000;
 */
int GetInfo (int command,struct GprInfoStruct *InfoPtr)
{
/***** Declare variables *****/

    int  i,itemp;             /* scratch variables and counters */
    int  file_hdr_bytes,num_hdrs,trace_hdr_bytes,samples_per_trace;
    int  num_channels,input_datatype,total_time,file_type;
    long num_traces;
    struct tm *newtime;
    time_t long_time;

    extern int Debug;
    extern int ANSI_THERE;

/***** Deallocate storage *****/

    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->created) return 1;
        DeallocInfoStruct(InfoPtr);
        return 0;
    }

/***** Test for balanced calls *****/

    if (InfoPtr->created) return 2;

/***** Determine GPR file storage format *****/
    itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                &samples_per_trace,&num_channels,&num_traces,
                &input_datatype,&total_time,&file_type,
                InfoPtr->data_filename);
    if (itemp > 0) return 20; /* printf("%s\n",GetGprFileTypeMsg[itemp]); */
    InfoPtr->input_format = file_type;

/***** Get tick mark trace locations and XYZ data if necessary *****/

    if (InfoPtr->mrk_filename[0] || InfoPtr->xyz_filename[0])
    {   int num_mrk_ticks,num_xyz_ticks;
        extern const char *GetMrkDataMsg[];
        extern const char *GetXyzDataMsg[];

        if (InfoPtr->mrk_filename[0])
        {   itemp = GetMrkData(InfoPtr->mrk_filename,&num_mrk_ticks,
                               &(InfoPtr->marks));
            if (Debug) printf("mrk_filename=%s  num_mrk_ticks=%d\n",
                                InfoPtr->mrk_filename,num_mrk_ticks);
            if (Debug) getch();
            if (itemp > 0)
            {   printf("\n%s\n",GetMrkDataMsg[itemp]);
                DeallocInfoStruct(InfoPtr);
                InfoPtr->num_marks = 0;
                return 15;
            }
            InfoPtr->num_marks = num_mrk_ticks;
        }
        if (InfoPtr->xyz_filename[0])
        {   itemp = GetXyzData(InfoPtr->xyz_filename,&num_xyz_ticks,
                                &(InfoPtr->XYZ));
            if (Debug) printf("xyz_filename=%s  num_xyz_ticks=%d\n",
                                InfoPtr->xyz_filename,num_xyz_ticks);
            if (Debug) getch();
            if (itemp > 0)
            {   printf("\n%s\n",GetXyzDataMsg[itemp]);
                DeallocInfoStruct(InfoPtr);
                InfoPtr->num_marks = 0;
                return 16;
            }
            if (InfoPtr->num_marks > 0)
            {   if (num_xyz_ticks != InfoPtr->num_marks)
                {   DeallocInfoStruct(InfoPtr);
                    InfoPtr->num_marks = 0;
                    return 17;
                }
            } else
                InfoPtr->num_marks = num_xyz_ticks;
        }
    }
/***** Get necessary information from info file/header *****/

    switch (InfoPtr->input_format)
    {

		case DZT:  case MOD_DZT:  /****************************************/
        {   char *cp,*cp1,*cp2,newstr[32];
            int num_hdrs,header_bytes;
            long num_traces;
            struct DztHdrStruct hdr;
            extern const char *ReadOneDztHeaderMsg[]; /* in gpr_io.h */
            extern const char *ant_number[];          /* in gpr_io.h */
            static char big_buff[1024]; /* removes from stack and places in data segment */

            /* get selected DZT header in file */
            itemp = ReadOneDztHeader(InfoPtr->data_filename,&num_hdrs,
                     &num_traces,InfoPtr->selected_channel,&header_bytes,&hdr);
            if (itemp > 0)
            {   if (ANSI_THERE)  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
                printf("\n%s\n",ReadOneDztHeaderMsg[itemp]);
                if (ANSI_THERE)  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                {   DeallocInfoStruct(InfoPtr);
                    return 5;
                }
            }
            if (Debug)
            {   itemp = PrintOneDztHeader(header_bytes,num_traces,"stdout",
                                         InfoPtr->data_filename,&hdr);
                if (itemp > 0)
                {   if (ANSI_THERE)  printf("%c%c31;40m",0x1B,0x5B);  /* red on black */
                    printf("\n%s\n",PrintOneDztHeaderMsg[itemp]);
                    if (ANSI_THERE)  printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
                }
                getch();
            }

            /* assign required values */
            if      (hdr.rh_bits ==  8) InfoPtr->input_datatype = -1;
            else if (hdr.rh_bits == 16) InfoPtr->input_datatype = -2;
            else if (hdr.rh_bits == 32) InfoPtr->input_datatype = -3;
            InfoPtr->total_traces        = num_traces;
            InfoPtr->num_input_channels  = num_hdrs;
            InfoPtr->file_header_bytes   = header_bytes;
            InfoPtr->trace_header_bytes  = 0;
            InfoPtr->in_samps_per_trace  = hdr.rh_nsamp;
            InfoPtr->in_bits_per_samp    = hdr.rh_bits;
            InfoPtr->in_time_window      = hdr.rh_range;

            /* assign rest of values */
            if (hdr.rh_range > 0.0 && hdr.rh_nsamp > 0)
            {   InfoPtr->timezero_sample = (-hdr.rh_position/hdr.rh_range) *
                                            (hdr.rh_nsamp - 1);
            } else
                InfoPtr->timezero_sample = 0;
            InfoPtr->traces_per_sec      = hdr.rh_sps;
            if (hdr.rh_nproc > 0)
            {   ConvertProcHist2((int)sizeof(big_buff),big_buff,(int)hdr.rh_nproc,
                                 ((char *)&hdr + hdr.rh_proc));
                if (strlen(big_buff))
                {   if ((cp = strstr(big_buff,"Static stacking N=")) != NULL)
                    {    cp = strstr(cp,"N");
                         cp += 2;
                         InfoPtr->number_of_stacks = atoi(cp);
                    }
                }
            }
            strcpy(newstr,hdr.rh_antname);
            /* GSSI changed format to include frequency; old version
               just has 4-digit model number */
            if (InfoPtr->nominal_frequency <= 0)
            {   if (strlen(hdr.rh_antname) > 4)  newstr[4] = 0;
                for (i=0; ant_number[i]; i++)
                {   if (strstr(ant_number[i],newstr))
                    {InfoPtr->nominal_frequency = atoi(ant_name[i]);
                        break; /* out of for loop */
                    }
                }
            }
            strcpy(InfoPtr->antenna_name,hdr.rh_antname);
            InfoPtr->traces_per_meter        = hdr.rh_spm;
            InfoPtr->meters_per_mark         = hdr.rh_mpm;
            InfoPtr->time_date_created.year  = hdr.rh_create.year;
            InfoPtr->time_date_created.month = hdr.rh_create.month;
            InfoPtr->time_date_created.day   = hdr.rh_create.day;
            InfoPtr->time_date_created.min   = hdr.rh_create.min;
            InfoPtr->time_date_created.sec2  = hdr.rh_create.sec2;
            if (hdr.rh_rgain != 0 && hdr.rh_nrgain != 0)
            {   InfoPtr->num_gain_pts =
                            *(unsigned short *)((char *)&hdr + hdr.rh_rgain);
                if (InfoPtr->num_gain_pts > 0)
                {   InfoPtr->gain_pts = (double *)malloc(InfoPtr->num_gain_pts * sizeof(double));
                    if (InfoPtr->gain_pts == NULL)
                    {   puts("GetInfo(): DZT can't allocate gain_pts.");
                        DeallocInfoStruct(InfoPtr);
                        return 6;
                    }
                    for (i=0; i<InfoPtr->num_gain_pts; i++)
                        InfoPtr->gain_pts[i] =
                            *(float *)((char *)&hdr + hdr.rh_rgain + 2 + 4*i);
                }
            }
            if (hdr.rh_proc != 0 && hdr.rh_nproc != 0)
            {   InfoPtr->proc_hist_bytes = hdr.rh_nproc;
                InfoPtr->proc_hist = (char *)malloc(InfoPtr->proc_hist_bytes);
                if (InfoPtr->proc_hist == NULL)
                {   puts("GetInfo(): DZT can't allocate proc_hist.");
                    DeallocInfoStruct(InfoPtr);
                    return 6;
                }
                for (i=0; i<InfoPtr->proc_hist_bytes; i++)
                     InfoPtr->proc_hist[i] = *((char *)&hdr + hdr.rh_proc + i);
            }
            if ( (InfoPtr->text == NULL || InfoPtr->text[0] == 0 ) &&
                 (hdr.rh_text != 0 && hdr.rh_ntext != 0) )
            {   InfoPtr->text_bytes = hdr.rh_ntext;
                InfoPtr->text = (char *)malloc(InfoPtr->text_bytes);
                if (InfoPtr->text == NULL)
                {   puts("GetInfo(): DZT can't allocate text.");
                    DeallocInfoStruct(InfoPtr);
                    return 6;
                }
                if (Debug)
                {   printf("text_bytes = %d rh_text = %d\n",InfoPtr->text_bytes,hdr.rh_ntext);
                    printf("rh_text =%s||\n",((char *)&hdr + hdr.rh_text));
                    getch();
                }
                strncpy(InfoPtr->text,((char *)&hdr + hdr.rh_text),InfoPtr->text_bytes-1);
                if (Debug)
                {   printf("text =%s||\n",InfoPtr->text);
                    getch();
                }
                InfoPtr->text[InfoPtr->text_bytes-1] = 0;
                if (Debug)
                {   printf("text =%s||\n",InfoPtr->text);
                    getch();
                }
            }
            if (InfoPtr->nominal_RDP <= 0.0)
                InfoPtr->nominal_RDP = hdr.rh_epsr;

            /* assign DT1/HD stuff */
            InfoPtr->job_number[0]   = 0;
            strcpy(InfoPtr->job_number,"8000");
            InfoPtr->title1[0]       = 0;
            InfoPtr->title2[0]       = 0;
            if (InfoPtr->text != NULL && InfoPtr->text[0] != 0)
            {   if ((cp1 = strchr(InfoPtr->text,'\n')) != NULL)
                {   i = cp1 - InfoPtr->text + 1;
                    strncpy(InfoPtr->title1,InfoPtr->text,_MINN(69,i));
                    InfoPtr->title1[_MINN(69,i)] = 0;
#if 0
                    cp2 = cp1 + 1;
                    if ((cp1 = strchr(cp2,'\n')) != NULL)
                    {   i = cp1 - cp2 + 1;
                        strncpy(InfoPtr->title2,cp2,_MINN(69,i));
                        InfoPtr->title2[_MINN(69,i)] = 0;
                    } else
                    {   strncpy(InfoPtr->title2,cp2,69);
                        InfoPtr->title2[69] = 0;
                    }
#endif
                } else
                {   strncpy(InfoPtr->title1,InfoPtr->text,69);
                    InfoPtr->title1[69] = 0;
                }
            }
            InfoPtr->position        = 0.0;
            InfoPtr->num_samples     = InfoPtr->in_samps_per_trace;
            InfoPtr->elevation       = 0.0;
            InfoPtr->num_bytes       = 2.0;
            InfoPtr->num_stacks      = InfoPtr->number_of_stacks;
            InfoPtr->time_window     = InfoPtr->in_time_window;
            InfoPtr->timezero_adjust = 0.0;
            InfoPtr->zero_flag       = 0.0;
            InfoPtr->time_of_day     = 0.0;
            InfoPtr->comment_flag    = 0.0;
            for (i=0; i<sizeof(InfoPtr->comment); i++)
                InfoPtr->comment[i] = 0;

            if (Debug) puts("GetInfo: end of DZT section");
            break;
        }

        case DT1:   /*************************************************/
        {   int print_it;
            int len;
            struct SsHdrInfoStruct  HdInfo;     /* buffer to hold info from HD file */

            /* Get info from HD file */
            InitDt1Parameters(&HdInfo);
            if (Debug)  print_it = 1;
            else        print_it = 0;
            itemp = GetSsHdFile(print_it,&HdInfo,InfoPtr->info_filename);
            if (itemp > 0)
            {   printf("\n%s\n",GetSsHdFileMsg[itemp]);
                DeallocInfoStruct(InfoPtr);
                return 5;
            }
            if (Debug) PrintSsHdInfo("stdout",InfoPtr->info_filename,&HdInfo);
            if (Debug) if (!getch()) getch();

            /* assign required values */
            InfoPtr->input_datatype      = 2;
            InfoPtr->total_traces        = HdInfo.num_traces;
            InfoPtr->file_header_bytes   = 0;
            InfoPtr->trace_header_bytes  = 128;
            InfoPtr->in_samps_per_trace  = HdInfo.num_samples;
            InfoPtr->in_bits_per_samp     = 16;
            InfoPtr->in_time_window      = HdInfo.total_time_ns;

            /* assign rest of values */
            strncpy(InfoPtr->job_number,HdInfo.job_number,sizeof(InfoPtr->job_number)-1);
            InfoPtr->job_number[sizeof(InfoPtr->job_number)-1] = 0;
            strncpy(InfoPtr->title1,HdInfo.title1,sizeof(InfoPtr->title1)-1);
            InfoPtr->title1[sizeof(InfoPtr->title1)-1] = 0;
            strncpy(InfoPtr->title2,HdInfo.title2,sizeof(InfoPtr->title2)-1);
            InfoPtr->title2[sizeof(InfoPtr->title2)-1] = 0;
            len = strlen(InfoPtr->title1) + 1;
            len += strlen(InfoPtr->title2) + 1;
            InfoPtr->text = (char *)malloc(len * sizeof(char));
            if (InfoPtr->text == NULL)
            {   if (Debug) puts("Unable to allocate storage for text");
                InfoPtr->text_bytes = 0;
            } else
            {   InfoPtr->text_bytes = len;
                InfoPtr->text[0] = 0;
                strcat(InfoPtr->text,InfoPtr->title1);
                strcat(InfoPtr->text,"\n");
                strcat(InfoPtr->text,InfoPtr->title2);
            }
            InfoPtr->timezero_sample     = HdInfo.time_zero_sample;
            InfoPtr->number_of_stacks    = HdInfo.num_stacks;
            InfoPtr->nominal_frequency   = HdInfo.ant_freq;
            InfoPtr->pulser_voltage      = HdInfo.pulser_voltage;
            InfoPtr->antenna_separation  = HdInfo.ant_sep;
            if (HdInfo.step_size != 0.0)
                InfoPtr->traces_per_meter = 1.0 / HdInfo.step_size;
            else
                InfoPtr->traces_per_meter = 0.0;
            InfoPtr->starting_position   = HdInfo.start_pos;
            InfoPtr->final_position      = HdInfo.final_pos;
            InfoPtr->position_step_size  = HdInfo.step_size;
            strncpy(InfoPtr->position_units,HdInfo.pos_units,sizeof(InfoPtr->position_units)-1);
            InfoPtr->position_units[sizeof(InfoPtr->position_units)-1] = 0;
            if (HdInfo.year < 70) HdInfo.year += 100; /* if >= 2000 ie. 00 */
            InfoPtr->time_date_created.year  = HdInfo.year - 80; /* year - 1980; 0-127 */
            InfoPtr->time_date_created.month = HdInfo.month;
            InfoPtr->time_date_created.day   = HdInfo.day;
            strncpy(InfoPtr->survey_mode,HdInfo.survey_mode,sizeof(InfoPtr->survey_mode)-1);
            InfoPtr->survey_mode[sizeof(InfoPtr->survey_mode)-1] = 0;
            if (HdInfo.proc_hist != NULL)
            {   int len = strlen(HdInfo.proc_hist);
                if (len > 0)
                {   InfoPtr->proc_hist_bytes = len + 1;
                    InfoPtr->proc_hist = (char *)malloc(InfoPtr->proc_hist_bytes);
                    if (InfoPtr->proc_hist == NULL)
                    {   puts("GetInfo(): DT1: can't allocate proc_hist.");
                        DeallocInfoStruct(InfoPtr);
                        return 6;
                    }
                    InfoPtr->proc_hist[0] = 0;
                    strcpy(InfoPtr->proc_hist,HdInfo.proc_hist);
                    InfoPtr->proc_hist[InfoPtr->proc_hist_bytes-1] = 0;
                }
            }
            if (Debug) printf("\nInfoPtr->proc_hist=%s\n\nHdInfo.proc_hist=%s",InfoPtr->proc_hist,HdInfo.proc_hist);
            if (Debug) puts("GetInfo: end of DT1 section");
            break;
        }

        case SGY:   /*************************************************/
        {   int swap_bytes;
            long num_traces;
            struct SegyReelHdrStruct hdr;
            extern const char *ReadSegyReelHdrMsg[];

            itemp = ReadSegyReelHdr(InfoPtr->info_filename,&swap_bytes,
                                    &num_traces,&hdr);
            if (itemp > 0)
            {   printf("\n%s\n",ReadSegyReelHdrMsg[itemp]);
                DeallocInfoStruct(InfoPtr);
                return 5;
            }
            if (Debug) PrintSegyReelHdr(swap_bytes,num_traces,"stdout",
                                        InfoPtr->info_filename,&hdr);

            /* assign required values */
            switch(hdr.format)
            {   case 1:
                    InfoPtr->input_datatype = 4;       /* float */
                    InfoPtr->in_bits_per_samp = 32;
                    break;
                case 2:  case 4:
                    InfoPtr->input_datatype = 3;       /* long signed int */
                    InfoPtr->in_bits_per_samp = 32;
                    break;
                case 3:
                    InfoPtr->input_datatype = 2;       /* short signed int */
                    InfoPtr->in_bits_per_samp = 16;
                    break;
                default:
                    DeallocInfoStruct(InfoPtr);
                    return 18;
                    /* break;  * unreachable code */
            }
            InfoPtr->total_traces        = num_traces;
            InfoPtr->file_header_bytes   = sizeof(struct SegyReelHdrStruct);
            InfoPtr->trace_header_bytes  = sizeof(struct SegyTraceHdrStruct);
            InfoPtr->in_samps_per_trace  = hdr.hns;
            InfoPtr->in_time_window   =
                 (hdr.hns - 1) * ((double)hdr.hdt / 1000.); /* was picosec */

            /* assign DT1/HD stuff */
            InfoPtr->job_number[0]   = 0;
            strcpy(InfoPtr->job_number,"8000");
            InfoPtr->title1[0]       = 0;
            InfoPtr->title2[0]       = 0;
            InfoPtr->position        = 0.0;
            InfoPtr->num_samples     = InfoPtr->in_samps_per_trace;
            InfoPtr->elevation       = 0.0;
            InfoPtr->num_bytes       = 2.0;
            InfoPtr->num_stacks      = InfoPtr->number_of_stacks;
            InfoPtr->time_window     = InfoPtr->in_time_window;
            InfoPtr->timezero_adjust = 0.0;
            InfoPtr->zero_flag       = 0.0;
            InfoPtr->time_of_day     = 0.0;
            InfoPtr->comment_flag    = 0.0;
            for (i=0; i<sizeof(InfoPtr->comment); i++)
                InfoPtr->comment[i] = 0;

            /* assign rest of values; NOTE these may override above values */
            ExtractSsInfoFromSegy(InfoPtr,&hdr);
            if (Debug) puts("GetInfo: end of SGY section");
            break;
        }

        case OTHER:  /*************************************************/
        {   int error = 0;
            char *error_msg[] =
            {   "No error",
                "file_header_bytes < 0",
                "trace_header_bytes < 0",
                "in_samps_per_trace < 1",
                "in_time_window <= 0.0",
                "invalid input_datatype",
                "fopen() returned NULL",
            } ;

            /* make sure required info read correctly from command file */
            if (InfoPtr->file_header_bytes < 0)   error = 1;
            if (InfoPtr->trace_header_bytes < 0)  error = 2;
            if (InfoPtr->in_samps_per_trace < 1)  error = 3;
            if (InfoPtr->in_time_window <= 0.0)   error = 4;
            switch (InfoPtr->input_datatype)
            {   case 1:  case -1:
                    InfoPtr->in_bits_per_samp = 8;
                    break;
                case 2:  case -2:  case -5:
                    InfoPtr->in_bits_per_samp = 16;
                    break;
                case 3:  case -3:  case 4: case -6:
                    InfoPtr->in_bits_per_samp = 32;
                    break;
                case 8:
                    InfoPtr->in_bits_per_samp = 64;
                    break;
                default:
                    error = 5;
                    InfoPtr->in_bits_per_samp = 0;  break;
            }

            /* calculate the rest of required info */
            if (!error)
            {   FILE *infile;
                char *cp1,*cp2;

                InfoPtr->num_input_channels = 1;
                if (InfoPtr->selected_channel >= InfoPtr->num_input_channels)
                    InfoPtr->selected_channel = InfoPtr->num_input_channels - 1;
                /* assign DT1/HD stuff */
                InfoPtr->job_number[0]   = 0;
                strcpy(InfoPtr->job_number,"8000");
                InfoPtr->title1[0]       = 0;
                InfoPtr->title2[0]       = 0;
                if (InfoPtr->text != NULL && InfoPtr->text[0] != 0)
                {   if ((cp1 = strchr(InfoPtr->text,'\n')) != NULL)
                    {   *cp1 = 0;
                        strncpy(InfoPtr->title1,InfoPtr->text,69);
                        InfoPtr->title1[69] = 0;
                        cp2 = cp1 + 1;
                        if ((cp1 = strchr(cp2,'\n')) != NULL)
                          {   *cp1 = 0;
                               strncpy(InfoPtr->title2,cp2,69);
                            InfoPtr->title2[69] = 0;
                        } else
                        {   strncpy(InfoPtr->title2,cp2,69);
                               InfoPtr->title2[69] = 0;
                           }
                    } else
                    {   strncpy(InfoPtr->title1,InfoPtr->text,69);
                        InfoPtr->title1[69] = 0;
                    }
                }
                InfoPtr->position        = 0.0;
                InfoPtr->num_samples     = InfoPtr->in_samps_per_trace;
                InfoPtr->elevation       = 0.0;
                InfoPtr->num_bytes       = 2.0;
                InfoPtr->num_stacks      = InfoPtr->number_of_stacks;
                InfoPtr->time_window     = InfoPtr->in_time_window;
                InfoPtr->timezero_adjust = 0.0;
                InfoPtr->zero_flag       = 0.0;
                InfoPtr->time_of_day     = 0.0;
                InfoPtr->comment_flag    = 0.0;
                for (i=0; i<sizeof(InfoPtr->comment); i++)
                    InfoPtr->comment[i] = 0;
                if ((infile = fopen(InfoPtr->data_filename,"rb")) == NULL)
                {   error = 6;
                    printf("GetInfo(): OTHER error occurred attempting to open file %s\n",InfoPtr->data_filename);
                } else
                {   /* determine number of GPR traces in file */
                    long start_byte,last_byte,num_bytes;

                    fseek(infile,(long)InfoPtr->file_header_bytes,SEEK_SET);
                    start_byte = ftell(infile);        /* get current location (byte) */
                    fseek(infile, 0L, SEEK_END);       /* go to end of file */
                    last_byte = ftell(infile);         /* get last location (byte) */
                    num_bytes = last_byte - start_byte;
                    InfoPtr->total_traces = num_bytes /
                        ((InfoPtr->in_samps_per_trace * InfoPtr->in_bits_per_samp/8) +
                         InfoPtr->trace_header_bytes);
                    fclose(infile);
                }
            }
            if (error > 0)
            {   printf("\nError #%d occurred in OTHER datatype info section:\n\t%s",
                        error,error_msg[error]);
                DeallocInfoStruct(InfoPtr);
                return 5;
            }

            /* rest of values must be assigned in command file */
            break;
        }

        default:
            DeallocInfoStruct(InfoPtr);
            return 3;
            /* break; (unreachable code) */
    }

/***** Finish initializing variables and verifying valid ranges *****/

    if (Debug) puts("Start of initialize and check section");

    /* Check for valid ranges in required parameters */
    if (InfoPtr->total_traces < 1)
    {   DeallocInfoStruct(InfoPtr);
        return 10;
    }
    if (InfoPtr->in_samps_per_trace < 2)
    {   DeallocInfoStruct(InfoPtr);
        return 11;
    }
    switch (InfoPtr->in_bits_per_samp)
    {   case 8:  case 16:  case 32:  case 64:
            break;
        default:
            DeallocInfoStruct(InfoPtr);
            return 12;
    }
    if (InfoPtr->in_time_window <= 0.0)
    {   DeallocInfoStruct(InfoPtr);
        return 13;
    }
    switch (InfoPtr->input_datatype)
    {   case  1:  case -1:  case  2:  case -2:  case -5:
        case  3:  case -3:  case -6:  case  4:  case  8:
            break;
        default:
            DeallocInfoStruct(InfoPtr);
            return 14;
            /* break; unreachable code */
    }

    /* Update modification time/date */
    time( &long_time );                 /* Get time as long integer */
    newtime = localtime( &long_time );  /* Convert to local time */
    InfoPtr->time_date_modified.month = newtime->tm_mon + 1;     /* 0 - 11 */
    InfoPtr->time_date_modified.day   = newtime->tm_mday;        /* 1 - 31 */
    InfoPtr->time_date_modified.year  = newtime->tm_year - 80;   /* year - 1900 - (19)80 */
    InfoPtr->time_date_modified.hour  = newtime->tm_hour;        /* 0 - 24 */
    InfoPtr->time_date_modified.min   = newtime->tm_min;
    InfoPtr->time_date_modified.sec2  = newtime->tm_sec/2;

    InfoPtr->created = TRUE;
    if (Debug) puts("End of initialize and check section");
    return 0;
}
/**************************** DeallocInfoStruct() ***************************/
/* Deallocate the buffer space in the information structure.  This function
 * will work correctly only if the num_... variables accurately reflect
 * the successful allocation of storage.
 *
 * Parameter list:
 *  struct GprInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_conv.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS
 * Date: April 3, 1995
 */
void DeallocInfoStruct (struct GprInfoStruct *InfoPtr)
{
    if (InfoPtr->num_marks)
    {   free(InfoPtr->marks);      InfoPtr->marks = NULL;
        free(InfoPtr->XYZ);        InfoPtr->XYZ   = NULL;
        InfoPtr->num_marks = 0;
    }
    if (InfoPtr->num_gain_pts)
    {   free(InfoPtr->gain_pts);   InfoPtr->gain_pts = NULL;
        InfoPtr->num_gain_pts = 0;
    }
    if (InfoPtr->proc_hist_bytes)
    {   free(InfoPtr->proc_hist);  InfoPtr->proc_hist = NULL;
        InfoPtr->proc_hist_bytes = 0;
    }
    if (InfoPtr->text_bytes)
    {   free(InfoPtr->text);       InfoPtr->text = NULL;
        InfoPtr->text_bytes = 0;
    }
    InfoPtr->created = FALSE;
}
/***************************** ConvertGprData() *****************************/
/* Opens input and output files, sets and stores output info header/file,
 * copies/converts GPR data from input to output file(s), closes files.
 * The data are converted block by block where a block's size is determined
 * by the available RAM.  Info from MRK files is added to output DZT files
 * which use SIR-10A+ ver. 5.2 format.
 *
 * Parameter list:
 *  struct GprInfoStruct *InfoPtr - address of information structure
 *
 * Requires: <errno.h>, <io.h>, <stdio.h>, <stdlib.h>, "assertjl.h",
 *           "gpr_conv.h", "gpr_io.h".
 * Calls: malloc, free, fopen, fclose, access, setvbuf, printf, assert,
 *        SetDztHeader, SaveSsHdFile, SetSgyFileHeader, SetDt1TraceHeader
 *        SetSgyTraceHeader.
 * Returns: 0 on success,
 *         >0 on error
 *
const char *ConvertGprDataMsg[] =
{   "ConvertGprData() No Errors.",
    "ConvertGprData() Error: Unable to allocate temporary storage.",
    "ConvertGprData() Error: Opening input file, file or path name not found.",
    "ConvertGprData() Error: Opening input file, read and write access denied.",
    "ConvertGprData() Error: Opening input file, unknown access error.",
*5* "ConvertGprData() Error: Opening output file, file or path name not found.",
    "ConvertGprData() Error: Opening output file, read and write access denied.",
    "ConvertGprData() Error: Opening output file, unknown access error.",
    "ConvertGprData() Error: Unrecognized input storage format.",
    "ConvertGprData() Error: Short count from fread().",
*10*"ConvertGprData() Error: Short count from fwrite().",
    "ConvertGprData() Error: Problem saving HD file.",
    "ConvertGprData() Error: Problem saving SGY file header file..",
    "ConvertGprData() Error: Floating point input data not supported.",
}
 *
 * Author: Jeff Lucius   USGS
 * Date: January 28, 1997; Aug 11, 1997;
 * Revisions: April 4, 2000; August 15, 2001;
 */
int ConvertGprData (struct GprInfoStruct *InfoPtr)
{
	int  i,j,error,num,itemp;     /* counters or scratch variables */
    int  num_traces;
    long dzt_trace_num,dt1_trace_num,sgy_trace_num,mrk_start_num;
    long block_bytes;             /* size of a data block in bytes */
    long file_bytes;              /* number of bytes used to store data */
    long total_file_bytes;        /* size of file */
    long disk_trace_bytes;        /* number of bytes in one trace */
    long disk_chunk_bytes;        /* number of bytes in one trace group */
    long tenth;                   /* used to inform user of progress */
    long num_blocks,count;        /* number of data blocks */
    long offset;                  /* bytes */
    void *data_block  = NULL;     /* pointer to block of traces */

    FILE *infile      = NULL;     /* pointer to IO struct for input file */
    FILE *dzt_outfile = NULL;     /* pointer to IO struct for output file */
    FILE *dt1_outfile = NULL;     /* pointer to IO struct for output file */
    FILE *sgy_outfile = NULL;     /* pointer to IO struct for output file */
    FILE *sg2_outfile = NULL;     /* pointer to IO struct for output file */
    FILE *rd3_outfile = NULL;     /* pointer to IO struct for output file */

    struct SsTraceHdrStruct Dt1TraceHdr;
    struct SegyTraceHdrStruct SgyTraceHdr;

    unsigned char  *uc_trace = NULL;
    unsigned short *us_trace = NULL;
    short int      *s_trace  = NULL;

    unsigned char *uc_p, *uc_p_start;
    char *c_p;
    unsigned short *us_p, *us_p_start;
    short *s_p;
    unsigned long *ul_p, *ul_p_start;
    long *l_p;

    extern int Debug;

    #if defined(_INTELC32_)
      char *inbuffer      = NULL;
      char *dzt_outbuffer = NULL;
      char *dt1_outbuffer = NULL;
      char *sgy_outbuffer = NULL;
      char *sg2_outbuffer = NULL;
      char *rd3_outbuffer = NULL;
      size_t vbufsize = 65536L;
    #else
	  size_t vbufsize = 31744;  /* 31 * 1024 */
    #endif

    if (Debug) puts("start of ConvertGprData()");

/***** Setup RAM buffer to speed conversion *****/

    /* Calculate sizes for input file */
    disk_trace_bytes  = InfoPtr->trace_header_bytes +
        (InfoPtr->in_samps_per_trace * (InfoPtr->in_bits_per_samp/8));
    disk_chunk_bytes  = InfoPtr->num_input_channels * disk_trace_bytes;
    file_bytes        = disk_chunk_bytes * InfoPtr->total_traces;
    total_file_bytes  = InfoPtr->file_header_bytes + file_bytes;

    /* Make block_bytes a multiple of disk_chunk_bytes */
    block_bytes       = (vbufsize/disk_chunk_bytes)*disk_chunk_bytes;
    if (block_bytes > file_bytes)
        block_bytes   = file_bytes;
    if (Debug) printf("disk_trace_bytes = %ld\ndisk_chunk_bytes = %ld\n\
                       file_bytes = %ld\ntotal_file_bytes = %ld\n\
                       block_bytes = %ld\n",disk_trace_bytes,disk_chunk_bytes,
                       file_bytes,total_file_bytes,block_bytes);

    /* Allocate maximum size buffer allowed by RAM */
    while ( (block_bytes >= disk_chunk_bytes) &&
            ((data_block = malloc((size_t)block_bytes)) == NULL) )
        block_bytes -= disk_chunk_bytes;
    if (data_block == NULL || block_bytes < disk_chunk_bytes)  return 1;
    if (Debug) printf("Actual block bytes used = %ld  data_block = %p\n",
                    block_bytes,data_block);
    if (Debug) getch();

    /* Determine number of blocks in the file */
    if (file_bytes % block_bytes)  num_blocks = file_bytes/block_bytes + 1;
    else                           num_blocks = file_bytes/block_bytes;
    tenth = num_blocks / 10;
    if (tenth < 1)  tenth = 1;
    if (Debug) printf("num_blocks = %ld\n",num_blocks);

/***** Open input file *****/

    /* Attempt to open binary file for read-only */
    if ((infile = fopen(InfoPtr->data_filename,"rb")) == NULL)
    {   if (access(InfoPtr->data_filename,6))
        {   if      (errno == ENOENT) error = 2;
			else if (errno == EACCES) error = 3;
            else                      error = 4;
        }
        free(data_block);
        return error;
    }

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(inbuffer,vbufsize);
      if (inbuffer)  setvbuf(infile,inbuffer,_IOFBF,vbufsize);
      else           setvbuf(infile,NULL,_IOFBF,vbufsize);
    #else
      setvbuf(infile,NULL,_IOFBF,vbufsize);
    #endif

/***** Open output file(s) *****/

    if (InfoPtr->dzt_outfilename[0])
    {   /* Attempt to open binary file for write-only */
        if ((dzt_outfile = fopen(InfoPtr->dzt_outfilename,"wb")) == NULL)
        {   if (access(InfoPtr->dzt_outfilename,6))
            {   if      (errno == ENOENT) error = 5;
                else if (errno == EACCES) error = 6;
                else                      error = 7;
            }
            free(data_block);
            fclose(infile);
            #if defined(_INTELC32_)
              realfree(inbuffer);
            #endif
            return error;
        }
        /* Speed up disk access by using large (conventional memory) buffer */
        #if defined(_INTELC32_)
          realmalloc(dzt_outbuffer,vbufsize);
          if (dzt_outbuffer)  setvbuf(dzt_outfile,dzt_outbuffer,_IOFBF,vbufsize);
          else                setvbuf(dzt_outfile,NULL,_IOFBF,vbufsize);
        #else
          setvbuf(dzt_outfile,NULL,_IOFBF,vbufsize);
        #endif
        if (Debug) puts("dzt output file opened");
    }

    if (InfoPtr->dt1_outfilename[0])
	{   /* Attempt to open binary file for write-only */
        if ((dt1_outfile = fopen(InfoPtr->dt1_outfilename,"wb")) == NULL)
        {   if (access(InfoPtr->dt1_outfilename,6))
            {   if      (errno == ENOENT) error = 5;
                else if (errno == EACCES) error = 6;
                else                      error = 7;
            }
            free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);  realfree(dzt_outbuffer);
            #endif
            return error;
        }
        /* Speed up disk access by using large (conventional memory) buffer */
        #if defined(_INTELC32_)
          realmalloc(dt1_outbuffer,vbufsize);
          if (dt1_outbuffer)  setvbuf(dt1_outfile,dt1_outbuffer,_IOFBF,vbufsize);
          else                setvbuf(dt1_outfile,NULL,_IOFBF,vbufsize);
        #else
          setvbuf(dt1_outfile,NULL,_IOFBF,vbufsize);
        #endif
        if (Debug) puts("dt1 output file opened");
    }

    if (InfoPtr->sgy_outfilename[0])
    {   /* Attempt to open binary file for write-only */
        if ((sgy_outfile = fopen(InfoPtr->sgy_outfilename,"wb")) == NULL)
        {   if (access(InfoPtr->sgy_outfilename,6))
            {   if      (errno == ENOENT) error = 5;
                else if (errno == EACCES) error = 6;
                else                      error = 7;
            }
            free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer); realfree(dzt_outbuffer); realfree(dt1_outbuffer);
            #endif
            return error;
        }
        /* Speed up disk access by using large (conventional memory) buffer */
        #if defined(_INTELC32_)
		  realmalloc(sgy_outbuffer,vbufsize);
          if (sgy_outbuffer)  setvbuf(sgy_outfile,sgy_outbuffer,_IOFBF,vbufsize);
          else                setvbuf(sgy_outfile,NULL,_IOFBF,vbufsize);
        #else
          setvbuf(sgy_outfile,NULL,_IOFBF,vbufsize);
        #endif
        if (Debug) puts("sgy output file opened");
    }

    if (InfoPtr->sg2_outfilename[0])
    {   /* Attempt to open binary file for write-only */
        if ((sg2_outfile = fopen(InfoPtr->sg2_outfilename,"wb")) == NULL)
        {   if (access(InfoPtr->sg2_outfilename,6))
            {   if      (errno == ENOENT) error = 5;
                else if (errno == EACCES) error = 6;
                else                      error = 7;
            }
            free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer); realfree(dzt_outbuffer);
              realfree(dt1_outbuffer); realfree(sgy_outbuffer);
            #endif
            return error;
        }
        /* Speed up disk access by using large (conventional memory) buffer */
        #if defined(_INTELC32_)
          realmalloc(sg2_outbuffer,vbufsize);
          if (sg2_outbuffer)  setvbuf(sg2_outfile,sg2_outbuffer,_IOFBF,vbufsize);
          else                setvbuf(sg2_outfile,NULL,_IOFBF,vbufsize);
        #else
          setvbuf(sg2_outfile,NULL,_IOFBF,vbufsize);
        #endif
        if (Debug) puts("sg2 output file opened");
    }

    if (InfoPtr->rd3_outfilename[0])
    {   /* Attempt to open binary file for write-only */
        if ((rd3_outfile = fopen(InfoPtr->rd3_outfilename,"wb")) == NULL)
        {   if (access(InfoPtr->rd3_outfilename,6))
            {   if      (errno == ENOENT) error = 5;
                else if (errno == EACCES) error = 6;
				else                      error = 7;
            }
            free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer); realfree(dzt_outbuffer);
              realfree(dt1_outbuffer); realfree(sgy_outbuffer);
              realfree(sg2_outbuffer); realfree(rd3_outbuffer);
            #endif
            return error;
        }
        /* Speed up disk access by using large (conventional memory) buffer */
        #if defined(_INTELC32_)
          realmalloc(rd3_outbuffer,vbufsize);
          if (rd3_outbuffer)  setvbuf(rd3_outfile,sg2_outbuffer,_IOFBF,vbufsize);
          else                setvbuf(rd3_outfile,NULL,_IOFBF,vbufsize);
        #else
          setvbuf(rd3_outfile,NULL,_IOFBF,vbufsize);
        #endif
        if (Debug) puts("rd3 output file opened");
    }

/***** Save info header/file and allocate storage for a single trace *****/

    /* file pointer should be correctly placed to start writing data at the
       end of this section */

    if (InfoPtr->dzt_outfilename[0]) /****************************/
    {   int hdr_num              = 1;
        unsigned short rh_nsamp;
        unsigned short rh_bits;
        short rh_zero;
        float rh_sps             = InfoPtr->traces_per_sec;
        float rh_spm             = InfoPtr->traces_per_meter;
        float rh_mpm             = InfoPtr->meters_per_mark;
        float rh_position;
        float rh_range;
        unsigned short rh_npass  = 1;
        unsigned short rh_nchan  = InfoPtr->num_output_channels; /* 1 */
        float rh_epsr            = InfoPtr->nominal_RDP;
        float rh_top             = 0.0;
		float rh_depth           = 0.0;
        char *rh_antname         = InfoPtr->antenna_name;
        char *rh_name            = InfoPtr->dzt_outfilename;
        char dummy_text[1];
        unsigned short rg_breaks = InfoPtr->num_gain_pts;
        float dummy_rg_values[1];
        float *rg_values         = NULL;
        unsigned short rh_nproc  = InfoPtr->proc_hist_bytes;
        unsigned char *proc_hist;
        unsigned char blank_proc_hist[1];
        struct DztDateStruct create_date;
        struct DztHdrStruct hdr;

        if (Debug) puts("start of section to save DZT header");
        if (rg_breaks > 0)
        {   rg_values = (float *)malloc(rg_breaks * sizeof(float));
            if (rg_values == NULL)
            {   free(data_block);
                fclose(infile);
                if (dzt_outfile) fclose(dzt_outfile);
                if (dt1_outfile) fclose(dt1_outfile);
                if (sgy_outfile) fclose(sgy_outfile);
                if (sg2_outfile) fclose(sg2_outfile);
                if (rd3_outfile) fclose(rd3_outfile);
                #if defined(_INTELC32_)
                  realfree(inbuffer); realfree(dzt_outbuffer); realfree(dt1_outbuffer);
                  realfree(sgy_outbuffer); realfree(sg2_outbuffer); realfree(rd3_outbuffer);
                #endif
                return 1;
            }
            for (i=0; i<rg_breaks; i++)  rg_values[i] = InfoPtr->gain_pts[i];
        } else
        {   dummy_rg_values[0] = 0.0;
            rg_values = dummy_rg_values;
            rg_breaks = 0;
        }
        if (InfoPtr->input_format == DZT)
        {   proc_hist = (unsigned char *)InfoPtr->proc_hist;
        } else
        {   if ( (InfoPtr->text == NULL || InfoPtr->text[0] == 0 ) &&
                 (InfoPtr->proc_hist != NULL && InfoPtr->proc_hist_bytes > 0) )
            {   InfoPtr->text_bytes = InfoPtr->proc_hist_bytes;
                InfoPtr->text = (char *)malloc(InfoPtr->text_bytes);
                if (InfoPtr->text == NULL)
                {   puts("ConvertGprData(): can't allocate InfoPtr->text.");
                    DeallocInfoStruct(InfoPtr);
                    return 1;
                }
                strcpy(InfoPtr->text,InfoPtr->proc_hist);
                InfoPtr->text[InfoPtr->text_bytes-1] = 0;
                if (Debug)
                {   printf("----- InfoPtr->text =%s||\n",InfoPtr->text);
                    getch();
                }
            } else if(InfoPtr->proc_hist != NULL && InfoPtr->proc_hist_bytes > 0)
            { /* just cat onto text */
                InfoPtr->text_bytes += InfoPtr->proc_hist_bytes;
                InfoPtr->text = realloc(InfoPtr->text,InfoPtr->text_bytes);
                if (InfoPtr->text == NULL)
                {   puts("ConvertGprData(): can't allocate InfoPtr->text.");
                    DeallocInfoStruct(InfoPtr);
                    return 1;
                }
                strcat(InfoPtr->text,"\n");
                strcat(InfoPtr->text,InfoPtr->proc_hist);
            } else
            {   blank_proc_hist[0] = 0;
                proc_hist = blank_proc_hist;
                rh_nproc = 0;
            }
        }
        if (InfoPtr->text == NULL)
		{   InfoPtr->text_bytes = 0;
        }
        if (InfoPtr->output_datatype == 0)       InfoPtr->output_datatype = -2;
        if (_ABS(InfoPtr->output_datatype) == 1) InfoPtr->output_datatype = -1;
        if (_ABS(InfoPtr->output_datatype) > 1)  InfoPtr->output_datatype = -2;

        if (InfoPtr->output_datatype == -1)  /* 8-bit data */
        {   long num_fillers = 0L;
            long mid_val = 128;

			/* check that in_samps_per_trace is a power of 2 */
			assert((long)InfoPtr->in_samps_per_trace < 65536L);
#if 1
			for (i=1; i<16; i++)
			{   if (InfoPtr->in_samps_per_trace > pow(2,(double)i) &&
					InfoPtr->in_samps_per_trace < pow(2,(double)(i+1)))
				{   num_fillers = pow(2,(double)(i+1)) - InfoPtr->in_samps_per_trace;
					break;
				}
			}
#endif
			/* calculate dependent values */
			InfoPtr->out_samps_per_trace = InfoPtr->in_samps_per_trace + num_fillers;
			rh_nsamp = InfoPtr->out_samps_per_trace;
			rh_bits  = 8;   /* sizeof(unsigned char) * 8 */
			InfoPtr->out_time_window = (double)(rh_nsamp-1) *
				(InfoPtr->in_time_window / (double)(InfoPtr->in_samps_per_trace-1));
			rh_range = InfoPtr->out_time_window;
			rh_position = rh_range * (InfoPtr->timezero_sample / rh_nsamp);

			/* allocate storage for output trace buffer */
			uc_trace = (unsigned char *)malloc((size_t)rh_nsamp * sizeof(unsigned char));
			if (uc_trace == NULL)
			{   free(data_block);
				fclose(infile);
				if (dzt_outfile) fclose(dzt_outfile);
				if (dt1_outfile) fclose(dt1_outfile);
				if (sgy_outfile) fclose(sgy_outfile);
				if (sg2_outfile) fclose(sg2_outfile);
				if (rd3_outfile) fclose(rd3_outfile);
				#if defined(_INTELC32_)
				  realfree(inbuffer);
				  realfree(dzt_outbuffer); realfree(dt1_outbuffer);
				  realfree(sgy_outbuffer); realfree(sg2_outbuffer);
				  realfree(rd3_outbuffer);
				#endif
				return 1;
			}
			for (i=InfoPtr->in_samps_per_trace; i<rh_nsamp; i++)
				uc_trace[i] = mid_val;
			if (Debug)
			{   printf("out_samps = %d  num_fillers = %ld mid_val = %ld\n",
					rh_nsamp,num_fillers,mid_val);
			}
		} else                         /* 16-bit data */
		{   long num_fillers = 0L;
			long mid_val = 32768L;

			/* check that in_samps_per_trace is a power of 2 */
			assert((long)InfoPtr->in_samps_per_trace < 65536L);
#if 1
			for (i=1; i<16; i++)
			{   if (InfoPtr->in_samps_per_trace > pow(2,(double)i) &&
					InfoPtr->in_samps_per_trace < pow(2,(double)(i+1)))
				{   num_fillers = pow(2,(double)(i+1)) - InfoPtr->in_samps_per_trace;
					break;
				}
			}
#endif
			/* calculate dependent values */
			InfoPtr->out_samps_per_trace = InfoPtr->in_samps_per_trace + num_fillers;
			rh_nsamp = InfoPtr->out_samps_per_trace;
			rh_bits  = 16;  /* sizeof(unsigned short) * 8 */
			InfoPtr->out_time_window = (double)(rh_nsamp-1) *
                (InfoPtr->in_time_window / (double)(InfoPtr->in_samps_per_trace-1));
            rh_range = InfoPtr->out_time_window;
            rh_position = rh_range * ((double)-InfoPtr->timezero_sample / (rh_nsamp-1));

            /* allocate storage for output trace buffer */
            us_trace = (unsigned short *)malloc((size_t)rh_nsamp * sizeof(unsigned short));
            if (us_trace == NULL)
            {   free(data_block);
                fclose(infile);
                if (dzt_outfile) fclose(dzt_outfile);
                if (dt1_outfile) fclose(dt1_outfile);
                if (sgy_outfile) fclose(sgy_outfile);
                if (sg2_outfile) fclose(sg2_outfile);
                if (rd3_outfile) fclose(rd3_outfile);
				#if defined(_INTELC32_)
                  realfree(inbuffer);
                  realfree(dzt_outbuffer); realfree(dt1_outbuffer);
                  realfree(sgy_outbuffer); realfree(sg2_outbuffer);
                  realfree(rd3_outbuffer);
                #endif
                return 1;
            }
            for (i=InfoPtr->in_samps_per_trace; i<rh_nsamp; i++)
                us_trace[i] = mid_val;
            if (Debug)
            {   printf("out_samps = %d  num_fillers = %ld mid_val = %ld\n",
                    rh_nsamp,num_fillers,mid_val);
            }
        }
		rh_zero = 0x0001 << (rh_bits - 1);
        create_date.sec2  = InfoPtr->time_date_created.sec2;
        create_date.min   = InfoPtr->time_date_created.min;
        create_date.hour  = InfoPtr->time_date_created.hour;
        create_date.day   = InfoPtr->time_date_created.day;
        create_date.month = InfoPtr->time_date_created.month;
        create_date.year  = InfoPtr->time_date_created.year;

        if (Debug) puts("before setting DZT header");
        SetDztHeader(hdr_num,rh_nsamp,rh_bits,rh_zero,rh_sps,rh_spm,rh_mpm,
                     rh_position,rh_range,rh_npass,rh_nchan,rh_epsr,rh_top,
                     rh_depth,rh_antname,rh_name,InfoPtr->text,rg_breaks,rg_values,
                     rh_nproc,proc_hist,&create_date,&hdr);
        if (Debug) puts("after setting header, before saving to disk");
        if (fwrite((void*)&hdr,sizeof(struct DztHdrStruct),1,dzt_outfile) < 1)
        {   free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            if (rd3_outfile) fclose(rd3_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);
              realfree(dzt_outbuffer); realfree(dt1_outbuffer);
              realfree(sgy_outbuffer); realfree(sg2_outbuffer);
              realfree(rd3_outbuffer);
            #endif
            return 10;
        }
		if (Debug) puts("dzt output file header saved");
    }

    if (InfoPtr->dt1_outfilename[0]) /****************************/
    {   int day               = InfoPtr->time_date_created.day;
        int month             = InfoPtr->time_date_created.month;
        int year              = InfoPtr->time_date_created.year + 1980 - 1900;
        long num_traces       = InfoPtr->total_traces;
        long num_samples      = InfoPtr->in_samps_per_trace;
        long time_zero_sample = InfoPtr->timezero_sample;
        int total_time_ns     = InfoPtr->in_time_window;
        double start_pos      = InfoPtr->starting_position;
        double final_pos      = InfoPtr->final_position;
        double step_size      = InfoPtr->position_step_size;
        char *pos_units       = InfoPtr->position_units;
		double ant_freq       = InfoPtr->nominal_frequency;
        double ant_sep        = InfoPtr->antenna_separation;
        double pulser_voltage = InfoPtr->pulser_voltage;
        int num_stacks        = InfoPtr->number_of_stacks;
        char *survey_mode     = InfoPtr->survey_mode;
        char *proc_hist;
        char *filename        = InfoPtr->hd_outfilename;
        char big_buff[1024];
        extern const char *SaveSsHdFileMsg[]; /* in gpr_io.h */

        if (Debug) puts("start of section to save DT1 header");
        if (InfoPtr->input_format == DZT)
        {   ConvertProcHist2((int)sizeof(big_buff),big_buff,
                             (int)InfoPtr->proc_hist_bytes,
                             InfoPtr->proc_hist);
            proc_hist = big_buff;
            if (big_buff[0] == 0)
                proc_hist = InfoPtr->text;
        } else
            proc_hist = InfoPtr->proc_hist;  /* proc_hist = InfoPtr->text; */
        itemp = SaveSsHdFile(day,month,year,num_traces,num_samples,
                  time_zero_sample,total_time_ns,start_pos,final_pos,
                  step_size,pos_units,ant_freq,ant_sep,pulser_voltage,
                  num_stacks,survey_mode,proc_hist,filename,
                  InfoPtr->data_filename,InfoPtr->job_number,
                  InfoPtr->title1,InfoPtr->title2,InfoPtr->traces_per_sec,
                  InfoPtr->meters_per_mark,InfoPtr->num_gain_pts,
                  InfoPtr->gain_pts);
        if (itemp > 0)
        {   printf("\n%s\n",SaveSsHdFileMsg[itemp]);
            free(data_block);
            fclose(infile);
			if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            if (rd3_outfile) fclose(rd3_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);
              realfree(dzt_outbuffer); realfree(dt1_outbuffer);
              realfree(sgy_outbuffer); realfree(sg2_outbuffer);
              realfree(rd3_outbuffer);
            #endif
            return 11;
        }
        if (Debug) puts("dt1 output header saved");
        s_trace = (short *)malloc(InfoPtr->in_samps_per_trace * sizeof(short));
		if (s_trace == NULL)
        {    free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            if (rd3_outfile) fclose(rd3_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);
              realfree(dzt_outbuffer); realfree(dt1_outbuffer);
              realfree(sgy_outbuffer); realfree(sg2_outbuffer);
              realfree(rd3_outbuffer);
            #endif
            return 1;
        }
        rewind(dt1_outfile);
    }

    if (InfoPtr->sgy_outfilename[0]) /****************************/
    {   int day               = InfoPtr->time_date_created.day;
        int month             = InfoPtr->time_date_created.month;
        int year              = InfoPtr->time_date_created.year + 1980;
        long num_traces       = InfoPtr->total_traces;
        long num_samples      = InfoPtr->in_samps_per_trace;
        long time_zero_sample = InfoPtr->timezero_sample;
        int total_time_ns     = InfoPtr->in_time_window;
        double start_pos      = InfoPtr->starting_position;
        double final_pos      = InfoPtr->final_position;
        double step_size      = InfoPtr->position_step_size;
		char *pos_units       = InfoPtr->position_units;
        double ant_freq       = InfoPtr->nominal_frequency;
        double ant_sep        = InfoPtr->antenna_separation;
        double pulser_voltage = InfoPtr->pulser_voltage;
        int num_stacks        = InfoPtr->number_of_stacks;
        char *survey_mode     = InfoPtr->survey_mode;
        char *proc_hist       = NULL;
        char blank_proc_hist[1];
        char dummy_text[1];
        char *text            = InfoPtr->text;
        char big_buff[1024];
        int num_gain_pts      = InfoPtr->num_gain_pts;
        double dummy_gain_pts[1];
        double *gain_pts      = InfoPtr->gain_pts;
        struct SegyReelHdrStruct hdr;

		if (Debug) puts("start of section to save SGY header");
        if (InfoPtr->input_format == DZT)
        {   big_buff[0] = 0;
            ConvertProcHist2((int)sizeof(big_buff),big_buff,
                             (int)InfoPtr->proc_hist_bytes,
                             InfoPtr->proc_hist);
            proc_hist = big_buff;
            if (big_buff[0] == 0)
                proc_hist = InfoPtr->text;
        } else
        {   proc_hist = InfoPtr->proc_hist;
            if (proc_hist == NULL)
            {   blank_proc_hist[0] = 0;
                proc_hist = blank_proc_hist;
            }
        }
        if (text == NULL)
        {   dummy_text[0] = 0;
            text = dummy_text;
        }
        if (gain_pts == NULL)
        {   dummy_gain_pts[0] = 0.0;
            gain_pts = dummy_gain_pts;
            num_gain_pts = 0;
        }
        SetSgyFileHeader(day,month,year,num_traces,num_samples,
                  time_zero_sample,total_time_ns,start_pos,final_pos,
                  step_size,pos_units,ant_freq,ant_sep,pulser_voltage,
                  num_stacks,survey_mode,proc_hist,text,
                  InfoPtr->job_number,InfoPtr->title1,InfoPtr->title2,
                  InfoPtr->data_filename,InfoPtr->traces_per_sec,
				  InfoPtr->meters_per_mark,num_gain_pts,
                  gain_pts,&hdr);
        if (fwrite((void*)&hdr,sizeof(struct SegyReelHdrStruct),1,sgy_outfile) < 1)
        {   free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            if (rd3_outfile) fclose(rd3_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);
              realfree(dzt_outbuffer); realfree(dt1_outbuffer);
              realfree(sgy_outbuffer); realfree(sg2_outbuffer);
              realfree(rd3_outbuffer);
			#endif
            return 12;
        }
        if (Debug) puts("sgy output header saved");
        s_trace = (short *)malloc(InfoPtr->in_samps_per_trace * sizeof(short));
        if (s_trace == NULL)
        {    free(data_block);
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            if (rd3_outfile) fclose(rd3_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);
              realfree(dzt_outbuffer); realfree(dt1_outbuffer);
              realfree(sgy_outbuffer); realfree(sg2_outbuffer);
              realfree(rd3_outbuffer);
            #endif
            return 1;
        }
    }

/***** Copy/convert Data *****/

    mrk_start_num = 0;      /* only used for DZT output files */
    sgy_trace_num = dzt_trace_num = 0;
    dt1_trace_num = 1;
    switch (InfoPtr->input_format)
    {

		case DZT:  case MOD_DZT:
		{   /* move to start of first trace block */
			rewind(infile);
			offset = InfoPtr->num_input_channels * InfoPtr->file_header_bytes;
			fseek(infile,offset,SEEK_SET);

			/* get data block by block */
			for (num=0, count=0; num<num_blocks; num++,count++)
			{   if ((num%tenth) == 0) printf(".");      /* indicate progress */
				/* check if less than "block_bytes" bytes left in file */
				offset = ftell(infile);
				if (block_bytes > (total_file_bytes - offset))
					block_bytes = total_file_bytes - offset;
				if (fread(data_block,(size_t)block_bytes,(size_t)1,infile) < 1)
				{   fclose(infile);
					if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return 9;
                }
                /* write data to selected files */
                error = 0;
                num_traces = block_bytes / disk_chunk_bytes;
if (Debug) { printf("\nbefore write: block_bytes = %d   disk_chunk_bytes = %d\n",
             block_bytes,disk_chunk_bytes); }
if (Debug) { printf("num_traces = %d\n",num_traces); }
                if (InfoPtr->dzt_outfilename[0])   /*** DZT ***/
                {   void *v_p;

                    /* point to start of selected channel NOT USED HERE!!!! */
                    v_p = (void *)((char *)data_block +
                              (InfoPtr->selected_channel * disk_trace_bytes));
                    for (i=0; i<num_traces; i++,dzt_trace_num++)
                    {   /* Set first two samples to SIR-10A+ v5.2 standard */
                        switch (InfoPtr->input_datatype)
                        {   case -1:
                                if (InfoPtr->num_input_channels == 1)
                                {   uc_p = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                } else
                                {   uc_p_start = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels);
                                    uc_p = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + InfoPtr->in_samps_per_trace * InfoPtr->selected_channel;
                                }
                                switch (InfoPtr->output_datatype)
								{   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = uc_p[j];
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        else
                                        {   us_trace[1] = uc_p_start[1];
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = uc_p[j] << 8;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;

                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        else
                                        {   us_trace[1] = uc_p_start[1] << 8;
                                        }
                                        break;
                                }
                                break;
                            case -2:
                                if (InfoPtr->num_input_channels == 1)
                                {   us_p = (unsigned short *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                } else
                                {   us_p_start = (unsigned short *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels);
                                    us_p = (unsigned short *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + InfoPtr->in_samps_per_trace * InfoPtr->selected_channel;
                                }
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = us_p[j] >> 8;
                                        uc_trace[0] = 0xFF;
										uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        else
                                        {   us_trace[1] = us_p_start[1] >> 8;
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = us_p[j];
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;

                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        else
                                        {   us_trace[1] = us_p_start[1];
                                        }
                                        break;
                                }
                                break;
                            case -3:
                                if (InfoPtr->num_input_channels == 1)
                                {   ul_p = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                } else
                                {   ul_p_start = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels);
                                    ul_p = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + InfoPtr->in_samps_per_trace * InfoPtr->selected_channel;
                                }
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = ul_p[j] >> 24;
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
												{   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        else
                                        {   uc_trace[1] = ul_p_start[1] >> 24;
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = ul_p[j] >> 16;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        else
                                        {   us_trace[1] = ul_p_start[1] >> 16;
                                        }
                                        break;
                                }
                                break;
                        }
                        /* write data to disk */
                        if (InfoPtr->output_datatype == -1)
                        {   if (fwrite((void *)uc_trace,
                                   InfoPtr->out_samps_per_trace * sizeof(unsigned char),
                                   1,dzt_outfile) < 1 )
                            {   error = 10;
                                   break; /* out of for loop working thru traces */
                            }
                        } else
                        {   if (fwrite((void *)us_trace,
                                   InfoPtr->out_samps_per_trace * sizeof(unsigned short),
                                   1,dzt_outfile) < 1 )
                            {   error = 10;
                                   break; /* out of for loop working thru traces */
                            }
                        }
                        /* point to start of next selected channel */
                        v_p = (char *)v_p + disk_chunk_bytes;
                    }
                }
				if (InfoPtr->dt1_outfilename[0])   /*** DT1 ***/
                {   for (i=0; i<num_traces; i++,dt1_trace_num++)
                    {   /* set values in the trace header */
                        SetDt1TraceHeader((float)dt1_trace_num,InfoPtr->position,
/* was num_samples instead of in_samps_per_trace */
                            InfoPtr->in_samps_per_trace,InfoPtr->elevation,
                            InfoPtr->num_bytes,(float)dt1_trace_num,
                            InfoPtr->num_stacks,(float)((int)InfoPtr->in_time_window),
                            InfoPtr->timezero_adjust,InfoPtr->zero_flag,
                            InfoPtr->time_of_day,InfoPtr->comment_flag,
                            InfoPtr->comment,&Dt1TraceHdr);
                        /* convert data to short int */
                        switch (InfoPtr->input_datatype)
                        {   case -1:
                                if (InfoPtr->num_input_channels == 1)
                                    uc_p = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                else
                                    uc_p = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + (InfoPtr->in_samps_per_trace * InfoPtr->selected_channel);
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((short)(uc_p[j]) - 128) * 256;
                                break;
                            case -2:
                                if (InfoPtr->num_input_channels == 1)
                                    us_p = (unsigned short *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                else
                                    us_p = (unsigned short *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + (InfoPtr->in_samps_per_trace * InfoPtr->selected_channel);
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)us_p[j] - 32768L;
                                break;
                            case -3:
                                if (InfoPtr->num_input_channels == 1)
                                    ul_p = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                else
                                    ul_p = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + (InfoPtr->in_samps_per_trace * InfoPtr->selected_channel);
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)(ul_p[j]/65536L) - 32768L;
                                break;
						}
                        /* save trace header and trace */
                        if (fwrite((void *)&Dt1TraceHdr,sizeof(struct SsTraceHdrStruct),
                                    1,dt1_outfile) < 1)
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        if (fwrite((void *)s_trace,InfoPtr->in_samps_per_trace * sizeof(short),
                                    1,dt1_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                    }
                }
                if (InfoPtr->sgy_outfilename[0])   /*** SGY ***/
                {   long timezero = (InfoPtr->timezero_sample/(InfoPtr->in_samps_per_trace-1))
                                    * InfoPtr->in_time_window;
                    long sample_int = ( InfoPtr->in_time_window /
                                       (InfoPtr->in_samps_per_trace-1) ) * 1000;
                    for (i=0; i<num_traces; i++,sgy_trace_num++)
                    {   /* set values in the trace header */
                        SetSgyTraceHeader((long)sgy_trace_num,
                            InfoPtr->antenna_separation,0.0,
                            0.0,timezero,InfoPtr->in_samps_per_trace,
                            sample_int,&SgyTraceHdr);

                        /* convert data to short int */
                        switch (InfoPtr->input_datatype)
                        {   case -1:
                                if (InfoPtr->num_input_channels == 1)
                                    uc_p = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                else
                                    uc_p = (unsigned char *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + InfoPtr->in_samps_per_trace * InfoPtr->selected_channel;
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((short)uc_p[j] - 128) * 256;
                                break;
                            case -2:
                                if (InfoPtr->num_input_channels == 1)
                                    us_p = (unsigned short *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                else
                                    us_p = (unsigned short *)data_block +
										   (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + InfoPtr->in_samps_per_trace * InfoPtr->selected_channel;
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)us_p[j] - 32768L;
                                break;
                            case -3:
                                if (InfoPtr->num_input_channels == 1)
                                    ul_p = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace);
                                else
                                    ul_p = (unsigned long *)data_block +
                                           (i * InfoPtr->in_samps_per_trace * InfoPtr->num_input_channels)
                                           + InfoPtr->in_samps_per_trace * InfoPtr->selected_channel;
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)(ul_p[j]/65536L) - 32768L;
                                break;
                        }
                        /* save trace header and trace */
                        if (fwrite((void *)&SgyTraceHdr,sizeof(struct SegyTraceHdrStruct),
                                    1,sgy_outfile) < 1)
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        if (fwrite((void *)s_trace,InfoPtr->in_samps_per_trace * sizeof(short),
                                    1,sgy_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                    }
                }
                if (error)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return error;
                }
			} /* end of for loop getting data blocks */
            break;
        }

        case DT1:
        {   /* move to start of first trace block */
            rewind(infile);

            /* get data block by block */
            for (num=0, count=0; num<num_blocks; num++,count++)
            {   if ((num%tenth) == 0) printf(".");      /* indicate progress */
                /* check if less than "block_bytes" bytes left in file */
                offset = ftell(infile);
                if (block_bytes > (total_file_bytes - offset))
                    block_bytes = total_file_bytes - offset;
                if (fread(data_block,(size_t)block_bytes,(size_t)1,infile) < 1)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return 9;
                }
                /* write data to selected files */
                error = 0;
                num_traces = block_bytes / disk_trace_bytes;
                if (InfoPtr->dzt_outfilename[0])              /*** DZT ***/
                {   /* point past first trace header */
                    /* s_p = (short *)((char *)data_block + sizeof(struct SsTraceHdrStruct)); */
                    if (Debug) printf("DT1 -> DZT: num_traces = %d\n",num_traces);
                    if (Debug) getch();
                    for (i=0; i<num_traces; i++,dzt_trace_num++)
                    {   /* point past 128-byte trace header */
                        s_p = (short *)( (char *)data_block +
                                         ((i*disk_trace_bytes)+128) );
                        if (Debug) printf("***** s_p = %p  trace = %d\n",s_p,i);
                        /* convert data to unsigned short int */
                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
							us_trace[j] = (long)(s_p[j]) + 32768L;
                        us_trace[0] = 0xFFFF;
                        us_trace[1] = 0xF000;
                        if (InfoPtr->num_marks > 0)
                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                {   us_trace[1] = 0xE800;
                                    mrk_start_num = j+1;
                                    break;  /* out for loop checking for marks */
                                }
                            }
                        }

                        /* write data to disk */
                        if (fwrite((void *)us_trace,
                                   InfoPtr->out_samps_per_trace * sizeof(unsigned short),
                                   1,dzt_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        /* point to start of next trace data */
                        /* s_p = (short *)((char *)s_p + disk_trace_bytes); */
                    }
                    if (Debug) getch();
                }
                if (InfoPtr->dt1_outfilename[0])              /*** DT1 ***/
                {   if (fwrite(data_block,(size_t)block_bytes,(size_t)1,dt1_outfile) < 1)
                    error = 10;
                }
                if (InfoPtr->sgy_outfilename[0])              /*** SGY ***/
                {   long timezero = (double)(InfoPtr->timezero_sample/(InfoPtr->in_samps_per_trace-1))
                                    * InfoPtr->in_time_window;
                    long sample_int = (InfoPtr->in_time_window /
                                       (InfoPtr->in_samps_per_trace-1) ) * 1000;
                    struct SsTraceHdrStruct *h_p;
                    void *v_p;

                    /* point header pointer at start of block */
                    h_p = (struct SsTraceHdrStruct *)data_block;
                    /* point data pointer past trace header */
                    v_p = (void *)((char *)data_block + sizeof(struct SsTraceHdrStruct));
                    for (i=0; i<num_traces; i++,sgy_trace_num++)
                    {   /* set values in the trace header */
                        SetSgyTraceHeader((long)sgy_trace_num,
                            InfoPtr->antenna_separation,(double)(h_p->elevation),
							(double)(h_p->position),timezero,InfoPtr->in_samps_per_trace,
                            sample_int,&SgyTraceHdr);

                        /* save trace header and trace */
                        if (fwrite((void *)&SgyTraceHdr,sizeof(struct SegyTraceHdrStruct),
                                    1,sgy_outfile) < 1)
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        if (fwrite(v_p,InfoPtr->in_samps_per_trace * sizeof(short),
                                    1,sgy_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        /* point to start of next trace header */
                        h_p = (struct SsTraceHdrStruct *)((char *)h_p + disk_trace_bytes);
                        /* point to start of next trace data */
                        v_p = (void *)((char *)v_p + disk_trace_bytes);
                        if (Debug) printf("##### h_p = %p   s_p = %p\n",h_p,s_p);
                    }
                    if (Debug) getch();
                }
                if (error)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return error;
                }
            } /* end of for loop getting data blocks */
            break;
        }

        case SGY:
        {   rewind(infile);
            fseek(infile,sizeof(struct SegyReelHdrStruct),SEEK_SET);

			for (num=0, count=0; num<num_blocks; num++)
            {   if ((num%tenth) == 0) printf(".");      /* indicate progress */
                /* check if less than "block_bytes" bytes left in file */
                offset = ftell(infile);
                if (block_bytes > (total_file_bytes - offset))
                    block_bytes = total_file_bytes - offset;
                if (fread(data_block,(size_t)block_bytes,(size_t)1,infile) < 1)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return 9;
                }
                /* write data to selected files */
                error = 0;
                num_traces = block_bytes / disk_chunk_bytes;
                if (InfoPtr->dzt_outfilename[0])              /*** DZT ***/
                {   /* point past trace header */
/* ???? will SGY data always be unsigned sort ??? jl 5/21/96 ???? */
                    s_p = (short *)((char *)data_block + sizeof(struct SegyTraceHdrStruct));
                    for (i=0; i<num_traces; i++)
                    {   /* convert data to unsigned short int */
                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                            us_trace[j] = (long)(s_p[j]) + 32768L;
                        us_trace[0] = 0xFFFF;
                        us_trace[1] = 0xF000;
                        if (InfoPtr->num_marks > 0)
                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                {   us_trace[1] = 0xE800;
                                    mrk_start_num = j+1;
                                    break;  /* out for loop checking for marks */
                                }
                            }
                        }
                        /* write data to disk */
                        if (fwrite((void *)us_trace,
								   InfoPtr->out_samps_per_trace * sizeof(unsigned short),
                                   1,dzt_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        /* point to start of next trace data */
                        s_p = (short *)((char *)s_p + disk_trace_bytes);
                    }
                }
                if (InfoPtr->dt1_outfilename[0])              /*** DT1 ***/
                {   struct SegyTraceHdrStruct *h_p;

                    /* point header pointer at start of block */
                    h_p = (struct SegyTraceHdrStruct *)data_block;
                    /* point data pointer past trace header */
                    s_p = (short *)((char *)data_block + sizeof(struct SegyTraceHdrStruct));
                    for (i=0; i<num_traces; i++,dt1_trace_num++)
                    {   /* set values in the trace header */
                        SetDt1TraceHeader((float)dt1_trace_num,InfoPtr->position,
/* was num_samples instead of in_samps_per_trace */
                            InfoPtr->in_samps_per_trace,InfoPtr->elevation,
                            InfoPtr->num_bytes,(float)dt1_trace_num,
                            InfoPtr->num_stacks,(float)InfoPtr->in_time_window,
                            InfoPtr->timezero_adjust,InfoPtr->zero_flag,
                            InfoPtr->time_of_day,InfoPtr->comment_flag,
                            InfoPtr->comment,&Dt1TraceHdr);

                        /* save trace header and trace */
                        if (fwrite((void *)&Dt1TraceHdr,sizeof(struct SsTraceHdrStruct),
                                    1,dt1_outfile) < 1)
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        if (fwrite((void *)s_p,InfoPtr->in_samps_per_trace * sizeof(short),
                                    1,dt1_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        /* point to start of next trace header */
                        h_p = (struct SegyTraceHdrStruct *)((char *)h_p + disk_trace_bytes);
                        /* point to start of next trace data */
                        s_p = (short *)((char *)s_p + disk_trace_bytes);
                    }
                }
                if (InfoPtr->sgy_outfilename[0])              /*** SGY ***/
                {   if (fwrite(data_block,(size_t)block_bytes,(size_t)1,sgy_outfile) < 1)
						error = 10;
                    if (Debug)
                    {   printf("SGY -> SGY: data_block = %p  bytes = %d\n",
                            data_block,block_bytes);
                        getch();
                    }
                }
                if (error)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return error;
                }
                count++;
            } /* end of for loop getting data blocks */
            break;
        }

        case OTHER:
        {   sgy_trace_num = dt1_trace_num = 0;
            rewind(infile);
            offset = InfoPtr->num_input_channels * InfoPtr->file_header_bytes;
            fseek(infile,offset,SEEK_SET);

            for (num=0, count=0; num<num_blocks; num++)
            {   if ((num%tenth) == 0) printf(".");      /* indicate progress */
                /* check if less than "block_bytes" bytes left in file */
                offset = ftell(infile);
                if (block_bytes > (total_file_bytes - offset))
                    block_bytes = total_file_bytes - offset;
                if (Debug) printf("num=%d block_bytes=%d\n",num,block_bytes);
                if (fread(data_block,(size_t)block_bytes,(size_t)1,infile) < 1)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
					if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return 9;
                }
                /* write data to selected files */
                error = 0;
                num_traces = block_bytes / disk_chunk_bytes;
                if (InfoPtr->dzt_outfilename[0])              /*** DZT ***/
                {   int offset;

                    assert(us_trace != NULL || uc_trace != NULL);
                    if (Debug) printf("DZT: num_traces = %d\n",num_traces);
                    for (i=0; i<num_traces; i++,dzt_trace_num++)
                    {   if (Debug) printf("i = %d\n",i);
                        offset = InfoPtr->trace_header_bytes +
                                 (i *  disk_trace_bytes);
                        /* convert data to unsigned char or short */
                        switch (InfoPtr->input_datatype)
                        {   case -1:
                                /* point past trace header */
                                uc_p = (unsigned char *)((char *)data_block + offset);
                                if (Debug)
                                {  printf("uc_p = %p offset = %d num_samps = %d\n",
                                           uc_p,offset,InfoPtr->in_samps_per_trace);
                                   getch();
                                }
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = uc_p[j];
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
											}
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = uc_p[j] << 8;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                }
                                break;
                            case 1:
                                /* point past trace header */
                                c_p = (char *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = ((short)c_p[j] + 128);
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = ((unsigned long)((short)c_p[j] + 128)) << 8;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
										{   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                }
                                break;
                            case -2:
                                /* point past trace header */
                                us_p = (unsigned short *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = us_p[j] >> 8;
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = us_p[j];
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
								}
                                break;
                            case -5:
                                /* point past trace header */
                                us_p = (unsigned short *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = us_p[j] >> 4;
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = us_p[j] << 4;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                }
                                break;
                            case 2:
                                /* point past trace header */
                                s_p = (short *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = ((unsigned long)((long)s_p[j] + 32768L)) >> 8;
										uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = ((long)s_p[j] + 32768L);
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                }
                                break;
                            case -3:
                                /* point past trace header */
                                ul_p = (unsigned long *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = ul_p[j] >> 24;
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
											}
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = ul_p[j] >> 16;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                }
                                break;
                            case -6:
                                /* point past trace header */
                                ul_p = (unsigned long *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = ul_p[j] >> 16;
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = ul_p[j] >> 8;
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
										{   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                }
                                break;
                            case 3:
                                /* point past trace header */
                                l_p = (long *)((char *)data_block + offset);
                                switch (InfoPtr->output_datatype)
                                {   case -1:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            uc_trace[j] = ((l_p[j]/16777216L) + 128L);
                                        uc_trace[0] = 0xFF;
                                        uc_trace[1] = 0xF0;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   uc_trace[1] = 0xE8;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
                                    case -2:
                                        for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                            us_trace[j] = ((l_p[j]/65536L) + 32768L);
                                        us_trace[0] = 0xFFFF;
                                        us_trace[1] = 0xF000;
                                        if (InfoPtr->num_marks > 0)
                                        {   for (j=mrk_start_num; j<InfoPtr->num_marks; j++)
                                            {   if ((long)dzt_trace_num == InfoPtr->marks[j])
                                                {   us_trace[1] = 0xE800;
                                                    mrk_start_num = j+1;
                                                    break;  /* out for loop checking for marks */
                                                }
                                            }
                                        }
                                        break;
								}
                                break;
                            case 4: case 8:
                                fclose(infile);
                                if (dzt_outfile) fclose(dzt_outfile);
                                if (dt1_outfile) fclose(dt1_outfile);
                                if (sgy_outfile) fclose(sgy_outfile);
                                if (sg2_outfile) fclose(sg2_outfile);
                                if (rd3_outfile) fclose(rd3_outfile);
                                #if defined(_INTELC32_)
                                  realfree(inbuffer);  realfree(dzt_outbuffer);
                                  realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                                  realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                                #endif
                                free(data_block);
                                return 13;
                        }
                        /* write data to disk */
                        if (InfoPtr->output_datatype == -1)
                        {   if (fwrite((void *)uc_trace,
                                   InfoPtr->out_samps_per_trace * sizeof(unsigned char),
                                   1,dzt_outfile) < 1 )
                            {   error = 10;
                                   break; /* out of for loop working thru traces */
                            }
                        } else
                        {   if (fwrite((void *)us_trace,
                                   InfoPtr->out_samps_per_trace * sizeof(unsigned short),
                                   1,dzt_outfile) < 1 )
                            {   error = 10;
                                   break; /* out of for loop working thru traces */
                            }
                        }
                    }
                }
                if (InfoPtr->dt1_outfilename[0])              /*** DT1 ***/
                {   assert(s_trace != NULL);
                    for (i=0; i<num_traces; i++,dt1_trace_num++)
                    {   if (Debug) printf("i = %d  trace_num = %d\n",i,dt1_trace_num);
                        /* convert data to short int */
                        switch (InfoPtr->input_datatype)
                        {   case -1:
                                /* point past trace header */
                                uc_p = (unsigned char *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
										(i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((short)uc_p[j] - 128) * 256 ;
                                break;
                            case 1:
                                /* point past trace header */
                                c_p = (char *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (short)c_p[j] * 256;
                                break;
                            case -2:
                                /* point past trace header */
                                us_p = (unsigned short *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)us_p[j] - 32768L;
                                break;
                            case -5:
                                /* point past trace header */
                                us_p = (unsigned short *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((long)us_p[j] - 32768L) * 16;
                                break;
                            case 2:
                                /* point past trace header */
                                s_p = (short *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = s_p[j];
                                break;
                            case -3:
                                /* point past trace header */
                                ul_p = (unsigned long *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)(ul_p[j]/65536L) - 32768L;
                                break;
                            case -6:
								/* point past trace header */
                                ul_p = (unsigned long *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((long)(ul_p[j]/65536L) - 32768L) * 256;
                                break;
                            case 3:
                                /* point past trace header */
                                l_p = (long *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = l_p[j]/65536L;
                                break;
                            case 4: case 8:
                                fclose(infile);
                                if (dzt_outfile) fclose(dzt_outfile);
                                if (dt1_outfile) fclose(dt1_outfile);
                                if (sgy_outfile) fclose(sgy_outfile);
                                if (sg2_outfile) fclose(sg2_outfile);
                                if (rd3_outfile) fclose(rd3_outfile);
                                #if defined(_INTELC32_)
                                  realfree(inbuffer);  realfree(dzt_outbuffer);
                                  realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                                  realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                                #endif
                                free(data_block);
                                return 13;
                        }
                        /* set values in the trace header */
                        SetDt1TraceHeader((float)dt1_trace_num,InfoPtr->position,
/* was num_samples instead of in_samps_per_trace */
                            InfoPtr->in_samps_per_trace,InfoPtr->elevation,
                            InfoPtr->num_bytes,(float)dt1_trace_num,
                            InfoPtr->num_stacks,(float)InfoPtr->in_time_window,
                            InfoPtr->timezero_adjust,InfoPtr->zero_flag,
                            InfoPtr->time_of_day,InfoPtr->comment_flag,
                            InfoPtr->comment,&Dt1TraceHdr);

                        /* save trace header and trace */
                        if (fwrite((void *)&Dt1TraceHdr,sizeof(struct SsTraceHdrStruct),
                                    1,dt1_outfile) < 1)
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
						if (fwrite((void *)s_trace,InfoPtr->in_samps_per_trace * sizeof(short),
                                    1,dt1_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                    } /* end of for loop working thru traces */
                }
                if (InfoPtr->sgy_outfilename[0])              /*** SGY ***/
                {   long timezero = (InfoPtr->timezero_sample/(InfoPtr->in_samps_per_trace-1))
                                    * InfoPtr->in_time_window;
                    long sample_int = ( InfoPtr->in_time_window /
                                       (InfoPtr->in_samps_per_trace-1) ) * 1000;

                    assert(s_trace != NULL);
                    for (i=0; i<num_traces; i++,sgy_trace_num++)
                    {   if (Debug) printf("i = %d  trace_num = %d\n",i,sgy_trace_num);
                        /* convert data to short int */
                        switch (InfoPtr->input_datatype)
                        {   case -1:
                                /* point past trace header */
                                uc_p = (unsigned char *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((short)uc_p[j] - 128) * 256 ;
                                break;
                            case 1:
                                /* point past trace header */
                                c_p = (char *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (short)c_p[j] * 256;
                                break;
                            case -2:
                                /* point past trace header */
                                us_p = (unsigned short *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)us_p[j] - 32768L;
                                break;
                            case -5:
                                /* point past trace header */
                                us_p = (unsigned short *)((char *)data_block +
										InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((long)us_p[j] - 32768L) * 16;
                                break;
                            case 2:
                                /* point past trace header */
                                s_p = (short *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = s_p[j];
                                break;
                            case -3:
                                /* point past trace header */
                                ul_p = (unsigned long *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = (long)(ul_p[j]/65536L) - 32768L;
                                break;
                            case -6:
                                /* point past trace header */
                                ul_p = (unsigned long *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = ((long)(ul_p[j]/65536L) - 32768L) * 256;
                                break;
                            case 3:
                                /* point past trace header */
                                l_p = (long *)((char *)data_block +
                                        InfoPtr->trace_header_bytes +
                                        (i *  disk_trace_bytes) );
                                for (j=0; j<InfoPtr->in_samps_per_trace; j++)
                                    s_trace[j] = l_p[j]/65536L;
                                break;
                            case 4: case 8:
                                fclose(infile);
                                if (dzt_outfile) fclose(dzt_outfile);
                                if (dt1_outfile) fclose(dt1_outfile);
                                if (sgy_outfile) fclose(sgy_outfile);
                                if (sg2_outfile) fclose(sg2_outfile);
                                if (rd3_outfile) fclose(rd3_outfile);
                                #if defined(_INTELC32_)
								  realfree(inbuffer);  realfree(dzt_outbuffer);
                                  realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                                  realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                                #endif
                                free(data_block);
                                return 13;
                        }
                        /* set values in the trace header */
                        SetSgyTraceHeader((long)sgy_trace_num,
                            InfoPtr->antenna_separation,0.0,
                            0.0,timezero,InfoPtr->in_samps_per_trace,
                            sample_int,&SgyTraceHdr);

                        /* save trace header and trace */
                        if (fwrite((void *)&SgyTraceHdr,sizeof(struct SegyTraceHdrStruct),
                                    1,sgy_outfile) < 1)
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                        if (fwrite((void *)s_trace,InfoPtr->in_samps_per_trace * sizeof(short),
                                    1,sgy_outfile) < 1 )
                        {   error = 10;
                            break; /* out of for loop working thru traces */
                        }
                    } /* end of for loop working thru traces */

                }
                if (error)
                {   fclose(infile);
                    if (dzt_outfile) fclose(dzt_outfile);
                    if (dt1_outfile) fclose(dt1_outfile);
                    if (sgy_outfile) fclose(sgy_outfile);
                    if (sg2_outfile) fclose(sg2_outfile);
                    if (rd3_outfile) fclose(rd3_outfile);
                    #if defined(_INTELC32_)
                      realfree(inbuffer);  realfree(dzt_outbuffer);
                      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
                      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
                    #endif
                    free(data_block);
                    return error;
                }
                count++;
            } /* end of for loop getting data blocks */
            break;
		}
        default:
            fclose(infile);
            if (dzt_outfile) fclose(dzt_outfile);
            if (dt1_outfile) fclose(dt1_outfile);
            if (sgy_outfile) fclose(sgy_outfile);
            if (sg2_outfile) fclose(sg2_outfile);
            if (rd3_outfile) fclose(rd3_outfile);
            #if defined(_INTELC32_)
              realfree(inbuffer);  realfree(dzt_outbuffer);
              realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
              realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
            #endif
            free(data_block);
            return 8;
    } /* end of switch block selecting input format */

    if (Debug) printf("Number of data blocks written = %d\n",count);
    fclose(infile);
    if (dzt_outfile) fclose(dzt_outfile);
    if (dt1_outfile) fclose(dt1_outfile);
    if (sgy_outfile) fclose(sgy_outfile);
    if (sg2_outfile) fclose(sg2_outfile);
    if (rd3_outfile) fclose(rd3_outfile);
    #if defined(_INTELC32_)
      realfree(inbuffer);  realfree(dzt_outbuffer);
      realfree(dt1_outbuffer);  realfree(sgy_outbuffer);
      realfree(sg2_outbuffer);  realfree(rd3_outbuffer);
    #endif
    free(data_block);
    return 0;
}
/**************************** end of GPR_CONV.C *****************************/
