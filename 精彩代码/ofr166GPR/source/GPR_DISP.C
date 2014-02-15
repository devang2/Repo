/******************************** GPR_DISP.C ********************************\
                              version 2.11.06.01
-----------------------------------------------------------------------------
INTRODUCTION

GPR_DISP.EXE displays one or more ground penetrating radar (GPR) files to the
CRT as an 8-bit gray-scale image or as wiggle traces and optionally saves the 
display to disk as an Encapsulated PostScript (EPS) file or as a PCX graphics 
file. The primary purpose is to visualize GPR data before and after 
manipulation and to produce graphics files that are suitable for printing and 
publication. The best print quality will be with EPS files. These files, 
however, are not readily read by graphics or paint programs, except for Corel 
PhotoPaint. EPS files are gray scale only. PCX files are a copy of the screen 
image - B&W or color. See details below. Press the Esc key to end the program 
after the image is displayed.

The input to GPR_DISP.EXE is a "CMD" file, an ASCII text file containing 
keywords (or commands or parameters) describing how to display the radar 
data. An optional "override" or "global" command file may be specified on the 
DOS command line that will take precedence over values found in the other 
command files.

The GPR data can be read from disk using the following formats:\
  - GSSI SIR-2, SIR-2000, and SIR-10 binary "DZT" files,
  - Sensors and Software pulseEKKO "DT1" and "HD" files, or
  - Society of Exploration Geophysicists SEG-Y formatted files.

If the storage format does not conform to any of the above or GPR_DISP has 
trouble reading the file correctly, there are options for the user to supply 
the required information.

A message file called GPR_DISP.LOG is opened when the program starts. It is 
located either in the directory where the program was called from or in the 
root directory of drive C. In multitasking environments, this may prevent 
more than one session of the program from executing in the same directory. 
The log file may contain more information regarding the failure or success of 
GPR_DISP as it executes. Sessions are appended at the end of the log file.

GPR_DISP.EXE is a protected-mode MS-DOS program that executes within the DOS 
operating system or within DOS (or console) windows in Microsoft Windows 
3.x/95/98. It will probably not execute within a console window in Windows 
NT. This program will force the computer CPU to work in "protected mode", 
that is, all memory can be utilized, not just the 1 MB that DOS normally has 
access to. If an error message occurs when trying to run GPR_DISP.EXE it may 
be because the requested amount of "protected mode" memory (the region size)
is set too low for the program or it is set to more than what is available on 
the computer. Use the program MODXCONF.EXE, which should have been
distributed with this software, to adjust the region size.

NOTE: Files used by this program must follow the naming conventions required 
by early versions of MS-DOS. Filenames can be only eight (8) characters long, 
with no embedded spaces, with an optional extension of up to three 
characters. This program will run in a Windows 95/98 console where filenames 
do not have to follow this convention. Unexpected results may occur if input 
file names, including the CMD file, are longer than 8 characters. For 
example, both command files dispfile.cmd and dispfile1.cmd will be seen as 
dispfile.cmd.
-----------------------------------------------------------------------------
IMAGE/DATA PROCESSING

When the GPR data are read into this program, they are immediately converted 
to 8-bit unsigned integers (values 0 to 255). This may cause some unexpected 
results if the GPR data are 16 bit but have a low dynamic range. Sixteen-bit
numbers are divided by 256 to get 8-bit numbers. If the data amplitude range 
is some low multiple of 256 only a few values will survive the conversion. In 
cases when the data range is less than 256 then 1 or no values may survive.

The CRT image presents the GPR data in one of five modes: trace/time, 
time/time, distance/time, distance/distance, or trace/sample.
  - Trace/time images simply display the data as adjacent traces (horizontal
    axis is trace number and vertical axis is sample time in nanoseconds,
    ns).
  - Trace/sample images display the data as adjacent traces, except the
    vertical axis is the sample number (rather than sample time).
  - Time/time images display the traces as equally located in time
    horizontally (seconds) and vertically (ns).
  - Distance/time images place the GPR traces in the appropriate location
    along the horizontal axis (in meters), with no elevation correction. The
    vertical axis is sample time, or travel-time, in ns.
  - Distance/distance images have been geometrically adjusted so traces are
    placed "correctly" in 2-D space with a (multi-)layer time-to-depth
    conversion applied (X- and Z-axes are in meters).

Some fundamental image processing operations are available to refine the 
presentation of the data.

 - Point processes (using look-up tables):
    - image contrast stretching (either of data or using look-up table)
      to enhance subtle features
    - image contrast compression to enhance strong reflectors
    - local contrast stretching (the endpoints of the gray scale remain the
      same but the rate of change of the middle portion is either greater
      or lessor than 1 to 1)
    - image brightening or darkening
    - negative of image
    - EPC recorder-style images (white is at the midpoint value and black
      occurs at both endpoints of the 8-bit range, with a gray-scale between)

 - Geometric processes
    - scaling (with interpolation or elimination if neccesary)
    - mirroring (data from disk may be displayed in reverse order and/or
      "upside down")

Some Hilbert transform processes are available. The instantaneous amplitude 
and instantaneous power can be calculated and displayed as is or wrapped by 
an envelope. 

"Background" removal is available. An average radar trace is calculated by 
adding all traces in the "window" together, sample by sample, and then
dividing each sample by the number of traces. This average trace is then 
normalized to have zero as the midpoint and is subtracted from each trace in 
the "window". This operation may not be appropriate for some data sets, 
especially those with few traces (a few hundred or less) or with outstanding 
"horizontal" features.

Data amplitude gain modification is available.  Gain may be removed or added 
or both.  The user supplies a set of gain values in decibels as 20 x 
log(ratio), where log is to base 10.  For example, to multiply data by 1000, 
a decibel value of 20 x log(1000) or 60 is used.  To decrease data by 10 (ie. 
multiply by 0.10), a decibel value of 20 x -1 or -20 is used.  A decibel 
value of 96 is approximately equivalent to a ratio of 65535:1 (or 2^16, the 
dynamic range of 16-bit data).

For example, to multiply all amplitudes by 2 use the following keywords.
change_gain = "TRUE"
num_gain_on = 2
gain_on[]   = 6 6     ; 20 x log(2) = 6

For users of GPR_PROC and GPRSLICE, GPR_DISP allows you to visualize data
manipulations before creating a new data file or slicing a group of files.

For velocity analysis, a small-sperical-object reflection hyperbola can be
overlaid on the image. Antenna seperation, object horizontal location, depth,
and radius, and the GPR wave velocity must be supplied. The velocity is in m/ns.
If you know RDP, then take the square root of the RDP and divide that into
0.2998 m/ns (the velocity of light in a vacuum).

-----------------------------------------------------------------------------
GRAPHICS FILES

Images can be stored optionally to disk using the Encapsulated PostScript
level 2 storage format, in either landscape or portrait paper orientation, or
using the PCX graphics file format as black and white or color images. Once
the data are displayed to the screen, the color/gray scale can be changed by
pressing the following keys.
   G (gray scale - the default), S (spectrum), R (reverse color/gray scale),
   1 (custom color palette 1), 2 (custom color palette 2),
   3 (custom color palette 3).

Only PCX files will preserve the color scale. EPS files are gray scale only.
PCX files are a direct copy of the pixels shown on the screen - image or
wiggle trace. The quality of the letters/numbers will not be as nice as with
EPS output using the Hershey fonts. Make sure that the root directory on the
C: drive and the drive you are running GPR_DISP from has a directory named
"hershey" and that it contains the font files. These files are available in
the anonymous ftp area of our server musette.cr.usgs.gov.

NOTE: To obtain a PCX file of only the GPR data, set the viewport parameters
to the full limits of the screen and do not add axes, title, or labels.
    SET:  vx1 = 0.0   vx2 = 133.333   vy1 = 0.0   vy2 = 100.0

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
May 19, 1994       - Completed DISPLAYM as a revision of DISPLAY1.C.
June 1, 1994       - Removed restriction on viewport alignment.
                   - Changed displaym.msg file to open for append rather
                     than erasing old contents.  Added date and time to file.
June 8, 1994       - Changed message file to have current date and time.
                   - Moved "Esc to Quit" inside "first_time" block only.
June 21, 1994      - Replaced TrimStr() with later version.
June 22, 1994      - Changed some formatting for easier reading.
                   - Added option to "square" (then rescale to 0 - 255)
                     the gray scale to enhance strong reflectors.
                   - Added SEG-2 option (place holders) but no code.
                   - Replaced lots of "size_t"s with "long"s.
June 23, 1994      - Fixed problem allocating "tick_xyz[][]" in
                     GetParameters() when reading from a RIF file, and
                     problem de-allocating it in DeallocInfoStruct().
                   - Fixed problem in GetRifFile().
                   - Finished cleaning out "size_t"s (replaced with "long").
September 30, 1994 - Minor change in DisplayParameters().
                   - Corrected numerous function argument type mis-matches
                     with protypes.
March 8, 1995      - Removed the requirement in GetDztChSubImage8() that
                     the first sample of the first trace to have all bits
                     set (GSSI SIR-10 data).  To check that bits are set,
                     #define VERIFY_SYNC before compilation.
March 10, 1995     - Fixed problem with EPS "caption" option (122),
                     modified serch for semicolon within double quotes, and
                     now check for empty strings.  All in GetCmdFileArgs().
March 13, 1995     - Modified DrawAxis() to allow plotting of titles for
                     left and right axes when ano_tick is turned off.
                   - Minor change in DisplayParameters().
                   - Fixed problem with EPS "caption" option (122) in
                     GetCmdFileArgs().
                   - Allowed user to specify video_mode equal to 0 (on
                     command line) if graphics chip type unknown.
March 14, 1994     - Made sure that a valid video mode is given by user in
                     GetParameters() and that it is done only once.
March 23, 1995     - Removed requirement that GSSI DZT files strictly
                     match version 5.x of the software.
March 27, 1995     - Added option for user to supply information about data
                     when it does not match any supported storage format.
                   - Modified LabelViewportsEPS() to allow help screen to
                     appear in mode 56 and 261.
March 30, 1995     - Replaced DZT functions with those that recognize
                     old style (512-byte) headers and format.
September 15, 1995 - Updated DrawAxis() to allow ticks on "inside" of
                     axis and implemented shifting, "offset", of title.
                   - Modified LabelViewportsEPS() to write better EPS files.
                   - Removed video_mode from command line arguments.
                   - Now allow command filename to not include the extension.
                   - Fixed incorrect algorithm in LocalStretchTable8().
October 10, 1995   - Modified GetSubImage8() to read "screen image" data,
                     ie. data stored row-by-row.
                   - Modified PrintUsageMsg().
October 16, 1995   - Modified LabelViewportsEPS() to adjust VX1 and VMAX
                     for smaller limits of portrait mode.
October 25, 1995   - Renamed program to GPR_DISP.
                   - Modified comments in the CMD file and here.
                   - Forced layer_val[0] to be 3 if num_layers == 1 in
                     GetParameters().
                   - Removed "draw_clippath" from program.
                   - Renamed err_file to log_file; and filename to
                     gpr_disp.log; and added message in file stating the
                     command file being processed.
                   - Changed "patience" message to be displayed for each
                     command file.
                   - Changed PrintUsageMsg() to change screen color when
                     ANSI.SYS is loaded.
October 26, 1995   - Changed DisplayParameters() to only display
                     inf_infilename when storage format is DT1.
                   - Now verify viewport limits in GetParameters().
                   - Removed "DZT header inconsistent" message from
                     GetParameters().
                   - Changed PrintUsageMsg().
October 27, 1995   - Removed support for DAT, GPG, GSG, and SG2 files.
                   - Added file-type detection routine to GetParameters().
                   - Added output to log file for all printf (except Debug).
October 30, 1995   - Use GetSubImage8() to read S&S and SEG-Y data files.
                   - Modify GetSubImage8() to correctly convert "12-bit"
                     and "24-bit" data to 8-bit unsigned, and to handle
                     floating point data (see GetParameters() too).
                   - Removed DZT functions from this source and placed
                     in dzt_io.c.
October 31, 1995   - Removed SGY functions from this source and placed
                     in sgy_io.c.
                   - Removed DT1 functions from this source and placed
                     in dt1_io.c.
November 1, 1995   - Split up giant switch statement in GetCmdFileArgs()
                     into 50-case or less blocks.
                   - Removed GetMrkData(), GetXyzData(), GetGprFileType(),
                     GetSubImage8(), and ExtractSsInfoFromSegy() and
                     placed in gen_io.c.
                   - Fixed problem in GetDataAsImage8() - forgot breaks.
November 2, 1995   - Added <dos.h> to gpr_disp.h.
November 6, 1995   - Added option to skip traces on input and in GetSubImage8()
                     in gen_io.c and GetDztChSubImage8 in dzt_io.c, and had
                     to modify GetCoordRanges(), GetParameters(), and
                     DisplayImage8() to account for skipping traces.
                   - Improved efficiency of InitGraphicsARRAY().
                   - Fixed problem when next_cmd_filename doesn't exist;
                     program was substituting .cmd!
November 7, 1995   - Added option to display markers wider than 1 pixel
                     column in DisplayImage8().
                   - Fixed how ant_freq found for DZT files in GetParameters().
                   - Added version number (1.0) to PrintUsageMsg().
                   - Added more info to help screen.
                   - Modified GetDoordRanges() for mode 4 (traverse dist.)
                     so can display in reverse (0 on right of window).
                   - Changed ParamInfoStruct to DispParamInfoStruct to avoid
                     confusion and problems with other programs/functions.
                   - Added coord_mode 5 (trace vs. sample) and modified
                     GetParameters(), GetDataLimits(), and DisplayImage8().
                   - Forced lock_first_samp for all storage format types
                     GetParameters().
                   - Fixed bug in GetCmdFileArgs() at case 100 - break
                     was missing.
                   - Changed way first_samp_time and last_samp_time
                    determined in GetParameters() and GetCmdFileArgs().
                   - Added total_time to "essential" list in GetParameters().
                   - Changed way first_samp calculated in GetParameters()
                     for DZT files to round to nearest whole number.
                   - Replaced all comparison of coord_mode to numbers with
                     manifest constant names.
November 8, 1995   - Changed GetParameters() to not terminate if
                     ReadOneDztHeader() returns 4 or 5 (non-standard header
                     values but still might be correct).
November 20, 1995  - Recompiled using latest gpr_io.lib and changed
                     GetParameters() to not terminate if ReadOneDztHeader()
                     returns "WARNING" in message string.
November 22, 1995  - Added option(s) to display data as wiggle traces
                     instead of an image, for all coord_mode's - new
                     function DisplayWiggle8().
November 27, 1995  - Added option(s) to superimpose wiggle traces on the
                     gray-scale image.  Changed DisplayImage8() so that
                     for coord_mode == 2, Vvals now holds plotted vertical
                     pixel locations.
                   - Added option to clip wiggle trace as a percentage
                     of wiggle_width.
                   - Changed version number to 1.10.
November 28, 1995  - Added options to remove/add range gain.
November 29, 1995  - Remove lookup table functions (5) and make new
                     library GPR_IFX.LIB which must now be linked in.
                   - Discovered that libhpgl.lib functions must remain with
                     the source code.  Apparently values must be set at
                     run-time for the library to function properly -
                     globals ??.
                   - Renamed functions that require 8-bit unsigned data to
                     end in 8u.
                   - Corrected problem when clipping wiggle (_LIMIT),
                     wasn't checking for "reversed" x-axis.
December 1, 1995   - Corrected problem in GetParameters() - video mode was
                     beeing checked before getting command file.
                   - Fixed problem in GetCmdFileArgs() case 1 when assigning
                     inf_infilename.
                   - Fixed problem in GetParameters() - was not copying
                     essential file info (like header bytes) into the
                     DispParamInfoStruct structure - required for getting
                     all data except DZT.
                   - Changed DisplayPrameters() to allow user to skip
                     display of axes labels, etc.
December 6, 1995   - Now can use step_size and start_position for DT1 and
                     SGY files to calculate tick_tracenum and tick_xyz if
                     coord_mode 2 or 4 requested - in GetParameters(). But,
                     Y and Z values are assigned 0.0 and horiz_mode is
                     forced to 1!
                   - Now assign axes titles default values in GetParameters()
                     based on coord_mode, if not assigned in command file.
December 12, 1995  - Changed LabelViewportsEPS() so that a message is sent
                     to the log file if fprintf() to the EPS file fails.
                   - Moved global declarations to gpr_disp.c from gpr_disp.h
                     and placed extern references to them in gpr_disp.h.
                   - Removed many variables from functions that were
                     declared but never used or assigned but never used.
                   - Changed version number to 1.11.
                   - In main(), now don't send message to log file about
                     which command file being processed if argv[1] == NULL;
                   - Removed fopen() attempt for adding ".cmd" from
                     GetParameters() and restrict to GetCmdFileArgs().
December 13, 1995  - Added Sound() when program fails.
December 15, 1995  - Added Sound (Beep) when done writing EPS file.
                   - Added global ANSI_THERE and only check once for
                     ansi.sys (plus wasn't checking properly before!).
                   - Removed default axis titles labels.
                   - Changed show_markers in DisplayImage8() to be pixels
                     vertically also.
January 17, 1996   - Added version # as a manifest constant in GPR_DISP.H
                     and changed main() and PrintUsageMsg() to use this
                     value.
                   - Removed Sound() except for errors.
                   - Changed OTHER to USER_DEF.
January 18, 1996   - Changed version number to 1.2.
                   - Added code to save screen image as a PCX file.
                   - Made save_eps and save_pcx globals.
January 23, 1996   - PCX color palettes are also now available.
Januray 25, 1996   - GetCmdFileArgs() checks that input data file specified.
January 30, 1996   - Missing break found in SGY section of GetParameters().
February 5, 1996   - Removed functions ANSI_there(), GetDosVersion(),
                     Sound(), Spline(), and TrimStr() and placed in
                     JL_UTIL1.C. - JL_UTIL1.LIB must now be linked in at
                     compile time.
                   - Changed version to 1.21.
                   - Got "incompatible types" error after put TrimStr()
                     into jl_util1, so had to cast return value to (char *)
                     when assigning to char * in GetCmdFileArgs() - ??.
May 23, 1996       - Add option to overlay horizontal and/or vertical lines
                     on the GPR data within a window.
June 7, 1996       - Fixed inconsistency in GetParametersMsg[] reporting.
June 12, 1996      - Fixed problem with range checking if vertical or
                     horizontal overlay lines added, in GetParameters().
                   - Added (hpgl.h) line styles for overlay lines.
                   - Improved "bad condition" checking in LabelViewportsEPS()
                     for overlay line plotting.
June 19, 1996      - Fixed problem in GetDztChSubImage8() (in DZT_IO.C) when
                     in skip trace loop EOF encountered - now call rewind
                     to reset EOF error indicator before continuing.
June 20, 1996      - Fixed problem in GetCmdFileArgs() hor/ver overline
                     parameters were restricted to postive values.
June 24, 1996      - Modified how pcx_outfilename handled in GetCmdFileArgs().
                   - Add option to use large paper format HP DesignJet 750
January 28, 1997   - Fixed problem in GetParameters() when using user-defined
                     storage formats. GetGprFileType() was over-writing good
                     values.
June 6, 1997       - Corrected problem in GetParameters() when
                     override_command_file is present. Now ignore error
                     message 3 from GetCmdFileArgs().
July 1, 1997       - Forced InfoPtr->layer_val[InfoPtr->num_layers-1] to be
                     3 for any number of layers in GetParameters().
                   - In GetCndFileArgs(), only look for num_layers-1
                     layer_vals.
August 11, 1997    - Added support for modified DZT formats.
September 30, 1997 - Allowed only one trace to be displayed if in wiggle
                     mode.
July 1, 1999       - Added jl_spectrum3,
July 16, 1999      - Updated revision to 1.4.
                   - Added instantaneous amplitude +/- envelope to options.
                   - Updated use and location of log file.
                   - Removed usage of eps_copies from command file.
                   - Updated documentation by putting descriptions into
                     GPR_DISP.TXT and using old GPR_DISP.CM_ as GPR_DISP.CMD.
                   - Removed option to use HP DesignJet 750.
                   - The user must now supply MRK and XYZ files when required.
                     They cannot be entered from the keyword file.
August 6, 1999     - Modified calculation of instantaneous amplitude.
August 10, 1999    - Added instantaneous amplitude with envelope option to
                     display options.
August 19, 1999    - Now multiply inst amp by sqrt(2) before copying into
                     image to improve dynamic range
August 23, 1999    - Added instantaneous power/energy.
August 25, 1999    - Standardardize inst amp with call to InstAttribTrace()
                     in jl_util1.lib.
September 1, 1999  - Added error checking for axes labels/ticks in
                     GetParameters().
                   - Verified that first_samp can only be >=0 because IDs
                     first sample to use from trace.
                   - Wrote a real histogram stretch routine. Old one was bull
                     and never worked right (GRO routine).
September 2, 1999  - Updated revision to 2.00.
                   - Standardardize inst pow with call to InstAttribTrace()
                     in jl_util1.lib.
                   - Update documentation and its format here.
                   - Added InstAmpImage8u() and InstPowImage8u() to
                     DisplayWiggle8().
September 3, 1999  - Added  AbsValImage8u() and SquareImage8u() to
                     DisplayImage8() and DisplayWiggle8(). Removed squaring of
                     lookup table entries.
                   - Updated documentation files.
September 14, 1999 - Updated documentation files.
August 28, 2000    - Made Y2K time/date corrections.
                   - Changed way version done - added date to number.
September 6, 2000  - Added error check for horiz_start == horiz_stop in
                     GetCoordRanges().
                   - Removed bug where num_layers could be > 0 if coord mode=4
                     in GetParameters(). Was causing a fault.
November 6, 2001   - Add routines to plot velocity hyperbola on image.
                   - Add fill(), unfill(), and rectangle() to GetLblFileArgs().
                   - Change "channel" input from user to be 1 as first channel,
                     was 0 was first. GetCmdFileArgs() immediately subtracts 1
                     and DisplayParameters() adds one for display only.

KNOWN BUGS:
  November 6, 1995 - A value of skip_traces can be selected that will
                     cramp memory availability and somehow cause a heap
                     overwritten error when attempting to free hpgl_array_8
                     or cause the skip loop to fail (EOF error).
                     It's possible that plot_pixel() may plot outside of
                     array boundaries, but I think I check all Px and Py
                     used.  Solution is to increase skip_traces so that
                     the number of image[] columns approaches the number
                     screen columns available (1024 max), or to lower
                     it so that image[] cannot be allocated and a suggested
                     value is computed and displayed, which can be used.
                     (I think I fixed this bug on June 19, 1996)
  June 16, 1997    - It's possible to get a heap block overwritten error
                     when generating PCX files.  Again, I think it's related
                     to the graphics library.

Supported compilers:
   Intel 386/486 C Code Builder 1.1a
     To compile for 80386 executable:
       icc /F /xnovm /s010000H  /zfloatsync gpr_disp.c libhpgl.lib gpr_io.lib
           gpr_ifx.lib pcx_io.lib jl_util1.lib
     To compile for 80486 executable :
       icc /F /xnovm /s010000H /zfloatsync /zmod486 gpr_disp.c libhpgl.lib
           gpr_io.lib gpr_ifx.lib pcx_io.lib jl_util1.lib

     /F removes the floating point emulator, reduces binary size.
     /xnovm specifies no virtual memory manager, disk access is too slow.
     /s010000H allocates 64K of stack space (required by graphics module).
       NOTE: This may have to be increased to 1 MB (/s0100000H) to
             accomodate some imaging routines in the graphics module.
     /zfloatsync insures the FPU is operand-synchronized with the CPU.
     /zmod486 generates specific 80486 instructions, speeds up program.
     libhpgl.lib  - Gary Olhoeft's 32-bit graphics modules.
     gpr_io.lib   - Jeff Lucius's GPR I/O functions (DZT, DT1, SGY).
     gpr_ifx.lib  - Jeff Lucius's image processing lookup table functions.
     pcx_io.lib   - Jeff Lucius's PCX storage format I/O functions.
     jl_util1.lib - Jeff Lucius's utility functions.

     to remove assert() functions from code also add:
     /DNDEBUG

 To run: GPR_DISP cmd_filename [override_cmd_filename]
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
 * main                                           (con I/O) (graphics)
 *  LOOP
 *   +-- GetParameters                            (con I/O)
 *   |     | These functions are responsible for reading the user-supplied
 *   |     | parameters from the command (or parameter) file specified on the
 *   |     | command line, opening the GPR data/info files to get additional
 *   |     | information about the data sets, and reading in any tick mark
 *   |     | and/or X-Y-Z data that may be required.  Hvals[], Vvals, and
 *   |     | Zvals[] are also allocated and assigned here.  These three arrays
 *   |     | are used to place the GPR data into hpgl_array_8[][].  The limits
 *   |     | of the user screen window are also set.
 *   |     +-- PrintUsageMsg                      (con I/O)
 *   |     +-- InitParameters
 *   |     +-- GetCmdFileArgs          (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetGprFileType          (disk I/O)
 *   |     +-- ReadOneDztHeader        (disk I/O)
 *   |     +-- InitDt1Parameters
 *   |     +-- GetSsHdFile             (disk I/O)
 *   |     +-- PrintSsHdInfo                      (con I/O)
 *   |     +-- ReadSegyReelHdr         (disk I/O)
 *   |     +-- PrintSegyReelHdr                   (con I/O)
 *   |     +-- TrimStr
 *   |     +-- GetOtherInfo            (disk I/O)
 *   |     +-- GetMrkData              (disk I/O)
 *   |     +-- GetXyzData              (disk I/O)
 *   |     |     +-- TrimStr
 *   |     +-- GetCoordRanges                     (con I/O)
 *   |     |     | This function is called if coord_mode == 2 or 4 to establish
 *   |     |     | first and last traces based on "distances" given by user.
 *   |     |     | "Vertical" (sample space) values are also assigned for
 *   |     |     | plotting if num_layers == 1, or coord_mode ==4.
 *   |     |     +-- Spline
 *   |     |            This function performs a cubic spline interpolation.
 *   |     +-- DeallocInfoStruct
 *   |     +-- GetDataLimits
 *   |             This function determines the user limits of the screen
 *   |             window if not specified by the user.
 *   +-- DisplayParameters                        (con I/O)
 *   |       This function displays the processing parameters to the user on
 *   |       the CRT screen.
 *   +-- GetDataAsImage8               (disk I/O) (con I/O)
 *   |     | These functions read in the requested parts of the GPR data and
 *   |     | convert to 8-bit (stored in the "image" member of the info
 *   |     | structure).  The "image" array is allocated here.
 *   |     +-- DeallocInfoStruct
 *   |     +-- GetDztChSubImage8       (disk I/O)
 *   |     +-- GetSubImage8            (disk I/O)
 *   +-- InitGraphicsARRAY                                  (graphics)
 *   |       This function allocates storage for the global hpgl_array_8[][]
 *   |       and initializes the graphics system to plot to that array.
 *   |       All values in the array are pre-set to either 0xFF (white) or
 *   |       0x00 (black), depending if EPS output requested or not.
 *   |       It is called only for first pass through loop.
 *   +-- SetDataViewport                                    (graphics)
 *   |       This function scales the viewport so that vertical units are the
 *   |       same as horizontal units for the "geometrically corrected"
 *   |       display option (coord_mode == 2).  The viewport may shrink to
 *   |       insure same scales but it won't expand.
 *   +-- DisplayImage8                            (con I/O) (graphics)
 *   |       These functions perform a few fundamental image processing
 *   |       operations on the data, optionally remove an average "background"
 *   |       waveform, and place the GPR data in the appropriate area of
 *   |       hpgl_array_8[][] using Hvals[], Vvals, and Zvals[] after converting
 *   |       to screen pixel space.  It is here where marks are added to image
 *   |       if show_markers is true.
 *   |     +-- InitTable8
 *   |     +-- ChangeRangeGain8u
 *   |     +-- RemAvgColumn8u
 *   |     +-- AbsValImage8u
 *   |     +-- SquareImage8u
 *   |     +-- InstAmpImage8u
 *   |     |     +-- FFT1D
 *   |     |     +-- Envelope
 *   |     |           +-- Spline
 *   |     +-- InstPowImage8u
 *   |     |     +-- FFT1D
 *   |     |     +-- Envelope
 *   |     |           +-- Spline
 *   |     +-- HistStretchImage8u
 *   |     +-- LocalStretchTable8
 *   |     +-- BrightenTable8
 *   |     +-- ReverseTable8
 *   +-- DisplayWiggle8                           (con I/O) (graphics)
 *   |       These functions perform a few fundamental image processing
 *   |       operations on the data, optionally remove an average "background"
 *   |       waveform, and display the GPR data as wiggle traces.
 *   |     +-- InitTable8
 *   |     +-- ChangeRangeGain8u
 *   |     +-- RemAvgColumn8u
 *   |     +-- AbsValImage8u
 *   |     +-- SquareImage8u
 *   |     +-- InstAmpImage8u
 *   |     |     +-- FFT1D
 *   |     |     +-- Envelope
 *   |     |           +-- Spline
 *   |     +-- InstPowImage8u
 *   |     |     +-- FFT1D
 *   |     |     +-- Envelope
 *   |     |           +-- Spline
 *   |     +-- HistStretchImage8u
 *   |     +-- LocalStretchTable8
 *   |     +-- BrightenTable8
 *   |     +-- ReverseTable8
 *   +-- PlotSelectWiggles8                       (con I/O) (graphics)
 *   +-- DeallocInfoStruct
 *   |       This function deallocates all storage in the information
 *   |       structure (including the image) except for "caption" and
 *   |       "pagenum" members.  The structure itself is deallocated at the
 *   |       end of main().
 *  GO TO LOOP if another command file specified

 *   +-- LabelViewportsEPS             (disk I/O) (con I/O) (graphics)
 *   |     | These functions "copy" the GPR data from hpgl_array_8[][] to the
 *   |     | CRT screen, add the axes, and add any vector plotting provided
 *   |     | in a "LBL" file.  The GPR data are written to the EPS file (if
 *   |     | requested) as copies of the entire CRT screen image before vector
 *   |     | graphics are performed.  vector graphics and labels may be placed
 *   |     | within the viewport boundaries (including on the image data) as
 *   |     | well as outside the viewport.
 *   |     +-- GetLblFileArgs                               (graphics)
 *   |     +-- DrawAxis                                     (graphics)
 *   |     |     +-- SetLabel
 *   |     +-- GPR_DISP_HelpScreen                  (con I/O)
 *   |     +-- jl_spectrum
 *   |     +-- jl_spectrum2
 *   |     +-- jl_spectrum3
 *   +-- LabelViewportsPCX             (disk I/O) (con I/O) (graphics)
 *   |     | These functions "copy" the GPR data from hpgl_array_8[][] to the
 *   |     | CRT screen, add the axes, and add any vector plotting provided
 *   |     | in a "LBL" file.  The GPR data are written to the PCX file as
 *   |     | copies of the entire CRT screen image.
 *   |     +-- GetLblFileArgs                               (graphics)
 *   |     +-- DrawAxis                                     (graphics)
 *   |     |     +-- SetLabel
 *   |     +-- GPR_DISP_HelpScreen                  (con I/O)
 *   |     +-- jl_spectrum
 *   |     +-- jl_spectrum2
 *   |     +-- jl_spectrum3
 *   |     +-- SetPcxHeader
 *   +-- ANSI_there                                              (DOS)
 *   |     +-- GetDosVersion                                     (DOS)
 *   +-- Sound                                                        (port I/O)
 *   +-- TerminateGraphicsARRAY                             (graphics)
 *   |       Calls plotter_off() and deallocates hpgl_array_8[][].
 */
/******************* Includes all required header files *********************/
#include "gpr_disp.h"

/**************************** Global variables ******************************/
int Debug = FALSE;               /* if TRUE, turn debugging on */
int Batch = FALSE;               /* if TRUE, supress interaction with user */
int Batchset,Debugset,Videoset;  /* Only use values from first command file */
int save_eps,save_pcx;           /* Only use values from first command file */
int ANSI_THERE = FALSE;          /* TRUE if ANSI.SYS loaded */

/* error and message file that all functions have access to */
FILE *log_file = NULL; /* pointer to FILE struct */

/* The array below is used to read in values from an ASCII file that contains
 * HPGL expressions as "C" code (typically a "LBL" file).
 */
const char *HPGL_LBLS[] =
{   "viewport","window","pen","ldir","csize",
    "lorg","plot","label","hpgl_select_font","frame",
    "clip","unclip","linetype","circle","arc",
    "wedge","ellipse","fill","unfill","rectangle",
    NULL,
} ;
/* The array below is used to read in values from an ASCII file that contains
 * control parameters.
 */
const char *GPR_DISP_CMDS[]=
{   "data_infilename","lbl_infilename","mrk_infilename","xyz_infilename","eps_outfilename",
    "channel","first_samp","last_samp","trace_per_sec","num_bad",
/* 10 */
    "bad_traces[]","abs_val","tick_tracenum[]","tick_xyz[][]","num_layers",
    "layer_rdp[]","layer_mode[]","layer_val[]","title","video_mode",
/* 20 */
    "stretch","range","epc_threshold","epc_contrast","epc_gain",
    "epc_sign","max_data_val","use_11x17","vert_exag","vx1",
/* 30 */
    "vx2","vy1","vy2","left","right",
    "bottom","top","coord_mode","horiz_mode","horiz_start",
/* 40 */
    "horiz_stop","debug","batch","laxis_title","laxis_max",
    "laxis_min","laxis_tick_show","laxis_tick_start","laxis_tick_int","laxis_tick_num",
/* 50 */
    "laxis_tick_ano","laxis_tick_skip","laxis_title_size","laxis_ano_size","raxis_title",
    "raxis_max","raxis_min","raxis_tick_show","raxis_tick_start","raxis_tick_int",
/* 60 */
    "raxis_tick_num","raxis_tick_ano","raxis_tick_skip","raxis_title_size","raxis_ano_size",
    "taxis_title","taxis_max","taxis_min","taxis_tick_show","taxis_tick_start",
/* 70 */
    "taxis_tick_int","taxis_tick_num","taxis_tick_ano","taxis_tick_skip","taxis_title_size",
    "taxis_ano_size","baxis_title","baxis_max","baxis_min","baxis_tick_show",
/* 80 */
    "baxis_tick_start","baxis_tick_int","baxis_tick_num","baxis_tick_ano","baxis_tick_skip",
    "baxis_title_size","baxis_ano_size","laxis_ano_left","laxis_ano_right","raxis_ano_left",
/* 90 */
    "raxis_ano_right","taxis_ano_left","taxis_ano_right","baxis_ano_left","baxis_ano_right",
    "first_trace_time","last_trace_time","first_trace","last_trace","lock_first_samp",
/* 100  Not used: v_12_same at slot 107. */
    "hpgl_select_font","first_samp_time","last_samp_time","background","show_markers",
    "portrait","next_cmd_filename","inst_amp","brightness","negative",
/* 110 */
    "laxis_tick_abs_min","laxis_tick_abs_max","raxis_tick_abs_min","raxis_tick_abs_max","taxis_tick_abs_min",
    "taxis_tick_abs_max","baxis_tick_abs_min","baxis_tick_abs_max","eps_select_font","caption_font_size",
/* 120 */
    "caption_left_edge","caption_long_edge","caption","caption_bottom_edge","pagenum_font_size",
    "pagenum_left_edge","pagenum_bottom_edge","pagenum","local_offset","local_change",
/* 130 */
    "display_all","display_none","eps_copies","laxis_title_offset","laxis_tick_mid_skip",
    "raxis_title_offset","raxis_tick_mid_skip","taxis_title_offset","taxis_tick_mid_skip","baxis_title_offset",
/* 140 */
    "baxis_tick_mid_skip","square","file_header_bytes","trace_header_bytes","other_format",
    "input_datatype","samples_per_trace","total_time","row_by_row","min_data_val",
/* 150 */
    "skip_traces","show_marker_width","display_wiggle","num_wiggles","wiggle_traceval[]",
    "wiggle_color","wiggle_fill","wiggle_width","wiggle_clip","change_gain",
/* 160 */
    "num_gain_off","gain_off[]","num_gain_on","gain_on[]","pcx_outfilename",
    "add_hlines","hline_start","hline_int","hline_stop","add_vlines",
/* 170 */
    "vline_start","vline_int","vline_stop","hline_style","vline_style",
    "use_designjet750","hp_x","hp_y","hp_xoff","hp_yoff",
/* 180 */
    "envelope","inst_pow","plot_hyperbola","ant_sep","obj_depth",
    "obj_radius","obj_loc","hyp_vel","hyp_color",NULL,
} ;  /* eps_copies not used in the command file */
     /* tick_tracenum[], tick_xyz[][] not used in the command file */
     /* 12, 13, 132 not used */

/* The rest of these are message strings that match codes returned by functions */
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR: Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",
    "GetParameters() ERROR: Problem getting information about input data file.",

    "GetParameters() ERROR: Invalid channel selection for DZT file.",
    "GetParameters() ERROR: No recognizable data/info input files specified.",
    "GetParameters() ERROR: Problem getting data from MRK file.",
    "GetParameters() ERROR: Problem getting data from XYZ file.",
    "GetParameters() ERROR: Number of tick marks not same in MRK and XYZ files.",

    "GetParameters() ERROR: GetCoordRanges() returned an error.",
    "GetParameters() ERROR: Depth correction requested but no model given.",
    "GetParameters() ERROR: Invalid coord_mode specified.",
    "GetParameters() ERROR: Problem getting info from HD file.",
    "GetParameters() ERROR: Not able to allocate sufficient temporary storage.",

    "GetParameters() ERROR: Tick mark trace numbers and XYZ location mot found.",
    "GetParameters() ERROR: Problem getting viewport limits for data.",
    "GetParameters() ERROR: Time section requested but insufficient information.",
    "GetParameters() ERROR: Viewport limits invalid; Check VX2 > VX1 and VY2 > VY1.",
    "GetParameters() ERROR: Problem getting info from SGY file.",

    "GetParameters() ERROR: Max and min values must be given for floating point data.",
    "GetParameters() ERROR: Storage format not recognized.  Must define in CMD file.",
} ;
const char *GetDataAsImage8Msg[] =
{   "GetDataAsImage8(): No errors.",
    "GetDataAsImage8() ERROR: Not able to allocate storage for GPR image[][].\n\t\tTry increasing value of skip_traces.",
    "GetDataAsImage8() ERROR: Problem getting data from input file.",
    "GetDataAsImage8() ERROR: image[][] in this struct must be deallocated first.",
} ;
const char *GetCoordRangesMsg[] =
{   "GetCoordRanges() No error.",
    "GetCoordRanges() ERROR: Coord_mode must be 2 or 4",
    "GetCoordRanges() ERROR: Tick marks and coordinates needed for rubber-sheeting.",
    "GetCoordRanges() ERROR: Tick marks for rubber-sheeting outside data range.",
    "GetCoordRanges() ERROR: Requested horizontal limits outside of data range.",
    "GetCoordRanges() ERROR: Problem splining data topography.",
    "GetCoordRanges() ERROR: Not able to allocate sufficient temporary storage.",
} ;
const char *GetDataLimitsMsg[] =
{   "GetDataLimits(): No errors.",
    "GetDataLimits() ERROR: Unrecognized layer model mode.",
    "GetDataLimits() ERROR: Unrecognized coordinate system mode.",
    "GetDataLimits() ERROR: horiz_start or horiz_stop == INVALID_VALUE.",
} ;
const char *InstAmpImage8uMsg[] =
{   "InstAmpImage8u(): No errors.",
    "InstAmpImage8u() ERROR: Not able to allocate sufficient temporary storage.",
} ;
const char *InstPowImage8uMsg[] =
{   "InstPowImage8u(): No errors.",
    "InstPowImage8u() ERROR: Not able to allocate sufficient temporary storage.",
} ;
const char *HistStretchImage8uMsg[] =
{   "HistStretchImage8u(): No errors.",
    "HistStretchImage8u() ERROR: Percent is outside of valid (1 - 99) range.",
    "HistStretchImage8u() ERROR: Not able to allocate sufficient temporary storage.",
} ;
const char *ChangeRangeGain8uMsg[] =
{   "ChangeRangeGain8u(): No errors.",
    "ChangeRangeGain8u() ERROR: Invalid number of columns or rows.",
    "ChangeRangeGain8u() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
    "ChangeRangeGain8u() ERROR: NULL pointer passed to function.",
    "ChangeRangeGain8u() ERROR: Unable to allocate temporary storage.",
} ;
const char *DisplayImage8Msg[] =
{   "DisplayImage8(): No errors.",
    "DisplayImage8() ERROR: One or more positioning arrays are NULL.",
    "DisplayImage8() ERROR: Not able to allocate sufficient temporary storage.",
    "DisplayImage8() ERROR: Unrecognized coordinate mode.",
    "DisplayImage8() ERROR: Unrecognized layer model mode.",
} ;
const char *DisplayWiggle8Msg[] =
{   "DisplayWiggle8(): No errors.",
    "DisplayWiggle8() ERROR: One or more positioning arrays are NULL.",
    "DisplayWiggle8() ERROR: Not able to allocate sufficient temporary storage.",
    "DisplayWiggle8() ERROR: Unrecognized coordinate mode.",
    "DisplayWiggle8() ERROR: Unrecognized layer model mode.",
} ;
const char *PlotSelectWiggles8Msg[] =
{   "PlotSelectWiggles8(): No errors.",
    "PlotSelectWiggles8() ERROR: One or more positioning arrays are NULL.",
    "PlotSelectWiggles8() ERROR: Not able to allocate sufficient temporary storage.",
    "PlotSelectWiggles8() ERROR: Cannot plot wiggles for DISTANCE-DISTANCE mode.",
    "PlotSelectWiggles8() ERROR: Unrecognized layer model mode.",
} ;
const char *PlotHyperbola8Msg[] =
{   "PlotHyperbola8(): No errors.",
    "PlotHyperbola8() ERROR: Coordinate mode must be DISTANCE-TIME, mode 4.",
} ;
const char *LabelViewportsEPSMsg[] =
{   "LabelViewportsEPS(): No errors.",
    "LabelViewportsEPS() ERROR: Invalid function argument.",
    "LabelViewportsEPS() ERROR: fprintf() to EPS file returned EOF.",
} ;
const char *LabelViewportsPCXMsg[] =
{   "LabelViewportsPCX(): No errors.",
    "LabelViewportsPCX() ERROR: Invalid function argument.",
    "LabelViewportsPCX() ERROR: unable to open output PCX file.",
    "LabelViewportsPCX() ERROR: short count from fwrite() to PCX file.",
} ;
const char *SetDataViewportMsg[] =
{   "SetDataViewport(): No errors.",
    "SetDataViewport() ERROR: X1 == X2 or Y1 == Y2.",
    "SetDataViewport() ERROR: xmin == xmax or ymin == ymax.",
} ;

/********************************** main() **********************************/
void main(int argc, char *argv[])
{
/***** Declare variables *****/
    /* following are variables used by main */
    char   log_filename[80];
    int    itemp;             /* scratch variable */
    int    num_viewports;     /* count number of viewports (data sets) */
    int    first_time;        /* flag indicating first time through loop */
    int    loop;              /* flag indicating when to stop looping */
    int    error;             /* flag indicating an error has occured */
    double dtemp;             /* scratch variable */

    /* following is an array of pointers to information stuctures;
       each structure is the key variable passed between functions
       and must be allocated before use. */
    struct DispParamInfoStruct *ParamInfo[MAX_VIEWPORTS];

    /* these variables used by the graphics system */
    int vcols,vrows,backcolor;
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [*vrows][*vcols] from hpgl.h */
      /* If a compiler other than Intel Code Builder is used, link will fail. */
    #endif

    /* these variables found near beginning of this source */
    extern int         Debug,Batch,Batchset,Debugset,Videoset,save_eps,save_pcx;
    extern FILE       *log_file;
    extern const char *GetParametersMsg[];
    extern const char *GetDataAsImage8Msg[];
    extern const char *GetDataLimitsMsg[];
    extern const char *SetDataViewportMsg[];
    extern const char *LabelViewportsEPSMsg[];
    extern const char *DisplayImage8Msg[];
    extern const char *PlotSelectWiggles8Msg[];
    extern const char *DisplayWiggle8Msg[];

/***** Initialize variables *****/

    /* open error message file */
    strcpy(log_filename,"GPR_DISP.LOG");
    log_file = fopen(log_filename,"a+t");  /* open text file for appending */
    if (log_file == NULL)
    {   strcpy(log_filename,"C:\\GPR_DISP.LOG");
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
        fprintf(log_file,"Messages from program GPR_DISP v. %s: %s %d, %d at %d:%02d:%02d\n",
                        GPR_DISP_VER,month[dosdate.month],dosdate.day,dosdate.year,
                        dostime.hour,dostime.minute,dostime.second);
        fprintf(log_file,"Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.\n");
    }

    /* Check if ansi.sys loaded */
    if (ANSI_there()) ANSI_THERE = TRUE;

    /* set all structure pointers to NULL */
    for (itemp=0; itemp<MAX_VIEWPORTS; itemp++)
        ParamInfo[itemp] = NULL;

    /* initialize rest of main() variables */
    num_viewports = 0;
    first_time    = TRUE;
    loop          = TRUE;
    error         = FALSE;
    Batchset      = FALSE;
    Debugset      = FALSE;
    Videoset      = FALSE;

/*############################# START OF LOOP ##############################*/

/***** Get parameters and data and copy images into hpgl_array_8[][] *****/
    while ( (loop == TRUE) &&
            (num_viewports < MAX_VIEWPORTS) &&
            (error == FALSE) )
    {
    /***** Allocate storage for another structure */
        if (log_file && argv[1])
        {   fprintf(log_file,"Processing command file: %s\n",argv[1]);
                              /* ParamInfo[num_viewports]->cmd_filename); */
        }
        ParamInfo[num_viewports] =
            (struct DispParamInfoStruct *)malloc(sizeof(struct DispParamInfoStruct));
        if (ParamInfo[num_viewports] == NULL)
        {   if (first_time)
            {   printf("GPR_DISP ERROR: unable to allocate storage for Info struct #%d\n",
                        num_viewports);
                if (log_file) fprintf(log_file,"GPR_DISP ERROR: unable to allocate storage for Info struct #%d\n",
                        num_viewports);
                if (log_file) fclose(log_file);
                if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                printf("\nGPR_DISP finished.\nFile %s on disk contains messages.\n",
                        log_filename);
                if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                exit(4);
            } else
            {   if (log_file) fprintf(log_file,"GPR_DISP ERROR: unable to allocate storage for Info struct #%d\n",
                        num_viewports);
                error = TRUE;
                continue;     /* goto end of while block */
            }
        }

    /***** Get information used to process data. *****/
        if (first_time)
        {   printf("\n\nGetting user parameters and data limits ...");
            itemp =  GetParameters(argc,argv,ParamInfo[num_viewports]);
            if (itemp > 0)
            {   printf("\n%s\n",GetParametersMsg[itemp]);
                if (log_file)
                {    fprintf(log_file,"%s\n",GetParametersMsg[itemp]);
                    fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                    fclose(log_file);
                }
                if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                printf("\nGPR_DISP finished.\nFile %s on disk contains messages.\n",log_filename);
                if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                exit(1);
            }
            printf("\r                                                     \r");
            Debugset = TRUE;
            Batchset = TRUE;
            save_eps = ParamInfo[num_viewports]->save_eps;
            save_pcx = ParamInfo[num_viewports]->save_pcx;
        } else
        {   itemp =  GetParameters(argc,argv,ParamInfo[num_viewports]);
            if (itemp > 0)
            {   if (log_file) fprintf(log_file,"%s\n",GetParametersMsg[itemp]);
                error = TRUE;
                continue;     /* goto end of while block */
            }
        }
        if (log_file)
        {   if (Batch) fprintf(log_file,"Program in Batch mode\n");
            if (Debug) fprintf(log_file,"Program in Debug mode - no graphics\n");
        }

        /* Display parameters and ask if user wants to continue */
        if (ParamInfo[0]->display_none == TRUE)
        {   ; /* no parameter display */
        } else if (first_time)
        {   DisplayParameters(ParamInfo[num_viewports]);
            if (!Batch)
            {   printf("Press <Esc> to quit or other key to continue ... ");
                if ((itemp = getch()) == ESC)
                {   DeallocInfoStruct(ParamInfo[num_viewports]);
                    puts("\nProgram terminated by user.\n");
                    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                    if (log_file)
                    {   fprintf(log_file,"End of messages.  Program terminated by user.\n");
                        fclose(log_file);
                        printf("File %s on disk contains messages.\n",log_filename);
                    }
                    puts("GPR_DISP finished.");
                    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                    exit(3);
                } else if (itemp == 0 || itemp == 0xE0 || itemp == 0xF0)
                    getch();
                printf("\r                                                           \r");
            }
        } else if (ParamInfo[0]->display_all)
        {   DisplayParameters(ParamInfo[num_viewports]);
        }

    /***** Get the data and store as an 8-bit image *****/
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        puts("\nGPR_DISP is retreiving data and converting it to 8-bit image.  When this");
        puts("operation completes, the screen will be blank for a while as the GPR data");
        puts("are copied into the image array to be displayed on the screen. Please be");
        puts("patient as this may take quite a while for large or multiple data sets ...");
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
        itemp = GetDataAsImage8(ParamInfo[num_viewports]);
        if (itemp > 0)
        {   DeallocInfoStruct(ParamInfo[num_viewports]);
            if (log_file) fprintf(log_file,"%s\n",GetDataAsImage8Msg[itemp]);
            if (itemp == 1)  /* data set too large for memory */
            {   unsigned long mem,stack,bytes,requested;

                mem = _memavl();
                switch (ParamInfo[num_viewports]->input_datatype)
                {   case UCHAR:  case CHAR:                 bytes = 1;  break;
                    case USHORT: case SHORT: case USHORT12: bytes = 2;  break;
                    case ULONG:  case LONG:  case ULONG24:
                    case FLOAT:                             bytes = 4;  break;
                    case DOUBLE:                            bytes = 8;  break;
                }
                requested = (ParamInfo[num_viewports]->last_trace - ParamInfo[num_viewports]->first_trace + 1)
                             * ParamInfo[num_viewports]->total_samps * bytes;
                stack = requested/mem;
                printf("\nSize of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
                printf("Try increasing skip_traces to %lu or higher.\n",stack+2);
                if (log_file) fprintf(log_file,"Size of GPR data file (%lu bytes) exceeds memory (%lu bytes).\n",requested,mem);
                if (log_file) fprintf(log_file,"Try increasing skip_traces to %lu or higher.\n",stack+2);
            }
            if (first_time)
            {   printf("%s\n",GetDataAsImage8Msg[itemp]);
                if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                if (log_file)
                {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                    fclose(log_file);
                    printf("File %s on disk contains messages.\n",log_filename);
                }
                puts("GPR_DISP finished.");
                if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                if (!Batch)
                {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                        Sound(dtemp,0.055);
                }
                exit(2);
            } else
            {   error = TRUE;
                continue;     /* goto end of while block */
            }
        }
        puts("");

    /***** Initialize plotting to ARRAY (screen will go blank) *****/
        if (first_time)
        {   if ( save_eps == TRUE || save_pcx == TRUE )
                backcolor = 1;  /* white background */
            else
                backcolor = 0;  /* black background */
            /* allocate storage for hpgl_array_8[][] */
            InitGraphicsARRAY(backcolor,ParamInfo[num_viewports]->video_mode,
                            ParamInfo[num_viewports]->portrait,&vcols,&vrows);
        }

        /* Set data viewport limits */
        if (ParamInfo[num_viewports]->coord_mode == DISTANCE_DISTANCE) /* 2 */
        {   itemp = SetDataViewport(vcols,vrows,
                          ParamInfo[num_viewports]->vert_exag,
                          ParamInfo[num_viewports]->left,
                          ParamInfo[num_viewports]->right,
                          ParamInfo[num_viewports]->bottom,
                          ParamInfo[num_viewports]->top,
                        &(ParamInfo[num_viewports]->VX1),
                        &(ParamInfo[num_viewports]->VX2),
                        &(ParamInfo[num_viewports]->VY1),
                        &(ParamInfo[num_viewports]->VY2));
            if (itemp > 0)
            {   DeallocInfoStruct(ParamInfo[num_viewports]);
                if (log_file) fprintf(log_file,"%s\n",SetDataViewportMsg[itemp]);
                if (first_time)
                {   TerminateGraphicsARRAY(vrows);
                    printf("\n%s\n",SetDataViewportMsg[itemp]);
                    if (log_file)
                    {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                        fclose(log_file);
                    }
                    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                    printf("\nGPR_DISP finished.\nFile %s on disk contains messages.\n",log_filename);
                    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                    if (!Batch)
                    {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                            Sound(dtemp,0.055);
                    }
                    exit(5);
                } else
                {   error = TRUE;
                    continue;     /* goto end of while block */
                }
            }
        } else                    /* coord_mode != 2 (DISTANCE_DISTANCE) */
        {   if (!Debug)
            {   if (ParamInfo[num_viewports]->hpgl_select_font == NULL)
                    hpgl_select_font("romtrplx");
                else
                    hpgl_select_font(ParamInfo[num_viewports]->hpgl_select_font);
                /* select screen area to use */
                viewport(ParamInfo[num_viewports]->VX1,
                         ParamInfo[num_viewports]->VX2,
                         ParamInfo[num_viewports]->VY1,
                         ParamInfo[num_viewports]->VY2);
                /* assign user units to viewport */
                window(ParamInfo[num_viewports]->left,
                       ParamInfo[num_viewports]->right,
                       ParamInfo[num_viewports]->bottom,
                       ParamInfo[num_viewports]->top);
                clip();               /* prevent plotting outside of window */
            }
        }
        if (!Debug)
            printf("\n\n\n\n\n\n\n\n        PROCESSING AND COPYING DATA TO IMAGE, PLEASE BE PATIENT ...");

    /***** Copy data into hpgl_array_8[][] *****/
        if (ParamInfo[num_viewports]->display_wiggle)
        {   itemp = DisplayWiggle8(vrows,ParamInfo[num_viewports]);
            if (itemp > 0)
            {   DeallocInfoStruct(ParamInfo[num_viewports]);
                if (log_file) fprintf(log_file,"%s\n",DisplayWiggle8Msg[itemp]);
                if (first_time)
                {   TerminateGraphicsARRAY(vrows);
                    printf("\n%s\n",DisplayWiggle8Msg[itemp]);
                    if (log_file)
                    {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                        fclose(log_file);
                    }
                    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                    printf("\nGPR_DISP finished.\nFile %s on disk contains messages.\n",log_filename);
                    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                    if (!Batch)
                    {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                            Sound(dtemp,0.055);
                    }
                    exit(6);
                } else
                {   error = TRUE;
                    continue;     /* goto end of while block */
                }
            }
        } else
        {   itemp = DisplayImage8(vcols,vrows,ParamInfo[num_viewports]);
            if (itemp > 0)
            {   DeallocInfoStruct(ParamInfo[num_viewports]);
                if (log_file) fprintf(log_file,"%s\n",DisplayImage8Msg[itemp]);
                if (first_time)
                {   TerminateGraphicsARRAY(vrows);
                    printf("\n%s\n",DisplayImage8Msg[itemp]);
                    if (log_file)
                    {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                        fclose(log_file);
                    }
                    if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                    printf("\nGPR_DISP finished.\nFile %s on disk contains messages.\n",log_filename);
                    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                    if (!Batch)
                    {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                            Sound(dtemp,0.055);
                    }
                    exit(6);
                } else
                {   error = TRUE;
                    continue;     /* goto end of while block */
                }
            }
            if (!Debug)
                printf("\r                                                                             \r");
            else
                puts("Back from DisplayImage8() and about to free storage");
            if (ParamInfo[num_viewports]->num_wiggles > 0)
            {   itemp = PlotSelectWiggles8(vrows,ParamInfo[num_viewports]);
                if (itemp > 0)
                {   DeallocInfoStruct(ParamInfo[num_viewports]);
                    if (log_file) fprintf(log_file,"%s\n",PlotSelectWiggles8Msg[itemp]);
                    if (first_time)
                    {   TerminateGraphicsARRAY(vrows);
                        printf("\n%s\n",PlotSelectWiggles8Msg[itemp]);
                        if (log_file)
                        {   fprintf(log_file,"End of messages.  Program terminated with an ERROR.\n");
                            fclose(log_file);
                        }
                        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
                        printf("\nGPR_DISP finished.\nFile %s on disk contains messages.\n",log_filename);
                        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
                        if (!Batch)
                        {   for (itemp=0,dtemp=2000.; itemp<10; itemp++,dtemp-=100.)
                                Sound(dtemp,0.055);
                        }
                        exit(6);
                    } else
                    {   error = TRUE;
                        continue;     /* goto end of while block */
                    }
                }
            }
        }

    /***** Plot velocity hyperbola if requested *****/
        if (ParamInfo[num_viewports]->plot_hyperbola)
        {   itemp = PlotHyperbola8(ParamInfo[num_viewports]);
            if (itemp > 0)
            {   if (log_file) fprintf(log_file,"%s\n",PlotHyperbola8Msg[itemp]);
                printf("\n%s\n",PlotHyperbola8Msg[itemp]);
            }
            if (Debug)
                puts("Back from PlotHyperbola()");
        }

    /***** Free storage not needed later (structure still alloced) *****/
        DeallocInfoStruct(ParamInfo[num_viewports]);

    /***** Determine if finished or not *****/
        if (ParamInfo[num_viewports]->next_cmd_filename[0] == 0)
            loop = FALSE;
        else   /* be sure its there! */
        {   FILE *fileptr;

            if ((fileptr = fopen(ParamInfo[num_viewports]->next_cmd_filename,"rt")) != NULL)
            {   argv[1] = ParamInfo[num_viewports]->next_cmd_filename;
                fclose(fileptr);
            }  else
            {   loop = FALSE;
                printf("ERROR: unable to open next command file %s\n",ParamInfo[num_viewports]->next_cmd_filename);
                if (log_file) fprintf(log_file,"ERROR: unable to open next command file %s\n",ParamInfo[num_viewports]->next_cmd_filename);
            }
        }
        first_time = FALSE;     /* flag that first loop has been completed */
        num_viewports++;        /* count number of viewports */
        if (Debug)
        {   printf("at end of loop block: num_viewports = %d. Press a key...\n",num_viewports);
            printf("argv[1] = %s\n",argv[1]);
            getch();
        }
    } /* end of while (loop) block */

/*############################### END OF LOOP ##############################*/

/***** Display ARRAY to CRT, add axes and labels, save to EPS/PCX file *****/
    if (num_viewports > 0)
    {   if (save_eps || !save_pcx)
        {   itemp = LabelViewportsEPS(vcols,vrows,backcolor,num_viewports,ParamInfo);
            if (itemp)
            {   printf("%s\n",LabelViewportsEPSMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",LabelViewportsEPSMsg[itemp]);
            }
        }
        if (save_pcx)
        {   itemp = LabelViewportsPCX(vcols,vrows,backcolor,num_viewports,ParamInfo);
            if (itemp)
            {   printf("%s\n",LabelViewportsPCXMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",LabelViewportsPCXMsg[itemp]);
            }
        }
    }

/***** Free storage and exit *****/
    TerminateGraphicsARRAY(vrows);  /* free hpgl_array_8[][], plotter_off() */
    for (itemp=0; itemp<num_viewports; itemp++)
    {   /* Couldn't free "caption" and "pagenum" in DeallocInfoStruct()
           because needed them in LabelViewportsEPS() */
        free(ParamInfo[itemp]->caption);
        free(ParamInfo[itemp]->pagenum);
        free(ParamInfo[itemp]);          /* free the structure */
    }

    if (log_file)
    {   fprintf(log_file,"End of messages.  Program terminated normally.\n");
        fclose(log_file);
        if (ANSI_THERE) printf("%c%c32;40m",0x1B,0x5B);
        printf("GPR_DISP finished.\nFile %s on disk contains messages.",log_filename);
        if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);
    }
    exit(0);
}
/****************************** GetParameters() *****************************/
/* Initialize processing parameters and get user-defined values.
 * Determine GPR file storage format.
 * Get information from the appropriate "information" files.
 * Get tick mark trace locations and XYZ data if coord_mode == 2 or 4.
 * Get tick mark trace locations if show_markers != 0.
 * Validate viewport, trace, and sample ranges.
 *
 * Parameter list:
 *  int   argc      - number of command-line arguments
 *  char *argv[]    - array of strings containing the arguments
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <math.h>, <stdio.h>, <stdlib.h>, <string.h>,
 *           "gpr_disp.h", "rif_pgms.h", "dzt_pgms.h", "gsg_pgms.h",
 *           "gpg_pgms.h".
 * Calls: printf, PrintUsageMsg, InitParameters, GetCmdFileArgs.
 *        strcpy, puts, strstr, atoi, atof, strchr, strcat, memcpy, strupr.
 *        ReadOneDztHeader, DeallocInfoStruct,
 *        GetMrkData, GetXyzData, GetCoordRanges,
 *        GetSsHdFile, PrintSsHdInfo, ReadSegyReelHdr, PrintSegyReelHdr,
 *
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *GetParametersMsg[] =
{   "GetParameters(): No errors.",
    "GetParameters() ERROR: Insufficient command line arguments.",
    "GetCmdFileArgs() ERROR: Unable to open input command file.",
    "GetCmdFileArgs() ERROR: Input file(s) not specified.",
    "GetParameters() ERROR: Problem getting information about input data file.",
*5* "GetParameters() ERROR: Invalid channel selection for DZT file.",
    "GetParameters() ERROR: No recognizable data/info input files specified.",
    "GetParameters() ERROR: Problem getting data from MRK file.",
    "GetParameters() ERROR: Problem getting data from XYZ file.",
    "GetParameters() ERROR: Number of tick marks not same in MRK and XYZ files.",
*10*"GetParameters() ERROR: GetCoordRanges() returned an error.",
    "GetParameters() ERROR: Depth correction requested but no model given.",
    "GetParameters() ERROR: Invalid coord_mode specified.",
    "GetParameters() ERROR: Problem getting info from HD file.",
    "GetParameters() ERROR: Not able to allocate sufficient temporary storage.",
*15*"GetParameters() ERROR: Tick mark trace numbers and XYZ location mot found.",
    "GetParameters() ERROR: Problem getting viewport limits for data.",
    "GetParameters() ERROR: Time section requested but insufficient information.",
    "GetParameters() ERROR: Viewport limits invalid; Check VX2 > VX1 and VY2 > VY1.",
    "GetParameters() ERROR: Problem getting info from SGY file.",
*20*"GetParameters() ERROR: Max and min values must be given for floating point data.",
    "GetParameters() ERROR: Storage format not recognized.  Must define in CMD file.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: January 28, 1997; July 1, 1997; September 1, 1999; November 2, 2001;
 */
int GetParameters (int argc, char *argv[],struct DispParamInfoStruct *InfoPtr)
{
/***** Declare variables *****/
    int    req_args = 2;   /* exe_name, and cmd_filename */
    int    i,itemp;        /* scratch variables and counters */
    int    modes[15] =     /* known 256-color video modes */
     { 263, 261, 56, 1024, 8514, 259, 48, 800, 99, 46, 257, 640, 98, 95, 20 };
    int    header_size;    /* number of bytes in DZT header */
    long   row,col;        /* scratch variables and counters */
    int  file_hdr_bytes,num_hdrs,trace_hdr_bytes,samples_per_trace;
    int  num_channels,input_datatype,total_time,file_type;
    long num_traces;
    extern int Debug,Videoset,save_eps,save_pcx;
    extern FILE *log_file;
    extern v_color;        /* in hpgl.h */
    extern const char *GetCoordRangesMsg[];
    extern const char *GetDataLimitsMsg[];
    extern const char *GetGprFileTypeMsg[];

/***** Verify usage *****/
    if (argc < req_args)
    {   PrintUsageMsg();
        return 1;
    }

/***** Initialize information structure *****/
    InitParameters(InfoPtr);
    strcpy(InfoPtr->cmd_filename,argv[1]);

/***** Get user-defined parameters from CMD file *****/
    itemp = GetCmdFileArgs(InfoPtr);
    if (itemp) return itemp;   /* 2 to 3 */

/***** Get user-defined parameters from override CMD file *****/
    if (argc > 2)
    {   strcpy(InfoPtr->cmd_filename,argv[2]);
        strcpy(InfoPtr->override_cmd_filename,argv[2]);
        itemp = GetCmdFileArgs(InfoPtr);
        if (itemp == 2) return itemp;
        strcpy(InfoPtr->cmd_filename,argv[1]);
    }

/***** Get graphics mode and check that it is available *****/
    if (!Videoset)
    {   if (Debug)
        {   printf("Getparameters() Debug: Press a key to clear screen to get mode ...");
            getch();
        }
        if (InfoPtr->video_mode < 20)
        {   /* find an available mode if user doesn't know */
            for (i=0; i<15; i++) /* try all known 256 color modes hi to lo res */
            {   if ((InfoPtr->video_mode = plotter_is(modes[i],"CRT")) >= 20)
                    break;
            }
        } else
        {   /* be sure user gave a known 256-color mode */
            int v_mode;

            v_mode = plotter_is(InfoPtr->video_mode,"CRT");
            if ( (v_mode != InfoPtr->video_mode) || (v_color != 255) )
            {   for (i=0; i<15; i++) /* try all known 256 color modes hi to lo res */
                { if ((InfoPtr->video_mode = plotter_is(modes[i],"CRT")) >= 20)
                    break;
                }
            }
        }
        Videoset = TRUE;

        /* return to text mode */
        plotter_off();
    }

/***** Determine GPR file storage format and essential info *****/
    if (InfoPtr->input_format == 0)
    {   itemp = GetGprFileType(&file_hdr_bytes,&num_hdrs,&trace_hdr_bytes,
                    &samples_per_trace,&num_channels,&num_traces,
                    &input_datatype,&total_time,&file_type,
                    InfoPtr->dat_infilename);
        if (itemp > 0)
        {   printf("%s\n",GetGprFileTypeMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetGprFileTypeMsg[itemp]);
            return 4;
        }
        if (file_type == OTHER)
        {   if (log_file) fprintf(log_file,"ERROR: Storage format not recognized.  User must define in CMD file.\n");
            return 21;
        }
        InfoPtr->input_format       = file_type;
        InfoPtr->file_header_bytes  = file_hdr_bytes;
        InfoPtr->trace_header_bytes = trace_hdr_bytes;
        InfoPtr->total_traces       = num_traces;
        InfoPtr->total_samps        = samples_per_trace;
        InfoPtr->total_time         = total_time;
        InfoPtr->ns_per_samp        = (double)InfoPtr->total_time / (InfoPtr->total_samps - 1);
        InfoPtr->input_datatype     = input_datatype;
    }

/***** Get necessary information from info file/header *****/
    if (InfoPtr->input_format == DZT)
    {   int num_hdrs,channel;
        struct DztHdrStruct DztHdr;                /* from gpr_io.h */
        extern const char *ReadOneDztHeaderMsg[];  /* from gpr_io.h */
        extern const char *ant_number[];           /* from gpr_io.h */
        extern const char *ant_name[];             /* from gpr_io.h */

        /* get first DZT header in file */
        channel = 0;
        itemp = ReadOneDztHeader(InfoPtr->dat_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),channel,
                                 &header_size,&DztHdr);
        if (itemp > 0)
        {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
            if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
            {   puts("Fatal error.");
                return 4;
            }
        }
        if (header_size < 1024)
        {   puts("This DZT header is not consistent with version 5.x SIR-10A software.");
            if (log_file) fprintf(log_file,"This DZT header is not consistent with version 5.x SIR-10A software.\n");
        }
        if (InfoPtr->channel >= num_hdrs) return 5;


        /* get selected header (if different than first) */
        if (InfoPtr->channel > 0)
        {   itemp = ReadOneDztHeader(InfoPtr->dat_infilename,&num_hdrs,
                                 &(InfoPtr->total_traces),InfoPtr->channel,
                                 &header_size,&DztHdr);
            if (itemp > 0)
            {   printf("%s\n",ReadOneDztHeaderMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",ReadOneDztHeaderMsg[itemp]);
                if (strstr(strupr((char *)(ReadOneDztHeaderMsg[itemp])),"WARNING") == NULL)
                {   puts("Fatal error.");
                    return 4;
                }
            }
        }

        /* assign values to info structure */
        InfoPtr->total_samps    = DztHdr.rh_nsamp;
        /*if (InfoPtr->first_samp == 0 && !InfoPtr->lock_first_samp) */
        if (!InfoPtr->lock_first_samp)
        {   InfoPtr->first_samp = ( ( (-1.0*DztHdr.rh_position)/DztHdr.rh_range
                                    ) * (double)(InfoPtr->total_samps - 1)
                                  ) + 0.5; /* expect truncation to get roundoff */
        }
        if (InfoPtr->first_samp < 0)   InfoPtr->first_samp = 0;
        if (InfoPtr->last_samp == 0)   InfoPtr->last_samp  = InfoPtr->total_samps - 1;
        if      (DztHdr.rh_bits ==  8) InfoPtr->input_datatype = UCHAR;
        else if (DztHdr.rh_bits == 16) InfoPtr->input_datatype = USHORT;
        else if (DztHdr.rh_bits == 32) InfoPtr->input_datatype = ULONG;
        InfoPtr->total_time       = DztHdr.rh_range;
        InfoPtr->ns_per_samp      = DztHdr.rh_range/(DztHdr.rh_nsamp-1);
        for (itemp=0; ant_number[itemp]; itemp++)
        {   if (strstr(DztHdr.rh_antname,ant_number[itemp]))
            {   InfoPtr->ant_freq = atoi(ant_name[itemp]);
                break;
            }
        }
        if (InfoPtr->trace_per_sec <= 0.0)
            InfoPtr->trace_per_sec  = DztHdr.rh_sps;
        InfoPtr->trace_per_m    = DztHdr.rh_spm;
        InfoPtr->start_pos      = 0.0;
        InfoPtr->final_pos      = 0.0;
        InfoPtr->step_size      = 0.0;
        if (Debug)
        {   printf("total_traces = %ld\n",InfoPtr->total_traces);
            printf("total_samps = %ld   total_time = %g\n",InfoPtr->total_samps,InfoPtr->total_time);
            printf("first_samp = %ld  last_samp = %ld\n",InfoPtr->first_samp,InfoPtr->last_samp);
            printf("ns_per_samp = %g  trace_per_sec = %g\n",InfoPtr->ns_per_samp,InfoPtr->trace_per_sec);
            printf("datatype = %d\n",InfoPtr->input_datatype);
            printf("trace_per_m=%g start_pos=%g final_pos=%g step_size=%g\n",
                InfoPtr->trace_per_m,InfoPtr->start_pos,InfoPtr->final_pos,InfoPtr->step_size);
            printf("num_hdrs = %d\n",num_hdrs);
            getch();
        }

    } else if (InfoPtr->input_format == DT1)
    {   int print_it;
        struct SsHdrInfoStruct  HdInfo;     /* buffer to hold info from HD file */
        extern const char *GetSsHdFileMsg[];

        /* Get info from HD file */
        InitDt1Parameters(&HdInfo);
        if (Debug)  print_it = 1;
        else        print_it = 0;
        itemp = GetSsHdFile(print_it,&HdInfo,InfoPtr->inf_infilename);
        if (itemp > 0)
        {   printf("%s\n",GetSsHdFileMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetSsHdFileMsg[itemp]);
            return 13;
        }
        if (Debug) PrintSsHdInfo("stdout",InfoPtr->inf_infilename,&HdInfo);
        if (Debug) if (!getch()) getch();

        InfoPtr->input_datatype = SHORT;
        InfoPtr->total_traces   = HdInfo.num_traces;
        InfoPtr->total_samps    = HdInfo.num_samples;
        InfoPtr->total_time     = HdInfo.total_time_ns;
        InfoPtr->ns_per_samp    = (double)HdInfo.total_time_ns/(HdInfo.num_samples-1);
        /* if (InfoPtr->first_samp == 0 && !InfoPtr->lock_first_samp) */
        if (!InfoPtr->lock_first_samp)
            InfoPtr->first_samp = HdInfo.time_zero_sample;
        InfoPtr->last_samp      = HdInfo.num_samples - 1;
        InfoPtr->ant_freq       = HdInfo.ant_freq;
        InfoPtr->trace_per_sec  = 0.0;
        if (HdInfo.step_size != 0.0)
        {   if ( strstr(strupr(HdInfo.pos_units),"FEET") ||
                 strstr(strupr(HdInfo.pos_units),"FT")
               )
            {   HdInfo.step_size *= 0.3048;  /* convert to meters */
                HdInfo.start_pos *= 0.3048;
                HdInfo.final_pos *= 0.3048;
            }
            InfoPtr->trace_per_m = 1.0 / HdInfo.step_size;
        } else
            InfoPtr->trace_per_m = 0.0;
        InfoPtr->start_pos      = HdInfo.start_pos;
        InfoPtr->final_pos      = HdInfo.final_pos;
        InfoPtr->step_size      = HdInfo.step_size;

        if (Debug)
        {   printf("total_traces = %ld\n",InfoPtr->total_traces);
            printf("total_samps = %ld   total_time = %g\n",InfoPtr->total_samps,InfoPtr->total_time);
            printf("first_samp = %ld  last_samp = %ld\n",InfoPtr->first_samp,InfoPtr->last_samp);
            printf("ns_per_samp = %g  trace_per_sec = %g\n",InfoPtr->ns_per_samp,InfoPtr->trace_per_sec);
            printf("datatype = %d\n",InfoPtr->input_datatype);
            printf("trace_per_m=%g start_pos=%g final_pos=%g step_size=%g\n",
                InfoPtr->trace_per_m,InfoPtr->start_pos,InfoPtr->final_pos,InfoPtr->step_size);
            getch();
        }

    } else if (InfoPtr->input_format == SGY)
    {   char  *cp,cmdstr[30],pos_units[12];
        char string[40][80];    /* should have 3200 continuous bytes */
        int swap_bytes,str_num,max_records,num,found;
        int comment_size,record_length;  /* bytes */
        long num_traces;
        double dtemp;
        struct SegyReelHdrStruct hdr;
        extern const char *ReadSegyReelHdrMsg[];

        itemp = ReadSegyReelHdr(InfoPtr->dat_infilename,&swap_bytes,
                                    &num_traces,&hdr);
        if (itemp > 0)
        {   printf("%s\n",ReadSegyReelHdrMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",ReadSegyReelHdrMsg[itemp]);
            return 19;
        }
        if (Debug) PrintSegyReelHdr(swap_bytes,num_traces,"stdout",
                                    InfoPtr->dat_infilename,&hdr);

        switch(hdr.format)
        {   case 1:          InfoPtr->input_datatype = FLOAT;  break;
            case 2:  case 4: InfoPtr->input_datatype =  LONG;  break;
            case 3:          InfoPtr->input_datatype = SHORT;  break;
            default: break;
        }
        InfoPtr->total_traces   = num_traces;
        InfoPtr->total_samps    = hdr.hns;
        InfoPtr->ns_per_samp    =
                 (hdr.hns - 1) * ((double)hdr.hdt / 1000.); /* was picosec */

        /* Extract S&S info from SEG-Y reel header */
        /* copy ASCII part of header and terminate strings */
        comment_size = sizeof(hdr.comment);
        record_length = 80;                          /* bytes */
        max_records = comment_size / record_length;  /* should be 40 */
        memcpy((void*)string,(void*)hdr.comment,
                (size_t)_MINN(comment_size,3200));
        for (i=0; i<max_records; i++)  string[i][record_length-1] = 0;
        if (Debug)
        {   for (i=0; i<40; i++)
            {   printf("string[%d]=%s\n",i,string[i]);
                getch();
            }
        }
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
            {    case 0:
                    itemp = atoi(cp);
                    if (itemp > 0)  InfoPtr->total_traces = itemp;
                    break;
                case 1:
                    itemp = atoi(cp);
                    if (itemp > 0)
                    {   InfoPtr->total_samps = itemp;
                        InfoPtr->last_samp   = itemp - 1;
                    }
                    break;
                case 2:
                    itemp = atoi(cp);
                    if (!InfoPtr->lock_first_samp)
                        InfoPtr->first_samp = itemp;
                    break;
                case 3:
                    itemp = atoi(cp);
                    if (itemp > 0) InfoPtr->total_time = atoi(cp);
                    break;
                case 4:
                    InfoPtr->start_pos = atof(cp);
                    break;
                case 5:
                    InfoPtr->final_pos = atof(cp);
                    break;
                case 6:
                    InfoPtr->step_size = atof(cp);
                    break;
                case 7:
                    pos_units[0] = 0;
                    strcpy(pos_units,cp);
                    pos_units[11] = 0;
                    break;
                case 8:
                    dtemp = atof(cp);
                    if (dtemp > 0.0) InfoPtr->ant_freq = dtemp;
                    break;
                case 13:
                    dtemp = atof(cp);
                    if (dtemp > 0.0) InfoPtr->trace_per_sec = dtemp;
                    break;
                default:
                    break;
                }
        }
        if (InfoPtr->step_size != 0.0)
        {   if ( strstr(strupr(pos_units),"FEET") ||
                 strstr(strupr(pos_units),"FT")
               )
            {   InfoPtr->step_size *= 0.3048;  /* convert to meters */
                InfoPtr->start_pos *= 0.3048;
                InfoPtr->final_pos *= 0.3048;
            }
            InfoPtr->trace_per_m = 1.0 / InfoPtr->step_size;
        } else
            InfoPtr->trace_per_m = 0.0;
        if (InfoPtr->total_samps > 1 && InfoPtr->total_time > 0.0)
            InfoPtr->ns_per_samp = InfoPtr->total_time / (InfoPtr->total_samps - 1);
        else
            InfoPtr->ns_per_samp = 0;
        if (Debug)
        {   printf("total_traces = %ld\n",InfoPtr->total_traces);
            printf("total_samps = %ld   total_time = %g\n",InfoPtr->total_samps,InfoPtr->total_time);
            printf("first_samp = %ld  last_samp = %ld\n",InfoPtr->first_samp,InfoPtr->last_samp);
            printf("ns_per_samp = %g  trace_per_sec = %g\n",InfoPtr->ns_per_samp,InfoPtr->trace_per_sec);
            printf("datatype = %d\n",InfoPtr->input_datatype);
            printf("trace_per_m=%g start_pos=%g final_pos=%g step_size=%g\n",
                InfoPtr->trace_per_m,InfoPtr->start_pos,InfoPtr->final_pos,InfoPtr->step_size);
            getch();
        }

    } else if (InfoPtr->input_format == USER_DEF)
    {   itemp = GetOtherInfo(InfoPtr);
        if (Debug)
        {   printf("After GetOtherInfo(): total_traces = %ld\n",InfoPtr->total_traces);
            printf("total_samps = %ld   total_time = %g\n",InfoPtr->total_samps,InfoPtr->total_time);
            printf("first_samp = %ld  last_samp = %ld\n",InfoPtr->first_samp,InfoPtr->last_samp);
            printf("ns_per_samp = %g  trace_per_sec = %g\n",InfoPtr->ns_per_samp,InfoPtr->trace_per_sec);
            printf("datatype = %d\n",InfoPtr->input_datatype);
            getch();
        }
        if (itemp) return 4;
    } else
        return 6;

    if (InfoPtr->input_datatype == FLOAT || InfoPtr->input_datatype == DOUBLE)
    {   if (InfoPtr->max_data_val <= InfoPtr->min_data_val)
            return 20;
    }

/***** Get tick mark trace locations and XYZ data if necessary *****/
    if (InfoPtr->coord_mode == DISTANCE_DISTANCE || InfoPtr->coord_mode == DISTANCE_TIME)
    {   /* 2 or 4 */
        int num_mrk_ticks,num_xyz_ticks;
        extern const char *GetMrkDataMsg[];
        extern const char *GetXyzDataMsg[];

        if (InfoPtr->mrk_infilename[0] && InfoPtr->xyz_infilename[0])
        {   itemp = GetMrkData(InfoPtr->mrk_infilename,&num_mrk_ticks,
                                &(InfoPtr->tick_tracenum));
            if (itemp > 0)
            {   printf("\n%s\n",GetMrkDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetMrkDataMsg[itemp]);
                DeallocInfoStruct(InfoPtr);
                InfoPtr->num_ticks = 0;
                return 7;
            }
            InfoPtr->num_ticks = num_mrk_ticks;
            itemp = GetXyzData(InfoPtr->xyz_infilename,&num_xyz_ticks,
                                &(InfoPtr->tick_xyz));
            if (itemp > 0 || num_xyz_ticks != num_mrk_ticks)
            {   printf("\n%s\n",GetXyzDataMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetXyzDataMsg[itemp]);
                DeallocInfoStruct(InfoPtr);
                InfoPtr->num_ticks = 0;
                if (itemp > 0)  return 8;
                else            return 9;
            }
        } else if (InfoPtr->input_format == DT1 || InfoPtr->input_format == SGY)
        {    if (InfoPtr->step_size)
            {   int j;

                if (Debug) puts("getting distance info from HD1 or SGY file");
                InfoPtr->num_ticks = InfoPtr->total_traces;
                InfoPtr->tick_tracenum = (long *)malloc(InfoPtr->num_ticks * sizeof(long));
                if (InfoPtr->tick_tracenum == NULL)
                {   InfoPtr->num_ticks = 0;
                    if (log_file) fprintf(log_file,"GetParameters(): malloc failed for tick_tracenum[]");
                    return 14;
                }
                InfoPtr->tick_xyz = (double **)malloc(InfoPtr->num_ticks * sizeof(double *));
                if (InfoPtr->tick_xyz == NULL)
                {   InfoPtr->num_ticks = 0;
                    free(InfoPtr->tick_tracenum);
                    InfoPtr->tick_tracenum = NULL;
                    if (log_file) fprintf(log_file,"GetParameters(): malloc failed for tick_xyz[]");
                    return 14;
                }
                for (i=0; i<InfoPtr->num_ticks; i++)
                {   InfoPtr->tick_xyz[i] = (double *)malloc(3 * sizeof(double));
                    if (InfoPtr->tick_xyz[i] == NULL)
                    {   for (j=0; j<i; j++) free(InfoPtr->tick_xyz[j]);
                        free(InfoPtr->tick_xyz);
                        free(InfoPtr->tick_tracenum);
                        InfoPtr->tick_xyz = NULL;
                        InfoPtr->tick_tracenum = NULL;
                        InfoPtr->num_ticks = 0;
                        if (log_file) fprintf(log_file,"GetParameters(): malloc failed for tick_xyz[][]");
                        return 14;
                    }
                }
                for (i=0; i<InfoPtr->num_ticks; i++)
                {   InfoPtr->tick_tracenum[i] = i;
                    InfoPtr->tick_xyz[i][0]  = i * InfoPtr->step_size; /* X */
                    InfoPtr->tick_xyz[i][1]  = 0.0;                    /* Y */
                    InfoPtr->tick_xyz[i][2]  = 0.0;                    /* Z */
                    /* elev. data can be stored in each trace header ! */
                }
                InfoPtr->horiz_mode = 1;
            } else
            {   return 15;  /* need MRK/XYZ file or DT1/SGY format */
            }
        } else if (InfoPtr->num_ticks == 0)
            return 15;
    }
    if ( (InfoPtr->coord_mode != DISTANCE_DISTANCE) && (InfoPtr->num_layers > 0) )
    {   free(InfoPtr->layer_rdp);  InfoPtr->layer_rdp  = NULL;
        free(InfoPtr->layer_mode); InfoPtr->layer_mode = NULL;
        free(InfoPtr->layer_val);  InfoPtr->layer_val  = NULL;
        InfoPtr->num_layers = 0;
    }

/***** Get tick mark trace locations if show_markers != 0 *****/
    if (InfoPtr->show_markers)
    {   int num_mrk_ticks;
        extern const char *GetMrkDataMsg[];

        if (InfoPtr->num_ticks == 0)
        {   if (InfoPtr->mrk_infilename[0])
            {   itemp = GetMrkData(InfoPtr->mrk_infilename,&num_mrk_ticks,
                                &(InfoPtr->tick_tracenum));
                if (itemp > 0)
                {   printf("\n%s\n",GetMrkDataMsg[itemp]);
                    if (log_file) fprintf(log_file,"%s\n",GetMrkDataMsg[itemp]);
                    DeallocInfoStruct(InfoPtr);
                    InfoPtr->num_ticks = 0;
                    return 7;
                }
                InfoPtr->num_ticks = num_mrk_ticks;
            }
        }
        if (InfoPtr->num_ticks == 0) InfoPtr->show_markers = 0;
    }

/***** Finish initializing variables and verifying valid ranges *****/
    /* limit first and last traces to valid ranges; first trace used from
       file must always be <= last trace used, regardless of which one is
       displayed "first", ie. to the left of the other. */
    if ( (InfoPtr->first_trace >= InfoPtr->total_traces) ||
         (InfoPtr->first_trace < 0)
       )
        InfoPtr->first_trace = 0;
    if ( (InfoPtr->last_trace == 0 ) ||
         (InfoPtr->last_trace < InfoPtr->first_trace) ||
         (InfoPtr->last_trace >= InfoPtr->total_traces)
       )
        InfoPtr->last_trace = InfoPtr->total_traces - 1;
    if (InfoPtr->skip_traces < 0) InfoPtr->skip_traces = 0;

    /* limit first and last samples to valid ranges; first sample used from
       the traces must always be <= last sample used, regardless of which one
       is displayed "first", ie. above the other. */
    if ( (InfoPtr->first_samp >= InfoPtr->total_samps) ||
         (InfoPtr->first_samp < 0)
       )
        InfoPtr->first_samp = 0;
    if ( (InfoPtr->last_samp == 0 ) ||
         (InfoPtr->last_samp < InfoPtr->first_samp) ||
         (InfoPtr->last_samp >= InfoPtr->total_samps) )
        InfoPtr->last_samp  = InfoPtr->total_samps - 1;
    if (InfoPtr->total_time <= 0.0)
        if (InfoPtr->ns_per_samp <=0)
            InfoPtr->coord_mode = TRACE_SAMPLE; /* 5 */

    /* limit first and last times to valid ranges; first trace time used from
       the file must always be <= last trace time used, regardless of which
       one is displayed "first", ie. toi the left the other; also, first
       sample time used from the traces must always be <= last sample time
       used, regardless of which one is displayed "first", ie. above the
       other. */
    if (InfoPtr->coord_mode == TIME_TIME) /* 3 */
    {   double total_time;

        if ( InfoPtr->trace_per_sec == INVALID_VALUE ||
             InfoPtr->trace_per_sec <= 0.0 ||
             InfoPtr->ns_per_samp   == INVALID_VALUE ||
             InfoPtr->ns_per_samp   <= 0.0 )
        {   DeallocInfoStruct(InfoPtr);
            return 17;
        }
        total_time = (InfoPtr->total_traces - 1)/InfoPtr->trace_per_sec;
        /* By default, the first trace in file has trace time of 0.0 */
        if (InfoPtr->first_trace_time == INVALID_VALUE)
            InfoPtr->first_trace_time = 0.0;
        if ( (InfoPtr->last_trace_time == INVALID_VALUE) ||
             (InfoPtr->last_trace_time <= InfoPtr->first_trace_time)
           )
            InfoPtr->last_trace_time = total_time;
        InfoPtr->first_trace = (InfoPtr->first_trace_time * InfoPtr->trace_per_sec)
                               + 0.5;
        InfoPtr->last_trace = (InfoPtr->last_trace_time * InfoPtr->trace_per_sec)
                               + 0.5;

        /* By default, the first sample in a trace has time of 0.0 */
        if (InfoPtr->first_samp_time == INVALID_VALUE)
            InfoPtr->first_samp_time = 0.0;
        if ( (InfoPtr->last_samp_time == INVALID_VALUE) ||
             (InfoPtr->last_samp_time <= InfoPtr->first_samp_time)
           )
            InfoPtr->last_samp_time =  InfoPtr->first_samp_time +
              ((InfoPtr->last_samp - InfoPtr->first_samp) * InfoPtr->ns_per_samp);
    }
    if (InfoPtr->coord_mode == TRACE_TIME || InfoPtr->coord_mode == DISTANCE_TIME) /* 1 or 4 */
    {   if ( InfoPtr->ns_per_samp == INVALID_VALUE ||
             InfoPtr->ns_per_samp <= 0.0 )
        {   DeallocInfoStruct(InfoPtr);
            return 17;
        }
        /* By default, the first sample in a trace has time of 0.0 */
        if (InfoPtr->first_samp_time == INVALID_VALUE)
            InfoPtr->first_samp_time = 0.0;
        if ( (InfoPtr->last_samp_time == INVALID_VALUE) ||
             (InfoPtr->last_samp_time <= InfoPtr->first_samp_time)
           )
            InfoPtr->last_samp_time =  InfoPtr->first_samp_time +
              ((InfoPtr->last_samp - InfoPtr->first_samp) * InfoPtr->ns_per_samp);
    }

    /* Verify coordinate ranges, determine number of cols and rows, and
       allocate storage for and assign Hvals, Vvals, and Zvals.
       GetCoordRanges() will allocate and assign Hvals, Vvals, and Zvals.
     */
    if (InfoPtr->coord_mode == DISTANCE_DISTANCE) /* 2 */
    {   if (InfoPtr->num_layers <= 0)
        {   DeallocInfoStruct(InfoPtr);
            return 11;
        }
        InfoPtr->layer_val[InfoPtr->num_layers-1] = 3;
        itemp = GetCoordRanges(InfoPtr);
        if (itemp > 0)
        {   printf("\n%s\n",GetCoordRangesMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetCoordRangesMsg[itemp]);
            DeallocInfoStruct(InfoPtr);
            return 10;
        }
    } else if (InfoPtr->coord_mode == DISTANCE_TIME) /* 4 */
    {   itemp = GetCoordRanges(InfoPtr);
        if (itemp > 0)
        {   printf("\n%s\n",GetCoordRangesMsg[itemp]);
            if (log_file) fprintf(log_file,"%s\n",GetCoordRangesMsg[itemp]);
            DeallocInfoStruct(InfoPtr);
            return 10;
        }
    } else   /* all coord_modes except 2 and 4 */
    {   int skip,samp_val;
        long column_num;
        double time_val,time_inc;

        if ((InfoPtr->coord_mode <= 0) || (InfoPtr->coord_mode > NUM_COORD_MODES) )
        {   DeallocInfoStruct(InfoPtr);
            return 12;
        }
        /* determine number of columns and rows */
        column_num = InfoPtr->last_trace - InfoPtr->first_trace + 1;
        if (InfoPtr->skip_traces > 0)
            InfoPtr->num_cols = 2 + ((column_num-2)/(InfoPtr->skip_traces+1)); /* expect truncation */
        else
            InfoPtr->num_cols = column_num;
        InfoPtr->num_rows = InfoPtr->last_samp - InfoPtr->first_samp + 1;

        /* allocate storage */
        InfoPtr->Hvals = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
        InfoPtr->Vvals = (double *)malloc((size_t)InfoPtr->num_rows * sizeof(double));
        InfoPtr->Zvals = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
        if ( InfoPtr->Hvals == NULL || InfoPtr->Vvals == NULL ||
             InfoPtr->Zvals == NULL )
        {   DeallocInfoStruct(InfoPtr);
            return 14;
        }

        /* assign values to Hvals[] and Vvals[] */
        skip = InfoPtr->skip_traces + 1;
        if (InfoPtr->coord_mode == TRACE_TIME) /* 1 */
        {   column_num = InfoPtr->first_trace;
            for (col=0; col<InfoPtr->num_cols; col++,column_num+=skip)
                InfoPtr->Hvals[col] = column_num;
            if (InfoPtr->Hvals[InfoPtr->num_cols - 1] > InfoPtr->last_trace)
                InfoPtr->Hvals[InfoPtr->num_cols - 1] = InfoPtr->last_trace;
            time_val = InfoPtr->first_samp_time;
            time_inc = InfoPtr->ns_per_samp;
            for (row=0; row<InfoPtr->num_rows; row++,time_val+=time_inc)
                InfoPtr->Vvals[row] = time_val;
        }
        if (InfoPtr->coord_mode == TIME_TIME) /* 3 */
        {   time_val = InfoPtr->first_trace_time;
            time_inc = (1.0 / InfoPtr->trace_per_sec) * skip;
            for (col=0; col<InfoPtr->num_cols; col++,time_val+=time_inc)
                InfoPtr->Hvals[col] = time_val;
            if (InfoPtr->Hvals[InfoPtr->num_cols - 1] > InfoPtr->last_trace_time)
                InfoPtr->Hvals[InfoPtr->num_cols - 1] = InfoPtr->last_trace_time;
            time_val = InfoPtr->first_samp_time;
            time_inc = InfoPtr->ns_per_samp;
            for (row=0; row<InfoPtr->num_rows; row++,time_val+=time_inc)
                InfoPtr->Vvals[row] = time_val;
        }
        if (InfoPtr->coord_mode == TRACE_SAMPLE) /* 5 */
        {   column_num = InfoPtr->first_trace;
            for (col=0; col<InfoPtr->num_cols; col++,column_num+=skip)
                InfoPtr->Hvals[col] = column_num;
            if (InfoPtr->Hvals[InfoPtr->num_cols - 1] > InfoPtr->last_trace)
                InfoPtr->Hvals[InfoPtr->num_cols - 1] = InfoPtr->last_trace;
            samp_val = InfoPtr->first_samp;
            for (row=0; row<InfoPtr->num_rows; row++,samp_val++)
                InfoPtr->Vvals[row] = samp_val;
        }
    }
#if 0
    if (Debug)
    {   puts("GetParameters: Hvals[] to follow");
        getch();
        for (i=0; i<InfoPtr->num_cols; i++) printf("%6d %g\n",i,InfoPtr->Hvals[i]);
        getch();
    }
#endif
    /* Check viewport ranges */
    if (InfoPtr->VX2 <= InfoPtr->VX1 || InfoPtr->VY2 <= InfoPtr->VY1)
    {   DeallocInfoStruct(InfoPtr);
        return 18;
    }
    /* If portrait output orientation requested, check viewport limits */
    if (InfoPtr->portrait)
    {   if (InfoPtr->VX2 > 75.0)  InfoPtr->VX2 = 75.0;
    }

    /* See if local contrast stretching was requested and set flag */
    if (InfoPtr->local_offset && InfoPtr->local_change)
        InfoPtr->local_stretch = TRUE;

/***** Get window limits for data viewport if not specified yet ****/
    itemp = GetDataLimits(InfoPtr);
    if (itemp > 0)
    {   printf("\n%s\n",GetDataLimitsMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",GetDataLimitsMsg[itemp]);
        DeallocInfoStruct(InfoPtr);
        return 16;
    }

    /* check line-overlay values */
    if (InfoPtr->hline_int == INVALID_VALUE ||
        InfoPtr->hline_int == 0.0) InfoPtr->add_hlines = FALSE;
    if (InfoPtr->add_hlines)
    {   if (InfoPtr->hline_start == INVALID_VALUE)
        {   if (InfoPtr->hline_int < 0.0) /* working towards minimal value */
                InfoPtr->hline_start = _MAXX(InfoPtr->bottom,InfoPtr->top);
            else                          /* working towards maximum value */
                InfoPtr->hline_start = _MINN(InfoPtr->bottom,InfoPtr->top);
        }
        if (InfoPtr->hline_stop == INVALID_VALUE)
        {   if (InfoPtr->hline_int < 0.0) /* working towards minimal value */
                InfoPtr->hline_stop = _MINN(InfoPtr->bottom,InfoPtr->top);
            else                          /* working towards maximum value */
                InfoPtr->hline_stop = _MAXX(InfoPtr->bottom,InfoPtr->top);
        }
    }
    if (InfoPtr->vline_int == INVALID_VALUE ||
        InfoPtr->vline_int == 0.0) InfoPtr->add_vlines = FALSE;
    if (InfoPtr->add_vlines)
    {   if (InfoPtr->vline_start == INVALID_VALUE)
        {   if (InfoPtr->vline_int < 0.0) /* working towards minimal value */
                InfoPtr->vline_start = _MAXX(InfoPtr->left,InfoPtr->right);
            else                          /* working towards maximum value */
                InfoPtr->vline_start = _MINN(InfoPtr->left,InfoPtr->right);
        }
        if (InfoPtr->vline_stop == INVALID_VALUE)
        {   if (InfoPtr->vline_int < 0.0) /* working towards minimal value */
                InfoPtr->vline_stop = _MINN(InfoPtr->left,InfoPtr->right);
            else                          /* working towards maximum value */
                InfoPtr->vline_stop = _MAXX(InfoPtr->left,InfoPtr->right);
        }
    }

/***** Check wiggle trace mode values; delete tracevals out of range *****/
    if (InfoPtr->wiggle_color < 0)
    {   if (save_eps || save_pcx)
            InfoPtr->wiggle_color =   0; /* black on white */
        else
            InfoPtr->wiggle_color = 255; /* white on black */
    }
#if 0
    if (InfoPtr->wiggle_color >= 254 && (save_eps || save_pcx))
        InfoPtr->wiggle_color =   0; /* black on white */
#endif
    if (InfoPtr->num_wiggles > 0)
    {   int i,j,reversed,remove;

        if (InfoPtr->wiggle_traceval == NULL)
        {   InfoPtr->num_wiggles = 0;
        } else
        {   reversed = FALSE;
            if (InfoPtr->Hvals[0] > InfoPtr->Hvals[InfoPtr->num_cols-1])
                reversed = TRUE;
            for (i=0; i<InfoPtr->num_wiggles; i++)
            {   remove = FALSE;
                if (!reversed)
                {   if ( (InfoPtr->wiggle_traceval[i][0] < InfoPtr->Hvals[0]) ||
                         (InfoPtr->wiggle_traceval[i][0] > InfoPtr->Hvals[InfoPtr->num_cols-1])
                       )
                        remove = TRUE;
                } else
                {   if ( (InfoPtr->wiggle_traceval[i][0] > InfoPtr->Hvals[0]) ||
                         (InfoPtr->wiggle_traceval[i][0] < InfoPtr->Hvals[InfoPtr->num_cols-1])
                       )
                        remove = TRUE;
                }
                if (remove)
                {   for (j=i+1; j<InfoPtr->num_wiggles; j++)
                    {   InfoPtr->wiggle_traceval[j-1][0] = InfoPtr->wiggle_traceval[j][0];
                        InfoPtr->wiggle_traceval[j-1][1] = InfoPtr->wiggle_traceval[j][1];
                    }
                    free(InfoPtr->wiggle_traceval[InfoPtr->num_wiggles-1]);
                    InfoPtr->num_wiggles--;
                }
            }
        }
    }
/***** Check EPS output limits if HP DesignJet 750 selected *****/
    if (InfoPtr->use_designjet750)
    {   if (InfoPtr->hp_xoff < 0.0) InfoPtr->hp_xoff = 0.0;
        if (InfoPtr->hp_yoff < 0.0) InfoPtr->hp_yoff = 0.0;
        if ((InfoPtr->hp_xoff +InfoPtr->hp_x) > 48.0)
            InfoPtr->hp_x = 48.0 - InfoPtr->hp_xoff;
        if ((InfoPtr->hp_yoff +InfoPtr->hp_y) > 36.0)
            InfoPtr->hp_y = 36.0 - InfoPtr->hp_yoff;
        if ( (InfoPtr->hp_x/InfoPtr->hp_y) > (4.0/3.0) )
        {   InfoPtr->hp_x = InfoPtr->hp_y * (4.0/3.0);
        } else if ( (InfoPtr->hp_x/InfoPtr->hp_y) < (4.0/3.0) )
        {   InfoPtr->hp_y = InfoPtr->hp_x / (4.0/3.0);
        }
    }

/***** Check range gain values *****/
    if (InfoPtr->num_gain_on == 1 || InfoPtr->num_gain_on < 0)
    {   InfoPtr->num_gain_on = 0;
        if (InfoPtr->gain_on)
        {   free(InfoPtr->gain_on);
            InfoPtr->gain_on = NULL;
        }
    }
    if (InfoPtr->num_gain_off == 1 || InfoPtr->num_gain_off < 0)
    {   InfoPtr->num_gain_off = 0;
        if (InfoPtr->gain_off)
        {   free(InfoPtr->gain_off);
            InfoPtr->gain_off = NULL;
        }
    }
    if (InfoPtr->num_gain_on >= 2 && InfoPtr->gain_on == NULL)
        InfoPtr->num_gain_on = 0;
    if (InfoPtr->num_gain_off >= 2 && InfoPtr->gain_off == NULL)
        InfoPtr->num_gain_off = 0;

/***** Check axes settingd *****/
    if (InfoPtr->laxis_min == InfoPtr->laxis_max)
    {   InfoPtr->laxis_max = INVALID_VALUE;
        InfoPtr->laxis_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: laxis_max equals laxis_min: values reset to default\n");
    }
    if (InfoPtr->laxis_tick_int == 0.0)
    {   InfoPtr->laxis_tick_int = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: laxis_tick_int set to zero: value reset to default\n");
    }
    if (InfoPtr->laxis_tick_abs_min == InfoPtr->laxis_tick_abs_max)
    {   InfoPtr->laxis_tick_abs_max = INVALID_VALUE;
        InfoPtr->laxis_tick_abs_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: laxis_tick_abs_max equals laxis_tick_abs_min: values reset to default\n");
    }
    if (InfoPtr->laxis_tick_skip < 0)
    {   InfoPtr->laxis_tick_skip = 0;
        if (log_file) fprintf(log_file,"INPUT ERROR: laxis_tick_skip less than zero: value reset to default\n");
    }

    if (InfoPtr->raxis_min == InfoPtr->raxis_max)
    {   InfoPtr->raxis_max = INVALID_VALUE;
        InfoPtr->raxis_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: raxis_max equals raxis_min: values reset to default\n");
    }
    if (InfoPtr->raxis_tick_int == 0.0)
    {   InfoPtr->raxis_tick_int = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: raxis_tick_int set to zero: value reset to default\n");
    }
    if (InfoPtr->raxis_tick_abs_min == InfoPtr->raxis_tick_abs_max)
    {   InfoPtr->raxis_tick_abs_max = INVALID_VALUE;
        InfoPtr->raxis_tick_abs_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: raxis_tick_abs_max equals raxis_tick_abs_min: values reset to default\n");
    }
    if (InfoPtr->raxis_tick_skip < 0)
    {   InfoPtr->raxis_tick_skip = 0;
        if (log_file) fprintf(log_file,"INPUT ERROR: raxis_tick_skip less than zero: value reset to default\n");
    }

    if (InfoPtr->baxis_min == InfoPtr->baxis_max)
    {   InfoPtr->baxis_max = INVALID_VALUE;
        InfoPtr->baxis_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: baxis_max equals baxis_min: values reset to default\n");
    }
    if (InfoPtr->baxis_tick_int == 0.0)
    {   InfoPtr->baxis_tick_int = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: baxis_tick_int set to zero: value reset to default\n");
    }
    if (InfoPtr->baxis_tick_abs_min == InfoPtr->baxis_tick_abs_max)
    {   InfoPtr->baxis_tick_abs_max = INVALID_VALUE;
        InfoPtr->baxis_tick_abs_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: baxis_tick_abs_max equals baxis_tick_abs_min: values reset to default\n");
    }
    if (InfoPtr->baxis_tick_skip < 0)
    {   InfoPtr->baxis_tick_skip = 0;
        if (log_file) fprintf(log_file,"INPUT ERROR: baxis_tick_skip less than zero: value reset to default\n");
    }

    if (InfoPtr->taxis_min == InfoPtr->taxis_max)
    {   InfoPtr->taxis_max = INVALID_VALUE;
        InfoPtr->taxis_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: taxis_max equals taxis_min: values reset to default\n");
    }
    if (InfoPtr->taxis_tick_int == 0.0)
    {   InfoPtr->taxis_tick_int = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: taxis_tick_int set to zero: value reset to default\n");
    }
    if (InfoPtr->taxis_tick_abs_min == InfoPtr->taxis_tick_abs_max)
    {   InfoPtr->taxis_tick_abs_max = INVALID_VALUE;
        InfoPtr->taxis_tick_abs_min = INVALID_VALUE;
        if (log_file) fprintf(log_file,"INPUT ERROR: taxis_tick_abs_max equals taxis_tick_abs_min: values reset to default\n");
    }
    if (InfoPtr->taxis_tick_skip < 0)
    {   InfoPtr->taxis_tick_skip = 0;
        if (log_file) fprintf(log_file,"INPUT ERROR: taxis_tick_skip less than zero: value reset to default\n");
    }

/***** Check velocity hyperbola params *****/
    if (InfoPtr->ant_sep < 0.0) InfoPtr->plot_hyperbola = FALSE;
    if (InfoPtr->obj_depth < 0.0) InfoPtr->plot_hyperbola = FALSE;
    if (InfoPtr->obj_radius < 1.0E-3) InfoPtr->obj_radius = 1.0E-3;
    if (InfoPtr->obj_radius <= 0.0) InfoPtr->plot_hyperbola = FALSE;
    if (InfoPtr->hyp_vel < 0.01 || InfoPtr->hyp_vel > 0.30) InfoPtr->plot_hyperbola = FALSE;

#if 0
/***** Assign axes titles if not assigned *****/
    switch (InfoPtr->coord_mode)
    {    case TRACE_TIME:
            if (InfoPtr->laxis_title[0] == 0 && InfoPtr->laxis_tick_show)
                strcpy(InfoPtr->laxis_title,"TIME (NS)");
            if (InfoPtr->raxis_title[0] == 0 && InfoPtr->raxis_tick_show)
                strcpy(InfoPtr->raxis_title,"TIME (NS)");
            if (InfoPtr->baxis_title[0] == 0 && InfoPtr->baxis_tick_show)
                strcpy(InfoPtr->baxis_title,"TRACE NUMBER");
            if (InfoPtr->taxis_title[0] == 0 && InfoPtr->taxis_tick_show)
                strcpy(InfoPtr->taxis_title,"TRACE NUMBER");
            break;
         case DISTANCE_DISTANCE:
            if (InfoPtr->laxis_title[0] == 0 && InfoPtr->laxis_tick_show)
                strcpy(InfoPtr->laxis_title,"DISTANCE (M)");
            if (InfoPtr->raxis_title[0] == 0 && InfoPtr->raxis_tick_show)
                strcpy(InfoPtr->raxis_title,"DISTANCE (M)");
            if (InfoPtr->baxis_title[0] == 0 && InfoPtr->baxis_tick_show)
                strcpy(InfoPtr->baxis_title,"DISTANCE (M)");
            if (InfoPtr->taxis_title[0] == 0 && InfoPtr->taxis_tick_show)
                strcpy(InfoPtr->taxis_title,"DISTANCE (M)");
            break;
         case TIME_TIME:
            if (InfoPtr->laxis_title[0] == 0 && InfoPtr->laxis_tick_show)
                strcpy(InfoPtr->laxis_title,"TIME (NS)");
            if (InfoPtr->raxis_title[0] == 0 && InfoPtr->raxis_tick_show)
                strcpy(InfoPtr->raxis_title,"TIME (NS)");
            if (InfoPtr->baxis_title[0] == 0 && InfoPtr->baxis_tick_show)
                strcpy(InfoPtr->baxis_title,"TIME (S)");
            if (InfoPtr->taxis_title[0] == 0 && InfoPtr->taxis_tick_show)
                strcpy(InfoPtr->taxis_title,"TIME (S)");
            break;
         case DISTANCE_TIME:
            if (InfoPtr->laxis_title[0] == 0 && InfoPtr->laxis_tick_show)
                strcpy(InfoPtr->laxis_title,"TIME (NS)");
            if (InfoPtr->raxis_title[0] == 0 && InfoPtr->raxis_tick_show)
                strcpy(InfoPtr->raxis_title,"TIME (NS)");
            if (InfoPtr->baxis_title[0] == 0 && InfoPtr->baxis_tick_show)
                strcpy(InfoPtr->baxis_title,"DISTANCE (M)");
            if (InfoPtr->taxis_title[0] == 0 && InfoPtr->taxis_tick_show)
                strcpy(InfoPtr->taxis_title,"DISTANCE (M)");
            break;
         case TRACE_SAMPLE:
            if (InfoPtr->laxis_title[0] == 0 && InfoPtr->laxis_tick_show)
                strcpy(InfoPtr->laxis_title,"SAMPLE NUMBER");
            if (InfoPtr->raxis_title[0] == 0 && InfoPtr->raxis_tick_show)
                strcpy(InfoPtr->raxis_title,"SAMPLE NUMBER");
            if (InfoPtr->baxis_title[0] == 0 && InfoPtr->baxis_tick_show)
                strcpy(InfoPtr->baxis_title,"TRACE NUMBER");
            if (InfoPtr->taxis_title[0] == 0 && InfoPtr->taxis_tick_show)
                strcpy(InfoPtr->taxis_title,"TRACE NUMBER");
            break;
    }
#endif

    return 0;
}
/****************************** PrintUsageMsg() *****************************/
/* Prints message to user on stdout.
 *
 * Requires: <stdio.h>, <dos.h>.
 * Calls: puts, sprintf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: January 23, 1996; July 15, 1999;
 */
void PrintUsageMsg (void)
{
    char str[10];
    extern int ANSI_THERE;

    puts("");
    if (ANSI_THERE)  printf("%c%c32;40m",0x1B,0x5B);  /* green on black */
    puts("\n###############################################################################");
    puts("  Usage: GPR_DISP cmd_filename [override_cmd_filename]");
    puts("    Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.");
    puts("  This program displays one or more ground penetrating radar (GPR) files");
    puts("    to the monitor as an image or as wiggle traces and can save the display to");
    puts("    disk as an Encapsulated PostScript (EPS) file or a PCX graphics file.");
    puts("  Required command line arguments:");
    puts("    cmd_filename - name of text file that follows the \"CMD\" format.");
    puts("  Optional command line arguments:");
    puts("    override_cmd_filename - command file whose values override all others.");
    puts("  Look at \"GPR_DISP.CMD\" and \"GPR_DISP.TXT\" for keyword file format, valid");
    puts("    keywords, and documentation.");
    puts("  Example call: GPR_DISP dfile1.cmd");
    printf("  Version %s - Last Revision Date: %s at %s\n",GPR_DISP_VER,DATETIME,__TIME__);
    puts("###############################################################################");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B);  /* white on black */
}
/***************************** InitParameters() *****************************/
/* Initialize all processing parameters to start-up (default) values.
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <string.h>, "gpr_disp.h".
 * Calls: strcpy.
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: June 24, 1996; July 15, 1999; August 23, 1999; September 3, 1999;
 * November 1, 2001;
 */
void InitParameters (struct DispParamInfoStruct *InfoPtr)
{
    extern int Debug;

    InfoPtr->cmd_filename[0]         = 0;
    InfoPtr->next_cmd_filename[0]    = 0;
    InfoPtr->override_cmd_filename[0] = 0;
    InfoPtr->inf_infilename[0]       = 0;
    InfoPtr->dat_infilename[0]       = 0;
    InfoPtr->lbl_infilename[0]       = 0;
    InfoPtr->mrk_infilename[0]       = 0;
    InfoPtr->xyz_infilename[0]       = 0;
    InfoPtr->input_format            = 0;
    InfoPtr->input_datatype          = 0;
    InfoPtr->max_data_val            = 0.0;
    InfoPtr->min_data_val            = 0.0;
    InfoPtr->if_image                = FALSE;
    InfoPtr->pcx_outfilename[0]      = 0;
    InfoPtr->eps_outfilename[0]      = 0;
    InfoPtr->eps_copies              = 1;  /* no longer used in command file */
    InfoPtr->display_all             = FALSE;
    InfoPtr->display_none            = FALSE;
    InfoPtr->file_header_bytes       = 0;
    InfoPtr->trace_header_bytes      = 0;
    InfoPtr->channel                 = 0;
    InfoPtr->ant_freq                = 0;
    InfoPtr->first_trace             = 0;
    InfoPtr->last_trace              = 0;
    InfoPtr->total_traces            = 0;
    InfoPtr->skip_traces             = 0;
    InfoPtr->lock_first_samp         = FALSE;
    InfoPtr->first_samp              = 0;
    InfoPtr->last_samp               = 0;
    InfoPtr->total_samps             = 0;
    InfoPtr->total_time              = 0.0;
    InfoPtr->ns_per_samp             = 0.0;
    InfoPtr->trace_per_sec           = 0.0;
    InfoPtr->trace_per_m             = 0.0;
    InfoPtr->start_pos               = 0.0;
    InfoPtr->final_pos               = 0.0;
    InfoPtr->step_size               = 0.0;
    InfoPtr->first_trace_time        = INVALID_VALUE;
    InfoPtr->last_trace_time         = INVALID_VALUE;
    InfoPtr->first_samp_time         = INVALID_VALUE;
    InfoPtr->last_samp_time          = INVALID_VALUE;
    InfoPtr->num_bad                 = 0;
    InfoPtr->bad_traces              = NULL;
    InfoPtr->num_ticks               = 0;
    InfoPtr->tick_tracenum           = NULL;
    InfoPtr->tick_xyz                = NULL;
    InfoPtr->num_layers              = 0;
    InfoPtr->layer_rdp               = NULL;
    InfoPtr->layer_mode              = NULL;
    InfoPtr->layer_val               = NULL;
    InfoPtr->Hvals                   = NULL;
    InfoPtr->Vvals                   = NULL;
    InfoPtr->Zvals                   = NULL;
    InfoPtr->title[0]                = 0;
    InfoPtr->video_mode              = 0;
    InfoPtr->stretch                 = 0;
    InfoPtr->background              = FALSE;
    InfoPtr->abs_val                 = FALSE;
    InfoPtr->square                  = FALSE;
    InfoPtr->inst_amp                = FALSE;
    InfoPtr->inst_pow                = FALSE;
    InfoPtr->envelope                = FALSE;
    InfoPtr->local_stretch           = FALSE;
    InfoPtr->local_offset            = 0.0;
    InfoPtr->local_change            = 0.0;
    InfoPtr->brightness              = 0;
    InfoPtr->negative                = FALSE;
    InfoPtr->background              = FALSE;
    InfoPtr->show_markers            = 0;
    InfoPtr->show_marker_width       = 1;
    InfoPtr->range                   = 255;
    InfoPtr->change_gain             = FALSE;
    InfoPtr->num_gain_off            = 0;
    InfoPtr->gain_off                = NULL;
    InfoPtr->num_gain_on             = 0;
    InfoPtr->gain_on                 = NULL;
    InfoPtr->epc_threshold           = 5;
    InfoPtr->epc_contrast            = 255;
    InfoPtr->epc_gain                = 2.0;
    InfoPtr->epc_sign                = 0;
    InfoPtr->display_wiggle          = FALSE;
    InfoPtr->num_wiggles             = 0;
    InfoPtr->wiggle_traceval         = NULL;
    InfoPtr->wiggle_color            = -1;
    InfoPtr->wiggle_fill             = 0;
    InfoPtr->wiggle_width            = 10.0;
    InfoPtr->wiggle_clip             = 100.0;
    InfoPtr->use_11x17               = FALSE;
    InfoPtr->portrait                = FALSE;
    InfoPtr->use_designjet750        = FALSE;
    InfoPtr->hp_x                    = 20.00;
    InfoPtr->hp_y                    = 15.00;
    InfoPtr->hp_xoff                 =  1.00;
    InfoPtr->hp_yoff                 =  1.00;
    InfoPtr->vert_exag               =   1.0;
    InfoPtr->VX1                     =  10.0;
    InfoPtr->VX2                     = 123.33333;
    InfoPtr->VY1                     =  10.0;
    InfoPtr->VY2                     =  90.0;
    InfoPtr->left                    = INVALID_VALUE;
    InfoPtr->right                   = INVALID_VALUE;
    InfoPtr->bottom                  = INVALID_VALUE;
    InfoPtr->top                     = INVALID_VALUE;
    InfoPtr->coord_mode              = 1;
    InfoPtr->horiz_mode              = 3;
    InfoPtr->horiz_start             = INVALID_VALUE;
    InfoPtr->horiz_stop              = INVALID_VALUE;
    InfoPtr->plot_hyperbola          = FALSE;
    InfoPtr->ant_sep                 = 0.0;
    InfoPtr->obj_depth               = 0.0;
    InfoPtr->obj_radius              = 0.0;
    InfoPtr->obj_loc                 = 0.0;
    InfoPtr->hyp_vel                 = 0.0;
    InfoPtr->hyp_color               = 0;
    InfoPtr->save_pcx                = FALSE;
    InfoPtr->save_eps                = FALSE;
    InfoPtr->created                 = FALSE;
    InfoPtr->image                   = NULL;
    InfoPtr->num_cols                = 0;
    InfoPtr->num_rows                = 0;
    strcpy(InfoPtr->eps_select_font,"Times-Roman");
    InfoPtr->caption_font_size       = 12;
    InfoPtr->caption_left_edge       = 1.25;
    InfoPtr->caption_bottom_edge     = 1.00;
    InfoPtr->caption_long_edge       = FALSE;
    InfoPtr->caption                 = NULL;
    InfoPtr->pagenum_font_size       = 12;
    InfoPtr->pagenum_left_edge       = 7.25;
    InfoPtr->pagenum_bottom_edge     = 10.00;
    InfoPtr->pagenum                 = NULL;
    strcpy(InfoPtr->hpgl_select_font,"romtrplx");
    InfoPtr->add_hlines              = FALSE;
    InfoPtr->hline_start             = INVALID_VALUE;
    InfoPtr->hline_int               = INVALID_VALUE;
    InfoPtr->hline_stop              = INVALID_VALUE;
    InfoPtr->hline_style             = 0;
    InfoPtr->add_vlines              = FALSE;
    InfoPtr->vline_start             = INVALID_VALUE;
    InfoPtr->vline_int               = INVALID_VALUE;
    InfoPtr->vline_stop              = INVALID_VALUE;
    InfoPtr->vline_style             = 0;
    InfoPtr->laxis_title[0]          = 0;
    InfoPtr->laxis_title_offset      = 0;
    InfoPtr->laxis_max               = INVALID_VALUE;
    InfoPtr->laxis_min               = INVALID_VALUE;
    InfoPtr->laxis_tick_show         = FALSE;
    InfoPtr->laxis_tick_start        = INVALID_VALUE;
    InfoPtr->laxis_tick_int          = INVALID_VALUE;
    InfoPtr->laxis_tick_abs_min      = INVALID_VALUE;
    InfoPtr->laxis_tick_abs_max      = INVALID_VALUE;
    InfoPtr->laxis_tick_num          = 9;
    InfoPtr->laxis_tick_ano          = FALSE;
    InfoPtr->laxis_tick_skip         = 0;
    InfoPtr->laxis_tick_mid_skip     = -1;
    InfoPtr->laxis_title_size        = 0.85;
    InfoPtr->laxis_ano_left          = 0;
    InfoPtr->laxis_ano_right         = -1;
    InfoPtr->laxis_ano_size          = 0.80;
    InfoPtr->raxis_title[0]          = 0;
    InfoPtr->raxis_title_offset      = 0;
    InfoPtr->raxis_max               = INVALID_VALUE;
    InfoPtr->raxis_min               = INVALID_VALUE;
    InfoPtr->raxis_tick_show         = FALSE;
    InfoPtr->raxis_tick_start        = INVALID_VALUE;
    InfoPtr->raxis_tick_int          = INVALID_VALUE;
    InfoPtr->raxis_tick_abs_min      = INVALID_VALUE;
    InfoPtr->raxis_tick_abs_max      = INVALID_VALUE;
    InfoPtr->raxis_tick_num          = 9;
    InfoPtr->raxis_tick_ano          = FALSE;
    InfoPtr->raxis_tick_skip         = 0;
    InfoPtr->raxis_tick_mid_skip     = -1;
    InfoPtr->raxis_ano_left          = 0;
    InfoPtr->raxis_ano_right         = -1;
    InfoPtr->raxis_title_size        = 0.85;
    InfoPtr->raxis_ano_size          = 0.80;
    InfoPtr->taxis_title[0]          = 0;
    InfoPtr->taxis_title_offset      = 0;
    InfoPtr->taxis_max               = INVALID_VALUE;
    InfoPtr->taxis_min               = INVALID_VALUE;
    InfoPtr->taxis_tick_show         = FALSE;
    InfoPtr->taxis_tick_start        = INVALID_VALUE;
    InfoPtr->taxis_tick_int          = INVALID_VALUE;
    InfoPtr->taxis_tick_abs_min      = INVALID_VALUE;
    InfoPtr->taxis_tick_abs_max      = INVALID_VALUE;
    InfoPtr->taxis_tick_num          = 9;
    InfoPtr->taxis_tick_ano          = FALSE;
    InfoPtr->taxis_tick_skip         = 0;
    InfoPtr->taxis_tick_mid_skip     = -1;
    InfoPtr->taxis_ano_left          = 0;
    InfoPtr->taxis_ano_right         = -1;
    InfoPtr->taxis_title_size        = 1.10;
    InfoPtr->taxis_ano_size          = 0.80;
    InfoPtr->baxis_title[0]          = 0;
    InfoPtr->baxis_title_offset      = 0;
    InfoPtr->baxis_max               = INVALID_VALUE;
    InfoPtr->baxis_min               = INVALID_VALUE;
    InfoPtr->baxis_tick_show         = FALSE;
    InfoPtr->baxis_tick_start        = INVALID_VALUE;
    InfoPtr->baxis_tick_int          = INVALID_VALUE;
    InfoPtr->baxis_tick_abs_min      = INVALID_VALUE;
    InfoPtr->baxis_tick_abs_max      = INVALID_VALUE;
    InfoPtr->baxis_tick_num          = 9;
    InfoPtr->baxis_tick_ano          = FALSE;
    InfoPtr->baxis_tick_skip         = 0;
    InfoPtr->baxis_tick_mid_skip     = -1;
    InfoPtr->baxis_ano_left          = 0;
    InfoPtr->baxis_ano_right         = -1;
    InfoPtr->baxis_title_size        = 0.85;
    InfoPtr->baxis_ano_size          = 0.80;
}
/***************************** GetCmdFileArgs() *****************************/
/* Get processing parameters from user from a command file.
 * Not used: v_12_same.
 *
const char *GPR_DISP_CMDS[]=
{   "data_infilename","lbl_infilename","mrk_infilename","xyz_infilename","eps_outfilename",
    "channel","first_samp","last_samp","trace_per_sec","num_bad",
* 10 *
    "bad_traces[]","abs_val","tick_tracenum[]","tick_xyz[][]","num_layers",
    "layer_rdp[]","layer_mode[]","layer_val[]","title","video_mode",
* 20 *
    "stretch","range","epc_threshold","epc_contrast","epc_gain",
    "epc_sign","max_data_val","use_11x17","vert_exag","vx1",
* 30 *
    "vx2","vy1","vy2","left","right",
    "bottom","top","coord_mode","horiz_mode","horiz_start",
* 40 *
    "horiz_stop","debug","batch","laxis_title","laxis_max",
    "laxis_min","laxis_tick_show","laxis_tick_start","laxis_tick_int","laxis_tick_num",
* 50 *
    "laxis_tick_ano","laxis_tick_skip","laxis_title_size","laxis_ano_size","raxis_title",
    "raxis_max","raxis_min","raxis_tick_show","raxis_tick_start","raxis_tick_int",
* 60 *
    "raxis_tick_num","raxis_tick_ano","raxis_tick_skip","raxis_title_size","raxis_ano_size",
    "taxis_title","taxis_max","taxis_min","taxis_tick_show","taxis_tick_start",
* 70 *
    "taxis_tick_int","taxis_tick_num","taxis_tick_ano","taxis_tick_skip","taxis_title_size",
    "taxis_ano_size","baxis_title","baxis_max","baxis_min","baxis_tick_show",
* 80 *
    "baxis_tick_start","baxis_tick_int","baxis_tick_num","baxis_tick_ano","baxis_tick_skip",
    "baxis_title_size","baxis_ano_size","laxis_ano_left","laxis_ano_right","raxis_ano_left",
* 90 *
    "raxis_ano_right","taxis_ano_left","taxis_ano_right","baxis_ano_left","baxis_ano_right",
    "first_trace_time","last_trace_time","first_trace","last_trace","lock_first_samp",
* 100 *
    "hpgl_select_font","first_samp_time","last_samp_time","background","show_markers",
    "portrait","next_cmd_filename","inst_amp","brightness","negative",
* 110 *
    "laxis_tick_abs_min","laxis_tick_abs_max","raxis_tick_abs_min","raxis_tick_abs_max","taxis_tick_abs_min",
    "taxis_tick_abs_max","baxis_tick_abs_min","baxis_tick_abs_max","eps_select_font","caption_font_size",
* 120 *
    "caption_left_edge","caption_long_edge","caption","caption_bottom_edge","pagenum_font_size",
    "pagenum_left_edge","pagenum_bottom_edge","pagenum","local_offset","local_change",
* 130 *
    "display_all","display_none","eps_copies","laxis_title_offset","laxis_tick_mid_skip",
    "raxis_title_offset","raxis_tick_mid_skip","taxis_title_offset","taxis_tick_mid_skip","baxis_title_offset",
* 140 *
    "baxis_tick_mid_skip","square","file_header_bytes","trace_header_bytes","other_format",
    "input_datatype","samples_per_trace","total_time","row_by_row","min_data_val",
* 150 *
    "skip_traces","show_marker_width","display_wiggle","num_wiggles","wiggle_traceval[]",
    "wiggle_color","wiggle_fill","wiggle_width","wiggle_clip","change_gain",
* 160 *
    "num_gain_off","gain_off[]","num_gain_on","gain_on[]","pcx_outfilename",
    "add_hlines","hline_start","hline_int","hline_stop","add_vlines",
* 170 *
    "vline_start","vline_int","vline_stop","hline_style","vline_style",
    "use_designjet750","hp_x","hp_y","hp_xoff","hp_yoff",
* 180 *
    "envelope","inst_pow","plot_hyperbola","ant_sep","obj_depth",
    "obj_radius","obj_loc","hyp_vel","hyp_color",NULL,
} ;  - eps_copies not used in the command file
     - tick_tracenum[], tick_xyz[][] not used in the command file
     - 12, 13, 132 not used
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <ctype.h>, <stdio.h>, <stdlib.h>, <string.h>, "gpr_disp.h".
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, strupr, strlwr, strncpy, atol, malloc, free, isdigit,
 *        Trimstr, strlen, getch.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: June 24, 1996; April 30, 1997; May 14, 1997; July 1, 1997;
 *       July 31, 1997; February 2, 1998; July 15, 1999; November 6, 2001;
 */
int GetCmdFileArgs (struct DispParamInfoStruct *InfoPtr)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp3,*cp4,cmdstr[30];
    int found,i,num,itemp,have_string;
    int dat_in_found = FALSE;
    long ltemp;
    double dtemp;
    FILE *infile = NULL;
    extern int Debug,Batch,Batchset,Debugset;

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
        while (GPR_DISP_CMDS[i])  /* requires last member to be NULL */
        {   if (strcmp(strlwr(cmdstr),GPR_DISP_CMDS[i]) == 0)
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
/* 11, 12, 13, 132 not used */
        switch (found)       /* cases depend on same order in GPR_DISP_CMD[] */
        {   case 0:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past first quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->dat_infilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->dat_infilename);
                if (Debug)  printf("data file = %s\n",InfoPtr->dat_infilename);
                if (strstr(InfoPtr->dat_infilename,".DT1") != NULL)
                {   strcpy(InfoPtr->inf_infilename,InfoPtr->dat_infilename);
                    cp2 =  strstr(InfoPtr->inf_infilename,".");
                    *cp2 = 0;
                    strcat(InfoPtr->inf_infilename,".HD");
                    if (Debug) printf("info file = %s\n",InfoPtr->inf_infilename);
                }
                if (InfoPtr->dat_infilename[0]) dat_in_found = TRUE;
                break;
            case 1:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->lbl_infilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->lbl_infilename);
                if (Debug) printf("lbl_infilename = %s\n",InfoPtr->lbl_infilename);
                break;
            case 2:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->mrk_infilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->mrk_infilename);
                if (Debug) printf("mrk_infilename = %s\n",InfoPtr->mrk_infilename);
                break;
            case 3:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->xyz_infilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->xyz_infilename);
                if (Debug) printf("xyz_infilename = %s\n",InfoPtr->xyz_infilename);
                break;
            case 4:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->eps_outfilename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->eps_outfilename);
                if (InfoPtr->eps_outfilename[0]) InfoPtr->save_eps = TRUE;
                if (Debug) printf("eps_outfilename = %s\n",InfoPtr->eps_outfilename);
                break;
            case 5:
                itemp = atoi(cp);
                /* let user use 1 to 4, we need 0 to 3 here */
                if (itemp >= 1)  InfoPtr->channel = itemp - 1;
                if (Debug) printf("channel = %d\n",InfoPtr->channel);
                break;
            case 6:
                ltemp = atol(cp);
                if (ltemp >= 0)  InfoPtr->first_samp = ltemp;
                break;
            case 7:
                ltemp = atol(cp);
                if (ltemp >= 0)  InfoPtr->last_samp = ltemp;
                break;
            case 8:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->trace_per_sec = INVALID_VALUE;
                    }
                } else
                {   dtemp = atof(cp);
                    if (dtemp >= 0.0)  InfoPtr->trace_per_sec = dtemp;
                }
                break;
            case 9:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->num_bad = itemp;
                break;
            case 10:
            {   char *cp_next = (char *)TrimStr(cp);
                int bad_found = 0;

                /* number of bad traces must be specified first */
                if (InfoPtr->num_bad <= 0) break;

                /* allocate storage for traces */
                InfoPtr->bad_traces = (long *)malloc(InfoPtr->num_bad * sizeof(long));
                if (InfoPtr->bad_traces == NULL)
                {   InfoPtr->num_bad = 0;
                    if (Debug) puts("\nmalloc failed for bad_traces[]");
                    break;              /* break out of case */
                }

                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (bad_found < InfoPtr->num_bad)
                        {   InfoPtr->bad_traces[bad_found] = atol(cp_next);
                            bad_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (bad_found < InfoPtr->num_bad)
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
                if (bad_found < InfoPtr->num_bad)
                {   if (Debug) puts("\nbad_found < num_bad");
                    InfoPtr->num_bad = 0;
                    free(InfoPtr->bad_traces);
                    InfoPtr->bad_traces = NULL;
                }
                break;
            }
            case 11:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->abs_val = FALSE;
                else             InfoPtr->abs_val = TRUE;
                break;
            case 12:
            {   char *cp_next = (char *)TrimStr(cp);
                int ticks_found = 0;

                /* number of tick marks must be specified first */
                if (InfoPtr->num_ticks <= 0) break;

                /* allocate storage for traces */
                InfoPtr->tick_tracenum = (long *)malloc(InfoPtr->num_ticks * sizeof(long));
                if (InfoPtr->tick_tracenum == NULL)
                {   InfoPtr->num_ticks = 0;
                    if (Debug) puts("\nmalloc failed for tick_tracenum[]");
                    break;              /* break out of case */
                }

                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (ticks_found < InfoPtr->num_ticks)
                        {   InfoPtr->tick_tracenum[ticks_found] = atol(cp_next);
                            ticks_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (ticks_found < InfoPtr->num_ticks)
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
                if (ticks_found < InfoPtr->num_ticks)
                {   if (Debug) puts("\ntick_found < num_ticks");
                    InfoPtr->num_ticks = 0;
                    free(InfoPtr->tick_tracenum);
                    InfoPtr->tick_tracenum = NULL;
                }
                break;
            }
            case 13:
            {   char *cp_next = (char *)TrimStr(cp);
                int ticks_found = 0;
                int i,j,xyz_found;

                /* number of tick marks must be specified first */
                if (InfoPtr->num_ticks <= 0) break;

                /* allocate storage for traces */
                InfoPtr->tick_xyz = (double **)malloc(InfoPtr->num_ticks * sizeof(double *));
                if (InfoPtr->tick_xyz == NULL)
                {   InfoPtr->num_ticks = 0;
                    if (Debug) puts("\nmalloc failed for tick_xyz[]");
                    break;              /* break out of case */
                }
                for (i=0; i<InfoPtr->num_ticks; i++)
                {   InfoPtr->tick_xyz[i] = (double *)malloc(3 * sizeof(double));
                    if (InfoPtr->tick_xyz[i] == NULL)
                    {   for (j=0; j<i; j++) free(InfoPtr->tick_xyz[j]);
                        free(InfoPtr->tick_xyz);
                        InfoPtr->tick_xyz = NULL;
                        if (Debug) puts("\nmalloc failed for tick_xyz[][]");
                        i = -1;
                        break;          /* break out of for loop */
                    }
                }
                if (i == -1) break;     /* break out of case */

                xyz_found = 0;
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (ticks_found < InfoPtr->num_ticks)
                        {   InfoPtr->tick_xyz[ticks_found][xyz_found] = atof(cp_next);
                            xyz_found++;
                            if (xyz_found >= 3)
                            {   xyz_found = 0;
                                ticks_found++;
                            }
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (ticks_found < InfoPtr->num_ticks)
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
                if (ticks_found < InfoPtr->num_ticks || xyz_found != 0)
                {   if (Debug) puts("\nticks_found < num_ticks or xyz_found !=0");
                    InfoPtr->num_ticks = 0;
                    for (i=0; i<InfoPtr->num_ticks; i++)
                        free(InfoPtr->tick_xyz[i]);
                    free(InfoPtr->tick_xyz);
                    InfoPtr->tick_xyz = NULL;
                }
                break;
            }
            case 14:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <= MAX_LAYERS)
                   InfoPtr->num_layers = itemp;
                break;
            case 15:
            {   char *cp_next = (char *)TrimStr(cp);
                int rdp_found = 0;

                /* number of layers must be specified first */
                if (InfoPtr->num_layers <= 0) break;

                /* allocate storage for traces */
                InfoPtr->layer_rdp = (double *)malloc(InfoPtr->num_layers * sizeof(double));
                if (InfoPtr->layer_rdp == NULL)
                {   InfoPtr->num_layers = 0;
                    if (Debug) puts("\nmalloc failed for layer_rdp[]");
                    break;              /* break out of case */
                }

                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (rdp_found < InfoPtr->num_layers)
                        {   InfoPtr->layer_rdp[rdp_found] = atof(cp_next);
                            rdp_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (rdp_found < InfoPtr->num_layers)
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
                if (rdp_found < InfoPtr->num_layers)
                {   if (Debug) puts("\nrdp_found < num_layers");
                    InfoPtr->num_layers = 0;
                    free(InfoPtr->layer_rdp);
                    InfoPtr->layer_rdp = NULL;
                }
                break;
            }
            case 16:
            {   char *cp_next = (char *)TrimStr(cp);
                int mode_found = 0;

                /* number of layers must be specified first */
                if (InfoPtr->num_layers <= 0) break;

                /* allocate storage for traces */
                InfoPtr->layer_mode = (int *)malloc(InfoPtr->num_layers * sizeof(int));
                if (InfoPtr->layer_mode == NULL)
                {   InfoPtr->num_layers = 0;
                    if (Debug) puts("\nmalloc failed for layer_mode[]");
                    break;              /* break out of case */
                }

                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (mode_found < InfoPtr->num_layers)
                        {   cp3 = strchr(cp_next,' '); /* look for next space */
                            cp2 = strchr(cp_next,'"'); /* look for first quote */
                            if (cp2 != NULL &&
                               ((cp3 != NULL && cp2 < cp3) || cp3==NULL) &&
                               !isdigit(cp2[1]))
                            {   cp_next = cp2 + 1;     /* step past quote */
                                cp2 = strchr(cp_next,'"');  /* look for second quote */
                                if (cp2 != NULL)
                                {   *cp2 = 0;          /* temporarily truncate */
                                    if (strchr(cp_next,'D') || strchr(cp_next,'d'))
                                    {   InfoPtr->layer_mode[mode_found] = 1;
                                        mode_found++;
                                    } else if (strchr(cp_next,'E') || strchr(cp_next,'e'))
                                    {   InfoPtr->layer_mode[mode_found] = 2;
                                        mode_found++;
                                    } else if (strchr(cp_next,'I') || strchr(cp_next,'i'))
                                    {   InfoPtr->layer_mode[mode_found] = 3;
                                        mode_found++;
                                    }
                                    cp_next = cp2 + 1; /* step past truncation */
                                }
                            } else
                            {   InfoPtr->layer_mode[mode_found] = atoi(cp_next);
                                mode_found++;
                            }
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (mode_found < InfoPtr->num_layers)
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
                if (mode_found < InfoPtr->num_layers)
                {   if (Debug) puts("\nmode_found < num_layers");
                    InfoPtr->num_layers = 0;
                    free(InfoPtr->layer_mode);
                    InfoPtr->layer_mode = NULL;
                }
                break;
            }
            case 17:
            {   char *cp_next = (char *)TrimStr(cp);
                int val_found = 0;

                /* number of layers must be specified first */
                if (InfoPtr->num_layers <= 0) break;

                /* allocate storage for traces */
                InfoPtr->layer_val = (double *)malloc(InfoPtr->num_layers * sizeof(double));
                if (InfoPtr->layer_val == NULL)
                {   InfoPtr->num_layers = 0;
                    if (Debug) puts("\nmalloc failed for layer_val[]");
                    break;              /* break out of case */
                } else                  /* initialize values */
                {   for (i=0; i<InfoPtr->num_layers; i++)
                        InfoPtr->layer_val[i] = INVALID_VALUE;
                }

                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (val_found < InfoPtr->num_layers - 1)
                        {   InfoPtr->layer_val[val_found] = atof(cp_next);
                            val_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (val_found < InfoPtr->num_layers - 1)
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
                if (val_found < InfoPtr->num_layers - 1)
                {   if (Debug) puts("\nval_found < num_layers");
                    InfoPtr->num_layers = 0;
                    free(InfoPtr->layer_val);
                    InfoPtr->layer_val = NULL;
                }
                break;
            }
            case 18:
                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (cp2 == cp) break;
                    } else
                    {   break;
                    }
                } else
                    break;

                strncpy(InfoPtr->title,cp,MAX_TITLE_LEN-1);
                break;
            case 19:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->video_mode = itemp;
                break;
            case 20:
                itemp = atoi(cp);
                if (itemp <  0)  itemp = 0;
                if (itemp > 99)  itemp = 99;
                InfoPtr->stretch = itemp;
                break;
            case 21:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->range = itemp;
                break;
            case 22:
                itemp = atoi(cp);
                if (itemp>=0 && itemp<=255) InfoPtr->epc_threshold = itemp;
                break;
            case 23:
                itemp = atoi(cp);
                if (itemp>=0 && itemp<=255) InfoPtr->epc_contrast = itemp;
                break;
            case 24:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->epc_gain = INVALID_VALUE;
                    }
                } else
                {   dtemp = atof(cp);
                    if (dtemp > 0.0) InfoPtr->epc_gain = dtemp;
                }
                break;
            case 25:
                itemp = atoi(cp);
                if (itemp == 0) InfoPtr->epc_sign = 0;
                else if (itemp > 0) InfoPtr->epc_sign = 1;
                else if (itemp < 0) InfoPtr->epc_sign = -1;
                break;
            case 26:
                InfoPtr->max_data_val = atof(cp);
                if (Debug) printf("max_data_val = %g\n",InfoPtr->max_data_val);
                break;
            case 27:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->use_11x17 = 0;
                else             InfoPtr->use_11x17 = 1;
                break;
            case 28:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->vert_exag = INVALID_VALUE;
                    }
                } else
                {   dtemp = atof(cp);
                    if (dtemp > 0.0) InfoPtr->vert_exag = dtemp;
                }
                break;
            case 29:         /* NOTE: viewport values must be >= 0 */
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->VX1 = dtemp;
                break;
            case 30:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->VX2 = dtemp;
                break;
            case 31:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->VY1 = dtemp;
                break;
            case 32:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->VY2 = dtemp;
                break;
            case 33:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->left = INVALID_VALUE;
                    }
                } else
                    InfoPtr->left = atof(cp);
                break;
            case 34:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->right = INVALID_VALUE;
                    }
                } else
                    InfoPtr->right = atof(cp);
                break;
            case 35:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->bottom = INVALID_VALUE;
                    }
                } else
                    InfoPtr->bottom= atof(cp);
                break;
            case 36:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->top = INVALID_VALUE;
                    }
                } else
                    InfoPtr->top = atof(cp);
                break;
            case 37:
                itemp = atoi(cp);
                if (itemp > 0) InfoPtr->coord_mode = itemp;
                break;
            case 38:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        strupr(cp);           /* convert to upper case */
                        if      (strstr(cp,"X")) itemp = 1;
                        else if (strstr(cp,"Y")) itemp = 2;
                        else if (strstr(cp,"T")) itemp = 3;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp>0 && itemp<4) InfoPtr->horiz_mode = itemp;
                break;
            case 39:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->horiz_start = INVALID_VALUE;
                    }
                } else
                    InfoPtr->horiz_start = atof(cp);
                break;
            case 40:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->horiz_stop = INVALID_VALUE;
                    }
                } else
                    InfoPtr->horiz_stop = atof(cp);
                break;
            case 41:
                if (Debugset) break;         /* only use first command file */
                itemp = 0;
                cp2 = strchr(cp,'"');        /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  Debug = FALSE;
                else             Debug = TRUE;
                Debugset = TRUE;
                break;
            case 42:
                if (Batchset) break;         /* only use first command file */
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
                    itemp = atoi(cp);
                if (itemp == 0)  Batch = FALSE;
                else             Batch = TRUE;
                break;
            case 43:
                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (cp2 == cp) break;
                    } else
                    {   break;
                    }
                } else
                    break;

                strncpy(InfoPtr->laxis_title,cp,MAX_TITLE_LEN-1);
                strcat(InfoPtr->laxis_title,"\0");
                break;
            case 44:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->laxis_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->laxis_max = atof(cp);
                break;
            case 45:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->laxis_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->laxis_min = atof(cp);
                break;
            case 46:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->laxis_tick_show = FALSE;  /* meters */
                else             InfoPtr->laxis_tick_show = TRUE;
                break;
            case 47:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->laxis_tick_start = INVALID_VALUE;
                    }
                } else
                    InfoPtr->laxis_tick_start = atof(cp);
                break;
            case 48:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->laxis_tick_int = INVALID_VALUE;
                    }
                } else
                    InfoPtr->laxis_tick_int = atof(cp);
                break;
            case 49:
                InfoPtr->laxis_tick_num = atoi(cp);
                break;
            default:
                break;
        }    /* end of 1st switch(found) block */
        switch (found)       /* cases depend on same order in GPR_DISP_CMD[] */
        {   case 50:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->laxis_tick_ano = FALSE;  /* meters */
                else             InfoPtr->laxis_tick_ano = TRUE;
                break;
            case 51:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->laxis_tick_skip = itemp;
                break;
            case 52:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->laxis_title_size = dtemp;
                break;
            case 53:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->laxis_ano_size = dtemp;
                break;
            case 54:
                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (cp2 == cp) break;
                    } else
                    {   break;
                    }
                } else
                    break;

                strncpy(InfoPtr->raxis_title,cp,MAX_TITLE_LEN-1);
                strcat(InfoPtr->raxis_title,"\0");
                break;
            case 55:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->raxis_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->raxis_max = atof(cp);
                break;
            case 56:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->raxis_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->raxis_min = atof(cp);
                break;
            case 57:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->raxis_tick_show = FALSE;  /* meters */
                else             InfoPtr->raxis_tick_show = TRUE;
                break;
            case 58:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->raxis_tick_start = INVALID_VALUE;
                    }
                } else
                    InfoPtr->raxis_tick_start = atof(cp);
                break;
            case 59:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->raxis_tick_int = INVALID_VALUE;
                    }
                } else
                    InfoPtr->raxis_tick_int = atof(cp);
                break;
            case 60:
                InfoPtr->raxis_tick_num = atoi(cp);
                break;
            case 61:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->raxis_tick_ano = FALSE;  /* meters */
                else             InfoPtr->raxis_tick_ano = TRUE;
                break;
            case 62:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->raxis_tick_skip = itemp;
                break;
            case 63:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->raxis_title_size = dtemp;
                break;
            case 64:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->raxis_ano_size = dtemp;
                break;
            case 65:
                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (cp2 == cp) break;
                    } else
                    {   break;
                    }
                } else
                    break;

                strncpy(InfoPtr->taxis_title,cp,MAX_TITLE_LEN-1);
                strcat(InfoPtr->taxis_title,"\0");
                break;
            case 66:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->taxis_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->taxis_max = atof(cp);
                break;
            case 67:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->taxis_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->taxis_min = atof(cp);
                break;
            case 68:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->taxis_tick_show = FALSE;  /* meters */
                else             InfoPtr->taxis_tick_show = TRUE;
                break;
            case 69:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->taxis_tick_start = INVALID_VALUE;
                    }
                } else
                    InfoPtr->taxis_tick_start = atof(cp);
                break;
            case 70:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->taxis_tick_int = INVALID_VALUE;
                    }
                } else
                    InfoPtr->taxis_tick_int = atof(cp);
                break;
            case 71:
                InfoPtr->taxis_tick_num = atoi(cp);
                break;
            case 72:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->taxis_tick_ano = FALSE;  /* meters */
                else             InfoPtr->taxis_tick_ano = TRUE;
                break;
            case 73:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->taxis_tick_skip = itemp;
                break;
            case 74:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->taxis_title_size = dtemp;
                break;
            case 75:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->taxis_ano_size = dtemp;
                break;
            case 76:
                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (cp2 == cp) break;
                    } else
                    {   break;
                    }
                } else
                    break;

                strncpy(InfoPtr->baxis_title,cp,MAX_TITLE_LEN-1);
                strcat(InfoPtr->baxis_title,"\0");
                break;
            case 77:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;              /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->baxis_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->baxis_max = atof(cp);
                break;
            case 78:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->baxis_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->baxis_min = atof(cp);
                break;
            case 79:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->baxis_tick_show = FALSE;  /* meters */
                else             InfoPtr->baxis_tick_show = TRUE;
                break;
            case 80:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->baxis_tick_start = INVALID_VALUE;
                    }
                } else
                    InfoPtr->baxis_tick_start = atof(cp);
                break;
            case 81:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->baxis_tick_int = INVALID_VALUE;
                    }
                } else
                    InfoPtr->baxis_tick_int = atof(cp);
                break;
            case 82:
                InfoPtr->baxis_tick_num = atoi(cp);
                break;
            case 83:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->baxis_tick_ano = FALSE;  /* meters */
                else             InfoPtr->baxis_tick_ano = TRUE;
                break;
            case 84:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->baxis_tick_skip = itemp;
                break;
            case 85:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->baxis_title_size = dtemp;
                break;
            case 86:
                dtemp = atof(cp);
                if (dtemp > 0.0) InfoPtr->baxis_ano_size = dtemp;
                break;
            case 87:
                itemp = atoi(cp);
                if (itemp >  6) itemp = 6;
                if (itemp < -6) itemp = -6;
                InfoPtr->laxis_ano_left = itemp;
                break;
            case 88:
                itemp = atoi(cp);
                if (itemp > 3) itemp = 3;
                InfoPtr->laxis_ano_right = itemp;
                break;
            case 89:
                itemp = atoi(cp);
                if (itemp >  6) itemp = 6;
                if (itemp < -6) itemp = -6;
                InfoPtr->raxis_ano_left = itemp;
                break;
            case 90:
                itemp = atoi(cp);
                if (itemp > 3) itemp = 3;
                InfoPtr->raxis_ano_right = itemp;
                break;
            case 91:
                itemp = atoi(cp);
                if (itemp >  6) itemp = 6;
                if (itemp < -6) itemp = -6;
                InfoPtr->taxis_ano_left = itemp;
                break;
            case 92:
                itemp = atoi(cp);
                if (itemp > 3) itemp = 3;
                InfoPtr->taxis_ano_right = itemp;
                break;
            case 93:
                itemp = atoi(cp);
                if (itemp >  6) itemp = 6;
                if (itemp < -6) itemp = -6;
                InfoPtr->baxis_ano_left = itemp;
                break;
            case 94:
                itemp = atoi(cp);
                if (itemp > 3) itemp = 3;
                InfoPtr->baxis_ano_right = itemp;
                break;
            case 95:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->first_trace_time = INVALID_VALUE;
                    }
                } else
                {   dtemp = atof(cp);
                    if (dtemp >= 0.0) InfoPtr->first_trace_time = dtemp;
                }
                break;
            case 96:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->last_trace_time = INVALID_VALUE;
                    }
                } else
                {   dtemp = atof(cp);
                    if (dtemp >= 0.0) InfoPtr->last_trace_time = dtemp;
                }
                break;
            case 97:
                ltemp = atol(cp);
                if (ltemp >= 0)  InfoPtr->first_trace = ltemp;
                break;
            case 98:
                ltemp = atol(cp);
                if (ltemp >= 0)  InfoPtr->last_trace = ltemp;
                break;
            case 99:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->lock_first_samp = FALSE;  /* meters */
                else             InfoPtr->lock_first_samp = TRUE;
                break;
            default:
                break;
        }   /* end of 2nd switch(found) block */
        switch (found)       /* cases depend on same order in GPR_DISP_CMD[] */
        {   case 100:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past first quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->hpgl_select_font,cp,strlen(InfoPtr->hpgl_select_font)-1);
                strlwr(InfoPtr->hpgl_select_font);
                break;
            case 101:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->first_samp_time = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->first_samp_time = atof(cp);
                }
                /*if (Debug) printf("GetCmdFileArgs: first_samp_time = %g\n",InfoPtr->first_samp_time); */
                break;
            case 102:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->last_samp_time = INVALID_VALUE;
                    }
                } else
                {   InfoPtr->last_samp_time = atof(cp);
                }
                break;
            case 103:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->background = FALSE;
                else             InfoPtr->background = TRUE;
                break;
            case 104:
                InfoPtr->show_markers = atoi(cp);
                break;
            case 105:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->portrait = FALSE;
                else             InfoPtr->portrait = TRUE;
                break;
            case 106:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->next_cmd_filename,cp,MAX_PATHLEN-1);
                strupr(InfoPtr->next_cmd_filename);
                break;
            case 107:
                if (InfoPtr->inst_pow == TRUE)
                {   InfoPtr->inst_amp = FALSE;
                    break;
                }
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->inst_amp = 0;
                else             InfoPtr->inst_amp = 1;
                break;
            case 108:
                itemp = atoi(cp);
                if (itemp >= -128 && itemp <=128)  InfoPtr->brightness = itemp;
                break;
            case 109:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->negative = FALSE;
                else             InfoPtr->negative = TRUE;
                break;
            case 110:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->laxis_tick_abs_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->laxis_tick_abs_min = atof(cp);
                break;
            case 111:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->laxis_tick_abs_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->laxis_tick_abs_max = atof(cp);
                break;
            case 112:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->raxis_tick_abs_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->raxis_tick_abs_min = atof(cp);
                break;
            case 113:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->raxis_tick_abs_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->raxis_tick_abs_max = atof(cp);
                break;
            case 114:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->taxis_tick_abs_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->taxis_tick_abs_min = atof(cp);
                break;
            case 115:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->taxis_tick_abs_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->taxis_tick_abs_max = atof(cp);
                break;
            case 116:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->baxis_tick_abs_min = INVALID_VALUE;
                    }
                } else
                    InfoPtr->baxis_tick_abs_min = atof(cp);
                break;
            case 117:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(cp,"INVALID_VALUE"))
                            InfoPtr->baxis_tick_abs_max = INVALID_VALUE;
                    }
                } else
                    InfoPtr->baxis_tick_abs_max = atof(cp);
                break;
            case 118:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                strncpy(InfoPtr->eps_select_font,cp,52-1);
                break;
            case 119:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->caption_font_size = itemp;
                break;
            case 120:
                InfoPtr->caption_left_edge = atof(cp);
                break;
            case 121:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->caption_long_edge = FALSE;
                else             InfoPtr->caption_long_edge = TRUE;
                break;
            case 122:
            {   char *cp_next;
                char *temp_caption = NULL;
                int num_chars = 0,i;

                /* check for empty string */
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp3 = cp2 + 1;             /* step past quote */
                    cp3 = strchr(cp3,'"');     /* look for second quote */
                    if (cp3 != NULL)
                    {   if (cp3 == cp2+1) break;
                    } else
                    {   break;
                    }
                } else
                    break;

                if (strlen(cp) == 0) break;
                cp_next = (char *)TrimStr(cp);
                if (strlen(cp_next) == 0) break;

                /* allocate temporary storage for captions, 2000 characters */
                temp_caption = (char *)malloc(MAX_CAPTION_LEN);
                if (temp_caption == NULL)
                {   InfoPtr->caption = NULL;
                    if (Debug) puts("\nmalloc failed for temp_caption");
                    break;              /* break out of case */
                }
                while (1) /* loop till we read in a command line */
                {   cp2 = strchr(cp_next,'"'); /* look for first quote */
                    if (cp2 != NULL)
                    {   cp_next = cp2 + 1;     /* step past quote */
                        cp2 = strchr(cp_next,'"');  /* look for second quote */
                        if (cp2 != NULL)
                        {   *cp2 = '\n';       /* change quote to \n */
                            *(cp2 + 1) = 0;    /* truncate string */
                            /* work through string */
                            while (*cp_next && num_chars < MAX_CAPTION_LEN)
                            {   temp_caption[num_chars] = *cp_next;
                                cp_next++; num_chars++;
                            }
                        }
                    }
                    /* get another line from the file */
                    if (fgets(str,MAX_CMDLINELEN,infile) == NULL)
                    {   break;      /* break out of while (1) */
                    } else
                    {   /* see if we have pulled in a command line */
                        if ( (cp2 = strchr(str,'=')) != NULL )
                        {   if ( (strchr(str,'"') == NULL) ||
                                 (cp2 < strchr(str,'"')) )
                            {   have_string = TRUE; /* may have a command line */
                                break;              /* break out of while (1) */
                            } else
                            {   have_string = FALSE; /* may have a caption line */
                                cp_next = (char *)TrimStr(str);
                            }
                        } else
                        {   have_string = FALSE;    /* may have a caption line */
                            cp_next = (char *)TrimStr(str);
                        }
                    }
                }
                /* allocate storage and copy string */
                if (num_chars > 0)
                {   InfoPtr->caption = (char *)malloc((size_t)(num_chars+1));
                    if (InfoPtr->caption == NULL)
                    {   free(temp_caption);
                        if (Debug) puts("\nmalloc failed for InfoPtr->captions");
                        break;              /* break out of case */
                    }
                    for (i=0; i<num_chars; i++)
                        InfoPtr->caption[i] = temp_caption[i];
                    InfoPtr->caption[num_chars] = 0;
                    free(temp_caption);
                }

                break;
            }
            case 123:
                InfoPtr->caption_bottom_edge = atof(cp);
                break;
            case 124:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->pagenum_font_size = itemp;
                break;
            case 125:
                InfoPtr->pagenum_left_edge = atof(cp);
                break;
            case 126:
                InfoPtr->pagenum_bottom_edge = atof(cp);
                break;
            case 127:
            {   int num_chars = 0;

                if (strlen(cp) == 0) break;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 == NULL) break;
                cp = cp2 + 1;                 /* step past quote */
                cp2 = strchr(cp,'"');         /* look for second quote */
                if (cp2 == NULL) break;
                *cp2 = 0;                     /* truncate at second quote */
                num_chars = cp2 - cp;         /* get number of characters */

                /* allocate storage and copy string */
                if (num_chars > 0)
                {   if (num_chars > MAX_PAGENUM_LEN)
                        num_chars = MAX_PAGENUM_LEN;
                    InfoPtr->pagenum = (char *)malloc((size_t)(num_chars+1));
                    if (InfoPtr->pagenum == NULL)
                    {   if (Debug) puts("\nmalloc failed for InfoPtr->pagenum");
                        break;              /* break out of case */
                    }
                    InfoPtr->pagenum[0] = 0;
                    strncpy(InfoPtr->pagenum,cp,MAX_PAGENUM_LEN);
                    strcat(InfoPtr->pagenum,"\0");
                }
                break;
            }
            case 128:
                dtemp = atof(cp);
                if (dtemp >= 0.0 && dtemp <= 127.5)
                    InfoPtr->local_offset = dtemp;
                break;
            case 129:
                InfoPtr->local_change = atof(cp);
                break;
            case 130:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->display_all = FALSE;
                else             InfoPtr->display_all = TRUE;
                break;
            case 131:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->display_none = FALSE;
                else             InfoPtr->display_none = TRUE;
                break;
            case 132:  /* not used in the command file */
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->eps_copies = itemp;
                break;
            case 133:
                InfoPtr->laxis_title_offset = atof(cp);
                break;
            case 134:
                InfoPtr->laxis_tick_mid_skip = atoi(cp);
                break;
            case 135:
                InfoPtr->raxis_title_offset = atof(cp);
                break;
            case 136:
                InfoPtr->raxis_tick_mid_skip = atoi(cp);
                break;
            case 137:
                InfoPtr->taxis_title_offset = atof(cp);
                break;
            case 138:
                InfoPtr->taxis_tick_mid_skip = atoi(cp);
                break;
            case 139:
                InfoPtr->baxis_title_offset = atof(cp);
                break;
            case 140:
                InfoPtr->baxis_tick_mid_skip = atoi(cp);
                break;
            case 141:
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->square = FALSE;
                else             InfoPtr->square = TRUE;
                break;
            case 142:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->file_header_bytes = itemp;
                if (Debug) printf("file_header_bytes = %d\n",InfoPtr->file_header_bytes);
                break;
            case 143:
                itemp = atoi(cp);
                if (itemp > 0)  InfoPtr->trace_header_bytes = itemp;
                if (Debug) printf("trace_header_bytes = %d\n",InfoPtr->trace_header_bytes);
                break;
            case 144:      /* other_format */
                itemp = 0;
                if (Debug) printf("input format: cp = %s\n",cp);
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
                if (itemp != 0)  InfoPtr->input_format = USER_DEF;
                if (Debug) printf("input_format = %d\n",InfoPtr->input_format);
                break;
            case 145:
                itemp = atoi(cp);
                if (Debug) printf("itemp (input_datatype) = %d\n",itemp);
                switch (itemp)
                {   case ULONG24: case USHORT12: case ULONG:  case USHORT:
                    case UCHAR:   case CHAR:     case SHORT:  case LONG:
                    case FLOAT:   case DOUBLE:
                        InfoPtr->input_datatype = itemp;
                        break;
                    default:
                        InfoPtr->input_datatype = 0;
                        break;
                }
                if (Debug) printf("input_datatype = %d\n",InfoPtr->input_datatype);
                break;
            case 146:    /* samples_per_trace */
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->total_samps = itemp;
                if (Debug) printf("total_samps = %d\n",InfoPtr->total_samps);
                break;
            case 147:    /* total_time */
                dtemp = atof(cp);
                if (dtemp > 0.0)  InfoPtr->total_time = dtemp;
                if (Debug) printf("total_time = %g\n",InfoPtr->total_time);
                break;
            case 148:    /* row_by_row */
                itemp = 0;
                if (Debug) printf("input format: cp = %s\n",cp);
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
                if (itemp != 0)  InfoPtr->if_image = TRUE;
                if (Debug) printf("if_image = %d\n",InfoPtr->if_image);
                break;
            case 149:
                InfoPtr->min_data_val = atof(cp);
                if (Debug) printf("min_data_val = %g\n",InfoPtr->min_data_val);
                break;
            case 150:
                itemp = atoi(cp);
                if (itemp >= 0) InfoPtr->skip_traces = itemp;
                if (Debug) printf("skip_traces = %d\n",InfoPtr->skip_traces);
                break;
            case 151:
                itemp = atoi(cp);
                if (itemp >= 1) InfoPtr->show_marker_width = itemp;
                if (Debug) printf("show_marker_width = %d\n",InfoPtr->show_marker_width);
                break;
            case 152:
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
                if (itemp != 0)  InfoPtr->display_wiggle = TRUE;
                if (Debug) printf("display_wiggle = %d\n",InfoPtr->display_wiggle);
                break;
            case 153:
                itemp = atoi(cp);
                if (itemp >= 0)  InfoPtr->num_wiggles = itemp;
                if (Debug) printf("num_wiggles = %d\n",InfoPtr->num_wiggles);
                break;
            case 154:
            {   char *cp_next = (char *)TrimStr(cp);
                int j,wiggle_found = 0;

                /* number of wiggles must be specified first */
                if (InfoPtr->num_wiggles <= 0) break;

                /* allocate storage for traces */
                InfoPtr->wiggle_traceval = (double **)malloc(InfoPtr->num_wiggles * sizeof(double*));
                if (InfoPtr->wiggle_traceval == NULL)
                {   InfoPtr->num_wiggles = 0;
                    if (Debug) puts("\nmalloc failed for wiggle_traceval");
                    break;  /* break out of case */
                } else
                {   for (i=0; i<InfoPtr->num_wiggles; i++)
                    {   InfoPtr->wiggle_traceval[i] = (double *)malloc(2 * sizeof(double));
                        if (InfoPtr->wiggle_traceval[i] == NULL)
                        {   for (j=0; j<i; j++)
                                free(InfoPtr->wiggle_traceval[j]);
                            free(InfoPtr->wiggle_traceval);
                            InfoPtr->wiggle_traceval = NULL;
                            break;  /* out of for loop */
                        }
                    }
                    if (InfoPtr->wiggle_traceval == NULL)
                    {   InfoPtr->num_wiggles = 0;
                        if (Debug) puts("\nmalloc failed for wiggle_traceval[]");
                        break;  /* break out of case */
                    }
                }
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (wiggle_found < InfoPtr->num_wiggles)
                        {   InfoPtr->wiggle_traceval[wiggle_found][0] = atof(cp_next);
                            wiggle_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (wiggle_found < InfoPtr->num_wiggles)
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
                if (wiggle_found < InfoPtr->num_wiggles)
                {   if (Debug) puts("\nwiggle_found < num_wiggles");
                    for (i=0; i<InfoPtr->num_wiggles; i++)
                        free(InfoPtr->wiggle_traceval[i]);
                    free(InfoPtr->wiggle_traceval);
                    InfoPtr->wiggle_traceval = NULL;
                    InfoPtr->num_wiggles = 0;
                }
                break;
            }
            case 155:
                InfoPtr->wiggle_color = atoi(cp);
                if (InfoPtr->wiggle_color <   0)  InfoPtr->wiggle_color =  -1;
                if (InfoPtr->wiggle_color > 255)  InfoPtr->wiggle_color =  -1;
                if (Debug) printf("wiggle_color = %d\n",InfoPtr->wiggle_color);
                break;
            case 156:
                InfoPtr->wiggle_fill = atoi(cp);
                if (Debug) printf("wiggle_fill = %d\n",InfoPtr->wiggle_fill);
                break;
            case 157:
                InfoPtr->wiggle_width = atof(cp);
                if (InfoPtr->wiggle_width < 0.001)  InfoPtr->wiggle_width = 0.001;
                if (InfoPtr->wiggle_width > 100.0)  InfoPtr->wiggle_width = 100.0;
                if (Debug) printf("wiggle_width = %g\n",InfoPtr->wiggle_width);
                break;
            case 158:
                InfoPtr->wiggle_clip = atof(cp);
                if (InfoPtr->wiggle_clip < 0.001)  InfoPtr->wiggle_clip = 0.001;
                if (InfoPtr->wiggle_clip > 100.0)  InfoPtr->wiggle_clip = 100.0;
                if (Debug) printf("wiggle_clip = %g\n",InfoPtr->wiggle_clip);
                break;
            case 159:
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
                if (itemp != 0)  InfoPtr->change_gain = TRUE;
                if (Debug) printf("change_gain = %d\n",InfoPtr->change_gain);
                break;
            case 160:
                itemp = atoi(cp);
                if (itemp >= 2 || itemp ==0)  InfoPtr->num_gain_off = itemp;
                if (Debug) printf("num_gain_off = %d\n",InfoPtr->num_gain_off);
                break;
            case 161:
            {   char *cp_next = (char *)TrimStr(cp);
                int inum,off_found = 0;

                /* number of points must be specified first */
                if (InfoPtr->num_gain_off <= 0) break;

                /* allocate storage for points */
                InfoPtr->gain_off = (double *)malloc(InfoPtr->num_gain_off * sizeof(double));
                if (InfoPtr->gain_off == NULL)
                {   InfoPtr->num_gain_off = 0;
                    if (Debug) puts("\nmalloc failed for gain_off");
                    break;  /* break out of case */
                }
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (off_found < InfoPtr->num_gain_off)
                        {   InfoPtr->gain_off[off_found] = atof(cp_next);
                            off_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (off_found < InfoPtr->num_gain_off)
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
                if (off_found < InfoPtr->num_gain_off)
                {   if (Debug) puts("\noff_found < num_gain_off");
                    free(InfoPtr->gain_off);
                    InfoPtr->gain_off = NULL;
                    InfoPtr->num_gain_off = 0;
                } else
                {   if (Debug)
                    {   printf("gain_off[] =");
                        for (inum=0; inum<InfoPtr->num_gain_off; inum++)
                        {   printf(" %g");
                        }
                        puts("");
                    }
                }
                break;
            }
            case 162:
                itemp = atoi(cp);
                if (itemp >= 2 || itemp ==0)  InfoPtr->num_gain_on = itemp;
                if (Debug) printf("num_gain_on = %d\n",InfoPtr->num_gain_on);
                break;
            case 163:
            {   char *cp_next = (char *)TrimStr(cp);
                int inum,on_found = 0;

                /* number of points must be specified first */
                if (InfoPtr->num_gain_on <= 0) break;

                /* allocate storage for points */
                InfoPtr->gain_on = (double *)malloc(InfoPtr->num_gain_on * sizeof(double));
                if (InfoPtr->gain_on == NULL)
                {   InfoPtr->num_gain_on = 0;
                    if (Debug) puts("\nmalloc failed for gain_on");
                    break;  /* break out of case */
                }
                while (1)
                {   while (*cp_next)    /* work through string */
                    {   TrimStr(cp_next);
                        if (on_found < InfoPtr->num_gain_on)
                        {   InfoPtr->gain_on[on_found] = atof(cp_next);
                            on_found++;
                            cp_next = strchr(cp_next,' '); /* look for seperator */
                            if (cp_next == NULL)
                                break;  /* break out of while (*cp_next) */
                        } else
                            break;      /* break out of while (*cp_next) */
                    }

                    if (on_found < InfoPtr->num_gain_on)
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
                if (on_found < InfoPtr->num_gain_on)
                {   if (Debug) puts("\non_found < num_gain_on");
                    free(InfoPtr->gain_on);
                    InfoPtr->gain_on = NULL;
                    InfoPtr->num_gain_on = 0;
                } else
                {   if (Debug)
                    {   printf("gain_on[] =");
                        for (inum=0; inum<InfoPtr->num_gain_on; i++)
                        {   printf(" %g");
                        }
                        puts("");
                    }
                }
                break;
            }
            case 164:
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;                 /* step past quote */
                    if (Debug) printf("new cp = %s\n",cp);
                    cp2 = strchr(cp,'"');         /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;                     /* truncate at second quote */
                        strncpy(InfoPtr->pcx_outfilename,cp,MAX_PATHLEN-1);
                        strupr(InfoPtr->pcx_outfilename);
                        if (InfoPtr->pcx_outfilename[0]) InfoPtr->save_pcx = TRUE;
                    }
                }
                if (Debug) printf("pcx_outfilename = %s  save_pcx = %d\n",
                                InfoPtr->pcx_outfilename,InfoPtr->save_pcx);
                break;
            case 165:
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
                if (itemp != 0)  InfoPtr->add_hlines = TRUE;
                if (Debug) printf("add_hlines = %d\n",InfoPtr->add_hlines);
                break;
            case 166:
                InfoPtr->hline_start = atof(cp);
                if (Debug) printf("hline_start = %g\n",InfoPtr->hline_start);
                break;
            case 167:
                InfoPtr->hline_int = atof(cp);
                if (Debug) printf("hline_int = %g\n",InfoPtr->hline_int);
                break;
            case 168:
                InfoPtr->hline_stop = atof(cp);
                if (Debug) printf("hline_stop = %g\n",InfoPtr->hline_stop);
                break;
            case 169:
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
                if (itemp != 0)  InfoPtr->add_vlines = TRUE;
                if (Debug) printf("add_vlines = %d\n",InfoPtr->add_vlines);
                break;
            case 170:
                InfoPtr->vline_start = atof(cp);
                if (Debug) printf("vline_start = %g\n",InfoPtr->vline_start);
                break;
            case 171:
                InfoPtr->vline_int = atof(cp);
                if (Debug) printf("vline_int = %g\n",InfoPtr->vline_int);
                break;
            case 172:
                InfoPtr->vline_stop = atof(cp);
                if (Debug) printf("vline_stop = %g\n",InfoPtr->vline_stop);
                break;
            case 173:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <8)  InfoPtr->hline_style = itemp;
                if (Debug) printf("hline_style = %d\n",InfoPtr->hline_style);
                break;
            case 174:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <8)  InfoPtr->vline_style = itemp;
                if (Debug) printf("vline_style = %d\n",InfoPtr->vline_style);
                break;
            case 175:
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
                if (itemp != 0)  InfoPtr->use_designjet750 = TRUE;
                if (Debug) printf("use_designjet750 = %d\n",InfoPtr->use_designjet750);
                break;
            case 176:
                InfoPtr->hp_x = atof(cp);
                if (Debug) printf("hp_x = %g\n",InfoPtr->hp_x);
                break;
            case 177:
                InfoPtr->hp_y = atof(cp);
                if (Debug) printf("hp_y = %g\n",InfoPtr->hp_y);
                break;
            case 178:
                InfoPtr->hp_xoff = atof(cp);
                if (Debug) printf("hp_xoff = %g\n",InfoPtr->hp_xoff);
                break;
            case 179:
                InfoPtr->hp_yoff = atof(cp);
                if (Debug) printf("hp_yoff = %g\n",InfoPtr->hp_yoff);
                break;
            case 180:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->envelope = 0;
                else             InfoPtr->envelope = 1;
                break;
            case 181:
                if (InfoPtr->inst_amp == TRUE)
                {   InfoPtr->inst_pow = FALSE;
                    break;
                }
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
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->inst_pow = 0;
                else             InfoPtr->inst_pow = 1;
                break;
            case 182:
                itemp = 0;
                cp2 = strchr(cp,'"');         /* look for first quote */
                if (cp2 != NULL)
                {   cp = cp2 + 1;             /* step past quote */
                    cp2 = strchr(cp,'"');     /* look for second quote */
                    if (cp2 != NULL)
                    {   *cp2 = 0;             /* truncate at second quote */
                        if (strstr(strupr(cp),"TRUE"))       itemp = 1;
                        else if (strstr(strupr(cp),"FALSE")) itemp = 0;
                    }
                } else
                    itemp = atoi(cp);
                if (itemp == 0)  InfoPtr->plot_hyperbola = 0;
                else             InfoPtr->plot_hyperbola = 1;
                if (Debug) printf("plot_hyperbola = %d\n",InfoPtr->plot_hyperbola);
                break;
            case 183:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->ant_sep = dtemp;
                if (Debug) printf("ant_sep = %g\n",InfoPtr->ant_sep);
                break;
            case 184:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->obj_depth = dtemp;
                if (Debug) printf("obj_depth = %g\n",InfoPtr->obj_depth);
                break;
            case 185:
                dtemp = atof(cp);
                if (dtemp >= 0.0) InfoPtr->obj_radius = dtemp;
                if (Debug) printf("obj_radius = %g\n",InfoPtr->obj_radius);
                break;
            case 186:
                InfoPtr->obj_loc = atof(cp);
                if (Debug) printf("obj_loc = %g\n",InfoPtr->obj_loc);
                break;
            case 187:
                dtemp = atof(cp);
                if (dtemp >= 0.01 && dtemp <= 0.30) InfoPtr->hyp_vel = dtemp;
                if (Debug) printf("hyp_vel = %g\n",InfoPtr->hyp_vel);
                break;
            case 188:
                itemp = atoi(cp);
                if (itemp >= 0 && itemp <= 255) InfoPtr->hyp_color = itemp;
                if (Debug) printf("hyp_color = %d\n",InfoPtr->hyp_color);
                break;
            default:
                break;
        }   /* end of 3rd switch(found) block */
    }   /* end of while(1) block */
/* 11, 12, 13, 132 not used */
    fclose(infile);
    if (Debug)  Batch = FALSE;
    if (!dat_in_found) return 3;

    return 0;
}
/**************************** DisplayParameters() ***************************/
/* Display parameters to CRT.
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, "jl_defs.h", "gpr_disp.h".
 * Calls: strupr, printf, puts, getch, kbhit, _ABS, strchr, sprintf.
 * Returns: void
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: June 24, 1996; May 14, 1997; Aug 11, 1997; July 16, 9999;
 *       August 23, 1999; September 3, 1999; August 28, 2000; November 6, 2001;
 */
void DisplayParameters (struct DispParamInfoStruct *InfoPtr)
{
    int i,lines,key;
    char str[10];
    char *coord_modes[] =
    {   "invalid mode",
        "raw traces (trace number vs. sample time in ns)",
        "geometrically corrected (distance in meters vs. depth in meters)",
        "stationary antenna (trace time in seconds vs. sample time in ns)",
        "horiz. rubbersheeting, no vert. correction (distance vs. sample time in ns)",
        "really raw traces (trace number vs. sample number)",
    } ;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    char *month[] = { "","January","February","March","April","May","June",
                      "July","August","September","October",
                      "November","December" } ;
    extern int Debug,Batch,ANSI_THERE,save_eps,save_pcx;

    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);

    if (ANSI_THERE) printf("%c%c31;40m",0x1B,0x5B); /* red on black */
    printf("\nGPR_DISP version %s (run time: %02d:%02d:%02d on %s %d, %d)\n",
                        GPR_DISP_VER,dostime.hour,dostime.minute,dostime.second,
                        month[dosdate.month],dosdate.day,dosdate.year);
    printf("         Computer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
    if (ANSI_THERE) printf("%c%c37;40m",0x1B,0x5B); /* white on black */
    lines = 3;
    printf("cmd_filename          = %s\n",strupr(InfoPtr->cmd_filename));
    lines++;
    if (InfoPtr->next_cmd_filename[0])
    {   printf("next_cmd_filename     = %s\n",strupr(InfoPtr->next_cmd_filename));
        lines++;
    }
    if (InfoPtr->override_cmd_filename[0])
    {   printf("override_cmd_filename = %s\n",strupr(InfoPtr->override_cmd_filename));
        lines++;
    }
    printf("dat_infilename        = %s\n",strupr(InfoPtr->dat_infilename));
    lines++;
    if (InfoPtr->input_format == DT1)
    {   printf("inf_infilename        = %s\n",strupr(InfoPtr->inf_infilename));
        lines++;
    }
    if (InfoPtr->mrk_infilename[0])
    {   printf("mrk_infilename        = %s\n",strupr(InfoPtr->mrk_infilename));
        lines++;
    }
    if (InfoPtr->xyz_infilename[0])
    {   printf("xyz_infilename        = %s\n",strupr(InfoPtr->xyz_infilename));
        lines++;
    }
    if (InfoPtr->lbl_infilename[0])
    {   printf("lbl_infilename        = %s\n",strupr(InfoPtr->lbl_infilename));
        lines++;
    }
    if (save_pcx)
    {   printf("pcx_outfilename       = %s    save_pcx = %s\n",
        strupr(InfoPtr->pcx_outfilename),save_pcx ? "TRUE" : "FALSE");
        lines++;
    }
    if (save_eps)
    {   /* printf("eps_outfilename       = %s    save_eps = %s   eps_copies = %d\n",
            strupr(InfoPtr->eps_outfilename),save_eps ? "TRUE" : "FALSE",
            InfoPtr->eps_copies); */
        printf("eps_outfilename       = %s    save_eps = %s\n",
            strupr(InfoPtr->eps_outfilename),save_eps ? "TRUE" : "FALSE");
        lines++;
        if (InfoPtr->use_designjet750)
        {   printf("    HP DesignJet 750 output:x = %g  y = %g  xoff = %g  yoff = %g\n",
                InfoPtr->hp_x,InfoPtr->hp_y,InfoPtr->hp_xoff,InfoPtr->hp_yoff);
            lines++;
        }
    }
    switch (InfoPtr->input_format)
    {   case DZT:   puts("data storage format is GSSI SIR-10 or SIR-2");  break;
        case DT1:   puts("data storage format is S&S PulseEKKO"); break;
        case SGY:   puts("data storage format is SEG-Y");         break;
        /* case RD3:   puts("data storage format is Mala RAMAC");         break; */
        case MOD_DZT:   puts("data storage format is modified GSSI DZT");  break;
        case USER_DEF:
            puts("data storage format is USER DEFINED");
            printf("file_header_bytes = %d trace_header_bytes = %d row_by_row = %s\n",
                    InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes,
                    InfoPtr->if_image ? "TRUE" : "FALSE");
            lines++;
            break;
        default:  puts("unrecognized data storage format");    break;
    }
    lines++;
    switch (InfoPtr->input_datatype)
    {   case  UCHAR: puts("data element type is 8-bit unsigned integers");  break;
        case   CHAR: puts("data element type is 8-bit signed integers");    break;
        case USHORT: puts("data element type is 16-bit unsigned integers"); break;
        case  SHORT: puts("data element type is 16-bit signed integers");   break;
        case  ULONG:
            puts("data element type is 32-bit unsigned integers");
            if (InfoPtr->max_data_val > InfoPtr->min_data_val)
            {    printf("        scaling range is from %g to %g\n",
                    InfoPtr->min_data_val,InfoPtr->max_data_val);
            }
            break;
        case LONG: puts("data element type is 32-bit signed integers");   break;
        case USHORT12: puts("data element type is 12-bit unsigned integers"); break;
        case ULONG24: puts("data element type is 24-bit unsigned integers"); break;
        case FLOAT:
            puts("data element type is 32-bit floating point");
            printf("        scaling range is from %g to %g\n",
                   InfoPtr->min_data_val,InfoPtr->max_data_val);
            lines++;
            break;
        case DOUBLE:
            puts("data element type is 64-bit floating point");
            printf("        scaling range is from %g to %g\n",
                   InfoPtr->min_data_val,InfoPtr->max_data_val);
            lines++;
            break;
        default: puts("unknown data element type"); break;
    }
    lines++;
    printf("file_header_bytes = %d    trace_header_bytes = %d\n",
        InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes);
    lines++;
    printf("traces/channel = %ld  samples/trace = %ld  time/trace = %g (ns)\n",
        InfoPtr->total_traces,InfoPtr->total_samps,InfoPtr->total_time);
    printf("channel     = %d (1 is first channel)  ant_freq   = %d MHz\n",
        InfoPtr->channel + 1,InfoPtr->ant_freq);
    lines++;
    printf("first_samp = %ld  last_samp = %ld  lock_first_samp = %s\n",
        InfoPtr->first_samp,InfoPtr->last_samp,
        InfoPtr->lock_first_samp ? "TRUE" : "FALSE");
    lines++;
    printf("first_trace = %ld  last_trace = %ld  skip_traces = %d\n",
        InfoPtr->first_trace,InfoPtr->last_trace,InfoPtr->skip_traces);
    lines += 2;
    if (InfoPtr->coord_mode == TIME_TIME) /* 3 */
        printf("first_trace_time  = %g   last_trace_time  = %g  (seconds)\n",
            InfoPtr->first_trace_time,InfoPtr->last_trace_time);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->coord_mode == TRACE_TIME ||
        InfoPtr->coord_mode == TIME_TIME ||
        InfoPtr->coord_mode == DISTANCE_TIME)  /* 1, 3, or 4 */
        printf("first_samp_time = %g  last_samp_time = %g (ns)\n",
            InfoPtr->first_samp_time,InfoPtr->last_samp_time);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("trace_per_sec = %g    ns_per_samp  = %g\n",
        InfoPtr->trace_per_sec,InfoPtr->ns_per_samp);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->num_bad > 0)
    {   printf("num_bad = %d\nbad_traces[] =",InfoPtr->num_bad);
        for (i=0; i<InfoPtr->num_bad; i++)
            printf(" %ld",InfoPtr->bad_traces[i]);
        puts("");
        lines++;
    }
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("coord_mode = %s\n",coord_modes[InfoPtr->coord_mode]);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->coord_mode == DISTANCE_DISTANCE ||
        InfoPtr->coord_mode == DISTANCE_TIME) /* 2 or 4 */
    {   printf("horiz_mode = %s  horiz_start = %g  horiz_stop = %g\n",
           InfoPtr->horiz_mode == 1 ? "X-coordinates" :
           (InfoPtr->horiz_mode == 2 ? "Y-coordinates" : "Traverse distance"),
           InfoPtr->horiz_start,InfoPtr->horiz_stop);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->plot_hyperbola)
    {   printf("Velocity hyperbola:\n");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("\tant_sep=%g m   hyp_vel=%g m/ns   hyp_color=%d\n",
                InfoPtr->ant_sep,InfoPtr->hyp_vel,InfoPtr->hyp_color);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("\tobj_depth = %g m   obj_radius = %g m   obj_loc = %g m\n",
                InfoPtr->obj_depth,InfoPtr->obj_radius,InfoPtr->obj_loc);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }

    printf("\nimage: num_rows = %ld  num_cols = %ld\n",
        InfoPtr->num_rows,InfoPtr->num_cols);
    lines += 2;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("VIEWPORT: VX1 = %g  VX2 = %g  VY1 = %g  VY2 = %g\n",
        InfoPtr->VX1,InfoPtr->VX2,InfoPtr->VY1,InfoPtr->VY2);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("WINDOW:  left = %g  right = %g  bottom = %g  top = %g\n",
        InfoPtr->left,InfoPtr->right,InfoPtr->bottom,InfoPtr->top);
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("video_mode = %d  vert_exag = %g  stretch = %d  background = %s\n",
        InfoPtr->video_mode,InfoPtr->vert_exag,InfoPtr->stretch,
        InfoPtr->background ? "TRUE" : "FALSE");
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    printf("inst_amp = %s  inst_pow = %s  envelope = %s\n",
        InfoPtr->inst_amp ? "TRUE" : "FALSE",InfoPtr->inst_pow ? "TRUE" : "FALSE",
        InfoPtr->envelope ? "TRUE" : "FALSE");
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (InfoPtr->change_gain)
    {   printf("change_gain = %s\n",InfoPtr->change_gain ? "TRUE" : "FALSE");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("    num_gain_off = %d [",InfoPtr->num_gain_off);
        if (InfoPtr->num_gain_off > 0)
        {   for (i=0; i<InfoPtr->num_gain_off; i++)
                printf(" %g",InfoPtr->gain_off[i]);
            printf("] db");
        }
        puts("");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("    num_gain_on = %d [",InfoPtr->num_gain_on);
        if (InfoPtr->num_gain_on > 0)
        {   for (i=0; i<InfoPtr->num_gain_on; i++)
                printf(" %g",InfoPtr->gain_on[i]);
            printf("] db");
        }
        puts("");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    if (InfoPtr->display_wiggle)
    {   printf("GPR data will be displayed as wiggle traces.\n");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("wiggle_color=%d wiggle_fill=%d wiggle_width=%g wiggle_clip=%g\n",
                InfoPtr->wiggle_color,InfoPtr->wiggle_fill,InfoPtr->wiggle_width,
                InfoPtr->wiggle_clip);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    } else
    {   printf("GPR data will be displayed as a gray-scale image.\n");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        if (InfoPtr->range) printf("range = %d\n",InfoPtr->range);
        else  printf("EPC: threshold = %d  contrast = %d  sign = %d   gain = %g\n",
            InfoPtr->epc_threshold,InfoPtr->epc_contrast,InfoPtr->epc_sign,
            InfoPtr->epc_gain);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("abs_val=%s  square=%s\n",
            InfoPtr->abs_val ? "TRUE" : "FALSE",InfoPtr->square ? "TRUE" : "FALSE");
        printf("brightness=%d  negative=%s  local_offset=%g  local_change=%g\n",
            InfoPtr->brightness,InfoPtr->negative ? "TRUE" : "FALSE",
            InfoPtr->local_offset,InfoPtr->local_change);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        if (InfoPtr->num_wiggles > 0)
        {   printf("num_wiggles = %d\nwiggle_traceval[] =",InfoPtr->num_wiggles);
            for (i=0; i<InfoPtr->num_wiggles; i++)
                printf(" %g",InfoPtr->wiggle_traceval[i][0]);
            puts("");
            lines++;
            if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
            printf("wiggle_color=%d wiggle_fill=%d wiggle_width=%g wiggle_clip=%g\n",
                    InfoPtr->wiggle_color,InfoPtr->wiggle_fill,
                    InfoPtr->wiggle_width,InfoPtr->wiggle_clip);
            lines++;
            if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
        }
    }
    if (InfoPtr->show_markers)
        printf("show_markers = %d (samples long)  %d pixels wide\n",
            _ABS(InfoPtr->show_markers),InfoPtr->show_marker_width);
    else
        puts("tick marks not displayed");
    lines++;
    if (!Batch && lines >= 23)
    {   printf("Press a key to continue ...");
        getch();
        printf("\r                                  \r");
        lines = 0;
    }
    if (save_eps)
    {   printf("output presentation is in %s mode;  use_11x17 = %s\n",
            InfoPtr->portrait ? "portrait" : "landscape",
            InfoPtr->use_11x17 ? "TRUE" : "FALSE");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        if (InfoPtr->caption != NULL)
        {   char *cp;

            printf("EPS: font=%s size=%d %g\" from left %g\" from bottom; long_edge=%s\n",
                     InfoPtr->eps_select_font,InfoPtr->caption_font_size,
                     InfoPtr->caption_left_edge,InfoPtr->caption_bottom_edge,
                     InfoPtr->caption_long_edge ? "TRUE" : "FALSE");
            lines++;
            if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
            printf("Caption = \n%s",InfoPtr->caption);
            cp = InfoPtr->caption;
            while((cp = strchr(cp,'\n')) != NULL)
            {   lines++;
                cp++;
            }
            if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                   lines = 0;
            }
        }
        if (InfoPtr->pagenum != NULL)
        {   printf("EPS pagenum: size=%d %g\" from left %g\" from bottom\n",
                     InfoPtr->pagenum_font_size,
                     InfoPtr->pagenum_left_edge,InfoPtr->pagenum_bottom_edge);
            lines++;
            if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                lines = 0;
            }
            printf("Pagenum = \n%s\n",InfoPtr->pagenum);
            lines++;
            if (!Batch && lines >= 23)
            {   printf("Press a key to continue ...");
                getch();
                printf("\r                                  \r");
                   lines = 0;
            }
        }
    }
    if (InfoPtr->num_ticks > 0)
    {   printf("num_ticks = %ld\n",InfoPtr->num_ticks);
        if (InfoPtr->tick_tracenum)
        {   printf("tick_tracenum[] =");
            for (i=0; i<InfoPtr->num_ticks; i++)
            {   printf(" %ld",InfoPtr->tick_tracenum[i]);
                lines++;
                if (!Batch && lines >= 23)
                {   printf("Press a key to continue ...");
                    /* while (kbhit()) getch(); */
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
            }
            puts("");
        }
        if (InfoPtr->tick_xyz)
        {   printf("\ntick_xyz[] =\n");
            for (i=0; i<InfoPtr->num_ticks; i++)
            {   printf("%g  %g  %g\n",InfoPtr->tick_xyz[i][0],
                        InfoPtr->tick_xyz[i][1],InfoPtr->tick_xyz[i][2]);
                lines++;
                if (!Batch && lines >= 23)
                {   printf("Press a key to continue ...");
                    /* while (kbhit()) getch(); */
                    getch();
                    printf("\r                                  \r");
                    lines = 0;
                }
            }
            puts("");
        }
    }
    if (InfoPtr->num_layers > 0)
    {   char *mode_descr[] =
        {   "Invalid mode",
            "\"constant depth below surface\"",
            "\"constant elevation\"",
            "\"infinite depth\"",
        } ;

        printf("num_layers = %d\n",InfoPtr->num_layers);
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("layer_rdp[] =");
        for (i=0; i<InfoPtr->num_layers; i++)
        {   printf(" %g",InfoPtr->layer_rdp[i]);
        }
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("\nlayer_mode[] =");
        for (i=0; i<InfoPtr->num_layers; i++)
        {   printf(" %s",mode_descr[InfoPtr->layer_mode[i]]);
        }
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
        printf("\nlayer_val[] =");
        for (i=0; i<InfoPtr->num_layers; i++)
        {   if (InfoPtr->layer_mode[i] == 3)
                printf(" -infinity");
            else
                printf(" %g",InfoPtr->layer_val[i]);
        }
        puts("");
        lines++;
        if (!Batch && lines >= 23)
        {   printf("Press a key to continue ...");
            getch();
            printf("\r                                  \r");
            lines = 0;
        }
    }
    key = !ENTER;
    if (!Batch)
    {   printf("Press <Enter> to see axes/overlay line info or any other key to continue ...");
        /* while (kbhit()) getch(); */
        key = getch();
        printf("\r                                                                         \r");
    }
    if (key == ENTER)
    {   printf("\n Display title = %s\n",InfoPtr->title);
        if (InfoPtr->add_hlines)
            printf("Horiz. overlay lines: start=%g interval=%g stop=%g style=%d\n",
                 InfoPtr->hline_start,InfoPtr->hline_int,InfoPtr->hline_stop,
                 InfoPtr->hline_style);
        else
            puts("No horizontal overlay lines.");
        if (InfoPtr->add_vlines)
            printf("Vert. overlay lines: start=%g interval=%g stop=%g style=%d\n",
                 InfoPtr->vline_start,InfoPtr->vline_int,InfoPtr->vline_stop,
                 InfoPtr->vline_style);
        else
            puts("No vertical overlay lines.");
        printf("Left axis title: %s\n",InfoPtr->laxis_title);
        printf("  min=%g  max=%g  tick_start=%g  tick_int=%g tick_mid_skip=%d\n",
            InfoPtr->laxis_min,InfoPtr->laxis_max,InfoPtr->laxis_tick_start,
            InfoPtr->laxis_tick_int,InfoPtr->laxis_tick_mid_skip);
        printf("  tick: show=%s num=%d ano=%s skip=%d abs_min=%g abs_max=%g\n",
            InfoPtr->laxis_tick_show ? "TRUE" : "FALSE",InfoPtr->laxis_tick_num,
            InfoPtr->laxis_tick_ano ? "TRUE" : "FALSE",InfoPtr->laxis_tick_skip,
            InfoPtr->laxis_tick_abs_min,InfoPtr->laxis_tick_abs_max);
        printf("  title_size=%g title_offset=%g ano_size=%g  ano_left=%d  ano_right=%d\n",
            InfoPtr->laxis_title_size,InfoPtr->laxis_title_offset,InfoPtr->laxis_ano_size,
            InfoPtr->laxis_ano_left,InfoPtr->laxis_ano_right);
        printf("Right axis title: %s\n",InfoPtr->raxis_title);
        printf("  min=%g  max=%g  tick_start=%g  tick_int=%g tick_mid_skip=%d\n",
            InfoPtr->raxis_min,InfoPtr->raxis_max,InfoPtr->raxis_tick_start,
            InfoPtr->raxis_tick_int,InfoPtr->raxis_tick_mid_skip);
        printf("  tick: show=%s num=%d ano=%s skip=%d abs_min=%g abs_max=%g\n",
            InfoPtr->raxis_tick_show ? "TRUE" : "FALSE",InfoPtr->raxis_tick_num,
            InfoPtr->raxis_tick_ano ? "TRUE" : "FALSE",InfoPtr->raxis_tick_skip,
            InfoPtr->raxis_tick_abs_min,InfoPtr->raxis_tick_abs_max);
        printf("  title_size=%g title_offset=%g ano_size=%g  ano_left=%d  ano_right=%d\n",
            InfoPtr->raxis_title_size,InfoPtr->raxis_title_offset,InfoPtr->raxis_ano_size,
            InfoPtr->raxis_ano_left,InfoPtr->raxis_ano_right);
        printf("Top axis title: %s\n",InfoPtr->taxis_title);
        printf("  min=%g  max=%g  tick_start=%g  tick_int=%g tick_mid_skip=%d\n",
            InfoPtr->taxis_min,InfoPtr->taxis_max,InfoPtr->taxis_tick_start,
            InfoPtr->taxis_tick_int,InfoPtr->taxis_tick_mid_skip);
        printf("  tick: show=%s num=%d ano=%s skip=%d abs_min=%g abs_max=%g\n",
            InfoPtr->taxis_tick_show ? "TRUE" : "FALSE",InfoPtr->taxis_tick_num,
            InfoPtr->taxis_tick_ano ? "TRUE" : "FALSE",InfoPtr->taxis_tick_skip,
            InfoPtr->taxis_tick_abs_min,InfoPtr->taxis_tick_abs_max);
        printf("  title_size=%g title_offset=%g ano_size=%g  ano_left=%d  ano_right=%d\n",
            InfoPtr->taxis_title_size,InfoPtr->taxis_title_offset,InfoPtr->taxis_ano_size,InfoPtr->taxis_ano_left,InfoPtr->taxis_ano_right);
        printf("Bottom axis title: %s\n",InfoPtr->baxis_title);
        printf("  min=%g  max=%g  tick_start=%g  tick_int=%g tick_mid_skip=%d\n",
            InfoPtr->baxis_min,InfoPtr->baxis_max,InfoPtr->baxis_tick_start,
            InfoPtr->baxis_tick_int,InfoPtr->baxis_tick_mid_skip);
        printf("  tick: show=%s num=%d ano=%s skip=%d abs_min=%g abs_max=%g\n",
            InfoPtr->baxis_tick_show ? "TRUE" : "FALSE",InfoPtr->baxis_tick_num,
            InfoPtr->baxis_tick_ano ? "TRUE" : "FALSE",InfoPtr->baxis_tick_skip,
            InfoPtr->baxis_tick_abs_min,InfoPtr->baxis_tick_abs_max);
        printf("  title_size=%g title_offset=%g ano_size=%g  ano_left=%d  ano_right=%d\n",
            InfoPtr->baxis_title_size,InfoPtr->baxis_title_offset,InfoPtr->baxis_ano_size,
            InfoPtr->baxis_ano_left,InfoPtr->baxis_ano_right);
    }
    /* Display warning */
    if (Debug)       puts("In TESTing mode.  No graphics");
    else if (Batch)  puts("In Batch processing mode.");
}
/****************************** GetOtherInfo() ******************************/
/* Verify info read from command file and calculate remaining minimum
 * required values.
 *
 * Parameters:
 *  struct DispParamInfoStruct *InfoPtr - adress of information structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, "gpr_disp.h"
 * Calls: fopen, fclose, ftell, fseek.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: October 27, 1995
 */
int GetOtherInfo (struct DispParamInfoStruct *InfoPtr)
{
    int error = 0;
    long samp_size;
    FILE *infile;

    /* Make sure info read correctly from command file */
    if (InfoPtr->input_datatype == 0)    error = 1;
    if (InfoPtr->total_samps == 0)       error = 2;
    if (InfoPtr->file_header_bytes < 0)  error = 3;
    if (InfoPtr->trace_header_bytes < 0) error = 4;
    if (InfoPtr->first_samp < 0)         error = 5;
    if (InfoPtr->total_samps < 1)        error = 6;

    /* Calculate number of bytes in 1 trace sample */
    switch (InfoPtr->input_datatype)
    {   case CHAR:  case UCHAR:                            samp_size = 1; break;
        case SHORT: case USHORT: case USHORT12:            samp_size = 2; break;
        case LONG:  case ULONG:  case ULONG24: case FLOAT: samp_size = 4; break;
        case DOUBLE:                                       samp_size = 8; break;
        default:
            error = 7;
            break;
    }

    /* Calculate the rest of minimal required info */
    if (!error)
    {   InfoPtr->ns_per_samp = InfoPtr->total_time/(InfoPtr->total_samps-1);
        if (InfoPtr->last_samp == 0)  InfoPtr->last_samp  = InfoPtr->total_samps - 1;
        if ((infile = fopen(InfoPtr->dat_infilename,"rb")) == NULL)
        {   error = 8;
            printf("ERROR occurred attempting to open file %s\n",InfoPtr->dat_infilename);
            if (log_file) fprintf(log_file,"ERROR occurred attempting to open file %s\n",InfoPtr->dat_infilename);
        } else
        {   /* determine number of GPR traces in file */
            long start_byte,last_byte,num_bytes;

            fseek(infile,(long)InfoPtr->file_header_bytes,SEEK_SET);
            start_byte = ftell(infile);        /* get current location (byte) */
            fseek(infile, 0L, SEEK_END);       /* go to end of file */
            last_byte = ftell(infile);         /* get last location (byte) */
            num_bytes = last_byte - start_byte;
            InfoPtr->total_traces = num_bytes /
                ((InfoPtr->total_samps * samp_size) + InfoPtr->trace_header_bytes);
            fclose(infile);
        }
    }

    return error;
}

/***************************** GetDataAsImage8() ****************************/
/* Get the data/info from the appropriate files and store as unsigned 8-bit
 * data row by row (ie. as an "image").  The data may be stored as any
 * recognized type and either row by row or column by column, as long as these
 * can be determined from data/info files.
 *
 * This function allocates storage for InfoPtr->image[][].
 *
 * NOTE: The "created" member of struct DispParamInfoStruct restricts the
 *       application of the struct to one data set and one grid at a time.
 *       To read multiple data sets and allocate multiple grids, either
 *       de-allocate the first one OR declare multiple structs to hold the
 *       information and pointers.
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <math.h>, <string.h>, <stdio.h>, <stdlib.h>,
 *           "gpr_disp1.h", "rif_pgms.h", "dzt_pgms.h", "gsg_pgms.h",
 *           "gpg_pgms.h".
 * Calls: malloc, free, printf, GetDatSubImage8, GetDztChSubImage8,
 *        GetGpgSubImage8, GetGsgSubImage8.
 * Returns: 0 on success,
 *         >0 on error.
 *
const char *GetDataAsImage8Msg[] =
{   "GetDataAsImage8(): No errors.",
    "GetDataAsImage8() ERROR: Not able to allocate storage for GPR image[][].\n\t\tTry increasing value of skip_traces.",
    "GetDataAsImage8() ERROR: Problem getting data from input file.",
    "GetDataAsImage8() ERROR: image[][] in this struct must be deallocated first.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: January 17, 1996
 */
int GetDataAsImage8 (struct DispParamInfoStruct *InfoPtr)
{
    int  itemp;          /* scratch variables and counters */
    long row,col;        /* scratch variables and counters */
    long image_row_size; /* number of bytes in one row of image */
    extern FILE *log_file;

    /* for GSSI SIR-10 files (see dzt_pgms.h) */
    extern const char *GetDztChSubImage8Msg[];

    /* for for all other formats */
    extern const char *GetSubImage8Msg[];

/***** Allocate storage for image[][] *****/
    if (InfoPtr->created) return 3;
    image_row_size = InfoPtr->num_cols * sizeof(unsigned char);
    InfoPtr->image = (unsigned char **)malloc((size_t)InfoPtr->num_rows * sizeof(unsigned char *));
    if (InfoPtr->image)
    {   for (row=0; row<InfoPtr->num_rows; row++)
        {   InfoPtr->image[row] = (unsigned char *)malloc((size_t)image_row_size);
            if (InfoPtr->image[row] == NULL)
            {   for (col=0; col<row; col++) free(InfoPtr->image[col]);
                InfoPtr->created = FALSE;
                return 1;
            }
        }
    } else
    {   InfoPtr->created = FALSE;
        return 1;
    }
    InfoPtr->created = TRUE;

/***** Get the data and store as 8-bit image (row by row) *****/
    itemp = 0;
    switch (InfoPtr->input_format)
    {   case DZT:  case MOD_DZT:
            itemp = GetDztChSubImage8(InfoPtr->dat_infilename,InfoPtr->channel,
                        InfoPtr->first_samp,InfoPtr->first_trace,InfoPtr->last_trace,
                        InfoPtr->skip_traces,InfoPtr->num_cols,InfoPtr->num_rows,
                        InfoPtr->image);
            if (itemp > 0)
            {   printf("\n%s\n",GetDztChSubImage8Msg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetDztChSubImage8Msg[itemp]);
            }
            break;
        case DT1:  case SGY:  case USER_DEF:
            itemp =  GetSubImage8(InfoPtr->dat_infilename,InfoPtr->input_datatype,
                        InfoPtr->if_image,InfoPtr->min_data_val,InfoPtr->max_data_val,
                        InfoPtr->file_header_bytes,InfoPtr->trace_header_bytes,
                        InfoPtr->first_trace,InfoPtr->last_trace,InfoPtr->skip_traces,
                        InfoPtr->first_samp,InfoPtr->total_samps,InfoPtr->num_cols,
                        InfoPtr->num_rows,InfoPtr->image);
            if (itemp > 0)
            {   printf("\n%s\n",GetSubImage8Msg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",GetSubImage8Msg[itemp]);
            }
            break;
        default:      /* unknown storage format */
            itemp = 2;
            break;
    }

    if (itemp)  return 2;
    else        return 0;
}
/***************************** GetCoordRanges() *****************************/
/* Find limits for horiz_start and horiz_stop and first_trace and last_trace.
 * Spline Hvals to get horizontal locations for each trace.
 * Spline Zvals to get elevations for each trace.
 * If only one layer, spline Vvals to get offset from surface (+ is above
 *   surface, - below) for each sample in a trace.
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <math.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h"
 * Calls: malloc, free, sqrt, Spline, printf.
 * Returns: 0 on success,
 *         >0 on error (offset into message string array)
 *
const char *GetCoordRangesMsg[] =
{   "GetCoordRanges() No error.",
    "GetCoordRanges() ERROR: Coord_mode must be 2 or 3.",
    "GetCoordRanges() ERROR: Tick marks and coordinates needed for rubber-sheeting.",
    "GetCoordRanges() ERROR: Tick marks for rubber-sheeting outside data range.",
    "GetCoordRanges() ERROR: Requested horizontal limits outside of data range.",
*5* "GetCoordRanges() ERROR: Problem splining data topography.",
    "GetCoordRanges() ERROR: Not able to allocate sufficient temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 7, 1995; September 6, 2000;
 */
int GetCoordRanges (struct DispParamInfoStruct *InfoPtr)
{
    int i,itemp,error,skip;
    int dat_forward;    /* records whether trace locations are increasing or decreasing */
    int hor_forward;    /* records whether hor. dir. is increasing or decreasing */
    long column_num;
    double *trace_num     = NULL; /* usable trace numbers */
    double *y             = NULL; /* "y" values pulled out of tick_xyz[][] or tick_val[] */
    double *tick_tracenum = NULL; /* copy InfoPtr array into double array */
    double *tick_trav     = NULL; /* traverse distances from tick_xyz[][] */
    extern const char *SplineMsg[];
    extern FILE *log_file;
    extern int Debug;

    if (InfoPtr->coord_mode != DISTANCE_DISTANCE &&
        InfoPtr->coord_mode != DISTANCE_TIME)
        return 1;

    /* must have at least 2 tick marks and valid data sets */
    if (InfoPtr->num_ticks < 2  || InfoPtr->tick_tracenum == NULL) return 2;
    if (InfoPtr->tick_xyz == NULL) return 2;

    /* verify range of tick marks is at least partially overlaps data;
       last_trace is always >= first_trace;
       tracenum[] always increases.
     */
    if ((InfoPtr->tick_tracenum[0] > InfoPtr->last_trace) ||
        (InfoPtr->tick_tracenum[InfoPtr->num_ticks-1] < InfoPtr->first_trace) )
        return 3;

    /* limit start and stop to valid ranges and find first and last traces */
    switch (InfoPtr->horiz_mode)
    {   case 1:   /* X-coordinates */
            /* verify requested horizontal start and stop within data range */
            if (InfoPtr->tick_xyz[0][0] < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                dat_forward = TRUE;
            else  dat_forward = FALSE;
            if (InfoPtr->horiz_start == InfoPtr->horiz_stop)
            {   InfoPtr->horiz_start = INVALID_VALUE;
                InfoPtr->horiz_stop  = INVALID_VALUE;
            }
            if (InfoPtr->horiz_start == INVALID_VALUE)
            {   if (InfoPtr->left == INVALID_VALUE)
                    InfoPtr->horiz_start = InfoPtr->tick_xyz[0][0];
                else
                    InfoPtr->horiz_start = InfoPtr->left;
            }
            if (InfoPtr->horiz_stop == INVALID_VALUE)
            {   if (InfoPtr->right == INVALID_VALUE)
                    InfoPtr->horiz_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];
                else
                    InfoPtr->horiz_stop = InfoPtr->right;
            }
            if (InfoPtr->horiz_start < InfoPtr->horiz_stop) hor_forward = TRUE;
            else  hor_forward = FALSE;
            if (dat_forward == TRUE)
            {   if ( (hor_forward == TRUE &&
                      (InfoPtr->horiz_start > InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0]
                      || InfoPtr->horiz_stop < InfoPtr->tick_xyz[0][0]))
                     || (hor_forward == FALSE &&
                      (InfoPtr->horiz_start < InfoPtr->tick_xyz[0][0]
                      || InfoPtr->horiz_stop > InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0]))
                   ) return 4;
            } else  /* dat_forward == FALSE */
            {   if ( (hor_forward == TRUE &&
                      (InfoPtr->horiz_start > InfoPtr->tick_xyz[0][0]
                      || InfoPtr->horiz_stop < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0]))
                     || (hor_forward == FALSE &&
                      (InfoPtr->horiz_start < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0]
                      || InfoPtr->horiz_stop > InfoPtr->tick_xyz[0][0]))
                   ) return 4;
            }

            /* find start and stop traces */
            if (dat_forward)
            {   if (hor_forward)
                {   /* find first trace */
                    if (InfoPtr->horiz_start <= InfoPtr->tick_xyz[0][0])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[0][0];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start > InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_start <= InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find last_trace */
                    if (InfoPtr->horiz_stop >= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop >= InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_stop < InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                } else  /* horiz direction reversed */
                {   /* find last trace */
                    if (InfoPtr->horiz_start >= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start >= InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_start < InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find first trace */
                    if (InfoPtr->horiz_stop <= InfoPtr->tick_xyz[0][0])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[0][0];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop > InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_stop <= InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                }   /* end hor dir rev */
            } else  /* dat direction reversed */
            {   if (hor_forward)
                {   /* find last trace */
                    if (InfoPtr->horiz_start <= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start <= InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_start > InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find first trace */
                    if (InfoPtr->horiz_stop >= InfoPtr->tick_xyz[0][0])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[0][0];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop < InfoPtr->tick_xyz[i-1][0] &&
                                   InfoPtr->horiz_stop >= InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                } else  /* horiz direction reversed */
                {   /* find first trace */
                    if (InfoPtr->horiz_start >= InfoPtr->tick_xyz[0][0])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[0][0];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start < InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_start >= InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find last trace */
                    if (InfoPtr->horiz_stop <= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][0];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop <= InfoPtr->tick_xyz[i-1][0] &&
                                InfoPtr->horiz_stop > InfoPtr->tick_xyz[i][0] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][0]) /
                                  (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                }   /* end hor dir rev */
            }  /* end dat dir rev */
            if (InfoPtr->last_trace >= InfoPtr->total_traces)
            {   InfoPtr->last_trace = InfoPtr->total_traces - 1;
                /* find why last trace was past end and interpolate new horiz_stop */
                for (i=1; i<InfoPtr->num_ticks; i++)
                {   if (InfoPtr->tick_tracenum[i] > InfoPtr->last_trace)
                    {   InfoPtr->horiz_stop =
                            InfoPtr->tick_xyz[i-1][0] +
                            ( (double)(InfoPtr->last_trace - InfoPtr->tick_tracenum[i-1])
                            / (double)(InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1])
                            )
                            * (InfoPtr->tick_xyz[i][0]-InfoPtr->tick_xyz[i-1][0]);
                        break;
                    }
                }
            }
            break;
        case 2:   /* y-coordinates */
            /* verify requested horizontal start and stop within data range */
            if (InfoPtr->tick_xyz[0][1] < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                dat_forward = TRUE;
            else dat_forward = FALSE;
            if (InfoPtr->horiz_start == INVALID_VALUE)
            {   if (InfoPtr->left == INVALID_VALUE)
                    InfoPtr->horiz_start = InfoPtr->tick_xyz[0][1];
                else
                    InfoPtr->horiz_start = InfoPtr->left;
            }
            if (InfoPtr->horiz_stop == INVALID_VALUE)
            {   if (InfoPtr->right == INVALID_VALUE)
                    InfoPtr->horiz_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];
                else
                    InfoPtr->horiz_stop = InfoPtr->right;
            }
            if (InfoPtr->horiz_start < InfoPtr->horiz_stop) hor_forward = TRUE;
            else hor_forward = FALSE;
            if (dat_forward == TRUE)
            {   if ( (hor_forward == TRUE &&
                      (InfoPtr->horiz_start > InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1]
                      || InfoPtr->horiz_stop < InfoPtr->tick_xyz[0][1]))
                     || (hor_forward == FALSE &&
                      (InfoPtr->horiz_start < InfoPtr->tick_xyz[0][1]
                      || InfoPtr->horiz_stop > InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1]))
                   ) return 4;
            } else
            {   if ( (hor_forward == TRUE &&
                      (InfoPtr->horiz_start > InfoPtr->tick_xyz[0][1]
                      || InfoPtr->horiz_stop < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1]))
                     || (hor_forward == FALSE &&
                      (InfoPtr->horiz_start < InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1]
                      || InfoPtr->horiz_stop > InfoPtr->tick_xyz[0][1]))
                   ) return 4;
            }

            /* find start and stop traces */
            if (dat_forward)
            {   if (hor_forward)
                {   /* find first trace */
                    if (InfoPtr->horiz_start <= InfoPtr->tick_xyz[0][1])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[0][1];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start > InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_start <= InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find last_trace */
                    if (InfoPtr->horiz_stop >= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop >= InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_stop < InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                } else  /* horiz direction reversed */
                {   /* find last trace */
                    if (InfoPtr->horiz_start >= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start >= InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_start < InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find first trace */
                    if (InfoPtr->horiz_stop <= InfoPtr->tick_xyz[0][1])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[0][1];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop > InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_stop <= InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                }   /* end hor dir rev */
            } else  /* dat direction reversed */
            {   if (hor_forward)
                {   /* find last trace */
                    if (InfoPtr->horiz_start <= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start <= InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_start > InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find first trace */
                    if (InfoPtr->horiz_stop >= InfoPtr->tick_xyz[0][1])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[0][1];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop < InfoPtr->tick_xyz[i-1][1] &&
                                   InfoPtr->horiz_stop >= InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                } else  /* horiz direction reversed */
                {   /* find first trace */
                    if (InfoPtr->horiz_start >= InfoPtr->tick_xyz[0][1])
                    {   InfoPtr->horiz_start = InfoPtr->tick_xyz[0][1];
                        InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_start < InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_start >= InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->first_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_start-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                    /* find last trace */
                    if (InfoPtr->horiz_stop <= InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1])
                    {   InfoPtr->horiz_stop = InfoPtr->tick_xyz[InfoPtr->num_ticks-1][1];
                        InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                    } else
                    {   for (i=1; i<InfoPtr->num_ticks; i++)
                        {   if (InfoPtr->horiz_stop <= InfoPtr->tick_xyz[i-1][1] &&
                                InfoPtr->horiz_stop > InfoPtr->tick_xyz[i][1] )
                            {   InfoPtr->last_trace =
                                  InfoPtr->tick_tracenum[i-1] +
                                  (((InfoPtr->horiz_stop-InfoPtr->tick_xyz[i-1][1]) /
                                  (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]))
                                  * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                            }
                        }
                    }
                }   /* end hor dir rev */
            }  /* end dat dir rev */
            if (InfoPtr->last_trace >= InfoPtr->total_traces)
            {   InfoPtr->last_trace = InfoPtr->total_traces - 1;
                /* find why last trace was past end and interpolate new horiz_stop */
                for (i=1; i<InfoPtr->num_ticks; i++)
                {   if (InfoPtr->tick_tracenum[i] > InfoPtr->last_trace)
                    {   InfoPtr->horiz_stop =
                            InfoPtr->tick_xyz[i-1][1] +
                            ( (double)(InfoPtr->last_trace - InfoPtr->tick_tracenum[i-1])
                            / (double)(InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1])
                            )
                            * (InfoPtr->tick_xyz[i][1]-InfoPtr->tick_xyz[i-1][1]);
                        break;
                    }
                }
            }
            break;
        case 3:  default:   /* traverse distance */
        {   double dtemp1,dtemp2;

            /* allocate storage for traverse distances */
            tick_trav = (double *)malloc(InfoPtr->num_ticks * sizeof(double));
            if (tick_trav == NULL)  return 6;

            /* calculate traverse distances */
            dat_forward = TRUE;
            tick_trav[0] =  0.0;
            for (i=1; i<InfoPtr->num_ticks; i++)
            {   dtemp1 = InfoPtr->tick_xyz[i][0] - InfoPtr->tick_xyz[i-1][0];
                dtemp2 = InfoPtr->tick_xyz[i][1] - InfoPtr->tick_xyz[i-1][1];
                tick_trav[i] = tick_trav[i-1] + sqrt(dtemp1*dtemp1 + dtemp2*dtemp2);
            }
            /* verify requested horizontal start and stop within data range */
            if (InfoPtr->horiz_start == INVALID_VALUE)
            {   if (InfoPtr->left == INVALID_VALUE)
                    InfoPtr->horiz_start = tick_trav[0];
                else
                    InfoPtr->horiz_start = InfoPtr->left;
            }
            if (InfoPtr->horiz_stop == INVALID_VALUE)
            {   if (InfoPtr->right == INVALID_VALUE)
                    InfoPtr->horiz_stop = tick_trav[InfoPtr->num_ticks-1];
                else
                    InfoPtr->horiz_stop = InfoPtr->right;
            }
            if (InfoPtr->horiz_start < InfoPtr->horiz_stop) hor_forward = TRUE;
            else                                            hor_forward = FALSE;
            if ( ( hor_forward == TRUE &&
                  (InfoPtr->horiz_start > tick_trav[InfoPtr->num_ticks-1] ||
                   InfoPtr->horiz_stop  < tick_trav[0]
                  )
                 ) ||
                 ( hor_forward == FALSE &&
                  (InfoPtr->horiz_start < tick_trav[0] ||
                   InfoPtr->horiz_stop  > tick_trav[InfoPtr->num_ticks-1]
                  )
                 )
               ) return 4;

            if (hor_forward)
            {   /* find first trace */
                if (InfoPtr->horiz_start <= tick_trav[0])
                {   InfoPtr->horiz_start = tick_trav[0];
                    InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                } else
                {   for (i=1; i<InfoPtr->num_ticks; i++)
                    {   if (InfoPtr->horiz_start > tick_trav[i-1] &&
                            InfoPtr->horiz_start <= tick_trav[i] )
                        {   InfoPtr->first_trace =
                              InfoPtr->tick_tracenum[i-1] +
                              (((InfoPtr->horiz_start-tick_trav[i-1]) /
                              (tick_trav[i]-tick_trav[i-1]))
                              * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                        }
                    }
                }
                /* find last_trace */
                if (InfoPtr->horiz_stop >= tick_trav[InfoPtr->num_ticks-1])
                {   InfoPtr->horiz_stop = tick_trav[InfoPtr->num_ticks-1];
                    InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                } else
                {   for (i=1; i<InfoPtr->num_ticks; i++)
                    {   if (InfoPtr->horiz_stop >= tick_trav[i-1] &&
                               InfoPtr->horiz_stop < tick_trav[i] )
                        {   InfoPtr->last_trace =
                              InfoPtr->tick_tracenum[i-1] +
                              (((InfoPtr->horiz_stop - tick_trav[i-1]) /
                              (tick_trav[i] - tick_trav[i-1]))
                              * (InfoPtr->tick_tracenum[i] - InfoPtr->tick_tracenum[i-1]));
                        }
                    }
                }
            } else  /* horizontal direction reversed */
            {   /* find first trace */
                if (InfoPtr->horiz_stop <= tick_trav[0])
                {   InfoPtr->horiz_stop = tick_trav[0];
                    InfoPtr->first_trace = InfoPtr->tick_tracenum[0];
                } else
                {   for (i=1; i<InfoPtr->num_ticks; i++)
                    {   if (InfoPtr->horiz_stop > tick_trav[i-1] &&
                            InfoPtr->horiz_stop <= tick_trav[i] )
                        {   InfoPtr->first_trace =
                              InfoPtr->tick_tracenum[i-1] +
                              (((InfoPtr->horiz_stop - tick_trav[i-1]) /
                              (tick_trav[i]-tick_trav[i-1]))
                              * (InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1]));
                        }
                    }
                }
                /* find last_trace */
                if (InfoPtr->horiz_start >= tick_trav[InfoPtr->num_ticks-1])
                {   InfoPtr->horiz_start = tick_trav[InfoPtr->num_ticks-1];
                    InfoPtr->last_trace = InfoPtr->tick_tracenum[InfoPtr->num_ticks-1];
                } else
                {   for (i=1; i<InfoPtr->num_ticks; i++)
                    {   if (InfoPtr->horiz_start >= tick_trav[i-1] &&
                               InfoPtr->horiz_start < tick_trav[i] )
                        {   InfoPtr->last_trace =
                              InfoPtr->tick_tracenum[i-1] +
                              (((InfoPtr->horiz_start - tick_trav[i-1]) /
                              (tick_trav[i] - tick_trav[i-1]))
                              * (InfoPtr->tick_tracenum[i] - InfoPtr->tick_tracenum[i-1]));
                        }
                    }
                }
            }
            if (InfoPtr->last_trace >= InfoPtr->total_traces)
            {   InfoPtr->last_trace = InfoPtr->total_traces - 1;
                /* find why last trace was past end and interpolate new horiz_stop */
                for (i=1; i<InfoPtr->num_ticks; i++)
                {   if (InfoPtr->tick_tracenum[i] > InfoPtr->last_trace)
                    {   InfoPtr->horiz_stop =
                            tick_trav[i-1] +
                            ( (double)(InfoPtr->last_trace - InfoPtr->tick_tracenum[i-1])
                            / (double)(InfoPtr->tick_tracenum[i]-InfoPtr->tick_tracenum[i-1])
                            )
                            * (tick_trav[i]-tick_trav[i-1]);
                        break;
                    }
                }
            }
            break;
        }   /* end of case 3: */
    }   /* end of switch on horiz_mode block */

    /* Determine number of columns and rows */
    column_num = InfoPtr->last_trace - InfoPtr->first_trace + 1;
    if (InfoPtr->skip_traces > 0)
    {   skip = InfoPtr->skip_traces + 1;
        InfoPtr->num_cols = 2 + ((column_num-2)/skip); /* expect truncation */
    } else
    {   skip = 1;
        InfoPtr->num_cols = column_num;
    }
    InfoPtr->num_rows = InfoPtr->last_samp - InfoPtr->first_samp + 1;

    /* Allocate temporary storage and spline vectors as required */
    InfoPtr->Hvals = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    InfoPtr->Vvals = (double *)malloc((size_t)InfoPtr->num_rows * sizeof(double));
    InfoPtr->Zvals = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    trace_num      = (double *)malloc((size_t)InfoPtr->num_cols * sizeof(double));
    tick_tracenum  = (double *)malloc((size_t)InfoPtr->num_ticks * sizeof(double));
    y              = (double *)malloc((size_t)InfoPtr->num_ticks * sizeof(double));
    if ( trace_num == NULL || y == NULL || tick_tracenum == NULL ||
         InfoPtr->Hvals == NULL || InfoPtr->Vvals == NULL ||
         InfoPtr->Zvals == NULL )
    {   free(tick_tracenum); free(tick_trav);  free(trace_num); free(y);
        free(InfoPtr->Hvals); free(InfoPtr->Vvals); free(InfoPtr->Zvals);
        InfoPtr->Hvals = NULL;
        InfoPtr->Vvals = NULL;
        InfoPtr->Zvals = NULL;
        return 6;
    }

    /* Assign values for Spline() */
    for (i=0; i<InfoPtr->num_cols; i++)
        trace_num[i] = InfoPtr->first_trace + (i * skip);
    if (trace_num[InfoPtr->num_cols - 1] > InfoPtr->last_trace)
        trace_num[InfoPtr->num_cols - 1] = InfoPtr->last_trace;
#if 0
    if (Debug)
    {   printf("GetCoordRanges: trace_num[] vals to follow:");
        printf("first_trace = %d  last_trace = %d  skip = %d  num_cols = %d\n",
                InfoPtr->first_trace,InfoPtr->last_trace,skip,InfoPtr->num_cols);
        getch();
        for (i=0; i<InfoPtr->num_cols; i++) printf("%6d %g\n",i,trace_num[i]);
        getch();
    }
#endif
    for (i=0; i<InfoPtr->num_ticks; i++) tick_tracenum[i] = InfoPtr->tick_tracenum[i];
    for (i=0; i<InfoPtr->num_ticks; i++) y[i] = InfoPtr->tick_xyz[i][2];

    /* Spline to get "Z" values for every trace used */
    itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,
                    tick_tracenum,y,trace_num,InfoPtr->Zvals);
    if (itemp > 0)
    {   printf("\nZvals: %s\n",SplineMsg[itemp]);
        if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
        free(tick_trav);  free(trace_num);  free(y);
        return 5;
    }
    error = 0;

    /* Spline to get "X" values for every trace used */
    switch (InfoPtr->horiz_mode)
    {   case 1:   /* X-coordinates */
            for (i=0; i<InfoPtr->num_ticks; i++) y[i] = InfoPtr->tick_xyz[i][0];
            itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,
                    tick_tracenum,y,trace_num,InfoPtr->Hvals);
            if (itemp > 0)
            {   printf("\nHvals (X): %s\n",SplineMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
                error = 5;
            }
            break;
        case 2:   /* Y-coordinates */
            for (i=0; i<InfoPtr->num_ticks; i++) y[i] = InfoPtr->tick_xyz[i][1];
            itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,tick_tracenum,
                    y,trace_num,InfoPtr->Hvals);
            if (itemp > 0)
            {   printf("\nHvals (Y): %s\n",SplineMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
                error = 5;
            }
            break;
        case 3:   /* traverse-coordinates */
            itemp = Spline(InfoPtr->num_ticks,InfoPtr->num_cols,tick_tracenum,
                    tick_trav,trace_num,InfoPtr->Hvals);
            if (itemp > 0)
            {   printf("\nZvals (T): %s\n",SplineMsg[itemp]);
                if (log_file) fprintf(log_file,"%s\n",SplineMsg[itemp]);
                error = 5;
            }
            break;
    }   /* end of switch on horiz_mode block */

    /* If layer == 1, spline to get "Y" depth offset values for every sample
       in a trace */
    if (InfoPtr->num_layers == 1)
    {   double samp_height;    /* meters per sample */
        double dtemp;

        samp_height = (0.2998 * InfoPtr->ns_per_samp)
                        / (2 * sqrt(InfoPtr->layer_rdp[0]));
        dtemp = 0.0;
        for (i=0; i<InfoPtr->num_rows; i++,dtemp-=samp_height)
            InfoPtr->Vvals[i] = dtemp; /* offset from surface; + above; - below */
    }

    /* If coord_mode == 4, assign time for each sample */
    if (InfoPtr->coord_mode == DISTANCE_TIME)
    {   double time_val,time_inc;

        time_val = InfoPtr->first_samp_time;
        time_inc = InfoPtr->ns_per_samp;
        for (i=0; i<InfoPtr->num_rows; i++,time_val+=time_inc)
                InfoPtr->Vvals[i] = time_val;
    }

    free(tick_tracenum); free(tick_trav);  free(trace_num);  free(y);
    return error;
}
/**************************** DeallocInfoStruct() ***************************/
/* Deallocate the buffer space in the information structure.  This function
 * will work correctly only if the num_... variables accurately reflect
 * the successful allocation of storage.
 *
 * NOTE: "caption" and "pagenum" are not deallocated.
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <stdlib.h>, "gpr_disp.h".
 * Calls: free.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 28, 1995
 */
void DeallocInfoStruct (struct DispParamInfoStruct *InfoPtr)
{
    int i;
    extern int Debug;

    if (InfoPtr->num_bad > 0)
    {   if (InfoPtr->bad_traces)
        {   free(InfoPtr->bad_traces);
            InfoPtr->bad_traces = NULL;
        }
        InfoPtr->num_bad = 0;
    }
    if (InfoPtr->num_ticks > 0)
    {   if (InfoPtr->tick_tracenum)
        {   free(InfoPtr->tick_tracenum);
            InfoPtr->tick_tracenum = NULL;
        }
        if (InfoPtr->tick_xyz)
        {   for (i=0; i<InfoPtr->num_ticks; i++)
                free(InfoPtr->tick_xyz[i]);
            free(InfoPtr->tick_xyz);
            InfoPtr->tick_xyz = NULL;
        }
        InfoPtr->num_ticks = 0;
    }
    if (InfoPtr->num_wiggles > 0)
    {   if (InfoPtr->wiggle_traceval)
        {   for (i=0; i<InfoPtr->num_wiggles; i++)
                free(InfoPtr->wiggle_traceval[i]);
            free(InfoPtr->wiggle_traceval);
            InfoPtr->wiggle_traceval = NULL;
        }
        InfoPtr->num_wiggles = 0;
    }
    if (InfoPtr->num_gain_off > 0)
    {   if (InfoPtr->gain_off)
        {   free(InfoPtr->gain_off);
            InfoPtr->gain_off = NULL;
        }
        InfoPtr->num_gain_off = 0;
    }
    if (InfoPtr->num_gain_on > 0)
    {   if (InfoPtr->gain_on)
        {   free(InfoPtr->gain_on);
            InfoPtr->gain_on = NULL;
        }
        InfoPtr->num_gain_on = 0;
    }
    if (InfoPtr->num_layers > 0)
    {   if (InfoPtr->layer_rdp)
        {   free(InfoPtr->layer_rdp);
            InfoPtr->layer_rdp = NULL;
        }
        if (InfoPtr->layer_mode)
        {   free(InfoPtr->layer_mode);
            InfoPtr->layer_mode = NULL;
        }
        if (InfoPtr->layer_val)
        {   free(InfoPtr->layer_val);
            InfoPtr->layer_val = NULL;
        }
        InfoPtr->num_layers = 0;
    }
    if (InfoPtr->Hvals)
    {   free(InfoPtr->Hvals);
        InfoPtr->Hvals = NULL;
    }
    if (InfoPtr->Vvals)
    {   free(InfoPtr->Vvals);
        InfoPtr->Vvals = NULL;
    }
    if (InfoPtr->Zvals)
    {   free(InfoPtr->Zvals);
        InfoPtr->Zvals = NULL;
    }

    if (InfoPtr->created)
    {   if (InfoPtr->image)
        {   for (i=0; i<InfoPtr->num_rows; i++)  free(InfoPtr->image[i]);
            free(InfoPtr->image);
            InfoPtr->image = NULL;
        }
        InfoPtr->created = FALSE;
        InfoPtr->num_cols = 0;
        InfoPtr->num_rows = 0;
    }
}
/****************************** GetDataLimits() *****************************/
/* Determine the left, right, bottom, and top user units to be used in the
 * data display window.  If values were given in the CMD file, those are
 * used, otherwise values are determined based on the coordinate mode (and
 * layer mode if coord_mode == 2).
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of structure
 *
 * Requires: <assert.h>, <math.h>, "gpr_disp.h".
 * Calls: assert, sqrt.
 * Returns:  0 on success, otherwise
 *          >0 on error (offset into message strings array.
 *
const char *GetDataLimitsMsg[] =
{   "GetDataLimits(): No errors.",
    "GetDataLimits() ERROR: Unrecognized layer model mode.",
    "GetDataLimits() ERROR: Unrecognized coordinate system mode.",
    "GetDataLimits() ERROR: horiz_start or horiz_stop == INVALID_VALUE.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 7, 1995
 * Revisions: September 30, 1997
 */
int GetDataLimits (struct DispParamInfoStruct *InfoPtr)
{
    long i;
    double zmin,zmax;

    switch (InfoPtr->coord_mode)
    {   case TRACE_TIME:
            if (InfoPtr->left   == INVALID_VALUE &&
                InfoPtr->right  == INVALID_VALUE )
                assert(InfoPtr->last_trace - InfoPtr->first_trace);
            assert(InfoPtr->last_samp_time - InfoPtr->first_samp_time);
            if (InfoPtr->left   == INVALID_VALUE) InfoPtr->left   = InfoPtr->first_trace;
            if (InfoPtr->right  == INVALID_VALUE) InfoPtr->right  = InfoPtr->last_trace;
            /* remember, first image row is toward top of screen viewport */
            if (InfoPtr->bottom == INVALID_VALUE) InfoPtr->bottom = InfoPtr->last_samp_time;
            if (InfoPtr->top    == INVALID_VALUE) InfoPtr->top    = InfoPtr->first_samp_time;
            break;
        case DISTANCE_DISTANCE:
            if (InfoPtr->horiz_start == INVALID_VALUE ||
                InfoPtr->horiz_stop  == INVALID_VALUE) return 3;
            assert( (InfoPtr->Zvals) &&
                    ((InfoPtr->horiz_stop - InfoPtr->horiz_start) != 0.0) );
            if (InfoPtr->left  == INVALID_VALUE) InfoPtr->left  = InfoPtr->horiz_start;
            if (InfoPtr->right == INVALID_VALUE) InfoPtr->right = InfoPtr->horiz_stop;
            /* find max and min of zvals[] */
            zmax = zmin = InfoPtr->Zvals[0];
            for (i=1; i<InfoPtr->num_cols; i++)
            {   if (InfoPtr->Zvals[i] > zmax)
                {   zmax = InfoPtr->Zvals[i];
                    continue;
                }
                if (InfoPtr->Zvals[i] < zmin)
                {   zmin = InfoPtr->Zvals[i];
                }
            }
            if (InfoPtr->top    == INVALID_VALUE) InfoPtr->top = zmax;
            if (InfoPtr->bottom == INVALID_VALUE)
            { /* calculate estimated lowest point using trace with lowest elevation */
                int    i,layer;
                long   current_samp; /* last sample with elevation calculated */
                double current_elev; /* elevation calculated for current_samp */
                double samp_height;  /* meters per point */
                double bottom_elev;  /* elevation of bottom of layer */
                double layer_bottom[MAX_LAYERS]; /* caclulate bottom elevations */

                for (layer=0; layer<InfoPtr->num_layers; layer++)
                {   switch (InfoPtr->layer_mode[layer])
                    {   case 1:
                            layer_bottom[layer] = zmin - InfoPtr->layer_val[layer];
                            break;
                        case 2:
                            layer_bottom[layer] = InfoPtr->layer_val[layer];
                            break;
                        case 3:
                            layer_bottom[layer] = INVALID_VALUE;
                            break;
                        default:
                            return 1;  /* unrecognized layer mode */
                    }
                }
                current_samp = InfoPtr->first_samp;
                current_elev = zmin;
                for (layer=0; layer<InfoPtr->num_layers; layer++)
                {   switch (InfoPtr->layer_mode[layer])
                    {   case 1:  /* uniform depth below surface */
                            samp_height = (0.2998*InfoPtr->ns_per_samp)
                                          / (2*sqrt(InfoPtr->layer_rdp[layer]));
                            bottom_elev = InfoPtr->layer_val[layer];
                            for (i=0; i<InfoPtr->num_layers; i++)
                            {   if (i != layer)
                                {   if (layer_bottom[i] != INVALID_VALUE)
                                    {   if (layer_bottom[i] > bottom_elev)
                                            bottom_elev = layer_bottom[i];
                                    }
                                }
                            }
                            while (current_elev > bottom_elev)
                            {   if (current_samp >= InfoPtr->last_samp) break;
                                current_samp++;
                                current_elev -= samp_height;
                            }
                            break;
                        case 2:  /* uniform elevation above sea level */
                            samp_height = (0.2998*InfoPtr->ns_per_samp)
                                          / (2*sqrt(InfoPtr->layer_rdp[layer]));
                            bottom_elev = InfoPtr->layer_val[layer];
                            for (i=0; i<InfoPtr->num_layers; i++)
                            {   if (i != layer)
                                {   if (layer_bottom[i] != INVALID_VALUE)
                                    {   if (layer_bottom[i] > bottom_elev)
                                            bottom_elev = layer_bottom[i];
                                    }
                                }
                            }
                            while (current_elev > bottom_elev)
                            {   if (current_samp >= InfoPtr->last_samp) break;
                                current_samp++;
                                current_elev -= samp_height;
                            }
                            break;
                        case 3:  /* only or lowest layer; no bottom */
                            samp_height = (0.2998*InfoPtr->ns_per_samp)
                                          / (2*sqrt(InfoPtr->layer_rdp[layer]));
                            while (current_samp < InfoPtr->last_samp)
                            {   current_samp++;
                                current_elev -= samp_height;
                            }
                            break;
                    }   /*  end of switch block */
                }   /* end of for (...num_layers...) block */
                InfoPtr->bottom = current_elev;
            }   /* end of if-else block */
            break;
        case TIME_TIME:
            assert( (InfoPtr->last_trace_time - InfoPtr->first_trace_time) &&
                    (InfoPtr->last_samp_time - InfoPtr->first_samp_time) );
            if (InfoPtr->left   == INVALID_VALUE) InfoPtr->left   = InfoPtr->first_trace_time;
            if (InfoPtr->right  == INVALID_VALUE) InfoPtr->right  = InfoPtr->last_trace_time;
            /* remember, first image row is toward top of screen viewport */
            if (InfoPtr->bottom == INVALID_VALUE) InfoPtr->bottom = InfoPtr->last_samp_time;
            if (InfoPtr->top    == INVALID_VALUE) InfoPtr->top    = InfoPtr->first_samp_time;
            break;
        case DISTANCE_TIME:
            assert( ((InfoPtr->horiz_stop - InfoPtr->horiz_start) != 0.0) &&
                    (InfoPtr->last_samp_time - InfoPtr->first_samp_time) );
            if (InfoPtr->left  == INVALID_VALUE) InfoPtr->left  = InfoPtr->horiz_start;
            if (InfoPtr->right == INVALID_VALUE) InfoPtr->right = InfoPtr->horiz_stop;
            /* remember, first image row is toward top of screen viewport */
            if (InfoPtr->bottom == INVALID_VALUE) InfoPtr->bottom = InfoPtr->last_samp_time;
            if (InfoPtr->top    == INVALID_VALUE) InfoPtr->top    = InfoPtr->first_samp_time;
            break;
        case TRACE_SAMPLE:
            assert( (InfoPtr->last_trace - InfoPtr->first_trace) &&
                    (InfoPtr->last_samp - InfoPtr->first_samp) );
            if (InfoPtr->left   == INVALID_VALUE) InfoPtr->left   = InfoPtr->first_trace;
            if (InfoPtr->right  == INVALID_VALUE) InfoPtr->right  = InfoPtr->last_trace;
            /* remember, first image row is toward top of screen viewport */
            if (InfoPtr->bottom == INVALID_VALUE) InfoPtr->bottom = InfoPtr->last_samp;
            if (InfoPtr->top    == INVALID_VALUE) InfoPtr->top    = InfoPtr->first_samp;
            break;
        default:
            return 2;   /* unrecognized coordinate mode */
    }
    return 0;
}
/****************************************************************************/
/********* graphics functions using libhpgl.lib and gpr_gfx.lib *************/
/****************************************************************************/
/***************************** DisplayImage8() ******************************/
/* This function "displays" or "copies" a grid of unsigned bytes values to
 * hpgl_array_8[][].
 *
 * Parameter list:
 *  int vcols       - number of CRT horizontal pixels
 *  int vrows       - number of CRT vertical pixels
 *  struct DispParamInfoStruct *InfoPtr - address of information structure
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *       This function inherits the graphics mode, CRT viewport, user window,
 *         and color/grayscale palette from the calling routine.
 *       Plotting is 2D only.
 *       The values in InfoPtr->Hvals, ->Vvals, and ->Zvals are transformed
 *         to pixel coordinates!  But presumably we no longer need them after
 *         this function has displayed the data to the CRT.
 *  !!!! The interpolation schemes in this function require that graphics
 *         has been initialized with plotting to the global array
 *         hpgl_array_8[][] using: plotter_is(0,"ARRAY");.  This array is the
 *         same size as the screen and matches it pixel for pixel.
 *
 * Requires: <hpgl.h>, <stdlib.h>, "gpr_disp.h".
 * Calls: malloc, free, hpgl_pen_color, plot_pixel, InitTable8, InstAmpImage8u,
 *        InstAmpImage8u, HistStretchImage8u, _ABS, RemAvgColumn8u,
 *        LocalStretchTable8, BrightenTable8, ReverseTable8, SquareImage8u,
 *        ChangeRangeGain8u, AbsValImage8u.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
 *
const char *DisplayImage8Msg[] =
{   "DisplayImage8(): No errors.",
    "DisplayImage8() ERROR: One or more positioning arrays are NULL.",
    "DisplayImage8() ERROR: Not able to allocate sufficient temporary storage.",
    "DisplayImage8() ERROR: Unrecognized coordinate mode.",
    "DisplayImage8() ERROR: Unrecognized layer model mode.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: December 15, 1995
 */
int DisplayImage8 (int vcols,int vrows,struct DispParamInfoStruct *InfoPtr)
{
    int i,col,row;                 /* counters */
    int ival,itemp;                /* scratch variables */
    int tablesize   = 256;         /* size of table[] */
    int *table = NULL;             /* [tablesize]; lookup table for grayscale */
    double VX2Y_ratio,VXMAX,VYMAX; /* viewport space */
    int isum,nn,nx,ny,icount;      /* used to fill in skipped pixel columns */
    int Pnx,Pny,Pn,w1,w2,iw1,iw2;  /* ditto */

    /* following variables hold values in pixel coordinates */
    int PX1,PX2,PY1,PY2;              /* viewport corners */
    int PXrange,PYrange;              /* width and height of viewport */
    int PXprev,PYprev;                /* previous pixels plotted */
    int Px,Py;                        /* current pixel to plot */
    int PYtop,PYbottom;
    double pix_per_winX,pix_per_winY; /* scale factors; pixels per user units */

    extern int hpgl_pen_color,v_rowy;     /* <hpgl.h> */
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [*vrows][*vcols] from hpgl.h */
    #endif
    extern int Debug;                     /* "gpr_disp.h" */
    extern FILE *log_file;                /* "gpr_disp.h" */
    extern const char *ChangeRangeGain8uMsg[];
    extern const char *InstAmpImage8uMsg[];
    extern const char *InstPowImage8uMsg[];

/***** Validate some parameters *****/
    if (InfoPtr->Hvals == NULL || (InfoPtr->Vvals == NULL && InfoPtr->Zvals == NULL))
        return 1;

/***** Allocate storage for work arrays *****/
    table = (int *)malloc(tablesize*sizeof(int));
    if (table == NULL)  return 2;

/***** Translate viewport limits of window to pixel coordinates *****/
    /* In THIS function, pixel space (0,0) is lower left corner of screen */
    VX2Y_ratio = (double)vcols/(double)vrows;
    VYMAX = 100.0;
    VXMAX = VYMAX * VX2Y_ratio;
    PX1 = (InfoPtr->VX1/VXMAX)*(vcols-1);
    PX2 = (InfoPtr->VX2/VXMAX)*(vcols-1);
    PY1 = (InfoPtr->VY1/VYMAX)*(vrows-1);
    PY2 = (InfoPtr->VY2/VYMAX)*(vrows-1);

    if (Debug)
    {   printf("DisplayImage8():\n\tvcols=%d vrows=%d ratio=%g\n",vcols,vrows,VX2Y_ratio);
        printf("\tPX1=%d PX2=%d PY1=%d PY2=%d\n",PX1,PX2,PY1,PY2);
        printf("\tVX1=%g VX2=%g VY1=%g VY2=%g\n",
            InfoPtr->VX1,InfoPtr->VX2,InfoPtr->VY1,InfoPtr->VY2);
    }

/***** Translate Hvals[] and Vvals[] to pixels.  Remember that left, right,
       bottom, and top are in "user" units appropriate to the
       coordinate mode selected. If bottom > top or left > right, then
       pix_per_winY and pix_per_winX will be negative, respectively, and
       coordinate transformations (from user units to pixels) will be done
       correctly below! *****/
    PXrange = PX2 - PX1;
    PYrange = PY2 - PY1;
    pix_per_winX = (double)PXrange / (InfoPtr->right - InfoPtr->left);
    pix_per_winY = (double)PYrange / (InfoPtr->top - InfoPtr->bottom);
    if (Debug)
    {   printf("\tpix_per_winX=%g  pix_per_winY=%g\n",pix_per_winX,pix_per_winY);
        getch();
    }
    for (col=0; col<InfoPtr->num_cols; col++)
    {   InfoPtr->Hvals[col] = PX1 +
                 ((InfoPtr->Hvals[col] - InfoPtr->left) * pix_per_winX);
    }
    /* convert wiggle traces from user units to pixels */
    if (InfoPtr->num_wiggles > 0)
    {   for (col=0; col<InfoPtr->num_wiggles; col++)
        {   InfoPtr->wiggle_traceval[col][1] = PX1 +
                     ((InfoPtr->wiggle_traceval[col][0] - InfoPtr->left) * pix_per_winX);
        }
    }
#if 0
    if (Debug)
    {   puts("DisplayImage8: Hvals[] to follow");
        getch();
        for (i=0; i<InfoPtr->num_cols; i++) printf("%6d %g\n",i,InfoPtr->Hvals[i]);
        getch();
    }
#endif
    /* convert Vvals[] from user units to pixels and find closest wiggles
        to assign vertical pixels to */
    switch (InfoPtr->coord_mode)
    {   case TRACE_TIME:  case TIME_TIME:  case DISTANCE_TIME: case TRACE_SAMPLE:
            /* For cases 1 and 3, Vvals[] represents a position within the
               user window. (The top is "horizontal" or flat.) */
            for (row=0; row<InfoPtr->num_rows; row++)
            {   InfoPtr->Vvals[row] = PY1 +
                    ((InfoPtr->Vvals[row] - InfoPtr->bottom) * pix_per_winY);
            }
            break;
        case DISTANCE_DISTANCE:
            /* For case 2, the positions of the samples within the user window
               must be calcuted seperately for each trace (column).  The top
               is not flat.  If num_layers == 1, Vvals[] hold the offset in
               user units for each sample from the surface (Zvals[]);
               + above surface, - below.  For multi-layer models, Vvals[]
               will hold the calculated vertical pixel location. */
            break;
        default:
            free(table);
            return 3;
    }

/***** Initialize lookup table for grayscale display *****/
    InitTable8(table,tablesize,InfoPtr->range,InfoPtr->epc_threshold,
              InfoPtr->epc_contrast,InfoPtr->epc_sign,InfoPtr->epc_gain);

/***** Perform image processing functions *****/
    /* Change range gain if requested */
    if (InfoPtr->change_gain)
    {   int itemp;
        extern const char *ChangeRangeGain8uMsg[];

        itemp = ChangeRangeGain8u(InfoPtr->num_cols,InfoPtr->num_rows,
                                InfoPtr->num_gain_off,InfoPtr->gain_off,
                                InfoPtr->num_gain_on,InfoPtr->gain_on,
                                InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",ChangeRangeGain8uMsg[itemp]);
        }
    }
    /* Remove a "background trace" if requested */
    if (InfoPtr->background)
        RemAvgColumn8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->image);

    /* Take absolute value of amplitude and multiply by 2 */
    if (InfoPtr->abs_val)
        AbsValImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->image);

    /* Square amplitudes and divide by 64 */
    if (InfoPtr->square)
        SquareImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->image);

    /* Replace amps with instantaneous amplitude and wrap with envelope */
    if (InfoPtr->inst_amp)
    {   int itemp;
        extern const char *InstAmpImage8uMsg[];

        itemp = InstAmpImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->envelope,InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",InstAmpImage8uMsg[itemp]);
        }
    }
    /* Replace amps with instantaneous power and wrap with envelope */
    if (InfoPtr->inst_pow)
    {   int itemp;
        double dtemp;
        extern const char *InstPowImage8uMsg[];

        itemp = InstPowImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->envelope,InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",InstPowImage8uMsg[itemp]);
        }
    }

    /* Enhance contrast of image if requested */
    if (InfoPtr->stretch)
    {   int itemp;
        extern const char *HistStretchImage8uMsg[];

        itemp = HistStretchImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->stretch,InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",HistStretchImage8uMsg[itemp]);
        }
    }

/***** Perform look-up table functions *****/
    /* Enhance local contrast of gray scale if requested */
    if (InfoPtr->local_stretch)
        LocalStretchTable8(table,tablesize,InfoPtr->local_offset,InfoPtr->local_change);

    /* Brighten or darken gray scale if requested */
    if (InfoPtr->brightness)
        BrightenTable8(table,tablesize,InfoPtr->brightness);

    /* Reverse gray scale if requested */
    if (InfoPtr->negative)
        ReverseTable8(table,tablesize);

/***** Copy image into hpgl_array_8[][] *****/
    if (Debug)
    {   puts("Debug mode: no plot_pixel() etc.");
        free(table);
        return 0;
    }
    switch (InfoPtr->coord_mode)
    {   case TRACE_TIME:  case TIME_TIME:  case DISTANCE_TIME: case TRACE_SAMPLE:
        {   int upside_down;

            /* Hvals[] and Vvals[] are in final pixel coordinates */
            if (InfoPtr->Hvals[0] < InfoPtr->Hvals[1])
                PXprev = InfoPtr->Hvals[0] - 1;
            else
                PXprev = InfoPtr->Hvals[0] + 1;
            if (InfoPtr->bottom < InfoPtr->top) upside_down = FALSE;
            else                                upside_down = TRUE;

            for (col=0; col<InfoPtr->num_cols; col++)
            {   if (InfoPtr->num_bad > 0)   /* don't display bad traces */
                {   itemp = 0;
                    for (i=0; i<InfoPtr->num_bad; i++)
                    {   if ((InfoPtr->first_trace + col) == InfoPtr->bad_traces[i])
                        {   itemp = 1;
                            break;
                        }
                    }
                    if (itemp) continue;    /* skip this trace */
                }
                Px = InfoPtr->Hvals[col];
                if (Px < PX1)     continue; /* beyond left edge of viewport */
                if (Px > PX2)     continue; /* beyond right edge of viewport */
                if (Px == PXprev) continue; /* avoid redrawing over pixels */
                if (upside_down) PYprev = InfoPtr->Vvals[0] + 1;
                else             PYprev = InfoPtr->Vvals[0] - 1;
                for (row=0; row<InfoPtr->num_rows; row++)
                {   Py = InfoPtr->Vvals[row];
                    if (Py < PY1)     continue; /* beyond bottom of viewport */
                    if (Py > PY2)     continue; /* beyond top of viewport */
                    if (Py == PYprev) continue; /* avoid redrawing over pixels */
                    hpgl_pen_color = table[InfoPtr->image[row][col]];
                    plot_pixel(Px,Py);

                    /* fill in vertical gaps if any (linear interpolation) */
                    if (_ABS(Py - PYprev) > 1 && row > 0)
                    {   Pn = _ABS(Py - PYprev);
                        itemp = hpgl_array_8[v_rowy-Py][Px];
                        ival =  hpgl_array_8[v_rowy-PYprev][Px] - itemp;
                        for (i=1; i<Pn; i++)
                        {   nn = itemp + ((i * ival) / Pn);
                            hpgl_pen_color = nn;
                            if (upside_down)  plot_pixel(Px,Py+i);
                            else              plot_pixel(Px,Py-i);
                        }
                    }
                    PYprev = Py;
                }
                /* fill in horizontal gaps between traces if any; this
                   algorithm is essentially weighted sliding "|--|" pattern
                   with boundary condition checks.  This algorithm is OK if
                   the distance between columns is <= about 10.  Greater than
                   that and substantial artifacts appear and an inverse
                   distance or square distance weighting should be used.
                   Note that [x=0,y=0] in pixel space is the lower-left screen
                   corner but [row=0][col=0] for the global array
                   hpgl_array_8[][] is the upper left screen corner.
                 */
                if (_ABS(Px - PXprev) > 1) /* is there a gap between pixel columns? */
                {   /* Pnx is pixel difference between image columns;
                       Pny is the number of vertical pixels to go up and down
                         the image columns from Py, the row being interpolated.
                     */
                    Pnx = _ABS(Px - PXprev);
                    Pny = Pnx / 4;
                    Pn  = Pnx + Pny;
                    /* work through each pixel row between FILLED image columns */
                    if (upside_down)
                    {   PYtop    = InfoPtr->Vvals[0];
                        PYbottom = InfoPtr->Vvals[InfoPtr->num_rows-1];

                    } else
                    {   PYtop    = InfoPtr->Vvals[InfoPtr->num_rows-1];
                        PYbottom = InfoPtr->Vvals[0];
                    }
                    for (Py=PYbottom; Py<=PYtop; Py++)
                    {   /* stay within data viewport */
                        if (Py < PY1) continue; /* beyond bottom of viewport */
                        if (Py > PY2) break;    /* beyond top of viewport */
                        /* for each screen horizontal pixel in the row */
                        for (nx=1; nx<Pnx; nx++)
                        { /* Pn-nx-ny is the weight for left-side image column (PXprev)
                             Pny+nx-ny is the weight for right-side image column (Px) */
                            w1 = Pn - nx;
                            w2 = Pny + nx;
                            isum = w1 * hpgl_array_8[v_rowy-Py][PXprev] +
                                   w2 * hpgl_array_8[v_rowy-Py][Px];
                            icount = w1 + w2;
                            for (ny=1; ny<=Pny; ny++)
                            {   iw1 = w1 - ny;
                                iw2 = w2 - ny;
                                if (Py-ny >= PY1)   /* stay within viewport */
                                {   itemp = v_rowy - (Py - ny);
                                    isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                            iw2 * hpgl_array_8[itemp][Px];
                                    icount += iw2 + iw1;
                                }
                                if (Py+ny <= PY2)   /* stay within viewport */
                                {   itemp = v_rowy - (Py + ny);
                                    isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                            iw2 * hpgl_array_8[itemp][Px];
                                    icount += iw2 + iw1;
                                }
                            }
                            hpgl_pen_color = isum/icount;
                            if (PXprev < Px)  plot_pixel(PXprev+nx,Py);
                            else              plot_pixel(PXprev-nx,Py);
                        }   /* end of for (...nx <= Pnx...) loop */
                    }   /* end of for (...Py<=PYtop...) loop */
                }   /* end of if (_ABS(Px - PXprev) > 1) block */
                PXprev = Px;
            }   /* end of for (col < num_cols) loop */
            /* Add tick markers if requested */
            if (InfoPtr->show_markers)
            {   int j,k,num_pixels,reversed;
                unsigned char uc_val;

                if (InfoPtr->show_markers < 0) uc_val = 255;    /* white */
                else                           uc_val =   0;    /* black */
                num_pixels = _ABS(InfoPtr->show_markers);
                reversed = FALSE;
                if (InfoPtr->Vvals[0] < InfoPtr->Vvals[InfoPtr->num_rows]) reversed = TRUE;
                for (i=0; i<InfoPtr->num_ticks; i++)
                {   col = InfoPtr->tick_tracenum[i] - InfoPtr->first_trace;
                    if (InfoPtr->skip_traces > 0)
                        col /= (InfoPtr->skip_traces + 1); /* truncation expected */
                    if (col < 0 || col >= InfoPtr->num_cols) continue;
                    Px = InfoPtr->Hvals[col];
                    if (Px < PX1)  continue; /* beyond left edge of viewport */
                    if (Px > PX2)  continue; /* beyond right edge of viewport */
                    /* fill in vertically */
                    Py = InfoPtr->Vvals[0];
                    for (row=0; row<num_pixels; row++)
                    {   if (!reversed) Py--;      /* down 1 pixel */
                        else           Py++;      /* up 1 pixel */
                        if (Py < PY1)  continue;  /* beyond bottom of viewport */
                        if (Py > PY2)  continue;  /* beyond top of viewport */
                        hpgl_array_8[v_rowy-Py][Px] = uc_val;
                        /* expand in horizontally */
                        k = Px;
                        for (j=1; j<InfoPtr->show_marker_width; j++)
                        {   k++;                    /* increment one pixel */
                            if (k < PX1)  continue; /* beyond left edge of viewport */
                            if (k > PX2)  continue; /* beyond right edge of viewport */
                            hpgl_array_8[v_rowy-Py][k] = uc_val;
                        }
                    }
                }   /* end of for (i=0; i<InfoPtr->num_ticks; i++) loop */
            }   /* end of if (InfoPtr->show_markers) block */
            break;
        }
        case DISTANCE_DISTANCE:
        {   int upside_down,PYstart;

            /* Hvals[] are in final pixel coordinates */
            if (InfoPtr->Hvals[0] < InfoPtr->Hvals[1])
                PXprev = InfoPtr->Hvals[0] - 1;
            else
                PXprev = InfoPtr->Hvals[0] + 1;
            if (InfoPtr->bottom < InfoPtr->top) upside_down = FALSE;
            else                                upside_down = TRUE;

            if (InfoPtr->num_layers == 1) /* Vvals[] are user offsets from Zvals[] */
            {   for (col=0; col<InfoPtr->num_cols; col++)
                {   if (InfoPtr->num_bad > 0)   /* don't display bad traces */
                    {   itemp = 0;
                        for (i=0; i<InfoPtr->num_bad; i++)
                        {   if ((InfoPtr->first_trace + col) == InfoPtr->bad_traces[i])
                            {   itemp = 1;
                                break;
                            }
                        }
                        if (itemp) continue;    /* skip this trace */
                    }
                    Px = InfoPtr->Hvals[col];
                    if (Px < PX1)     continue; /* beyond left edge of viewport */
                    if (Px > PX2)     continue; /* beyond right edge of viewport */
                    if (Px == PXprev) continue; /* avoid redrawing over pixels */
                    PYstart = PY1 +
                              ( (InfoPtr->Zvals[col] - InfoPtr->bottom)
                                * pix_per_winY );
                    if (!upside_down)
                    {   PYprev   = PYstart + 1;
                        PYtop    = PYstart;
                        PYbottom = PYstart +
                            (InfoPtr->Vvals[InfoPtr->num_rows-1] * pix_per_winY);
                    } else
                    {   PYprev   = PYstart - 1;
                        PYtop    = PYstart +
                            (InfoPtr->Vvals[InfoPtr->num_rows-1] * pix_per_winY);
                        PYbottom = PYstart;
                    }
                    for (row=0; row<InfoPtr->num_rows; row++)
                    {   Py = PYstart + (InfoPtr->Vvals[row] * pix_per_winY);
                        if (Py < PY1)     continue; /* beyond bottom of viewport */
                        if (Py > PY2)     continue; /* beyond top of viewport */
                        if (Py == PYprev) continue; /* avoid redrawing over pixels */
                        hpgl_pen_color = table[InfoPtr->image[row][col]];
                        plot_pixel(Px,Py);

                        /* fill in vertical gaps if any (linear interpolation) */
                        if (_ABS(Py - PYprev) > 1 && row > 0)
                        {   Pn = _ABS(Py - PYprev);
                            itemp = hpgl_array_8[v_rowy-Py][Px];
                            ival =  hpgl_array_8[v_rowy-PYprev][Px] - itemp;
                            for (i=1; i<Pn; i++)
                            {   nn = itemp + ((i * ival) / Pn);
                                hpgl_pen_color = nn;
                                if (upside_down)  plot_pixel(Px,Py-i);
                                else              plot_pixel(Px,Py+i);
                            }
                        }
                        PYprev = Py;
                    }
                    /* fill in horizontal gaps between traces if any */
                    if (_ABS(Px - PXprev) > 1) /* is there a gap between pixel columns? */
                    {   /* Pnx is pixel difference between image columns;
                           Pny is the number of vertical pixels to go up and down
                             the image columns from Py, the row being interpolated.
                         */
                        Pnx = _ABS(Px - PXprev);
                        Pny = Pnx / 4;
                        Pn  = Pnx + Pny;

                        /* work through each pixel row between FILLED image columns */
                        for (Py=PYbottom; Py<=PYtop; Py++)
                        {   /* stay within data viewport */
                            if (Py < PY1) continue; /* beyond bottom of viewport */
                            if (Py > PY2) break;    /* beyond top of viewport */
                            /* for each screen horizontal pixel in the row */
                            for (nx=1; nx<Pnx; nx++)
                            { /* Pn-nx-ny is the weight for PXprev-side image column
                                 Pny+nx-ny is the weight for Px-side image column */
                                w1 = Pn - nx;
                                w2 = Pny + nx;
                                isum = w1 * hpgl_array_8[v_rowy-Py][PXprev] +
                                       w2 * hpgl_array_8[v_rowy-Py][Px];
                                icount = w1 + w2;
                                for (ny=1; ny<=Pny; ny++)
                                {   iw1 = w1 - ny;
                                    iw2 = w2 - ny;
                                    if (Py-ny >= PY1)   /* stay within viewport */
                                    {   itemp = v_rowy - (Py - ny);
                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                iw2 * hpgl_array_8[itemp][Px];
                                        icount += iw2 + iw1;
                                    }
                                    if (Py+ny <= PY2)   /* stay within viewport */
                                    {   itemp = v_rowy - (Py + ny);
                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                iw2 * hpgl_array_8[itemp][Px];
                                        icount += iw2 + iw1;
                                    }
                                }
                                hpgl_pen_color = isum/icount;
                                if (PXprev < Px)  plot_pixel(PXprev+nx,Py);
                                else              plot_pixel(PXprev-nx,Py);
                            }   /* end of for (...nx <= Pnx...) loop */
                        }   /* end of for (...Py<=PYtop...) loop */
                    }   /* end of if (_ABS(Px - PXprev) > 1) block */
                    PXprev = Px;
                }   /* end of for (col < num_cols) loop */
            } else
            {   /* multi-layer depth correction model */
                long   current_row;
                int    layer;
                double current_elev,bottom_elev;
                double samp_height;   /* meters per sample point */
                double layer_bottom[MAX_LAYERS];

                for (col=0; col<InfoPtr->num_cols; col++)
                {   if (InfoPtr->num_bad > 0)   /* don't display bad traces */
                    {   itemp = 0;
                        for (i=0; i<InfoPtr->num_bad; i++)
                        {   if ((InfoPtr->first_trace + col) == InfoPtr->bad_traces[i])
                            {   itemp = 1;
                                break;
                            }
                        }
                        if (itemp) continue;    /* skip this trace */
                    }
                    Px = InfoPtr->Hvals[col];
                    if (Px < PX1)     continue; /* beyond left edge of viewport */
                    if (Px > PX2)     continue; /* beyond right edge of viewport */
                    if (Px == PXprev) continue; /* avoid redrawing over pixels */
                    PYstart = PY1 +
                              ( (InfoPtr->Zvals[col] - InfoPtr->bottom)
                                * pix_per_winY );
                    if (!upside_down) PYprev = PYstart + 1;
                    else              PYprev = PYstart - 1;
/* if (col==0) puts(""); */
                    /* determine bottom of each layer */
                    for (layer=0; layer<InfoPtr->num_layers; layer++)
                    {   switch (InfoPtr->layer_mode[layer])
                        {   case 1:
                                layer_bottom[layer] = InfoPtr->Zvals[col] -
                                                    InfoPtr->layer_val[layer];
                                break;
                            case 2:
                                layer_bottom[layer] = InfoPtr->layer_val[layer];
                                break;
                            case 3:
                                layer_bottom[layer] = INVALID_VALUE;
                                break;
                            default:
                                free(table);
                                return 4;  /* unrecognized layer mode */
                        }
/* if (col==0) printf("layer_bottom #%d = %g\n",layer,layer_bottom[layer]); */
                    }
                    current_row = 0;
                    current_elev = InfoPtr->Zvals[col];
                    for (layer=0; layer<InfoPtr->num_layers; layer++)
                    {   switch (InfoPtr->layer_mode[layer])
                        {   case 1:  /* uniform depth below surface */
                                samp_height = (0.2998*InfoPtr->ns_per_samp)
                                          / (2*sqrt(InfoPtr->layer_rdp[layer]));
                                bottom_elev = layer_bottom[layer];
                                /* determine if a lower layer's bottom is above current layer's bottom */
                                for (i=layer; i<InfoPtr->num_layers; i++)
                                {   if ((i != layer) && (layer_bottom[i] != INVALID_VALUE))
                                    {   if (layer_bottom[i] > bottom_elev)
                                            bottom_elev = layer_bottom[i];
                                    }
                                }
/*if (col==0) printf("1:: layer %d: samp_height = %g bottom_elev = %g  current_elev = %g\n",
                  layer,samp_height,bottom_elev,current_elev);
if (col==0) printf("1:: current_row = %d ns_per_samp = %g sgrt(RDP)=%g\n",
                  current_row,InfoPtr->ns_per_samp,sqrt(InfoPtr->layer_rdp[layer]));
 */
                                for ( ; current_elev > bottom_elev;
                                     current_row++, current_elev -= samp_height)
                                {   if (current_row >= InfoPtr->num_rows) break;
                                    Py = PY1 +
                                         ( (current_elev - InfoPtr->bottom) *
                                            pix_per_winY );
                                    if (Py < PY1)     continue;
                                    if (Py > PY2)     continue;
                                    if (Py == PYprev) continue;
                                    hpgl_pen_color = table[InfoPtr->image[current_row][col]];
                                    plot_pixel(Px,Py);

                                    /* fill in vertical gaps if any (linear interpolation) */
                                    if (_ABS(Py - PYprev) > 1 && row > 0)
                                    {   Pn = _ABS(Py - PYprev);
                                        itemp = hpgl_array_8[v_rowy-Py][Px];
                                        ival =  hpgl_array_8[v_rowy-PYprev][Px] - itemp;
                                        for (i=1; i<Pn; i++)
                                        {   nn = itemp + ((i * ival) / Pn);
                                            hpgl_pen_color = nn;
                                            if (upside_down)  plot_pixel(Px,Py-i);
                                            else              plot_pixel(Px,Py+i);
                                        }
                                    }
                                    PYprev = Py;

                                    /* fill in horizontal gaps between traces if any */
                                    if (_ABS(Px - PXprev) > 1) /* is there a gap between pixel columns? */
                                    {   Pnx = _ABS(Px - PXprev);
                                        Pny = Pnx / 4;
                                        Pn  = Pnx + Pny;
                                        if (!upside_down)
                                        {   PYtop    = PYstart;
                                            PYbottom = PYprev;
                                        } else
                                        {   PYtop    = PYprev;
                                                PYbottom = PYstart;
                                        }
                                        for (Py=PYbottom; Py<=PYtop; Py++)
                                        {   /* stay within data viewport */
                                            if (Py < PY1) continue;
                                            if (Py > PY2) break;
                                            for (nx=1; nx<Pnx; nx++)
                                            {   w1 = Pn - nx;
                                                w2 = Pny + nx;
                                                isum = w1 * hpgl_array_8[v_rowy-Py][PXprev] +
                                                       w2 * hpgl_array_8[v_rowy-Py][Px];
                                                icount = w1 + w2;
                                                for (ny=1; ny<=Pny; ny++)
                                                {   iw1 = w1 - ny;
                                                    iw2 = w2 - ny;
                                                    if (Py-ny >= PY1)
                                                    {   itemp = v_rowy - (Py - ny);
                                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                                iw2 * hpgl_array_8[itemp][Px];
                                                        icount += iw2 + iw1;
                                                    }
                                                    if (Py+ny <= PY2)
                                                    {   itemp = v_rowy - (Py + ny);
                                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                                iw2 * hpgl_array_8[itemp][Px];
                                                        icount += iw2 + iw1;
                                                    }
                                                }
                                                hpgl_pen_color = isum/icount;
                                                if (PXprev < Px)  plot_pixel(PXprev+nx,Py);
                                                else              plot_pixel(PXprev-nx,Py);
                                            }   /* end of for (...n <= Pn...) loop */
                                        }   /* end of for (Py ...) loop */
                                    }   /* end of if ((PXprev+1) < Px) block */
                                }   /* end of for (;current_elev > bottom_elev;...) loop */
                                break;
                            case 2:  /* uniform elevation above sea level */
                                samp_height = (0.2998*InfoPtr->ns_per_samp)
                                              / (2*sqrt(InfoPtr->layer_rdp[layer]));
                                bottom_elev = layer_bottom[layer];
                                for (i=0; i<InfoPtr->num_layers; i++)
                                {   if ((i != layer) && (layer_bottom[i] != INVALID_VALUE))
                                    {   if (layer_bottom[i] > bottom_elev)
                                            bottom_elev = layer_bottom[i];
                                    }
                                }
/* if (col==0) printf("2:: layer %d: samp_height = %g bottom_elev = %g  current_elev = %g\n",
                  layer,samp_height,bottom_elev,current_elev);
if (col==0) printf("2:: current_row = %d ns_per_samp = %g sgrt(RDP)=%g\n",
                  current_row,InfoPtr->ns_per_samp,sqrt(InfoPtr->layer_rdp[layer]));
*/
                                for ( ; current_elev > bottom_elev;
                                     current_row++,current_elev-=samp_height)
                                {   if (current_row >= InfoPtr->num_rows) break;
                                    Py = PY1 +
                                        ( (current_elev - InfoPtr->bottom) *
                                           pix_per_winY );
                                    if (Py < PY1)     continue;
                                    if (Py > PY2)     continue;
                                    if (Py == PYprev) continue;
                                    hpgl_pen_color = table[InfoPtr->image[current_row][col]];
                                    plot_pixel(Px,Py);

                                    /* fill in vertical gaps if any (linear interpolation) */
                                    if (_ABS(Py - PYprev) > 1 && row > 0)
                                    {   Pn = _ABS(Py - PYprev);
                                        itemp = hpgl_array_8[v_rowy-Py][Px];
                                        ival =  hpgl_array_8[v_rowy-PYprev][Px] - itemp;
                                        for (i=1; i<Pn; i++)
                                        {   nn = itemp + ((i * ival) / Pn);
                                            hpgl_pen_color = nn;
                                            if (upside_down)  plot_pixel(Px,Py-i);
                                            else              plot_pixel(Px,Py+i);
                                        }
                                    }
                                    PYprev = Py;

                                    /* fill in horizontal gaps between traces if any */
                                    if (_ABS(Px - PXprev) > 1) /* is there a gap between pixel columns? */
                                    {   Pnx = _ABS(Px - PXprev);
                                        Pny = Pnx / 4;
                                        Pn  = Pnx + Pny;
                                        if (!upside_down)
                                        {   PYtop    = PYstart;
                                            PYbottom = PYprev;
                                        } else
                                        {   PYtop    = PYprev;
                                                PYbottom = PYstart;
                                        }
                                        for (Py=PYbottom; Py<=PYtop; Py++)
                                        {   /* stay within data viewport */
                                            if (Py < PY1) continue;
                                            if (Py > PY2) break;
                                            for (nx=1; nx<Pnx; nx++)
                                            {   w1 = Pn - nx;
                                                w2 = Pny + nx;
                                                isum = w1 * hpgl_array_8[v_rowy-Py][PXprev] +
                                                       w2 * hpgl_array_8[v_rowy-Py][Px];
                                                icount = w1 + w2;
                                                for (ny=1; ny<=Pny; ny++)
                                                {   iw1 = w1 - ny;
                                                    iw2 = w2 - ny;
                                                    if (Py-ny >= PY1)
                                                    {   itemp = v_rowy - (Py - ny);
                                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                                iw2 * hpgl_array_8[itemp][Px];
                                                        icount += iw2 + iw1;
                                                    }
                                                    if (Py+ny <= PY2)
                                                    {   itemp = v_rowy - (Py + ny);
                                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                                iw2 * hpgl_array_8[itemp][Px];
                                                        icount += iw2 + iw1;
                                                    }
                                                }
                                                hpgl_pen_color = isum/icount;
                                                if (PXprev < Px)  plot_pixel(PXprev+nx,Py);
                                                else              plot_pixel(PXprev-nx,Py);
                                            }   /* end of for (...n <= Pn...) loop */
                                        }   /* end of for (Py ...) loop */
                                    }   /* end of if ((PXprev+1) < Px) block */
                                }   /* end of for (;current_row < InfoPtr->num_rows; ...) loop */
                                break;
                            case 3:  /* only or lowest layer; no bottom */
                                samp_height = (0.2998*InfoPtr->ns_per_samp)
                                              / (2*sqrt(InfoPtr->layer_rdp[layer]));
/* if (col==0) printf("3:: layer %d: samp_height = %g bottom_elev = %g  current_elev = %g\n",
                  layer,samp_height,bottom_elev,current_elev);
if (col==0) printf("3:: current_row = %d ns_per_samp = %g sgrt(RDP)=%g\n",
                  current_row,InfoPtr->ns_per_samp,sqrt(InfoPtr->layer_rdp[layer]));
*/
                                for ( ; current_row < InfoPtr->num_rows;
                                     current_row++,current_elev-=samp_height)
                                {   Py = PY1 +
                                         ( (current_elev - InfoPtr->bottom) *
                                            pix_per_winY );
                                    if (Py < PY1)     continue;
                                    if (Py > PY2)     continue;
                                    if (Py == PYprev) continue;
                                    hpgl_pen_color = table[InfoPtr->image[current_row][col]];
                                    plot_pixel(Px,Py);

                                    /* fill in vertical gaps if any (linear interpolation) */
                                    if (_ABS(Py - PYprev) > 1 && row > 0)
                                    {   Pn = _ABS(Py - PYprev);
                                        itemp = hpgl_array_8[v_rowy-Py][Px];
                                        ival =  hpgl_array_8[v_rowy-PYprev][Px] - itemp;
                                        for (i=1; i<Pn; i++)
                                        {   nn = itemp + ((i * ival) / Pn);
                                            hpgl_pen_color = nn;
                                            if (upside_down)  plot_pixel(Px,Py-i);
                                            else              plot_pixel(Px,Py+i);
                                        }
                                    }
                                    PYprev = Py;

                                    /* fill in horizontal gaps between traces if any */
                                    if (_ABS(Px - PXprev) > 1) /* is there a gap between pixel columns? */
                                    {   Pnx = _ABS(Px - PXprev);
                                        Pny = Pnx / 4;
                                        Pn  = Pnx + Pny;
                                        if (!upside_down)
                                        {   PYtop    = PYstart;
                                            PYbottom = PYprev;
                                        } else
                                        {   PYtop    = PYprev;
                                                PYbottom = PYstart;
                                        }
                                        for (Py=PYbottom; Py<=PYtop; Py++)
                                        {   /* stay within data viewport */
                                            if (Py < PY1) continue;
                                            if (Py > PY2) break;
                                            for (nx=1; nx<Pnx; nx++)
                                            {   w1 = Pn - nx;
                                                w2 = Pny + nx;
                                                isum = w1 * hpgl_array_8[v_rowy-Py][PXprev] +
                                                       w2 * hpgl_array_8[v_rowy-Py][Px];
                                                icount = w1 + w2;
                                                for (ny=1; ny<=Pny; ny++)
                                                {   iw1 = w1 - ny;
                                                    iw2 = w2 - ny;
                                                    if (Py-ny >= PY1)
                                                    {   itemp = v_rowy - (Py - ny);
                                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                                iw2 * hpgl_array_8[itemp][Px];
                                                        icount += iw2 + iw1;
                                                    }
                                                    if (Py+ny <= PY2)
                                                    {   itemp = v_rowy - (Py + ny);
                                                        isum += iw1 * hpgl_array_8[itemp][PXprev] +
                                                                iw2 * hpgl_array_8[itemp][Px];
                                                        icount += iw2 + iw1;
                                                    }
                                                }
                                                hpgl_pen_color = isum/icount;
                                                if (PXprev < Px)  plot_pixel(PXprev+nx,Py);
                                                else              plot_pixel(PXprev-nx,Py);
                                            }   /* end of for (...n <= Pn...) loop */
                                        }   /* end of for (Py ...) loop */
                                    }   /* end of if ((PXprev+1) < Px) block */
                                }   /* end of for (;current_row < InfoPtr->num_rows;...) loop */
                            break;
                        }   /*  end of switch (InfoPtr->layer_mode[layer]) block */
                    }   /* end of for (...num_layers...) block */
                       PXprev = Px;
                }   /* end of for (...num_cols...) block */
            }   /* end of if (InfoPtr->num_layers == 1) else block */
            break;
        }   /* end of case 2: block */
    }   /* end of switch (InfoPtr->coord_mode) block */
/*
puts("press a key");
getch(); */
/***** Free storage *****/
    free(table);
    return 0;
}
/******************************* AbsValImage8u() ****************************/
/* This function replaces the trace amplitudes with their average values
 * multiplied by 2.0.
 *
 * Parameter list:
 *  int num_cols          - number of columns in the grid
 *  int num_rows          - number of rows in the grid
 *  unsigned char **image - pointer to 2D array, [num_rows][num_cols]
 *
 * Requires: "gpr_disp.h", "assertjl.h".
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Lakewood, CO
 * Date: September 3, 1999;
 */
void AbsValImage8u (long num_cols,long num_rows,unsigned char **image)
{
    long col,row,ltemp;          /* scratch variables */

    /* reality check */
    assert(num_cols > 0 && num_rows > 0 && image != NULL);

    for (col=0; col<num_cols; col++)
    {   for (row=0; row<num_rows; row++)
        {    ltemp = image[row][col];               /* 0 -> 255 */
             ltemp -= 128;                          /* -128 -> 127 */
             if (ltemp < 0) ltemp *= -1;            /* 0 -> 128 */
             ltemp *= 2;                            /* 0 -> 256 */
             image[row][col] = _LIMIT(0,ltemp,255); /* 0 -> 255 */
        }
    }

    return;
}
/******************************* SquareImage8u() ****************************/
/* This function replaces the trace amplitudes with their squared values
 * divide by 64.0.
 *
 * Parameter list:
 *  int num_cols          - number of columns in the grid
 *  int num_rows          - number of rows in the grid
 *  unsigned char **image - pointer to 2D array, [num_rows][num_cols]
 *
 * Requires: "gpr_disp.h", "assertjl.h".
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Lakewood, CO
 * Date: September 3, 1999;
 */
void SquareImage8u (long num_cols,long num_rows,unsigned char **image)
{
    long col,row,ltemp;          /* scratch variables */

    /* reality check */
    assert(num_cols > 0 && num_rows > 0 && image != NULL);

    for (col=0; col<num_cols; col++)
    {   for (row=0; row<num_rows; row++)
        {    ltemp = image[row][col];               /* 0 -> 255 */
             ltemp -= 128;                          /* -128 -> 127 */
             ltemp *= ltemp;                        /* 0 -> 16384 */
             ltemp /= 64;                           /* 0 -> 256 */
             image[row][col] = _LIMIT(0,ltemp,255); /* 0 -> 255 */
        }
    }

    return;
}
/****************************** InstAmpImage8u() ****************************/
/* This function replaces the trace amplitudes with their intantaneous
 * amplitudes determined from the Hilbert Transform and analytic signal. These
 * can optionally be replaced again with an envelope wrapping the positive
 * peaks.
 *
 * Parameter list:
 *  int num_cols          - number of columns in the grid
 *  int num_rows          - number of rows in the grid
 *  int envelope          - flag to wrap with envelope or not
 *  unsigned char **image - pointer to 2D array, [num_rows][num_cols]
 *
 * Requires: <math.h>, <stdlib.h>, "gpr_disp.h", "jl_util1.h", "assertjl.h".
 * Calls: assert, malloc, free, cos, sqrt, FFT1D, Envelope, Spline.
 * Returns: 0 on success, or
 *         >0 if error (offset int message strings array).
 *
const char *InstAmpImage8uMsg[] =
{   "InstAmpImage8u(): No errors.",
    "InstAmpImage8u() ERROR: Not able to allocate sufficient temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: July 15, 1999; August 6, 1999; August 19, 1999; August 25, 1999;
 */
int InstAmpImage8u (long num_cols,long num_rows,int envelope,unsigned char **image)
{
    int attrib,preproc;
    long i,col,row,itemp;        /* scratch variables */
    long num_fillers;            /* for FFT1D */
    long trace_length;           /* for FFT1D */
    double *d_trace = NULL;      /* [num_rows] for Envelope() */
    double *temp_trace = NULL;   /* [num_rows] for FFT1D() */
    double *cmplx_trace = NULL;  /* [2 *num_rows] for FFT1D() */
    double t,step,dtemp,dmean;   /* scratch variables */
    static int pwindow  = 24;    /* Hanning window used to preprocess */

    extern FILE *log_file;                /* "gpr_disp.h" */

    /* reality check */
    assert(num_cols > 0 && num_rows > 0 && image != NULL);

	/* Check if trace length is a power of 2, <= 65536 */
    num_fillers = 0;
    trace_length = num_rows;
    for (i=1; i<16; i++)
	{   if (num_rows > pow(2,(double)i) &&
	 		num_rows < pow(2,(double)(i+1)))
	 	{   trace_length = pow(2,(double)(i+1));
            num_fillers = trace_length - num_rows;
	 		break;
	 	}
    }
    preproc = TRUE;
    if (trace_length < pwindow) preproc = FALSE;

    /* Allocate storage and initialize to 0 */
    d_trace     = (double *)calloc(trace_length,sizeof(double));
    temp_trace  = (double *)calloc(trace_length,sizeof(double));
    cmplx_trace = (double *)calloc(2 * trace_length,sizeof(double));
    if (d_trace == NULL || temp_trace == NULL || cmplx_trace == NULL)
    {   free(d_trace);  free(temp_trace);  free(cmplx_trace);
        return 1;
    }

    /* Calculate instantaneous amps for every column (trace) in GPR image */
    for (col=0; col<num_cols; col++)
    {
        /* copy sample (8-bit) values to double array
           InstAttribTrace() will remove mean
         */
        for (row=0; row<num_rows; row++)
            temp_trace[row] = image[row][col];  /* 0 -> 255 */

        attrib = 1;
        InstAttribTrace(trace_length,preproc,attrib,temp_trace,cmplx_trace);

        /* wrap transformed trace with envelope if requested */
        if (envelope)
        {   itemp = Envelope(num_rows,temp_trace,d_trace);
            if (itemp > 0)
            {   free(d_trace);  free(cmplx_trace);  free(temp_trace);
                if (log_file)
                {   fprintf(log_file,"Problem wrapping trace with an envelope. Program proceeded with no wrap.\n");
                }
            } else
            {   /* copy envelope values back into original array */
                for (row=0; row<num_rows; row++)
                {   temp_trace[row] = d_trace[row];
                }
            }
        }

        /* copy transformed values back into image *
           NOTE: only pos values so don't need to add dmean back in but can rescale
                 by sqrt(2).
        */
        for (row=0; row<num_rows; row++)
        {   temp_trace[row] *= sqrt(2.0);
            if (temp_trace[row] <   0.0) temp_trace[row] =   0.0;
            if (temp_trace[row] > 255.0) temp_trace[row] = 255.0;
            image[row][col] = temp_trace[row];
        }
    }   /* end of for (col=0; col<num_cols; col++) */

    /* Free storage and return success */
    free(d_trace);  free(cmplx_trace);  free(temp_trace);

    return 0;
}
/****************************** InstPowImage8u() ****************************/
/* This function replaces the trace amplitudes with their intantaneous
 * power determined from the Hilbert Transform and analytic signal. These
 * can optionally be replaced again with an envelope wrapping the positive
 * peaks.
 *
 * Parameter list:
 *  int num_cols          - number of columns in the grid
 *  int num_rows          - number of rows in the grid
 *  int envelope          - flag to wrap with envelope or not
 *  unsigned char **image - pointer to 2D array, [num_rows][num_cols]
 *
 * Requires: <math.h>, <stdlib.h>, "gpr_disp.h", "jl_util1.h", "assertjl.h".
 * Calls: assert, malloc, free, cos, sqrt, FFT1D, Envelope, Spline.
 * Returns: 0 on success, or
 *         >0 if error (offset int message strings array).
 *
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: August 23, 1999; September 2, 1999;
 */
int InstPowImage8u (long num_cols,long num_rows,int envelope,unsigned char **image)
{
    int attrib,preproc;
    long i,col,row,itemp;        /* scratch variables */
    long num_fillers;            /* for FFT1D */
    long trace_length;           /* for FFT1D */
    double *d_trace = NULL;      /* [num_rows] for Envelope() */
    double *temp_trace = NULL;   /* [num_rows] for FFT1D() */
    double *cmplx_trace = NULL;  /* [2 *num_rows] for FFT1D() */
    double t,step,dtemp,dmean;   /* scratch variables */
    static int pwindow  = 24;    /* Hanning window used to preprocess */

    extern FILE *log_file;                /* "gpr_disp.h" */

    /* reality check */
    assert(num_cols > 0 && num_rows > 0 && image != NULL);

	/* Check if trace length is a power of 2, <= 65536 */
    num_fillers = 0;
    trace_length = num_rows;
    for (i=1; i<16; i++)
	{   if (num_rows > pow(2,(double)i) &&
	 		num_rows < pow(2,(double)(i+1)))
	 	{   trace_length = pow(2,(double)(i+1));
            num_fillers = trace_length - num_rows;
	 		break;
	 	}
    }
    preproc = TRUE;
    if (trace_length < pwindow) preproc = FALSE;

    /* Allocate storage and initialize to 0 */
    d_trace     = (double *)calloc(trace_length,sizeof(double));
    temp_trace  = (double *)calloc(trace_length,sizeof(double));
    cmplx_trace = (double *)calloc(2 * trace_length,sizeof(double));
    if (d_trace == NULL || temp_trace == NULL || cmplx_trace == NULL)
    {   free(d_trace);  free(temp_trace);  free(cmplx_trace);
        return 1;
    }

    /* Calculate instantaneous amps for every column (trace) in GPR image */
    for (col=0; col<num_cols; col++)
    {
        /* copy sample (8-bit) values to double array
           InstAttribTrace() will remove mean
         */
        for (row=0; row<num_rows; row++)
            temp_trace[row] = image[row][col];  /* 0 -> 255 */

        attrib = 2;
        InstAttribTrace(trace_length,preproc,attrib,temp_trace,cmplx_trace);

        /* wrap transformed trace with envelope if requested */
        if (envelope)
        {   itemp = Envelope(num_rows,temp_trace,d_trace);
            if (itemp > 0)
            {   free(d_trace);  free(cmplx_trace);  free(temp_trace);
                if (log_file)
                {   fprintf(log_file,"Problem wrapping trace with an envelope. Program proceeded with no wrap.\n");
                }
            } else
            {   /* copy envelope values back into original array */
                for (row=0; row<num_rows; row++)
                {   temp_trace[row] = d_trace[row];
                }
            }
        }

        /* copy transformed values back into image *
           NOTE: only pos values so don't need to add dmean back in but can rescale
                 by /= 128.0.
        */
        for (row=0; row<num_rows; row++)
        {   temp_trace[row] /= 128.0;
            if (temp_trace[row] <   0.0) temp_trace[row] =   0.0;
            if (temp_trace[row] > 255.0) temp_trace[row] = 255.0;
            image[row][col] = temp_trace[row];
        }
    }   /* end of for (col=0; col<num_cols; col++) */

    /* Free storage and return success */
    free(d_trace);  free(cmplx_trace);  free(temp_trace);

    return 0;
}
/******************************* Envelope() ********************************/
/* Wrap the data series with an envelope that connects th positive highs.
 *
 * Parameters:
 *     int  n     - number of members in data array and envelope array
 *   double *a  - pointer to array of data values
 *   double *b  - pointer to array for envelope values
 *
 * NOTE: Storage for arrays pointed to in parameter list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <stdio.h>, "gprslice.h"
 * Calls: Spline
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
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
/**************************** HistStretchImage8u() **************************/
/* This function uses a histogram to stretch an image and enhance contrasts.
 * It will only work with 8-bit (unsigned char) data.  It will work properly
 * only if the data is mono-modal with small standard of deviation (i.e.
 * values are clustered about a central value -- which is of course why we
 * would want to enhance the contrasts).
 *
 * Parameter list:
 *  int num_cols          - number of columns in the grid
 *  int num_rows          - number of rows in the grid
 *  int percent           - percent (1 to 99) to stretch grid
 *  unsigned char **image - pointer to 2D array, [rows][cols]
 *
 *                            HISTOGRAM
 *                             +  +  +   -- histmode
 *                         +      *
 *    streched data -- +         *  *         +
 *         |       +            *    *           +
 *         |   +               *       *              +
 *         +        original-- *        *                  +
 *      +             data    *          * ----------- (histmode/100) * percent
 * +                      * * *           * * *
 * ---------------------------|---+------|-----------------------------
 * 0                  hist_min  hist_mid  hist_max
 *                            |___|______|
 *                             hist_del
 *
 *
 *
 * Algorithm:
 * [i] = histogram index
 * [j] = old image value
 * interp_i = the real-valued interpolated "old" index (between 2 old indices)
 * hist_mid = the old-index location of the maximum value in the histogram
 * hist_min = the old-index location of the value that is p% of x[hist_mid]
 * hist_max = the old-index location of the value that is p% of x[hist_mid]
 * hist_del = hist_mid - hist_min -or- hist_max - hist_mid
 *
 * for j=0 -> hist_mid-1
 *   new_val  = INT(hist_min + (j-0) * (hist_del/(hist_mid-0)))
 *
 * for j=hist_mid+1 -> 254
 *   new_val = INT(hist_min + (j-hist_mid) * (hist_del/(255-hist_mid)))
 *
 * Requires: <stdlib.h>, "gpr_disp.h".
 * Calls: calloc, free, _BETWEEN.
 * Returns: 0 on success, or
 *         >0 if error (offset int message strings array).
 *
const char *HistStretchImage8uMsg[] =
{   "HistStretchImage8u(): No errors.",
    "HistStretchImage8u() ERROR: Percent is outside of valid (1 - 99) range.",
    "HistStretchImage8u() ERROR: Not able to allocate sufficient temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Lakewood, CO
 * Date: September 1, 1999
 */
int HistStretchImage8u (long num_cols,long num_rows,int percent,unsigned char **image)
{
    long i,col,row,ref_i,ref_ip1;
    int *hst,*hist;
    int hist_min,hist_max,histmode,hist_mid;
    double dtemp,oldval,newval,hist_del_lo,hist_del_hi;

    /* Reality check */
    if (percent<1 || percent>99) return 1;

    /* Allocate storage and initialize to 0 */
    hst  = (int *)calloc(256,sizeof(int));
    hist = (int *)calloc(256,sizeof(int));
    if (hst==NULL || hist==NULL)
    {   free(hst); free(hist);
        return 2;
    }

    /* Compile histogram */
    for (row=0; row<num_rows; row++)
    {   for (col=0; col<num_cols; col++)
            hst[(image[row][col])]++;
    }

    /* Smooth histogram */
    hist[0] = (hst[0]+hst[1]+hst[2])/3;
    hist[1] = (hst[0]+hst[1]+hst[2]+hst[3])/4;
    for (i=2; i<=253 ; i++)
        hist[i] = (hst[i-2]+hst[i-1]+hst[i]+hst[i+1]+hst[i+2])/5;
    hist[254] = (hst[252]+hst[253]+hst[254]+hst[255])/4;
    hist[255] = (hst[253]+hst[254]+hst[255])/3;

    /* Find mode of histogram */
    hist_mid = 128;
    histmode = 0;
    for (i=0; i<=255; i++)
    {   if (hist[i] > histmode)
        {   histmode = hist[i];
            hist_mid = i;
        }
    }

    /* Find % stretch points in histogram */
    dtemp = histmode * percent / 100.0;

    for (i=hist_mid; i>=0; i--)
    {   if (hist[i] <= dtemp)
        {   hist_min = i;
            break;
        }
    }
    hist_del_lo = hist_mid - hist_min;
    if (hist_del_lo <= 0) hist_del_lo = 1;

    for (i=hist_mid; i<255; i++)
    {   if (hist[i] <= dtemp)
        {   hist_max = i;
            break;
        }
    }
    hist_del_hi = hist_max - hist_mid;
    if (hist_del_hi <= 0) hist_del_hi = 1;

    /* Contrast stretch grid */
    for (row=0; row<num_rows; row++)
    {   for (col=0; col<num_cols; col++)
        {   newval = oldval = image[row][col];   /* fail safe */
            if (oldval == 0)
            {   newval = hist_min;
            } else if (oldval > 0 && oldval < hist_mid)
            {   newval = hist_min + ( (oldval-0) * hist_del_lo / (hist_mid-0) );
            } else if (oldval == hist_mid)
            {   newval = hist_mid;
            } else if (oldval > hist_mid && oldval < 255)
            {   newval = hist_mid + ( (oldval-hist_mid) * hist_del_hi / (255-hist_mid) );
            } else if (oldval == 255)
            {   newval = hist_max;
            }
            image[row][col] = _LIMIT(0.0,newval,255.0);
        }
#if 0    /* GRO method */
        {   itemp = _BETWEEN(0,(255*((int)image[row][col]-histmn))/histdel,255);
            image[row][col] = (unsigned char)(itemp);
        }
#endif
    }

    /* Free storage and return success */
    free(hst);  free(hist);
    return 0;
}
/**************************** ChangeRangeGain8u() ***************************/
/* This function changes the gain applied to radar traces.
 *
 * Parameters:
 *  long num_cols     - number of columns in image
 *  long num_rows     - number of rows in image
 *  int  num_gain_off - size of gain_off[]; 0 or >=2
 *  double *gain_off  - set of gain to be removed in decibels
 *  int  num_gain_on  - size of gain_on[]; 0 or >=2
 *  double *gain_on   - set of gain to be added in decibels
 *  unsigned char **image  - [num_rows][num_cols]; pointer to 2D image
 *
 * Requires: <math.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h".
 * Calls: malloc, free, pow, printf, puts.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
const char *ChangeRangeGain8uMsg[] =
{   "ChangeRangeGain8u(): No errors.",
    "ChangeRangeGain8u() ERROR: Invalid number of columns or rows.",
    "ChangeRangeGain8u() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
    "ChangeRangeGain8u() ERROR: NULL pointer passed to function.",
    "ChangeRangeGain8u() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 28, 1995
 */
int ChangeRangeGain8u (long num_cols,long num_rows,int num_gain_off,
                     double *gain_off,int num_gain_on,double *gain_on,
                     unsigned char **image)
{
    int i,gain1,gain2,first_pt,last_pt,col,row;
    double *gain_off_func,*gain_on_func,dtemp,gain_off_inc,gain_on_inc;
    extern int Debug;

    /* Check function parameters */
    if (num_cols < 1 || num_rows < 2) return 1;
    if (num_gain_off < 0 || num_gain_on < 0) return 2;
    if (num_gain_off == 1 || num_gain_on == 1) return 2;
    if (image == NULL) return 3;
    if (Debug)
    {   puts("ChangeRangeGain8u() Debug)");
        printf("num_cols=%ld  num_rows=%ld num_gain_off=%d  num_gain_on=%d\n",
                 num_cols,num_rows,num_gain_off,num_gain_on);
    }

    /* Allocate storage for gain functions */
    gain_off_func = (double *)malloc(num_rows * sizeof(double));
    if (gain_off_func == NULL) return 4;
    gain_on_func = (double *)malloc(num_rows * sizeof(double));
    if (gain_on_func == NULL)
    {   free(gain_off_func);
        return 4;
    }

    /* Initialize gain functions to 0.0 */
    for (i=0; i<num_rows; i++)  gain_off_func[i] = gain_on_func[i] = 0.0;

    /* Calculate number of samples (rows) between break points */
    gain_off_inc = gain_on_inc = 0.0;
    if (num_gain_off >= 2)
    {   if (gain_off == NULL)
        {   free(gain_off_func);  free(gain_on_func);
            return 3;
        }
        gain_off_inc = (double)(num_rows-1) / (num_gain_off - 1);
    }
    if (num_gain_on >= 2)
    {   if (gain_on == NULL)
        {   free(gain_off_func);  free(gain_on_func);
            return 3;
        }
        gain_on_inc  = (double)(num_rows-1) / (num_gain_on - 1);
    }
    if (Debug)
    {   printf("gain_off_inc = %g  gain_on_inc = %g\n",gain_off_inc,gain_on_inc);
        getch();
    }

    /* Calculate gain functions (linearly iterpolate between break points) */
    if (gain_off_inc > 0.0)
    {   for (gain1=0; gain1<num_gain_off-1; gain1++)
        {   gain2    = gain1 + 1;
            first_pt = gain1 * gain_off_inc;  /* truncation expected */
            last_pt  = gain2 * gain_off_inc;  /* truncation expected */
            dtemp    = gain_off[gain2] - gain_off[gain1];
            for (i=first_pt; i<last_pt && i<num_rows; i++)
            {   gain_off_func[i] = gain_off[gain1] + (dtemp*(i-first_pt) / gain_off_inc);
            }
        }
        gain_off_func[0] = gain_off[0];
        gain_off_func[num_rows-1] = gain_off[num_gain_off-1];
    }

    if (gain_on_inc > 0.0)
    {   for (gain1=0; gain1<num_gain_on-1; gain1++)
        {   gain2    = gain1 + 1;
            first_pt = gain1 * gain_on_inc;   /* truncation expected */
            last_pt  = gain2 * gain_on_inc;   /* truncation expected */
            dtemp    = gain_on[gain2] - gain_on[gain1];
            for (i=first_pt; i<last_pt && i<num_rows; i++)
            {   gain_on_func[i] = gain_on[gain1] + (dtemp*(i-first_pt) / gain_on_inc);
            }
        }
        gain_on_func[0] = gain_on[0];
        gain_on_func[num_rows-1] = gain_on[num_gain_on-1];
    }

    /* Change gain */
    for (col=0; col<num_cols; col++)
    {   for (row=0; row<num_rows; row++)
        {   dtemp  = image[row][col];
            dtemp -= 128.0;
            dtemp *= pow(10.,(.05*(gain_on_func[row]-gain_off_func[row])));
            dtemp += 128.0;
            dtemp  = _LIMIT(0.0,dtemp,255.0);
if (Debug && col==0) printf("old=%u new=%g\n,",(unsigned)image[row][col],dtemp);
            image[row][col] = dtemp;
        }
    }

    free(gain_on_func);
    free(gain_off_func);
    return 0;
}
/***************************** RemAvgColumn8u() *****************************/
/* This function adds all the columns in the image together then divides by
 * number of columns to get an average columns (trace), which can be
 * considered the "background".
 * The average column is normalized so that it is centered about zero then
 * subtracted from each column in the image.
 *
 * Parameters:
 *  long num_cols - number of columns in grid
 *  long num_rows - number of rows in grid
 *  unsigned char **image  - [num_rows][num_cols]; pointer to 2D image
 *
 * Requires: <assert.h>.
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: June 23, 1994
 */
void RemAvgColumn8u (long num_cols,long num_rows,unsigned char **image)
{
    long col,row;
    double dtemp,avg;

    assert(num_cols > 0 && num_rows > 0 && image != NULL);
    for (row=0; row<num_rows; row++)
    {   avg = 0.0;
        /* add up all the values for one row */
        for (col=0; col<num_cols; col++)  avg += image[row][col];
        /* get average value for that row */
        avg /= (double)num_cols;
        /* set "central" value to 0 */
        avg -= 128.0;
        /* subtract average value from the row in the image */
        for (col=0; col<num_cols; col++)
        {   dtemp = (double)image[row][col] - avg;
            if      (dtemp <   0.0) image[row][col] = 0;
            else if (dtemp > 255.0) image[row][col] = 255;
            else                    image[row][col] = dtemp;
        }
    }
}
/***************************** DisplayWiggle8() *****************************/
/* This function "displays" or "copies" a grid of unsigned bytes values to
 * hpgl_array_8[][] as wigle traces.
 *
 * Parameter list:
 *  int vrows       - number of CRT vertical pixels
 *  struct DispParamInfoStruct *InfoPtr - address of information structure
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *       This function inherits the graphics mode, CRT viewport, user window,
 *         and color/grayscale palette from the calling routine.
 *       Plotting is 2D only.
 *
 * Requires: <hpgl.h>, <stdlib.h>, "gpr_disp.h".
 * Calls: malloc, free, pen, plot, InitTable8, HistStretchImage8u,
 *        RemAvgColumn8u, LocalStretchTable8, BrightenTable8, ReverseTable8,
 *        SquareImage8u, ChangeRangeGain8u, AbsValImage8u.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
 *
const char *DisplayWiggle8Msg[] =
{   "DisplayWiggle8(): No errors.",
    "DisplayWiggle8() ERROR: One or more positioning arrays are NULL.",
    "DisplayWiggle8() ERROR: Not able to allocate sufficient temporary storage.",
    "DisplayWiggle8() ERROR: Unrecognized coordinate mode.",
    "DisplayWiggle8() ERROR: Unrecognized layer model mode.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: December 12, 1995; September 3, 1999;
 */
int DisplayWiggle8 (int vrows,struct DispParamInfoStruct *InfoPtr)
{
    int pen_color;                 /* pen color */
    int i,col,row;                 /* counters */
    int ival,itemp;                /* scratch variables */
    int tablesize   = 256;         /* size of table[] */
    int *table = NULL;             /* [tablesize]; lookup table for grayscale */
    int fill_neg,fill_pos,reversed;
    double x_width,x_left,x_mid,x,y;        /* to draw trace */
    double clip_left,clip_right,clip_width;
    double fill_x,fill_y,dy,x_prev,y_prev;  /* to fill trace */
    double VYMAX,PY1,PY2;

    extern int Debug;                     /* "gpr_disp.h" */
    extern FILE *log_file;                /* "gpr_disp.h" */

/***** Validate some parameters *****/
    if ( InfoPtr->Hvals == NULL ||
        (InfoPtr->Vvals == NULL && InfoPtr->Zvals == NULL)
       )  return 1;

    switch (InfoPtr->coord_mode)
    {   case TRACE_TIME:  case TIME_TIME:  case DISTANCE_TIME: case TRACE_SAMPLE:
            /* For cases 1, 3, 4, and 5, Vvals[] represents a position within the
               user window. (The top is "horizontal" or flat.) */
            break;
        case DISTANCE_DISTANCE:
            /* For case 2, the positions of the samples within the user window
               must be calcuted seperately for each trace (column).  The top
               is not flat.  If num_layers == 1, Vvals[] hold the offset in
               user units for each sample from the surface (Zvals[]);
               + above surface, - below.  For multi-layer models, Vvals[] is
               not used. */
            break;
        default:
            return 3;
    }

/***** Allocate storage for work arrays *****/
    table = (int *)malloc(tablesize*sizeof(int));
    if (table == NULL)  return 2;


/***** Initialize lookup table for grayscale display *****/
    InitTable8(table,tablesize,InfoPtr->range,InfoPtr->epc_threshold,
              InfoPtr->epc_contrast,InfoPtr->epc_sign,InfoPtr->epc_gain);

/***** Perform image processing functions *****/
    if (InfoPtr->change_gain)
    {   int itemp;
        extern const char *ChangeRangeGain8uMsg[];

        itemp = ChangeRangeGain8u(InfoPtr->num_cols,InfoPtr->num_rows,
                                InfoPtr->num_gain_off,InfoPtr->gain_off,
                                InfoPtr->num_gain_on,InfoPtr->gain_on,
                                InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",ChangeRangeGain8uMsg[itemp]);
        }
    }
    /* Remove a "background trace" if requested */
    if (InfoPtr->background)
        RemAvgColumn8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->image);

    /* Take absolute value of amplitude and multiply by 2 */
    if (InfoPtr->abs_val)
        AbsValImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->image);

    /* Square amplitudes and divide by 64 */
    if (InfoPtr->square)
        SquareImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->image);

    /* Replace amps with instantaneous amplitude and wrap with envelope */
    if (InfoPtr->inst_amp)
    {   int itemp;
        extern const char *InstAmpImage8uMsg[];

        itemp = InstAmpImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->envelope,InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",InstAmpImage8uMsg[itemp]);
        }
    }

    /* Replace amps with instantaneous power and wrap with envelope */
    if (InfoPtr->inst_pow)
    {   int itemp;
        double dtemp;
        extern const char *InstPowImage8uMsg[];

        itemp = InstPowImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->envelope,InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",InstPowImage8uMsg[itemp]);
        }
    }


    /* Enhance contrast of image if requested */
    if (InfoPtr->stretch)
    {   int itemp;
        extern const char *HistStretchImage8uMsg[];

        itemp = HistStretchImage8u(InfoPtr->num_cols,InfoPtr->num_rows,InfoPtr->stretch,InfoPtr->image);
        if (itemp > 0)
        {   if (log_file)  fprintf(log_file,"%s\n",HistStretchImage8uMsg[itemp]);
        }
    }

    /* Enhance local contrast of gray scale if requested */
    if (InfoPtr->local_stretch)
        LocalStretchTable8(table,tablesize,InfoPtr->local_offset,InfoPtr->local_change);

    /* Brighten or darken gray scale if requested */
    if (InfoPtr->brightness)
        BrightenTable8(table,tablesize,InfoPtr->brightness);

    /* Reverse gray scale if requested */
    if (InfoPtr->negative)
        ReverseTable8(table,tablesize);

/***** Display data as wiggle traces *****/
    /* Calculate jump distance for fill operation */
    /* In THIS function, pixel space (0,0) is lower left corner of screen */
    /* VX2Y_ratio = (double)vcols/(double)vrows; */
    VYMAX = 100.0;
    /* VXMAX = VYMAX * VX2Y_ratio; */
    PY1 = (InfoPtr->VY1/VYMAX)*(vrows-1);
    PY2 = (InfoPtr->VY2/VYMAX)*(vrows-1);
    dy = _ABS((InfoPtr->top - InfoPtr->bottom) / (PY2 - PY1));

    /* Calculate wiggle trace widths */
    x_width = (InfoPtr->right - InfoPtr->left) * (InfoPtr->wiggle_width/100.0);
    clip_width = (InfoPtr->wiggle_clip/100.0) * x_width;
    fill_neg = fill_pos = FALSE;
    if (InfoPtr->wiggle_fill < 0)  fill_neg = TRUE;
    if (InfoPtr->wiggle_fill > 0)  fill_pos = TRUE;
    reversed = FALSE;
    if (InfoPtr->left > InfoPtr->right) reversed = TRUE;
    if (Debug)
    {   puts("DisplayWiggle8() in debug mode: no plot() etc.");
        printf("x_width = %g  clip_width = %g\n",x_width,clip_width);
        free(table);
        return 0;
    }

    viewport(InfoPtr->VX1,InfoPtr->VX2,InfoPtr->VY1,InfoPtr->VY2);
    window(InfoPtr->left,InfoPtr->right,InfoPtr->bottom,InfoPtr->top);
    clip();
    pen_color = InfoPtr->wiggle_color;
    pen(pen_color);
    linetype(0);

    switch (InfoPtr->coord_mode)
    {   case TRACE_TIME:  case TIME_TIME:  case DISTANCE_TIME: case TRACE_SAMPLE:
        {
            for (col=0; col<InfoPtr->num_cols; col++)
            {   if (InfoPtr->num_bad > 0)   /* don't display bad traces */
                {   itemp = 0;
                    for (i=0; i<InfoPtr->num_bad; i++)
                    {   if ((InfoPtr->first_trace + col) == InfoPtr->bad_traces[i])
                        {   itemp = 1;
                            break;
                        }
                    }
                    if (itemp) continue;    /* skip this trace */
                }
                x_mid = InfoPtr->Hvals[col];   /* middle of wiggle area */
                x_left = x_mid - (x_width/2.0);  /* left edge of wiggle area */
                clip_left = x_mid - (clip_width/2.0);
                clip_right = x_mid + (clip_width/2.0);
                row = 0;
                y = InfoPtr->Vvals[row];
                ival = table[InfoPtr->image[row][col]];  /* gray value */
                x = x_left + (double)(ival * x_width)/255.0;
                if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                else           x = _LIMIT(clip_right,x,clip_left);
                x_prev = x;
                y_prev = y;
                for (row=1; row<InfoPtr->num_rows; row++)
                {   y = InfoPtr->Vvals[row];
                    ival = table[InfoPtr->image[row][col]];  /* gray value */
                    x = x_left + (double)(ival * x_width)/255.0;
                    if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                    else           x = _LIMIT(clip_right,x,clip_left);
                    plot(x_prev,y_prev,-2);
                    plot(x,y,-1);

                    /* fill in to x_mid if requested */
                    if (fill_neg && x < x_mid)
                    {   if (y < y_prev)
                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                 if (fill_x >= x_mid) continue;
                                 plot(fill_x,fill_y,-2);
                                 plot(x_mid,fill_y,-1);
                            }
                        }
                        if (y > y_prev)
                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                 if (fill_x >= x_mid) continue;
                                 plot(fill_x,fill_y,-2);
                                 plot(x_mid,fill_y,-1);
                            }
                        }
                    }
                    if (fill_pos && x > x_mid)
                    {   if (y < y_prev)
                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                 if (fill_x <= x_mid) continue;
                                 plot(fill_x,fill_y,-2);
                                 plot(x_mid,fill_y,-1);
                            }
                        }
                        if (y > y_prev)
                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                 if (fill_x <= x_mid) continue;
                                 plot(fill_x,fill_y,-2);
                                 plot(x_mid,fill_y,-1);
                            }
                        }
                    }
                    x_prev = x;
                    y_prev = y;
                }   /* end of for (row=1; row<InfoPtr->num_rows; row++) loop */
            }   /* end of for (col=0; col<InfoPtr->num_cols; col++) loop */
            break;
        }
        case DISTANCE_DISTANCE:
        {   double y_start;

            if (InfoPtr->num_layers == 1) /* Vvals[] are user offsets from Zvals[] */
            {   for (col=0; col<InfoPtr->num_cols; col++)
                {   if (InfoPtr->num_bad > 0)   /* don't display bad traces */
                    {   itemp = 0;
                        for (i=0; i<InfoPtr->num_bad; i++)
                        {   if ((InfoPtr->first_trace + col) == InfoPtr->bad_traces[i])
                            {   itemp = 1;
                                break;
                            }
                        }
                        if (itemp) continue;    /* skip this trace */
                    }
                    x_mid = InfoPtr->Hvals[col];
                    x_left = x_mid - (x_width/2.0);  /* left edge of wiggle area */
                    clip_left = x_mid - (clip_width/2.0);
                    clip_right = x_mid + (clip_width/2.0);
                    y_start = InfoPtr->Zvals[col];
                    row = 0;
                    y = y_start + InfoPtr->Vvals[row];
                    ival = table[InfoPtr->image[row][col]];  /* gray value */
                    x = x_left + (double)(ival * x_width)/255.0;
                    if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                    else           x = _LIMIT(clip_right,x,clip_left);
                    x_prev = x;
                    y_prev = y;
                    for (row=1; row<InfoPtr->num_rows; row++)
                    {   y = y_start + InfoPtr->Vvals[row];
                        ival = table[InfoPtr->image[row][col]];  /* gray value */
                        x = x_left + (double)(ival * x_width)/255.0;
                        if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                        else           x = _LIMIT(clip_right,x,clip_left);
                        plot(x_prev,y_prev,-2);
                        plot(x,y,-1);

                        /* fill in to x_mid if requested */
                        if (fill_neg && x < x_mid)
                        {   if (y < y_prev)
                            {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                     if (fill_x >= x_mid) continue;
                                     plot(fill_x,fill_y,-2);
                                     plot(x_mid,fill_y,-1);
                                }
                            }
                            if (y > y_prev)
                            {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                     if (fill_x >= x_mid) continue;
                                     plot(fill_x,fill_y,-2);
                                     plot(x_mid,fill_y,-1);
                                }
                            }
                        }
                        if (fill_pos && x > x_mid)
                        {   if (y < y_prev)
                            {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                     if (fill_x <= x_mid) continue;
                                     plot(fill_x,fill_y,-2);
                                     plot(x_mid,fill_y,-1);
                                }
                            }
                            if (y > y_prev)
                            {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                     if (fill_x <= x_mid) continue;
                                     plot(fill_x,fill_y,-2);
                                     plot(x_mid,fill_y,-1);
                                }
                            }
                        }
                        x_prev = x;
                        y_prev = y;
                    }
                }   /* end of for (col < num_cols) loop */
            } else
            {   /* multi-layer depth correction model */
                long   current_row;
                int    layer;
                double current_elev,bottom_elev;
                double samp_height;   /* meters per sample point */
                double layer_bottom[MAX_LAYERS];

                for (col=0; col<InfoPtr->num_cols; col++)
                {   if (InfoPtr->num_bad > 0)   /* don't display bad traces */
                    {   itemp = 0;
                        for (i=0; i<InfoPtr->num_bad; i++)
                        {   if ((InfoPtr->first_trace + col) == InfoPtr->bad_traces[i])
                            {   itemp = 1;
                                break;
                            }
                        }
                        if (itemp) continue;    /* skip this trace */
                    }
                    x_mid = InfoPtr->Hvals[col];
                    x_left = x_mid - (x_width/2.0);  /* left edge of wiggle area */
                    clip_left = x_mid - (clip_width/2.0);
                    clip_right = x_mid + (clip_width/2.0);
                    y_start = InfoPtr->Zvals[col];
                    /* determine bottom of each layer */
                    for (layer=0; layer<InfoPtr->num_layers; layer++)
                    {   switch (InfoPtr->layer_mode[layer])
                        {   case 1:
                                layer_bottom[layer] = y_start - InfoPtr->layer_val[layer];
                                break;
                            case 2:
                                layer_bottom[layer] = InfoPtr->layer_val[layer];
                                break;
                            case 3:
                                layer_bottom[layer] = INVALID_VALUE;
                                break;
                            default:
                                free(table);
                                return 4;  /* unrecognized layer mode */
                        }
                    }
                    current_row = 0;
                    current_elev = y_start;
                    for (layer=0; layer<InfoPtr->num_layers; layer++)
                    {   samp_height = (0.2998*InfoPtr->ns_per_samp)
                                      / (2*sqrt(InfoPtr->layer_rdp[layer]));
                        bottom_elev = layer_bottom[layer];
                        switch (InfoPtr->layer_mode[layer])
                        {   case 1:  /* uniform depth below surface */
                                /* determine if a lower layer's bottom is above current layer's bottom */
                                for (i=0; i<InfoPtr->num_layers; i++)
                                {   if ((i != layer) && (layer_bottom[i] != INVALID_VALUE))
                                    {   if (layer_bottom[i] > bottom_elev)
                                            bottom_elev = layer_bottom[i];
                                    }
                                }
                                y_prev = current_elev;
                                ival = table[InfoPtr->image[current_row][col]];  /* gray value */
                                x_prev = x_left + (double)(ival * x_width)/255.0;
                                if (!reversed) x_prev = _LIMIT(clip_left,x_prev,clip_right);
                                else           x_prev = _LIMIT(clip_right,x_prev,clip_left);
                                current_elev -= samp_height;
                                current_row++;
                                for ( ; current_elev > bottom_elev;
                                     current_row++, current_elev -= samp_height)
                                {   if (current_row >= InfoPtr->num_rows) break;
                                    y = current_elev;
                                    ival = table[InfoPtr->image[current_row][col]];  /* gray value */
                                    x = x_left + (double)(ival * x_width)/255.0;
                                    if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                                    else           x = _LIMIT(clip_right,x,clip_left);
                                    plot(x_prev,y_prev,-2);
                                    plot(x,y,-1);

                                    /* fill in to x_mid if requested */
                                    if (fill_neg && x < x_mid)
                                    {   if (y < y_prev)
                                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                                 if (fill_x >= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                        if (y > y_prev)
                                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                                 if (fill_x >= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                    }
                                    if (fill_pos && x > x_mid)
                                    {   if (y < y_prev)
                                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                                 if (fill_x <= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                        if (y > y_prev)
                                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                                 if (fill_x <= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }

                                    }
                                    y_prev = y;
                                    x_prev = x;
                                }   /* end of for (;current_elev > bottom_elev;...) loop */
                                break;
                            case 2:  /* uniform elevation above sea level */
                                for (i=0; i<InfoPtr->num_layers; i++)
                                {   if ((i != layer) && (layer_bottom[i] != INVALID_VALUE))
                                    {   if (layer_bottom[i] > bottom_elev)
                                            bottom_elev = layer_bottom[i];
                                    }
                                }
                                y_prev = current_elev;
                                ival = table[InfoPtr->image[current_row][col]];  /* gray value */
                                x_prev = x_left + (double)(ival * x_width)/255.0;
                                if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                                else           x = _LIMIT(clip_right,x,clip_left);
                                current_elev -= samp_height;
                                current_row++;
                                for ( ; current_elev > bottom_elev;
                                     current_row++,current_elev-=samp_height)
                                {   if (current_row >= InfoPtr->num_rows) break;
                                    y = current_elev;
                                    ival = table[InfoPtr->image[current_row][col]];  /* gray value */
                                    x = x_left + (double)(ival * x_width)/255.0;
                                    if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                                    else           x = _LIMIT(clip_right,x,clip_left);
                                    plot(x_prev,y_prev,-2);
                                    plot(x,y,-1);

                                    /* fill in to x_mid if requested */
                                    if (fill_neg && x < x_mid)
                                    {   if (y < y_prev)
                                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                                 if (fill_x >= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                        if (y > y_prev)
                                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                                 if (fill_x >= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                    }
                                    if (fill_pos && x > x_mid)
                                    {   if (y < y_prev)
                                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                                 if (fill_x <= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                        if (y > y_prev)
                                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                                 if (fill_x <= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }

                                    }
                                    y_prev = y;
                                    x_prev = x;
                                }   /* end of for (;current_row < InfoPtr->num_rows; ...) loop */
                                break;
                            case 3:  /* only or lowest layer; no bottom */
                                y_prev = current_elev;
                                ival = table[InfoPtr->image[current_row][col]];  /* gray value */
                                x_prev = x_left + (double)(ival * x_width)/255.0;
                                if (!reversed) x_prev = _LIMIT(clip_left,x_prev,clip_right);
                                else           x_prev = _LIMIT(clip_right,x_prev,clip_left);
                                current_elev -= samp_height;
                                current_row++;
                                for ( ; current_row < InfoPtr->num_rows;
                                     current_row++,current_elev-=samp_height)
                                {   y = current_elev;
                                    ival = table[InfoPtr->image[current_row][col]];  /* gray value */
                                    x = x_left + (double)(ival * x_width)/255.0;
                                    if (!reversed) x = _LIMIT(clip_left,x,clip_right);
                                    else           x = _LIMIT(clip_right,x,clip_left);
                                    plot(x_prev,y_prev,-2);
                                    plot(x,y,-1);

                                    /* fill in to x_mid if requested */
                                    if (fill_neg && x < x_mid)
                                    {   if (y < y_prev)
                                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                                 if (fill_x >= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                        if (y > y_prev)
                                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                                 if (fill_x >= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                    }
                                    if (fill_pos && x > x_mid)
                                    {   if (y < y_prev)
                                        {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                                            {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                                                 if (fill_x <= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }
                                        if (y > y_prev)
                                        {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                                            {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                                                 if (fill_x <= x_mid) continue;
                                                 plot(fill_x,fill_y,-2);
                                                 plot(x_mid,fill_y,-1);
                                            }
                                        }

                                    }
                                    y_prev = y;
                                    x_prev = x;
                                }   /* end of for (;current_row < InfoPtr->num_rows;...) loop */
                                break;
                        }   /*  end of switch (InfoPtr->layer_mode[layer]) block */
                    }   /* end of for (...num_layers...) block */
                }   /* end of for (...num_cols...) block */
            }   /* end of if (InfoPtr->num_layers == 1) else block */
            break;
        }   /* end of case 2: block */
    }   /* end of switch (InfoPtr->coord_mode) block */

/***** Free storage *****/
    free(table);
    return 0;
}
/************************** PlotSelectWiggles8() ****************************/
/* This function superimposes traces as wiggle traces on an image
 *
 * Parameter list:
 *  int vrows       - number of CRT vertical pixels
 *  struct DispParamInfoStruct *InfoPtr - address of information structure
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *       This function inherits the graphics mode, CRT viewport, user window,
 *         and color/grayscale palette from the calling routine.
 *       Plotting is 2D only.
 *       This function MUST ONLY BE USED in conjunction with DisplayImage8()
 *         which sets the values for InfoPtr->wiggle_traceval[][1].
 *       Hvals[] and Vvals[] must now be pixel coordinates.
 *
 * Requires: <hpgl.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h".
 * Calls: pen, plot, printf, puts, getch.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
 *
const char *PlotSelectWiggles8Msg[] =
{   "PlotSelectWiggles8(): No errors.",
    "PlotSelectWiggles8() ERROR: One or more positioning arrays are NULL.",
    "PlotSelectWiggles8() ERROR: Not able to allocate sufficient temporary storage.",
    "PlotSelectWiggles8() ERROR: Cannot plot wiggles for DISTANCE-DISTANCE mode.",
    "PlotSelectWiggles8() ERROR: Unrecognized layer model mode.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: December 12, 1995
 */
int PlotSelectWiggles8 (int vrows,struct DispParamInfoStruct *InfoPtr)
{
    unsigned char *uc_column;
    int pen_color;                 /* pen color */
    int i,row;                     /* counters */
    int ival;                      /* scratch variables */
    int fill_neg,fill_pos,reversed;
    int Px,Py;
    double x_width,x_left,x_mid,x,y;        /* to draw trace */
    double fill_x,fill_y,dy,x_prev,y_prev;  /* to fill trace */
    double clip_left,clip_right,clip_width;
    double VYMAX,PY1,PY2,PYdiff,y_range;

    extern int Debug;                     /* "gpr_disp.h" */
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [vrows][vcols] from hpgl.h */
    #endif
    extern int v_colx,v_rowy;             /* from hpgl.h */

/***** Validate some parameters *****/
    if ( InfoPtr->wiggle_traceval == NULL )  return 1;
    if ( InfoPtr->coord_mode == DISTANCE_DISTANCE) return 3;

/***** Allocate storage for one column *****/
    uc_column = (unsigned char *)malloc(InfoPtr->num_rows * sizeof(unsigned char));
    if (uc_column == NULL)  return 2;

/***** Display data as wiggle traces *****/
    /* Calculate jump distance for fill operation */
    /* In THIS function, pixel space (0,0) is lower left corner of screen;
       but for hpgl_array_8[][] (0,0) is upper left corner of screen. */
    /* VX2Y_ratio = (double)vcols/(double)vrows; */
    VYMAX = 100.0;
    /* VXMAX = VYMAX * VX2Y_ratio; */
    PY1 = (InfoPtr->VY1/VYMAX)*(vrows-1);
    PY2 = (InfoPtr->VY2/VYMAX)*(vrows-1);
    PYdiff = PY2 - PY1;
    y_range = InfoPtr->top - InfoPtr->bottom;
    dy = _ABS( y_range / PYdiff);
    x_width = (InfoPtr->right - InfoPtr->left) * (InfoPtr->wiggle_width/100.0);
    clip_width = (InfoPtr->wiggle_clip/100.0) * x_width;
    if (Debug)
    {   puts("PlotSelectWiggles8() in debug mode: no plot() etc.");
        printf("wiggle_width=%g x_width=%g  clip_width=%g  dy=%g\n",
               InfoPtr->wiggle_width,x_width,clip_width,dy);
        getch();
        return 0;
    }
/* fprintf(log_file,"wiggle_width=%g x_width=%g  clip_width=%g  dy=%g\n",
               InfoPtr->wiggle_width,x_width,clip_width,dy); */
    viewport(InfoPtr->VX1,InfoPtr->VX2,InfoPtr->VY1,InfoPtr->VY2);
    window(InfoPtr->left,InfoPtr->right,InfoPtr->bottom,InfoPtr->top);
    clip();
    pen_color = InfoPtr->wiggle_color;
    pen(pen_color);
    linetype(0);
    fill_neg = fill_pos = FALSE;
    if (InfoPtr->wiggle_fill < 0)  fill_neg = TRUE;
    if (InfoPtr->wiggle_fill > 0)  fill_pos = TRUE;
    reversed = FALSE;
    if (InfoPtr->left > InfoPtr->right) reversed = TRUE;

    for (i=0; i<InfoPtr->num_wiggles; i++)
    {   x_mid = InfoPtr->wiggle_traceval[i][0]; /* user units */
        x_left = x_mid - (x_width/2.0);  /* left edge of wiggle area */
        clip_left = x_mid - (clip_width/2.0);
        clip_right = x_mid + (clip_width/2.0);
        Px = InfoPtr->wiggle_traceval[i][1] + 0.5;
        for (row=0; row<InfoPtr->num_rows; row++)
        {   Py = InfoPtr->Vvals[row] + 0.5;
            uc_column[row] = hpgl_array_8[v_rowy-Py][Px];
        }
        row = 0;
        Py = InfoPtr->Vvals[row] + 0.5;
        ival = uc_column[row] - InfoPtr->brightness;
        x = x_left + (ival/255.0) * x_width;
        if (!reversed) x = _LIMIT(clip_left,x,clip_right);
        else           x = _LIMIT(clip_right,x,clip_left);
        y = InfoPtr->bottom + ( ((Py-PY1)/PYdiff) * y_range );
        x_prev = x;
        y_prev = y;
        for (row=1; row<InfoPtr->num_rows; row++)
        {   Py = InfoPtr->Vvals[row] + 0.5;
            ival = uc_column[row] - InfoPtr->brightness;
            x = x_left + (ival/255.0) * x_width;
            if (!reversed) x = _LIMIT(clip_left,x,clip_right);
            else           x = _LIMIT(clip_right,x,clip_left);
            y = InfoPtr->bottom + ( ((Py-PY1)/PYdiff) * y_range );
            if (_ABS(y - y_prev) < 0.001) continue;
            plot(x_prev,y_prev,-2);
            plot(x,y,-1);
/* if (i==1) fprintf(log_file,"row=%d x_mid=%g x_left=%g Px=%d Py=%d ival=%u x=%g y=%g\n",
            row,x_mid,x_left,Px,Py,(unsigned)ival,x,y); */

            /* fill in to x_mid if requested */
            if (fill_neg && x < x_mid)
            {   if (y < y_prev)
                {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                    {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                         if (fill_x >= x_mid) continue;
                         plot(fill_x,fill_y,-2);
                         plot(x_mid,fill_y,-1);
                    }
                }
                if (y > y_prev)
                {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                    {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                         if (fill_x >= x_mid) continue;
                         plot(fill_x,fill_y,-2);
                         plot(x_mid,fill_y,-1);
                    }
                }
            }
            if (fill_pos && x > x_mid)
            {   if (y < y_prev)
                {   for (fill_y=y; fill_y<=y_prev; fill_y+=dy)
                    {   fill_x = x + (((fill_y-y)/(y_prev-y))*(x_prev-x));
                         if (fill_x <= x_mid) continue;
                         plot(fill_x,fill_y,-2);
                         plot(x_mid,fill_y,-1);
                    }
                }
                if (y > y_prev)
                {   for (fill_y=y_prev; fill_y<=y; fill_y+=dy)
                    {   fill_x = x_prev + (((fill_y-y_prev)/(y-y_prev))*(x-x_prev));
                         if (fill_x <= x_mid) continue;
                         plot(fill_x,fill_y,-2);
                         plot(x_mid,fill_y,-1);
                    }
                }
            }
            x_prev = x;
            y_prev = y;
        }   /* end of for (row=1; row<InfoPtr->num_rows; row++) loop */
    }   /* end of for (i=0; i<InfoPtr->num_wiggles; i++) loop */

    return 0;
}
/**************************** PlotHyperbola8() ******************************/
/* This function superimposes a velocity hyperbola on an image.
 *
 * Parameter list:
 *  struct DispParamInfoStruct *InfoPtr - address of information structure
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *       This function inherits the graphics mode, CRT viewport, user window,
 *         and color/grayscale palette from the calling routine.
 *       Plotting is 2D only.
 *       This function MUST ONLY BE USED only after a call to DisplayImage8().
 *
 * Requires: <hpgl.h>, <math.h>, "gpr_disp.h".
 * Calls: pen, plot, viewport, window, clip, circle, unclip, sqrt.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
 *
const char *PlotHyperbola8Msg[] =
{   "PlotHyperbola8(): No errors.",
    "PlotHyperbola8() ERROR: Coordinate mode must be DISTANCE-TIME, mode 4.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 6, 2001
 */
int PlotHyperbola8 (struct DispParamInfoStruct *InfoPtr)
{
    double horiz_loc, horiz_step;
    double ant_step, ant1_loc, ant2_loc, ant1_dist, ant2_dist;
    double total_dist, total_time;
    double dtempx, dtempy, dtimed, dtempt;
    int    pen_color;
    extern int Debug;             /* "gpr_disp.h" */

/***** Validate some parameters *****/
    if (InfoPtr->coord_mode != DISTANCE_TIME) return 1;

/***** Initialize graphics *****/
    viewport(InfoPtr->VX1,InfoPtr->VX2,InfoPtr->VY1,InfoPtr->VY2);
    window(InfoPtr->left,InfoPtr->right,InfoPtr->bottom,InfoPtr->top);
    clip();
    pen_color = InfoPtr->hyp_color;
    pen(pen_color);
    linetype(0);

/***** Draw object and cross *****/
    dtempx = (InfoPtr->right-InfoPtr->left)/20.0;
    dtempy = dtempx;
    dtimed = (2*InfoPtr->obj_depth) / InfoPtr->hyp_vel;
    dtempt = dtempy / InfoPtr->hyp_vel;
    plot(InfoPtr->obj_loc-dtempx,dtimed,-2);
    plot(InfoPtr->obj_loc+dtempx,dtimed,-1);
    plot(InfoPtr->obj_loc,dtimed-dtempt,-2);
    plot(InfoPtr->obj_loc,dtimed+dtempt,-1);
    /* circle(InfoPtr->obj_loc,dtimed,InfoPtr->obj_radius/InfoPtr->hyp_vel,0.0174533); */

/***** Draw hyperbola *****/
    ant_step = InfoPtr->ant_sep/2.0;
    horiz_step = (InfoPtr->horiz_stop - InfoPtr->horiz_start) / 100.0;
    horiz_loc = InfoPtr->horiz_start;
    ant1_loc = horiz_loc - ant_step;
    ant2_loc = horiz_loc + ant_step;
    ant1_dist = sqrt((InfoPtr->obj_loc-ant1_loc)*(InfoPtr->obj_loc-ant1_loc)
                      + (InfoPtr->obj_depth)*(InfoPtr->obj_depth));
    ant2_dist = sqrt((InfoPtr->obj_loc-ant2_loc)*(InfoPtr->obj_loc-ant2_loc)
                      + (InfoPtr->obj_depth)*(InfoPtr->obj_depth));
    ant1_dist -= InfoPtr->obj_radius;
    ant2_dist -= InfoPtr->obj_radius;
    total_dist = ant1_dist + ant2_dist;
    total_time = total_dist / InfoPtr->hyp_vel;
    plot(horiz_loc, total_time, -2);
    while (horiz_loc <= InfoPtr->horiz_stop)
    {   horiz_loc += horiz_step;
        ant1_loc = horiz_loc - ant_step;
        ant2_loc = horiz_loc + ant_step;
        ant1_dist = sqrt((InfoPtr->obj_loc-ant1_loc)*(InfoPtr->obj_loc-ant1_loc)
                          + (InfoPtr->obj_depth)*(InfoPtr->obj_depth));
        ant2_dist = sqrt((InfoPtr->obj_loc-ant2_loc)*(InfoPtr->obj_loc-ant2_loc)
                          + (InfoPtr->obj_depth)*(InfoPtr->obj_depth));
        ant1_dist -= InfoPtr->obj_radius;
        ant2_dist -= InfoPtr->obj_radius;
        total_dist = ant1_dist + ant2_dist;
        total_time = total_dist / InfoPtr->hyp_vel;
        plot(horiz_loc, total_time, -1);
    }
    unclip();
    return 0;
}
/**************************** LabelViewportsEPS() ***************************/
/* This function displays hpgl_array_8[][] to the CRT, adds axes using
 * PostScript labels, adds other labels from a ".LBL" file, and (optionally)
 * saves the screen image to disk as an Encapsulated PostScript file to
 * be printed on either 8.5x11 or 11x17 inch paper, and in either landscape
 * or portrait orientation.
 *
 * Everything to this point is just to get radar data into hpgl_array_8[]][]
 * in the correct place.
 *
 * ***** NOTE *****: On Sept. 15, 1995 a major modification to this function
 *                   was made in how EPS files are written.  The old way was
 *                   incorrect.
 *
 * NOTE: Storage for hpgl_array_8[][] must have been previously allocated.
 *       The GPR data have already been copied into hpgl_array_8[][].
 *
 * HPGL and function variables' relationship to hardcopy from Phaser III PXi:
 * 72 PostScript units per paper inch.
 * ---------------------------------------------------------------------------
 * |   ps_xtranslate                                          hpgl_ury       |
 * |   |      +---------------------------------------------------+ hpgl_urx |
 * |   |      |   the CRT screen and hpgl_array_8                 |          |
 * |   |      |      fits into this space                         |          |
 * |   V      | hpgl_p2yc                                         |          |
 * |          |       VY2 +------------------------------+ ps_VY2 |          |
 * |          |           |  this is one viewport        |        |          |
 * |          |           |  where radar data are        |        |          |
 * |          | hpgl_p1yc |  displayed in                |        |          |
 * |          |       VY1 +------------------------------+ ps_VY1 |          |
 * |          |          VX1      (viewport units)      VX2       |          |
 * |          |         ps_VX1   (PostScript units)    ps_VX2     |          |
 * |          |       hpgl_p1xc    (pixel space)     hpgl_p2xc    |          |
 * |          |                                                   |          |
 * |          |      ps_xscale = hpgl_ury - hpgl_lly;             |          |
 * |          |      ps_yscale = hpgl_urx - hpgl_llx;             |          |
 * |          |      CRT_pixcols = v_colx + 1;                    |          |
 * |          |      CRT_pixrows = v_coly + 1;                    |          |
 * | hpgl_llx +---------------------------------------------------+          |
 * |      hpgl_lly (PostScript units)                                        |
 * |                         8.5x11 or 11x17 paper in landscape orientation  |
 * |  ps_ytranslate ---->                                                    |
 * ---------------------------------------------------------------------------
 *
 * ---------------------------------------------------------------------------
 * |                                                          hpgl_urx       |
 * |          +---------------------------------------------------+ hpgl_ury |
 * |          |   the CRT screen fits into this space             |          |
 * |          |                                                   |          |
 * |          | hpgl_p2yc                                         |          |
 * |          |       VY2 +------------------------------+ ps_VY2 |          |
 * |          |           |  this is one viewport        |        |          |
 * |          |           |  where radar data are        |        |          |
 * |          | hpgl_p1yc |  displayed in                |        |          |
 * |          |       VY1 +------------------------------+ ps_VY1 |          |
 * |          |          VX1      (viewport units)      VX2       |          |
 * |          |         ps_VX1   (PostScript units)    ps_VX2     |          |
 * |          |       hpgl_p1xc    (pixel space)     hpgl_p2xc    |          |
 * |          |                                                   |          |
 * |          |      ps_xscale = hpgl_urx - hpgl_llx;             |          |
 * |  ^       |      ps_yscale = hpgl_ury - hpgl_lly;             |          |
 * |  |       |      CRT_pixcols = v_colx + 1;                    |          |
 * |  |       |      CRT_pixrows = v_coly + 1;                    |          |
 * | hpgl_lly +---------------------------------------------------+          |
 * |  |   hpgl_llx (PostScript units)                                        |
 * |  |                      8.5x11 or 11x17 paper in portrait orientation   |
 * |  ps_ytranslate                                                          |
 * |             for 8.5x11, ps_V.. units are 1.33333 times desired units    |
 * |             for 11x17, ps_V.. units are 1.3333 times desired units      |
 * |  ps_xtranslate ---->                                                    |
 * ---------------------------------------------------------------------------
 *
 * Parameter list:
 *  int vcols      - number columns used on screen (and in hpgl_array_8[][]
 *  int vrows      - number rows used on screen (and in hpgl_array_8[][]
 *  int backcolor  - background color, 0 for black (0x00), 1 for white (0xFF)
 *                   == 1 if saving EPS file
 *  int num_viewports - number of valid pointers in InfoPtr[]
 *  struct DispParamInfoStruct *InfoPtr[] - array of pointers to structures
 *  char *title    - title for display
 *
 * Requires: <conio.h>, <hpgl.h>, <stdio.h>, <string.h>, "gpr_disp.h".
 * Calls: postscript, plotter_is, grey, plot_image, fprintf, hpgl_select_font,
 *        viewport, window, frame, clip, plot, kbhit, getch, printf, rewind,
 *        fclose, unclip, GetLblFileArgs, DrawAxis, Sound, jl_spectrum,
 *        jl_spectrum2, jl_spectrum3.
 * Returns: 0 on success,
 *         >0 on error (offset into message strings array).
 *
const char *LabelViewportsEPSMsg[] =
{   "LabelViewportsEPS(): No errors.",
    "LabelViewportsEPS() ERROR: Invalid function argument.",
    "LabelViewportsEPS() ERROR: fprintf() to EPS file returned EOF."),
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: June 24, 1996
 */
int LabelViewportsEPS (int vcols,int vrows,int backcolor,int num_viewports,
            struct DispParamInfoStruct *InfoPtr[])
{
    unsigned char pal[768];
    int i,j,k,itemp,col,row,chr,port;
    int pen_number,rotate;
    int ps_xtranslate,ps_ytranslate,ps_xscale,ps_yscale; /* PostScript units */
    int ps_VX1,ps_VY1,ps_VX2,ps_VY2;                     /* PostScript units */
    int CRT_VX1,CRT_VX2,CRT_VY1,CRT_VY2,CRT_cols,CRT_rows; /* CRT pixel units */
    double VX_max,VY_max;                                /* CRT viewport units */

    /* these variables found near beginning of this source */
    extern int Debug,save_eps;
    extern FILE *log_file;

    /* these variables found in hpgl.h */
    extern int v_colx,v_rowy;                            /* last column and row */
    extern int hpgl_p1xc,hpgl_p2xc,hpgl_p2yc,hpgl_p1yc;  /* pixel space */
    extern int hpgl_llx,hpgl_lly,hpgl_urx,hpgl_ury;      /* PostScript space */
    extern int hpgl_pen_color;                           /* "color" to turn CRT pixel into */
    extern FILE *hpgl_file;                              /* FILE * for PostScript file */
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [*vrows][*vcols] from hpgl.h */
    #endif

    /* Reality check */
    if ( (InfoPtr == NULL) || (num_viewports < 1) ||
         (num_viewports > MAX_VIEWPORTS) ||
         (vcols == 0) || (vrows == 0) ) return 1;

    /* Setup pen colors */
    if (backcolor) pen_number =   0;   /* black on white background */
    else           pen_number = 255;   /* white on black background */

    /* Initialize graphics system */
    if (Debug)
    {   printf("LabelViewportsEPS() Debug: no graphics.\n");
        return 0;
    }
    plotter_off();
    if (save_eps) /* setup plotting to both CRT and PostScript file */
    {   /* setup PostScript page size with margins that maintain 1.3/1.0
           screen ratio.  72 PostScript units per inch. Phaser III PXi
           will not draw within 0.25 in of paper edge.
         */
        if (InfoPtr[0]->use_11x17)
        {   /* set up for 11 by 17 inch paper, 0.25 in margins top and bottom.
               1.25 inches (90 PS units) seems to be a magic number that
               must added or subtracted at various times for 11 x 17 paper.
             */
            if (InfoPtr[0]->portrait)
            {   hpgl_llx =   24;  /*   18  (0.250 in) * 1.3333333 */
                hpgl_urx = 1008;  /*  756 (10.500 in) * 1.3333333 */
                hpgl_lly =  120;  /*   90  (1.250 in) * 1.3333333 */
                hpgl_ury = 1432;  /* 1074 (14.916 in) * 1.3333333 */
                ps_xtranslate = 0;    /* 0.0 inch */
                ps_ytranslate = 0;
                rotate = 0;
            } else  /* landscape mode */
            {   hpgl_llx =  19;         /* 0.25 in (but 18 doesn't work, 19 does */
                hpgl_lly = 109;         /* 0.25 in + 1.25 in */
                hpgl_urx =  774;        /* 11 in - 0.25 in */
                hpgl_ury = 1115;        /* 17 in - 0.25 in - 1.25 */
                ps_xtranslate = 882;    /* 11 in + 1.25 in "vertical down"    */
                ps_ytranslate = 90;     /*  0 in + 1.25 in "horizontal right" */
                rotate = 90;
            }
        } else if (InfoPtr[0]->use_designjet750)
        {   /* set up for HP Designjet 750 */
#if 0
            if (InfoPtr[0]->portrait)
            {   hpgl_llx =  96;  /*  72 (1.000 in) * 1.3333333 */
                hpgl_urx = 687;  /* 515 (7.153 in) * 1.3333333 */
                hpgl_lly =  96;  /*  72 (1.000 in) * 1.3333333 */
                hpgl_ury = 884;  /* 663 (9.204 in) * 1.3333333 */
                ps_xtranslate = 0;    /* 0.0 inch */
                ps_ytranslate = 0;
                rotate = 0;
            } else  /* landscape mode */
#endif
            {   hpgl_llx = InfoPtr[0]->hp_xoff * 72;
                hpgl_urx = hpgl_llx + (InfoPtr[0]->hp_x * 72);
                hpgl_lly = InfoPtr[0]->hp_yoff * 72;
                hpgl_ury = hpgl_lly + (InfoPtr[0]->hp_y * 72);
                ps_xtranslate = hpgl_ury;  /* ????? */
                ps_ytranslate = 0;    /* 0.0 in */
                rotate   = 90;        /* rotate 90 degrees if in landscape mode */
            }
        } else
        {   /* set up for 8.5 by 11 inch paper, 1 inch margins on left and bottom */
            if (InfoPtr[0]->portrait)
            {   hpgl_llx =  96;  /*  72 (1.000 in) * 1.3333333 */
                hpgl_urx = 687;  /* 515 (7.153 in) * 1.3333333 */
                hpgl_lly =  96;  /*  72 (1.000 in) * 1.3333333 */
                hpgl_ury = 884;  /* 663 (9.204 in) * 1.3333333 */
                ps_xtranslate = 0;    /* 0.0 inch */
                ps_ytranslate = 0;
                rotate = 0;
            } else  /* landscape mode */
            {   hpgl_llx = 72;   /* 1 in */
                hpgl_urx = 540;  /* 7.5 in */
                hpgl_lly = 72;   /* 1 in */
                hpgl_ury = 696;  /* 9.6666666 in */
                ps_xtranslate = 612; /* 8.5 in */
                ps_ytranslate = 0;    /* 0.0 in */
                rotate   = 90;        /* rotate 90 degrees if in landscape mode */
            }
        }   /* end of select 11x17 or 8.5x11 block */

        /* specify PostScript output */
        postscript();

        /* following command opens hpgl_file & initializes video */
        plotter_is(InfoPtr[0]->video_mode,InfoPtr[0]->eps_outfilename);

        /* rewind file so this function writes the beginning part */
        rewind(hpgl_file);

        /* add new intro */
        if (fprintf(hpgl_file, "%sPS-Adobe-3.0 EPSF-3.0\r\n","%!") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "%sBoundingBox: %d %d %d %d\r\n","%%",
            hpgl_llx,hpgl_lly,hpgl_urx,hpgl_ury) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "%sCreator: USGS-GPR_DISP MHP JEL LIBHPGL\r\n","%%") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "%sEndComments\r\n","%%") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* add command for number of copies */
        if (fprintf(hpgl_file, "%sBeginSetup\r\n","%%") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "/#copies %d def\r\n",InfoPtr[0]->eps_copies) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "%sEndSetup\r\n","%%") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* save current graphics values */
        if (fprintf(hpgl_file, "gsave\r\n") == EOF)  /* for file */
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

/***** start of EPS image writing block *****/
        if (fprintf(hpgl_file, "gsave\r\n") == EOF)  /* for image */
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

    } else
    {   /* following command enables plotting to CRT only */
        plotter_is(InfoPtr[0]->video_mode,"CRT");
    }   /* end of if (save_eps) block */

    /* unclip(); */

    /* Set CRT screen palette */
    grey();                    /* gray scale 0 to 255 */

    /* Send image data to CRT */
    plot_image(hpgl_array_8, 0, vcols-1, 0, vrows-1);

    if (save_eps) /* copy entire hpgl_array_8 to EPS file */
    {
        /* calculate placement of corners of image in PS units */
        double crt_ratio = (double)vcols/(double)vrows;
        double VX1,VX2,VY1,VY2;

        VX1 = VY1 = 0.0;                   /* CRT viewport units */
        VX2 = VX_max = 100.0 * crt_ratio;
        VY2 = VY_max = 100.0;

        if (InfoPtr[0]->portrait)
        {   VX2 = VX_max *= 0.5;
            ps_VX1 = hpgl_llx + ( (VX1/VX_max)*(double)(hpgl_urx-hpgl_llx) * 0.75);
            ps_VX2 = hpgl_llx + ( (VX2/VX_max)*(double)(hpgl_urx-hpgl_llx) * 0.75);
            ps_VY1 = hpgl_lly + ( (VY1/VY_max)*(double)(hpgl_ury-hpgl_lly) * 0.75);
            ps_VY2 = hpgl_lly + ( (VY2/VY_max)*(double)(hpgl_ury-hpgl_lly) * 0.75);
        } else  /* landscape mode */
        {   if (InfoPtr[0]->use_11x17)
            {   ps_VX1 = hpgl_lly + ( (VX1/VX_max)*(double)(hpgl_ury-hpgl_lly) ) - 90;
                ps_VX2 = hpgl_lly + ( (VX2/VX_max)*(double)(hpgl_ury-hpgl_lly) ) - 90;
                ps_VY1 = hpgl_llx + ( (VY1/VY_max)*(double)(hpgl_urx-hpgl_llx) ) + 90;
                ps_VY2 = hpgl_llx + ( (VY2/VY_max)*(double)(hpgl_urx-hpgl_llx) ) + 90;
            } else
            {   /* 8.5 x 11 paper */
                ps_VX1 = hpgl_lly + (VX1/VX_max)*(double)(hpgl_ury-hpgl_lly);
                ps_VX2 = hpgl_lly + (VX2/VX_max)*(double)(hpgl_ury-hpgl_lly);
                ps_VY1 = hpgl_llx + (VY1/VY_max)*(double)(hpgl_urx-hpgl_llx);
                ps_VY2 = hpgl_llx + (VY2/VY_max)*(double)(hpgl_urx-hpgl_llx);
            }
        }
        ps_xscale = ps_VX2 - ps_VX1 + 1;
        ps_yscale = ps_VY2 - ps_VY1 + 1;

        /* determine number of cols and rows in the viewport and
           position of viewport in the array */
        CRT_VX1 = (VX1/VX_max)*(double)(vcols-1);
        CRT_VX2 = (VX2/VX_max)*(double)(vcols-1);
        CRT_VY1 = (VY1/VY_max)*(double)(vrows-1);
        CRT_VY2 = (VY2/VY_max)*(double)(vrows-1);
        CRT_cols = CRT_VX2 - CRT_VX1 + 1;
        CRT_rows = CRT_VY2 - CRT_VY1 + 1;

        /* move origin back 8.5 (or 11) inches if in landscape mode, 0 otherwise */
        if (fprintf(hpgl_file,"%d %d translate\r\n",ps_xtranslate,ps_ytranslate) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* rotate 90 degrees clockwise if in landscape mode, 0 otherwise */
        if (fprintf(hpgl_file,"%d rotate\r\n",rotate) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* specify 2-pixel line width */
        if (fprintf(hpgl_file,"%f setlinewidth\r\n",0.25) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* next line establishes where to place lower left corner of the image */
        if (fprintf(hpgl_file, "%d %d translate\r\n", ps_VX1, ps_VY1) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* next line establishes how big the image is in PS units */
        if (fprintf(hpgl_file, "%d %d scale\r\n", ps_xscale, ps_yscale) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* next line tells printer how long an image (horizontal) row is */
        if (fprintf(hpgl_file, "/picstr %d string def\r\n",CRT_cols) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* next lines tells printer how big image is (cols x rows x 8-bits) */
        if (fprintf(hpgl_file, "%d %d 8\r\n",CRT_cols,CRT_rows) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* next line tells the printer scan image from the top;
           if image is to be scanned from bottom use this format:
               fprintf(hpgl_file, "[%d 0 0 %d 0 0]\n", vcols, vrows);
           The general format is this: [a b c d tx ty], where
               x' = ax + by + tx
               y' = cx + dy + ty.
           If b and c are 0.0 then a rectangular image is drawn.  If either
           are not 0, then a parallelogram is drawn or a rotated rectangle or
           parallelogram.
         */
        if (fprintf(hpgl_file, "[%d 0 0 -%d 0 %d]\r\n", CRT_cols, CRT_rows, CRT_rows) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* next 2 lines prepare printer to receive the image */
        if (fprintf(hpgl_file, "{currentfile picstr readhexstring pop}\r\n") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "image\r\n") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* send image data to PostScript file */
        /* send array to file starting at top and left of CRT viewport */
        k = 0;
        row = (vrows-1) - CRT_VY2; /* first row of hpgl_array_8 is at
                                      top of screen */
        for (i=0; i<CRT_rows; i++,row++)
        {   col = CRT_VX1;
            for (j=0; j<CRT_cols; j++,col++)
            {   fprintf(hpgl_file, "%02X", hpgl_array_8[row][col]);
                k++;
                if ((k % 72) == 0)
                {   if (fprintf(hpgl_file, "\r\n") == EOF)
                    {   plotter_off();
                        if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                        return 2;
                    }
                    /* if every fprintf checked, it takes forever! */
                }
            }
        }
        if (fprintf(hpgl_file, "grestore\r\n") == EOF) /* for image */
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

/***** end of EPS image writing block *****/

/***** start of EPS vector writing block *****/

        if (fprintf(hpgl_file, "gsave\r\n") == EOF) /* for vectors */
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* output caption if on short side of paper in portrait mode */
        if ( (InfoPtr[0]->caption_long_edge == FALSE) &&
             (InfoPtr[0]->caption != NULL) )
        {   char *current_str,*end_of_str,*cp;
            int x_offset,y_offset,y_inc,num_lines;

            if (fprintf(hpgl_file, "/%s findfont\r\n",InfoPtr[0]->eps_select_font) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "%d scalefont\r\n",InfoPtr[0]->caption_font_size) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "setfont\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "0.0 0.0 0.0 setrgbcolor\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            x_offset = InfoPtr[0]->caption_left_edge * 72.0;
            y_offset = InfoPtr[0]->caption_bottom_edge * 72.0;
            y_inc    = (double)InfoPtr[0]->caption_font_size * 1.2;

            /* discover how many lines in caption */
            num_lines = 0;
            cp = InfoPtr[0]->caption;
            while((cp = strchr(cp,'\n')) != NULL)
            {   num_lines++;
                cp++;
            }

            y_offset += (num_lines - 1) * y_inc;
            current_str = InfoPtr[0]->caption;
            for (i=0; i<num_lines; i++)
            {   end_of_str = strchr(current_str,'\n');
                *end_of_str = 0;
                if (fprintf(hpgl_file, "%d %d moveto\r\n(%s) show\r\n",
                      x_offset, y_offset, current_str) == EOF)
                {   plotter_off();
                    if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                    return 2;
                }
                y_offset -= y_inc;
                current_str = end_of_str + 1;
            }
        }

        /* output pagenum on short side of paper before rotation */
        if (InfoPtr[0]->pagenum != NULL)
        {   int x_offset,y_offset;

            if (fprintf(hpgl_file, "/%s findfont\n",InfoPtr[0]->eps_select_font) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "%d scalefont\r\n",InfoPtr[0]->pagenum_font_size) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "setfont\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "0.0 0.0 0.0 setrgbcolor\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            x_offset = InfoPtr[0]->pagenum_left_edge * 72.0;
            y_offset = InfoPtr[0]->pagenum_bottom_edge * 72.0;
            if (fprintf(hpgl_file, "%d %d moveto\r\n(%s) show\r\n",
                      x_offset, y_offset, InfoPtr[0]->pagenum) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
        }

        /* output caption if on long side of paper after rotation */
        if ( (InfoPtr[0]->portrait == FALSE)         &&
             (InfoPtr[0]->caption_long_edge == TRUE) &&
             (InfoPtr[0]->caption != NULL)             )
        {   char *current_str,*end_of_str,*cp;
            int x_offset,y_offset,y_inc,num_lines;

            if (fprintf(hpgl_file, "gsave\n") == EOF)  /* local */
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file,"%d %d translate\r\n",ps_xtranslate,ps_ytranslate) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file,"%d rotate\r\n",rotate) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file,"%f setlinewidth\r\n",0.25) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file,"0.0 0.0 0.0 setrgbcolor\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "/%s findfont\r\n",InfoPtr[0]->eps_select_font) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "%d scalefont\r\n",InfoPtr[0]->caption_font_size) == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "setfont\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            if (fprintf(hpgl_file, "0 0 0 setrgbcolor\r\n") == EOF)
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
            x_offset = InfoPtr[0]->caption_left_edge * 72.0;
            y_offset = InfoPtr[0]->caption_bottom_edge * 72.0;
            y_inc    = (double)InfoPtr[0]->caption_font_size * 1.2;

            /* discover how many lines in caption */
            num_lines = 0;
            cp = InfoPtr[0]->caption;
            while((cp = strchr(cp,'\n')) != NULL)
            {   num_lines++;
                cp++;
            }

            y_offset += (num_lines - 1) * y_inc;
            current_str = InfoPtr[0]->caption;
            for (i=0; i<num_lines; i++)
            {   end_of_str = strchr(current_str,'\n');
                *end_of_str = 0;
                if (fprintf(hpgl_file, "%d %d moveto\r\n(%s) show\r\n",
                            x_offset, y_offset, current_str) == EOF)
                {   plotter_off();
                    if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                    return 2;
                }
                y_offset -= y_inc;
                current_str = end_of_str + 1;
            }
            if (fprintf(hpgl_file, "grestore\r\n") == EOF)  /* local */
            {   plotter_off();
                if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
                return 2;
            }
        }

        /* move origin back 8.5 (or 11) inches if in landscape mode, 0 otherwise */
        if (fprintf(hpgl_file,"%d %d translate\r\n",ps_xtranslate,ps_ytranslate) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* rotate 90 degrees clockwise if in landscape mode, 0 otherwise */
        if (fprintf(hpgl_file,"%d rotate\r\n",rotate) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* specify 2-pixel line width */
        if (fprintf(hpgl_file,"%f setlinewidth\r\n",0.25) == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }

        /* set black rbg color */
        if (fprintf(hpgl_file,"0.0 0.0 0.0 setrgbcolor\r\n") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
    }   /* end of if (save_eps) block */

    /* Cycle through viewports; output is to screen and also to EPS file */
    for (port=0; port<num_viewports; port++)
    {   /* set default viewport, window, pen, linetype, and font */
        viewport(InfoPtr[port]->VX1,InfoPtr[port]->VX2,InfoPtr[port]->VY1,InfoPtr[port]->VY2);
        window(InfoPtr[port]->left,InfoPtr[port]->right,InfoPtr[port]->bottom,InfoPtr[port]->top);
        pen(pen_number);
        linetype(0);
        if (InfoPtr[0]->hpgl_select_font == NULL)
            hpgl_select_font("romtrplx");
        else
            hpgl_select_font(InfoPtr[0]->hpgl_select_font);

        /* draw overlay lines */
        if (InfoPtr[port]->add_hlines)
        {   double dval,delta=0.001;

            clip();
            linetype(InfoPtr[port]->hline_style);
            if (InfoPtr[port]->hline_stop > InfoPtr[port]->hline_start
                && InfoPtr[port]->hline_int > 0.0)
            {   dval = InfoPtr[port]->hline_start;
                while (dval < (InfoPtr[port]->hline_stop + delta))
                {   plot(InfoPtr[port]->left,dval,-2);
                    plot(InfoPtr[port]->right,dval,-1);
                    dval += InfoPtr[port]->hline_int;
                }
            } else if (InfoPtr[port]->hline_stop < InfoPtr[port]->hline_start
                       && InfoPtr[port]->hline_int < 0.0)
            {   dval = InfoPtr[port]->hline_start;
                while (dval > (InfoPtr[port]->hline_stop - delta))
                {   plot(InfoPtr[port]->left,dval,-2);
                    plot(InfoPtr[port]->right,dval,-1);
                    dval += InfoPtr[port]->hline_int;
                }
            }
            linetype(0);
            unclip();
        }
        if (InfoPtr[port]->add_vlines)
        {   double dval,delta=0.001;

            clip();
            linetype(InfoPtr[port]->vline_style);
            if (InfoPtr[port]->vline_stop > InfoPtr[port]->vline_start
                       && InfoPtr[port]->vline_int > 0.0)
            {   dval = InfoPtr[port]->vline_start;
                while (dval < (InfoPtr[port]->vline_stop + delta))
                {   plot(dval,InfoPtr[port]->bottom,-2);
                    plot(dval,InfoPtr[port]->top,-1);
                    dval += InfoPtr[port]->vline_int;
                }
            } else if (InfoPtr[port]->vline_stop < InfoPtr[port]->vline_start
                       && InfoPtr[port]->vline_int < 0.0)
            {   dval = InfoPtr[port]->vline_start;
                while (dval > (InfoPtr[port]->vline_stop - delta))
                {   plot(dval,InfoPtr[port]->bottom,-2);
                    plot(dval,InfoPtr[port]->top,-1);
                    dval += InfoPtr[port]->vline_int;
                }
            }
            linetype(0);
            unclip();
        }

        /* get commands from LBL file */
        if (InfoPtr[port]->lbl_infilename[0])
            GetLblFileArgs(InfoPtr[port]->lbl_infilename);

        /* reset again because LBL file may have changed them */
        viewport(InfoPtr[port]->VX1,InfoPtr[port]->VX2,InfoPtr[port]->VY1,InfoPtr[port]->VY2);
        window(InfoPtr[port]->left,InfoPtr[port]->right,InfoPtr[port]->bottom,InfoPtr[port]->top);
        pen(pen_number);
        linetype(0);
        if (InfoPtr[0]->hpgl_select_font == NULL)
            hpgl_select_font("romtrplx");
        else
            hpgl_select_font(InfoPtr[0]->hpgl_select_font);

        /* draw a frame around viewport */
        frame();              /* draw 1 pixel frame at edge of viewport */

        /* Send axis labels to CRT and to PostScript file;
           (hpgl_array_8[][] is left alone) */
        /* Use default values for axes limits if none specified */
        if (InfoPtr[port]->raxis_min      == INVALID_VALUE)
            InfoPtr[port]->raxis_min = InfoPtr[port]->bottom;
        if (InfoPtr[port]->raxis_max      == INVALID_VALUE)
            InfoPtr[port]->raxis_max = InfoPtr[port]->top;
        if (InfoPtr[port]->laxis_min      == INVALID_VALUE)
            InfoPtr[port]->laxis_min = InfoPtr[port]->bottom;
        if (InfoPtr[port]->laxis_max      == INVALID_VALUE)
            InfoPtr[port]->laxis_max = InfoPtr[port]->top;
        if (InfoPtr[port]->taxis_min      == INVALID_VALUE)
            InfoPtr[port]->taxis_min = InfoPtr[port]->left;
        if (InfoPtr[port]->taxis_max      == INVALID_VALUE)
            InfoPtr[port]->taxis_max = InfoPtr[port]->right;
        if (InfoPtr[port]->baxis_min      == INVALID_VALUE)
            InfoPtr[port]->baxis_min = InfoPtr[port]->left;
        if (InfoPtr[port]->baxis_max      == INVALID_VALUE)
            InfoPtr[port]->baxis_max = InfoPtr[port]->right;
        if (InfoPtr[port]->raxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->raxis_tick_abs_min = InfoPtr[port]->raxis_min;
        if (InfoPtr[port]->raxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->raxis_tick_abs_max = InfoPtr[port]->raxis_max;
        if (InfoPtr[port]->laxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->laxis_tick_abs_min = InfoPtr[port]->laxis_min;
        if (InfoPtr[port]->laxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->laxis_tick_abs_max = InfoPtr[port]->laxis_max;
        if (InfoPtr[port]->taxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->taxis_tick_abs_min = InfoPtr[port]->taxis_min;
        if (InfoPtr[port]->taxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->taxis_tick_abs_max = InfoPtr[port]->taxis_max;
        if (InfoPtr[port]->baxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->baxis_tick_abs_min = InfoPtr[port]->baxis_min;
        if (InfoPtr[port]->baxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->baxis_tick_abs_max = InfoPtr[port]->baxis_max;
        if (InfoPtr[port]->raxis_tick_num == INVALID_VALUE) InfoPtr[port]->raxis_tick_num = 9;
        if (InfoPtr[port]->laxis_tick_num == INVALID_VALUE) InfoPtr[port]->laxis_tick_num = 9;
        if (InfoPtr[port]->taxis_tick_num == INVALID_VALUE) InfoPtr[port]->taxis_tick_num = 9;
        if (InfoPtr[port]->baxis_tick_num == INVALID_VALUE) InfoPtr[port]->baxis_tick_num = 9;

        /* Draw axes and title.  MUST reset viewport after last call to DrawAxis() */
        itemp = DrawAxis(DA_RIGHT,pen_number,vcols,vrows,InfoPtr[port]->raxis_tick_show,
                 InfoPtr[port]->raxis_tick_num,InfoPtr[port]->raxis_tick_ano,
                 InfoPtr[port]->raxis_tick_skip,InfoPtr[port]->raxis_tick_mid_skip,
                 InfoPtr[port]->raxis_ano_left,
                 InfoPtr[port]->raxis_ano_right,InfoPtr[port]->raxis_min,
                 InfoPtr[port]->raxis_max,InfoPtr[port]->raxis_tick_start,
                 InfoPtr[port]->raxis_tick_int,
                 InfoPtr[port]->raxis_tick_abs_min,InfoPtr[port]->raxis_tick_abs_max,
                 InfoPtr[port]->raxis_title_size,InfoPtr[port]->raxis_title_offset,
                 InfoPtr[port]->raxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->raxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_LEFT,pen_number,vcols,vrows,InfoPtr[port]->laxis_tick_show,
                 InfoPtr[port]->laxis_tick_num,InfoPtr[port]->laxis_tick_ano,
                 InfoPtr[port]->laxis_tick_skip,InfoPtr[port]->laxis_tick_mid_skip,
                 InfoPtr[port]->laxis_ano_left,
                 InfoPtr[port]->laxis_ano_right,InfoPtr[port]->laxis_min,
                 InfoPtr[port]->laxis_max,InfoPtr[port]->laxis_tick_start,
                 InfoPtr[port]->laxis_tick_int,
                 InfoPtr[port]->laxis_tick_abs_min,InfoPtr[port]->laxis_tick_abs_max,
                 InfoPtr[port]->laxis_title_size,InfoPtr[port]->laxis_title_offset,
                 InfoPtr[port]->laxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->laxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_BOTTOM,pen_number,vcols,vrows,InfoPtr[port]->baxis_tick_show,
                 InfoPtr[port]->baxis_tick_num,InfoPtr[port]->baxis_tick_ano,
                 InfoPtr[port]->baxis_tick_skip,InfoPtr[port]->baxis_tick_mid_skip,
                 InfoPtr[port]->baxis_ano_left,
                 InfoPtr[port]->baxis_ano_right,InfoPtr[port]->baxis_min,
                 InfoPtr[port]->baxis_max,InfoPtr[port]->baxis_tick_start,
                 InfoPtr[port]->baxis_tick_int,
                 InfoPtr[port]->baxis_tick_abs_min,InfoPtr[port]->baxis_tick_abs_max,
                 InfoPtr[port]->baxis_title_size,InfoPtr[port]->baxis_title_offset,
                 InfoPtr[port]->baxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->baxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_TOP,pen_number,vcols,vrows,InfoPtr[port]->taxis_tick_show,
                 InfoPtr[port]->taxis_tick_num,InfoPtr[port]->taxis_tick_ano,
                 InfoPtr[port]->taxis_tick_skip,InfoPtr[port]->taxis_tick_mid_skip,
                 InfoPtr[port]->taxis_ano_left,
                 InfoPtr[port]->taxis_ano_right,InfoPtr[port]->taxis_min,
                 InfoPtr[port]->taxis_max,InfoPtr[port]->taxis_tick_start,
                 InfoPtr[port]->taxis_tick_int,
                 InfoPtr[port]->taxis_tick_abs_min,InfoPtr[port]->taxis_tick_abs_max,
                 InfoPtr[port]->taxis_title_size,InfoPtr[port]->taxis_title_offset,
                 InfoPtr[port]->taxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->taxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_TITLE,pen_number,vcols,vrows,InfoPtr[port]->taxis_tick_show,
                 InfoPtr[port]->taxis_tick_num,InfoPtr[port]->taxis_tick_ano,
                 InfoPtr[port]->taxis_tick_skip,InfoPtr[port]->taxis_tick_mid_skip,
                 InfoPtr[port]->taxis_ano_left,
                 InfoPtr[port]->taxis_ano_right,InfoPtr[port]->taxis_min,
                 InfoPtr[port]->taxis_max,InfoPtr[port]->taxis_tick_start,
                 InfoPtr[port]->taxis_tick_int,
                 InfoPtr[port]->taxis_tick_abs_min,InfoPtr[port]->taxis_tick_abs_max,
                 InfoPtr[port]->taxis_title_size,InfoPtr[port]->taxis_title_offset,
                 InfoPtr[port]->taxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);

        /* reset again because DrawAxis() changed them */
        viewport(InfoPtr[port]->VX1,InfoPtr[port]->VX2,InfoPtr[port]->VY1,InfoPtr[port]->VY2);
        window(InfoPtr[port]->left,InfoPtr[port]->right,InfoPtr[port]->bottom,InfoPtr[port]->top);
        pen(pen_number);
        linetype(0);
        if (InfoPtr[0]->hpgl_select_font == NULL)
            hpgl_select_font("romtrplx");
        else
            hpgl_select_font(InfoPtr[0]->hpgl_select_font);

    } /* end of for (port=0; port<num_viewports; port++) block */

    if (save_eps)
    {   if (fprintf(hpgl_file, "\r\ngrestore\r\n") == EOF) /* for vectors */
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "showpage\r\n") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "grestore\r\n") == EOF) /* for file */
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        if (fprintf(hpgl_file, "%sTrailer\r\n","%%") == EOF)
        {   plotter_off();
            if (log_file) fprintf(log_file,"LabelViewportsEPS() ERROR writing image to EPS file.\n");
            return 2;
        }
        fclose(hpgl_file); /* closes file before graphics library can */
        /* if (!Batch) Sound(500,0.25); */
    }

/***** end of EPS vector writing block *****/

    /* NOTE TO DEVELOPERS: Here is where you can add code/functions to
       modify the appearance of the screen or add vector graphics to the
       screen and the PostScript file.  plotter_off() has not been called yet.
     */
    if (!Batch)
    {   while (kbhit())
        {   if (!getch()) getch();      /* clear buffer */
        }
        while ((chr = getch()) != ESC)
        {   if (chr == 'g' || chr == 'G') grey();
            if (chr == 's' || chr == 'S') spectrum();
            if (chr == 'r' || chr == 'R') spectrum_rev();
            if (chr == '1') jl_spectrum(pal);
            if (chr == '2') jl_spectrum2(pal);
            if (chr == '3') jl_spectrum3(pal);
            if (chr == HELP)
            {   unsigned char **screen_image = NULL;
                long size,i,j;
                extern int v_mode;   /* in hpgl.h */

                /* allocate storage for screen in 1024x768x256 mode */
                if (v_mode != 1024 && v_mode != 56 && v_mode != 261) continue;
                screen_image = (unsigned char **)malloc(vrows * sizeof(unsigned char *));
                if (screen_image == NULL)  continue;
                size = vcols * sizeof(unsigned char);
                for (i=0; i<vrows; i++)
                {   screen_image[i] = (unsigned char *)malloc((size_t)size);
                    if (screen_image[i] == NULL)
                    {   for (j=0; j<i; j++) free(screen_image[j]);
                         free(screen_image);
                         screen_image = NULL;
                         break;
                    }
                }
                if (screen_image == NULL) continue;

                /* capture screen */
                grab1024(screen_image);

                /* go to text mode */
                plotter_off();

                /* display help screen */
                GPR_DISP_HelpScreen();
                getch();

                /* restore screen and grey scale palette */
                plotter_is(InfoPtr[0]->video_mode,"CRT");
                grey();
                plot1024(screen_image);
                for (i=0; i<vrows; i++) free(screen_image[i]);
                free(screen_image);
            }
        }
    }

    /* Terminate graphics mode and close PostScript file */
    plotter_off();

    return 0;
}
/**************************** LabelViewportsPCX() ***************************/
/* This function adds axes and other labels to the viewports while
 * plotter_is(0,"ARRAY") is in effect, then copies hpgl_array_8[][] to the
 * CRT.  The screen image is then saved as a PCX file.
 *
 * Everything to this point is just to get radar data into hpgl_array_8[]][]
 * in the correct place.
 *
 * NOTE: Storage for hpgl_array_8[][] must have been previously allocated.
 *       The GPR data have already been copied into hpgl_array_8[][].
 *
 * Parameter list:
 *  int vcols      - number columns used on screen (and in hpgl_array_8[][])
 *  int vrows      - number rows used on screen (and in hpgl_array_8[][])
 *  int backcolor  - background color, 0 for black (0x00), 1 for white (0xFF)
 *                   == 1 if saving EPS file
 *  int num_viewports - number of valid pointers in InfoPtr[]
 *  struct DispParamInfoStruct *InfoPtr[] - array of pointers to structures
 *  char *title    - title for display
 *
 * Requires: <conio.h>, <hpgl.h>, <stdio.h>, <string.h>, "gpr_disp.h".
 * Calls: plotter_is, grey, plot_image, fprintf, hpgl_select_font,
 *        viewport, window, frame, clip, plot, kbhit, getch, printf, rewind,
 *        fopen, fclose, ferror, frwite, clearerr, GetLblFileArgs, DrawAxis,
 *        SetPcxHeader, jl_spectrum, jl_spectrum2, jl_spectrum3.
 * Returns: 0 on success,
 *         >0 on error (offset into message strings array).
 *
const char *LabelViewportsPCXMsg[] =
{   "LabelViewportsPCX(): No errors.",
    "LabelViewportsPCX() ERROR: Invalid function argument.",
    "LabelViewportsPCX() ERROR: unable to open output PCX file.",
    "LabelViewportsPCX() ERROR: short count from fwrite() to PCX file.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: January 23, 1996
 */
int LabelViewportsPCX (int vcols,int vrows,int backcolor,int num_viewports,
            struct DispParamInfoStruct *InfoPtr[])
{
    unsigned char pal[768];
    unsigned char EncodedBuffer[PCX_BUF_SIZE];
    int i,j,itemp,col,row,chr,port;
    int pen_number,error,bytes;
    int CRT_VX1,CRT_VX2,CRT_VY1,CRT_VY2,CRT_cols,CRT_rows; /* pixel units */
    double VX_max,VY_max;                                  /* viewport units */
    FILE *pcx_file;
    struct PcxHdrStruct PcxHdr;

    /* these variables found near beginning of this source */
    extern int Debug;
    extern FILE *log_file;

    /* these variables found in hpgl.h */
    extern int v_colx,v_rowy;                            /* last column and row */
    extern int hpgl_pen_color;                           /* "color" to turn CRT pixel into */
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [*vrows][*vcols] from hpgl.h */
    #endif

    /* Reality check */
    if ( (InfoPtr == NULL) || (num_viewports < 1) ||
         (num_viewports > MAX_VIEWPORTS) ||
         (vcols == 0) || (vrows == 0) ) return 1;

    /* Setup pen colors */
    if (backcolor) pen_number =   0;   /* black on white background */
    else           pen_number = 255;   /* white on black background */

    /* Initialize graphics system */
    if (Debug)
    {   printf("LabelViewportsPCX() Debug: no graphics.\n");
        return 0;
    }
    plotter_off();
    plotter_is(0,"ARRAY");

    /* Set CRT screen palette */
    grey();                    /* gray scale 0 to 255 */

    /* Cycle through viewports; output is to screen */
    for (port=0; port<num_viewports; port++)
    {   /* set default viewport, window, pen, linetype, and font */
        viewport(InfoPtr[port]->VX1,InfoPtr[port]->VX2,InfoPtr[port]->VY1,InfoPtr[port]->VY2);
        window(InfoPtr[port]->left,InfoPtr[port]->right,InfoPtr[port]->bottom,InfoPtr[port]->top);
        pen(pen_number);
        linetype(0);
        if (InfoPtr[0]->hpgl_select_font == NULL)
            hpgl_select_font("romtrplx");
        else
            hpgl_select_font(InfoPtr[0]->hpgl_select_font);

        /* get commands from LBL file */
        if (InfoPtr[port]->lbl_infilename[0])
            GetLblFileArgs(InfoPtr[port]->lbl_infilename);

        /* reset viewport and window because LBL file may have changed them */
        viewport(InfoPtr[port]->VX1,InfoPtr[port]->VX2,InfoPtr[port]->VY1,InfoPtr[port]->VY2);
        window(InfoPtr[port]->left,InfoPtr[port]->right,InfoPtr[port]->bottom,InfoPtr[port]->top);
        pen(pen_number);
        linetype(0);

        /* draw a frame around viewport */
        frame();              /* draw 1 pixel frame at edge of viewport */

        /* use default values for axes limits if none specified */
        if (InfoPtr[port]->raxis_min      == INVALID_VALUE)
            InfoPtr[port]->raxis_min = InfoPtr[port]->bottom;
        if (InfoPtr[port]->raxis_max      == INVALID_VALUE)
            InfoPtr[port]->raxis_max = InfoPtr[port]->top;
        if (InfoPtr[port]->laxis_min      == INVALID_VALUE)
            InfoPtr[port]->laxis_min = InfoPtr[port]->bottom;
        if (InfoPtr[port]->laxis_max      == INVALID_VALUE)
            InfoPtr[port]->laxis_max = InfoPtr[port]->top;
        if (InfoPtr[port]->taxis_min      == INVALID_VALUE)
            InfoPtr[port]->taxis_min = InfoPtr[port]->left;
        if (InfoPtr[port]->taxis_max      == INVALID_VALUE)
            InfoPtr[port]->taxis_max = InfoPtr[port]->right;
        if (InfoPtr[port]->baxis_min      == INVALID_VALUE)
            InfoPtr[port]->baxis_min = InfoPtr[port]->left;
        if (InfoPtr[port]->baxis_max      == INVALID_VALUE)
            InfoPtr[port]->baxis_max = InfoPtr[port]->right;
        if (InfoPtr[port]->raxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->raxis_tick_abs_min = InfoPtr[port]->raxis_min;
        if (InfoPtr[port]->raxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->raxis_tick_abs_max = InfoPtr[port]->raxis_max;
        if (InfoPtr[port]->laxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->laxis_tick_abs_min = InfoPtr[port]->laxis_min;
        if (InfoPtr[port]->laxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->laxis_tick_abs_max = InfoPtr[port]->laxis_max;
        if (InfoPtr[port]->taxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->taxis_tick_abs_min = InfoPtr[port]->taxis_min;
        if (InfoPtr[port]->taxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->taxis_tick_abs_max = InfoPtr[port]->taxis_max;
        if (InfoPtr[port]->baxis_tick_abs_min == INVALID_VALUE)
            InfoPtr[port]->baxis_tick_abs_min = InfoPtr[port]->baxis_min;
        if (InfoPtr[port]->baxis_tick_abs_max == INVALID_VALUE)
            InfoPtr[port]->baxis_tick_abs_max = InfoPtr[port]->baxis_max;
        if (InfoPtr[port]->raxis_tick_num == INVALID_VALUE) InfoPtr[port]->raxis_tick_num = 9;
        if (InfoPtr[port]->laxis_tick_num == INVALID_VALUE) InfoPtr[port]->laxis_tick_num = 9;
        if (InfoPtr[port]->taxis_tick_num == INVALID_VALUE) InfoPtr[port]->taxis_tick_num = 9;
        if (InfoPtr[port]->baxis_tick_num == INVALID_VALUE) InfoPtr[port]->baxis_tick_num = 9;

        /* Draw axes and title.  MUST reset viewport after last call to DrawAxis() */
        itemp = DrawAxis(DA_RIGHT,pen_number,vcols,vrows,InfoPtr[port]->raxis_tick_show,
                 InfoPtr[port]->raxis_tick_num,InfoPtr[port]->raxis_tick_ano,
                 InfoPtr[port]->raxis_tick_skip,InfoPtr[port]->raxis_tick_mid_skip,
                 InfoPtr[port]->raxis_ano_left,
                 InfoPtr[port]->raxis_ano_right,InfoPtr[port]->raxis_min,
                 InfoPtr[port]->raxis_max,InfoPtr[port]->raxis_tick_start,
                 InfoPtr[port]->raxis_tick_int,
                 InfoPtr[port]->raxis_tick_abs_min,InfoPtr[port]->raxis_tick_abs_max,
                 InfoPtr[port]->raxis_title_size,InfoPtr[port]->raxis_title_offset,
                 InfoPtr[port]->raxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->raxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_LEFT,pen_number,vcols,vrows,InfoPtr[port]->laxis_tick_show,
                 InfoPtr[port]->laxis_tick_num,InfoPtr[port]->laxis_tick_ano,
                 InfoPtr[port]->laxis_tick_skip,InfoPtr[port]->laxis_tick_mid_skip,
                 InfoPtr[port]->laxis_ano_left,
                 InfoPtr[port]->laxis_ano_right,InfoPtr[port]->laxis_min,
                 InfoPtr[port]->laxis_max,InfoPtr[port]->laxis_tick_start,
                 InfoPtr[port]->laxis_tick_int,
                 InfoPtr[port]->laxis_tick_abs_min,InfoPtr[port]->laxis_tick_abs_max,
                 InfoPtr[port]->laxis_title_size,InfoPtr[port]->laxis_title_offset,
                 InfoPtr[port]->laxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->laxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_BOTTOM,pen_number,vcols,vrows,InfoPtr[port]->baxis_tick_show,
                 InfoPtr[port]->baxis_tick_num,InfoPtr[port]->baxis_tick_ano,
                 InfoPtr[port]->baxis_tick_skip,InfoPtr[port]->baxis_tick_mid_skip,
                 InfoPtr[port]->baxis_ano_left,
                 InfoPtr[port]->baxis_ano_right,InfoPtr[port]->baxis_min,
                 InfoPtr[port]->baxis_max,InfoPtr[port]->baxis_tick_start,
                 InfoPtr[port]->baxis_tick_int,
                 InfoPtr[port]->baxis_tick_abs_min,InfoPtr[port]->baxis_tick_abs_max,
                 InfoPtr[port]->baxis_title_size,InfoPtr[port]->baxis_title_offset,
                 InfoPtr[port]->baxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->baxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_TOP,pen_number,vcols,vrows,InfoPtr[port]->taxis_tick_show,
                 InfoPtr[port]->taxis_tick_num,InfoPtr[port]->taxis_tick_ano,
                 InfoPtr[port]->taxis_tick_skip,InfoPtr[port]->taxis_tick_mid_skip,
                 InfoPtr[port]->taxis_ano_left,
                 InfoPtr[port]->taxis_ano_right,InfoPtr[port]->taxis_min,
                 InfoPtr[port]->taxis_max,InfoPtr[port]->taxis_tick_start,
                 InfoPtr[port]->taxis_tick_int,
                 InfoPtr[port]->taxis_tick_abs_min,InfoPtr[port]->taxis_tick_abs_max,
                 InfoPtr[port]->taxis_title_size,InfoPtr[port]->taxis_title_offset,
                 InfoPtr[port]->taxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->taxis_title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);
        itemp = DrawAxis(DA_TITLE,pen_number,vcols,vrows,InfoPtr[port]->taxis_tick_show,
                 InfoPtr[port]->taxis_tick_num,InfoPtr[port]->taxis_tick_ano,
                 InfoPtr[port]->taxis_tick_skip,InfoPtr[port]->taxis_tick_mid_skip,
                 InfoPtr[port]->taxis_ano_left,
                 InfoPtr[port]->taxis_ano_right,InfoPtr[port]->taxis_min,
                 InfoPtr[port]->taxis_max,InfoPtr[port]->taxis_tick_start,
                 InfoPtr[port]->taxis_tick_int,
                 InfoPtr[port]->taxis_tick_abs_min,InfoPtr[port]->taxis_tick_abs_max,
                 InfoPtr[port]->taxis_title_size,InfoPtr[port]->taxis_title_offset,
                 InfoPtr[port]->taxis_ano_size,InfoPtr[port]->VX1,InfoPtr[port]->VX2,
                 InfoPtr[port]->VY1,InfoPtr[port]->VY2,InfoPtr[port]->title);
        if (Debug && itemp) printf("DrawAxis() ERROR %d\n",itemp);

        /* reset viewport and window because DrawAxis() changed them */
        viewport(InfoPtr[port]->VX1,InfoPtr[port]->VX2,InfoPtr[port]->VY1,InfoPtr[port]->VY2);
        window(InfoPtr[port]->left,InfoPtr[port]->right,InfoPtr[port]->bottom,InfoPtr[port]->top);
        pen(pen_number);
        linetype(0);

    } /* end of for (port=0; port<num_viewports; port++) block */

    /* Enable plotting to CRT only */
    plotter_is(InfoPtr[0]->video_mode,"CRT");

    /* Set CRT screen palette */
    grey();                    /* gray scale 0 to 255 */

    /* Send image data to CRT */
    plot_image(hpgl_array_8, 0, vcols-1, 0, vrows-1);

    /* NOTE TO DEVELOPERS: Here is where you can add code/functions to
       modify the appearance of the screen.
     */
    for (i=0,j=0; i<=765; i+=3,j++)
        pal[i] = pal[i+1] = pal[i+2] = j;

    if (!Batch)
    {   while (kbhit())
        {   if (!getch()) getch();      /* clear buffer */
        }
        while ((chr = getch()) != ESC)
        {   if (chr == 'g' || chr == 'G')
            {   grey();
                for (i=0,j=0; i<=765; i+=3,j++)
                     pal[i] = pal[i+1] = pal[i+2] = j;
            }
            if (chr == 's' || chr == 'S') spectrum();
            if (chr == 'r' || chr == 'R')
            {   spectrum_rev();
                for (i=0,j=255; i<=765; i+=3,j--)
                     pal[i] = pal[i+1] = pal[i+2] = j;
            }
            if (chr == '1') jl_spectrum(pal);
            if (chr == '2') jl_spectrum2(pal);
            if (chr == '3') jl_spectrum3(pal);
            if (chr == HELP)
            {   unsigned char **screen_image = NULL;
                long size,i,j;
                extern int v_mode;   /* in hpgl.h */

                /* allocate storage for screen in 1024x768x256 mode */
                if (v_mode != 1024 && v_mode != 56 && v_mode != 261) continue;
                screen_image = (unsigned char **)malloc(vrows * sizeof(unsigned char *));
                if (screen_image == NULL)  continue;
                size = vcols * sizeof(unsigned char);
                for (i=0; i<vrows; i++)
                {   screen_image[i] = (unsigned char *)malloc((size_t)size);
                    if (screen_image[i] == NULL)
                    {   for (j=0; j<i; j++) free(screen_image[j]);
                         free(screen_image);
                         screen_image = NULL;
                         break;
                    }
                }
                if (screen_image == NULL) continue;

                /* capture screen */
                grab1024(screen_image);

                /* go to text mode */
                plotter_off();

                /* display help screen */
                GPR_DISP_HelpScreen();
                getch();

                /* restore screen and grey scale palette */
                plotter_is(InfoPtr[0]->video_mode,"CRT");
                grey();
                plot1024(screen_image);
                for (i=0; i<vrows; i++) free(screen_image[i]);
                free(screen_image);
            }
        }
    }

    /* Terminate graphics mode */
    plotter_off();

    /* Generate PCX file */
    printf("\n\n\n\n\n\n\n\n\n\n                          SAVING PCX FILE ...");
    error = 0;
    if ((pcx_file = fopen(InfoPtr[0]->pcx_outfilename,"wb")) == NULL)
        error = 2;
    if (!error)
    {   /* SetPcxHeader(int BitsPerPixel,int X1,int Y1,int X2,int Y2,
                   int HorDpi,int VerDpi,int NumBitPlanes,int BytesPerLine,
                   int PaletteType,int HorScrnSize,int VerScrnSize,
                   struct PcxHdrStruct *HdrPtr)
         */
        SetPcxHeader(8,0,0,vcols-1,vrows-1,vcols,vrows,1,vcols,1,vcols,vrows,
                     &PcxHdr);
        if ((fwrite((void *)&PcxHdr,sizeof(struct PcxHdrStruct),(size_t)1,pcx_file)) < 1)
        {   error = 3;
            if (log_file)
                fprintf(log_file,"ERROR encountered saving PCX header\n");
        }
        if (!error)
        {   /* write image data to disk */
            for (i=0; i<vrows; i++)
            {   bytes = PcxEncodeScanLine(hpgl_array_8[i],EncodedBuffer,vcols);
                fwrite((void *)EncodedBuffer,sizeof(unsigned char),(size_t)bytes,pcx_file);
            }
            if (ferror(pcx_file))  /* file stream error; EOF or frite short count */
            {   clearerr(pcx_file);
                error = 3;
                if (log_file)
                    fprintf(log_file,"ERROR encountered saving PCX image data\n");
            }
            /* write palette to disk */
            if (!error)
            {   unsigned char chr = 0x0C;

                fwrite((void *)&chr,sizeof(unsigned char),(size_t)1,pcx_file);
                fwrite((void *)pal,sizeof(unsigned char),(size_t)768,pcx_file);
                if (ferror(pcx_file))  /* file stream error; EOF or frite short count */
                {   clearerr(pcx_file);
                    error = 3;
                    if (log_file)
                        fprintf(log_file,"ERROR encountered saving PCX palette\n");
                }
            }
        }
        fclose(pcx_file);
        printf("\n\n\nPCX file %s saved to disk.\n",InfoPtr[0]->pcx_outfilename);
        if (log_file)
            fprintf(log_file,"PCX file %s saved to disk.\n",InfoPtr[0]->pcx_outfilename);
        if (error)
        {   printf("ERRORS encountered saving PCX file!\n");
            if (log_file)
                fprintf(log_file,"ERRORS encountered saving PCX file!\n");
        }
    }

    return error;
}
void GPR_DISP_HelpScreen (void)
{
   puts("\n                         Program GPR_DISP HELP Screen\n");
   puts("Key        Usage\n");
   puts("g or G     calls grey()");
   puts("s or S     calls spectrum()");
   puts("r or R     calls spectrum_rev()");
   puts("1 to 3     calls custom palletes (PCX files will store colors)");
   puts("  F1       to get this help screen");
   puts("<Esc>      to end program");
   puts("\nFollowing options not implemented yet.");
   puts("alt-I      increase image contrast");
   puts("alt-D      decrease image contrast");
   puts("alt-W      whiten (brighten) image");
   puts("alt-B      blacken (darken) image");
   puts("\n\n\nPress a key to return to image.");
}
/************************ TerminateGraphicsARRAY() **************************/
/* This function turns the graphics system off, retsores video mode 3, and
 * release storage allocated to hpgl_array_8.
 *
 * Parameter list:
 *  int vrows - number of CRT vertical pixels (used to allocate and
 *              deallocate hpgl_array_8.
 *
 * WARNING: information held in global array hpgl_array_8[][] is essentially
 *          unavailable after this call.
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *
 * Requires: <hpgl.h>, <stdlib.h>, <stdio.h>.
 * Calls: plotter_off, free, printf.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 6, 1995
 */
void TerminateGraphicsARRAY (int vrows)
{
    int i;
    extern int Debug;
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [*vrows][*vcols] from hpgl.h */
    #endif

    if (Debug)  puts("TerminateGraphicsARRAY() called.");
    else        plotter_off();            /* terminate graphics system */

    /* free_image(hpgl_array_8,v_rowy+1); */
    for (i=0; i<vrows; i++)
    {   free(hpgl_array_8[i]);
/* if (Debug) printf("\tfreed hpgl_array_8[%d]\n",i); */
    }
    free(hpgl_array_8);        /* deallocate storage for global variable */
if (Debug) puts("freed hpgl_array_8");
    hpgl_array_8 = NULL;
}
/*************************** InitGraphicsARRAY() ****************************/
/* This function initializes the graphics system to utilize the global array
 * hpgl_array_8[][].
 *
 * Parameter list:
 *  int backcolor - background color, 0 for black, 1 for white
 *  int vmode     - video mode
 *  int portrait  - if 0, then landscape output orientationa and entire screen
 *                  used; if !=0, then portrait orientation and X-direction
 *                  viewport values limited to 75.
 *  int *vcols    - address of variable to return number of CRT horizontal pixels
 *  int *vrows    - address of variable to return number of CRT vertical pixels
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *
 * Requires: <hpgl.h>, <stdlib.h>, <string.h>, <stdio.h>.
 * Calls: plotter_is, plotter_off, malloc, free, memset, printf, gets, puts.
 * Returns: 0 on success, or
 *         >0 if error.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: November 6, 1995
 */
int InitGraphicsARRAY (int backcolor,int vmode,int portrait,int *vcols,int *vrows)
{
    int setval;
    long i,j,size;
    extern int Debug;
    extern int v_colx,v_rowy;             /* from hpgl.h */
    #if defined(_INTELC32_)
      extern unsigned char **hpgl_array_8; /* [*vrows][*vcols] from hpgl.h */
    #endif

    /* Get number of screen columns and rows */
    if (Debug)
    {   printf("\nInitGraphicsARRAY(): vmode = %d Press a key (screen will clear) ..",vmode);
        getch();
        puts("");
    }
    plotter_is(vmode,"CRT");   /* initialize graphics mode */
    *vcols = v_colx + 1;       /* number of screen horizontal pixels */
    *vrows = v_rowy + 1;       /* number of screen vertical pixels */
    plotter_off();             /* return to text mode */
    /* in portrait mode, maximum viewport used is (0.0,75.0,0.0,100.0) */
    if (portrait) *vcols = 0.75 * *vrows;
    if (Debug)  printf("\tvcols = %d  vrows = %d\n",*vcols,*vrows);

    /* Allocate storage for global variable and set to backcolor */
    #if 0
      /* this snippet shows original intention and lack of error checking */
      hpgl_array_8 = image(v_rowy+1,v_colx+1);
      for (i=0; i<v_rowy+1; i++)
          memset(&hpgl_array_8[i][0], 0, v_colx+1);
    #endif
    hpgl_array_8 = (unsigned char **)malloc(*vrows * sizeof(unsigned char *));
    if (hpgl_array_8 == NULL)  return 1;
    size = *vcols * sizeof(unsigned char);
    if (backcolor) setval = 0xFF; /* white background */
    else           setval = 0x00; /* black background */
    for (i=0; i< *vrows; i++)
    {   hpgl_array_8[i] = (unsigned char *)malloc((size_t)size);
        if (hpgl_array_8[i] == NULL)
        {   for (j=0; j<i; j++) free(hpgl_array_8[j]);
             free(hpgl_array_8);
             hpgl_array_8 = NULL;
             return 1;
        }
        memset(hpgl_array_8[i], setval, (size_t)size);
    }

    /* Initialize graphics to be re-routed from screen to hpgl_array_8[] */
    if (!Debug)
        plotter_is(0,"ARRAY");    /* refers to global variable hpgl_array_8 */

    return 0;
}
/******************************** DrawAxis() ********************************/
/* This function attaches an axis to one side of a rectangle defined by VX1,
 * VX2, VY1, and VY2 (the minimums and maximums of the X and Y directions
 * of the screen viewport).  Values for the endpoints of the axis must be
 * given.  A label for the axis may also be given.
 * If tick_start or tick_int are not both given then tick_num is used to
 * calculate the tick mark locations. Then the actual number of tick marks and
 * the interval between marks is decided by this function, based on the suggested
 * value.  Useful ranges are between +/-0.001 and +/-999999.0.  Only tick
 * annotations (the values) that do not touch each other will be drawn.
 * Also if the tick marks themselves are so close as to touch each other,
 * the number of ticks will be reduced.
 * Tick marks are drawn only between axis_min and axis_max.
 *
 * NOTE:  This version differs from earlier opnes in that the endpoints of
 *        the axis do not have to be drawn and Can now label left and right
 *        axes with tick_ano turned off.
 *
 * Parameter list:
 *  int side          - indicates which side of the viewport to place axis on
 *                      1=right, 2=top, -1=left, -2=bottom
 *                      0=no ticks, label title at top
 *  int pennumber     - value to set pen to, color depends on palette in use
 *  int vcols         - number of CRT horizontal pixels
 *  int vrows         - number of CRT vertical pixels
 *  int tick_show     -  0 to not draw ticks, 1 to draw on outside of window,
 *                      -1 to draw on inside of window
 *  int tick_num      - >0, suggested number of tick marks for axis (optional)
 *  int tick_ano      - 1 to label tick marks, 0 for no labels
 *  int tick_skip     - number of tick marks to skip between labels
 *  int tick_mid_skip - number of tick marks to skip between 2nd order tick
 *  int ano_left      - number of places to left of the decimal for the tick label
 *  int ano_right     - number of places to right of the decimal for the tick label
 *  double axis_max   - value for top of left or right axes, or right on top or bottom axes
 *  double axis_min   - value for bottom of left or right axes or left on top or bottom axes
 *  double tick_start - value for first tick mark (valid value optional)
 *  double tick_int   - separation between ticks (valid value optional)
 *  double tick_abs_min - no ticks displayed "before" this value
 *  double tick_abs_max - no ticks displayed "after" this value
 *  double title_size - multiplier for axis title csize()
 *  double title_offset - number of characters widths to move title closer (-)
 *                        or farther (+) from axis
 *  double ano_size   - multiplier for tick mark label csize()
 *  double VX1,VX2    - horizontal left and right edges of screen viewport for
 *                      primary data display window (range is 0 to 133.333)
 *  double VY1,VY2    - vertical left and right edges of screen viewport for
 *                      primary data display window (range is 0 to 100.00)
 *                NOTE: viewport() origin is lower left screen corner.
 *  char *title       - axis label (none if NULL string)
 *
 * ERROR messages:
 * const char *DrawAxisMsg[] =
 * {  "DrawAxis(): No errors.",
 *    "DrawAxis() ERROR: Invalid value for \"side\".",
 *    "DrawAxis() WARNING: No room for axis to attach to viewport.",
 *    "DrawAxis() WARNING: Invalid range for viewport limits.",
 *    "DrawAxis() ERROR: Axis endpoints out of SetLabel() range.",
 * } ;
 *
 * Manifest constants:
 *    #define TRUE   1
 *    #define FALSE  0
 *    #define INVALID_VALUE 1.0E19
 *    #define DA_TITLE          0
 *    #define DA_RIGHT          1
 *    #define DA_TOP            2
 *    #define DA_LEFT         (-1)
 *    #define DA_BOTTOM       (-2)
 *    #define _ABS(a)  ( ((a) < 0) ? (-(a)) : (a) )
 *    #define _MAXX(a,b) ( ((a)>(b)) ? (a) : (b) )
 *
 * Globals:
 *  int Debug;
 *
 * NOTES:
 *   1) The graphics in this function require GRO's protected mode graphics
 *      libraries.
 *   2) This function inherits the graphics mode and coordinate systems
 *      of the calling function.  Plotting is 2D only.
 *
 * WARNINGS:
 *   1) The viewport is modified!
 *
 * Requires: <hpgl.h>, <math.h>, "jl_defs.h".
 * Calls: unclip, lorg, ldir, csize, pen, plot, pow, modf, SetLabel, printf.
 * Returns: 0 on success, or
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: September 15, 1995
 */
int DrawAxis (int side,int pen_number,int vcols,int vrows,
              int tick_show,int tick_num,int tick_ano,int tick_skip,int tick_mid_skip,
              int ano_left,int ano_right,double axis_min,double axis_max,
              double tick_start,double tick_int,double tick_abs_min,
              double tick_abs_max,double title_size,double title_offset,
              double ano_size,double VX1,double VX2,double VY1,double VY2,
              char *title)
{
    char   str[20];
    int    itemp,i,left,right,tick_count,inside;
    double max_axis,chr_size,user_per_vp,tick_size,label_off;
    double ano_width,min_ano_sep,last_ano,first_ano;
    double dtemp,mult,frac,ipart;
    double pix_per_vp,axis_range;
    double var1,var2,var3,var4,var5,edge;
    double tick_set[5] = {1.0, 2.0, 2.5, 5.0, 10.0};

    static int set_size  = 5;
    static int tick_pix_width = 5;
    static double pi     = 3.14159265358979323846;
    static double min_VX = 0.0;           /* GRO hpgl lib limits for screen */
    static double min_VY = 0.0;
    static double max_VX;
    static double max_VY = 100.0;
    static double rounding_error = 0.0001;

    extern int Debug;

    /* Reality check */
    if (VX2 == VX1 || VY2 == VY1 || VX2 < VX1 || VY2 < VY1) return 3;
    if (tick_num == 0) tick_num = 9;
    if (side == DA_TITLE) tick_num = 0;
    inside = FALSE;
    if (tick_show == FALSE) tick_num = 0;
    if (tick_show < 0) inside = TRUE;
    if (title_size == 0.0 || title_size == INVALID_VALUE)
    {   if (side) title_size = 0.85;
        else      title_size = 1.10;
    }
    if (ano_size == 0.0 || ano_size == INVALID_VALUE) ano_size = 0.80;
    if (ano_left == INVALID_VALUE)  ano_left  =  0;
    if (ano_left > 6)               ano_left  =  6;
    if (ano_right == INVALID_VALUE) ano_right = -1;
    if (ano_right > 3)              ano_right =  3;

    /* Set viewport to axis area, scale window so x and y user units the same,
     * get maximum viewport axis for csize(), set tick length to 1 viewport
     * horizontal unit.  Viewport origin is lower left screen corner.
     * pix_per_vp converts viewport units to number of pixels.
     */
    max_VX = 100.0 * ((double)vcols/(double)vrows);
    axis_range = _ABS(axis_max-axis_min);
    if (Debug)
    {   printf("\nDrawAxis: xmin=%g xmax=%g ymin=%g ymax=%g\n",min_VX,max_VX,min_VY,max_VY);
        printf("            VX1=%G VX2=%G VY1=%G VY2=%G\n",VX1,VX2,VY1,VY2);
        printf("            axis_min=%g  axis_max=%g  axis_range=%g\n",axis_min,axis_max,axis_range);
        printf("            vcols = %d  vrows = %d  tick_ano = %d tick_skip = %d ano_left = %d  ano_right = %d\n",
                vcols,vrows,tick_ano,tick_skip,ano_left,ano_right);
        printf("            tick_start = %g tick_num = %d  tick_int = %g  tick_abs_min = %g  tick_abs_max = %g\n",
                tick_start,tick_num,tick_int,tick_abs_min,tick_abs_max);
    } else
    {   unclip();
        pen(pen_number);
    }
    switch (side)
    {   case DA_RIGHT:             /* 1: right side */
            if (Debug) puts("DA_RIGHT");
            if (max_VX == VX2) return 2;
            var1 = VX2;
            var2 = max_VX - VX2;
            var3 = VY2 - VY1;
            var4 = max_VY;
            var5 = vrows;
            edge = var1+(var2/var3)*axis_range;
            if (!Debug)  viewport(VX2,max_VX,VY1,VY2);
            break;
        case DA_TITLE:  case DA_TOP:   /* 0 and 2: top side */
            if (Debug) puts("DA_TITLE or DA_TOP");
            if (max_VY == VY2) return 2;
            var1 = VY2;
            var2 = max_VY - VY2;
            var3 = VX2 - VX1;
            var4 = max_VX;
            var5 = vcols;
            edge = var1+(var2/var3)*axis_range;
            if (!Debug)  viewport(VX1,VX2,VY2,max_VY);
            break;
        case DA_LEFT:             /* left side */
            if (Debug) puts("DA_LEFT");
            if (VX1 == min_VX) return 2;
            var1 = VX1;
            var2 = VX1 - min_VX;
            var3 = VY2 - VY1;
            var4 = max_VY;
            var5 = vrows;
            edge = var1-(var2/var3)*axis_range;
            if (!Debug)  viewport(min_VX,VX1,VY1,VY2);
            break;
        case DA_BOTTOM:             /* bottom side */
            if (Debug) puts("DA_BOTTOM");
            if (VY1 == min_VY) return 2;
            var1 = VY1;
            var2 = VY1 - min_VY;
            var3 = VX2 - VX1;
            var4 = max_VX;
            var5 = vcols;
            edge = var1-(var2/var3)*axis_range;
            if (!Debug)  viewport(VX1,VX2,min_VY,VY1);
            break;
        default:
            return 1;
    }
    if (var3 > var2)
    {   max_axis = var3;
        user_per_vp = axis_range/max_axis;
    } else
    {   max_axis = var2;
        user_per_vp = _ABS(edge-var1)/max_axis;
    }
    tick_size = _ABS(edge-var1)/(var2);
    pix_per_vp = var5 / var4;
    if (tick_num != 0)
    {   while ((var3/tick_num)*pix_per_vp < tick_pix_width)
        {   tick_num /= 2;
            if (tick_num == 0) break;
        }
    }
    if (!Debug)
    {   switch (side)
        {   case DA_RIGHT:
                window(VX2,edge,axis_min,axis_max);
                break;
            case DA_TITLE:
            case DA_TOP:
                window(axis_min,axis_max,VY2,edge);
                break;
            case DA_LEFT:
                window(edge,VX1,axis_min,axis_max);
                break;
            case DA_BOTTOM:
                window(axis_min,axis_max,edge,VY1);
                break;
        }
    }

    /* Calculate sizes;
     * csize(1.0) == 2% of maximum viewport axis;
     * chr_size is title_size * 2% of full screen viewport vertical axis (100.0);
     * chr_size and ano_size are in viewport units;
     * user_per_vp converts viewport units to user units;
     * ano_width is character block width and height in user units;
     * min_ano_sep will have to be calculated on the fly for horizontal axes;
     */
    tick_size  *= 0.90;                           /* reduce ticksize slighty */
    label_off = tick_size;
    if (inside)
    {   tick_size *= -1.0;
        label_off *= 0.5;
    }
    ano_size   *= max_VY / max_axis;              /* csize for tick annotation */
    ano_width   = ano_size * 0.02 * max_axis * user_per_vp;
    min_ano_sep = 1.5 * ano_width;                /* this works good for left and right axes */
    chr_size    = title_size * (max_VY/max_axis); /* csize for axis labels */
    if (Debug)
        printf("side=%d tick_num=%d chr_size=%g max_axis=%g user_per_vp=%g\ntick_size=%g ano_width=%g ano_size=%g\n",
                side,tick_num,chr_size,max_axis,user_per_vp,tick_size,ano_width,ano_size);

    /* Draw axis and labels */
    if (side)
    {   /* Calculate interval between tick marks and starting tick, if nec. */
        if (tick_num && (tick_int==INVALID_VALUE || tick_start==INVALID_VALUE
                                                 || tick_int==0.0))
        {   if (tick_int==INVALID_VALUE || tick_int==0.0)
                tick_int = (axis_max-axis_min)/tick_num;   /* user units */
            left = 0;  right = -1;
            itemp = SetLabel(&left,&right,tick_int,str);
            if (itemp) return 4;
            if (left > 0)  mult = pow(10.0,(double)left-1.0);
            if (left < 0)  mult = pow(10.0,(double)(-left)-1.0);
            dtemp = _ABS(tick_int);
            if (left==1 && dtemp<1.0)
            {   if (dtemp >= 0.001) mult = 0.001;
                if (dtemp >= 0.01)  mult = 0.01;
                if (dtemp >= 0.1)   mult = 0.1;
            }
            tick_set[0] *= mult;
            for (i=1; i<set_size; i++)
            {   tick_set[i] *= mult;
                if (dtemp>=tick_set[i-1] && dtemp<tick_set[i])
                {   if (dtemp < (tick_set[i]-tick_set[i-1])/2.0)
                        dtemp = tick_set[i-1];
                    else
                        dtemp = tick_set[i];
                    break;
                }
            }
            if (tick_int > 0) tick_int =  dtemp;
            else              tick_int = -dtemp;
            if (tick_start == INVALID_VALUE)
            {   frac = modf(axis_min/tick_int,&ipart);
                if (_ABS(frac) < rounding_error) tick_start = axis_min;
                else                             tick_start = ipart * tick_int;
            }
        }
        if (Debug)
        {   printf("tick_int=%g  tick_start=%g  tick_skip=%d  tick_ano=%d\n",
                tick_int,tick_start,tick_skip,tick_ano);
            if (!getch()) getch();
        }

        /* Draw ticks and label */
        first_ano  = INVALID_VALUE;
        if (tick_num)
        {   tick_count = 0;
            tick_skip++;      /* turn 0 into 1, 1 into 2, etc. for modulo */
            tick_mid_skip++;  /* turn 0 into 1, 1 into 2, etc. for modulo */
            switch (side)
            {   case DA_RIGHT:      /* right side */
                    if (!Debug)
                    {   ldir(0.0);  lorg(2);  csize(ano_size);
                    }
                    if (tick_ano)
                    {   if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(VX2,dtemp,-2);
                                plot(VX2+tick_size,dtemp,-1);
                            } else if (tick_mid_skip > 0 &&  (tick_count % tick_mid_skip)==0)
                            {   plot(VX2,dtemp,-2);
                                plot(VX2+0.75*tick_size,dtemp,-1);
                            } else
                            {   plot(VX2,dtemp,-2);
                                plot(VX2+0.5*tick_size,dtemp,-1);
                            }
                        } else
                            printf("RIGHT tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if (Debug)
                            {   printf("           diff1=%g  diff2=%g  min_sep=%g  last_ano=%g\n",_ABS(dtemp-last_ano),_ABS(axis_max-dtemp),min_ano_sep,last_ano);
                                printf("     count=%d skip=%d mod=%d\n",tick_count,tick_skip,tick_count%tick_skip);
                            }
                            if ((tick_count % tick_skip) == 0)
                            {   if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   left = ano_left;  right = ano_right;
                                    SetLabel(&left,&right,dtemp,str);
                                    if (!Debug) label(VX2+label_off,dtemp,str);
                                    else        printf("     skip=%d left=%d right=%d x=%g y=%g\n",tick_skip,ano_left,ano_right,VX2+tick_size,dtemp);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE)  first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
                case DA_LEFT:     /* left side */
                    if (!Debug)
                    {   lorg(8);  ldir(0.0);  csize(ano_size);
                    }
                    if (tick_ano)
                    {   if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(VX1,dtemp,-2);
                                plot(VX1-tick_size,dtemp,-1);
                            } else if (tick_mid_skip > 0 && (tick_count % tick_mid_skip)==0)
                            {   plot(VX1,dtemp,-2);
                                plot(VX1-0.75*tick_size,dtemp,-1);
                            } else
                            {   plot(VX1,dtemp,-2);
                                plot(VX1-0.5*tick_size,dtemp,-1);
                            }
                        } else
                            printf("LEFT tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if (Debug)
                            {   printf("           diff1=%g  diff2=%g  min_sep=%g last_ano=%g test=%d\n",_ABS(dtemp-last_ano),_ABS(axis_max-dtemp),min_ano_sep,last_ano,((_ABS(dtemp-last_ano) > min_ano_sep) && (_ABS(axis_max-dtemp) > min_ano_sep)));
                                printf("     count=%d skip=%d mod=%d\n",tick_count,tick_skip,tick_count%tick_skip);
                            }
                            if ((tick_count % tick_skip) == 0)
                            {   if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   left = ano_left;  right = ano_right;
                                    SetLabel(&left,&right,dtemp,str);
                                    if (!Debug) label(VX1-label_off,dtemp,str);
                                    else        printf("     skip=%d left=%d right=%d x=%g y=%g\n",tick_skip,ano_left,ano_right,VX1-tick_size,dtemp);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE)  first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
                case DA_TOP:      /* top side */
                    if (!Debug)
                    {   lorg(6);  ldir(0.0);  csize(ano_size);
                    }
                    if (tick_ano)
                    {   left = ano_left;  right = ano_right;
                        SetLabel(&left,&right,axis_min,str);
                        min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                        if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                        if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(dtemp,VY2,-2);
                                plot(dtemp,VY2+tick_size,-1);
                            } else if (tick_mid_skip > 0 && (tick_count % tick_mid_skip)==0)
                            {   plot(dtemp,VY2,-2);
                                plot(dtemp,VY2+0.75*tick_size,-1);
                            } else
                            {   plot(dtemp,VY2,-2);
                                plot(dtemp,VY2+0.5*tick_size,-1);
                            }
                        } else
                            printf("TOP tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if ((tick_count % tick_skip) == 0)
                            {   left = ano_left;  right = ano_right;
                                SetLabel(&left,&right,dtemp,str);
                                min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                                if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                                if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   if (!Debug) label(dtemp,VY2+2.0*label_off,str);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE) first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
                case DA_BOTTOM:     /* bottom side */
                    if (!Debug)       /* draw leftmost tick */
                    {   lorg(4); ldir(0.0);  csize(ano_size);
                    }
                    if (tick_ano)     /* label leftmost tick */
                    {   left = ano_left;  right = ano_right;
                        SetLabel(&left,&right,axis_min,str);
                        min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                        if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                        if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(dtemp,VY1,-2);
                                plot(dtemp,VY1-tick_size,-1);
                            } else if (tick_mid_skip > 0 && (tick_count % tick_mid_skip)==0)
                            {   plot(dtemp,VY1,-2);
                                plot(dtemp,VY1-0.75*tick_size,-1);
                            } else
                            {   plot(dtemp,VY1,-2);
                                plot(dtemp,VY1-0.5*tick_size,-1);
                            }
                        } else
                            printf("BOTTOM tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if (Debug) printf("     count=%d skip=%d mod=%d\n",tick_count,tick_skip,tick_count%tick_skip);
                            if (!(tick_count % tick_skip))
                            {   left = ano_left;  right = ano_right;
                                SetLabel(&left,&right,dtemp,str);
                                min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                                if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                                if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   if (!Debug) label(dtemp,VY1-2.0*label_off,str);
                                    else        printf("     left=%d right=%d x=%g y=%g\n",left,right,dtemp,VY1-2.0*tick_size);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE)  first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
            } /* end of switch block */
        } /* end of if (tick_num) */

        if (title[0] != 0)
        {   int left1,left2,right1,right2;

            csize(chr_size);
            switch (side)
            {   case DA_RIGHT:      /* right side */
                    if (first_ano != INVALID_VALUE)
                    {   left = ano_left;  right = ano_right;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,first_ano,str);
                        left1 = ano_left;  right1 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left1 < 0) left1 = -left1 + 1;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,last_ano,str);
                        left2 = ano_left; right2 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left2 < 0) left2 = -left2 + 1;
                        left = _MAXX(left1,left2);
                        right = _MAXX(right1,right2);
                    } else
                    {   left = right = 0;
                    }
                    if (!Debug)
                    {   lorg(4);  ldir(pi/2.0);
                        label( VX2+((double)left+right+1.5+title_offset)*ano_width,
                               (axis_max+axis_min)/2.0, title );
                    } else
                    {   printf("left1=%d left2=%d left=%d right1=%d right2=%d right=%d\n",
                            left1,left2,left,right1,right2,right);
                        printf("label at x=%g y=%g\n",
                            VX2+((double)left+right+1.5+title_offset)*ano_width,(axis_max+axis_min)/2.0);
                    }
                    break;
                case DA_LEFT:
                    if (first_ano != INVALID_VALUE)
                    {   left = ano_left;  right = ano_right;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,first_ano,str);
                        left1 = ano_left;  right1 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left1 < 0) left1 = -left1 + 1;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,last_ano,str);
                        left2 = ano_left; right2 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left2 < 0) left2 = -left2 + 1;
                        left = _MAXX(left1,left2);
                        right = _MAXX(right1,right2);
                    } else
                    {   left = right = 0;
                    }
                    if (!Debug)
                    {   lorg(6);  ldir(pi/2.0);
                        label( VX1-((double)left+right+2.5+title_offset)*ano_width,
                               (axis_max+axis_min)/2.0, title );
                    } else
                    {   printf("left1=%d left2=%d left=%d right1=%d right2=%d right=%d\n",
                            left1,left2,left,right1,right2,right);
                        printf("label at x=%g y=%g\n",
                            VX1-((double)left+right+2.5+title_offset)*ano_width,(axis_max+axis_min)/2.0);
                    }
                    break;
                case DA_TOP:
                    if (!Debug)
                    {   lorg(6);  ldir(0.0);
                        label( (axis_max+axis_min)/2.0, VY2+(3+title_offset)*ano_width, title );
                    }
                    break;
                case DA_BOTTOM:
                    if (!Debug)
                    {   lorg(4);  ldir(0.0);
                        label( (axis_max+axis_min)/2.0, VY1-(3+title_offset)*ano_width, title );
                    }
                    break;
            }   /* end of switch block */
        }    /* if (title[0]) */
    } else /* if (side) */
    {   if (!Debug)
        {   lorg(6);  ldir(0.0);  csize(chr_size);
            label((axis_max+axis_min)/2.0,VY2+(4+title_offset)*label_off,title); /* title label at top */
        }
    }

    return 0;
}
/******************************** SetLabel() ********************************/
/*
 * This function formats a floating point value into a string.
 *
 * Parameter list:
 *  int *left  - address of variable holding number of characters to left of
 *               of the decimal.  If == 0, then this function determines the
 *               count up to 6.  Otherwise, this function uses the value in
 *               "left" to format the string.
 *  int *right - address of variable holding number of characters to right of
 *               of the decimal.  If == -1, then this function determines the
 *               count up to 3.  Otherwise, this function uses the value in
 *               "right" to format the string.
 *  double val - the floating point value.
 *  char *str  - pointer to a buffer large enough to hold the string.
 *               15 would be sufficient.
 *
 * Requires: <math.h>, <stdio.h>, "jl_defs.h".
 * Calls: ceil, log10, modf, sprintf, _ABS, _MAXX, printf.
 * Returns: 0 on success, or
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: March 7, 1994
 */
int SetLabel (int *left,int *right,double val,char *str)
{
    char fmt[12];
    int lft;
    double frac, ipart, dtemp;
    double delta = 1.0e-7;      /* correct for floatingpoint round off error */
    extern int Debug;

    if (Debug) printf("SetLabel(): left=%d right=%d val=%g\n",*left,*right,val);
    /* Get number of characters to the left of the decimal */
    if (*left == 0)
    {   if (val != 0.0)
        {   if (val > 0) *left = _MAXX(1,(int)ceil(log10(_ABS(val)+.001)));
            else         *left = _MAXX(1,(int)ceil(log10(_ABS(val)-.001)));
        }
        else            *left = 1;
        if (val < 0.0)  *left *= -1;
    }
    if (*left > 0 && val < 0) *left *= -1;
    if (*left > 6 || *left < -6) return 1; /* number too large */

    /* Get number of characters to the right of the decimal */
    /* NOTE: this part doesn't seem to always work correctly ! */
    if (*right < 0)
    {   *right = 0;
        dtemp = _ABS(val);
        frac = (modf(dtemp,&ipart));       /* get the fraction */
        dtemp = (frac * 10.0) + delta;
        frac = modf(dtemp,&ipart);         /* is there a digit in tenths? */
        if (ipart > 0.0)  (*right) = 1;
        dtemp = (frac * 10.0) + delta;
        frac = modf(dtemp,&ipart);         /* is there a digit in hundredths? */
        if (ipart > 0.0)  (*right) = 2;
        dtemp = (frac * 10.0) + delta;
        frac = modf(dtemp,&ipart);         /* is there a digit in thousandths? */
        if (ipart > 0.0)  (*right) = 3;
    } else
       if (*right > 3) (*right) = 3;
    lft = *left;
/********/
    if (*right == 0)
    {   if (val > 0) sprintf(str,"%d",(int)(val+0.000001));
        else         sprintf(str,"%d",(int)(val-0.000001));
    }
/********/
    else
    {   if (lft < 0) lft = -lft + 1;
        sprintf(fmt,"%c%d.%df",'%',lft + *right + 1, *right);
        sprintf(str,fmt,val);
        if (Debug)  printf("   fmt=%s  str=%s\n",fmt,str);
    }
    return 0;
}
/***************************** GetLblFileArgs() *****************************/
/* Get labeling parameters from user from a command file and implement HPGL
 * commands.  The label file must look just like the "C" code would look that
 * would implement the HPGL commands.  Semicolons separate multiple
 * expressions on the same line and comments are allowed (anything past "/*"
 * is ignored).  Blank lines and invalid expressions are ignored.  Only a
 * limited subset of HPGL expressions is implemented at this time.  Loops, if
 * blocks, etc. and local variables are not supported at this time.
 *
 * const char *HPGL_LBLS[]=
 * {   "viewport","window","pen","ldir","csize",
 *     "lorg","plot","label","hpgl_select_font","frame",
 *     "clip","unclip","linetype","circle","arc",
 *     "wedge","ellipse","fill","unfill","rectangle",
 *     NULL,
 * } ;
 *
 * NOTES:
 *   1) The graphics in this function require GRO's protected mode graphics
 *      libraries.
 *   2) This function inherits the graphics mode and coordinate systems
 *      of the calling function.
 *
 * Parameter list:
 *    char *filename - name of label file
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, <hpgl.h>, attached header.
 * Calls: fopen, fclose, fgets, atoi, atof, strcpy, strstr, strncat, strchr,
 *        strcmp, Trimstr, printf,
 *        viewport,window,pen,ldir,csize,lorg,plot,label,hpgl_select_font,
 *        frame,clip,unclip,linetype,circle,arc,wedge,ellipse.
 * Returns: 0 on success,
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: October 27, 1995
 */
int GetLblFileArgs (char *filename)
{
    char str[MAX_CMDLINELEN],*cp,*cp2,*cp_old,cmdstr[20];
    char cParam1[MAX_CMDLINELEN];
    int found,i,num,loop;
    int iParam1;
    double dParam1,dParam2,dParam3,dParam4,dParam5,dParam6;
    FILE *infile = NULL;
    extern int Debug;

    if ((infile = fopen(filename,"rt")) == NULL)  return 1;

    while (fgets(str,MAX_CMDLINELEN,infile)) /* read till EOF or error */
    {   loop = TRUE;
        cp_old = str;                 /* point to start of string */
        cp = strstr(cp_old,"/*");     /* look for start of C comment */
        if (cp) *cp = 0;              /* terminate string at start of comment */
        while (loop == TRUE)
        {   cmdstr[0] = 0;            /* set to 0 so strncat works right */
            cp2 = cp_old;             /* point to start of string */
            cp = strchr(cp2,'(');     /* look for left parens */
            cp_old = strchr(cp2,';'); /* look for semicolon */
            if (cp_old == NULL) loop = FALSE;
            else  cp_old++;           /* step past semicolon */
            if (cp == NULL) continue; /* invalid label line so ignore */
            num = (int)(cp-cp2);      /* number of chars before left parens */
            strncat(cmdstr,cp2,(size_t)num);  /* copy to working string */
            TrimStr(cmdstr);          /* remove leading and trailing blanks */
            found = -1;
            for (i=0; HPGL_LBLS[i] != NULL; i++)
            {   if (strcmp(cmdstr,HPGL_LBLS[i]) == 0)
                {   found = i;        /* store index into HPGL_LBLS[] */
                    break;
                }
            }
            if (found == -1) continue;    /* bad or missing label word */
            cp++;                         /* step 1 past left parens */
            cp2 = strchr(cp,')');         /* look for right parens */
            if (cp2 == NULL) continue;    /* invalid label line so ignore */
            switch (found)  /* cases depend on same order in HPGL_LBLS[] */
            {   case 0:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,')');    /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    if (Debug) printf("viewport(%g,%g,%g,%g)\n",dParam1,dParam2,dParam3,dParam4);
                    else       viewport(dParam1,dParam2,dParam3,dParam4);
                    break;
                case 1:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    if (Debug) printf("window(%g,%g,%g,%g)\n",dParam1,dParam2,dParam3,dParam4);
                    else       window(dParam1,dParam2,dParam3,dParam4);
                    break;
                case 2:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    iParam1 = atoi(cp);
                    if (Debug) printf("pen(%d)\n",iParam1);
                    else       pen(iParam1);
                    break;
                case 3:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    if (Debug) printf("ldir(%g)\n",dParam1);
                    else       ldir(dParam1);
                    break;
                case 4:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    if (Debug) printf("csize(%g)\n",dParam1);
                    else       csize(dParam1);
                    break;
                case 5:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    iParam1 = atoi(cp);
                    if (Debug) printf("lorg(%d)\n",iParam1);
                    else       lorg(iParam1);
                    break;
                case 6:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    iParam1 = atoi(cp);
                    if (Debug) printf("plot(%g,%g,%d)\n",dParam1,dParam2,iParam1);
                    else       plot(dParam1,dParam2,iParam1);
                    break;
                case 7:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = strchr(cp2+1,'"');   /* look for first double quote past second comma */
                    if (cp == NULL) break;    /* invalid label line so ignore */
                    cp++;                     /* step past first double quote */
                    cp2 = strchr(cp,'"');     /* look for second double quote */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    strcpy(cParam1,cp);
                    if (Debug) printf("label(%g,%g,\"%s\")\n",dParam1,dParam2,cParam1);
                    else       label(dParam1,dParam2,cParam1);
                    break;
                case 8:
                    cp2 = strchr(cp,'"');     /* look for first double quote past second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    cp = cp2+1;               /* step past first double quote */
                    cp2 = strchr(cp,'"');     /* look for second double quote */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    strcpy(cParam1,cp);
                    if (Debug) printf("hpgl_select_font(\"%s\")\n",cParam1);
                    else       hpgl_select_font(cParam1);
                    break;
                case 9:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    if (Debug) puts("frame()");
                    else       frame();
                    break;
                case 10:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    if (Debug) puts("clip()");
                    else       clip();
                    break;
                case 11:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    if (Debug) puts("unclip()");
                    else       unclip();
                    break;
                case 12:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    iParam1 = atoi(cp);
                    if (Debug) printf("linetype(%d)\n",iParam1);
                    else       linetype(iParam1);
                    break;
                case 13:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    if (Debug) printf("circle(%g,%g,%g,%g)\n",dParam1,dParam2,dParam3,dParam4);
                    else       circle(dParam1,dParam2,dParam3,dParam4);
                    break;
                case 14:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,',');     /* look for fourth comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    cp = cp2+1;               /* step past fourth comma */
                    cp2 = strchr(cp,',');     /* look for fifith comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam5 = atof(cp);
                    cp = cp2+1;               /* step past fifth comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam6 = atof(cp);
                    if (Debug)
                        printf("arc(%g,%g,%g,%g,%g,%g)\n",
                               dParam1,dParam2,dParam3,dParam4,dParam5,dParam6);
                    else
                        arc(dParam1,dParam2,dParam3,dParam4,dParam5,dParam6);
                    break;
                case 15:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,',');     /* look for fourth comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    cp = cp2+1;               /* step past fourth comma */
                    cp2 = strchr(cp,',');     /* look for fifith comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam5 = atof(cp);
                    cp = cp2+1;               /* step past fifth comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam6 = atof(cp);
                    if (Debug)
                        printf("wedge(%g,%g,%g,%g,%g,%g)\n",
                               dParam1,dParam2,dParam3,dParam4,dParam5,dParam6);
                    else
                        wedge(dParam1,dParam2,dParam3,dParam4,dParam5,dParam6);
                    break;
                case 16:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,',');     /* look for fourth comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    cp = cp2+1;               /* step past fourth comma */
                    cp2 = strchr(cp,',');     /* look for fifith comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam5 = atof(cp);
                    cp = cp2+1;               /* step past fifth comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam6 = atof(cp);
                    if (Debug)
                        printf("ellipse(%g,%g,%g,%g,%g,%g)\n",
                               dParam1,dParam2,dParam3,dParam4,dParam5,dParam6);
                    else
                        ellipse(dParam1,dParam2,dParam3,dParam4,dParam5,dParam6);
                    break;
                case 17:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    if (Debug) puts("fill()");
                    else       fill();
                    break;
                case 18:
                    cp2 = strchr(cp,')');     /* look for right parenthesis */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    if (Debug) puts("unfill()");
                    else       unfill();
                    break;
                case 19:
                    cp2 = strchr(cp,',');     /* look for first comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam1 = atof(cp);
                    cp = cp2+1;               /* step past first comma */
                    cp2 = strchr(cp,',');     /* look for second comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam2 = atof(cp);
                    cp = cp2+1;               /* step past second comma */
                    cp2 = strchr(cp,',');     /* look for third comma */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam3 = atof(cp);
                    cp = cp2+1;               /* step past third comma */
                    cp2 = strchr(cp,')');     /* look for right parens */
                    if (cp2 == NULL) break;   /* invalid label line so ignore */
                    *cp2 = 0;
                    dParam4 = atof(cp);
                    if (Debug) printf("rectangle(%g,%g,%g,%g)\n",dParam1,dParam2,dParam3,dParam4);
                    else       rectangle(dParam1,dParam2,dParam3,dParam4);
                    break;
                default:
                    break;
            }  /* end of switch block */
        }  /* end of while loop to get more label commands */
    }  /* end of while loop to get records */
    fclose(infile);
    return 0;
}
/***************************** SetDataViewport() ****************************/
/* This function scales the viewport so that vertical units are the same as
 * horizontal units.  The viewport may shrink to insure same scales but it
 * won't expand.
 *
 * Parameter list:
 *  int vcols        - number of CRT horizontal pixels
 *  int vrows        - number of CRT vertical pixels
 *  double vert_exag - amount of vertical exaggeration for display
 *  double xmin,xmax - horizontal minimum and maximum in user units for window
 *  double ymin,ymax - vertical minimum and maximum in user units for window
 *  double *X1,*X2   - addresses of variables, horizontal left and right edges
 *                       of screen viewport for primary data display window
 *                       (range is 0 to 133.333)
 *  double *Y1,*Y2   - addresses of variables, vertical left and right edges
 *                       of screen viewport for primary data display window
 *                       (range is 0 to 100.00)
 *
 * NOTE: The graphics in this function require GRO's protected mode graphics
 *         libraries.
 *       This function ignores the fact that in certain modes, horizontal
 *          pixels are not the same width as vertical pixels.
 *       viewport() origin is lower left screen corner.
 *
 * Requires: <hpgl.h>, <stdio.h>, "gpr_disp.h".
 * Calls: viewport, window, clip, _ABS, printf.
 * Returns: 0 if success, or
 *         >0 on error (offset int message strings arra).
 *
const char *SetDataViewportMsg[] =
{   "SetDataViewport(): No errors.",
    "SetDataViewport() ERROR: X1 == X2 or Y1 == Y2.",
    "SetDataViewport() ERROR: xmin == xmax or ymin == ymax.",
} ;
 *
 * Author: Jeff Lucius   USGS   Crustal Imaging and Characterization Team
 * Date: April 30, 1994
 */
int SetDataViewport (int vcols,int vrows,double vert_exag,
                     double xmin,double xmax,double ymin,double ymax,
                     double *X1,double *X2,double *Y1,double *Y2)
{
    double dtemp,XYratio,xyratio,midX,midY;
    extern int Debug;

    /* Scale viewport to selected data */
    if ((*X2 == *X1) || (*Y2 == *Y1)) return 1;
    if ((xmax == xmin) || (ymax == ymin)) return 2;
    if (vert_exag <= 0.0) vert_exag = 1.0;
    XYratio = (*X2 - *X1) / (*Y2 - *Y1);
    xyratio = _ABS(xmax-xmin)/(_ABS(ymax-ymin)*vert_exag);
    midX = (*X1 + *X2) / 2.0;
    midY = (*Y1 + *Y2) / 2.0;
    if (xyratio > XYratio)
    {   dtemp = ((_ABS(*X2 - *X1)/_ABS(xmax-xmin))*(_ABS(ymax-ymin)*vert_exag))/2.0;
        *Y1 = midY - dtemp;
        *Y2 = midY + dtemp;
    } else
    {   dtemp = ((_ABS(*Y2 - *Y1)/(_ABS(ymax-ymin)*vert_exag))*_ABS(xmax-xmin))/2.0;
        *X1 = midX - dtemp;
        *X2 = midX + dtemp;
    }

    if (Debug)
    {   printf("SetDataViewport():\nxmin=%g xmax=%g ymin=%g ymax=%g\n",xmin,xmax,ymin,ymax);
        printf("X1=%g  X2=%g  Y1=%g  Y2=%g\n",*X1,*X2,*Y1,*Y2);
        printf("vcols=%d  vrows=%d\n",vcols,vrows);
        getch();
    } else
    {   viewport(*X1,*X2,*Y1,*Y2);    /* select screen area to use */
        window(xmin,xmax,ymin,ymax);  /* assign user units to viewport */
        clip();                       /* prevent plotting outside of window */
    }
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
    pal[0] = 0;            /* black */
    pal[1] = 0;
    pal[2] = 0;
    pal[255*3]     = 255;  /* white */
    pal[255*3 + 1] = 255;
    pal[255*3 + 2] = 255;

    palette_256(pal);
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
    pal[0] = 0;            /* black */
    pal[1] = 0;
    pal[2] = 0;
    pal[255*3]     = 255;  /* white */
    pal[255*3 + 1] = 255;
    pal[255*3 + 2] = 255;

    palette_256(pal);
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

    pal[0] = 0;            /* black */
    pal[1] = 0;
    pal[2] = 0;
    pal[255*3]     = 255;  /* white */
    pal[255*3 + 1] = 255;
    pal[255*3 + 2] = 255;

    palette_256(pal);
}
/*********************** end of graphics functions **************************/
/**************************** end of gpr_disp.c *****************************/

