#ifndef GPR_PROC_H
#define GPR_PROC_H  1.00
/* GPR_PROC.H  version 1.05.02.01 */
/* Jeff Lucius  U.S. Geological Survey
 * May 2, 2001
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <conio.h>    /* getch */
#include <malloc.h>   /* _memavl */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <float.h>    /* FLT_MAX */
#include <math.h>     /* sqrt */
#include <stdio.h>    /* printf,FILE,NULL,_IOFBF,fopen,fclose,fgets,puts,
                         fread,fwrite,setvbuf,fseek,SEEK_SET,SEEK_END,
                         ftell,size_t,sprintf,ferror,clearerr */
#include <stdlib.h>   /* atof,atoi,NULL,malloc,free,size_t,exit,
                         _MAX_DRIVE,_MAX_DIR,_MAX_FNAME,_MAX_EXT,
                         _splitpath (non-ANSI),realloc */
#include <string.h>   /* strcpy,strlen,strstr,memset,strncpy,memcpy,strcat,
                         strlwr (non-ANSI),strupr (non-ANSI),strcmp,size_t */

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "gpr_dfx.h"  /* GPR data manipulation library */
#include "gpr_io.h"   /* GPR data I/O library */
#include "jl_defs.h"  /* constants and macros common to different functions */
#include "jl_util1.h" /* JL utility functions library */

/*************************** Notes to developers ****************************/
/*
To add a new processing technique or variable:  (OLD INSTRUCTIONS 3/29/00)
  1. Update the default command file GPR_PROC.CMD anf GPR_PROC.TXT/DOC with the
     new option and default value, and add descriptive comments for the option.
  2. Update this file GPR_PROC.H.
     a. Add a new manifest constant (with a unique value) in
        "Manifest constants" and increase the value of the "NUM_FUNCS"
        constant.
     b. Add a new member to the ProcParamInfoStruct structure in
        "New data types". This structure passes all necessary information
        between functions.
  3. Update the source code file GPR_PROC.C.
     a. Add the new command-file name to the end of GPR_PROC_CMDS[] array in
        "Global variables".
     a. Initialize the new structure member in InitParameters().
     b. Add valid-value checking in GetParameters() if neccesary.
     c. Add the new option in GetCmdFileArgs() and update that function's
        copy of GPR_PROC_CMDS[] in its intro.
     d. Update DisplayParameters() to print out the new option's value.
     e. Add a new case in the processing switch block in ProcessData().
        (If the function returns an error code, add a new message to the
         end of ProcessDataMsg[] array in GPR_PROC.H and to the copy of that
         array in the ProcessData() intro.  Be sure the error code matches
         the messages position within the array).
     f. Add a new case in the switch block in SaveGprData() that updates
         proc_hist.
  4. Update the source code and header files GPR_DFX.C and GPR_DFX.H.
     a. Add the function prototype(s) in "Function prototypes".
     b. Add the new function
     c. Add a new error message as appropriate in "Global variables".
  5. Recompile as specified in the discussion at the beginning of GPR_DFX.C
        (update library) and GPR_PROC.C.
 */
/*************************** Manifest constants *****************************/
#define GPR_PROC_VER "1.05.02.01"
#define DATETIME "May 2, 2001"
#define MAX_PROC_HIST_SIZE  512

#ifndef NUM_PROC_FUNCS   /* these provide unique IDs to processing options */
  #define MAX_PROC_FUNCS 100
  #define NO_PROCESSING    0
  #define FFT_FILTER       1
  #define AMP_ADJ          2
  #define GLOB_REM         3
  #define WIND_REM         4
  #define HSMOOTH          5
  #define VSMOOTH          6
  #define RG_OFF           7
  #define DECON            8
  #define MIGRATE          9
  #define SMOOTH2D        10
  #define AMP_SCALE       11
  #define STACK           12
  #define TEMP_MED        13
  #define SPAT_MED        14
  #define RG_ON           15
  #define SAMP_SLIDE      16
  #define TRACE_EQUAL     17
  #define INST_AMP        18
  #define INST_POW        19
  #define GLOB_DIFF       20
  #define WIND_DIFF       21
  #define NUM_PROC_FUNCS  22
