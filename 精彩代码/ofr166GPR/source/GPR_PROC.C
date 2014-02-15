/******************************** GPR_PROC.C ********************************\
                              version 1.05.02.00
-----------------------------------------------------------------------------
INTRODUCTION

GPR_PROC.EXE processes digital ground penetrating radar (GPR) data. It does
not convert storage formats. GPR_CONV.EXE can be used to convert between
popular storage formats and user-defined formats. There is no graphic display
of the data. To display the processed data, use programs such as GPR_DISP.EXE
or FIELDVEW.EXE. This program can select a subset of traces from a file to
use but cannot change the number of samples per trace. Use GPR_SAMP.EXE to 
resample GPR traces. The GPR data are converted to 64-bit floating point 
values (doubles) before processing, and back to native data type after 
processing. This creates extra memory demands, quintupling the RAM storage
required to process an entire data set. GPR_CNDS.EXE can reduce the size of a
data set by eliminating traces (specifying starting and ending traces and 
number of traces to skip). If a data set is split into two parts, then 
GPR_JOIN.EXE can re-join the two data sets. The program GPR_STAK.EXE also 
reduces the size of a data set by stacking the data. Finally, to place the 
traces at uniform distances from each other, use GPR_XFRM.EXE

The input to GPR_PROC.EXE is a "CMD" file, an ASCII text file containing 
keywords (or commands) which are discussed below.

The GPR data can be read automatically from disk using the following formats 
only:
  - GSSI SIR-10A ver 4.x or 5.x binary "DZT" files (1 or more 512- or 1024-
    byte headers followed by 1 or more channels of data, trace by trace),
    NOTE: Only ONE channel will be processed and stored for multiple-channel
          DZT files.
  - Sensors and Software PulseEKKO "DT1" and "HD" files, or
  - Society of Exploration Geophysicists SEG-Y files that follow the Sensors
    and Software style for formatting the reel (file) header.

If the storage format does not conform to any of the above or this program is 
having trouble reading the file correctly, there are options for the user to 
specify the required information.

A message file called GPR_PROC.LOG is opened when the program starts. It is 
located either in the directory where the program was called from or in the 
root directory of drive C. This open file may prevent more than one session
of the program from executing in the same directory. The log file may contain
more information regarding the failure or success of GPR_PROC as it executes. 
Each session is appended at the end of the log file.

GPR_PROC.EXE is a protected-mode MS-DOS program that executes within the DOS
operating system or within DOS (or console) windows in Microsoft Windows 
3.x/95/98. It probably will not execute within a console window in Windows
NT. This program will force the computer CPU to work in "protected mode", 
that is, all memory can be utilized, not just the 1 MB that DOS normally has 
access to. If an error message occurs when trying to run GPR_PROC.EXE it may
be because the requested amount of "protected mode" memory (the region size)
is set too low for the program or is more than what is available on the
computer. Use the program MODXCONF.EXE, which should have been distributed
with this software, to adjust the region size.

NOTE: Files used by this program must follow the naming conventions required
by early versions of MS-DOS. Filenames can be only eight (8) characters long,
with no embedded spaces, with an optional extension of up to three
characters. This program will run in a Windows 95/98 console where filenames
do not have to follow this convention. Unexpected results may occur if input
file names, including the CMD file, are longer than 8 characters. For
example, both command files procfile.cmd and procfile1.cmd will be seen as
procfile.cmd.

-----------------------------------------------------------------------------
PROCESSING FUNCTIONS

The processing functions are executed in the order they are given in the
keyword file. They can be called in any order, but the default order
(established in GPR_PROC.CMD) follows. This arrangement is a suggestion only
and is not intended to be a processing flow chart. Up to a maximum of 100
processing steps can be called.
- Remove a global background trace
- Remove range gain
- Adjust trace mean up or down
- Low-, High-, or Band-pass filter traces
- Shift samples up or down
- Add range gain
- Remove a sliding-window "background" average
- Stack traces
- Scale the trace amplitudes
- Smooth horizontally over several traces
- Smooth vertically over samples within a trace
- Apply a spatial ("horizontal") median filter
- Apply a temporal ("vertical") median filter
- Transform traces to instantaneous amplitude
- Transform traces to instantaneous power
- Equalize all traces

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
February 8, 1994    - Completed as an extensive revision of GPR_PROC.C.
                    - Changed name to PROCESS1.C.
                    - Some features are not implemented but have places
                      reserved for them.
February 15, 1994   - Added "trace_per_sec" to GPG header structure and
                      to ParamInfoStruct.  The value is not used here but
                      simply passed through.
                    - Corrected calculation of "first_proc_samp" for DZT
                      files in GetData().
June 21, 1994       - Replaced all "size_t" with "long" in function
                      declarations in header files.
                    - Fixed GetCmdFileArgs(): break was missing for case 21,
                      and now breaks out of case if
                      mean_adjust == INVALID_VALUE or stack == 0.
                    - Corrected calculation of "ns_per_samp" for DZT files
                      in GetData().
                    - Updated SaveDztFile() to set all bits in first sample
                      for each trace (2nd, 3rd, and 4th channels do not
                      have first sample bits set!).
                    - Updated PrintUsageMsg().
                    - Added "first_proc_samp" to allow for reserved samples
                      and trace headers.  Following functions were modified:
                      AdjustMeanGridTraces(), FftFilterGridTraces(),
                      StackGrid(), RescaleGrid(), RemGlobBckgrd(),
                      RemWindBckgrd(), SmoothHorizontally(),
                      SmoothVertically(), GetData(), ProcessData(), and
                      InitParameters().
June 28, 1994       - Modified SaveDztData() and SaveDztFile(); "rg_breaks"
                      was not being initialized for other-than-DZT input
                      files plus fixed other problems.
                    - Now copies tick marker locations from the RIF file to
                      the DZT file (if that is output format) and sets
                      second sample in output DZT files to 0xF1 or 0xF100
                      in SaveDztData.
August 29, 1994     - Now copies comments from RIF file to DZT header (if
                      those are input and output formats) in SaveDztData().
                    - Corrected spelling error in DisplayParameters()
                      message.
                    - Made sure filename[] in main() was initialized.
November 16, 1994   - Add temporal ("vertical") and spatial ("horizontal")
                      median filters.
                    - Fixed numerous warnings where a function argument
                      should be size_t and was not.
November 17, 1994   - Replaced Intel's <assert.h> with my "assertjl.h"
March 8, 1995       - Removed the requirement in GetDztChSubGrid8() and
                      GetDztChSubGrid16() that the first sample of the
                      first trace to have all bits set (GSSI SIR-10 data).
                      To check that bits are set, #define VERIFY_SYNC
                      before compilation.
March 23, 1995      - Removed requirement that GSSI DZT files strictly
                      match version 5.x of the software.
March 28, 1995      - Revised the functions that read DZT headers to
                      re-access the file using a 512-byte header if 1024-
                      byte header reports a format error.
                    - Made program recognize storage format using filename
                      extension.  Allowed user to specify the storage
                      format.
March 29, 1995      - Removed support for GPG and GSG file formats.
                    - Added ant_freq and trace_per_sec as parameters the
                      user can optionally supply in the command file.
                    - Revised the handling of the ant_freq variable.
                    - Modified SaveDztFile() to add filler if the number
                      of samples is not a power of 2.
                    - Removed option to specify info files for input or
                      output.  User must name the data files.
                    - Clarified use of first_samp and last_samp, they are
                      just passed through to output header/file.
                    - GetDztChSubGrid8() and GetDztChSubGrid16() can now
                      handle 512-byte headers (old style) also.
                    - Added checks in GetData() for valid ranges for
                      required variables: total_traces, total_samps,
                      input_datatype, and ns_per_samp.
March 30, 1995      - Added error/message file output.
                    - Fixed assignment of antenna name in SaveDztData()
                      when input type not standard DZT file.
                    - Minor corrections in GetDztChSubGrid8() and
                      GetDztChSubGrid16().
January 25, 1996    - Changed name to GPR_PROC v. 1.00.
                    - Changed ParamInfoStruct to ProcParamInfoStruct.
                    - Removed support for DAT/RIF and SEG-2 files.
                    - Removed variable "last_samp".
                    - Changed GetData() to GetGprDataAsGrid().
                    - Made changes to PrintUsageMsg(), GetParameters(),
                      and DisplayParameters() to be like GPR_DISP.
                    - GetCmdFileArgs() will now add ".cmd" if missing.
                    - Modified FftFilterGridTraces(), RescaleGrid(),
                      AdjustMeanGridTRaces(), and RemGridGlobBckgrnd()
                      to handle limits of data types -5 and -6 correctly.
January 26, 1996    - Changed ProcessData() to ProcessGprData().
                    - Added function to change trace range gain.
                    - Changed SaveData() to SaveGprData().
                    - Added provision in main() to check for duplicate
                      input and output filenames.
                    - Now zero-out info struct before using it.
                    - Added option to save data using input user-defined
                      format.
January 29, 1996    - Removed the "passed-through" variables: first_samp,
                      ant_freq, trace_per_sec, trace_per_m, start_pos,
                      final_pos, and step_size.
                    - Changed data output style: Output files now must
                      be same storage format as input files.  "Pass-through"
                      info is gotten form input file header.
January 30, 1996    - Revised documentation in CMD file.
                    - Implemented "INVALID_VALUE" in GetCmdFileArgs().
                    - Moved parameter initialization and verification from
                      GetGprDataAsGrid() to GetParameters().
                    - GPR_IO.LIB must be linked now for data I/O functions.
                    - Updated SaveDztData().
January 31, 1996    - Added SaveDt1Data(), SaveSgyData(), SaveUsrData().
February 1, 1996    - Changed SetDztHeader() to SetDzt5xHeader(), making
                      error checking improvements and implementing ver. 5.x
                      changes.
                    - Fixed problem in RescaleGrid(), wasn't scaling
                      correctly for unsigned data.
February 2, 1996    - Fixed serious bug in SmoothGridVertically(), wasn't
                      initializing sliding queue correctly.
                    - Modified ChangeGridRangeGain() to use first_proc_samp.
February 5, 1996    - Removed functions AdjustMeanGridTraces(),
                      ApplyGridSpatMedFilter(), ApplyGridTempMedFilter(),
                      ChangeGridRangeGain(), FftFilterGridTraces(),
                      RemGridGlobBckgrnd(), RemGridWindBckgrnd(),
                      RescaleGrid(), SmoothGridHorizontally(),
                      SmoothGridVertically(), and StackGrid() and placed in
                      GPR_DFX.C. - GPR_DFX.LIB must now be linked in at
                      compile time.
                    - Modified ProcessGprData() to use message codes
                      returned by processing functions.
                    - Removed functions ANSI_there(), GetDosVersion(),
                      FftFilterTrace(), BandPassFFT1D(), FFT1D(),
                      RealFFT1D(), Sound(), and TrimStr() and placed in
                      JL_UTIL1.C. - JL_UTIL1.LIB must now be linked in at
                      compile time.
May 23, 1996        - Added values to methods when recording to processing
                      history field for output file header.  And check
                      that not overwriting to proc_hist[].  Increased
                      proc_hist[] size to 400.  All in SaveGprData().
                    - ...in_time_window changed from int to double.
December 23, 1996   - Increase version to 1.1.
                    - Added sample sliding.
                    - Update developer instructions in GPR_PROC.H.
December 24, 1996   - Repaired FftFilterGridTraces() in GPR_DFX.C -
                      fund. frequency not calculated exactly right and
                      verified rest of FFT/filtering correct.
                    - Added command file name to list of log file meaasages.
December 30, 1996   - Increase version to 1.11.
                    - Add trace amplitude equalization.
January 13, 1997    - Clarify use of slide_samps - that DZT range gain
                      values in header are NOT modified.
                    - Minor changes in main().
August 11, 1997     - Added support for modified DZT formats.
August 22, 1997     - Changed "gpr_proc.log" to a variable.  If open fails,
                      try to open as "c:\gpr_proc.log"
                    - Increase version to 1.2.
                    - Modified ending messages to include output filename and
                      log filename.
September 4, 1997   - Increase version to 2.0.
                    - Changed how Nyquist and Fundamental frequencies are
                      calculated and how lo_freq_cuttoff and hi_freq_cutoff
                      are checked in GetParameters().
                    - Fixed call to FftFilterGridTraces() to allow non-power
                      of 2 number of rows in grid.
                    - Changed usage of first_proc_samp so that it is either
                      0 or 2 (for DZT files). Then added header_samps to
                      info structure to record equivalent number of samples
                      in each trace header.  Previously, first_proc_samp was
                      the number of bytes in a trace header and processing
                      functions were executing improperly for non-DZT files!
September 24, 1998  - Fixed problem in GetParameters() where input user
                      storage format values overwritten with bad ones.
April 27, 1999      - Fixed problem in SlideSamples (in gpr_dfx.lib).
                      Negative slide were wrong direction and going out of
                      bounds.
August 18, 1999     - Increase version to 3.0.
                    - Now convert to double before processing and then back
                      again afterwards.
                    - Change how mean_adjust works since now have doubles.
                      Since median already subtracted and will be added later,
                      it now acts more like brightening and darkening.
                    - Added range gain values to processing history comments.
                    - Improved how text added to proc_hist[] in SaveGprData().
August 19, 1999     - Add instantaneous amplitude and instantaneous power.
August 20, 1999     - Rename "mean_adjust" to "amp_adjust".
                    - First_trace, last_trace, and channel are now referenced
                      from one for the user. Internally, index is from 0.
August 25, 1999     - Data are now forced to doubles only for processing in
                      ProcessGprData(); native type is not supported.
March 14-22, 2000   - Changed name to GPR_PROC and version to 1.00.
                    - Up to 100 processing steps can now be called - and each
                      function more than once by adding proc_value[] and
                      proc_set[].
                    - Now free dgrid[][] before leaving ProcessGprData().
                    - Added mrk_infilename.
                    - Added DeallocInfoStruct().
                    - Fixed: __DATE__ is not Y2K compliant!
                    - Had to change how revision compile date handled. Codebldr
                      is not Y2K compliant! Now use #define to be set compile
                      date by hand and _dos_getdate() and _dos_gettime() for
                      run-time.
                    - Set proc_hist size to #define 512.
                    - Removed user-defined storage types
March 28, 2000      - Now allow negative numbers for amp_scale.
                    - Added foreground removal for global (ie., assign
                      background trace to all traces) and window (ie. the sub-
                      horizontal "background" is left and high-angle features
                      are removed).
March 29, 2000      - Changed version numver to reflect date: 1.03.29.00.
                    - Improved documentation.
                    - Fixed how defualt values for hi-cut and lo-cut are
                      calculated in GetGprDataAsGrid().
July 24, 2000       - Added author message to MSG file.                     

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /zfloatsync /xregion=32M gpr_proc.c gpr_io.lib gpr_dfx.lib
          jl_util1.lib
     To compile for 80486 executable:
       icc /F /xnovm /zfloatsync /zmod486 /xregion=32M gpr_proc.c gpr_io.lib
            gpr_dfx.lib jl_util1.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions, speeds up program.
     /xregion=32M sets the region size to 32 Mbytes
     Jeff Lucius's GPR I/O functions (DZT, DT1, SGY) are in gpr_io.lib.
     Jeff Lucius's GPR data manipulation functions are in gpr_dfx.lib.
     Jeff Lucius's utility functions are in jl_util1.lib.

     to remove assert() functions from code also add:
     /DNDEBUG

To run: GPR_PROC cmd_filename
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
 *   |           +-- TrimStr
 *   +-- DisplayParameters                             (con I/O)
 *   |       This function displays the user-input parameters on the CRT.
 *  Start LOOP
 *   +-- GetGprDataAsGrid                    (disk I/O)(con I/O)
 *   |     +-- GetGprFileType                (disk I/O)
 *   |     +-- ReadOneDztHeader              (disk I/O)
 *   |     +-- InitDt1Parameters
 *   |     +-- GetSsHdFile                   (disk I/O)
 *   |     +-- PrintSsHdInfo                           (con I/O)
 *   |     +-- ReadSegyReelHdr               (disk I/O)
 *   |     +-- PrintSegyReelHdr                        (con I/O)
 *   |     +-- GetDztChSubGrid8              (disk I/O)
 *   |     +-- GetDztChSubGrid16             (disk I/O)
 *   |     +-- GetGprSubGrid                 (disk I/O)
 *   |     +-- TrimStr
 *   +-- ProcessGprData                      (disk I/O)(con I/O)
 *   |     +-- ChangeGridRangeGain
 *   |     +-- StackGrid
 *   |     +-- SlideSamples
 *   |     +-- RescaleGrid
 *   |     +-- EqualizeGridTraces
 *   |     +-- FftFilterGridTraces
 *   |     |     +-- FftFilterTrace
 *   |     |           +-- BandPassFFT1D
 *   |     |                +-- RealFFT1D
 *   |     |                      +-- FFT1D
 *   |     +-- InstAtribGridTraces
 *   |     |     +-- InstAtribTrace
 *   |     |           +-- FFT1D
 *   |     +-- AdjustMeanGridTraces
 *   |     +-- RemGridGlobBckgrnd
 *   |     +-- RemGridWindBckgrnd
 *   |     +-- SmoothGridHorizontally
 *   |     +-- SmoothGridVertically
 *   |     +-- ApplyGridSpatMedFilter
 *   |     |     +-- ucCompare
 *   |     |     +-- cCompare
 *   |     |     +-- usCompare
 *   |     |     +-- sCompare
 *   |     |     +-- ulCompare
 *   |     |     +-- lCompare
 *   |     |     +-- fCompare
 *   |     |     +-- dCompare
 *   |     +-- ApplyGridTempMedFilter
 *   |           +-- ucCompare
 *   |           +-- cCompare
 *   |           +-- usCompare
 *   |           +-- sCompare
 *   |           +-- ulCompare
 *   |           +-- lCompare
 *   |           +-- fCompare
 *   |           +-- dCompare
 *   +-- SaveGprData                         (disk I/O)(con I/O)
 *   |     +-- SaveDztData                   (disk I/O)(con I/O)
 *   |     |     +-- ReadOneDztHeader        (disk I/O)
 *   |     |     +-- SetDzt5xHeader
 *   |     |     +-- SaveDztFile             (disk I/O)
 *   |     +-- SaveDt1Data                   (disk I/O)
 *   |     |     +-- GetSsHdFile             (disk I/O)
 *   |     |     +-- SaveSsHdFile            (disk I/O)
 *   |     +-- SaveSgyData                   (disk I/O)
 *   |           +-- ReadSegyReelHdr         (disk I/O)
 *   |           +-- ExtractSsInfoFromSegy
 *   |           +-- SetSgyFileHeader
 *  END LOOP
 *   +-- DeallocInfoStruct
 */

