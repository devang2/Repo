#ifndef GPR_IO_H
#define GPR_IO_H
/* GPR_IO.H
 * Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * January 28, 1997  August 8, 1997; June 30, 1999; September 2, 1999;
 * December 21, 2000 [SU stuff];
 *
 * This header file should be included whenever the library GPR_IO.LIB
 * is linked with source/object code.
 */
/********************** Include required header files ***********************/
/* non-ANSI  headers */
#include <io.h>       /* access */
#include <conio.h>    /* getch,kbhit */

/* ANSI-compatible headers */
#include <errno.h>    /* errno */
#include <math.h>     /* pow */
#include <stdio.h>    /* fopen,fclose,fread,fwrite,FILE,NULL,size_t,fseek,
						 ftell,rewind,printf,fprintf,sprintf,fgets,puts */
#include <stdlib.h>   /* malloc,free,calloc,NULL,atoi,atof */
#include <string.h>   /* strstr,strncpy,strcmp,strchr,strcpy,strncat,strlen,
						 strupr(non-ANSI),memset,strcat,strlwr(non-ANSI) */
#include <time.h>     /* time,localtime,time_t,struct tm */

/* application headers */
#include "assertjl.h" /* custom version of assert */
#include "jl_defs.h"  /* manifest constants common to differents functions */

/****************** Manifest constants for general usage ********************/
#ifndef STORAGE_FORMATS
  #define STORAGE_FORMATS
  #define DZT     1    /* GSSI SIR-10 ver 3.x through ver 5.x */
  #define DT1     2    /* Sensors & Software PulsEKKO */
  #define SGY     3    /* SEG SEG-Y */
  #define OTHER   4    /* user defined - must remain 4 */
  #define USR     4    /* user defined - must remain 4 */
  #define SG2     5    /* SEG SEG-2 */
  #define RD3     6    /* Mala Geoscience RAMAC */
  #define SIR2    7    /* GSSI SIR-2 */
  #define MOD_DZT 8    /* modified DZT format */
  #define SU      9    /* SU (like SEG-Y but no ASCII & reel headers */
  #define GPR_DATA 200 /* GPR data in SU files */

#endif

#ifndef INPUT_DATATYPES
  #define INPUT_DATATYPES
  #define CHAR     (1)
  #define UCHAR    (-1)
  #define SHORT    (2)
  #define USHORT   (-2)
  #define USHORT12 (-5)
  #define LONG     (3)
  #define ULONG    (-3)
  #define ULONG24  (-6)
  #define FLOAT    (4)
  #define DOUBLE   (8)
#endif


/******************** Manifest constants for DZT files **********************/
#define MAX_RG_BREAKS 8  /* version 5.1 beta and earlier SIR-10 software */

#define PR_VIIRL     1  /* PR_... are codes for interpreting the processing history */
#define PR_VIIRH     2
#define PR_VTCL      3
#define PR_VTCH      4
#define PR_VBOXL     5
#define PR_VBOXH     6
#define PR_VTRIL     7
#define PR_VTRIH     8
#define PR_VFIRL     9
#define PR_VFIRH    10
#define PR_HIIRL    11
#define PR_HIIRH    12
#define PR_HTCL     13
#define PR_HTCH     14
#define PR_HBOXL    15
#define PR_HBOXH    16
#define PR_HTRIL    17
#define PR_HTRIH    18
#define PR_HFIRL    19
#define PR_HFIRH    20
#define PR_LPF      21
#define PR_HPF      22
#define PR_STS      23
#define PR_STS_NEW  52    /* added by USGS for v. 5.1 beta but conflict below? */

#define PR_MULK     24    /* new codes as of 1/11/94 draft */
#define PR_FUNC     25
#define PR_AGC      26
#define PR_RGAIN    27
#define PR_HILB     28
#define PR_SPEC     29
#define PR_PDEC     30
#define PR_MIG      35
#define PR_KMIG     36
#define PR_FCOMP    40
#define PR_LCOMP    41
#define PR_SCORR    45
#define PR_MSCORR   46
#define PR_3DP      47
#define PR_SNORM    48
#define PR_HNORM    49
#define PR_SPF      50
#define PR_3DC      51
#define PR_ADDK     52
#define PR_AGCE     53
#define PR_XYZ     128
#define PR_RG      129
#define PR_DCM     130
#define PR_LBL     131
#define PR_SND     132
#define PR_OTHF    133

/****************************** New data types ******************************/
/* Make sure the following structures are packed on byte boundaries! */
#if defined(_INTELC32_)
	#pragma align (DztDateStruct=1)
	#pragma align (DztHdrStruct=1)
	#pragma align (DztHdrStructAlt=1)
	#pragma align (SsHdrInfoStruct=1)
	#pragma align (SsTraceHdrStruct=1)
	#pragma align (SegyReelHdrStruct=1)
	#pragma align (SegyTraceHdrStruct=1)
	#pragma align (SuTraceHdrStruct=1)
	#pragma align (TimeDateStruct=1)
#elif defined(__BORLANDC__)
	#pragma option -a-
#elif defined(_MSC_VER)
	#pragma pack(1)
#elif defined(__ZTC__)
	#pragma ZTC align 1
#elif defined(__WATCOMC__)
	#pragma pack (1)
#endif

/* Next two structures are equivalent to GSSI's struct rfdate */
struct DztDateStruct
{   unsigned sec2  : 5;      /* second/2  (0-29) */
	unsigned min   : 6;      /* minute    (0-59) */
	unsigned hour  : 5;      /* hour      (0-23) */
	unsigned day   : 5;      /* day       (1-31) */
	unsigned month : 4;      /* month     (1-12; 1=Jan, 2=Feb, etc. */
	unsigned year  : 7;      /* year-1980 (0-127; 1980-2107) */
} ;  /* 4 bytes (32 bits) if tightly packed */

