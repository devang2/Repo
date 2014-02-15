******************************* GPR_XSU.CMD ********************************
This file is GPR_XSU.CMD. It is the default keyword file (or command file)
for GPR_XSU.EXE version 1.12.21.00.  December 21, 2000

You should modify this file and save it with a different name.
The file GPR_XSU.CM_ is this same file with the documentation stripped out.
You may find it more convenient once you know how to assign the keyword values.

Contact:
Jeff Lucius
U.S. Geological Survey
Box 25046, Denver Federal Center, MS 964
Denver, CO  80225-0046
303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
email: lucius@usgs.gov

***************************** PROGRAM CONTROL *******************************
batch = "FALSE"

************************** INPUT AND OUTPUT DATA ****************************
dzt_infilename  = "" ; GSSI DZT GPR files only
channel         = 0  ; valid values are 0 to 3
mrk_infilename  = "" ; marked-trace numbers (traces are indexed from 0)
xyz_infilename  = "" ; XYZ location for each mark in the MRK file
mrk_offset      = 0  ; distance from "marked" point on antenna array to center
                     ; of ant. array. >0 is to forward (direction of tow)
ant_sep         = 0  ; distance in meters between centers of the antennas
  ; NOTE: The tranmitter is assumed to be the leading antenna
  ;       and the receiver is the trailing antenna in the SU files

dzt_outfilename = "" ; will be in GSSI format with the "dzt" extension.
su_outfilename  = "" ; will be in SU format with the "su" extension.

  ;************************** TRANSFORMING OPTIONS **************************
spatial_dir    = 0   ; 0 = X-direction from XYZ file (default)
  ;					   1 = Y-direction from XYZ file

  : NOTE: spatial_start and spatial_stop must have the same directional sense
  ;       as the selected coordinates in the XYZ file. For example, if
  ;       spatial_dir is 1 and Y values are increasing as trace numbers are
  ;       increasing, then spatial_start must be less than spatial_stop and
  ;       spatial_step must be positive. If the Y values decrease as the
  ;       trace numbers increase, then spatial_start must be greater than
  ;       spatial_stop and spatial_step must be negative.
  ; NOTE: There must be at least 3 output traces (or bins)

spatial_start  = 0.0 ; starting coordinate

spatial_stop   = 0.0 ; ending coordinate

spatial_step   = 0.0 ; uniform increment between traces > 0

bin_size       = 0.0 ; size of the bin used to caclulate new traces;
  ;                    If <= 0.0, then same as spatial_step.
  ;                    If > 0.0, then can be < or > spatial_step.
  ;                    The bins are centered about the new trace locations
  ;                    (determined from the above 4 values).  All traces
  ;                    located in a bin are averaged to create the new trace.

  ;************************** OPTIONAL INFORMATION **************************
  ; If the values for date and time below are left as negative numbers
  ;   then the date and time for when the output file is created will be used.
  ; Zeroes are valid input values.
year   = -1  ; valid range 0 to 4000 (no Y3K bug here!)
month  = -1  ; valid range 0 to 12, Jan=1, Dec=12
day    = -1  ; valid range 0 to 31, 1=1st day of month
hour   = -1  ; valid range 0 to 23, 1 = 1 AM, 23 = 11 PM
minute = -1  ; valid range 0 to 59
second = -1  ; valid range 0 to 59
;************************* END OF FILE GPR_XSU.CMD **************************
