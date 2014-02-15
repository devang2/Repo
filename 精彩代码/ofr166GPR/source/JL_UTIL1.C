/* JL_UTIL1.C - source code for functions in JL_UTIL1.LIB
 *
 * Jeff Lucius
 * Geophysicist
 * U.S. Geological Survey
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@usgs.gov
 *
 * Revision Dates: August 24, 1999;
 *
 * To compile for use in JL_UTIL1.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c JL_UTIL1.C
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /c compiles only, no linking.
 *
 *     to remove assert() functions from code also add:
 *     /DNDEBUG
 *
 * Functions in this module:
 *   ANSI_there       - tests to see if ANSI.SYS is loaded
 *   BandPassFFT1D    - bandpass frequency filter of a time series
 *   CreateDir        - creates a DOS directory at any allowed level
 *   FftFilterTrace   - high/low frequency filter of a time series
 *   FFT1D            - performs standard FFT/IFFT of a time series
 *   GetDosVersion    - reports DOS version (old style)
 *   InstAttribTrace  - calcs inst-amp or inst-pow of a time series
 *   LinFit           - least-squares fit to data with a straight line
 *   RealFFT1D        - performs "packed" FFT of real function
 *   Sound            - generates sound using PC speaker
 *   Spline           - cubic spline
 *   Spline2          - cubic spline allowing interp outside of ends
 *   TrimStr          - removes blanks/unprintables from start and end of string
 */

/******************* Includes all required header files *********************/
#include "jl_util1.h"

/**************************** Global variables ******************************/
const char *CreateDirMsg[] =
{   "CreateDir(): No errors.",
    "CreateDir() ERROR: pathname is empty.",
    "CreateDir() ERROR: drive letter missing.",
    "CreateDir() ERROR: drives A and B are not allowed.",
    "CreateDir() ERROR: improper path; doubled backslashes",

    "CreateDir() ERROR: unable to count number of directories.",
    "CreateDir() ERROR: unable to make ALL the directories (some may exist).",
    "CreateDir() ERROR: directory path too long to add backslash.",
} ;
const char *LinFitMsg[] =
{   "LinFit(): No errors.",
    "LinFit() ERROR: Number of data pairs less than 3.",
    "LinFit() ERROR: NULL pointer sent to function.",
} ;
const char *SplineMsg[] =
{   "Spline(): No errors.",
    "Spline() ERROR: arrays too small to spline.",
    "Spline() ERROR: independent variable array not strictly monotonic.",
    "Spline() ERROR: interpolated independent variable array not strictly monotonic.",
    "Spline() ERROR: interpolated independent variable array exceed limits.",
    "Spline() ERROR: unable to allocate temporary storage.",
} ;
const char *Spline2Msg[] =
{   "Spline2(): No errors.",
    "Spline2() ERROR: arrays too small to spline.",
    "Spline2() ERROR: independent variable array not strictly monotonic.",
    "Spline2() ERROR: interpolated independent variable array not strictly monotonic.",
    "Spline2() ERROR: unable to allocate temporary storage.",
} ;

/******************************* ANSI_there() *******************************/
/* Determine if the ANSI.SYS driver has been installed.
 *
 * Parameter list:
 *   void
 *
 * Requires: <dos.h>, "jl_defs.h".
 * Calls: int86, GetDosVersion.
 * Returns: int = 0 if ANSI.SYS not installed,
 *                1 if it is installed.
 *
 * Author: Jeff Lucius   K2 Software   Lakewood, CO
 * Date: February 5, 1996
 */
int ANSI_there (void)
{
    union REGS regs;
    int mode,major_ver,minor_ver;

    GetDosVersion(&mode,&major_ver,&minor_ver);

    if (major_ver >= 4)
    {   regs.x.ax = 0x1A00;
        int86(INT_MULTIPLEX,&regs,&regs);
        if (regs.h.al == 0) return 0;  /* not installed */
        else                return 1;
    } else
        return 0;
}
/***************************** BandPassFFT1D() ******************************/
/* High-, low-, or band-pass filter a real time series using a discrete FFT
 * and half of a Hanning window to smooth the cuttoffs.
 *
       FFT filtering uses the "packed" real-values-only FFT modified from
       "Numerical Recipes in C", so wavenmubers are organized as,
       assuming trace length == 512:
          wavenumber  = 0,256,1r,1i,2r,2i,3r,3i, ... , 254r,254i,255r,255i
          array index = 0,  1, 2, 3, 4, 5, 6, 7, ... ,  508, 509, 510, 511
          where   wavenumber: 0 = DC offset,
                            256 = Nyquist,
                              1 = fundamental,
                              r = real part, and
                              i = imaginary part.
       This arangement works becaues the imaginary parts of the DC and
       Nyquist frequencies have no imaginary component - there can be no phase
       shift therefore the phase spectrum [inv-tan (imag/real)] must be 0, and
       the imaginary part must be 0 (division by 0 is illegal).  In a "normal"
       complex FFT, 1024 samples would be required for the 512 sample-length
       trace.  FFT values are "mirrored" around the "Nyquist midpoint" with
       the sign of the sine transform (imaginary component) values reversed
       from one side to the other.  Cosine transform (real component) values
       maintain same sign.  The "packed" real fft takes advantage of this
       symmetry, preserving the actual values, and the sign of the
       "first-half".
 *
 * Parameters:
 *   long data_length = length of the real time series.
 *   long hwindow     = half-width (one side) of Hanning window to apply,
 *                     should be an even number.
 *   long low_cutoff  = index of FFT (wavenumber) to start saving coefficients;
 *                     coefficients below this value will be smoothed to zero;
 *   long high_cutoff = index of FFT to stop saving cofficients.
 *                     coefficients above this value will be smoothed to zero;
 *   double *data    = real array of values "data_length" in size to be filtered.
 *
 * NOTE: To save time, the calling function must allocate storage for all
 *       arrays and there is little error checking.  Here is the check for
 *       valid ranges of the cutoffs:
 *          if (low_cutoff < 0)   low_cutoff = 0;
 *          if (high_cutoff < 0)  high_cutoff = 0;
 *          if (low_cutoff > trace_length/2)  low_cutoff = trace_length/2;
 *          if (high_cutoff > trace_length/2) high_cutoff = trace_length/2;
 *          if (high_cutoff < low_cutoff)  high_cutoff = low_cutoff;
 *
 *       This version calls RealFFT1D() which returns only the positive
 *       frequency half of the FFT, with the base level stored as data[0]
 *       the "Nyquist" value stored as data[1].
 *
 *       The Hanning window half-width will be adjusted smaller if too wide
 *       for the low- or high-cutoff specified.
 *
 * Requires: <math.h>, and "jl_util1.h".
 * Calls: assert, cos, RealFFT1D.
 * Returns: void, but data[] now contains the band-passed time series.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 1, 1993, January 25, 1996, February 5, 1996
 */
