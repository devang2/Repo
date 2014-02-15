/******************************** GPR_XSU.C *********************************\
                            version 1.01.10.01
GPR_XSU v. 1.12.21.00 transforms digital ground penetrating radar data from
traces collected evenly in time to evenly-placed spatially-located traces.
Use GPR_PROC to process the data before transforming.  The conversion
is performed as follows.  The user specifies start and stop locations and
a step size, and locations are calculated for the new traces.  The user
also specifies a bin size (which defaults to the step size but may be
smaller or greater than the step size to accomodate dense or sparse data
sets).  Each bin is centered about a new trace location.  A location
is assigned each old trace by splining, using the data from the MRK and
XYZ files.  All traces located within the bin boundaries are averaged to
create the new trace for each bin.  Empty bins are set to the mean value
of the input data type (eg. 32768 for unsigned short integers).  Empty
bins can be avoided by enlarging the step size or the bin size.  X, Y, and
Z values are calculated for each bin and saved to disk as an "XYZ" file.

Only the following computer storage formats are recognized.

GSSI SIR-10A version 3.x to 5.x binary files, 8-bit or 16-bit unsigned
  integers with an embedded descriptive 512- or 1024-byte header.
  Data files usually have the extension "DZT".

Data are stored to disk in the DZT and/or SU format along with new XYZ and
MRK files.

The input to this program is a "CMD" file, an ASCII text file containing
commands (or parameters) describing how to process the radar data.  The
CMD file specifies the data file name (and optionally the storage format).
Inspect the example file GPR_XSU.CMD for usage.

NOTES:
  Only 1 to 4 headers are supported in DZT files.
  There is no graphic display of the data.
  To display the processed data, use programs such as GPR_DISP.EXE or
    FIELDVEW.EXE.

Jeff Lucius, Geophysicist
U.S. Geological Survey
Geologic Division, Crustal Imaging and Characterization Team
Box 25046  Denver Federal Center  MS 964
Denver, CO  80225-0046
303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
email: lucius@usgs.gov

Revision History:
February 13, 1997   - GPR_XFRM completed as an extensive revision of GPR_PROC.C.
August 11, 1997     - Added support for modified DZT formats.
June 8, 1999        - Fixed bug in finding first trace to use.
August 4, 1999      - Fixed bug in DZT_IO.C when num_traces exceeds num_cols.
                      Really need to check that first_trace plus num_cols is
                      less than num_traces.
December 21, 2000   - GPR_XSU completed as an extensive revision of GPR_XFRM.
                    - Changed version number to reflect date: 1.12.12.00.
                    - Had to change how dates are handled. Codebldr is not Y2K
                      compliant! Now use #define to be set compile date by
                      hand and _dos_getdate() and _dos_gettime() for run-time.
                    - Added author message to MSG file.
                    - Added new values for transforming; the marked point can
                      now be offset from the antenna array center - mrk_offset,
                      and the actual trace location (array center) is
                      calculated.
                    - Added statistics output to logfile.
                    - Now calculates transmitter and receiver XYZs and adds to
                      the XYZ output file.
                    - Writes only DZT and SU as output formats.
January 10, 2001    - Changed SU output to 4-byte floats from signed short ints.
                    - Fixed bug where trace header was not updated for every
                      trace.
January 10, 2001    - Fixed index (from i to trace) in writingtrace headers in
                      XsuSaveSuData().

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /zfloatsync GPR_XSU.C gpr_io.lib jl_util1.lib
     To compile for 80486 executable:
       icc /F /xnovm /zfloatsync /zmod486 GPR_XSU.C gpr_io.lib jl_util1.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions, speeds up program.
     Jeff Lucius's GPR I/O functions (DZT, SU) are in gpr_io.lib.
     Jeff Lucius's utility functions are in jl_util1.lib.

     to remove assert() functions from code also add:
     /DNDEBUG

 To run: GPR_XSU cmd_filename

\****************************************************************************/

/*************************** Function dependency ****************************/
/*
 * con I/O  - keyboard and/or text-mode screen used
 * disk I/O - reads from and/or writes to disk
 * DOS      - DOS/BIOS interrupt performed
 * graphics - graphics modes required, direct writes to ports/video adaptor
 * port I/O - hardware I/O ports used
 * RAM      - direct RAM memory access
 *
 * main                                                (con I/O)
 *   +-- ANSI_there                                             (DOS)
 *   |     +-- GetDosVersion                                    (DOS)
 *   +-- Sound                                                       (port I/O)
 *   +-- XsuGetParams                                 (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line and opening the GPR data/info files to get
 *   |     | additional information about the data sets.
 *   |     +-- XsuPrintUsageMsg                        (con I/O)
 *   |     +-- XsuInitParams
 *   |     +-- XsuGetCmdFileArgs             (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetGprFileType                (disk I/O)
 *   |     +-- ReadOneDztHeader              (disk I/O)
 *   |     +-- TrimStr
 *   |     +-- GetMrkData                    (disk I/O)
 *   |     +-- GetXyzData                    (disk I/O)
 *   |     +-- XsuCalcMrkLocs
 *   |     +-- XsuGetXfrmRanges
 *   |            This function finds the first and last traces to read and
 *   |            calculates X, Y, Z for each trace.
 *   +-- XsuDisplayParams                             (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- XsuGetGprDataAsGrid                 (disk I/O)(con I/O)
 *   |     | This function retrieves the data from disk and stores in a grid.
 *   |     +-- GetDztChSubGrid8              (disk I/O)
 *   |     +-- GetDztChSubGrid16             (disk I/O)
 *   +-- XsuXformGprData                     (disk I/O)(con I/O)
 *   |       This function allocates storage and initializes the Bin Array and
 *   |       places the traces in the bin, calculating the average trace for
 *   |       each bin.  Then each bin trace is resample if requested.  X, Y,
 *   |       Z values are calculated for each bin.
 *   +-- XsuSaveGprData                      (disk I/O)(con I/O)
 *   |     | This function copies the traces from the bins into the input
 *   |     | grid (converting the datatype) and calls the data save function.
 *   |     +-- XsuSaveDztData                (disk I/O)(con I/O)
 *   |     |     +-- ReadOneDztHeader        (disk I/O)
 *   |     |     +-- SetDzt5xHeader
 *   |     |     +-- SaveDztFile             (disk I/O)
 *   |     +-- XsuSaveSuData                 (disk I/O)
 *   |     |     +-- SetSuFileHeader
 *   +-- XsuSaveXyzMrkData
 *   |       This function creates XYZ and MRK files for the new DZT file.
 *   +-- XsuDeallocInfoStruct
 */

/******************* Includes all required header files *********************/
#include "GPR_XSU.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The next array is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_XSU_CMDS[] =
{   "debug","batch","dzt_infilename","mrk_infilename","xyz_infilename",
    "dzt_outfilename","channel","spatial_dir","spatial_start","spatial_stop",

    "spatial_step","bin_size","su_outfilename","mrk_offset","ant_sep",
    "year","month","day","hour","minute",

    "second",NULL,
} ;

const char *XsuGetParamsMsg[] =
{   "XsuGetParams(): No errors.",
	"XsuGetParams() ERROR:  Insufficient command line arguments.",
    "XsuGetCmdFileArgs() ERROR: Unable to open input command file.",
    "XsuGetCmdFileArgs() ERROR: Output file not specified.",
    "XsuGetCmdFileArgs() ERROR: Input file(s) not specified.",

    "XsuGetParams() ERROR: Problem getting information about input data file.",
    "XsuGetParams() ERROR: Invalid channel selection for DZT file.",
    "XsuGetParams() ERROR: Problem getting info from HD file.",
    "XsuGetParams() ERROR: Problem getting info from SGY file.",
    "XsuGetParams() ERROR: Problem determining user-defined parameters.",

    "XsuGetParams() ERROR: No recognizable data/info input files specified.",
    "XsuGetParams() ERROR: Invalid input data element type .",
    "XsuGetParams() ERROR: Less than two traces in file.",
    "XsuGetParams() ERROR: Less than two samples per trace.",
    "XsuGetParams() ERROR: nanoseconds per sample is <= 0.0.",

    "XsuGetParams() ERROR: trace_header_size must be a multiple of sample size.",
    "XsuGetParams() ERROR: spatial distance must be a multiple of spatial_step.",
    "XsuGetParams() ERROR: MRK and XYZ files must be specified for this GPR file.",
    "XsuGetParams() ERROR: Problem getting data from MRK file.",
    "XsuGetParams() ERROR: Problem getting data from XYZ file.",

    "XsuGetParams() ERROR: Number of tick marks not same in MRK and XYZ files.",
    "XsuGetParams() ERROR: Not able to allocate sufficient temporary storage.",
    "XsuGetParams() ERROR: Less than two samples per trace for transformed data.",
    "XsuGetParams() ERROR: New total_time or ns_per_samp <= 0.0.",
    "XsuGetParams() ERROR: Storage format is not GSSI DZT.",

    "XsuGetParams() ERROR: Problem in subroutine XsuGetXfrmRanges().",
    "XsuGetParams() ERROR: Number of bins exceeds number of input traces.",
	"XsuGetParams() ERROR: Spatial_step direction must match start->stop direction.",
	"XsuGetParams() ERROR: Unable to allocate temporary storage for splining.",
	"XsuGetParams() ERROR: Problem splining data for mark offsets.",

	"XsuGetParams() ERROR: Problem splining data for antenna offsets.",
	"XsuGetParams() ERROR: spatial_step equals 0.",
} ;

const char *XsuGetGprDataAsGridMsg[] =
{   "XsuGetGprDataAsGrid(): No errors.",
    "XsuGetGprDataAsGrid() ERROR: Attemp made to de-allocate storage before allocating.",
    "XsuGetGprDataAsGrid() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "XsuGetGprDataAsGrid() ERROR: Invalid input data element type .",
    "XsuGetGprDataAsGrid() ERROR: No recognized data/info input files specified.",

    "XsuGetGprDataAsGrid() ERROR: Not able to allocate storage for grid.",
    "XsuGetGprDataAsGrid() ERROR: Problem getting data from input file.",
} ;
const char *XsuGetXfrmRangesMsg[] =
{   "XsuGetXfrmRanges() No error.",
    "XsuGetXfrmRanges() ERROR: Number of marked traces < 2.",
    "XsuGetXfrmRanges() ERROR: Marked traces outside data range.",
    "XsuGetXfrmRanges() ERROR: spatial_start/_stop are the reverse of data direction.",
    "XsuGetXfrmRanges() ERROR: Problem splining data topography.",

    "XsuGetXfrmRanges() ERROR: Not able to allocate sufficient temporary storage.",
} ;
const char *XsuCalcMrkLocsMsg[] =
{   "XsuCalcMrkLocs() No error.",
    "XsuCalcMrkLocs() ERROR: Number of grid columns is less than 2.",
    "XsuCalcMrkLocs() ERROR: Not able to allocate sufficient temporary storage.",
	"XsuCalcMrkLocs() ERROR: Problem splining data for mark offsets",
} ;
const char *XsuCalcBinLocsMsg[] =
{   "XsuCalcBinLocs() No error.",
    "XsuCalcBinLocs() ERROR: Number of grid columns is less than 3.",
    "XsuCalcBinLocs() ERROR: Not able to allocate sufficient T/R XYZ storage.",
    "XsuCalcBinLocs() ERROR: Not able to allocate sufficient temporary storage.",
	"XsuCalcBinLocs() ERROR: Problem splining data for antenna offsets",
} ;

const char *XsuSaveXyzMrkDataMsg[] =
{   "XsuSaveXyzMrkData(): No errors.",
    "XsuSaveXyzMrkData() ERROR: Data/information was not saved to disk.",
} ;

const char *XsuXformGprDataMsg[] =
{   "XsuXformGprData(): No errors.",
    "XsuXformGprData() ERROR: Invalid input data element type .",
    "XsuXformGprData() ERROR: Not able to allocate storage for bin array.",
    "XsuXformGprData() ERROR: At least one empty bin was found. See log file.",
    "XsuXformGprData() ERROR: Not able to allocate temporary storage.",

    "XsuXformGprData() ERROR: Problem splining data.  See log file.",
} ;

const char *XsuSaveGprDataMsg[] =
{   "XsuSaveGprData(): No errors.",
    "XsuSaveGprData() ERROR: Invalid/unknown file storage format.",
	"XsuSaveGprData() ERROR: Data/information was not saved to disk.",
} ;

const char *XsuSaveDztDataMsg[] =
{   "XsuSaveDztData(): No errors.",
    "XsuSaveDztData() ERROR: Invalid datatype for output DZT file.",
    "XsuSaveDztData() ERROR: Problem getting input file header.",
    "XsuSaveDztData() ERROR: Problem saving DZT data.",
} ;

const char *XsuSaveSuDataMsg[] =
{   "XsuSaveDt1Data(): No errors.",
    "XsuSaveDt1Data() ERROR: Unable to allocate temporary storage.",
    "XsuSaveDt1Data() ERROR: Unable to open new SU file on disk.",
    "XsuSaveDt1Data() ERROR: Problem saving SU file to disk.",
} ;

