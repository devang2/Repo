/******************************** GPRSLICE.C *********************************\
                                 version 1.10.03.01
-----------------------------------------------------------------------------
INTRODUCTION

GPRSLICE.EXE generates a volume of transformed ground penetrating radar (GPR)
data from a series of GPR profiles. The program GPR_PROC should be used
before calling GPRSLICE if the data need to be filtered or otherwise
processed. GPRSLICE can remove the "background" (the average trace) from each
profile and it can expand the dynamic range of the volume data to the maximum
allowed by the data storage type (96 decibels).

The purpose of generating a volume of GPR data is to extract information from 
the radar survey that is not as easily seen in the individual profiles. One 
way to do this is to enhance features that are correlated or somehow related 
to each other. This process is commonly called "stacking" in geophysical data 
processing. Stacking is achieved by summing all the data of interest and 
dividing by the number of data values. For time series, such as GPR traces, 
the same sample number from each series is summed and the result divided by 
the number of series. This creates a new ("average") time series in which 
correlated features are enhanced and random ones are reduced. 

The formation of a volume of transformed GPR data is somewhat more complex in 
that traces are not preserved. The procedure used by GPRSLICE is as follows. 
Each GPR file is read in individually. A background trace can be removed at 
this time. The GPR traces that fall within a search window around a volume 
cell are stacked together. The amplitudes of this stacked trace are then 
transformed (converted to absolute values or instantaneous amplitudes for 
example). All sample values that fall within a layer (Z direction) of the 
volume are averaged and then assigned to the volume cell for that layer. 
After all files have been read, the final volume cell value is calculated by 
averaging all values assigned to it.

The volume consists of 16-bit, unsigned integers (range 0 to 65535). The 
volume can be sliced perpendicular to one of the three coordinate axes. The 
most common slices would be horizontal layers perpendicular to the Z-axis. 
Slices from the volume are then written to disk in any, or all, of three 
formats described in the Output Files section.

The input to GPRSLICE.EXE is a "CMD" file, an ASCII text file containing 
keywords (or commands) which are discussed below.

The GPR data can be read from disk using the following formats only:
  - GSSI SIR-10A ver 4.x or 5.x binary "DZT" files (1 or more 512- or 1024-
    byte headers followed by 1 or more channels of data, trace by trace),
  NOTE: DZT files should have only ONE channel. Multiple-channel files will
        not be sliced correctly.
  - Sensors and Software PulseEKKO "DT1" and "HD" files, or
  - Society of Exploration Geophysicists SEG-Y files that follow the Sensors
    and Software style for formatting the reel (file) header.

A message file called GPRSLICE.LOG is opened when the program starts. It is
located either in the directory where the program was called from or in the
root directory of drive C. This open file may prevent more than one session
of the program from executing in the same directory. The log file may contain 
more information regarding the failure or success of GPRSLICE as it executes. 
Sessions are appended at the end of the log file.

GPRSLICE.EXE is a protected-mode MS-DOS program that executes within the DOS 
operating system or within DOS (or console) windows in Microsoft Windows 
3.x/95/98. It will probably not execute within a console window in Windows 
NT. This program will force the computer CPU to work in "protected mode", 
that is, all memory can be utilized, not just the 1 MB that DOS normally has 
access to. If an error message occurs when trying to run GPRSLICE.EXE it may 
be because the requested amount of "protected mode" memory is set too low for 
the program or is more than what is available on the computer. Use the 
program MODXCONF.EXE, which should have been distributed with this software, 
to adjust the region size.

NOTE: Files used by this program must follow the naming conventions required 
by early versions of MS-DOS. Filenames can be only eight (8) characters long,
with no embedded spaces, with an optional extension of up to three
characters. This program will run in a Windows 95/98 console where filenames 
do not have to follow this convention. Unexpected results may occur if input 
file names, including the CMD file, are longer than 8 characters. For 
example, both command files slicegrd.cmd and slicegrd1.cmd will be seen as 
slicegrd.cmd.

-----------------------------------------------------------------------------
GENERATING THE VOLUME

To generate the volume you specify start and stop locations and the number of
sections in all three dimensions, X, Y, and Z. These define the volume cells, 
or compartments, that the data will be assigned to. Sections along the X-axis 
are called columns. Along the Y-axis, sections are called rows.  Layers are 
sections along the Z-axis. The average of the data within the cell is 
assigned to the center of the cell. If, for example, you want to define 3
layers between 0 and 30 ns, Z_first would be 0, Z_last would be 30, and
Z_layers would be 3. Each layer would be 10 ns "thick" (0-10, 10-20, 20-30 
ns).

The location of the middle of the cell is used for its coordinate location 
(such as in the text file used for input to SURFER). For example, if 2
columns are defined for the X direction between X_first = 0 and X_last = 2,
then the cell locations are recorded at 0.5 and 1.5 meters.

There must be at least one slice (one section in the axis direction that the
slice is perpendicular to) and two sections in each of the other two
directions. For example, for horizontal slices (perpendicular to the Z-axis) 
there must be one section in the Z direction and at least two sections in
each of the X and Y directions. This program generates planes of data, not 
points or lines.

A search box, which is centered on the middle of each volume cell, is used to 
determine which data will be assigned to a volume cell. By default, the 
search box is the same size and location as the cell. The user has the option 
to change the size of the search box, either smaller or larger to accommodate 
dense or sparse data sets. The search box size essentially modifies the size 
of the volume cell to make them overlap or to separate them by space/time. 
For example, for a horizontal slice between 10 and 20 ns, the default search 
box size would be 10 ns, centered on the middle of the layer at 15 ns. Values 
between 10 and 20 ns would be used. If the search box size in the Z direction 
was assigned to 20 ns, then values between 5 and 25 ns would be used (15 plus 
and minus 10 ns). This feature is required when the length of volume sections 
along the X or Y directions are less than the distance between GPR profiles. 
Likewise, the search box size can be restricted to include only one profile 
or to include adjacent profiles when the section length is close to the 
profile spacing.

The location of the GPR data can have any orientation with respect to the 
volume. Parallel profiles are not required. Storage formats can be different.
The time window, sample per trace, sample rate, and antenna frequency can
vary from file to file. Other programs such as GPR_REV, GPR_STAK, GPR_CNDS, 
GPR_JOIN, GPR_XFRM, and GPR_SAMP can be used but are not required to provide 
the corrected positioning of traces or to change the number of samples or 
traces. GPR data outside of the search boxes are ignored.

X, Y, and Z values for each trace in a profile are determined from the 
(required) "MRK" and "XYZ" files. MRK files contain the location of "marked" 
traces in each file, that is the traces for which the spatial locations are
known. XYZ files contain the spatial locations that correspond to each marked
trace. The Z values are not used as the sample "locations" are travel times 
offsets (in ns) from the start time (see keyword below).

Each GPR file is read in sequence, and the amplitudes of the trace samples 
are assigned to search boxes. The traces within each search box are stacked.
The stacked trace values are converted to absolute values, squares, or 
instantaneous amplitudes. In addition, an envelope can be draped across the 
positive peaks of the transformed trace. Empty search boxes are set to zero
or a median value (32768).  Empty search boxes can be avoided by enlarging
the cell size and/or the box size. The fastest way to generate slices will be
using absolute values. Using instantaneous amplitudes (calculated from the
analytic signal determined from the Hilbert transform of the trace) will take
the longest time to generate slices.

-----------------------------------------------------------------------------
OUTPUT FILES

Each slice from the volume is written to disk as a separate file with
numerically increasing filenames. The user supplies a template (up to six
characters long) and the program appends numbers (01, 02, 03, ..., 11, 12,
etc.). There are three storage formats to choose from. Any or all can be
chosen. The user can also select the output directory.

An information file is written, with "INF" filename extension, which
summarizes the manipulations of GPRSLICE and the output filenames and
locations.

"TXT" files are text files that list the two spatial coordinates and the
amplitude value for each station in the slice. In text files only, the
amplitudes of the volume are reduced by a factor of 8 to range from 0 to 8191
(2^13 - 1). Text files can be used in spreadsheet programs or as input data
for SURFER.

"T3D" files write only the amplitude values to a binary file. Amplitudes are
8 bit unsigned (0-255). These binary files can be input directly into GPR_DISP
or into other applications such as SLICER/T3D.
Values are written as follows:
  for (layer=0; layer<num_layers; layer++)
    for (row=num_rows-1; row>=0; row--)
      for (col=num_cols-1; col>=0; col--)

"SLD" files write only the amplitude values to a binary file. Amplitudes are
16 bit unsigned (0-65535). These binary files can be input directly into
GPR_DISP or into other applications such as SLICER/T3D.
Values are written as follows:
  for (layer=num_layers-1; layer>=0; layer--)
    for (row=0; row<num_rows; row++)
      for (col=0; col<num_cols; col++)

"PCX" files are graphics files containing the amplitudes stored in the
compressed PCX format. The PCX graphics files can be input into any graphics
application (word processors, draw programs such as Corel Draw, and paint
programs such as Corel PhotoPaint) or sequentially into SLICER/T3D.
SLICER/T3D forms a new data volume that can be viewed in interesting ways.
SURFER grids individual slices and displays them separately.

The volume slice in each of the output files is organized depending on the
direction the slice is taken and the storage format. The first slice is
written to the first file. Slices are written upside down for PCX files,
which have the origin at the upper left of the CRT screen. Layers correspond
to increments in the Z direction; rows to increments in the Y direction; and
columns to increments in the X direction. For horizontal slices
(perpendicular to the Z-axis) information is written row by row starting near
the origin. For vertical slices information is written layer by layer. Each
slice has a constant value for its distance from the coordinate origin that
increments uniformly along the axis that the slice is perpendicular to. For
example, vertical slices perpendicular to the Y-axis will have a constant Y-
value for all the information in a file.
-----------------------------------------------------------------------------
WRITTEN BY:
Jeff Lucius
U.S. Geological Survey
Box 25046  Denver Federal Center  MS 964
Denver, CO  80225-0046
voice: 303-236-1413
fax:   303-236-1425
email: lucius@usgs.gov
-----------------------------------------------------------------------------
Revision History:
June 25, 1999      - Start modification of GPR_VOL and GPR_XFRM into this
                     program GPRSLICE.
June 30, 1999      - Eliminate writing volume; only slices to disk.
                   - Only traces to be used from GPR file are read.
                   - Standard MRK and XYZ files are required for ALL GPR data.
                   - Amplitude transforms include: original, absolute value,
                     squared, cubed, and instantaneous amplitude. An
                     envelope connecting positive peaks of any transform can be
                     used instead.
                   - Time zero can be specified to apply to all files.
July 16, 1999      - Fixed problem with calculating instantaneous amp. -
                     was not forcing temporary array length to power of 2
                     for FFT1D and had wrong counter installed for checking
                     middle of array for applying Hilbert filter.
July 20, 1999      - Fixed problem in writing TXT data files - had row and
                     column loop controllers reversed.
                   - Modified SaveGprSliceInfo() to calc and write min, max,
                     and average values of all Z.
                   - Modified SaveGprSliceData() to divide Z values by 8 for
                     text files.
July 21, 1999      - Reversed order or writing rows to PCX file - now start
                     at last row.
                   - Finish documentation.
                   - Rename "bins" to "search boxes".
July 23, 1999      - Corrected display and usage of overwrite. Changed name
                     to "overwrite_protect" and set TRUE to protect.
                   - Allowed for one slice in any selected direction.
                   - Reversed Z start and stop so trace bottom always down.
July 26, 1999      - Removed option for user to specify "other" formats.
July 27, 1999      - Added option to expand range of data to fill entire
                     range of data type (0-65535) at end.
                   - Removed weighting choices - can only average.
                   - Added option to remove background first from file.
                   - Revised to stack traces first in window and only apply
                     transform to stacked trace.
                   - Removed velocity option.
July 28, 1999      - Stopped reversing Z_start/stop and now have first
                     layer at top of volume (Z_start) and write files that way.
                   - Changed may true/false keywords are handled so that
                     independant of init value (old way assumed init value
                     was false!).
July 29, 1999      - Added init value for Vol to be 0 for envelope option.
                   - Removed cube transform option.
                   - Repaired transform methods to cover limits of ushort.
                   - Changed inst amp method to sub mean from trace before
                     FFT and add back after IFFT (need to keep relative
                     shape for getting summ of squares).
                   - Fixed create directory code (wasn't there before!).
July 30, 1999      - Changed way user inputs volume station locations. Now
                     tells me the edges of volume and number of blocks in
                     each direction. I then convert to my old system.
                   - Volume max and min are not correct.
                   - PCX spectrum no longer has black and white end points.
                   - Fixed how X and Y slices written to disk.
August 3, 1999     - Replaced directory code with CreateDir() from jl_util.
August 5, 1999     - Fixed problem in calculating instantaneous amplitude.
                   - Added line checking to DisplyParameters() to pause.
August 6, 1999     - Fixed problems with start_time (only positive used)
                     limit check before file read in.
                   - Now switch back to cwd before every exit.
                   - Now allow user to supply alpha string instead of
                     numeric for "slice_direction" and "xfrm_method".
                   - Improved DisplayParameters().
August 9, 1999     - Now clear keboard buffer at start of DisplayParameters().
August 30 ,1999    - Fixed mistake in GetGprDataAsGridU16() - missing
                     brackets in for loop for character data was causing
                     heap block overwritten messages at runtime.
September 2, 1999  - Added instantaneous power (xfrm_method 4).
September 14, 1999 - Improved documentation file.
                   - Added "NONE" for xfrm_method option 0.
                   - Added switch back to original directory after user ESC
                     to end after displaying parameters.
                   - Replace code to switch directories from using system() to
                     using _chdrive() etc.
October 14, 1999   - Add ferror() to check for disk I/O errors in
                     SaveGprSliceData(). Full disk errors are NOT reported by
                     fprintf as errors.
December 21, 1999  - Add HistStretchImage16u() option to stretch values before
                     envelope() or div by 8 for txt file.
                   - Updated version to 1.2.
December 22, 1999  - Remove histogram stretching because inapropriate - there
                     are not enough counts and data are sparse - lots of zeroes
                     between counts.
                   - Added "multiply" keyword so user can increase values.
July 27, 2000      - Add option and function to save slices as T3D volume.
                   - Changed way version done - added data to number.
                   - Changed check for over-write, added [0].
                   - Changed way pcx file values calced. Now don't change
                     Volumne[][][] directly; assign to ustemp first.
August 2, 2000     - Added message to INF file - t3d files are 0 - 255.
September 29, 2000 - Fixed docs for Z-first, Z_last in DOC file.
                   - Fixed problem in SplineXyzData(); error was not being
                     intialized before going through "linear" branch.
                   - Added check to see if requested Volume is larger than what
                     RAM can hold and tells user.
March 26, 2001     - Corrected spelling in usage message and added some docs.
October 3, 2001    - Add output for SlicerDicer.

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /zfloatsync GPRSLICE.C gpr_io.lib jl_util1.lib pcx_io.lib
     To compile for 80486 executable:
       icc /F /xnovm /zfloatsync /zmod486 GPRSLICE.C gpr_io.lib jl_util1.lib
           pcx_io.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions, speeds up program.
     gpr_io.lib   - Jeff Lucius's GPR I/O functions (DZT, DT1, SGY).
     jl_util1.lib - Jeff Lucius's utility functions.
     pcx_io.lib   - Jeff Lucius's PCX storage format I/O functions.

     to remove assert() functions from code also add:
     /DNDEBUG

 To run: GPRSLICE cmd_filename
\****************************************************************************/

/*************************** Function dependency ****************************/
/******************* Function dependency and resource usage *****************/
/*
 * con I/O  - console I/O - keyboard and/or text-mode screen used
 * disk I/O - reads from and/or writes to disk
 * DOS      - DOS/BIOS interrupt performed
 * graphics - graphics modes required, direct writes to ports/video adaptor
 * port I/O - hardware I/O ports used
 * RAM      - direct access of CPU RAM memory
 *
 * main
 *   +-- ANSI_there                                             (DOS)
 *   |     +-- GetDosVersion                                    (DOS)
 *   +-- Sound                                                       (port I/O)
 *   +-- GetParameters                                 (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line and opening the GPR data/info files to get
 *   |     | additional information about the data sets.
 *   |     +-- PrintUsageMsg                           (con I/O)
 *   |     +-- InitParameters
 *   |     +-- GetCmdFileArgs                (disk I/O)
 *   |     |     +-- TrimStr
 *   +-- DisplayParameters                             (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- GenerateSlices                         (disk I/O)(con I/O)
 *   |     +-- GetGprFileType                (disk I/O)
 *   |     +-- GetOtherInfo                  (disk I/O)
 *   |     +-- GetMrkData                    (disk I/O)
 *   |     +-- GetXyzData                    (disk I/O)
 *   |     +-- SplineXyzData
 *   |     |     +-- Spline
 *   |     +-- GetGprDataAsGridU16           (disk I/O)(con I/O)
 *   |     |     This function retrieves the data from disk and stores in
 *   |     |     a grid as unsigned 16-bit integers.
 *   |     +-- RemAvgTrace16u
 *   |     +-- InstAttribTrace
 *   |     +-- HistStretchImage16u
 *   |     +-- Envelope
 *   |           +-- Spline
 *   +-- SaveGprSliceData                     (disk I/O)
 *   +-- SaveGprSliceInfo                     (disk I/O)
 *   +-- SaveGprSliceVolume                   (disk I/O)
 *   +-- DeallocInfoStruct
 *         +-- GetGprDataAsGridU16
 */

/******************* Includes all required header files *********************/
#include "gprslice.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The next array is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPRSLICE_CMDS[] =
{   "debug","batch","num_input_files","input_filelist[]","txt_outfilename",
    "pcx_outfilename","overwrite_protect","xfrm_method","multiply","x_first",

    "x_last","x_columns","y_first","y_last","y_rows",
    "z_first","z_last","z_layers","box_xsize","box_ysize",

    "box_zsize","background","bin_outfilename","envelope","slice_direction",
    "start_time","out_directory","inf_outfilename","expand","t3d_outfilename",

    "sld_outfilename",NULL,
} ;

const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR:  Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",

    "GetParameters() ERROR: X: Inappropriate values for first, last, and columns.",
    "GetParameters() ERROR: X: There must be at least two columns.",
    "GetParameters() ERROR: Y: Inappropriate values for first, last, and rows.",
    "GetParameters() ERROR: Y: There must be at least two rows.",
    "GetParameters() ERROR: Z: Inappropriate values for first, last, and layers.",

    "GetParameters() ERROR: Z: There must be at least two layers.",
    "GetParameters() ERROR: search box X size must be > 0",
    "GetParameters() ERROR: search box Y size must be > 0",
    "GetParameters() ERROR: search box Z size must be > 0",
    "GetParameters() ERROR: unable to open a data file from the input list.",

    "GetParameters() ERROR: unable to open a XYZ file from the input list.",
    "GetParameters() ERROR: unable to open a MRK file from the input list.",
    "GetParameters() ERROR: there must be at least one input file.",
} ;

