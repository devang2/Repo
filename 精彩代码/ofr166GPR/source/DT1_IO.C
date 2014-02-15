/* DT1_IO.C - source code for functions in GPR_IO.LIB
 *
 * Jeff Lucius
 * Geophysicist
 * U.S. Geological Survey
 * Geologic Division, Branch of Geophysics
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@musette.cr.usgs.gov
 *
 * To compile for use in GPR_IO.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c DT1_IO.C
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /c compiles only, no linking.
 *
 * Functions in this module:
 *   GetSsHdFile
 *   GetSsTrace
 *   InitDt1Parameters
 *   PrintSsHdInfo
 *   PrintSsTraceHdr
 *   SaveSsHdFile
 *   SetDt1TraceHeader
 *
 */

/* Include header files */
#include "gpr_io.h"

/* Declare globals */
/* These are the descriptors to the left of the equal sign, "=", in the ASCII
   header file. */
const char *SS_HD_CMDS[] =
{   "NUMBER OF TRACES","NUMBER OF PTS/TRC","TIMEZERO AT POINT","TOTAL TIME WINDOW","STARTING POSITION",
    "FINAL POSITION","STEP SIZE USED","POSITION UNITS","NOMINAL FREQUENCY","ANTENNA SEPARATION",
	"PULSER VOLTAGE (V)","NUMBER OF STACKS","SURVEY MODE",NULL,
} ;
/* message strings for functions */
const char *GetSsHdFileMsg[] =
{   "GetSsHdFile(): No errors.",
    "GetSsHdFile() ERROR: NULL pointer in parameter list.",
    "GetSsHdFile() ERROR: Unable to open input file.",
    "GetSsHdFile() ERROR: Unexpected EOF.",
    "GetSsHdFile() ERROR: Unable to allocate storage for proc_hist.",
    "GetSsHdFile() ERROR: This file not recognized as a S&S HD file.",
} ;
const char *GetSsTraceMsg[] =
{   "GetSsTrace(): No errors.",
    "GetSsTrace() ERROR: NULL pointer in parameter list.",
    "GetSsTrace() ERROR: Short count from fread() (header).",
    "GetSsTrace() ERROR: Short count from fread() (data).",
} ;
const char *SaveSsHdFileMsg[] =
{   "SaveSsHdFile(): No errors.",
	"SaveSsHdFile() ERROR: NULL pointer passed for output HD filename.",
	"SaveSsHdFile() ERROR: Unable to open output HD file.",
	"SaveSsHdFile() ERROR: Short count from fwrite().",
} ;


/* Local function prototypes */
static char *TrimStr(char *p);

/*************************** InitDt1Parameters() ****************************/
/* Initialize all Sensors & Software HD info parameters to start-up values.
 *
 * Parameter list:
 *  struct SsHdrInfoStruct *InfoPtr - address of structure
 *
 * Requires: "gpr_io.h".
 * Calls: none.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: April 12, 1995; August 13, 2001;
 */
