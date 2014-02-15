#ifndef GPR_XSU_H 
#define GPR_XSU_H  1.01.10.01
/* GPR_XSU.H  version 1.01.10.01 */
/* Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * February 10, 1997 (old GRP_XFRM.H)
 * December 21, 2000 [SU stuff; date function updates]
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <conio.h>    /* getch */
#include <malloc.h>   /* _memavl */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <math.h>     /* sqrt,pow */
#include <stdio.h>    /* printf,FILE,NULL,_IOFBF,fopen,fclose,fgets,puts,
                         fread,fwrite,setvbuf,fseek,SEEK_SET,SEEK_END,
                         ftell,size_t,sprintf,ferror,clearerr */
#include <stdlib.h>   /* atof,atoi,NULL,malloc,free,size_t,exit,
                         _MAX_DRIVE,_MAX_DIR,_MAX_FNAME,_MAX_EXT,
                         _splitpath (non-ANSI),realloc */
#include <string.h>   /* strcpy,strlen,strstr,memset,strncpy,memcpy,strcat,
                         strlwr (non-ANSI),strupr (non-ANSI),strcmp,size_t */
#include <time.h>     /* struct tm,time_t,localtime */

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "gpr_io.h"   /* GPR data I/O library */
#include "jl_defs.h"  /* constants and macros common to different functions */
#include "jl_util1.h" /* JL utility functions library */

/*************************** Manifest constants *****************************/
#define GPR_XSU_VER "1.01.10.01"
#define DATETIME "January 10, 2001"

#ifndef NUM_SPATIAL_DIR
  #define NUM_SPATIAL_DIR 2
  #define X_DIR   0
  #define Y_DIR   1
#endif

/******************************* New data types *****************************/
/* This structure hold the bin information */
struct XfrmBin
{   double  col_loc;   /* location of bin in column space (independant variable
                          for splining) */
    double  x;         /* "X" location assigned to bin */
    double  y;         /* "Y" location assigned to bin */
    double  z;         /* "Z" location assigned to bin */
    double  left;      /* location of left side of bin */
    double  right;     /* location of right side of bin */
    double  lead_X;    /* "X" location of leading antenna */
    double  lead_Y;    /* "Y" location of leading antenna */
    double  lead_Z;    /* "Z" location of leading antenna */
    double  trail_X;   /* "X" location of trailing antenna */
    double  trail_Y;   /* "Y" location of trailing antenna */
    double  trail_Z;   /* "Z" location of trailing antenna */
    long    num_items; /* number of items in the bin (number of traces) */
    long    item_size; /* size/length of each item (number of samples) */
    double *sum;       /* [item_size]; pointer to array to hold sum of items */
} ;

/* This structure passes all necessary information between functions. */
struct XsuParamInfoStruct
{   /* Input filenames */
    char cmd_filename[MAX_PATHLEN];   /* filename of command file */
    char dzt_infilename[MAX_PATHLEN]; /* filename of the binary data file */
    char mrk_infilename[MAX_PATHLEN]; /* filename of the ASCII marker file */
    char xyz_infilename[MAX_PATHLEN]; /* filename of the ASCII X-Y-Z file */

    /* Output filenames */
    char dzt_outfilename[MAX_PATHLEN]; /* output binary data file */
    char su_outfilename[MAX_PATHLEN];  /* output binary data file */
    char mrk_outfilename[MAX_PATHLEN]; /* output ASCII info file */
    char xyz_outfilename[MAX_PATHLEN]; /* output ASCII info file */

    /* Input data descriptors */
    int    storage_format; /* constant value indicating data storage format;
                             see manifest constants in gpr_io.h */
    int    input_datatype; /* type of input data elements and input grid
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
                               samples and trace headers; this value is
                               controlled by the data type: 2 for DZT files
                               128 for DT1 files, 240 for SGY files, and
                               "trace_header_bytes" for user-defined files. */
    long   data_samps;    /* total number of samples in each trace; this
                             value is determined from the data/info file and
                             controls (along with trace_header_bytes) the
                             number of rows in the grid */
    long   total_traces;  /* total number of traces in the input file */
                          /* traces before first_trace and after last_trace are
                             not read from the file */
    long   first_trace;   /* start transforming at this trace (inclusive),
                             indexed from 0 */
    long   last_trace;    /* stop transforming at this trace (inclusive),
                             indexed from 0 */
    double total_time;    /* number of nanoseconds per trace */
    double ns_per_samp;   /* number of nanoseconds per sample (sample rate) */
    long   num_ticks;     /* number of markers/xyz sets */
    long  *tick_tracenum; /* [num_ticks] 1D array of marker trace numbers */
    double **tick_xyz;    /* [num_ticks][3] 2D array of marker coordinates */
    double *trace_Xval;   /* [num_cols] 1D array of trace X-values */
    double *trace_Yval;   /* [num_cols] 1D array of trace Y-values */
    double *trace_Zval;   /* [num_cols] 1D array of trace Z-values */
    int    dat_forward;   /* forward = 1; reverse = 0; */
    double ant_sep;       /* antenna center to center seperation in meters */
    double timezero;      /* timezero in ns */
    double mrk_offset;    /* ant. array center to mark point */