struct TimeDateStruct
{   unsigned sec2  : 5;      /* second/2  (0-29) */
	unsigned min   : 6;      /* minute    (0-59) */
	unsigned hour  : 5;      /* hour      (0-23) */
	unsigned day   : 5;      /* day       (1-31) */
	unsigned month : 4;      /* month     (1-12; 1=Jan, 2=Feb, etc. */
	unsigned year  : 7;      /* year-1980 (0-127; 1980-2107) */
} ; /* 4 bytes (32 bits) if tightly packed */

/* Note: the following structure incorporates the changes noted in the
 *       preliminary draft for extended RADAN file header format as
 *       faxed to Jeff Lucius by Leo Galinovsky (GSSI) 1/11/94.
 * This is the header that starts each DZT file (SIR-10 and SIR-2)
 */
struct DztHdrStruct
{   unsigned short       rh_tag;       /* 0x0Nff, where N = rh_nchan-1 (0 - 15) */
	unsigned short       rh_data;      /* offset to data in file (1024 * rh_nchan) */
	unsigned short       rh_nsamp;     /* samples per scan (2 - 65535) */
	unsigned short       rh_bits;      /* bits per data word (8, 16, 32, 64) */
	short                rh_zero;      /* binary offset (-128, -32768, etc.) */
	float                rh_sps;       /* scans per second */
	float                rh_spm;       /* scans per meter */
	float                rh_mpm;       /* meters per mark */
	float                rh_position;  /* position (ns) */
	float                rh_range;     /* range (ns) */
	unsigned short       rh_npass;     /* scans per pass for 2D files */
	struct DztDateStruct rh_create;    /* date created */
	struct DztDateStruct rh_modif;     /* date modified */
	unsigned short       rh_rgain;     /* offset to range gain function */
	unsigned short       rh_nrgain;    /* size of range gain function (2 - rh_nsamp) */
	unsigned short       rh_text;      /* offset to text */
	unsigned short       rh_ntext;     /* size of text */
	unsigned short       rh_proc;      /* offset to processing history */
	unsigned short       rh_nproc;     /* size of processing history */
	unsigned short       rh_nchan;     /* number of channels */
	float                rh_epsr;      /* average dielectric constant */
	float                rh_top;       /* top position in meters */
	float                rh_depth;     /* range in meters */
	char                 reserved[31];
	char                 rh_dtype;     /* bits: 7 6 5 4 3 2 1 0
															  0  unsigned data
															  1    signed data
															0     8-bit int
															1    16-bit int
														  1      32-bit int
														1        64-bit int
													  1          32-bit float
													1            64-bit float (USGS extension)
                                                X X X            not used
                                        */
    char                 rh_antname[14]; /* antenna name (eg. 3105 (300 MHz) ) */
    unsigned short       rh_chanmask;    /* active channels mask
                         format is 0x530X, where X has following bits set:
                                          bits: 7 6 5 4 3 2 1 0
                                                              1  1 channel
                                                            1 1  2 channels
                                                          1 1 1  3 channels
                                                        1 1 1 1  4 channels
                                                0 0 0 0            not used
                                          */
    char                 rh_name[12];    /* this file name */
    unsigned short       rh_chksum;      /* checksum for header */
    /* 128 bytes to here */
    char                 variable[896];  /* range gain, comments, and processing
                                            history */
} ;  /* 1024 bytes if tightly packed */
/*
 * WARNING! In older DZT files, the rh_data field may not actually be the
 *          offset to data in multi-channel files!  It is probably best to
 *          read in all the headers which leaves the file pointer at the
 *          start of the first scan or read in the first header to
 *          discover the number of channels than rewind the file and move
 *          the file pointer "number of channels times sizeof(the header
 *          structure)".
 */
/* Here is an equivalent structure with better field names */
struct DztHdrStructAlt
{   unsigned short       dzt_hdr_id;
    unsigned short       offset_to_data;
    unsigned short       samples_per_scan;
    unsigned short       bits_per_sample;
    short                offset_for_zero;
    float                scans_per_second;
    float                scans_per_meter;
    float                meters_per_mark;
    float                start_time_offset;
    float                total_time_range;
    unsigned short       scans_per_pass;
    struct DztDateStruct create_date;
    struct DztDateStruct modified_date;
    unsigned short       offset_to_rgain;
    unsigned short       sizeof_rgain;
    unsigned short       offset_to_text;
    unsigned short       sizeof_text;
    unsigned short       offset_to_proc_hist;
    unsigned short       sizeof_proc_hist;
    unsigned short       number_of_channels;
    float                ave_rel_diel_perm;
    float                top_in_meters;
    float                range_in_meters;
    char                 reserved[31];
    char                 data_type;
    char                 ant_number[14];
    unsigned short       channel_mask;
    char                 this_filename[12];
    unsigned short       checksum;
    /* 128 bytes to here */
    char                 variable[896];
} ;  /* 1024 bytes if tightly packed */

/* This structure is defined so that information extracted from the ASCII
   *.HD header file can be retained in an easily usable format. */
struct SsHdrInfoStruct
{   char      job_number[16];    /* job number (only 1st 10 used) */
    char      title1[80];        /* title 1 (only 1st 70 used) */
    char      title2[80];        /* title 2 (optional; only 1st 70 used) */
    short int num_traces;        /* up to 32767 */
    short int num_samples;       /* up to 32767 */
    short int time_zero_sample;  /* time zero is at this sample number */
    short int num_stacks;        /* number of stacks used to make trace */
    short int total_time_ns;     /* total time window in nanoseconds */
    float     start_pos;         /* starting position along traverse */
    float     final_pos;         /* final position along traverse */
    float     step_size;         /* distance between stations */
    float     ant_freq;          /* nominal center frequency */
    float     ant_sep;           /* antenna separation */
    float     pulser_voltage;    /* voltage of radar pulser unit */
    char      pos_units[8];      /* "metres", "meters", "feet" */
    char      survey_mode[32];   /* "Reflection", etc. */
    short int year;              /* year - 1900; date read from 3rd file line */
    short int month;             /* 1 to 12 */
    short int day;               /* 1 to 31 */
    char     *proc_hist;         /* lines up to "SURVEY MODE" seem standard,
                                    with the processing history and other
                                    information starting after that and to the
                                    end of file; storage for info will have to
                                    be allocated */
} ;

