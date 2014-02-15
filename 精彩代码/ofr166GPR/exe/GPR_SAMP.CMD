; This file is GPR_SAMP.CMD, the default command file (or parameter file) for
; the program GPR_SAMP.EXE version 1.0
;
; Last Revision Date: September 11, 1997
;-----------------------------------------------------------------------------
; GPR_SAMP.DOC
;
; GPR_SAMP v. 1.0 resamples digital ground penetrating radar data.  The number
; of trace and/or samples can be reduced and the number of samples per trace
; can be increased or decreased.  The following computer storage formats are 
; automatically recognized.
;
;   GSSI SIR-10A version 3.x to 5.x binary files, 8-bit or 16-bit unsigned
;     integers with an embedded descriptive 512- or 1024-byte header.
;     Data files usually have the extension "DZT".
;   Sensors and Software pulseEKKO binary files, 16-bit signed integers
;     accompanied by a descriptive ASCII "HD" file.  Data files must have
;     the extension "DT1".
;   SEG-Y formatted files.  Data files usually have the extension "SGY".
;
; If the storage format does not conform to any of the above or this
; program is having trouble reading the file correctly, there are
; options in the CMD file for the user to specify required parameters.
;
; After the data have been manipulated, they are stored to disk in the same 
; format as they were read in.  An exception is made for early versions GSSI
; DZT files that had 512-byte headers.  A current (version 5.x) 1024-byte 
; header is written into the output file.  File extensions are forced to
; DZT for GSSI files and to DT1 for PulseEKKO files.
;
; To convert from one GPR storage format to another, use program
; GPR_CONV.EXE.  Because some storage formats maintain information about
; the data in each trace header, both the trace header and trace
; data block are stored as a unit as one column in the program's internal
; storage grid.  If the size of the trace header is not an even multiple of
; the data element (sample) size, the program will stop and report an error.
; This should only be a problem with user-defined storage formats.
;
; The input to this program is a "CMD" file, an ASCII text file containing
; commands (or parameters) describing how to process the radar data.  The
; CMD file specifies the data file name (and optionally the storage format).
; Inspect the example file GPR_SAMP.CMD for usage.
;
; NOTES:
;   Only 1 to 4 headers are supported in DZT files.
;   There is no graphic display of the data.
;   To change storage formats use GPR_CONV.EXE.
;   To process the data use GPR_PROC.EXE.
;   To display the processed data, use programs such as GPR_DISP.EXE or
;     FIELDVEW.EXE.
;
; Jeff Lucius, Geophysicist
; U.S. Geological Survey, Geologic Division
; Central Mineral Resources Program
; Box 25046  Denver Federal Center  MS 964
; Denver, CO  80225-0046
; 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
; email: lucius@usgs.gov
;-----------------------------------------------------------------------------
; GPR_SAMP is controlled by a "CMD" file, an ASCII text file containing
; commands and parameters describing how to process the radar data.
;
; RULES FOR CONSTRUCTING "GPR_SAMP.CMD" FILES:
;  1. All valid keywords and their defaults are shown here.
;  2. Keywords can be upper, lower, or mixed case.
;  3. Only lines with an equal sign and valid keyword to left of the equal
;       sign will be used.  All OTHERS ARE IGNORED (except for sets of data,
;       see 12 below).  Blank lines are ignored everywhere.
;  4. Equal signs don't have to line up.
:  5. Spaces are ignored (and removed) except those within strings.
;  6. A semicolon begins a comment, except within strings.
;  7. Strings must be enclosed in double quotation marks.
;       Examples: dat_infilename = "file33.dzt"     
;  8. "TRUE" == 1. "FALSE" == 0.  "INVALID_VALUE" == 1.0E19.
;  9. Sets of numbers or strings must be separated by a space or a
;       "new line" characters (ie. a CR/LF pair from pressing <Enter>).
; 10. Keywords can be given in any order, except the num_... keywords which
;       must precede the arrays, or sets of data, they are describing.
; 11. Lines should be less than 160 characters long (characters past 159 are
;      ignored).
; 12. Keywords that expect sets of data end in square brackets.
;      Example: num_gain_on = 2
;               gain_on[]   = 6.0 6.0    ; multiply data by 2.0
; 13. Almost all values have defaults except for the input and output data
;       filenames.  These must be specified.
; 14. Parameters or commands may appear more than once, but usually only the
;       last instance is preserved (ie., earlier values are lost or written
;       over).
;
;-----------------------------------------------------------------------------
; The statements below give a commented listing of the default command file.
; The related file GPR_SAMP.CM_ is this same file with most comments
; stripped out.
;
  ;*************************** PROGRAM CONTROL *******************************
