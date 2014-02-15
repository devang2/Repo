/* GEN_IO.C - source code for functions in GPR_IO.LIB
 *
 *  NOTE: If this, or any source file, for GPR_IO.LIB is modified,
 *        then nearly all GPR programs will have to be recompiled!
 *
 * Jeff Lucius, Geophysicist
 * U.S. Geological Survey
 * Geologic Division, Mineral Resources Program
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@usgs.gov
 *
 * To compile for use in GPR_IO.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c GEN_IO.C dzt_io.obj
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /c compiles only, no linking.
 *
 * Functions in this module:
 *   ExtractSsInfoFromSegy
 *   GetGprFileType
 *   GetGprSubGrid
 *   GetMrkData
 *   GetSubImage8
 *   GetXyzData
 *   InitGprInfoStruct
 */

/* Include header files */
#include "gpr_io.h"

/* Declare globals */
const char *GetGprFileTypeMsg[] =
{   "GetGprFileType(): No errors.",
    "GetGprFileType() ERROR: One or more function arguments is NULL.",
    "GetGprFileType() ERROR opening input GPR file: file or path name not found.",
    "GetGprFileType() ERROR opening input GPR file: read access denied.",
    "GetGprFileType() ERROR : Unable to open input GPR file.",
	"GetGprFileType() ERROR: Short count from fread().",
    "GetGprFileType() ERROR opening input *.HD file: file or path name not found.",
    "GetGprFileType() ERROR opening input *.HD file: read access denied.",
    "GetGprFileType() ERROR : Unable to open input info file.",
} ;
const char *GetGprSubGridMsg[] =
{   "GetGprSubGrid(): No errors.",
	"GetGprSubGrid() Error: fopen() of GPR data file failed.",
	"GetGprSubGrid() Error: Short count from fread().",
	"GetGprSubGrid() Error: EOF encountered -- grid rows exceed number of scans.",
	"GetGprSubGrid() Error: Number of samples exceeds number of grid rows.",

	"GetGprSubGrid() Error: Unable to allocate temporaray storage.",
	"GetGprSubGrid() Error: NULL pointer in argument list.",
	"GetGprSubGrid() Error: Invalid datatype.",
	"GetGprSubGrid() Error: trace header not multiple of sample size.",
} ;
const char *GetSubGrid16uMsg[] =
{   "GetSubGrid16u: No errors.",
    "GetSubGrid16u() Error: Unable to open input data file.",
    "GetSubGrid16u() Error: Short count from fread().",
    "GetSubGrid16u() Error: EOF encountered -- gird cols exceed number of traces.",
    "GetSubGrid16u() Error: Last trace sample exceeds number of grid rows.",
    "GetSubGrid16u() Error: Unable to allocate temporaray storage.",
    "GetSubGrid16u() Error: Invalid data type specified.",
    "GetSubGrid16u() Error: NULL pointer in argument list.",
    "GetSubGrid16u() Error: Max and min values must be given for float point data.",
} ;
const char *GetMrkDataMsg[] =
{   "GetMrkData() No Errors.",
    "GetMrkData() Error: Unable to open MRK file.",
	"GetMrkData() Error: Problem getting data from MRK file.",
    "GetMrkData() Error: Unable to allocate storage for MRK values.",
    "GetMrkData() Error: EOF reached or error before all ticks retreived.",
	"GetMrkData() Error: Less than two ticks in file.",
} ;
const char *GetSubImage8Msg[] =
{   "GetSubImage8(): No errors.",
    "GetSubImage8() Error: Unable to open input data file.",
    "GetSubImage8() Error: Short count from fread().",
    "GetSubImage8() Error: EOF encountered -- image cols exceed number of scans.",
	"GetSubImage8() Error: Last scan sample exceeds number of image rows.",
    "GetSubImage8() Error: Unable to allocate temporaray storage.",
    "GetSubImage8() Error: Invalid data type specified.",
    "GetSubImage8() Error: NULL pointer in argument list.",
    "GetSubImage8() Error: Max and min values must be given for float point data.",
} ;
const char *GetXyzDataMsg[] =
{   "GetXyzData() No Errors.",
    "GetXyzData() Error: Unable to open XYZ file.",
    "GetXyzData() Error: Problem getting data from XYZ file.",
	"GetXyzData() Error: Unable to allocate storage for XYZ values.",
    "GetXyzData() Error: EOF reached or error before all values retreived.",
    "GetXyzData() Error: Less than two ticks in file.",
} ;

/* Local function prototypes */
static char *TrimStr(char *p);

/******************************* GetMrkData() *******************************/
/* Open MRK file, allocate storage, retrieve values, and close file.
 * The file is ASCII text, with one item on each line.  The first line
 * contains the number of tick marks, and the trace numbers for each mark are
 * placed one to a line. For example:
 3
1024
2050
3096
 * Three tick marks.  They are 1024, 2050, and 3096.
 *
 * NOTE: This function allocates storage for a variable declared in another
 *       function.  The address must be passed to this function.
 *
 * Parameter list:
 *  char *filename - string, name of ASCII file
 *  int *num_ticks - address of variable, will hold number of tick marks
 *  long **ticks   - address of pointer, storage allocated here, the trace
 *                   numbers from the file
 *
 * Requires: <stdio.h>, <stdlib.h>, "gpr_io.h".
 * Calls: fopen, fclose, fgets, malloc, free, atoi.
 * Returns: 0 on success,
 *         >0 on error (offset into GetDataMsg[])
 *
const char *GetMrkDataMsg[] =
{   "GetMrkData() No Errors.",
	"GetMrkData() Error: Unable to open MRK file.",
    "GetMrkData() Error: Problem getting data from MRK file.",
    "GetMrkData() Error: Unable to allocate storage for MRK values.",
    "GetMrkData() Error: EOF reached or error before all ticks retreived.",
    "GetMrkData() Error: Less than two ticks in file.",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: June 23, 1994
 */
int GetMrkData (char *filename,int *num_ticks,long **ticks)
{
    char str[MAX_CMDLINELEN];
    int num;
    FILE *infile;

    /* Attemp to open MRK file */
    if ((infile = fopen(filename,"rt")) == NULL) return 1;

    /* Get number of tick marks */
    if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)
    {   fclose (infile);
        return 2;
    }
    *num_ticks = atoi(str);
    if (*num_ticks < 2)
    {   *num_ticks = 0;
        return 5;
    }

    /* Allocate storage for tick trace numbers */
    (*ticks) = (long *)malloc(*num_ticks * sizeof(long));
    if ((*ticks) == NULL)
    {   fclose (infile);
        return 3;
    }

    /* Retrieve the values */
    for (num=0; num< *num_ticks; )
    {   if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)  break;
        if (str[0] == 0)  continue;
        (*ticks)[num] = atol(str);
        num++;
    }
    fclose(infile);

    /* Did we get enough  values? */
    if (num < *num_ticks)
    {   free(*ticks);
        *ticks = NULL;
        *num_ticks = 0;
        return 4;
    }
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
 * Three tick marks, with coordinates (10.0,0.0,293.1), (20.0,0.0,293.2),
 * and (30.0,0.0,293.3).
 *
 * NOTE: This function allocates storage for a variable declared in another
 *       function.  The address must be passed to this function.
 *
 * Parameter list:
 *  char *filename - string, name of ASCII file
 *  int *num_ticks - address of variable, will hold number of tick marks
 *  double ***xyz  - address of 2D pointer, storage allocated here, the X, Y,
 *                   and Z values from the file
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "gpr_io.h".
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
    "GetXyzData() Error: Less than two ticks in file.",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 18, 1994
 */
int GetXyzData (char *filename,int *num_ticks,double ***xyz)
{
    char str[MAX_CMDLINELEN], *cp;
    int i,j,num;
    FILE *infile;

    /* Attemp to open XYZ file */
    if ((infile = fopen(filename,"rt")) == NULL) return 1;

    /* Get number of tick marks */
    if (fgets(str,MAX_CMDLINELEN-1,infile) == NULL)
    {   fclose(infile);
        return 2;
    }
    *num_ticks = atoi(str);
    if (*num_ticks < 2)
    {   *num_ticks = 0;
        return 5;
    }

    /* Allocate storage for tick X-Y-Z coordinates */
    (*xyz) = (double **)malloc(*num_ticks * sizeof(double *));
    if ((*xyz) == NULL)
    {   fclose(infile);
        return 3;
    }
    for (i=0; i< *num_ticks; i++)
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
    for (num=0; num< *num_ticks;)
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
    if (num < *num_ticks)
    {   for (i=0; i< *num_ticks; i++)  free((*xyz)[i]);
        free(*xyz);
        (*xyz) = NULL;
        *num_ticks = 0;
        return 4;
    }
    return 0;
}
/************************** ExtractSsInfoFromSegy() *************************/
/* Extract information from the ASCII portion of the SEG-Y file header
 * following Sensors and Software's PulseEKKO format.
 *
 * Parameters:
 * struct GprInfoStruct *InfoPtr  - address of info structure
 * struct SegyReelHdrStruct *HdrPtr - address of header structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *       Values in the InfoPtr structure should have previously initialized.
 *       HdrPtr should point to a copy of the SEGY file header.
 *
 * Requires: <conio.h>, <stdio.h>, <stdlib.h>, <string.h>, "assertjl.h",
 *           "gpr_io.h", "jl_defs.h"
 * Calls: fgets, printf, puts, strcmp, strchr, assert, strcpy, calloc,
 *        strncat, TrimStr, ftell, fseek, strupr, malloc, free, getch.
 * Returns: void.
 *
const char *SEGY_ASCII_CMDS[] =
{   "NUMBER OF TRACES","NUMBER OF PTS/TRC","TIMEZERO AT POINT",
	"TOTAL TIME WINDOW","STARTING POSITION","FINAL POSITION",
	"STEP SIZE USED","POSITION UNITS","NOMINAL FREQUENCY",
	"ANTENNA SEPARATION","PULSER VOLTAGE (V)","NUMBER OF STACKS",
	"SURVEY MODE","TRACES PER SECOND","METERS PER MARK",
	"NUMBER OF GAIN PTS","GAIN POINTS",NULL,
} ;
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: May 23, 1996; August 14, 2001;
 */
