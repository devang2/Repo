; This file is GPR_XFRM.CMD, the default command file (or parameter file) for
; the program GPR_XFRM.EXE version 1.0.
;
; Last Revision Date: May 2, 2001
  ;*************************** PROGRAM CONTROL *******************************
  ;
batch        = "FALSE" ; place program in batch mode (no pauses) if "TRUE";
  ;                      program will normally pause before ending, press
  ;                      ESC to quit
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
  ; specifed, the ".HD" filename will be assumed.  Only DT1/HD files must
  ; have those filename extensions.
  ;
  ; Ramac data files can be read by assigning correct values to the next five
  ; parameters.
  ;
dat_infilename  = ""  ; input GPR binary data file name
  ;
channel        = 0  ; channel number in multichannel data sets indexed from 0.
  ;                   GSSI data can have up to 4 channels (channel=0,1,2, or 3).
  ;                   PulsEKKO and Ramac data only have 1 channel (channel=0).
  ;
  ;     NOTE: only single-channel files are valid.
  ;
  ;############################## NOTE #######################################
  ; >> IF << the GPR format DOES NOT CONFORM to any of the above formats
  ; then the next five parameters, file_header_bytes, trace_header_bytes,
  ; samples_per_trace, total_time, and input_datatype, MUST be specified.
  ; Otherwise, ignore them.
  ; Use GPR_CONV to convert between storage formats.
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
  ;         For PulseEKKO files, each GPR trace is preceeded by a 128-byte
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
  ;********************* SPECIFYING OTHER INPUT FILES ************************
  ; Ancillary input ASCII data file names required for GSSI DZT data or
  ; S&S DT1/HD or SEG-Y data if not collected in step mode.
  ;
  ; MRK and XYZ files have the number of sets stated on the first file record
  ; with the sets listed on following records.
  ; Example MRK file:
  ;  3
  ;  104
  ;  256
  ;  897
  ;
  ; Example corrsponding XYZ file (comments are ignored and not required):
  ;  3
  ;  10.0  10.0  293.456   ; XYZ for trace 104
  ;  20.0  10.0  294.567   ; XYZ for trace 256
  ;  30.0  10.0  295.678   ; XYZ for trace 897
  ;
mrk_infilename  = ""  ; file containing tick marker trace locations
  ;
xyz_infilename  = ""  ; file containing X, Y, and Z locations of tick markers
  ;
  ;********************* SPECIFICATION OF OUTPUT FILE ************************
  ; The processed GPR data are stored in the SAME FORMAT as the input data.
  ; Use GPR_CONV.EXE to convert between storage formats.
  ;
dat_outfilename  = ""  ; binary GPR data file.  For S&S files, an "HD" file
  ;                      will also be created.  For Ramac files, an "RAD"
  ;                      file will also be created.
  ;	                     Output files are single channel only.
  ;
  ;******************** SELECTING TRANSFORMING OPTIONS ***********************
  ; This group determines how data are transformed.
  ;
spatial_dir    = 0   ; direction to use; default = 0
  ;						0 = X-direction from XYZ file
  ;						1 = Y-direction from XYZ file
  ;
  : NOTE: spatial_start and spatial_stop must have the same directional sense
  ;       as the selected coordinates in the XYZ file. For example, if
  ;       spatial_dir is 1 and Y values are increasing as trace numbers are
  ;       increasing, then spatial_start must be less than spatial_stop and
  ;       spatial_step must be positive. If the Y values decrease as the
  ;       trace numbers increase, then spatial_start must be greater than
  ;       spatial_stop and spatial_step must be negative.
  ;
spatial_start  = 0.0 ; starting coordinate
  ;
spatial_stop   = 0.0 ; ending coordinate
  ;
spatial_step   = 0.0 ; uniform increment between traces
  ;
bin_size       = 0.0 ; size of the bin used to caclulate new traces;
  ;                    If <= 0.0, then same as spatial_step size.
  ;                    If > 0.0, then can be < or > spatial_step size.
  ;                    The bins are centered about the new trace locations
  ;                    (determined from the above 4 values).  All traces
  ;                    located in a bin are averaged to create the new trace.
  ;
;************************* END OF FILE GPR_XFRM.CMD **************************