void InitDt1Parameters (struct SsHdrInfoStruct *InfoPtr)
{
    memset(InfoPtr->job_number,' ',sizeof(InfoPtr->job_number));
    memset(InfoPtr->title1,' ',sizeof(InfoPtr->title1));
    memset(InfoPtr->title2,' ',sizeof(InfoPtr->title2));
    InfoPtr->title1[sizeof(InfoPtr->title1)-1] = 0;
    InfoPtr->title2[sizeof(InfoPtr->title1)-2] = 0;
    InfoPtr->num_traces        = 0;
    InfoPtr->num_samples       = 0;
    InfoPtr->time_zero_sample  = 0;
    InfoPtr->num_stacks        = 0;
    InfoPtr->total_time_ns     = 0;
    InfoPtr->start_pos         = 0.0;
    InfoPtr->final_pos         = 0.0;
    InfoPtr->step_size         = 0.0;
    InfoPtr->ant_freq          = 0.0;
    InfoPtr->ant_sep           = 0.0;
    InfoPtr->pulser_voltage    = 0.0;
    memset(InfoPtr->pos_units,' ',sizeof(InfoPtr->pos_units));
    InfoPtr->pos_units[sizeof(InfoPtr->pos_units)-1] = 0;
    memset(InfoPtr->survey_mode,' ',sizeof(InfoPtr->survey_mode));
    InfoPtr->survey_mode[sizeof(InfoPtr->survey_mode)-1] = 0;
    InfoPtr->year              = 0;
    InfoPtr->month             = 0;
    InfoPtr->day               = 0;
    InfoPtr->proc_hist         = NULL;
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

    /* strip of blanks etc. by inserting terminating 0 */
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
/******************************* GetSsHdFile() ******************************/
/* Read the Sensors & Software header file and store data.
 * Optionally print out info.
 *
 * Parameters:
 *  int print_it      - prints header info if TRUE
 *  struct SsHdrInfoStruct *HdInfo - address of header info structure
 *  char *hd_filename - pointer to filename string
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 * NOTE: This function allocates storage for the "proc_hist" field in the
 *       HdInfo structure.  It does not deallocate it.
 *
 * Requires: <io.h>, <errno.h>, <stdio.h>, <stdlib.h>, <string.h>, <conio.h>,
 *            "gpr_io.h".
 * Calls: fopen, fclose, rewind, fgets, access, printf, puts, strcmp, strchr,
 *        strncat, TrimStr, ftell, fseek, strupr, strncpy, atoi, atof, calloc,
 *        getch.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *SS_HD_CMDS[] =
{   "NUMBER OF TRACES","NUMBER OF PTS/TRC","TIMEZERO AT POINT","TOTAL TIME WINDOW","STARTING POSITION",
    "FINAL POSITION","STEP SIZE USED","POSITION UNITS","NOMINAL FREQUENCY","ANTENNA SEPARATION",
    "PULSER VOLTAGE (V)","NUMBER OF STACKS","SURVEY MODE",NULL,
} ;
const char *GetSsHdFileMsg[] =
{   "GetSsHdFile(): No errors.",
    "GetSsHdFile() ERROR: NULL pointer in parameter list.",
    "GetSsHdFile() ERROR: Unable to open input file.",
    "GetSsHdFile() ERROR: Unexpected EOF.",
    "GetSsHdFile() ERROR: Unable to allocate storage for proc_hist.",
    "GetSsHdFile() ERROR: This file not recognized as a S&S HD file.",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: April 13, 1995
 */
int GetSsHdFile (int print_it,struct SsHdrInfoStruct *HdInfo,
                char *hd_filename)
{
    char  str[80], *cp, *cp2, cmdstr[30];
    int   i, num, itemp, found, field_found, proc_start_line, line_count;
    int date_found;
    double dtemp;
    FILE *IO_file;
    extern int errno;

    /* Reality check */
    if (HdInfo == NULL || hd_filename == NULL) return 1;

    /* Open header file */
    if ((IO_file = fopen(hd_filename,"rt")) == NULL)
    {   printf("\nERROR: Unable to open input file %s\n",hd_filename);
        if (access(hd_filename,4))
        {   if      (errno == ENOENT) puts("File or path name not found.");
            else if (errno == EACCES) puts("Read Access denied.");
            else                      puts("Unknown access error.");
        }
        return 2;
    }

    /* Initialize string arrays */
    HdInfo->job_number[0]  = 0;
    HdInfo->title1[0]      = 0;
    HdInfo->title2[0]      = 0;
    HdInfo->pos_units[0]   = 0;
    HdInfo->survey_mode[0] = 0;

    /* Get date, job number, and titles */
    date_found = -1;
    for (i=0; i<10; i++)
    {   fgets(str,80,IO_file);
        if (str == NULL)
        {   fclose(IO_file);
            return 3;
        }
        if (strchr(str,'/'))
        {   date_found = i;
            break;  /* out of for loop */
        }
    }
    rewind(IO_file);
    if (date_found >= 0)
    {   cp = str;
        cp2 = strchr(cp,'/');
        if (cp2)
        {   *cp2 = 0;
            cp = cp2 - 2;
            if (print_it) printf("cp=%s\n",cp);
            HdInfo->day = atoi(cp);
            cp = cp2+1;
            cp2 = strchr(cp,'/');
            if (cp2)
            {   *cp2 = 0;
                if (print_it) printf("cp=%s\n",cp);
                HdInfo->month = atoi(cp);
                cp = cp2+1;
                if (print_it) printf("cp=%s\n",cp);
                HdInfo->year = atoi(cp);
            }
        }
        if (print_it) printf("date found %d: year=%d month=%d day=%d\n",
                        date_found,HdInfo->year,HdInfo->month,HdInfo->day);
        i = 0;
        if (i < date_found)
        {   fgets(str,80,IO_file);
            if (str == NULL)
            {   fclose(IO_file);
                return 3;
            }
            TrimStr(str);
            if (strlen(str))
            {   strncpy(HdInfo->job_number,str,9);
                HdInfo->job_number[9] = 0;
            }
        }
        i++;
        if (i < date_found)
        {   fgets(str,80,IO_file);
            if (str == NULL)
            {   fclose(IO_file);
                return 3;
            }
            TrimStr(str);
            if (strlen(str))
            {   strncpy(HdInfo->title1,str,69);
                HdInfo->title1[69] = 0;
            }
        }
        i++;
        if (i < date_found)
        {   fgets(str,80,IO_file);
            if (str == NULL)
            {   fclose(IO_file);
                return 3;
            }
            TrimStr(str);
            if (strlen(str))
            {   strncpy(HdInfo->title2,str,69);
                HdInfo->title2[69] = 0;
            }
        }
    } else
    {   fgets(str,80,IO_file);
        if (str == NULL)
        {   fclose(IO_file);
            return 3;
        }
        if (strchr(str,'=') == NULL)
        {   TrimStr(str);
            if (strlen(str))
            {   strncpy(HdInfo->job_number,str,9);
                HdInfo->job_number[9] = 0;
            }
        }
        fgets(str,80,IO_file);
        if (str == NULL)
        {   fclose(IO_file);
            return 3;
        }
        if (strchr(str,'=') == NULL)
        {   TrimStr(str);
            if (strlen(str))
            {   strncpy(HdInfo->title1,str,69);
                HdInfo->title1[69] = 0;
            }
        }
        fgets(str,80,IO_file);
        if (str == NULL)
        {   fclose(IO_file);
            return 3;
        }
        if (strchr(str,'=') == NULL)
        {   TrimStr(str);
            if (strlen(str))
            {   strncpy(HdInfo->title2,str,69);
                HdInfo->title2[69] = 0;
            }
        }
    }
    if (print_it)
    {   printf("job_number = %s\n",HdInfo->job_number);
        printf("title1 = %s\n",HdInfo->title1);
        printf("title2 = %s\n",HdInfo->title2);
    }

    /* Get rest of info */
    field_found = 0;
    rewind(IO_file);
    line_count = 0;
    while (fgets(str,80,IO_file) > 0)  /* get rest of info */
    {   line_count++;
        if (print_it) printf("%s",str);
        cmdstr[0] = 0;            /* set to 0 so strncat works right */
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
                break;
            }
            i++;
        }
        if (found == -1) continue;    /* bad or missing command word */
        cp++;                         /* step 1 past equal sign */
        TrimStr(cp);                  /* remove leading and trailing blanks */
        switch (found)  /* cases depend on same order in SS_HD__CMD[] */
        {   case 0:
                itemp = atoi(cp);
                if (itemp > 0)  HdInfo->num_traces = itemp;
                field_found ++;
                if (print_it) printf("num_traces = %d\n",HdInfo->num_traces);
                break;
            case 1:
                itemp = atoi(cp);
                if (itemp > 0)  HdInfo->num_samples = itemp;
                field_found ++;
                if (print_it) printf("num_samples = %d\n",HdInfo->num_samples);
                break;
            case 2:
                itemp = atoi(cp);
                if (itemp >= 0) HdInfo->time_zero_sample = itemp;
                field_found ++;
                if (print_it) printf("time_zero_sample = %d\n",HdInfo->time_zero_sample);
                break;
            case 3:
                itemp = atoi(cp);
                if (itemp > 0) HdInfo->total_time_ns = itemp;
                field_found ++;
                if (print_it) printf("total_time_ns = %d\n",HdInfo->total_time_ns);
                break;
            case 4:
                HdInfo->start_pos = atof(cp);
                field_found ++;
                if (print_it) printf("start_pos = %g\n",HdInfo->start_pos);
                break;
            case 5:
                HdInfo->final_pos = atof(cp);
                field_found ++;
                if (print_it) printf("final_pos = %g\n",HdInfo->final_pos);
                break;
            case 6:
                HdInfo->step_size = atof(cp);
                field_found ++;
                if (print_it) printf("step_size = %g\n",HdInfo->step_size);
                break;
            case 7:
                HdInfo->pos_units[0] = 0;
                strncpy(HdInfo->pos_units,cp,sizeof(HdInfo->pos_units));
                HdInfo->pos_units[sizeof(HdInfo->pos_units)-1] = 0;
                field_found ++;
                if (print_it) printf("pos_units = %s\n",HdInfo->pos_units);
                break;
            case 8:
                dtemp = atof(cp);
                if (dtemp > 0.0) HdInfo->ant_freq = dtemp;
                field_found ++;
                if (print_it) printf("ant_freq = %g\n",HdInfo->ant_freq);
                break;
            case 9:
                dtemp = atof(cp);
                if (dtemp > 0.0) HdInfo->ant_sep = dtemp;
                field_found ++;
                if (print_it) printf("ant_sep = %g\n",HdInfo->ant_sep);
                break;
            case 10:
                dtemp = atof(cp);
                if (dtemp > 0.0) HdInfo->pulser_voltage = dtemp;
                field_found ++;
                if (print_it) printf("pulser_voltage = %g\n",HdInfo->pulser_voltage);
                break;
            case 11:
                itemp = atoi(cp);
                if (itemp > 0)  HdInfo->num_stacks = itemp;
                field_found ++;
                if (print_it) printf("num_stacks = %d\n",HdInfo->num_stacks);
                break;
            case 12:
                HdInfo->survey_mode[0] = 0;
                strncpy(HdInfo->survey_mode,cp,sizeof(HdInfo->survey_mode));
                HdInfo->survey_mode[sizeof(HdInfo->survey_mode)-1] = 0;
                if (print_it) printf("survey_mode = %s\n",HdInfo->survey_mode);
                field_found ++;
                proc_start_line = line_count;
                break;
            default:
                break;
        }
        if (print_it) getch();
    }
    if (print_it) printf("field_found = %d\n",field_found);
    if (field_found < 1)
    {   fclose(IO_file);
        return 5;
    }

    /* Get processing history */
    rewind(IO_file);
    for (i=0; i<proc_start_line; i++) fgets(str,80,IO_file);
    num = 0;                             /* count how many lines left */
    while (fgets(str,80,IO_file) > 0) num++;
    if (print_it) printf("num = %d\n",num);
    if (num > 0)
    {   HdInfo->proc_hist = (char *)calloc((size_t)((num*80) + 1),1);
        if (HdInfo->proc_hist == NULL)
        {   fclose(IO_file);
            return 4;
        }  /* NOTE: I don't think this is dealloced anywhere! */
        rewind(IO_file);
        for (i=0; i<proc_start_line; i++) fgets(str,80,IO_file);
        cp = HdInfo->proc_hist;                /* point to start of buffer */
        for (i=0; i<num; i++)
        {   fgets(cp,80,IO_file);
            if (print_it) printf("%s",cp);
            cp = strchr(cp,0);                 /* point at NULL terminator */
        }
        if (print_it) printf("proc hist = %s\n",HdInfo->proc_hist);
    }
    fclose(IO_file);
    return 0;
}
/****************************** PrintSsHdInfo() *****************************/
/* Print out the contents of a S&S Info struct to an output device
 * (stdout, stderr, a disk file, etc.)..
 *
 * Parameters:
 *   char *out_filename - "stdout", "stderr", or a DOS filename
 *   char *hd_filename  - DOS name of the *.HD file
 *   struct SsHdrInfoStruct *HdInfo - address of header info structure
 *
 * Requires: <string.h>, <stdio.h>, <conio.h>, "gpr_io.h".
 * Calls: strstr, fprintf, getch, kbhit, fopen, fclose.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 14, 1996
 */