void ExtractSsInfoFromSegy (struct GprInfoStruct *InfoPtr,
                            struct SegyReelHdrStruct *HdrPtr)
{
    char  *cp, *cp2, cmdstr[30];
    char string[40][80];    /* should have 3200 continuous bytes */
    int i, num, itemp, found, str_num, gain_pts_found, date_found;
    int c_skip,comment_size,record_length;  /* bytes */
    int max_records;
    int day, month, year;
    double dtemp;
    extern int Debug;

    /* Reality check */
    assert(HdrPtr != NULL && InfoPtr != NULL);
    if (Debug)
    {   printf("ExtractSsInfoFromSegy():\n");
        getch();
    }

    /* Copy ASCII part of header and terminate strings */
    comment_size = sizeof(HdrPtr->comment);
    record_length = 80;                          /* bytes */
    max_records = comment_size / record_length;  /* should be 40 */
    memcpy((void*)string,(void*)HdrPtr->comment,
            (size_t)_MINN(comment_size,3200));
    for (i=0; i<max_records; i++)  string[i][record_length-1] = 0;
    if (Debug)
    {   for (i=0; i<40; i++)
        {   printf("string[%d]=%s\n",i,string[i]);
			getch();
		}
    }

    /* Skip C01 C02 etc. if present at start of each string */
    c_skip = 0;
    if (string[1][0] == 'c' ||  string[1][0] == 'C') c_skip = 4;
    if (Debug) printf("c_skip = %d\n",c_skip);

    /* Initialize string arrays */
    InfoPtr->job_number[0] = 0;
    InfoPtr->title1[0] = 0;
    InfoPtr->title2[0] = 0;

    /* Get date */
    date_found = -1;
    for (i=0; i<40; i++)
    {    if (strchr(string[i],'/'))
         {   date_found = i;
             break;  /* out of for loop */
         }
    }
    if (date_found >= 0)
    {   day = month = year = -1;
        cp = string[date_found];
        cp2 = strchr(cp,'/');
        if (cp2)
        {   *cp2 = 0;
            cp = cp2 - 2;
            if (Debug) printf("cp=%s\n",cp);
            day = atoi(cp);
            cp = cp2+1;
            cp2 = strchr(cp,'/');
            if (cp2)
            {   *cp2 = 0;
                if (Debug) printf("cp=%s\n",cp);
                month = atoi(cp);
                cp = cp2+1;
                if (Debug) printf("cp=%s\n",cp);
                year = atoi(cp);
            }
        }
        if (Debug) printf("date found %d: year=%d month=%d day=%d\n",date_found,year,month,day);
        i = 0;
        if (i < date_found)
		{   strncpy(InfoPtr->job_number,string[i]+c_skip,9);
			InfoPtr->job_number[9] = 0;
        }
        i++;
        if (i < date_found)
        {   strncpy(InfoPtr->title1,string[i]+c_skip,69);
            InfoPtr->title1[69] = 0;
        }
        i++;
        if (i < date_found)
        {   strncpy(InfoPtr->title2,string[i]+c_skip,69);
            InfoPtr->title2[69] = 0;
        }
    } else
    {   i = 0;
        if (strchr(string[i],'=') == NULL)
        {   strncpy(InfoPtr->job_number,string[i]+c_skip,9);
            InfoPtr->job_number[9] = 0;
        }
        i++;
        if (strchr(string[i],'=') == NULL)
        {   strncpy(InfoPtr->title1,string[i]+c_skip,69);
            InfoPtr->title1[69] = 0;
        }
        i++;
        if (strchr(string[i],'=') == NULL)
        {   strncpy(InfoPtr->title2,string[i]+c_skip,69);
            InfoPtr->title2[69] = 0;
        }
    }
    if (Debug)
    {   printf("job_number = %s\n",InfoPtr->job_number);
        printf("title1 = %s\n",InfoPtr->title1);
        printf("title2 = %s\n",InfoPtr->title2);
    }
    if (year >= 0 && month >= 0 && day >= 0)
    {   if (year < 70) year += 100; /* if >= 2000 ie. 00 */
        InfoPtr->time_date_created.year  = year + 1900 - 1980;
        InfoPtr->time_date_created.month = month;
        InfoPtr->time_date_created.day   = day;
        if (Debug) puts("time_date_created assigned\n");
    }

    /* Get rest of info */
    gain_pts_found = 0;
	for (str_num=0; str_num<40; str_num++)
	{   cmdstr[0] = 0;                      /* set to 0 so strncat works right */
        if (Debug)
        {   printf("string[%d]=%s\n",str_num,string[str_num]);
            getch();
        }
        cp = strchr(string[str_num],'=');   /* look for equal sign */
        if (cp == NULL) continue;           /* invalid command line so ignore */
        num = (int)(cp-string[str_num]);    /* number of chars before equal sign */
        strncat(cmdstr,string[str_num],(size_t)num);  /* copy to working string */
        TrimStr(cmdstr);                    /* remove leading and trailing blanks */
        if (Debug) printf("cmdstr = %s\n",cmdstr);
        found = -1;
        i = 0;
        while (SEGY_ASCII_CMDS[i])     /* requires last member to be NULL */
        {   if (strstr(strupr(cmdstr),SEGY_ASCII_CMDS[i]) != NULL)
            {   found = i;             /* store index into SEGY_ASCII_CMDS[] */
                break;
            }
            i++;
        }
        if (found == -1) continue;    /* bad or missing command word */
        cp++;                         /* step 1 past equal sign */
        TrimStr(cp);                  /* remove leading and trailing blanks */
        if (Debug)  printf("cp=%s\n",cp);
        switch (found)  /* cases depend on same order in SEGY_ASCII_CMDS[] */
        {   case 0:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->total_traces = itemp;
                if (Debug)
                    printf("total_traces = %d\n",InfoPtr->total_traces);
                break;
            case 1:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->in_samps_per_trace = itemp;
                if (Debug)
                    printf("in_samps_per_trace = %d\n",InfoPtr->in_samps_per_trace);
                break;
            case 2:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->timezero_sample = itemp;
                if (Debug)
                    printf("timezero_sample = %d\n",InfoPtr->timezero_sample);
                break;
            case 3:
				dtemp = atof(cp);
				if (dtemp > 0.0) InfoPtr->in_time_window = dtemp;
                if (Debug)
					printf("in_time_window = %g\n",InfoPtr->in_time_window);
                break;
            case 4:
                InfoPtr->starting_position = atof(cp);
                if (Debug)
                    printf("starting_position = %g\n",InfoPtr->starting_position);
                break;
            case 5:
                InfoPtr->final_position = atof(cp);
                if (Debug)
                    printf("final_position = %g\n",InfoPtr->final_position);
                break;
            case 6:
                InfoPtr->position_step_size = atof(cp);
                if (Debug)
                    printf("position_step_size = %g\n",InfoPtr->position_step_size);
                break;
            case 7:
                InfoPtr->position_units[0] = 0;
                strncpy(InfoPtr->position_units,cp,sizeof(InfoPtr->position_units));
                InfoPtr->position_units[sizeof(InfoPtr->position_units)-1] = 0;
                if (Debug)
                    printf("position_units = %s\n",InfoPtr->position_units);
                break;
            case 8:
                itemp = atoi(cp);
                if (dtemp > 0) InfoPtr->nominal_frequency = itemp;
                if (Debug)
                    printf("nominal_frequency = %d\n",InfoPtr->nominal_frequency);
                break;
            case 9:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->antenna_separation = dtemp;
                if (Debug)
                    printf("antenna_separation = %g\n",InfoPtr->antenna_separation);
                break;
            case 10:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->pulser_voltage = dtemp;
                if (Debug)
                    printf("pulser_voltage = %g\n",InfoPtr->pulser_voltage);
                break;
            case 11:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->number_of_stacks = itemp;
                if (Debug)
                    printf("number_of_stacks = %d\n",InfoPtr->number_of_stacks);
                break;
            case 12:
                InfoPtr->survey_mode[0] = 0;
                strncpy(InfoPtr->survey_mode,cp,sizeof(InfoPtr->survey_mode));
                InfoPtr->survey_mode[sizeof(InfoPtr->survey_mode)-1] = 0;
                if (Debug)
                    printf("survey_mode = %s\n",InfoPtr->survey_mode);
                break;
            case 13:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->traces_per_sec = dtemp;
                if (Debug)
                    printf("traces_per_sec = %g\n",InfoPtr->traces_per_sec);
                break;
            case 14:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->meters_per_mark = dtemp;
                if (Debug)
                    printf("meters_per_mark = %g\n",InfoPtr->meters_per_mark);
                break;
            case 15:
                itemp = atoi(cp);
                if (itemp > 1)  InfoPtr->num_gain_pts = itemp;
                InfoPtr->gain_pts = (double *)malloc(InfoPtr->num_gain_pts*sizeof(double));
                if (InfoPtr->gain_pts == NULL)
                    InfoPtr->num_gain_pts = 0;
                if (Debug)
                    printf("num_gain_pts = %d\n",InfoPtr->num_gain_pts);
                break;
            case 16:
                if (InfoPtr->num_gain_pts <= 0) break;
                cp2 = cp;
                for (i=gain_pts_found; i<InfoPtr->num_gain_pts; i++)
                {   InfoPtr->gain_pts[i] = atof(cp2);
                    gain_pts_found ++;
                    cp2 = strchr(cp2,' ');
                    if (cp2 == NULL) break; /* out of for loop */
                }
                break;
            default:
                break;
        }
        if (Debug) getch();
    }
    /* Check on gain points */
    if (gain_pts_found == 0)
    {   if (InfoPtr->gain_pts) free(InfoPtr->gain_pts);
        InfoPtr->num_gain_pts = 0;
    }
    if (gain_pts_found < InfoPtr->num_gain_pts)
        InfoPtr->num_gain_pts = gain_pts_found;

