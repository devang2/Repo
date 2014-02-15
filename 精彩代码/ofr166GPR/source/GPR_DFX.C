/* GPR_DFX.C - source code for functions in GPR_DFX.LIB
 *
 * Jeff Lucius
 * U.S. Geological Survey
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@usgs.gov
 *
 * To compile for use in GPR_DFX.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c GPR_DFX.C
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
 *   AdjustMeanGridTraces
 *   ApplyGridSpatMedFilter
 *   ApplyGridTempMedFilter
 *   ChangeGridRangeGain
 *   EqualizeGridTraces
 *   FftFilterGridTraces
 *   InstAttribGridTraces
 *   RemGridGlobBckgrnd
 *   RemGridGlobForgrnd
 *   RemGridWindBckgrnd
 *   RemGridWindForgrnd
 *   RescaleGrid
 *   SlideSamples
 *   SmoothGridHorizontally
 *   SmoothGridVertically
 *   StackGrid
 */

/******************* Includes all required header files *********************/
#include "gpr_dfx.h"

/**************************** Global variables ******************************/
const char *ApplyGridSpatMedFilterMsg[] =
{   "ApplyGridSpatMedFilter(): No errors.",
	"ApplyGridSpatMedFilter() ERROR: Unable to allocate temporary storage.",
	"ApplyGridSpatMedFilter() ERROR: Input parameters are bad.",
} ;

const char *ApplyGridTempMedFilterMsg[] =
{   "ApplyGridTempMedFilter(): No errors.",
	"ApplyGridTempMedFilter() ERROR: Unable to allocate temporary storage.",
	"ApplyGridTempMedFilter() ERROR: Input parameters are bad.",
} ;

const char *ChangeGridRangeGainMsg[] =
{   "ChangeGridRangeGain(): No errors.",
	"ChangeGridRangeGain() ERROR: Invalid number of columns or rows.",
	"ChangeGridRangeGain() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
	"ChangeGridRangeGain() ERROR: NULL pointer passed to function.",
	"ChangeGridRangeGain() ERROR: Unable to allocate temporary storage.",

	"ChangeGridRangeGain() ERROR: Invalid data type for grid.",
} ;
const char *InstAttribGridTracesMsg[] =
{   "InstAttribGridTraces(): No errors.",
	"InstAttribGridTraces() ERROR: Number of samples greater then 65536.",
	"InstAttribGridTraces() ERROR: Unable to allocate temporary storage.",
} ;
const char *FftFilterGridTracesMsg[] =
{   "FftFilterGridTraces(): No errors.",
	"FftFilterGridTraces() ERROR: Number of samples greater than 65536.",
	"FftFilterGridTraces() ERROR: Unable to allocate temporary storage.",
} ;

const char *RemGridGlobBckgrndMsg[] =
{   "RemGridGlobBckgrnd(): No errors.",
	"RemGridGlobBckgrnd() ERROR: Invalid function arguments.",
	"RemGridGlobBckgrnd() ERROR: Unable to allocate temporary storage.",
} ;

const char *RemGridGlobForgrndMsg[] =
{   "RemGridGlobForgrnd(): No errors.",
	"RemGridGlobForgrnd() ERROR: Invalid function arguments.",
	"RemGridGlobForgrnd() ERROR: Unable to allocate temporary storage.",
} ;

const char *RemGridWindBckgrndMsg[] =
{   "RemGridWindBckgrnd(): No errors.",
	"RemGridWindBckgrnd() ERROR: Invalid function arguments.",
	"RemGridWindBckgrnd() ERROR: Unable to allocate temporary storage.",
} ;

const char *RemGridWindForgrndMsg[] =
{   "RemGridWindForgrnd(): No errors.",
	"RemGridWindForgrnd() ERROR: Invalid function arguments.",
	"RemGridWindForgrnd() ERROR: Unable to allocate temporary storage.",
} ;

const char *SmoothGridHorizontallyMsg[] =
{   "SmoothGridHorizontally(): No errors.",
	"SmoothGridHorizontally() ERROR: Invalid function arguments.",
	"SmoothGridHorizontally() ERROR: Unable to allocate temporary storage.",
} ;

const char *SmoothGridVerticallyMsg[] =
{   "SmoothGridVertically(): No errors.",
	"SmoothGridVertically() ERROR: Invalid function arguments.",
	"SmoothGridVertically() ERROR: Unable to allocate temporary storage.",
} ;

const char *StackGridMsg[] =
{   "StackGrid(): No errors.",
	"StackGrid() ERROR: Invalid function arguments.",
} ;

