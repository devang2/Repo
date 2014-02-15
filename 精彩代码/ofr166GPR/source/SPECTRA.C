/* SPECTRA.C    version 2.0
 * Display amplitude spectra of a GPR scan to the cRT and optionally save
 * to disk as a HPGL file.
 *
 * Jeff Lucius, Geophysicist
 * Department of Interior
 * U.S. Geological Survey
 * Geologic Division
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 *
 * Revision History:
 *   March 22, 1993 - Completed.
 *   March 18, 1994 - Revised to handle 8-bit and 16-bit unsigned data with a
 *                    known file header size.
 *                  - Calculation of fundamental freq. was corrected from
 *                    num_samps to num_samps-1 times the samp_rate.
 *   March 13, 1995 - Added video_mode to command line arguments.
 *   May 14, 1997   - Removed video_mode to command line arguments.
 *                  - added data type long
 *                  - Added trace header bytes
 *   April 24, 2001 - Add nicer axes using DrawAxis().
 *                  - Add nice fonts.
 *                  - Changed version to 2.0
 *
 * Supported compilers:
 *   Intel 386/486 C Code Builder 1.1a
 *     To compile for 80386 executable:
 *       icc /F /xnovm /s010000H  spectra.c libhpgl.lib
 *     To compile for 80486 executable :
 *       icc /F /xnovm /s010000H /zmod486 spectra.c libhpgl.lib
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /s010000H allocates 64K of stack space (required by graphics module)
 *       NOTE: This may have to be increased to 1 MB (/s0100000H) to
 *             accomdate some imaging routines in the graphics module.
 *     /zmod486 generates specific 80486 instructions, speeds up program on 80486
 *     Gary Olhoeft's 32-bit graphics modules are in libhpgl.lib
 *
 *  To run: SPECTRA filename file_hdr trace_hdr samp_bytes num_samps num samp_rate [pltfile]
 */

/* Header files */
#include <conio.h>     /* getch */
#include <hpgl.h>      /* GRO HPGL graphics functions */
#include <math.h>      /* cos,sin,sqrt,log10 */
#include <stdio.h>     /* fopen/fclose,fread,FILE,fseek,puts,printf,NULL */
#include <stdlib.h>    /* atoi */
#include <string.h>    /* strcpy */
#include "jl_defs.h"

int test  = 0;
int Debug = 0;

/* Function prototypes */
void FFT1D(double *data,int nn,int isign);
int DrawAxis(int side,int pen_number,int vcols,int vrows,
      int tick_show,int tick_num,int tick_ano,int tick_skip,int tick_mid_skip,
      int ano_left,int ano_right,double axis_min,double axis_max,
      double tick_start,double tick_int,double tick_abs_min,
      double tick_abs_max,double title_size,double title_offset,
      double ano_size,double VX1,double VX2,double VY1,double VY2,
      char *title);
int SetLabel(int *left,int *right,double val,char *str);