void BandPassFFT1D (long data_length,long hwindow,long low_cutoff,long high_cutoff,
                  double *data)
{
    long i;
    int default_win  = 8;            /* smooth cutoff over 8 values */
    int win1,win2;
    long nyquist      = data_length/2;
    double step,t,dtemp;
    static double pi = 3.14159265358979323846;

    assert(data_length > 1  && data);

    /* Verify valid ranges for input values */
    if (hwindow <= 0) hwindow = default_win;
    if (hwindow % 2) hwindow++;     /* window width must be even */
    win1 = win2 = hwindow;
    if (low_cutoff < win1)  win1 = low_cutoff;
    if ((nyquist-high_cutoff) < win2)  win2 = nyquist-high_cutoff;

    /* User supplied low and high cutoffs assume standard FFT of a 512-long
     * scan.  But this method has packed the positive real and imaginary sets
     * into the 512-long series.  So a low-cutoff of 6 is now at 12.
     */
    low_cutoff *= 2;
    high_cutoff *= 2;

    /* Forward transform */
    RealFFT1D(data,data_length,1);

    /* Do high-pass (low-cut) part of filter */
    if ((low_cutoff>0) && (low_cutoff<(data_length-2)))  /* the valid useful range */
    {   step = pi/(double)win1;
        data[0] = 0.0;                            /* base level or mean */
        /* skip over data[1] which is nyquist */
        for (i=2; i<=low_cutoff-2*win1; i+=2)
        {   data[i]   = 0.0;
            data[i+1] = 0.0;
        }
        for (t=step,i=(low_cutoff-2*win1+2); t<win1*step; t+=step,i+=2)
        {   dtemp = 0.5 - 0.5*cos(t);
            data[i]   *= dtemp;
            data[i+1] *= dtemp;
        }
    }

    /* Do low-pass (high-cut) part of filter */
    if ((high_cutoff>0) && (high_cutoff<(data_length-2)))   /* the valid useful range */
    {   step = pi/(double)win2;
        data[1]   = 0.0;                  /* nyquist */
        for (i=high_cutoff+2*win2; i<=data_length-2; i+=2)
        {   data[i]   = 0.0;
            data[i+1] = 0.0;
        }
        for (t=step,i=(high_cutoff+2*win2-2); t<win2*step; t+=step,i-=2)
        {   dtemp = 0.5 - 0.5*cos(t);
            data[i]   *= dtemp;
            data[i+1] *= dtemp;
        }
    }

    /* Inverse transform */
    RealFFT1D(data,data_length,-1);
}
/***************************** GetDosVersion() ******************************/
/* Get version number of DOS.
 *
 * Parameter list:
 *   int *mode      - 0 if DOS loaded in conventional memory
 *                  - 1 if DOS loaded into HMA
 *   int *major_ver - major version number
 *   int *minor_ver - minor version number
 *
 * Example usage:
 *    int mode,major_ver,minor_ver;
 *    GetDosVersion(&mode,&major_ver,&minor_ver);
 *
 * Requires: <dos.h>, "jl_defs.h", "assertjl.h".
 * Calls: assert, int86.
 * Returns: void.
 *
 * Author: Jeff Lucius   K2 Software   Lakewood, CO
 * Date: February 5, 1996
 */
void GetDosVersion(int *mode,int *major_ver,int *minor_ver)
{
    union REGS regs;

    assert(mode && major_ver && minor_ver);

    *mode = 0;
    regs.x.bx = 0x0000;
    regs.x.ax = 0x3306;       /* this function not changed by SETVER */
    int86(INT_DOS,&regs,&regs);
    if (regs.x.bx == 0x0000)  /* version less than 5.0 */
    {   regs.x.ax = 0x3000;
        int86(INT_DOS,&regs,&regs);
        if (regs.h.al == 0)
        {   *major_ver = 1;
            *minor_ver = 0;   /* actually any of the 1.x versions */
        } else
        {   *major_ver = regs.h.al;  /* for version 3.3, would be 3 */
            *minor_ver = regs.h.ah;  /* for version 3.3, would be 30 */
        }
    } else
    {   *major_ver = regs.h.bl;
        *minor_ver = regs.h.bh;
        *mode = (regs.h.dh & 0x10);
    }
}
/*************************** InstAttribTrace() ****************************/
/* Transform trace amplitudes into instantaneous amplitude or power.
 * The input trace must be an array of doubles previously allocated.
 * The trace can be optionally smoothed at the ends using a Hanning window
 * before the forward FFT (the ends are not unsmoothed after the FFT, though).
 * The time series average is removed before FFT and NOT added back.
 *
 * On input, dscan[] holds the original time series and cscan[] is empty.
 * On output, dscan[] holds the instantanaeous aspect of the time series and
 *            the even (real) array members of cscan[] hold the Hilbert
 *            Transform of the input time series.
 *
 * Parameters:
 *   long length    = length of the time series
 *   int preproc    = TRUE to smooth ends of the time series before FFT.
 *   int attrib     == 0, invalid value = no action
 *                  == 1, for instantaneous amplitude
 *                  != 1, for instantaneous power
 *   double *dscan  = [length] allocated work array
 *   double *cscan  = [2*length] allocated work array
 *
 * NOTE: To save time, the calling function must allocate storage for the
 *       arrays. If the length of the trace IS NOT a power of 2 the function
 *       returns.
 *
 * NOTE: There are no error messages! If attrib == 0, or length is not a power
 *       if 2 then the function just returns.
 *
 * Requires: <math.h>, "jl_util1.h", "assertjl.h".
 * Calls: assert, cos, pow, FFT1D.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Lakewood, CO   lucius@usgs.gov
 * Date: August 19, 1999
 * Revisions: August 24, 1999;
 */