/************************** AdjustMeanGridTraces() **************************/
/* Adjust each trace in the grid to a new value.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  double new_mean - value to adjust trace mean to
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: To speed up function, there is NO error checking. But tests are made
 *       to avoid wrap around and out if range errors.
 *
 * Requires: "assertjl.h", "gpr_dfx.h".
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
void AdjustMeanGridTraces (long num_cols,long num_rows,long first_samp,
						   int header_samps,int datatype,double new_mean,
                           void **grid)
{
	long trace,samp;
	double dmean,dtemp,num_pts;

	assert(grid && first_samp>=0 && header_samps>=0);

	num_pts = num_rows - first_samp + 1;
	switch (datatype)
	{   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   dmean = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += uc_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)uc_pp[trace][samp] + dmean;
                    if (dtemp <   0.0)      uc_pp[trace][samp] =   0;
                    else if (dtemp > 255.0) uc_pp[trace][samp] = 255;
                    else                    uc_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 1:
        {   char **c_pp = (char **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += c_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)c_pp[trace][samp] + dmean;
                    if (dtemp < -128.0)     c_pp[trace][samp] = -128;
                    else if (dtemp > 127.0) c_pp[trace][samp] =  127;
					else                    c_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case -2:
        {   unsigned short **us_pp = (unsigned short **)grid;

            for (trace=0; trace<num_cols; trace++)
			{   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += us_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] + dmean;
					if (dtemp < 0.0)          us_pp[trace][samp] = 0;
                    else if (dtemp > 65535.0) us_pp[trace][samp] = 65535L;
                    else                      us_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					dmean += us_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] + dmean;
					if (dtemp < 0.0)         us_pp[trace][samp] = 0;
					else if (dtemp > 4095.0) us_pp[trace][samp] = 4095;
					else                     us_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += s_pp[trace][samp];
                dmean /= num_pts;
				dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)s_pp[trace][samp] + dmean;
                    if (dtemp < -32768.0)     s_pp[trace][samp] = -32768L;
                    else if (dtemp > 32767.0) s_pp[trace][samp] =  32767;
                    else                      s_pp[trace][samp] = dtemp;
                }
			}
            break;
        }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                  for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += ul_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)ul_pp[trace][samp] + dmean;
                    if (dtemp < 0.0)               ul_pp[trace][samp] = 0;
                    else if (dtemp > 4294967295.0) ul_pp[trace][samp] = 4294967295L;
                    else                           ul_pp[trace][samp] = dtemp;
                }
            }
            break;
		}
        case 3:
        {   long **l_pp = (long **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += l_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)l_pp[trace][samp] + dmean;
                    if (dtemp < -2147483648.0)     l_pp[trace][samp] = -2147483648L;
					else if (dtemp > 2147483647.0) l_pp[trace][samp] =  2147483647L;
                    else                           l_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 4:
		{   float **f_pp = (float **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += f_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)f_pp[trace][samp] + dmean;
                    if (dtemp < -FLT_MAX)     f_pp[trace][samp] = -FLT_MAX;
                    else if (dtemp > FLT_MAX) f_pp[trace][samp] =  FLT_MAX;
                    else                      f_pp[trace][samp] = dtemp;
				}
            }
            break;
        }
        case 8:
        {   double **d_pp = (double **)grid;

			for (trace=0; trace<num_cols; trace++)
            {   dmean = 0.0;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    dmean += d_pp[trace][samp];
                dmean /= num_pts;
                dmean = new_mean - dmean;
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] += dmean;
            }
            break;
        }
    }
}
/************************* ApplyGridSpatMedFilter() *************************/
/* Apply a spatial ("horizontal") median filter to the grid.
 *
 * Parameters:
 *  long num_cols  - number of columns in grid
 *  long num_rows  - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long win_len - number of points in the filter (odd number)
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * Requires: <stdlib>, "gpr_dfx.h".
 * Calls: qsort, maloc, free, ucCompare, cCompare, usCompare,
 *        sCompare, ulCompare, lCompare, fCompare, dCompare.
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *ApplyGridSpatMedFilterMsg[] =
{   "ApplyGridSpatMedFilter(): No errors.",
	"ApplyGridSpatMedFilter() ERROR: Unable to allocate temporary storage.",
	"ApplyGridSpatMedFilter() ERROR: Input parameters are bad.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int ApplyGridSpatMedFilter (long num_cols,long num_rows,long first_samp,
						int header_samps,int datatype,long win_len,void **grid)
{
	long trace,samp,side_len,i;
	size_t num_vals;

	/* Reality check */
	if (win_len < 2 || header_samps < 0 || first_samp < 0 || grid == NULL ||
		num_cols < win_len || num_rows == 0)
		return 2;
	if ((win_len % 2) == 0) win_len++; /* make odd if even */

	/* Filter the data */
	side_len = win_len / 2;   /* integer truncation expected */
	switch (datatype)
	{   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;
			unsigned char *uc_p1,*uc_p2;

            uc_p1 = (unsigned char *)malloc(win_len * sizeof(char));
            uc_p2 = (unsigned char *)malloc(num_cols * sizeof(char));
			if ((uc_p1 == NULL) || (uc_p1 == NULL))
            {   free(uc_p1);  free(uc_p2);
				return 1;
			}

			/* work through grid row by row */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   /* copy all values in a row to a temporary array */
				for (trace=0; trace<num_cols; trace++)
                    uc_p2[trace] = uc_pp[trace][samp];
                /* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
					/* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
                        uc_p1[num_vals]  = uc_p2[i];
						num_vals++;
                    }
                    /* sort the array */
					if (num_vals == 0) continue;
                    qsort(uc_p1,num_vals,sizeof(char),ucCompare);
                    /* assign "middle" value to grid */
                    uc_pp[trace][samp] = uc_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case  1:
        {   char **c_pp = (char **)grid;
            char *c_p1,*c_p2;

            c_p1 = (char *)malloc(win_len * sizeof(char));
			c_p2 = (char *)malloc(num_cols * sizeof(char));
            if ((c_p1 == NULL) || (c_p1 == NULL))
            {   free(c_p1);  free(c_p2);
				return 1;
            }

            /* work through grid row by row */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
                    c_p2[trace] = c_pp[trace][samp];
                /* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
                        c_p1[num_vals]  = c_p2[i];
                        num_vals++;
                    }
					/* sort the array */
                    if (num_vals == 0) continue;
                    qsort(c_p1,num_vals,sizeof(char),cCompare);
                    /* assign "middle" value to grid */
                    c_pp[trace][samp] = c_p1[num_vals/2]; /* integer truncation expected */
                }
            }
			break;
        }
        case -2:  case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;
            unsigned short *us_p1,*us_p2;

            us_p1 = (unsigned short *)malloc(win_len * sizeof(short));
            us_p2 = (unsigned short *)malloc(num_cols * sizeof(short));
            if ((us_p1 == NULL) || (us_p1 == NULL))
            {   free(us_p1);  free(us_p2);
				return 1;
            }

			/* work through grid row by row */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
                    us_p2[trace] = us_pp[trace][samp];
                /* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
				{   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
                        us_p1[num_vals]  = us_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(us_p1,num_vals,sizeof(short),usCompare);
                    /* assign "middle" value to grid */
                    us_pp[trace][samp] = us_p1[num_vals/2]; /* integer truncation expected */
                }
			}
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;
            short *s_p1,*s_p2;

			s_p1 = (short *)malloc(win_len * sizeof(short));
            s_p2 = (short *)malloc(num_cols * sizeof(short));
            if ((s_p1 == NULL) || (s_p1 == NULL))
            {   free(s_p1);  free(s_p2);
				return 1;
            }

            /* work through grid row by row */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
                    s_p2[trace] = s_pp[trace][samp];
                /* apply filter to each element in the row */
				for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
                        s_p1[num_vals]  = s_p2[i];
                        num_vals++;
					}
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(s_p1,num_vals,sizeof(short),sCompare);
                    /* assign "middle" value to grid */
                    s_pp[trace][samp] = s_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;
            unsigned long *ul_p1,*ul_p2;

            ul_p1 = (unsigned long *)malloc(win_len * sizeof(long));
            ul_p2 = (unsigned long *)malloc(num_cols * sizeof(long));
            if ((ul_p1 == NULL) || (ul_p1 == NULL))
            {   free(ul_p1);  free(ul_p2);
				return 1;
            }

            /* work through grid row by row */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
                    ul_p2[trace] = ul_pp[trace][samp];
                /* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
                        ul_p1[num_vals]  = ul_p2[i];
						num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(ul_p1,num_vals,sizeof(long),ulCompare);
                    /* assign "middle" value to grid */
                    ul_pp[trace][samp] = ul_p1[num_vals/2]; /* integer truncation expected */
				}
            }
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;
            long *l_p1,*l_p2;

            l_p1 = (long *)malloc(win_len * sizeof(long));
            l_p2 = (long *)malloc(num_cols * sizeof(long));
            if ((l_p1 == NULL) || (l_p1 == NULL))
            {   free(l_p1);  free(l_p2);
				return 1;
			}

            /* work through grid row by row */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
                    l_p2[trace] = l_pp[trace][samp];
				/* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
                        l_p1[num_vals]  = l_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(l_p1,num_vals,sizeof(long),lCompare);
                    /* assign "middle" value to grid */
					l_pp[trace][samp] = l_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;
			float *f_p1,*f_p2;

            f_p1 = (float *)malloc(win_len * sizeof(float));
            f_p2 = (float *)malloc(num_cols * sizeof(float));
            if ((f_p1 == NULL) || (f_p1 == NULL))
            {   free(f_p1);  free(f_p2);
				return 1;
            }

            /* work through grid row by row */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
					f_p2[trace] = f_pp[trace][samp];
                /* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
                    {   if ((i < 0) || (i >= num_cols)) continue;
						f_p1[num_vals]  = f_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(f_p1,num_vals,sizeof(float),fCompare);
                    /* assign "middle" value to grid */
                    f_pp[trace][samp] = f_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case 8:
		{   double **d_pp = (double **)grid;
            double *d_p1,*d_p2;

            d_p1 = (double *)malloc(win_len * sizeof(double));
            d_p2 = (double *)malloc(num_cols * sizeof(double));
            if ((d_p1 == NULL) || (d_p1 == NULL))
            {   free(d_p1);  free(d_p2);
				return 1;
            }

            /* work through grid row by row */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   /* copy all values in a row to a temporary array */
                for (trace=0; trace<num_cols; trace++)
                    d_p2[trace] = d_pp[trace][samp];
                /* apply filter to each element in the row */
                for (trace=0; trace<num_cols; trace++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the row into an array */
                    for (i=trace-side_len; i<=trace+side_len; i++)
					{   if ((i < 0) || (i >= num_cols)) continue;
                        d_p1[num_vals]  = d_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(d_p1,num_vals,sizeof(double),dCompare);
					/* assign "middle" value to grid */
                    d_pp[trace][samp] = d_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
    }
    return 0;
}
/************************* ApplyGridTempMedFilter() *************************/
/* Apply a temporal ("vertical") median filter to the grid.
 *
 * Parameters:
 *  long num_cols  - number of columns in grid
 *  long num_rows  - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long win_len - number of points in the filter (odd number)
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * Requires: <stdlib.h>, "gpr_dfx.h".
 * Calls: qsort, malloc, free, ucCompare, cCompare, usCompare,
 *        sCompare, ulCompare, lCompare, fCompare, dCompare.
 * Returns: 0 on success,
 *         >0 if error.
 *
const char *ApplyGridTempMedFilterMsg[] =
{   "ApplyGridTempMedFilter(): No errors.",
	"ApplyGridTempMedFilter() ERROR: Unable to allocate temporary storage.",
	"ApplyGridTempMedFilter() ERROR: Input parameters are bad.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int ApplyGridTempMedFilter (long num_cols,long num_rows,long first_samp,
						int header_samps,
                        int datatype,long win_len,void **grid)
{
    long trace,samp,side_len,i;
    size_t num_vals;

    /* Reality check */
    if (win_len < 2 || first_samp < 0 || grid == NULL ||
        num_cols < win_len || num_rows == 0)
		return 2;
	if ((win_len % 2) == 0) win_len++; /* make odd if even */

	/* Filter the data */
    side_len = win_len / 2;   /* integer truncation expected */
    switch (datatype)
    {   case -1:
        {   unsigned char **uc_pp = (unsigned char **)grid;
            unsigned char *uc_p1,*uc_p2;

            uc_p1 = (unsigned char *)malloc(win_len * sizeof(char));
            uc_p2 = (unsigned char *)malloc(num_rows * sizeof(char));
            if ((uc_p1 == NULL) || (uc_p1 == NULL))
            {   free(uc_p1);  free(uc_p2);
				return 1;
			}

			/* work through grid column by column */
			for (trace=0; trace<num_cols; trace++)
			{   /* copy all values in a column to a temporary array */
				for (samp=0; samp<num_rows; samp++)
					uc_p2[samp] = uc_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        uc_p1[num_vals]  = uc_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(uc_p1,num_vals,sizeof(char),ucCompare);
                    /* assign "middle" value to grid */
                    uc_pp[trace][samp] = uc_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case  1:
		{   char **c_pp = (char **)grid;
            char *c_p1,*c_p2;

            c_p1 = (char *)malloc(win_len * sizeof(char));
            c_p2 = (char *)malloc(num_rows * sizeof(char));
            if ((c_p1 == NULL) || (c_p1 == NULL))
            {   free(c_p1);  free(c_p2);
				return 1;
            }

            /* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    c_p2[samp] = c_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
					{   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        c_p1[num_vals]  = c_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(c_p1,num_vals,sizeof(char),cCompare);
                    /* assign "middle" value to grid */
                    c_pp[trace][samp] = c_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case -2:  case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;
            unsigned short *us_p1,*us_p2;

            us_p1 = (unsigned short *)malloc(win_len * sizeof(short));
            us_p2 = (unsigned short *)malloc(num_rows * sizeof(short));
            if ((us_p1 == NULL) || (us_p1 == NULL))
			{   free(us_p1);  free(us_p2);
				return 1;
            }

            /* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    us_p2[samp] = us_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        us_p1[num_vals]  = us_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
					qsort(us_p1,num_vals,sizeof(short),usCompare);
                    /* assign "middle" value to grid */
                    us_pp[trace][samp] = us_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;
            short *s_p1,*s_p2;

            s_p1 = (short *)malloc(win_len * sizeof(short));
            s_p2 = (short *)malloc(num_rows * sizeof(short));
            if ((s_p1 == NULL) || (s_p1 == NULL))
            {   free(s_p1);  free(s_p2);
				return 1;
            }

            /* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
			{   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    s_p2[samp] = s_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        s_p1[num_vals]  = s_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(s_p1,num_vals,sizeof(short),sCompare);
                    /* assign "middle" value to grid */
                    s_pp[trace][samp] = s_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
		}
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;
            unsigned long *ul_p1,*ul_p2;

            ul_p1 = (unsigned long *)malloc(win_len * sizeof(long));
            ul_p2 = (unsigned long *)malloc(num_rows * sizeof(long));
            if ((ul_p1 == NULL) || (ul_p1 == NULL))
            {   free(ul_p1);  free(ul_p2);
				return 1;
            }

            /* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    ul_p2[samp] = ul_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
					/* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        ul_p1[num_vals]  = ul_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(ul_p1,num_vals,sizeof(long),ulCompare);
                    /* assign "middle" value to grid */
                    ul_pp[trace][samp] = ul_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;
            long *l_p1,*l_p2;

            l_p1 = (long *)malloc(win_len * sizeof(long));
			l_p2 = (long *)malloc(num_rows * sizeof(long));
            if ((l_p1 == NULL) || (l_p1 == NULL))
            {   free(l_p1);  free(l_p2);
				return 1;
            }

            /* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    l_p2[samp] = l_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        l_p1[num_vals]  = l_p2[i];
                        num_vals++;
                    }
					/* sort the array */
                    if (num_vals == 0) continue;
                    qsort(l_p1,num_vals,sizeof(long),lCompare);
                    /* assign "middle" value to grid */
                    l_pp[trace][samp] = l_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;
            float *f_p1,*f_p2;

            f_p1 = (float *)malloc(win_len * sizeof(float));
            f_p2 = (float *)malloc(num_rows * sizeof(float));
            if ((f_p1 == NULL) || (f_p1 == NULL))
            {   free(f_p1);  free(f_p2);
				return 1;
            }

			/* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    f_p2[samp] = f_pp[trace][samp];
                /* apply filter to each element in the column */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        f_p1[num_vals]  = f_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(f_p1,num_vals,sizeof(float),fCompare);
                    /* assign "middle" value to grid */
                    f_pp[trace][samp] = f_p1[num_vals/2]; /* integer truncation expected */
                }
			}
            break;
        }
        case 8:
        {   double **d_pp = (double **)grid;
            double *d_p1,*d_p2;

            d_p1 = (double *)malloc(win_len * sizeof(double));
            d_p2 = (double *)malloc(num_rows * sizeof(double));
            if ((d_p1 == NULL) || (d_p1 == NULL))
            {   free(d_p1);  free(d_p2);
				return 1;
            }

            /* work through grid column by column */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy all values in a column to a temporary array */
                for (samp=0; samp<num_rows; samp++)
                    d_p2[samp] = d_pp[trace][samp];
                /* apply filter to each element in the column */
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   num_vals = 0;
                    /* try to copy win_len elements from the column into an array */
                    for (i=samp-side_len; i<=samp+side_len; i++)
                    {   if ((i < header_samps+first_samp) || (i >= num_rows)) continue;
                        d_p1[num_vals]  = d_p2[i];
                        num_vals++;
                    }
                    /* sort the array */
                    if (num_vals == 0) continue;
                    qsort(d_p1,num_vals,sizeof(double),dCompare);
					/* assign "middle" value to grid */
                    d_pp[trace][samp] = d_p1[num_vals/2]; /* integer truncation expected */
                }
            }
            break;
        }
    }
    return 0;
}
/*************************** ChangeGridRangeGain() **************************/
/* This function changes the gain applied to radar traces.
 *
 * Parameters:
 *  long num_cols     - number of columns in grid[][]
 *  long num_rows     - number of rows in grid[][]
 *  int  first_samp   - first sample of trace to work with
 *	int header_samps - number of "samples" in trace header
 *  int  num_gain_off - size of gain_off[]; 0 or >=2
 *  double *gain_off  - set of gain to be removed in decibels
 *  int  num_gain_on  - size of gain_on[]; 0 or >=2
 *  double *gain_on   - set of gain to be added in decibels
 *  int datatype      - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  void **grid       - [num_cols][num_rows]; pointer to 2D image
 *
 * Requires: <math.h>, <stdio.h>, <stdlib.h>, "gpr_disp.h".
 * Calls: malloc, free, pow.
 * Returns: 0 on success, or
 *         >0 if error (offset into message strings array).
 *
const char *ChangeGridRangeGainMsg[] =
{   "ChangeGridRangeGain(): No errors.",
	"ChangeGridRangeGain() ERROR: Invalid number of columns or rows.",
	"ChangeGridRangeGain() ERROR: num_gain_off and num_gain_on must be 0 or >=2.",
	"ChangeGridRangeGain() ERROR: NULL pointer passed to function.",
	"ChangeGridRangeGain() ERROR: Unable to allocate temporary storage.",

	"ChangeGridRangeGain() ERROR: Invalid data type for grid.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int ChangeGridRangeGain (long num_cols,long num_rows,int first_samp,
					 int header_samps,int num_gain_off,double *gain_off,
                     int num_gain_on,double *gain_on,int datatype,void **grid)
{
    int i,gain1,gain2,first_pt,last_pt,col,row,num_samps,gain_index;
    double *gain_off_func,*gain_on_func,dtemp,gain_off_inc,gain_on_inc;

    /* Check function parameters */
    if (num_cols < 1 || num_rows < 2) return 1;
    if (num_gain_off < 0 || num_gain_on < 0) return 2;
	if (num_gain_off == 1 || num_gain_on == 1) return 2;
    if (grid == NULL) return 3;
    switch (datatype)
    {   case -1: case 1: case -2: case 2: case -3: case 3:
        case -5: case -6: case 4: case  8:
            break;
        default:
            return 5;
			/* break;   ** unreachable code */
    }

    /* Allocate storage for gain functions */
    num_samps = num_rows - header_samps;
    gain_off_func = (double *)malloc(num_samps * sizeof(double));
    if (gain_off_func == NULL) return 4;
    gain_on_func = (double *)malloc(num_samps * sizeof(double));
    if (gain_on_func == NULL)
	{   free(gain_off_func);
        return 4;
    }

    /* Initialize gain functions to 0.0 */
    for (i=0; i<num_samps; i++)  gain_off_func[i] = gain_on_func[i] = 0.0;

	/* Calculate number of samples (rows) between break points */
    gain_off_inc = gain_on_inc = 0.0;
    if (num_gain_off >= 2)
    {   if (gain_off == NULL)
        {   free(gain_off_func);  free(gain_on_func);
			return 3;
        }
        gain_off_inc = (double)(num_samps-1) / (num_gain_off - 1);
    }
    if (num_gain_on >= 2)
    {   if (gain_on == NULL)
        {   free(gain_off_func);  free(gain_on_func);
            return 3;
		}
        gain_on_inc  = (double)(num_samps-1) / (num_gain_on - 1);
    }

    /* Calculate gain functions (linearly iterpolate between break points) */
    if (gain_off_inc > 0.0)
    {   for (gain1=0; gain1<num_gain_off-1; gain1++)
        {   gain2    = gain1 + 1;
			first_pt = gain1 * gain_off_inc;  /* truncation expected */
            last_pt  = gain2 * gain_off_inc;  /* truncation expected */
            dtemp    = gain_off[gain2] - gain_off[gain1];
            for (i=first_pt; i<last_pt && i<num_samps; i++)
            {   gain_off_func[i] = gain_off[gain1] + (dtemp*(i-first_pt) / gain_off_inc);
            }
        }
        gain_off_func[0]           = gain_off[0];
		gain_off_func[num_samps-1] = gain_off[num_gain_off-1];
    }

    if (gain_on_inc > 0.0)
    {   for (gain1=0; gain1<num_gain_on-1; gain1++)
        {   gain2    = gain1 + 1;
            first_pt = gain1 * gain_on_inc;   /* truncation expected */
            last_pt  = gain2 * gain_on_inc;   /* truncation expected */
            dtemp    = gain_on[gain2] - gain_on[gain1];
            for (i=first_pt; i<last_pt && i<num_samps; i++)
            {   gain_on_func[i] = gain_on[gain1] + (dtemp*(i-first_pt) / gain_on_inc);
            }
		}
        gain_on_func[0]           = gain_on[0];
        gain_on_func[num_samps-1] = gain_on[num_gain_on-1];
    }

    /* Change gain */
    switch (datatype)
    {   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = uc_pp[col][row];
                    dtemp -= 128.0;
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp += 128.0;
                    dtemp  = _LIMIT(0.0,dtemp,255.0);
                    uc_pp[col][row] = dtemp;
				}
            }
            break;
        }
        case  1:
        {   char **c_pp = (char **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = c_pp[col][row];
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp  = _LIMIT(-128.0,dtemp,127.0);
                    c_pp[col][row] = dtemp;
                }
            }
            break;
        }
        case -2:
		{   unsigned short **us_pp = (unsigned short **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = us_pp[col][row];
                    dtemp -= 32768.0;
					dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
					dtemp += 32768.0;
                    dtemp  = _LIMIT(0.0,dtemp,65535.0);
                    us_pp[col][row] = dtemp;
                }
            }
            break;
        }
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = us_pp[col][row];
                    dtemp -= 2048.0;
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp += 2048.0;
                    dtemp  = _LIMIT(0.0,dtemp,4095.0);
					us_pp[col][row] = dtemp;
                }
            }
            break;
        }
        case  2:
        {   short **s_pp = (short **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
				{   dtemp  = s_pp[col][row];
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp  = _LIMIT(-32768.0,dtemp,32767.0);
                    s_pp[col][row] = dtemp;
                }
            }
            break;
        }
		case -3:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = ul_pp[col][row];
                    dtemp -= 2147483648.0;
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp += 2147483648.0;
                    dtemp  = _LIMIT(0.0,dtemp,4294967295.0);
					ul_pp[col][row] = dtemp;
                }
            }
            break;
        }
        case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

			for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = ul_pp[col][row];
                    dtemp -= 8388608.0;
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp += 8388608.0;
                    dtemp  = _LIMIT(0.0,dtemp,16777215.0);
                    ul_pp[col][row] = dtemp;
                }
            }
			break;
        }
        case  3:
        {   long **l_pp = (long **)grid;

            for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
                {   dtemp  = l_pp[col][row];
                    dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
                    dtemp  = _LIMIT(-2147483648.0,dtemp,2147483647.0);
                    l_pp[col][row] = dtemp;
                }
            }
            break;
        }
        case  4:
        {   float **f_pp = (float **)grid;

			for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
				{   dtemp  = f_pp[col][row];
					dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
					dtemp  = _LIMIT(-FLT_MAX,dtemp,FLT_MAX);
					f_pp[col][row] = dtemp;
				}
			}
			break;
		}
		case  8:
		{   double **d_pp = (double **)grid;

			for (col=0; col<num_cols; col++)
            {   for (row=header_samps+first_samp, gain_index=first_samp;
                     row<num_rows,                gain_index<num_samps;
                     row++,                       gain_index++)
				{   dtemp  = d_pp[col][row];
					dtemp *= pow(10.,(.05*(gain_on_func[gain_index]-gain_off_func[gain_index])));
					d_pp[col][row] = dtemp;
				}
			}
			break;
		 }
	}
	free(gain_on_func);
	free(gain_off_func);
	return 0;
}
/*************************** EqualizeGridTraces() ***************************/
/* Multiply each sample in each trace in the grid by a constant value so
 * that the sum of all sample amplitudes (about 0) is the same as the sum
 * for the base_trace.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype    - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long base_trace - use sum of amplitudes from this trace to normalize others
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: To speed up function, there is NO error checking. But tests are made
 *       to avoid wrap around and out if range errors.
 *
 * Requires: <float.h>, "assertjl.h", "jl_defs.h", "gpr_dfx.h".
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: January 2, 1997
 * Revisions:  September 4, 1997
 */
