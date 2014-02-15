#ifndef GPRSLICE_H
#define GPRSLICE_H  1.10.03.01
/* GPRSLICE.H  version 1.10.03.01 */
/* Jeff Lucius  U.S. Geological Survey
 * February 24, 1997; July 23, 1999; July 30, 1999; August 6, 1999;
 * September 2, 1999; September 30, 1999; October 14, 1999; December 22, 1999;
 * July 27, 2000; September 29, 2000; March 26, 2001; October 3, 2001;
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <conio.h>   /* getch */
#include <direct.h>  /* chdir, getcwd, mkdir */
#include <io.h>      /* access */
#include <malloc.h>  /* _memavl */
#include <process.h> /* system */
#include <dos.h>     /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <math.h>     /* sqrt,pow */
#include <stdio.h>    /* printf,FILE,NULL,_IOFBF,fopen,fclose,fgets,puts,
                         fread,fwrite,setvbuf,fseek,SEEK_SET,SEEK_END,EOF,
                         ftell,size_t,sprintf,ferror,clearerr */
#include <stdlib.h>   /* abs,atof,atoi,NULL,malloc,free,size_t,exit,
                         _MAX_DRIVE,_MAX_DIR,_MAX_FNAME,_MAX_EXT,
                         _splitpath (non-ANSI),realloc */
#include <string.h>   /* strcpy,strlen,strstr,memset,strncpy,memcpy,strcat,
                         strlwr (non-ANSI),strupr (non-ANSI),strcmp,size_t */

/* Lucius application headers */
#include "assertjl.h" /* JL custom version of assert */
#include "gpr_io.h"   /* JL GPR data I/O library */
#include "jl_defs.h"  /* JL constants and macros */
#include "jl_util1.h" /* JL utility functions library */
#include "pcx_io.h"   /* JL PCX graphics I/O library */

/*************************** Manifest constants *****************************/
#define GPRSLICE_VER "1.10.03.01"
#define DATETIME "October 3, 2001"

#ifndef NUM_XFRM_METHODS
  #define NUM_XFRM_METHODS (5)
  #define XFRM_NONE (0)
  #define XFRM_ABSO (1)
  #define XFRM_SQRE (2)
  #define XFRM_INST (3)
  #define XFRM_POWR (4)
#endif

#ifndef NUM_SLICE_DIRECTIONS
  #define NUM_SLICE_DIRECTIONS (3)
  #define X_AXIS  (0)
  #define Y_AXIS  (1)
  #define Z_AXIS  (2)
#endif

/******************************* New data types *****************************/
/* Make sure the following structures are packed on byte boundaries! */
#if defined(_INTELC32_)
    #pragma align (VolCellStruct=1)
#elif defined(__BORLANDC__)
    #pragma option -a-
#elif defined(_MSC_VER)
    #pragma pack(push,old_packing)
    #pragma pack(1)
#elif defined(__ZTC__)
    #pragma ZTC align 1
#elif defined(__WATCOMC__)
    #pragma pack (1)
#endif

/* This structure holds the bin information */
struct VolCellStruct
{   float sum_wgts;  /* sum of weights used on the items */
    float sum;       /* sum of the items */
} ;  /* 10 bytes if tightly packed; probably 12 otherwise */

/* Restore 2- or 4-byte alignment */
#if defined(__BORLANDC__)
    #pragma option -a
#elif defined(_MSC_VER)
    #pragma pack(pop,old_packing)
#elif defined(__ZTC__)
    #pragma ZTC align
#elif defined(__WATCOMC__)
    #pragma pack (4)
#endif

/* This structure passes all necessary information between functions. */
struct SliceParamInfoStruct
{   long structID;                    /* SLIC (0x534C4943) */

    /* Input filenames */
    int  num_input_files;             /* number of input files */
    char cmd_filename[MAX_PATHLEN];   /* filename of command file */
    char **dat_infilename;            /* pointer to array of filenames */
    char inf_infilename[MAX_PATHLEN]; /* filename of an S&S HD info file */
    char mrk_infilename[MAX_PATHLEN]; /* filename of an MRK file */
    char xyz_infilename[MAX_PATHLEN]; /* filename of an XYZ file */

    /* Output filenames */
    char out_directory[MAX_PATHLEN];   /* directory for output files */
    char inf_outfilename[MAX_PATHLEN]; /* output information text file */
    char pcx_outfilename[MAX_PATHLEN]; /* template for series of output
                                          PCX graphics slice files - 6 char long */
    char bin_outfilename[MAX_PATHLEN]; /* template for series of output
                                          binary slice files - 6 char long */
    char txt_outfilename[MAX_PATHLEN]; /* template for series of output
                                           text slice files - 6 char long */
    char t3d_outfilename[MAX_PATHLEN]; /* output binary T3D file - 8 char long */
    char sld_outfilename[MAX_PATHLEN]; /* output binary SlicerDicer file -
                                           8 char long */
    int  overwrite_protect;            /* flag to permit overwriting existing
                                          disk files */