/******************* Includes all required header files *********************/
#include "gpr_proc.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The next array is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_PROC_CMDS[] =
{   "debug","batch","num_input_files","input_filelist[]","output_filelist[]",
    "channel","vsmooth","hsmooth","high_freq_cutoff","low_freq_cutoff",

    "preprocFFT","amp_adjust","glob_bckgrnd_rem","wind_bckgrnd_rem","amp_scale",
    "stack","spatial_median","temporal_median","num_gain_off","gain_off[]",

    "num_gain_on","gain_on[]","samp_slide","trace_equalize","inst_amp",
    "inst_pow","wind_forgrnd_rem","glob_forgrnd_rem",NULL,
} ;

const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR:  Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",

    "GetParameters() ERROR: Input filename is incorrectly duplicated in output list.",
    "GetParameters() ERROR: Filename is duplicated in output list.",
} ;
const char *GetGprDataAsGridMsg[] =
{   "GetGprDataAsGrid(): No errors.",
    "GetGprDataAsGrid() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetGprDataAsGrid() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "GetGprFileType() ERROR: Problem getting information about input data file.",
    "ReadOneDztHeader() ERROR: Problem getting information about input data file.",

    "ReadOneDztHeader() ERROR: Invalid channel selection for GSSI DZT file.",
    "GetSsHdFile() ERROR: Problem getting info from S&S HD file.",
    "ReadSegyReelHdr() ERROR: Problem getting info from SGY file.",
    "GetGprDataAsGrid() ERROR: No recognizable data/info input files specified.",
    "GetGprDataAsGrid() ERROR: Invalid input data element type .",

    "GetGprDataAsGrid() ERROR: Less than one trace in file.",
    "GetGprDataAsGrid() ERROR: Less than two samples per trace.",
    "GetGprDataAsGrid() ERROR: nanoseconds per sample is <= 0.0.",
    "GetGprDataAsGrid() ERROR: trace_header_size must be a multiple of sample size.",
    "GetGprDataAsGrid() ERROR: Not able to allocate storage for grid.",

    "GetGprDataAsGrid() ERROR: Problem getting data from input file.",
} ;
const char *ProcessGprDataMsg[] =
{   "ProcessGprData(): No errors.",
    "ProcessGprData() ERROR: Processing function reported error. See log file.",
    "ProcessGprData() ERROR: Unable to allocate storage to convert data to doubles.",
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

/********************************** main() **********************************/
void main (int argc,char *argv[])
{
/***** Declare variables *****/
    /* following are scratch variables used by main */
    char log_filename[80];
    int  itemp,num;
    double dtemp;

    /* following is the information structure passed between functions */
    struct ProcParamInfoStruct ParamInfo;

    /* these variables found near beginning of this source */
    extern int Debug,Batch,ANSI_THERE;
    extern FILE *log_file;
    extern const char *GetParametersMsg[];
    extern const char *GetGprDataAsGridMsg[];
    extern const char *ProcessGprDataMsg[];
    extern const char *SaveGprDataMsg[];

/***** Initialize variables *****/
    /* Open error message file */
    strcpy(log_filename,"gprsproc.log");
    log_file = fopen(log_filename,"a+t");
    if (log_file == NULL)
    {   strcpy(log_filename,"c:\\gprsproc.log");
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
        fprintf(log_file,"Messages from program GPR_PROC v. %s: %s %d, %d at %d:%02d:%02d\n",
                        GPR_PROC_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute,dostime.second);
        fprintf(log_file,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.\n");
    }
    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Zero-out information structure */
    memset((void *)&ParamInfo,0x00,sizeof(struct ProcParamInfoStruct));

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
        printf("\nGPR_PROC finished.\nFile %s on disk contains messages.\n",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(1);
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
            puts("GPR_PROC finished.");
            if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
            exit(3);
        } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
            getch();
        printf("\r                                                                    \r");
    }

/***** Start of LOOP *****/
    for (num=0; num<ParamInfo.num_input_files; num++)
    {   char *cp,str[160];

        /**** Initialize some variables *****/
        ParamInfo.file_num = num;

        cp = strstr(ParamInfo.dat_infilename[num],".DT1");
        if (cp != NULL)
        {   strncpy(ParamInfo.hd_infilename,ParamInfo.dat_infilename[num],
                   (size_t)(cp-ParamInfo.dat_infilename[num]));
            strcat(ParamInfo.hd_infilename,".HD\0");
        }
        cp = strstr(ParamInfo.dat_outfilename[num],".DT1");
        if (cp != NULL)
        {   strncpy(ParamInfo.hd_outfilename,ParamInfo.dat_outfilename[num],
                   (size_t)(cp-ParamInfo.dat_outfilename[num]));
            strcat(ParamInfo.hd_outfilename,".HD\0");
        }

        str[0] = 0;
        strcpy(str,ParamInfo.dat_infilename[num]);
        cp = strstr(str,".");
        if (cp != NULL)  *cp = 0;
        ParamInfo.mrk_infilename[0] = 0;
        strcpy(ParamInfo.mrk_infilename,str);
        strcat(strupr(ParamInfo.mrk_infilename),".MRK");

        str[0] = 0;
        strcpy(str,ParamInfo.dat_infilename[num]);
        cp = strstr(str,".");
        if (cp != NULL)  *cp = 0;
        ParamInfo.xyz_infilename[0] = 0;
        strcpy(ParamInfo.xyz_infilename,str);
        strcat(strupr(ParamInfo.xyz_infilename),".XYZ");

        /***** Get the data *****/
        printf("Getting file %s ...",ParamInfo.dat_infilename[num]);
        itemp = GetGprDataAsGrid(FUNC_CREATE,&ParamInfo);
        printf("\r                                                                   \r");
        if (itemp > 0)
        {   printf("\n%s\n",GetGprDataAsGridMsg[itemp]);
            printf("File #%d %s NOT PROCESSED\n",num,ParamInfo.dat_infilename[num]);
            if (log_file)
            {   fprintf(log_file,"\n%s\n",GetGprDataAsGridMsg[itemp]);
                fprintf(log_file,"File #%d %s NOT PROCESSED\n",num,ParamInfo.dat_infilename[num]);
            }
            if (itemp == 14)  /* data set too large for memory */
            {   unsigned long mem,stack,bytes,requested;

                mem = _memavl();
                switch (ParamInfo.input_datatype)
                {   case -1:  case 1:                    bytes = 1;  break;
                    case -2:  case 2:  case 5:           bytes = 2;  break;
                    case -3:  case 3:  case 6:  case 4:  bytes = 4;  break;
                    case  8:                             bytes = 8;  break;
                }
                requested = (ParamInfo.last_trace - ParamInfo.first_trace + 1)
                             * ParamInfo.total_samps * bytes;
                stack = requested/mem;
                printf("\nSize of GPR data file (%g MB) exceeds memory (%g MB).\n",requested/(1024.0*1024.0),mem/(1024.0*1024.0));
                printf("Try using GPR_CNDS first with skip_traces set to %lu or higher.\n",stack+2);
                printf("Also, try increasing the region size of GPR_PROC.EXE using MODXCONF.\n");
                if (log_file)
                {   fprintf(log_file,"Size of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
                    fprintf(log_file,"Try using GPR_CNDS first with skip_traces set %lu or higher.\n",stack+2);
                }
            }
            continue; /* at end of for LOOP */
        }

        /***** Process the data *****/
        printf("Processing file %s ...",ParamInfo.dat_infilename[num]);
        itemp = ProcessGprData(&ParamInfo);
        printf("\r                                                                   \r");
        if (itemp > 0)
        {   printf("\n%s\n",ProcessGprDataMsg[itemp]);
            printf("File #%d %s NOT PROCESSED\n",num,ParamInfo.dat_infilename[num]);
            if (log_file)
            {   fprintf(log_file,"\n%s\n",ProcessGprDataMsg[itemp]);
                fprintf(log_file,"File #%d %s NOT PROCESSED\n",num,ParamInfo.dat_infilename[num]);
            }
            if (itemp == 2)  /* data set too large for memory */
            {   unsigned long mem,stack,bytes,requested;

                mem = _memavl();
                switch (ParamInfo.input_datatype)
                {   case -1:  case 1:                    bytes = 1;  break;
                    case -2:  case 2:  case 5:           bytes = 2;  break;
                    case -3:  case 3:  case 6:  case 4:  bytes = 4;  break;
                    case  8:                             bytes = 8;  break;
                }
                requested = (ParamInfo.last_trace - ParamInfo.first_trace + 1)
                             * ParamInfo.total_samps * (bytes+8);
                stack = requested/mem;
                printf("\nSize of GPR data file and 8-byte copy (%g MB) exceeds memory (%g MB).\n",requested/(1024.0*1024.0),mem/(1024.0*1024.0));
                printf("Try using GPR_CNDS first with skip_traces set to %lu or higher.\n",stack+2);
                printf("REMEMBER: STACKing does not occur before the grid is converted to doubles\n");
                printf("Also, try increasing the region size of GPR_PROC.EXE using MODXCONF.\n");
                if (log_file) fprintf(log_file,"Size of GPR data file and 8-byte copy (%g MB) exceeds memory (%g MB).\n",requested/(1024*1024),mem/(1024*1024));
                if (log_file) fprintf(log_file,"Try using GPR_CNDS first with skip_traces set %lu or higher.\n",stack+2);
            }
            GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
            continue; /* at end of for LOOP */
        }

        /***** Save grid to disk *****/
        printf("Saving file %s ...",ParamInfo.dat_infilename[num]);
        itemp = SaveGprData(&ParamInfo);
        printf("\r                                                                   \r");
        if (itemp > 0)
        {   printf("\n%s\n",SaveGprDataMsg[itemp]);
            printf("File #%d %s NOT SAVED\n",num,ParamInfo.dat_infilename[num]);
            if (log_file)
            {   fprintf(log_file,"\n%s\n",SaveGprDataMsg[itemp]);
                fprintf(log_file,"File #%d %s NOT SAVED\n",num,ParamInfo.dat_infilename[num]);
            }
            GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
            continue; /* at end of for LOOP */
        }
        GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
    } /* end of for (num=0; num<ParamInfo.num_input_files; num++) */
/***** End of LOOP *****/

/***** Free storage and terminate program *****/
    itemp = GetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
    DeallocInfoStruct(&ParamInfo);
    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
    if (log_file)
    {   fprintf(log_file,"End of messages. Program terminated normally.\n");
        fprintf(log_file,"GPR_PROC finished. Processed data saved to disk.\n");
        fclose(log_file);
        printf("\nFile %s on disk contains messages.\n",log_filename);
    }
    printf("GPR_PROC finished.\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
    exit(0);
}
/****************************** GetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct ProcParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <math.h>, <stdio.h>, <stdlib.h>, <string.h>, "gprsproc.h".
 * Calls: strcpy, printf, fprintf, getch, strstr, strupr, puts, strncat,
 *        atoi, atof, pow,
 *        PrintUsageMsg, GetCmdFileArgs, InitParameters.
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR:  Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",

    "GetParameters() ERROR: Input filename is incorrectly duplicated in output list.",
    "GetParameters() ERROR: Filename is duplicated in output list.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: December 30, 1996
 * Revisions: August 11, 1997; August 25, 1997; September 4, 1997;
 *            September 24, 1998; August 18, 1999
 *            March 22, 2000; March 31, 2000;
 */
int GetParameters (int argc,char *argv[],struct ProcParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/
    int  req_args = 2;       /* exe_name and cmd_filename */
    int  i,j,itemp;          /* scratch variables and counters */

    extern int Debug;        /* at beginning of this source */

/***** Verify usage *****/
    if (Debug) puts("Start of GetParameters()");
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

/**** Check for duplicate filenames *****/
    /* IN names can be in OUT list if in OUT BEFORE in IN */
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   for (j=i; j<InfoPtr->num_input_files; j++)
            if (strcmp(InfoPtr->dat_infilename[i],InfoPtr->dat_outfilename[j])==0)
                return 5;
    }
    /* names in OUT list cannot repeat */
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   for (j=i+1; j<InfoPtr->num_input_files; j++)
            if (strcmp(InfoPtr->dat_outfilename[i],InfoPtr->dat_outfilename[j])==0)
                return 6;
    }
    /* names in IN list can repeat but must go to different OUT names */
    /* no check */

    return 0;
}
/****************************** PrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>.
 * Calls: puts, sprintf, printf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: January 25, 1996; August 18, 1999;
 *       March 21, 2000; March 31, 2000;
 */
void PrintUsageMsg (void)
{
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("###############################################################################");
    puts("  Usage: GPR_PROC cmd_filename");
    puts("    Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
    puts("  This program processes radar data and saves the result to disk.");
    puts("  Required command line arguments:");
    puts("     cmd_filename - name of a text keyword file that follows the \"CMD\" format.");
	puts("  Look at \"GPR_PROC.CMD\" and \"GPR_PROC.TXT\" for keyword file format, valid");
    puts("    keywords, and documentation.");
    puts("  Example call: GPR_PROC pfile1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_PROC_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** InitParameters() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct ProcParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "gprsproc.h".
 * Calls: none.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: December 30, 1996
 * Revisions: August 22, 1997; August 25, 1997; September 4, 1997;
 *            August 18, 1999;
 *            March 21, 2000; March 28, 2000;
 */
void InitParameters (struct ProcParamInfoStruct *InfoPtr)
{
    int i;

    InfoPtr->cmd_filename[0]    = 0;
    InfoPtr->num_input_files    = 0;
    InfoPtr->file_num           = 0;
    InfoPtr->dat_infilename     = NULL;
    InfoPtr->hd_infilename[0]   = 0;
    InfoPtr->mrk_infilename[0]  = 0;
    InfoPtr->xyz_infilename[0]  = 0;
    InfoPtr->dat_outfilename     = NULL;
    InfoPtr->hd_outfilename[0]  = 0;
    InfoPtr->mrk_outfilename[0] = 0;
    InfoPtr->xyz_outfilename[0] = 0;
    InfoPtr->storage_format     = 0;
    InfoPtr->input_datatype     = 0;
    InfoPtr->file_header_bytes  = 0;
    InfoPtr->trace_header_bytes = 0;
    InfoPtr->channel            = 0;
    InfoPtr->first_proc_samp    = 0L;
    InfoPtr->header_samps       = 0;
    InfoPtr->total_samps        = 0L;
    InfoPtr->first_trace        = -1L;
    InfoPtr->last_trace         = -1L;
    InfoPtr->total_traces       = 0L;
    InfoPtr->total_time         = 0.0;
    for (i=0; i<MAX_PROC_FUNCS; i++) InfoPtr->proc_order[i] = 0;
    for (i=0; i<MAX_PROC_FUNCS; i++) InfoPtr->proc_value[i] = 0.0;
    for (i=0; i<MAX_PROC_FUNCS; i++) InfoPtr->proc_set[i] = NULL;
    InfoPtr->num_proc           = 0;
    InfoPtr->num_gain_off       = 0;
    InfoPtr->num_gain_on        = 0;
    InfoPtr->amp_scale          = 0.0;
    InfoPtr->stack              = 0L;
    InfoPtr->lo_freq_cutoff     = -1.0;   /* MHz */
    InfoPtr->hi_freq_cutoff     = -1.0;   /* MHz */
    InfoPtr->preprocFFT         = TRUE;
    InfoPtr->amp_adjust         = INVALID_VALUE;
    InfoPtr->glob_bckgrnd_rem   = FALSE;
    InfoPtr->glob_forgrnd_rem   = FALSE;
    InfoPtr->wind_bckgrnd_rem   = 0L;
    InfoPtr->wind_forgrnd_rem   = 0L;
    InfoPtr->hsmooth            = 0L;
    InfoPtr->vsmooth            = 0L;
    InfoPtr->spatial_median     = 0L;
    InfoPtr->temporal_median    = 0L;
    InfoPtr->samp_slide         = 0;
    InfoPtr->trace_equalize     = -1;
    InfoPtr->inst_amp           = FALSE;
    InfoPtr->inst_pow           = FALSE;
    for (i=0; i<sizeof(InfoPtr->proc_hist); i++) InfoPtr->proc_hist[i] = 0;
    InfoPtr->created            = FALSE;
    InfoPtr->grid               = NULL;
    InfoPtr->num_cols           = 0L;
    InfoPtr->num_rows           = 0L;
}
/**************************** DeallocInfoStruct() ***************************/
/* Deallocate arrays in the information structure.
 *
 * Parameter list:
 *  struct ProcParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gprslice.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: March 21, 2000
 */
void DeallocInfoStruct (struct ProcParamInfoStruct *InfoPtr)
{
    int i;
    extern int Debug;

    if (Debug)
    {   printf("Start of DeallocInfoStruct(); InfoPtr->num_proc=%d",InfoPtr->num_proc);
    }

    for (i=0; i<InfoPtr->num_proc; i++)
    {   if (InfoPtr->proc_set[i] != NULL)
            free(InfoPtr->proc_set[i]);
    }
    if (Debug)
    {   puts("End of DeallocInfoStruct()");
    }
}
/***************************** GetCmdFileArgs() *****************************/
/* Get processing parameters from user from the DOS command line.
 * Valid file commands:
const char *GPR_PROC_CMDS[] =
{   "debug","batch","num_input_files","input_filelist[]","output_filelist[]",
    "channel","vsmooth","hsmooth","high_freq_cutoff","low_freq_cutoff",

    "preprocFFT","amp_adjust","glob_bckgrnd_rem","wind_bckgrnd_rem","amp_scale",
    "stack","spatial_median","temporal_median","num_gain_off","gain_off[]",

    "num_gain_on","gain_on[]","samp_slide","trace_equalize","inst_amp",
    "inst_pow","wind_forgrnd_rem","glob_forgrnd_rem",NULL,
} ;
 * Parameter list:
 *  struct ProcParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <stdio.h>, <stdlib.h>, <string.h>, "gprsproc.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, strlwr (non-ANSI), strupr (non-ANSI), Trimstr, strncpy,
 *        assert, atol.
 * Returns: 0 on success,
 *         >0 on error (offsets 2, 3, and 4 into GetParametersMsg[] strings).
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: December 30, 1996
 * Revisions: August 22, 1997; September 4, 1997; August 20, 1999;
 *            March 21, 2000; March 28, 2000;
 */
int GetCmdFileArgs (struct ProcParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];

    int found,i,num,itemp,have_string;
    int j;
    int next_proc         = 0;
    int dat_in_found      = FALSE;
    int dat_out_found     = FALSE;
    int fft_lo_set        = FALSE;
    int fft_hi_set        = FALSE;
    int gain_off_set      = -1;
    int gain_on_set       = -1;
    long ltemp;
    double dtemp;
    FILE *infile          = NULL;
    extern int Debug;
    extern const char *GPR_PROC_CMDS[];

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
        while (GPR_PROC_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_PROC_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_PROC_CMDS[] */
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
        {   printf("found=%d cp=%s\n",found,cp);
            getch();
        }
#endif
        switch (found)  /* cases depend on same order in GPR_PROC_CMDS[] */
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
                if (itemp >= 0)  InfoPtr->num_input_files = itemp;
                if (Debug) printf("num_input_files=%d\n",InfoPtr->num_input_files);
                break;
            case 3:                         /* input_filelist[] */
            {   char *cp_next = (char *)TrimStr(cp);
                int file_found = 0;

                /* number of files must be specified first */
                if (InfoPtr->num_input_files <= 0) break;

                /* allocate storage for filenames */
                InfoPtr->dat_infilename = (char **)malloc(InfoPtr->num_input_files * sizeof(char *));
                if (InfoPtr->dat_infilename == NULL)
                {   InfoPtr->num_input_files = 0;
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
                        InfoPtr->num_input_files = 0;
                        if (Debug) puts("\nmalloc failed for dat_infilename[]");
                        if (log_file) fprintf(log_file,"malloc failed for dat_infilename[]\n");
                        break;  /* out of for loop */
                    }
                }

                if (InfoPtr->num_input_files == 0) break; /* break out of case */

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
                    InfoPtr->num_input_files = 0;
                    for (j=0; j<InfoPtr->num_input_files; j++)
                        free(InfoPtr->dat_infilename[j]);
                    free(InfoPtr->dat_infilename);
                    InfoPtr->dat_infilename = NULL;
                    InfoPtr->num_input_files = 0;
                    dat_in_found = FALSE;
                }
                if (Debug)
                {    for (i=0; i<InfoPtr->num_input_files; i++)
                        printf("File %3d: %s\n",i,InfoPtr->dat_infilename[i]);
                }
                break;
            }
            case 4:
            {   char *cp_next = (char *)TrimStr(cp);
                int file_found = 0;

                /* number of files must be specified first */
                if (InfoPtr->num_input_files <= 0) break;

                /* allocate storage for filenames */
                InfoPtr->dat_outfilename = (char **)malloc(InfoPtr->num_input_files * sizeof(char *));
                if (InfoPtr->dat_outfilename == NULL)
                {   InfoPtr->num_input_files = 0;
                    if (Debug) puts("\nmalloc failed for dat_outfilename");
                    if (log_file) fprintf(log_file,"malloc failed for dat_outfilename\n");
                    break;      /* break out of case */
                }
                for (i=0; i<InfoPtr->num_input_files; i++)
                {   InfoPtr->dat_outfilename[i] = (char *)malloc(MAX_PATHLEN * sizeof(char));
                    if (InfoPtr->dat_outfilename[i] == NULL)
                    {   for (j=0; j<i; j++) free(InfoPtr->dat_outfilename[j]);
                        free(InfoPtr->dat_outfilename);
                        InfoPtr->dat_outfilename = NULL;
                        InfoPtr->num_input_files = 0;
                        if (Debug) puts("\nmalloc failed for dat_outfilename[]");
                        if (log_file) fprintf(log_file,"malloc failed for dat_outfilename[]\n");
                        break;  /* out of for loop */
                    }
                }

                if (InfoPtr->num_input_files == 0) break; /* break out of case */

                /* get the file names */
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (file_found < InfoPtr->num_input_files)
                        {   cp2 = strchr(cp_next,' '); /* look for seperator */
                            if (cp2 != NULL)
                            {   *cp2 = 0;    /* truncate */
                                strcpy(InfoPtr->dat_outfilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,InfoPtr->dat_outfilename[file_found]);
                                file_found++;
                                *cp2 = ' ';  /* reset separator */
                                /* TrimStr(cp_next); */
                            } else
                            {   strcpy(InfoPtr->dat_outfilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,InfoPtr->dat_outfilename[file_found]);
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
                dat_out_found = TRUE;
                if (file_found < InfoPtr->num_input_files)
                {   if (Debug) puts("\nfile_found < num_input_files");
                    if (log_file) fprintf(log_file,"GetCmdFileArgs(): files found < num_input_files\n");
                    InfoPtr->num_input_files = 0;
                    for (j=0; j<InfoPtr->num_input_files; j++)
                        free(InfoPtr->dat_outfilename[j]);
                    free(InfoPtr->dat_outfilename);
                    InfoPtr->dat_outfilename = NULL;
                    InfoPtr->num_input_files = 0;
                    dat_out_found = FALSE;
                }
                if (Debug)
                {    for (i=0; i<InfoPtr->num_input_files; i++)
                        printf("File %3d: %s\n",i,InfoPtr->dat_outfilename[i]);
                }
                break;
            }
            case 5:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->channel = itemp-1;
                if (Debug) printf("channel=%d\n",InfoPtr->channel);
                break;
            case 6:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->vsmooth = itemp;
                if (InfoPtr->vsmooth > 0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = VSMOOTH;
                    InfoPtr->proc_value[next_proc] = InfoPtr->vsmooth;
                    next_proc++;
                }
                if (Debug) printf("vsmooth=%d\n",InfoPtr->vsmooth);
                break;
            case 7:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->hsmooth = itemp;
                if (InfoPtr->hsmooth > 0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = HSMOOTH;
                    InfoPtr->proc_value[next_proc] = InfoPtr->hsmooth;
                    next_proc++;
                }
                if (Debug) printf("hsmooth=%d\n",InfoPtr->hsmooth);
                break;
            case 8:
                dtemp = atof(cp);
                if (dtemp >= 0 && fft_hi_set == FALSE)
                    InfoPtr->hi_freq_cutoff = dtemp * 1.0E06;
                fft_hi_set = TRUE;
                if (Debug) printf("hi_freq_cutoff=%g\n",InfoPtr->hi_freq_cutoff);
                if (InfoPtr->hi_freq_cutoff >= 0 && fft_lo_set)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = FFT_FILTER;
                    InfoPtr->proc_value[next_proc] = 2;
                    /* allocate storage for traces */
                    InfoPtr->proc_set[next_proc] = (double *)malloc(2 * sizeof(double));
                    if (InfoPtr->proc_set[next_proc] == NULL)
                    {   InfoPtr->hi_freq_cutoff = -1;
                        fft_hi_set = FALSE; /* clear flag for next time */
                        InfoPtr->proc_order[next_proc] = 0;
                        if (Debug) puts("\nmalloc failed for high FFT_FILTER");
                        break;  /* break out of case */
                    }
                    InfoPtr->proc_set[next_proc][0] = InfoPtr->lo_freq_cutoff;
                    InfoPtr->proc_set[next_proc][1] = InfoPtr->hi_freq_cutoff;
                    next_proc++;
                    fft_lo_set = FALSE;
                    fft_hi_set = FALSE;
                }
                break;
            case 9:
                dtemp = atof(cp);
                if (dtemp >= 0 && fft_lo_set == FALSE)
                    InfoPtr->lo_freq_cutoff = dtemp * 1.0E06;
                fft_lo_set = TRUE;
                if (Debug) printf("lo_freq_cutoff=%g\n",InfoPtr->lo_freq_cutoff);
                if (InfoPtr->lo_freq_cutoff >= 0 && fft_hi_set)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = FFT_FILTER;
                    InfoPtr->proc_value[next_proc] = 2;
                    /* allocate storage for traces */
                    InfoPtr->proc_set[next_proc] = (double *)malloc(2 * sizeof(double));
                    if (InfoPtr->proc_set[next_proc] == NULL)
                    {   InfoPtr->lo_freq_cutoff = -1;
                        fft_lo_set = FALSE; /* clear flag for next time */
                        InfoPtr->proc_order[next_proc] = 0;
                        if (Debug) puts("\nmalloc failed for low FFT_FILTER");
                        break;  /* break out of case */
                    }
                    InfoPtr->proc_set[next_proc][0] = InfoPtr->lo_freq_cutoff;
                    InfoPtr->proc_set[next_proc][1] = InfoPtr->hi_freq_cutoff;
                    next_proc++;
                    fft_lo_set = FALSE;
                    fft_hi_set = FALSE;
                }
                break;


            case 10:
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
                if (Debug) printf("preprocFFT=%d\n",InfoPtr->preprocFFT);
                break;
            case 11:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->amp_adjust = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->amp_adjust = atof(cp);
                }

                if (InfoPtr->amp_adjust != INVALID_VALUE)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = AMP_ADJ;
                    InfoPtr->proc_value[next_proc] = InfoPtr->amp_adjust;
                    next_proc++;
                }
                if (Debug) printf("amp_adjust=%g\n",InfoPtr->amp_adjust);
                break;
            case 12:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)
                    InfoPtr->glob_bckgrnd_rem = FALSE;
                else
                {   InfoPtr->glob_bckgrnd_rem = TRUE;
                    assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = GLOB_REM;
                    InfoPtr->proc_value[next_proc] = InfoPtr->glob_bckgrnd_rem;
                    next_proc++;
                }
                if (Debug) printf("glob_bckgrnd_rem=%g\n",InfoPtr->glob_bckgrnd_rem);
                break;
            case 13:
                itemp = atoi(cp);
                if (itemp <= 1)
                    InfoPtr->wind_bckgrnd_rem = 0;
                else
                {   if (!(itemp % 2)) itemp++;    /* force to be odd number */
                    InfoPtr->wind_bckgrnd_rem = itemp;
                    assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = WIND_REM;
                    InfoPtr->proc_value[next_proc] = InfoPtr->wind_bckgrnd_rem;
                    next_proc++;
                }
                if (Debug) printf("wind_bckgrnd_rem=%g\n",InfoPtr->wind_bckgrnd_rem);
                break;
            case 14:
                InfoPtr->amp_scale = atof(cp);
                /* if (InfoPtr->amp_scale < 0.0) InfoPtr->amp_scale = 0.0; */
                if (InfoPtr->amp_scale != 0.0 && InfoPtr->amp_scale != 1.0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = AMP_SCALE;
                    InfoPtr->proc_value[next_proc] = InfoPtr->amp_scale;
                    next_proc++;
                }
                if (Debug) printf("amp_scale=%g\n",InfoPtr->amp_scale);
                break;
            case 15:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->stack = itemp;
                if (InfoPtr->stack > 0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = STACK;
                    InfoPtr->proc_value[next_proc] = InfoPtr->stack;
                    next_proc++;
                }
                if (Debug) printf("stack=%g\n",InfoPtr->stack);
                break;
            case 16:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->spatial_median = itemp;
                if (InfoPtr->spatial_median > 0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = SPAT_MED;
                    if ((InfoPtr->spatial_median % 2) == 0)
                        InfoPtr->spatial_median++; /* make odd if even */
                    InfoPtr->proc_value[next_proc] = InfoPtr->spatial_median;
                    next_proc++;
                }
                if (Debug) printf("spatial_median=%g\n",InfoPtr->spatial_median);
                break;
            case 17:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->temporal_median = itemp;
                if (InfoPtr->temporal_median > 0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = TEMP_MED;
                    if ((InfoPtr->temporal_median % 2) == 0)
                        InfoPtr->temporal_median++; /* make odd if even */
                    InfoPtr->proc_value[next_proc] = InfoPtr->temporal_median;
                    next_proc++;
                }
                if (Debug) printf("temporal_median=%g\n",InfoPtr->temporal_median);
                break;
            case 18:
                itemp = atoi(cp);
                if (itemp >= 2 || itemp ==0)  InfoPtr->num_gain_off = itemp;
                if (Debug) printf("num_gain_off = %d\n",InfoPtr->num_gain_off);
                if ( (InfoPtr->num_gain_off >= 2) && (gain_off_set < 0) )
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = RG_OFF;
                    InfoPtr->proc_value[next_proc] = InfoPtr->num_gain_off;
                    gain_off_set = next_proc;  /* record postion */
                    next_proc++;
                }
                if (Debug) printf("num_gain_off=%g\n",InfoPtr->num_gain_off);
                break;
            case 19:
            {   char *cp_next = TrimStr(cp);
                int off_found = 0;

                /* number of points must be specified first */
                if (gain_off_set < 0) break;  /* break out of case */

                /* allocate storage for traces */
                InfoPtr->proc_set[gain_off_set] = (double *)malloc(InfoPtr->num_gain_off * sizeof(double));
                if (InfoPtr->proc_set[gain_off_set] == NULL)
                {   InfoPtr->num_gain_off = 0;
                    next_proc--;    /* remove from list */
                    InfoPtr->proc_order[next_proc] = 0;
                    InfoPtr->proc_value[next_proc] = 0;
                    gain_off_set = -1; /* clear set value for next time */
                    if (Debug) puts("\nmalloc failed for gain_off; next_proc decremented.");
                    break;  /* break out of case */
                }
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (off_found < InfoPtr->num_gain_off)
                        {   InfoPtr->proc_set[gain_off_set][off_found] = atof(cp_next);
                            off_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (off_found < InfoPtr->num_gain_off)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {   break;      /* break out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            if (strchr(str,'='))
                            {   have_string = TRUE;
                                break;  /* break out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = TrimStr(str);
                            }
                        }
                    } else
                        break;          /* break out of while (1) */
                }
                if (off_found < InfoPtr->num_gain_off)
                {   if (Debug) puts("\noff_found < num_gain_off");
                    free(InfoPtr->proc_set[gain_off_set]);
                    InfoPtr->proc_set[gain_off_set] = NULL;
                    InfoPtr->num_gain_off = 0;
                    next_proc--;    /* remove from list */
                    InfoPtr->proc_order[next_proc] = 0;
                    InfoPtr->proc_value[next_proc] = 0;
                }
                gain_off_set = -1; /* clear set value for next time */
                break;
            }
            case 20:
                itemp = atoi(cp);
                if (itemp >= 2 || itemp ==0)  InfoPtr->num_gain_on = itemp;
                if (Debug) printf("num_gain_on = %d\n",InfoPtr->num_gain_on);
                if ( (InfoPtr->num_gain_on >= 2) && (gain_on_set < 0) )
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = RG_ON;
                    InfoPtr->proc_value[next_proc] = InfoPtr->num_gain_on;
                    gain_on_set = next_proc;  /* record postion */
                    next_proc++;
                if (Debug) printf("num_gain_on=%g\n",InfoPtr->num_gain_on);
                }
                break;
            case 21:
            {   char *cp_next = TrimStr(cp);
                int on_found = 0;

                /* number of points must be specified first */
                if (gain_on_set < 0) break;  /* break out of case */

                /* allocate storage for traces; gain_off_set = next_proc */
                InfoPtr->proc_set[gain_on_set] = (double *)malloc(InfoPtr->num_gain_on * sizeof(double));
                if (InfoPtr->proc_set[gain_on_set] == NULL)
                {   InfoPtr->num_gain_on = 0;
                    next_proc--;    /* remove from list */
                    InfoPtr->proc_order[next_proc] = 0;
                    InfoPtr->proc_value[next_proc] = 0;
                    gain_on_set = -1; /* clear set value for next time */
                    if (Debug) puts("\nmalloc failed for gain_on; next_proc decremented.");
                    break;  /* break out of case */
                }
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (on_found < InfoPtr->num_gain_on)
                        {   InfoPtr->proc_set[gain_on_set][on_found] = atof(cp_next);
                            on_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (on_found < InfoPtr->num_gain_on)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {   break;      /* break out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            if (strchr(str,'='))
                            {   have_string = TRUE;
                                break;  /* break out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = TrimStr(str);
                            }
                        }
                    } else
                        break;          /* break out of while (1) */
                }
                if (on_found < InfoPtr->num_gain_on)
                {   if (Debug) puts("\noff_found < num_gain_on");
                    free(InfoPtr->proc_set[gain_on_set]);
                    InfoPtr->proc_set[gain_on_set] = NULL;
                    InfoPtr->proc_value[gain_on_set] = 0;
                    InfoPtr->num_gain_on = 0;
                    next_proc--;    /* remove from list */
                    InfoPtr->proc_order[next_proc] = 0;
                    InfoPtr->proc_value[next_proc] = 0;
                }
                gain_on_set = -1; /* clear set value for next time */
                break;
            }
            case 22:
                InfoPtr->samp_slide = atoi(cp);
                if (InfoPtr->samp_slide != 0)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = SAMP_SLIDE;
                    InfoPtr->proc_value[next_proc] = InfoPtr->samp_slide;
                    next_proc++;
                }
                if (Debug) printf("samp_slide = %d\n",InfoPtr->samp_slide);
                break;
            case 23:
                InfoPtr->trace_equalize = atoi(cp);
                if (InfoPtr->trace_equalize != -1)
                {   assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = TRACE_EQUAL;
                    InfoPtr->proc_value[next_proc] = InfoPtr->trace_equalize;
                    next_proc++;
                }
                if (Debug) printf("trace_equalize = %d\n",InfoPtr->trace_equalize);
                break;
            case 24:
                if (InfoPtr->inst_pow == TRUE)
                {   InfoPtr->inst_amp = FALSE;
                    break;
                }
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
                if (itemp == 0)
                {   InfoPtr->inst_amp = FALSE;
                }  else
                {   InfoPtr->inst_amp = TRUE;
                    assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = INST_AMP;
                    InfoPtr->proc_value[next_proc] = InfoPtr->inst_amp;
                    next_proc++;
                }
                if (Debug) printf("inst_amp = %d\n",InfoPtr->inst_amp);
                break;
            case 25:
                if (InfoPtr->inst_amp == TRUE)
                {   InfoPtr->inst_pow = FALSE;
                    break;
                }
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
                if (itemp == 0)
                {   InfoPtr->inst_pow = FALSE;
                }  else
                {   InfoPtr->inst_pow = TRUE;
                    assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = INST_POW;
                    InfoPtr->proc_value[next_proc] = InfoPtr->inst_pow;
                    next_proc++;
                }
                if (Debug) printf("inst_pow = %d\n",InfoPtr->inst_pow);
                break;
            case 26:
                itemp = atoi(cp);
                if (itemp <= 1)
                    InfoPtr->wind_forgrnd_rem = 0;
                else
                {   if (!(itemp % 2)) itemp++;    /* force to be odd number */
                    InfoPtr->wind_forgrnd_rem = itemp;
                    assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = WIND_DIFF;
                    InfoPtr->proc_value[next_proc] = InfoPtr->wind_forgrnd_rem;
                    next_proc++;
                }
                if (Debug) printf("wind_forgrnd_rem=%g\n",InfoPtr->wind_forgrnd_rem);
                break;
            case 27:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)
                    InfoPtr->glob_forgrnd_rem = FALSE;
                else
                {   InfoPtr->glob_forgrnd_rem = TRUE;
                    assert(next_proc < MAX_PROC_FUNCS);
                    InfoPtr->proc_order[next_proc] = GLOB_DIFF;
                    InfoPtr->proc_value[next_proc] = InfoPtr->glob_forgrnd_rem;
                    next_proc++;
                }
                if (Debug) printf("glob_forgrnd_rem=%g\n",InfoPtr->glob_forgrnd_rem);
                break;
            default:
                break;
        }
    }
    InfoPtr->num_proc = next_proc;  /* indexed from 0 */
    fclose(infile);
    if (!dat_out_found)  return 3;
    if (!dat_in_found)   return 4;
    return 0;
}
/**************************** DisplayParameters() ***************************/
/* Display parameters to CRT.  Also calculates approximate storage requirements.
 *
 * Parameter list:
 *  struct ProcParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <dos.h>, "gprsproc.h".
 * Calls: printf, puts, _dos_getdate, _dos_gettime.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: January 2, 1997
 * Revisions: August 22, 1997; August 25, 1997; August 29, 1997; \
 *            September 4, 1997; August 10, 1999; August 20, 1999;
 *            March 22, 2000; March 29, 2000; March 31, 2000;
 */
