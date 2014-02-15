#ifndef GPR_CMPG_H
#define GPR_CMPG_H  1.01.23.02
/* GPR_CMPG.H */
/* Jeff Lucius  U.S. Geological Survey
 * January 23, 2002;
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <dos.h>      /* _dos_getdate,_dos_gettime,
                         struct dosdate_t,struct dostime_t */
#include <conio.h>    /* getch */
#include <malloc.h>   /* _memavl */

/* ANSI-compatible headers */
#include <math.h>     /* sqrt */
#include <stdio.h>    /* printf,FILE,NULL,fopen,fclose,puts,
                         fgets,size_t,sprintf */
#include <stdlib.h>   /* atof,atoi,atol,NULL,calloc,malloc,free,size_t */
#include <string.h>   /* strcpy,strchr,strstr,strlwr (non-ANSI),memset,strcmp,
                         strupr (non-ANSI),strncpy,strlen,strncat,strcat */

/* Lucius application headers */
#include "assertjl.h" /* JL custom version of assert */
#include "gpr_io.h"   /* JL GPR data I/O library */
#include "jl_defs.h"  /* JL constants and macros */

/*************************** Notes to developers ****************************/
/*
To add a new image or data processing technique, or any other command option:
  1. Update the default command files GPR_CMPG.CMD and GPR_CMPG.CM_ with the
     new option and default value, and add descriptive comments for the option.
  2. Update this file, GPR_CMPG.H.
     a. Add a new member to the ParamInfoStruct structure in
        "Global data types".  This structure passes all necessary information
        between functions.
     b. Add any new header files that are required and add the function
        prototype, if required, to the appropriate sections.
  3. Update the source code file GPR_CMPG.C.
     a. Add the new command file option name to the end of GPR_CMPG_CMDS[]
        array in "Global variables".
     b. Initialize the new structure member in InitParameters().
     c. Add the new option in GetCmdFileArgs() and update that function's
        copy of GPR_CMPG_CMDS[] in its intro.
     d. Update DisplayParameters() to print out the new option's value.
     e. If appropriate: modify DeallocInfoStruct() if arrays are involved.
     f. If appropriate: add the new function(s) to the source code (and add
        the function prototype(s) and any new include files to GPR_CMPG.H,
        see 2.b above).
  4. Recompile as specified in the discussion at the beginning of GPR_CMPG.C.
 */
/*************************** Manifest constants *****************************/
#define GPR_CMPG_VER   "1.01.23.02" /* current software version */
#define DATETIME       "January 23, 2002"

/******************************* New data types *****************************/
/* This structure passes all necessary information between functions. */

struct CmpgParamInfoStruct
{   /* input filenames and data format types */
    char cmd_filename[MAX_PATHLEN];   /* filename of command file */
    int  num_input_files;             /* number of input files */
    int  file_num;                    /* current file being processed, from 0 */
    char **dat_infilename;            /* pointer to array of filenames */

    /* Output filenames */
    char   dzt_outfilename[MAX_PATHLEN]; /* the dzt file NMO collection */
    char   inf_outfilename[MAX_PATHLEN]; /* the ASCII information file */

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
    /* data description */
    int    display_none;  /* set to "TRUE" to supress displaying parameters
                             when program starts up */
    int    file_header_bytes;  /* number of bytes in file header */
    int    trace_header_bytes; /* number of bytes in each trace header */
    int    channel;       /* channel number in multi-channel data sets (0 - 3);
                              indexed from 0; default is 0 */
    int    ant_freq;      /* frequency in MHz of the radar antenna */
    long   total_traces;  /* total number of traces in the file */
    int    samp_first;    /* the time zero sample number, indexed from 0, can be neg. */
    int    total_samps;   /* total number of samples in each trace (grid row size)*/
    double total_time;    /* number of nanoseconds per trace */
    double ns_per_samp;   /* number of nanoseconds per sample */

    double offset_first;  /* distance between antennas in first file (meters),
                             default is -1 */
    double offset_incr;   /* the uniform increment in offset between files,
                             default is -1 */
    double offset_final;  /* distance between antennas in last file (meters) */
    double pos_start;     /* horizontal location of first trace in every file,
                             assumed to be mid-way between antennas,
                             default is 0.0 */
    double pos_step;      /* uniform separation between traces in each file,
                             default is 0.0 */
    double pos_final;