#endif

/******************************* New data types *****************************/
/* This structure passes all necessary information between functions. */
struct ProcParamInfoStruct
{   /* Input filenames */
    char cmd_filename[MAX_PATHLEN];   /* filename of command file */
    int  num_input_files;             /* number of input files */
    int  file_num;                    /* current file being processed, from 0 */
    char **dat_infilename;            /* pointer to array of filenames */
    char hd_infilename[MAX_PATHLEN];  /* filename of the ASCII information file */
    char mrk_infilename[MAX_PATHLEN]; /* filename of the ASCII marker file */
    char xyz_infilename[MAX_PATHLEN]; /* filename of the ASCII marker file */

    /* Output filenames */
    char **dat_outfilename;            /* pointer to array of filenames */
    char hd_outfilename[MAX_PATHLEN];  /* output ASCII S&S info file */
    char mrk_outfilename[MAX_PATHLEN]; /* filename of the ASCII marker file */
    char xyz_outfilename[MAX_PATHLEN]; /* filename of the ASCII marker file */

    /* Input data descriptors */
    int    storage_format; /* constant value indicating data storage format;
                             see manifest constants above */
    int    input_datatype; /* type of input data elements, grid, and output
                    if ==  1, 1-byte chars;          == -1, unsigned chars
                       ==  2, 2-byte ints or shorts; == -2, unsigned shorts
                       ==  3, 4-byte ints or longs;  == -3, unsigned longs
                       ==  4, 4-byte floats
                       == -5, unsigned shorts but only first 12-bits used
                       == -6, unsigned longs but only first 24-bits used
                       ==  8, 8-byte doubles
                            */
    int    file_header_bytes; /* number of bytes in file header */
    int    trace_header_bytes; /* number of bytes in each trace header */
    int    channel;       /* channel number in multi-channel data sets (0 - 3);
                             indexed from 0; default is 0 */
    long   first_proc_samp; /* start processing a trace at this sample,
                               indexed from 0; this allows for reserved
                               samples in DZT traces */
    int    header_samps;    /* the equivalent number of data samples in
                               each trace header;0 for DZT files;
                               64 for DT1 files, 240/data_bytes for SGY
                               files, and "trace_header_bytes"/data_bytes
                               for user-defined files. */
    long   total_samps;   /* total number of samples in each trace; this
                             value is determined from the data/info file and
                             controls (along with trace_header_bytes) the
                             number of rows in the grid */
    long   total_traces;  /* total number of traces in the file */
    double total_time;    /* number of nanoseconds per trace */
    double ns_per_samp;   /* number of nanoseconds per sample */

    /* Input data-set size limiters */
    long   first_trace;     /* start processing at this trace (inclusive),
                               indexed from 0 */
    long   last_trace;      /* stop processing at this trace (inclusive),
                               indexed from 0 */

