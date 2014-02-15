#ifndef JL_UTIL1_H
#define JL_UTIL1_H
/* JL_UTIL1.H
 * Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * February 10, 1997; August 3, 1999; August 18, 1999;
 *
 * This header file should be included whenever the library JL_UTIL1.LIB
 * is linked with source/object code.
 *
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <dos.h>      /* REGS, int86 */
#include <i32.h>      /* _inbyte,_outbyte */
#include <direct.h>   /* chdir, mkdir; include <dir.h> in MSC */

/* ANSI-compatible headers */
#include <math.h>     /* cos, sin, sqrt, fabs */
#include <stdlib.h>   /* malloc, free, _splitpath, _MAX_DIR, _MAX_PATH,
                         _MAX_FNAME, _MAX_EXT */
#include <string.h>   /* strlen, strcpy, strchr, strcat */

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "jl_defs.h"  /* manifest constants common to differents functions */

/******************* Global variables used in jl_util1.c*********************/
extern const char *CreateDirMsg[];
extern const char *LinFitMsg[];
extern const char *SplineMsg[];
extern const char *Spline2Msg[];

/*************************** Function prototypes ****************************/
int  ANSI_there(void);
void BandPassFFT1D(long data_length,long hwindow,long low_cutoff,long high_cutoff,
        double *data);
int  CreateDir(char *dirname);
void InstAttribTrace(long length,int preproc,int attrib,double *dscan,
        double *cscan);
void FftFilterTrace(long trace_length,int preproc,long low_cutoff,
        long high_cutoff,double *dscan);
void FFT1D(double *data,long nn,int isign);
void GetDosVersion(int *mode,int *major_ver,int *minor_ver);
int  LinFit(int npts,int mode,double *X,double *Y,double *sigmaY,double *A,
        double *sigmaA,double *B,double *sigmaB,double *R);
void RealFFT1D(double *data,long nn,int isign);
void Sound(double freq,double duration);
int  Spline(long n,long nnew,double x[],double y[],double xnew[],double ynew[]);
int  Spline2(long n,long nnew,double x[],double y[],double xnew[],double ynew[]);
char *TrimStr(char *p);


#endif /* #ifndef JL_UTIL1_H */
