#ifndef GPR_DFX_H
#define GPR_DFX_H
/* GPR_DFX.H
 * Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * December 30, 1996; August 28, 1997; September 4, 1997; August 18, 1999;
 * August 26, 1999; March 28, 2000
 *
 * This header file should be included whenever the library GPR_DFX.LIB
 * is linked with source/object code.
 *
 */
/********************** Include required header files ***********************/
/* ANSI-compatible headers */
#include <float.h>    /* FLT_MAX */
#include <math.h>     /* pow,cos,sqr */
#include <stdlib.h>   /* qsort,malloc,free,calloc */
#include <string.h>   /* memset */

/* Lucius application headers */
#include "assertjl.h" /* custom version of assert */
#include "jl_defs.h"  /* manifest constants common to differents functions */

/******************* Global variables used in gpr_dfx.c**********************/
extern const char *ApplyGridSpatMedFilterMsg[];
extern const char *ApplyGridTempMedFilterMsg[];
extern const char *ChangeGridRangeGainMsg[];
extern const char *InstAttribGridTracesMsg[];
extern const char *FftFilterGridTracesMsg[];
extern const char *RemGridGlobBckgrndMsg[];
extern const char *RemGridGlobForgrndMsg[];
extern const char *RemGridWindBckgrndMsg[];
extern const char *RemGridWindForgrndMsg[];
extern const char *SmoothGridHorizontallyMsg[];
extern const char *SmoothGridVerticallyMsg[];
extern const char *StackGridMsg[];

/*************************** Function prototypes ****************************/
void AdjustMeanGridTraces(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,double new_mean,void **grid);
int  ApplyGridSpatMedFilter(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,long win_len,void **grid);
int  ApplyGridTempMedFilter(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,long win_len,void **grid);
int  ChangeGridRangeGain(long num_cols,long num_rows,int first_proc_samp,
		int header_samps,int num_gain_off,double *gain_off,int num_gain_on,
		double *gain_on,int datatype,void **grid);
void EqualizeGridTraces(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,long base_trace,void **grid);
int  FftFilterGridTraces(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,int preproc,double ns_per_samp,
		double lo_freq_cutoff,double hi_freq_cutoff,void **grid);
int  InstAttribGridTraces(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,int attrib,void **grid);
int  RemGridGlobBckgrnd(long num_cols,long num_rows,long first_samp,
        int header_samps,int datatype,void **grid);
int  RemGridGlobForgrnd(long num_cols,long num_rows,long first_samp,
        int header_samps,int datatype,void **grid);
int  RemGridWindBckgrnd(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,long win_length,void **grid);
int  RemGridWindForgrnd(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,long win_length,void **grid);
void RescaleGrid(long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,double amp_scale,void **grid);
void SlideSamples (long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,int samp_slide,void **grid);
int  SmoothGridHorizontally(long num_cols,long num_rows,long first_samp,
        int header_samps,int datatype,long half_length,void **grid);
int  SmoothGridVertically(long num_cols,long num_rows,long first_samp,
        int header_samps,int datatype,long half_length,void **grid);
int  StackGrid(long *num_cols,long *num_rows,long first_samp,
		int header_samps,int datatype,long stack,void **grid);

/* following functions used by qsort() in ApplyGridSpatMedFilter() and
	 ApplyGridTempMedFilter() */
static int ucCompare(const void *elem1,const void *elem2);
static int cCompare(const void *elem1,const void *elem2);
static int usCompare(const void *elem1,const void *elem2);
static int sCompare(const void *elem1,const void *elem2);
static int ulCompare(const void *elem1,const void *elem2);
static int lCompare(const void *elem1,const void *elem2);
static int fCompare(const void *elem1,const void *elem2);
static int dCompare(const void *elem1,const void *elem2);

#endif /* #ifndef GPR_DFX_H */