void PrintSsHdInfo (char *out_filename,char *hd_filename,
				   struct SsHdrInfoStruct *HdInfo)
{
	char *cp1,str[80];
	int isfile = 0;
	FILE *stream = NULL;

	/* Assign or open stream */
	if      (strstr(out_filename,"stdout"))  stream = stdout;
	else if (strstr(out_filename,"stderr"))  stream = stderr;
	else
	{   stream = fopen(out_filename,"wt");
		isfile = 1;
	}
	if (stream == NULL) stream = stdout;

	/* Print the header information */
	while (kbhit()) getch();           /* clear keyboard buffer */
	fprintf(stream,"\n\nS&S HD info from file %s\n",hd_filename);
	if (HdInfo->job_number[0])
	{   strcpy(str,HdInfo->job_number);
		cp1 = strchr(str,'\n');
		if (cp1 != NULL) *cp1 = 0;
		fprintf(stream,"%s\n",str);
	}
	if (HdInfo->title1[0])
	{   strcpy(str,HdInfo->title1);
		cp1 = strchr(str,'\n');
		if (cp1 != NULL) *cp1 = 0;
		fprintf(stream,"%s\n",str);
	}
	if (HdInfo->title2[0])
	{   strcpy(str,HdInfo->title2);
		cp1 = strchr(str,'\n');
		if (cp1 != NULL) *cp1 = 0;
		fprintf(stream,"%s\n",str);
	}
	fprintf(stream,"%02d/%02d/%02d\n",HdInfo->day,HdInfo->month,HdInfo->year);
	fprintf(stream,"NUMBER OF TRACES     = %hd\n",HdInfo->num_traces);
	fprintf(stream,"NUMBER OF PTS/TRC    = %hd\n",HdInfo->num_samples);
	fprintf(stream,"TIMEZERO AT POINT    = %hd\n",HdInfo->time_zero_sample);
	fprintf(stream,"TOTAL TIME WINDOW    = %hd\n",HdInfo->total_time_ns);
	fprintf(stream,"STARTING POSITION    = %f\n",HdInfo->start_pos);
	fprintf(stream,"FINAL POSITION       = %f\n",HdInfo->final_pos);
	fprintf(stream,"STEP SIZE USED       = %f\n",HdInfo->step_size);
	fprintf(stream,"POSITION UNITS       = %s\n",HdInfo->pos_units);
	fprintf(stream,"NOMINAL FREQUENCY    = %f\n",HdInfo->ant_freq);
	fprintf(stream,"ANTENNA SEPARATION   = %f\n",HdInfo->ant_sep);
	fprintf(stream,"PULSER VOLTAGE (V)   = %d\n",(int)HdInfo->pulser_voltage);
	fprintf(stream,"NUMBER OF STACKS     = %hd\n",HdInfo->num_stacks);
	fprintf(stream,"SURVEY MODE          = %s\n",HdInfo->survey_mode);
	printf("Press a key to see processing history ...");
	getch();
	puts("");
	if (HdInfo->proc_hist && HdInfo->proc_hist[0])
		fprintf(stream,"%s\n",HdInfo->proc_hist);

