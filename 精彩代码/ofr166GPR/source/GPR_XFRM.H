#ifndef GPR_XFRM_H
#define GPR_XFRM_H  1.04.17.01
/* GPR_XFRM.H  version 1.04.17.01 */
/* Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * February 10, 1997; April 17, 2001;
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <dos.h>      /* _dos_getdate,_dos_gettime,
                         struct dosdate_t,struct dostime_t */
#include <conio.h>    /* getch */
#include <malloc.h>   /* _memavl */

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

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "gpr_io.h"   /* GPR data I/O library */
#include "jl_defs.h"  /* constants and macros common to different functions */
#include "jl_util1.h" /* JL utility functions library */

/*************************** Manifest constants *****************************/
#define GPR_XFRM_VER "1.04.17.01"  /* current software version */
#define DATETIME     "April 17, 2001"

#ifndef NUM_SPATIAL_DIR
  #define NUM_SPATIAL_DIR 2
  #define X_DIR   0
  #define Y_DIR   1
#endif

/******************************* New data types *****************************/
/* This structure hold the bin information */
struct XfrmBin
{   double  col_loc;   /* location of bin in column space (independant variable
                          for splining */
    double  x;         /* "X" location assigned to bin */
    double  y;         /* "Y" location assigned to bin */
    double  z;         /* "Z" location assigned to bin */
    double  left;      /* location of left side of bin */
    double  right;     /* location of right side of bin */
    long    num_items; /* number of items in the bin (number of traces) */
    long    item_size; /* size/length of each item (number of samples) */
    double *sum;       /* [item_size]; pointer to array to hold sum of items */
} ;

/* This structure passes all necessary information between functions. */
struct XfrmParamInfoStruct
{   /* Input filenames */
    char cmd_filename[MAX_PATHLEN];   /* filename of command file */
    char dat_infilename[MAX_PATHLEN]; /* filename of the binary data file */
    char inf_infilename[MAX_PATHLEN]; /* filename of the ASCII information file */
    char mrk_infilename[MAX_PATHLEN]; /* filename of the ASCII marker file */
    char xyz_infilename[MAX_PATHLEN]; /* filename of the ASCII X-Y-Z file */

    /* Output filenames */
    char dat_outfilename[MAX_PATHLEN]; /* output binary data file */
    char inf_outfilename[MAX_PATHLEN]; /* output ASCII info file */
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
    double ns_per_samp;   /* number of nanoseconds per sample (two-way travel) */
    long   num_ticks;     /* number of markers/xyz sets */
    long  *tick_tracenum; /* pointer to 1D array of marker trace numbers */
    double **tick_xyz;    /* pointer to 2D array of marker coordinates */
    double *trace_Xval;   /* pointer to 1D array of trace X-values */
    double *trace_Yval;   /* pointer to 1D array of trace Y-values */
    double *trace_Zval;   /* pointer to 1D array of trace Z-values */
    int    dat_forward;   /* forward = 1; reverse = 0; */

    /* transforming descriptors */
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
    struct XfrmBin *BinArray; /* pointer to array of bin structures */
    long   num_bins;        /* same as number of new output traces */
    double bin_size;        /* width of bin in user units */
    int    show_stats;      /* true or false */

    /* Input grid information (grid has same type as input data) */
    int    created;       /* flag, tracks allocation of "grid[][]" */
    void **grid;          /* pointer to 2D grid storage area [num_cols][num_rows] */
    long   num_cols;      /* number of traces to transform (not necessarily
                               same as in file); number of columns in grid */
    long   num_rows;      /* number of samples in each trace plus the trace
                               header divided into "samples";
                               number of rows in grid */
} ;
/********************* Global variables from gpr_proc.c *********************/
extern int Debug, Batch, ANSI_THERE;
extern FILE *log_file;
extern const char *GPR_XFRM_CMDS[];
extern const char *GetParametersMsg[];
extern const char *GetGprDataAsGridMsg[];
extern const char *GetXfrmRangesMsg[];
extern const char *TransformGprDataMsg[];
extern const char *XfrmSaveMrkXyzDataMsg[];
extern const char *XfrmSaveGprDataMsg[];
extern const char *XfrmSaveDztDataMsg[];
extern const char *XfrmSaveDt1DataMsg[];
extern const char *XfrmSaveSgyDataMsg[];
extern const char *XfrmSaveUsrDataMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif
int   GetParameters(int argc, char *argv[],struct XfrmParamInfoStruct *ParamInfoPtr);
void  PrintUsageMsg(void);
void  InitParameters(struct XfrmParamInfoStruct *InfoPtr);
int   GetCmdFileArgs(struct XfrmParamInfoStruct *InfoPtr);
int   GetGprDataAsGrid(int command,struct XfrmParamInfoStruct *InfoPtr);
int   GetOtherInfo(struct XfrmParamInfoStruct *InfoPtr);
void  DisplayParameters(struct XfrmParamInfoStruct *ParamInfoPtr);
int   TransformGprData(struct XfrmParamInfoStruct *InfoPtr);
int   XfrmSaveMrkXyzData(struct XfrmParamInfoStruct *InfoPtr);
int   XfrmSaveGprData(struct XfrmParamInfoStruct *InfoPtr);
int   XfrmSaveDztData(struct XfrmParamInfoStruct *InfoPtr);
int   XfrmSaveDt1Data(struct XfrmParamInfoStruct *InfoPtr);
int   XfrmSaveSgyData(struct XfrmParamInfoStruct *InfoPtr);
int   XfrmSaveUsrData(struct XfrmParamInfoStruct *InfoPtr);
void  DeallocInfoStruct(struct XfrmParamInfoStruct *InfoPtr);
#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* #ifndef GPR_XFRM_H */
