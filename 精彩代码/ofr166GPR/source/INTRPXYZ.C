/******************************** INTRPXYZ.C ********************************\
 *                               version 1.00
 * INTRPXYZ reads in XYZ data and calculates, using linear or cubic
 * spline interpolation, a new set of dependant values.  The independant
 * value may be either X, Y, or Z or traverse distance.  The interpolated
 * value may be either X, Y, Z, traverse distance, or a position value (as
 * the fourth field in the XYZ file).  The interpolated values are saved as
 * an XYZ file.
 * values are saved as an XYZ file.
 *
 * The input to this program is a "CMD" file, an ASCII text file containing
 * commands (or parameters).  The CMD file specifies the XYZ input and
 * output file names.  Inspect the file INTRPXYZ.CMD for usage.
 *
 * Jeff Lucius
 * Geophysicist
 * U.S. Geological Survey
 * Geologic Division, Mineral Resources Team
 * Box 25046, Denver Federal Center, MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1200 (secretary), 303-236-1425 (fax)
 * email: lucius@usgs.gov
 *
 * Revision History:
 *  June 20, 1997     - Completed version 1.00.
 *  June 25, 1997     - Modify GetCmdFileArgs() so that if an equal sign
 *  					appears after out_indep[], but no values are supplied,
 *                      then num_out does not get reset to 0.
 *                    - Added check for interp_method in GetParameters().
 *                    - Moved interp code into InterpXyz().
 *                    - Improved documentation in the CMD and source files.
 *  June 27, 1997     - Added optional fourth field to XYZ file format so
 *                      postion number can be used to get X-Y-Z.
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm intrpxyz.c /zfloatsync jl_util1.lib
 *     To compile for 80486 executable :
 *       icc /F /xnovm /zfloatsync /zmod486 intrpxyz.c jl_util1.lib
 *
 *     /F removes the floating point emulator, reduces binary size.
 *     /xnovm specifies no virtual memory manager, disk access is too slow.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     jl_util1.lib - Jeff Lucius's utility functions
 *
 * To run: INTRPXYZ cmd_filename
 *
\****************************************************************************/

/******************* Function dependency and resource usage *****************/
/*
 * con I/O  - console I/O - keyboard and/or text-mode screen used
 * disk I/O - reads from and/or writes to disk
 * DOS      - DOS/BIOS interrupt performed
 * graphics - graphics modes required, direct writes to ports/video adaptor
 * port I/O - hardware I/O ports used
 * RAM      - direct access of CPU RAM memory
 *
 * (main)                              (disk I/O) (con I/O)
 *   +-- ANSI_there                                              (DOS)
 *   |     +-- GetDosVersion                                     (DOS)
 *   +-- GetParameters                            (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line and reading in the X-Y-Z-P data.  Storage is also
 *   |     | alocated here for the arrays.
 *   |     +-- PrintUsageMsg                      (con I/O)
 *   |     +-- InitParameters
 *   |     +-- GetCmdFileArgs          (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetXyzData              (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetXyzpData              (disk I/O)
 *   |           +-- TrimStr
 *   +-- DisplayParameters                        (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- InterpXyz
 *   +-- DeallocInfoStruct
 */

/******************* Includes all required header files *********************/
#include "intrpxyz.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *INTRPXYZ_CMDS[]=
{   "xyz_infilename","xyz_outfilename","interp_method","indep_value","num_out",
	"out_start","out_stop","out_step","out_indep[]",NULL,
} ;

/* The rest of these are message strings that match function return codes */
const char *GetXyzDataMsg[] =
{   "GetXyzData() No Errors.",
	"GetXyzData() Error: Unable to open XYZ file.",
	"GetXyzData() Error: Problem getting data from XYZ file.",
	"GetXyzData() Error: Unable to allocate storage for XYZ values.",
	"GetXyzData() Error: EOF reached or error before all values retreived.",
	"GetXyzData() Error: Less than two sets in file.",
} ;
const char *GetXyzpDataMsg[] =
{   "GetXyzpData() No Errors.",
	"GetXyzpData() Error: Unable to open XYZ file.",
	"GetXyzpData() Error: Problem getting data from XYZ file.",
	"GetXyzpData() Error: Unable to allocate storage for XYZ values.",
	"GetXyzpData() Error: EOF reached or error before all values retreived.",
	"GetXyzpData() Error: Less than two sets in file.",
} ;
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
	"GetParameters() ERROR: Insufficient command line arguments.",
	"GetCmdFileArgs() ERROR: Unable to open input command file.",
	"GetCmdFileArgs() ERROR: Input XYZ file not specified.",
	"GetCmdFileArgs() ERROR: Output XYZ file not specified.",

	"GetParameters() ERROR: Problem getting data from XYZ file.",
	"GetParameters() ERROR: Invalid input independant value.",
	"GetParameters() ERROR: Not able to allocate sufficient temporary storage.",
	"GetParameters() ERROR: num_out equals 0.",
	"GetParameters() ERROR: out_start or out_stop are invalid.",

	"GetParameters() ERROR: out_start or out_step are invalid.",
	"GetParameters() ERROR: out_start or out_stop is outside of input range.",
	"GetParameters() ERROR: Unable to allocate storage for out_xyz[].",
	"GetParameters() ERROR: Unable to allocate storage for out_xyz[][].",
	"GetParameters() ERROR: Output XYZ filename same as input filename.",

	"GetParameters() ERROR: Unable to allocate storage work arrays.",
	"GetParameters() ERROR: Unable to allocate storage for out_indep.",
	"GetParameters() ERROR: Invalid or unrecognized interpolation method.",
} ;
const char *InterpXyzMsg[] =
{   "InterpXyz(): No errors.",
	"InterpXyz() ERROR: Problem performing interpolation.",
} ;
const char *LinearInterpMsg[] =
{   "LinearInterp(): No errors.",
    "LinearInterp() ERROR: Less than 2 values in array.",
    "LinearInterp() ERROR: interpolated independent variable array exceeds limits.",
} ;

