#ifndef GPR_DISP_H
#define GPR_DISP_H  2.11.02.01
/* GPR_DISP.H */
/* Jeff Lucius  U.S. Geological Survey
 * June 24, 1996; July 15, 1999; August 23, 1999; September 3, 1999;
 * August 28, 2000; September 6, 2000; November 6, 2001;
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <dos.h>      /* REGS,int86,_dos_getdate,_dos_gettime,struct dosdate_t,
                         struct dostime_t */
#include <conio.h>    /* getch */
#include <malloc.h>   /* _memavl */

/* ANSI-compatible headers */
#include <ctype.h>    /* isdigit */
#include <math.h>     /* sqrt, cos */
#include <stdio.h>    /* printf,FILE,NULL,fopen,fclose,puts,fseek,
                         fgets,size_t,ftell,sprintf,SEEK_SET,rewind,EOF */
#include <stdlib.h>   /* atof,atoi,atol,NULL,calloc,malloc,free,size_t */
#include <string.h>   /* strcpy,strchr,strstr,strlwr (non-ANSI),memset,
                         strupr (non-ANSI),strncpy,strlen,strncat */
#include <time.h>     /* struct tm,time_t,localtime */

/* Lucius application headers */
#include "assertjl.h" /* JL custom version of assert */
#include "gpr_ifx.h"  /* JL GPR image-table manipulation library */
#include "gpr_io.h"   /* JL GPR data I/O library */
#include "hpgl.h"     /* Olhoeft's HPGL 32-bit graphics library */
#include "jl_defs.h"  /* JL constants and macros */
#include "pcx_io.h"   /* JL PCX file I/O library */

/*************************** Notes to developers ****************************/
/*
To add a new image or data processing technique, or any other command option:
  1. Update the default command files GPR_DISP.CMD and GPR_DISP.CM_ with the
     new option and default value, and add descriptive comments for the option.
  2. Update this file, GPR_DISP.H.
     a. Add a new member to the ParamInfoStruct structure in
        "Global data types".  This structure passes all necessary information
        between functions.
     b. Add any new header files that are required and add the function
        prototype, if required, to the appropriate sections.
  3. Update the source code file GPR_DISP.C.
     a. Add the new command file option name to the end of GPR_DISP_CMDS[]
        array in "Global variables".
     b. Initialize the new structure member in InitParameters().
     c. Add the new option in GetCmdFileArgs() and update that function's
        copy of GPR_DISP_CMDS[] in its intro.
     d. Update DisplayParameters() to print out the new option's value.
     e. If appropriate: modify DeallocInfoStruct() if arrays are involved.
     f. If appropriate: add a new function call in DisplayImage8() and
        DisplayWiggle8(). If the function returns an error code, add a new
        message to the end of DisplayImage8Msg[] and DisplayWiggle8Msg[] arrays
        in GPR_DISP.H and to the copy of those arrays in the function intros.
        Be sure the error code matches the messages position within the array.
     g. If appropriate: add the new function(s) to the source code (and add
        the function prototype(s) and any new include files to GPR_DISP.H,
        see 2.b above).
  4. Recompile as specified in the discussion at the beginning of GPR_DISP.C.
 */
/*************************** Manifest constants *****************************/
#define GPR_DISP_VER   "2.11.06.01" /* current software version */
#define DATETIME       "November 6, 2001"

#define MAX_LAYERS        10  /* maximum number of depth model layers */
#define MAX_VIEWPORTS     20  /* maximum number of viewports (data sets) */
#define MAX_CAPTION_LEN 2000  /* maximum number of characters for figure
                                 captions */
#define MAX_PAGENUM_LEN  140  /* maximum number of characters for page numbers
                                    or running heads */

#ifndef NUM_STORAGE_FORMATS
  #define NUM_STORAGE_FORMATS 4
  #define DZT      1          /* GSSI ver 3.x to 5.x */
  #define DT1      2          /* Sensors & Software PulseEKKO */
  #define SGY      3          /* SEG SEG-Y */
  #define USER_DEF 4          /* user defined (must always be 4) */
#endif

#ifndef NUM_COORD_MODES
  #define NUM_COORD_MODES   5 /* number of coordinate modes avaliable */
                              /*   horizontal      vertical           */
  #define TRACE_TIME        1 /*  trace number        ns              */
  #define DISTANCE_DISTANCE 2 /*     meters         meters            */
  #define TIME_TIME         3 /*    seconds           ns              */
  #define DISTANCE_TIME     4 /*     meters           ns              */
  #define TRACE_SAMPLE      5 /*  trace number   sample number        */
