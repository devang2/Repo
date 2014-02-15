***************************** GPR_PROC.CMD ********************************
This file is GPR_PROC.CMD. It is the default keyword file (or command file)
for GPR_PROC.EXE version 1.05.02.00.  May 2, 2001

You should modify this file and save it with a different name.
The file GPR_PROC.CM_ is this same file with the documentation stripped out.
You may find it easier to use once you know how to assign the keyword values.

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
num_input_files = 0
input_filelist[]
output_filelist[]
channel = 1            ; OPTIONAL - For use ONLY with multi-channel DZT files

**************************  PROCESSING OPTIONS ******************************
; NOTE that options are performed in the order they are listed by you. One 
; suggested sequence is given below.
glob_bckgrnd_rem = "FALSE"
glob_forgrnd_rem = "FALSE"
num_gain_off = 0
gain_off[]
amp_adjust       = "INVALID_VALUE"
low_freq_cutoff  = -1.0 ; NOTE: both low and high cutoffs must be given if 
high_freq_cutoff = -1.0 ;       either or both are requested
preprocFFT       = "TRUE"
samp_slide       = 0
wind_bckgrnd_rem = 0
wind_forgrnd_rem = 0
stack            = 0
amp_scale        = 1.0
hsmooth          = 0
vsmooth          = 0
spatial_median   = 0
temporal_median  = 0
inst_amp         = "FALSE"
inst_pow         = "FALSE"
num_gain_on  = 0
gain_on[]
trace_equalize   = -1
************************* END OF FILE GPR_PROC.CMD **************************