void InstAttribTrace (long length,int preproc,int attrib,double *dscan,
                      double *cscan)
{
    long i,itemp;               /* scratch variables */
    double davg;                /* arithmetic average of the time series */
    double t,step,dtemp;        /* scratch variables */
    static int pwindow    = 24; /* size of Hanning window used to preprocess */
    static double twopi   = 2 * 3.14159265358979323846;

    /* Reality check */
    assert(length > 1 && dscan && cscan);
    if (attrib == 0) return;   /* do nothing */

	/* Check if trace length a power of 2, <= 65536 */
    {   for (i=1; i<16; i++)
		{   if (length > pow(2,(double)i) &&
				length < pow(2,(double)(i+1)))
			{   return;        /* do nothing */
				break;
			}
		}
    }

    /* Find average value of the time series */
    davg = 0.0;
    for (i=0; i<length; i++)  davg += dscan[i];
    davg /= length;

    /* Remove average (to minimize wavenumber 0) */
    for (i=0; i<length; i++)  dscan[i] -= davg;

    if (length < pwindow) preproc = FALSE;

    /* Smooth ends to 0 (remove discontinuities at ends to mimic periodicity) */
    if (preproc)
    {   step = twopi/pwindow;
        itemp = length-1;
        for (t=0.0,i=0; i<pwindow/2; t+=step,i++)
        {   dtemp = 0.5 - 0.5*cos(t);          /* Hanning window */
            dscan[i] *= dtemp;
            dscan[itemp-i] *= dtemp;
        }
    }

    /* initialize complex trace */
    for (i=0; i<length; i++)
    {   cscan[i*2]     = dscan[i];  /* real part */
        cscan[(i*2)+1] = 0.0;       /* imag part */
    }

    /* forward transform */
    FFT1D(cscan,length,1);

	/* apply Hilbert filter i sgn s according to Bracewell
       POS freq:     RE -> IM; IM*-1 -> RE
       NEG freq:  RE*-1 -> IM;    IM -> RE
        */
    for (i=0; i<length; i++)
    {   dtemp = cscan[(i*2)+1];             /* copy imag part to dtemp */
        if (i<=length/2)               /* positive frequencies */
        {   cscan[(i*2)+1] = cscan[(i*2)];  /* real       --> imag part */
            cscan[i*2]     = -dtemp;        /* imag *= -1 --> real part */
        } else                         /* negative frequencies */
        {   cscan[(i*2)+1] = -cscan[(i*2)]; /* real *= -1 --> imag part */
            cscan[i*2]     = dtemp;         /* imag       --> real part */
        }
    }

    /* inverse transform */
    FFT1D(cscan,length,-1);  /* Hilbert is real part of cscan; [i*2] */

    /* calculate instantaneous amp/pow */
    for (i=0; i<length; i++)
    {   /* sum the squares */
        dscan[i] = (dscan[i]*dscan[i]) + (cscan[i*2]*cscan[i*2]);

        /* take square root if inst amp wanted*/
        if (attrib == 1)
        {   if (dscan[i] > 0) dscan[i] = sqrt(dscan[i]);
            else              dscan[i] = 0.0;
        }
    }
    return;
}
/****************************** FftFilterTrace() ****************************/
/* High-, low-, or band-pass filter a GPR trace using an FFT filter.
 * The input trace must be an array of doubles previously allocated.
 * The trace can be optionally smoothed at the ends using a Hanning window
 * before the forward FFT (the ends are not unsmoothed after the FFT, though).
 *
 * Parameters:
 *   long trace_length = length of the GPR trace.
 *   int preproc      = TRUE to smooth ends of trace before FFT.
 *   long low_cutoff   = index of FFT (wavenumber) to start saving coefficients;
 *                      coefficients below this value will be smoothed to zero;
 *   long high_cutoff  = index of FFT to stop saving cofficients;
 *                      coefficients above this value will be smoothed to zero;
 *   double *dscan    = work array "trace_length" in size.
 *
 * NOTE: To save time, the calling function must allocate storage for the
 *       array and there is little error checking.  Here is the check for
 *       the cutoffs:
 *          if (low_cutoff < 0)  low_cutoff = 0;
 *          if (high_cutoff > trace_length/2) high_cutoff = trace_length/2;
 *          if (high_cutoff < low_cutoff)  high_cutoff = low_cutoff;
 *
 * Requires: <math.h>, and "jl_util1.h", "assertjl.h".
 * Calls: assert, cos, BandPassFFT1D.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: November 16, 1993, February 5, 1996
 */
void FftFilterTrace (long trace_length,int preproc,long low_cutoff,
                     long high_cutoff,double *dscan)
{
    long i,itemp;               /* scratch variables */
    double dmean;               /* arithmetic mean of the trace */
    double t,step,dtemp;        /* scratch variables */
    static int pwindow    = 24; /* Hanning window used to preprocess */
    static long fwindow   = 8;  /* half-width of Hanning window used by filter */
    static double twopi   = 2 * 3.14159265358979323846;

    assert(trace_length > 1 && dscan);

    /* Find average value of the time series */
    dmean = 0.0;
    for (i=0; i<trace_length; i++)  dmean += dscan[i];
    dmean /= (double)trace_length;

    /* Remove DC shift (to minimize wavenumber 0) */
    for (i=0; i<trace_length; i++)  dscan[i] -= dmean;

    /* Smooth ends to 0 (remove discontinuities at ends to insure periodicity) */
    if (preproc)
    {   step = twopi/(double)pwindow;
        itemp = trace_length-1;
        for (t=0.0,i=0; i<pwindow/2; t+=step,i++)
        {   dtemp = 0.5 - 0.5*cos(t);          /* Hanning window */
            dscan[i] *= dtemp;
            dscan[itemp-i] *= dtemp;
        }
    }

    /* Filter trace using the FFT */
    BandPassFFT1D(trace_length,fwindow,low_cutoff,high_cutoff,dscan);

    /* Add mean back into time series */
    for (i=0; i<trace_length; i++)  dscan[i] += dmean;
}
/******************************* RealFFT1D() ********************************/
/* FFT modified from "Numerical Recipes in C" so that the array is indexed
 * from 0.  Calculates the discrete Fourier Transform of a set of nn
 * real-valued data points if isign == 1.  data[0...nn-1]) is replaced by the
 * positive frequency half of its complex Fourier Transform.  The real-valued
 * first and last components (the mean F[0] and the Nyquist F[N/2]) are
 * returned as elements data[0] and data[1].  The remainder of data[] is
 * arranged as (real,imaginary,real,imaginary,etc.).  nn MUST be a power of 2
 * (which is NOT verified).  This routine also calculates the inverse
 * transform of a complex data array if it is the transform of real data if
 * isign == -1.  The IFFT is normalized before returning.  This was function
 * realft() in "Numerical Recipes in C".
 * NOTE:  The advantage of this method is that storage and computation time
 *        are cut nearly in half (the time series is only half as long!) with
 *        no significant increase in floating-point round-off error.  The
 *        error of this routine is on the order of 10 exp -16 (after fft and
 *        ifft).
 *
 * Input parameters:
 *   data   - [nn], array of real-valued doubles
 *   nn     - size of data[]
 *   isign  - if 1, perform FFT
 *          - if -1, perform IFFT
 * Output parameters:
 *   data[] contains either the packed FFT or the IFFT
 *
 * Requires: <math.h>.
 * Calls: sin, FFT1D().
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: November 10, 1992
 */