void EqualizeGridTraces(long num_cols,long num_rows,long first_samp,
				int header_samps, int datatype,long base_trace,void **grid)
{
	long trace,samp;
	double dtemp,base_total,amp_total,factor;

	assert(grid && first_samp>=0 && (base_trace>=0 && base_trace<num_rows) && header_samps>=0);

	switch (datatype)
	{   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)uc_pp[base_trace][samp] - 128.0;
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)uc_pp[trace][samp] - 128.0;
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)uc_pp[trace][samp] - 128.0;
					dtemp *= factor;
					if (dtemp < -128.0)  dtemp = -128;
					if (dtemp >  127.0)  dtemp =  127;
					dtemp += 128.0;
					uc_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case 1:
		{   char **c_pp = (char **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)c_pp[base_trace][samp];
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)c_pp[trace][samp];
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)c_pp[trace][samp];
					dtemp *= factor;
					if (dtemp < -128.0)  dtemp = -128;
					if (dtemp >  127.0)  dtemp =  127;
					c_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case -2:
		{   unsigned short **us_pp = (unsigned short **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)us_pp[base_trace][samp] - 32768.0;
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)us_pp[trace][samp] - 32768.0;
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)us_pp[trace][samp] - 32768.0;
					dtemp *= factor;
					if (dtemp < -32768.0)  dtemp = -32768.0;
					if (dtemp >  32767.0)  dtemp =  32767.0;
					dtemp += 32768.0;
					us_pp[trace][samp] = dtemp;
				}
			}
			break;
        }
        case -5:
		{   unsigned short **us_pp = (unsigned short **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)us_pp[base_trace][samp] - 2048.0;
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)us_pp[trace][samp] - 2048.0;
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)us_pp[trace][samp] - 2048.0;
					dtemp *= factor;
					if (dtemp < -2048.0)  dtemp = -2048;
					if (dtemp >  2047.0)  dtemp =  2047;
					dtemp += 2048.0;
					us_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case 2:
		{   short **s_pp = (short **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)s_pp[base_trace][samp];
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)s_pp[trace][samp];
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)s_pp[trace][samp];
					dtemp *= factor;
					if (dtemp < -32768.0)  dtemp = -32768.0;
					if (dtemp >  32767.0)  dtemp =  32767.0;
					s_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case -3:
		{   unsigned long **ul_pp = (unsigned long **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)ul_pp[base_trace][samp] - 2147483647.0;
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)ul_pp[trace][samp] - 2147483647.0;
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)ul_pp[trace][samp] - 2147483647.0;
					dtemp *= factor;
					if (dtemp < -2147483647.0)  dtemp = -2147483647.0;
					if (dtemp >  2147483646.0)  dtemp =  2147483646.0;
					dtemp += 2147483647.0;
					ul_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case -6:
		{   unsigned long **ul_pp = (unsigned long **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)ul_pp[base_trace][samp] - 8388608.0;
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)ul_pp[trace][samp] - 8388608.0;
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)ul_pp[trace][samp] - 8388608.0;
					dtemp *= factor;
					if (dtemp < -8388608.0)  dtemp = -8388608.0;
					if (dtemp >  8388607.0)  dtemp =  8388607.0;
					dtemp += 8388608.0;
					ul_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
        case 3:
        {   long **l_pp = (long **)grid;

			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)l_pp[base_trace][samp];
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)l_pp[trace][samp];
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)l_pp[trace][samp];
					dtemp *= factor;
					if (dtemp < -2147483648.0)  dtemp = -2147483648.0;
					if (dtemp >  2147483647.0)  dtemp =  2147483647.0;
					l_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case 4:
		{   float **f_pp = (float **)grid;

			/* Warning: Assumes centered about 0 */
			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   dtemp = (double)f_pp[base_trace][samp];
				base_total += _ABS(dtemp);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)f_pp[trace][samp];
					amp_total += _ABS(dtemp);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)f_pp[trace][samp];
					dtemp *= factor;
					if (dtemp < -FLT_MAX)  dtemp = -FLT_MAX;
					if (dtemp >  FLT_MAX)  dtemp =  FLT_MAX;
					f_pp[trace][samp] = dtemp;
				}
			}
			break;
		}
		case 8:
		{   double **d_pp = (double **)grid;

			/* Warning: Assumes centered about 0 */
			base_total = 0.0;
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   base_total += _ABS(d_pp[base_trace][samp]);
			}
			for (trace=0; trace<num_cols; trace++)
			{   amp_total = 0.0;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   amp_total += _ABS(d_pp[trace][samp]);
				}
				factor = base_total/amp_total;
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   d_pp[trace][samp] *= factor;
				}
			}
			break;
		}
	}
}
/*************************** InstAttribGridTraces() ***************************/
/* Transform the trace amplitudes into instantaneous amplitudes or
 * instantaneous power. "Preproccesing" occurs automatically.
 *
 * NOTE: InstAttribTrace() will automatically remove the average value from
 *       each trace so adjustment of unsigned data is unnecessary.
 *
 * NOTE: data
 *
 * Parameters:
 *  long num_cols     - number of columns in grid
 *  long num_rows     - number of rows in grid
 *  long first_samp   - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  int attrib        == 0, invalid value
 *                    == 1, for instantaneous amplitude
 *                    == 2, for instantaneous power
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: Tests are made to avoid wrap around and out-of-range errors.
 *
 * WARNING: This function may not be appropriate for grids that have traces
 *          which contain trace headers!
 * NOTE: Modifications August 29, 1997 should fix above warning!
 *
 * Requires: <math.h>, <stdio.h>, "assertjl.h", "gpr_dfx.h", "gpr_fft.h".
 * Calls: assert, malloc, free, pow, FftFilterTrace.
 * Returns:  0 on sucess or if no filtering required.
 *          >0 on error.
const char *InstAttribGridTracesMsg[] =
{   "InstAttribGridTraces(): No errors.",
	"InstAttribGridTraces() ERROR: Number of samples greater then 65536.",
	"InstAttribGridTraces() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: August 18, 1999
 * Revisions: August 25, 1999;
 */