batch        = "FALSE" ; place program in batch mode (no pauses) if "TRUE";
  ;
debug        = "FALSE" ; place program in debug mode if "TRUE" (for developers)
  ;
  ;********************* SPECIFICATION OF INPUT DATA *************************
  ; The storage format is determined by inspecting the file.  If the program
  ; cannot recognize a flavor of the three formats below then an error
  ; message may be issued.
  ; Recognized storage formats are:
  ;   DZT     GSSI SIR-10A file with embedded (512- or 1024-byte) info header
  ;   DT1     Sensors & Software pulseEKKO file with matching HD info file
  ;   SGY     SEG SEG-Y format
  ; DT1 and HD files are assumed paired, i.e. both have same filename with
  ; different extensions.  So, if a data file with a ".DT1" extension is
  ; specified, the ".HD" filename will be assumed.  Only DT1/HD files must
  ; have those filename extensions.
  ;
  ; Ramac data files can be read by assigning correct values to the next five
  ; parameters.
  ;
dat_infilename  = ""  ; input GPR binary data file name
  ;
  ;############################## NOTE #######################################
  ; >> IF << the GPR format DOES NOT CONFORM to any of the above formats
  ; then the next five parameters, file_header_bytes, trace_header_bytes,
  ; samples_per_trace, total_time, and input_datatype, MUST be specified.
  ; Otherwise, ignore them.
  ; If you just want to convert the storage format (no processing performed)
  ; then GPR_CONV is the program to use.
  ;###########################################################################
  ;
file_header_bytes  = 0  ; replace with number of bytes in the file header.
  ;         PulseEKKO data files do not have a file header - the information
  ;         is held in another file with a .HD extension.  GSSI files have
  ;         either a 512-byte (old style) or 1024-byte (current style) header.
  ;         However, DZT file can have up to 4 file headers - one for each 
  ;         channel.  SEG-Y file have a 3600-byte header.  Ramac data files 
  ;         have no file header.
  ;
trace_header_bytes = 0  ; replace with number of bytes in each trace header.
  ;         For PulseEKKO files, each GPR trace is preceded by a 128-byte
  ;         header.  For GSSI files, the traces are not preceeded by a
  ;         header, but the first 2 samples (not necessarily bytes) are
  ;         reserved.  SEG-Y files have 240-byte trace header.  Ramac data files
  ;         have no trace neaders.
  ;
samples_per_trace  = 0  ; replace with number of samples per trace.
  ;         For PulseEKKO data, the number of samples per trace is recorded
  ;         in the HD file (NUMBER OF PTS/TRC).  For GSSI data, the number
  ;         of samples per trace is a power of 2 greater than 128 up to
  ;         65536, typically 256, 512, or 1024.  The information is recorded
  ;         in the .DZT file header in the rh_nsamp field.  For Ramac files,
  ;         the RAD text file records the number of samples.  For SEG-Y files,
  ;         look in the comment area of the file header.
  ;
total_time         = 0  ; replace with total number of nanoseconds per trace.
  ;         For PulseEKKO data, look at the "TOTAL TIME WINDOW" field in the
  ;         .HD file.  For GSSI data the value is recorded in the file header.
  ;         For SEG-Y files, look in the comment area of the file header.  For
  ;         Ramac files, the TIMEWINDOW parameter records the time per trace
  ;         in microseconds (multiply by 1000 to get ns).
  ;
input_datatype     = 0  ; type of input data element, replace with one
  ;                       of the following element types:
  ;          1 for 1-byte signed chars
  ;         -1 for 1-byte unsigned chars            (GSSI)
  ;          2 for 2-byte signed ints or shorts     (PulseEKKO, Ramac, SEG-Y)
  ;         -2 for 2-byte unsigned shorts           (GSSI)
  ;         -5 for 2-byte unsigned shorts, but only first 12-bits used
  ;          3 for 4-byte signed ints or longs      (SEG-Y)
  ;         -3 for 4-byte unsigned longs
  ;         -6 for 4-byte unsigned longs, but only first 24-bits used
  ;          4 for 4-byte floats                    (SEG-Y)
  ;          8 for 8-byte doubles
  ;
  ;         For example:  8-bit GSSI data are unsigned characters (values
  ;         from 0 to 255), use -1 for input_datatype.  For 16-bit GSSI data
  ;         (values from 0 to 65535) use -2.  PulseEKKO and Ramac data are 
  ;         typically 16-bit signed integers (values from -32768 to 32767),
  ;         use 2 for input_datatype.  For SEG-Y data, the input_datatype
  ;         can be 2 (signed short integers), 3 (signed long integers), or
  ;         4 (4-byte floating point reals).  Data types are stored in the
  ;         files header of DZT and SGY files.  PulseEKKO and Ramac do not
  ;         record the data type.
  ;
  ;********************* SPECIFICATION OF OUTPUT FILE ************************
  ; The processed GPR data are stored in same format as input data set
  ;