const char *GenerateSlicesMsg[] =
{   "GenerateSlices(): No errors.",
    "GenerateSlices() ERROR: Not able to allocate storage for volume.",
    "GenerateSlices() ERROR: Not able to allocate storage for volume cell info.",
    "GenerateSlices() ERROR: Problem getting GPR file type and essential info.",
    "GenerateSlices() ERROR: GPR file format not recognized.",

    "GenerateSlices() ERROR: Insufficient or invalid info for user-defined format.",
    "GenerateSlices() ERROR: Problem getting XYZ data.",
    "GenerateSlices() ERROR: Number of XYZ records != to number of marked traces.",
    "GenerateSlices() ERROR: Problem getting GPR data.",
    "GenerateSlices() ERROR: Problem getting MRK data.",

    "GenerateSlices() ERROR: Problem Splining data.",
    "GenerateSlices() ERROR: Not able to allocate storage for temporary trace arrays.",
    "GenerateSlices() ERROR: Problem generating envelope over trace.",
  } ;

const char *GetGprDataAsGridU16Msg[] =
{   "GetGprDataAsGridU16(): No errors.",
    "GetGprDataAsGridU16() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetGprDataAsGridU16() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "GetGprDataAsGridU16() ERROR: Invalid input data element type .",
    "GetGprDataAsGridU16() ERROR: No recognized data/info input files specified.",

    "GetGprDataAsGridU16() ERROR: Not able to allocate storage for grid.",
    "GetGprDataAsGridU16() ERROR: Trace header size must be integer multiple of sample bytes.",
    "GetGprDataAsGridU16() ERROR: Not able to allocate storage for one trace.",
    "GetGprDataAsGridU16() ERROR: Unable to open input data file.",
    "GetGprDataAsGridU16() ERROR: fread() of data file returned an error.",

    "GetGprDataAsGridU16() ERROR: End of File found.",
    "GetGprDataAsGridU16() ERROR: Floating point types not supported.",
    "GetGprDataAsGridU16() ERROR: Number of traces read less than number of grid columns.",
} ;

const char *SaveGprSliceDataMsg[] =
{   "SaveGprSliceData(): No errors.",
    "SaveGprSliceData() ERROR: Unable to open output TXT file.",
    "SaveGprSliceData() ERROR: Unable to open output BIN file.",
    "SaveGprSliceData() ERROR: Unable to open output PCX file.",
    "SaveGprSliceData() ERROR: Problem saving TXT data.",


    "SaveGprSliceData() ERROR: Problem saving BIN data.",
    "SaveGprSliceData() ERROR: Problem saving PCX header.",
    "SaveGprSliceData() ERROR: Problem allocating buffer for row of PCX data.",
    "SaveGprSliceData() ERROR: Problem saving PCX data.",
    "SaveGprSliceData() ERROR: Problem saving PCX palette.",

    "SaveGprSliceData() ERROR: DISK I/O error (may be full).",
    "SaveGprSliceData() ERROR: Unable to open output T3D file.",
    "SaveGprSliceData() ERROR: Problem saving T3D data.",
} ;
const char *SaveGprSliceInfoMsg[] =
{   "SaveGprSliceInfo(): No errors.",
    "SaveGprSliceInfo() ERROR: Unable to open output TXT file.",
    "SaveGprSliceData() ERROR: Problem saving TXT data.",
} ;

const char *SplineXyzDataMsg[] =
{   "SplineXyzData() No error.",
    "SplineXyzData() ERROR: Number of marked traces < 2.",
    "SplineXyzData() ERROR: Marked trace numbers not in proper order.",
    "SplineXyzData() ERROR: Marked trace numbers outside of data trace numbers.",
    "SplineXyzData() ERROR: Not able to allocate sufficient storage.",

    "SplineXyzData() ERROR: Problem splining X topography data.",
    "SplineXyzData() ERROR: Problem splining Y topography data.",
    "SplineXyzData() ERROR: Problem splining Z topography data.",
} ;

/********************************** main() **********************************/
void main (int argc,char *argv[])
{
/***** Declare variables *****/
    /* following are scratch variables used by main */
    char log_filename[80];
    char *cp, drive[80],str[80];
    int    itemp,driveno;
    unsigned long membytes,Volbytes,Cellbytes;
    double dtemp;

    /* following is the information structure passed between functions */
    struct SliceParamInfoStruct ParamInfo;

    /* these variables found near beginning of this source */
    extern int Debug,Batch,ANSI_THERE;
    extern FILE *log_file;
    extern const char *GetParametersMsg[];
    extern const char *GetGprDataAsGridU16Msg[];
    extern const char *GenerateSlicesMsg[];

/***** Initialize variables *****/
    /* Open error message file */
    strcpy(log_filename,"GPRSLICE.LOG");
    log_file = fopen(log_filename,"a+t");  /* open text file for appending */
    if (log_file == NULL)
    {   strcpy(log_filename,"C:\\GPRSLICE.LOG");
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
        fprintf(log_file,"Messages from program GPRSLICE v. %s: %s %d, %d at %d:%02d:%02d\n",
                        GPRSLICE_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute,dostime.second);
        fprintf(log_file,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.\n");
    }

    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* Zero-out information structure */
    memset((void *)&ParamInfo,0x00,sizeof(struct SliceParamInfoStruct));

/***** Get information used to process data *****/
    if (log_file && argv[1])
    {   fprintf(log_file,"Processing command file: %s\n",argv[1]);
    }
    printf("Getting user parameters ...");
    itemp =  GetParameters(argc,argv,&ParamInfo);
    printf("\r                                          \r");
    if (itemp > 0)
    {   printf("\n%s\n",GetParametersMsg[itemp]);
        if (log_file)
        {   fprintf(log_file,"%s\n",GetParametersMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
        }
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("\nGPRSLICE finished.\nFile %s on disk contains messages.\n",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);

        /* Switch back to original directory */
        strcpy(str,ParamInfo.cwd);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.cwd);

        exit(1);
    }


    if (log_file)
    {   if (Batch) fprintf(log_file,"Program in Batch mode.\n");
        if (Debug) fprintf(log_file,"Program in Debug mode.\n");
    }

/***** Display parameters and ask if user wants to continue *****/

    if (Debug) printf("before DisplayParameters()\n");
    DisplayParameters(&ParamInfo);

    /* Estimate storage requirements */
    Volbytes = sizeof(unsigned short) * ParamInfo.vol_num_cols * ParamInfo.vol_num_rows * ParamInfo.vol_num_layers;
    Cellbytes = sizeof(struct VolCellStruct) * ParamInfo.vol_num_cols * ParamInfo.vol_num_rows * ParamInfo.vol_num_layers;
    membytes = _memavl();

    printf("\nYour computer has                    %6.2f MB available.\n",
        (membytes/1024.0)/1024.0);
    printf("The GPR Volume requires              %6.2f MB.\n",
        (Volbytes/1024.0)/1024.0);
    printf("The Volume Cell information requires %6.2f MB.\n",
        (Cellbytes/1024.0)/1024.0);
    if ((Volbytes + Cellbytes) > membytes)
    {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
            Sound(dtemp,0.055);
        if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); /* red on black */
        puts("\nThe program was stopped becuase there is not enough memory on your computer.");
        puts("Reduce one or more of X_columns, Y_rows, Z_layers and try again.\n");

        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B); /* green on black */
        if (log_file)
        {   fprintf(log_file,"Your computer has %6.2f MB available.\n",(membytes/1024.0)/1024.0);
            fprintf(log_file,"\nThe GPR Volume requires  = %6.2f MB.\n",(Volbytes/1024.0)/1024.0);
            fprintf(log_file,"The Volume cell information requires %6.2f MB.\n",(Cellbytes/1024.0)/1024.0);
            fprintf(log_file,"End of messages.  Program terminated because not enough RAM.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("GPRSLICE finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

        /* Switch back to original directory */
        strcpy(str,ParamInfo.cwd);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.cwd);

        exit(2);
    }

    if (!Batch)
    {   printf("\nPress <Esc> to quit or other key to continue ... ");
        if ((itemp = getch()) == ESC)
        {   puts("\nProgram terminated by user.");
            if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
            if (log_file)
            {   fprintf(log_file,"End of messages.  Program terminated by user.\n");
                fclose(log_file);
                printf("File %s on disk contains messages.\n",log_filename);
            }
            puts("GPRSLICE finished.");
            if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);

            /* Switch back to original directory */
            strcpy(str,ParamInfo.cwd);
            cp = strchr(str,':');
            if (cp != NULL)
            {   *cp = 0;
                strupr(str);
                driveno = 1 + str[0] - 'A';
                _chdrive(driveno);
            }
            chdir(ParamInfo.cwd);

            exit(3);
        } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
            getch();
        printf("\r                                                                    \r");
    }

    /* Overwrite protection */
    if (ParamInfo.overwrite_protect)
    {   strcpy(str,ParamInfo.out_directory);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.out_directory);
        if ((ParamInfo.inf_outfilename[0] && access(ParamInfo.inf_outfilename,0x00) == 0) ||
            (ParamInfo.pcx_outfilename[0] && access(ParamInfo.pcx_outfilename,0x00) == 0) ||
            (ParamInfo.bin_outfilename[0] && access(ParamInfo.bin_outfilename,0x00) == 0) ||
            (ParamInfo.t3d_outfilename[0] && access(ParamInfo.t3d_outfilename,0x00) == 0) ||
            (ParamInfo.sld_outfilename[0] && access(ParamInfo.sld_outfilename,0x00) == 0) ||
            (ParamInfo.txt_outfilename[0] && access(ParamInfo.txt_outfilename,0x00) == 0)
           )
        {   if (ANSI_THERE) printf("%c%c1;31;40m",0x1B,0x5B); /* red on black */
            printf("\nERROR: Output filename(s) exist but \"overwrite_protect\" is set to TRUE.\n");
            printf("Directory searched was %s\n",getcwd(str,80));
            /* if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B); */ /* green on black */
            /* if (ANSI_THERE) printf("%c%c36;40m",0x1B,0x5B); */ /* cyan on black */
            /* if (ANSI_THERE) printf("%c%c,33;40m",0x1B,0x5B); */ /* yellow on black */
            if (ANSI_THERE) printf("%c%c0;35;40m",0x1B,0x5B); /* magenta on black */
            if (log_file)
            {   fprintf(log_file,"ERROR: Output filename(s) exist but \"overwrite_protect\" is set to TRUE.\n");
                fprintf(log_file,"ERROR: Directory searched was %s\n",getcwd(str,80));
                fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                fclose(log_file);
                printf("File %s on disk contains messages.\n",log_filename);
            }
            puts("GPRSLICE finished.");
            if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

            /* Switch back to original directory */
            strcpy(str,ParamInfo.cwd);
            cp = strchr(str,':');
            if (cp != NULL)
            {   *cp = 0;
                strupr(str);
                driveno = 1 + str[0] - 'A';
                _chdrive(driveno);
            }
            chdir(ParamInfo.cwd);

            exit(2);
        }

        /* Switch back to original directory */
        strcpy(str,ParamInfo.cwd);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.cwd);
    }

/***** Generate the Slices *****/
    printf("Generating Slices:\n");
    itemp = GenerateSlices(&ParamInfo);
    printf("\r                                                              \r");
    if (itemp > 0)
    {   printf("\n%s\n",GenerateSlicesMsg[itemp]);
        DeallocInfoStruct(&ParamInfo);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"%s\n",GenerateSlicesMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPRSLICE finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }

        /* Switch back to original directory */
        strcpy(str,ParamInfo.cwd);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.cwd);

        exit(5);
    }

/***** Save Slices to disk *****/
    printf("Saving slices to disk ...");
    itemp = SaveGprSliceData(&ParamInfo);
    printf("\r                                                              \r");
    if (itemp > 0)
    {   DeallocInfoStruct(&ParamInfo);
        printf("\n%s\n",SaveGprSliceDataMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"%s\n",SaveGprSliceDataMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPRSLICE finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }

        /* Switch back to original directory */
        strcpy(str,ParamInfo.cwd);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.cwd);

        exit(6);
    }

/***** Save Information file to disk *****/
    printf("Saving slice info to disk ...");
    itemp = SaveGprSliceInfo(&ParamInfo);
    printf("\r                                   \r");
    if (itemp > 0)
    {   DeallocInfoStruct(&ParamInfo);
        printf("\n%s\n",SaveGprSliceInfoMsg[itemp]);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        if (log_file)
        {   fprintf(log_file,"%s\n",SaveGprSliceInfoMsg[itemp]);
            fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
            fclose(log_file);
            printf("File %s on disk contains messages.\n",log_filename);
        }
        puts("\nGPRSLICE finished.");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        if (!Batch)
        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                Sound(dtemp,0.055);
        }

        /* Switch back to original directory */
        strcpy(str,ParamInfo.cwd);
        cp = strchr(str,':');
        if (cp != NULL)
        {   *cp = 0;
            strupr(str);
            driveno = 1 + str[0] - 'A';
            _chdrive(driveno);
        }
        chdir(ParamInfo.cwd);

        exit(6);
    }

/***** Free storage and terminate program *****/
    DeallocInfoStruct(&ParamInfo);
    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
    if (ParamInfo.num_empty_cells > 0)
    {   printf("%d Volume cells were empty. See INF file.\n",ParamInfo.num_empty_cells);
        if (log_file)
            fprintf(log_file,"%d Volume cells were empty. See INF file.\n",ParamInfo.num_empty_cells);
    }
    if (log_file)
    {   fprintf(log_file,"End of messages.  Program terminated normally.\n");
        fprintf(log_file,"GPRSLICE finished.  Slices saved to disk in %s.\n",
                ParamInfo.out_directory);
        fclose(log_file);
        printf("File %s on disk contains messages.\n",log_filename);
    }
    printf("Slices and information saved to disk in %s.\n",ParamInfo.out_directory);
    printf("GPRSLICE finished.\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);

    /* Switch back to original directory */
    strcpy(str,ParamInfo.cwd);
    cp = strchr(str,':');
    if (cp != NULL)
    {   *cp = 0;
        strupr(str);
        driveno = 1 + str[0] - 'A';
        _chdrive(driveno);
    }
    chdir(ParamInfo.cwd);

    exit(0);
}
/******************************* end of main() ******************************/

/****************************** GetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <direct.h>, <stdio.h>, <stdlib.h>, <string.h>, "gprslice.h".
 * Calls: strncpy, fprintf, fopen, fclose, strstr, strcat, getcwd, strlen,
 *        PrintUsageMsg, GetCmdFileArgs, InitParameters, CreateDir, _chdrive,
 *        chdir, sprintf, abs.
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR:  Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Output file not specified.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",

    "GetParameters() ERROR: X: Inappropriate values for first, last, and columns.",
    "GetParameters() ERROR: X: There must be at least two columns.",
    "GetParameters() ERROR: Y: Inappropriate values for first, last, and rows.",
    "GetParameters() ERROR: Y: There must be at least two rows.",
    "GetParameters() ERROR: Z: Inappropriate values for first, last, and layers.",

    "GetParameters() ERROR: Z: There must be at least two layers.",
    "GetParameters() ERROR: search box X size must be > 0",
    "GetParameters() ERROR: search box Y size must be > 0",
    "GetParameters() ERROR: search box Z size must be > 0",
    "GetParameters() ERROR: unable to open a data file from the input list.",

    "GetParameters() ERROR: unable to open a XYZ file from the input list.",
    "GetParameters() ERROR: unable to open a MRK file from the input list.",
    "GetParameters() ERROR: there must be at least one input file.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 24, 1997;  August 6, 1997;  July 13, 1999;  July 23, 1999;
 *       July 30, 1999; August 3, 1999; August 6, 1999; September 14, 1999;
 *       July 26, 2000;
 */
