; This file is GPR_DISP.CMD, the default command file (or parameter file) for
; the program GPR_DISP.EXE version 2.11.06.01 without most of the documentation.
:
; Revision Dates: September 11, 1997; July 21, 1999; August 23, 1999;
;                 September 3, 1999; August 28, 2000; November 6, 2001;
  ;
  ;*************************** PROGRAM CONTROL *******************************
  ; NOTE: These parameters only used from the FIRST command file.
batch        = "FALSE"
display_all  = "TRUE"
display_none = "FALSE"
debug        = "FALSE"
  ;
  ;******************** TO DISPLAY MULTIPLE GPR FILES ************************
next_cmd_filename = ""
  ;
  ;********************* SPECIFICATION OF INPUT DATA *************************
data_infilename  = "5LINE07.DZT"  
  ;
  ;############################## NOTE #######################################
  ; >> IF << the GPR format DOES NOT CONFORM to any of the above formats
  ; then the next six parameters, (other_format, file_header_bytes,
  ; trace_header_bytes, samples_per_trace, total_time, and input_datatype)
  ; MUST be specified. And set other_format to "TRUE".  Otherwise, ignore them.
  ; Use GPR_CONV to convert the between storage formats.
  ;###########################################################################
other_format  = "FALSE"
file_header_bytes  = 0
trace_header_bytes = 0
samples_per_trace  = 0
total_time         = 0
input_datatype     = 0
max_data_val = 0.0
min_data_val = 0.0
row_by_row   = "FALSE"
  ;
  ;******************* SPECIFYING OPTIONAL INPUT FILES ***********************
mrk_infilename  = ""
xyz_infilename  = ""
lbl_infilename  = ""
  ;
  ;************************ SPECIFYING HARDCOPY FILES ************************
pcx_outfilename  = ""
eps_outfilename  = ""
  ;
  ;**************** SELECTING CHANNEL, TRACES, AND SAMPLES *******************
channel     = 1 ; DZT channels can be 1, 2, 3, or 4
skip_traces = 0 
lock_first_samp = "FALSE"
first_samp  = 0
last_samp   = 0
  ;
  ;*********************** ELIMINATING UNWANTED TRACES ***********************
num_bad     = 0
bad_traces[]
  ;
  ;*********************** SPECIFYING COORDINATE MODE ************************
  ; coord_mode  horizontal     vertical          description
  ;                axis          axis
  ;      0          -             -              invalid mode
  ;      1      trace number   sample time (ns)  "raw" traces, default
  ;      2      distance (m)   distance (m)      geometrically corrected
  ;      3      time (sec)     time (ns)         stationary antenna
  ;      4      distance (m)   time (ns)         horizontal rubbersheeting,
  ;                                              no topo correction
  ;      5      trace number   sample number     "really raw" data, sample
  ;                                              rate unknown
  ; ------
  ; NOTE: first_samp, last_samp, and lock_first_samp are active for all modes.
  ; ------
coord_mode      = 1
  ;
  ;     WARNING: If a keyword appears more than once then the last occurence
  ;              of the keyword is used, so all equal signs have been removed
  ;              from the next 5 subsection below.
  ;***************************************************************************
  ; For coord_mode == 1, the next keywords are used:
first_trace      0
last_trace       0
first_samp_time  "INVALID_VALUE"
last_samp_time   "INVALID_VALUE"
  ;
  ;***************************************************************************
  ; For coord_mode == 2, the next keywords are used:
horiz_mode   "T"
horiz_start  "INVALID_VALUE"
horiz_stop   "INVALID_VALUE"
num_layers   0
layer_rdp[]
layer_mode[]
layer_val[]
  ;
  ;***************************************************************************
  ; For coord_mode == 3, the next keywords are used:
trace_per_sec     0.0
first_trace_time  "INVALID_VALUE"
last_trace_time   "INVALID_VALUE"
first_samp_time   "INVALID_VALUE"
last_samp_time    "INVALID_VALUE"
  ;
  ;***************************************************************************
  ; For coord_mode == 4, the next keywords are used:
horiz_mode       "T"
horiz_start      "INVALID_VALUE"
horiz_stop       "INVALID_VALUE"
first_samp_time  "INVALID_VALUE"
last_samp_time   "INVALID_VALUE"
num_ticks        0
tick_tracenum[]
tick_xyz[][]
  ;
  ;***************************************************************************
  ; For coord_mode == 5, the next keywords are used:
first_trace      0
last_trace       0
  ;
  ;************************ CRT GRAPHICS DISPLAY MODE ************************
video_mode = 0
  ;
  ;******************** PLACING DATA WINDOW ON THE CRT ***********************