    /* transforming descriptors */
    /* NOTE: spatial_start and spatial_stop must have the same directional sense
             as the selected coordinates in the XYZ file. For example, if
             spatial_dir is 1 and Y values are increasing as trace numbers are
             increasing, then spatial_start must be less than spatial_stop and
             spatial_step must be positive. If the Y values decrease as the
             trace numbers increase, then spatial_start must be greater than
             spatial_stop and spatial_step must be negative. */
    int    spatial_dir;     /* direction to use
                               0 = X-direction from XYZ file
                               1 = Y-direction from XYZ file */
    double spatial_start;   /* starting coordinate */
    double spatial_stop;    /* ending coordinate (may be less than _start) */
    double spatial_step;    /* uniform increment between traces */
    double mean_value;      /* middle value for input_datatype */
    char   proc_hist[400];  /* will hold descriptive string of transforming values */
    double min_trace_sep;   /* minimum distance between traces */
    double max_trace_sep;   /* maximum distance between traces */
    struct XfrmBin *BinArray; /* [num_bins] array of bin structures */
    long   num_bins;        /* same as number of new output traces */
    double bin_size;        /* width of bin in user units */

    /* Input grid information (grid has same type as input data) */
    int    created;       /* flag; tracks allocation of "grid[][]" */
    void **grid;          /* [num_cols][num_rows] 2D grid storage area */
    long   num_cols;      /* number of traces to transform (not necessarily
                               same as in file); number of columns in grid */
    long   num_rows;      /* number of samples in each trace plus the trace
                               header divided into "samples";
                               number of rows in grid */

    /* Optional information */
    int year;             /* negative values will force the date and time of */
    int month;            /*   output file creation to be used. */
    int day;
    int hour;
    int minute;
    int second;
} ;
/********************* Global variables from gpr_xsu.c *********************/
extern int Debug, Batch, ANSI_THERE;
extern FILE *log_file;
extern const char *GPR_XSU_CMDS[];
extern const char *XsuGetParamsMsg[];
extern const char *XsuGetGprDataAsGridMsg[];
extern const char *XsuGetXfrmRangesMsg[];
extern const char *XsuXformGprDataMsg[];
extern const char *XsuCalcMrkLocsMsg[];
extern const char *XsuCalcBinLocsMsg[];
extern const char *XsuSaveGprDataMsg[];
extern const char *XsuSaveDztDataMsg[];
extern const char *XsuSaveSuDataMsg[];

/*************************** Function prototypes ****************************/
/* functions with source code in gpr_xsu.c */
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif
int   XsuGetParams(int argc, char *argv[],struct XsuParamInfoStruct *ParamInfoPtr);
void  XsuPrintUsageMsg(void);
void  XsuInitParams(struct XsuParamInfoStruct *InfoPtr);
int   XsuGetCmdFileArgs(struct XsuParamInfoStruct *InfoPtr);
int   XsuGetGprDataAsGrid(int command,struct XsuParamInfoStruct *InfoPtr);
void  XsuDisplayParams(struct XsuParamInfoStruct *ParamInfoPtr);
int   XsuGetXfrmRanges(struct XsuParamInfoStruct *InfoPtr);
int   XsuCalcMrkLocs(struct XsuParamInfoStruct *InfoPtr);
int   XsuCalcBinLocs(struct XsuParamInfoStruct *InfoPtr);
int   XsuXformGprData(struct XsuParamInfoStruct *InfoPtr);
int   XsuSaveGprData(struct XsuParamInfoStruct *InfoPtr);
int   XsuSaveDztData(struct XsuParamInfoStruct *InfoPtr);
int   XsuSaveSuData(struct XsuParamInfoStruct *InfoPtr);
int   XsuSaveXyzMrkData(struct XsuParamInfoStruct *InfoPtr);
void  XsuDeallocInfoStruct(struct XsuParamInfoStruct *InfoPtr);
void  SetSuTraceHeader2(long trace_num,double ant_sep,double elev,
        double Tx_x,double Tx_y,double Rc_x,double Rc_y,
        long timezero,long num_samps,long sample_int,
        long year, long day, long hour, long minute, long second,
        struct SuTraceHdrStruct *HdrPtr);
#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* #ifndef GPR_XSU_H */