int GetParameters (int argc,char *argv[],struct SliceParamInfoStruct *InfoPtr)
{

/***** Declare variables *****/
    char *cp;
    char str[MAX_PATHLEN];
    char drive[MAX_PATHLEN];
    int  req_args = 2;          /* exe_name and cmd_filename */
    int  i,itemp,len,driveno;   /* scratch variables and counters */
    double dtemp,dtemp2,dtemp3,dtemp4; /* scratch variables */
    FILE *file;

    extern int Debug;                        /* beginning of this source */
    extern FILE *log_file;                   /* beginning of this source */
    extern char *CreateDirMsg[];             /* from jl_util.h */

/***** Verify usage *****/
    if (argc < req_args)
    {   PrintUsageMsg();
        return 1;
    }

/***** Initialize information structure *****/
    InitParameters(InfoPtr);
    strncpy(InfoPtr->cmd_filename,argv[1],sizeof(InfoPtr->cmd_filename)-1);

    /* Get curent working directory */
    getcwd(InfoPtr->cwd,MAX_PATHLEN);

/***** Get user-defined parameters from CMD file *****/
    itemp = GetCmdFileArgs(InfoPtr);
    if (itemp > 0) return itemp;      /* 2 to 4 */

/***** Finish initializing variables and verifying valid ranges *****/
    if (Debug) puts("Start of initialize and check section");

    /* Check that output directory exists or attempt to create it */
    if (InfoPtr->out_directory[0] == 0)       /* defined? */
        strcpy(InfoPtr->out_directory,InfoPtr->cwd);
    itemp = CreateDir(InfoPtr->out_directory);
    if (itemp > 0)
    {   if (log_file)
        {   fprintf(log_file,"Directory creation failed for %s\n%s\nCurrent directory (%s) used.\n",
                    InfoPtr->out_directory,CreateDirMsg[itemp],InfoPtr->cwd);
        }
        InfoPtr->out_directory[0] = 0;
        strcpy(InfoPtr->out_directory,InfoPtr->cwd);
    } else /* success */
    {   if (log_file)
        {   fprintf(log_file,"out_directory is (%s)\n",InfoPtr->out_directory);
        }
    }

    /* Switch back to original directory */
    strcpy(str,InfoPtr->cwd);
    cp = strchr(str,':');
    if (cp != NULL)
    {   *cp = 0;
        strupr(str);
        driveno = 1 + str[0] - 'A';
        _chdrive(driveno);
    }
    chdir(InfoPtr->cwd);

    /* Calculate number of columns, rows, and layers */
    if ( (InfoPtr->X_last <= InfoPtr->X_first) || (InfoPtr->X_columns < 1) )
        return 5;
	if ( (InfoPtr->X_columns == 1) && (InfoPtr->slice_direction != X_AXIS) )
        return  6;
    InfoPtr->X_step = (InfoPtr->X_last - InfoPtr->X_first) / InfoPtr->X_columns;
    InfoPtr->X_start = InfoPtr->X_first + (InfoPtr->X_step/2.0);
    InfoPtr->X_stop = InfoPtr->X_start + ( (InfoPtr->X_columns - 1) * InfoPtr->X_step );
    InfoPtr->vol_num_cols = InfoPtr->X_columns;

    if ( (InfoPtr->Y_last <= InfoPtr->Y_first) || (InfoPtr->Y_rows < 1) )
        return 7;
	if ( (InfoPtr->Y_rows == 1) && (InfoPtr->slice_direction != Y_AXIS) )
        return  8;
    InfoPtr->Y_step = (InfoPtr->Y_last - InfoPtr->Y_first) / InfoPtr->Y_rows;
    InfoPtr->Y_start = InfoPtr->Y_first + (InfoPtr->Y_step/2.0);
    InfoPtr->Y_stop = InfoPtr->Y_start + ( (InfoPtr->Y_rows - 1) * InfoPtr->Y_step );
    InfoPtr->vol_num_rows = InfoPtr->Y_rows;

    if ( (InfoPtr->Z_last <= InfoPtr->Z_first) || (InfoPtr->Z_layers < 1) )
        return 9;
	if ( (InfoPtr->Z_layers == 1) && (InfoPtr->slice_direction != Z_AXIS) )
        return  10;
    InfoPtr->Z_step = (InfoPtr->Z_last - InfoPtr->Z_first) / InfoPtr->Z_layers;
    InfoPtr->Z_start = InfoPtr->Z_first + (InfoPtr->Z_step/2.0);
    InfoPtr->Z_stop = InfoPtr->Z_start + ( (InfoPtr->Z_layers - 1) * InfoPtr->Z_step );
    InfoPtr->vol_num_layers = InfoPtr->Z_layers;

    /* Check search box sizes */
    if (InfoPtr->box_Xsize <= 0.0) InfoPtr->box_Xsize = _ABS(InfoPtr->X_step);
    if (InfoPtr->box_Ysize <= 0.0) InfoPtr->box_Ysize = _ABS(InfoPtr->Y_step);
    if (InfoPtr->box_Zsize <= 0.0) InfoPtr->box_Zsize = _ABS(InfoPtr->Z_step);
    if (InfoPtr->box_Xsize <= 0.0) return 11;
    if (InfoPtr->box_Ysize <= 0.0) return 12;
    if (InfoPtr->box_Zsize <= 0.0) return 13;

    /* Set mean_value (volume data type is 16-bit unsigned int) */
    InfoPtr->mean_value = 32768.0;

    /* Make sure all input files exist */
    if (InfoPtr->num_input_files < 1) return 17;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   /* check data file */
        file = fopen(InfoPtr->dat_infilename[i],"rb");
        if (file == NULL) return 14;
        fclose(file);

        /* check XYZ file */
        str[0] = 0;
        strcpy(str,InfoPtr->dat_infilename[i]);
        cp = strstr(str,".");
        if (cp != NULL)  *cp = 0;
        strcat(str,".XYZ");
        file = fopen(str,"rt");
        if (file == NULL) return 15;
        fclose(file);

        /* check MRK file */
        str[0] = 0;
        strcpy(str,InfoPtr->dat_infilename[i]);
        cp = strstr(str,".");
        if (cp != NULL)  *cp = 0;
        strcat(str,".MRK");
        file = fopen(str,"rt");
        if (file == NULL) return 16;
        fclose(file);
    }

    /* Force file extensions */
    if (InfoPtr->inf_outfilename[0] == 0)
        strcpy(InfoPtr->inf_outfilename,InfoPtr->dat_infilename[0]);
    cp = strstr(InfoPtr->inf_outfilename,".");
    if (cp != NULL)  *cp = 0;
    strcat(InfoPtr->inf_outfilename,".INF");

    if (InfoPtr->t3d_outfilename[0])
    {   cp = strstr(InfoPtr->t3d_outfilename,".");
        if (cp != NULL)  *cp = 0;
        strcat(InfoPtr->t3d_outfilename,".T3D");
    }

    if (InfoPtr->sld_outfilename[0])
    {   cp = strstr(InfoPtr->sld_outfilename,".");
        if (cp != NULL)  *cp = 0;
        strcat(InfoPtr->sld_outfilename,".SLD");
    }

    /* Truncate output file templates to 6 characters */
    if (InfoPtr->pcx_outfilename[0])
    {   cp = strstr(InfoPtr->pcx_outfilename,".");
        if (cp != NULL)  *cp = 0;
        if (strlen(InfoPtr->pcx_outfilename) > 6 )
            InfoPtr->pcx_outfilename[6] = 0;
    }

    if (InfoPtr->txt_outfilename[0])
    {   cp = strstr(InfoPtr->txt_outfilename,".");
        if (cp != NULL)  *cp = 0;
        if (strlen(InfoPtr->txt_outfilename) > 6 )
            InfoPtr->txt_outfilename[6] = 0;
    }

    if (InfoPtr->bin_outfilename[0])
    {   cp = strstr(InfoPtr->bin_outfilename,".");
        if (cp != NULL)  *cp = 0;
        if (strlen(InfoPtr->bin_outfilename) > 6 )
            InfoPtr->bin_outfilename[6] = 0;
    }

    return 0;
}
/****************************** PrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>.
 * Calls: puts, printf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 13, 1997; July 15, 1999; December 22, 1999; July 26, 2000;
 *       March 26, 2001;
 */
void PrintUsageMsg (void)
{
    char str[10];
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("###############################################################################");
    puts("  Usage: GPRSLICE cmd_filename");
    puts("    Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.");
    puts("  This program imports GPR data files and generates slices or a volume of");
    puts("    transformed GPR data. The data can be left as original or transformed into");
    puts("    the absolute value, square, instantaneous amplitude, or instantaneous power.");
    puts("    The slices can be vertical (perpendicular to either the X or Y axis) or");
    puts("    horizontal (perpendicular to the Z axis). Each slice is written to disk as");
    puts("    a separate file: PCX for graphics viewers; TXT for SURFER, text editors, or");
    puts("    spreadsheets; BIN for binary file importers like TRANSFORM/SLICER/T3D.");
    puts("  Required command line arguments:");
    puts("    cmd_filename - name of a text file that follows the \"CMD\" format.");
	puts("  Look at \"GPRSLICE.CMD\" and \"GPRSLICE.TXT\" for keyword file format, valid");
    puts("    keywords, and documentation.");
    puts("  Example call: GPRSLICE sgrid1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPRSLICE_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** InitParameters() *****************************/
/* Initialize all processing variables to start-up (default) values.
 *
 * Parameter list:
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "gprslice.h".
 * Calls: none.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 24, 1997; July 12, 1999; July 30, 1999; December 22, 1999;
 *       July 26, 2000; October 2, 2001;
 */
void InitParameters (struct SliceParamInfoStruct *InfoPtr)
{
    InfoPtr->structID           = 0x534C4943L; /* "SLIC" in hex */
    InfoPtr->num_input_files    = 0;
    InfoPtr->cmd_filename[0]    = 0;
    InfoPtr->dat_infilename     = NULL;
    InfoPtr->inf_infilename[0]  = 0;
    InfoPtr->mrk_infilename[0]  = 0;
    InfoPtr->xyz_infilename[0]  = 0;
    InfoPtr->out_directory[0]   = 0;
    InfoPtr->inf_outfilename[0] = 0;
    InfoPtr->txt_outfilename[0] = 0;
    InfoPtr->pcx_outfilename[0] = 0;
    InfoPtr->bin_outfilename[0] = 0;
    InfoPtr->t3d_outfilename[0] = 0;
    InfoPtr->sld_outfilename[0] = 0;
    InfoPtr->overwrite_protect  = TRUE;
    InfoPtr->cwd[0]             = 0;
    InfoPtr->storage_format     = 0;  /* see gpr_io.h for defined values */
    InfoPtr->input_datatype     = 0;
    InfoPtr->file_header_bytes  = 0;
    InfoPtr->trace_header_bytes = 0;
    InfoPtr->first_proc_samp    = 0;  /* not used here but kept for compatibility */
    InfoPtr->data_samps         = 0;
    InfoPtr->num_traces         = 0;
    InfoPtr->first_trace        = -1L;
    InfoPtr->last_trace         = -1L;
    InfoPtr->total_time         = 0.0;
    InfoPtr->ns_per_samp        = 0.0;
    InfoPtr->num_ticks          = 0;
    InfoPtr->tick_tracenum      = NULL;
    InfoPtr->tick_xyz           = NULL;
    InfoPtr->trace_Xval         = NULL;
    InfoPtr->trace_Yval         = NULL;
    InfoPtr->trace_Zval         = NULL;
    InfoPtr->xfrm_method        = 1;
    InfoPtr->envelope           = FALSE;
    InfoPtr->X_start            = 0.0;
    InfoPtr->X_stop             = 0.0;
    InfoPtr->X_step             = 0.0;
    InfoPtr->Y_start            = 0.0;
    InfoPtr->Y_stop             = 0.0;
    InfoPtr->Y_step             = 0.0;
    InfoPtr->Z_start            = 0.0;
    InfoPtr->Z_stop             = 0.0;
    InfoPtr->Z_step             = 0.0;
    InfoPtr->X_first            = 0.0;
    InfoPtr->X_last             = 0.0;
    InfoPtr->X_columns          = 0.0;
    InfoPtr->Y_first            = 0.0;
    InfoPtr->Y_last             = 0.0;
    InfoPtr->Y_rows             = 0.0;
    InfoPtr->Z_first            = 0.0;
    InfoPtr->Z_last             = 0.0;
    InfoPtr->Z_layers           = 0.0;
    InfoPtr->box_Xsize          = 0.0;
    InfoPtr->box_Ysize          = 0.0;
    InfoPtr->box_Zsize          = 0.0;
    InfoPtr->mean_value         = INVALID_VALUE;
    InfoPtr->start_time         = 0.0;
    InfoPtr->created            = FALSE;
    InfoPtr->grid               = NULL;
    InfoPtr->grid_num_cols      = 0;
    InfoPtr->grid_num_rows      = 0;
    InfoPtr->Volume             = NULL;
    InfoPtr->vol_num_cols       = 0;
    InfoPtr->vol_num_rows       = 0;
    InfoPtr->vol_num_layers     = 0;
    InfoPtr->VolCell            = NULL;
    InfoPtr->slice_direction    = Z_AXIS;   /* horizontal */
    InfoPtr->num_empty_cells    = 0;
    InfoPtr->Z_min              = 65535.0;
    InfoPtr->Z_max              = 0.0;
    InfoPtr->multiply           = 1.0;
    InfoPtr->expand             = FALSE;
    InfoPtr->background         = FALSE;
}
/**************************** DeallocInfoStruct() ***************************/
/* Deallocate arrays in the information structure.
 *
 * Parameter list:
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gprslice.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Golden, CO
 * Date: February 21, 1997; June 30, 1999
 */
void DeallocInfoStruct (struct SliceParamInfoStruct *InfoPtr)
{
    int i,j;

    if (InfoPtr->dat_infilename != NULL)
    {   for (i=0; i<InfoPtr->num_input_files; i++)
            free(InfoPtr->dat_infilename[i]);
        free(InfoPtr->dat_infilename);
        InfoPtr->dat_infilename[i] = NULL;
    }

    if (InfoPtr->Volume != NULL)
    {   for (i=0; i<InfoPtr->vol_num_cols; i++)
        {   for (j=0; j<InfoPtr->vol_num_rows; j++)
                free(InfoPtr->Volume[i][j]);
            free(InfoPtr->Volume[i]);
        }
        free(InfoPtr->Volume);
        InfoPtr->Volume = NULL;
    }

    if (InfoPtr->VolCell != NULL)
    {   for (i=0; i<InfoPtr->vol_num_cols; i++)
        {   for (j=0; j<InfoPtr->vol_num_rows; j++)
                free(InfoPtr->VolCell[i][j]);
            free(InfoPtr->VolCell[i]);
        }
        free(InfoPtr->VolCell);
        InfoPtr->VolCell = NULL;
    }

    if ((InfoPtr->tick_tracenum) != NULL)
    {    free(InfoPtr->tick_tracenum);
        InfoPtr->tick_tracenum = NULL;
    }

    if ((InfoPtr->tick_xyz) != NULL)
    {    for (i=0; i<InfoPtr->num_ticks; i++)  free(InfoPtr->tick_xyz[i]);
        free(InfoPtr->tick_xyz);
        InfoPtr->tick_xyz = NULL;
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

    GetGprDataAsGridU16(FUNC_DESTROY,0,InfoPtr);

}
/***************************** GetCmdFileArgs() *****************************/
/* Get processing parameters from user from the DOS command line.
 * Valid file commands:
const char *GPRSLICE_CMDS[] =
{   "debug","batch","num_input_files","input_filelist[]","txt_outfilename",
    "pcx_outfilename","overwrite_protect","xfrm_method","multiply","x_first",

    "x_last","x_columns","y_first","y_stop","y_rows",
    "z_first","z_stop","z_layers","box_xsize","box_ysize",

    "box_zsize","background","bin_outfilename","envelope","slice_direction",
    "start_time","out_directory","inf_outfilename","expand","t3d_outfilename",

    "sld_outfilename",NULL,
} ;
 * Parameter list:
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <stdio.h>, <stdlib.h>, <string.h>, "gprslice.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, strlwr (non-ANSI), strupr (non-ANSI), Trimstr, strncpy,
 *        assert.
 * Returns: 0 on success,
 *         >0 on error (offsets 2, 3, and 4 into GetParametersMsg[] strings).
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 21, 1997; July 28, 1999; August 6, 1999; September 14, 1999;
 *       December 21, 1999; July 26, 2000; October 2, 2001;
 */
int GetCmdFileArgs (struct SliceParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];

    int found,i,j,num,itemp,have_string;
    int dat_in_found      = FALSE;
    int dat_out_found     = FALSE;
    double dtemp;
    FILE *infile;
    extern int Debug;
    extern FILE *log_file;
    extern const char *GPRSLICE_CMDS[];

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
#if 0
        if (Debug)
        {   printf("cmdstr = %s\n",cmdstr);
            getch();
        }
#endif
        found = -1;
        i = 0;
        while (GPRSLICE_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPRSLICE_CMDS[i]) == 0)
            {   found = i;        /* store index into GPRSLICE_CMDS[] */
                break;
            }
            i++;
        }
        if (found == -1) continue;    /* bad or missing command word */
        cp++;                         /* step 1 past equal sign */
        /* truncate string at semicolon if present outside of double quotes */
        cp3 = cp4 = NULL;             /* initialize pointers */
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
        switch (found)  /* cases depend on same order in GPRSLICE_CMDS[] */
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
                if (Debug) printf("Batch = %d\n",Batch);
                break;
            case 2:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->num_input_files = itemp;
                if (Debug) printf("num_input_files = %d\n",InfoPtr->num_input_files);
                break;
            case 3:
            {   char *cp_next = (char *)TrimStr(cp);
                int file_found = 0;

                /* number of files must be specified first */
                if (InfoPtr->num_input_files <= 0) break;

                /* allocate storage for filenames */
                InfoPtr->dat_infilename = (char **)malloc(InfoPtr->num_input_files * sizeof(char *));
                if (InfoPtr->dat_infilename == NULL)
                {   InfoPtr->num_input_files = 0;
                    if (Debug) puts("\nmalloc failed for dat_infilename");
                    if (log_file) fprintf(log_file,"malloc failed for dat_infilename\n");
                    break;      /* break out of case */
                }
                for (i=0; i<InfoPtr->num_input_files; i++)
                {   InfoPtr->dat_infilename[i] = (char *)malloc(MAX_PATHLEN * sizeof(char));
                    if (InfoPtr->dat_infilename[i] == NULL)
                    {   for (j=0; j<i; j++) free(InfoPtr->dat_infilename[j]);
                        free(InfoPtr->dat_infilename);
                        InfoPtr->dat_infilename = NULL;
                        InfoPtr->num_input_files = 0;
                        if (Debug) puts("\nmalloc failed for dat_infilename[]");
                        if (log_file) fprintf(log_file,"malloc failed for dat_infilename[]\n");
                        break;  /* out of for loop */
                    }
                }

                if (InfoPtr->num_input_files == 0) break; /* break out of case */

                /* get the file names */
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (file_found < InfoPtr->num_input_files)
                        {   cp2 = strchr(cp_next,' '); /* look for seperator */
                            if (cp2 != NULL)
                            {   *cp2 = 0;    /* truncate */
                                strcpy(InfoPtr->dat_infilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,InfoPtr->dat_infilename[file_found]);
                                file_found++;
                                *cp2 = ' ';  /* reset separator */
                                /* TrimStr(cp_next); */
                            } else
                            {   strcpy(InfoPtr->dat_infilename[file_found],cp_next);
                                if (Debug) printf("file_found=%d =%s\n",file_found,InfoPtr->dat_infilename[file_found]);
                                file_found++;
                            }
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (file_found < InfoPtr->num_input_files)
                    {   if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                        {
                            break;      /* break out of while (1) */
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
                        break;      /* break out of while (1) */
                }
                dat_in_found = TRUE;
                if (file_found < InfoPtr->num_input_files)
                {   if (Debug) puts("\nfile_found < num_input_files");
                    if (log_file) fprintf(log_file,"GetCmdFileArgs(): files found < num_input_files\n");
                    InfoPtr->num_input_files = 0;
                    for (j=0; j<InfoPtr->num_input_files; j++)
                        free(InfoPtr->dat_infilename[j]);
                    free(InfoPtr->dat_infilename);
                    InfoPtr->dat_infilename = NULL;
                    InfoPtr->num_input_files = 0;
                    dat_in_found = FALSE;
                }
                if (Debug)
                {    for (i=0; i<InfoPtr->num_input_files; i++)
                        printf("File %3d: %s\n",i,InfoPtr->dat_infilename[i]);
                }
                break;
            }
            case 4:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->txt_outfilename,cp,sizeof(InfoPtr->txt_outfilename)-1);
                strcat(InfoPtr->txt_outfilename,"\0");
                strupr(InfoPtr->txt_outfilename);
                if (InfoPtr->txt_outfilename[0]) dat_out_found = TRUE;
                if (Debug) printf("txt_outfilename = %s\n",InfoPtr->txt_outfilename);
                break;
            case 5:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->pcx_outfilename,cp,sizeof(InfoPtr->pcx_outfilename)-1);
                strcat(InfoPtr->pcx_outfilename,"\0");
                strupr(InfoPtr->pcx_outfilename);
                if (InfoPtr->pcx_outfilename[0]) dat_out_found = TRUE;
                if (Debug) printf("pcx_outfilename = %s\n",InfoPtr->pcx_outfilename);
                break;
            case 6:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                {   itemp = atoi(cp);
                }
                if (itemp == 0)  InfoPtr->overwrite_protect = FALSE;
                else InfoPtr->overwrite_protect = TRUE;
                if (Debug) printf("overwrite_protect = %d\n",InfoPtr->overwrite_protect);
                break;
            case 7:
                itemp = atoi(cp);
                itemp = 1;                    /* init value for xfrm_method */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        strupr(cp);           /* convert to upper case */
                        if      (strstr(cp,"NONE")) itemp = 0;
                        else if (strstr(cp,"ABS"))  itemp = 1;
                        else if (strstr(cp,"SQR"))  itemp = 2;
                        else if (strstr(cp,"INST")) itemp = 3;
                        else if (strstr(cp,"POWR")) itemp = 4;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp>=0 && itemp<NUM_XFRM_METHODS)
                    InfoPtr->xfrm_method = itemp;
                if (Debug) printf("xfrm_method = %d\n",InfoPtr->xfrm_method);
                break;
            case 8:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->multiply = dtemp;
                if (Debug) printf("multiply = %g\n",InfoPtr->multiply);
                break;
            case 9:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->X_first = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->X_first = atof(cp);
                }
                if (Debug) printf("X_first = %g\n",InfoPtr->X_first);
                break;
            case 10:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->X_last = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->X_last = atof(cp);
                }
                if (Debug) printf("X_last = %g\n",InfoPtr->X_last);
                break;
            case 11:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->X_columns = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->X_columns = (int)(atof(cp));
                }
                if (Debug) printf("X_columns = %g\n",InfoPtr->X_columns);
                break;
            case 12:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->Y_first = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->Y_first = atof(cp);
                }
                if (Debug) printf("Y_first = %g\n",InfoPtr->Y_first);
                break;
            case 13:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->Y_last = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->Y_last = atof(cp);
                }
                if (Debug) printf("Y_last = %g\n",InfoPtr->Y_last);
                break;
            case 14:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->Y_rows = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->Y_rows = (int)(atof(cp));
                }
                if (Debug) printf("Y_rows = %g\n",InfoPtr->Y_rows);
                break;
            case 15:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->Z_first = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->Z_first = atof(cp);
                }
                if (Debug) printf("Z_first = %g\n",InfoPtr->Z_first);
                break;
            case 16:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->Z_last = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->Z_last = atof(cp);
                }
                if (Debug) printf("Z_last = %g\n",InfoPtr->Z_last);
                break;
            case 17:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->Z_layers = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->Z_layers = (int)(atof(cp));
                }
                if (Debug) printf("Z_layers = %g\n",InfoPtr->Z_layers);
                break;
            case 18:
                dtemp = atof(cp);
                if (dtemp >= 0.0)   InfoPtr->box_Xsize = dtemp;
                if (Debug) printf("box_Xsize = %g\n",InfoPtr->box_Xsize);
                break;
            case 19:
                dtemp = atof(cp);
                if (dtemp >= 0.0)   InfoPtr->box_Ysize = dtemp;
                if (Debug) printf("box_Ysize = %g\n",InfoPtr->box_Ysize);
                break;
            case 20:
                dtemp = atof(cp);
                if (dtemp >= 0.0)   InfoPtr->box_Zsize = dtemp;
                if (Debug) printf("box_Zsize = %g\n",InfoPtr->box_Zsize);
                break;
            case 21:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                {   itemp = atoi(cp);
                }
                if (itemp == 0)  InfoPtr->background = FALSE;
                if (itemp != 0)  InfoPtr->background = TRUE;
                if (Debug) printf("background = %d\n",InfoPtr->background);
                break;
            case 22:
                break; /* disabled */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->bin_outfilename,cp,sizeof(InfoPtr->bin_outfilename)-1);
                strcat(InfoPtr->bin_outfilename,"\0");
                strupr(InfoPtr->bin_outfilename);
                if (InfoPtr->bin_outfilename[0]) dat_out_found = TRUE;
                if (Debug) printf("bin_outfilename = %s\n",InfoPtr->bin_outfilename);
                break;
            case 23:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                {   itemp = atoi(cp);
                }
                if (itemp == 0)  InfoPtr->envelope = FALSE;
                if (itemp != 0)  InfoPtr->envelope = TRUE;
                if (Debug) printf("envelope = %d\n",InfoPtr->envelope);
                break;
            case 24:
                itemp = atoi(cp);
                itemp = 2;                    /* init value for slice_direction */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        strupr(cp);           /* convert to upper case */
                        if      (strstr(cp,"X")) itemp = 0;
                        else if (strstr(cp,"Y")) itemp = 1;
                        else if (strstr(cp,"Z")) itemp = 2;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp>=0 && itemp<NUM_SLICE_DIRECTIONS)
                    InfoPtr->slice_direction = itemp;
                if (Debug) printf("slice_direction = %d\n",InfoPtr->slice_direction);
                break;
            case 25:
                InfoPtr->start_time = atof(cp);
                if (Debug) printf("start_time = %g\n",InfoPtr->start_time);
                break;
            case 26:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->out_directory,cp,sizeof(InfoPtr->out_directory)-1);
                strcat(InfoPtr->out_directory,"\0");
                strupr(InfoPtr->out_directory);
                if (Debug) printf("out_directory = %s\n",InfoPtr->out_directory);
                break;
            case 27:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->inf_outfilename,cp,sizeof(InfoPtr->inf_outfilename)-1);
                strcat(InfoPtr->inf_outfilename,"\0");
                strupr(InfoPtr->inf_outfilename);
                if (InfoPtr->inf_outfilename[0])
                if (Debug) printf("inf_outfilename = %s\n",InfoPtr->inf_outfilename);
                break;
            case 28:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                {   itemp = atoi(cp);
                }
                if (itemp == 0)  InfoPtr->expand = FALSE;
                if (itemp != 0)  InfoPtr->expand = TRUE;
                if (Debug) printf("expand = %d\n",InfoPtr->expand);
                break;
            case 29:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->t3d_outfilename,cp,sizeof(InfoPtr->t3d_outfilename)-1);
                strcat(InfoPtr->t3d_outfilename,"\0");
                strupr(InfoPtr->t3d_outfilename);
                if (InfoPtr->t3d_outfilename[0]) dat_out_found = TRUE;
                if (Debug) printf("t3d_outfilename = %s\n",InfoPtr->t3d_outfilename);
                break;
            case 30:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->sld_outfilename,cp,sizeof(InfoPtr->sld_outfilename)-1);
                strcat(InfoPtr->sld_outfilename,"\0");
                strupr(InfoPtr->sld_outfilename);
                if (InfoPtr->sld_outfilename[0]) dat_out_found = TRUE;
                if (Debug) printf("sld_outfilename = %s\n",InfoPtr->sld_outfilename);
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
/**************************** DisplayParameters() ***************************/
/* Display parameters to CRT.  Also calculates approximate storage requirements.
 *
 * Parameter list:
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, "gprslice.h".
 * Calls: printf, puts, _dos_getdate, _dos_gettime.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 24, 1997; August 5, 1997; August 6, 1997; July 13, 1999;
 *       July 20, 1999; July 30, 1999; August 6, 1999; August 9, 1999;
 *       September 2, 1999; September 14, 1999; December 22, 1999;
 *       July 26, 2000; August 2, 2000;
 */