/*********************************** main() *********************************/

void main(int argc,char *argv[])
{
/***** Declare variables *****/
    /* following are variables used by main */
	int i,itemp;
	FILE *outfile;

	/* Following is an information stucture.  This structure is the key
	   variable passed between functions.
	 */
	struct InterpXyzInfoStruct ParamInfo;

	/* These variables found near beginning of this source */
	extern int         Debug,Batch,ANSI_THERE;

	/* This variable from jl_util1.lib */
	extern const char *SplineMsg[];

/***** Initialize variables *****/
	/* Check if ansi.sys loaded */
	if (ANSI_there()) ANSI_THERE = TRUE;

/***** Get information used to process data and get the XYZ input data *****/
	itemp = GetParameters(argc,argv,&ParamInfo);
	if (itemp > 0)
	{   printf("\n%s\n",GetParametersMsg[itemp]);
		DeallocInfoStruct(&(ParamInfo));
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		puts("\nGPR_DISP finished with an error.\n");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		exit(1);
	}

/***** Display information *****/
	DisplayParameters(&ParamInfo);
	if (!Batch)
	{   printf("Press <Esc> to quit or other key to continue ... ");
		if ((itemp = getch()) == ESC)
		{   DeallocInfoStruct(&ParamInfo);
			puts("\nProgram terminated by user.\n");
			if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
			puts("INTRPXYZ finished.");
			if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
			exit(3);
		} else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
			getch();  /* clear out the extra char in buffer */
	}

/***** Interpolate data *****/
	printf("\nInterpolating ....");
	itemp = InterpXyz(&ParamInfo);
	if (itemp > 0)
	{   printf("\n%s\n",InterpXyzMsg[itemp]);
		DeallocInfoStruct(&(ParamInfo));
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		puts("\nINTRPXYZ finished with an error.\n");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		exit(1);
	}

/***** Write output XYZ file *****/
	if (Debug)
	{   for (i=0; i<ParamInfo.num_out; i++)
            printf("%4d  X=%g  Y=%g  Z=%g\n",i,ParamInfo.out_xyz[i][0],
                ParamInfo.out_xyz[i][1],ParamInfo.out_xyz[i][2]);
    }

    if ((outfile = fopen(ParamInfo.xyz_outfilename,"wt")) == NULL)
	{   printf("\nERROR: Unable to open output XYZ file.\n");
		DeallocInfoStruct(&(ParamInfo));
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		puts("\nINTRPXYZ finished with an error.\n");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		exit(1);
	}
	fprintf(outfile,"%d\n",ParamInfo.num_out);
	for (i=0; i<ParamInfo.num_out; i++)
		fprintf(outfile,"%9.3f  %9.3f  %9.3f\n",ParamInfo.out_xyz[i][0],
			ParamInfo.out_xyz[i][1],ParamInfo.out_xyz[i][2]);
	fclose(outfile);

/***** Free storage and exit *****/
	DeallocInfoStruct(&(ParamInfo));
	if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
	puts("\nINTRPXYZ finished.\n");
	if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
	exit(0);
}
/****************************** GetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 * Get XYZ data.
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct InterpXyzInfoStruct *InfoPtr - address of structure
 *
 * Requires: <math.h>, <stdio.h>, <stdlib>, <string.h>, "intrpxyz.h".
 * Calls: PrintUsageMsg, InitParameters, GetCmdFileArgs, GetXyzData,
 *        strcpy, printf, malloc, free, sqrt.
 *
 *
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
	"GetParameters() ERROR: Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Input XYZ file not specified.",
    "GetCmdFileArgs() ERROR: Output XYZ file not specified.",

    "GetParameters() ERROR: Problem getting data from XYZ file.",
    "GetParameters() ERROR: Invalid input independant value.",
	"GetParameters() ERROR: Not able to allocate sufficient temporary storage.",
	"GetParameters() ERROR: num_out equals 0.",
	"GetParameters() ERROR: out_start or out_stop are invalid.",

	"GetParameters() ERROR: out_start or out_step are invalid.",
	"GetParameters() ERROR: out_start or out_stop is outside of input range.",
	"GetParameters() ERROR: Unable to allocate storage for out_xyz[].",
	"GetParameters() ERROR: Unable to allocate storage for out_xyz[][].",
	"GetParameters() ERROR: Output XYZ filename same as input filename.",

	"GetParameters() ERROR: Unable to allocate storage work arrays.",
	"GetParameters() ERROR: Unable to allocate storage for out_indep.",
	"GetParameters() ERROR: Invalid or unrecognized interpolation method.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 27, 1997
 */