int main(int argc, char *argv[])
{
	char plotfile[80],str[80];
	void *scan     = NULL;
	int video_mode;
	int modes[] =     /* known 256-color video modes (exc. 18) */
		{ 263, 261, 56, 1024, 8514, 259, 48, 800, 99, 46, 257, 640, 98, 95, 20, 18 };
	int period     = 24;
	int i,j,itemp;
    int scan_bytes,scan_number,half_length,samp_bytes;
	int file_hdr_bytes,trace_hdr_bytes;
	int filebytes,filescans,num_samps;
    int vcols,vrows;
	double dmax,dtemp;
	double samp_rate;         /* ns per scan point */
	double mean;
	double *d_scan     = NULL;
	double *cmplx_scan = NULL;
    double pi          = 3.14159265358979323846;
	double twopi       = 2*pi;
    double fundamental,nyquist,dmean;
    double step,t;
    FILE *infile;

	/* Get command line parameters */
	if (argc < 8)
	{   puts("\n\n#############################################################################");
		puts("Usage:");
		puts("SPECTRA filename file_hdr trace_hdr samp_bytes num_samps num samp_rate [pltfile]");
		puts("  Display a GPR trace and its amplitude spectra as wiggle traces.");
        puts("  Computer code written by Jeff Lucius, USGS, lucius@usgs.gov.");
		puts("    \"filename\" is the input GPR file.");
		puts("    \"file_hdr\" is the number of bytes in the header at the start of");
		puts("        the file. Enter 0 for no header.");
		puts("    \"trace_hdr\" is the number of bytes in the header at the start of");
		puts("        each trace. Enter 0 for no header.");
		puts("    \"samp_bytes\" is the number of bytes in each scan sample. Use 1 for");
		puts("        8-bit data, 2 for 16-bit data, etc. Make negative for signed data.");
		puts("    \"num_samps\" is the number of samples in each trace.");
		puts("    \"num\" is the trace to use from the file. The first trace is trace 0.");
		puts("    \"samp_rate\" is the number of nanoseconds per trace sample.");
		puts("    \"pltfile\" is optional, if not given output is to the CRT only.");
        puts("              If given, then the screen plot is saved to a PLT file.");
		printf("   Version 2.0 - Last Revision Date: %s at %s\n","April 24, 2001",__TIME__);
		puts("#############################################################################");
		return 1;
	}
	if ((infile = fopen(argv[1],"rb")) == NULL)
	{   printf("\nERROR opening input file %s\n",argv[1]);
		return 2;
	}
	file_hdr_bytes  = atoi(argv[2]);
	trace_hdr_bytes = atoi(argv[3]);
	samp_bytes      = atoi(argv[4]);
	num_samps       = atoi(argv[5]);
	scan_number     = atoi(argv[6]);
	samp_rate       = atof(argv[7]);
	if (argc > 8) strcpy(plotfile,argv[8]);
	else          strcpy(plotfile,"CRT");

	/* Validate parameters and initialize */
	if (samp_rate <= 0.0)
	{   puts("ERROR: Sample rate <= 0.0.");
		fclose(infile);
		return 3;
	}
	if (samp_bytes == 1 || samp_bytes == -1)
		mean = 0x80U;
	else if (samp_bytes == 2 || samp_bytes == -2)
		mean = 0x8000U;
	else if (samp_bytes == 4 || samp_bytes == -4)
		mean = 0x80000000U;
	else
	{   puts("ERROR: samp_bytes must be 1, -1, 2, -2, 4, or -4.");
		fclose(infile);
		return 4;
	}
	half_length = num_samps / 2;
	scan_bytes  = num_samps * samp_bytes;
	if (scan_bytes < 0) scan_bytes *= -1;
	fseek(infile, 0L, SEEK_END);       /* go to end of file */
	filebytes   = ftell(infile);       /* get last location (byte) */
	rewind(infile);
	filescans   = (filebytes - file_hdr_bytes) / scan_bytes;
	if (scan_number < 0) scan_number = 0;
	if (scan_number >= filescans) scan_number = filescans-1;
	fundamental = (1000.0/((double)(num_samps-1) * samp_rate)); /* MHz */
	nyquist = fundamental * half_length;

	/* Dipslay info */
	puts("\n");
    printf("\nSpectra version 2.0\nComputer code written by Jeff Lucius, USGS, lucius@usgs.gov\n");
	printf("filename        = %s\n",argv[1]);
	printf("file_hdr_bytes  = %d\n",file_hdr_bytes);
	printf("trace_hdr_bytes = %d\n",trace_hdr_bytes);
	printf("samp_bytes  	= %d\n",samp_bytes);
	printf("num_samps   	= %d\n",num_samps);
	printf("num_scans   	= %d\n",filescans);
	printf("scan_number 	= %d\n",scan_number);
	printf("samp_rate   	= %g\n",samp_rate);
	printf("fundamental 	= %g MHz\n",fundamental);
	printf("nyquist     	= %g MHz\n",nyquist);
	printf("plotfile    	= %s\n",plotfile);
	printf("\nPress a key to continue or <Esc> to quit...");
	if (getch() == 27)
	{   fclose(infile);
		return 0;
	}
	puts("");

	/* Allocate storage */
	scan = (void *)malloc(scan_bytes);
	d_scan = (double *)malloc(scan_bytes*sizeof(double));
	cmplx_scan = (double *)malloc(2*scan_bytes*sizeof(double));
	if (scan==NULL || d_scan==NULL || cmplx_scan==NULL)
	{   puts("\nERROR allocating storage.");
		fclose(infile);
		free(scan);  free(d_scan);  free(cmplx_scan);
		return 3;
	}

	/* Get the scan from the file */
	fseek(infile,(long)(file_hdr_bytes + (scan_number*(scan_bytes+trace_hdr_bytes)) + trace_hdr_bytes),SEEK_SET);
	if ((fread(scan,(size_t)scan_bytes,(size_t)1,infile)) < 1)
	{   puts("\nERROR getting scan from file.");
		fclose(infile);
		free(scan);  free(d_scan);  free(cmplx_scan);
		return 4;
	}
	fclose(infile);

	/* Convert to double and find average value of the time series */
	dmean = 0.0;
	if (samp_bytes == 1)
	{   for (i=0; i<num_samps; i++)
		{   d_scan[i] = ((unsigned char *)scan)[i];
			dmean += d_scan[i];
		}
	} else if (samp_bytes == -1)
	{   for (i=0; i<num_samps; i++)
		{   d_scan[i] = ((char *)scan)[i];
			dmean += d_scan[i];
		}
	} else if (samp_bytes == 2)
	{   for (i=0; i<num_samps; i++)
		{   d_scan[i] = ((unsigned short *)scan)[i];
			dmean += d_scan[i];
		}
	} else if (samp_bytes == -2)
	{   for (i=0; i<num_samps; i++)
		{   d_scan[i] = ((short *)scan)[i];
			dmean += d_scan[i];
		}
	} else if (samp_bytes == 4)
	{   for (i=0; i<num_samps; i++)
		{   d_scan[i] = ((unsigned long *)scan)[i];
			dmean += d_scan[i];
		}
	} else if (samp_bytes == -4)
	{   for (i=0; i<num_samps; i++)
		{   d_scan[i] = ((long *)scan)[i];
			dmean += d_scan[i];
		}
	}
	dmean /= (double)num_samps;

	if (!test)
	{
        /* Initialize graphics: plotter_is(video_mode,plotfile); */
        for (i=0; i<15; i++) /* try all known 256 color modes hi to lo res */
		{ if ((video_mode = plotter_is(modes[i],plotfile)) >= 20)
				break;  /* out of for loop */
		}
        vcols = v_colx + 1;       /* number of screen horizontal pixels */
        vrows = v_rowy + 1;       /* number of screen vertical pixels */
		lorg(3);
		ldir(0.0);
		csize(1.0);
        hpgl_select_font("romtrplx");

		/* Display input scan */
		viewport(10.,125.,55.,90.);
		window(0.,(double)num_samps-1.,-mean,mean);
		pen(3);
		frame();
		sprintf(str,"GPR wavelet #%d from %s",scan_number,argv[1]);
		label(0.,1.1 * mean,str);
		pen(15);
		plot(0.0,(double)((unsigned char *)scan)[0]-mean,-2);
		if (samp_bytes > 0)
		{   for (i=1; i<num_samps; i++)
				plot((double)i,d_scan[i]-mean,-1);
		} else
		{   for (i=1; i<num_samps; i++)
				plot((double)i,d_scan[i],-1);
		}

		pen(4);                           /* draw 0 line */
		plot(0.0,0.0,-2);
		plot((double)num_samps,0.,-1);

/*
		viewport(10.,125.,55.,90.);
		window(0.,(double)half_length,0.0,1.0);
        csize(0.5);
		lorg(4);
		pen(15);
		label(1.0,0.8,"0");
		plot(0.0,1.0,-2);
		plot(0.0,0.9,-1);
		sprintf(str,"%5d",num_samps-1);
		label(1.0,0.8,str);
		plot(1.0,1.0,-2);
		plot(1.0,0.9,-1);
 */
    /* int DrawAxis(int side,int pen_number,int vcols,int vrows,
          int tick_show,int tick_num,int tick_ano,int tick_skip,int tick_mid_skip,
          int ano_left,int ano_right,double axis_min,double axis_max,

          double tick_start,double tick_int,double tick_abs_min,
          double tick_abs_max,double title_size,double title_offset,
          double ano_size,double VX1,double VX2,double VY1,double VY2,
          char *title);
     */
		sprintf(str,"Sample number (%g ns/samp)",samp_rate);
        itemp=DrawAxis(-2,15,vcols,vrows,
                        1,9,1,0,-1,
                        0,-1,0.0,(double)(num_samps-1),

                        0.0,INVALID_VALUE,0.0,
                        INVALID_VALUE,0.6,0.5,
                        0.5,10.0,125.0,55.0,90.0,
                        str);

	}

	/* Remove DC shift (to minimize wavenumber 0) */
	for (i=0; i<num_samps; i++)  d_scan[i] -= dmean;

	/* Smooth ends (remove dicontinuities at ends to insure periodicity) */
	step = twopi/(double)period;
	itemp = num_samps-1;
	for (t=0.0,j=0; j<period/2; t+=step,j++)
	{   dtemp = 0.5 - 0.5*cos(t);          /* Hanning window */
		d_scan[j] *= dtemp;
		d_scan[itemp-j] *= dtemp;
	}

	/* Copy real time series to a complex array */
	for (i=0; i<num_samps; i++)
	{   cmplx_scan[i<<1] = d_scan[i];
		cmplx_scan[(i<<1)+1] = 0.0;
	}

	/* FFT, calculate amplitude spectra, and display */
	FFT1D(cmplx_scan,num_samps,1);
	dmax = -1e20;
	for (i=0; i<num_samps/2; i++)
	{   itemp = i<<1;
		d_scan[i] = sqrt(cmplx_scan[itemp]*cmplx_scan[itemp] + cmplx_scan[itemp+1]*cmplx_scan[itemp+1]);
		if (d_scan[i] > dmax) dmax = d_scan[i];
	}
	if (!test)
	{
		viewport(10.,125.,8.,43.);
		window(0.,(double)num_samps-1.,-mean,mean);
		frame();
		pen(3);
        csize(1.0);
        lorg(3);
		label(0.,1.1 * mean,"Amplitude spectra");

        viewport(10.,125.,8.,43.);
		window(0.,(double)half_length,0.0,dmax);
		pen(15);
		plot(1.0,d_scan[1],-2);
		for (i=2; i<half_length; i++)
			plot((double)i,d_scan[i],-1);

    /* int DrawAxis(int side,int pen_number,int vcols,int vrows,
          int tick_show,int tick_num,int tick_ano,int tick_skip,int tick_mid_skip,
          int ano_left,int ano_right,double axis_min,double axis_max,

          double tick_start,double tick_int,double tick_abs_min,
          double tick_abs_max,double title_size,double title_offset,
          double ano_size,double VX1,double VX2,double VY1,double VY2,
          char *title);
     */
        itemp=DrawAxis(-2,15,vcols,vrows,
                        1,9,1,0,-1,
                        0,0,fundamental+0.0001,nyquist+0.0001,

                        fundamental+0.0001,(double)((int)((nyquist/10)+0.5)),0.0,
                        1000000,0.6,0.5,
                        0.5,10.0,125.0,8.0,43.0,
                        "Frequency (Hz)");

		/* Clean-up */
		while (kbhit()) getch();
		getch();
		plotter_off();
	}

    free(scan);  free(d_scan);  free(cmplx_scan);
	printf("d_scan[0]=%g  dmax=%g\n",d_scan[0],dmax);
	printf("fundamental = %g\n",fundamental);
	printf("nyquist = %g\n",nyquist);
	printf("scan mean = %g\n",dmean);
    return 0;
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
 * Calls: none.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: November 10, 1992
 */
void FFT1D (double *data,int nn,int isign)
{
    int i,j,m,n,mmax,istep,jp1;
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
/******************************** DrawAxis() ********************************/
/* This function attaches an axis to one side of a rectangle defined by VX1,
 * VX2, VY1, and VY2 (the minimums and maximums of the X and Y directions
 * of the screen viewport).  Values for the endpoints of the axis must be
 * given.  A label for the axis may also be given.
 * If tick_start or tick_int are not both given then tick_num is used to
 * calculate the tick mark locations. Then the actual number of tick marks and
 * the interval between marks is decided by this function, based on the suggested
 * value.  Useful ranges are between +/-0.001 and +/-999999.0.  Only tick
 * annotations (the values) that do not touch each other will be drawn.
 * Also if the tick marks themselves are so close as to touch each other,
 * the number of ticks will be reduced.
 * Tick marks are drawn only between axis_min and axis_max.
 *
 * NOTE:  This version differs from earlier opnes in that the endpoints of
 *        the axis do not have to be drawn and Can now label left and right
 *        axes with tick_ano turned off.
 *
 * Parameter list:
 *  int side          - indicates which side of the viewport to place axis on
 *                      1=right, 2=top, -1=left, -2=bottom
 *                      0=no ticks, label title at top
 *  int pennumber     - value to set pen to, color depends on palette in use
 *  int vcols         - number of CRT horizontal pixels
 *  int vrows         - number of CRT vertical pixels
 *  int tick_show     -  0 to not draw ticks, 1 to draw on outside of window,
 *                      -1 to draw on inside of window
 *  int tick_num      - >0, suggested number of tick marks for axis (optional)
 *  int tick_ano      - 1 to label tick marks, 0 for no labels
 *  int tick_skip     - number of tick marks to skip between labels
 *  int tick_mid_skip - number of tick marks to skip between 2nd order tick
 *  int ano_left      - number of places to left of the decimal for the tick label
 *  int ano_right     - number of places to right of the decimal for the tick label
 *  double axis_max   - value for top of left or right axes, or right on top or bottom axes
 *  double axis_min   - value for bottom of left or right axes or left on top or bottom axes
 *  double tick_start - value for first tick mark (valid value optional)
 *  double tick_int   - separation between ticks (valid value optional)
 *  double tick_abs_min - no ticks displayed "before" this value
 *  double tick_abs_max - no ticks displayed "after" this value
 *  double title_size - multiplier for axis title csize()
 *  double title_offset - number of characters widths to move title closer (-)
 *                        or farther (+) from axis
 *  double ano_size   - multiplier for tick mark label csize()
 *  double VX1,VX2    - horizontal left and right edges of screen viewport for
 *                      primary data display window (range is 0 to 133.333)
 *  double VY1,VY2    - vertical left and right edges of screen viewport for
 *                      primary data display window (range is 0 to 100.00)
 *                NOTE: viewport() origin is lower left screen corner.
 *  char *title       - axis label (none if NULL string)
 *
 * ERROR messages:
 * const char *DrawAxisMsg[] =
 * {  "DrawAxis(): No errors.",
 *    "DrawAxis() ERROR: Invalid value for \"side\".",
 *    "DrawAxis() WARNING: No room for axis to attach to viewport.",
 *    "DrawAxis() WARNING: Invalid range for viewport limits.",
 *    "DrawAxis() ERROR: Axis endpoints out of SetLabel() range.",
 * } ;
 *
 * Manifest constants:
 *    #define TRUE   1
 *    #define FALSE  0
 *    #define INVALID_VALUE 1.0E19
 *    #define DA_TITLE          0
 *    #define DA_RIGHT          1
 *    #define DA_TOP            2
 *    #define DA_LEFT         (-1)
 *    #define DA_BOTTOM       (-2)
 *    #define _ABS(a)  ( ((a) < 0) ? (-(a)) : (a) )
 *    #define _MAXX(a,b) ( ((a)>(b)) ? (a) : (b) )
 *
 * Globals:
 *  int Debug;
 *
 * NOTES:
 *   1) The graphics in this function require GRO's protected mode graphics
 *      libraries.
 *   2) This function inherits the graphics mode and coordinate systems
 *      of the calling function.  Plotting is 2D only.
 *
 * WARNINGS:
 *   1) The viewport is modified!
 *
 * Requires: <hpgl.h>, <math.h>, "jl_defs.h".
 * Calls: unclip, lorg, ldir, csize, pen, plot, pow, modf, SetLabel, printf.
 * Returns: 0 on success, or
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Team   Lakewood, CO
 * Date: September 15, 1995
 */
int DrawAxis (int side,int pen_number,int vcols,int vrows,
              int tick_show,int tick_num,int tick_ano,int tick_skip,int tick_mid_skip,
              int ano_left,int ano_right,double axis_min,double axis_max,
              double tick_start,double tick_int,double tick_abs_min,
              double tick_abs_max,double title_size,double title_offset,
              double ano_size,double VX1,double VX2,double VY1,double VY2,
              char *title)
{
    char   str[20];
    int    itemp,i,left,right,tick_count,inside;
    double max_axis,chr_size,user_per_vp,tick_size,label_off;
    double ano_width,min_ano_sep,last_ano,first_ano;
    double dtemp,mult,frac,ipart;
    double pix_per_vp,axis_range;
    double var1,var2,var3,var4,var5,edge;
    double tick_set[5] = {1.0, 2.0, 2.5, 5.0, 10.0};

    static int set_size  = 5;
    static int tick_pix_width = 5;
    static double pi     = 3.14159265358979323846;
    static double min_VX = 0.0;           /* GRO hpgl lib limits for screen */
    static double min_VY = 0.0;
    static double max_VX;
    static double max_VY = 100.0;
    static double rounding_error = 0.0001;

    extern int Debug;

    /* Reality check */
    if (VX2 == VX1 || VY2 == VY1 || VX2 < VX1 || VY2 < VY1) return 3;
    if (tick_num == 0) tick_num = 9;
    if (side == DA_TITLE) tick_num = 0;
    inside = FALSE;
    if (tick_show == FALSE) tick_num = 0;
    if (tick_show < 0) inside = TRUE;
    if (title_size == 0.0 || title_size == INVALID_VALUE)
    {   if (side) title_size = 0.85;
        else      title_size = 1.10;
    }
    if (ano_size == 0.0 || ano_size == INVALID_VALUE) ano_size = 0.80;
    if (ano_left == INVALID_VALUE)  ano_left  =  0;
    if (ano_left > 6)               ano_left  =  6;
    if (ano_right == INVALID_VALUE) ano_right = -1;
    if (ano_right > 3)              ano_right =  3;

    /* Set viewport to axis area, scale window so x and y user units the same,
     * get maximum viewport axis for csize(), set tick length to 1 viewport
     * horizontal unit.  Viewport origin is lower left screen corner.
     * pix_per_vp converts viewport units to number of pixels.
     */
    max_VX = 100.0 * ((double)vcols/(double)vrows);
    axis_range = _ABS(axis_max-axis_min);
    if (Debug)
    {   printf("\nDrawAxis: xmin=%g xmax=%g ymin=%g ymax=%g\n",min_VX,max_VX,min_VY,max_VY);
        printf("            VX1=%G VX2=%G VY1=%G VY2=%G\n",VX1,VX2,VY1,VY2);
        printf("            axis_min=%g  axis_max=%g  axis_range=%g\n",axis_min,axis_max,axis_range);
        printf("            vcols = %d  vrows = %d  tick_ano = %d tick_skip = %d ano_left = %d  ano_right = %d\n",
                vcols,vrows,tick_ano,tick_skip,ano_left,ano_right);
        printf("            tick_start = %g tick_num = %d  tick_int = %g  tick_abs_min = %g  tick_abs_max = %g\n",
                tick_start,tick_num,tick_int,tick_abs_min,tick_abs_max);
    } else
    {   unclip();
        pen(pen_number);
    }
    switch (side)
    {   case DA_RIGHT:             /* 1: right side */
            if (Debug) puts("DA_RIGHT");
            if (max_VX == VX2) return 2;
            var1 = VX2;
            var2 = max_VX - VX2;
            var3 = VY2 - VY1;
            var4 = max_VY;
            var5 = vrows;
            edge = var1+(var2/var3)*axis_range;
            if (!Debug)  viewport(VX2,max_VX,VY1,VY2);
            break;
        case DA_TITLE:  case DA_TOP:   /* 0 and 2: top side */
            if (Debug) puts("DA_TITLE or DA_TOP");
            if (max_VY == VY2) return 2;
            var1 = VY2;
            var2 = max_VY - VY2;
            var3 = VX2 - VX1;
            var4 = max_VX;
            var5 = vcols;
            edge = var1+(var2/var3)*axis_range;
            if (!Debug)  viewport(VX1,VX2,VY2,max_VY);
            break;
        case DA_LEFT:             /* left side */
            if (Debug) puts("DA_LEFT");
            if (VX1 == min_VX) return 2;
            var1 = VX1;
            var2 = VX1 - min_VX;
            var3 = VY2 - VY1;
            var4 = max_VY;
            var5 = vrows;
            edge = var1-(var2/var3)*axis_range;
            if (!Debug)  viewport(min_VX,VX1,VY1,VY2);
            break;
        case DA_BOTTOM:             /* bottom side */
            if (Debug) puts("DA_BOTTOM");
            if (VY1 == min_VY) return 2;
            var1 = VY1;
            var2 = VY1 - min_VY;
            var3 = VX2 - VX1;
            var4 = max_VX;
            var5 = vcols;
            edge = var1-(var2/var3)*axis_range;
            if (!Debug)  viewport(VX1,VX2,min_VY,VY1);
            break;
        default:
            return 1;
    }
    if (var3 > var2)
    {   max_axis = var3;
        user_per_vp = axis_range/max_axis;
    } else
    {   max_axis = var2;
        user_per_vp = _ABS(edge-var1)/max_axis;
    }
    tick_size = _ABS(edge-var1)/(var2);
    pix_per_vp = var5 / var4;
    if (tick_num != 0)
    {   while ((var3/tick_num)*pix_per_vp < tick_pix_width)
        {   tick_num /= 2;
            if (tick_num == 0) break;
        }
    }
    if (!Debug)
    {   switch (side)
        {   case DA_RIGHT:
                window(VX2,edge,axis_min,axis_max);
                break;
            case DA_TITLE:
            case DA_TOP:
                window(axis_min,axis_max,VY2,edge);
                break;
            case DA_LEFT:
                window(edge,VX1,axis_min,axis_max);
                break;
            case DA_BOTTOM:
                window(axis_min,axis_max,edge,VY1);
                break;
        }
    }

    /* Calculate sizes;
     * csize(1.0) == 2% of maximum viewport axis;
     * chr_size is title_size * 2% of full screen viewport vertical axis (100.0);
     * chr_size and ano_size are in viewport units;
     * user_per_vp converts viewport units to user units;
     * ano_width is character block width and height in user units;
     * min_ano_sep will have to be calculated on the fly for horizontal axes;
     */
    tick_size  *= 0.90;                           /* reduce ticksize slighty */
    label_off = tick_size;
    if (inside)
    {   tick_size *= -1.0;
        label_off *= 0.5;
    }
    ano_size   *= max_VY / max_axis;              /* csize for tick annotation */
    ano_width   = ano_size * 0.02 * max_axis * user_per_vp;
    min_ano_sep = 1.5 * ano_width;                /* this works good for left and right axes */
    chr_size    = title_size * (max_VY/max_axis); /* csize for axis labels */
    if (Debug)
        printf("side=%d tick_num=%d chr_size=%g max_axis=%g user_per_vp=%g\ntick_size=%g ano_width=%g ano_size=%g\n",
                side,tick_num,chr_size,max_axis,user_per_vp,tick_size,ano_width,ano_size);

    /* Draw axis and labels */
    if (side)
    {   /* Calculate interval between tick marks and starting tick, if nec. */
        if (tick_num && (tick_int==INVALID_VALUE || tick_start==INVALID_VALUE
                                                 || tick_int==0.0))
        {   if (tick_int==INVALID_VALUE || tick_int==0.0)
                tick_int = (axis_max-axis_min)/tick_num;   /* user units */
            left = 0;  right = -1;
            itemp = SetLabel(&left,&right,tick_int,str);
            if (itemp) return 4;
            if (left > 0)  mult = pow(10.0,(double)left-1.0);
            if (left < 0)  mult = pow(10.0,(double)(-left)-1.0);
            dtemp = _ABS(tick_int);
            if (left==1 && dtemp<1.0)
            {   if (dtemp >= 0.001) mult = 0.001;
                if (dtemp >= 0.01)  mult = 0.01;
                if (dtemp >= 0.1)   mult = 0.1;
            }
            tick_set[0] *= mult;
            for (i=1; i<set_size; i++)
            {   tick_set[i] *= mult;
                if (dtemp>=tick_set[i-1] && dtemp<tick_set[i])
                {   if (dtemp < (tick_set[i]-tick_set[i-1])/2.0)
                        dtemp = tick_set[i-1];
                    else
                        dtemp = tick_set[i];
                    break;
                }
            }
            if (tick_int > 0) tick_int =  dtemp;
            else              tick_int = -dtemp;
            if (tick_start == INVALID_VALUE)
            {   frac = modf(axis_min/tick_int,&ipart);
                if (_ABS(frac) < rounding_error) tick_start = axis_min;
                else                             tick_start = ipart * tick_int;
            }
        }
        if (Debug)
        {   printf("tick_int=%g  tick_start=%g  tick_skip=%d  tick_ano=%d\n",
                tick_int,tick_start,tick_skip,tick_ano);
            if (!getch()) getch();
        }

        /* Draw ticks and label */
        first_ano  = INVALID_VALUE;
        if (tick_num)
        {   tick_count = 0;
            tick_skip++;      /* turn 0 into 1, 1 into 2, etc. for modulo */
            tick_mid_skip++;  /* turn 0 into 1, 1 into 2, etc. for modulo */
            switch (side)
            {   case DA_RIGHT:      /* right side */
                    if (!Debug)
                    {   ldir(0.0);  lorg(2);  csize(ano_size);
                    }
                    if (tick_ano)
                    {   if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(VX2,dtemp,-2);
                                plot(VX2+tick_size,dtemp,-1);
                            } else if (tick_mid_skip > 0 &&  (tick_count % tick_mid_skip)==0)
                            {   plot(VX2,dtemp,-2);
                                plot(VX2+0.75*tick_size,dtemp,-1);
                            } else
                            {   plot(VX2,dtemp,-2);
                                plot(VX2+0.5*tick_size,dtemp,-1);
                            }
                        } else
                            printf("RIGHT tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if (Debug)
                            {   printf("           diff1=%g  diff2=%g  min_sep=%g  last_ano=%g\n",_ABS(dtemp-last_ano),_ABS(axis_max-dtemp),min_ano_sep,last_ano);
                                printf("     count=%d skip=%d mod=%d\n",tick_count,tick_skip,tick_count%tick_skip);
                            }
                            if ((tick_count % tick_skip) == 0)
                            {   if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   left = ano_left;  right = ano_right;
                                    SetLabel(&left,&right,dtemp,str);
                                    if (!Debug) label(VX2+label_off,dtemp,str);
                                    else        printf("     skip=%d left=%d right=%d x=%g y=%g\n",tick_skip,ano_left,ano_right,VX2+tick_size,dtemp);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE)  first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
                case DA_LEFT:     /* left side */
                    if (!Debug)
                    {   lorg(8);  ldir(0.0);  csize(ano_size);
                    }
                    if (tick_ano)
                    {   if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(VX1,dtemp,-2);
                                plot(VX1-tick_size,dtemp,-1);
                            } else if (tick_mid_skip > 0 && (tick_count % tick_mid_skip)==0)
                            {   plot(VX1,dtemp,-2);
                                plot(VX1-0.75*tick_size,dtemp,-1);
                            } else
                            {   plot(VX1,dtemp,-2);
                                plot(VX1-0.5*tick_size,dtemp,-1);
                            }
                        } else
                            printf("LEFT tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if (Debug)
                            {   printf("           diff1=%g  diff2=%g  min_sep=%g last_ano=%g test=%d\n",_ABS(dtemp-last_ano),_ABS(axis_max-dtemp),min_ano_sep,last_ano,((_ABS(dtemp-last_ano) > min_ano_sep) && (_ABS(axis_max-dtemp) > min_ano_sep)));
                                printf("     count=%d skip=%d mod=%d\n",tick_count,tick_skip,tick_count%tick_skip);
                            }
                            if ((tick_count % tick_skip) == 0)
                            {   if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   left = ano_left;  right = ano_right;
                                    SetLabel(&left,&right,dtemp,str);
                                    if (!Debug) label(VX1-label_off,dtemp,str);
                                    else        printf("     skip=%d left=%d right=%d x=%g y=%g\n",tick_skip,ano_left,ano_right,VX1-tick_size,dtemp);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE)  first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
                case DA_TOP:      /* top side */
                    if (!Debug)
                    {   lorg(6);  ldir(0.0);  csize(ano_size);
                    }
                    if (tick_ano)
                    {   left = ano_left;  right = ano_right;
                        SetLabel(&left,&right,axis_min,str);
                        min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                        if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                        if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(dtemp,VY2,-2);
                                plot(dtemp,VY2+tick_size,-1);
                            } else if (tick_mid_skip > 0 && (tick_count % tick_mid_skip)==0)
                            {   plot(dtemp,VY2,-2);
                                plot(dtemp,VY2+0.75*tick_size,-1);
                            } else
                            {   plot(dtemp,VY2,-2);
                                plot(dtemp,VY2+0.5*tick_size,-1);
                            }
                        } else
                            printf("TOP tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if ((tick_count % tick_skip) == 0)
                            {   left = ano_left;  right = ano_right;
                                SetLabel(&left,&right,dtemp,str);
                                min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                                if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                                if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   if (!Debug) label(dtemp,VY2+2.0*label_off,str);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE) first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
                case DA_BOTTOM:     /* bottom side */
                    if (!Debug)       /* draw leftmost tick */
                    {   lorg(4); ldir(0.0);  csize(ano_size);
                    }
                    if (tick_ano)     /* label leftmost tick */
                    {   left = ano_left;  right = ano_right;
                        SetLabel(&left,&right,axis_min,str);
                        min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                        if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                        if (tick_int > 0)
                            last_ano = tick_start - 2*min_ano_sep;
                        else
                            last_ano = tick_start + 2*min_ano_sep;
                    }
                    for (dtemp=tick_start; ; dtemp+=tick_int,tick_count++)
                    {   if (_ABS(dtemp) <= rounding_error) dtemp = 0.0;
                        if (tick_int > 0)
                        {   if (dtemp < axis_min) continue;
                            if (dtemp > axis_max) break;
                            if (dtemp < tick_abs_min) continue;
                            if (dtemp > tick_abs_max) break;
                        } else
                        {   if (dtemp > axis_min) continue;
                            if (dtemp < axis_max) break;
                            if (dtemp > tick_abs_min) continue;
                            if (dtemp < tick_abs_max) break;
                        }
                        if (!Debug)
                        {   if ((tick_count % tick_skip)==0)
                            {   plot(dtemp,VY1,-2);
                                plot(dtemp,VY1-tick_size,-1);
                            } else if (tick_mid_skip > 0 && (tick_count % tick_mid_skip)==0)
                            {   plot(dtemp,VY1,-2);
                                plot(dtemp,VY1-0.75*tick_size,-1);
                            } else
                            {   plot(dtemp,VY1,-2);
                                plot(dtemp,VY1-0.5*tick_size,-1);
                            }
                        } else
                            printf("BOTTOM tick: count=%d dtemp=%g\n",tick_count,dtemp);
                        if (tick_ano)
                        {   if (Debug) printf("     count=%d skip=%d mod=%d\n",tick_count,tick_skip,tick_count%tick_skip);
                            if (!(tick_count % tick_skip))
                            {   left = ano_left;  right = ano_right;
                                SetLabel(&left,&right,dtemp,str);
                                min_ano_sep = 1.5 * _ABS(_ABS(left)+right) * ano_width;
                                if (Debug) printf("min_ano_sep = %g\n",min_ano_sep);
                                if (_ABS(dtemp-last_ano) > min_ano_sep)
                                {   if (!Debug) label(dtemp,VY1-2.0*label_off,str);
                                    else        printf("     left=%d right=%d x=%g y=%g\n",left,right,dtemp,VY1-2.0*tick_size);
                                    last_ano = dtemp;
                                    if (first_ano == INVALID_VALUE)  first_ano = dtemp;
                                }
                            }
                        }
                    }
                    break;
            } /* end of switch block */
        } /* end of if (tick_num) */

        if (title[0] != 0)
        {   int left1,left2,right1,right2;

            csize(chr_size);
            switch (side)
            {   case DA_RIGHT:      /* right side */
                    if (first_ano != INVALID_VALUE)
                    {   left = ano_left;  right = ano_right;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,first_ano,str);
                        left1 = ano_left;  right1 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left1 < 0) left1 = -left1 + 1;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,last_ano,str);
                        left2 = ano_left; right2 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left2 < 0) left2 = -left2 + 1;
                        left = _MAXX(left1,left2);
                        right = _MAXX(right1,right2);
                    } else
                    {   left = right = 0;
                    }
                    if (!Debug)
                    {   lorg(4);  ldir(pi/2.0);
                        label( VX2+((double)left+right+1.5+title_offset)*ano_width,
                               (axis_max+axis_min)/2.0, title );
                    } else
                    {   printf("left1=%d left2=%d left=%d right1=%d right2=%d right=%d\n",
                            left1,left2,left,right1,right2,right);
                        printf("label at x=%g y=%g\n",
                            VX2+((double)left+right+1.5+title_offset)*ano_width,(axis_max+axis_min)/2.0);
                    }
                    break;
                case DA_LEFT:
                    if (first_ano != INVALID_VALUE)
                    {   left = ano_left;  right = ano_right;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,first_ano,str);
                        left1 = ano_left;  right1 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left1 < 0) left1 = -left1 + 1;
                        if (ano_left == 0 || ano_right == -1)
                            SetLabel(&ano_left,&ano_right,last_ano,str);
                        left2 = ano_left; right2 = ano_right;
                        ano_left = left; ano_right = right;
                        if (left2 < 0) left2 = -left2 + 1;
                        left = _MAXX(left1,left2);
                        right = _MAXX(right1,right2);
                    } else
                    {   left = right = 0;
                    }
                    if (!Debug)
                    {   lorg(6);  ldir(pi/2.0);
                        label( VX1-((double)left+right+2.5+title_offset)*ano_width,
                               (axis_max+axis_min)/2.0, title );
                    } else
                    {   printf("left1=%d left2=%d left=%d right1=%d right2=%d right=%d\n",
                            left1,left2,left,right1,right2,right);
                        printf("label at x=%g y=%g\n",
                            VX1-((double)left+right+2.5+title_offset)*ano_width,(axis_max+axis_min)/2.0);
                    }
                    break;
                case DA_TOP:
                    if (!Debug)
                    {   lorg(6);  ldir(0.0);
                        label( (axis_max+axis_min)/2.0, VY2+(3+title_offset)*ano_width, title );
                    }
                    break;
                case DA_BOTTOM:
                    if (!Debug)
                    {   lorg(4);  ldir(0.0);
                        label( (axis_max+axis_min)/2.0, VY1-(3+title_offset)*ano_width, title );
                    }
                    break;
            }   /* end of switch block */
        }    /* if (title[0]) */
    } else /* if (side) */
    {   if (!Debug)
        {   lorg(6);  ldir(0.0);  csize(chr_size);
            label((axis_max+axis_min)/2.0,VY2+(4+title_offset)*label_off,title); /* title label at top */
        }
    }

    return 0;
}
/******************************** SetLabel() ********************************/
/*
 * This function formats a floating point value into a string.
 *
 * Parameter list:
 *  int *left  - address of variable holding number of characters to left of
 *               of the decimal.  If == 0, then this function determines the
 *               count up to 6.  Otherwise, this function uses the value in
 *               "left" to format the string.
 *  int *right - address of variable holding number of characters to right of
 *               of the decimal.  If == -1, then this function determines the
 *               count up to 3.  Otherwise, this function uses the value in
 *               "right" to format the string.
 *  double val - the floating point value.
 *  char *str  - pointer to a buffer large enough to hold the string.
 *               15 would be sufficient.
 *
 * Requires: <math.h>, <stdio.h>, "jl_defs.h".
 * Calls: ceil, log10, modf, sprintf, _ABS, _MAXX, printf.
 * Returns: 0 on success, or
 *         >0 on error.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Team   Lakewood, CO
 * Date: March 7, 1994
 */