    /* Get processing history */
    found = -1;
    for (str_num=0; str_num<40; str_num++) /* find "SURVEY MODE" */
    {    if (strstr(strupr(string[str_num]),"SURVEY MODE"))
         {   found = str_num;
             break; /* out of for loop */
         }
    }
    found++;   /* go to next record */
    if (found > 0)
    {   int last = 0, bytes = 0, len;

        for (str_num=found; str_num<40; str_num++)  /* find "blank" lines */
        {   cp = string[str_num];
            TrimStr(cp);               /* strip leading and trailing blanks */
            len = strlen(cp) - c_skip;
            if (len < 5)
            {   last = str_num;
                break; /* out of for loop */
            } else
                bytes += len + 1;      /* add up required storage bytes */
        }
        if (last > found)
        {   /* allocate storage for buffer */
            InfoPtr->proc_hist_bytes = bytes + 1;
            InfoPtr->proc_hist = (char *)calloc((size_t)InfoPtr->proc_hist_bytes,1);
            if (InfoPtr->proc_hist == NULL)
            {   InfoPtr->proc_hist_bytes = 0;
            } else
            {   InfoPtr->proc_hist[InfoPtr->proc_hist_bytes-1] = 0;
                cp = InfoPtr->proc_hist;           /* point to start of buffer */
                for (i=found; i<last; i++)
                {   if ( cp + strlen(string[i]+c_skip) >=
                         InfoPtr->proc_hist + InfoPtr->proc_hist_bytes )
                        break; /* out of for loop */
                    strncpy(cp,string[i]+c_skip,79-c_skip);
                    cp = strchr(cp,0);             /* point at NULL terminator */
                    *cp = '\n';                    /* change NULL to CR */
                    cp ++;                         /* point past CR */
                    if (cp >= InfoPtr->proc_hist + InfoPtr->proc_hist_bytes)
                        break; /* out of for loop */
                }
                InfoPtr->proc_hist[InfoPtr->proc_hist_bytes-1] = 0;
            }
            if (Debug)
                if (InfoPtr->proc_hist[0])
                    printf("proc_hist = %s\n",InfoPtr->proc_hist);
        }
    }
    return;
}
/********************************* TrimStr() ********************************/
/* Deletes leading and trailing blanks and non-printable characters from
 * the string at p.  Extended ASCII characters (0x80 to 0xFF) are considered
 * non-printable.  If extended ASCII characters are to be kept, change code
 * to "...<0x21 || ....==0x7F ...".  Beginning of string remains at p.  If
 * the string consists entirely of spaces, tabs or other unwanted characters,
 * the NULL string is returned (p[0] will equal 0).
 *
 * Requires: <string.h>.
 * Calls: strlen.
 * Returns: char* to beginning of string.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 3, 1994
 */