int GetParameters (int argc, char *argv[],struct InterpXyzInfoStruct *InfoPtr)
{
/***** Declare variables *****/
	int    req_args = 2;   /* exe_name, and cmd_filename */
	int    i,j,itemp;        /* scratch variables and counters */
	extern int Debug;
	extern const char *GetXyzDataMsg[];
	extern const char *GetXyzpDataMsg[];

/***** Initialize information structure *****/
	InitParameters(InfoPtr);
    strcpy(InfoPtr->cmd_filename,argv[1]);

/***** Verify usage *****/
    if (argc < req_args)
    {   PrintUsageMsg();
        return 1;
    }

/***** Get user-defined parameters from CMD file *****/
	itemp = GetCmdFileArgs(InfoPtr);
	if (itemp) return itemp;   /* 2 to 4 */

/***** Get XYZ data from file *****/
	if (InfoPtr->indep_value == P_VALUE)
	{   itemp = GetXyzpData(InfoPtr->xyz_infilename,&(InfoPtr->num_in),&(InfoPtr->in_xyzp));
		if (itemp > 0)
		{   printf("\n%s\n",GetXyzpDataMsg[itemp]);
			InfoPtr->num_in = 0;
			return 5;
		}
	}  else
	{   itemp = GetXyzData(InfoPtr->xyz_infilename,&(InfoPtr->num_in),&(InfoPtr->in_xyzp));
		if (itemp > 0)
		{   printf("\n%s\n",GetXyzDataMsg[itemp]);
			InfoPtr->num_in = 0;
			return 5;
		}
	}

	if (Debug)
	{   for (i=0; i<InfoPtr->num_in; i++)
		{   printf("%G  %G  %G",InfoPtr->in_xyzp[i][0],InfoPtr->in_xyzp[i][1],InfoPtr->in_xyzp[i][2]);
			if (InfoPtr->indep_value == P_VALUE) printf("  %G",InfoPtr->in_xyzp[i][3]);
			printf("\n");
		}
	}

/***** Finish initializing variables and verifying valid ranges *****/
	itemp = 1;
	for (i=1; i<=NUM_INTERP; i++)
	{   if (InfoPtr->interp_method == i)
		{   itemp = 0;
			break;   /* out of for loop */
		}
	}
	if (itemp) return 17;

	/* Copy first and last independant values and calc traverse if nec. */
	switch (InfoPtr->indep_value)
	{   case 1:   /* X */
			InfoPtr->in_start = InfoPtr->in_xyzp[0][0];
			InfoPtr->in_stop  = InfoPtr->in_xyzp[InfoPtr->num_in-1][0];
			break;
		case 2:   /* Y */
			InfoPtr->in_start = InfoPtr->in_xyzp[0][1];
			InfoPtr->in_stop  = InfoPtr->in_xyzp[InfoPtr->num_in-1][1];
			break;
		case 3:   /* Z */
			InfoPtr->in_start = InfoPtr->in_xyzp[0][2];
			InfoPtr->in_stop  = InfoPtr->in_xyzp[InfoPtr->num_in-1][2];
			break;
		case 4:   /* traverse */
		{   double dx,dy,dz,sum;

			InfoPtr->in_trav = (double *)malloc(InfoPtr->num_in * sizeof(double));
			if (InfoPtr->in_trav == NULL)  return 7;
			sum = 0.0;
			InfoPtr->in_trav[0] = sum;
			for (i=1; i<InfoPtr->num_in; i++)
			{   dx = InfoPtr->in_xyzp[i][0] - InfoPtr->in_xyzp[i-1][0];
				dy = InfoPtr->in_xyzp[i][1] - InfoPtr->in_xyzp[i-1][1];
				dz = InfoPtr->in_xyzp[i][2] - InfoPtr->in_xyzp[i-1][2];
				sum += sqrt(dx*dx + dy*dy + dz*dz);
				InfoPtr->in_trav[i] = sum;
			}
			InfoPtr->in_start = InfoPtr->in_trav[0];
			InfoPtr->in_stop  = InfoPtr->in_trav[InfoPtr->num_in-1];
			break;
		}
		case 5:  /* position number */
			InfoPtr->in_start = InfoPtr->in_xyzp[0][3];
			InfoPtr->in_stop  = InfoPtr->in_xyzp[InfoPtr->num_in-1][3];
			break;
		default:
			return 6;
	}

	/* Calculate output independant values if required */
	if (InfoPtr->num_out == 0) return 8;
	if (InfoPtr->out_indep == NULL)  /* != NULL if user supplied in cmd file */
	{   if (InfoPtr->out_step == INVALID_VALUE)
        {   if (InfoPtr->out_start == INVALID_VALUE ||
                InfoPtr->out_stop == INVALID_VALUE)  return 9;
            InfoPtr->out_step = (InfoPtr->out_stop - InfoPtr->out_start) / (InfoPtr->num_out - 1);
		} else
		{   if (InfoPtr->out_start == INVALID_VALUE ||
				InfoPtr->out_step == INVALID_VALUE)  return 10;
        }
		InfoPtr->out_indep = (double *)malloc(InfoPtr->num_out * sizeof(double));
        if (InfoPtr->out_indep == NULL) return 16;

        for (i=0; i<InfoPtr->num_out; i++)
            InfoPtr->out_indep[i] = InfoPtr->out_start + (i*InfoPtr->out_step);
    } else
    {   InfoPtr->out_start = InfoPtr->out_indep[0];
		InfoPtr->out_stop  = InfoPtr->out_indep[InfoPtr->num_out-1];
    }

    /* Check that output line is within range of input line */
    if (InfoPtr->in_start < InfoPtr->in_stop)
    {   if (InfoPtr->out_start < InfoPtr->in_start ||
            InfoPtr->out_stop > InfoPtr->in_stop) return 11;
    } else
    {   if (InfoPtr->out_start > InfoPtr->in_start ||
            InfoPtr->out_stop < InfoPtr->in_stop) return 11;
    }

    /* Allocate storage for out_xyz */
    InfoPtr->out_xyz = (double **)malloc(InfoPtr->num_out * sizeof(double *));
    if (InfoPtr->out_xyz == NULL)  return 12;
    for (i=0; i<InfoPtr->num_out; i++)
    {   InfoPtr->out_xyz[i] = (double *)malloc(3 * sizeof(double));
		if (InfoPtr->out_xyz[i] == NULL)
        {  for (j=0; j<i; j++)  free(InfoPtr->out_xyz[j]);
		   free(InfoPtr->out_xyz);
           InfoPtr->out_xyz = NULL;
           return 13;
        }
	}

    /* Check input and output names differ */
    if (strcmp(InfoPtr->xyz_infilename,InfoPtr->xyz_outfilename) == 0)
        return 14;

    /* Allocate storage for working arrays */
    InfoPtr->x = (double *)malloc(InfoPtr->num_in * sizeof(double));
    InfoPtr->y = (double *)malloc(InfoPtr->num_in * sizeof(double));
	InfoPtr->ynew = (double *)malloc(InfoPtr->num_out * sizeof(double));
    if (InfoPtr->x == NULL || InfoPtr->y == NULL || InfoPtr->ynew == NULL)
	{    free(InfoPtr->x);
        free(InfoPtr->y);
        free(InfoPtr->ynew);
        return 15;
    }

    return 0;
}
/****************************** PrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>, <dos.h>.
 * Calls: puts, sprintf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 27, 1997
 */