void DisplayParameters (struct SliceParamInfoStruct *InfoPtr)
{
    char str[80];
    int i,len,num;
    int lines = 0, maxlines = 21;
    double volume_size,vol_cell_size;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    extern int Debug,Batch,ANSI_THERE;

    if (Debug) puts("DisplayParameters()");
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    /* clear keyboard buffer */
    while (kbhit())
    {   if (!getch()) getch();
    }

    /* if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); */ /* red on black */
    if (ANSI_THERE) printf("%c%c33;40m",0x1B,0x5B); /* yellow on black */
    printf("\nGPRSLICE version %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPRSLICE_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    lines += 3;
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */

    printf("\ncmd_filename = %s\n",strupr(InfoPtr->cmd_filename));
    printf("\ninput data files [%d]:\n",InfoPtr->num_input_files);
    lines += 4;

    len = 0;
    num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {  if (strlen(InfoPtr->dat_infilename[i]) > len)
           len = strlen(InfoPtr->dat_infilename[i])+1;
    }
    num = 80 / len;
    if (num < 1) num = 1;
    if (num > 5) num = 5;
    for (i=0; i<InfoPtr->num_input_files; i++)
    {   printf("%s ",InfoPtr->dat_infilename[i]);
        if ( (i>0) && !((i+1)%num) )
        {   puts("");
            lines ++;
            if (!Batch && lines >= maxlines)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
        }
    }
    puts("\n");
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    printf("current working directory = %s\n",strupr(InfoPtr->cwd));
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("out_directory             = %s\n",strupr(InfoPtr->out_directory));
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->inf_outfilename[0])
    {   printf("inf_outfilename           = %s\n",strupr(InfoPtr->inf_outfilename));
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->txt_outfilename[0])
    {   printf("txt_outfilename template  = %s\n",strupr(InfoPtr->txt_outfilename));
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->pcx_outfilename[0])
    {   printf("pcx_outfilename template  = %s\n",strupr(InfoPtr->pcx_outfilename));
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
/*     disabled
    if (InfoPtr->bin_outfilename[0])
    {   printf("bin_outfilename template  = %s\n",strupr(InfoPtr->bin_outfilename));
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
*/
    if (InfoPtr->t3d_outfilename[0])
    {   printf("t3d_outfilename           = %s\n",strupr(InfoPtr->t3d_outfilename));
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->sld_outfilename[0])
    {   printf("sld_outfilename           = %s\n",strupr(InfoPtr->sld_outfilename));
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    printf("Overwrite protection of output files is %s\n",
        InfoPtr->overwrite_protect ? "ON" : "OFF");
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    if (ANSI_THERE) printf("%c%c36;40m",0x1B,0x5B); /* cyan on black */
    puts("\nUSER INPUT FOR TRANSFORMING PARAMETERS:");
    puts("                  meters                   meters               nanoseconds");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    printf("   X_first = %11.4f    Y_first = %11.4f     Z_first = %11.4f\n",
        InfoPtr->X_first,InfoPtr->Y_first,InfoPtr->Z_first);
    lines += 4;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("    X_last = %11.4f     Y_last = %11.4f      Z_last = %11.4f\n",
        InfoPtr->X_last,InfoPtr->Y_last,InfoPtr->Z_last);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf(" X_columns = %6d          Y_rows = %6d         Z_layers = %6d\n",
        (int)InfoPtr->X_columns,(int)InfoPtr->Y_rows,(int)InfoPtr->Z_layers);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("cell_Xsize = %11.4f cell_Ysize = %11.4f  cell_Zsize = %11.4f\n",
        InfoPtr->X_step,InfoPtr->Y_step,InfoPtr->Z_step);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf(" box_Xsize = %11.4f  box_Ysize = %11.4f   box_Zsize = %11.4f\n",
        InfoPtr->box_Xsize,InfoPtr->box_Ysize,InfoPtr->box_Zsize);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("                                                   Start time = %g ns\n",
            InfoPtr->start_time);
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("\nMethod for converting stacked trace in search box:\n");
    lines += 2;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    switch (InfoPtr->xfrm_method)
    {   case XFRM_NONE:
            printf("    (as they are)\n");
            break;
        case XFRM_ABSO:
            printf("    ABSOLUTE VALUE\n");
            break;
        case XFRM_SQRE:
            printf("    SQUARE\n");
            break;
        case XFRM_INST:
            printf("    INSTANTANEOUS AMPLITUDE\n");
            break;
        case XFRM_POWR:
            printf("    INSTANTANEOUS POWER\n");
            break;
    }
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->envelope)
    {   puts("Values from ENVELOPE connecting positive peaks will be saved\n");
        lines += 2;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    } else
        puts("No envelope will be used");

    switch (InfoPtr->slice_direction)
    {   case X_AXIS:
            puts("Slices are vertical planes perpendicular to the X axis.");
            break;
        case Y_AXIS:
            puts("Slices are vertical planes perpendicular to the Y axis.");
            break;
        case Z_AXIS:
            puts("Slices are horizontal planes perpendicular to the Z axis.");
            break;
    }
    lines ++;
    if (!Batch && lines >= maxlines)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }

    if (InfoPtr->expand)
    {   printf("Volume amplitudes will be expanded linearly to fill range 0 to 65535\n");
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->multiply > 0.0 && InfoPtr->multiply != 1.0)
    {   printf("Volume amplitudes will be multiplied by %g\n",InfoPtr->multiply);
        lines ++;
        if (!Batch && lines >= maxlines)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }

    /* Issue messages */
    if (Debug)       puts("In TESTing mode.");
    else if (Batch)  puts("In Batch processing mode.");
}
/***************************** GenerateSlices() *****************************/
/* This function allocates storage for the volume, reads each GPR file, and
 * fills the cells in the volume.
 *
 * Storage for data grid is alloced and dealloced here.
 *
 * Parameter list:
 *  struct SliceParamInfoStruct *ParamInfoPtr - address of information structure
HERE check below
 *
 * Requires: <math.h>, <stdlib.h>, <string.h>, "jl_util1.h", "gprslice.h".
 * Calls: _ABS, malloc, free, printf, puts, strstrm, strncpy, strcat, sqrt,
 *        GetGprFileType, GetOtherInfo, GetXyzData, GetMrkData, SplineXyzData,
 *        Spline, InstAttribTrace, Envelope, RemAvgTrace16u.
 * Returns:  0 on success, or
 *          >0 if an error has occurred.
 *
const char *GenerateSlicesMsg[] =
{   "GenerateSlices(): No errors.",
    "GenerateSlices() ERROR: Not able to allocate storage for volume.",
    "GenerateSlices() ERROR: Not able to allocate storage for volume cell info.",
    "GenerateSlices() ERROR: Problem getting GPR file type and essential info.",
    "GenerateSlices() ERROR: GPR file format not recognized.",

    "GenerateSlices() ERROR: Insufficient or invalid info for user-defined format.",
    "GenerateSlices() ERROR: Problem getting XYZ data.",
    "GenerateSlices() ERROR: Number of XYZ records != to number of marked traces.",
    "GenerateSlices() ERROR: Problem getting GPR data.",
    "GenerateSlices() ERROR: Problem getting MRK data.",

    "GenerateSlices() ERROR: Problem Splining data.",
    "GenerateSlices() ERROR: Not able to allocate storage for temporary trace arrays.",
    "GenerateSlices() ERROR: Problem generating envelope over trace.",
  } ;
 *
 * Strategy:
 *   Initialize volumes - allocate storage and set starting values
 *   For each GPR file
 *     Determine format
 *     Allocate storage and get MRK and XYZ and assign spatial coords
 *     Get the data
 *     Allocate temporary storage
 *     Remove background if requested
 *     Place data in boxes
 *       for every column in volume (X)
 *         for every row in volume (Y)
 *           for every column in grid (trace)
 *             if within X-Y search bin
 *               add for stacking
 *           stack traces in bin
 *           transform amp
 *           wrap with envelope
 *           for every layer in volume (Z)
 *             for every sample in stacked trace
 *               if within Z search bin
 *                 accumulate sum into volume station
 *     Free temporary storage
 *   Calculate Volume cell values
 *     for each column
 *       for each row
 *         for each layer
 *           divide sum of values by number of values
 *           find max and min
 *   Expand dynamic range if requested
 *      subtract min and multiply by [65535/(max-min)]
 *   Multiply amplitudes if requested
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 24, 1997; August 5, 1997; July 12, 1999; July 30, 1999;
 *       August 5, 1999; September 2, 1999; December 22, 1999;
 */