/********************************** main() **********************************/
void main (int argc,char *argv[])
{
/***** Declare variables *****/
    char log_filename[80];

    /* following are scratch variables used by main */
    int  itemp;
    double dtemp;

    /* following is the information structure passed between functions */
    struct XsuParamInfoStruct ParamInfo;

    /* these variables found near beginning of this source */
    extern int Debug,Batch,ANSI_THERE;
    extern FILE *log_file;
    extern const char *XsuGetParamsMsg[];
    extern const char *XsuGetGprDataAsGridMsg[];
    extern const char *XsuXformGprDataMsg[];
    extern const char *XsuSaveGprDataMsg[];
    extern const char *XsuSaveXyzMrkDataMsg[];
    
/***** Initialize variables *****/
    /* Open error message file */
    strcpy(log_filename,"GPR_XSU.log");
    log_file = fopen(log_filename,"a+t");
    if (log_file == NULL)
    {   strcpy(log_filename,"c:\\GPR_XSU.log");
        log_file = fopen(log_filename,"a+t");
    }
    if (log_file)
    {   struct dosdate_t dosdate;
        struct dostime_t dostime;
        char *month[] = { "","January","February","March","April","May","June",
                            "July","August","September","October",
                            "November","December" } ;

        _dos_getdate(&dosdate);
        _dos_gettime(&dostime);
        fprintf(log_file,"\n**************************************************************************\n");
        fprintf(log_file,"Messages from program GPR_XSU v. %s: %s %d, %d at %d:%02d:%02d\n",
                        GPR_XSU_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute,dostime.second);
        fprintf(log_file,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.\n");
    }

    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Zero-out information structure */
    memset((void *)&ParamInfo,0x00,sizeof(struct XsuParamInfoStruct));

/***** Get information used to process data *****/
    if (log_file && argv[1])
        fprintf(log_file,"Processing command file: %s\n",argv[1]);

    printf("Getting user parameters ...");
    itemp =  XsuGetParams(argc,argv,&ParamInfo);
    printf("\r                                          \r");
    if (itemp > 0)
    {   printf("\n%s\n",XsuGetParamsMsg[itemp]);
        XsuDeallocInfoStruct(&ParamInfo);
        if (log_file)
        {   fprintf(log_file,"%s\n",XsuGetParamsMsg[itemp]);
            fprintf(log_file,"bin_size=%g step=%g min_trace_sep=%g max_trace_sep=%g\n",
                ParamInfo.bin_size,ParamInfo.spatial_step,ParamInfo.min_trace_sep,ParamInfo.max_trace_sep);
            fclose(log_file);
		}
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		printf("\nGPR_XSU finished.\nFile %s on disk contains messages.\n",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        exit(1);
    }

    if (strcmp(ParamInfo.dzt_infilename,ParamInfo.dzt_outfilename) == 0 ||
        strcmp(ParamInfo.mrk_infilename,ParamInfo.mrk_outfilename) == 0 ||
        strcmp(ParamInfo.xyz_infilename,ParamInfo.xyz_outfilename) == 0
       )
    {   printf("\nERROR: Input filename is duplicated in output list.\n");
        XsuDeallocInfoStruct(&ParamInfo);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"ERROR: Input filename is duplicated in output list.\n");
            fprintf(log_file,"bin_size=%g step=%g min_trace_sep=%g max_trace_sep=%g\n",
                ParamInfo.bin_size,ParamInfo.spatial_step,ParamInfo.min_trace_sep,ParamInfo.max_trace_sep);
            fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("GPR_XSU finished.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		exit(2);
	}
	if (log_file)
	{   if (Batch) fprintf(log_file,"Program in Batch mode.\n");
		if (Debug) fprintf(log_file,"Program in Debug mode.\n");
	}

/***** Display parameters and ask if user wants to continue *****/
	XsuDisplayParams(&ParamInfo);
	if (!Batch)
	{   printf("\nPress <Esc> to quit or other key to continue ... ");
		if ((itemp = getch()) == ESC)
		{   puts("\nProgram terminated by user.");
			XsuDeallocInfoStruct(&ParamInfo);
			if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
			if (log_file)
			{   fprintf(log_file,"Program terminated by user.\n");
				fclose(log_file);
				printf("File %s on disk contains messages.\n",log_filename);
			}
			puts("GPR_XSU finished.");
			if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
			exit(3);
		} else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
			getch();
		printf("\r                                                                    \r");
	}

/***** Get the data *****/
	/* printf("Getting Data ..."); */
    printf("Getting data from GPR file %s ...",ParamInfo.dzt_infilename);
	itemp = XsuGetGprDataAsGrid(FUNC_CREATE,&ParamInfo);
	printf("\r                                                                 \r");
	if (itemp > 0)
	{   printf("\n%s\n",XsuGetGprDataAsGridMsg[itemp]);
		if (log_file)
        {   fprintf(log_file,"\n%s\n",XsuGetGprDataAsGridMsg[itemp]);
            fprintf(log_file,"File %s NOT PROCESSED\n",ParamInfo.dzt_infilename);
        }
		if (itemp == 5)  /* data set too large for memory */
		{   unsigned long mem,stack,bytes,requested;

			mem = _memavl();
			switch (ParamInfo.input_datatype)
			{   case -1:  case 1:                    bytes = 1;  break;
				case -2:  case 2:  case 5:           bytes = 2;  break;
				case -3:  case 3:  case 6:  case 4:  bytes = 4;  break;
				case  8:                             bytes = 8;  break;
			}
			requested = (ParamInfo.last_trace - ParamInfo.first_trace + 1)
						 * ParamInfo.data_samps * bytes;
			stack = requested/mem;
            printf("\nSize of GPR data file (%g MB) exceeds memory (%g MB).\n",requested/(1024.0*1024.0),mem/(1024.0*1024.0));
            printf("Try using GPR_CNDS first with skip_traces set to %lu or higher.\n",stack+2);
            printf("Also, try increasing the region size of GPR_XSU.EXE using MODXCONF.\n");
            if (log_file)
            {   fprintf(log_file,"Size of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
                fprintf(log_file,"Try using GPR_CNDS first with skip_traces set %lu or higher.\n",stack+2);
            }
		}
		XsuDeallocInfoStruct(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		if (log_file)
		{   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("\nGPR_XSU finished with an error.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		if (!Batch)
		{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
				Sound(dtemp,0.055);
		}
		exit(4);
	}

/***** Transform the data *****/
	printf("Transforming Data.  Please wait ...");
	itemp = XsuXformGprData(&ParamInfo);
	printf("\r                                      \r");
	if (itemp > 0)
	{   printf("\n%s\n",XsuXformGprDataMsg[itemp]);
		XsuGetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
		XsuDeallocInfoStruct(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		if (log_file)
		{   fprintf(log_file,"%s\n",XsuXformGprDataMsg[itemp]);
			fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("\nGPR_XSU finished with an error.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		if (!Batch)
		{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
				Sound(dtemp,0.055);
		}
		exit(5);
	}

/***** Save grid to disk *****/
	/* printf("Saving data to disk ..."); */
    printf("Saving file %s ...",ParamInfo.dzt_infilename);
	itemp = XsuSaveGprData(&ParamInfo);
	printf("\r                                                                  \r");
	if (itemp > 0)
	{   printf("\n%s\n",XsuSaveGprDataMsg[itemp]);
		XsuGetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
		XsuDeallocInfoStruct(&ParamInfo);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		if (log_file)
		{   fprintf(log_file,"%s\n",XsuSaveGprDataMsg[itemp]);
			fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("\nGPR_XSU finished with an error.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		if (!Batch)
		{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
				Sound(dtemp,0.055);
		}
		exit(6);
	}

/***** Save XYZ info to disk *****/
    if (ParamInfo.dzt_outfilename[0])
    {   printf("Saving XYZ and MRK files to disk ...");
        itemp = XsuSaveXyzMrkData(&ParamInfo);
	    printf("\r                                         \r");
        if (itemp > 0)
	    {   printf("\n%s\n",XsuSaveXyzMrkDataMsg[itemp]);
	        XsuGetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
        	XsuDeallocInfoStruct(&ParamInfo);
	       	if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
	        if (log_file)
        	{   fprintf(log_file,"%s\n",XsuSaveXyzMrkDataMsg[itemp]);
	       		fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
	        	fclose(log_file);
		        printf("File %s on disk contains messages.\n",log_filename);
        	}
	       	puts("\nGPR_XSU finished with an error.");
	        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        	if (!Batch)
	       	{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
	        		Sound(dtemp,0.055);
        	}
	       	exit(7);
        }
    }

/***** Free storage and terminate program *****/
	itemp = XsuGetGprDataAsGrid(FUNC_DESTROY,&ParamInfo);
	XsuDeallocInfoStruct(&ParamInfo);
	if (itemp > 0)
	{   printf("\n%s\n",XsuGetGprDataAsGridMsg[itemp]);
		if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
		if (log_file)
		{   fprintf(log_file,"%s\n",XsuGetGprDataAsGridMsg[itemp]);
			fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
			fclose(log_file);
			printf("File %s on disk contains messages.\n",log_filename);
		}
		puts("\nGPR_XSU finished with an error.");
		if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
		if (!Batch)
		{   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
				Sound(dtemp,0.055);
		}
		exit(7);
	}

	if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
	if (log_file)
	{   fprintf(log_file,"End of error messages.  Program terminated normally.\n");
		fprintf(log_file,"GPR_XSU finished.  Grid saved to disk in %s.\n",ParamInfo.dzt_outfilename);
		fclose(log_file);
		printf("\nFile %s on disk contains messages.\n",log_filename);
	}
	printf("GPR_XSU finished.  Grid saved to disk in ");
	if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
	printf("%s.\n",ParamInfo.dzt_outfilename);
	exit(0);
}
/******************************* end of main() ******************************/

/****************************** XsuGetParams() *****************************/
/* Initialize processing parameters and get user-defined values.
 * Determine GPR file storage format.
 * Get information from the appropriate "information" files.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <math.h>, "GPR_XSU.h".
 * Calls: strcpy, printf, fprintf, getch, strstr, strupr, puts, strncat,
 *        atoi, atof,
 *        XsuPrintUsageMsg, XsuGetCmdFileArgs, XsuInitParams.
 *        ReadOneDztHeader, GetMrkData, GetXyzData, XsuGetXfrmRanges,
 *        XsuCalcMrkLocs
 *
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *XsuGetParamsMsg[] =
{   "XsuGetParams(): No errors.",
	"XsuGetParams() ERROR:  Insufficient command line arguments.",
	"XsuGetCmdFileArgs() ERROR: Unable to open input command file.",
	"XsuGetCmdFileArgs() ERROR: Output file not specified.",
	"XsuGetCmdFileArgs() ERROR: Input file(s) not specified.",

	"XsuGetParams() ERROR: Problem getting information about input data file.",
	"XsuGetParams() ERROR: Invalid channel selection for DZT file.",
	"XsuGetParams() ERROR: Problem getting info from HD file.",
	"XsuGetParams() ERROR: Problem getting info from SGY file.",
	"XsuGetParams() ERROR: Problem determining user-defined parameters.",

	"XsuGetParams() ERROR: No recognizable data/info input files specified.",
	"XsuGetParams() ERROR: Invalid input data element type .",
	"XsuGetParams() ERROR: Less than two traces in file.",
	"XsuGetParams() ERROR: Less than two samples per trace.",
	"XsuGetParams() ERROR: nanoseconds per sample is <= 0.0.",

	"XsuGetParams() ERROR: trace_header_size must be a multiple of sample size.",
	"XsuGetParams() ERROR: spatial distance must be a multiple of spatial_step.",
	"XsuGetParams() ERROR: MRK and XYZ files must be specified for this GPR file.",
	"XsuGetParams() ERROR: Problem getting data from MRK file.",
	"XsuGetParams() ERROR: Problem getting data from XYZ file.",

	"XsuGetParams() ERROR: Number of tick marks not same in MRK and XYZ files.",
	"XsuGetParams() ERROR: Not able to allocate sufficient temporary storage.",
	"XsuGetParams() ERROR: Less than two samples per trace for transformed data.",
    "XsuGetParams() ERROR: New total_time or ns_per_samp <= 0.0.",
    "XsuGetParams() ERROR: Storage format is not GSSI DZT.",

    "XsuGetParams() ERROR: Problem in subroutine XsuGetXfrmRanges().",
	"XsuGetParams() ERROR: Number of bins exceeds number of input traces.",
	"XsuGetParams() ERROR: Spatial_step direction must match start->stop direction.",
	"XsuGetParams() ERROR: Unable to allocate temporary storage for splining.",
	"XsuGetParams() ERROR: Problem splining data for mark offsets.",

	"XsuGetParams() ERROR: Problem splining data for antenna offsets.",
	"XsuGetParams() ERROR: spatial_step equals 0.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 7, 1997; December 19, 2000;
 */
int XsuGetParams (int argc,char *argv[],struct XsuParamInfoStruct *InfoPtr)
{

/***** Declare variables *****/
    int  req_args = 2;          /* exe_name and cmd_filename */
	int  i,itemp;               /* scratch variables and counters */
    long samp_bytes;            /* number of bytes in one trace sample */
    double dtemp,dtemp2,dtemp3,dtemp4; /* scratch variables */

    /* this group used by GetGprFileType() */
    int  file_hdr_bytes,num_hdrs,trace_hdr_bytes,samples_per_trace;
    int  num_channels,input_datatype,total_time,file_type;
    long num_traces;

    /* this group used to get DZT file */
    int channel,header_size;
    struct DztHdrStruct DztHdr;                /* from gpr_io.h */
    extern const char *ReadOneDztHeaderMsg[];  /* from gpr_io.h */

    extern int Debug;                        /* beginning of this source */
    extern FILE *log_file;                   /* beginning of this source */
    extern const char *GetGprFileTypeMsg[];  /* from gpr_io.h */

/***** Verify usage *****/
    if (argc < req_args)
    {   XsuPrintUsageMsg();
        return 1;
    }

/***** Initialize information structure *****/
    XsuInitParams(InfoPtr);
    strncpy(InfoPtr->cmd_filename,argv[1],sizeof(InfoPtr->cmd_filename)-1);

/***** Get user-defined parameters from CMD file *****/
    itemp = XsuGetCmdFileArgs(InfoPtr);
    if (itemp > 0) return itemp;      /* 2 to 4 */

/***** Determine GPR file storage format and essential info *****/
    if (InfoPtr->storage_format == 0)
    {   itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                    &samples_per_trace,&num_channels,&num_traces,
                    &input_datatype,&total_time,&file_type,
                    InfoPtr->dzt_infilename);
        if (itemp > 0)
		{   printf("\n%s\n",GetGprFileTypeMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
            return 5;
        }
        if (!(file_type == DZT || file_type == MOD_DZT))
        {   if (log_file) fprintf(log_file,"ERROR: Storage format is not GSSI DZT.\n");
            return 24;
        }
        InfoPtr->storage_format     = file_type;
        InfoPtr->file_header_bytes  = file_hdr_bytes;
        InfoPtr->trace_header_bytes = trace_hdr_bytes;  /* 0 for GSSI DZT file */
        InfoPtr->total_traces       = num_traces;
        InfoPtr->data_samps         = samples_per_trace;
        InfoPtr->total_time         = total_time;
        InfoPtr->ns_per_samp        = (double)InfoPtr->total_time / (InfoPtr->data_samps - 1);
        InfoPtr->input_datatype     = input_datatype;
    }
    if (InfoPtr->storage_format != DZT && InfoPtr->storage_format != MOD_DZT)
        return 10; /* only DZT files used for now */

/***** Get necessary information from info file/header *****/
    /* get first DZT header in file */
    channel = 0;
    itemp = ReadOneDztHeader(InfoPtr->dzt_infilename,&num_hdrs,
                             &(InfoPtr->total_traces),channel,
                             &header_size,&DztHdr);
    if (itemp > 0)
    {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
        if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
        {   puts("Fatal error.");
            return 5;
        }
    }
    if (header_size < 1024)
    {   puts("This DZT header is not consistent with version 5.x SIR-10A software.");
        if (log_file) fprintf(log_file,"This DZT header is not consistent with version 5.x SIR-10A software.\n");
    }
    if (InfoPtr->channel >= num_hdrs) return 6;

    /* get selected header (if different than first) */
    if (InfoPtr->channel > 0)
    {   itemp = ReadOneDztHeader(InfoPtr->dzt_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),InfoPtr->channel,
                                 &header_size,&DztHdr);
        if (itemp > 0)
        {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
            if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
            {   puts("Fatal error.");
                return 5;
            }
        }
    }

    /* assign values to info structure */
    InfoPtr->data_samps       = DztHdr.rh_nsamp;
    if      (DztHdr.rh_bits ==  8) InfoPtr->input_datatype = -1;
    else if (DztHdr.rh_bits == 16) InfoPtr->input_datatype = -2;
    else if (DztHdr.rh_bits == 32) InfoPtr->input_datatype = -3;
    InfoPtr->total_time       = DztHdr.rh_range;
	InfoPtr->ns_per_samp      = DztHdr.rh_range/(DztHdr.rh_nsamp-1);
    InfoPtr->first_proc_samp  = 2;
    if (Debug)
    {   printf("total_traces = %u data_samps = %u total_time = %g\n",
    			InfoPtr->total_traces,InfoPtr->data_samps,InfoPtr->total_time);
        printf("ns_per_samp = %g  datatype = %d  first_proc_samp = %d\n",
                InfoPtr->ns_per_samp,InfoPtr->input_datatype,InfoPtr->first_proc_samp);
        printf("num_hdrs = %d\n",num_hdrs);
        getch();
    }

/***** Get tick mark trace locations and XYZ data *****/
    if (InfoPtr->mrk_infilename[0] && InfoPtr->xyz_infilename[0])
    {
        int num_mrk_ticks,num_xyz_ticks;
        extern const char *GetMrkDataMsg[];  /* in gpr_io.h */
        extern const char *GetXyzDataMsg[];  /* in gpr_io.h */

        /* NOTE: GetMrkData() and GetXyzData() allocate storage for
                 ->tick_tracenum[] and ->tick_xyz[][] */
        itemp = GetMrkData(InfoPtr->mrk_infilename,&num_mrk_ticks,&(InfoPtr->tick_tracenum));
        if (itemp > 0)
        {   printf("\n%s\n",GetMrkDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetMrkDataMsg[itemp]);
            return 18;
        }
        InfoPtr->num_ticks = num_mrk_ticks;
        itemp = GetXyzData(InfoPtr->xyz_infilename,&num_xyz_ticks,&(InfoPtr->tick_xyz));
        if (itemp > 0 || num_xyz_ticks != num_mrk_ticks)
        {   printf("\n%s\n",GetXyzDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetXyzDataMsg[itemp]);
            if (itemp > 0)  return 19;
            else            return 20;
        }
    } else
    {   return 17;
    }
    if (Debug)
    {   puts("\nOld mark locations:");
        for (i=0; i<InfoPtr->num_ticks; i++)
        { printf("[%d] X=%g Y=%g Z=%g\n",i,InfoPtr->tick_xyz[i][0],
                  InfoPtr->tick_xyz[i][1],InfoPtr->tick_xyz[i][2]);
        }
        getch();
    }

/***** Adjust mark XYZs if mrk_offset != 0 *****/
        if (InfoPtr->mrk_offset != 0) /* marks not at center of antenna array */
        {   itemp = XsuCalcMrkLocs(InfoPtr);
            if (itemp > 0)
            {   printf("\n%s\n",XsuCalcMrkLocsMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",XsuCalcMrkLocsMsg[itemp]);
                return 29;
            }
        }
        if (Debug)
        {   puts("\nNew mark locations:");
            for (i=0; i<InfoPtr->num_ticks; i++)
            { printf("[%d] X=%g Y=%g Z=%g\n",i,InfoPtr->tick_xyz[i][0],
                      InfoPtr->tick_xyz[i][1],InfoPtr->tick_xyz[i][2]);
            }
            getch();
        }

/***** Finish initializing variables and verifying valid ranges *****/

    if (Debug) puts("Start of initialize and check section");

    /* Check for valid ranges in required parameters */
    switch (InfoPtr->input_datatype)
    {   case 1: case -1:                    samp_bytes = 1;  break;
        case 2: case -2: case -5:           samp_bytes = 2;  break;
        case 3: case -3: case  4: case -6:  samp_bytes = 4;  break;
        case 8:                             samp_bytes = 8;  break;
        default:
            return 11;
            /* break;   ** unreachable code */
    }
    if (InfoPtr->total_traces < 2)   return 12;
    if (InfoPtr->data_samps < 2)     return 13;
    if (InfoPtr->ns_per_samp <= 0.0) return 14;

    /* Limit first and last traces used to valid ranges */
    if (InfoPtr->first_trace < 0)
		InfoPtr->first_trace = 0;
    if (InfoPtr->last_trace < 0)
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->last_trace <= InfoPtr->first_trace)
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->last_trace > InfoPtr->total_traces - 1)
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->first_trace >= InfoPtr->last_trace)
        InfoPtr->first_trace = 0;

    /* Verify coordinate ranges; determine number of input columns (traces);
       allocate storage for and assign trace_Xvals, trace_Yvals, and
       trace_Zvals; spline to get X, Y, and Z locations for each trace;
       find smallest and largest spacing between traces from trace_?vals.
     */
    itemp = XsuGetXfrmRanges(InfoPtr);
    if (itemp > 0)
    {   printf("\n%s\n",XsuGetXfrmRangesMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",XsuGetXfrmRangesMsg[itemp]);
        return 25;
    }

    /* Assign number of rows in the input data grid */
    itemp = InfoPtr->trace_header_bytes/samp_bytes;  /* truncation expected */
    if ((itemp * samp_bytes) != InfoPtr->trace_header_bytes)  return 15;
    InfoPtr->num_rows = (InfoPtr->trace_header_bytes/samp_bytes) +
                        InfoPtr->data_samps;

    /* Calculate number of output traces (same as number of bins) */
    if (InfoPtr->spatial_step == 0)
    {   if (log_file)
        {   fprintf(log_file,"spatial_step = %g\n",InfoPtr->spatial_step);
        }
        return 31;
    }
    if (InfoPtr->spatial_dir != 0 && InfoPtr->spatial_dir != 1)
        InfoPtr->spatial_dir = 0;
	dtemp = InfoPtr->spatial_stop - InfoPtr->spatial_start;
	dtemp2 = dtemp/InfoPtr->spatial_step;
	if (dtemp2 < 0.0)
	{   if (log_file)
		{   fprintf(log_file,"%s\n",XsuGetParamsMsg[27]);
			fprintf(log_file,"\nstart=%g  stop=%g  step=%g\n",InfoPtr->spatial_start,
				InfoPtr->spatial_stop,InfoPtr->spatial_step);
		}
		return 27;
	}
	dtemp3 = dtemp2 * InfoPtr->spatial_step;
	dtemp4 = _ABS(dtemp3 - dtemp);
	if (dtemp4 > 0.0)
	{   if (log_file)
		{   fprintf(log_file,"%s\n",XsuGetParamsMsg[16]);
			fprintf(log_file,"\nstop=%g  start=%g  stop-start=%g\nnum_steps=%g step=%g num_steps*step=%g diff=%g\n",InfoPtr->spatial_stop,
                InfoPtr->spatial_start,dtemp,dtemp2,InfoPtr->spatial_step,dtemp3);
        }
        return 16;
    }
	InfoPtr->num_bins = (int)dtemp2 + 1;
    if (InfoPtr->num_bins > InfoPtr->num_cols)
    {   if (log_file)
        {   fprintf(log_file,"num_bins = %ld\n",InfoPtr->num_bins);
        }
        return 26;
    }

    /* Check bin size */
	if (InfoPtr->bin_size == 0.0) InfoPtr->bin_size = _ABS(InfoPtr->spatial_step);

    /* Set mean_value to appropriate value */
    switch (InfoPtr->input_datatype)
    {    case 1:  case 2:  case 3:  case 4:  case 8: /* signed data */
            InfoPtr->mean_value = 0.0;
            break;
        case -1:   /* unsigned characters */
            InfoPtr->mean_value = 128.0;
            break;
        case -2:   /* unsigned shorts or 2-byte ints */
            InfoPtr->mean_value = 32768.0;
            break;
        case -3:   /* unsigned longs or 4-byte ints */
            InfoPtr->mean_value = 2147483648.0;
            break;
        case -5:   /* unsigned shorts, only first 12 bits used */
            InfoPtr->mean_value = 2048.0;
			break;
        case -6:   /* unsigned longs, only first 24 bits used */
            InfoPtr->mean_value = 8388608.0;
            break;
    }

    return 0;
}
/****************************** XsuGetXfrmRanges() *****************************/
/* Find limits for spatial_start and spatial_stop and first_trace and last_trace.
 * Spline to get X, Y, and Z locations for each trace.
 * Find smallest and largest spacing between traces from "Xvals".
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <math.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h"
 * Calls: assert, malloc, free, sqrt, Spline, printf.
 * Returns: 0 on success,
 *         >0 on error (offset into message string array)
 *
const char *XsuGetXfrmRangesMsg[] =
{   "XsuGetXfrmRanges() No error.",
    "XsuGetXfrmRanges() ERROR: Number of marked traces < 2.",
    "XsuGetXfrmRanges() ERROR: Marked traces outside data range.",
    "XsuGetXfrmRanges() ERROR: spatial_start/_stop are the reverse of data direction.",
    "XsuGetXfrmRanges() ERROR: Problem splining data topography.",
*5* "XsuGetXfrmRanges() ERROR: Not able to allocate sufficient temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: February 11, 1997
 */
int XsuGetXfrmRanges (struct XsuParamInfoStruct *InfoPtr)
{
    int i,itemp,error;
    int dat_forward;    /* records whether trace locations are increasing or decreasing */
    int hor_forward;    /* records whether hor. dir. is increasing or decreasing */
    double dtemp;
    double *trace_num     = NULL; /* usable trace numbers */
    double *y             = NULL; /* "y" values pulled out of tick_xyz[][] */
    double *tick_tracenum = NULL; /* copy InfoPtr array into double array */

    extern const char *SplineMsg[]; /* in jl_util.c */
    extern FILE *log_file;
    extern int Debug;


    /* Check that arrays are allocated */
    assert(InfoPtr->tick_tracenum && InfoPtr->tick_xyz);

    /* Must have at least 2 tick marks and valid data sets */
    if (InfoPtr->num_ticks < 2) return 1;

    /* Verify range of tick marks at least partially overlaps data;
       last_trace is always >= first_trace;
       tracenum[] always increases.
     */
    if ((InfoPtr->tick_tracenum[0] > InfoPtr->last_trace) ||
        (InfoPtr->tick_tracenum[InfoPtr->num_ticks-1] < InfoPtr->first_trace) )
        return 2;


    /* Limit start and stop to valid ranges and find first and last traces */
    switch (InfoPtr->spatial_dir)
    {   case X_DIR:   /* X-coordinates */
            /* verify requested horizontal start and stop within data range */
            if (InfoPtr->tick_xyz[0][0] < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                InfoPtr->dat_forward = dat_forward = TRUE;
			else  InfoPtr->dat_forward = dat_forward = FALSE;
            if (InfoPtr->spatial_start == INVALID_VALUE)
                InfoPtr->spatial_start = InfoPtr->tick_xyz[0][0];
            if (InfoPtr->spatial_stop == INVALID_VALUE)
                InfoPtr->spatial_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];

            if (InfoPtr->spatial_start < InfoPtr->spatial_stop)
                hor_forward = TRUE;
            else  hor_forward = FALSE;
            if (dat_forward != hor_forward)  return 3;

            /* find start and stop traces */
            if (dat_forward)
            {   if (hor_forward)
                {   /* find first trace */
                    if (InfoPtr->spatial_start <= InfoPtr->tick_xyz[0][0])
                    {   InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->spatial_start >= InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->spatial_start < InfoPtr->tick_xyz[i][0])
                                InfoPtr->first_trace = InfoPtr->tick_tracenum[i-1];
                        }
                    }
                    /* find last_trace */
                    if (InfoPtr->spatial_stop >= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                    {   InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->spatial_stop > InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->spatial_stop <= InfoPtr->tick_xyz[i][0] )
                                InfoPtr->last_trace = InfoPtr->tick_tracenum[i];
                        }
					}
                }
            } else
            {   if (!hor_forward)
                {   /* find first trace */
                    if (InfoPtr->spatial_start >= InfoPtr->tick_xyz[0][0])
                    {   InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->spatial_start <= InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->spatial_start > InfoPtr->tick_xyz[i][0])
                                InfoPtr->first_trace = InfoPtr->tick_tracenum[i-1];
                        }
                    }
                    /* find last trace */
                    if (InfoPtr->spatial_stop <= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                    {   InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->spatial_stop < InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->spatial_stop >= InfoPtr->tick_xyz[i][0] )
                                InfoPtr->last_trace = InfoPtr->tick_tracenum[i];
                        }
                    }
                }
            }
            break;

        case Y_DIR:   /* y-coordinates */
            /* verify requested horizontal start and stop within data range */
            if (InfoPtr->tick_xyz[0][1] < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                InfoPtr->dat_forward = dat_forward = TRUE;
            else  InfoPtr->dat_forward = dat_forward = FALSE;
            if (InfoPtr->spatial_start == INVALID_VALUE)
                InfoPtr->spatial_start = InfoPtr->tick_xyz[0][1];
            if (InfoPtr->spatial_stop == INVALID_VALUE)
				InfoPtr->spatial_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];

            if (InfoPtr->spatial_start < InfoPtr->spatial_stop)
                hor_forward = TRUE;
            else  hor_forward = FALSE;

            if (dat_forward != hor_forward)  return 3;

            /* find start and stop traces */
            if (dat_forward)
            {   if (hor_forward)
                {   /* find first trace */
                    if (InfoPtr->spatial_start <= InfoPtr->tick_xyz[0][1])
					{   InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
					} else
					{   for (i=1; i<InfoPtr->num_ticks; i++)
						{   if (InfoPtr->spatial_start >= InfoPtr->tick_xyz[i-1][1] &&
								InfoPtr->spatial_start < InfoPtr->tick_xyz[i][1])
								InfoPtr->first_trace = InfoPtr->tick_tracenum[i-1];
						}
					}
					/* find last_trace */
					if (InfoPtr->spatial_stop >= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
					{   InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
					} else
					{   for (i=1; i<InfoPtr->num_ticks; i++)
						{   if (InfoPtr->spatial_stop > InfoPtr->tick_xyz[i-1][1] &&
								InfoPtr->spatial_stop <= InfoPtr->tick_xyz[i][1] )
								InfoPtr->last_trace = InfoPtr->tick_tracenum[i];
						}
					}
				}
			} else
			{   if (!hor_forward)
				{   /* find first trace */
					if (InfoPtr->spatial_start >= InfoPtr->tick_xyz[0][1])
					{   InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->spatial_start <= InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->spatial_start > InfoPtr->tick_xyz[i][1])
                                InfoPtr->first_trace = InfoPtr->tick_tracenum[i-1];
                        }
                    }
                    /* find last trace */
                    if (InfoPtr->spatial_stop <= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                    {   InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->spatial_stop < InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->spatial_stop >= InfoPtr->tick_xyz[i][1] )
                                InfoPtr->last_trace = InfoPtr->tick_tracenum[i];
                        }
                    }
                }
            }
            break;
    }   /* end of switch on spatial_dir block */

    /* Determine number of columns */
    InfoPtr->num_cols = InfoPtr->last_trace - InfoPtr->first_trace + 1;

    /* Allocate temporary storage and spline vectors as required */
    InfoPtr->trace_Xval = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    InfoPtr->trace_Yval = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    InfoPtr->trace_Zval = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    trace_num      = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    tick_tracenum  = (double *)malloc((size_t)InfoPtr->num_ticks * sizeof(double));
    y              = (double *)malloc((size_t)InfoPtr->num_ticks * sizeof(double));
    if ( trace_num == NULL || y == NULL || tick_tracenum == NULL ||
         InfoPtr->trace_Xval == NULL || InfoPtr->trace_Yval == NULL ||
         InfoPtr->trace_Zval == NULL )
	{   free(tick_tracenum);  free(trace_num); free(y);
        free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
        InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
        return 5;
    }

    /* Assign values for Spline() */
    for (i=0; i<InfoPtr->num_cols; i++)
        trace_num[i] = InfoPtr->first_trace + i;
    if (trace_num[InfoPtr->num_cols - 1] > InfoPtr->last_trace)
        trace_num[InfoPtr->num_cols - 1] = InfoPtr->last_trace;

    for (i=0; i<InfoPtr->num_ticks; i++)
        tick_tracenum[i] = InfoPtr->tick_tracenum[i];

    /* Spline to get X, Y, and Z values for every trace used */
    error = 0;

    for (i=0; i<InfoPtr->num_ticks; i++)
        y[i] = InfoPtr->tick_xyz[i][0];  /* X-coordinates */
    itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,
                   tick_tracenum,y,trace_num,InfoPtr->trace_Xval);
    if (itemp > 0)
    {   printf("\ntrace_Xval (X): %s\n",SplineMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
        error = 4;
    }

    for (i=0; i<InfoPtr->num_ticks; i++)
        y[i] = InfoPtr->tick_xyz[i][1];  /* Y-coordinates */
    itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,
                   tick_tracenum,y,trace_num,InfoPtr->trace_Yval);
    if (itemp > 0)
    {   printf("\ntrace_Yval (Y): %s\n",SplineMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
        error = 4;
    }

    for (i=0; i<InfoPtr->num_ticks; i++)
        y[i] = InfoPtr->tick_xyz[i][2];  /* Z-coordinates */
    itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,
                   tick_tracenum,y,trace_num,InfoPtr->trace_Zval);
    if (itemp > 0)
    {   printf("\ntrace_Zval (Y): %s\n",SplineMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
        error = 4;
    }

    /* Find min and max trace separations */
    InfoPtr->min_trace_sep =  1000.0;
    InfoPtr->max_trace_sep = -1000.0;
    switch (InfoPtr->spatial_dir)
    {   case X_DIR:
            for (i=1; i<InfoPtr->num_cols; i++)
            {   dtemp = _ABS(InfoPtr->trace_Xval[i] - InfoPtr->trace_Xval[i-1]);
                if (dtemp < InfoPtr->min_trace_sep) InfoPtr->min_trace_sep = dtemp;
                if (dtemp > InfoPtr->max_trace_sep) InfoPtr->max_trace_sep = dtemp;
            }
            break;
        case Y_DIR:
            for (i=1; i<InfoPtr->num_cols; i++)
            {   dtemp = _ABS(InfoPtr->trace_Yval[i] - InfoPtr->trace_Yval[i-1]);
                if (dtemp < InfoPtr->min_trace_sep) InfoPtr->min_trace_sep = dtemp;
                if (dtemp > InfoPtr->max_trace_sep) InfoPtr->max_trace_sep = dtemp;
            }
            break;
    }

    free(tick_tracenum);  free(trace_num);  free(y);
    return error;
}

/****************************** XsuCalcMrkLocs() *****************************/
/* Spline to get X, Y, and Z locations for center of antenna array
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <math.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h"
 * Calls: assert, malloc, free, sqrt, Spline, printf.
 * Returns: 0 on success,
 *         >0 on error (offset into message string array)
 *
const char *XsuCalcMrkLocsMsg[] =
{   "XsuCalcMrkLocs() No error.",
    "XsuCalcMrkLocs() ERROR: Number of grid columns is less than 2.",
    "XsuCalcMrkLocs() ERROR: Not able to allocate sufficient temporary storage.",
	"XsuCalcMrkLocs() ERROR: Problem splining data for mark offsets",
} ;
 *
 * Author: Jeff Lucius  USGS lucius@usgs.gov
 * Date: December 19, 2000
 */
int XsuCalcMrkLocs (struct XsuParamInfoStruct *InfoPtr)
{
    int    i,itemp;
    double *tick_travdis, *cntr_travdis, *y;
    double delx, dely, delz;
    double X1, X2, Y1, Y2, Z1, Z2;
    double xdiff, ydiff, zdiff, dist, frac;

    extern const char *Spline2Msg[]; /* in jl_util.c */
    extern FILE *log_file;
    extern int Debug;

    /* Check that arrays are allocated */
    assert(InfoPtr->tick_tracenum && InfoPtr->tick_xyz);

    /* Must have at least 2 tick marks */
    if (InfoPtr->num_ticks < 2) return 1;

    /* Allocate temporary storage */
    /* the calculated traverse distances: independent "X" values for spline */
    tick_travdis = (double *)malloc((size_t)(InfoPtr->num_ticks) * sizeof(double));

    /* the assigned dependent "Y" values (X, Y, & Z) for spline */
    y = (double *)malloc((size_t)InfoPtr->num_ticks * sizeof(double));

    /* the adjusted traverses distance: indepenedent "XNEW" values for spline */
    cntr_travdis = (double *)malloc((size_t)InfoPtr->num_ticks * sizeof(double));

    /* the splined dependent "YNEW" values - unalloc these later like the others*/
    InfoPtr->trace_Xval = (double *)malloc((size_t)(InfoPtr->num_ticks) * sizeof(double));
    InfoPtr->trace_Yval = (double *)malloc((size_t)(InfoPtr->num_ticks) * sizeof(double));
    InfoPtr->trace_Zval = (double *)malloc((size_t)(InfoPtr->num_ticks) * sizeof(double));

    if ( tick_travdis == NULL || y == NULL || cntr_travdis == NULL ||
         InfoPtr->trace_Xval == NULL || InfoPtr->trace_Yval == NULL ||
         InfoPtr->trace_Zval == NULL )
        {   free(tick_travdis); free(y); free(cntr_travdis);
            free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
            InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
            return 3;
        }

    /* Assign values for Spline() */
    /* initialize Xval, Yval, & Zval so can tell if set by Spline2()*/
    for (i=0; i<InfoPtr->num_ticks; i++)
    {   InfoPtr->trace_Xval[i] = INVALID_VALUE; /* (1.0E19) in jldefs.h */
        InfoPtr->trace_Yval[i] = INVALID_VALUE;
        InfoPtr->trace_Zval[i] = INVALID_VALUE;
    }
    /* "X" is traverse distances */
    tick_travdis[0] = 0.0;
    for (i=1; i<InfoPtr->num_ticks; i++)
    {   delx = InfoPtr->tick_xyz[i][0]- InfoPtr->tick_xyz[i-1][0];
        dely = InfoPtr->tick_xyz[i][1]- InfoPtr->tick_xyz[i-1][1];
        delz = InfoPtr->tick_xyz[i][2]- InfoPtr->tick_xyz[i-1][2];
        tick_travdis[i] = tick_travdis[i-1] + sqrt((delx*delx)+(dely*dely)+(delz*delz));
    }

    /* "XNEW" is traverse distances - mrk_offset */
    for (i=0; i<InfoPtr->num_ticks; i++)
    {   cntr_travdis[i] = tick_travdis[i] - InfoPtr->mrk_offset;
        /* Ok to have XNEW out of range of X for Spline2 */
    }

    /* Spline to get X, Y, and Z values for every trace used */
    /* int Spline2 (long n,long nnew,
                    double x[],double y[],
                    double xnew[],double ynew[]) */
    for (i=0; i<InfoPtr->num_ticks; i++)
        y[i] = InfoPtr->tick_xyz[i][0];  /* X-coordinates */
    itemp = Spline2(InfoPtr->num_ticks,InfoPtr->num_ticks,
                    tick_travdis,y,
                    cntr_travdis,InfoPtr->trace_Xval);
    if (itemp > 0)
    {   printf("\ntrace_Xval (X): %s\n",Spline2Msg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",Spline2Msg[itemp]);
       	free(tick_travdis); free(y); free(cntr_travdis);
        free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
        InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
        return 3;
    }

    for (i=0; i<InfoPtr->num_ticks; i++)
        y[i] = InfoPtr->tick_xyz[i][1];  /* Y-coordinates */
    itemp = Spline2(InfoPtr->num_ticks,InfoPtr->num_ticks,
                    tick_travdis,y,
                    cntr_travdis,InfoPtr->trace_Yval);
    if (itemp > 0)
    {   printf("\ntrace_Xval (X): %s\n",Spline2Msg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",Spline2Msg[itemp]);
       	free(tick_travdis); free(y); free(cntr_travdis);
        free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
        InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
        return 3;
    }

    for (i=0; i<InfoPtr->num_ticks; i++)
        y[i] = InfoPtr->tick_xyz[i][2];  /* Z-coordinates */
    itemp = Spline2(InfoPtr->num_ticks,InfoPtr->num_ticks,
                    tick_travdis,y,
                    cntr_travdis,InfoPtr->trace_Zval);
    if (itemp > 0)
    {   printf("\ntrace_Xval (X): %s\n",Spline2Msg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",Spline2Msg[itemp]);
      	free(tick_travdis); free(y); free(cntr_travdis);
        free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
        InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
        return 3;
    }

    /* Check for unassigned "YNEW" values and use linear interpolation */
    for (i=0; i<InfoPtr->num_ticks-1; i++)
    {   if (InfoPtr->trace_Xval[i] == INVALID_VALUE)
        {
            if (Debug)
            {   printf("unassigned at Xval[%d]=%g\n",i,InfoPtr->trace_Xval[i]);
                getch();
            }
            X1 = InfoPtr->tick_xyz[i][0];
            X2 = InfoPtr->tick_xyz[i+1][0];
            Y1 = InfoPtr->tick_xyz[i][1];
            Y2 = InfoPtr->tick_xyz[i+1][1];
            Z1 = InfoPtr->tick_xyz[i][2];
            Z2 = InfoPtr->tick_xyz[i+1][2];
            xdiff = X2 - X1;
            ydiff = Y2 - Y1;
            zdiff = Z2 - Z1;
            dist = sqrt((xdiff*xdiff)+(ydiff*ydiff)+(zdiff*zdiff));
            frac = InfoPtr->mrk_offset / dist; /* recognize sign of mrk_offset */
            InfoPtr->trace_Xval[i] = X1 - (frac * xdiff); /* neg move is pos mrk_offset */
            InfoPtr->trace_Yval[i] = Y1 - (frac * ydiff);
            InfoPtr->trace_Zval[i] = Z1 - (frac * zdiff);
        }
    }
    /* last one is special */
    if (InfoPtr->trace_Xval[InfoPtr->num_ticks-1] == INVALID_VALUE)
    {   X1 = InfoPtr->tick_xyz[InfoPtr->num_ticks-2][0];
        X2 = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];
        Y1 = InfoPtr->tick_xyz[InfoPtr->num_ticks-2][1];
        Y2 = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];
        Z1 = InfoPtr->tick_xyz[InfoPtr->num_ticks-2][2];
        Z2 = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][2];
        xdiff = X2 - X1;
        ydiff = Y2 - Y1;
        zdiff = Z2 - Z1;
        dist = sqrt((xdiff*xdiff)+(ydiff*ydiff)+(zdiff*zdiff));
        frac = InfoPtr->mrk_offset / dist; /* recognize sign of mrk_offset */
        InfoPtr->trace_Xval[InfoPtr->num_ticks-1] = X2 - (frac * xdiff); /* neg move is pos mrk_offset */
        InfoPtr->trace_Yval[InfoPtr->num_ticks-1] = Y2 - (frac * ydiff);
        InfoPtr->trace_Zval[InfoPtr->num_ticks-1] = Z2 - (frac * zdiff);
    }

    /* copy new values into old places */
    for (i=0; i<InfoPtr->num_ticks; i++)
    {   InfoPtr->tick_xyz[i][0] = InfoPtr->trace_Xval[i];
        InfoPtr->tick_xyz[i][1] = InfoPtr->trace_Yval[i];
        InfoPtr->tick_xyz[i][2] = InfoPtr->trace_Zval[i];
    }

    /* de-alloc arrays */
  	free(tick_travdis); free(y); free(cntr_travdis);
    free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
    InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
    return 0;
}

/****************************** XsuPrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>.
 * Calls: puts.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 7, 1997; December 15, 2000; January 10, 2001;
 */
void XsuPrintUsageMsg (void)
{
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("                                                                               ");
    puts("###############################################################################");
    puts("  Usage: GPR_XSU cmd_filename");
    puts("    Computer code written by Jeff Lucius  USGS  lucius@usgs.gov");
    puts("  This program transforms radar data by converting traces equally spaced in");
    puts("    time to equally spaced in location or distance.");
    puts("    The new file is saved to disk in the DZT and/or SU format.");
    puts("  Required command line arguments:");
    puts("     cmd_filename - name of ASCII command file that follows the \"CMD\" format.");
    puts("  Inspect \"GPR_XSU.CMD\" for command file format and valid parameters.");
    puts("  Example call: GPR_XSU xfile1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_XSU_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** XsuInitParams() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "GPR_XSU.h".
 * Calls: none.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 11, 1997; December 20, 2000;
 */
void XsuInitParams (struct XsuParamInfoStruct *InfoPtr)
{
    int i;

    InfoPtr->cmd_filename[0]    = 0;
    InfoPtr->dzt_infilename[0]  = 0;
    InfoPtr->mrk_infilename[0]  = 0;
    InfoPtr->xyz_infilename[0]  = 0;
    InfoPtr->dzt_outfilename[0] = 0;
    InfoPtr->su_outfilename[0]  = 0;
    InfoPtr->mrk_outfilename[0] = 0;
    InfoPtr->xyz_outfilename[0] = 0;
    InfoPtr->storage_format     = 0;
    InfoPtr->input_datatype     = 0;
    InfoPtr->file_header_bytes  = 0;
    InfoPtr->trace_header_bytes = 0;
    InfoPtr->channel            = 0;
    InfoPtr->first_proc_samp    = 2; /* DZT only otherwise use 0 */
    InfoPtr->data_samps         = 0;
    InfoPtr->total_traces       = 0;
    InfoPtr->first_trace        = -1;
    InfoPtr->last_trace         = -1;
    InfoPtr->total_time         = 0.0;
    InfoPtr->ns_per_samp        = 0.0;
    InfoPtr->num_ticks          = 0;
    InfoPtr->tick_tracenum      = NULL;
    InfoPtr->tick_xyz           = NULL;
    InfoPtr->trace_Xval         = NULL;
    InfoPtr->trace_Yval         = NULL;
    InfoPtr->trace_Zval         = NULL;
    InfoPtr->dat_forward        = 1;
    InfoPtr->ant_sep            = 0;
    InfoPtr->timezero           = 0;
    InfoPtr->mrk_offset         = 0;
    InfoPtr->spatial_dir        = X_DIR;
    InfoPtr->spatial_start      = INVALID_VALUE;
    InfoPtr->spatial_stop       = INVALID_VALUE;
    InfoPtr->spatial_step       = 0.0;
    InfoPtr->mean_value         = INVALID_VALUE;
    for (i=0; i<sizeof(InfoPtr->proc_hist); i++) InfoPtr->proc_hist[i] = 0;
    InfoPtr->min_trace_sep      = 0.0;
    InfoPtr->max_trace_sep      = 0.0;
    InfoPtr->BinArray           = NULL;
    InfoPtr->num_bins           = 0;
	InfoPtr->bin_size           = 0.0;
    InfoPtr->created            = FALSE;
    InfoPtr->grid               = NULL;
    InfoPtr->num_cols           = 0;
    InfoPtr->num_rows           = 0;
    InfoPtr->year               = -1;
    InfoPtr->month              = -1;
    InfoPtr->day                = -1;
    InfoPtr->minute             = -1;
    InfoPtr->second             = -1;
}
/***************************** XsuGetCmdFileArgs() *****************************/
/* Get processing parameters from user from the DOS command line.
 * Valid file commands:
const char *GPR_XSU_CMDS[] =
{   "debug","batch","dzt_infilename","mrk_infilename","xyz_infilename",
    "dzt_outfilename","channel","spatial_dir","spatial_start","spatial_stop",

    "spatial_step","bin_size","su_outfilename","mrk_offset","ant_sep"
    "year","month","day","hour","minute",

    "second",NULL,
} ;
 * Parameter list:
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <stdio.h>, <stdlib.h>, <string.h>, "GPR_XSU.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, strlwr (non-ANSI), strupr (non-ANSI), Trimstr, strncpy,
 *        assert.
 * Returns: 0 on success,
 *         >0 on error (offsets 2, 3, and 4 into XsuGetParamsMsg[] strings).
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 11, 1997; December 20, 2000;
 */
int XsuGetCmdFileArgs (struct XsuParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];

    int found,i,num,itemp,have_string;
    int dat_in_found      = FALSE;
    int dat_out_found     = FALSE;
    double dtemp;
    FILE *infile          = NULL;
    extern int Debug;
    extern const char *GPR_XSU_CMDS[];

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
        while (GPR_XSU_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_XSU_CMDS[i]) == 0)
            {   found = i;        /* store index into GPR_XSU_CMDS[] */
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
        {   printf("cp = %s\n",cp);
            getch();
        }
#endif
        switch (found)  /* cases depend on same order in GPR_XSU_CMDS[] */
        {   case 0:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0) Debug = FALSE;
                else            Debug = TRUE;
                break;
            case 1:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = TRUE;
                        else if (strstr(strupr(cp),"FALSE")) itemp = FALSE;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0) Batch = FALSE;
                else            Batch = TRUE;
                break;
            case 2:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dzt_infilename,cp,sizeof(InfoPtr->dzt_infilename)-1);
                strcat(InfoPtr->dzt_infilename,"\0");
                strupr(InfoPtr->dzt_infilename);
                if (InfoPtr->dzt_infilename[0]) dat_in_found = TRUE;
                if (Debug) printf("data=%s\n",InfoPtr->dzt_infilename);
                break;
            case 3:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->mrk_infilename,cp,sizeof(InfoPtr->mrk_infilename)-1);
                strcat(InfoPtr->mrk_infilename,"\0");
                strupr(InfoPtr->mrk_infilename);
                if (Debug) printf("data=%s\n",InfoPtr->mrk_infilename);
                break;
            case 4:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->xyz_infilename,cp,sizeof(InfoPtr->xyz_infilename)-1);
                strcat(InfoPtr->xyz_infilename,"\0");
                strupr(InfoPtr->xyz_infilename);
                if (Debug) printf("data=%s\n",InfoPtr->xyz_infilename);
                break;
            case 5:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dzt_outfilename,cp,sizeof(InfoPtr->dzt_outfilename)-1);
                strcat(InfoPtr->dzt_outfilename,"\0");
                strupr(InfoPtr->dzt_outfilename);
                cp2 = strstr(InfoPtr->dzt_outfilename,".");
                if (cp2 != NULL)
                {   strncpy(InfoPtr->xyz_outfilename,InfoPtr->dzt_outfilename,
                            (size_t)(cp2-InfoPtr->dzt_outfilename));
                    strcat(InfoPtr->xyz_outfilename,".XYZ\0");
                    strncpy(InfoPtr->mrk_outfilename,InfoPtr->dzt_outfilename,
                            (size_t)(cp2-InfoPtr->dzt_outfilename));
                    strcat(InfoPtr->mrk_outfilename,".MRK\0");
                }
                if (InfoPtr->dzt_outfilename[0]) dat_out_found = TRUE;
                break;
            case 6:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->channel = itemp;
                break;
            case 7:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp < NUM_SPATIAL_DIR)
                    InfoPtr->spatial_dir = itemp;
                break;
            case 8:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->spatial_start = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->spatial_start = atof(cp);
                }
                break;
            case 9:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->spatial_stop = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->spatial_stop = atof(cp);
                }
                break;
            case 10:
                dtemp = atof(cp);
				if (dtemp != 0.0) InfoPtr->spatial_step = dtemp;
                break;
            case 11:
                dtemp = atof(cp);
				if (dtemp != 0.0) InfoPtr->bin_size = dtemp;
                break;
            case 12:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->su_outfilename,cp,sizeof(InfoPtr->su_outfilename)-1);
                strcat(InfoPtr->su_outfilename,"\0");
                strupr(InfoPtr->su_outfilename);
                if (InfoPtr->su_outfilename[0]) dat_out_found = TRUE;
                break;
            case 13:
                dtemp = atof(cp);
                InfoPtr->mrk_offset = dtemp;
                break;
            case 14:
                dtemp = atof(cp);
                if (dtemp > 0.0)InfoPtr->ant_sep = dtemp;
                break;
            case 15:
                itemp = atof(cp);
                if (itemp >= 0) InfoPtr->year = itemp;
                break;
            case 16:
                itemp = atof(cp);
                if (itemp >= 0) InfoPtr->month = itemp;
                break;
            case 17:
                itemp = atof(cp);
                if (itemp >= 0) InfoPtr->day = itemp;
                break;
            case 18:
                itemp = atof(cp);
                if (itemp >= 0) InfoPtr->hour = itemp;
                break;
            case 19:
                itemp = atof(cp);
                if (itemp >= 0) InfoPtr->minute = itemp;
                break;
            default:
                break;
        }
    }
    fclose(infile);
    if (!dat_out_found)  return 3;
    if (!dat_in_found)   return 4;
    return 0;
}
/**************************** XsuDisplayParams() ***************************/
/* Display parameters to CRT.  Also calculates approximate storage requirements.
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <dos.h> "GPR_XSU.h".
 * Calls: printf, puts, _dos_getdate, _dos_gettime.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 7, 1997; December 18, 2000; January 10, 2001;
 */