void PrintUsageMsg (void)
{
	char str[10];
	extern int ANSI_THERE;

	puts("");
	if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
	puts("                                                                               ");
	puts("###############################################################################");
	puts("   Usage: INTERPXYZ cmd_filename                                               ");
	puts("     This program reads in XYZ data and calculates, using linear or cubic      ");
	puts("     spline interpolation, a new set of dependant values.  The independant     ");
	puts("     value may be either X, Y, Z, Position number, or traverse distance.       ");
	puts("                                                                               ");
	puts("   Required command line arguments:                                            ");
	puts("     cmd_filename - name of ASCII command file that follows the \"CMD\" format.");
	puts("                                                                               ");
	puts("   Inspect \"INTRPXYZ.CMD\" for command file format and valid parameters.      ");
	puts("                                                                               ");
	sprintf(str,"%g",INTRPXYZ_VER);
	printf("   Version %s - Last Revision Date: %s at %s      \n",str,__DATE__,__TIME__);
	puts("###############################################################################");
	if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** InitParameters() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct InterpXyzInfoStruct *InfoPtr - address of structure
 *
 * Requires: "intrpxyz.h".
 * Calls: none.
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 27, 1997
 */
void InitParameters (struct InterpXyzInfoStruct *InfoPtr)
{
	InfoPtr->cmd_filename[0]    = 0;             /* initializer   */
	InfoPtr->xyz_infilename[0]  = 0;             /* initializer   */
	InfoPtr->xyz_outfilename[0] = 0;             /* initializer   */
	InfoPtr->interp_method      = 1;             /* default value */
	InfoPtr->indep_value        = 0;             /* invalid value */
	InfoPtr->num_in             = 0;             /* invalid value */
	InfoPtr->in_xyzp            = NULL;          /* initializer   */
	InfoPtr->in_trav            = NULL;          /* initializer   */
	InfoPtr->in_start           = INVALID_VALUE; /* invalid value */
	InfoPtr->in_stop            = INVALID_VALUE; /* invalid value */
	InfoPtr->num_out            = 0;             /* invalid value */
	InfoPtr->out_xyz            = NULL;          /* initializer   */
	InfoPtr->out_indep          = NULL;          /* initializer   */
	InfoPtr->out_start          = INVALID_VALUE; /* invalid value */
	InfoPtr->out_stop           = INVALID_VALUE; /* invalid value */
	InfoPtr->out_step           = INVALID_VALUE; /* invalid value */
	InfoPtr->x                  = NULL;          /* initializer   */
	InfoPtr->y                  = NULL;          /* initializer   */
	InfoPtr->ynew               = NULL;          /* initializer   */
}
/***************************** GetCmdFileArgs() *****************************/
/* Get processing parameters from user from a command file.
        
const char *INTRPXYZ_CMDS[]=
{   "xyz_infilename","xyz_outfilename","interp_method","indep_value","num_out",
    "out_start","out_stop","out_step","out_indep[]",NULL,
} ;
 *
 * Parameter list:
 *  struct InterpXyzInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <stdio.h>, <stdlib.h>, <string.h>, "intrpxyz.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strncat, strchr,
 *        strcmp, strupr, strlwr, strncpy, atol, malloc, free,
 *        Trimstr, getch.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 25, 1997
 */
int GetCmdFileArgs (struct InterpXyzInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];
    int found,i,num,itemp,have_string;
    int xyz_in_found = FALSE;
    int xyz_out_found = FALSE;
    double dtemp;
    FILE *infile = NULL;
    extern int Debug,Batch;

    if (Debug) puts("");
    if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
    {   char *cptr;

		if ((cptr = strchr(InfoPtr->cmd_filename,'.')) == NULL)
        {   strcat(InfoPtr->cmd_filename,".");
            if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
            {   strcat(InfoPtr->cmd_filename,"cmd");
                if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
                    return 2;
            }
        } else
        {   *cptr = 0;
			strcat(InfoPtr->cmd_filename,".cmd");
            if ((infile = fopen(InfoPtr->cmd_filename,"rt")) == NULL)
                return 2;
        }
    }
    have_string = FALSE;
    while (1)            /* read till EOF or error */
    {   if (have_string == FALSE) /* if have not already retrieved file line */
        {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                break;   /* out of while(1) loop */
        } else
        {   have_string = FALSE;
        }
        cmdstr[0] = 0;            /* set to 0 so strncat() works right */
        cp = strchr(str,'=');     /* look for equal sign */
        if (cp == NULL) continue; /* invalid command line so ignore */
        num = (int)(cp-str);      /* number of chars before equal sign */
        strncat(cmdstr,str,(size_t)num);  /* copy to working string */
		TrimStr(cmdstr);          /* remove leading and trailing blanks */
        found = -1;
        i = 0;
        while (INTRPXYZ_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),INTRPXYZ_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_DISP_CMDS[] */
                break;
            }
            i++;
        }
        if (found == -1) continue;    /* bad or missing command word */
        cp++;                         /* step 1 past equal sign */
        /* truncate string at semicolon if present outside of double quotes */
        cp2 = cp3 = cp4 = NULL;       /* initialize pointers */
		cp2 = strchr(cp,'"');         /* see if first quote present */
        if (cp2 != NULL)
            cp3 = strchr(cp2+1,'"');  /* see if second quote present */
        cp4 = strchr(cp,';');         /* see if semicolon present */
        if (cp4 != NULL)              /* if present ... */
        {   if (cp3 != NULL)
            {   if (cp4 < cp2 || cp4 > cp3)
                    *cp4 = 0;         /* truncate at semicolon */
            }
		}
        TrimStr(cp);                  /* remove leading and trailing blanks */
