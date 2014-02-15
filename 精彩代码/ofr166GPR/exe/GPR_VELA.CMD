; This file is GPR_VELA.CMD, the default command file (or parameter file) for
; the program GPR_VELA.EXE version 1.04.11.01 without most of the documentation.
:
; Revision Dates: April 11, 2001
  ;
  ; ************************************************
  ; ************ REQUIRED KEYWORDS *****************
  ; ************************************************
dzt_infilename   = ""    ; INPUT DZT GPR file
dzt_outfilename  = ""    ; OUTPUT DZT-formatted file with CMP, NMO, vel. pos_start        = 0     ; antenna separation for first trace used (meters)
pos_step         = 0     ; uniform separation steps
vel_start        = 0.0   ; first NMO velocity in m/ns (0.01 -> 0.30)
vel_step         = 0.0   ; velocity increment between NMOs in m/ns
vel_num          = 0     ; number of NMOs
  ;
  ; ************************************************
  ; ************ OPTIONAL KEYWORDS *****************
  ; ************************************************
  ;*************************** PROGRAM CONTROL *******************************
batch        = "FALSE"
display_none = "FALSE"
  ;
  ;********************* SPECIFICATION OF INPUT DATA *************************
channel         = 0      ; indexed from 0 (default is 0)
  ;
  ;**************************** NMO PARAMETERS *******************************
trace_first      = 0     ; first trace to use from file for NMO
                         ; indexed from 0 (default is 0)
trace_last       = 0     ; last trace to use from file for NMO
                         ; if = 0 then last trace in file is used
samp_first       = 0     ; sample number for time-zero
                         ; indexed from 0 (default is 0)
mute             = 0.0   ; the stretch percentage where muting occurs
                         ;  eg. 50 = 50% maximum stretch: (Tnmo-To)/To

  ;************************ CMP RANGE GAIN PARAMETERS ************************
rg_start_trace    = 0    ; start gain at this trace (goes to last trace)
rg_start_samp     = 0    ; start gain at this samp on first trace
rg_stop_samp      = 0    ; stop gain at this samp on first trace
rg_step           = 0    ; move start and stop samps down by this much for
                         ;   every trace after the start trace
rg_num_on         = 0    ; only 0 (no gain) or 2 (gain) are allowed
rg_on[0]          = 0    ; in db; 6 db = 2X; 12 db = 4X; etc.
rg_on[1]          = 0    ; in db