void XsuDisplayParams (struct XsuParamInfoStruct *InfoPtr)
{
    char str[10];
    int lines;
    long in_grid_size;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    extern int Debug,Batch,ANSI_THERE;

    if (Debug) puts("XsuDisplayParams()");
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    /* Estimate storage rerquirements */
    in_grid_size = InfoPtr->num_cols * InfoPtr->num_rows;
    switch (InfoPtr->input_datatype)
    {   case 2: case -2: case -5:
            in_grid_size *= 2;
            break;
        case 3: case -3: case 4: case -6:
            in_grid_size *= 4;
            break;
        case 8:
            in_grid_size *= 8;
            break;
    }

    /* if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); */ /* red on black */
    if (ANSI_THERE) printf("%c%c33;40m",0x1B,0x5B); /* yellow on black */
    printf("\nGPR_XSU v. %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_XSU_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    lines += 3;
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

    printf("cmd_filename          = %s\n",strupr(InfoPtr->cmd_filename));
    lines ++;
    printf("dzt_infilename        = %s\n",strupr(InfoPtr->dzt_infilename));
    lines ++;
    printf("mrk_infilename        = %s\n",strupr(InfoPtr->mrk_infilename));
    lines ++;
    printf("xyz_infilename        = %s\n",strupr(InfoPtr->xyz_infilename));
    lines ++;
    if (InfoPtr->dzt_outfilename[0])
    {   printf("dzt_outfilename       = %s\n",strupr(InfoPtr->dzt_outfilename));
        lines ++;
        printf("mrk_outfilename       = %s\n",strupr(InfoPtr->mrk_outfilename));
        lines ++;
        printf("xyz_outfilename       = %s\n",strupr(InfoPtr->xyz_outfilename));
        lines ++;
    }
    if (InfoPtr->su_outfilename[0])
    {   printf("su_outfilename        = %s\n",strupr(InfoPtr->su_outfilename));
        lines ++;
    }
    switch (InfoPtr->storage_format)
	{   case DZT:   puts("data storage format is GSSI SIR-10 or SIR-2"); break;
		case MOD_DZT: puts("data storage format is modified GSSI DZT"); break;
        default:  puts("unrecognized data storage format");    break;
    }
    lines ++;
    switch (InfoPtr->input_datatype)
    {   case -1: puts("data element type is 8-bit unsigned integers");  break;
        case  1: puts("data element type is 8-bit signed integers");    break;
        case -2: puts("data element type is 16-bit unsigned integers"); break;
        case  2: puts("data element type is 16-bit signed integers");   break;
        case -3: puts("data element type is 32-bit unsigned integers"); break;
        case  3: puts("data element type is 32-bit signed integers");   break;
        case -5: puts("data element type is 12-bit unsigned integers"); break;
        case -6: puts("data element type is 24-bit unsigned integers"); break;
        case  4: puts("data element type is 32-bit floating point");    break;
        case  8: puts("data element type is 64-bit floating point");    break;
        default: puts("unknown data element type"); break;
    }
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

	if (InfoPtr->storage_format == DZT || InfoPtr->storage_format == MOD_DZT)
	{	printf("DZT channel = %d \n",InfoPtr->channel);
        lines ++;
    }
    printf("first_trace = %ld  last_trace = %ld  total_traces = %ld\n",
        InfoPtr->first_trace,InfoPtr->last_trace,InfoPtr->total_traces);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("data_samps  = %ld  first_proc_samp = %ld\n",
        InfoPtr->data_samps,InfoPtr->first_proc_samp);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("ns_per_samp = %g   total_time = %g (ns)\n",
        InfoPtr->ns_per_samp,InfoPtr->total_time);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("num_cols = %ld  num_rows = %ld  est. grid size = %ld bytes (%g MB)\n",
        InfoPtr->num_cols,InfoPtr->num_rows,in_grid_size,(double)in_grid_size/(1024.0*1024.0));
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("mrk_offset = %g\n",InfoPtr->mrk_offset);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("ant_sep    = %g\n",InfoPtr->ant_sep);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    if (ANSI_THERE) printf("%c%c36;40m",0x1B,0x5B); /* cyan on black */
    puts("\nTRANSFORMING PARAMETERS:");
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    switch (InfoPtr->spatial_dir)
    {   case X_DIR:
            printf("spatial_dir   = X direction\n");
            break;
        case Y_DIR:
            printf("spatial_dir   = Y direction\n");
            break;
        default:
            puts("invalid spatial_dir selected\n");
            break;
    }
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("spatial_start = %g  spatial_stop = %g  spatial_step = %g\n",
        InfoPtr->spatial_start,InfoPtr->spatial_stop,InfoPtr->spatial_step);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
	printf("bin size = %g  num bins = %ld\n",InfoPtr->bin_size,InfoPtr->num_bins);
    lines ++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("smallest trace separation = %g  largest trace separation = %g\n",
        InfoPtr->min_trace_sep,InfoPtr->max_trace_sep);

    /* Issue messages */
    if (Debug)       puts("In TESTing mode.");
    else if (Batch)  puts("In Batch processing mode.");
}
/*************************** XsuGetGprDataAsGrid() *****************************/
/* Get the data/info from the appropriate files.
 * Allocate/De-allocate storage for grid[][].
 * Verify valid parameters and ranges.
 *
 * NOTE: The "created" member of struct XsuParamInfoStruct restricts the
 *       application of the struct to one data set and one grid at a time.
 *       To read multiple data sets and allocate multiple grids, either
 *       de-allocate the first one OR declare multiple structs to hold the
 *       information and pointers.
 *
 * Method:
 *   1. Declare variables
 *   2. De-allocate storage if command==FUNC_DESTROY and return, else
 *   3. Test for balanced calls
 *   4. Allocate storage for grid[][]
 *   5. Copy disk file data into grid[][]
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <string.h>, <stdio.h>, <stdlib.h>, "GPR_XSU.h".
 * Calls: strcpy, malloc, free, printf, puts,
 *        GetDztChSubGrid8, GetDztChSubGrid16.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *XsuGetGprDataAsGridMsg[] =
{   "XsuGetGprDataAsGrid(): No errors.",
    "XsuGetGprDataAsGrid() ERROR: Attemp made to de-allocate storage before allocating.",
    "XsuGetGprDataAsGrid() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "XsuGetGprDataAsGrid() ERROR: Invalid input data element type .",
    "XsuGetGprDataAsGrid() ERROR: No recognized data/info input files specified.",

    "XsuGetGprDataAsGrid() ERROR: Not able to allocate storage for grid.",
    "XsuGetGprDataAsGrid() ERROR: Problem getting data from input file.",
} ;
 NOTE: message 5 is checked by calling routine (main).
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 6, 1996; December 12, 2000
 */
int XsuGetGprDataAsGrid (int command,struct XsuParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/
    long i,j,itemp;             /* scratch variables and counters */
    long ptr_size;              /* size of element in first dimension of grid */
    long grid_col_size;         /* number of bytes in one trace */
    long samp_bytes;            /* number of bytes in one trace sample */

/***** Deallocate storage *****/
    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->created) return 1;
        for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return 0;
    }