int SetLabel (int *left,int *right,double val,char *str)
{
    char fmt[12];
    int lft;
    double frac, ipart, dtemp;
    double delta = 1.0e-7;      /* correct for floatingpoint round off error */
    extern int Debug;

    if (Debug) printf("SetLabel(): left=%d right=%d val=%g\n",*left,*right,val);
    /* Get number of characters to the left of the decimal */
    if (*left == 0)
    {   if (val != 0.0)
        {   if (val > 0) *left = _MAXX(1,(int)ceil(log10(_ABS(val)+.001)));
            else         *left = _MAXX(1,(int)ceil(log10(_ABS(val)-.001)));
        }
        else            *left = 1;
        if (val < 0.0)  *left *= -1;
    }
    if (*left > 0 && val < 0) *left *= -1;
    if (*left > 6 || *left < -6) return 1; /* number too large */

    /* Get number of characters to the right of the decimal */
    /* NOTE: this part doesn't seem to always work correctly ! */
    if (*right < 0)
    {   *right = 0;
        dtemp = _ABS(val);
        frac = (modf(dtemp,&ipart));       /* get the fraction */
        dtemp = (frac * 10.0) + delta;
        frac = modf(dtemp,&ipart);         /* is there a digit in tenths? */
        if (ipart > 0.0)  (*right) = 1;
        dtemp = (frac * 10.0) + delta;
        frac = modf(dtemp,&ipart);         /* is there a digit in hundredths? */
        if (ipart > 0.0)  (*right) = 2;
        dtemp = (frac * 10.0) + delta;
        frac = modf(dtemp,&ipart);         /* is there a digit in thousandths? */
        if (ipart > 0.0)  (*right) = 3;
    } else
       if (*right > 3) (*right) = 3;
    lft = *left;
/********/
    if (*right == 0)
    {   if (val > 0) sprintf(str,"%d",(int)(val+0.000001));
        else         sprintf(str,"%d",(int)(val-0.000001));
    }
/********/
    else
    {   if (lft < 0) lft = -lft + 1;
        sprintf(fmt,"%c%d.%df",'%',lft + *right + 1, *right);
        sprintf(str,fmt,val);
        if (Debug)  printf("   fmt=%s  str=%s\n",fmt,str);
    }
    return 0;
}