#if 1
        if (Debug)
        {    printf("cp = %s\n",cp);
            getch();
        }
#endif
        switch (found)       /* cases depend on same order in GPR_DISP_CMD[] */
        {   case 0:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past first quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->xyz_infilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->xyz_infilename);
                if (Debug) printf("xyz_infilename = %s\n",InfoPtr->xyz_infilename);
                if (InfoPtr->xyz_infilename[0]) xyz_in_found = TRUE;
                break;
            case 1:
				cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->xyz_outfilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->xyz_outfilename);
                if (Debug) printf("xyz_outfilename = %s\n",InfoPtr->xyz_outfilename);
                if (InfoPtr->xyz_outfilename[0]) xyz_out_found = TRUE;
				break;
            case 2:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <= NUM_INTERP)
                    InfoPtr->interp_method = itemp;
                if (Debug) printf("interp_method = %d\n",InfoPtr->interp_method);
                break;
			case 3:
				itemp = atoi(cp);
				if (itemp >= 0 && itemp <= NUM_VALUES)
                    InfoPtr->indep_value = itemp;
                if (Debug) printf("indep_value = %d\n",InfoPtr->indep_value);
                break;
            case 4:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->num_out = itemp;
                if (Debug) printf("num_out = %d\n",InfoPtr->num_out);
                break;
            case 5:
                dtemp = atof(cp);
                InfoPtr->out_start = dtemp;
                if (Debug) printf("out_start = %g\n",InfoPtr->out_start);
                break;
            case 6:
                dtemp = atof(cp);
                InfoPtr->out_stop = dtemp;
                if (Debug) printf("out_stop = %g\n",InfoPtr->out_stop);
                break;
            case 7:
                dtemp = atof(cp);
                InfoPtr->out_step = dtemp;
                if (Debug) printf("out_step = %g\n",InfoPtr->out_step);
                break;
            case 8:
            {   char *cp_next = (char *)TrimStr(cp);
				int val_found = 0;

                /* number of values must be specified first */
                if (InfoPtr->num_out <= 0) break;

                /* allocate storage for traces */
				InfoPtr->out_indep = (double *)malloc(InfoPtr->num_out * sizeof(double));
				if (InfoPtr->out_indep == NULL)
				{   /* InfoPtr->num_out = 0; */
					if (Debug) puts("\nmalloc failed for out_indep[]");
                    break;              /* break out of case */
                }

                while (1)
                {   while (*cp_next)    /* work through string */
					{   TrimStr(cp_next);
                        if (val_found < InfoPtr->num_out)
						{   InfoPtr->out_indep[val_found] = atof(cp_next);
                            val_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (val_found < InfoPtr->num_out)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {   break;      /* break out of while (1) */
                        } else
                        {   /* see if we have pulled in a command line */
                            if (strchr(str,'='))
                            {   have_string = TRUE;
                                break;  /* break out of while (1) */
                            } else
                            {   have_string = FALSE;
                                cp_next = (char *)TrimStr(str);
                            }
                        }
                    } else
                        break;          /* break out of while (1) */
                }
                if (val_found < InfoPtr->num_out)
                {   if (Debug) puts("\nval_found < num_out");
					/* InfoPtr->num_out = 0; */
					free(InfoPtr->out_indep);
                    InfoPtr->out_indep = NULL;
				}
                if (Debug)
                {   for (i=0; i<InfoPtr->num_out; i++)
                        printf("out_indep[%d] = %g\n",i,InfoPtr->out_indep[i]);
                }
                break;
            }
        }   /* end of switch (found) block */
    }   /* end of while(1) block */
	fclose(infile);
    if (Debug)  Batch = FALSE;
    if (!xyz_in_found) return 3;
    if (!xyz_out_found) return 4;

    return 0;
}
/******************************* GetXyzData() *******************************/
/* Open Xyz file, allocate storage, retrieve values, and close file.
 * The file is ASCII text, with one item on the first line, the number of
 * tick marks.  The X, Y, and Z for each mark are placed one set to a line,
 * with each value seperated by at least one space.
 * For example:
 3
10.0   0.0  293.1
20.0   0.0  293.2
30.0   0.0  293.3
 * Three sets, with coordinates (10.0,0.0,293.1), (20.0,0.0,293.2),
 * and (30.0,0.0,293.3).
 *
 * NOTE: This function allocates storage for a variable declared in another
 *       function.  The address must be passed to this function.
 *
 * Parameter list:
 *  char *filename - string, name of ASCII file
 *  int *num_sets  - address of variable, will hold number of tick marks
 *  double ***xyz  - address of 2D pointer, storage allocated here, the X, Y,
 *                   and Z values from the file
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "intrpxzy.h".
 * Calls: fopen, fclose, fgets, malloc, free, strchr, TrimStr, atoi, atof.
 * Returns: 0 on success,
 *         >0 on error
 *
const char *GetXyzDataMsg[] =
{   "GetXyzData() No Errors.",
    "GetXyzData() Error: Unable to open XYZ file.",
    "GetXyzData() Error: Problem getting data from XYZ file.",
    "GetXyzData() Error: Unable to allocate storage for XYZ values.",
    "GetXyzData() Error: EOF reached or error before all values retreived.",
    "GetXyzData() Error: Less than two sets in file.",
}
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 18, 1997
 */