    int    trace_first;   /* first trace to use from the file */
    int    trace_last;    /* last trace to use from the file */
    int    trace_num;     /* number of traces to use including trace_first */
    char   proc_hist[600]; /* will hold descriptive string of transforming values */

    /* NMO parameters */
    double velocity;      /* velocity for NMO in m/ns (0.01 -> 0.3) */
    double mute;          /* stretch percentage at which muting is started */

    /* Range gain parameters */
    int rg_start_trace;   /* start gain at this trace (goes to last trace) */
    int rg_start_samp;    /* start gain at this samp on first trace */
    int rg_stop_samp;     /* stop gain at this samp on first trace */
    int rg_step;          /* move start and stop samps down by this much for
                             every trace after the start trace */
    int    rg_num_on;     /* = 0 or 2; fixed at max of 2 for now */
    double rg_on[2];      /* in db; 6 db = 2X; 12 db = 4X; etc. */
    int    rg_num_off;    /* = 0; not user adjustable */
    double rg_off[2];     /* not user adjustable */

    /* grid[][] is a 2D grid that is the stacked CMP section*/
    int    grid_created;   /* flag; tracks allocation of "grid[][]" */
    unsigned short **grid; /* [num_traces][num_samps] */
    int   num_traces;      /* number of traces used from the file;
                              number of traces in first grid */
    int   num_samps;       /* number of samples in each trace;
                              number of rows in grid */

    /* vgrid[][][] is the array of CMP gathers
           [0 - N-1]  = [num_input_files][num_samps] - CMP gathers after mute
           [N - 2N-1] = [num_input_files][num_samps] - CMP gathers after NMO
           where    N = trace_num (or num_traces)
            num_vgrids = 2N
     */
    int    vgrid_created;    /* flag, tracks allocation of "vgrid[][][]" */
    unsigned short ***vgrid; /* [num_vgrids][num_files][num_samps] */
    int   num_vgrids;        /* number of grids in vgrid[][][] */
    int   num_files;         /* number of stacks, folds, or files;
                                number of columns in each grid */

    /* dgrid is the array input data files */
    int    dgrid_created;    /* flag, tracks allocation of "vgrid[][][]" */
    unsigned short ***dgrid; /* [num_input_files][num_traces][num_samps] */

} ;

/********************* Global variables from gpr_vela.c *********************/
extern int Debug;             /* turns debugging on */
extern int Batch;             /* supresses interaction with user */
extern int ANSI_THERE;

/* message file that all functions have access to */
extern FILE *log_file;        /* pointer to FILE struct */

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
extern const char *GPR_CMPG_CMDS[];

/* These are message strings that match codes returned by functions */
extern const char *CmpgGetParametersMsg[];
extern const char *CmpgAllocGridsMsg[];
extern const char *CmpgGetDataAsGrid16uMsg[];
extern const char *CmpgSortDataMsg[];
extern const char *CmpgPerformNMOMsg[];
extern const char *ChangeRangeGain16uMsg[];
extern const char *CmpgCalcCmpStackedSectionMsg[];
extern const char *CmpgSaveGprDataMsg[];
extern const char *CmpgSaveDztDataMsg[];
extern const char *CmpgSaveInfoMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int  CmpgGetParameters(int argc, char *argv[],struct CmpgParamInfoStruct *InfoPtr);
void CmpgPrintUsageMsg(void);
void CmpgInitParameters(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgGetCmdFileArgs(struct CmpgParamInfoStruct *InfoPtr);
void CmpgDisplayParameters(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgAllocGrids(struct CmpgParamInfoStruct *InfoPtr);
void CmpgDeallocGrids(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgGetDataAsGrid16u(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgSortData(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgPerformNMO(struct CmpgParamInfoStruct *InfoPtr);
int  ChangeRangeGain16u(int num_samps,int start_samp,int stop_samp,
        int num_gain_off,double *gain_off,int num_gain_on,double *gain_on,
        double *gain_off_func,double *gain_on_func,unsigned short *ustrace);
int  CmpgCalcCmpStackedSection(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgSaveGprData(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgSaveDztData(struct CmpgParamInfoStruct *InfoPtr);
int  CmpgSaveInfo(struct CmpgParamInfoStruct *InfoPtr);

#endif /* #ifndef GPR_CMPG_H */