vx1        = 10.0
vx2        = 123.333
vy1        = 10.0
vy2        = 90.0
vert_exag  = 1.0
  ;
  ;**************** ASSIGNING USER UNITS TO THE DATA WINDOW ******************
left   = "INVALID_VALUE"
right  = "INVALID_VALUE"
top    = "INVALID_VALUE"
bottom = "INVALID_VALUE"
  ;
  ;******************* IMAGE PROCESSING AND APPEARANCE ***********************
range        = 255
change_gain  = "FALSE"
num_gain_off = 0
gain_off[]
num_gain_on  = 0
gain_on[]
background   = "FALSE"
abs_val      = "FALSE"
square       = "FALSE"
inst_amp     = "FALSE"
inst_pow     = "FALSE"
envelope     = "FALSE"
stretch      = 0
local_offset = 0.0
local_change = 0.0
brightness   = 0
negative     = "FALSE"
show_markers      = 0
show_marker_width = 1
epc_threshold = 5
epc_contrast  = 255
epc_sign      = 0
epc_gain      = 2.0
  ;
  ;********************* WIGGLE TRACE DISPLAY OF DATA ************************
display_wiggle = "FALSE"
num_wiggles    = 0
wiggle_traceval[]
wiggle_color   = -1
wiggle_fill    = 0
wiggle_width   = 10.0
wiggle_clip    = 100.0
  ;
  ;************** OVERLAYING A HYPERBOLA FOR VELOCITY ANALYSIS ***************
plot_hyperbola = "FALSE"
ant_sep        = 0
obj_depth      = 0
obj_radius     = 0
obj_loc        = 0
hyp_vel        = 0
hyp_color      = 0
  ;
  ;***************** MODIFYING AND CONTROLLING EPS OUTPUT ********************
portrait          = "FALSE"
use_11x17         = "FALSE"
eps_select_font   = "Times-Roman"
caption_font_size = 12
caption_left_edge = 1.25
caption_bottom_edge = 1.00
caption_long_edge = "FALSE"
caption = ""
pagenum_font_size = 12
pagenum_left_edge = 7.25
pagenum_bottom_edge = 10.00    
pagenum = ""                   
  ;
  ;******************* SELECTING FONTS FOR CRT AND EPS ***********************
hpgl_select_font = "romtrplx"
  ;
  ;****************** OVERLAYING GRID LINES ON THE DATA *******************
add_hlines    = "FALSE"
hline_start
hline_int
hline_stop
hline_style   = 0
add_vlines    = "FALSE"
vline_start
vline_int
vline_stop
vline_style   = 0
  ;
  ;******************************** TITLE ************************************
title  = ""
  ;
  ;************** CONTROLLING AXIS TICKS, ANNOTATION AND LABELS **************
laxis_title = ""
laxis_title_offset = 0.0
laxis_title_size = 0.85
laxis_max             
laxis_min             
laxis_tick_show = 0   
laxis_tick_start      
laxis_tick_int        
laxis_tick_abs_min    
laxis_tick_abs_max    
laxis_tick_num  = 9   
laxis_tick_ano  = "FALSE"
laxis_tick_skip = 0
laxis_tick_mid_skip = -1
laxis_ano_left        
laxis_ano_right       
laxis_ano_size   = 0.80
  ;
raxis_title           
raxis_title_offset = 0.0
raxis_title_size = 0.85
raxis_max             
raxis_min             
raxis_tick_show = 0   
raxis_tick_start      
raxis_tick_int        
raxis_tick_abs_min    
raxis_tick_abs_max    
raxis_tick_num  = 9   
raxis_tick_ano  = "FALSE"
raxis_tick_skip = 0   
raxis_tick_mid_skip = -1
raxis_ano_left
raxis_ano_right       
raxis_ano_size   = 0.80
  ;
taxis_title           
taxis_title_offset = 0.0
taxis_title_size = 1.10
taxis_max             
taxis_min
taxis_tick_show = 0   
taxis_tick_start      
taxis_tick_int        
taxis_tick_abs_min    
taxis_tick_abs_max    
taxis_tick_num  = 0   
taxis_tick_ano  = "FALSE"
taxis_tick_skip = 0   
taxis_tick_mid_skip = -1
taxis_ano_left        
taxis_ano_right       
taxis_ano_size   = 0.80
  ;
baxis_title           
baxis_title_offset = 0.0
baxis_title_size = 0.85 
baxis_max             
baxis_min
baxis_tick_show = 0   
baxis_tick_start
baxis_tick_int        
baxis_tick_abs_min    
baxis_tick_abs_max    
baxis_tick_num  = 9   
baxis_tick_ano  = "FALSE"
baxis_tick_skip = 0   
baxis_tick_mid_skip = -1
baxis_ano_left        
baxis_ano_right       
baxis_ano_size   = 0.80 