int GetXyzData (char *filename,int *num_sets,double ***xyz)
{
	char str[MAX_CMDLINELEN], *cp;
	int i,j,num;
	FILE *infile;

	/* Attemp to open XYZ file */
	if ((infile = fopen(filename,"rt")) == NULL) return 1;

	/* Get number of sets */
	if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)
	{   fclose(infile);
		return 2;
	}
	*num_sets = atoi(str);
	if (*num_sets < 2)
	{   *num_sets = 0;
		return 5;
	}

	/* Allocate storage for X-Y-Z coordinates */
	(*xyz) = (double **)malloc(*num_sets * sizeof(double *));
	if ((*xyz) == NULL)
    {   fclose(infile);
        return 3;
    }
    for (i=0; i< *num_sets; i++)
	{   (*xyz)[i] = (double *)malloc(3 * sizeof(double));
        if ((*xyz)[i] == NULL)
        {  for (j=0; j<i; j++)  free((*xyz)[j]);
           free(*xyz);
           (*xyz) = NULL;
           fclose(infile);
           return 3;
        }
    }

    /* Retrieve the values */
    for (num=0; num< *num_sets;)
    {   if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)  break;
        TrimStr(str);
        if (str[0] == 0)  continue;
        (*xyz)[num][0] = atof(str);
		cp = strchr(str,' ');
		if (cp == NULL) break;
		TrimStr(cp);
		(*xyz)[num][1] = atof(cp);
		cp = strchr(cp,' ');
		if (cp == NULL) break;
		TrimStr(cp);
		(*xyz)[num][2] = atof(cp);
		num++;
	}
	fclose(infile);

	/* Did we get enough  values? */
	if (num < *num_sets)
	{   for (i=0; i< *num_sets; i++)  free((*xyz)[i]);
		free(*xyz);
		(*xyz) = NULL;
		*num_sets = 0;
		return 4;
	}
	return 0;
}
/****************************** GetXyzpData() *******************************/
/* Open XYZ file, allocate storage, retrieve values, and close file.
 * The file is ASCII text, with one item on the first line, the number of
 * tick marks.  The X, Y, Z, and P values are placed one set to a line,
 * with each value seperated by at least one space.
 * For example:
 3
10.0   0.0  293.1  0
20.0   0.0  293.2  3
30.0   0.0  293.3  6
 *
 * NOTE: This function allocates storage for a variable declared in another
 *       function.  The address must be passed to this function.
 *
 * Parameter list:
 *  char *filename - string, name of ASCII file
 *  int *num_sets  - address of variable, will hold number of tick marks
 *  double ***xyzp - address of 2D pointer, storage allocated here, the X, Y,
 *                   Z, and P values from the file
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "intrpxzy.h".
 * Calls: fopen, fclose, fgets, malloc, free, strchr, TrimStr, atoi, atof.
 * Returns: 0 on success,
 *         >0 on error
 *
const char *GetXyzpDataMsg[] =
{   "GetXyzpData() No Errors.",
	"GetXyzpData() Error: Unable to open XYZ file.",
	"GetXyzpData() Error: Problem getting data from XYZ file.",
	"GetXyzpData() Error: Unable to allocate storage for XYZ values.",
	"GetXyzpData() Error: EOF reached or error before all values retreived.",
	"GetXyzpData() Error: Less than two sets in file.",
}
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 27, 1997
 */
int GetXyzpData (char *filename,int *num_sets,double ***xyzp)
{
	char str[MAX_CMDLINELEN], *cp;
	int i,j,num;
	FILE *infile;

	/* Attemp to open XYZ file */
	if ((infile = fopen(filename,"rt")) == NULL) return 1;

	/* Get number of sets */
	if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)
	{   fclose(infile);
		return 2;
	}
	*num_sets = atoi(str);
	if (*num_sets < 2)
	{   *num_sets = 0;
		return 5;
	}

	/* Allocate storage for X-Y-Z-P coordinates */
	(*xyzp) = (double **)malloc(*num_sets * sizeof(double *));
	if ((*xyzp) == NULL)
	{   fclose(infile);
		*num_sets = 0;
		return 3;
	}
	for (i=0; i< *num_sets; i++)
	{   (*xyzp)[i] = (double *)malloc(4 * sizeof(double));
		if ((*xyzp)[i] == NULL)
		{  for (j=0; j<i; j++)  free((*xyzp)[j]);
		   free(*xyzp);
		   (*xyzp) = NULL;
		   fclose(infile);
		   *num_sets = 0;
		   return 3;
		}
	}

	/* Retrieve the values */
	for (num=0; num< *num_sets;)
	{   if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)  break;
		TrimStr(str);
		if (str[0] == 0)  continue;
		(*xyzp)[num][0] = atof(str);

		cp = strchr(str,' ');
		if (cp == NULL) break;
		TrimStr(cp);
		(*xyzp)[num][1] = atof(cp);

		cp = strchr(cp,' ');
		if (cp == NULL) break;
		TrimStr(cp);
		(*xyzp)[num][2] = atof(cp);

		cp = strchr(cp,' ');
		if (cp == NULL) break;
		TrimStr(cp);
		(*xyzp)[num][3] = atof(cp);

		num++;
	}
	fclose(infile);

	/* Did we get enough  values? */
	if (num < *num_sets)
	{   for (i=0; i< *num_sets; i++)  free((*xyzp)[i]);
		free(*xyzp);
		(*xyzp) = NULL;
		*num_sets = 0;
		return 4;
	}
	return 0;
}

/**************************** DeallocInfoStruct() ***************************/
/* Deallocate the buffer space in the information structure.  This function
 * will work correctly only if the num_... variables accurately reflect
 * the successful allocation of storage.
 *
 * Parameter list:
 *  struct InterpXyzInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_disp.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Lakewood, CO
 * Date: June 19, 1997
 */