/* This structure is present before every trace in the *.DT1 data file */
struct SsTraceHdrStruct
{ /* trace identification header */
    float trace_num;        /* trace sequence number within line */
    float position;         /* position along traverse */
    float num_samples;      /* samples per trace */
    float elevation;        /* elevation data if available */
    float unass5;           /* unassigned */
    float num_bytes;        /* bytes per sample; always 2 for Rev. 3 firmware */
    float aux_trace_num;    /* trace number again */
    float num_stacks;       /* number of stacks used to get trace */
    float time_window;      /* time window in ns */
    float unass10;          /* unassigned */
    float unass11;          /* unassigned */
    float unass12;          /* unassigned */
    float unass13;          /* unassigned */
    float unass14;          /* unassigned */
    float unass15;          /* unassigned */
    float unass16;          /* unassigned */
    float unass17;          /* unassigned */
    float unass18;          /* unassigned */
    float unass19;          /* unassigned */
    float unass20;          /* unassigned */
    float time_zero_adjust; /* where sample x = sample x + adjustment */
    float zero_flag;        /* 0 = data OK, 1 = zero data */
    float unass23;          /* unassigned */
    float time;             /* seconds past midnight */
    float comment_flag;     /* 1 = comment attached */
    char comment[28];       /* optional character comments */
} ;  /* 128 bytes if tightly packed */

/* the SEG-Y reel identification header */
struct SegyReelHdrStruct
{ /* ASCII- or EBCDIC-coded block */
    char comment[3200]; /* 3200-byte text area */
  /* binary-coded block */
    long int  jobid;   /* job identification number */
    long int  lino;    /* line number (only one line per reel) */
    long int  reno;    /* reel number */
    short int ntrpr;   /* number of data traces per record */
    short int nart;    /* number of auxiliary traces per record */
    short int hdt;     /* sample interval in micro secs for this reel */
    short int dto;     /* same for original field recording */
    short int hns;     /* number of samples per trace for this reel */
    short int nso;     /* number of samples per trace for original field recording */
    short int format;  /* data sample format code:
                                1 = floating point (4 bytes)
                                2 = fixed point (4 bytes)
                                3 = fixed point (2 bytes)
                                4 = fixed point w/gain code (4 bytes) */
    short int fold;    /* CDP fold expected per CDP ensemble */
    short int tsort;   /* trace sorting code:
                                1 = as recorded (no sorting)
                                2 = CDP ensemble
                                3 = single fold continuous profile
                                4 = horizontally stacked */
    short int vscode;  /* vertical sum code:
                                1 = no sum
                                2 = two sum ...
                                N = N sum (N = 32,767) */
    short int hsfs;    /* sweep frequency at start */
    short int hsfe;    /* sweep frequency at end */
    short int hslen;   /* sweep length (ms) */
    short int hstyp;   /* sweep type code:
                                1 = linear
                                2 = parabolic
                                3 = exponential
                                4 = other */
    short int schn;    /* trace number of sweep channel */
    short int hstas;   /* sweep trace taper length (msec) at start if
                          tapered (the taper starts at zero time
                          and is effective for this length) */
    short int hstae;   /* sweep trace taper length (msec) at end
                          (the ending taper starts at sweep length
                          minus the taper length at end) */
    short int htatyp;  /* sweep trace taper type code:
                                1 = linear
                                2 = cos-squared
                                3 = other */
    short int hcorr;   /* correlated data traces code:
                                1 = no
                                2 = yes */
    short int bgrcv;   /* binary gain recovered code:
                                1 = yes
                                2 = no */
    short int rcvm;    /* amplitude recovery method code:
                                1 = none
                                2 = spherical divergence
                                3 = AGC
                                4 = other */
    short int mfeet;   /* measurement system code:
                                1 = meters
                                2 = feet */
    short int polyt;   /* impulse signal polarity code:
                                1 = increase in pressure or upward
                                    geophone case movement gives
                                    negative number on tape
                                2 = increase in pressure or upward
                                    geophone case movement gives
                                    positive number on tape */
    short int vpol;    /* vibratory polarity code:
                                code    seismic signal lags pilot by
                                1    337.5 to  22.5 degrees
                                2     22.5 to  67.5 degrees
                                3     67.5 to 112.5 degrees
                                4    112.5 to 157.5 degrees
                                5    157.5 to 202.5 degrees
                                6    202.5 to 247.5 degrees
                                7    247.5 to 292.5 degrees
                                8    293.5 to 337.5 degrees */
    short int hunass[170];  /* unassigned */
}  ;  /* 3600 bytes if tightly packed */

 /* the SEG-Y trace identification header */