/***** Test for balanced calls *****/
    if (InfoPtr->created) return 2;
    InfoPtr->created = FALSE;

/***** Allocate storage for grid[][] with same type as input data sets *****/
    /* get row pointer size */
    switch (InfoPtr->input_datatype)
    {   case 1: case -1:
            ptr_size = sizeof(char *);
            samp_bytes = 1;
            break;
        case 2: case -2: case -5:
            ptr_size = sizeof(short *);
            samp_bytes = 2;
            break;
        case 3: case -3: case -6:
            ptr_size = sizeof(long *);
            samp_bytes = 4;
            break;
        case 4:
            ptr_size = sizeof(float *);
            samp_bytes = 4;
            break;
        case 8:
            ptr_size = sizeof(double *);
            samp_bytes = 8;
            break;
        default:
            return 3;
    }
    grid_col_size = InfoPtr->num_rows * samp_bytes;
    InfoPtr->grid = (void **)malloc((size_t)(InfoPtr->num_cols * ptr_size));
    if (InfoPtr->grid)
    {   for (i=0; i<InfoPtr->num_cols; i++)
        {   InfoPtr->grid[i] = (void *)malloc((size_t)grid_col_size);
            if (InfoPtr->grid[i] == NULL)
            {   for (j=0; j<i; j++) free(InfoPtr->grid[j]);
                free(InfoPtr->grid);
                InfoPtr->grid = NULL;
                return 5;
            }
        }
    } else
    {   return 5;
    }