#endif

/******************************* New data types *****************************/
/* This structure passes all necessary information between functions. */

struct DispParamInfoStruct
{   /* input filenames and data format types */
    char cmd_filename[MAX_PATHLEN];      /* the first input ASCII CMD file with
                                            keywords */
    char next_cmd_filename[MAX_PATHLEN]; /* the next input ASCII CMD file; NULL
                                            (or empty string) if no more */
    char override_cmd_filename[MAX_PATHLEN]; /* overide keyword file */
    char dat_infilename[MAX_PATHLEN]; /* the binary data file */
    char inf_infilename[MAX_PATHLEN]; /* the ASCII information file */
    char lbl_infilename[MAX_PATHLEN]; /* the input ASCII LBL label file*/
    char mrk_infilename[MAX_PATHLEN]; /* the input ASCII marked trace file */
    char xyz_infilename[MAX_PATHLEN]; /* the input ASCII XYZ file for marked
                                         traces */
    int  input_format;    /* constant value indicating data storage format */
    int  input_datatype;  /* type of input data element when required */
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
    double max_data_val;  /* maximum value to use for scaling floats */
    double min_data_val;  /* minimum value to use for scaling floats */
    int    if_image;      /* set to "TRUE" if file is an "image" file
                             with data stored row-by-row */

    /* output filenames */
    char   pcx_outfilename[MAX_PATHLEN]; /* output screen image PCX file */
    char   eps_outfilename[MAX_PATHLEN]; /* output screen image EPS file */
    int    eps_copies; /* use this parameter to specify multiple hard copies */
    int    save_pcx;   /* flag, save screen image as PCX binary file if TRUE */
    int    save_eps;   /* flag, save screen image as EPS binary file if TRUE */

    /* data description */
    int    display_all;   /* set to "TRUE" to display parameters for all CMD
                             files, otherwise only first CMD file displayed */
    int    display_none;  /* set to "TRUE" to supress displaying parameters
                             when program starts up */
    int    file_header_bytes;  /* number of bytes in file header */
    int    trace_header_bytes; /* number of bytes in each trace header */
    int    channel;       /* channel number in multi-channel data sets (0 - 3);
                              indexed from 0; default is 0.
                              Note that in CMD file user uses 1 to 4! */
    int    ant_freq;      /* frequency in MHz of the radar antenna */
    long   first_trace;   /* start displaying at this trace (inclusive) */
    long   last_trace;    /* stop displaying at this trace (inclusive) */
    long   total_traces;  /* total number of traces in the file */
    long   skip_traces;   /* number of traces to skip for every one read */
    int    lock_first_samp; /* if TRUE, info file can't override first_samp */
    long   first_samp;    /* first sample point to display from each trace */
    long   last_samp;     /* last sample point to display from each trace */
    long   total_samps;   /* total number of samples in each trace */
    double total_time;    /* number of nanoseconds per trace */
    double ns_per_samp;   /* number of nanoseconds per sample (two-way travel) */
    double trace_per_sec; /* number of traces recorded per second */
    double trace_per_m;   /* number of traces recorded per meter */
    double start_pos;     /* start location for first trace */
    double final_pos;     /* final location for last trace */
    double step_size;     /* distance between traces */
    double first_trace_time; /* earliest trace time (in seconds) to display */
    double last_trace_time;  /* latest trace time (in seconds) to display */
    double first_samp_time;  /* earliest sample time (in nss) to display */
    double last_samp_time;   /* latest sample time (in ns) to display */
    long   num_bad;       /* number of traces NOT to use out of data file */
    long  *bad_traces;    /* [num_bad], the bad trace numbers if num_bad > 0 */
    long   num_ticks;     /* number of markers in data */
    long  *tick_tracenum; /* [num_ticks], the marked trace numbers */
    double **tick_xyz;    /* [num_ticks][3], coordinates for marked traces */
    int    num_layers;    /* number of layers in depth-correction model */
    double *layer_rdp;    /* [num_layers], the RDP for each layer */
    int    *layer_mode;   /* [num_layers], determines whether the bottom of a
                             layer is:
                                at uniform depth below the surface (d or 1),
                                at a uniform elevation (e or 2), or
                                infinite in depth (i or 3) (lowest layer in a
                                multilayer model).
                             NOTE: This value ignored if only one layer. */
    double *layer_val;    /* [num_layers], the depth or elevation corresponding
                             to the selected layer mode, in user units */
    double *Hvals;        /* [num_cols], the horizontal-direction location for
                             the first sample of each trace in image[][] */
    double *Vvals;        /* [num_rows], the vertical-direction location for
                             all samples of each trace in image[][] if
                             coord_mode == 1, 3, 4, or 5; or vertical offset of
                             each sample from the surface if coord_mode == 2
                             and only one model layer */
    double *Zvals;        /* [num_cols], the vertical-direction location for
                             the first sample of each trace in image[][] if
                             coord_mode == 2 */