static char *TrimStr (char *p)
{
    int end,i,len,trail=0,lead=0;

    /* return if NULL string */
    if ((end=(len=strlen(p))) == 0)  return p;

    /* count blanks etc. at end */
    while ((end>0) && (p[--end]<0x21 || p[end]>0x7E))  trail++;

    /* strip off blanks etc. by inserting terminating 0 */
    if (trail)  p[len-=trail] = 0;

    /* return if NULL string */
    if (len == 0) return p;

    /* count blanks etc. at beginning */
    while (p[lead]<0x21 || p[lead]>0x7E)  lead++;

    /* remove leading blanks etc. by shifting characters forward */
    if (lead)  for (i=lead; i<=len; i++)  p[i-lead] = p[i];

    /* return pointer to start of string */
    return p;
}
/****************************** GetGprFileType() ****************************/
/* Determine GPR file storage format by inspecting files
 *
 * Parameter list:
 *  int *file_hdr_bytes    - address of variable, bytes in the file header
 *  int *num_hdrs          - address of variable, number of file headers
 *  int *trace_hdr_bytes   - address of variable, bytes in each trace header
 *  int *samples_per_trace - address of variable, samples per trace
 *  int *num_channels       - address of variable, number of channels per file
 *  long *num_traces       - address of variable, number of traces in file
 *  int *input_datatype    - address of variable, coded data element type
 *          1 for 1-byte signed chars;
 *         -1 for 1-byte unsigned chars
 *          2 for 2-byte signed ints or shorts;
 *         -2 for 2-byte unsigned shorts
 *         -5 for 2-byte unsigned shorts, but only first 12-bits used
 *          3 for 4-byte signed ints or longs;
 *         -3 for 4-byte unsigned longs
 *         -6 for 4-byte unsigned longs, but only first 24-bits used
 *          4 for 4-byte floats
 *          8 for 8-byte doubles
 *  int *total_time        - address of variable, nanoseconds per trace
 *  int *file_type         - address of variable, storage format as coded in
 *                           GPR_DISP.H.  Codes are:
 *               0    invalid value
 *     DZT     = 1    GSSI SIR-10 (ver. 4 or 5)
 *     DT1     = 2    S&S PulseEKKO
 *     SGY     = 3    SEG SEG-Y
 *     OTHER   = 4    user defined (unknown)
 *  char *filename         - pointer to string, name of GPR data file
 *
 * Requires: <io.h>, <stdio.h>, <string.h>, "assertjl.h", "gpr_io.h".
 * Calls: assert, fopen, fclose, fread, rewind, fseek, ftell, memset, access,
 *        strcpy, strchr, strncat, strstr, strupr, strcat, strupr,
 *        TrimStr, SWAP_2BYTES.
 *
 * Returns: 0 on success,
 *         >0 on error.
const char *GetGprFileTypeMsg[] =
{ GetGprFileType(): No errors.",
  GetGprFileType() ERROR: One or more function arguments is NULL.",
  GetGprFileType() ERROR opening input GPR file: file or path name not found.",
  GetGprFileType() ERROR opening input GPR file: read access denied.",
  GetGprFileType() ERROR : Unable to open input GPR file.",
  GetGprFileType() ERROR: Short count from fread().",
  GetGprFileType() ERROR opening input *.HD file: file or path name not found.",
  GetGprFileType() ERROR opening input *.HD file: read access denied.",
  GetGprFileType() ERROR : Unable to open input info file.",
} ;
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 14, 1996
 *       August 8, 1997  - check for modofied SIR-10
 *
*/
int GetGprFileType (int *file_hdr_bytes,int *num_hdrs,int *trace_hdr_bytes,
					int *samples_per_trace,int *num_channels,long *num_traces,
					int *input_datatype,int *total_time,int *file_type,
					char *filename)
{
	int i,error;
	FILE *infile;
	extern int Debug;    /* must be application source or elsewhere */
	extern int errno;    /* in <errno.h> */

	/* Reality check */
	if (file_hdr_bytes == NULL    || trace_hdr_bytes == NULL ||
		samples_per_trace == NULL || input_datatype == NULL  ||
        total_time == NULL        || file_type == NULL ||
        filename == NULL)  return 1;

	/* Attempt to open file */
	if ((infile = fopen(filename,"rb")) == NULL)
	{   if (access(filename,4))
		{   if      (errno == ENOENT) error = 2;
			else if (errno == EACCES) error = 3;
			else                      error = 4;
		}
		return error;
	}

	/* Initialize variables */
    *file_hdr_bytes    = -1;   /* set these to invalid values */
	*num_hdrs          = -1;
	*trace_hdr_bytes   = -1;
	*samples_per_trace = -1;
	*num_channels      = -1;
	*input_datatype    =  0;
	*total_time        =  0;
	*file_type         =  0;

	error              =  1;   /* set for failure */

/***** Check if GSSI DZT file *****/
	if (error)
	{   unsigned char uctemp;
		unsigned short ustemp,checksum;
		int            header_bytes,channel,local_error;
		long           traces,start_byte,last_byte,num_bytes;
		struct DztHdrStruct hdr;  /* in dzt_pgms.h */

	/***** Determine if a valid (recognized) DZT header *****/
		if (Debug) puts("Checking DZT header format");

		/* Get header as if 1024-bytes long */
		local_error = error = 0;
		header_bytes = sizeof(struct DztHdrStruct);  /* 1024 */
		assert(header_bytes == 1024);                /* double check */
		if (fread((void *)&hdr,(size_t)header_bytes,(size_t)1,infile) < 1)
		{   fclose(infile);
			return 5;
		}

		/* Check essential information */
		if ( (hdr.rh_bits !=  8) && (hdr.rh_bits != 16) &&
			 (hdr.rh_bits != 32)
		   )  error = 1;         /* sample size incorrect */
		if (!error)
		{   error = 1;           /* number of samples not a power of 2 */
			for (i=1; i<=16; i++)
			{   if (hdr.rh_nsamp == pow(2,(double)i))
				{   error = 0;
					break;   /* out of for loop */
				}
			}
		}
		/* Probably a modified DZT file if num samples not power of 2 */
		if (error == 1)
		{	*file_type = MOD_DZT;
			error = 0;
		}

		/* Check required information */
		if (!error)
		{   *num_hdrs = hdr.rh_nchan;
			/* as of 11/17/95 only 1 to 4 channels, but SIR-10H may have up to 16 */
			if (*num_hdrs < 1 || *num_hdrs > 16)
				error = 1;       /* invalid number of channels */
		}

	/***** Determine if old (512-byte) or new (1024-byte) header *****/
		if (!error && *file_type != MOD_DZT)
		{   /* check last 50 bytes in header to see if all zeroes -- highly unlikely
			   that processing/comment area is full or that data will be all 0's
			 */
			fseek(infile,(long)(*num_hdrs * 1024) - 51L,SEEK_SET);
			for (i=0; i<50; i++)
			{   if (fread((void *)&uctemp,(size_t)1,(size_t)1,infile) < 1)
				{   fclose(infile);
					return 5;
				}
				if (uctemp != 0)
				{   header_bytes = 512;
					fseek(infile,(long)(*num_hdrs * 512) - 51L,SEEK_SET);
					for (i=0; i<50; i++)
					{   if (fread((void *)&uctemp,(size_t)1,(size_t)1,infile) < 1)
						{   fclose(infile);
							return 5;
						}
						if (uctemp != 0) error = 1;
				   }
				   break;  /* out of for loop */
				}
			}
		}

	/***** Determine number of GPR scans in file *****/
		if (!error)
		{   fseek(infile, 0L, SEEK_END);       /* go to end of file */
			last_byte = ftell(infile);         /* get last location (byte) */
			num_bytes = last_byte - (*num_hdrs * header_bytes);
			*num_traces = num_bytes / (hdr.rh_nsamp * (hdr.rh_bits / 8));
			*num_traces /= *num_hdrs;
		}
		if (!error)
		{   /* assign values to function arguments */
			*file_hdr_bytes    = header_bytes;
			*trace_hdr_bytes   = 0;
			*samples_per_trace = hdr.rh_nsamp;
			*num_channels      = hdr.rh_nchan;
			switch (hdr.rh_bits)
			{   case  8: *input_datatype = -1; break;
				case 16: *input_datatype = -2; break;
				case 32: *input_datatype = -3; break;
				default: *input_datatype =  0; break;
			}
			*total_time        = hdr.rh_range;
			if (*file_type != MOD_DZT)
				*file_type  = DZT;
		 }
	}

/***** Check if S&S DT1 file *****/
	if (error && strstr(strupr(filename),"DT1"))
	{   char  str[80], *cp, cmdstr[30], hd_filename[80];
		int num,traces,num_samples,found,total_time_ns;
		FILE *infile2;
		struct SsTraceHdrStruct hdr;      /* in "dt1_pgms.h" */
		extern const char *SS_HD_CMDS[];  /* in "dt1_pgms.h" */

		/* Check HD info file first */
		if (Debug) puts("Checking S&S HD format");
		strcpy(hd_filename,filename);
		cp = strchr(hd_filename,'.');
		if (cp) *cp = 0;
		strcat(hd_filename,".HD");
        if ((infile2 = fopen(hd_filename,"rt")) == NULL)
        {   if (access(hd_filename,4))
			{   if      (errno == ENOENT) error = 6;
                else if (errno == EACCES) error = 7;
				else                      error = 8;
            }
			fclose(infile);
            return error;
		}

		error = 0;                    /* reset for failure */
		while (fgets(str,80,infile2) > 0)
		{   cmdstr[0] = 0;            /* set to 0 so strncat works right */
            cp = strchr(str,'=');     /* look for equal sign */
            if (cp == NULL) continue; /* invalid command line so ignore */
			num = (int)(cp-str);      /* number of chars before equal sign */
            strncat(cmdstr,str,(size_t)num);  /* copy to working string */
            TrimStr(cmdstr);          /* remove leading and trailing blanks */
            found = -1;
			i = 0;
            while (SS_HD_CMDS[i])     /* requires last member to be NULL */
            {   if (strcmp(strupr(cmdstr),SS_HD_CMDS[i]) == 0)
				{   found = i;        /* store index into SS_HD_CMDS[] */
                    error = 0;        /* set for success */
					break;
                }
				i++;
            }
            if (found == -1) continue;    /* bad or missing command word */
            cp++;                         /* step 1 past equal sign */
            TrimStr(cp);                  /* remove leading and trailing blanks */
			switch (found)  /* cases depend on same order in SS_HD_CMD[] */
			{   case 0:
                    traces = atoi(cp);
                    if (Debug) printf("num_traces = %d\n",traces);
					break;
                case 1:
                    num_samples = atoi(cp);
                    if (Debug) printf("num_samples = %d\n",num_samples);
					break;
                case 3:
                    total_time_ns = atoi(cp);
					if (Debug) printf("total_time_ns = %d\n",total_time_ns);
                    break;
				default:
                    break;
			}
        }
        fclose(infile2);

        if (!error)  /* HD file OK, so may be a S&S file */
		{   /* Check DT1 file now */
			if (Debug) puts("Checking S&S DT1 file");
			rewind(infile);
			if (fread((void *)&hdr,sizeof(struct SsTraceHdrStruct),1,infile) < 1)
			{   fclose(infile);
				return 5;   /* fatal error */
            }
			if (hdr.trace_num != hdr.aux_trace_num) error = 1;
			if (Debug) printf("trace_num = %g  aux_trace_num = %g\n",hdr.trace_num,hdr.aux_trace_num);
            if (!error)
			{   /* assign values to function arguments */
                *file_hdr_bytes    = 0;
				*num_hdrs          = 0;
				*trace_hdr_bytes   = sizeof(struct SsTraceHdrStruct);
				*samples_per_trace = num_samples;
				*num_channels      = 1;
				*num_traces        = traces;
				*input_datatype    = 2;  /* signed shorts */
				*total_time        = total_time_ns;
				*file_type         = DT1;
			}
		}
    }

/***** Check if SEG-Y SGY file ******/
	if (error)
	{   int num_samples,total_time_ns;
		long traces,start_byte,last_byte,num_bytes,data_block_bytes;
		struct SegyReelHdrStruct hdr; /* in "sgy_pgms.h" */

		if (Debug) puts("Checking SEG-Y format");
		/* Get header */
		rewind(infile);
		if (fread((void *)&hdr,sizeof(struct SegyReelHdrStruct),1,infile) < 1)
		{   fclose(infile);
			return 5;
		}

		/* Swap bytes if neccessary */
		if (hdr.format > 255)
		{   SWAP_2BYTES(hdr.format);   /* data type */
			SWAP_2BYTES(hdr.hns);      /* samples per trace */
			SWAP_2BYTES(hdr.hdt);      /* sample interval in picoseconds */
        }

        /* Verify we have a SEG-Y file header */
        if (hdr.format >= 1 && hdr.format <= 4)
		{   error = 0;               /* set for success */
			num_samples =  hdr.hns;
			total_time_ns = 0.5 + ((hdr.hdt * (num_samples - 1.0) ) / 1000.0);
		}

		if (!error)
		{   /* Determine number of GPR traces in file */
			traces = -1L;                        /* default is error value */
			start_byte = ftell(infile);          /* get current location (byte) */
			fseek(infile, 0L, SEEK_END);         /* go to end of file */
			last_byte = ftell(infile);           /* get last location (byte) */
			fseek(infile, start_byte, SEEK_SET); /* return file pointer to first scan */
			num_bytes = last_byte - start_byte;  /* number of bytes left in file */
			data_block_bytes = num_samples;      /* number of samples per trace */
			/* multiply data_block_bytes by number of bytes per sample */
			switch(hdr.format)
			{   case 3:  data_block_bytes *= 2;  break;
				default: data_block_bytes *= 4;  break;
			}
			/* add size of trace header */
			data_block_bytes += sizeof(struct SegyTraceHdrStruct);
			/* calculate number of traces */
			traces = num_bytes / data_block_bytes;

			/* Assign values to function arguments */
			*file_hdr_bytes    = sizeof(struct SegyReelHdrStruct);
			*num_hdrs          = 1;
			*trace_hdr_bytes   = sizeof(struct SegyTraceHdrStruct);
			*samples_per_trace = num_samples;
			*num_channels      = 1;
			*num_traces        = traces;
			switch(hdr.format)
			{   case 1:            *input_datatype = 4; break; /* floats */
				case 2: case 4: *input_datatype = 3; break; /* signed longs */
				case 3:         *input_datatype = 2; break; /* signed shorts */
			}
			*total_time        = total_time_ns;
			*file_type         = SGY;
		}
	}


/***** Default is other; use invalid values defined above *****/
    if (error)
    {   *file_type = OTHER;
    }

    /* Close file and return success */
    fclose(infile);
    return 0;

}
/****************************** GetGprSubGrid() *****************************/
/* Open a GPR file, copy a subset of the data into the grid, and close the
 * file.
 *
 * Parameters:
 *  char *filename  - pointer to string containing data filename
 *  int  datatype   - type of input data elements, grid, and output
					if ==  1, 1-byte chars;          == -1, unsigned chars
					   ==  2, 2-byte ints or shorts; == -2, unsigned shorts
					   ==  3, 4-byte ints or longs;  == -3, unsigned longs
					   ==  4, 4-byte floats
					   == -5, unsigned shorts but only first 12-bits used
					   == -6, unsigned longs but only first 24-bits used
					   ==  8, 8-byte doubles
 *  long file_header_bytes  - number of bytes in file header
 *  long trace_header_bytes - number of bytes in each trace header
 *  long first_trace - first trace to get from file, indexed from 0
 *  long total_samps - number of samples in a trace on the disk
 *  long num_cols    - number of columns (traces) in grid[][]
 *  long num_rows    - number of rows (samples per scan) in grid[][]
 *  void **grid      - pointer to 2D array [num_cols][num_rows]
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "displaym.h"
 * Calls: fopen, fclose, fread, setvbuf, fseek, feof, realmalloc, realfree.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *GetGprSubGridMsg[] =
{   "GetGprSubGrid(): No errors.",
	"GetGprSubGrid() Error: fopen() of GPR data file failed.",
	"GetGprSubGrid() Error: Short count from fread().",
	"GetGprSubGrid() Error: EOF encountered -- grid rows exceed number of scans.",
	"GetGprSubGrid() Error: Number of samples exceeds number of grid rows.",

	"GetGprSubGrid() Error: Unable to allocate temporaray storage.",
	"GetGprSubGrid() Error: NULL pointer in argument list.",
	"GetGprSubGrid() Error: Invalid datatype.",
	"GetGprSubGrid() Error: trace header not multiple of sample size.",
} ;
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: January 25, 1996
 */
