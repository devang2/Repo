#ifndef GPR_IFX_H
#define GPR_IFX_H
/* GPR_IFX.H
 * Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * November 29, 1995
 *
 * This header file should be included whenever the library GPR_IFX.LIB
 * is linked with source/object code.
 *
 */
/********************** Include required header files ***********************/
/* non-ANSI  headers */
#include <conio.h>    /* getch */

/* ANSI-compatible headers */
#include <stdio.h>    /* puts,printf - for usage by Debug blocks */
#include <string.h>   /* strlen - used only by TrimStr() */

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "jl_defs.h"  /* manifest constants common to differents functions */

/******************* Global variables used in gpr_ifx.c**********************/
extern int Debug;     /* must be declared in the application source */

/*************************** Function prototypes ****************************/
void BrightenTable8(int *table,int tablesize,int brightness);
void InitTable8(int *table,int tablesize,int range,int threshold,
		int contrast,int sign,double gain);
void LocalStretchTable8(int *table,int tablesize,double offset,double change);
void ReverseTable8(int *table,int tablesize);
void SquareTable8(int *table,int tablesize);

#endif /* #ifndef GPR_IFX_H */