    /* data manipulation */
    int    change_gain;   /* flag to add/remove range gain */
    int    num_gain_off;  /* number of points for removing gain */
    double *gain_off;     /* [num_gain_off] */
    int    num_gain_on;   /* number of points for adding gain */
    double *gain_on;      /* [num_gain_off] */
    int    background;    /* remove the average of all traces */
    int    abs_val;       /* converts to absolute values */
    int    square;        /* square (then rescale) the values */
    int    inst_amp;      /* calcs instantaneous amplitude */
    int    inst_pow;      /* calcs instantaneous power */
    int    envelope;      /* wraps inst. amps. in an envelop */
    int    stretch;       /* percent to enhance contrasts in image
                             (range 0 to 99, normally 1-4) - histogram */

    /* look-up table processing */
    int    range;         /* grayscale range expander/contractor;
                             0 to use EPC-style display */
    int    local_stretch; /* TRUE if a local contrast stretch is requested */
    double local_offset;  /* the half-width of the local stretch window from
                             the midpoint of the gray range (127.5) */
    double local_change;  /* the amount to add to the high end of the local
                             contrast window and to subtract from the low end */
    int    brightness;    /* brightens or darkens image; range is +/- 128;
                             default is 0 */
    int    negative;      /* reverse gray scale to go from white to black
                             if TRUE; default is FALSE */

    /* marked trace display */
    int    show_markers;      /* display tick marks on the data */
    int    show_marker_width; /* width of marker line in screen pixels */

    /* EPC-style display */
    int    epc_threshold;     /* distance from midpoint to start darkening */
    int    epc_contrast;      /* distance from midpoint to black */
    double epc_gain;          /* multiplies contrast */
    int    epc_sign;          /* if >0, values below midpoint are white;
                                 if <0, values above midpoint are white */

    /* wiggle trace display */
    int    display_wiggle;     /* flag to use wiggle mode */
    int    num_wiggles;        /* if display_wiggle == 0, then size of array
                                  of traces to overlay with wiggles */
    double  **wiggle_traceval; /* [num_wiggles][2] where
                                     [0] = trace location in user units
                                  [1] = trace location horizontal pixel */
    int    wiggle_color;
    int    wiggle_fill;
    double wiggle_width;
    double wiggle_clip;

    /* EPS output controls */
    int    portrait;            /* FALSE for landscape orientation */
    int    use_11x17;           /* format for 11 by 17 inch paper if TRUE
                                   otherwise format for 8.5 by 11 inch */
    int    use_designjet750;    /* format EPS output for HP Design Jet 750 */
    char   eps_select_font[52]; /* xxact name of resident PS font */
    int    caption_font_size;   /* point size (72 points per inch) */
    double caption_left_edge;   /* inches from "left" side of paper */
    double caption_bottom_edge; /* inches from "bottom" side of paper */
    int    caption_long_edge;   /* set to "TRUE" to print on long edge,
                                   otherwise caption on short edge */
    char   *caption;            /* pointer to string to hold figure caption,
                                   to be allocated */
    int    pagenum_font_size;   /* point size (72 points per inch) */
    double pagenum_left_edge;   /* inches from "left" side of paper */
    double pagenum_bottom_edge; /* inches from "bottom" side of paper */
    char   *pagenum;            /* pointer to string holding page "number" */