struct SegyTraceHdrStruct
{   long int tracl;    /* trace sequence number within line */
    long int tracr;    /* trace sequence number within reel */
    long int fldr;     /* field record number */
    long int tracf;    /* trace number within field record */
    long int ep;       /* energy source point number */
    long int cdp;      /* CDP ensemble number */
    long int cdpt;     /* trace number within CDP ensemble */
    short int trid;    /* trace identification code:
                                1 = seismic data
                                2 = dead
                                3 = dummy
                                4 = time break
                                5 = uphole
                                6 = sweep
                                7 = timing
                                8 = water break
                                9---, N = optional use (N = 32,767)
                                Following are CWP id flags:
                                9 = autocorrelation
                                10 = Fourier transformed - no packing
                                     xr[0],xi[0], ..., xr[N-1],xi[N-1]
                                11 = Fourier transformed - unpacked Nyquist
                                     xr[0],xi[0],...,xr[N/2],xi[N/2]
                                12 = Fourier transformed - packed Nyquist
                                      even N:
                                     xr[0],xr[N/2],xr[1],xi[1], ...,
                                   xr[N/2 -1],xi[N/2 -1]
                                   (note the exceptional second entry)
                                     odd N:
                                     xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
                                   xr[(N-1)/2 -1],xi[(N-1)/2 -1],
                                   xi[(N-1)/2]
                                   (note the exceptional second & last entries)
                                13 = Complex signal in the time domain
                                     xr[0],xi[0], ..., xr[N-1],xi[N-1]
                                14 = Fourier transformed - amplitude/phase
                                     a[0],p[0], ..., a[N-1],p[N-1]
                                15 = Complex time signal - amplitude/phase
                                     a[0],p[0], ..., a[N-1],p[N-1]
                                16 = Real part of complex trace from 0 to
                                 Nyquist
                                17 = Imag part of complex trace from 0 to
                                 Nyquist
                                18 = Amplitude of complex trace from 0 to
                                 Nyquist
                                19 = Phase of complex trace from 0 to Nyquist
                                21 = Wavenumber time domain (k-t)
                                22 = Wavenumber frequency (k-omega)
                                30 = Depth-Range (z-x) traces
                               101 = Seismic data packed to bytes (by supack1)
                               102 = Seismic data packed to 2 bytes (by supack2)
                               200 = GPR data
                                */
    short int nvs;     /* number of vertically summed traces (see vscode
                          in reel header structure) */
    short int nhs;     /* number of horizontally summed traces (see vscode
                          in reel header structure) */
    short int duse;    /* data use:
                          1 = production
                          2 = test */
    long int offset;   /* distance from source point to receiver
                          group (negative if opposite to direction
                          in which the line was shot) */
    long int gelev;    /* receiver group elevation from sea level
                          (above sea level is positive) */
    long int selev;    /* source elevation from sea level
                          (above sea level is positive) */
    long int sdepth;   /* source depth below surface (positive) */
    long int gdel;     /* datum elevation at receiver group */
    long int sdel;     /* datum elevation at source */
    long int swdep;    /* water depth at source */
    long int gwdep;    /* water depth at receiver group */
    short int scalel;  /* scale factor for previous 7 entries
                          with value plus or minus 10 to the
                          power 0, 1, 2, 3, or 4 (if positive,
                          multiply, if negative divide) */
    short int scalco;  /* scale factor for next 4 entries
                          with value plus or minus 10 to the
                          power 0, 1, 2, 3, or 4 (if positive,
                          multiply, if negative divide) */
    long int  sx;      /* X source coordinate */
    long int  sy;      /* Y source coordinate */
    long int  gx;      /* X group coordinate */
    long int  gy;      /* Y group coordinate */
    short int counit;  /* coordinate units code:
                          for previous four entries
                          1 = length (meters or feet)
                          2 = seconds of arc (in this case, the
                          X values are longitude and the Y values
                          are latitude, a positive value designates
                          the number of seconds east of Greenwich
                          or north of the equator */
    short int wevel;   /* weathering velocity */
    short int swevel;  /* subweathering velocity */
    short int sut;     /* uphole time at source */
    short int gut;     /* uphole time at receiver group */
    short int sstat;   /* source static correction */
    short int gstat;   /* group static correction */
    short int tstat;   /* total static applied */
    short int laga;    /* lag time A, time in ms between end of 240-
                          byte trace identification header and time
                          break, positive if time break occurs after
                          end of header, time break is defined as
                          the initiation pulse which maybe recorded
                          on an auxiliary trace or as otherwise
                          specified by the recording system */
    short int lagb;    /* lag time B, time in ms between the time break
                          and the initiation time of the energy source,
                          may be positive or negative */
    short int delrt;   /* delay recording time, time in ms between
                          initiation time of energy source and time
                          when recording of data samples begins
                          (for deep water work if recording does not
                          start at zero time) */
    short int muts;    /* mute time--start */
    short int mute;    /* mute time--end */
    unsigned short int ns; /* number of samples in this trace */
    unsigned short int dt; /* sample interval; in micro-seconds */
    short int gain;    /* gain type of field instruments code:
                          1 = fixed
                          2 = binary
                          3 = floating point
                          4 ---- N = optional use */
    short int igc;     /* instrument gain constant */
    short int igi;     /* instrument early or initial gain */
    short int corr;    /* correlated:
                          1 = no
                          2 = yes */
    short int sfs;     /* sweep frequency at start */
    short int sfe;     /* sweep frequency at end */
    short int slen;    /* sweep length in ms */
    short int styp;    /* sweep type code:
                          1 = linear
                          2 = parabolic
                          3 = exponential
                          4 = other */
    short int stas;    /* sweep trace taper length at start in ms */
    short int stae;    /* sweep trace taper length at end in ms */
    short int tatyp;   /* taper type: 1=linear, 2=cos^2, 3=other */
    short int afilf;   /* alias filter frequency if used */
    short int afils;   /* alias filter slope */
    short int nofilf;  /* notch filter frequency if used */
    short int nofils;  /* notch filter slope */
    short int lcf;     /* low cut frequency if used */
    short int hcf;     /* high cut frequncy if used */
    short int lcs;     /* low cut slope */
    short int hcs;     /* high cut slope */
    short int year;    /* year data recorded */
    short int day;     /* day of year */
    short int hour;    /* hour of day (24 hour clock) */
    short int minute;  /* minute of hour */
    short int sec;     /* second of minute */
    short int timbas;  /* time basis code:
                          1 = local
                          2 = GMT
                          3 = other */
    short int trwf;    /* trace weighting factor, defined as 1/2^N
                          volts for the least sigificant bit */
    short int grnors;  /* geophone group number of roll switch
                          position one */
    short int grnofr;  /* geophone group number of trace one within
                          original field record */
    short int grnlof;  /* geophone group number of last trace within
                          original field record */
    short int gaps;    /* gap size (total number of groups dropped) */
    short int otrav;   /* overtravel taper code:
                          1 = down (or behind)
                          2 = up (or ahead) */
    short int unass[30]; /* unassigned -- for optional info */
#if 0
    /* local assignments, su version of segy headers */
    float d1;          /* sample spacing for non-seismic data */
    float f1;          /* first sample location for non-seismic data */
    float d2;          /* sample spacing between traces */
    float f2;          /* first trace location */
    float ungpow;      /* negative of power used for dynamic
                          range compression */
    float unscale;     /* reciprocal of scaling factor to normalize range */
    short int mark;    /* mark selected traces */
    short int unass[17]; /* unassigned--NOTE: last entry causes
                            a break in the word alignment, if we REALLY
                            want to maintain 240 bytes, the following
                            entry should be an odd number of short/UINT2
                            OR do the insertion above the "mark" keyword
                            entry */
#endif
} ;  /* 240 bytes if tightly packed */

 /* the SU trace identification header */
