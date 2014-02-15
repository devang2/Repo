#ifndef GPR_SAMP_H
#define GPR_SAMP_H  1.08.09.01
/* GPR_SAMP.H  version 1.08.09.01 */
/* Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * September 5, 1997; August 9, 2001;
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <conio.h>    /* getch */
#include <malloc.h>   /* _memavl */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <float.h>    /* FLT_MAX */
#include <math.h>     /* sqrt,cos,sin,pow */
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
#include "gpr_io.h"   /* GPR data I/O library */
#include "jl_defs.h"  /* constants and macros common to different functions */
#include "jl_util1.h" /* JL utility functions library - ANSI_there,
                         BandPassFFT1D, FftFilterTrace, FFT1D, GetDosVersion,
                         LinFit, RealFFT1D, Sound, Spline, Spline2, TrimStr */

/*************************** Manifest constants *****************************/
#define GPR_SAMP_VER "1.08.09.01"
#define DATETIME "August 9, 2001"
#define MAX_PROC_HIST_SIZE 400

/******************************* New data types *****************************/
/* This structure passes all necessary information between functions. */
struct SampParamInfoStruct
{   /* Input filenames */
    char cmd_filename[MAX_PATHLEN];   /* filename of command file */
    char dat_infilename[MAX_PATHLEN]; /* filename of the binary data file */
    char hd_infilename[MAX_PATHLEN];  /* filename of the ASCII information file */

    /* Output filenames */
    char dat_outfilename[MAX_PATHLEN]; /* output binary data file */
    char hd_outfilename[MAX_PATHLEN];  /* output ASCII S&S info file */

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
    long   first_samp;      /* start processing at this sample number (inclusive),
                               indexed from 0 */
    long   last_samp;       /* stop processing at this sample number (inclusive),
                               indexed from 0 */
    long   new_total_samps; /* new total number of samples in each trace, determined from
                               first_samp and last_samp */
    double new_total_time;  /* new number of nanoseconds per trace determined from
                               first_samp and last_samp */
    double new_ns_per_samp; /* new number of nanoseconds per sample determined from
                               first_samp, last_samp and resample */
    long   resample;       /* new number of samples per trace, changes sample rate */
    double lo_freq_cutoff; /* high-pass filter frequency in MHz, zeroes before
                              here, 0 for no filtering */
    double hi_freq_cutoff; /* low-pass filter fequency MHz, zeroes past here,
                              0 for no filtering */
    int    preprocFFT;     /* flag, preprocess scans for FFT if TRUE and FFT
                              filtering is performed (tapers edges to 0) */
    char   proc_hist[512];
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
/********************* Global variables from gpr_samp.c *********************/
extern int Debug, Batch, ANSI_THERE;
extern FILE *log_file;
extern const char *GPR_SAMP_CMDS[];
extern const char *GetParametersMsg[];
extern const char *GetGprDataAsGridMsg[];
extern const char *ResampleGprDataMsg[];
extern const char *SaveGprDataMsg[];
extern const char *SaveDt1DataMsg[];
extern const char *SaveSgyDataMsg[];
extern const char *SaveUsrDataMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int   GetParameters(int argc, char *argv[],struct SampParamInfoStruct *ParamInfoPtr);
void  PrintUsageMsg(void);
void  InitParameters(struct SampParamInfoStruct *InfoPtr);
int   GetCmdFileArgs(struct SampParamInfoStruct *InfoPtr);
int   GetGprDataAsGrid(int command,struct SampParamInfoStruct *InfoPtr);
int   GetOtherInfo(struct SampParamInfoStruct *InfoPtr);
void  DisplayParameters(struct SampParamInfoStruct *ParamInfoPtr);
int   ResampleGprData(struct SampParamInfoStruct *InfoPtr);
int   SaveGprData(struct SampParamInfoStruct *InfoPtr);
int   SaveDztData(struct SampParamInfoStruct *InfoPtr);
int   SaveDt1Data(struct SampParamInfoStruct *InfoPtr);
int   SaveSgyData(struct SampParamInfoStruct *InfoPtr);
int   SaveUsrData(struct SampParamInfoStruct *InfoPtr);

#endif /* #ifndef GPR_SAMP_H */