void DeallocInfoStruct (struct InterpXyzInfoStruct *InfoPtr)
{
    int i;
    extern int Debug;

    if (InfoPtr->num_in > 0)
    {   if (InfoPtr->in_trav)
        {   free(InfoPtr->in_trav);
            InfoPtr->in_trav = NULL;
        }
		if (InfoPtr->in_xyzp)
		{   for (i=0; i<InfoPtr->num_in; i++)
				free(InfoPtr->in_xyzp[i]);
			free(InfoPtr->in_xyzp);
			InfoPtr->in_xyzp = NULL;
        }
        if (InfoPtr->x)
        {   free(InfoPtr->x);
            InfoPtr->x = NULL;
        }
        if (InfoPtr->y)
        {   free(InfoPtr->y);
            InfoPtr->y = NULL;
		}
        InfoPtr->num_in = 0;
    }

    if (InfoPtr->num_out > 0)
	{   if (InfoPtr->out_xyz)
        {   for (i=0; i<InfoPtr->num_out; i++)
                free(InfoPtr->out_xyz[i]);
            free(InfoPtr->out_xyz);
            InfoPtr->out_xyz = NULL;
        }
        if (InfoPtr->ynew)
        {   free(InfoPtr->ynew);
            InfoPtr->ynew = NULL;
        }
        InfoPtr->num_out = 0;
    }
}
/**************************** DisplayParameters() ***************************/
/* Display parameters to CRT.
 *
 * Parameter list:
 *  struct InterpXyzInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, "jl_defs.h", "gpr_disp.h".
 * Calls: strupr, printf, puts, getch, sprintf.
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Lakewood, CO
 * Date: June 20, 1997
 */
void DisplayParameters (struct InterpXyzInfoStruct *InfoPtr)
{
    int i,key,lines;
    char str[10];
    extern int Debug,Batch,ANSI_THERE;

    lines = 0;
    if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); /* red on black */
    sprintf(str,"%g",INTRPXYZ_VER);
    printf("\nINTRPXYZ v. %s (Latest Program Compile Date = %s at %s):\n",str,__DATE__,__TIME__);
	lines++;
	if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
	printf("cmd_filename     = %s\n",strupr(InfoPtr->cmd_filename));
    lines++;
    printf("xyz_infilename   = %s\n",strupr(InfoPtr->xyz_infilename));
	lines++;
    printf("xyz_outfilename  = %s\n",strupr(InfoPtr->xyz_outfilename));
    lines++;

    switch (InfoPtr->interp_method)
    {   case  1: puts("Interpolation method: linear");        break;
        case  2: puts("Interpolation method: cubic spline");  break;
    }
    lines++;
    switch (InfoPtr->indep_value)
    {   case  1: puts("Independant value: X");        break;
        case  2: puts("Independant value: Y");        break;
        case  3: puts("Independant value: Z");        break;
        case  4: puts("Independant value: traverse"); break;
    }
    printf("number of input XYZ sets = %d\n",InfoPtr->num_in);
    lines++;
    printf("in_start  = %g  in_stop  = %g\n",
        InfoPtr->in_start,InfoPtr->in_stop);
    lines++;
    printf("number of output XYZ sets = %d\n",InfoPtr->num_out);
    lines++;
    printf("out_start = %g  out_stop = %g  out_step = %g\n",
        InfoPtr->out_start,InfoPtr->out_stop,InfoPtr->out_step);
    lines++;

    key = -1;
    if (!Batch)
    {   printf("Press <space bar> to see input XYZ sets and output independant values ...");
        key = getch();
        printf("\r                                                                           \r");
        lines++;
    }
    if (key == 0x20)  /* spacebar */
    {   for (i=0; i<InfoPtr->num_in; i++)
        {   if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
				printf("\r                                  \r");
                lines = 0;
			}
			printf("Input XYZ [%d] = %g %g %g",i,InfoPtr->in_xyzp[i][0],
				InfoPtr->in_xyzp[i][1],InfoPtr->in_xyzp[i][2]);
			if (InfoPtr->indep_value == P_VALUE) printf("  %G",InfoPtr->in_xyzp[i][3]);
			printf("\n");
			lines++;
		}
		for (i=0; i<InfoPtr->num_out; i++)
		{   if (!Batch && lines >= 23)
			{   printf("Press a key to continue ...");
				getch();
				printf("\r                                  \r");
				lines = 0;
			}
			printf("Output independant [%d] = %g\n",i,InfoPtr->out_indep[i]);
			lines++;
		}
	}

	/* Display warning */
	if (Debug)       puts("In TESTing mode.");
	else if (Batch)  puts("In Batch processing mode.");
}
/******************************** InterpXyz() *******************************/
/* Interpolate.
 *
 * Parameter list:
 *  struct InterpXyzInfoStruct *InfoPtr - address of structure
 *
 * Requires: "intrpxyz.h".
 * Calls: LinearInterp, Spline.
 *
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *InterpXyzMsg[] =
{   "InterpXyz(): No errors.",
	"InterpXyz() ERROR: Problem performing interpolation.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Team   Lakewood, CO
 * Date: June 25, 1997
 */