#define SU_NFLTS	32768	/* Arbitrary limit on data array size	*/
struct SuTraceHdrStruct
{   long int tracl;    /* trace sequence number within line */
    long int tracr;    /* trace sequence number within reel */
    long int fldr;     /* field record number */
    long int tracf;    /* trace number within field record */
    long int ep;       /* energy source point number */
    long int cdp;      /* CDP ensemble number */
    long int cdpt;     /* trace number within CDP ensemble */
    short int trid;    /* trace identification code:
                                1 = seismic data
                                2 = dead
                                3 = dummy
                                4 = time break
                                5 = uphole
                                6 = sweep
                                7 = timing
                                8 = water break
                                9---, N = optional use (N = 32,767)
                                Following are CWP id flags:
                                9 = autocorrelation
                                10 = Fourier transformed - no packing
                                     xr[0],xi[0], ..., xr[N-1],xi[N-1]
                                11 = Fourier transformed - unpacked Nyquist
                                     xr[0],xi[0],...,xr[N/2],xi[N/2]
                                12 = Fourier transformed - packed Nyquist
                                      even N:
                                     xr[0],xr[N/2],xr[1],xi[1], ...,
                                   xr[N/2 -1],xi[N/2 -1]
                                   (note the exceptional second entry)
                                     odd N:
                                     xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
                                   xr[(N-1)/2 -1],xi[(N-1)/2 -1],
                                   xi[(N-1)/2]
                                   (note the exceptional second & last entries)
                                13 = Complex signal in the time domain
                                     xr[0],xi[0], ..., xr[N-1],xi[N-1]
                                14 = Fourier transformed - amplitude/phase
                                     a[0],p[0], ..., a[N-1],p[N-1]
                                15 = Complex time signal - amplitude/phase
                                     a[0],p[0], ..., a[N-1],p[N-1]
                                16 = Real part of complex trace from 0 to
                                 Nyquist
                                17 = Imag part of complex trace from 0 to
                                 Nyquist
                                18 = Amplitude of complex trace from 0 to
                                 Nyquist
                                19 = Phase of complex trace from 0 to Nyquist
                                21 = Wavenumber time domain (k-t)
                                22 = Wavenumber frequency (k-omega)
                                30 = Depth-Range (z-x) traces
                               101 = Seismic data packed to bytes (by supack1)
                               102 = Seismic data packed to 2 bytes (by supack2)
                               200 = GPR data
                                */
    short int nvs;     /* number of vertically summed traces (see vscode
                          in reel header structure) */
    short int nhs;     /* number of horizontally summed traces (see vscode
                          in reel header structure) */
    short int duse;    /* data use:
                          1 = production
                          2 = test */
    long int offset;   /* distance from source point to receiver
                          group (negative if opposite to direction
                          in which the line was shot) */
    long int gelev;    /* receiver group elevation from sea level
                          (above sea level is positive) */
    long int selev;    /* source elevation from sea level
                          (above sea level is positive) */
    long int sdepth;   /* source depth below surface (positive) */
    long int gdel;     /* datum elevation at receiver group */
    long int sdel;     /* datum elevation at source */
    long int swdep;    /* water depth at source */
    long int gwdep;    /* water depth at receiver group */
    short int scalel;  /* scale factor for previous 7 entries
                          with value plus or minus 10 to the
                          power 0, 1, 2, 3, or 4 (if positive,
                          multiply, if negative divide) */
    short int scalco;  /* scale factor for next 4 entries
                          with value plus or minus 10 to the
                          power 0, 1, 2, 3, or 4 (if positive,
                          multiply, if negative divide) */
    long int  sx;      /* X source coordinate */
    long int  sy;      /* Y source coordinate */
    long int  gx;      /* X group coordinate */
    long int  gy;      /* Y group coordinate */
    short int counit;  /* coordinate units code:
                          for previous four entries
                          1 = length (meters or feet)
                          2 = seconds of arc (in this case, the
                          X values are longitude and the Y values
                          are latitude, a positive value designates
                          the number of seconds east of Greenwich
                          or north of the equator */
    short int wevel;   /* weathering velocity */
    short int swevel;  /* subweathering velocity */
    short int sut;     /* uphole time at source */
    short int gut;     /* uphole time at receiver group */
    short int sstat;   /* source static correction */
    short int gstat;   /* group static correction */
    short int tstat;   /* total static applied */
    short int laga;    /* lag time A, time in ms between end of 240-
                          byte trace identification header and time
                          break, positive if time break occurs after
                          end of header, time break is defined as
                          the initiation pulse which maybe recorded
                          on an auxiliary trace or as otherwise
                          specified by the recording system */
    short int lagb;    /* lag time B, time in ms between the time break
                          and the initiation time of the energy source,
                          may be positive or negative */
    short int delrt;   /* delay recording time, time in ms between
                          initiation time of energy source and time
                          when recording of data samples begins
                          (for deep water work if recording does not
                          start at zero time) */
    short int muts;    /* mute time--start */
    short int mute;    /* mute time--end */
    unsigned short int ns; /* number of samples in this trace */
    unsigned short int dt; /* sample interval; in micro-seconds */
    short int gain;    /* gain type of field instruments code:
                          1 = fixed
                          2 = binary
                          3 = floating point
                          4 ---- N = optional use */
    short int igc;     /* instrument gain constant */
    short int igi;     /* instrument early or initial gain */
    short int corr;    /* correlated:
                          1 = no
                          2 = yes */
    short int sfs;     /* sweep frequency at start */
    short int sfe;     /* sweep frequency at end */
    short int slen;    /* sweep length in ms */
    short int styp;    /* sweep type code:
                          1 = linear
                          2 = parabolic
                          3 = exponential
                          4 = other */
    short int stas;    /* sweep trace taper length at start in ms */
    short int stae;    /* sweep trace taper length at end in ms */
    short int tatyp;   /* taper type: 1=linear, 2=cos^2, 3=other */
    short int afilf;   /* alias filter frequency if used */
    short int afils;   /* alias filter slope */
    short int nofilf;  /* notch filter frequency if used */
    short int nofils;  /* notch filter slope */
    short int lcf;     /* low cut frequency if used */
    short int hcf;     /* high cut frequncy if used */
    short int lcs;     /* low cut slope */
    short int hcs;     /* high cut slope */
    short int year;    /* year data recorded */
    short int day;     /* day of year */
    short int hour;    /* hour of day (24 hour clock) */
    short int minute;  /* minute of hour */
    short int sec;     /* second of minute */
    short int timbas;  /* time basis code:
                          1 = local
                          2 = GMT
                          3 = other */
    short int trwf;    /* trace weighting factor, defined as 1/2^N
                          volts for the least sigificant bit */
    short int grnors;  /* geophone group number of roll switch
                          position one */
    short int grnofr;  /* geophone group number of trace one within
                          original field record */
    short int grnlof;  /* geophone group number of last trace within
                          original field record */
    short int gaps;    /* gap size (total number of groups dropped) */
    short int otrav;   /* overtravel taper code:
                          1 = down (or behind)
                          2 = up (or ahead) */