void RealFFT1D (double *data,long nn,int isign)
{
    long i,i1,i2,i3,i4,n2p2,n;
    double c1,c2,h1r,h1i,h2r,h2i;
    double wr,wi,wpr,wpi,wtemp,theta;
    double wrth2r,wrth2i,with2r,with2i;
    static double pi = 3.14159265358979323846;

    n = nn >> 1;
    theta = pi/(double)n;
    c1 = 0.5;
    if (isign == 1)               /* forward transform */
    {   c2 = -0.5;
        FFT1D(data,n,1);
    } else                        /* set up for inverse transform */
    {   c2 = 0.5;
        theta = -theta;
    }
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0 + wpr;
    wi = wpi;
    n2p2 = n + n + 2;
    for (i = 1; i<n/2; i++)      /* case==0 done separately below */
    {   i1 = i + i;
        i2 = i1 + 1;
        i3 = n2p2 - i2 - 1;
        i4 = i3+1;
        h1r =  c1 * (data[i1] + data[i3]);
        h1i =  c1 * (data[i2] - data[i4]);
        h2r = -c2 * (data[i2] + data[i4]);
        h2i =  c2 * (data[i1] - data[i3]);
        wrth2r = wr * h2r;
        wrth2i = wr * h2i;
        with2r = wi * h2r;
        with2i = wi * h2i;
        data[i1] =  h1r + wrth2r - with2i;
        data[i2] =  h1i + wrth2i + with2r;
        data[i3] =  h1r - wrth2r + with2i;
        data[i4] = -h1i + wrth2i + with2r;
        wtemp = wr;
        wr += wtemp*wpr - wi*wpi;
        wi += wtemp*wpi + wi*wpr;
    }
    if (isign > 0)                /* finish forward transform */
    {   h1r = data[0];
        data[0] += data[1];
        data[1] = h1r - data[1];
    } else                        /* inverse transform */
    {   h1r = data[0];
        data[0] = c1 * (h1r+data[1]);
        data[1] = c1 * (h1r-data[1]);
        FFT1D(data,n,-1);
    }
    return;
}
/********************************** FFT1D() *********************************/
/* FFT modified from "Numerical Recipes in C" so that the array is indexed
 * from 0.  Replaces data[] by its discrete Fourier transform, if isign == 1.
 * Replaces data[] by its inverse discrete Fourier Transform, if isign == -1.
 * data[0...2*nn-1] is a double array of complex numbers arranged as real,
 * imaginary,real,imaginary,real,imaginary, etc. nn MUST be an integer power
 * of 2 (which is NOT verified).  If real data are used then the imaginary
 * components of data[] must be 0.0; stuff data[] using something like this:
 * for (i=0; i<nn; i++) { data[i<<1] = realdata[i]; data[(i<<1)+1] = 0.0; }.
 * This was function four1() in Numerical Recipes.  Note that the original
 * algorithm leaves the coefficients multiplied by a factor of sqrt(nn),
 * which was not removed after the inverse fft operation by dividing data[]
 * by nn.  This algorithm removes that factor after the ifft.  Removing
 * the factor sqrt(nn) each time is also possible but increases the number
 * of total floating point operations (in a fft/ifft pair) and therefore also
 * increases the chance for significant floating point round-off error.  The
 * error of this routine is on the order of 10 exp -16 (after fft and ifft).
 *
 * Input parameters:
 *   data   - [2*nn], array of complex-valued doubles
 *   nn     - half the size of data[], number of complex-valued pairs
 *   isign  - if 1, perform FFT
 *          - if -1, perform IFFT
 * Output parameters:
 *   data[] contains either the complex FFT or the IFFT
 *
 * Requires: <math.h>.
 * Calls: sin.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: November 10, 1992
 */
void FFT1D (double *data,long nn,int isign)
{
    long i,j,m,n,mmax,istep,jp1;
    double wr,wpr,wpi,wi,theta,tempr,tempi;
    static double twopi = 6.28318530717958647692;

    n = nn << 1;
    j = 0;
    for (i=0; i<n-1; i+=2)
    {   if (j > i)
        {   tempr = data[j];      data[j] = data[i];      data[i] = tempr;
            tempr = data[j+1];  data[j+1] = data[i+1];  data[i+1] = tempr;
        }
        m = nn;
        while (m >= 2 && j >= m)
        {   j -= m;
            m >>= 1;
        }
        j += m;
    }
    mmax=2;
    while (n > mmax)
    {   istep = mmax << 1;
        theta = twopi/(double)(isign*mmax);
        tempr = sin(0.5*theta);
        wpr = -2.0*tempr*tempr;
        wpi = sin(theta);
        wr = 1.0;
        wi = 0.0;
        for (m=1; m<mmax; m+=2)
        {   for (i=m-1; i<n; i+=istep)
            {   j = i + mmax;
                jp1 = j + 1;
                tempr = wr*data[j]   - wi*data[jp1];
                tempi = wr*data[jp1] + wi*data[j];
                data[j]   = data[i] - tempr;
                data[jp1] = data[i+1] - tempi;
                data[i]   += tempr;
                data[i+1] += tempi;
            }
            tempr = wr;
            wr += tempr*wpr -    wi*wpi;
            wi +=    wi*wpr + tempr*wpi;
        }
        mmax = istep;
    }
    if (isign < 0)
    {   double normalizer = 1.0/(double)nn;
        for (i=0; i<n; i++)  data[i] *= normalizer;  /* remove factor */
    }
    return;
}
/******************************* CreateDir() *******************************/
/* Create a directory on disk (A: and B: drives excluded). Backslashes will
 * be added to beginning and end of the path as neccessary. No file name
 * allowed. A file name will be treated as a directory name.
 *
 * Function arguments:
 *  char *dirname - complete pathname, must include drive letter and
 *                  exclude filename
 *
 * External variables:
 *   none
 *
 * Headers:
 *   <direct.h>  chdir, mkdir
 *   <stdlib.h>  _splitpath, _MAX_DIR, _MAX_PATH, _MAX_FNAME, _MAX_EXT
 *   <string.h>  strcpy, strchr, strcat, strlen
 *
 * Non-library function calls:
 *   none
 *
 * Returns: 0 on success,
 *         >0 on error (offset into message string)
 *         -1 if path already exists
 *
const char *CreateDirMsg[] =
{   "CreateDir(): No errors.",
    "CreateDir() ERROR: pathname is empty.",
    "CreateDir() ERROR: drive letter missing.",
    "CreateDir() ERROR: drives A and B are not allowed.",
    "CreateDir() ERROR: improper path; doubled backslashes",

    "CreateDir() ERROR: unable to count number of directories.",
    "CreateDir() ERROR: unable to make ALL the directories (some may exist).",
    "CreateDir() ERROR: directory path too long to add backslash."
} ;
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: August 3, 1999
 */