dat_outfilename  = ""  ; binary GPR data file.  For S&S files, an "HD" file
  ;                      will also be created.
  ;
  ;************** SELECTING DATA CHANNEL, TRACES, AND SAMPLES ****************
  ; This group determines which traces and samples are used from a file.
  ; They effectively allow you to select a rectangular subset of data to 
  ; extract from the data file and store to disk, with or without processing 
  ; the data.
  ; If "first_trace" or "last_trace" are not specified or are both <0, then
  ; all traces will be used.  
  ;
  ; NOTES: Output files are single channel only.
  ;        There must be at least 3 samples per trace (to define a 
  ;          cycle/frequency).
  ;        Sample and trace numbers start at 0.  So if there are 512 samples,
  ;          the first sample number is 0 and the last is 511.
  ;
channel     = 0   ; channel number in multichannel data sets indexed from 0.
  ;                 GSSI data can have up to 4 channels (channel=0,1,2, or 3).
  ;                 PulseEKKO and RAMAC data contain only 1 channel (channel=0).
  ;
first_trace = -1  ; start processing at this trace (inclusive).
  ;
last_trace  = -1  ; stop processing at this trace (inclusive).
  ;
  ;**************************** RESAMPLING OPTIONS ****************************
  ; This group determines how data are resampled; 
  ;
  ; If "first_samp" or "last_samp" are not specified or are both <0, then
  ; all trace samples will be used.  
  ;
  ; If all trace samples are not used, ie. first_samp or last_samp are > 0,
  ; then the total time of the trace will be reduced ONLY for non-GSSI DZT file
  ; and a low-cut filter will be used.  The sample rate has not changed.  
  ; To "expand" the new piece of the trace out to the original length, then 
  ; assign resample the appropriate value (number of original sample) and then
  ; the sample rate will change (and a high-cut filter will be used based
  ; on the largest sampling rate encountered).
  ;
first_samp  = -1  ; start processing at this sample number (inclusive).
  ;
last_samp   = -1  ; stop processing at this sample number (inclusive).
  ;         
resample    = 0   ; if >=3, the new number of samples for each trace;
  ;                 min is 3 (for splining) max is 65536.  A cubic spline 
  ;                 is used for iterpolation.  FFT filtering is
  ;                 applied to low-pass filter if reducing or increasing
  ;                 samples.  The total time (which determines the 
  ;                 lowest frequency in the trace) does not change.  
  ;                 The highest frequency in the trace is determined by
  ;                 the sample rate - determined from the number of
  ;                 samples.  Hence, a high-cut filter is always applied
  ;                 to avoid aliasing (when reducing samples) or to avoid
  ;                 introducing frequencies not present in the original
  ;                 trace (when increasing samples).
  ;            NOTE: For DZT files, resample will be forced to the next
  ;                  higher power of 2 (if not already a power of 2, such
  ;                  as 128, 256, 512, 1024, 2048, 4096, etc.)
  ;
  ;  ############ NOTE #############
  ;  The resampling operation DOES NOT change the range gain records
  ;  in DZT headers (or comment area of other storage types).  To keep the
  ;  DZT header information correct, first remove range gain from DZT files
  ;  using GPR_PROC, run this program, then using GPR_PROC add gain back
  ;  to the data (it can be the same or different).  To calculate new db 
  ;  values, linear interpolation can be used between the old gain db values.
  ;  ###############################
  ;
preprocFFT       = "TRUE"  ; flag, preprocess scans for FFT if "TRUE".
  ;                          This operation tapers the start and end of each 
  ;                          trace to the median value.  A Hanning cosine 
  ;                          taper is always applied to edges of the filter 
  ;                          pass block.
  ;                      
  ;  ############ NOTE #############
  ;  If the total time per trace has changed (if first_samp or last_samp are not
  ;  the default values) or the sample rate has changed (if resample >=3), then
  ;  the data will be FFT filtered
  ;  ###############################
  ;
;************************* END OF FILE GPR_SAMP.CMD **************************