int GenerateSlices (struct SliceParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/

    char  *cp,str[80];
    int    col,row,layer;             /* offsets into volume */
    int    trace,samp;                /* offsets into grid */
    int    i,j,k,l,itemp,file_num;    /* scratch variables and counters */
    int    step,stack_count;          /* scratch variables and counters */
    int    num_samps,num_ticks;       /* scratch variables and counters */
    int    preproc, attrib;           /* for InstAttribTrace() */
    long   num_fillers;               /* for FFT1D */
    long   trace_length;              /* for FFT1D */
    double samp_step;                 /* interval ns between samps */
    double x,x1,x2,y,y1,y2,z,z1,z2;   /* cell values */
    double X,Y,Z;                     /* X, Y, and Z for a trace */
    double samp_z;                    /* Z value for trace sample */
    double samp_val;                  /* values for sample based on "xfrm_method" */
    double *stack_trace = NULL;       /* temp array */
    double *d_trace = NULL;           /* temp array */
    double *h_trace = NULL;           /* temp array for HIlbert transform */
    double *temp_trace = NULL;        /* temp array */
    double *cmplx_trace = NULL;       /* complex trace for FFT */
    double dtemp,dmean;               /* temp variable */
    double vol_default;

    /* this group used by GetGprFileType() */
    int  file_hdr_bytes,num_hdrs,trace_hdr_bytes,samples_per_trace;
    int  num_channels,input_datatype,total_time,file_type;
    long num_traces;

    extern int Debug;
    extern FILE *log_file;
    extern const char *GetGprFileTypeMsg[];
    extern const char *GetXyzDataMsg[];
    extern const char *GetGprDataAsGridU16Msg[];

/***** Allocate and initialize Volume *****/

    /* Set initializing value */
    if (InfoPtr->envelope)
    {   vol_default = 0.0;
    } else
    {   switch (InfoPtr->xfrm_method)
        {   case XFRM_NONE:
                vol_default = InfoPtr->mean_value;
                break;
            default:                  /* all others positive only */
                vol_default = 0.0;
                break;
        }
    }

    /* Allocate volume */
    InfoPtr->Volume = (unsigned short ***)malloc(InfoPtr->vol_num_cols * sizeof(unsigned short **));
    if (InfoPtr->Volume == NULL) return 1;
    for (i=0; i<InfoPtr->vol_num_cols; i++)
    {   InfoPtr->Volume[i] = (unsigned short **)malloc(InfoPtr->vol_num_rows * sizeof(unsigned short *));
        if (InfoPtr->Volume[i] == NULL)
        {   for (j=0; j<i; j++) free(InfoPtr->Volume[j]);
            free(InfoPtr->Volume);
            InfoPtr->Volume = NULL;
            return 1;
        }
        for (j=0; j<InfoPtr->vol_num_rows; j++)
        {   InfoPtr->Volume[i][j] = (unsigned short *)malloc(InfoPtr->vol_num_layers * sizeof(unsigned short));
/* printf("[%d][%d] %p\n",i,j,InfoPtr->Volume[i][j]); */
            if (InfoPtr->Volume[i][j] == NULL)
            { /*  for (k=0; k<j; k++) free(InfoPtr->Volume[i][k]);
                for (l=0; l<InfoPtr->vol_num_cols; l++) free(InfoPtr->Volume[l]);
                free(InfoPtr->Volume);
                InfoPtr->Volume = NULL;
               */ return 1;
            }
        }
    }

    /* Initialize volume */
    for (i=0; i<InfoPtr->vol_num_cols; i++)
    {   for (j=0; j<InfoPtr->vol_num_rows; j++)
        {   for (k=0; k<InfoPtr->vol_num_layers; k++)
            {   InfoPtr->Volume[i][j][k] = vol_default;
            }
        }
    }

    /* Allocate cells */
    InfoPtr->VolCell = (struct VolCellStruct ***)malloc(InfoPtr->vol_num_cols * sizeof(struct VolCellStruct **));
    if (InfoPtr->VolCell == NULL) return 2;
    for (i=0; i<InfoPtr->vol_num_cols; i++)
    {   InfoPtr->VolCell[i] = (struct VolCellStruct **)malloc(InfoPtr->vol_num_rows * sizeof(struct VolCellStruct *));
        if (InfoPtr->VolCell[i] == NULL)
        {   for (j=0; j<i; j++) free(InfoPtr->VolCell[j]);
            free(InfoPtr->VolCell);
            InfoPtr->VolCell = NULL;
            return 2;
        }
        for (j=0; j<InfoPtr->vol_num_rows; j++)
        {   InfoPtr->VolCell[i][j] = (struct VolCellStruct *)malloc(InfoPtr->vol_num_layers * sizeof(struct VolCellStruct));
            if (InfoPtr->VolCell[i][j] == NULL)
            {   for (k=0; k<j; k++) free(InfoPtr->VolCell[i][k]);
                for (l=0; l<InfoPtr->vol_num_cols; l++) free(InfoPtr->VolCell[l]);
                free(InfoPtr->VolCell);
                InfoPtr->VolCell = NULL;
                return 2;
            }
        }
    }

    /* Initialize cells */
    for (i=0; i<InfoPtr->vol_num_cols; i++)
    {   for (j=0; j<InfoPtr->vol_num_rows; j++)
        {   for (k=0; k<InfoPtr->vol_num_layers; k++)
            {   InfoPtr->VolCell[i][j][k].sum_wgts  = 0.0;
                InfoPtr->VolCell[i][j][k].sum       = 0.0;
            }
         }
    }

/***** Cycle through the GPR files *****/

    for (file_num=0; file_num<InfoPtr->num_input_files; file_num++)
    {   printf("Slicing file %s ",InfoPtr->dat_infilename[file_num]);

    /***** Determine GPR file storage format and essential info *****/

        itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                    &samples_per_trace,&num_channels,&num_traces,
                    &input_datatype,&total_time,&file_type,
                    InfoPtr->dat_infilename[file_num]);
        if (itemp > 0)
        {   printf("%s\n",GetGprFileTypeMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
            return 3;
        }
        if (file_type == USR)
        {   if (log_file) fprintf(log_file,"ERROR: Storage format not recognized.  User must define in CMD file.\n");
            return 4;
        }
        InfoPtr->storage_format     = file_type;
        InfoPtr->input_datatype     = input_datatype;
        InfoPtr->file_header_bytes  = file_hdr_bytes;
        InfoPtr->trace_header_bytes = trace_hdr_bytes;  /* 0 for GSSI DZT file */
        InfoPtr->data_samps         = samples_per_trace;
        InfoPtr->num_traces         = num_traces;
        InfoPtr->total_time         = total_time;
        InfoPtr->ns_per_samp        = (double)InfoPtr->total_time / (InfoPtr->data_samps - 1);
        if (file_type == DZT) InfoPtr->first_proc_samp = 2;
        else                  InfoPtr->first_proc_samp = trace_hdr_bytes;

        /* Check start time */
        if (abs(InfoPtr->start_time) > InfoPtr->total_time)
            InfoPtr->start_time = InfoPtr->total_time;

    /***** Allocate storage for MRK data and get them *****/

        str[0] = 0;
        strcpy(str,InfoPtr->dat_infilename[file_num]);
        cp = strstr(str,".");
        if (cp != NULL)  *cp = 0;
        InfoPtr->mrk_infilename[0] = 0;
        strcpy(InfoPtr->mrk_infilename,str);
        strcat(InfoPtr->mrk_infilename,".MRK");

        itemp = GetMrkData(InfoPtr->mrk_infilename,&(InfoPtr->num_ticks),&(InfoPtr->tick_tracenum));
        if (itemp > 0)
        {   printf("%s\n",GetMrkDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetMrkDataMsg[itemp]);
            return 9;
        }

    /***** Allocate storage for XYZ data and get them *****/

        str[0] = 0;
        strcpy(str,InfoPtr->dat_infilename[file_num]);
        cp = strstr(str,".");
        if (cp != NULL)  *cp = 0;
        InfoPtr->xyz_infilename[0] = 0;
        strcpy(InfoPtr->xyz_infilename,str);
        strcat(InfoPtr->xyz_infilename,".XYZ");

        itemp = GetXyzData(InfoPtr->xyz_infilename,&num_ticks,&(InfoPtr->tick_xyz));
        if (itemp > 0)
        {   printf("%s\n",GetXyzDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetXyzDataMsg[itemp]);
            return 6;  /* DeallocInfoStruct() will take care of structure arrays */
        }
        if (num_ticks != InfoPtr->num_ticks) return 7;

    /***** Allocate storage and interpolate to get XYZ for every trace *****/

        /* sets first and last traces; grid_num_cols = num traces used from file
           After this function, trace 0 will be first trace used from file.
         */
        itemp = SplineXyzData(InfoPtr);
        if (itemp > 0)
        {   printf("%s\n",SplineXyzDataMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",SplineXyzDataMsg[itemp]);
            return 10;  /* DeallocInfoStruct() will take care of structure arrays */
        }

    /***** Get the data *****/

        /* allocate storage for grid and fill it */
        /* sets InfoPtr->grid_num_rows */
        /* first trace read is InfoPtr->first_trace to match splined XYZ */
        itemp = GetGprDataAsGridU16(FUNC_CREATE,file_num,InfoPtr);
        if (itemp > 0)
        {   printf("\n%s\n",GetGprDataAsGridU16Msg[itemp]);
            if (log_file) fprintf(log_file,"\n%s\n",GetGprDataAsGridU16Msg[itemp]);
            if (itemp == 5)  /* data set too large for memory */
            {   unsigned long mem,stack,bytes,requested;

                mem = _memavl();
                switch (InfoPtr->input_datatype)
                {   case -1:  case 1:                    bytes = 1;  break;
                    case -2:  case 2:  case 5:           bytes = 2;  break;
                    case -3:  case 3:  case 6:  case 4:  bytes = 4;  break;
                    case  8:                             bytes = 8;  break;
                }
                requested = InfoPtr->num_traces * InfoPtr->data_samps * bytes;
                stack = requested/mem;
                printf("\nSize of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
                printf("Try using GPR_CNDS first with skip_traces set to %lu or higher.\n",stack+2);
                if (log_file) fprintf(log_file,"Size of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
                if (log_file) fprintf(log_file,"Try using GPR_CNDS first with skip_traces set %lu or higher.\n",stack+2);
            }
            return 8;  /* DeallocInfoStruct() will take care of structure arrays */
        }

    /***** Check if trace length is a power of 2, <= 65536 for FFT *****/

        num_samps = InfoPtr->grid_num_rows;
        num_fillers = 0;
        trace_length = num_samps;
        for (i=1; i<16; i++)
    	{   if (num_samps > pow(2,(double)i) &&
	     		num_samps < pow(2,(double)(i+1)))
	 	    {   trace_length = pow(2,(double)(i+1));
                num_fillers = trace_length - num_samps;
	     		break;
	 	    }
        }

    /***** Allocate storage for temporary trace arrays */

        stack_trace = (double *)malloc(trace_length * sizeof(double));
        d_trace     = (double *)malloc(trace_length * sizeof(double));
        h_trace     = (double *)malloc(trace_length * sizeof(double));
        cmplx_trace = (double *)malloc(2 * trace_length * sizeof(double));
        temp_trace  = (double *)malloc(trace_length * sizeof(double));
        if (stack_trace == NULL || d_trace == NULL || h_trace == NULL ||
            cmplx_trace == NULL || temp_trace == NULL)
        {   free(stack_trace);  free(d_trace);  free(h_trace);
            free(temp_trace);  free(cmplx_trace);
            GetGprDataAsGridU16(FUNC_DESTROY,file_num,InfoPtr);
            return 11;  /* DeallocInfoStruct() will take care of structure arrays */
        }

    /***** Remove background trace if requested *****/

        if (InfoPtr->background)
            RemAvgTrace16u(InfoPtr->grid_num_cols,InfoPtr->grid_num_rows,
                            InfoPtr->grid);

    /***** Place data in boxes *****/

        /* x y z are coordinates of center of volume cell
           x1,x2 y1,y2 z1,z2 are limits of search box
           X Y Z are coordinates of top of trace
           col row layer are cell locations in the volume
           trace is the trace number
           samp_z is sample time
         */
        samp_step = InfoPtr->ns_per_samp;   /* positive direction is down */

        /* step is for progress notification to user */
        step = InfoPtr->vol_num_cols / 10;
        if (step < 1) step = 1;

        /* for every column (X) in volume */
        for (col=0; col<InfoPtr->vol_num_cols; col++)
        {   if (!(col % step)) printf(".");  /* notify user of progress */

            /* middle value and edges of search bin in X direction */
            x  = InfoPtr->X_start + (col * InfoPtr->X_step);
            x1 = x - (InfoPtr->box_Xsize / 2.0);
            x2 = x + (InfoPtr->box_Xsize / 2.0);

            /* for every row (Y) in volume */
            for (row=0; row<InfoPtr->vol_num_rows; row++)
            {   /* middle value and edges of search bin in Y direction */
                y  = InfoPtr->Y_start + (row * InfoPtr->Y_step);
                y1 = y - (InfoPtr->box_Ysize / 2.0);
                y2 = y + (InfoPtr->box_Ysize / 2.0);

                /* reset for next stacking */
                stack_count = 0;
                for (i=0; i<trace_length; i++)
                    stack_trace[i] =  0.0;

                /* for every column (trace) in grid: stack within bin */
                for (trace=0; trace<InfoPtr->grid_num_cols; trace++)
                {   X = InfoPtr->trace_Xval[trace];
                    Y = InfoPtr->trace_Yval[trace];

                    /* is trace within limits of this search bin? */
                    if (X>=x1 && X<x2 && Y>=y1 && Y<y2)
                    {   /* add samples (grid rows) to stack trace
                           and increment count */
                        stack_count++;
                        for(samp=0; samp<num_samps; samp++)
                            stack_trace[samp] += InfoPtr->grid[trace][samp];
                    }
                }

                /* calculate stacked trace (this is NOT "centered" about 0);
                   that is range is 0-65535 */
                if (stack_count > 0)
                {   for (i=0; i<num_samps; i++)
                        stack_trace[i] /= stack_count;
                } else /* no traces in bin */
                {   continue;  /* at end of for every volume row loop */
                }

                /* transform the stacked trace (range is 0-65535) */
                switch (InfoPtr->xfrm_method)
                {   case XFRM_ABSO:  /* absolute value */
                        for (i=0; i<trace_length; i++)
                        {   temp_trace[i] = _ABS(stack_trace[i] - InfoPtr->mean_value);
                            /* multiply by 2 to keep within limits of ushort */
                            temp_trace[i] *= 2.0;
                        }
                        break;
                    case XFRM_SQRE:  /* squared value */
                        for (i=0; i<trace_length; i++)
                        {   /* copy and subtract mean */
                            temp_trace[i] = stack_trace[i] - InfoPtr->mean_value;
                            /* square the value */
                            temp_trace[i] *= temp_trace[i];
                            /* divide by 16384 to keep within limits of ushort */
                            temp_trace[i] /= 16384.0;
                        }
                        break;
                    case XFRM_INST:   /* instantaneous amplitude */
                        for (i=0; i<trace_length; i++)\
                            temp_trace[i] = stack_trace[i];        /* copy */
                        attrib = 1;
                        preproc = TRUE;
                        if (trace_length < 24) preproc = FALSE;
                        /* 24 is hardwired into InstAttribTrace() */
                        InstAttribTrace(trace_length,preproc,attrib,temp_trace,cmplx_trace);
                        for (i=0; i<trace_length; i++)\
                            temp_trace[i] *= sqrt(2.0);            /* rescale */
                        break;
                    case XFRM_POWR:   /* instantaneous power */
                        for (i=0; i<trace_length; i++)\
                            temp_trace[i] = stack_trace[i];        /* copy */
                        attrib = 2;
                        preproc = TRUE;
                        if (trace_length < 24) preproc = FALSE;
                        /* 24 is hardwired into InstAttribTrace() */
                        InstAttribTrace(trace_length,preproc,attrib,temp_trace,cmplx_trace);
                        for (i=0; i<trace_length; i++)\
                            temp_trace[i] /= 32767.0;              /* rescale */
                        break;
                    default:
                        for (i=0; i<trace_length; i++)
                            temp_trace[i] = stack_trace[i];
                        break;
                }  /* end of switch (InfoPtr->xfrm_method) block */

                /* wrap transformed trace with an envelope if requested */
                if (InfoPtr->envelope)
                {   itemp = Envelope(num_samps,temp_trace,d_trace);
                    if (itemp > 0)
                    {   free(stack_trace);  free(d_trace);  free(h_trace);
                        free(temp_trace);  free(cmplx_trace);
                        GetGprDataAsGridU16(FUNC_DESTROY,file_num,InfoPtr);
                        return 12;  /* DeallocInfoStruct() will take care of structure arrays */
                    }

                    /* copy envelope back to temp */
                    for (samp=0; samp<num_samps; samp++)
                    {   temp_trace[samp] = d_trace[samp];
                    }
                }

                /* Work through transformed stacked trace (layer by layer)
                   and place amplitudes into cells */
/*  !!!!!!!!!!!
Example:
  Layer#   Z-val    sample#   sample(ns)
    0       0         0          -2 Z       Z_start = 0
    1       1         1          -1         Z_stop = 6
    2       2 z1      2           0         Z_step = 1
    3       3 z       3           1 samp_z  samp_step = 1
    4       4 z2      4           2         start_time = -2
    5       5         5           3         vol_num_layers = 7
    6       6         6           4
*/
                for (layer=0; layer<InfoPtr->vol_num_layers;layer++)
                {   /* middle value and edges of search bin in Z direction */
                    z  = InfoPtr->Z_start + (layer * InfoPtr->Z_step);
                    z1 = z - (InfoPtr->box_Zsize / 2.0);
                    z2 = z + (InfoPtr->box_Zsize / 2.0);

                    /* for every sample in temp_trace */
                    for (samp=0; samp<num_samps; samp++)
                    {   /* calculate sample time */
                        samp_z = InfoPtr->start_time + (samp * samp_step);

                        /* is sample in search bin */
                        if (samp_z <  z1) continue; /* at end of for every samp loop */
                        if (samp_z >= z2) break;    /* out of for every samp loop */

                        /* copy and limit range to unsigned short (grid type) */
                        samp_val = temp_trace[samp];
                        if (samp_val <     0.0) samp_val =     0.0;
                        if (samp_val > 65535.0) samp_val = 65535.0;

                        /* sum item value */
                        InfoPtr->VolCell[col][row][layer].sum += samp_val;

                        /* increment counter */
                        InfoPtr->VolCell[col][row][layer].sum_wgts += 1.0;

                    } /* for every transformed trace sample */
                } /* for every volume layer */
            } /* for every volume row */
        } /* for every volume column */

    /***** Free storage so can get another file *****/

        /* free InfoPtr allocation */
        free(InfoPtr->tick_tracenum);
        InfoPtr->tick_tracenum = NULL;

        for (i=0; i<InfoPtr->num_ticks; i++)
        {    free(InfoPtr->tick_xyz[i]);
        }
        free(InfoPtr->tick_xyz);
        InfoPtr->tick_xyz = NULL;
        free(InfoPtr->trace_Xval);
        free(InfoPtr->trace_Yval);
        free(InfoPtr->trace_Zval);
        InfoPtr->trace_Xval = NULL;
        InfoPtr->trace_Yval = NULL;
        InfoPtr->trace_Zval = NULL;

        /* free temporary allocation */
        free(stack_trace);  free(d_trace);  free(h_trace);
        free(temp_trace);  free(cmplx_trace);
        stack_trace = NULL; d_trace = NULL;  h_trace = NULL;
        cmplx_trace = NULL;  temp_trace = NULL;

        /* free grid allocation */
        GetGprDataAsGridU16(FUNC_DESTROY,file_num,InfoPtr);

       printf("\r                                                                      \r");
    }  /*********** end of for() loop cycling thru data files ************/

/***** Calculate average value for each cell *****/

    printf("Calculating Volume cell values ... ");
    step = InfoPtr->vol_num_cols / 10;
    if (step < 1) step = 1;

    /* volume and grid are 16-bit unsigned short */
    InfoPtr->Z_min = 65536.0;
    InfoPtr->Z_max = 0.0;
    InfoPtr->num_empty_cells = 0;
    for (col=0; col<InfoPtr->vol_num_cols; col++)
    {   if (!(col % step)) printf("+");
        for (row=0; row<InfoPtr->vol_num_rows; row++)
        {   for (layer=0; layer<InfoPtr->vol_num_layers; layer++)
            {   if (InfoPtr->VolCell[col][row][layer].sum_wgts > 0.0)
                {   /* Calculate average value at station */
                    dtemp = InfoPtr->VolCell[col][row][layer].sum / InfoPtr->VolCell[col][row][layer].sum_wgts;

                    /* limit range to 16-bit unsigned (grid type) */
                    if (dtemp < 0.0)
                        dtemp = 0.0;
                    if (dtemp > 65535.0)
                        dtemp = 65535.0;

                    /* round to nearest 0.5->0 and 65535.5->65535 */
                    InfoPtr->Volume[col][row][layer] = (unsigned short)(dtemp + 0.5);

                    /* find max and min */
                    InfoPtr->Z_min = _MINN(((unsigned short)(dtemp + 0.5)),InfoPtr->Z_min);
                    InfoPtr->Z_max = _MAXX(((unsigned short)(dtemp + 0.5)),InfoPtr->Z_max);
                } else
                {   InfoPtr->num_empty_cells++;
                    InfoPtr->Volume[col][row][layer] = (unsigned short)vol_default;
                }
                if (Debug)
                {    printf("Volume[%3d][%3d][%3d]=%u sum=%g sum_wgts=%g Z_min=%g Z_max=%g\n",
                        col,row,layer,
                        (unsigned short)InfoPtr->Volume[col][row][layer],
                        InfoPtr->VolCell[col][row][layer].sum,
                        InfoPtr->VolCell[col][row][layer].sum_wgts,
                        InfoPtr->Z_min,InfoPtr->Z_max);
                }
            }
        }
    }
    if (Debug)
    {   FILE *file2;

        file2 = fopen("vol.txt","w+t");
        fprintf(file2,"col row layer  vol         sum           wgt\n");
        for (row=0; row<InfoPtr->vol_num_rows; row++)
            for (col=0; col<InfoPtr->vol_num_cols; col++)
                for (layer=0; layer<InfoPtr->vol_num_layers; layer++)
                    fprintf(file2,"[%2d][%2d][%2d] %6d %12d %12d\n",col,row,layer,
                            InfoPtr->Volume[col][row][layer],
                            (int)InfoPtr->VolCell[col][row][layer].sum,
                            (int)InfoPtr->VolCell[col][row][layer].sum_wgts);
        fclose(file2);
    }

/***** Expand dynamic range if requested *****/
    if ( InfoPtr->expand && (InfoPtr->Z_max > InfoPtr->Z_min) )
    {
        dtemp = 65535.0 / (InfoPtr->Z_max - InfoPtr->Z_min);
        if (Debug)
        {    printf("Expand: ratio=%g  min=%g  max= %g",
                    dtemp,InfoPtr->Z_min,InfoPtr->Z_max);
        }
        for (col=0; col<InfoPtr->vol_num_cols; col++)
        {   for (row=0; row<InfoPtr->vol_num_rows; row++)
            {   for(layer=0; layer<InfoPtr->vol_num_layers; layer++)
                {   if (InfoPtr->VolCell[col][row][layer].sum_wgts > 0.0)
                    {   Z  = InfoPtr->Volume[col][row][layer];
                        Z -= InfoPtr->Z_min;
                        Z *= dtemp;

                        /* limit range to 16-bit unsigned short (grid type) */
                        if (Z <     0.0) Z =     0.0;
                        if (Z > 65535.0) Z = 65535.0;

                        InfoPtr->Volume[col][row][layer] = (unsigned short)Z;
                    }
                }
            }
        }
    }

/***** Multiply amplitudes if requested *****/
    if (InfoPtr->multiply > 0.0 && InfoPtr->multiply != 1.0)
    {
        for (col=0; col<InfoPtr->vol_num_cols; col++)
        {   for (row=0; row<InfoPtr->vol_num_rows; row++)
            {   for(layer=0; layer<InfoPtr->vol_num_layers; layer++)
                {   if (InfoPtr->VolCell[col][row][layer].sum_wgts > 0.0)
                    {   Z  = InfoPtr->Volume[col][row][layer];
                        Z *= InfoPtr->multiply;

                        /* limit range to 16-bit unsigned short (grid type) */
                        if (Z <     0.0) Z =     0.0;
                        if (Z > 65535.0) Z = 65535.0;

                        InfoPtr->Volume[col][row][layer] = (unsigned short)Z;
                    }
                }
            }
        }
    }

    printf("\r                                                       \r");
    return 0;
}
/************************* GetGprDataAsGridU16() ****************************/
/* Get the data/info from the appropriate files and convert to unsigned shorts.
 * Allocate/De-allocate storage for grid[][].
 * Verify valid parameters and ranges.
 *
 * Modified 6/99 to only read in traces that will be used from file.
 *
 * NOTE: The "created" member of struct SliceParamInfoStruct restricts the
 *       application of the struct to one data set and one grid at a time.
 *       To read multiple data sets and allocate multiple grids, either
 *       de-allocate the first one OR declare multiple structs to hold the
 *       information and pointers.
 *
 * Method:
 *   1. Declare variables
 *   2. De-allocate storage if command==FUNC_DESTROY and return, else
 *   3. Test for balanced calls
 *   4. Allocate storage
 *   5. Copy disk file data into grid[][]
 *
 * Parameter list:
 *  int command   - 0 to allocate storage; !0 to deallocate storage
 *  int file_num  - file number (indexed from 0)
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdio.h>, <stdlib.h>, "jl_utils.h", "gprslice.h".
 * Calls: malloc, free, printf, puts, fopen, fclose, fread, setvbuf, feof,
 *        realmalloc, realfree.
 * Returns: 0 on success,
 *         >0 on error, offset into array of messages strings.
 *
const char *GetGprDataAsGridU16Msg[] =
{   "GetGprDataAsGridU16(): No errors.",
    "GetGprDataAsGridU16() ERROR: Attemp made to de-allocate storage before allocating.",
    "GetGprDataAsGridU16() ERROR: Attemp made to re-allocate storage.  De-allocate first.",
    "GetGprDataAsGridU16() ERROR: Invalid input data element type .",
    "GetGprDataAsGridU16() ERROR: No recognized data/info input files specified.",

    "GetGprDataAsGridU16() ERROR: Not able to allocate storage for grid.",
    "GetGprDataAsGridU16() ERROR: Trace header size must be integer multiple of sample bytes.",
    "GetGprDataAsGridU16() ERROR: Not able to allocate storage for one trace.",
    "GetGprDataAsGridU16() ERROR: Unable to open input data file.",
    "GetGprDataAsGridU16() ERROR: fread() of data file returned an error.",

    "GetGprDataAsGridU16() ERROR: End of File found.",
    "GetGprDataAsGridU16() ERROR: Floating point types not supported.",
    "GetGprDataAsGridU16() ERROR: Number of traces read less than number of grid columns.",
} ;
 * !!!  NOTE: message 5 is checked by calling routine. !!!
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 24, 1996, Aug 11, 1997; July 12, 1999; August 30, 1999;
 */
int GetGprDataAsGridU16 (int command,int file_num,struct SliceParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/

    int error;
    long i,j;                   /* scratch variables and counters */
    long grid_col_size;         /* number of bytes in one trace */
    long samp_bytes;            /* number of bytes in one trace sample */
    long traces_read;
    long col,row,ltemp,offset;
    long disk_trace_bytes,header_samps;
    float ftemp;
    void *trace  = NULL;
    void *vptr   = NULL;
    FILE *infile = NULL;
    extern int Debug;

    #if defined(_INTELC32_)
      char *buffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
    #endif

/***** Deallocate storage *****/

    if (command == FUNC_DESTROY)
    {   if (!InfoPtr->created) return 1;
        if (InfoPtr->grid == NULL) return 0;
        for (i=0; i<InfoPtr->grid_num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        InfoPtr->created = FALSE;
        return 0;
    }

/***** Test for balanced calls *****/

    if (InfoPtr->created)      return 2;
    if (InfoPtr->grid != NULL) return 2;
    InfoPtr->created = FALSE;

/***** Determine number of columns and rows *****/

    switch (InfoPtr->storage_format)
    {   case DZT: case DT1:  case SGY:  case USR:  case SIR2:  case MOD_DZT:
            break;
        default:  /* unrecognized format */
            return 4;
    }

    switch (InfoPtr->input_datatype)
    {   case 1:  case -1:            samp_bytes = sizeof(char);   break;
        case 2:  case -2:  case -5:  samp_bytes = sizeof(short);  break;
        case 3:  case -3:  case -6:  samp_bytes = sizeof(long);   break;
        case 4:  case 8:             return 11;
        default:                     return 6;
    }

    header_samps = InfoPtr->trace_header_bytes / samp_bytes; /* expect truncation */
    if (header_samps * samp_bytes != InfoPtr->trace_header_bytes) return 6;

    InfoPtr->grid_num_rows = InfoPtr->data_samps + header_samps;

/***** Allocate storage for grid[][] and trace[] *****/

    grid_col_size = InfoPtr->grid_num_rows * sizeof(unsigned short);
    InfoPtr->grid = (unsigned short **)malloc((size_t)(InfoPtr->grid_num_cols * sizeof(unsigned short *)));
    if (InfoPtr->grid)
    {   for (i=0; i<InfoPtr->grid_num_cols; i++)
        {   InfoPtr->grid[i] = (unsigned short *)malloc((size_t)grid_col_size);
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

    disk_trace_bytes = InfoPtr->trace_header_bytes + (InfoPtr->data_samps * samp_bytes);
    trace = (void *)malloc((size_t)disk_trace_bytes);
    if (trace == NULL)
    {   for (i=0; i<InfoPtr->grid_num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return 7;
    }

/***** Open input file *****/

    if ((infile = fopen(InfoPtr->dat_infilename[file_num],"rb")) == NULL)
    {   for (i=0; i<InfoPtr->grid_num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        free(trace);
        return 8;
    }

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(buffer,vbufsize);
      if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
      else         setvbuf(infile,NULL,_IOFBF,vbufsize);
    #endif

/***** Get the data *****/

    /* Get data from disk and convert to unsigned 16-bit integers */
    traces_read = 0;
    error = 0;
    offset = InfoPtr->file_header_bytes + (InfoPtr->first_trace * disk_trace_bytes);
    fseek(infile,offset,SEEK_SET);       /* goto first used trace */

    /* Trace by trace */
    for (col=0; col<InfoPtr->grid_num_cols; col++)
    {
        /* reset error flag and get one trace */
        error = 0;
        if (fread(trace,(size_t)disk_trace_bytes,(size_t)1,infile) < 1)
        {   error = 9;
            if (feof(infile)) error = 10;
            break;  /* out of for loop reading traces from file */
        }
        traces_read++;

        /* point past trace header */
        vptr = (void *)((char *)trace + InfoPtr->trace_header_bytes);
        switch (InfoPtr->input_datatype)
        {   case -1:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                {   ltemp = ((unsigned char *)vptr)[row];
                    InfoPtr->grid[col][row] = ltemp << 8;
                }
                break;
            case 1:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                {   ltemp = ((char *)vptr)[row];
                    InfoPtr->grid[col][row] = ltemp + 128;
                }
                break;
            case -2:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                    InfoPtr->grid[col][row] = ((unsigned short *)vptr)[row];
                break;
            case -5:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                    InfoPtr->grid[col][row] = (((unsigned short *)vptr)[row]) >> 4;
                break;
            case 2:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                {   ltemp = ((short *)vptr)[row];
                    InfoPtr->grid[col][row] = ltemp + 32768L;
                }
                break;
            case -3:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                    InfoPtr->grid[col][row] = (((unsigned long *)vptr)[row]) >> 16;
                break;
            case -6:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                    InfoPtr->grid[col][row] = (((unsigned long *)vptr)[row]) >> 8;
                break;
            case 3:
                for (row=0; row<InfoPtr->grid_num_rows; row++)
                {   ftemp = ((long *)vptr)[row];
                    ftemp += 2147483648L;
                    InfoPtr->grid[col][row] = ftemp / 65536L;
                }
                break;
            }   /* end of switch block */

    }  /* end of for loop reading traces from file */

    if (!error && (traces_read < InfoPtr->grid_num_cols)) error = 12;

/***** Close file, release buffers, and return *****/

    fclose(infile);
    free(trace);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    if (error > 0)  /* problem getting data */
    {   /* deallocate storage and return error code */
        for (i=0; i<InfoPtr->grid_num_cols; i++) free(InfoPtr->grid[i]);
        free(InfoPtr->grid);
        InfoPtr->grid = NULL;
        return error;
    } else
    {   /* flag structure and return success */
        InfoPtr->created = TRUE;
        return 0;
    }
}

/***************************** SplineXyzData() *****************************/
/* Spline or linear interpolate to get X, Y, and Z locations for each trace.
 *
 * NOTE: this program allocates storage for "trace_Xval", "trace_Yval",
 *       "trace_Zval" arrays.
 *
 * Parameter list:
 *  struct SliceParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: "assertjl.h", <stdio.h>, <stdlib.h>, "gprslice.h"
 * Calls: assert, malloc, free, Spline, printf, fprintf.
 * Returns: 0 on success,
 *         >0 on error (offset into message string array)
 *
const char *SplineXyzDataMsg[] =
{   "SplineXyzData() No error.",
    "SplineXyzData() ERROR: Number of marked traces < 2.",
    "SplineXyzData() ERROR: Marked trace numbers not in proper order.",
    "SplineXyzData() ERROR: Marked trace numbers outside of data trace numbers.",
    "SplineXyzData() ERROR: Not able to allocate sufficient storage.",

    "SplineXyzData() ERROR: Problem splining X topography data.",
    "SplineXyzData() ERROR: Problem splining Y topography data.",
    "SplineXyzData() ERROR: Problem splining Z topography data.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Golden, CO
 * Date: July 13, 1999; September 27, 2000;
 */
int SplineXyzData (struct SliceParamInfoStruct *InfoPtr)
{
    int i,itemp,num_ticks,num_cols,error=0;
    double dtemp;
    double *trace_num     = NULL; /* usable trace numbers */
    double *y             = NULL; /* "y" values pulled out of tick_xyz[][] */
    double *tick_tracenum = NULL; /* copy InfoPtr array into double array */

    extern const char *SplineMsg[];
    extern FILE *log_file;
    extern int Debug;


    /* Check that arrays are allocated */
    assert(InfoPtr->tick_tracenum && InfoPtr->tick_xyz);

    /* Must have at least 2 tick marks */
    num_ticks = InfoPtr->num_ticks;
    if (num_ticks < 2) return 1;

    /* Verify range of tick marks is subset of data trace numbers.
       tick_tracenum[] always increases.
       There must be at least 3 traces used.
     */
    for (i=1; i<num_ticks; i++)
    {   if (InfoPtr->tick_tracenum[i] <= InfoPtr->tick_tracenum[i-1])
        {   return 2;
        }
    }
    if ((InfoPtr->tick_tracenum[0] >= InfoPtr->num_traces-2) ||
        (InfoPtr->tick_tracenum[0] < 0)                      ||
        (InfoPtr->tick_tracenum[num_ticks-1] < 0)            ||
        (InfoPtr->tick_tracenum[num_ticks-1] >= InfoPtr->num_traces)
       )
        return 3;

    /* assign first and last traces */
    InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
    InfoPtr->last_trace  = InfoPtr->tick_tracenum[num_ticks-1];

    /* this is the number of traces in the grid and number to be read from file */
    InfoPtr->grid_num_cols = InfoPtr->last_trace - InfoPtr->first_trace + 1;
    num_cols = InfoPtr->grid_num_cols;

    /* Allocate storage */
    InfoPtr->trace_Xval = (double *)malloc((size_t)num_cols * sizeof(double));
    InfoPtr->trace_Yval = (double *)malloc((size_t)num_cols * sizeof(double));
    InfoPtr->trace_Zval = (double *)malloc((size_t)num_cols * sizeof(double));
    trace_num      = (double *)malloc((size_t)num_cols * sizeof(double));
    tick_tracenum  = (double *)malloc((size_t)num_ticks * sizeof(double));
    y              = (double *)malloc((size_t)num_ticks * sizeof(double));
    if ( trace_num == NULL || y == NULL || tick_tracenum == NULL ||
         InfoPtr->trace_Xval == NULL || InfoPtr->trace_Yval == NULL ||
         InfoPtr->trace_Zval == NULL )
    {   free(tick_tracenum);  free(trace_num); free(y);
        free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
        InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
        return 4;
    }

    /* Interpolate for X, Y, Z for each used trace */
    if (num_ticks < 3)
    {   /* linear interpolation */
        /* interpolate X */
        error = 0;
        dtemp = InfoPtr->tick_xyz[num_ticks-1][0] - InfoPtr->tick_xyz[0][0];
        dtemp /= InfoPtr->tick_tracenum[num_ticks-1] - InfoPtr->tick_tracenum[0];
        for (i=0; i<num_cols; i++)
            InfoPtr->trace_Xval[i] = InfoPtr->tick_xyz[0][0] + (i * dtemp);
        InfoPtr->trace_Xval[num_cols - 1] = InfoPtr->tick_xyz[num_ticks-1][0];

        /* interpolate Y */
        dtemp = InfoPtr->tick_xyz[num_ticks-1][1] - InfoPtr->tick_xyz[0][1];
        dtemp /= InfoPtr->tick_tracenum[num_ticks-1] - InfoPtr->tick_tracenum[0];
        for (i=0; i<num_cols; i++)
            InfoPtr->trace_Yval[i] = InfoPtr->tick_xyz[0][1] + (i * dtemp);
        InfoPtr->trace_Yval[num_cols - 1] = InfoPtr->tick_xyz[num_ticks-1][1];

        /* interpolate Z */
        dtemp = InfoPtr->tick_xyz[num_ticks-1][2] - InfoPtr->tick_xyz[0][2];
        dtemp /= InfoPtr->tick_tracenum[num_ticks-1] - InfoPtr->tick_tracenum[0];
        for (i=0; i<num_cols; i++)
            InfoPtr->trace_Zval[i] = InfoPtr->tick_xyz[0][2] + (i * dtemp);
        InfoPtr->trace_Zval[num_cols - 1] = InfoPtr->tick_xyz[num_ticks-1][2];
    } else
    {   /* assign independent values for Spline() */
        /* xold [num_ticks] */
        for (i=0; i<num_ticks; i++)
            tick_tracenum[i] = InfoPtr->tick_tracenum[i];
		/* xnew [num_cols] */
        for (i=0; i<num_cols; i++)
            trace_num[i] = (int)(InfoPtr->tick_tracenum[0] + i);

        error = 0;
        /* spline X */
        /* assign dependent yold [num_ticks] value for Spline() */
        for (i=0; i<num_ticks; i++)
            y[i] = InfoPtr->tick_xyz[i][0];     /* X-coordinates */
        itemp = Spline(num_ticks,num_cols,
                       tick_tracenum,y,trace_num,InfoPtr->trace_Xval);
        if (itemp > 0)
        {   printf("\ntrace_Xval (X): %s\n",SplineMsg[itemp]);
            if (log_file) fprintf(log_file,"\nSplineXyzData() trace_Xval: %s\n",SplineMsg[itemp]);
            error = 5;
            free(tick_tracenum);  free(trace_num);  free(y);
            free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
            InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
        }

        if (!error)
        {   /* assign dependent yold [num_ticks] value for Spline() */
        	for (i=0; i<InfoPtr->num_ticks; i++)
                y[i] = InfoPtr->tick_xyz[i][1];  /* Y-coordinates */
            itemp = Spline(num_ticks,InfoPtr->grid_num_cols,
                       tick_tracenum,y,trace_num,InfoPtr->trace_Yval);
            if (itemp > 0)
            {   printf("\ntrace_Yval (Y): %s\n",SplineMsg[itemp]);
                if (log_file) fprintf(log_file,"\nSplineXyzData() trace_Yval: %s\n",SplineMsg[itemp]);
                error = 6;
                free(tick_tracenum);  free(trace_num);  free(y);
                free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
                InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
            }

            if (!error)
            {   /* assign dependent yold [num_ticks] value for Spline() */
            	for (i=0; i<InfoPtr->num_ticks; i++)
                	y[i] = InfoPtr->tick_xyz[i][2];  /* Z-coordinates */
                itemp = Spline(num_ticks,InfoPtr->grid_num_cols,
                       tick_tracenum,y,trace_num,InfoPtr->trace_Zval);
                if (itemp > 0)
                {   printf("\ntrace_Zval (Z): %s\n",SplineMsg[itemp]);
                    if (log_file) fprintf(log_file,"\nSplineXyzData() trace_Zval: %s\n",SplineMsg[itemp]);
                    error = 7;
                    free(tick_tracenum);  free(trace_num);  free(y);
                    free(InfoPtr->trace_Xval); free(InfoPtr->trace_Yval); free(InfoPtr->trace_Zval);
                    InfoPtr->trace_Xval = NULL; InfoPtr->trace_Yval = NULL; InfoPtr->trace_Zval = NULL;
                }
            }
        }
    }

    free(tick_tracenum);  free(trace_num);  free(y);
    return error;
}
/**************************** RemAvgTrace16u() *****************************/
/* This function adds all the traces in a file together then divides by
 * number of traces to get an average trace, which can be considered the
 * "background". The average trace is normalized so that it is centered about
 * zero then subtracted from each column in the image.
 *
 * Parameters:
 *  long num_cols - number of columns in grid
 *  long num_rows - number of rows in grid
 *  unsigned short **grid  - [num_cols][num_rows]; pointer to 2D array
 *
 * Requires: <assert.h>.
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Team   Lakewood, CO
 * Date: July 27, 1999
 */
void RemAvgTrace16u (int num_cols,int num_rows,unsigned short **grid)
{
    long col,row;
    double dtemp,avg;

    assert(num_cols > 0 && num_rows > 0 && grid != NULL);
    for (row=0; row<num_rows; row++)
    {   avg = 0.0;
        /* add up all the values for one row */
        for (col=0; col<num_cols; col++)  avg += grid[col][row];
        /* get average value for that row */
        avg /= (double)num_cols;
        /* set "central" value to 0 */
        avg -= 32768.0;
        /* subtract average value from the row in the image */
        for (col=0; col<num_cols; col++)
        {   dtemp = (double)grid[col][row] - avg;
            if      (dtemp <   0.0)   grid[col][row] = 0;
            else if (dtemp > 65535.0) grid[col][row] = 65535;
            else                      grid[col][row] = dtemp;
        }
    }
}
/******************************* Envelope() ********************************/
/* Wrap the data series with an envelope that connects th positive highs.
 *
 * Parameters:
 *   int  n     - number of members in data array and envelope array
 *   double *a  - pointer to array of data values - input
 *   double *b  - pointer to array for envelope values - output
 *
 * NOTE: Storage for arrays pointed to in parameter list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, "gprslice.h"
 * Calls: Spline
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: June 30, 1999
 */
int Envelope (int n,double *a,double *b)
{
    int i,error,inc;
    long nold,nnew;
    double *x = NULL, *y = NULL, *xnew = NULL;

    /* Allocate temporary storage */
    x = (double *)malloc(n*sizeof(double));
    y = (double *)malloc(n*sizeof(double));
    xnew = (double *)malloc(n*sizeof(double));
    if (x==NULL || y==NULL || xnew == NULL)
    {   free(x);  free(y);  free(xnew);
        return 101;
    }

    /* Initialize arrays */
    nnew = n;
    for(i=0; i<nnew; i++)
    {   xnew[i] = i;
        x[i] = 0;
        y[i] = 0;
        b[i] = 0;
    }

    /* Find envelope around a[] */
    nold = 0;
    inc = 1;
    x[nold] = 0;                /* assign first value */
    y[nold] = a[0];
    nold++;                     /* increment nold for next assignment */
    inc = 0;                    /* reset direction indicator */
    for(i=1; i<n; i++)
    {   if(a[i] >= a[i-1])
        {   inc = 1;            /* inc = 1 while going uphill */
        } else
        {   if (inc == 1)
            {   x[nold] = i-1;
                y[nold] = a[i-1];
                nold++;         /* increment nold for next assignment */
                inc = 0;        /* reset direction indicator */
            }
        }
    }
    if(x[nold-1] < n-1)
    {   x[nold] = n-1;
        y[nold] = a[n-1];
        nold++;
    }
    error = Spline(nold,nnew,x,y,xnew,b);
    if (error)
    {   free(x); free(y);  free(xnew);
        return error;
    }

    /* Free temp storage and return */
    free(x); free(y);  free(xnew);
    return 0;
}
/*************************** SaveGprSliceData() ****************************/
/* Save the slices to disk in the formats requested.
 *
 * Parameters:
 *  struct SliceParamInfoStruct *InfoPtr - adress of information structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <process.h>, <stdio.h>, <stdlib>, <string.h>, "gprslice.h",
 *           "pcx_io.h"
 * Calls: strcpy, _chdrive, chdir, fopen, fclose, fwrite, fprintf, sprintf,
 *        strupr, malloc, ferror.
 * Returns: 0 on success,
 *         >0 on error.
const char *SaveGprSliceDataMsg[] =
{   "SaveGprSliceData(): No errors.",
    "SaveGprSliceData() ERROR: Unable to open output TXT file.",
    "SaveGprSliceData() ERROR: Unable to open output BIN file.",
    "SaveGprSliceData() ERROR: Unable to open output PCX file.",
    "SaveGprSliceData() ERROR: Problem saving TXT data.",

    "SaveGprSliceData() ERROR: Problem saving BIN data.",
    "SaveGprSliceData() ERROR: Problem saving PCX header.",
    "SaveGprSliceData() ERROR: Problem allocating buffer for row of PCX data.",
    "SaveGprSliceData() ERROR: Problem saving PCX data.",
    "SaveGprSliceData() ERROR: Problem saving PCX palette.",

    "SaveGprSliceData() ERROR: DISK I/O error (may be full).",
    "SaveGprSliceData() ERROR: Unable to open output T3D file.",
    "SaveGprSliceData() ERROR: Problem saving T3D data.",
} ;
 *
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: February 24, 1997  August 5, 1997; July 13, 1999; July 20, 1999;
 *       July 28, 1999; September 14, 1999; July 26, 2000;
 *       October 2, 2001;
 */
int SaveGprSliceData(struct SliceParamInfoStruct *InfoPtr)
{
    char str[100],str1[100];
    char *cp;
    char drive[MAX_PATHLEN],newname[MAX_PATHLEN];
    unsigned char uctemp;
    int col,row,layer,num_cols,num_rows,num_layers;
    int driveno,itemp;
    unsigned short ustemp;
    long count;
    double X, Y, Z, amp;
    FILE *outfile;

    num_cols   = InfoPtr->vol_num_cols;
    num_rows   = InfoPtr->vol_num_rows;
    num_layers = InfoPtr->vol_num_layers;

/**** Switch to output directory *****/
    strcpy(str,InfoPtr->out_directory);
    cp = strchr(str,':');
    if (cp != NULL)
    {   *cp = 0;
        strupr(str);
        driveno = 1 + str[0] - 'A';
        _chdrive(driveno);
    }
    chdir(InfoPtr->out_directory);

/***** Write slices to disk *****/
	printf("\r                                                             \r");
    if (InfoPtr->slice_direction == X_AXIS)
    {
        /* For every column (slice) in volume */
        for (col=0; col<num_cols; col++)
        {
            /* TEXT files */
            if (InfoPtr->txt_outfilename[0])
            {   /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->txt_outfilename,col+1);
                newname[8] = 0;
                strcat(strupr(newname),".TXT");
                /* open file */
                outfile = fopen(newname,"wt");
                if (outfile == NULL)
                {   if (log_file) fprintf(log_file,"Problem opening file %s\n",newname);
                	return 1;
                }
                printf("\r                                                             \r");
                printf("Writing slice %d to file %s ...",col+1,newname);

                /* write Z Y amp */
                fprintf(outfile,"\n");
                Z = InfoPtr->Z_start;
                for (layer = 0; layer<num_layers; layer++)
                {   Y = InfoPtr->Y_start;
                    for (row=0; row<num_rows; row++)
                    {
                        /*** Reduce size of amp value to 13 bits */
                        amp = InfoPtr->Volume[col][row][layer] / 8.0;

                        if (fprintf(outfile," %11.4f %11.4f %6d\n",Z,Y,(int)amp) < 0)
                        {   if (log_file)
                                fprintf(log_file,"ERROR encountered saving TXT data\n");
                            fclose(outfile);
                            return 4;
                        }
                        if (ferror(outfile))
                        {   fclose(outfile);
                            return 10;
                        }
                        Y += InfoPtr->Y_step;
                    }
                    Z += InfoPtr->Z_step;
                }
                fclose(outfile);
            }
#if 0
            /* BIN files */
            if (InfoPtr->bin_outfilename[0])
            {   /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->bin_outfilename,col+1);
                newname[8] = 0;
                strcat(strupr(newname),".BIN");

                /* open file */
                outfile = fopen(newname,"wb");
                if (outfile == NULL) return 2;

                printf("\r                                                                    \r");
                printf("Writing slice %d to file %s ...",col+1,newname);
                for (layer=0; layer<num_layers; layer++)
                {   for (row=0; row<num_rows; row++)
                    {   count = fwrite(&(InfoPtr->Volume[col][row][layer]),sizeof(unsigned int),(size_t)1,outfile);
                        if (count < 1)
                        {   if (log_file)
                                fprintf(log_file,"ERROR encountered saving TXT data\n");
                            fclose(outfile);
                            return 5;
                        }
                        if (ferror(outfile))
                        {   fclose(outfile);
                            return 10;
                        }
                    }
                }
                fclose(outfile);
            }
#endif
            /* PCX files */
            if (InfoPtr->pcx_outfilename[0])
            {   struct PcxHdrStruct PcxHdr;
                static unsigned char EncodedBuffer[PCX_BUF_SIZE]; /* [2048] */
                unsigned char *ucrow;
                unsigned char chr = 0x0C;
                int bytes;
                static unsigned char pal[768];

                /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->pcx_outfilename,col+1);
                newname[8] = 0;
                strcat(strupr(newname),".PCX");

                /* open file */
                outfile = fopen(newname,"wb");
                if (outfile == NULL) return 3;
                printf("\r                                                                    \r");
                printf("Writing slice %d to file %s ...",col+1,newname);

                /* SetPcxHeader(int BitsPerPixel,int X1,int Y1,int X2,int Y2,
                       int HorDpi,int VerDpi,int NumBitPlanes,int BytesPerLine,
                       int PaletteType,int HorScrnSize,int VerScrnSize,
                       struct PcxHdrStruct *HdrPtr)
                */
                SetPcxHeader(8,0,0,num_rows-1,num_layers-1,num_rows,num_layers,1,num_rows,1,num_rows,num_layers,
                         &PcxHdr);
                if (fwrite((void *)&PcxHdr,sizeof(struct PcxHdrStruct),(size_t)1,outfile) < 1)
                {   if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX header\n");
                    fclose(outfile);
                    return 6;
                }
                if (ferror(outfile))
                {   fclose(outfile);
                    return 10;
                }

                /* write image data to disk starting at first layer */
                ucrow = (unsigned char *)malloc(num_rows * sizeof(unsigned char));
                if (ucrow == NULL)
                {   fclose(outfile);
                    return 7;
                }
                for (layer=0; layer<num_layers; layer++)
                {   for (row=0; row<num_rows; row++)
                    {   ustemp = InfoPtr->Volume[col][row][layer] / 256;
                        if (ustemp > 255)  ustemp = 255;
                        ucrow[row] = ustemp;
                    }
                    bytes = PcxEncodeScanLine(ucrow,EncodedBuffer,num_rows);
                    if (fwrite((void *)EncodedBuffer,sizeof(unsigned char),(size_t)bytes,outfile) < 1)
                    {   if (log_file)
                            fprintf(log_file,"ERROR encountered saving PCX image\n");
                        fclose(outfile);
                        return 8;
                    }
                    if (ferror(outfile))
                    {   fclose(outfile);
                        return 10;
                    }
                }

                /* write palette to disk */
                fwrite((void *)&chr,sizeof(unsigned char),(size_t)1,outfile);
                jl_spectrum2(pal);        /* green background */
                /* jl_spectrum(pal); */   /* blue background */
                /* jl_spectrum3(pal); */  /* green background  but "sharper" */
                if (fwrite((void *)pal,sizeof(unsigned char),(size_t)768,outfile) < 1)
                {   if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX palette\n");
                    fclose(outfile);
                    return 9;
                }
                if (ferror(outfile))
                {   fclose(outfile);
                    return 10;
                }
                fclose(outfile);
            }
        }  /* end of for (col=0; col<num_cols; col++) loop */
	    printf("\r                                                                \r");
     }  /* end of if (InfoPtr->slice_direction == X_AXIS) */

    if (InfoPtr->slice_direction == Y_AXIS)
    {
        /* For every row (slice) in volume */
        for (row=0; row<num_rows; row++)
        {
            /* TEXT files */
            if (InfoPtr->txt_outfilename[0])
            {   /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->txt_outfilename,row+1);
                newname[8] = 0;
                strcat(strupr(newname),".TXT");
                /* open file */
                outfile = fopen(newname,"wt");
                if (outfile == NULL)
                {   if (log_file) fprintf(log_file,"Problem opening file %s\n",
            	    						newname);
                	return 1;
                }
                printf("\r                                                             \r");
                printf("Writing slice %d to file %s ...",row+1,newname);

                /* write X Z amp */
                fprintf(outfile,"\n");
                Z = InfoPtr->Z_start;
                for (layer = 0; layer<num_layers; layer++)
                {   X = InfoPtr->X_start;
                    for (col=0; col<num_cols; col++)
                    {
                        /*** Reduce size of amp value to 13 bits */
                        amp = InfoPtr->Volume[col][row][layer] / 8.0;

                        if (fprintf(outfile," %11.4f %11.4f %6d\n",Z,X,(int)amp) < 0)
                        {   if (log_file)
                                fprintf(log_file,"ERROR encountered saving TXT data\n");
                            fclose(outfile);
                            return 4;
                        }
                        if (ferror(outfile))
                        {   fclose(outfile);
                            return 10;
                        }
                        X += InfoPtr->X_step;
                    }
                    Z += InfoPtr->Z_step;
                }
                fclose(outfile);
            }
#if 0
            /* BIN files */
            if (InfoPtr->bin_outfilename[0])
            {   /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->bin_outfilename,row+1);
                newname[8] = 0;
                strcat(strupr(newname),".BIN");

                /* open file */
                outfile = fopen(newname,"wb");
                if (outfile == NULL) return 2;

                printf("\r                                                                    \r");
                printf("Writing slice %d to file %s ...",row+1,newname);
                for (layer=0; layer<num_layers; layer++)
                {   for (col=0; col<num_cols; col++)
                    {   count = fwrite(&(InfoPtr->Volume[col][row][layer]),sizeof(unsigned int),(size_t)1,outfile);
                        if (count < 1)
                        {   if (log_file)
                                fprintf(log_file,"ERROR encountered saving TXT data\n");
                            fclose(outfile);
                            return 5;
                        }
                        if (ferror(outfile))
                        {   fclose(outfile);
                            return 10;
                        }
                    }
                }
                fclose(outfile);
            }
#endif
            /* PCX files */
            if (InfoPtr->pcx_outfilename[0])
            {   struct PcxHdrStruct PcxHdr;
                static unsigned char EncodedBuffer[PCX_BUF_SIZE]; /* [2048] */
                unsigned char *ucrow;
                unsigned char chr = 0x0C;
                int bytes;
                static unsigned char pal[768];

                /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->pcx_outfilename,row+1);
                newname[8] = 0;
                strcat(strupr(newname),".PCX");

                /* open file */
                outfile = fopen(newname,"wb");
                if (outfile == NULL) return 3;
                printf("\r                                                                    \r");
                printf("Writing slice %d to file %s ...",row+1,newname);

                /* SetPcxHeader(int BitsPerPixel,int X1,int Y1,int X2,int Y2,
                       int HorDpi,int VerDpi,int NumBitPlanes,int BytesPerLine,
                       int PaletteType,int HorScrnSize,int VerScrnSize,
                       struct PcxHdrStruct *HdrPtr)
                */
                SetPcxHeader(8,0,0,num_cols-1,num_layers-1,num_cols,num_layers,1,num_cols,1,num_cols,num_layers,
                         &PcxHdr);
                if (fwrite((void *)&PcxHdr,sizeof(struct PcxHdrStruct),(size_t)1,outfile) < 1)
                {   if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX header\n");
                    fclose(outfile);
                    return 6;
                }
                if (ferror(outfile))
                {   fclose(outfile);
                    return 10;
                }

                /* write image data to disk starting at last layer */
                ucrow = (unsigned char *)malloc(num_cols * sizeof(unsigned char));
                if (ucrow == NULL)
                {   fclose(outfile);
                    return 7;
                }
                for (layer=0; layer<num_layers; layer++)
                {   for (col=0; col<num_cols; col++)
                    {   ustemp = InfoPtr->Volume[col][row][layer] / 256;
                        if (ustemp > 255)  ustemp = 255;
                        ucrow[col] = ustemp;
                    }
                    bytes = PcxEncodeScanLine(ucrow,EncodedBuffer,num_cols);
                    if (fwrite((void *)EncodedBuffer,sizeof(unsigned char),(size_t)bytes,outfile) < 1)
                    {   if (log_file)
                            fprintf(log_file,"ERROR encountered saving PCX image\n");
                        fclose(outfile);
                        return 8;
                    }
                    if (ferror(outfile))
                    {   fclose(outfile);
                        return 10;
                    }
                }

                /* write palette to disk */
                fwrite((void *)&chr,sizeof(unsigned char),(size_t)1,outfile);
                jl_spectrum2(pal);        /* green background */
                /* jl_spectrum(pal); */   /* blue background */
                /* jl_spectrum3(pal); */  /* green background  but "sharper" */
                if (fwrite((void *)pal,sizeof(unsigned char),(size_t)768,outfile) < 1)
                {   if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX palette\n");
                    fclose(outfile);
                    return 9;
                }
                if (ferror(outfile))
                {   fclose(outfile);
                    return 10;
                }
                fclose(outfile);
            }
        }  /* end of for (row=0; row<num_rows; row++) loop */
	    printf("\r                                                                \r");
     }  /* end of if (InfoPtr->slice_direction == Y_AXIS) */

    if (InfoPtr->slice_direction == Z_AXIS)
    {
        /* For every layer (slice) in volume */
        for (layer=0; layer<num_layers; layer++)
        {
            /* TEXT files */
            if (InfoPtr->txt_outfilename[0])
            {   /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->txt_outfilename,layer+1);
                newname[8] = 0;
                strcat(strupr(newname),".TXT");

                /* open file */
                outfile = fopen(newname,"wt");
                if (outfile == NULL)
                {   if (log_file) fprintf(log_file,"Problem opening file %s\n",
            	    						newname);
                	return 1;
                }

                printf("\r                                                             \r");
                printf("Writing slice %d to file %s ...",layer+1,newname);

                /* write X Y amp */
                fprintf(outfile,"\n");
                Y = InfoPtr->Y_start;
                for (row = 0; row<num_rows; row++)
                {   X = InfoPtr->X_start;
                    for (col=0; col<num_cols; col++)
                    {
                        /*** Reduce size of amp value to 13 bits */
                        amp = InfoPtr->Volume[col][row][layer] / 8.0;

                        if (fprintf(outfile," %11.4f %11.4f %6d\n",X,Y,(int)amp) < 0)
                        {   if (log_file)
                                fprintf(log_file,"ERROR encountered saving TXT data\n");
                            fclose(outfile);
                            return 4;
                        }
                        if (ferror(outfile))
                        {   fclose(outfile);
                            return 10;
                        }
                        X += InfoPtr->X_step;
                    }
                    Y += InfoPtr->Y_step;
                }
                fclose(outfile);
            }
#if 0
            /* BIN files */
            if (InfoPtr->bin_outfilename[0])
            {   /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->bin_outfilename,layer+1);
                newname[8] = 0;
                strcat(strupr(newname),".BIN");

                /* open file */
                outfile = fopen(newname,"wb");
                if (outfile == NULL) return 2;

                printf("\r                                                                    \r");
                printf("Writing slice %d to file %s ...",layer+1,newname);
                for (row=0; row<num_rows; row++)
                {   for (col=0; col<num_cols; col++)
                    {   count = fwrite(&(InfoPtr->Volume[col][row][layer]),sizeof(unsigned int),(size_t)1,outfile);
                        if (count < 1)
                        {   if (log_file)
                                fprintf(log_file,"ERROR encountered saving TXT data\n");
                            fclose(outfile);
                            return 5;
                        }
                        if (ferror(outfile))
                        {   fclose(outfile);
                            return 10;
                        }
                    }
                }
                fclose(outfile);
            }
#endif
            /* PCX files */
            if (InfoPtr->pcx_outfilename[0])
            {   struct PcxHdrStruct PcxHdr;
                static unsigned char EncodedBuffer[PCX_BUF_SIZE]; /* [2048] */
                unsigned char *ucrow;
                unsigned char chr = 0x0C;
                int bytes;
                static unsigned char pal[768];

                /* construct filename */
                sprintf(newname,"%s%02d",InfoPtr->pcx_outfilename,layer+1);
                newname[8] = 0;
                strcat(strupr(newname),".PCX");

                /* open file */
                outfile = fopen(newname,"wb");
                if (outfile == NULL) return 3;
                printf("\r                                                                    \r");
                printf("Writing slice %d to file %s ...",layer+1,newname);

                /* SetPcxHeader(int BitsPerPixel,int X1,int Y1,int X2,int Y2,
                       int HorDpi,int VerDpi,int NumBitPlanes,int BytesPerLine,
                       int PaletteType,int HorScrnSize,int VerScrnSize,
                       struct PcxHdrStruct *HdrPtr)
                */
                SetPcxHeader(8,0,0,num_cols-1,num_rows-1,num_cols,num_rows,1,num_cols,1,num_cols,num_rows,
                         &PcxHdr);
                if (fwrite((void *)&PcxHdr,sizeof(struct PcxHdrStruct),(size_t)1,outfile) < 1)
                {   if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX header\n");
                    fclose(outfile);
                    return 6;
                }
                if (ferror(outfile))
                {   fclose(outfile);
                    return 10;
                }

                /* write image data to disk starting at last row */
                ucrow = (unsigned char *)malloc(num_cols * sizeof(unsigned char));
                if (ucrow == NULL)
                {    fclose(outfile);
                    return 7;
                }
                for (row=num_rows-1; row>=0; row--)
                {   for (col=0; col<num_cols; col++)
                    {   ustemp = InfoPtr->Volume[col][row][layer]/ 256;
                        if (ustemp > 255)  ustemp = 255;
                        ucrow[col] = ustemp;
                    }
                    bytes = PcxEncodeScanLine(ucrow,EncodedBuffer,num_cols);
                    if (fwrite((void *)EncodedBuffer,sizeof(unsigned char),(size_t)bytes,outfile) < 1)
                    {   if (log_file)
                            fprintf(log_file,"ERROR encountered saving PCX image\n");
                        fclose(outfile);
                        return 8;
                    }
                    if (ferror(outfile))
                    {   fclose(outfile);
                        return 10;
                    }
                }

                /* write palette to disk */
                fwrite((void *)&chr,sizeof(unsigned char),(size_t)1,outfile);
                jl_spectrum2(pal);        /* green background */
                /* jl_spectrum(pal); */   /* blue background */
                /* jl_spectrum3(pal); */  /* green background  but "sharper" */
                if (fwrite((void *)pal,sizeof(unsigned char),(size_t)768,outfile) < 1)
                {   if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX palette\n");
                    fclose(outfile);
                    return 9;
                }
                if (ferror(outfile))
                {   fclose(outfile);
                    return 10;
                }
                fclose(outfile);
            }
        }  /* end of for (layer=0; layer<num_layers; layer++) loop */
	    printf("\r                                                                \r");
     }  /* end of if (InfoPtr->slice_direction == Z_AXIS) */

/***** Write T3D volume to disk *****/
    if (InfoPtr->t3d_outfilename[0])
	{   printf("\r                                                             \r");
	    /* Open output file */
		outfile = fopen(InfoPtr->t3d_outfilename,"wb");
		if (outfile == NULL) return 11;

		for (layer=0; layer<num_layers; layer++)
		{   for (row=num_rows-1; row>=0; row--)
			{   for (col=num_cols-1; col>=0; col--)
                {   ustemp = InfoPtr->Volume[col][row][layer] / 256;
                    if (ustemp > 255)  ustemp = 255;
                    uctemp = ustemp;
                    count = fwrite(&uctemp,sizeof(unsigned char),(size_t)1,outfile);
					if (count < 1)
					{   fclose(outfile);
						return 12;
					}
				}
			}
		}

		fclose(outfile);
    }

/***** Write SlicerDicer volume to disk *****/
    if (InfoPtr->sld_outfilename[0])
	{   printf("\r                                                             \r");
	    /* Open output file */
		outfile = fopen(InfoPtr->sld_outfilename,"wb");
		if (outfile == NULL) return 11;
#if 0
		for (layer=0; layer<20; layer++)
		{   for (row=0; row<20; row++)
			{   for (col=0; col<10; col++)
                {   InfoPtr->Volume[col][row][layer] = 0;
				}
			}
		}
puts("corner marked");
#endif
		for (layer=num_layers-1; layer>=0; layer--)
		{   for (row=0; row<num_rows; row++)
 			{   for (col=0; col<num_cols; col++) 
                {   ustemp = InfoPtr->Volume[col][row][layer];
                    count = fwrite(&ustemp,sizeof(unsigned short),(size_t)1,outfile);
					if (count < 1)
					{   fclose(outfile);
						return 12;
					}
				}
			}
		}

		fclose(outfile);
    }

/**** Switch back to original directory *****/
    strcpy(str,InfoPtr->cwd);
    cp = strchr(str,':');
    if (cp != NULL)
    {   *cp = 0;
        strupr(str);
        driveno = 1 + str[0] - 'A';
        _chdrive(driveno);
    }
    chdir(InfoPtr->cwd);

    return 0;
}
void jl_spectrum (unsigned char *pal)
{
    int red,green,blue,i,diff;
    /* unsigned char pal[768]; */

    diff  = 4;
    red   = 255;
    green = 0;
    blue  = 0;
    for (i=0; i<64*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        red   -= diff;
        green += diff;
    }
    red   = 0;
    green = 255;
    blue  = 0;
    for (i=64*3; i<128*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        green -= diff;
        blue  += diff;
    }
    red   = 0;
    green = 0;
    blue  = 255;
    for (i=128*3; i<192*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        green += diff;
        blue  -= diff;
    }
    red   = 0;
    green = 255;
    blue  = 0;
    for (i=192*3; i<255*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        green -= diff;
        red   += diff;
    }
#if 0
    pal[0] = 0;            /* black */
    pal[1] = 0;
    pal[2] = 0;
    pal[255*3]     = 255;  /* white */
    pal[255*3 + 1] = 255;
    pal[255*3 + 2] = 255;
#endif
}
void jl_spectrum2 (unsigned char *pal)
{
    int red,green,blue,i,diff;
    /* unsigned char pal[768]; */

    diff  = 2;
    red   = 255;
    green = 0;
    blue  = 0;
    for (i=0; i<128*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        red   -= diff;
        green += diff;
    }
    red   = 0;
    green = 255;
    blue  = 0;
    for (i=128*3; i<255*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        green -= diff;
        blue  += diff;
    }
#if 0
    pal[0] = 0;            /* black */
    pal[1] = 0;
    pal[2] = 0;
    pal[255*3]     = 255;  /* white */
    pal[255*3 + 1] = 255;
    pal[255*3 + 2] = 255;
#endif
}
void jl_spectrum3 (unsigned char *pal)
{
    int red,green,blue,i,diff;
    /* unsigned char pal[768]; */

    diff  = 5;

    red   = 255;              /* red */
    green = 0;
    blue  = 0;
    for (i=0; i<51*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        green += diff;
    }

    red   = 255;              /* yellow */
    green = 255;
    blue  = 0;
    for (i=51*3; i<102*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        red   -= diff;
    }

    red   = 0;                /* green */
    green = 255;
    blue  = 0;
    for (i=102*3; i<153*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        blue  += diff;
    }

    red   = 0;                /* cyan */
    green = 255;
    blue  = 255;
    for (i=153*3; i<204*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        green -= diff;
    }

    red   = 0;                 /* blue */
    green = 0;
    blue  = 255;
    for (i=204*3; i<255*3; i+=3)
    {   pal[i]   = red;
        pal[i+1] = green;
        pal[i+2] = blue;
        red   += diff;
    }

#if 0
    pal[0] = 0;            /* black */
    pal[1] = 0;
    pal[2] = 0;
    pal[255*3]     = 255;  /* white */
    pal[255*3 + 1] = 255;
    pal[255*3 + 2] = 255;
#endif
}
/*************************** SaveGprSliceInfo() ****************************/
/* Save the slices information to disk.
 *
 * Parameters:
 *  struct SliceParamInfoStruct *InfoPtr - adress of information structure
 *
 * Requires: <process.h>, <stdio.h>, <stdlib>, <string.h>, "gprslice.h",
 *           "pcx_io.h"
 * Calls: strcpy, _chdrive, chdir fopen, fclose, fprintf, sprintf, strupr,
 *        malloc.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *SaveGprSliceInfoMsg[] =
{   "SaveGprSliceInfo(): No errors.",
    "SaveGprSliceInfo() ERROR: Unable to open output TXT file.",
    "SaveGprSliceData() ERROR: Problem saving TXT data.",
} ;
 * Author: Jeff Lucius   USGS   lucius@usgs.gov
 * Date: July 16, 1999; July 23, 1999; July 30, 1999; September 14, 1999
 *       December 22, 1999; July 27, 2000; August 2, 2000;
 */