int CreateDir (char *dirname)
{
	char drive[_MAX_DIR];
	char path[_MAX_PATH];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    char copy[_MAX_PATH];
    char str[_MAX_PATH];
    char *cp,*cp1,*cp2;
	int ch = '\\' ;  /* 92 = '\' */
    int len,i,num_dirs,atend,return_val,made;
    unsigned old_drive,num_drives,new_drive;

    /* NULL string? */
    if (dirname == NULL) return 1;

    /* add trailing '\' so splitpath() doesn't interpret a file */
    len = strlen(dirname);
    if (dirname[len-1] != ch)
    {   if ( len < (_MAX_PATH-1) )
        {   dirname[len] = ch;
            dirname[len+1] = 0;
        } else
        {   return 7;
        }
    }
    _splitpath(dirname,drive,path,fname,ext);

    /* Is a drive letter given? */
    if (drive[0] == 0) return 2;

    /* Get current drive and make sure > 1 (C: or higher)*/
    new_drive = drive[0];       /* get ANSI representative for letter */
    new_drive -= 'A';
    if (new_drive < 2) return 3;

    /* add leading '\' if missing so directory counter works correctly */
    if (path[0] != ch)
    {   strcpy(copy,dirname); /* backup for safety */
        strcpy(dirname,drive);
        strcat(dirname,"\\");
        strcat(dirname,path);
    }

    /* check for 2 or more backslashes */
    if (strstr(dirname,"\\\\")) return 4;

    /* Copy string */
    strcpy(str,dirname);

    /* count directories */
    if ( (cp1 = strchr(str,ch)) == NULL ) return 5;
    cp1++;  /* point past first '\' */
    num_dirs = 1;
    while ( (cp1 = strchr(cp1,ch)) != NULL  )
    {   cp1++;  /* point past next '\' */
        num_dirs++;
    }

    /* point to first '\' */
    strcpy(str,dirname);
    if ( (cp1 = strchr(str,ch)) == NULL ) return 5;
    atend = 0;
    return_val = 0;
    made = 0;
    for (i=0; i<num_dirs; i++)
    {   cp1 = strchr(cp1+1,ch); /* point past '\' and look for next one */
        if ( cp1 == NULL )
        {   /* at end */
            atend = 1;
        } else
        {   /* temporarily truncate */
            *cp1 = 0;

        }
        if (chdir(str))                /* is it there? */
        {   /* No - change dir failed */
            if (mkdir(str))            /* if not, can I make it */
            {   /* make failed */      /* failure to make any dir is terminal */
                return_val = 6;
                break;      /* out of loop */
            } else
            {   /* make succeded */
                if (!atend) *cp1 = ch;  /* restore '\' */
                made = 1;
                continue;   /* at end of loop */
            }
        } else
        {   /* Yes - change dir succeeded */
            if (!atend) *cp1 = ch;  /* restore '\' */
            continue;   /* at end of loop */
        }
    }
    if (!made) return -1;
    return return_val;
}
/********************************* LinFit() *********************************/
/* The function makes a least-squares fit to data with a straight line:
 * Y = A + BX.  This function is addapted from Bevington, P.R., 1969, Data
 * Reduction and Error Analysis for the Physical Sciences: McGraw-Hill Book
 * Co., New York, NY, 336 p., page 105 program 6-1.
 *
 * Parameters:
 *   int     npts   - number of pairs of data points
 *   int     mode   - determines method of weighting least-squares fit
 *                    +1 (instrumental) weight = 1.0/(sigmayY[i]^2)
 *                     0 (no weighting) weight = 1.0
 *                    -1 (statistical)  weight = 1/Y[i]
 *   double *X      - [npts], pointer to array of data points for independant
 *                     variable; previously allocated and assigned
 *   double *Y      - [npts], pointer to array of data points for dependant
 *                     variable; previously allocated and assigned
 *   double *sigmaY - [npts], pointer to array of standard deviations for
 *                     Y data points; previously allocated and assigned;
 *                     can be NULL if mode <=0
 *   double *A      - address of variable; Y intercept of fitted straight line
 *   double *sigmaA - address of variable; standard deviation of A
 *   double *B      - address of variable; slope of fitted straight line
 *   double *sigmaB - address of variable; standard deviation of B
 *   double *R      - address of variable; linear correlation coefficient
 *
 * Requires: <math.h>, "jl_utils.h"
 * Calls: sqrt.
 * Returns: 0 on success, or
 *         >0 if error or warning (offset into an array of message strings).
 *
const char *LinFitMsg[] =
{   "LinFit(): No errors.",
    "LinFit() ERROR: Number of data pairs less than 3.",
    "LinFit() ERROR: NULL pointer sent to function.",
} ;
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 10, 1997
 */
int LinFit (int npts,int mode,double *X,double *Y,double *sigmaY,double *A,
            double *sigmaA,double *B,double *sigmaB,double *R)
{
    int i;
    double sum,sumx,sumy,sumxx,sumxy,sumyy,x,y,weight,delta,var;

    /* Verify calling parameters */
    if (npts <=3) return 1;

    if (X==NULL || Y==NULL || A==NULL || B==NULL || R==NULL || sigmaA==NULL ||
       sigmaB==NULL || (mode>0 && sigmaY==NULL)) return 2;

    /* Initialize variables */
    sum = sumx = sumy = sumxx = sumxy = sumyy = 0.0;
    if (mode < 0) mode = -1;
    if (mode > 0) mode = 1;

    /* Accumulate weighted sums */
    for (i=0; i<npts; i++)
    {   x = X[i];
        y = Y[i];
        switch (mode)
        {   case -1:
              if      (y < 0)  weight = 1.0 / (-y);
              else if (y == 0) weight = 1.0;
              else if (y > 0)  weight = 1.0 / y;
              break;
            case 0:
              weight = 1.0;
              break;
            case 1:
              weight = 1.0 / (sigmaY[i]*sigmaY[i]);
              break;
       }
       sum   += weight;
       sumx  += weight * x;
       sumy  += weight * y;
       sumxx += weight * x * x;
       sumxy += weight * x * y;
       sumyy += weight * y * y;
    }

    /* Calculate coefficients and standard deviations */
    delta = (sum * sumxx) - (sumx * sumx);
    *A = ((sumxx * sumy) - (sumx * sumxy)) / delta;
    *B = ((sumxy * sum) - (sumx * sumy)) / delta;
    switch (mode)
    {   case -1:  case 1:
            var = 1.0;
            break;
        case 0:
            i = npts - 2;
            var = (sumyy + (*A * *A * sum) + (*B * *B *sumxx) -
                   (2.0 * ((*A *sumy + *B * sumxy) - (*A * *B * sumx)))
                  ) / i;
            break;
    }
    *sigmaA = sqrt((var * sumxx) / delta);
    *sigmaB = sqrt((var * sum)   / delta);
    *R      = ((sum * sumxy) - (sumx * sumy)) /
               sqrt(delta*((sum*sumyy) - (sumy*sumy)));

    return 0;
}
/********************************** Sound() *********************************/
/* The system timer, a 8254 programmable interrupt timer (PIT) or equivalent,
 * controls the motion of the speaker.  The default PIT oscillating
 * frequency is the system bus oscillator's 14.31818 MHz signal divided
 * by 12 which is 1.1931817 MHz.  For the speaker to generate a frequency
 * of say 2000 Hz it must be cycled 2000 times a second.  The timer
 * frequency is divided by the desired frequency and Channel 2 of the PIT
 * is sent this value (programmed) to achieve this.  The PIT then waits
 * that number of oscillations before moving the speaker cone.
 *
 * The duration is calculated using the timer tick count stored in the BIOS
 * data area (0040:006C) and assumes there are 18.2 clicks per second.
 * If the timer frequency has been increased, this function will not
 * be "working" in seconds unless the default ISR is invoked appropriately.
 * Duration is accurate to about 1/18 of a second.
 *
 * Parameters:
 *   double freq     - speaker output in Hz.
 *   double duration - how long to leave the speaker on in seconds.
 *
 * The following must be supplied:
 * #include <conio.h> or <dos.h>    ** for _inbyte, _outbyte **
 *
 * Requires: <conio.h>, <i32.h>.
 * Calls: IN1, OUT1, PEEK4.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: April 5, 1993
 */