void DisplayParameters (struct ProcParamInfoStruct *InfoPtr)
{
    char str[80];
    int i,num,len;
    int lines = 0, maxlines = 21;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    extern int Debug,Batch,ANSI_THERE;

    if (Debug) puts("DisplayParameters()");
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    /* if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); */ /* red on black */
    if (ANSI_THERE) printf("%c%c33;40m",0x1B,0x5B); /* yellow on black */
    printf("\nGPR_PROC v. %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_PROC_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

    printf("cmd_filename = %s\n",strupr(InfoPtr->cmd_filename));
    printf("input data files [%d]:\n",InfoPtr->num_input_files);
    lines = 4;
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

    printf("output data files [%d]:\n",InfoPtr->num_input_files);
    len = 0;
    num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {  if (strlen(InfoPtr->dat_outfilename[i]) > len)
           len = strlen(InfoPtr->dat_outfilename[i])+1;
    }
    num = 80 / len;
    if (num < 1) num = 1;
    if (num > 5) num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   printf("%s ",InfoPtr->dat_outfilename[i]);
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

    printf("For DZT files only: channel = %d \n",InfoPtr->channel+1);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    if (ANSI_THERE) printf("%c%c36;40m",0x1B,0x5B); /* cyan on black */
    puts("USER INPUT FOR PROCESSING PARAMETERS:");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->num_proc <=0 ) puts("No data processing was requested.");
    for (num=0; num<InfoPtr->num_proc; num++)
    {   switch(InfoPtr->proc_order[num])
        {   case FFT_FILTER:
                printf("Low frequency cutoff = %9.3f MHz\n",InfoPtr->proc_set[num][0]/1.E06);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                printf("High frequency cutoff = %9.3f MHz\n",InfoPtr->proc_set[num][1]/1.E06);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                printf("preprocFFT = %s\n",InfoPtr->preprocFFT ? "TRUE" : "FALSE");
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case AMP_ADJ:
                printf("Trace amplitudes will be adjusted (shifted) by %g\n",
                    InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case GLOB_REM:
                printf("Global background removal = TRUE\n");
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case GLOB_DIFF:
                printf("Global foreground removal = TRUE\n");
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case WIND_REM:
                printf("Windowed background removal = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case WIND_DIFF:
                printf("Windowed foreground removal = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case HSMOOTH:
                printf("Horizontal (spatial) smoothing value = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case VSMOOTH:
                printf("Vertical (temporal) smoothing value = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case RG_OFF:
                printf("Gain-off points = %d [",(int)InfoPtr->proc_value[num]);
                for (i=0; i<InfoPtr->proc_value[num]; i++)
                    printf(" %g",InfoPtr->proc_set[num][i]);
                printf("] db\n");
                lines ++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case RG_ON:
                printf("Gain-on points = %d [",(int)InfoPtr->proc_value[num]);
                for (i=0; i<InfoPtr->proc_value[num]; i++)
                    printf(" %g",InfoPtr->proc_set[num][i]);
                printf("] db\n");
                lines ++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case AMP_SCALE:
                printf("Amplitude scaling multiplier = %g\n",InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case STACK:
                printf("Trace stacking = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case TEMP_MED:
                printf("Temperal (vertical) median filter value = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case SPAT_MED:
                printf("Spatial (horizontal) median filter value = %d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case SAMP_SLIDE:
                printf("Sample slide = %d [positions move %s]\n",(int)InfoPtr->proc_value[num],
                        InfoPtr->proc_value[num] < 0 ? "up" : "down");
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case TRACE_EQUAL:
                printf("Traces will be equalized to trace #%d\n",(int)InfoPtr->proc_value[num]);
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case INST_AMP:
                printf("Trace values will be converted to instantaneous amplitude\n");
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case INST_POW:
                printf("Trace values will be converted to instantaneous power\n");
                lines++;
                if (!Batch && lines>=maxlines)
                {   printf("Press a key to continue ...");
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
                break;
            case DECON: case MIGRATE: case SMOOTH2D:
                break;
            default:
                break;
        }
    }

    /* Issue messages */
    if (Debug)       puts("In TESTing mode.");
    else if (Batch)  puts("In Batch processing mode.");
}
/*************************** GetGprDataAsGrid() *****************************/
/* Get the data/info from the appropriate files.
 *
 * NOTE: The "created" member of struct ProcParamInfoStruct restricts the
 *       application of the struct to one data set and one grid at a time.
 *       To read multiple data sets and allocate multiple grids, either
 *       de-allocate the first one OR declare multiple structs to hold the
 *       information and pointers.
 *
 * Method:
 *   1. Declare variables
 *   2. De-allocate storage if command==FUNC_DESTROY and return, else
 *   3. Test for balanced calls
 *   4. Determine GPR file storage format.
 *   5. Get information from the appropriate "information" files.
 *   6. Verify valid parameters and ranges.
 *   7. Allocate storage for grid[][]
 *   8. Copy disk file data into grid[][]
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct ProcParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "gprsproc.h".
 * Calls: strcpy, strstr, strupr, strcmp, malloc, free, printf, fprintf, puts,
 *        ReadOneDztHeader, GetSsHdFile, PrintSsHdInfo, ReadSegyReelHdr,
 *        PrintSegyReelHdr, GetDztChSubGrid8, GetDztChSubGrid16, GetGprSubGrid,
 *        TrimStr.
 * Returns: 0 on success,
 *         >0 on error.  #14 is checked by calling routine
 *
const char *GetGprDataAsGridMsg[] =
{   "GetGprDataAsGrid(): No errors.",
    "GetGprDataAsGrid() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetGprDataAsGrid() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "GetGprFileType() ERROR: Problem getting information about input data file.",
    "ReadOneDztHeader() ERROR: Problem getting information about input data file.",

    "ReadOneDztHeader() ERROR: Invalid channel selection for GSSI DZT file.",
    "GetSsHdFile() ERROR: Problem getting info from S&S HD file.",
    "ReadSegyReelHdr() ERROR: Problem getting info from SGY file.",
    "GetGprDataAsGrid() ERROR: No recognizable data/info input files specified.",
    "GetGprDataAsGrid() ERROR: Invalid input data element type .",

    "GetGprDataAsGrid() ERROR: Less than one trace in file.",
    "GetGprDataAsGrid() ERROR: Less than two samples per trace.",
    "GetGprDataAsGrid() ERROR: nanoseconds per sample is <= 0.0.",
    "GetGprDataAsGrid() ERROR: trace_header_size must be a multiple of sample size.",
    "GetGprDataAsGrid() ERROR: Not able to allocate storage for grid.",

    "GetGprDataAsGrid() ERROR: Problem getting data from input file.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: January 30, 1996
 * Revisions:  September 4, 1997;
 *             March 22, 2000; March 29, 2000;
 */
int GetGprDataAsGrid (int command,struct ProcParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/
    int  file_num;
    long i,j,num,itemp;        /* scratch variables and counters */
    long ptr_size;             /* size of element in first dimension of grid */
    long grid_col_size;        /* number of bytes in one trace */
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

    if (Debug) puts("Start of GetGprDataAsGrid()");

/***** Deallocate storage *****/
    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->created) return 1;
        for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        InfoPtr->created = FALSE;
        return 0;
    }

/***** Test for balanced calls *****/
    if (InfoPtr->created) return 2;

/***** Determine GPR file storage format and essential info *****/
    file_num = InfoPtr->file_num;
    itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                &samples_per_trace,&num_channels,&num_traces,
                &input_datatype,&total_time,&file_type,
                InfoPtr->dat_infilename[file_num]);
    if (itemp > 0)
    {   printf("%s\n",GetGprFileTypeMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
        return 3;
    }
    if (file_type == MOD_DZT)  file_type = DZT;
    InfoPtr->storage_format = file_type;

/***** Get necessary information from info file/header *****/
    if (InfoPtr->storage_format == DZT)
    {   int channel,header_size;
        struct DztHdrStruct DztHdr;                /* from gpr_io.h */
        extern const char *ReadOneDztHeaderMsg[];  /* from gpr_io.h */

        /* get first DZT header in file */
        channel = 0;
        header_size = 0;
        itemp = 0;
        itemp = ReadOneDztHeader(InfoPtr->dat_infilename[file_num],&num_hdrs,
                                 &(InfoPtr->total_traces),channel,
                                 &header_size,&DztHdr);
        if (itemp > 0)
        {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
            if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                return 4;
        }
        if (InfoPtr->channel >= num_hdrs) return 5;
        if (header_size < 1024)
        {   if (log_file) fprintf(log_file,"This DZT header is not consistent with version 5.x SIR-10A software.\n");
        }

        /* get selected header (if different than first) */
        if (InfoPtr->channel > 0)
        {   itemp = ReadOneDztHeader(InfoPtr->dat_infilename[file_num],&num_hdrs,
                                 &(InfoPtr->total_traces),InfoPtr->channel,
                                 &header_size,&DztHdr);
            if (itemp > 0)
            {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
                if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                    return 4;
            }
        }

        /* assign values to info structure */
        if      (DztHdr.rh_bits ==  8) InfoPtr->input_datatype = -1;
        else if (DztHdr.rh_bits == 16) InfoPtr->input_datatype = -2;
        else if (DztHdr.rh_bits == 32) InfoPtr->input_datatype = -3;
        InfoPtr->last_trace      = InfoPtr->total_traces - 1;
        InfoPtr->first_trace     = 0;
        InfoPtr->total_samps     = DztHdr.rh_nsamp;
        InfoPtr->total_time      = DztHdr.rh_range;
        InfoPtr->ns_per_samp     = DztHdr.rh_range/(DztHdr.rh_nsamp-1);
        InfoPtr->first_proc_samp = 2;
        InfoPtr->header_samps    = 0;
        if (Debug)
        {   printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            printf("num_hdrs = %d\n***paused***\n",num_hdrs);
            getch();
        }

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
        {   if (Debug) printf("%s\n",GetSsHdFileMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetSsHdFileMsg[itemp]);
            return 6;
        }
        if (Debug)
        {   PrintSsHdInfo("stdout",InfoPtr->hd_infilename,&HdInfo);
            if (!getch()) getch();
        }

        InfoPtr->input_datatype  = 2;
        InfoPtr->total_traces    = HdInfo.num_traces;
        InfoPtr->last_trace      = InfoPtr->total_traces - 1;
        InfoPtr->first_trace     = 0;
        InfoPtr->total_samps     = HdInfo.num_samples;
        InfoPtr->total_time      = HdInfo.total_time_ns;
        InfoPtr->ns_per_samp     = (double)HdInfo.total_time_ns/(HdInfo.num_samples-1);
        InfoPtr->first_proc_samp = 0;
        InfoPtr->header_samps    = InfoPtr->trace_header_bytes/2;  /* 128/2-byte shorts */

        if (Debug)
        {   printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n***paused***\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            getch();
        }

    } else if (InfoPtr->storage_format == SGY)
    {   char  *cp,cmdstr[30];
        char string[40][80];    /* should have 3200 continuous bytes */
        int swap_bytes,str_num,max_records,num,found;
        int comment_size,record_length,samp_bytes;  /* bytes */
        long num_traces;
        struct SegyReelHdrStruct hdr;            /* from gpr_io.h */
        extern const char *ReadSegyReelHdrMsg[]; /* from gpr_io.h */

        itemp = ReadSegyReelHdr(InfoPtr->dat_infilename[file_num],&swap_bytes,
                                &num_traces,&hdr);
        if (itemp > 0)
        {   if (log_file) fprintf(log_file,"%s\n",ReadSegyReelHdrMsg[itemp]);
            return 7;
        }
        if (Debug) PrintSegyReelHdr(swap_bytes,num_traces,"stdout",
                                    InfoPtr->dat_infilename[file_num],&hdr);

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
        InfoPtr->last_trace     = InfoPtr->total_traces - 1;
        InfoPtr->first_trace     = 0;
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
        InfoPtr->first_proc_samp = 0;
        InfoPtr->header_samps    = InfoPtr->trace_header_bytes/samp_bytes;

        if (Debug)
        {   printf("total_traces = %u total_samps = %u total_time = %g\n",
                InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
            printf("ns_per_samp=%g  datatype=%d  first_proc_samp=%d  header_samps=%d\n***paused***\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp,
                InfoPtr->header_samps);
            getch();
        }

    } else  /* unknown or unspecified storage format */
    {   return 8;
    }

/***** Finish initializing variables and verifying valid ranges *****/
    if (Debug) puts("Start of initialize and check section");
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
            return 9;
            /* break;   ** unreachable code */
    }



    if (InfoPtr->total_traces < 1)   return 10;
    if (InfoPtr->total_samps < 3)    return 11;
    if (InfoPtr->ns_per_samp <= 0.0) return 12;

    /* Verify whole number of samples in each trace header */
    if (InfoPtr->trace_header_bytes > 0)
    {   itemp = InfoPtr->trace_header_bytes/samp_bytes;  /* truncation expected */
        if ((itemp * samp_bytes) != InfoPtr->trace_header_bytes)  return 13;
    }

    /* Assign number of columns and rows in the grid */
    InfoPtr->num_rows = InfoPtr->header_samps + InfoPtr->total_samps;
    InfoPtr->num_cols = InfoPtr->last_trace - InfoPtr->first_trace + 1;

    for (num=0; num<InfoPtr->num_proc; num++)
    {   /* Get "base" trace for trace equalization */
        if (InfoPtr->proc_order[num] == TRACE_EQUAL)
        {   if (InfoPtr->proc_value[num] == -2)
                InfoPtr->proc_value[num] = (InfoPtr->last_trace-InfoPtr->first_trace)/2;
            if (InfoPtr->proc_value[num] == -3)
                InfoPtr->proc_value[num] =  InfoPtr->last_trace;
            if (InfoPtr->proc_value[num] > InfoPtr->last_trace)
                InfoPtr->proc_value[num] =  InfoPtr->last_trace;
            if (InfoPtr->proc_value[num] < -3)
                InfoPtr->proc_value[num] =  InfoPtr->first_trace;
            /* else trace_equalize = trace number */
        }
        /* Limit horizontal and vertical sliding windows to half of relavent length */
        if (InfoPtr->proc_order[num] == HSMOOTH)
        {   if (InfoPtr->proc_value[num] > (InfoPtr->num_cols/2 - 1))
                InfoPtr->proc_value[num] = InfoPtr->num_cols/2 - 1;
        }
        if (InfoPtr->proc_order[num] == VSMOOTH)
        {   if (InfoPtr->proc_value[num] > (InfoPtr->total_samps/2 - 1))
                InfoPtr->proc_value[num] = InfoPtr->total_samps/2 - 1;
        }

        /* Limit frequencies to within range of data */
        if (InfoPtr->proc_order[num] == FFT_FILTER)
        {   Nyq_freq  = 1.0E09 / (InfoPtr->ns_per_samp*2.0);
            fund_freq = 1.0E09 / ( (InfoPtr->total_samps-1) * (InfoPtr->ns_per_samp) );
            /* proc_set[num][0] = low;  proc_set[num][1] = hi */
            if (InfoPtr->proc_set[num][0] <= 0.0)
                InfoPtr->proc_set[num][0] = fund_freq;   /* no lo-cut filter */
            if (InfoPtr->proc_set[num][0] < fund_freq)
                InfoPtr->proc_set[num][0] = fund_freq;   /* no lo-cut filter */
            if (InfoPtr->proc_set[num][1] <= 0.0)
                InfoPtr->proc_set[num][1] = Nyq_freq;    /* no hi-cut filter */
            if (InfoPtr->proc_set[num][1] > Nyq_freq)
                InfoPtr->proc_set[num][1] = Nyq_freq;    /* no hi-cut filter */
            if (InfoPtr->proc_set[num][0] > InfoPtr->proc_set[num][1])
            {   InfoPtr->proc_set[num][0] = fund_freq;   /* no high-pass filter */
                InfoPtr->proc_set[num][1] = Nyq_freq;    /* no low-pass filter */
            }
        }

        /* If sliding window longer than data set, select global instead */
        if (InfoPtr->proc_order[num] == WIND_REM)
        {   if (InfoPtr->proc_value[num] > (InfoPtr->last_trace - InfoPtr->first_trace + 1))
            {   InfoPtr->proc_order[num] = GLOB_REM;
                InfoPtr->proc_value[num] = TRUE;
            }
        }

        /* Limit sliding window to length of data set */
        if (InfoPtr->proc_order[num] == WIND_DIFF)
        {   if (InfoPtr->proc_value[num] > (InfoPtr->last_trace - InfoPtr->first_trace + 1))
            {   InfoPtr->proc_value[num] = InfoPtr->last_trace - InfoPtr->first_trace + 1;
            }
        }

        /* check sample sliding ranges */
        if (InfoPtr->proc_order[num] == SAMP_SLIDE)
        {   if (InfoPtr->proc_value[num] > (InfoPtr->total_samps-1))
                InfoPtr->proc_value[num] = InfoPtr->total_samps-1;
            if (InfoPtr->proc_value[num] < -(InfoPtr->total_samps-1))
                InfoPtr->proc_value[num] = -(InfoPtr->total_samps-1);
        }

        /* Check range gain values */
        if (InfoPtr->proc_order[num] == RG_ON)
        {   if (InfoPtr->proc_value[num] == 1 || InfoPtr->proc_value[num] < 0)
            {   InfoPtr->proc_value[num] = 0;
            }
            if (InfoPtr->proc_value[num] >= 2 && InfoPtr->proc_set[num] == NULL)
                InfoPtr->proc_value[num] = 0;
        }
        if (InfoPtr->proc_order[num] == RG_OFF)
        {   if (InfoPtr->proc_value[num] == 1 || InfoPtr->proc_value[num] < 0)
            {   InfoPtr->proc_value[num] = 0;
            }
            if (InfoPtr->proc_value[num] >= 2 && InfoPtr->proc_set[num] == NULL)
                InfoPtr->proc_value[num] = 0;
        }
    }

/***** Allocate storage for grid[][] with same type as input data sets *****/
    grid_col_size = InfoPtr->num_rows * samp_bytes;
    InfoPtr->grid = (void **)malloc((size_t)(InfoPtr->num_cols * ptr_size));
    if (InfoPtr->grid)
    {   for (i=0; i<InfoPtr->num_cols; i++)
        {   InfoPtr->grid[i] = (void *)malloc((size_t)grid_col_size);
            if (InfoPtr->grid[i] == NULL)
            {   for (j=0; j<i; j++) free(InfoPtr->grid[j]);
                free(InfoPtr->grid);
                InfoPtr->grid = NULL;
                return 14;
            }
        }
    } else
    {   return 14;
    }

/***** Get the data *****/
    if (Debug) puts("At start of get data section");
    itemp = 0;
    switch (InfoPtr->storage_format)
    {   case DZT:
        {   extern const char *GetDztChSubGrid8Msg[];   /* from gpr_io.h */
            extern const char *GetDztChSubGrid16Msg[];  /* from gpr_io.h */

            if (InfoPtr->input_datatype == -1)
            {   itemp = GetDztChSubGrid8(InfoPtr->dat_infilename[file_num],InfoPtr->channel,
                        InfoPtr->first_trace,InfoPtr->num_cols,InfoPtr->num_rows,
                        (unsigned char **)(InfoPtr->grid));
                if (Debug) printf("\n%s\n",GetDztChSubGrid8Msg[itemp]);
                if ((itemp > 0) && log_file)
                    fprintf(log_file,"%s\n",GetDztChSubGrid8Msg[itemp]);
            } else if (InfoPtr->input_datatype == -2)
            {   itemp = GetDztChSubGrid16(InfoPtr->dat_infilename[file_num],InfoPtr->channel,
                        InfoPtr->first_trace,InfoPtr->num_cols,InfoPtr->num_rows,
                        (unsigned short **)(InfoPtr->grid));
                if (Debug) printf("\n%s\n",GetDztChSubGrid16Msg[itemp]);
                if ((itemp > 0) && log_file)
                    fprintf(log_file,"%s\n",GetDztChSubGrid16Msg[itemp]);
            } else
            {   if (Debug) puts("\nSorry, only 8-bit and 16-bit GSSI data supported at this time.");
                if (log_file) fprintf(log_file,"Sorry, only 8-bit and 16-bit GSSI data supported at this time.");
                itemp = 1;
            }
            break;
        }
        case DT1:  case SGY:
        {   extern const char *GetGprSubGridMsg[];

            itemp = GetGprSubGrid(InfoPtr->dat_infilename[file_num],InfoPtr->input_datatype,
                         InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes,
                         InfoPtr->first_trace,InfoPtr->total_samps,
                         InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->grid);
            if (Debug) printf("\n%s\n",GetGprSubGridMsg[itemp]);
            if ((itemp > 0) && log_file)
                fprintf(log_file,"%s\n",GetGprSubGridMsg[itemp]);
            break;
        }
    }
    if (itemp > 0)  /* problem getting data */
    {   /* deallocate storage and return error code */
        for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return 15;
    } else
    {   /* flag structure and return success */
        InfoPtr->created = TRUE;
        return 0;
    }
}
/***************************** ProcessGprData() *****************************/
/* This is the driver routine for processing the GPR data. All data are
 * converted to doubles before processing and back to native format after
 * processing.
 *
 * NOTES: Maximum trace length = 2 exp 16 = 65536 samples (64 K).
 *
 * Parameter list:
 *  struct ProcParamInfoStruct *ParamInfoPtr - address of information structure
 *
 * Requires: <float.h>, <math.h>, <stdlib.h>, <string.h>, "jl_util1.h", "gprsproc.h".
 * Calls: fprintf,
 *         FftFilterGridTraces, RemGridGlobBckgrnd, RemGrdWindBckgrnd,
 *         SmoothGridHorizontally, SmoothGridVertically, AdjustMeanGridTraces,
 *         RescaleGrid, StackGrid, ApplyGridTempMedFilter, InstAtribGridTraces,
 *         ChangeGridRangeGain, ApplyGridSpatMedFilter.
 * Returns:  0 on success, or
 *          >0 if an error has occurred.
 *          <0 if warning (duplicate in/out filename)
 *
const char *ProcessGprDataMsg[] =
{   "ProcessGprData(): No errors.",
    "ProcessGprData() ERROR: Processing function reported error. See log file.",
    "ProcessGprData() ERROR: Unable to allocate storage to convert data to doubles.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: January 13, 1997
 * Revisions: August 21, 1997; August 29, 1997; September 4, 1997;
 *            August 19, 1999; August 26, 1999;
 *            March 22, 2000;
 */
int ProcessGprData (struct ProcParamInfoStruct *InfoPtr)
{
    int    i,j,itemp;
    int    datatype;
    double **dgrid;
    extern FILE *log_file;
    extern const char *ProcessGprDataMsg[];
    extern const char *ApplyGridSpatMedFilterMsg[];
    extern const char *ApplyGridTempMedFilterMsg[];
    extern const char *ChangeGridRangeGainMsg[];
    extern const char *FftFilterGridTracesMsg[];
    extern const char *InstAttribGridTracesMsg[];
    extern const char *RemGridGlobBckgrndMsg[];
    extern const char *RemGridWindBckgrndMsg[];
    extern const char *SmoothGridHorizontallyMsg[];
    extern const char *SmoothGridVerticallyMsg[];
    extern const char *StackGridMsg[];

    /* Convert to doubles for processing */
    /* allocate storage */
    dgrid = (double **)malloc(InfoPtr->num_cols * sizeof(double *));
    if (dgrid == NULL)
    {   return 2;
    } else
    {   for (i=0; i<InfoPtr->num_cols; i++)
        {   dgrid[i] = (double *)malloc(InfoPtr->num_rows * sizeof(double));
            if (dgrid[i] == NULL)
            {   for (j=0; j<i; j++) free(dgrid[j]);
                free(dgrid);
                dgrid = NULL;
                return 2;
            }
        }
    }
    switch (InfoPtr->input_datatype)
    {   case CHAR:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((char**)InfoPtr->grid)[i][j];
                }
            }
            break;
        case UCHAR:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((unsigned char**)InfoPtr->grid)[i][j];
                    dgrid[i][j] -= 128.0;
                }
            }
            break;
        case SHORT:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((short**)InfoPtr->grid)[i][j];
                }
            }
            break;
        case USHORT:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((unsigned short**)InfoPtr->grid)[i][j];
                    dgrid[i][j] -= 32768.0;
                }
            }
            break;
        case USHORT12:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((unsigned short**)InfoPtr->grid)[i][j];
                    dgrid[i][j] -= 2048.0;
                }
            }
            break;
        case LONG:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((long**)InfoPtr->grid)[i][j];
                }
            }
            break;
        case ULONG:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((unsigned long**)InfoPtr->grid)[i][j];
                    dgrid[i][j] -= 2147483648.0;
                }
            }
            break;
        case ULONG24:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((unsigned long**)InfoPtr->grid)[i][j];
                    dgrid[i][j] -= 8388608.0;
                }
            }
            break;
        case FLOAT:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   dgrid[i][j] = ((float**)InfoPtr->grid)[i][j];
                }
            }
            break;
    }  /* end of switch (InfoPtr->input_datatype) block */

    /* Cycle thru list and perform operations in sequence */
    itemp = 0;
    datatype = DOUBLE;
    for (i=0; (i < MAX_PROC_FUNCS) && (InfoPtr->proc_order[i] > 0); i++)
    {   switch (InfoPtr->proc_order[i])
        {   case FFT_FILTER:
                /* function will return 0 on success or if filtering not required */
                itemp = FftFilterGridTraces(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,InfoPtr->preprocFFT,InfoPtr->ns_per_samp,
                            InfoPtr->proc_set[i][0],InfoPtr->proc_set[i][1],
                            (void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"FFT_FILTER: %s\n",FftFilterGridTracesMsg[itemp]);
                    itemp = 1;
                }
                break;
            case INST_AMP:
            {   int attrib = 1; /* instantaneous amplitude */

                /* function will return 0 on success or if filtering not required */
                itemp = InstAttribGridTraces(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,attrib,(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"INST_AMP: %s\n",InstAttribGridTracesMsg[itemp]);
                    itemp = 1;
                }

                break;
            }
            case INST_POW:
            {   int attrib = 2; /* instantaneous power */

                /* function will return 0 on success or if filtering not required */
                itemp = InstAttribGridTraces(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,attrib,(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"INST_POW: %s\n",InstAttribGridTracesMsg[itemp]);
                    itemp = 1;
                }
                break;
            }
            case AMP_ADJ:
                if (InfoPtr->proc_value[i] != INVALID_VALUE)
                    AdjustMeanGridTraces(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,InfoPtr->proc_value[i],(void **)dgrid);
                break;
            case GLOB_REM:
                if (InfoPtr->proc_value[i] != 0)
                    itemp = RemGridGlobBckgrnd(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"GLOB_REM: %s\n",RemGridGlobBckgrndMsg[itemp]);
                    itemp = 1;
                }
                break;
            case GLOB_DIFF:
                if (InfoPtr->proc_value[i] != 0)
                    itemp = RemGridGlobForgrnd(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"GLOB_DIFF: %s\n",RemGridGlobForgrndMsg[itemp]);
                    itemp = 1;
                }
                break;
            case WIND_REM:
                if (InfoPtr->proc_value[i] > 0)
                    itemp = RemGridWindBckgrnd(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"WIND_REM: %s\n",RemGridWindBckgrndMsg[itemp]);
                    itemp = 1;
                }
                break;
            case WIND_DIFF:
                if (InfoPtr->proc_value[i] > 0)
                    itemp = RemGridWindForgrnd(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"WIND_DIFF: %s\n",RemGridWindForgrndMsg[itemp]);
                    itemp = 1;
                }
                break;
            case HSMOOTH:
                if (InfoPtr->proc_value[i] > 0)
                    itemp = SmoothGridHorizontally(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"HSMOOTH: %s\n",SmoothGridHorizontallyMsg[itemp]);
                    itemp = 1;
                }
                break;
            case VSMOOTH:
                if (InfoPtr->proc_value[i] > 0)
                    itemp = SmoothGridVertically(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"VSMOOTH: %s\n",SmoothGridVerticallyMsg[itemp]);
                    itemp = 1;
                }
                break;
            case RG_ON:
            {   int num_off = 0;

                itemp = ChangeGridRangeGain(InfoPtr->num_cols,InfoPtr->num_rows,
                                InfoPtr->first_proc_samp,InfoPtr->header_samps,
                                num_off,NULL,
                                (int)InfoPtr->proc_value[i],InfoPtr->proc_set[i],
                                datatype,(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"RG_ON: %s\n",ChangeGridRangeGainMsg[itemp]);
                    itemp = 1;
                }
                break;
            }
            case RG_OFF:
            {   int num_on = 0;

                itemp = ChangeGridRangeGain(InfoPtr->num_cols,InfoPtr->num_rows,
                                InfoPtr->first_proc_samp,InfoPtr->header_samps,
                                (int)InfoPtr->proc_value[i],InfoPtr->proc_set[i],
                                num_on,NULL,
                                datatype,(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"RG_OFF: %s\n",ChangeGridRangeGainMsg[itemp]);
                    itemp = 1;
                }
                break;
            }
            case DECON:
                break;
            case MIGRATE:
                break;
            case SMOOTH2D:
                break;
            case AMP_SCALE:
                if (InfoPtr->proc_value[i] != 0 && InfoPtr->proc_value[i] != 1.0)
                    RescaleGrid(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,InfoPtr->proc_value[i],(void **)dgrid);
                break;
            case STACK:
                if (InfoPtr->proc_value[i] > 1)
                    StackGrid(&(InfoPtr->num_cols),&(InfoPtr->num_rows),
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"STACK: %s\n",StackGridMsg[itemp]);
                    itemp = 1;
                }
                break;
            case SPAT_MED:
                if (InfoPtr->proc_value[i] > 2)
                    itemp = ApplyGridSpatMedFilter(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"SPAT_MED: %s\n",ApplyGridSpatMedFilterMsg[itemp]);
                    itemp = 1;
                }
                break;
            case TEMP_MED:
                if (InfoPtr->proc_value[i] > 2)
                    itemp = ApplyGridTempMedFilter(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                if (itemp > 0)
                {   if (log_file)
                        fprintf(log_file,"TEMP_MED: %s\n",ApplyGridTempMedFilterMsg[itemp]);
                    itemp = 1;
                }
                break;
            case SAMP_SLIDE:
                if (InfoPtr->proc_value[i] != 0)
                    SlideSamples(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(int)InfoPtr->proc_value[i],(void **)dgrid);
                break;
            case TRACE_EQUAL:
                if (InfoPtr->proc_value[i] != -1)
                    EqualizeGridTraces(InfoPtr->num_cols,InfoPtr->num_rows,
                            InfoPtr->first_proc_samp,InfoPtr->header_samps,
                            datatype,(long)InfoPtr->proc_value[i],(void **)dgrid);
                break;
        }  /* end of switch (InfoPtr->proc_order[i]) block */
        if (itemp > 0)
        {   printf("\n%s\n",ProcessGprDataMsg[itemp]);
            if (log_file)
                fprintf(log_file,"%s\n",ProcessGprDataMsg[itemp]);

            /* Deallocate storage */
            for (i=0; i<InfoPtr->num_cols; i++)
                free(dgrid[i]);
            free(dgrid);
            dgrid = NULL;

            return 1;
        }
    }  /* end of for (i=0; (i < MAX_PROC_FUNCS) && ...  loop */

    /* copy back to grid[][]; remember mean was removed from unsigned types */
    switch (InfoPtr->input_datatype)
    {   case CHAR:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 255.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((char**)InfoPtr->grid)[i][j] = _LIMIT(-128.0,dgrid[i][j],127.0);
                }
            }
            break;
        case UCHAR:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] *= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 127.0;
                } else
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] += 128.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((unsigned char**)InfoPtr->grid)[i][j] = _LIMIT(0.0,dgrid[i][j],255.0);
                }
            }
            break;
        case SHORT:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 65535.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((short**)InfoPtr->grid)[i][j] = _LIMIT(-32768.0,dgrid[i][j],32767.0);
                }
            }
            break;
        case USHORT:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] *= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 32767.0;
                } else
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] += 32768.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((unsigned short**)InfoPtr->grid)[i][j] = _LIMIT(0.0,dgrid[i][j],65535.0);
                }
            }
            break;
        case USHORT12:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] *= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 32767.0;
                } else
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] += 2048.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((unsigned short**)InfoPtr->grid)[i][j] = _LIMIT(0.0,dgrid[i][j],4095.0);
                }
            }
            break;
        case LONG:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 4294967295.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((long**)InfoPtr->grid)[i][j] = _LIMIT(-2147483648.0,dgrid[i][j],2147483647.0);
                }
            }
            break;
        case ULONG:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] *= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 2147483647.0;
                } else
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] +=  2147483648.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((unsigned long**)InfoPtr->grid)[i][j] = _LIMIT(0.0,dgrid[i][j],4294967295.0);
                }
            }
            break;
        case ULONG24:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   /* rescale if inst_amp or inst_pow otherwise add mean */
                if (InfoPtr->inst_amp)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] *= sqrt(2.0);
                } else if (InfoPtr->inst_pow)
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] /= 2147483647.0;
                } else
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        dgrid[i][j] +=  8388608.0;
                }
                {   for (j=0; j<InfoPtr->num_rows; j++)
                        ((unsigned long**)InfoPtr->grid)[i][j] = _LIMIT(0.0,dgrid[i][j],16777215.0);
                }
            }
            break;
        case FLOAT:
            for (i=0; i<InfoPtr->num_cols; i++)
            {   for (j=0; j<InfoPtr->num_rows; j++)
                {   ((float**)InfoPtr->grid)[i][j] = _LIMIT(FLT_MAX,dgrid[i][j],FLT_MAX);
                }
            }
            break;
    }  /* end of switch (InfoPtr->input_datatype) block */

    /* Deallocate storage */
    for (i=0; i<InfoPtr->num_cols; i++)
        free(dgrid[i]);
    free(dgrid);
    dgrid = NULL;

    return 0;
}
/******************************** SaveGprData() ********************************/
/* This is the driver routine for saving the GPR data to disk.
 *
 * Parameter list:
 *  struct ProcParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <dos.h>, "gprsproc.h".
 * Calls: printf, fprintf, sprintf, strcat, _dos_getdate, _dos_gettime,
 *        SaveDztData, SaveDt1Data, SaveSgyData.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SaveGprDataMsg[] =
{   "SaveGprData(): No errors.",
    "SaveGprData() ERROR: Invalid/unknown file storage format.",
    "SaveGprData() ERROR: Data/information was not saved to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: December 30, 1996
 * Revisions: August 25, 1997; August 20, 1999;
 *            March 22, 2000; March 28, 2000;
 */