int GetGprSubGrid (char *filename,int datatype,long file_header_bytes,
		long trace_header_bytes,long first_trace,long total_samps,
		long num_cols,long num_rows,void **grid)
{
	void *trace = NULL;
	int error;
	long disk_trace_bytes,col,row,samp_bytes,samp,ltemp,offset,header_samps;
	float ftemp;
	FILE *infile = NULL;
	extern int Debug;

	#if defined(_INTELC32_)
	  char *buffer = NULL;
	  size_t vbufsize = 64512;  /* 63 KB */
	#endif

	if (Debug)
	{   printf("GetGprSubGrid():  filename = %s datatype = %ld  first_trace = %ld\n\
			total_samps = %ld num_cols = %ld  num_rows = %ld\n\
			file_header_bytes = %ld  trace_header_bytes = %ld",
			filename,datatype,first_trace,total_samps,num_cols,
			num_rows,file_header_bytes,trace_header_bytes);
		getch();
	}

	/* Verify parameters */
	if (grid == NULL) return 6;
	switch (datatype)         /* number of bytes per sample */
	{   case 1:  case -1:			        samp_bytes = 1;  break;
		case 2:  case -2:  case -5:         samp_bytes = 2;  break;
		case 3:  case -3:  case 4: case -6: samp_bytes = 4;  break;
		case 8:                             samp_bytes = 8;  break;
		default:
			return 7;
			/* break;  ** (unreachable code) */
	}
	header_samps = trace_header_bytes / samp_bytes;
	if ((header_samps * samp_bytes) != trace_header_bytes)
		return 8;
	if ((total_samps + header_samps) > num_rows)  return 4;

	/* Allocate storage for one trace from disk */
	disk_trace_bytes = trace_header_bytes + (total_samps * samp_bytes);
	trace = (void *)malloc((size_t)disk_trace_bytes);
	if (trace == NULL) return 5;

	/* Open input file */
	if ((infile = fopen(filename,"rb")) == NULL)
	{   free(trace);
		return 1;
	}

	/* Speed up disk access by using large (conventional memory) buffer */
	#if defined(_INTELC32_)
	  realmalloc(buffer,vbufsize);
	  if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
	  else         setvbuf(infile,NULL,_IOFBF,vbufsize);
	#endif

	/* Get data from disk */
	error = 0;
	offset = file_header_bytes + (first_trace * disk_trace_bytes);
	fseek(infile,offset,SEEK_SET);       /* goto first requested trace */
	for (col=0; col<num_cols; col++)
	{   if (fread(grid[col],(size_t)disk_trace_bytes,(size_t)1,infile) < 1)
		{   error = 2;
			if (feof(infile)) error = 3;
			break;
		}
	}

	/* Close file, release buffers, and return */
	fclose(infile);
	free(trace);
	#if defined(_INTELC32_)
	  realfree(buffer);
	#endif
	return error;
}
/****************************** GetSubImage8() ******************************/
/* Open a GPR file, copy a subset of the data into the grid, and close the
 * file.  Data type larger than characters are scaled down to 8-bits.
 * Floating point types are converted using user-defined data extremes.
 *
 * Versions after May 14, 1997 allow scaling of all data types, not just
 * floating points.
 *
 * Parameters:
 *  char *filename   - pointer to string containing GPR filename
 *  int  datatype    - type of input data elements, grid, and output
                    if ==  1, 1-byte chars;          == -1, unsigned chars
                       ==  2, 2-byte ints or shorts; == -2, unsigned shorts
                       ==  3, 4-byte ints or longs;  == -3, unsigned longs
                       ==  4, 4-byte floats
                       == -5, unsigned shorts but only first 12-bits used
                       == -6, unsigned longs but only first 24-bits used
                       ==  8, 8-byte doubles
 *  int  if_image    - if stored as an image, row by row, first row is "top"
 *  double min_data_val
 *  double max_data_val
 *  long file_header_bytes  - number of bytes in file header
 *  long trace_header_bytes - number of bytes in each trace header
 *  long first_trace - first trace to get from file, indexed from 0
 *  long last_trace  - last trace to get from file, indexed from 0
 *  long skip_traces - number of traces to skip for every one read in
 *  long first_samp  - first sample to store in grid, indexed from 0
 *  long total_samps - number of samples in a trace on the disk
 *  long num_cols    - number of columns (traces) in grid[][]
 *  long num_rows    - number of rows (samples per scan) in grid[][]
 *  unsigned char **image - pointer to 2D array [num_rows][num_cols]
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, <stdlib.h>, "gpr_io.h"
 * Calls: fopen, fclose, fread, setvbuf, fseek, feof.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *GetSubImage8Msg[] =
{"GetSubImage8(): No errors.",
 "GetSubImage8() Error: Unable to open input data file.",
 "GetSubImage8() Error: Short count from fread().",
 "GetSubImage8() Error: EOF encountered -- image cols exceed number of scans.",
 "GetSubImage8() Error: Last scan sample exceeds number of image rows.",
 "GetSubImage8() Error: Unable to allocate temporaray storage.",
 "GetSubImage8() Error: Invalid data type specified.",
 "GetSubImage8() Error: NULL pointer in argument list.",
 "GetSubImage8() Error: Max and min values must be given for float point data.",
} ;
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: November 6, 1995;  May 14, 1997; January 16, 2001;
 */