    /* Miscellaneous */
    char cwd[MAX_PATHLEN]; /* startup directory */

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
    long   first_proc_samp; /* start processing a trace at this sample,
                               indexed from 0; this allows for reserved
                               samples and trace headers; this value is
                               controlled by the data type: 2 for DZT files
                               128 for DT1 files, 240 for SGY files, and
                               "trace_header_bytes" for user-defined files.
                             Not used in here, kept for compatibility.
                             */
    long   data_samps;    /* total number of samples in each trace; this
                             value is determined from the data/info file */
    long   num_traces;    /* total number of traces in the input file */
    long   first_trace;   /* first trace from with XYZ value */
    long   last_trace;    /* last trace from with XYZ value */
    double total_time;    /* number of nanoseconds per trace */
    double ns_per_samp;   /* number of nanoseconds per sample (two-way travel) */
    int    num_ticks;     /* number of marked traces */
    long  *tick_tracenum; /* pointer to 1D array of marked trace numbers; [num_ticks] */
    double **tick_xyz;    /* pointer to 2D array of marker coordinates; [num_ticks][3] */
    double *trace_Xval;   /* pointer to 1D array of trace X-values */
    double *trace_Yval;   /* pointer to 1D array of trace Y-values */
    double *trace_Zval;   /* pointer to 1D array of trace Z-values */

    /* transforming descriptors */
    int    xfrm_method;   /* method of transforming amplitudes
                             0 = use original
                             1 = use absolute value
                             2 = use square
                             3 = use cube
                             4 = use instantaneous amplitude (from Hilbert)
                           */
    int    envelope;      /* flag to use envelope around amplitudes */
    int    background;    /* removes background trace */
    int    expand;        /* expands dynamic range of data */
    double multiply;      /* value to multiply volume amplitudes by */
    double Z_min;
    double Z_max;

    /* coordinates of volume stations */
    double X_start;       /* X-plane starting coordinate */
    double X_stop;        /* X-plane ending coordinate (may be less than _start) */
    double X_step;        /* X-plane uniform increment between columns */
    double Y_start;       /* Y-plane starting coordinate */
    double Y_stop;        /* Y-plane ending coordinate (may be less than _start) */
    double Y_step;        /* Y-plane uniform increment between rows */
    double Z_start;       /* Z-plane starting coordinate */
    double Z_stop;        /* Z-plane ending coordinate (may be less than _start) */
    double Z_step;        /* Z-plane uniform increment between layers */

    /* coordinates of volume corners */
    double X_first;       /* X coordinate left side of volume*/
    double X_last;        /* X coordinate right side of volume */
    double X_columns;     /* number of sections in X direction */
    double Y_first;       /* Y scoordinate left side of volume */
    double Y_last;        /* Y coordinate right side of volume */
    double Y_rows;        /* number of sections in Y direction */
    double Z_first;       /* Z coordinate bottom side of volume */
    double Z_last;        /* Z coordinate top side of volume */
    double Z_layers;      /* number of sections in Z direction */

    double start_time;    /* ns offset to first samp (neg. if time 0 > samp 0) */

    /* search box size */
    double box_Xsize;     /* X-direction width of search bin - column */
    double box_Ysize;     /* Y-direction width of search bin - row */
    double box_Zsize;     /* Z-direction width of search bin - layer */

    /* Input grid information (data will be converted to unsigned 16-bit ints) */
    int    created;
    unsigned short **grid; /* pointer to 2D grid storage area [num_cols][num_rows] */
    int    grid_num_cols;  /* number of traces to transform (not necessarily
                               same as in file); number of columns in grid */
    int    grid_num_rows;  /* number of samples in each trace plus the trace
                               header divided into "samples";
                               number of rows in grid */

    /* Output volume information; NOTE volumes are written to as a binary
       volume [x][y][z] where the first index (x) varies the most rapidly
       (for Slicer 3D) */
    unsigned short ***Volume;  /* pointer to 3D array of volume values */
    double mean_value;       /* middle value for volume datatype (32768) */
    int    vol_num_cols;     /* number of columns in volume (X direction) */
    int    vol_num_rows;     /* number of rows in volume (Y direction) */
    int    vol_num_layers;   /* number of layers in volume (Z direction) */
    struct VolCellStruct ***VolCell; /* pointer to 3D array of cell info */
    int    slice_direction;  /* plane is perpendicular to this axis */
    int    num_empty_cells;
} ;
/********************** Global variables from GPRSLICE.C *********************/
extern int Debug, Batch, ANSI_THERE;
extern FILE *log_file;
extern const char *GPRSLICE_CMDS[];
extern const char *GetParametersMsg[];
extern const char *GenerateSlicesMsg[];
extern const char *GetGprDataAsGridU16Msg[];
extern const char *SaveGprSliceDataMsg[];
extern const char *SaveGprSliceInfoMsg[];
extern const char *SplineXyzDataMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int   GetParameters(int argc,char *argv[],struct SliceParamInfoStruct *ParamInfoPtr);
void  PrintUsageMsg(void);
void  InitParameters(struct SliceParamInfoStruct *InfoPtr);
int   GetCmdFileArgs(struct SliceParamInfoStruct *InfoPtr);
void  DisplayParameters(struct SliceParamInfoStruct *InfoPtr);
void  DeallocInfoStruct(struct SliceParamInfoStruct *InfoPtr);
int   Envelope(int n,double *a,double *b);
void  RemAvgTrace16u(int num_cols,int num_rows,unsigned short **grid);
int   GenerateSlices(struct SliceParamInfoStruct *InfoPtr);
int   GetGprDataAsGridU16(int command,int file_num,struct SliceParamInfoStruct *InfoPtr);
void  jl_spectrum(unsigned char *pal);
void  jl_spectrum2(unsigned char *pal);
void  jl_spectrum3(unsigned char *pal);
int   SaveGprSliceData(struct SliceParamInfoStruct *InfoPtr);
int   SaveGprSliceInfo(struct SliceParamInfoStruct *InfoPtr);
int   SplineXyzData(struct SliceParamInfoStruct *InfoPtr);

#endif /* #ifndef GPRSLICE_H */
