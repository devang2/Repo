#ifndef INTRPXYZ_H
#define INTRPXYZ_H  1.00
/*
 * INTRPXYZ.H  version 1.00
 *
 * Jeff Lucius  U.S. Geological Survey  Mineral Resources Team  Lakewood, CO
 * June 27, 1997
 *
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <conio.h>    /* getch */

/* ANSI-compatible headers */
#include <stdio.h>    /* puts,printf,FILE,fopen,fclose,fgets,sprintf,fprintf */
#include <stdlib.h>   /* atof,atoi,NULL,calloc,free,size_t */
#include <string.h>   /* strcpy,strchr,strstr,strlwr (non-ANSI),
                         strupr (non-ANSI),strncpy,strlen,strncat */

/* application headers */
#include "jl_util1.h" /* utility functions: Spline, TrimStr */

/*************************** Manifest constants *****************************/
#define INTRPXYZ_VER 1.00  /* current software version */

#ifndef NUM_INTERP
  #define NUM_INTERP   2
  #define LINEAR       1
  #define CUBIC_SPLINE 2
#endif

#ifndef NUM_VALUES
  #define NUM_VALUES   5
  #define X_VALUE      1
  #define Y_VALUE      2
  #define Z_VALUE      3
  #define T_VALUE      4
  #define P_VALUE      5
#endif

/******************************* New data types *****************************/
struct InterpXyzInfoStruct
{
	char cmd_filename[MAX_PATHLEN];
	char xyz_infilename[MAX_PATHLEN];  /* filename of input ASCII XYZ file */
	char xyz_outfilename[MAX_PATHLEN]; /* filename of output ASCII XYZ file */

	int interp_method;  /* 1 for linear, 2 for cubic spline */
	int indep_value;    /* 1=X, 2=Y, 3=Z, 4=traverse, 5=position number */

	int num_in;
	double **in_xyzp;   /* [num_in][4], the input X, Y, Z, and P values */
	double *in_trav;    /* [num_in], the calculated input traverse distances */
	double in_start;    /* copied from first value in in_xyzp or in_trav */
	double in_stop;     /* copied from last value in in_xyzp or in_trav */

	int num_out;
	double **out_xyz;   /* [num_out][3], the output X, Y, and Z values */
	double *out_indep;  /* [num_out], the output independant values */
	double out_start;   /* first value in out_indep */
	double out_stop;    /* last value in out_indep */
	double out_step;    /* increment between out_indep values */

	double *x;
	double *y;
	double *ynew;

} ;

/********************* Global variables from intrpxyz.c *********************/
extern int Debug;      /* if TRUE, turn debugging on */
extern int Batch;      /* if TRUE, supress interaction with user */
extern int ANSI_THERE; /* if TRUE, ANSI.SYS is loaded */

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
extern const char *INTRPXYZ_CMDS[];

/* These are message strings that match codes returned by functions */
extern const char *GetParametersMsg[];
extern const char *GetXyzDataMsg[];
extern const char *GetXyzpDataMsg[];
extern const char *InterpXyzMsg[];
extern const char *LinearInterpMsg[];
extern const char *SplineMsg[];

/*************************** Function prototypes ****************************/
void DeallocInfoStruct(struct InterpXyzInfoStruct *InfoPtr);
void DisplayParameters(struct InterpXyzInfoStruct *InfoPtr);
int  GetCmdFileArgs(struct InterpXyzInfoStruct *InfoPtr);
int  GetParameters(int argc, char *argv[],struct InterpXyzInfoStruct *InfoPtr);
int  GetXyzData(char *filename,int *num_ticks,double ***xyz);
int  GetXyzpData(char *filename,int *num_sets,double ***xyzp);
void InitParameters(struct InterpXyzInfoStruct *InfoPtr);
int  InterpXyz(struct InterpXyzInfoStruct *InfoPtr);
int  LinearInterp(long n,long nnew,double x[],double y[],double xnew[],double ynew[]);
void PrintUsageMsg(void);

#endif /* #ifndef GPR_DISP_H */