int InstAttribGridTraces (long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,int attrib,void **grid)
{
    int     preproc;
	long    i,trace,samp,dsamp;       /* scratch variables and counters */
	long	num_samps;                /* number of actual trace samples */
	long    num_fillers;              /* numer of fill samples if num_samps not power of 2 */
    long    trace_length;             /* number of "samples" used for FFT (power of 2) */
	double *dscan     = NULL;         /* temp. array for FFT */
	double *cscan     = NULL;         /* temp. "complex" array */

	assert(grid && first_samp>=0 && header_samps>=0 && num_rows>0 && num_cols>0);

    if (attrib == 0) return 0;
    if (attrib != 1) attrib = 2;

	/* Find number of trace samples */
    num_samps = num_rows - header_samps;
    if (num_samps > 65536L) return 1;

	/* Check if trace length a power of 2, <= 65536 */
    num_fillers = 0;
    trace_length = num_samps;
    {   for (i=1; i<16; i++)
		{   if (num_samps > pow(2,(double)i) &&
				num_samps < pow(2,(double)(i+1)))
			{   trace_length = pow(2,(double)(i+1));
                num_fillers = trace_length - num_samps;
				break;
			}
		}
    }

	/* Allocate work arrays */
	dscan = (double *)malloc(trace_length * sizeof(double));
	cscan = (double *)malloc(2* trace_length * sizeof(double));
	if (dscan == NULL || cscan == NULL) return 2;

	/* Copy trace to work array, filter, and copy back */
    preproc = TRUE;
	switch (datatype)
	{   case UCHAR:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((unsigned char **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] *= sqrt(2.0);
                    else             dscan[dsamp] /= 127.0;
                    ((unsigned char **)(grid))[trace][samp] = _LIMIT(0.0,dscan[dsamp],255.0);
                }
            }
			break;
        case CHAR:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((char **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] /= sqrt(2.0);
                    else             dscan[dsamp] /= 255.0;
                    ((char **)(grid))[trace][samp] = _LIMIT(-128.0,dscan[dsamp],127.0);
                }
            }
			break;
		case USHORT:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((unsigned short **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] *= sqrt(2.0);
                    else             dscan[dsamp] /= 32767.0;
                    ((unsigned short **)(grid))[trace][samp] = _LIMIT(0.0,dscan[dsamp],65535.0);
                }
            }
			break;
		case USHORT12:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((unsigned short **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] *= sqrt(2.0);
                    else             dscan[dsamp] /= 32767.0;
                    ((unsigned short **)(grid))[trace][samp] = _LIMIT(0.0,dscan[dsamp],4095.0);
                }
            }
            break;
		case SHORT:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((short **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] /= sqrt(2.0);
                    else             dscan[dsamp] /= 65535.0;
                    ((short **)(grid))[trace][samp] = _LIMIT(-32768.0,dscan[dsamp],32767.0);
                }
            }
            break;
        case ULONG:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((unsigned long **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] *= sqrt(2.0);
                    else             dscan[dsamp] /= 2147483647.0;
                    ((unsigned long **)(grid))[trace][samp] = _LIMIT(0.0,dscan[dsamp],4294967295.0);
                }
            }
            break;
        case ULONG24:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((unsigned long **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] *= sqrt(2.0);
                    else             dscan[dsamp] /= 2147483647.0;
                    ((unsigned long **)(grid))[trace][samp] = _LIMIT(0.0,dscan[dsamp],16777215.0);
                }
            }
            break;
        case LONG:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
				    dscan[dsamp] = ((long **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                {   if (attrib == 1) dscan[dsamp] /= sqrt(2.0);
                    else             dscan[dsamp] /= 4294967295.0;
                    ((long **)(grid))[trace][samp] = _LIMIT(-2147483648.0,dscan[dsamp],2147483647.0);
                }
            }
            break;
        case FLOAT:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
					dscan[dsamp] = ((float **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                    ((float **)(grid))[trace][samp] = dscan[dsamp];
            }
			break;
        case DOUBLE:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
					dscan[dsamp] = ((double **)(grid))[trace][samp];
                if (num_fillers)
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;

                InstAttribTrace(trace_length,preproc,attrib,dscan,cscan);

				for (dsamp=first_samp,   samp=header_samps+first_samp;
                     dsamp<trace_length, samp<num_rows;
                     dsamp++,            samp++)
                    ((double **)(grid))[trace][samp] = dscan[dsamp];
            }
            break;
	}

	/* Deallocate storage and return success */
    free(dscan); free(cscan);
    return 0;
}
/************************** FftFilterGridTraces() ***************************/
/* Determine if FFT filtering requested and implement.
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
 *  long num_cols     - number of columns in grid
 *  long num_rows     - number of rows in grid
 *  long first_samp   - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  int preproc      - smooth trace edges to 0 if !=0
 *  double ns_per_samp - sample rate in nanoseconds per sample point
 *  double lo_freq_cutoff - lowest freq. in Hz to allow in data
 *  double hi_freq_cutoff - highest freq. in Hz to allow in data
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: Tests are made to avoid wrap around and out-of-range errors.
 *
 * WARNING: This function may not be appropriate for grids that have traces
 *          which contain trace headers!
 * NOTE: Modifications August 28, should fix above warning!
 *
 * Requires: <math.h>, <stdio.h>, "assertjl.h", "gpr_dfx.h", "gpr_fft.h".
 * Calls: assert, malloc, free, pow, FftFilterTrace.
 * Returns:  0 on sucess or if no filtering required.
 *          >0 on error.
const char *FftFilterGridTracesMsg[] =
{   "FftFilterGridTraces(): No errors.",
	"FftFilterGridTraces() ERROR: Number of samples greater then 65536.",
	"FftFilterGridTraces() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: December 24, 1996
 * Revisions: August 29, 1997; September 4, 1997; September 10, 1997
 */