/***** Get the data *****/
    itemp = 0;
    switch (InfoPtr->storage_format)
	{   case DZT:  case MOD_DZT:
        {   extern const char *GetDztChSubGrid8Msg[];   /* from gpr_io.h */
            extern const char *GetDztChSubGrid16Msg[];  /* from gpr_io.h */

            if (InfoPtr->input_datatype == -1)
            {   itemp = GetDztChSubGrid8(InfoPtr->dzt_infilename,InfoPtr->channel,
                        InfoPtr->first_trace,InfoPtr->num_cols,InfoPtr->num_rows,
                        (unsigned char **)(InfoPtr->grid));
                if (itemp > 0)
                {   printf("\n%s\n",GetDztChSubGrid8Msg[itemp]);
                    if (log_file) fprintf(log_file,"%s\n",GetDztChSubGrid8Msg[itemp]);
                }
            } else if (InfoPtr->input_datatype == -2)
            {   itemp = GetDztChSubGrid16(InfoPtr->dzt_infilename,InfoPtr->channel,
                        InfoPtr->first_trace,InfoPtr->num_cols,InfoPtr->num_rows,
                        (unsigned short **)(InfoPtr->grid));
                if (itemp > 0)
                {   printf("\n%s\n",GetDztChSubGrid16Msg[itemp]);
                    if (log_file) fprintf(log_file,"%s\n",GetDztChSubGrid16Msg[itemp]);
                }
            } else
            {   puts("\nSorry, only 8-bit and 16-bit GSSI data supported at this time.");
                if (log_file) fprintf(log_file,"Sorry, only 8-bit and 16-bit GSSI data supported at this time.");
                itemp = 1;
            }
            break;
        }
        default:  /* unrecognized format */
            /* deallocate storage and return error code */
            for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
            free(InfoPtr->grid);
            InfoPtr->grid = NULL;
            return 4;
    }
    if (itemp > 0)  /* problem getting data */
    {   /* deallocate storage and return error code */
        for (i=0; i<InfoPtr->num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return 6;
    } else
    {   /* flag structure and return success */
        InfoPtr->created = TRUE;
        return 0;
    }
}

/*************************** XsuXformGprData() *****************************/
/* This is the driver routine for transforming the GPR data.
 *
 * NOTES: Maximum trace length = 2 exp 15 = 32768 samples (32 K).
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *ParamInfoPtr - address of information structure
 *
 * Requires: <math.h>, <stdlib.h>, <string.h>, "jl_util1.h", "GPR_XSU.h".
 * Calls: _ABS, malloc, free, printf, puts, getch, calloc, sqrt, Spline2.
 * Returns:  0 on success, or
 *          >0 if an error has occurred.
 *
const char *XsuXformGprDataMsg[] =
{   "XsuXformGprData(): No errors.",
    "XsuXformGprData() ERROR: Invalid input data element type .",
    "XsuXformGprData() ERROR: Not able to allocate storage for bin array.",
    "XsuXformGprData() ERROR: At least one empty bin was found. See log file.",
    "XsuXformGprData() ERROR: Not able to allocate temporary storage.",

    "XsuXformGprData() ERROR: Problem splining data.  See log file.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Date: February 12, 1997; December 21, 2000;
 */