#define GRDR_TIMERFREQ 1193181.7 /* PIT system timer frequency; 14318180./12. */
#define GRDR_CH0_FREQ 18.206508  /* default Ch.0 interrupt frequency (1193181.7/65536.) */
#define GRDR_DATASEG 0x0040      /* BIOS data area segment */
#define GRDR_OFFSET 0x006C       /* offset to timer count LSB in BIOS data area */
#define Beep() Sound(880.0,0.1)
#define IN1(port)      (int)(_inbyte((unsigned short)(port)))
#define OUT1(port,val) (_outbyte((unsigned short)(port),(unsigned char)(val)))
#define PEEK4(seg,off) (*(long*)((unsigned)((seg)<<4)+(unsigned)(off)))
void Sound (double freq,double duration)
{
    unsigned char portval;
    unsigned long end;
    union
    {   unsigned divisor;
        unsigned char c[2];
    } count;

    /* Verify parameters */
    if ((freq!=0.0) && (freq<20.0)) freq = 20.0;
    if (freq > 20000.0)  freq = 20000.0;
    if (duration < 0.055) duration = 0.055;

    /* Calculate output value for PIT Channel 2 */
    if (freq > 0.0) count.divisor = (unsigned)((double)GRDR_TIMERFREQ/freq);
    else            count.divisor = 0;
    OUT1(0x43, 0xb6);       /* Tell PIT that LSB then MSB is headed for
                                    Ch. 2 (using operation mode 3 - square
                                    wave, and 16-bit binary counters) */
    OUT1(0x42, count.c[0]); /* least significant byte */
    OUT1(0x42, count.c[1]); /* most significant byte */

    /* Make the sound */
    portval = IN1(0x61);         /* get I/O port 61h bit settings (8 bits) */
    if (freq > 0.0)
    {   end = (unsigned long)(duration * GRDR_CH0_FREQ);
        OUT1(0x61, portval|0x03); /* set bits 0 and 1 to turn speaker on
                                          and direct control through PIT Ch. 2 */
        end += (unsigned long)PEEK4(GRDR_DATASEG,GRDR_OFFSET);
        while ((unsigned long)PEEK4(GRDR_DATASEG,GRDR_OFFSET) < end)
            ;                      /* do nothing */
    }
    OUT1(0x61, portval&0xfc); /* clear bits 0 and 1 to turn speaker off */
}
#undef GRDR_TIMERFREQ
#undef GRDR_CH0_FREQ
#undef GRDR_DATASEG
#undef GRDR_OFFSET
#undef Beep
#undef IN1
#undef OUT1
#undef PEEK4
/******************************** Spline() **********************************/
/*  This function performs a cubic spline interpolation to get the values
 *  "ynew[i]" as a function of "xnew[i]".  "xnew[]" is an independent variable
 *  array assigned values between (and/or inclusive of) the endpoints of the
 *  independent series "x[]".  The values "y[i]" are defined for every point
 *  "x[i]".  Essentially, "ynew[]" is interpolated along "y[]".
 *
 *  NOTE: Both "x[]" and "xnew[]" must be strictly monotonically increasing,
 *        with no adjacent values equal.  This version of the function
 *        permits monotonically decreasing values, by "reversing" the series
 *        before splining then returning the values to original order.  "x[]"
 *        "znew[]" may be monotonic on opposite or the same directions.
 *
 * #####################################################################
 * #                                                                   #
 * #   NOTE: Versions before 2/12/97 have 2 MAJOR!!! bugs.             #
 * #         1. When sequences were "reversed" only X was reversed!    #
 * #         2. When xnew was reversed n, rather than nnew, was used.  #
 * #                                                                   #
 * #####################################################################
 *
 *  Parameters:
 *  Input:
 *      n       - size of x[] and y[] arrays.
 *      new     - size of xnew[] and ynew[] arrays.
 *      x[]     - independent variable array.
 *      y[]     - dependent variable array.
 *      xnew[]  - interpolated independent variable array.  The start and end
 *                of these values should not exceed the start and end of x[].
 *  Output:
 *      ynew[]  - interpolated dependent variable array.
 *
 *  Requires: <stdlib.h>, <math.h>, and attached header.
 *  Calls: malloc, free, sqrt, fabs.
 *  Returns: 0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *SplineMsg[] =
{   "Spline(): No errors.",
    "Spline() ERROR: arrays too small to spline.",
    "Spline() ERROR: independent variable array not strictly monotonic.",
    "Spline() ERROR: interpolated independent variable array not strictly monotonic.",
    "Spline() ERROR: interpolated independent variable array exceed limits.",
    "Spline() ERROR: unable to allocate temporary storage.",
}
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 12, 1997
 */