int FftFilterGridTraces (long num_cols,long num_rows,long first_samp,
		int header_samps,int datatype,
		int preproc,double ns_per_samp,double lo_freq_cutoff,
		double hi_freq_cutoff,void **grid)
{
	long    i,trace,samp,dsamp;       /* scratch variables and counters */
	long	num_samps;                /* number of actual trace samples */
	long    num_fillers;              /* numer of fill samples if num_samps not power of 2 */
    long    trace_length;             /* number of "samples" used for FFT (power of 2) */
	long    lo_wavenum_cutoff,hi_wavenum_cutoff; /* FFT wavenumber, k */
	long    min_wavenum,max_wavenum;  /* min and max FFT wavenumbers for a trace */
	double  fund_freq,Nyq_freq;       /* fundamental and Nyquist freq. of a trace */
	double *dscan     = NULL;         /* temp. array for FFT filtering */

	assert(grid);
	assert(first_samp>=0);
	assert(header_samps>=0);
	assert(num_rows>0);
	assert(num_cols>0);
	assert(ns_per_samp>0.0);

	/* Find number of trace samples */
    num_samps = num_rows - header_samps;
    if (num_samps > 65536L) return 1;

	/* Check if trace length a power of 2, <= 65536 */
    num_fillers = 0;
    trace_length = num_samps;
    {   for (i=1; i<16; i++)
		{   if (num_samps > pow(2,(double)i) &&
				num_samps < pow(2,(double)(i+1)))
			{   trace_length = pow(2,(double)(i+1));
                num_fillers = trace_length - num_samps;
				break;
			}
		}
    }

	/* Translate cutoff frequencies into wavenumbers and verify ranges.
	   freq = wavenum / (num_rows * ns_per_samp * 1.0E-09)
	   wavenum = freq * (num_rows * ns_per_samp * 1.0E-09)
			   = freq / fund_freq
	*/
	min_wavenum  = 0;
	max_wavenum  = trace_length / 2;
	/* Nyq_freq     = 1.0 / (ns_per_samp * 2.0E-09); */   /* Hz */
	/* fund_freq    = Nyq_freq / (num_rows / 2.0); */     /* Hz */
	Nyq_freq     = 1.0E09 / (2.0 * ns_per_samp); /* Hz */
	fund_freq    = 1.0E09 / ( (trace_length-1) * ns_per_samp ); /* Hz */

	lo_wavenum_cutoff = min_wavenum;   /* default value for no filter */
	hi_wavenum_cutoff = max_wavenum;   /* default value for no filter */
	if (lo_freq_cutoff > fund_freq)
	   lo_wavenum_cutoff = (long)((lo_freq_cutoff / fund_freq) + 0.5);
	if (hi_freq_cutoff < Nyq_freq)
	   hi_wavenum_cutoff = (long)((hi_freq_cutoff / fund_freq) + 0.5);
	if (lo_wavenum_cutoff < min_wavenum)  lo_wavenum_cutoff = min_wavenum;
	if (hi_wavenum_cutoff > max_wavenum)  hi_wavenum_cutoff = max_wavenum;
	if (lo_wavenum_cutoff > hi_wavenum_cutoff)
	{   /* conflict so no filtering */
		lo_wavenum_cutoff = min_wavenum;
		hi_wavenum_cutoff = max_wavenum;
	}

	if ( (lo_wavenum_cutoff <= min_wavenum) &&
		 (hi_wavenum_cutoff >= max_wavenum) )
		return 0;  /* no filtering required */

	/* Allocate work array */
	dscan = (double *)malloc(trace_length * sizeof(double));
	if (dscan == NULL) return 2;

	/* Copy trace to work array, filter, and copy back */
	switch (datatype)
	{   case -1:
			for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((unsigned char **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 128.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
				{   if (dscan[dsamp] <   0.0) dscan[dsamp] =   0.0;
					if (dscan[dsamp] > 255.0) dscan[dsamp] = 255.0;
					((unsigned char **)(grid))[trace][samp] = dscan[dsamp];
				}
			}
			break;
        case 1:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((char **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
				{   if (dscan[dsamp] < -128.0) dscan[dsamp] = -128.0;
					if (dscan[dsamp] >  127.0) dscan[dsamp] =  127.0;
					((char **)(grid))[trace][samp] = dscan[dsamp];
				}
			}
			break;
		case -2:
			for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((unsigned short **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 32768.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
				{   if (dscan[dsamp] <     0.0) dscan[dsamp] =     0.0;
					if (dscan[dsamp] > 65535.0) dscan[dsamp] = 65535.0;
					((unsigned short **)(grid))[trace][samp] = dscan[dsamp];
				}
			}
			break;
		case -5:
			for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((unsigned short **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 2048.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
				{   if (dscan[dsamp] <    0.0) dscan[dsamp] =    0.0;
					if (dscan[dsamp] > 4095.0) dscan[dsamp] = 4095.0;
					((unsigned short **)(grid))[trace][samp] = dscan[dsamp];
				}
			}
			break;
		case 2:
			for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((short **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
				{   if (dscan[dsamp] < -32768.0) dscan[dsamp] = -32768.0;
                    if (dscan[dsamp] >  32767.0) dscan[dsamp] =  32767.0;
                    ((short **)(grid))[trace][samp] = dscan[dsamp];
                }
			}
            break;
        case -3:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
                    dscan[dsamp] = ((unsigned long **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 2147483648.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
                               hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
                {   if (dscan[dsamp] <          0.0) dscan[dsamp] =          0.0;
                    if (dscan[dsamp] > 4294967295.0) dscan[dsamp] = 4294967295.0;
					((unsigned long **)(grid))[trace][samp] = dscan[dsamp];
                }
			}
            break;
        case -6:
			for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
                    dscan[dsamp] = ((unsigned long **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 8388608.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
                               hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
                {   if (dscan[dsamp] <        0.0) dscan[dsamp] =        0.0;
                    if (dscan[dsamp] > 16777215.0) dscan[dsamp] = 16777215.0;
					((unsigned long **)(grid))[trace][samp] = dscan[dsamp];
                }
			}
            break;
        case 3:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
                    dscan[dsamp] = ((long **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
                {   if (dscan[dsamp] < -2147483648.0) dscan[dsamp] = -2147483648.0;
					if (dscan[dsamp] >  2147483647.0) dscan[dsamp] =  2147483647.0;
					((unsigned char **)(grid))[trace][samp] = dscan[dsamp];
                }
            }
            break;
        case 4:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((float **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;
				FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
							   hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
                {   if (dscan[dsamp] < -FLT_MAX) dscan[dsamp] = -FLT_MAX;
					if (dscan[dsamp] >  FLT_MAX) dscan[dsamp] =  FLT_MAX;
                    ((float **)(grid))[trace][samp] = dscan[dsamp];
				}
            }
			break;
        case 8:
            for (trace=0; trace<num_cols; trace++)
			{   for (dsamp=0,            samp=header_samps; 
                     dsamp<trace_length, samp<num_rows; 
                     dsamp++,            samp++)
					dscan[dsamp] = ((double **)(grid))[trace][samp];
                if (num_fillers)  
                    for (samp=num_samps; samp<trace_length; samp++)
                        dscan[samp] = 0.0;
                FftFilterTrace(trace_length,preproc,lo_wavenum_cutoff,
                               hi_wavenum_cutoff,dscan);
				for (dsamp=first_samp,   samp=header_samps+first_samp; 
                     dsamp<trace_length, samp<num_rows;  
                     dsamp++,            samp++)
                    ((double **)(grid))[trace][samp] = dscan[dsamp];
            }
            break;
	}

	/* Deallocate storage and return success */
    free(dscan);
    return 0;
}
/*************************** RemGridGlobBckgrnd() ***************************/
/* This function adds all the traces in a the grid together then divides by
 * number of scans to get an average trace, which can be conside_lenred the
 * background.
 * The trace is normalized so that it is centered about zero then subtracted
 * from each trace in the grid.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 *
 * Requires: <float.h>, <stdlib.h>, and "gpr_dfx.h".
 * Calls: calloc, free.
 * Returns: 0 on success, or
 *         >0 on error
 *
const char *RemGridGlobBckgrndMsg[] =
{   "RemGridGlobBckgrnd(): No errors.",
	"RemGridGlobBckgrnd() ERROR: Invalid function arguments.",
	"RemGridGlobBckgrnd() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int RemGridGlobBckgrnd (long num_cols,long num_rows,long first_samp,
                        int header_samps,int datatype,void **grid)
{
	long trace,samp;
	double dtemp;
	double *total       = NULL;

	/* Validate function argument list */
	if ( (grid == NULL) || (num_cols == 0) || (num_rows == 0) || first_samp < 0 ||
          header_samps < 0)
		return 1;

	/* Allocate storage for work array */
	total = (double *)calloc((size_t)num_rows,sizeof(double)); /* set to 0 */
	if (total == NULL) return 2;

    switch (datatype)
    {   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

			/* add up all the traces */
			for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					total[samp] += uc_pp[trace][samp];
			}
			/* get average trace and set mean to 0 */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   total[samp] /= (double)num_cols;
				total[samp] -= 128.0;
			}
			/* subtract average trace from grid */
			for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)uc_pp[trace][samp] - total[samp];
					if (dtemp < 0.0)        uc_pp[trace][samp] = 0;
                    else if (dtemp > 255.0) uc_pp[trace][samp] = 255;
                    else                    uc_pp[trace][samp] = dtemp;
                }
			}
            break;
        }
		case 1:
        {   char **c_pp = (char **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += c_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
			/* subtract average trace from grid */
			for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)c_pp[trace][samp] - total[samp];
                    if (dtemp < -128.0)     c_pp[trace][samp] = -128;
                    else if (dtemp > 127.0) c_pp[trace][samp] =  127;
					else                    c_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case -2:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += us_pp[trace][samp];
            }
            /* get average trace and set mean to 0 */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   total[samp] /= (double)num_cols;
                total[samp] -= 32768.0;
            }
            /* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] - total[samp];
                    if (dtemp < 0.0)          us_pp[trace][samp] = 0;
                    else if (dtemp > 65535.0) us_pp[trace][samp] = 65535L;
                    else                      us_pp[trace][samp] = dtemp;
                }
            }
            break;
		}
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += us_pp[trace][samp];
            }
            /* get average trace and set mean to 0 */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {    total[samp] /= (double)num_cols;
				total[samp] -= 32768.0;
            }
            /* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] - total[samp];
                    if (dtemp < 0.0)         us_pp[trace][samp] = 0;
                    else if (dtemp > 4095.0) us_pp[trace][samp] = 4095;
                    else                     us_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 2:
		{   short **s_pp = (short **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += s_pp[trace][samp];
			}
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
            /* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)s_pp[trace][samp] - total[samp];
                    if (dtemp < -32768.0)     s_pp[trace][samp] = -32768L;
                    else if (dtemp > 32767.0) s_pp[trace][samp] =  32767;
                    else                      s_pp[trace][samp] = dtemp;
                }
            }
			break;
		   }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += ul_pp[trace][samp];
            }
            /* get average trace and set mean to 0 */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   total[samp] /= (double)num_cols;
                total[samp] -= 2147483648.0;
            }
            /* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)ul_pp[trace][samp] - total[samp];
                    if (dtemp < 0.0)               ul_pp[trace][samp] = 0;
                    else if (dtemp > 4294967295.0) ul_pp[trace][samp] = 4294967295L;
					else                           ul_pp[trace][samp] = dtemp;
                }
            }
            break;
           }
        case 3:
		{   long **l_pp = (long **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += l_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
            /* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)l_pp[trace][samp] - total[samp];
					if (dtemp < -2147483648.0)     l_pp[trace][samp] = -2147483648L;
                    else if (dtemp > 2147483647.0) l_pp[trace][samp] =  2147483647L;
                    else                           l_pp[trace][samp] = dtemp;
                }
			}
            break;
		   }
        case 4:
        {   float **f_pp = (float **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += f_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
            /* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)f_pp[trace][samp] - total[samp];
                    if (dtemp < -FLT_MAX)     f_pp[trace][samp] = -FLT_MAX;
                    else if (dtemp > FLT_MAX) f_pp[trace][samp] =  FLT_MAX;
                    else                      f_pp[trace][samp] = dtemp;
                }
			}
            break;
           }
        case 8:
        {   double **d_pp = (double **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += d_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
			/* subtract average trace from grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] -= total[samp];
            }
            break;
		}
    }

	free(total);
    return 0;
}
/*************************** RemGridGlobForgrnd() ***************************/
/* This function adds all the traces in a the grid together then divides by
 * number of scans to get an average trace, which can be conside_lenred the
 * background.
 * The trace is normalized so that it is centered about zero then subtracted
 * from each trace in the grid.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 *
 * Requires: <float.h>, <stdlib.h>, and "gpr_dfx.h".
 * Calls: calloc, free.
 * Returns: 0 on success, or
 *         >0 on error
 *
const char *RemGridGlobForgrndMsg[] =
{   "RemGridGlobForgrnd(): No errors.",
	"RemGridGlobForgrnd() ERROR: Invalid function arguments.",
	"RemGridGlobForgrnd() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997; March 28, 2000;
 */
int RemGridGlobForgrnd (long num_cols,long num_rows,long first_samp,
                        int header_samps,int datatype,void **grid)
{
	long trace,samp;
	double dtemp;
	double *total       = NULL;

	/* Validate function argument list */
	if ( (grid == NULL) || (num_cols == 0) || (num_rows == 0) || first_samp < 0 ||
          header_samps < 0)
		return 1;

	/* Allocate storage for work array */
	total = (double *)calloc((size_t)num_rows,sizeof(double)); /* set to 0 */
	if (total == NULL) return 2;

    switch (datatype)
    {   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

			/* add up all the traces */
			for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					total[samp] += uc_pp[trace][samp];
			}
			/* get average trace and set mean to 0 */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   total[samp] /= (double)num_cols;
				total[samp] -= 128.0;
			}
			/* subtract average trace from grid */
			for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = total[samp];
					if (dtemp < 0.0)        uc_pp[trace][samp] = 0;
                    else if (dtemp > 255.0) uc_pp[trace][samp] = 255;
                    else                    uc_pp[trace][samp] = dtemp;
                }
			}
            break;
        }
		case 1:
        {   char **c_pp = (char **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += c_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
			/* assign average trace to grid */
			for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
                    if (dtemp < -128.0)     c_pp[trace][samp] = -128;
                    else if (dtemp > 127.0) c_pp[trace][samp] =  127;
					else                    c_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case -2:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += us_pp[trace][samp];
            }
            /* get average trace and set mean to 0 */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)
			{   total[samp] /= (double)num_cols;
                total[samp] -= 32768.0;
            }
            /* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
                    if (dtemp < 0.0)          us_pp[trace][samp] = 0;
                    else if (dtemp > 65535.0) us_pp[trace][samp] = 65535L;
                    else                      us_pp[trace][samp] = dtemp;
                }
            }
            break;
		}
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += us_pp[trace][samp];
            }
            /* get average trace and set mean to 0 */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {    total[samp] /= (double)num_cols;
				total[samp] -= 32768.0;
            }
            /* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
                    if (dtemp < 0.0)         us_pp[trace][samp] = 0;
                    else if (dtemp > 4095.0) us_pp[trace][samp] = 4095;
                    else                     us_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 2:
		{   short **s_pp = (short **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += s_pp[trace][samp];
			}
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
            /* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
                    if (dtemp < -32768.0)     s_pp[trace][samp] = -32768L;
                    else if (dtemp > 32767.0) s_pp[trace][samp] =  32767;
                    else                      s_pp[trace][samp] = dtemp;
                }
            }
			break;
		   }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += ul_pp[trace][samp];
            }
            /* get average trace and set mean to 0 */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
            {   total[samp] /= (double)num_cols;
                total[samp] -= 2147483648.0;
            }
            /* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
                    if (dtemp < 0.0)               ul_pp[trace][samp] = 0;
                    else if (dtemp > 4294967295.0) ul_pp[trace][samp] = 4294967295L;
					else                           ul_pp[trace][samp] = dtemp;
                }
            }
            break;
           }
        case 3:
		{   long **l_pp = (long **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += l_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
            /* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
					if (dtemp < -2147483648.0)     l_pp[trace][samp] = -2147483648L;
                    else if (dtemp > 2147483647.0) l_pp[trace][samp] =  2147483647L;
                    else                           l_pp[trace][samp] = dtemp;
                }
			}
            break;
		   }
        case 4:
        {   float **f_pp = (float **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += f_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
            /* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = total[samp];
                    if (dtemp < -FLT_MAX)     f_pp[trace][samp] = -FLT_MAX;
                    else if (dtemp > FLT_MAX) f_pp[trace][samp] =  FLT_MAX;
                    else                      f_pp[trace][samp] = dtemp;
                }
			}
            break;
           }
        case 8:
        {   double **d_pp = (double **)grid;

            /* add up all the traces */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    total[samp] += d_pp[trace][samp];
            }
            /* get average trace */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)
                total[samp] /= (double)num_cols;
			/* assign average trace to grid */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] = total[samp];
            }
            break;
		}
    }

	free(total);
    return 0;
}
/*************************** RemGridWindBckgrnd() ***************************/
/* This function adds all the traces within a sliding window along the grid
 * together then divides by number of traces to get an average trace for that
 * windows, which can be considered the background trace.
 * The trace is normalized so that it is centered about zero then subtracted
 * from the trace in the center of the window.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long win_length - number of traces to be used in the sliding window;
 *                   if an even value, will be decremented by one to be odd
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 *
 * Requires: "assertjl.h", <float.h>, <stdlib.h>, and "gpr_dfx.h".
 * Calls: malloc, calloc, free, assert.
 * Returns: 0 on success, or
 *         >0 on error.
const char *RemGridWindBckgrndMsg[] =
{   "RemGridWindBckgrnd(): No errors.",
	"RemGridWindBckgrnd() ERROR: Invalid function arguments.",
	"RemGridWindBckgrnd() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int RemGridWindBckgrnd (long num_cols,long num_rows,long first_samp,
						int header_samps,int datatype,
				        long win_length,void **grid)
{
	long i,j,trace,samp;    /* counters */
	long itemp1,mid_queue;  /* scratch variables */
	long q_index;           /* tracks which trace to replace next in the queue */
	long next_trace;        /* tracks which trace to get next from grid */
	double dtemp;          /* scratch variables */
	double  *avg   = NULL; /* average value for traces in window */
	double **queue = NULL; /* circular queue to hold currently used traces */

	/* Validate function argument list */
	if ( (grid == NULL)   || (num_cols == 0) || (num_rows == 0) ||
		 (win_length < 3) || win_length >= num_cols || first_samp < 0 ||
         header_samps < 0)
		 return 1;
	if ((win_length % 2) == 0) win_length--;  /* must be an odd number */

	/* Allocate storage for work arrays */
	avg = (double *)calloc((size_t)num_rows,sizeof(double));
	if (avg == NULL) return 4;
	queue = (double **)malloc(win_length*sizeof(double *));
	if (queue)
	{   for (i=0; i<win_length; i++)
		{   queue[i] = (double *)malloc(num_rows*sizeof(double));
			if (queue[i] == NULL)
			{   for (j=0; j<i; j++) free(queue[i]);
				free(avg);  free(queue);
				return 2;
			}
		}
	} else
	{   free(avg);
		return 2;
	}

	/* Perform the sliding window background removal */
    switch (datatype)
    {   case -1:
        {   unsigned char **uc_pp = (unsigned char **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
			for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
				assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = uc_pp[trace][samp];
                    avg[samp] += uc_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)uc_pp[trace][samp] - avg[samp] + 128.0;
                    if (dtemp < 0.0)        uc_pp[trace][samp] = 0;
                    else if (dtemp > 255.0) uc_pp[trace][samp] = 255;
                    else                    uc_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
						queue[q_index][samp] = uc_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
			}
            break;
        }
        case 1:
        {   char **c_pp = (char **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   queue[itemp1][samp] = c_pp[trace][samp];
                    avg[samp] += c_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
			   next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)c_pp[trace][samp] - avg[samp];
                    if (dtemp < -128.0)     c_pp[trace][samp] = -128;
                    else if (dtemp > 127.0) c_pp[trace][samp] =  127;
                    else                    c_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = c_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
				next_trace++;
            }
            break;
        }
        case -2:
		{   unsigned short **us_pp = (unsigned short **)grid;

			/* initialize queue: can only pull in half because starting at
			   index=0, so average second half of queue and assign to all
			   first half values (this does not change average for queue).
			 */
			/* initialize values */
			next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
			mid_queue  = next_trace - 1;       /* middle trace in queue */
			itemp1     = mid_queue;            /* offset into queue */

			/* copy values from grid into second half of queue; sum into avg */
			for (trace=0; trace<next_trace; trace++,itemp1++)
			{   /* here's a good place for debug check of array limits */
				assert( (trace < num_cols) && (itemp1 < win_length) );
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   queue[itemp1][samp] = us_pp[trace][samp];
					avg[samp] += us_pp[trace][samp];
				}
			}

			/* calculate average for second half of queue */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
			 */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] - avg[samp] + 32768.0;
                    if (dtemp < 0.0)          us_pp[trace][samp] = 0;
                    else if (dtemp > 65535.0) us_pp[trace][samp] = 65535L;
                    else                      us_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

				/* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = us_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
					for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
		}
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
			   first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
			itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = us_pp[trace][samp];
                    avg[samp] += us_pp[trace][samp];
				}
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

			/* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
			   the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)us_pp[trace][samp] - avg[samp] + 32768.0;
                    if (dtemp < 0.0)         us_pp[trace][samp] = 0;
                    else if (dtemp > 4095.0) us_pp[trace][samp] = 4095;
                    else                     us_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

				/* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = us_pp[next_trace][samp];
				} else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

				/* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

			/* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
			mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = s_pp[trace][samp];
                    avg[samp] += s_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
			}

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
			{   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)s_pp[trace][samp] - avg[samp];
                    if (dtemp < -32768.0)     s_pp[trace][samp] = -32768L;
                    else if (dtemp > 32767.0) s_pp[trace][samp] =  32767;
                    else                      s_pp[trace][samp] = dtemp;
				}

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = s_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
				}

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

				/* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
			/* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
			for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = ul_pp[trace][samp];
                    avg[samp] += ul_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
			   centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)ul_pp[trace][samp] - avg[samp] + 2147483648.0;
                    if (dtemp < 0.0)               ul_pp[trace][samp] = 0;
					else if (dtemp > 4294967295.0) ul_pp[trace][samp] = 4294967295L;
                    else                           ul_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = ul_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
			}
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
			/* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   queue[itemp1][samp] = l_pp[trace][samp];
                    avg[samp] += l_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

			/* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)l_pp[trace][samp] - avg[samp];
                    if      (dtemp < -2147483648.0) l_pp[trace][samp] = -2147483648L;
                    else if (dtemp >  2147483647.0) l_pp[trace][samp] =  2147483647L;
                    else                            l_pp[trace][samp] = dtemp;
                }

				/* remove the q_index trace values from the average */
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = l_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
				if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
				assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = f_pp[trace][samp];
                    avg[samp] += f_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)f_pp[trace][samp] - avg[samp] + 127.0;
                    if (dtemp < -FLT_MAX)     f_pp[trace][samp] = -FLT_MAX;
                    else if (dtemp > FLT_MAX) f_pp[trace][samp] =  FLT_MAX;
                    else                      f_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

				/* copy next grid trace into the queue */
                if (next_trace < num_cols)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = f_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
		}
        case 8:
		{   double **d_pp = (double **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid int second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = d_pp[trace][samp];
                    avg[samp] += d_pp[trace][samp];
				}
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
			   the average trace values for unsigned data; signed data is
               left alone.
			 */
            for (trace=0; trace<num_cols; trace++)
            {   /* subtract current average from current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] -= avg[samp];

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = d_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
				}

				/* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
    }

    /* Deallocate storage and return success */
    for (i=0; i<win_length; i++) free(queue[i]);
    free(queue);
    free(avg);
    return 0;
}
/******************************* RescaleGrid() ******************************/
/* Multiply each sample in each trace in the grid by a constant value.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  double amp_scale - value to multiply sample by
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: To speed up function, there is NO error checking. But tests are made
 *       to avoid wrap around and out if range errors.
 *
 * Requires: <float.h>, "assertjl.h", "gpr_dfx.h".
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
void RescaleGrid (long num_cols,long num_rows,long first_samp,
				  int header_samps,int datatype,double amp_scale,void **grid)
{
	long trace,samp;
	double dtemp;

	assert(grid && first_samp>=0 && header_samps>=0);

	switch (datatype)
    {   case -1:
        {   unsigned char **uc_pp = (unsigned char **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)uc_pp[trace][samp] - 128.0;
					dtemp *= amp_scale;
                    if (dtemp < -128.0)  dtemp = -128;
					if (dtemp >  127.0)  dtemp =  127;
                    dtemp += 128.0;
                    uc_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 1:
        {   char **c_pp = (char **)grid;

			for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)c_pp[trace][samp] * amp_scale;
                    if (dtemp < -128.0)     c_pp[trace][samp] = -128;
                    else if (dtemp > 127.0) c_pp[trace][samp] =  127;
                    else                    c_pp[trace][samp] = dtemp;
                }
            }
			break;
        }
        case -2:
        {   unsigned short **us_pp = (unsigned short **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] - 32768.0;
                    dtemp *= amp_scale;
                    if (dtemp < -32768.0)  dtemp = -32768.0;
                    if (dtemp >  32767.0)  dtemp =  32767.0;
                    dtemp += 32768.0;
                    us_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

			for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)us_pp[trace][samp] - 2048.0;
                    dtemp *= amp_scale;
                    if (dtemp < -2048.0)  dtemp = -2048.0;
                    if (dtemp >  2047.0)  dtemp =  2047.0;
                    dtemp += 2048.0;
					us_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)s_pp[trace][samp] * amp_scale;
                    if (dtemp < -32768.0)     s_pp[trace][samp] = -32768L;
                    else if (dtemp > 32767.0) s_pp[trace][samp] =  32767;
					else                      s_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case -3:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)ul_pp[trace][samp] - 2147483647.0;
                    dtemp *= amp_scale;
                    if (dtemp < -2147483647.0)  dtemp = -2147483647.0;
                    if (dtemp >  2147483646.0)  dtemp =  2147483646.0;
                    dtemp += 2147483647.0;
                    ul_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
		case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)ul_pp[trace][samp] - 8388608.0;
                    dtemp *= amp_scale;
                    if (dtemp < -8388608.0)  dtemp = -8388608.0;
                    if (dtemp >  8388607.0)  dtemp =  8388607.0;
                    dtemp += 8388608.0;
                    ul_pp[trace][samp] = dtemp;
				}
			}
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;

            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = (double)l_pp[trace][samp] * amp_scale;
                    if (dtemp < -2147483648.0)     l_pp[trace][samp] = -2147483648L;
                    else if (dtemp > 2147483647.0) l_pp[trace][samp] = 2147483647L;
					else                           l_pp[trace][samp] = dtemp;
                }
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;

            /* Warning: Assumes centered about 0 */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = (double)f_pp[trace][samp] * amp_scale;
                    if (dtemp < -FLT_MAX)     f_pp[trace][samp] = -FLT_MAX;
                    else if (dtemp > FLT_MAX) f_pp[trace][samp] =  FLT_MAX;
                    else                      f_pp[trace][samp] = dtemp;
                }
            }
			break;
        }
        case 8:
        {   double **d_pp = (double **)grid;

            /* Warning: Assumes centered about 0 */
            for (trace=0; trace<num_cols; trace++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] *= amp_scale;
            }
            break;
        }
    }
}
/*************************** RemGridWindForgrnd() ***************************/
/* This function adds all the traces within a sliding window along the grid
 * together then divides by number of traces to get an average trace for that
 * windows, which can be considered the background trace.
 * The trace is normalized so that it is centered about zero then assigned
 * to the trace in the center of the window.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long win_length - number of traces to be used in the sliding window;
 *                   if an even value, will be decremented by one to be odd
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 *
 * Requires: "assertjl.h", <float.h>, <stdlib.h>, and "gpr_dfx.h".
 * Calls: malloc, calloc, free, assert.
 * Returns: 0 on success, or
 *         >0 on error.
const char *RemGridWindForgrndMsg[] =
{   "RemGridWindForgrnd(): No errors.",
	"RemGridWindForgrnd() ERROR: Invalid function arguments.",
	"RemGridWindForgrnd() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Central Mineral Resources Program   Lakewood, CO
 * Date: February 5, 1996
 * Revisions:  September 4, 1997; March 28, 2000;
 */