int InterpXyz (struct InterpXyzInfoStruct *InfoPtr)
{
	int i, itemp;

	extern const char *GetParametersMsg[];
	extern const char *LinearInterpMsg[];

	switch (InfoPtr->indep_value)
	{   case X_VALUE:
		{
			/* copy X values */
			for (i=0; i<InfoPtr->num_in; i++)                 /* X */
				InfoPtr->x[i] = InfoPtr->in_xyzp[i][0];
			for (i=0; i<InfoPtr->num_out; i++)
				InfoPtr->out_xyz[i][0] = InfoPtr->out_indep[i];

			/* interpolate for Y and Z */
			switch (InfoPtr->interp_method)
			{   case LINEAR:
					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;

				case CUBIC_SPLINE:
					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];
					break;
			}
			break;
		}
		case Y_VALUE:
		{
			/* copy Y values */
			for (i=0; i<InfoPtr->num_in; i++)                 /* Y */
				InfoPtr->x[i] = InfoPtr->in_xyzp[i][1];
			for (i=0; i<InfoPtr->num_out; i++)
				InfoPtr->out_xyz[i][1] = InfoPtr->out_indep[i];

			/* interpolate for X and Z */
			switch (InfoPtr->interp_method)
			{   case LINEAR:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;

				case CUBIC_SPLINE:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;
			}
			break;
		}
		case Z_VALUE:
		{
			/* copy Z values */
			for (i=0; i<InfoPtr->num_in; i++)                 /* Z */
				InfoPtr->x[i] = InfoPtr->in_xyzp[i][2];
			for (i=0; i<InfoPtr->num_out; i++)
				InfoPtr->out_xyz[i][2] = InfoPtr->out_indep[i];

			/* interpolate for X and Y */
			switch (InfoPtr->interp_method)
			{   case LINEAR:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					break;

				case CUBIC_SPLINE:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];
					break;
			}
			break;
		}
		case T_VALUE:
		{
			/* copy traverse values */
			for (i=0; i<InfoPtr->num_in; i++)                 /* T */
				InfoPtr->x[i] = InfoPtr->in_trav[i];

			/* interpolate for X, Y, Z */
			switch (InfoPtr->interp_method)
			{   case LINEAR:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;

				case CUBIC_SPLINE:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;
			}   /* end of switch (InfoPtr->interp_method) block */
			break;
		}
		case P_VALUE:
		{
			/* copy P values */
			for (i=0; i<InfoPtr->num_in; i++)                 /* P */
				InfoPtr->x[i] = InfoPtr->in_xyzp[i][3];

			/* interpolate for X, Y, and Z */
			switch (InfoPtr->interp_method)
			{   case LINEAR:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = LinearInterp((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",LinearInterpMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;

				case CUBIC_SPLINE:
					for (i=0; i<InfoPtr->num_in; i++)         /* X */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][0];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}

					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][0] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Y */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][1];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][1] = InfoPtr->ynew[i];

					for (i=0; i<InfoPtr->num_in; i++)         /* Z */
						InfoPtr->y[i] = InfoPtr->in_xyzp[i][2];
					itemp = Spline((long)InfoPtr->num_in,(long)InfoPtr->num_out,InfoPtr->x,InfoPtr->y,InfoPtr->out_indep,InfoPtr->ynew);
					if (itemp > 0)
					{   printf("\n%s\n",SplineMsg[itemp]);
						return 1;
					}
					for (i=0; i<InfoPtr->num_out; i++)
						InfoPtr->out_xyz[i][2] = InfoPtr->ynew[i];

					break;
			}
			break;
		}
	}   /* end of switch (InfoPtr->indep_value) block */
	return 0;
}
/****************************** LinearInterp() ******************************/
/*  This function performs a linear interpolation to get the values
 *  "ynew[i]" as a function of "xnew[i]".  "xnew[]" is an independent variable
 *  array assigned values between (and/or inclusive of) the endpoints of the
 *  independent series "x[]".  The values "y[i]" are defined for every point
 *  "x[i]".  Essentially, "ynew[]" is interpolated along "y[]".
 *
 *  Parameters:
 *  Input:
 *      n       - size of x[] and y[] arrays.
 *      new     - size of xnew[] and ynew[] arrays.
 *      x[]     - independent variable array.
 *      y[]     - dependent variable array.
 *      xnew[]  - interpolated independent variable array.  The start and end
 *                of these values should not exceed the start and end of x[].
 *  Output:
 *      ynew[]  - interpolated dependent variable array.
 *
 *  Requires: "intrpxyz.h".
 *  Calls: none.
 *  Returns: 0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *LinearInterpMsg[] =
{   "LinearInterp(): No errors.",
	"LinearInterp() ERROR: Less than 2 values in array.",
	"LinearInterp() ERROR: interpolated independent variable array exceeds limits.",
}
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: June 19, 1997
 */
int LinearInterp (long n,long nnew,double x[],double y[],double xnew[],double ynew[])
{
    long i, j, ip1, im1;
    double h,t;

    /* Pre-qualify input */
    if ((n < 2) || (nnew < 1)) return 1;
#if 0
    if (x[0] < x[n-1])
        if ((xnew[0] < x[0]) || (xnew[nnew-1] > x[n-1])) return 2;
    else
        if ((xnew[0] > x[0]) || (xnew[nnew-1] < x[n-1])) return 2;
#endif

	/* Interpolate */
	if (x[0] < x[n-1])
	{    i = 0;                    /* first member of x[] and y[] arrays */
		for (j=0; j<nnew; j++)    /* look at each member of xnew[] array */
		{   t = xnew[j];
			do
			{   i++;
			} while ((i < n-1) && (t > x[i]));
			ip1 = i;
			i--;
			h  = t - x[i];
			ynew[j] = y[i] + ( (h/(x[ip1] - x[i])) * (y[ip1] - y[i]) );
		}
	} else
	{    i = n-1;                      /* first member of x[] and y[] arrays */
		for (j=nnew-1; j>=0; j--)   /* look at each member of xnew[] array */
		{   t = xnew[j];
			do
			{   i--;
			} while ((i >= 0) && (t > x[i]));
			im1 = i;
			i++;
			h  = t - x[i];
			ynew[j] = y[i] + ( (h/(x[im1] - x[i])) * (y[im1] - y[i]) );
		}
	}

	return 0;
}
/**************************** end of linintrp.c *****************************/