int XsuXformGprData (struct XsuParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/

    int min_hits =  10000;
    int max_hits = -10000;
    int bin_empty = FALSE;
    int itemp;
    long i,j,k,count;              /* scratch variables and counters */
    long sum_samp;                 /* offset into BinArray[].sum[] */
    long start_num;                /* offset into trace_?val[] */
    double dtemp1;

    double X1, X2, Y1, Y2, Z1, Z2;
    double xdiff, ydiff, zdiff, dist, frac;

    double sumx, sumxx, var, stdev, mean;
    double *x,*y,*xnew,*ynew;      /* for Spline2() */
    double c1,c2,xx,x1,x2,x3,x4;         /* for linear interpolation */
    double diff,diff1,diff2,diff3,diff4; /* for linear interpolation */

    extern int Debug;
    extern FILE *log_file;
    extern const char *XsuXformGprDataMsg[];
    extern const char *Spline2Msg[];

/***** Allocate/Set Bin structure *****/

    /* Allocate array of bin structures */
    InfoPtr->BinArray = (struct XfrmBin *)malloc(InfoPtr->num_bins * sizeof(struct XfrmBin));
    if (InfoPtr->BinArray == NULL) return 2;

	dtemp1 = _ABS(InfoPtr->bin_size) / 2.0;
    for (i=0; i<InfoPtr->num_bins; i++)
    {
        /* assign/initialize bin coords based on binning direction */
        switch (InfoPtr->spatial_dir)
        {   case X_DIR:

                InfoPtr->BinArray[i].x  = InfoPtr->spatial_start + (i * InfoPtr->spatial_step);
                InfoPtr->BinArray[i].y  = INVALID_VALUE;

                /* assign width limits of bin */
                if (InfoPtr->dat_forward)
                {   InfoPtr->BinArray[i].left = InfoPtr->BinArray[i].x - dtemp1;
                    InfoPtr->BinArray[i].right = InfoPtr->BinArray[i].x + dtemp1;
                } else
                {   InfoPtr->BinArray[i].left = InfoPtr->BinArray[i].x + dtemp1;
                    InfoPtr->BinArray[i].right = InfoPtr->BinArray[i].x - dtemp1;
                }
                break;
            case Y_DIR:

                InfoPtr->BinArray[i].y  = InfoPtr->spatial_start + (i * InfoPtr->spatial_step);
                InfoPtr->BinArray[i].x  = INVALID_VALUE;

                /* assign width limits of bin */
                if (InfoPtr->dat_forward)
                {   InfoPtr->BinArray[i].left = InfoPtr->BinArray[i].y - dtemp1;
                    InfoPtr->BinArray[i].right = InfoPtr->BinArray[i].y + dtemp1;
                } else
                {   InfoPtr->BinArray[i].left = InfoPtr->BinArray[i].y + dtemp1;
                    InfoPtr->BinArray[i].right = InfoPtr->BinArray[i].y - dtemp1;
                }
                break;
        }
        InfoPtr->BinArray[i].z  = INVALID_VALUE;

        /* initialize other members */
        InfoPtr->BinArray[i].col_loc   = INVALID_VALUE; /* so can check later */
        InfoPtr->BinArray[i].num_items = 0;    /* will count number of traces in bin */
        InfoPtr->BinArray[i].item_size = InfoPtr->num_rows; /* won't change */
        InfoPtr->BinArray[i].lead_X    = INVALID_VALUE;
        InfoPtr->BinArray[i].lead_Y    = INVALID_VALUE;
        InfoPtr->BinArray[i].lead_Z    = INVALID_VALUE;
        InfoPtr->BinArray[i].trail_X   = INVALID_VALUE;
        InfoPtr->BinArray[i].trail_Y   = INVALID_VALUE;
        InfoPtr->BinArray[i].trail_Z   = INVALID_VALUE;

        /* allocate storage for trace */
        InfoPtr->BinArray[i].sum =(double *)malloc(InfoPtr->BinArray[i].item_size * sizeof(double));
        if (InfoPtr->BinArray[i].sum == NULL)
        {   for (j=0; j<i; j++) free(InfoPtr->BinArray[j].sum);
            free(InfoPtr->BinArray);
            InfoPtr->BinArray = NULL;
            InfoPtr->num_bins = 0;
            return 2;
        }

        /* set sum[] values to 0.0 */
        for (j=0; j<InfoPtr->BinArray[i].item_size; j++)
            InfoPtr->BinArray[i].sum[j] = 0.0;
    }

    /* Force endpoint bin values (in case of roundoff error) */
    switch (InfoPtr->spatial_dir)
    {   case X_DIR:
            InfoPtr->BinArray[0].x = InfoPtr->spatial_start;
            InfoPtr->BinArray[InfoPtr->num_bins-1].x = InfoPtr->spatial_stop;
            break;
        case Y_DIR:
            InfoPtr->BinArray[0].y = InfoPtr->spatial_start;
            InfoPtr->BinArray[InfoPtr->num_bins-1].y = InfoPtr->spatial_stop;
            break;
    }

    /* allocate temporary storage for Splining */
    x = (double *)malloc(InfoPtr->num_cols * sizeof(double));
    if (x == NULL) return 4;
    y = (double *)malloc(InfoPtr->num_cols * sizeof(double));
    if (y == NULL)
    {   free(x);
        return 4;
    }
    xnew = (double *)malloc(InfoPtr->num_bins * sizeof(double));
    if (xnew == NULL)
    {   free(x);  free(y);
        return 4;
    }
    ynew = (double *)malloc(InfoPtr->num_bins * sizeof(double));
    if (ynew == NULL)
    {   free(x);  free(y);  free(xnew);
        return 4;
    }

    /* Locate each bin in column space to get independant variable for splining */
    switch (InfoPtr->spatial_dir)
    {   case X_DIR:
            /* first assign ->col_loc values that lie outside of original line values */
            x1 = InfoPtr->trace_Xval[0];                   /* 1st */
            x2 = InfoPtr->trace_Xval[1];                   /* 2nd */
            x3 = InfoPtr->trace_Xval[InfoPtr->num_cols-2]; /* next to last */
            x4 = InfoPtr->trace_Xval[InfoPtr->num_cols-1]; /* last */
            diff1 = x2 - x1;
            diff2 = x4 - x3;
            c1 = 0;
            c2 = InfoPtr->num_cols-2;

            for (i=0; i<InfoPtr->num_bins; i++)
            {   xx = InfoPtr->BinArray[i].x;
                if ( ((xx < x1) && InfoPtr->dat_forward) ||
                     (!InfoPtr->dat_forward && (xx > x1))
                   )
                {   diff = xx - x1;
                    InfoPtr->BinArray[i].col_loc = c1+ diff/diff1;

                }  else    if ( ((xx > x4) && InfoPtr->dat_forward) ||
                             (!InfoPtr->dat_forward && (xx < x4))
                           )
                {   diff = xx - x3;
                    InfoPtr->BinArray[i].col_loc = c2+ diff/diff2;
                }
            }
            /* next, spline to get remaining values; Spline2() allows xnew[]
               to lie out side of range of x[].  Values of ->col_loc already
               assigned should be ignored by Spline2() and can be recopied. */
            for (i=0; i<InfoPtr->num_cols; i++)
                x[i] = InfoPtr->trace_Xval[i];    /* X coord for every trace */
            for (i=0; i<InfoPtr->num_cols; i++)
                y[i] = i;                         /* trace column number */
            for (i=0; i<InfoPtr->num_bins; i++)
                xnew[i] = InfoPtr->BinArray[i].x; /* X coord for every bin */
            for (i=0; i<InfoPtr->num_bins; i++)
                ynew[i] = InfoPtr->BinArray[i].col_loc; /* bin in column space */

            itemp = Spline2(InfoPtr->num_cols,InfoPtr->num_bins,x,y,xnew,ynew);
            if (itemp > 0)
            {   if (log_file) fprintf(log_file,"1: %s\n",Spline2Msg[itemp]);
                printf("1: %s\n",Spline2Msg[itemp]);
                free(x); free(y);  free(xnew);  free(ynew);
                return 6;
            }
            for (i=0; i<InfoPtr->num_bins; i++)
                InfoPtr->BinArray[i].col_loc = ynew[i];

            /* last, check for unassigned values */
            for (i=0; i<InfoPtr->num_bins; i++)
            {   if (InfoPtr->BinArray[i].col_loc == INVALID_VALUE)
                {
printf("X_DIR: Empty bin.col_loc found at %d\n",i);
                }
            }
            break;

        case Y_DIR:
            x1 = InfoPtr->trace_Yval[0];                   /* 1st */
            x2 = InfoPtr->trace_Yval[1];                   /* 2nd */
            x3 = InfoPtr->trace_Yval[InfoPtr->num_cols-2]; /* next to last */
            x4 = InfoPtr->trace_Yval[InfoPtr->num_cols-1]; /* last */
            diff1 = x2 - x1;
            diff2 = x4 - x3;
            c1 = 0;
            c2 = InfoPtr->num_cols-2;

            for (i=0; i<InfoPtr->num_bins; i++)
            {   xx = InfoPtr->BinArray[i].y;
                if ( ((xx < x1) && InfoPtr->dat_forward) ||
                     (!InfoPtr->dat_forward && (xx > x1))
                   )
                {   diff = xx - x1;
                    InfoPtr->BinArray[i].col_loc = c1 + diff/diff1;

                }  else    if ( ((xx > x4) && InfoPtr->dat_forward) ||
                             (!InfoPtr->dat_forward && (xx < x4))
                           )
                {   diff = xx - x3;
                    InfoPtr->BinArray[i].col_loc = c2 + diff/diff2;
                }
            }
            /* next, spline to get remaining values; Spline2() allows xnew[]
               to lie out side of range of x[].  Values of ->col_loc already
               assigned should be ignored by Spline2() and can be recopied. */
            for (i=0; i<InfoPtr->num_cols; i++)
                x[i] = InfoPtr->trace_Yval[i];    /* Y coord for every trace */
            for (i=0; i<InfoPtr->num_cols; i++)
                y[i] = i;                         /* trace column number */
            for (i=0; i<InfoPtr->num_bins; i++)
                xnew[i] = InfoPtr->BinArray[i].y; /* Y coord for every bin */
            for (i=0; i<InfoPtr->num_bins; i++)
                ynew[i] = InfoPtr->BinArray[i].col_loc; /* bin in column space */

            itemp = Spline2(InfoPtr->num_cols,InfoPtr->num_bins,x,y,xnew,ynew);
            if (itemp > 0)
            {   if (log_file) fprintf(log_file,"2: %s\n",Spline2Msg[itemp]);
                printf("2: %s\n",Spline2Msg[itemp]);
                free(x); free(y);  free(xnew);  free(ynew);
                return 6;
            }

            for (i=0; i<InfoPtr->num_bins; i++)
                InfoPtr->BinArray[i].col_loc = ynew[i];

            /* last, check for unassigned values SHOULD ALL BE FULL!!! */
            for (i=0; i<InfoPtr->num_bins; i++)
            {   if (InfoPtr->BinArray[i].col_loc == INVALID_VALUE)
                {
printf("Y_DIR: Empty bin.col_loc found at %d: x=%g y=%g\n",
    i,InfoPtr->BinArray[i].x,InfoPtr->BinArray[i].y);

                }
            }
            break;
    }  /* end of switch (InfoPtr->spatial_dir) block */

    /* Get other 2 coordinates for each bin; this will be somewhat opposite
       of above procedure */
    switch (InfoPtr->spatial_dir)
    {   case X_DIR:
            /* spline to get coords along line overlap */
            for (i=0; i<InfoPtr->num_cols; i++)
                x[i] = i;                         /* trace column number */
            for (i=0; i<InfoPtr->num_cols; i++)
                y[i] = InfoPtr->trace_Yval[i];    /* Y coord for every trace */
            for (i=0; i<InfoPtr->num_bins; i++)
                xnew[i] = InfoPtr->BinArray[i].col_loc; /* bin in column space */

            itemp = Spline2(InfoPtr->num_cols,InfoPtr->num_bins,x,y,xnew,ynew);
            if (itemp > 0)
            {   if (log_file) fprintf(log_file,"3: %s\n",Spline2Msg[itemp]);
                printf("3: %s\n",Spline2Msg[itemp]);
                free(x); free(y);  free(xnew);  free(ynew);
                return 6;
            }
            for (i=0; i<InfoPtr->num_bins; i++)
                InfoPtr->BinArray[i].y = ynew[i]; /* Y coord for bins */

            /* extrapolate to get coords outside of original line */
            x1 = 0;                   /* 1st */
            x2 = 1;                   /* 2nd */
            x3 = InfoPtr->num_cols-2; /* next to last */
            x4 = InfoPtr->num_cols-1; /* last */
            diff1 = x2 - x1;
            diff2 = x4 - x3;
            diff3 = InfoPtr->trace_Yval[(int)x2] - InfoPtr->trace_Yval[(int)x1];
            diff4 = InfoPtr->trace_Yval[(int)x4] - InfoPtr->trace_Yval[(int)x3];
            c1 = InfoPtr->trace_Yval[(int)x1];
            c2 = InfoPtr->trace_Yval[(int)x3];

            for (i=0; i<InfoPtr->num_bins; i++)
            {   xx = InfoPtr->BinArray[i].col_loc;
                if (xx < x1)
                {   diff = xx - x1;
                    InfoPtr->BinArray[i].y = c1 + (diff/diff1)*diff3;

                }  else    if (xx > x4)
                {   diff = xx - x3;
                    InfoPtr->BinArray[i].y = c2 + (diff/diff2)*diff4;
                }
            }

            /* check for unassigned values SHOULD ALL BE FULL!!!`1 */
            for (i=0; i<InfoPtr->num_bins; i++)
            {   if (InfoPtr->BinArray[i].y == INVALID_VALUE)
                {
printf("X_DIR: Empty bin.y found at %d\n",i);
                }
            }

            break;

        case Y_DIR:
            /* spline to get coords along line overlap */
            for (i=0; i<InfoPtr->num_cols; i++)
                x[i] = i;                         /* trace column number */
            for (i=0; i<InfoPtr->num_cols; i++)
                y[i] = InfoPtr->trace_Xval[i];    /* X coord for every trace */
            for (i=0; i<InfoPtr->num_bins; i++)
                xnew[i] = InfoPtr->BinArray[i].col_loc; /* bin in column space */

            itemp = Spline2(InfoPtr->num_cols,InfoPtr->num_bins,x,y,xnew,ynew);
            if (itemp > 0)
            {   if (log_file) fprintf(log_file,"4: %s\n",Spline2Msg[itemp]);
                printf("4: %s\n",Spline2Msg[itemp]);
                free(x); free(y);  free(xnew);  free(ynew);
                return 6;
            }
            for (i=0; i<InfoPtr->num_bins; i++)
                InfoPtr->BinArray[i].x = ynew[i]; /* X coord for bins */

            /* extrapolate to get coords outside of original line */
            x1 = 0;                   /* 1st */
            x2 = 1;                   /* 2nd */
            x3 = InfoPtr->num_cols-2; /* next to last */
            x4 = InfoPtr->num_cols-1; /* last */
            diff1 = x2 - x1;
            diff2 = x4 - x3;
            diff3 = InfoPtr->trace_Xval[(int)x2] - InfoPtr->trace_Xval[(int)x1];
            diff4 = InfoPtr->trace_Xval[(int)x4] - InfoPtr->trace_Xval[(int)x3];
            c1 = InfoPtr->trace_Xval[(int)x1];
            c2 = InfoPtr->trace_Xval[(int)x3];

            for (i=0; i<InfoPtr->num_bins; i++)
            {   xx = InfoPtr->BinArray[i].col_loc;
                if (xx < x1)
                {   diff = xx - x1;
                    InfoPtr->BinArray[i].x = c1+ (diff/diff1)*diff3;

                }  else    if (xx > x4)
                {   diff = xx - x3;
                    InfoPtr->BinArray[i].x = c2+ (diff/diff2)*diff4;
                }
            }

            /* check for unassigned values SHOULD ALL BE FULL!!!`1 */
            for (i=0; i<InfoPtr->num_bins; i++)
            {   if (InfoPtr->BinArray[i].x == INVALID_VALUE)
                {
printf("Y_DIR: Empty bin.x found at %d\n",i);
                }
            }

            break;
    }

    /* same as above for Z coords */
    /* spline to get coords along line overlap */
    for (i=0; i<InfoPtr->num_cols; i++)
        x[i] = i;                         /* trace column number */
    for (i=0; i<InfoPtr->num_cols; i++)
        y[i] = InfoPtr->trace_Zval[i];    /* Z coord for every trace */
    for (i=0; i<InfoPtr->num_bins; i++)
        xnew[i] = InfoPtr->BinArray[i].col_loc; /* bin in column space */

    itemp = Spline2(InfoPtr->num_cols,InfoPtr->num_bins,x,y,xnew,ynew);
    if (itemp > 0)
    {   if (log_file) fprintf(log_file,"5: %s\n",Spline2Msg[itemp]);
        printf("5: %s\n",Spline2Msg[itemp]);
        free(x); free(y);  free(xnew);  free(ynew);
        return 6;
    }
    for (i=0; i<InfoPtr->num_bins; i++)
        InfoPtr->BinArray[i].z = ynew[i]; /* Z coord for bins */

    /* extrapolate to get coords outside of original line */
    x1 = 0;                   /* 1st */
    x2 = 1;                   /* 2nd */
    x3 = InfoPtr->num_cols-2; /* next to last */
    x4 = InfoPtr->num_cols-1; /* last */
    diff1 = x2 - x1;
    diff2 = x4 - x3;
    diff3 = InfoPtr->trace_Zval[(int)x2] - InfoPtr->trace_Zval[(int)x1];
    diff4 = InfoPtr->trace_Zval[(int)x4] - InfoPtr->trace_Zval[(int)x3];
    c1 = InfoPtr->trace_Zval[(int)x1];
    c2 = InfoPtr->trace_Zval[(int)x3];

    for (i=0; i<InfoPtr->num_bins; i++)
    {   xx = InfoPtr->BinArray[i].col_loc;
        if (xx < x1)
        {   diff = xx - x1;
            InfoPtr->BinArray[i].z = c1 + (diff/diff1)*diff3;

        }  else    if (xx > x4)
        {   diff = xx - x3;
            InfoPtr->BinArray[i].z = c2 + (diff/diff2)*diff4;
        }
    }

    /* check for unassigned values SHOULD ALL BE FULL!!!`1 */
    for (i=0; i<InfoPtr->num_bins; i++)
    {   if (InfoPtr->BinArray[i].z == INVALID_VALUE)
        {
printf("Z_DIR: Empty bin.z found at %d\n",i);
        }
    }

    /* Free temporary storage */
    free(x);  free(y);  free(xnew);  free(ynew);

    /* Calculate the leading and trailing antenna locations */
    for (i=0; i<InfoPtr->num_bins-1; i++)
    {   X1 = InfoPtr->BinArray[i].x;
        X2 = InfoPtr->BinArray[i+1].x;
        Y1 = InfoPtr->BinArray[i].y;
        Y2 = InfoPtr->BinArray[i+1].y;
        Z1 = InfoPtr->BinArray[i].z;
        Z2 = InfoPtr->BinArray[i+1].z;
        xdiff = X2 - X1;
        ydiff = Y2 - Y1;
        zdiff = Z2 - Z1;
        dist = sqrt((xdiff*xdiff)+(ydiff*ydiff)+(zdiff*zdiff));
        frac = (InfoPtr->ant_sep/2.0) / dist;
        InfoPtr->BinArray[i].lead_X  = X1 + (frac * xdiff); /* pos move is for leading ant */
        InfoPtr->BinArray[i].lead_Y  = Y1 + (frac * ydiff);
        InfoPtr->BinArray[i].lead_Z  = Z1 + (frac * zdiff);
        InfoPtr->BinArray[i].trail_X = X1 - (frac * xdiff); /* neg move is for trailing ant */
        InfoPtr->BinArray[i].trail_Y = Y1 - (frac * ydiff);
        InfoPtr->BinArray[i].trail_Z = Z1 - (frac * zdiff);

    }
    /* last one is special */
    X1 = InfoPtr->BinArray[InfoPtr->num_bins-2].x;
    X2 = InfoPtr->BinArray[InfoPtr->num_bins-1].x;
    Y1 = InfoPtr->BinArray[InfoPtr->num_bins-2].y;
    Y2 = InfoPtr->BinArray[InfoPtr->num_bins-1].y;
    Z1 = InfoPtr->BinArray[InfoPtr->num_bins-2].z;
    Z2 = InfoPtr->BinArray[InfoPtr->num_bins-1].z;
    xdiff = X2 - X1;
    ydiff = Y2 - Y1;
    zdiff = Z2 - Z1;
    dist = sqrt((xdiff*xdiff)+(ydiff*ydiff)+(zdiff*zdiff));
    frac = (InfoPtr->ant_sep/2.0) / dist;
    InfoPtr->BinArray[InfoPtr->num_bins-1].lead_X  = X2 + (frac * xdiff); /* pos move is for leading ant */
    InfoPtr->BinArray[InfoPtr->num_bins-1].lead_Y  = Y2 + (frac * ydiff);
    InfoPtr->BinArray[InfoPtr->num_bins-1].lead_Z  = Z2 + (frac * zdiff);
    InfoPtr->BinArray[InfoPtr->num_bins-1].trail_X = X2 - (frac * xdiff); /* neg move is for trailing ant */
    InfoPtr->BinArray[InfoPtr->num_bins-1].trail_Y = Y2 - (frac * ydiff);
    InfoPtr->BinArray[InfoPtr->num_bins-1].trail_Z = Z2 - (frac * zdiff);

#if 0
i = InfoPtr->num_bins-1;
printf("bin #%d: x=%g y=%g z=%g lt=%g rt=%g\n     X1=%g X2=%g Y1=%g Y2=%g Z1=%g Z2=%g\n     lX=%g lY=%g lZ=%g\n     tX=%g tY=%g tZ=%g\n",
 i,InfoPtr->BinArray[i].x,InfoPtr->BinArray[i].y,InfoPtr->BinArray[i].z,
 InfoPtr->BinArray[i].left,InfoPtr->BinArray[i].right,
 X1,X2,Y1,Y2,Z1,Z2,
 InfoPtr->BinArray[i].lead_X,InfoPtr->BinArray[i].lead_Y,InfoPtr->BinArray[i].lead_Z,
 InfoPtr->BinArray[i].trail_X,InfoPtr->BinArray[i].trail_Y,InfoPtr->BinArray[i].trail_Z
 );
getch();
#endif

    if (Debug)
    {   puts("\nAfter initialization:");
        for (i=0; i<InfoPtr->num_bins; i++)
        {  printf("bin #%d: x=%g y=%g z=%g lt=%g rt=%g lX=%g lY=%g lZ=%g tX=%g tY=%g tZ=%g items=%ld size=%ld sum[0]=%g\n",
            i,InfoPtr->BinArray[i].x,InfoPtr->BinArray[i].y,InfoPtr->BinArray[i].z,
            InfoPtr->BinArray[i].left,InfoPtr->BinArray[i].right,
            InfoPtr->BinArray[i].lead_X,InfoPtr->BinArray[i].lead_Y,InfoPtr->BinArray[i].lead_Z,
            InfoPtr->BinArray[i].trail_X,InfoPtr->BinArray[i].trail_Y,InfoPtr->BinArray[i].trail_Z,
            InfoPtr->BinArray[i].num_items,InfoPtr->BinArray[i].item_size,
            InfoPtr->BinArray[i].sum[0]);
        }
        getch();
    }

/***** Place data in bins *****/

    start_num = 0;  /* offset into trace_?val[] to start search each time */
    count = InfoPtr->num_bins/10;
    if (count < 1) count = 1;
    /*  i        = current bin number
        j        = current trace_?val and input grid trace number
        k        = current sample in current trace in input grid
        sum_samp = current sample in bin sum trace
     */
    for (i=0; i<InfoPtr->num_bins; i++) /* for each bin */
    {   if (!(i % count)) printf(".");
        for (j=start_num; j<InfoPtr->num_cols; j++) /* search through trace_Xval[] */
        {
            /* does trace fall within boundaries of this bin? */
            if ( ( (InfoPtr->spatial_dir == X_DIR) && (InfoPtr->dat_forward) &&
                   (InfoPtr->BinArray[i].left < InfoPtr->trace_Xval[j]) &&
                   (InfoPtr->BinArray[i].right >= InfoPtr->trace_Xval[j])
                 )
                  ||
                 ( (InfoPtr->spatial_dir == X_DIR) && (!InfoPtr->dat_forward) &&
                   (InfoPtr->BinArray[i].left > InfoPtr->trace_Xval[j]) &&
                   (InfoPtr->BinArray[i].right <= InfoPtr->trace_Xval[j])
                 )
                  ||
                 ( (InfoPtr->spatial_dir == Y_DIR) && (InfoPtr->dat_forward) &&
                   (InfoPtr->BinArray[i].left < InfoPtr->trace_Yval[j]) &&
                   (InfoPtr->BinArray[i].right >= InfoPtr->trace_Yval[j])
                 )
                  ||
                 ( (InfoPtr->spatial_dir == Y_DIR) && (!InfoPtr->dat_forward) &&
                   (InfoPtr->BinArray[i].left > InfoPtr->trace_Yval[j]) &&
                   (InfoPtr->BinArray[i].right <= InfoPtr->trace_Yval[j])
                 )
               )
            {   /* yes, so increment total */
                InfoPtr->BinArray[i].num_items++;

                /* assign trace header; sum data */
                sum_samp = 0;
                switch (InfoPtr->input_datatype)
                {   case -1:
                    {   unsigned char **ucp = (unsigned char **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = ucp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += ucp[j][k];
                        break;
                    }
                    case 1:
                    {   char **cp = (char **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = cp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += cp[j][k];
                        break;
                    }
                    case -2:   case -5:
                    {   unsigned short **usp = (unsigned short **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = usp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += usp[j][k];
                        break;
                    }
                    case 2:
                    {   short **sp = (short **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = sp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += sp[j][k];
                        break;
                    }
                    case -3:  case -6:
                    {   unsigned long **ulp = (unsigned long **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = ulp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += ulp[j][k];
                        break;
                    }
                    case 3:
                    {   long **lp = (long **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = lp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += lp[j][k];
                        break;
                    }
                    case 4:
                    {   float **fp = (float **)InfoPtr->grid;

						for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = fp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += fp[j][k];
                        break;
                    }
                    case 8:
                    {   double **dp = (double **)InfoPtr->grid;

                        for (k=0; k<InfoPtr->first_proc_samp; k++, sum_samp++)
                                InfoPtr->BinArray[i].sum[sum_samp] = dp[j][k];
                        for (k = InfoPtr->first_proc_samp;
                             k < InfoPtr->num_rows, sum_samp<InfoPtr->BinArray[i].item_size;
                             k++, sum_samp++)
                            InfoPtr->BinArray[i].sum[sum_samp] += dp[j][k];
                        break;
                    }
                } /* end of switch (InfoPtr->input_datatype) block */
            }
            /* check to see if past end of bin */
            if ( ( (InfoPtr->spatial_dir == X_DIR) && InfoPtr->dat_forward &&
                   (InfoPtr->trace_Xval[j] > InfoPtr->BinArray[i].right)
                 )
                  ||
                 ( (InfoPtr->spatial_dir == X_DIR) && !InfoPtr->dat_forward &&
                   (InfoPtr->trace_Xval[j] < InfoPtr->BinArray[i].right)
                 )
                  ||
                 ( (InfoPtr->spatial_dir == Y_DIR) && InfoPtr->dat_forward &&
                   (InfoPtr->trace_Yval[j] > InfoPtr->BinArray[i].right)
                 )
                  ||
                 ( (InfoPtr->spatial_dir == Y_DIR) && !InfoPtr->dat_forward &&
                   (InfoPtr->trace_Yval[j] < InfoPtr->BinArray[i].right)
                 )
               )
			{   if (InfoPtr->bin_size == InfoPtr->spatial_step)
                    start_num = j;  /* pick up where we left off */
                else
                    start_num = 0;  /* go back to start */

                break; /* out of for loop working through trace_Xval */
            }
        }  /* end of for (j=start_num; j<InfoPtr->num_cols; j++) */

    } /* end of for (i=0; i<InfoPtr->num_bins; i++) */

    /* Calculate average trace for bin or normalize empty bin trace */
    for (i=0; i<InfoPtr->num_bins; i++) /* for each bin */
    {    if (InfoPtr->BinArray[i].num_items > 0)
        {   for (j=InfoPtr->first_proc_samp; j<InfoPtr->BinArray[i].item_size; j++)
                InfoPtr->BinArray[i].sum[j] /= InfoPtr->BinArray[i].num_items;
        } else
        {   for (j=0; j<InfoPtr->BinArray[i].item_size; j++)
                InfoPtr->BinArray[i].sum[j] = InfoPtr->mean_value;
            bin_empty = TRUE;
        }
    }

    if (Debug)
    {   puts("\nAfter filling bins:");
        for (i=0; i<InfoPtr->num_bins; i++)
        {  printf("bin #%d: x=%g left=%g right=%g num_items=%ld item_size=%ld sum[0]=%g\n",
            i,InfoPtr->BinArray[i].x,InfoPtr->BinArray[i].left,InfoPtr->BinArray[i].right,
            InfoPtr->BinArray[i].num_items,InfoPtr->BinArray[i].item_size,
            InfoPtr->BinArray[i].sum[0]);
        }
        getch();
    }

    /* report statistics */
    printf("\n\nXsuXformGprData() Report:\nTrace separation: minimum = %g   maximum = %g\n",
            InfoPtr->min_trace_sep,InfoPtr->max_trace_sep);

    if (bin_empty)
    {   printf("Empty bin number(s):");
        if (log_file) fprintf(log_file,"Empty bin number(s):");
        for (i=0; i<InfoPtr->num_bins; i++)
        {   if (InfoPtr->BinArray[i].num_items == 0)
            {   printf(" %ld",i);
                if (log_file) fprintf(log_file," %ld",i);
            }
        }
        if (log_file) fprintf(log_file,"To avoid empty bins, increase bin size or spatial step.\n");
        puts("");
    } else
        puts("There are no empty bins");

    sumx = sumxx = 0.0;
    for (i=0; i<InfoPtr->num_bins; i++)
    {   if (InfoPtr->BinArray[i].num_items < min_hits)
            min_hits = InfoPtr->BinArray[i].num_items;
        if (InfoPtr->BinArray[i].num_items > max_hits)
            max_hits = InfoPtr->BinArray[i].num_items;

        sumx  += InfoPtr->BinArray[i].num_items;
        sumxx += InfoPtr->BinArray[i].num_items * InfoPtr->BinArray[i].num_items;
    }
    var = ((InfoPtr->num_bins*sumxx) - (sumx*sumx)) /
          (InfoPtr->num_bins*(InfoPtr->num_bins-1));
    stdev = sqrt(var);
    mean = sumx/InfoPtr->num_bins;
    printf("Bin items: miniumum hits = %d  maximum hits = %d\n",min_hits,max_hits);
    printf("           standard dev. = %g  mean = %g\n",stdev, mean);

    if (log_file)
    {   fprintf(log_file,"Trace separation: minimum = %g   maximum = %g\n",
                InfoPtr->min_trace_sep,InfoPtr->max_trace_sep);
        fprintf(log_file,"There are no empty bins\n");
        fprintf(log_file,"Bin items: miniumum hits = %d  maximum hits = %d\n",
                min_hits,max_hits);
        fprintf(log_file,"           standard dev. = %g  mean = %g\n",stdev, mean);
    }

    return 0;
}

/************************** XsuSaveGprData() *******************************/
/* This is the driver routine for saving the GPR data to disk.
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <time.h>, <dos.h> "GPR_XSU.h".
 * Calls: printf, fprintf, sprintf, strcat strlen,_dos_getdate,_dos_gettime,
 *        XsuSaveDztData, XsuSaveDt1Data, XsuSaveSgyData, XsuSaveUsrData.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *XsuSaveGprDataMsg[] =
{   "XsuSaveGprData(): No errors.",
    "XsuSaveGprData() ERROR: Invalid/unknown file storage format.",
    "XsuSaveGprData() ERROR: Data/information was not saved to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: February 6, 1997; December 14, 2000;
 */
int XsuSaveGprData (struct XsuParamInfoStruct *InfoPtr)
{
    char str[240];
    int i,j,itemp;
    int file_saved = FALSE;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    extern FILE *log_file;               /* beginning of this source */
    extern const char *XsuSaveDztDataMsg[]; /* beginning of this source */
    extern const char *XsuSaveDt1DataMsg[]; /* beginning of this source */
    extern const char *XsuSaveSgyDataMsg[]; /* beginning of this source */
    extern const char *XsuSaveUsrDataMsg[]; /* beginning of this source */

    /* Fill processing history string */
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
    sprintf(InfoPtr->proc_hist,"\nGPR_XSU v. %s (USGS-JL) on %s %d, %d at %d:%02d\n",
            GPR_XSU_VER,month[dosdate.month],dosdate.day,dosdate.year,
            dostime.hour,dostime.minute);
    sprintf(str,"GPR_XSU Parameters: start=%g stop=%g step=%g bin_size=%g\n",
            InfoPtr->spatial_start,InfoPtr->spatial_stop,
            InfoPtr->spatial_step,InfoPtr->bin_size);
    strcat(InfoPtr->proc_hist,str);

    /* Copy output traces from BinArray to input grid */
    for (i=0; i<InfoPtr->num_bins; i++)
    {   for (j=0; j<InfoPtr->num_rows; j++)
        {   switch (InfoPtr->input_datatype)
            {   case -1:
                {   unsigned char **ucp = (unsigned char **)InfoPtr->grid;

                    ucp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case 1:
                {   char **cp = (char **)InfoPtr->grid;

                    cp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case -2:   case -5:
                {   unsigned short **usp = (unsigned short **)InfoPtr->grid;

                    usp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case 2:
                {   short **sp = (short **)InfoPtr->grid;

                    sp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case -3:  case -6:
                {   unsigned long **ulp = (unsigned long **)InfoPtr->grid;

                    ulp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case 3:
                {   long **lp = (long **)InfoPtr->grid;

                    lp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case 4:
                {   float **fp = (float **)InfoPtr->grid;

                    fp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
                case 8:
                {   double **dp = (double **)InfoPtr->grid;

                    dp[i][j] = InfoPtr->BinArray[i].sum[j];
                    break;
                }
            }
        }
    }

/***** NOTE: ->grid now contains the output traces *****/

    if (InfoPtr->dzt_outfilename[0])
    {   itemp = XsuSaveDztData(InfoPtr);
        if (itemp > 0)
        {   printf("\n%s\n",XsuSaveDztDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",XsuSaveDztDataMsg[itemp]);
        } else
            file_saved = TRUE;
    }

    if (InfoPtr->su_outfilename[0])
    {   itemp = XsuSaveSuData(InfoPtr);
        if (itemp > 0)
        {   printf("\n%s\n",XsuSaveSuDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",XsuSaveSuDataMsg[itemp]);
        } else
            file_saved = TRUE;
    }

    if (file_saved) return 0;
    else            return 2;
}
/******************************* XsuSaveDztData() ******************************/
/* This function saves the GPR data to disk using the DZT format.
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_xsu.h".
 * Calls: SetDzt5xHeader, ReadOneDztHeader, SaveDztFile, _splitpath (non-ANSI),
 *        strcpy, strncpy, strcat, sprintf, puts, printf, strlen.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *XsuSaveDztDataMsg[] =
{   "XsuSaveDztData(): No errors.",
	"XsuSaveDztData() ERROR: Invalid datatype for output DZT file.",
    "XsuSaveDztData() ERROR: Problem getting input file header.",
    "XsuSaveDztData() ERROR: Problem saving data.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 7, 1997
 */
int XsuSaveDztData (struct XsuParamInfoStruct *InfoPtr)
{
    int i,itemp;
    char name[12],antname[14], drive[_MAX_DRIVE], dir[_MAX_DIR], text[400];
    char fname[_MAX_FNAME], fext[_MAX_EXT];
    int max_proc_hist_size = 400;
    unsigned char proc_hist[400];  /* hopefully 400 is big enough! */
    int hdr_num,num_hdrs,header_size,rh_dtype;
    unsigned short rh_nchan,rg_breaks,rh_nproc;
    float rg_values[8];    /* this limit may change in future versions */
    long num_traces;
    struct DztHdrStruct hdr;              /* struct in gpr_io.h */
    struct DztHdrStruct *hdrPtrArray[1];
    struct DztDateStruct create_date;
    extern const char *SaveDztFileMsg[];  /* in gpr_io.h, dzt_io.c */
    extern FILE *log_file;                /* beginning of this source */

    switch (InfoPtr->input_datatype)
    {   case -1:  case -2:    break;
        default:              return 1;
    }

	/* Read input DZT header again */
	itemp = ReadOneDztHeader(InfoPtr->dzt_infilename,&num_hdrs,
							 &num_traces,InfoPtr->channel,&header_size,&hdr);
	if (itemp > 0)
	{   if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
		return 2;
	}

	/* Set/copy values for output header */
	if (hdr.rh_dtype == 0 && InfoPtr->input_datatype != -1)
	{   if (InfoPtr->input_datatype == -2) rh_dtype = 0x0002;
		else if (InfoPtr->input_datatype == -3) rh_dtype = 0x0004;
		else rh_dtype = 0x0000;
	}
	rg_breaks = *(unsigned short *) ((char *)&hdr + hdr.rh_rgain);
	for (i=0; i<rg_breaks && i<8; i++)
		rg_values[i] = *(float *) ((char *)&hdr + hdr.rh_rgain + 2 + 4*i);
	strncpy(text,(char *)&hdr + hdr.rh_text,sizeof(text)-1);
	if (strlen(text) < sizeof(text)-strlen(InfoPtr->proc_hist)-2)
		strcat(text,InfoPtr->proc_hist);
	rh_nproc = hdr.rh_nproc;
	if (rh_nproc > max_proc_hist_size-1) rh_nproc = max_proc_hist_size-1;
	for (i=0; i<rh_nproc; i++)
		proc_hist[i] = *((char *)&hdr + hdr.rh_proc + i);
	proc_hist[i] = 0;  /* counts on i being past for-loop limits */
	hdr_num = 1;
	rh_nchan = 1;
	_splitpath(InfoPtr->dzt_outfilename,drive,dir,fname,fext);
	strcpy(name,fname);
	strncpy(antname,hdr.rh_antname,sizeof(antname)-1);
	antname[sizeof(antname)-1] = 0;
	create_date = hdr.rh_create;
	hdr.rh_spm = 1.0 / InfoPtr->bin_size;
	hdr.rh_mpm = InfoPtr->bin_size;
	hdr.rh_sps = 0.0;
	/* Note: Do not send pointers from hdr through this function!
			 SetDzt5xHeader() clears structure arrays before writing to
			 them! Other members are OK as they are sent by value.
	 */
	SetDzt5xHeader(hdr_num,hdr.rh_nsamp,hdr.rh_bits,hdr.rh_zero,hdr.rh_sps,
				 hdr.rh_spm,hdr.rh_mpm,hdr.rh_position,hdr.rh_range,
				 hdr.rh_npass,rh_nchan,hdr.rh_epsr,hdr.rh_top,hdr.rh_depth,
				 rh_dtype,antname,name,text,rg_breaks,rg_values,rh_nproc,
				 proc_hist,&create_date,&hdr);

	/* Set "trace header" values */
	switch (InfoPtr->input_datatype)
	{   case -1:
		{   unsigned char **ucp = (unsigned char **)InfoPtr->grid;

			for (i=0; i<InfoPtr->num_bins; i++)
			{   ucp[i][0] = 0xFFU;
				ucp[i][1] = 0xE8U;
			}
			break;
		}
		case -2:
		{   unsigned short **usp = (unsigned short **)InfoPtr->grid;

			for (i=0; i<InfoPtr->num_bins; i++)
			{   usp[i][0] = 0xFFFFU;
				usp[i][1] = 0xE800U;
			}
			break;
		}
		case -3:
        {   unsigned long **ulp = (unsigned long **)InfoPtr->grid;

            for (i=0; i<InfoPtr->num_bins; i++)
            {   ulp[i][0] = 0xFFFFFFFFUL;
                ulp[i][1] = 0xE8000000UL;
            }
            break;
        }
    }

    /* Save to disk */
    hdrPtrArray[0] = &hdr;
    itemp = SaveDztFile(InfoPtr->dzt_outfilename,InfoPtr->num_bins,
                InfoPtr->num_rows,(int)hdr.rh_bits,hdr_num,
                hdrPtrArray,InfoPtr->grid);
    if (itemp > 0)
    {   if (log_file) fprintf(log_file,"%s\n",SaveDztFileMsg[itemp]);
        return 3;
    }

    return 0;
}

#if 1
/****************************** XsuSaveSuData() ******************************/
/* This function saves the GPR data to disk using the DT1 format.
 *
int XsuSaveSuData (struct XsuParamInfoStruct *InfoPtr)
{ return 0;
}
 * Parameter list:
 *  struct XsuParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, <string.h>, <dos.h>, "GPR_XSU.h".
 * Calls: memset, fprintf, fopen, fclose, strcat, realloc, ferror, fwrite,
 *        clearerr, SetSuTraceHeader, _dos_getdate, _dos_gettime.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *XsuSaveSuDataMsg[] =
{   "XsuSaveDt1Data(): No errors.",
    "XsuSaveDt1Data() ERROR: Unable to allocate temporary storage.",
    "XsuSaveDt1Data() ERROR: Unable to open new SU file on disk.",
    "XsuSaveDt1Data() ERROR: Problem saving SU file to disk.",
} ;
 *
 * Author: Jeff Lucius   USGS lucius@usgs.gov
 * Revisions: January 23, 2001;
 */
int XsuSaveSuData (struct XsuParamInfoStruct *InfoPtr)
{
    int    itemp, i, day, trace, samp;
    float *fp;
    unsigned char **ucp;
    unsigned short **usp;
    long   trace_size, ltemp;
    struct SuTraceHdrStruct hdr;
    FILE *outfile;
    #if defined(_INTELC32_)
      char *outbuffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
    #endif
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    int days[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 } ;

    extern int Debug;                           /* beginning of this source */
    extern FILE *log_file;                      /* beginning of this source */

    /* Allocate storage for one trace (signed short ints, 2 bytes) */
    fp = (float *)malloc(InfoPtr->num_rows * sizeof(float));
    if (fp == NULL) return 1;

    /* Open output SU file */
    if ((outfile = fopen(InfoPtr->su_outfilename,"wb")) == NULL)  return 2;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(outbuffer,vbufsize);
      if (outbuffer)  setvbuf(outfile,outbuffer,_IOFBF,vbufsize);
      else            setvbuf(outfile,NULL,_IOFBF,vbufsize);
    #endif

    /* Set the date and time */
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
    if (InfoPtr->year < 0)
    {   InfoPtr->year = dosdate.year;
    }
    if (InfoPtr->month < 0)
    {   InfoPtr->month = dosdate.month;
    }
    if (InfoPtr->day < 0)
    {   InfoPtr->day = dosdate.day;
    }
    if (InfoPtr->hour < 0)
    {   InfoPtr->hour = dostime.hour;
    }
    if (InfoPtr->minute < 0)
    {   InfoPtr->minute = dostime.minute;
    }
    if (InfoPtr->second < 0)
    {   InfoPtr->second = dostime.second;
    }
    if (InfoPtr->month > 0 && InfoPtr->day > 0)
    {   day = 0;
        day += days[InfoPtr->month];
        if (InfoPtr->month > 2 && !(InfoPtr->year)%4) day++; /* leap year */
        day += InfoPtr->day;
    }

    /* Set trace header values */
    for (i=0; i<InfoPtr->num_bins; i++)
    {   SetSuTraceHeader2((long)i,InfoPtr->ant_sep,InfoPtr->trace_Zval[i],
            InfoPtr->BinArray[i].lead_X,InfoPtr->BinArray[i].lead_Y,
            InfoPtr->BinArray[i].trail_X,InfoPtr->BinArray[i].trail_Y,
            (long)InfoPtr->timezero,(long)InfoPtr->num_rows,(long)InfoPtr->ns_per_samp,
            (long)InfoPtr->year,(long)day,(long)InfoPtr->hour,
            (long)InfoPtr->minute,(long)InfoPtr->second,&hdr);
    }

    /* Save data to disk */
    itemp = 0;
    trace_size = sizeof(float) * InfoPtr->num_rows;
    for (trace = 0; trace < InfoPtr->num_bins; trace++)
    {
        SetSuTraceHeader2((long)trace,InfoPtr->ant_sep,InfoPtr->trace_Zval[trace],
            InfoPtr->BinArray[trace].lead_X,InfoPtr->BinArray[trace].lead_Y,
            InfoPtr->BinArray[trace].trail_X,InfoPtr->BinArray[trace].trail_Y,
            (long)InfoPtr->timezero,(long)InfoPtr->num_rows,(long)InfoPtr->ns_per_samp,
            (long)InfoPtr->year,(long)day,(long)InfoPtr->hour,
            (long)InfoPtr->minute,(long)InfoPtr->second,&hdr);

        /* write trace header */
        fwrite(&hdr,sizeof(struct SuTraceHdrStruct),(size_t)1,outfile);
        /* convert DZT unsigned values to 4-byte floats */
       	switch (InfoPtr->input_datatype)
   	    {   case -1:
            {   ucp = (unsigned char **)InfoPtr->grid;
                for (samp=0; samp<InfoPtr->num_rows; samp++)
                {   ltemp = ucp[trace][samp];
                    ltemp -= 128;
                    fp[samp] = _LIMIT(-128,ltemp,127);
                }
                break;
            }
        	case -2:
	        {   usp = (unsigned short **)InfoPtr->grid;
                for (samp=0; samp<InfoPtr->num_rows; samp++)
                {   ltemp = usp[trace][samp];
                    ltemp -= 32768;
                    fp[samp] = _LIMIT(-32768,ltemp,32767);
                }
   			    break;
            }
            default:
                printf("\aERROR! input_datatype not selected for SU output!\n");
                getch();
        }

        /* write trace data */
        fwrite(fp,(size_t)trace_size,(size_t)1,outfile);
    }
    if (ferror(outfile))
    {   clearerr(outfile);
        itemp = 3;
    }
    #if defined(_INTELC32_)
      realfree(outbuffer);
    #endif
    fclose(outfile);
    return itemp;
}
#endif
/************************* XsuSaveXyzMrkData() *****************************/
/* This is functio saves the bin XYZ info to disk.
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *ParamInfoPtr - address of structure
 *
 * Requires: <stdio.h>, "gpr_xsu.h".
 * Calls: fopen, fclose, fprintf.
 * Returns:  0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *XfrmSaveXyzDataMsg[] =
{   "XfrmSaveXyzData(): No errors.",
    "XfrmSaveXyzData() ERROR: Unable to open output XYZ file.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 11, 1997; December 20, 2000;
 */
int XsuSaveXyzMrkData (struct XsuParamInfoStruct *InfoPtr)
{
    int i;
    FILE *outfile;

    outfile = fopen(InfoPtr->xyz_outfilename,"wt");
    if (outfile == NULL) return 1;
    fprintf(outfile,"%d   ; GPR_XSU.EXE: trace-X Y Z trailing-X Y Z leading-X Y Z\n",InfoPtr->num_bins);
    for (i=0; i<InfoPtr->num_bins; i++)
    {   fprintf(outfile,"%.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n",
                InfoPtr->BinArray[i].x,InfoPtr->BinArray[i].y,InfoPtr->BinArray[i].z,
                InfoPtr->BinArray[i].trail_X,InfoPtr->BinArray[i].trail_Y,InfoPtr->BinArray[i].trail_Z,
                InfoPtr->BinArray[i].lead_X,InfoPtr->BinArray[i].lead_Y,InfoPtr->BinArray[i].lead_Z
               );
    }
    fclose(outfile);

    outfile = fopen(InfoPtr->mrk_outfilename,"wt");
    if (outfile == NULL) return 1;
    fprintf(outfile,"%d   ; File written by GPR_XSU.EXE\n",InfoPtr->num_bins);
    for (i=0; i<InfoPtr->num_bins; i++)
        fprintf(outfile,"%6d\n",i);

    fclose(outfile);
    return 0;
}
/*************************** XsuDeallocInfoStruct() ***************************/
/* Deallocate the buffer space in the information structure.  This function
 * will work correctly only if the num_... variables accurately reflect
 * the successful allocation of storage.
 *
 * Parameter list:
 *  struct XsuParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_xsu.h"
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Golden, CO
 * Date: February 10, 1997; December 19, 2000;
 */
void XsuDeallocInfoStruct (struct XsuParamInfoStruct *InfoPtr)
{
    int i;
    extern int Debug;

    if (InfoPtr->num_ticks > 0)
    {   if (InfoPtr->tick_tracenum)
        {   free(InfoPtr->tick_tracenum);
            InfoPtr->tick_tracenum = NULL;
        }
        if (InfoPtr->tick_xyz)
        {   for (i=0; i<InfoPtr->num_ticks; i++)  free(InfoPtr->tick_xyz[i]);
            free(InfoPtr->tick_xyz);
            InfoPtr->tick_xyz = NULL;
        }
        InfoPtr->num_ticks = 0;
    }
    if (InfoPtr->BinArray != NULL)
    {   for (i=0; i<InfoPtr->num_bins; i++)
        {   free(InfoPtr->BinArray[i].sum);
        }
        free(InfoPtr->BinArray);
        InfoPtr->BinArray = NULL;
        InfoPtr->num_bins = 0;
    }
    if (InfoPtr->trace_Xval != NULL)
    {   free(InfoPtr->trace_Xval);
        InfoPtr->trace_Xval = NULL;
    }
    if (InfoPtr->trace_Yval != NULL)
    {   free(InfoPtr->trace_Yval);
        InfoPtr->trace_Yval = NULL;
    }
    if (InfoPtr->trace_Zval != NULL)
    {   free(InfoPtr->trace_Zval);
        InfoPtr->trace_Zval = NULL;
    }
}
#if 1
/**************************** SetSuTraceHeader() ****************************/
/* Set the fields in the SEG-Y trace header.
 *
 * Parameters:
 *  long trace_num      - trace sequence number within line, indexed from 1
 *  double ant_sep      - meters between antennas
 *  double elev         - elevation at trace in meters
 *  double Tx_x         - transmitter X position in meters
 *  double Tx_y         - transmitter Y position in meters
 *  double Rc_x         - receiver X position in meters
 *  double Rc_y         - receiver Y position in meters
 *  long timezero       - timezero in nanoseconds
 *  long num_samps      - number of samples in this trace
 *  long sample_int     - sample rate in nanoseconds
 *  long year           - year data recorded/modified/created; 0 - 3000
 *  long day            - day of year; 1 - 366
 *  long hour           - hour of day; 0 - 23
 *  long minute         - minute of hour; 0 - 59
 *  long second         - second of minute; 0 - 59
 *  struct SuTraceHdrStruct *HdrPtr - address of allocated trace header struct
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <string.h>, "gpr_io.h".
 * Calls: memset.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: December 21, 2000
 */
void SetSuTraceHeader2 (long trace_num,double ant_sep,double elev,
        double Tx_x,double Tx_y,double Rc_x,double Rc_y,
        long timezero,long num_samps,long sample_int,
        long year, long day, long hour, long minute, long second,
        struct SuTraceHdrStruct *HdrPtr)
{
    /* Initialize all fields to 0 */
    memset((void *)HdrPtr,0,sizeof(struct SuTraceHdrStruct));

    /* Set relavent fields */
    HdrPtr->tracl  = trace_num+1;      /* trace sequence number within line */
    HdrPtr->tracr  = trace_num+1;      /* trace sequence number within reel */
    HdrPtr->fldr   = trace_num+1;      /* field record number */
    HdrPtr->tracf  = 1;                /* trace number within field record */
    HdrPtr->cdp    = 0;             /* should this be something else? */
    HdrPtr->cdpt   = 0;
    HdrPtr->trid   = 200;              /* trace ID code 200=USGS GPR*/
	HdrPtr->nvs    = 1;                /* pulseEKKO standard */
    HdrPtr->nhs    = 1;                /* pulseEKKO standard */
    HdrPtr->duse   = 1;                /* pulseEKKO standard */
    HdrPtr->offset = ant_sep * 1000;   /* truncation expected */
    HdrPtr->gelev  = elev * 1000;      /* truncation expected */
    HdrPtr->selev  = elev * 1000;      /* truncation expected */
    HdrPtr->gdel   = elev * 1000;      /* truncation expected */
    HdrPtr->sdel   = elev * 1000;      /* truncation expected */
    HdrPtr->scalel = -1000;            /* divide values by 1000 to get original */
    HdrPtr->scalco = -1000;            /* divide values by 1000 to get original */
    HdrPtr->sx     = Tx_x * 1000;      /* truncation expected*/
    HdrPtr->sy     = Tx_y * 1000;      /* truncation expected */
    HdrPtr->gx     = Rc_x * 1000;      /* truncation expected */
    HdrPtr->gy     = Rc_y * 1000;      /* truncation expected */
    HdrPtr->counit = 1;                /* coordinate units code: length */
    HdrPtr->laga   = timezero * 1000;  /* timezero */
    HdrPtr->ns     = num_samps;        /* number of samples in this trace */
    HdrPtr->dt     = sample_int * 1000;/* sample interval */
    HdrPtr->gain   = 0;                /* gain type code: fixed=1 */
    HdrPtr->year   = _LIMIT(0,year,4000);
    HdrPtr->day    = _LIMIT(0,day,366);
    HdrPtr->hour   = _LIMIT(0,hour,23);
    HdrPtr->minute = _LIMIT(0,minute,59);
    HdrPtr->sec    = _LIMIT(0,second,59);
    HdrPtr->timbas = 1;                /* time basis: local */
    strcpy((char *)HdrPtr->unass,"GPR_XSU lucius@usgs.gov"); /* 28 bytes available */
}
#endif
/**************************** end of GPR_XSU.C *****************************/