	if (isfile) fclose(stream);
}
/******************************* GetSsTrace() *******************************/
/* Read one S&S trace header and trace data.  The file must have been
 * previously opened using fopen() and the file pointer must be at the start
 * of the trace data block and header.
 *
 * Parameters:
 *  int data_block_bytes - number of bytes in data block excluding header
 *  struct SsTraceHdrStruct *hdrPtr - address of trace header structure
 *  short *s_data     - pointer to allocated storage buffer of correct size
 *  FILE *infile   - pointer to FILE structure for the input file, assigned
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, "gpr_io.h".
 * Calls: fread.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *GetSsTraceMsg[] =
{   "GetSsTrace(): No errors.",
    "GetSsTrace() ERROR: NULL pointer in parameter list.",
    "GetSsTrace() ERROR: Short count from fread() (header).",
    "GetSsTrace() ERROR: Short count from fread() (data).",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: January 23, 1995
 */
int GetSsTrace (int data_block_bytes,struct SsTraceHdrStruct *hdrPtr,
                  short *s_data,FILE *infile)
{
    /* Verify parameter list */
    if (hdrPtr == NULL || s_data == NULL || infile == NULL) return 1;

    /* Get header */
    if (fread((void *)hdrPtr,sizeof(struct SsTraceHdrStruct),1,infile) < 1)
        return 2;

    /* Get data */
    if (fread((void *)s_data,data_block_bytes,1,infile) < 1)
		return 3;

    return 0;
}
/****************************** SaveSsHdFile() ******************************/
/* Create a Sensors & Software HD file.
 * NULL pointers are allowed in the argument list except for "filename".
 *
 * Parameters:
 *  int day                -
 *  int month              -
 *  int year               -
 *  long num_traces        -
 *  long num_samples       -
 *  long time_zero_sample  -
 *  int total_time_ns      -
 *  double start_pos       -
 *  double final_pos       -
 *  double step_size       -
 *  char *pos_units        -
 *  double ant_freq        -
 *  double ant_sep         -
 *  double pulser_voltage  -
 *  int num_stacks         -
 *  char *survey_mode      -
 *  char *proc_hist        -
 *  char *filename         -
 *  char *source_filename  -
 *  char *job_number       -
 *  char *title1           -
 *  char *title2           -
 *  double traces_per_sec  -
 *  double meters_per_mark -
 *  int num_gain_pts       -
 *  double gain_pts        -
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, <string.h>, "gpr_io.h".
 * Calls: fopen, fprintf, fclose, strcat, strlen, strcpy, strchr.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *SaveSsHdFileMsg[] =
{   "SaveSsHdFile(): No errors.",
	"SaveSsHdFile() ERROR: NULL pointer passed for output HD filename.",
	"SaveSsHdFile() ERROR: Unable to open output HD file.",
	"SaveSsHdFile() ERROR: Short count from fwrite().",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 14, 1996; August 14, 2001;
 */
