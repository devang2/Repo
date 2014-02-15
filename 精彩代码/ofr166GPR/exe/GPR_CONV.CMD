******************************* GPR_CONV.CMD ********************************
This file is GPR_CONV.CMD. It is the default keyword file (or command file)
for GPR_CONV.EXE version 2.05.03.00.  May 3, 2001

You should modify this file and save it with a different name.
The file GPR_CONV.CM_ is this same file with the documentation stripped out.
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
use_mark_file = "FALSE"
use_xyz_file = "FALSE"
channel = 1             ; OPTIONAL - For use ONLY with multi-channel DZT files
other_format = "FALSE"  ; OPTIONAL - For use ONLY with custom storage formats
file_header_bytes  = 0  ; OPTIONAL - For use ONLY with custom storage formats
trace_header_bytes = 0  ; OPTIONAL - For use ONLY with custom storage formats
in_samps_per_trace = 0  ; OPTIONAL - For use ONLY with custom storage formats
in_time_window     = 0  ; OPTIONAL - For use ONLY with custom storage formats
input_datatype     = 0  ; OPTIONAL - For use ONLY with custom storage formats
output_datatype    = 0  ; OPTIONAL - Limited applicability

********************* ADDITIONAL OPTIONAL INFORMATION ***********************
timezero_sample    = 0
traces_per_sec     = 0.0
number_of_stacks   = 1
nominal_frequency  = 0
pulser_voltage     = 0.0
antenna_separation = 0.0
antenna_name       = ""
traces_per_meter   = 0.0
meters_per_mark    = 0.0
starting_position  = 0.0
final_position     = 0.0
position_step_size = 0.0
position_units     = ""
year_created       = 0
month_created      = 0
day_created        = 0
hour_created       = 0
minute_created     = 0
num_gain_pts       = 0
gain_pts[]         =
text               = ""
proc_hist          = ""
survey_mode        = ""
nominal_RDP        = 0.0
************************* END OF FILE GPR_CONV.CMD **************************
