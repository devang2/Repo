; This file is GPRSLICE.CMD, the default command file (or keyword file) for
; the program GPRSLICE.EXE version 1.10.03.01.
; Last Revision Date: October 3, 2001
;*************************** PROGRAM CONTROL *******************************
batch = "FALSE"
************************* SPECIFYING INPUT DATA ****************************
num_input_files = 0
input_filelist[]
************************ SPECIFYING OUTPUT FILES ***************************
out_directory   = ""
inf_outfilename = ""
txt_outfilename = ""
pcx_outfilename = ""
t3d_outfilename = ""
sld_outfilename = ""
overwrite_protect = "TRUE"
*************************** DEFINING THE VOLUME ****************************
slice_direction = "Z"
;  0 or "X" = X axis; verticle slices parallel to Y axis
;  1 or "Y" = Y axis; verticle slices parallel to X axis
;  2 or "Z" = Z axis; horizontal slices
X_first   = 0.0
X_last    = 0.0
X_columns = 0
Y_first   = 0.0
Y_last    = 0.0
Y_rows    = 0
Z_first   = 0.0
Z_last    = 0.0
Z_layers  = 0
start_time = 0.0
************************* DEFINING THE SEARCH BOX **************************
box_Xsize = 0.0
box_Ysize = 0.0
box_Zsize = 0.0
*********************** MANIPULATING THE AMPLITUDES ************************
xfrm_method = "ABS"
;  0 or "NONE" = use original data
;  1 or "ABS"  = absolute value
;  2 or "SQR"  = square
;  3 or "INST" = instantaneous amplitude
;  4 or "POW"  = instantaneous power
envelope   = "FALSE"
expand     = "FALSE"
multiply   = 1.0
background = "FALSE"
************************ END OF FILE GPRSLICE.CMD **************************