    /* local assignments, su version of segy headers */
	float d1;	     /* sample spacing for non-seismic data */
	float f1;	     /* first sample location for non-seismic data */
	float d2;	     /* sample spacing between traces */
	float f2;	     /* first trace location */
	float ungpow;	 /* negative of power used for dynamic range compression */
	float unscale;	 /* reciprocal of scaling factor to normalize range */
	int   ntr;       /* number of traces */
	short mark;	     /* mark selected traces */
    short shortpad;  /* alignment padding */
	short unass[14]; /* unassigned--NOTE: last entry causes
			            a break in the word alignment, if we REALLY
         			    want to maintain 240 bytes, the following
			            entry should be an odd number of short/UINT2
			            OR do the insertion above the "mark" keyword entry */
#if 0
	float  data[SU_NFLTS];
    short int unass[30]; /* unassigned -- for optional info - SEGY version */
#endif
} ;  /* 240 bytes if tightly packed */

/* Restore 2- or 4-byte alignment */
#if defined(__BORLANDC__)
    #pragma option -a
#elif defined(_MSC_VER)
    #pragma pack(2)
#elif defined(__ZTC__)
    #pragma ZTC align
#elif defined(__WATCOMC__)
    #pragma pack (4)
#endif

typedef struct GprInfoStruct
{   /* Input filenames */
    char cmd_filename[MAX_PATHLEN];     /* command file */
    char data_filename[MAX_PATHLEN];    /* binary data file */
    char info_filename[MAX_PATHLEN];    /* ASCII/binary info file */
    char mrk_filename[MAX_PATHLEN];     /* ASCII file of marker trace locations */
    char xyz_filename[MAX_PATHLEN];     /* ASCII file of marker X-Y-Z locations */
    int  input_format;     /* constant value indicating data storage format */

    /* Output filenames */
    char dzt_outfilename[MAX_PATHLEN];  /* GSSI binary data/info file */
    char dt1_outfilename[MAX_PATHLEN];  /* S&S binary data file */
    char  hd_outfilename[MAX_PATHLEN];  /* S&S ASCII info file */
    char sgy_outfilename[MAX_PATHLEN];  /* SEG-Y binary file */
    char sg2_outfilename[MAX_PATHLEN];  /* SEG-2 binary file */
    char rd3_outfilename[MAX_PATHLEN];  /* Ramac binary file */
    char rad_outfilename[MAX_PATHLEN];  /* Ramac ASCII info file */
    char dsf_outfilename[MAX_PATHLEN];  /* Ramac ??? file */
    char grd_outfilename[MAX_PATHLEN];  /* SURFER binary GRD file */
    char inf_outfilename[MAX_PATHLEN];  /* output information text file */

    /* Input file information */
    int    input_datatype;   /* type of input data elements:
                    if ==  1, 1-byte chars;          == -1, unsigned chars
                       ==  2, 2-byte ints or shorts; == -2, unsigned shorts
                       ==  3, 4-byte ints or longs;  == -3, unsigned longs
                       ==  4, 4-byte floats
                       == -5, unsigned shorts but only first 12-bits used
                       == -6, unsigned longs but only first 24-bits used
                       ==  8, 8-byte doubles
                           */
    long   total_traces;       /* number of traces recorded in file */
    int    num_input_channels; /* for multi-channel input data sets */
    int    selected_channel;   /* channel to use from multi-channel data sets;
                                  indexed from 1 */

    /* Required input file information */
    int    file_header_bytes;  /* number of bytes in file header */
    int    trace_header_bytes; /* number of bytes in each trace header */
    int    in_samps_per_trace; /* number of samples per trace */
    int    in_bits_per_samp;   /* number of bits per sample (8, 16, 32) */
    double in_time_window;     /* number of nanoseconds per trace */

    /* Output file information */
    int    output_datatype;     /* type of input data elements, as above */
    int    num_output_channels; /* for multi-channel output data sets */
    int    out_samps_per_trace; /* number of samples per trace */
    int    out_bits_per_samp  ; /* number of bits per sample (8, 16, 32) */
    double out_time_window;     /* number of nanoseconds per trace */

    /* Recording information  */
    int    timezero_sample;    /* "time zero" at this sample */
    double traces_per_sec;     /* number of traces recorded per second */
    int    number_of_stacks;   /* number of traces stacked to make one
                                  recorded trace */

    /* Antenna information */
    int    nominal_frequency;  /* (MHz) nominal frequency of antenna */
    double pulser_voltage;     /* (V) transmitter voltage */
    double antenna_separation; /* (m) distance between Tx and Rx antennas */
    char   antenna_name[16];   /* serial or model number */

    /* Geometric positioning information */
    double traces_per_meter;   /* number of traces recorded per meter */
    double meters_per_mark;    /* number of meters between tick markers */
    double starting_position;  /* position of first trace */
    double final_position;     /* position of last trace */
    double position_step_size; /* units between traces */
    char   position_units[16]; /* feet, inches, meters, centimeters */
    int    num_marks;          /* number of values in marks[] and XYZ[][3] */
    long   *marks;             /* pointer to array of marker trace numbers */
    double **XYZ;              /* pointer to 2D array of X-Y-Z coordinates
                                  for markers; [num_marks][3] */

    /* Date and time information */
    struct TimeDateStruct time_date_created;
    struct TimeDateStruct time_date_modified;

    /* Range gain information */
    int    num_gain_pts;       /* number of values in gain_pts[] */
    double *gain_pts;          /* pointer to array of gain values (in db) */

    /* Processing history */
    int proc_hist_bytes;       /* number of bytes in proc_hist[] */
    char *proc_hist;           /* buffer with coded or ASCII data */

    /* Comments or text */
    int text_bytes;            /* number of bytes in text[] */
    char *text;                /* buffer with ASCII data */
    char survey_mode[32];      /* reflection, transmission, WARR, etc. */
    char job_number[16];       /* pulseEKKO job number (only 10 char used) */
    char title1[80];           /* pulseEKKO title 1 (only 70 char used) */
    char title2[80];           /* pulseEKKO title 2 (only 70 char used) */

    /* Velocity information */
    double nominal_RDP;        /* average relative dielectric permittivity */

    /* pulseEKKO trace header information */
    float position;            /* position */
    float num_samples;         /* samples per trace */
    float elevation;           /* elevation */
    float num_bytes;           /* bytes per sample */
    float num_stacks;          /* stacks per recorded trace */
    float time_window;         /* ns per trace */
    float timezero_adjust;     /* where: point(x) = point(x + adjustment) */
    float zero_flag;           /* 0 = data OK, 1 = zero data */
    float time_of_day;         /* seconds past midnight */
    float comment_flag;        /* 1 = comment attached */
    char  comment[28];         /* comment */

    /* Structure initialized and members allocated flag */
    int created;
} GPR_INFO ;