int SaveSsHdFile (int day,int month,int year,long num_traces,long num_samples,
                  long time_zero_sample,int total_time_ns,double start_pos,
                  double final_pos,double step_size,char *pos_units,
                  double ant_freq,double ant_sep,double pulser_voltage,
				  int num_stacks,char *survey_mode,char *proc_hist,
				  char *filename,char *source_filename,
                  char *job_number,char *title1,char *title2,
                  double traces_per_sec,double meters_per_mark,
				  int num_gain_pts,double *gain_pts)
{
	char *cp1,str[80],str2[80];
	int i;
	FILE *outfile;

	/* Reality check */
	if (filename == NULL) return 1;

	/* Open stream */
	if ((outfile = fopen(filename,"wt"))== NULL)  return 2;

	/* Write info */
	/* fprintf(outfile,"8000\n\n"); */
	if (job_number && job_number[0])
	{   strncpy(str,job_number,10);
		str[10] = 0;
		cp1 = strchr(str,'\n');
		if (cp1 != NULL) *cp1 = 0;
		cp1 = strchr(str,'\r');
		if (cp1 != NULL) *cp1 = 0;
		fprintf(outfile,"%s\r\n",str);
	}
	if (title1 && title1[0])
	{   strncpy(str,title1,70);
		str[70] = 0;
		cp1 = strchr(str,'\n');
		if (cp1 != NULL) *cp1 = 0;
		cp1 = strchr(str,'\r');
		if (cp1 != NULL) *cp1 = 0;
		fprintf(outfile,"%s\r\n",str);
	}
	if (title2 && title2[0])
	{   strncpy(str,title2,70);
		str[70] = 0;
		cp1 = strchr(str,'\n');
		if (cp1 != NULL) *cp1 = 0;
		cp1 = strchr(str,'\r');
		if (cp1 != NULL) *cp1 = 0;
		fprintf(outfile,"%s\r\n",str);
	}

	fprintf(outfile,"%02d/%02d/%02d\r\n",day,month,year);
	fprintf(outfile,"NUMBER OF TRACES   = %ld \r\n",num_traces);
	fprintf(outfile,"NUMBER OF PTS/TRC  = %ld \r\n",num_samples);
	fprintf(outfile,"TIMEZERO AT POINT  = %ld \r\n",time_zero_sample);
	fprintf(outfile,"TOTAL TIME WINDOW  = %d \r\n",total_time_ns);
	fprintf(outfile,"STARTING POSITION  = %f \r\n",start_pos);
	fprintf(outfile,"FINAL POSITION     = %f \r\n",final_pos);
	fprintf(outfile,"STEP SIZE USED     = %f \r\n",step_size);
	if (pos_units && pos_units[0])
		fprintf(outfile,"POSITION UNITS     = %s \r\n",pos_units);
	else
		fprintf(outfile,"POSITION UNITS     = unknown \r\n");
	fprintf(outfile,"NOMINAL FREQUENCY  = %f \r\n",ant_freq);
	fprintf(outfile,"ANTENNA SEPARATION = %f \r\n",ant_sep);
	fprintf(outfile,"PULSER VOLTAGE (V) = %f \r\n",pulser_voltage);
	fprintf(outfile,"NUMBER OF STACKS   = %d \r\n",num_stacks);
	if (survey_mode && survey_mode[0])
		fprintf(outfile,"SURVEY MODE        = %s \r\n",survey_mode);
	else
		fprintf(outfile,"SURVEY MODE        = unknown \r\n");
	if (source_filename && source_filename[0])
		 fprintf(outfile,"SOURCE DATA FILE   = %s \r\n",source_filename);
	if (traces_per_sec > 0)
		fprintf(outfile,"TRACES PER SECOND  = %f \r\n",traces_per_sec);
	if (meters_per_mark > 0)
		fprintf(outfile,"METERS PER MARK    = %f \r\n",meters_per_mark);
	if (gain_pts && num_gain_pts > 0)
	{   fprintf(outfile,"NUMBER OF GAIN PTS = %d \r\n",num_gain_pts);
		sprintf(str,"GAIN POINTS (db) =");
		for (i=0; i<num_gain_pts; i++)
		{   if (strlen(str) >= 70)  /* can't fit any more on line */
			{   fprintf(outfile,"%s\n",str);
				sprintf(str,"GAIN POINTS (db) = ");
			}
			sprintf(str2," %7.3f",gain_pts[i]);
			strcat(str,str2);
		}
		/* make sure all strings have been printed */
		if (strlen(str) > strlen("GAIN POINTS (db) = "))
			fprintf(outfile,"%s\r\n",str);
	}
	if (proc_hist && proc_hist[0])
	{
#if 0
        for (i=1; proc_hist[i]; i++)
		{
            if ( (proc_hist[i-1] == 0x0D) && (proc_hist[i] == 0x0A) )
			{   proc_hist[i] = ' '; /* was [i-1] */
			}
		}
#endif
		fprintf(outfile,"%s",proc_hist);
	}

	fclose(outfile);
	return 0;
}
/*************************** SetDt1TraceHeader() ****************************/
/* Set the fields in the DT1 trace header.
 *
 * Parameters:
 *  float trace_num         - trace sequence number within line
 *  float position          - position along traverse
 *  float num_samples       - samples per trace
 *  float elevation         - elevation data if available
 *  float num_bytes         - bytes per sample; always 2 for Rev. 3 firmware
 *  float aux_trace_num     - trace number again
 *  float num_stacks        - number of stacks used to get trace
 *  float time_window       - time window in ns
 *  float time_zero_adjust  - where sample x = sample x + adjustment
 *  float zero_flag         - 0 = data OK, 1 = zero data
 *  float time              - seconds past midnight
 *  float comment_flag      - 1 = comment attached
 *  char *comment           - optional character comments
 *  struct SsTraceHdrStruct *HdrPtr - address of trace header structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <string.h>, "gpr_io.h".
 * Calls: memset.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: April 12, 1995; August 13, 2001
 */