    /* CRT/screen controls/info */
    int    video_mode; /* VGA display mode */
    double hp_x;       /* "width" (horizontal "CRT" dimension)  */
    double hp_y;       /* "height" (vertical "CRT" dimension) */
    double hp_xoff;
    double hp_yoff;
    double VX1,VX2;    /* data window, left and right viewport coordinates;
                          In landscape mode (portrait == 0) ranges are 0.0 to
                          133.333; in portrait mode, ranges are 0.0 to 75.0 */
    double VY1,VY2;    /* data window, bottom and top viewport coordinates;
                          range is 0.0 to 100.0 in both portrait and landscape
                          orientations */
    double left,right; /* data window, left and right user-unit coordinates */
    double bottom,top; /* data window, bottom and top user-unit coordinates */
    double vert_exag;  /* vertical exaggeration of display (coord_mode = 2) */
    char   hpgl_select_font[12]; /* select from available fonts in libhpgl.lib
                                    default is "romtrplx" */

    /* coordinate modes */
    int    coord_mode;  /* determines data window coordinate systems */
/* mode   horizontal     vertical          description
   0          -              -             invalid mode
   1      trace number   sample time (ns)  "raw" traces, default
   2      distance (m)   distance (m)      geometrically corrected
   3      time (sec)     time (ns)         stationary antenna
   4      distance (m)   time (ns)         horizontal rubbersheeting,
                         no topo correction
   5      trace number   sample number     "really raw" data, sample rate
                         unknown

    These modes are for 2-D displays.
    Distance units are in meters.
    Time units are nanoseconds (vertically) or seconds (horizontally).
    Update NUM_COORD_MODES above if adding more modes.
*/
    int horiz_mode;   /* determines what coordinates are used for the
         horizontal direction for coord_mode == 2.
         Choices are (default is 3):
         1 = X-coordinates (Xvals array)
         2 = Y-coordinates (Yvals array)
         3 = traverse distance coordinates, sqrt(X*X + Y*Y)
           */
    double horiz_start;    /* horizontal location to start using data */
    double horiz_stop;     /* horizontal location to stop using data */

    /* Velocity hyperbola */
    int plot_hyperbola;    /* "true" to plot a velocity hyperbola */
    double ant_sep;        /* distance (m) between antenna centers */
    double obj_depth;      /* distance (m) to object from surface */
    double obj_radius;     /* radius (m) of object - circular */
    double obj_loc;        /* loc of object in use horizontal coordinates */
    double hyp_vel;        /* GPR wave velocity on m/ns */
    int    hyp_color;      /* pen color for drawing line */

    /* image grid information */
    int    created;        /* flag, tracks allocation of "image[][]" */
    unsigned char **image; /* [num_rows][num_cols]; pointer to 2D image */
    long   num_cols;       /* number of traces to process (not necessarily
                              same as in file); number of columns in grid */
    long   num_rows;       /* number of samples in each trace to be saved;
                              number of rows in grid */

    /* grid line overlay controls */
    int    add_hlines;     /* !=0 to overlay horizontal lines in window */
    double hline_start;    /* start horizontal lines here */
    double hline_int;      /* interval in user units between horiz. lines */
    double hline_stop;     /* stop horizontal lines here */
    int    hline_style;    /* horizontal line style */
    int    add_vlines;     /* !=0 to overlay vertical lines in window */
    double vline_start;    /* start vertical lines here */
    double vline_int;      /* interval in user units between vertical lines */
    double vline_stop;     /* stop vertical lines here */
    int    vline_style;    /* vertical line style */

    /* left axis display/annotation controls */
    char   laxis_title[MAX_TITLE_LEN]; /* title for axis */
    double laxis_title_offset;
    double laxis_max;        /* maximum (top) value for vertical axis */
    double laxis_min;        /* minimum (bottom) value for vertical axis */
    int    laxis_tick_show;  /* display ticks if TRUE; only ticks in the range
                                laxis_min to laxis_max will be shown */
    double laxis_tick_start; /* minimum value for first tick */
    double laxis_tick_int;   /* interval between ticks */
    double laxis_tick_abs_min; /* no ticks displayed "before" this value */
    double laxis_tick_abs_max; /* no ticks displayed "after" this value */
    int    laxis_tick_num;   /* suggested number of ticks */
    int    laxis_tick_ano;   /* annotate ticks if TRUE */
    int    laxis_tick_skip;  /* number of ticks to skip between annotations
                                annotated ticks will be longer */
    int    laxis_tick_mid_skip;
    int    laxis_ano_left;   /* places to left of decimal (up to 6) */
    int    laxis_ano_right;  /* places to right of decimal (up to 3) */
    double laxis_title_size; /* CSIZE for title */
    double laxis_ano_size;   /* CSIZE for tick annotation */