/******************** Global variables from dzt_lib.c ***********************/
extern const char *month_abbr[];
extern const char *ant_number[];
extern const char *ant_name[];
extern const char *GetDztChSubGrid8Msg[];
extern const char *GetDztChSubGrid16Msg[];
extern const char *GetDztChSubImage8Msg[];
extern const char *PrintOneDztHeaderMsg[];
extern const char *ReadOneDztHeaderMsg[];
extern const char *SaveDztFileMsg[];
extern const char *ph_viirl;
extern const char *ph_viirh;
extern const char *ph_vfirl;
extern const char *ph_vfirh;
extern const char *ph_vboxl;
extern const char *ph_vboxh;
extern const char *ph_vtril;
extern const char *ph_vtrih;
extern const char *ph_vtcl;
extern const char *ph_vtch;
extern const char *ph_hiirl;
extern const char *ph_hiirh;
extern const char *ph_hfirl;
extern const char *ph_hfirh;
extern const char *ph_hboxl;
extern const char *ph_htril;
extern const char *ph_htcl;
extern const char *ph_hboxh;
extern const char *ph_htrih;
extern const char *ph_htch;
extern const char *ph_hsts;

/******************** Global variables from dt1_lib.c ***********************/
extern const char *SS_HD_CMDS[];
extern const char *GetSsHdFileMsg[];
extern const char *GetSsTraceMsg[];
extern const char *SaveSsHdFileMsg[];

/******************** Global variables from sgy_lib.c ***********************/
extern const char *SEGY_ASCII_CMDS[];
extern const char *GetSegyReelHdrMsg[];
extern const char *GetSegyTraceMsg[];
extern const char *ReadSegyReelHdrMsg[];

/******************** Global variables from gen_lib.c ***********************/
extern const char *GetGprFileTypeMsg[];
extern const char *GetGprSubGridMsg[];
extern const char *GetMrkDataMsg[];
extern const char *GetSubImage8Msg[];
extern const char *GetXyzDataMsg[];

/*************************** Function prototypes ****************************/
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif
/***** DZT files *****/
char *ConvertProcHist2(int bufsiz,char *buff,int hist_size,char *proc_hist);
void EditDztHeader(int field_id,void *field_val,struct DztHdrStruct *hdrPtr);
int  GetDztChSubGrid8(char *filename,int channel,long first_scan,long num_cols,
        long num_rows,unsigned char **grid);
int  GetDztChSubGrid16(char *filename,int channel,long first_scan,long num_cols,
        long num_rows,unsigned short **grid);
int  GetDztChSubImage8(char *filename,int channel,long first_samp,
        long first_trace,long last_trace,long skip_traces,long num_cols,
        long num_rows,unsigned char **image);
int  GetDztFile(int command,char *filename,int *num_hdrs,int *num_scans,
        int *num_samples,int *num_bits,struct DztHdrStruct *hdrPtr1,
        struct DztHdrStruct *hdrPtr2,struct DztHdrStruct *hdrPtr3,
        struct DztHdrStruct *hdrPtr4,void ***grid);
int  PrintOneDztHeader(int header_size,long num_traces,char *out_filename,
        char *dzt_filename,struct DztHdrStruct *hdrPtr);
int  ReadOneDztHeader(char *filename,int *num_hdrs,long *num_scans,int channel,
        int *header_size,struct DztHdrStruct *hdrPtr);