int RemGridWindForgrnd (long num_cols,long num_rows,long first_samp,
						int header_samps,int datatype,
				        long win_length,void **grid)
{
	long i,j,trace,samp;    /* counters */
	long itemp1,mid_queue;  /* scratch variables */
	long q_index;           /* tracks which trace to replace next in the queue */
	long next_trace;        /* tracks which trace to get next from grid */
	double dtemp;          /* scratch variables */
	double  *avg   = NULL; /* average value for traces in window */
	double **queue = NULL; /* circular queue to hold currently used traces */

	/* Validate function argument list */
	if ( (grid == NULL)   || (num_cols == 0) || (num_rows == 0) ||
		 (win_length < 3) || win_length >= num_cols || first_samp < 0 ||
         header_samps < 0)
		 return 1;
	if ((win_length % 2) == 0) win_length--;  /* must be an odd number */

	/* Allocate storage for work arrays */
	avg = (double *)calloc((size_t)num_rows,sizeof(double));
	if (avg == NULL) return 4;
	queue = (double **)malloc(win_length*sizeof(double *));
	if (queue)
	{   for (i=0; i<win_length; i++)
		{   queue[i] = (double *)malloc(num_rows*sizeof(double));
			if (queue[i] == NULL)
			{   for (j=0; j<i; j++) free(queue[i]);
				free(avg);  free(queue);
				return 2;
			}
		}
	} else
	{   free(avg);
		return 2;
	}

	/* Perform the sliding window background removal */
    switch (datatype)
    {   case -1:
        {   unsigned char **uc_pp = (unsigned char **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
			for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
				assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = uc_pp[trace][samp];
                    avg[samp] += uc_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp] + 128.0;
                    if (dtemp < 0.0)        uc_pp[trace][samp] = 0;
                    else if (dtemp > 255.0) uc_pp[trace][samp] = 255;
                    else                    uc_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
						queue[q_index][samp] = uc_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
			}
            break;
        }
        case 1:
        {   char **c_pp = (char **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   queue[itemp1][samp] = c_pp[trace][samp];
                    avg[samp] += c_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
			   next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp];
                    if (dtemp < -128.0)     c_pp[trace][samp] = -128;
                    else if (dtemp > 127.0) c_pp[trace][samp] =  127;
                    else                    c_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = c_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
				next_trace++;
            }
            break;
        }
        case -2:
		{   unsigned short **us_pp = (unsigned short **)grid;

			/* initialize queue: can only pull in half because starting at
			   index=0, so average second half of queue and assign to all
			   first half values (this does not change average for queue).
			 */
			/* initialize values */
			next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
			mid_queue  = next_trace - 1;       /* middle trace in queue */
			itemp1     = mid_queue;            /* offset into queue */

			/* copy values from grid into second half of queue; sum into avg */
			for (trace=0; trace<next_trace; trace++,itemp1++)
			{   /* here's a good place for debug check of array limits */
				assert( (trace < num_cols) && (itemp1 < win_length) );
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   queue[itemp1][samp] = us_pp[trace][samp];
					avg[samp] += us_pp[trace][samp];
				}
			}

			/* calculate average for second half of queue */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
			 */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp] + 32768.0;
                    if (dtemp < 0.0)          us_pp[trace][samp] = 0;
                    else if (dtemp > 65535.0) us_pp[trace][samp] = 65535L;
                    else                      us_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

				/* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = us_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
					for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
		}
        case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
			   first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
			itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = us_pp[trace][samp];
                    avg[samp] += us_pp[trace][samp];
				}
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

			/* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
			   the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   dtemp = avg[samp] + 32768.0;
                    if (dtemp < 0.0)         us_pp[trace][samp] = 0;
                    else if (dtemp > 4095.0) us_pp[trace][samp] = 4095;
                    else                     us_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

				/* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = us_pp[next_trace][samp];
				} else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

				/* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

			/* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
			mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = s_pp[trace][samp];
                    avg[samp] += s_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
			for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
			}

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
			{   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp];
                    if (dtemp < -32768.0)     s_pp[trace][samp] = -32768L;
                    else if (dtemp > 32767.0) s_pp[trace][samp] =  32767;
                    else                      s_pp[trace][samp] = dtemp;
				}

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = s_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
				}

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

				/* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
			/* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
			for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = ul_pp[trace][samp];
                    avg[samp] += ul_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
			{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
			   centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp] + 2147483648.0;
                    if (dtemp < 0.0)               ul_pp[trace][samp] = 0;
					else if (dtemp > 4294967295.0) ul_pp[trace][samp] = 4294967295L;
                    else                           ul_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = ul_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
			}
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
			/* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
				{   queue[itemp1][samp] = l_pp[trace][samp];
                    avg[samp] += l_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

			/* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp];
                    if      (dtemp < -2147483648.0) l_pp[trace][samp] = -2147483648L;
                    else if (dtemp >  2147483647.0) l_pp[trace][samp] =  2147483647L;
                    else                            l_pp[trace][samp] = dtemp;
                }

				/* remove the q_index trace values from the average */
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = l_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
				if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
				assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = f_pp[trace][samp];
                    avg[samp] += f_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
               the average trace values for unsigned data; signed data is
               left alone.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   dtemp = avg[samp];
                    if (dtemp < -FLT_MAX)     f_pp[trace][samp] = -FLT_MAX;
                    else if (dtemp > FLT_MAX) f_pp[trace][samp] =  FLT_MAX;
                    else                      f_pp[trace][samp] = dtemp;
                }

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

				/* copy next grid trace into the queue */
                if (next_trace < num_cols)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = f_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* add the new trace value into the average */
				for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
		}
        case 8:
		{   double **d_pp = (double **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid int second half of queue; sum into avg */
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = d_pp[trace][samp];
                    avg[samp] += d_pp[trace][samp];
				}
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* Work through grid, sliding window along, updating the queue
			   and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.  Note that the average value should be
               centered about 0, so a median range value is subtracted from
			   the average trace values for unsigned data; signed data is
               left alone.
			 */
            for (trace=0; trace<num_cols; trace++)
            {   /* assign current average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] = avg[samp];

                /* remove the q_index trace values from the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] -= queue[q_index][samp]/(double)win_length;

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = d_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
				}

				/* add the new trace value into the average */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += queue[q_index][samp]/(double)win_length;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
    }

    /* Deallocate storage and return success */
    for (i=0; i<win_length; i++) free(queue[i]);
    free(queue);
    free(avg);
    return 0;
}
/****************************** SlideSamples() ******************************/
/* Slide each sample value up or down locations in each trace in the grid.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  int samp_slide - value to slide samples by
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: To speed up function, there is NO error checking. But tests are made
 *       to avoid wrap around and out if range errors.
 *
 * Requires: <float.h>, "assertjl.h", "gpr_dfx.h".
 * Calls: assert.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   
 * Date: December 23, 1996
 * Revisions: September 4, 1997; September 10, 1997
 *            April 27, 1999 slide samps algorithm wrong if negative
 */