int SaveGprData (struct ProcParamInfoStruct *InfoPtr)
{
    char str[80], str2[80];
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    int i,itemp,num;
    int bytes;              /* length of current proc_hist line */
    int file_num;
    int file_saved = FALSE;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    extern FILE *log_file;               /* beginning of this source */
    extern const char *SaveDztDataMsg[]; /* beginning of this source */
    extern const char *SaveDt1DataMsg[]; /* beginning of this source */
    extern const char *SaveSgyDataMsg[]; /* beginning of this source */
    extern const char *SaveUsrDataMsg[]; /* beginning of this source */

    if (Debug) puts("Start of SaveGprData()");

    /* Fill processing history string */
    file_num = InfoPtr->file_num;
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
    sprintf(InfoPtr->proc_hist,"\nGPR_PROC v.%s (USGS-JL) %d/%d/%02d [%02d:%02d]\n",
            GPR_PROC_VER,dosdate.month,dosdate.day,dosdate.year-2000,dostime.hour,dostime.minute);
    sprintf(str,"INPUT: %s\n",InfoPtr->dat_infilename[file_num]);
    strcat(InfoPtr->proc_hist,str);
    if (InfoPtr->num_proc == 0)  strcat(InfoPtr->proc_hist,"DATA NOT PROCESSED\n");
    for (num=0; num<InfoPtr->num_proc; num++)
    {   str[0] = 0;
        switch (InfoPtr->proc_order[num])
        {   case FFT_FILTER:
                sprintf(str,"FREQ FIL:lo=%g hi=%g MHz\n",
                        InfoPtr->proc_set[num][0]/1.0e+6,InfoPtr->proc_set[num][1]/1.0e+6);
                break;
            case INST_AMP:
                sprintf(str,"INST AMP\n");
                break;
            case INST_POW:
                sprintf(str,"INST POW\n");
                break;
            case AMP_ADJ:
                if (InfoPtr->amp_adjust != INVALID_VALUE)
                    sprintf(str,"AMPS ADJ=%g\n", InfoPtr->proc_value[num]);
                break;
            case GLOB_REM:
                sprintf(str,"GLOB BKGRND\n");
                break;
            case GLOB_DIFF:
                sprintf(str,"GLOB FRGRND\n");
                break;
            case WIND_REM:
                sprintf(str,"WIN BKGRND=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case WIND_DIFF:
                sprintf(str,"WIN FRGRND=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case HSMOOTH:
                sprintf(str,"H SMOOTH=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case VSMOOTH:
                sprintf(str,"V SMOOTH=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case RG_ON:
                sprintf(str,"GAIN ADD:%d[",(int)InfoPtr->proc_value[num]);
                for (i=0; i<(int)InfoPtr->proc_value[num]; i++)
                {   sprintf(str2," %g",InfoPtr->proc_set[num][i]);
                    strcat(str,str2);
                }
                strcat(str,"]\n");
                break;
            case RG_OFF:
                sprintf(str,"GAIN REM:%d[",(int)InfoPtr->proc_value[num]);
                for (i=0; i<(int)InfoPtr->proc_value[num]; i++)
                {   sprintf(str2," %g",InfoPtr->proc_set[num][i]);
                    strcat(str,str2);
                }
                strcat(str,"]\n");
                break;
            case DECON:
                sprintf(str,"DECON\n");
                break;
            case MIGRATE:
                sprintf(str,"MIGRAT\n");
                break;
            case SMOOTH2D:
                sprintf(str,"2D SMOOTH\n");
                break;
            case AMP_SCALE:
                sprintf(str,"AMP MULT=%g\n",InfoPtr->proc_value[num]);
                break;
            case STACK:
                sprintf(str,"DATA STACK=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case SPAT_MED:
                sprintf(str,"SPAT MED=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case TEMP_MED:
                sprintf(str,"TEMP MED=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case SAMP_SLIDE:
                sprintf(str,"SAMP SLIDE=%d\n",(int)InfoPtr->proc_value[num]);
                break;
            case TRACE_EQUAL:
                sprintf(str,"TRACE EQUAL TO #%d\n",(int)InfoPtr->proc_value[num]);
                break;
        }
        if ( (strlen(InfoPtr->proc_hist) + strlen(str)) <= (sizeof(InfoPtr->proc_hist)-1) )
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
 *  struct ProcParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gprsproc.h".
 * Calls: SetDzt5xHeader, ReadOneDztHeader, SaveDztFile, _splitpath (non-ANSI),
 *        strcpy, strncpy, strcat, sprintf, puts, printf, strlen, TrimStr.
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
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: February 1, 1996
 * Revisions: August 25, 1997; August 18, 1999;
 *            March 20, 2000;
 */
int SaveDztData (struct ProcParamInfoStruct *InfoPtr)
{
    int i,itemp;
    int file_num;
    char name[12],antname[14], drive[_MAX_DRIVE], dir[_MAX_DIR];
    char fname[_MAX_FNAME], fext[_MAX_EXT];
    char text[MAX_PROC_HIST_SIZE];
    char str[512];
    unsigned char proc_hist[MAX_PROC_HIST_SIZE];  /* hopefully it is big enough! */
    int hdr_num,num_hdrs,header_size,rh_dtype;
    unsigned short rh_nchan,rg_breaks,rh_nproc;
    float rg_values[8];    /* this limit may change in future versions */
    long num_traces;
    struct DztHdrStruct hdr;              /* struct in gpr_io.h */
    struct DztHdrStruct *hdrPtrArray[1];
    struct DztDateStruct create_date;
    extern const char *SaveDztFileMsg[];  /* in gpr_io.h, dzt_io.c */
    extern FILE *log_file;                /* beginning of this source */

    if (Debug) puts("Start of SaveDztData()");
    switch (InfoPtr->input_datatype)
    {   case -1:  case -2:    break;
        default:              return 1;
    }

    /* Read input DZT header again */
    file_num = InfoPtr->file_num;
    if (Debug) puts("    SaveDztData(): before ReadOneDztHeader()");
    itemp = ReadOneDztHeader(InfoPtr->dat_infilename[file_num],&num_hdrs,
                             &num_traces,InfoPtr->channel,&header_size,&hdr);
    if (itemp > 0)
    {   if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
        return 2;
    }

    /* Set/copy values for output header */
    if (Debug) puts("    SaveDztData(): start of set/copy values");
    hdr.rh_nsamp = InfoPtr->total_samps;
    if (hdr.rh_dtype == 0 && InfoPtr->input_datatype != -1)
    {   if (InfoPtr->input_datatype == -2)      rh_dtype = 0x0002;
        else if (InfoPtr->input_datatype == -3) rh_dtype = 0x0004;
        else                                    rh_dtype = 0x0000;
    }

    rg_breaks = *(unsigned short *) ((char *)&hdr + hdr.rh_rgain);
    for (i=0; i<rg_breaks && i<8; i++)
        rg_values[i] = *(float *) ((char *)&hdr + hdr.rh_rgain + 2 + 4*i);

    /* copy this program processing history into text/comment area */
    strncpy(str,(char *)&hdr + hdr.rh_text,sizeof(str)-1);
    TrimStr(str);
    strncpy(text,str,sizeof(text)-1);
    if (strlen(text) < sizeof(text) - strlen(InfoPtr->proc_hist)-2)
        if ((strlen(text) + strlen(InfoPtr->proc_hist)) < (sizeof(text)-2))
            strcat(text,InfoPtr->proc_hist);

    /* copy old proccessing history */
    rh_nproc = hdr.rh_nproc;
    for (i=0; i<rh_nproc && i<(MAX_PROC_HIST_SIZE-1); i++)
        proc_hist[i] = *((char *)&hdr + hdr.rh_proc + i);
    proc_hist[i] = 0;  /* counts on i being past for-loop limits */

    hdr_num = 1;
    rh_nchan = 1;
    _splitpath(InfoPtr->dat_outfilename[file_num],drive,dir,fname,fext);
    strcpy(name,fname);
    strncpy(antname,hdr.rh_antname,sizeof(antname)-1);
    antname[sizeof(antname)-1] = 0;
    create_date = hdr.rh_create;
    /* NOTE: Do not send pointers from hdr through this function!
             SetDzt5xHeader() clears structure arrays before writing to
             them! Other members are OK as they are sent by value.
     */
    if (Debug) puts("    SaveDztData(): before SetDzt5xHeader()");
    SetDzt5xHeader(hdr_num,hdr.rh_nsamp,hdr.rh_bits,hdr.rh_zero,hdr.rh_sps,
                 hdr.rh_spm,hdr.rh_mpm,hdr.rh_position,hdr.rh_range,
                 hdr.rh_npass,rh_nchan,hdr.rh_epsr,hdr.rh_top,hdr.rh_depth,
                 rh_dtype,antname,name,text,rg_breaks,rg_values,rh_nproc,
                 proc_hist,&create_date,&hdr);

    /* Save to disk */
    if (Debug) puts("    SaveDztData(): before SaveDztFile()");
    hdrPtrArray[0] = &hdr;
    itemp = SaveDztFile(InfoPtr->dat_outfilename[file_num],InfoPtr->num_cols,
                (long)hdr.rh_nsamp,(int)hdr.rh_bits,hdr_num,
                hdrPtrArray,InfoPtr->grid);
    if (Debug) printf("    SaveDztData(): after SaveDztFile(); itemp=%d\n",itemp);
    if (Debug) getch();
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
 *  struct ProcParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, "gprsproc.h".
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
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: January 31, 1996
 * Revisions: September 9, 1997;
 *            March 22, 2000;
 */
int SaveDt1Data (struct ProcParamInfoStruct *InfoPtr)
{
    int  file_num;

    int itemp,print_it,num_gain_pts,proc_hist_size,trace_size;
    long trace;
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
    meters_per_mark = traces_per_sec = 0.0;
    num_gain_pts = 0;
    proc_hist_size = strlen(HdInfo.proc_hist) + 1;
    proc_hist_size += strlen(InfoPtr->proc_hist) + 2;
    realloc(HdInfo.proc_hist,proc_hist_size);
    strcat(HdInfo.proc_hist,InfoPtr->proc_hist);
    itemp = SaveSsHdFile((int)HdInfo.day,(int)HdInfo.month,(int)HdInfo.year,
                         InfoPtr->num_cols,InfoPtr->total_samps,
                         (long)HdInfo.time_zero_sample,(int)HdInfo.total_time_ns,
                         (double)HdInfo.start_pos,(double)HdInfo.final_pos,
                         (double)HdInfo.step_size,HdInfo.pos_units,
                         (double)HdInfo.ant_freq,(double)HdInfo.ant_sep,
                         (double)HdInfo.pulser_voltage,(int)HdInfo.num_stacks,
                         HdInfo.survey_mode,HdInfo.proc_hist,
                         InfoPtr->hd_outfilename,
                         InfoPtr->dat_outfilename[file_num],
                         HdInfo.job_number,HdInfo.title1,HdInfo.title2,
                         traces_per_sec,meters_per_mark,num_gain_pts,
                         gain_pts);
    if (itemp > 0)
    {   if (log_file) fprintf(log_file,"%s\n",SaveSsHdFileMsg[itemp]);
        return 3;
    }

    /* Open Output DT1 file */
    if ((outfile = fopen(InfoPtr->dat_outfilename[file_num],"wb")) == NULL)  return 4;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(outbuffer,vbufsize);
      if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
      else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
    #endif

    /* Save data to disk */
    itemp = 0;
    trace_size = InfoPtr->num_rows * 2;   /* data bytes + header bytes */
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
 *  struct ProcParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, "gprsproc.h".
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
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: May 23, 1996;
 * Revisions: March 22, 2000;
 */
int SaveSgyData (struct ProcParamInfoStruct *InfoPtr)
{
    int  file_num;

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
    file_num = InfoPtr->file_num;
    itemp = ReadSegyReelHdr(InfoPtr->dat_infilename[file_num],&swap_bytes,&num_traces,
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
    num_samples      = Info.in_samps_per_trace;
    time_zero_sample = Info.timezero_sample;
    total_time_ns    = Info.in_time_window;
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
                     InfoPtr->dat_outfilename[file_num],traces_per_sec,meters_per_mark,
                     num_gain_pts,gain_pts,&hdr);
    free(proc_hist);

    /* Open output SGY file */
    if ((outfile = fopen(InfoPtr->dat_outfilename[file_num],"wb")) == NULL)  return 3;

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
    trace_size *= InfoPtr->num_rows;   /* data bytes + header bytes */
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
/**************************** end of GPR_PROC.C *****************************/