int  SaveDztFile(char *filename,long num_traces,long num_samps,int num_bits,
        int num_hdrs,struct DztHdrStruct *hdrPtr[],void **grid);
void SetDztHeader(int hdr_num,unsigned short rh_nsamp,unsigned short rh_bits,
        short rh_zero,float rh_sps,float rh_spm,float rh_mpm,
        float rh_position,float rh_range,unsigned short rh_npass,
        unsigned short rh_nchan,float rh_epsr,float rh_top,float rh_depth,
        char *rh_antname,char *rh_name,char *text,unsigned short rg_breaks,
        float *rg_values,unsigned short rh_nproc,
        unsigned char *proc_hist,struct DztDateStruct *create_date,
        struct DztHdrStruct *hdrPtr);
void SetDzt5xHeader(int hdr_num,unsigned short rh_nsamp,unsigned short rh_bits,
        short rh_zero,float rh_sps,float rh_spm,float rh_mpm,
        float rh_position,float rh_range,unsigned short rh_npass,
        unsigned short rh_nchan,float rh_epsr,float rh_top,float rh_depth,
        int rh_dtype,char *antname,char *rh_name,char *text,
        unsigned short rg_breaks,float *rg_values,unsigned short rh_nproc,
        unsigned char *proc_hist,struct DztDateStruct *create_date,
        struct DztHdrStruct *hdrPtr);

/***** DT1 files *****/
int  GetSsHdFile(int print_it,struct SsHdrInfoStruct *HdInfo,
        char *hd_filename);
int  GetSsTrace(int data_block_bytes,struct SsTraceHdrStruct *hdrPtr,
        short *s_data,FILE *infile);
void InitDt1Parameters(struct SsHdrInfoStruct *InfoPtr);
void PrintSsHdInfo(char *out_filename,char *hd_filename,
        struct SsHdrInfoStruct *HdInfo);
void PrintSsTraceHdr(char *out_filename,struct SsTraceHdrStruct *hdrPtr);
int  SaveSsHdFile(int day,int month,int year,long num_traces,long num_samples,
        long time_zero_sample,int total_time_ns,double start_pos,
        double final_pos,double step_size,char *pos_units,
        double ant_freq,double ant_sep,double pulser_voltage,
        int num_stacks,char *survey_mode,char *proc_hist,
        char *filename,char *source_filename,
        char *job_number,char *title1,char *title2,
        double traces_per_sec,double meters_per_mark,
        int num_gain_pts,double *gain_pts);
void SetDt1TraceHeader(float trace_num,float position,float num_samples,
        float elevation,float num_bytes,float aux_trace_number,
        float num_stacks,float time_window,float time_zero_adjust,
        float zero_flag,float time,float comment_flag,char *comment,
        struct SsTraceHdrStruct *Dt1TraceHdr);

/***** SGY files *****/
int  GetSegyReelHdr(int *swap_bytes, int *num_traces,
        struct SegyReelHdrStruct *hdrPtr, FILE *infile);
int  GetSegyTrace(int swap_bytes,int format,long data_block_bytes,
        int num_samps,struct SegyTraceHdrStruct *hdrPtr,
        void *v_data,FILE *infile);
void PrintSegyReelHdr(int swap_bytes,long num_traces,char *out_filename,
        char *sgy_filename,struct SegyReelHdrStruct *hdrPtr);
void PrintSegyTraceHdr(char *out_filename,struct SegyTraceHdrStruct *hdrPtr);
int  ReadSegyReelHdr(char *filename,int *swap_bytes,long *num_traces,
        struct SegyReelHdrStruct *hdrPtr);
void SetSgyFileHeader(int day,int month,int year,long num_traces,
        long num_samples,long time_zero_sample,int total_time_ns,
        double start_pos,double final_pos,double step_size,char *pos_units,
        double ant_freq,double ant_sep,double pulser_voltage,int num_stacks,
        char *survey_mode,char *proc_hist,char *text,char *job_number,
        char *title1,char *title2,char *source_filename,double traces_per_sec,
        double meters_per_mark,int num_gain_pts,double *gain_pts,
        struct SegyReelHdrStruct *HdrPtr);
void SetSgyTraceHeader(long trace_num,double ant_sep,double elev,
        double position,long timezero,long num_samps,long sample_int,
        struct SegyTraceHdrStruct *HdrPtr);

/***** SU files *****/
void SetSuTraceHeader(long trace_num,double ant_sep,double elev,
        double Tx_x,double Tx_y,double Rc_x,double Rc_y,
        long timezero,long num_samps,long sample_int,
        long year, long day, long hour, long minute, long second,
        struct SuTraceHdrStruct *HdrPtr);
void PrintSuTraceHdr(char *out_filename,struct SuTraceHdrStruct *hdrPtr);

/***** General files *****/
void ExtractSsInfoFromSegy(struct GprInfoStruct *InfoPtr,
        struct SegyReelHdrStruct *HdrPtr);
int  GetGprFileType(int *file_hdr_bytes,int *num_hdrs,int *trace_hdr_bytes,
        int *samples_per_trace,int *num_channels,long *num_traces,
        int *input_datatype,int *total_time,int *file_type,char *filename);
int  GetGprSubGrid(char *filename,int datatype,long file_header_bytes,
        long trace_header_bytes,long first_trace,long total_samps,
        long num_cols,long num_rows,void **grid);
int  GetSubImage8(char *filename,int datatype,int if_image,
        double min_data_val,double max_data_val,
        long file_header_bytes,long trace_header_bytes,long first_trace,
        long last_trace,long skip_traces,long first_samp,long total_samps,
        long num_cols,long num_rows,unsigned char **image);
int  GetMrkData(char *filename,int *num_ticks,long **ticks);
int  GetXyzData(char *filename,int *num_ticks,double ***xyz);
void InitGprInfoStruct(struct GprInfoStruct *InfoPtr);
#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif

#endif /* #ifndef GPR_IO_H */