    /* right axis display/annotation controls */
    char   raxis_title[MAX_TITLE_LEN]; /* title for axis */
    double raxis_title_offset;
    double raxis_max;        /* maximum (top) value for right vertical axis */
    double raxis_min;        /* minimum (bottom) value for right vert. axis */
    int    raxis_tick_show;  /* display ticks if TRUE; only ticks in the range
                                raxis_min to raxis_max will be shown */
    double raxis_tick_start; /* minimum value for first tick */
    double raxis_tick_int;   /* interval between ticks */
    double raxis_tick_abs_min; /* no ticks displayed "before" this value */
    double raxis_tick_abs_max; /* no ticks displayed "after" this value */
    int    raxis_tick_num;   /* suggested number of ticks */
    int    raxis_tick_ano;   /* annotate ticks if TRUE */
    int    raxis_tick_skip;  /* number of ticks to skip between annotations
                                annotated ticks will be longer */
    int    raxis_tick_mid_skip;
    int    raxis_ano_left;   /* places to left of decimal (up to 6) */
    int    raxis_ano_right;  /* places to right of decimal (up to 3) */
    double raxis_title_size; /* CSIZE for title */
    double raxis_ano_size;   /* CSIZE for tick annotation */

    /* top axis display/annotation controls */
    char   title[MAX_TITLE_LEN]; /* title for display */
    char   taxis_title[MAX_TITLE_LEN]; /* title for axis */
    double taxis_title_offset;
    double taxis_max;        /* maximum (right) value for top horiz. axis */
    double taxis_min;        /* minimum (left) value for top horizontal axis */
    int    taxis_tick_show;  /* display ticks if TRUE; only ticks in the range
                                taxis_min to taxis_max will be shown */
    double taxis_tick_start; /* minimum value for first tick */
    double taxis_tick_int;   /* interval between ticks */
    double taxis_tick_abs_min; /* no ticks displayed "before" this value */
    double taxis_tick_abs_max; /* no ticks displayed "after" this value */
    int    taxis_tick_num;   /* suggested number of ticks */
    int    taxis_tick_ano;   /* annotate ticks if TRUE */
    int    taxis_tick_skip;  /* number of ticks to skip between annotations
                                annotated ticks will be longer */
    int    taxis_tick_mid_skip;
    int    taxis_ano_left;   /* places to left of decimal (up to 6) */
    int    taxis_ano_right;  /* places to right of decimal (up to 3) */
    double taxis_title_size; /* CSIZE for title */
    double taxis_ano_size;   /* CSIZE for tick annotation */

    /* bottom axis display/annotation controls */
    char   baxis_title[MAX_TITLE_LEN]; /* title for axis */
    double baxis_title_offset;
    double baxis_max;        /* maximum (right) value for bottom horiz. axis */
    double baxis_min;        /* minimum (left) value for bottom horiz. axis */
    int    baxis_tick_show;  /* display ticks if TRUE; only ticks in the range
                                baxis_min to baxis_max will be shown */
    double baxis_tick_start; /* minimum value for first tick */
    double baxis_tick_int;   /* interval between ticks */
    double baxis_tick_abs_min; /* no ticks displayed "before" this value */
    double baxis_tick_abs_max; /* no ticks displayed "after" this value */
    int    baxis_tick_num;   /* suggested number of ticks */
    int    baxis_tick_ano;   /* annotate ticks if TRUE */
    int    baxis_tick_skip;  /* number of ticks to skip between annotations
                annotated ticks will be longer */
    int    baxis_tick_mid_skip;
    int    baxis_ano_left;   /* places to left of decimal (up to 6) */
    int    baxis_ano_right;  /* places to right of decimal (up to 3) */
    double baxis_title_size; /* CSIZE for title */
    double baxis_ano_size;   /* CSIZE for tick annotation */
} ;

/********************* Global variables from gpr_disp.c *********************/
extern int Debug;             /* turns debugging on */
extern int Batch;             /* supresses interaction with user */
extern int Batchset,Debugset; /* Only use values from first command file */
extern int Videoset;          /* Only use values from first command file */
extern int save_eps,save_pcx; /* Only use values from first command file */

/* message file that all functions have access to */
extern FILE *log_file;        /* pointer to FILE struct */