int GetSubImage8 (char *filename,int datatype,int if_image,
		double min_data_val,double max_data_val,long file_header_bytes,
		long trace_header_bytes,long first_trace,long last_trace,
		long skip_traces,long first_samp,long total_samps,
		long num_cols,long num_rows,unsigned char **image)
{
	void *trace = NULL, *vptr = NULL;
	int error;
	long disk_trace_size,col,row,samp_size,samp,ltemp,offset,traces_read;
	float ftemp;
	double dtemp;
	FILE *infile = NULL;
	extern int Debug;

	#if defined(_INTELC32_)
	  char *buffer = NULL;
	  size_t vbufsize = 64512;  /* 63 KB */
	#endif

	if (Debug)
	{   printf("GetSubImage8():  filename = %s datatype = %ld  first_trace = %ld\n\r\
first_samp = %ld  total_samps = %ld  num_cols = %ld  num_rows = %ld\n\r\
file_header_bytes = %ld  trace_header_bytes = %ld  skip_traces = %ld\n\r\
min_data_val = %g  max_data_val = %g  if_image = %d  last_trace = %ld\n",
			filename,datatype,first_trace,first_samp,total_samps,num_cols,
			num_rows,file_header_bytes,trace_header_bytes,skip_traces,
			min_data_val,max_data_val,if_image,last_trace);
		getch();
	}

	/* Verify parameters */
	if (filename == NULL || image == NULL)  return 7;
	if ((first_samp + num_rows -1) > total_samps) return 4;
	switch (datatype)
	{   case  1:  samp_size = sizeof(char);   break;
		case -1:  samp_size = sizeof(unsigned char);   break;
		case  2:  samp_size = sizeof(short);  break;
		case -2:  samp_size = sizeof(unsigned short);  break;
		case -5:  samp_size = sizeof(unsigned short);  break;
		case  3:  samp_size = sizeof(long);   break;
		case -3:  samp_size = sizeof(unsigned long);   break;
		case -6:
			samp_size = sizeof(unsigned long);
			break;
		case  4:
			if (max_data_val <= min_data_val) return 8;
			samp_size = sizeof(float);
			break;
		case  8:
			if (max_data_val <= min_data_val) return 8;
			samp_size = sizeof(double);
			break;
		default:
			return 6;
	}

	if (datatype == 4 || datatype == 8)
	{   if (max_data_val <= min_data_val) return 8;
	}
	if (skip_traces < 0) skip_traces = 0;

	/* Allocate storage for one trace from disk */
	if (if_image)  total_samps = num_cols;
	disk_trace_size = trace_header_bytes + (total_samps * samp_size);
	if (Debug)
	{	printf("samp_size = %ld  disk_trace_size = %ld  total_samps = %ld\n",
				samp_size,disk_trace_size,total_samps);
		getch();
	}
	trace = (void *)malloc((size_t)disk_trace_size);
	if (trace == NULL) return 5;

	/* Open input file */
	if ((infile = fopen(filename,"rb")) == NULL)
	{   free(trace);
		return 1;
	}

	/* Speed up disk access by using large (conventional memory) buffer */
	#if defined(_INTELC32_)
	  realmalloc(buffer,vbufsize);
	  if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
	  else         setvbuf(infile,NULL,_IOFBF,vbufsize);
	#endif

	/* Get data from disk */
	offset = file_header_bytes + (first_trace * disk_trace_size);
	fseek(infile,offset,SEEK_SET);       /* goto first requested trace */
	traces_read = 0;
	if (if_image)   /* screen image */
	{   if (Debug) puts("image data selected");
		for (row=0; row<num_rows; row++)
		{   /* reset error and get trace */
			error = 0;
			if (fread(trace,(size_t)disk_trace_size,(size_t)1,infile) < 1)
			{   error = 2;
				if (feof(infile)) error = 3;
				if (Debug)
				{   printf("fread error - get trace loop. row=%ld error=%d traces_read=%ld\n",
							row,error,traces_read);
					getch();
				}
				break;  /* out of for loop reading traces from file */
			}
			traces_read++;

			/* point past trace header */
			vptr = (void *)((char *)trace + trace_header_bytes);

			switch (datatype)
			{   case -1:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
						image[row][col] = ((unsigned char *)vptr)[samp];
					break;
				case 1:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
						image[row][col] = ((int)((char *)vptr)[samp]) + 128;
					break;
				case -2:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
						image[row][col] = (((unsigned short *)vptr)[samp]) >> 8;
					break;
				case -5:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
						image[row][col] = (((unsigned short *)vptr)[samp]) >> 4;
					break;
				case 2:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
					{   ltemp = ((short *)vptr)[samp];
						ltemp += 32768L;
						ltemp /= 256;
						image[row][col] = ltemp;
					}
					break;
				case -3:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
						image[row][col] = (((unsigned long *)vptr)[samp]) >> 24;
					break;
				case -6:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
						image[row][col] = (((unsigned long *)vptr)[samp]) >> 16;
					break;
				case 3:
					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
					{   dtemp = ((long *)vptr)[samp];
						dtemp += 2147483648L;
						dtemp /= 16843009L;
						image[row][col] = dtemp;
					}
					break;
				case 4:
				{   float range = max_data_val - min_data_val;

					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
					{   ftemp = ((float *)vptr)[samp];
						if (ftemp > max_data_val) ftemp = max_data_val;
						if (ftemp < min_data_val) ftemp = min_data_val;
						ftemp -= min_data_val;
						ftemp /= range;
						ftemp *= 255.0;
						ftemp = _LIMIT(0.0,ftemp,255.0);
						image[row][col] = ftemp;
					}
					break;
				}
				case 8:
				{   double range = max_data_val - min_data_val;

					samp = first_samp;
					for (col=0; col<num_cols; col++,samp++)
					{   dtemp = ((double *)vptr)[samp];
						if (dtemp > max_data_val) dtemp = max_data_val;
						if (dtemp < min_data_val) dtemp = min_data_val;
                        dtemp -= min_data_val;
						dtemp /= range;
						dtemp *= 255;
						dtemp = _LIMIT(0.0,dtemp,255.0);
						image[row][col] = dtemp;
					}
					break;
				}
			}   /* end of switch block */
			/* skip traces if requested */
			for (ltemp = 0; ltemp<skip_traces; ltemp++)
			{   if (traces_read >= last_trace)
				{   break;  /* out of for loop getting skipped traces */
				} else
				{   if (fread(trace,(size_t)disk_trace_size,(size_t)1,infile) < 1)
					{   error = 2;
						if (Debug)
						{   printf("fread error in skip loop.  row=%ld ltemp = %ld\n",row,ltemp);
							getch();
						}
						if (feof(infile))  /* back up to get last trace */
						{    /* should never get here ! */
							 fseek(infile,last_trace * disk_trace_size,SEEK_SET);
							 if (Debug)
							 {   printf("EOF found in skip loop;  ltemp = %ld  ftell = %ld\n",ltemp,ftell(infile));
								 getch();
							 }
						}
						break;  /* out of for loop getting skipped traces */
					}
					traces_read++;
				}
			}
		}
	} else   /* normal radar data */
	{   for (col=0; col<num_cols; col++)
		{   /* reset error and get trace */
			error = 0;
			if (fread(trace,(size_t)disk_trace_size,(size_t)1,infile) < 1)
			{   error = 2;
				if (feof(infile)) error = 3;
				if (Debug)
				{   printf("fread error - get trace loop. col=%ld error=%d traces_read=%ld\n",
							col,error,traces_read);
					getch();
				}
				break;  /* out of for loop reading traces from file */
			}
			traces_read++;

			/* point past trace header */
			vptr = (void *)((char *)trace + trace_header_bytes);

			/* get data and convert */
			switch (datatype)
			{   case -1:
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
						image[row][col] = ((unsigned char *)vptr)[samp];
					break;
				case 1:
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
						image[row][col] = ((int)((char *)vptr)[samp]) + 128;
					break;
				case -2:
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
						image[row][col] = (((unsigned short *)vptr)[samp]) >> 8;
					break;
				case -5:
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
						image[row][col] = (((unsigned short *)vptr)[samp]) >> 4;
					break;
				case 2:
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
					{   ltemp = ((short *)vptr)[samp];
						ltemp += 32768L;
						ltemp /= 256;
						image[row][col] = ltemp;
					}
					break;
				case -3:
				{	double range;

					if (max_data_val <= min_data_val)
					{   max_data_val = 4294967295;
						min_data_val = 0.0;
					}
					range = max_data_val - min_data_val;
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
					{   dtemp = ((unsigned long *)vptr)[samp];
						if (dtemp > max_data_val) dtemp = max_data_val;
						if (dtemp < min_data_val) dtemp = min_data_val;
						dtemp -= min_data_val;
						dtemp /= range;
						dtemp *= 255.0;
						dtemp = _LIMIT(0.0,dtemp,255.0);
						image[row][col] = dtemp;
					}
					break;
				}

/*
					samp = first_samp; /* -3 */
					for (row=0; row<num_rows; row++,samp++)
						image[row][col] = (((unsigned long *)vptr)[samp]) >> 24;
					break;

					samp = first_samp; /* -6 */
					for (row=0; row<num_rows; row++,samp++)
						image[row][col] = (((unsigned long *)vptr)[samp]) >> 16;
*/
				case -6:
					if (max_data_val <= min_data_val)
					{   max_data_val = 16777215;
						min_data_val = 0.0;
					}
					range = max_data_val - min_data_val;
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
					{   dtemp = ((unsigned long *)vptr)[samp];
						if (dtemp > max_data_val) dtemp = max_data_val;
						if (dtemp < min_data_val) dtemp = min_data_val;
						dtemp -= min_data_val;
						dtemp /= range;
						dtemp *= 255.0;
						dtemp = _LIMIT(0.0,dtemp,255.0);
						image[row][col] = dtemp;
					}
					break;
				case 3:
				{	double range;

					if (max_data_val <= min_data_val)
					{   max_data_val =  2147483647.0;
						min_data_val = -2147483648.0;
					}
					range = max_data_val - min_data_val;
					samp = first_samp;
					for (row=0; row<num_rows; row++,samp++)
					{   dtemp = ((long *)vptr)[samp];
						if (dtemp > max_data_val) dtemp = max_data_val;
						if (dtemp < min_data_val) dtemp = min_data_val;
						/*
						dtemp += 2147483648L;
						dtemp /= 16843009L;
						*/
						dtemp -= min_data_val;
						dtemp /= range;
						dtemp *= 255.0;
						dtemp = _LIMIT(0.0,dtemp,255.0);
						image[row][col] = dtemp;
					}
					break;
				}
				case 4:
				{   float range = max_data_val - min_data_val;

					samp = first_samp;
					for (row=0; row<num_cols; row++,samp++)
					{   ftemp = ((float *)vptr)[samp];
						if (ftemp > max_data_val) ftemp = max_data_val;
						if (ftemp < min_data_val) ftemp = min_data_val;
						ftemp -= min_data_val;
						ftemp /= range;
						ftemp *= 255.0;
						ftemp = _LIMIT(0.0,ftemp,255.0);
						image[row][col] = ftemp;
					}
					break;
				}
				case 8:
				{   double range = max_data_val - min_data_val;

					samp = first_samp;
					for (row=0; row<num_cols; row++,samp++)
					{   dtemp = ((double *)vptr)[samp];
						if (dtemp > max_data_val) dtemp = max_data_val;
						if (dtemp < min_data_val) dtemp = min_data_val;
						dtemp -= min_data_val;
						dtemp /= range;
						dtemp *= 255.0;
						dtemp = _LIMIT(0.0,dtemp,255.0);
						image[row][col] = dtemp;
					}
					break;
				}
			}   /* end of switch block */
			/* skip traces if requested */
			for (ltemp=0; ltemp<skip_traces; ltemp++)
			{   if (traces_read >= last_trace)
				{   if (Debug)
					{   printf("traces_read = %ld  last_trace = %ld\n",traces_read,last_trace);
						getch();
					}
					break;  /* out of for loop getting skipped traces */
				} else
				{   if (fread(trace,(size_t)disk_trace_size,(size_t)1,infile) < 1)
					{   error = 2;
						if (Debug)
						{   printf("fread error - skip loop. col=%ld ltemp=%ld traces_read=%ld\n",col,ltemp,traces_read);
							getch();
						}
						if (feof(infile))  /* back up to get last trace */
						{    /* should never get here ! */
							 fseek(infile,last_trace * disk_trace_size,SEEK_SET);
							 if (Debug)
							 {   printf("EOF found - skip loop; ltemp=%ld ftell=%ld traces_read=%ld\n",ltemp,ftell(infile),traces_read);
								 getch();
							 }
						}
						break;  /* out of for loop getting skipped traces */
					}
					traces_read++;
				}
			}
		}   /* end of for loop reading traces from file */
	}   /* end of if (image) else blocks */

	/* Close file, release buffers, and return */
	fclose(infile);
	free(trace);
	#if defined(_INTELC32_)
	  realfree(buffer);
	#endif
	return error;
}
/*************************** InitGprInfoStruct() ****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct GprInfoStruct *InfoPtr - address of structure
 *
 * Requires: <string.h>, "gpr_conv.h".
 * Calls: strcpy.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: January 28, 1997; September 2, 1999;
 */