void SetDt1TraceHeader (float trace_num,float position,float num_samples,
        float elevation,float num_bytes,float aux_trace_num,
        float num_stacks,float time_window,float time_zero_adjust,
        float zero_flag,float time,float comment_flag,char *comment,
        struct SsTraceHdrStruct *Dt1TraceHdr)
{
    int i;

    memset((void*)Dt1TraceHdr,0,sizeof(struct SsTraceHdrStruct));
    Dt1TraceHdr->trace_num        = trace_num;
    Dt1TraceHdr->position         = position;
    Dt1TraceHdr->num_samples      = num_samples;
    Dt1TraceHdr->elevation        = elevation;
    Dt1TraceHdr->num_bytes        = 2;   /* always 2 */
    Dt1TraceHdr->aux_trace_num    = aux_trace_num;
    Dt1TraceHdr->num_stacks       = num_stacks;
    Dt1TraceHdr->time_window      = time_window;
    Dt1TraceHdr->time_zero_adjust = time_zero_adjust;
    Dt1TraceHdr->zero_flag        = zero_flag;
    Dt1TraceHdr->time             = time;
    Dt1TraceHdr->comment_flag     = comment_flag;
    if (comment_flag == 0)
        for (i=0; i<sizeof(Dt1TraceHdr->comment); i++)
            Dt1TraceHdr->comment[i] = 0;
    else
        for (i=0; i<sizeof(Dt1TraceHdr->comment); i++)
            Dt1TraceHdr->comment[i] = comment[i];
}
/***************************** PrintSsTraceHdr() ****************************/
/* Print out the contents of a S&S trace header to an output device
 * (stdout, stderr, a disk file, etc.)..
 *
 * Parameters:
 *   char *out_filename - "stdout", "stderr", or a DOS filename
 *   struct SsTraceHdrStruct *hdrPtr - address of the trace header structure
 *
 * Requires: <string.h>, <stdio.h>, <conio.h>, "gpr_io.h".
 * Calls: strstr, fprintf, fopen, fclose, getch, kbhit.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: January 23, 1995
 */
