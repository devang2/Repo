This file is GPR_CMPG.CMD, the default command file (or parameter file) for
the program GPR_CMPG.EXE version 1.01.23.02 without most of the documentation.

Revision Dates: January 23, 2002

                ********************************************
                ************ REQUIRED KEYWORDS *************
                ********************************************
************************** INPUT AND OUTPUT DATA *****************************
num_input_files = 0
input_filelist[]         ; INPUT DZT GPR files; must be in order starting with
                         ;   smallest offset; offset increment is constant
dzt_outfilename          ; OUTPUT DZT-formatted file with stacked section and
                         ;   gathers
offset_first     = -1    ; distance between antennas in first file (meters)
offset_incr      = -1    ; the uniform increment in offset between files
pos_start        = 0.0   ; horizontal location of first trace in every file,
                         ;   assumed to be mid-way between antennas
pos_step         = 0.0   ; uniform separation between traces in each file

****************************** NMO PARAMETERS ********************************
velocity         = 0.0   ; the NMO velocity in m/ns (0.01 -> 0.30)

                ********************************************
                ************ OPTIONAL KEYWORDS *************
                ********************************************
****************************** PROGRAM CONTROL *******************************
batch        = "FALSE"
display_none = "FALSE"
  ;
  ;********************* SPECIFICATION OF INPUT DATA *************************
channel         = 0      ; indexed from 0 (default is 0)
                         ;   For use ONLY with multi-channel DZT files
  ;
  ;**************************** NMO PARAMETERS *******************************
samp_first       = 0     ; sample number for time-zero
                         ; indexed from 0 (default is 0)
mute             = 0.0   ; the stretch percentage where muting occurs
                         ;  eg. 50 = 50% maximum stretch: (Tnmo-To)/To
                         ; Must be > 0.0

  ;************************ CMP RANGE GAIN PARAMETERS ************************
rg_start_trace    = 0    ; start gain at this trace (goes to last trace)
rg_start_samp     = 0    ; start gain at this samp on start trace
rg_stop_samp      = 0    ; stop gain at this samp on start trace
rg_step           = 0    ; move start and stop samps down by this much for
                         ;   every trace after the start trace
rg_num_on         = 0    ; only 0 (no gain) or 2 (gain) are allowed
rg_on[0]          = 0    ; in db; 6 db = 2X; 12 db = 4X; etc.
rg_on[1]          = 0    ; in db