int SaveGprSliceInfo(struct SliceParamInfoStruct *InfoPtr)
{
    char str[100];
    char *cp;
    char drive[MAX_PATHLEN];
    int i,j,k,layer,num_cols,num_rows,num_layers,slice,num_slices,driveno;
    double Z;
    double val,step;
    FILE *outfile;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;

    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    /* Switch to output directory */
    strcpy(str,InfoPtr->out_directory);
    cp = strchr(str,':');
    if (cp != NULL)
    {   *cp = 0;
        strupr(str);
        driveno = 1 + str[0] - 'A';
        _chdrive(driveno);
    }
    chdir(InfoPtr->out_directory);

    /* calc stats on max/min */
    num_cols = InfoPtr->vol_num_cols;
    num_rows = InfoPtr->vol_num_rows;
    num_layers = InfoPtr->vol_num_layers;

    /* Open text file */
    outfile = fopen(InfoPtr->inf_outfilename,"wt");
    if (outfile == NULL)  return 1;

    /* Write information */
    fprintf(outfile,"\nGPRSLICE v. %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPRSLICE_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    fprintf(outfile,"         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    fprintf(outfile,"cmd_filename = %s\n",strupr(InfoPtr->cmd_filename));
    fprintf(outfile,"input data files [%d]:\n",InfoPtr->num_input_files);

    for (i=0; i<InfoPtr->num_input_files; i++)
    {   fprintf(outfile,"%15s",InfoPtr->dat_infilename[i]);
        if ( (i>0) && !((i+1)%5) ) fprintf(outfile,"\n");
    }
    fprintf(outfile,"\n");

    fprintf(outfile,"out_directory            = %s\n",strupr(InfoPtr->out_directory));
    if (InfoPtr->inf_outfilename[0])
        fprintf(outfile,"inf_outfilename          = %s\n",strupr(InfoPtr->inf_outfilename));
    if (InfoPtr->txt_outfilename[0])
        fprintf(outfile,"txt_outfilename template = %s\n",strupr(InfoPtr->txt_outfilename));
    if (InfoPtr->pcx_outfilename[0])
        fprintf(outfile,"pcx_outfilename template = %s\n",strupr(InfoPtr->pcx_outfilename));
/* disabled
    if (InfoPtr->bin_outfilename[0])
        fprintf(outfile,"bin_outfilename template = %s\n",strupr(InfoPtr->bin_outfilename));
*/
    if (InfoPtr->t3d_outfilename[0])
        fprintf(outfile,"t3d_outfilename          = %s\n",strupr(InfoPtr->t3d_outfilename));
    if (InfoPtr->sld_outfilename[0])
        fprintf(outfile,"sld_outfilename          = %s\n",strupr(InfoPtr->sld_outfilename));
    fprintf(outfile,"Overwrite protection of output files is %s\n",
        InfoPtr->overwrite_protect ? "ON" : "OFF");

    fprintf(outfile,"\nTRANSFORMING PARAMETERS:\n");
    fprintf(outfile,"   X_first = %11.4f    Y_first = %11.4f     Z_first = %11.4f\n",
        InfoPtr->X_first,InfoPtr->Y_first,InfoPtr->Z_first);
    fprintf(outfile,"    X_last = %11.4f     Y_last = %11.4f      Z_last = %11.4f\n",
        InfoPtr->X_last,InfoPtr->Y_last,InfoPtr->Z_last);
    fprintf(outfile," X_columns = %6d          Y_rows = %6d         Z_layers = %6d\n",
        (int)InfoPtr->X_columns,(int)InfoPtr->Y_rows,(int)InfoPtr->Z_layers);
    fprintf(outfile,"cell_Xsize = %11.4f cell_Ysize = %11.4f  cell_Zsize = %11.4f\n",
        InfoPtr->X_step,InfoPtr->Y_step,InfoPtr->Z_step);
    fprintf(outfile," box_Xsize = %11.4f  box_Ysize = %11.4f   box_Zsize = %11.4f\n",
        InfoPtr->box_Xsize,InfoPtr->box_Ysize,InfoPtr->box_Zsize);
    fprintf(outfile,"Method for converting stacked trace in search box:\n");
    switch (InfoPtr->xfrm_method)
    {   case XFRM_NONE:
            fprintf(outfile,"    (as they are)\n");
            break;
        case XFRM_ABSO:
            fprintf(outfile,"    ABSOLUTE VALUE\n");
            break;
        case XFRM_SQRE:
            fprintf(outfile,"    SQUARE\n");
            break;
        case XFRM_INST:
            fprintf(outfile,"    INSTANTANEOUS AMPLITUDE\n");
            break;
        case XFRM_POWR:
            fprintf(outfile,"    INSTANTANEOUS POWER\n");
            break;
    }
    fprintf(outfile,"Start time of first sample is %g ns\n",InfoPtr->start_time);

    if (InfoPtr->envelope)
        fputs("Values from ENVELOPE connecting positive peaks was saved.\n",outfile);
    else
        fputs("No envelope was used.\n",outfile);

    switch (InfoPtr->slice_direction)
    {   case X_AXIS:
            fputs("Slices are vertical planes perpendicular to the X axis.\n",outfile);
            val = InfoPtr->X_start;
            step = InfoPtr->box_Xsize;
            num_slices = InfoPtr->vol_num_cols;
            fprintf(outfile,"Number of slices (X direction)                = %d\n",InfoPtr->vol_num_cols);
            fprintf(outfile,"Number of rows (Z direction) in each slice    = %d\n",InfoPtr->vol_num_layers);
            fprintf(outfile,"Number of columns (Y direction) in each slice = %d\n",InfoPtr->vol_num_rows);
            break;
        case Y_AXIS:
            fputs("Slices are vertical planes perpendicular to the Y axis.\n",outfile);
            val = InfoPtr->Y_start;
            step = InfoPtr->box_Ysize;
            num_slices = InfoPtr->vol_num_rows;
            fprintf(outfile,"Number of slices (Y direction)                = %d\n",InfoPtr->vol_num_rows);
            fprintf(outfile,"Number of rows (Z direction) in each slice    = %d\n",InfoPtr->vol_num_layers);
            fprintf(outfile,"Number of columns (X direction) in each slice = %d\n",InfoPtr->vol_num_cols);
            break;
        case Z_AXIS:
            fputs("Slices are horizontal planes perpendicular to the Z axis.\n",outfile);
            val = InfoPtr->Z_start;
            step = InfoPtr->box_Zsize;
            num_slices = InfoPtr->vol_num_layers;
            fprintf(outfile,"Number of slices (Z direction)                = %d\n",InfoPtr->vol_num_layers);
            fprintf(outfile,"Number of rows (Y direction) in each slice    = %d\n",InfoPtr->vol_num_rows);
            fprintf(outfile,"Number of columns (X direction) in each slice = %d\n",InfoPtr->vol_num_cols);
            break;
    }
    fprintf(outfile,"Original minimum amplitude value = %g\nOriginal maximum amplitude value = %g\n",InfoPtr->Z_min,InfoPtr->Z_max);
    if (InfoPtr->expand)
        fprintf(outfile,"NOTE: amplitude values above were expanded to range from 0 to 65535\n");
    if (InfoPtr->multiply > 0.0 && InfoPtr->multiply != 1.0)
        fprintf(outfile,"NOTE: amplitude values above were multiplied by %g\n",InfoPtr->multiply);
    if (InfoPtr->txt_outfilename[0])
        fprintf(outfile,"NOTE: amplitude values for text file have been divided by 8. Range can be 0 to 8191.\n");
    if (InfoPtr->t3d_outfilename[0])
        fprintf(outfile,"NOTE: amplitude values for T3D binary file have been divided by 256. Range can be 0 to 255.\n");
    if (InfoPtr->sld_outfilename[0])
        fprintf(outfile,"NOTE: amplitude values for SLD binary file can be 0 to 65535.\n");
    switch (InfoPtr->slice_direction)
    {   case X_AXIS:
            fprintf(outfile,"\nX Slice:     FROM          TO\n");
            break;
        case Y_AXIS:
            fprintf(outfile,"\nY Slice:     FROM          TO\n");
            break;
        case Z_AXIS:
            fprintf(outfile,"\nZ Slice:     FROM          TO\n");
            break;
    }
    for (slice=0; slice<num_slices; slice++)
    {   fprintf(outfile," %4d %11.4f %11.4f\n",slice+1,val-(step/2),val+(step/2));
        val += step;
    }

    /* Report on empty cells */
    if (InfoPtr->num_empty_cells > 0)
    {   fprintf(outfile,"\n%d volume cells were empty:\nColumn(X)   Row(Y) Layer(Z)\n",InfoPtr->num_empty_cells);
        for (i=0; i<InfoPtr->vol_num_cols; i++)
        {   for (j=0; j<InfoPtr->vol_num_rows; j++)
            {   for (k=0; k<InfoPtr->vol_num_layers; k++)
                {   if (InfoPtr->VolCell[i][j][k].sum_wgts <= 0.0)
                    {   fprintf(outfile,"  %6d    %6d    %6d\n",
                                i+1,j+1,k+1);
                    }
                }
            }
        }
    }

    /* Close file */
    fclose(outfile);

    /* Switch back to original directory */
    strcpy(str,InfoPtr->cwd);
    cp = strchr(str,':');
    if (cp != NULL)
    {   *cp = 0;
        strupr(str);
        driveno = 1 + str[0] - 'A';
        _chdrive(driveno);
    }
    chdir(InfoPtr->cwd);

    return 0;
}
/***************************** end of GPRSLICE.C *****************************/