void SlideSamples (long num_cols,long num_rows,long first_samp,
				   int header_samps,int datatype,int samp_slide,void **grid)
{
	long trace,samp,new_loc,temp_loc;

	assert(grid);
	assert(first_samp>=0);
	assert(header_samps>=0);
	switch (datatype)
	{   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							uc_pp[trace][samp] = uc_pp[trace][new_loc];
						else
							uc_pp[trace][samp] = 128;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							uc_pp[trace][samp] = uc_pp[trace][new_loc];
						else
							uc_pp[trace][samp] = 128;
					}
				}
			}
			break;
		}
		case 1:
		{   char **c_pp = (char **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							c_pp[trace][samp] = c_pp[trace][new_loc];
						else
							c_pp[trace][samp] = 0;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							c_pp[trace][samp] = c_pp[trace][new_loc];
						else
							c_pp[trace][samp] = 0;
					}
				}
			}
			break;
		}
		case -2:
		{   unsigned short **us_pp = (unsigned short **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							us_pp[trace][samp] = us_pp[trace][new_loc];
						else
							us_pp[trace][samp] = 32768L;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							us_pp[trace][samp] = us_pp[trace][new_loc];
						else
							us_pp[trace][samp] = 32768L;
					}
				}
			}
			break;
		}
		case -5:
		{   unsigned short **us_pp = (unsigned short **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							us_pp[trace][samp] = us_pp[trace][new_loc];
						else
							us_pp[trace][samp] = 2048;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							us_pp[trace][samp] = us_pp[trace][new_loc];
						else
							us_pp[trace][samp] = 2048;
					}
				}
			}
			break;
		}
		case 2:
		{   short **s_pp = (short **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							s_pp[trace][samp] = s_pp[trace][new_loc];
						else
							s_pp[trace][samp] = 0;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							s_pp[trace][samp] = s_pp[trace][new_loc];
						else
							s_pp[trace][samp] = 0;
					}
				}
			}
			break;
		}
		case -3:
		{   unsigned long **ul_pp = (unsigned long **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							ul_pp[trace][samp] = ul_pp[trace][new_loc];
						else
							ul_pp[trace][samp] = 2147483647L;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							ul_pp[trace][samp] = ul_pp[trace][new_loc];
						else
							ul_pp[trace][samp] = 2147483647L;
					}
				}
			}
			break;
		}
		case -6:
		{   unsigned long **ul_pp = (unsigned long **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							ul_pp[trace][samp] = ul_pp[trace][new_loc];
						else
							ul_pp[trace][samp] = 8388608L;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							ul_pp[trace][samp] = ul_pp[trace][new_loc];
						else
							ul_pp[trace][samp] = 8388608L;
					}
				}
			}
			break;
		}
		case 3:
		{   long **l_pp = (long **)grid;

			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							l_pp[trace][samp] = l_pp[trace][new_loc];
						else
							l_pp[trace][samp] = 0;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							l_pp[trace][samp] = l_pp[trace][new_loc];
						else
							l_pp[trace][samp] = 0;
					}
				}
			}
			break;
		}
		case 4:
		{   float **f_pp = (float **)grid;

			/* Warning: Assumes centered about 0 */
			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							f_pp[trace][samp] = f_pp[trace][new_loc];
						else
							f_pp[trace][samp] = 0;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							f_pp[trace][samp] = f_pp[trace][new_loc];
						else
							f_pp[trace][samp] = 0;
					}
				}
			}
			break;
		}
		case 8:
		{   double **d_pp = (double **)grid;

			/* Warning: Assumes centered about 0 */
			for (trace=0; trace<num_cols; trace++)
			{   if (samp_slide > 0)
				{   temp_loc = header_samps+first_samp + samp_slide;
					for (samp=num_rows-1; samp>=header_samps+first_samp; samp--)
					{   new_loc = samp - samp_slide;
						if (samp >= temp_loc)
							d_pp[trace][samp] = d_pp[trace][new_loc];
						else
							d_pp[trace][samp] = 0;
					}
				} else if (samp_slide < 0)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					{   new_loc = samp - samp_slide;
						if (new_loc < num_rows)
							d_pp[trace][samp] = d_pp[trace][new_loc];
						else
							d_pp[trace][samp] = 0;
					}
				}
			}
			break;
		}
	}

}
/************************* SmoothGridHorizontally() *************************/
/* This function smooths the data "horizontally" by sliding a window through
 * the grid and averaging the traces within the window using a Hanning
 * function (0.5 - 0.5 * cos(t)), where the middle trace has a weight of 1
 * and the traces at either end of the window have weights of 0.  The
 * weighted average trace is then copied into the grid.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long half_length - number of traces to be used for one side of the sliding
 *                    window
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * Requires: "assertjl.h", <float.h>, <math.h>, <stdlib.h>, <string.h>,
 *           "gpr_dfx.h".
 * Calls: malloc, free, assert, cos, memset.
 * Returns: 0 on success, or
 *         >0 on error.
 *
const char *SmoothGridHorizontallyMsg[] =
{   "SmoothGridHorizontally(): No errors.",
	"SmoothGridHorizontally() ERROR: Invalid function arguments.",
	"SmoothGridHorizontally() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   
 * Date: January 25, 1996
 * Revisions:  September 4, 1997
 */
