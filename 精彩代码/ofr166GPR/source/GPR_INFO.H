#ifndef GPR_INFO_H
#define GPR_INFO_H
/* GPR_INFO.H  version 1.08.08.01 */
/* Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * November 30, 1995; August 8, 2001;
 */
/********************** Include required header files ***********************/

   /* check all these again after take out all lib stuff */

/* non-ANSI-compatible headers */
#include <dos.h>      /* REGS,int86 */
#include <conio.h>    /* getch */

/* ANSI-compatible headers */
#include <ctype.h>    /* isdigit */
#include <math.h>     /* sqrt */
#include <stdio.h>    /* printf,FILE,NULL,fopen,fclose,puts,fread,fseek,
                         fgets,size_t,ftell,sprintf,SEEK_SET,rewind,EOF */
#include <stdlib.h>   /* atof,atoi,atol,NULL,calloc,malloc,free,size_t */
#include <string.h>   /* strcpy,strchr,strstr,strlwr (non-ANSI),memset,
                         strupr (non-ANSI),strncpy,strlen,strncat */
#include <dos.h>      /* _dos_getdate, _dos_gettime, struct dosdate_t,
                         struct dostime_t */

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "jl_defs.h"  /* manifest constants common to differents functions */
#include "gpr_io.h"   /* GPR data I/O library */
#include "gpr_ifx.h"  /* GPR image-table manipulation library */
#include "hpgl.h"     /* Olhoeft's HPGL 32-bit graphics library */

/*************************** Notes to developers ****************************/
/*
To add a new image or data processing technique, or any other command option:
  1. Update the default command file GPR_DISP.CMD with the new option and
     default value, and add descriptive comments for the option.
  2. Update this file, GPR_DISP.H.
     a. Add a new member to the ParamInfoStruct structure in
        "Global data types".  This structure passes all necessary information
        between functions.
     b. Add the new command file option name to the end of GPR_DISP_CMDS[]
        array in "Global variables".
     c. Add any new header files that are required and add the function
        prototype, if required, to the appropriate sections.
  3. Update the source code file GPR_DISP.C.
     a. Initialize the new structure member in InitParameters().
     b. Add the new option in GetCmdFileArgs() and update that function's
        copy of GPR_DISP_CMDS[] in its intro.
     c. Update DisplayParameters() to print out the new option's value.
     d. If appropriate: modify DeallocInfoStruct() if arrays are involved.
     e. If appropriate: add a new function call in DisplayImage8().
        (If the function returns an error code, add a new message to the
         end of DisplayImage8Msg[] array in GPR_DISP.H and to the copy of
         that array in the DisplayImage8() intro.  Be sure the error code
         matches the messages position within the array).
	 f. If appropriate: add the new function(s) to the source code (and add
		the function prototype(s) and any new include files to GPR_DISP.H,
		see 2.c above).
  4. Recompile as specified in the discussion at the beginning of GPR_DISP.C.
 */
/*************************** Manifest constants *****************************/
#ifndef INT_DOS
  #define INT_DOS         0x21  /* DOS interrupt number, 33 */
  #define INT_MULTIPLEX   0x2F  /* DOS multiplex interrupt number, 47 */
#endif

#define GPR_INFO_VER "1.08.08.01"
#define DATETIME "August 8, 2001"

/**************************** Global variables ******************************/
/* if Debug == TRUE, turn debugging on */
/* if Batch == TRUE, supress interaction with user */
int Debug = FALSE;

/*************************** Function prototypes ****************************/

/* utility functions (jl_utils.c) */
int  ANSI_there(void);
void GetDosVersion(int *mode,int *major_ver,int *minor_ver);

#endif /* #ifndef GPR_INFO_H */
