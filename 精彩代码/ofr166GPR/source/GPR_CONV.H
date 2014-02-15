#ifndef GPR_CONV_H
#define GPR_CONV_H 2.08.15.01
/* GPR_CONV.H  version 2.08.15.01 */
/* Jeff Lucius  U.S. Geological Survey
 * May 21, 1996; September 10, 1999; March 3, 2000;
 * March 31, 2000; April 4, 2000; May 3, 2001; Ahust 15, 2001
 */
/********************** Include required header files ***********************/
/* non-ANSI-compatible headers */
#include <conio.h>    /* getch */
#include <io.h>       /* access */
#include <errno.h>    /* ENOENT,EACCES */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* ANSI-compatible headers */
#include <float.h>    /* FLT_MAX */
#include <math.h>     /* pow */
#include <stdio.h>    /* printf,FILE,NULL,_IOFBF,fopen,fclose,fgets,puts,
                         fread,fwrite,setvbuf,fseek,SEEK_SET,ftell,sprintf */
#include <stdlib.h>   /* atof,atoi,atol,NULL,malloc,free,calloc,size_t,exit */
#include <string.h>   /* strcpy,strlen,strstr,memset,strncpy,memcpy,
                         strupr (non-ANSI),strlwr (non-ANSI),size_t */

/* Lucius application headers */
#include "assertjl.h" /* JL custom version of assert */
#include "gpr_io.h"   /* JL GPR data I/O library */
#include "jl_defs.h"  /* JL constants and macros */
#include "jl_util1.h" /* JL utility functions library */

/*************************** Manifest constants *****************************/
#define GPR_CONV_VER "2.08.15.01"
#define DATETIME "August 15, 2001"
#define MAX_TEXT  640   /* maximum characters in the text and proc_hist
                           fields of GPR_INFO struct */

/****************************** New data types ******************************/
struct FilesInfoStruct
{   int  num_input_files;       /* number of input files */
    int  file_num;              /* current file being processed, from 0 */
    char **dat_infilename;      /* pointer to array of filenames */
    char **dat_outfilename;     /* pointer to array of filenames */
    int use_mark_file;          /* flag */
    int use_xyz_file;           /* flag */
} ;

/**************************** Global variables ******************************/
extern int Debug, Batch, ANSI_THERE;
extern const char *CONV_CMDS[];
extern const char *month_names[];
extern const char *GetParametersMsg[];
extern const char *GetInfoMsg[];
extern const char *ConvertGprDataMsg[];

/*************************** Function prototypes ****************************/
/* functions specific to this program */
int   GetParameters(int argc, char *argv[],struct GprInfoStruct *InfoPtr,
                    struct FilesInfoStruct *FilesInfoPtr);
void  PrintUsageMsg(void);
void  InitParameters(struct GprInfoStruct *InfoPtr);
int   GetCmdFileArgs(struct GprInfoStruct *InfoPtr,
                     struct FilesInfoStruct *FilesInfoPtr);
char *TrimStr(char *p);
int   GetInfo(int command,struct GprInfoStruct *InfoPtr);
void  DisplayParameters(struct GprInfoStruct *InfoPtr,
                        struct FilesInfoStruct *FilesInfoPtr);
void  DeallocInfoStruct (struct GprInfoStruct *InfoPtr);
int   ConvertGprData(struct GprInfoStruct *InfoPtr);

#endif /* #ifndef GPR_CONV_H */