int SmoothGridHorizontally (long num_cols,long num_rows,long first_samp,
                            int header_samps,int datatype,long half_length,
                            void **grid)
{
    long i,j,trace,samp;    /* counters */
    long itemp1,mid_queue;  /* scratch variables */
    long q_index;           /* tracks which trace to replace next in the queue */
	long next_trace;        /* tracks which trace to get next from grid */
    long win_length;        /* size of circular queue */
    long bytes1;            /* number of bytes in avg[] buffer */
    double weight;          /* scratch variables */
    double total_weight;    /* sum of Hanning weights */
	double step;            /* increment between Hanning coefficients */
	double  *avg    = NULL; /* average value for traces in window */
	double **queue  = NULL; /* circular queue to hold currently used traces */
	double *weights = NULL; /* hanning weighting coefficients */
	static double twopi = 2 * 3.14159265358979323846;

	/* Validate function argument list */
	if ( (grid == NULL)   || (num_cols == 0) || (num_rows == 0) ||
		 (half_length < 2) || half_length >= num_cols/2 || first_samp < 0 ||
         header_samps < 0)
		 return 1;

	/* Allocate storage for work arrays */
	win_length = 2 * half_length + 1;
	bytes1 = num_rows * sizeof(double);
	avg = (double *)malloc((size_t)bytes1);
	if (avg == NULL) return 2;
	queue = (double **)malloc(win_length*sizeof(double *));
	if (queue)
	{   for (i=0; i<win_length; i++)
		{   queue[i] = (double *)malloc(num_rows*sizeof(double));
			if (queue[i] == NULL)
			{   for (j=0; j<i; j++) free(queue[i]);
				free(avg);  free(queue);
				return 2;
			}
		}
	} else
	{   free(avg);
		return 2;
	}
	weights = (double *)malloc(win_length * sizeof(double));
	if (weights == NULL)
	{   for (i=0; i<win_length; i++) free(queue[i]);
		free(queue); free(avg);
		return 2;
	}

	/* Pre-calculate weights for hanning window */
	weight = total_weight = 0.0;
	step   = twopi/(double)(win_length + 1);
	for (i=0; i<win_length; i++)
	{   weight += step;
		weights[i] = 0.5 - 0.5*cos(weight);   /* Hanning window */
		total_weight += weights[i];
	}

	/* Perform the sliding window weighted horizontal smoothing */
	switch (datatype)
    {   case -1:
        {   unsigned char **uc_pp = (unsigned char **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
			 */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
			{   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = uc_pp[trace][samp];
                    avg[samp] += uc_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

			/* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
			 */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    uc_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = uc_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
				{   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 1:
        {   char **c_pp = (char **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
			   first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = c_pp[trace][samp];
                    avg[samp] += c_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
			for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    c_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = c_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
				}

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
		}
        case -2:  case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = us_pp[trace][samp];
					avg[samp] += us_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
			{   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    us_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
				{   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = us_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

				/* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
			memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = s_pp[trace][samp];
                    avg[samp] += s_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
			q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
			{   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    s_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = s_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
						avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
			/* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = ul_pp[trace][samp];
                    avg[samp] += ul_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
			for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

			/* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    ul_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = ul_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

				/* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 3:
		{   long **l_pp = (long **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = l_pp[trace][samp];
                    avg[samp] += l_pp[trace][samp];
                }
			}

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    l_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = l_pp[next_trace][samp];
				} else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
				if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
            mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
			{   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = f_pp[trace][samp];
                    avg[samp] += f_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

			/* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
               grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
					f_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = f_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
					if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
        case 8:
        {   double **d_pp = (double **)grid;

            /* initialize queue: can only pull in half because starting at
               index=0, so average second half of queue and assign to all
               first half values (this does not change average for queue).
             */
            /* initialize values */
            next_trace = ((win_length-1)/2)+1; /* next trace to get from grid */
			mid_queue  = next_trace - 1;       /* middle trace in queue */
            itemp1     = mid_queue;            /* offset into queue */

            /* copy values from grid into second half of queue; sum into avg */
            memset((void *)avg,0x00,(size_t)bytes1);
            for (trace=0; trace<next_trace; trace++,itemp1++)
            {   /* here's a good place for debug check of array limits */
                assert( (trace < num_cols) && (itemp1 < win_length) );
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   queue[itemp1][samp] = d_pp[trace][samp];
                    avg[samp] += d_pp[trace][samp];
                }
            }

            /* calculate average for second half of queue */
            for (samp=header_samps+first_samp; samp<num_rows; samp++)  avg[samp] /= next_trace;

            /* copy average values into first half of queue */
            for (itemp1=0; itemp1<mid_queue; itemp1++)
            {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
					queue[itemp1][samp] = avg[samp];
            }

            /* initialize index of trace to be replaced next with new trace */
            q_index = 0;

            /* calculate weighted average of traces in the queue */
            memset((void *)avg,0x00,(size_t)bytes1);
            itemp1 = q_index;
            for (i=0; i<win_length; i++)
            {   weight = weights[i];
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    avg[samp] += weight * queue[itemp1][samp];
                itemp1++;
                if (itemp1 >= win_length) itemp1 = 0;
            }
            for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

            /* Work through grid, sliding window along, updating the queue
               and the average trace values.  When window slides past end of
			   grid, update queue with current average trace rather than
               next grid trace.
             */
            for (trace=0; trace<num_cols; trace++)
            {   /* copy weighted average to current trace in grid */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                    d_pp[trace][samp] = avg[samp];

                /* copy next grid trace into the queue */
                if (next_trace < num_cols)
                {   for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = d_pp[next_trace][samp];
                } else
                {   /* window is sliding past end, so repeat avg[] values */
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        queue[q_index][samp] = avg[samp];
                }

                /* calculate weighted average of traces in the queue */
                memset((void *)avg,0x00,(size_t)bytes1);
				itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   weight = weights[i];
                    for (samp=header_samps+first_samp; samp<num_rows; samp++)
                        avg[samp] += weight * queue[itemp1][samp];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                for (samp=header_samps+first_samp; samp<num_rows; samp++) avg[samp] /= total_weight;

                /* update q_index and next_trace */
                q_index++;
                if (q_index >= win_length) q_index = 0;
                next_trace++;
            }
            break;
        }
    }

    /* Deallocate storage and return success */
	for (i=0; i<win_length; i++) free(queue[i]);
    free(queue);
    free(avg);
    free(weights);
    return 0;
}
/************************** SmoothGridVertically() **************************/
/* This function smooths the data "vertically" by sliding a window through
 * a trace and averaging the samples within the window using a Hanning
 * function (0.5 - 0.5 * cos(t)), where the middle sample has a weight of 1
 * and the samples at either end of the window have weights of 0.  The
 * weighted average sample is then copied into the trace.  There is no range
 * testing here when converting from double to lesser types, because the
 * average of a group of values (with weights <=1) will always be in the
 * correct range.
 *
 * Parameters:
 *  long num_cols   - number of columns in grid
 *  long num_rows   - number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long half_length - number of samples to be used for one side of the sliding
 *                    window
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * Requires: "assertjl.h", <float.h>, <math.h>, <stdlib.h>,"gpr_dfx.h".
 * Calls: malloc, free, assert, cos.
 * Returns: 0 on success, or
 *         >0 on error.
 *
const char *SmoothGridVerticallyMsg[] =
{   "SmoothGridVertically(): No errors.",
	"SmoothGridVertically() ERROR: Invalid function arguments.",
	"SmoothGridVertically() ERROR: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int SmoothGridVertically (long num_cols,long num_rows,long first_samp,
						  int header_samps,int datatype,long half_length,
                          void **grid)
{
	long i,trace,samp,count; /* counters */
	long itemp1,mid_queue;   /* scratch variables */
	long q_index;            /* tracks which sample to replace next in the queue */
	long next_samp;          /* tracks which sample to get next from the trace */
	long win_length;         /* size of circular queue */
	double weight;           /* scratch variables */
	double total_weight;     /* sum of Hanning weights */
	double step;             /* increment between Hanning coefficients */
	double wgt_avg;          /* weighted average sample of window */
	double *queue   = NULL;  /* circular queue to hold currently used samples */
	double *weights = NULL;  /* hanning weighting coefficients */
	static double twopi = 2 * 3.14159265358979323846;

	/* Validate function argument list */
	if ( (grid == NULL)   || (num_cols == 0) || (num_rows == 0) ||
		 (half_length < 2) || half_length >= num_rows/2 || first_samp <= 0 ||
         header_samps < 0)
		return 1;

	/* Allocate storage for work arrays */
	win_length = 2 * half_length + 1;
	queue = (double *)malloc(win_length*sizeof(double));
	if (queue == NULL)  return 2;
	weights = (double *)malloc(win_length * sizeof(double));
	if (weights == NULL)
	{   free(queue);
		return 2;
	}

	/* Pre-calculate weights for hanning window */
	weight = total_weight = 0.0;
	step   = twopi/(double)(win_length + 1);
	for (i=0; i<win_length; i++)
	{   weight += step;
		weights[i] = 0.5 - 0.5*cos(weight);   /* Hanning window */
		total_weight += weights[i];
	}

	/* Perform the sliding window weighted vertical smoothing */
	switch (datatype)
	{   case -1:
		{   unsigned char **uc_pp = (unsigned char **)grid;

			/* Work through grid trace by trace */
			for (trace=0; trace<num_cols; trace++)
			{   /* initialize queue: can only pull in half because starting at
				   index=0, so average second half of queue and assign to all
				   first half values (this does not change average for queue).
				 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = uc_pp[trace][samp];
                    wgt_avg += uc_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    uc_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = uc_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case 1:
        {   char **c_pp = (char **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = c_pp[trace][samp];
                    wgt_avg += c_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    c_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = c_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case -2:  case -5:
        {   unsigned short **us_pp = (unsigned short **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = us_pp[trace][samp];
                    wgt_avg += us_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    us_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = us_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = s_pp[trace][samp];
                    wgt_avg += s_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    s_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = s_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case -3:  case -6:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = ul_pp[trace][samp];
                    wgt_avg += ul_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    ul_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = ul_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = l_pp[trace][samp];
                    wgt_avg += l_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    l_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = l_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = f_pp[trace][samp];
                    wgt_avg += f_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    f_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = f_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
        case 8:
        {   double **d_pp = (double **)grid;

            /* Work through grid trace by trace */
            for (trace=0; trace<num_cols; trace++)
            {   /* initialize queue: can only pull in half because starting at
                   index=0, so average second half of queue and assign to all
                   first half values (this does not change average for queue).
                 */
                /* initialize values */
                next_samp = header_samps+first_samp + ((win_length-1)/2)+1; /* next sample to get from trace */
                mid_queue = half_length;          /* middle sample in queue */

                /* copy values from trace into second half of queue; sum into avg */
                wgt_avg = 0.0;
                itemp1  = mid_queue;              /* offset into queue */
                count = 0;
                for (samp=header_samps+first_samp; samp<next_samp; samp++)
                {   /* here's a good place for debug check of array limits */
                    assert( (samp < num_rows) && (itemp1 < win_length) );
                    queue[itemp1] = d_pp[trace][samp];
                    wgt_avg += d_pp[trace][samp];
                    count++;
                    itemp1++;
                }

                /* calculate average for second half of queue */
                wgt_avg /= count;

                /* copy average values into first half of queue */
                for (itemp1=0; itemp1<mid_queue; itemp1++)
                    queue[itemp1] = wgt_avg;

                /* initialize index of sample in queue to be replaced next
                   with new sample */
                q_index = 0;

                /* calculate weighted average of all samples in the queue */
                wgt_avg = 0.0;
                itemp1 = q_index;
                for (i=0; i<win_length; i++)
                {   /* here's a good place for debug check of array limits */
                    assert(itemp1 < win_length);
                    wgt_avg += weights[i] * queue[itemp1];
                    itemp1++;
                    if (itemp1 >= win_length) itemp1 = 0;
                }
                wgt_avg /= total_weight;

                /* Work through trace, sliding window along, updating the
                   queue and the average sample value.  When window slides
                   past end of trace, update queue with current average trace
                   rather than (non-existent) next trace sample.
                 */
                for (samp=header_samps+first_samp; samp<num_rows; samp++)
                {   /* copy weighted average to current sample in trace */
                    d_pp[trace][samp] = wgt_avg;

                    /* copy next trace sample into the queue */
                    if (next_samp < num_rows)
                        queue[q_index] = d_pp[trace][next_samp];
                    else
                        queue[q_index] = wgt_avg;

                    /* calculate weighted average of traces in the queue */
                    wgt_avg = 0.0;
                    itemp1 = q_index;
                    for (i=0; i<win_length; i++)
                    {   /* here's a good place for debug check of array limits */
                        assert(itemp1 < win_length);
                        wgt_avg += weights[i] * queue[itemp1];
                        itemp1++;
                        if (itemp1 >= win_length) itemp1 = 0;
                    }
                    wgt_avg /= total_weight;

                    /* update q_index and next_samp */
                    q_index++;
                    if (q_index >= win_length) q_index = 0;
                    next_samp++;      /* OK for next_samp to be >= num_rows */
                }
            }
            break;
        }
    }

    /* Deallocate storage and return success */
    free(queue);
    free(weights);
    return 0;
}
/******************************** StackGrid() *******************************/
/* Stack specified number of traces in the grid into a new average trace.
 * num_cols is reduced and columns no longer need in the grid are freed.
 *
 * Parameters:
 *  long *num_cols  - address of variable, number of columns in grid
 *  long *num_rows  - address of variable, number of rows in grid
 *  long first_samp - do not change values before this sample
 *	int header_samps - number of "samples" in trace header
 *  int datatype - type of grid data elements
 *                 if ==  1, 1-byte chars;          == -1, unsigned chars
 *                    ==  2, 2-byte ints or shorts; == -2, unsigned shorts
 *                    ==  3, 4-byte ints or longs;  == -3, unsigned longs
 *                    ==  4, 4-byte floats
 *                    == -5, unsigned shorts but only first 12-bits used
 *                    == -6, unsigned longs but only first 24-bits used
 *                    ==  8, 8-byte doubles
 *  long stack   - number of traces to stack
 *  void **grid  - pointer to grid storage areas; [num_cols][num_rows]
 *
 * Requires: <stdlib.h>, "gpr_dfx.h".
 * Calls: free.
 * Returns: 0 on success, or
 *         >0 on error.
 *
const char *StackGridMsg[] =
{   "StackGrid(): No errors.",
	"StackGrid() ERROR: Invalid function arguments.",
} ;
 *
 * Author: Jeff Lucius   USGS   
 * Date: February 5, 1996
 * Revisions:  September 4, 1997
 */
int StackGrid (long *num_cols,long *num_rows,long first_samp,
			   int header_samps,int datatype,long stack,void **grid)
{
	long trace,samp,col,grid_col,next_col,num_pts;
	double dtemp;

	/* Reality check */
	if (stack <= 1) return 0;
	if (first_samp < 0 || grid == NULL || num_cols == NULL || num_rows == NULL ||
        header_samps < 0)
		return 1;

	/* stack the data */
	switch (datatype)
	{   case -1:
        {   unsigned char **uc_pp = (unsigned char **)grid;

			grid_col = -1;
			for (trace=0; trace< *num_cols; trace+=stack)
            {   grid_col++;
                next_col = trace + stack;
                if (next_col > *num_cols) next_col = *num_cols;
				num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = uc_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += uc_pp[col][samp];
                    }
                    uc_pp[grid_col][samp] = dtemp/num_pts;
                }
			}
            break;
        }
        case  1:
        {   char **c_pp = (char **)grid;

			grid_col = -1;
			for (trace=0; trace< *num_cols; trace+=stack)
			{   grid_col++;
				next_col = trace + stack;
				if (next_col > *num_cols) next_col = *num_cols;
				num_pts = next_col - trace;
				for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = c_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += c_pp[col][samp];
                    }
                    c_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
        case -2:
        {   unsigned short **us_pp = (unsigned short **)grid;

            grid_col = -1;
			for (trace=0; trace< *num_cols; trace+=stack)
            {   grid_col++;
				next_col = trace + stack;
                if (next_col > *num_cols) next_col = *num_cols;
                num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = us_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += us_pp[col][samp];
                    }
                    us_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
        case 2:
        {   short **s_pp = (short **)grid;

            grid_col = -1;
            for (trace=0; trace< *num_cols; trace+=stack)
			{   grid_col++;
                next_col = trace + stack;
				if (next_col > *num_cols) next_col = *num_cols;
                num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = s_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += s_pp[col][samp];
                    }
                    s_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
        case -3:
        {   unsigned long **ul_pp = (unsigned long **)grid;

            grid_col = -1;
            for (trace=0; trace< *num_cols; trace+=stack)
            {   grid_col++;
				next_col = trace + stack;
                if (next_col > *num_cols) next_col = *num_cols;
                num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = ul_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += ul_pp[col][samp];
                    }
                    ul_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
        case 3:
        {   long **l_pp = (long **)grid;

            grid_col = -1;
            for (trace=0; trace< *num_cols; trace+=stack)
            {   grid_col++;
                next_col = trace + stack;
				if (next_col > *num_cols) next_col = *num_cols;
                num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = l_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += l_pp[col][samp];
                    }
                    l_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
        case 4:
        {   float **f_pp = (float **)grid;

            grid_col = -1;
            for (trace=0; trace< *num_cols; trace+=stack)
            {   grid_col++;
                next_col = trace + stack;
                if (next_col > *num_cols) next_col = *num_cols;
				num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = f_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += f_pp[col][samp];
                    }
                    f_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
        case 8:
        {   double **d_pp = (double **)grid;

            grid_col = -1;
            for (trace=0; trace< *num_cols; trace+=stack)
            {   grid_col++;
                next_col = trace + stack;
                if (next_col > *num_cols) next_col = *num_cols;
                num_pts = next_col - trace;
                for (samp=header_samps+first_samp; samp< *num_rows; samp++)
                {   dtemp = d_pp[trace][samp];
                    for (col=trace+1; col<next_col; col++)
                    {   dtemp += d_pp[col][samp];
                    }
                    d_pp[grid_col][samp] = dtemp/num_pts;
                }
            }
            break;
        }
    }

    /* Delete "extra" columns */
    for (col=grid_col+1; col< *num_cols; col++)
    {   free(grid[col]);
        grid[col] = NULL;
    }

    /* Change number of columns */
    *num_cols = grid_col + 1;

	return 0;
}
/******************************** ucCompare() *******************************/
/* Compare 2 unsigned char values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS   
 * Date: November 16, 1994
 */
static int ucCompare(const void *elem1, const void *elem2)
{   if      (*(unsigned char *)elem1  < *(unsigned char *)elem2)
        return -1;
    else if (*(unsigned char *)elem1 == *(unsigned char *)elem2)
        return  0;
    else
        return  1;
}
/********************************* cCompare() *******************************/
/* Compare 2 char values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS   
 * Date: November 16, 1994
 */
static int cCompare(const void *elem1, const void *elem2)
{   if      (*(char *)elem1  < *(char *)elem2)
        return -1;
    else if (*(char *)elem1 == *(char *)elem2)
        return  0;
    else
        return  1;
}
/******************************** usCompare() *******************************/
/* Compare 2 unsigned short values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS
 * Date: November 16, 1994
 */
static int usCompare(const void *elem1, const void *elem2)
{   if      (*(unsigned short *)elem1  < *(unsigned short *)elem2)
        return -1;
    else if (*(unsigned short *)elem1 == *(unsigned short *)elem2)
        return  0;
    else
        return  1;
}
/********************************* sCompare() *******************************/
/* Compare 2 short values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS   
 * Date: November 16, 1994
 */
static int sCompare(const void *elem1, const void *elem2)
{   if      (*(short *)elem1  < *(short *)elem2)
        return -1;
    else if (*(short *)elem1 == *(short *)elem2)
        return  0;
    else
        return  1;
}
/******************************** ulCompare() *******************************/
/* Compare 2 unsigned long values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS   
 * Date: November 16, 1994
 */
static int ulCompare(const void *elem1, const void *elem2)
{   if      (*(unsigned long *)elem1  < *(unsigned long *)elem2)
        return -1;
    else if (*(unsigned long *)elem1 == *(unsigned long *)elem2)
        return  0;
    else
        return  1;
}
/********************************* lCompare() *******************************/
/* Compare 2 long values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS   
 * Date: November 16, 1994
 */
static int lCompare(const void *elem1, const void *elem2)
{   if      (*(long *)elem1  < *(long *)elem2)
        return -1;
    else if (*(long *)elem1 == *(long *)elem2)
        return  0;
    else
        return  1;
}
/********************************* fCompare() *******************************/
/* Compare 2 float values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS   
 * Date: November 16, 1994
 */
static int fCompare(const void *elem1, const void *elem2)
{   if      (*(float *)elem1  < *(float *)elem2)
        return -1;
    else if (*(float *)elem1 == *(float *)elem2)
        return  0;
    else
        return  1;
}
/********************************* dCompare() *******************************/
/* Compare 2 double values and return result.
 *
 * Parameters:
 *  const void *elem1 - pointer to first value
 *  const void *elem2 - pointer to second value
 *
 * Requires: none.
 * Calls: none.
 * Returns: int: -1 if elem1  < elem2
 *                0 if elem1 == elem2
 *                1 if elem1  > elem2
 *
 * Author: Jeff Lucius   USGS
 * Date: November 16, 1994
 */
static int dCompare(const void *elem1, const void *elem2)
{   if      (*(double *)elem1  < *(double *)elem2)
        return -1;
    else if (*(double *)elem1 == *(double *)elem2)
        return  0;
    else
        return  1;
}
/***************************** end of GPR_DFX.C *****************************/