void InitGprInfoStruct (struct GprInfoStruct *InfoPtr)
{
	size_t i;

	InfoPtr->cmd_filename[0]     = 0;      /* invalid value */
	InfoPtr->data_filename[0]    = 0;      /* invalid value */
	InfoPtr->info_filename[0]    = 0;      /* invalid value */
	InfoPtr->mrk_filename[0]     = 0;      /* initializing value */
	InfoPtr->xyz_filename[0]     = 0;      /* initializing value */
	InfoPtr->input_format        = 0;      /* invalid value */
	InfoPtr->dzt_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->dt1_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->hd_outfilename[0]   = 0;      /* initializing value */
	InfoPtr->sgy_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->sg2_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->rd3_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->rad_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->dsf_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->grd_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->inf_outfilename[0]  = 0;      /* initializing value */
	InfoPtr->input_datatype      = 0;      /* invalid value */
	InfoPtr->output_datatype     = 0;      /* invalid value */
	InfoPtr->total_traces        = 0;      /* invalid value */
	InfoPtr->num_input_channels  = 1;      /* default value */
	InfoPtr->num_output_channels = 1;      /* default value */
	InfoPtr->selected_channel    = 0;      /* default value */
	InfoPtr->file_header_bytes   = 0;      /* invalid value */
	InfoPtr->trace_header_bytes  = 0;      /* invalid value */
	InfoPtr->in_samps_per_trace  = 0;      /* invalid value */
	InfoPtr->out_samps_per_trace = 0;      /* invalid value */
	InfoPtr->in_bits_per_samp    = 0;      /* invalid value */
	InfoPtr->out_bits_per_samp   = 0;      /* invalid value */
	InfoPtr->in_time_window      = 0.0;    /* invalid value */
	InfoPtr->out_time_window     = 0.0;    /* invalid value */
    InfoPtr->timezero_sample     = 0;      /* default value */
    InfoPtr->traces_per_sec      = 0.0;    /* default value */
    InfoPtr->number_of_stacks    = 1;      /* default value */
    InfoPtr->nominal_frequency   = 0;      /* default value */
    InfoPtr->pulser_voltage      = 0.0;    /* default value */
    InfoPtr->antenna_separation  = 0.0;    /* default value */
    for (i=0; i<sizeof(InfoPtr->antenna_name); i++)
        InfoPtr->antenna_name[i] = ' ';    /* default value */
    InfoPtr->traces_per_meter    = 0.0;    /* default value */
    InfoPtr->meters_per_mark     = 0.0;    /* default value */
    InfoPtr->starting_position   = 0.0;    /* default value */
    InfoPtr->final_position      = 0.0;    /* default value */
    InfoPtr->position_step_size  = 0.0;    /* default value */
    for (i=0; i<sizeof(InfoPtr->position_units); i++)
		InfoPtr->position_units[i] = ' ';  /* initializing value */
    strcpy(InfoPtr->position_units,"not specified"); /* default value */
    InfoPtr->num_marks           = 0;      /* default value */
    InfoPtr->marks               = NULL;   /* initializing value */
    InfoPtr->XYZ                 = NULL;   /* initializing value */
    InfoPtr->time_date_created.sec2   = 0; /* default value */
    InfoPtr->time_date_created.min    = 0; /* default value */
    InfoPtr->time_date_created.hour   = 0; /* default value */
    InfoPtr->time_date_created.day    = 0; /* invalid value */
    InfoPtr->time_date_created.month  = 0; /* invalid value */
    InfoPtr->time_date_created.year   = 0; /* default value */
    InfoPtr->time_date_modified.sec2  = 0; /* default value */
    InfoPtr->time_date_modified.min   = 0; /* default value */
    InfoPtr->time_date_modified.hour  = 0; /* default value */
    InfoPtr->time_date_modified.day   = 0; /* invalid value */
    InfoPtr->time_date_modified.month = 0; /* invalid value */
    InfoPtr->time_date_modified.year  = 0; /* default value */
    InfoPtr->num_gain_pts        = 0;      /* default value */
    InfoPtr->gain_pts            = NULL;   /* initializing value */
    InfoPtr->proc_hist_bytes     = 0;      /* default value */
    InfoPtr->proc_hist           = NULL;   /* initializing value */
    InfoPtr->text_bytes          = 0;      /* default value */
    InfoPtr->text                = NULL;   /* initializing value */
    for (i=0; i<sizeof(InfoPtr->survey_mode); i++)
        InfoPtr->survey_mode[i] = ' ';     /* initializing value */
    strcpy(InfoPtr->position_units,"not specified"); /* default value */
    for (i=0; i<sizeof(InfoPtr->job_number); i++)
        InfoPtr->job_number[i] = ' ';      /* default value */
    for (i=0; i<sizeof(InfoPtr->title1); i++)
        InfoPtr->title1[i] = ' ';          /* default value */
    for (i=0; i<sizeof(InfoPtr->title2); i++)
        InfoPtr->title2[i] = ' ';          /* default value */
    InfoPtr->nominal_RDP         = 0.0;    /* default value */
    InfoPtr->position            = 0.0;    /* default value */
    InfoPtr->elevation           = 0.0;    /* default value */
    InfoPtr->timezero_adjust     = 0.0;    /* default value */
    InfoPtr->zero_flag           = 0.0;    /* default value */
    InfoPtr->time_of_day         = 0.0;    /* default value */
	InfoPtr->comment_flag        = 0.0;    /* default value */
    for (i=0; i<sizeof(InfoPtr->comment); i++)
        InfoPtr->comment[i] = 0;           /* default value */
    InfoPtr->created             = FALSE;
}
/***************************** GetSubGrid16u() ******************************/

/* NOTE:  HERE: This func NOT TESTED YET !!!!!!!!!!!!!!!!!!!!!! */


/* Open a GPR file, copy a subset of the data into the grid, and close the
 * file.  Data type larger than characters are scaled down to unsigned 16-bits.
 * Floating point types are converted using user-defined data extremes.
 *
 * Versions after May 14, 1997 allow scaling of data types larger than 16 bits,
 *  not just floating points.
 *
 * Parameters:
 *  char *filename   - pointer to string containing GPR filename
 *  int  datatype    - type of input data elements, grid, and output
                    if ==  1, 1-byte chars;          == -1, unsigned chars
                       ==  2, 2-byte ints or shorts; == -2, unsigned shorts
                       ==  3, 4-byte ints or longs;  == -3, unsigned longs
                       ==  4, 4-byte floats
                       == -5, unsigned shorts but only first 12-bits used
                       == -6, unsigned longs but only first 24-bits used
                       ==  8, 8-byte doubles
 *  double min_data_val - required only if datatype equals 4 or 8
 *  double max_data_val - required only if datatype equals 4 or 8
       Note: above 2 vals not used if max <= min
 *  long file_header_bytes  - number of bytes in file header
 *  long trace_header_bytes - number of bytes in each trace header
 *  long first_trace - first trace to get from file, indexed from 0
 *  long last_trace  - last trace to get from file, indexed from 0
 *  long skip_traces - number of traces to skip for every one read in
       Note: no traces skip if above value <= 0
 *  long first_samp  - first sample to store in grid, indexed from 0
 *  long total_samps - number of samples in a trace on the disk
 *  long num_cols    - number of columns (traces) in grid[][]
 *  long num_rows    - number of rows (samples per scan) in grid[][]
 *  unsigned short **grid - pointer to 2D array [num_rows][num_cols]
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, <stdlib.h>, "gpr_io.h"
 * Calls: fopen, fclose, fread, setvbuf, fseek, feof.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *

const char *GetSubGrid16uMsg[] =
{"GetSubGrid16u: No errors.",
 "GetSubGrid16u() Error: Unable to open input data file.",
 "GetSubGrid16u() Error: Short count from fread().",
 "GetSubGrid16u() Error: EOF encountered -- gird cols exceed number of traces.",
 "GetSubGrid16u() Error: Last trace sample exceeds number of grid rows.",
 "GetSubGrid16u() Error: Unable to allocate temporaray storage.",
 "GetSubGrid16u() Error: Invalid data type specified.",
 "GetSubGrid16u() Error: NULL pointer in argument list.",
 "GetSubGrid16u() Error: Max and min values must be given for float point data.",
} ;
 *
 * Author: Jeff Lucius   USGS  lucius@usgs.gov
 * Date: January 16, 2001;
 */