/* The array below is used to read in values from an ASCII file that contains
 * HPGL expressions as "C" code (typically a "LBL" file).
 */
extern const char *HPGL_LBLS[];

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
extern const char *GPR_DISP_CMDS[];

/* These are message strings that match codes returned by functions */
extern const char *GetParametersMsg[];
extern const char *GetDataAsImage8Msg[];
extern const char *GetCoordRangesMsg[];
extern const char *GetDataLimitsMsg[];
extern const char *HistStretchImage8uMsg[];
extern const char *ChangeRangeGain8uMsg[];
extern const char *DisplayImage8Msg[];
extern const char *DisplayWiggle8Msg[];
extern const char *PlotSelectWiggles8Msg[];
extern const char *PlotHyperbola8Msg[];
extern const char *LabelViewportsEPSMsg[];
extern const char *LabelViewportsPCXMsg[];
extern const char *SetDataViewportMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int  GetParameters(int argc, char *argv[],struct DispParamInfoStruct *InfoPtr);
void PrintUsageMsg(void);
void InitParameters(struct DispParamInfoStruct *InfoPtr);
int  GetCmdFileArgs(struct DispParamInfoStruct *InfoPtr);
int  GetDataAsImage8(struct DispParamInfoStruct *InfoPtr);
int  GetOtherInfo(struct DispParamInfoStruct *InfoPtr);
void DeallocInfoStruct(struct DispParamInfoStruct *InfoPtr);
void DisplayParameters(struct DispParamInfoStruct *InfoPtr);
int  GetCoordRanges(struct DispParamInfoStruct *InfoPtr);
int  GetDataLimits(struct DispParamInfoStruct *InfoPtr);
void GPR_DISP_HelpScreen(void);

/* functions that use libhpgl.lib but must remain with this source */
int  InitGraphicsARRAY(int backcolor,int vmode,int portrait,int *vcols,
       int *vrows);
int  SetDataViewport(int vcols,int vrows,double vert_exag,double xmin,
       double xmax,double ymin,double ymax,double *X1,double *X2,double *Y1,
       double *Y2);
int  DisplayImage8(int vcols,int vrows,struct DispParamInfoStruct *InfoPtr);
int  DisplayWiggle8(int vrows,struct DispParamInfoStruct *InfoPtr);
int  PlotSelectWiggles8(int vrows,struct DispParamInfoStruct *InfoPtr);
int  PlotHyperbola8 (struct DispParamInfoStruct *InfoPtr);
int  LabelViewportsEPS(int vcols,int vrows,int backcolor,int num_viewports,
       struct DispParamInfoStruct *InfoPtr[]);
int  LabelViewportsPCX(int vcols,int vrows,int backcolor,int num_viewports,
       struct DispParamInfoStruct *InfoPtr[]);
int  DrawAxis(int side,int pen_number,int vcols,int vrows,
       int tick_show,int tick_num,int tick_ano,int tick_skip,int tick_mid_skip,
       int ano_left,int ano_right,double axis_min,double axis_max,
       double tick_start,double tick_int,double tick_abs_min,
       double tick_abs_max,double title_size,double title_offset,
       double ano_size,double VX1,double VX2,double VY1,double VY2,
       char *title);
int  SetLabel(int *left,int *right,double val,char *str);
int  GetLblFileArgs(char *filename);
void jl_spectrum(unsigned char *pal);
void jl_spectrum2(unsigned char *pal);
void jl_spectrum3(unsigned char *pal);
void TerminateGraphicsARRAY(int vrows);

/* data manipulation functions (eventually in gpr_dfx.c for gpr_dfx.lib?) */
int  ChangeRangeGain8u(long num_cols,long num_rows,int num_gain_off,
       double *gain_off,int num_gain_on,double *gain_on,
       unsigned char **image);
void AbsValImage8u(long num_cols,long num_rows,unsigned char **image);
void SquareImage8u(long num_cols,long num_rows,unsigned char **image);
int  InstAmpImage8u(long num_cols,long num_rows,int envelope,
       unsigned char **image);
int  InstPowImage8u(long num_cols,long num_rows,int envelope,
       unsigned char **image);
int  Envelope(int n,double *a,double *b);
int  HistStretchImage8u(long num_cols,long num_rows,int percent,
       unsigned char **image);
void RemAvgColumn8u(long num_cols,long num_rows,unsigned char **image);

#endif /* #ifndef GPR_DISP_H */