    /* processing descriptors */
    /* NOTE: user specifies frequencies in MHz (Megahertz) */
    int    num_proc;
    int    proc_order[NUM_PROC_FUNCS]; /* maintains processing order list */
    double proc_value[NUM_PROC_FUNCS]; /* maintains processing value list */
    double *proc_set[NUM_PROC_FUNCS];  /* maintains processing value set */
    int    num_gain_off;
    int    num_gain_on;
    double amp_scale;      /* value to multiply all samples by */
    long   stack;          /* number of traces to stack into an average trace */
    double lo_freq_cutoff; /* high-pass filter frequency in MHz, zeroes before
                              here, 0 for no filtering */
    double hi_freq_cutoff; /* low-pass filter fequency MHz, zeroes past here,
                              0 for no filtering */
    int    preprocFFT;     /* flag, preprocess scans for FFT if TRUE and FFT
                              filtering is performed (tapers edges to 0) */
    double amp_adjust;        /* add a value to the trace amplitudes;
                              no adjustment if INVALID_VALUE */
    int    glob_bckgrnd_rem; /* flag, remove background trace if TRUE */
    int    glob_forgrnd_rem; /* flag, remove foreground trace if TRUE */
    long   wind_bckgrnd_rem; /* if >1, size of sliding window for background trace;
                             if an even number, value will be incremented by 1;
                             0 or 1 for no sliding window */
    long   wind_forgrnd_rem; /* if >1, size of sliding window for background trace;
                             if an even number, value will be incremented by 1;
                             0 or 1 for no sliding window; background is preserved
                             in this case and forground (high-angle stuff) removed */
    long   hsmooth;         /* half width of horizontal smoothing hanning window,
                               >=2; 0 for none */
    long   vsmooth;         /* half width of vertical smoothing hanning window,
                               >=2; 0 for none */
    long   spatial_median;  /* if odd number > 0, then number of points in a spatial
                             median filter ("horizontal" across the traces) */
    long   temporal_median; /* if odd number > 0, then number of points in a temporal
                             median filter ("vertical" down each trace) */
    int    samp_slide;      /* number of locations to slide sample location
                               in a tracel; positive slides location down */
    int    trace_equalize;  /* flag to equalize trace amplitudes
                               -1 no equalization; 0 use first trace, -2 use
                               middle trace, -3 use last trace, n = use
                               trace n */
    int    inst_amp;        /* flag, convert traces to instantaneous amplitude
                               if TRUE */
    int    inst_pow;        /* flag, convert traces to instantaneous power
                               if TRUE */
    char   proc_hist[MAX_PROC_HIST_SIZE];
                            /* will hold descriptive string of processing methods */

    /* Grid information (grid has same type as input data) */
    int    created;       /* flag, tracks allocation of "grid[][]" */
    void **grid;          /* pointer to 2D grid storage area [num_cols][num_rows] */
    long   num_cols;      /* number of traces to process (not necessarily
                               same as in file); number of columns in grid */
    long   num_rows;      /* number of samples in each trace plus the trace
                               header divided into "samples";
                               number of rows in grid */
} ;
/********************* Global variables from gpr_proc.c *********************/
extern int Debug, Batch, ANSI_THERE;
extern FILE *log_file;
extern const char *GPR_PROC_CMDS[];
extern const char *GetParametersMsg[];
extern const char *GetGprDataAsGridMsg[];
extern const char *ProcessGprDataMsg[];
extern const char *SaveGprDataMsg[];
extern const char *SaveDt1DataMsg[];
extern const char *SaveSgyDataMsg[];
extern const char *SaveUsrDataMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int   GetParameters(int argc, char *argv[],struct ProcParamInfoStruct *ParamInfoPtr);
void  PrintUsageMsg(void);
void  InitParameters(struct ProcParamInfoStruct *InfoPtr);
void  DeallocInfoStruct(struct ProcParamInfoStruct *InfoPtr);
int   GetCmdFileArgs(struct ProcParamInfoStruct *InfoPtr);
int   GetGprDataAsGrid(int command,struct ProcParamInfoStruct *InfoPtr);
int   GetOtherInfo(struct ProcParamInfoStruct *InfoPtr);
void  DisplayParameters(struct ProcParamInfoStruct *ParamInfoPtr);
int   ProcessData(struct ProcParamInfoStruct *InfoPtr);
int   SaveGprData(struct ProcParamInfoStruct *InfoPtr);
int   SaveDztData(struct ProcParamInfoStruct *InfoPtr);
int   SaveDt1Data(struct ProcParamInfoStruct *InfoPtr);
int   SaveSgyData(struct ProcParamInfoStruct *InfoPtr);
int   SaveUsrData(struct ProcParamInfoStruct *InfoPtr);

#endif /* #ifndef GPR_PROC_H */
