#ifndef GPR_VELA_H
#define GPR_VELA_H  1.04.11.01
/* GPR_VELA.H */
/* Jeff Lucius  U.S. Geological Survey
 * April 12, 2001;
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
  1. Update the default command files GPR_VELA.CMD and GPR_VELA.CM_ with the
     new option and default value, and add descriptive comments for the option.
  2. Update this file, GPR_VELA.H.
     a. Add a new member to the ParamInfoStruct structure in
        "Global data types".  This structure passes all necessary information
        between functions.
     b. Add any new header files that are required and add the function
        prototype, if required, to the appropriate sections.
  3. Update the source code file GPR_VELA.C.
     a. Add the new command file option name to the end of GPR_VELA_CMDS[]
        array in "Global variables".
     b. Initialize the new structure member in InitParameters().
     c. Add the new option in GetCmdFileArgs() and update that function's
        copy of GPR_VELA_CMDS[] in its intro.
     d. Update DisplayParameters() to print out the new option's value.
     e. If appropriate: modify DeallocInfoStruct() if arrays are involved.
     f. If appropriate: add the new function(s) to the source code (and add
        the function prototype(s) and any new include files to GPR_VELA.H,
        see 2.b above).
  4. Recompile as specified in the discussion at the beginning of GPR_VELA.C.
 */
/*************************** Manifest constants *****************************/
#define GPR_VELA_VER   "1.04.12.01" /* current software version */
#define DATETIME       "April 12, 2001"

/******************************* New data types *****************************/
/* This structure passes all necessary information between functions. */

struct VelaParamInfoStruct
{   /* input filenames and data format types */
    char cmd_filename[MAX_PATHLEN];      /* the first input ASCII CMD file with
                                            keywords */
    char dzt_infilename[MAX_PATHLEN]; /* the binary GSSI DZT data file */
    int  input_format;    /* constant value indicating data storage format */
    int  input_datatype;  /* type of input data element */
    /* element types:
        1, 1-byte signed chars                           CHAR
       -1, unsigned chars                                UCHAR
        2, 2-byte signed ints or shorts                  SHORT
       -2, unsigned shorts                               USHORT
       -5, unsigned shorts but only first 12-bits used   USHORT12
        3, 4-byte signed ints or longs                   LONG
       -3, unsigned longs                                ULONG
       -6, unsigned longs but only first 24-bits used    ULONG24
        4, 4-byte floats                                 FLOAT
        8, 8-byte doubles                                DOUBLE
     */

    /* output filenames */
    char   dzt_outfilename[MAX_PATHLEN]; /* the dzt file NMO collection */
    char   inf_outfilename[MAX_PATHLEN]; /* the ASCII information file */

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
    double pos_start;     /* start antenna separation for first trace */
    double pos_final;     /* final antenna separation for last trace */
    double pos_step;      /* distance between traces */
    int    trace_first;   /* first trace to use from the file */
    int    trace_last;    /* last trace to use from the file */
    int    trace_num;     /* number of traces to use including trace_first */
    char   proc_hist[600]; /* will hold descriptive string of transforming values */

    /* NMO parameters */
    double vel_start;     /* velocity for first NMO in m/ns (0.01->0.3) */
    double vel_last;      /* velocity for first NMO in m/ns (0.01->0.3) */
    double vel_step;      /* increment between velocities */
    int    vel_num;       /* number of velocity intervals */
    double mute;          /* stretch percentage at which muting is started */
    int    num_nmos_displayed; /* number of NMOs to show on CRT */

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

    /* grid[][][] is an array of 2D data grids. The first one contains the
       input data. The remaining contain the constant velocity NMO data. */
    int    grid_created; /* flag, tracks allocation of "grid[][][]" */
    unsigned short ***grid; /* [num_grids][trace_num][num_samps] */
    int   num_grids;     /* number of grids; original plus gained data plus
                            one for each NMO  */
    int   num_traces;    /* number of traces used from the file;
                            number of columns in each grid */
    int   num_samps;     /* number of samples in each trace;
                            number of rows in grid */

    /* vgrid[][] is the array of stacked NMO CMPs (the velocity spectrum)
       - one "trace" or column for each NMO */
    int    vgrid_created;   /* flag, tracks allocation of "vgrid[][]" */
    unsigned short **vgrid; /* [num_nmos][num_samps] */
    int    num_nmos;        /* number of NMOs; number of columns in grid */
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
extern const char *GPR_VELA_CMDS[];

/* These are message strings that match codes returned by functions */
extern const char *VelaGetParametersMsg[];
extern const char *GetDataAsGrid16uMsg[];
extern const char *VelaPerformNMOMsg[];
extern const char *ChangeRangeGain16uMsg[];
extern const char *VelaSaveGprDataMsg[];
extern const char *VelaSaveDztDataMsg[];
extern const char *VelaSaveInfoMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int  VelaGetParameters(int argc, char *argv[],struct VelaParamInfoStruct *InfoPtr);
void VelaPrintUsageMsg(void);
void VelaInitParameters(struct VelaParamInfoStruct *InfoPtr);
int  VelaGetCmdFileArgs(struct VelaParamInfoStruct *InfoPtr);
void VelaDisplayParameters(struct VelaParamInfoStruct *InfoPtr);
int  GetDataAsGrid16u(int command,struct VelaParamInfoStruct *InfoPtr);
void VelaDeallocInfoStruct(struct VelaParamInfoStruct *InfoPtr);
int  VelaPerformNMO(struct VelaParamInfoStruct *InfoPtr);
int  VelaCalcVelSpectrum(int command,struct VelaParamInfoStruct *InfoPtr);
int  ChangeRangeGain16u(int num_samps,int start_samp,int stop_samp,
        int num_gain_off,double *gain_off,int num_gain_on,double *gain_on,
        double *gain_off_func,double *gain_on_func,unsigned short *ustrace);
int  VelaSaveGprData(struct VelaParamInfoStruct *InfoPtr);
int  VelaSaveDztData(struct VelaParamInfoStruct *InfoPtr);
int  VelaSaveInfo(struct VelaParamInfoStruct *InfoPtr);

#endif /* #ifndef GPR_VELA_H */