int GetSubGrid16u (char *filename,int datatype,
		double min_data_val,double max_data_val,long file_header_bytes,
		long trace_header_bytes,long first_trace,long last_trace,
		long skip_traces,long first_samp,long total_samps,
		long num_cols,long num_rows,unsigned short **grid)
{
	void *trace = NULL, *vptr = NULL;
	int error;
	long disk_trace_size,col,row,samp_size,samp,ltemp,offset,traces_read;
	float ftemp;
	double dtemp;
	FILE *infile = NULL;
	extern int Debug;

	#if defined(_INTELC32_)
	  char *buffer = NULL;
	  size_t vbufsize = 64512;  /* 63 KB */
	#endif

	if (Debug)
	{   printf("GetSubGrid16u():  filename = %s datatype = %ld  first_trace = %ld\n\r\
first_samp = %ld  total_samps = %ld  num_cols = %ld  num_rows = %ld\n\r\
file_header_bytes = %ld  trace_header_bytes = %ld  skip_traces = %ld\n\r\
min_data_val = %g  max_data_val = %g  last_trace = %ld\n",
			filename,datatype,first_trace,first_samp,total_samps,num_cols,
			num_rows,file_header_bytes,trace_header_bytes,skip_traces,
			min_data_val,max_data_val,last_trace);
		getch();
	}

	/* Verify parameters */
	if (filename == NULL || image == NULL)  return 7;
	if ((first_samp + num_rows -1) > total_samps) return 4;
	switch (datatype)
	{   case  1:  samp_size = sizeof(char);   break;
		case -1:  samp_size = sizeof(unsigned char);   break;
		case  2:  samp_size = sizeof(short);  break;
		case -2:  samp_size = sizeof(unsigned short);  break;
		case -5:  samp_size = sizeof(unsigned short);  break;
		case  3:  samp_size = sizeof(long);   break;
		case -3:  samp_size = sizeof(unsigned long);   break;
		case -6:
			samp_size = sizeof(unsigned long);
			break;
		case  4:
			if (max_data_val <= min_data_val) return 8;
			samp_size = sizeof(float);
			break;
		case  8:
			if (max_data_val <= min_data_val) return 8;
			samp_size = sizeof(double);
			break;
		default:
			return 6;
	}

	if (datatype == 4 || datatype == 8)
	{   if (max_data_val <= min_data_val) return 8;
	}
	if (skip_traces < 0) skip_traces = 0;

	/* Allocate storage for one trace from disk */
	disk_trace_size = trace_header_bytes + (total_samps * samp_size);
	if (Debug)
	{	printf("samp_size = %ld  disk_trace_size = %ld  total_samps = %ld\n",
				samp_size,disk_trace_size,total_samps);
		getch();
	}
	trace = (void *)malloc((size_t)disk_trace_size);
	if (trace == NULL) return 5;

	/* Open input file */
	if ((infile = fopen(filename,"rb")) == NULL)
	{   free(trace);
		return 1;
	}

	/* Speed up disk access by using large (conventional memory) buffer */
	#if defined(_INTELC32_)
	  realmalloc(buffer,vbufsize);
	  if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
	  else         setvbuf(infile,NULL,_IOFBF,vbufsize);
	#endif

	/* Get data from disk */
	offset = file_header_bytes + (first_trace * disk_trace_size);
	fseek(infile,offset,SEEK_SET);       /* goto first requested trace */
	traces_read = 0;
	for (col=0; col<num_cols; col++)
	{   /* reset error and get trace */
		error = 0;
		if (fread(trace,(size_t)disk_trace_size,(size_t)1,infile) < 1)
		{   error = 2;
			if (feof(infile)) error = 3;
			if (Debug)
			{   printf("fread error - get trace loop. col=%ld error=%d traces_read=%ld\n",
						col,error,traces_read);
				getch();
			}
			break;  /* out of for loop reading traces from file */
		}
		traces_read++;

		/* point past trace header */
		vptr = (void *)((char *)trace + trace_header_bytes);

		/* get data and convert */
		switch (datatype)
		{   case -1:
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
					grid[col][row] = ((unsigned char *)vptr)[samp] << 8;
				break;
			case 1:
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
					grid[col][row] = (((int)((char *)vptr)[samp]) + 128) << 8;
				break;
			case -2:
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
					grid[col][row] = ((unsigned short *)vptr)[samp];
				break;
			case -5:
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
					grid[col][row] = (((unsigned short *)vptr)[samp]) << 4;
				break;
			case 2:
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
				{   ltemp = ((short *)vptr)[samp];
					ltemp += 32768L;
					ltemp = _LIMIT(0L,ltemp,65535L);
					grid[col][row] = ltemp;
				}
				break;
			case -3:
			{	double range;

				if (max_data_val <= min_data_val)
				{   max_data_val = 4294967295;
					min_data_val = 0.0;
				}
				range = max_data_val - min_data_val;
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
				{   dtemp = ((unsigned long *)vptr)[samp];
					if (dtemp > max_data_val) dtemp = max_data_val;
					if (dtemp < min_data_val) dtemp = min_data_val;
					dtemp -= min_data_val;
					dtemp /= range;
					dtemp *= 65535.0;
					dtemp = _LIMIT(0.0,dtemp,65535.0);
					grid[col][row] = dtemp;
				}
				break;
			}
			case -6:
			{	double range;

				if (max_data_val <= min_data_val)
				{   max_data_val = 16777215;
					min_data_val = 0.0;
				}
				range = max_data_val - min_data_val;
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
				{   dtemp = ((unsigned long *)vptr)[samp];
					if (dtemp > max_data_val) dtemp = max_data_val;
					if (dtemp < min_data_val) dtemp = min_data_val;
					dtemp -= min_data_val;
					dtemp /= range;
					dtemp *= 65535.0;
					dtemp = _LIMIT(0.0,dtemp,65535.0);
					grid[col][row] = dtemp;
				}
				break;
   			}
            case 3:
			{	double range;

				if (max_data_val <= min_data_val)
				{   max_data_val =  2147483647.0;
					min_data_val = -2147483648.0;
				}
				range = max_data_val - min_data_val;
				samp = first_samp;
				for (row=0; row<num_rows; row++,samp++)
				{   dtemp = ((long *)vptr)[samp];
					if (dtemp > max_data_val) dtemp = max_data_val;
					if (dtemp < min_data_val) dtemp = min_data_val;
					dtemp -= min_data_val;
					dtemp /= range;
					dtemp *= 65535.0;
					dtemp = _LIMIT(0.0,dtemp,65535.0);
					grid[col][row] = dtemp;
				}
				break;
			}
			case 4:
			{   float range = max_data_val - min_data_val;

				samp = first_samp;
				for (row=0; row<num_cols; row++,samp++)
				{   ftemp = ((float *)vptr)[samp];
					if (ftemp > max_data_val) ftemp = max_data_val;
					if (ftemp < min_data_val) ftemp = min_data_val;
					ftemp -= min_data_val;
					ftemp /= range;
    				ftemp *= 65535.0;
	    			ftemp = _LIMIT(0.0,ftemp,65535.0);
					grid[col][row] = ftemp;
				}
				break;
			}
			case 8:
			{   double range = max_data_val - min_data_val;

				samp = first_samp;
				for (row=0; row<num_cols; row++,samp++)
				{   dtemp = ((double *)vptr)[samp];
					if (dtemp > max_data_val) dtemp = max_data_val;
					if (dtemp < min_data_val) dtemp = min_data_val;
					dtemp -= min_data_val;
					dtemp /= range;
					dtemp *= 65535.0;
					dtemp = _LIMIT(0.0,dtemp,65535.0);
					grid[col][row] = dtemp;
				}
				break;
			}
		}   /* end of switch block */

		/* skip traces if requested */
		for (ltemp=0; ltemp<skip_traces; ltemp++)
		{   if (traces_read >= last_trace)
			{   if (Debug)
				{   printf("traces_read = %ld  last_trace = %ld\n",traces_read,last_trace);
					getch();
				}
				break;  /* out of for loop getting skipped traces */
			} else
			{   if (fread(trace,(size_t)disk_trace_size,(size_t)1,infile) < 1)
				{   error = 2;
					if (Debug)
					{   printf("fread error - skip loop. col=%ld ltemp=%ld traces_read=%ld\n",col,ltemp,traces_read);
						getch();
					}
					if (feof(infile))  /* back up to get last trace */
					{    /* should never get here ! */
						 fseek(infile,last_trace * disk_trace_size,SEEK_SET);
						 if (Debug)
						 {   printf("EOF found - skip loop; ltemp=%ld ftell=%ld traces_read=%ld\n",ltemp,ftell(infile),traces_read);
							 getch();
						 }
					}
					break;  /* out of for loop getting skipped traces */
				}
				traces_read++;
			}
		}   /* end of loop for getting skipped traces */
    }   /* end of for loop reading traces from file */

	/* Close file, release buffers, and return */
	fclose(infile);
	free(trace);
	#if defined(_INTELC32_)
	  realfree(buffer);
	#endif
	return error;
}