void PrintSsTraceHdr (char *out_filename,struct SsTraceHdrStruct *hdrPtr)
{
    int isfile   = 0;
    FILE *stream = NULL;

    /* Assign or open stream */
    if      (strstr(out_filename,"stdout"))  stream = stdout;
    else if (strstr(out_filename,"stderr"))  stream = stderr;
    else
    {   stream = fopen(out_filename,"wt");
        isfile = 1;
    }
    if (stream == NULL) stream = stdout;

    /* Print the header information */
    while (kbhit()) getch();           /* clear keyboard buffer */
    fprintf(stream,"\nS&S header for trace number %f:\n",hdrPtr->trace_num);
    fprintf(stream,"position along traverse = %f\n",hdrPtr->position);
    fprintf(stream,"samples per trace       = %f\n",hdrPtr->num_samples);
    fprintf(stream,"bytes per sample        = %f\n",hdrPtr->num_bytes);
    fprintf(stream,"number of stacks        = %f\n",hdrPtr->num_stacks);
    fprintf(stream,"time window             = %f\n",hdrPtr->time_window);
    fprintf(stream,"time zero adjust        = %f\n",hdrPtr->time_zero_adjust);
    fprintf(stream,"elevation data          = %f\n",hdrPtr->elevation);
    fprintf(stream,"OK flag (0 is OK)       = %f\n",hdrPtr->zero_flag);
    fprintf(stream,"seconds past midnight   = %f\n",hdrPtr->time);
    fprintf(stream,"comment flag            = %f\n",hdrPtr->comment);
    fprintf(stream,"%s\n",hdrPtr->comment);

    if (isfile) fclose(stream);
    return;
}