int Spline (long n,long nnew,double x[],double y[],double xnew[],double ynew[])
{
    long i, j, ip1, nm1;
    long halfn   = 0;
    long halfnew = 0;
    double *s    = NULL;
    double *g    = NULL;
    double *work = NULL;
    double eps   = 1.0e-6;    /* allowable error factor */
    double xi, xim1, xip1, yi, xx, h, t, w, u, dtemp;

    /* Pre-qualify input */
    if ((n < 3) || (nnew < 1)) return 1;  /* are there enough points? */
    nm1 = n - 1;
    if (x[n-1] < x[0])                    /* is vector decreasing? */
    {   halfn = n/2;                      /* truncation wanted */
        for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
        {   dtemp = x[i];
            x[i] = x[j];
            x[j] = dtemp;
        }
        for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
        {   dtemp = y[i];
            y[i] = y[j];
            y[j] = dtemp;
        }
    }
    if (xnew[nnew-1] < xnew[0])                /* is vector decreasing? */
    {   halfnew = nnew/2;                    /* truncation wanted */
        for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = xnew[i];
            xnew[i] = xnew[j];
            xnew[j] = dtemp;
        }
        for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = ynew[i];
            ynew[i] = ynew[j];
            ynew[j] = dtemp;
        }
    }
    for (i=1; i<n; i++)   /* is sequence strictly monotonic and increasing? */
    {   if (x[i] <= x[i-1])   /* no */
        {   if (halfn)
            {   for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = x[i];
                    x[i] = x[j];
                    x[j] = dtemp;
                }
                for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = y[i];
                    y[i] = y[j];
                    y[j] = dtemp;
                }
            }
            if (halfnew)
            {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = xnew[i];
                    xnew[i] = xnew[j];
                    xnew[j] = dtemp;
                }
                for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = ynew[i];
                    ynew[i] = ynew[j];
                    ynew[j] = dtemp;
                }
            }
            return 2;
        }
    }
    for (i=1; i<nnew; i++) /* is sequence strictly monotonic and increasing? */
    {   if (xnew[i] <= xnew[i-1])   /* no */
        {   if (halfn)
            {   for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = x[i];
                    x[i] = x[j];
                    x[j] = dtemp;
                }
                for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = y[i];
                    y[i] = y[j];
                    y[j] = dtemp;
                }
            }
            if (halfnew)
            {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = xnew[i];
                    xnew[i] = xnew[j];
                    xnew[j] = dtemp;
                }
                for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = ynew[i];
                    ynew[i] = ynew[j];
                    ynew[j] = dtemp;
                }
            }
            return 3;
        }
    }
    /* do interpolated independent values exceed range? */
    if ((xnew[0] < x[0]) || (xnew[nnew-1] > x[nm1])) return 4;

    /* Allocate storage for work arrays */
    s =    (double*)malloc(sizeof(double)*n);
    g =    (double*)malloc(sizeof(double)*n);
    work = (double*)malloc(sizeof(double)*n);
    if (s==NULL || g==NULL || work==NULL)
    {   if (s)    free(s);
        if (g)    free(g);
        if (work) free(work);
        if (halfn)
        {   for (i=0,j=n-1; i<halfn; i++,j--)  /* reverse sequence */
            {   dtemp = x[i];
                x[i] = x[j];
                x[j] = dtemp;
            }
            for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
            {   dtemp = y[i];
                y[i] = y[j];
                y[j] = dtemp;
            }
        }
        if (halfnew)
        {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
            {   dtemp = xnew[i];
                xnew[i] = xnew[j];
                xnew[j] = dtemp;
            }
            for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
            {   dtemp = ynew[i];
                ynew[i] = ynew[j];
                ynew[j] = dtemp;
            }
        }
        return 5;
    }

    /* Initialize coefficients */
    for (i=1; i<nm1; i++)
    {   xi   = x[i];
        xim1 = x[i-1];
        xip1 = x[i+1];
        yi   = y[i];
        xx = xi - xim1;
        work[i] = 0.5*xx/(xip1-xim1);
        t = ((y[i+1]-yi)/(xip1-xi) - (yi-y[i-1])/xx)/(xip1-xim1);
        s[i] = 2.0 * t;
        g[i] = 3.0 * t;
    }
    s[0]   = 0.0;
    s[nm1] = 0.0;

    /* Solve system of equations */
    w = 1.07179676976;    /* 8.0 - 4.0*sqrt(3.0); */
    do
    {   u = 0.0;
        for (i=1; i<nm1; i++)
        {   xx = work[i];
            t = w*( g[i]-s[i] - (xx*s[i-1]) - ((0.5-xx)*s[i+1]) );
            h = fabs(t);
            if (h > u) u = h;
            s[i] +=  t;
        }
    } while (u >= eps);
    for (i=0; i<nm1; i++)
        g[i] = (s[i+1] - s[i]) / (x[i+1] - x[i]);

    /* Interpolate */
    i = 0;                   /* first member of x[] nd y[] arrays */
    for (j=0; j<nnew; j++)    /* look at each member of xnew[] array */
    {   t = xnew[j];
        do
        {   i++;
        } while ((i < nm1) && (t > x[i]));
        ip1 = i;
        i--;
        h  = xnew[j] - x[i];
        ynew[j] = ( (y[ip1] - y[i]) / (x[ip1] - x[i]) ) * h +
                    y[i] +
                    ( h * (xnew[j] - x[ip1]) ) *
                    ( (s[i] + s[ip1] + s[i] + (g[i] * h)) / 6.0 );
    }

    /* Deallocate storage and return success */
    free(s);
    free(g);
    free(work);
    if (halfn)
    {   for (i=0,j=n-1; i<halfn; i++,j--)  /* reverse sequence */
        {   dtemp = x[i];
            x[i] = x[j];
            x[j] = dtemp;
        }
        for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
        {   dtemp = y[i];
            y[i] = y[j];
            y[j] = dtemp;
        }
    }
    if (halfnew)
    {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = xnew[i];
            xnew[i] = xnew[j];
            xnew[j] = dtemp;
        }
        for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = ynew[i];
            ynew[i] = ynew[j];
            ynew[j] = dtemp;
        }
    }
    return 0;
}
/******************************* Spline2() **********************************/
/*  This function performs a cubic spline interpolation to get the values
 *  "ynew[i]" as a function of "xnew[i]".  "xnew[]" is an independent variable
 *  array assigned values that between or outside of the endpoints of the
 *  independent series "x[]".  The values "y[i]" are defined for every point
 *  "x[i]".  Essentially, "ynew[]" is interpolated along "y[]".
 *
 *  NOTE: Both "x[]" and "xnew[]" must be strictly monotonically increasing,
 *        with no adjacent values equal.  This version of the function
 *        permits monotonically decreasing values, by "reversing" the series
 *        before splining then returning the values to original order.  "x[]"
 *        "znew[]" may be monotonic on opposite or the same directions.
 *
 * #####################################################################
 * # NOTE: This function differs from Spline() by allowing xnew[] values to
 * #       lie outside the endpoints of x[].  Only values within the limits
 * #       of x[] are used to get ynew[].
 * # You should initialize ynew[] before calling this function so you know
 * # if a ynew[] value has not been set.
 * #####################################################################
 *
 * #####################################################################
 * #                                                                   #
 * #   NOTE: Versions before 2/12/97 have 2 MAJOR!!! bugs.             #
 * #         1. When sequences were "reversed" only X was reversed!    #
 * #         2. When xnew was reversed n, rather than nnew, was used.  #
 * #                                                                   #
 * #####################################################################
 *
 *  Parameters:
 *  Input:
 *      n       - size of x[] and y[] arrays.
 *      new     - size of xnew[] and ynew[] arrays.
 *      x[]     - independent variable array.
 *      y[]     - dependent variable array.
 *      xnew[]  - interpolated independent variable array.  The start and end
 *                of these values should not exceed the start and end of x[].
 *  Output:
 *      ynew[]  - interpolated dependent variable array.
 *
 *  Requires: <stdlib.h>, <math.h>, and attached header.
 *  Calls: malloc, free, sqrt, fabs.
 *  Returns: 0 on success, or
 *          >0 if error or warning (offset into an array of message strings).
 *
const char *Spline2Msg[] =
{   "Spline2(): No errors.",
    "Spline2() ERROR: arrays too small to spline.",
    "Spline2() ERROR: independent variable array not strictly monotonic.",
    "Spline2() ERROR: interpolated independent variable array not strictly monotonic.",
    "Spline2() ERROR: unable to allocate temporary storage.",
}
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 12, 1997
 */
int Spline2 (long n,long nnew,double x[],double y[],double xnew[],double ynew[])
{
    long i, j, ip1, nm1;
    long halfn   = 0;
    long halfnew = 0;
    double *s    = NULL;
    double *g    = NULL;
    double *work = NULL;
    double eps   = 1.0e-6;    /* allowable error factor */
    double xi, xim1, xip1, yi, xx, h, t, w, u, dtemp;

    /* Pre-qualify input */

    if ((n < 3) || (nnew < 1)) return 1;  /* are there enough points? */
    nm1 = n - 1;
    if (x[n-1] < x[0])                    /* is vector decreasing? */
    {   halfn = n/2;                      /* truncation wanted */
        for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
        {   dtemp = x[i];
            x[i] = x[j];
            x[j] = dtemp;
        }
        for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
        {   dtemp = y[i];
            y[i] = y[j];
            y[j] = dtemp;
        }
    }
    if (xnew[nnew-1] < xnew[0])           /* is vector decreasing? */
    {   halfnew = nnew/2;                    /* truncation wanted */
        for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = xnew[i];
            xnew[i] = xnew[j];
            xnew[j] = dtemp;
        }
        for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = ynew[i];
            ynew[i] = ynew[j];
            ynew[j] = dtemp;
        }
    }
    for (i=1; i<n; i++)   /* is sequence strictly monotonic and increasing? */
    {   if (x[i] <= x[i-1])   /* no */
        {   if (halfn)
            {   for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = x[i];
                    x[i] = x[j];
                    x[j] = dtemp;
                }
                for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = y[i];
                    y[i] = y[j];
                    y[j] = dtemp;
                }
            }
            if (halfnew)
            {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = xnew[i];
                    xnew[i] = xnew[j];
                    xnew[j] = dtemp;
                }
                for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = ynew[i];
                    ynew[i] = ynew[j];
                    ynew[j] = dtemp;
                }
            }
            return 2;
        }
    }
    for (i=1; i<nnew; i++) /* is sequence strictly monotonic and increasing? */
    {   if (xnew[i] <= xnew[i-1])   /* no */
        {   if (halfn)
            {   for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = x[i];
                    x[i] = x[j];
                    x[j] = dtemp;
                }
                for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
                {   dtemp = y[i];
                    y[i] = y[j];
                    y[j] = dtemp;
                }
            }
            if (halfnew)
            {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = xnew[i];
                    xnew[i] = xnew[j];
                    xnew[j] = dtemp;
                }
                for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
                {   dtemp = ynew[i];
                    ynew[i] = ynew[j];
                    ynew[j] = dtemp;
                }
            }
            return 2;
        }
    }

    /* Allocate storage for work arrays */
    s =    (double*)malloc(sizeof(double)*n);
    g =    (double*)malloc(sizeof(double)*n);
    work = (double*)malloc(sizeof(double)*n);
    if (s==NULL || g==NULL || work==NULL)
    {   if (s)    free(s);
        if (g)    free(g);
        if (work) free(work);
        if (halfn)
        {   for (i=0,j=n-1; i<halfn; i++,j--)  /* reverse sequence */
            {   dtemp = x[i];
                x[i] = x[j];
                x[j] = dtemp;
            }
            for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
            {   dtemp = y[i];
                y[i] = y[j];
                y[j] = dtemp;
            }
        }
        if (halfnew)
        {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
            {   dtemp = xnew[i];
                xnew[i] = xnew[j];
                xnew[j] = dtemp;
            }
            for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
            {   dtemp = ynew[i];
                ynew[i] = ynew[j];
                ynew[j] = dtemp;
            }
        }
        return 4;
    }

    /* Initialize coefficients */
    for (i=1; i<nm1; i++)
    {   xi   = x[i];
        xim1 = x[i-1];
        xip1 = x[i+1];
        yi   = y[i];
        xx = xi - xim1;
        work[i] = 0.5*xx/(xip1-xim1);
        t = ((y[i+1]-yi)/(xip1-xi) - (yi-y[i-1])/xx)/(xip1-xim1);
        s[i] = 2.0 * t;
        g[i] = 3.0 * t;
    }
    s[0]   = 0.0;
    s[nm1] = 0.0;

    /* Solve system of equations */
    w = 1.07179676976;    /* 8.0 - 4.0*sqrt(3.0); */
    do
    {   u = 0.0;
        for (i=1; i<nm1; i++)
        {   xx = work[i];
            t = w*( g[i]-s[i] - (xx*s[i-1]) - ((0.5-xx)*s[i+1]) );
            h = fabs(t);
            if (h > u) u = h;
            s[i] +=  t;
        }
    } while (u >= eps);
    for (i=0; i<nm1; i++)
        g[i] = (s[i+1] - s[i]) / (x[i+1] - x[i]);

    /* Interpolate */
    i = 0;                    /* first member of x[] and y[] arrays */
    for (j=0; j<nnew; j++)    /* look at each member of xnew[] array */
    {   if ((xnew[j] < x[0]) || (xnew[j] > x[nm1])) continue; /* skip */
        t = xnew[j];
        do
        {   i++;
        } while ((i < nm1) && (t > x[i]));
        ip1 = i;
        i--;
        h  = xnew[j] - x[i];
        ynew[j] = ( (y[ip1] - y[i]) / (x[ip1] - x[i]) ) * h +
                    y[i] +
                    ( h * (xnew[j] - x[ip1]) ) *
                    ( (s[i] + s[ip1] + s[i] + (g[i] * h)) / 6.0 );
    }

    /* Deallocate storage and return success */
    free(s);
    free(g);
    free(work);
    if (halfn)
    {   for (i=0,j=n-1; i<halfn; i++,j--)  /* reverse sequence */
        {   dtemp = x[i];
            x[i] = x[j];
            x[j] = dtemp;
        }
        for (i=0,j=n-1; i<halfn; i++,j--) /* reverse sequence */
        {   dtemp = y[i];
            y[i] = y[j];
            y[j] = dtemp;
        }
    }
    if (halfnew)
    {   for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = xnew[i];
            xnew[i] = xnew[j];
            xnew[j] = dtemp;
        }
        for (i=0,j=nnew-1; i<halfnew; i++,j--) /* reverse sequence */
        {   dtemp = ynew[i];
            ynew[i] = ynew[j];
            ynew[j] = dtemp;
        }
    }
    return 0;
}
/********************************* TrimStr() ********************************/
/* Deletes leading and trailing blanks and non-printable characters from
 * the string at p.  Extended ASCII characters (0x80 to 0xFF) are considered
 * non-printable.  If extended ASCII characters are to be kept, change code
 * to "...<0x21 || ....==0x7F ...".  Beginning of string remains at p.  If
 * the string consists entirely of spaces, tabs or other unwanted characters,
 * the NULL string is returned (p[0] will equal 0).
 *
 * Requires: <string.h>.
 * Calls: strlen.
 * Returns: char* to beginning of string.
 *
 * Author: Jeff Lucius   USGS   Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 */
char *TrimStr (char *p)
{
    int end,i,len,trail=0,lead=0;

    if (p == NULL) return p;

    /* return if NULL string */
    if ((end=(len=strlen(p))) == 0)  return p;

    /* count blanks etc. at end */
    while ((end>0) && (p[--end]<0x21 || p[end]>0x7E))  trail++;

    /* strip of blanks etc. by inserting terminating 0 */
    if (trail)  p[len-=trail] = 0;

    /* return if NULL string */
    if (len == 0) return p;

    /* count blanks etc. at beginning */
    while (p[lead]<0x21 || p[lead]>0x7E)  lead++;

    /* remove leading blanks etc. by shifting characters forward */
    if (lead)  for (i=lead; i<=len; i++)  p[i-lead] = p[i];

    /* return pointer to start of string */
    return p;
}
