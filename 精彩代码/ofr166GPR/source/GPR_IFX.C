/* GPR_IFX.C - source code for functions in GPR_IFX.LIB
 *
 * Jeff Lucius
 * Geophysicist
 * U.S. Geological Survey
 * Geologic Division, Branch of Geophysics
 * Box 25046  Denver Federal Center  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@musette.cr.usgs.gov
 *
 * To compile for use in GPR_IFX.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c GPR_IFX.C
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /c compiles only, no linking.
 *
 * Functions in this module:
 *   BrightenTable8
 *   InitTable8
 *   LocalStretchTable8
 *   ReverseTable8
 *   SquareTable8
 */

/* Include header files */
#include "gpr_ifx.h"

/* Local function prototypes */
static char *TrimStr(char *p);

/***************************** BrightenTable8() *****************************/
/* Brighten or darken a look-up table for 8-bit grayscale display that has
 * been previously initialized.
 *
 * Parameter list:
 *  int *table     - array to be used for grayscale
 *  int tablesize  - number of members in table[]; should be 256 for this function
 *  int brightness - amount to brighten or darken table values; range +/- 128
 *
 * Requires : <assertjl.h>, <stdio.h>, "jl_defs.h".
 * Calls: assert, _MAXX, _MINN, puts, getch.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: April 27, 1994
 */
void BrightenTable8 (int *table,int tablesize,int brightness)
{
    int i;
    extern int Debug;

	assert((table != NULL) && (tablesize > 0));
    for (i=0; i<tablesize; i++)
        table[i] = _MAXX(_MINN(table[i]+brightness,255),0);

    if (Debug)
	{   puts("Brighten Table[] = ");
		for (i=0; i<tablesize; i++) printf("%d ",table[i]);
        puts("");
		getch();
    }
}
/******************************* InitTable8() *******************************/
/* Initialize a simple look-up table for 8-bit grayscale display if range > 0.
 * If range is = 0, then the display will look similar to EPC graphic
 * recorder output.
 *
 * Parameter list:
 *  int *table      - array to be used for grayscale
 *  int tablesize   - number of members in table[]; must be 256 for this function
 *  int range       - if >0 then squeeze grayscale if <255 of stretch if >255
 *                    if  0 then use EPC style dipslay
 *    parameters below are used if range==0
 *  int threshold   - EPC-style display, distance from midpoint to start darkening
 *                       (band of white at midpoint is wider if > 0)
 *  int contrast    - EPC-style display, maximum value for "black" (1 to 255)
 *  int sign        - EPC-style display, if >0, values below midpoint are white
 *                       if <0, values values above midpoint are white
 *  int gain        - EPC-style display, multiplies contrast ("black" occurs
 *                       closer to midpoint)
 *
 * Example 1 enhanced contrast: range = 128, tablesize = 256.
 *  table[]:         |                          |
 *  index: 0 1 .... 63 64 65 66 .......... 190 191 192 193 ...... 254 255
 *  value: 0 0 ....  0  1  3  5 .......... 253 255 255 255 ...... 255 255
 *                   |                          |
 *
 * Example 2 diminished contrast: range = 512, tablesize = 256.
 *  table[]:
 *  index:  0  1  2  3  4  5  6 .... 248 249 250 251 252 253 254 255
 *  value: 63 63 63 64 64 65 65 .... 186 187 187 188 188 189 189 190
 *
 * Requires : <math.h>, <stdiio.h>, "jl_defs.h".
 * Calls: _MAXX, _MINN, _ABS, printf, getch, puts.
 * Returns: 0 on success,
 *         >0 if error.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: May 6, 1994
 */
void InitTable8 (int *table,int tablesize,int range,int threshold,
                int contrast,int sign,double gain)
{
    int i,rlow,rhigh,ptmax,graymax;
	int mid         = 127;
	int white       = 255;
    double scale,step,dtemp;
    extern int Debug;

	assert((table != NULL) && (tablesize == 256));
	/* range = _ABS(range); */
    if (range < 0) range = 255;
    if (range == 0)  /* setup table to look like EPC graphic recorder output */
    {   if (threshold < 1)   threshold = 1;
		if (threshold > 126) threshold = 126;
        if (Debug) puts("range == 0");
        for (i=0; i<threshold; i++)
        {   table[mid+i] = white;
			table[mid-i] = white;
        }
        if (gain < 1.0)   gain = 1.0;
        ptmax = _MAXX(threshold,_MINN(127,(int)((double)(mid-threshold)/gain)));
        if (contrast < 1)   contrast = 1;
        if (contrast > 255) contrast = 255;
		graymax = 255-contrast;
		step = (double)(contrast)/ptmax;
        for (i=threshold,dtemp=step; i<threshold+ptmax; i++,dtemp+=step)
        {   table[mid+i] = 255.0 - dtemp;
            table[mid-i] = 255.0 - dtemp;
		}
		for (i=threshold+ptmax; i<=127; i++)
        {   table[mid+i] = graymax;
            table[mid-i] = graymax;
		}
        table[255] = graymax;
        if (sign > 0)  for (i=0; i<127; i++)          table[i] = white;
		if (sign < 0)  for (i=128; i<tablesize; i++)  table[i] = white;
    } else if (range == 255)
	{   for (i=0; i<tablesize; i++) table[i] = i;
        if (Debug) puts("range == 255");
    } else
    {   rlow  = _MAXX(0,(mid - range/2));
        rhigh = _MINN(255,(rlow + range));
        for (i=0; i<rlow; i++)            table[i] = 0;
		for (i=rhigh+1; i<tablesize; i++) table[i] = 255;
		scale = (double)(tablesize-1)/(double)range;
        if (scale >= 1.0) mid = 0;              /* midpoint stays at 127 */
        else              mid -= rhigh*scale/2; /* starting value in expanded scale */
        if (Debug)
			printf("range=%d  rlow=%d  rhigh=%d  scale=%g  mid=%d\n",
				range,rlow,rhigh,scale,mid);
        for (i=rlow; i<=rhigh; i++)
            table[i] = (i-rlow)*scale + mid;
	}
    if (Debug)
    {   puts("Gray Scale Table[] = ");
		for (i=0; i<tablesize; i++) printf("%d ",table[i]);
        puts("");
		getch();
    }
}
/*************************** LocalStretchTable8() ***************************/
/* Enhance the contrast locally of a look-up table for 8-bit grayscale
 * display that has been previously initialized.
 *
 * Parameter list:
 *  int *table    - array to be used for grayscale
 *  int tablesize - number of members in table[]; must be 256 for this function
 *  int offset    - number of values of either side of midpoint to strech
 *  int change    - amount to increase or decrease the values
 *
 *                              * -- original table values
 *                        +  *
 *                    +   *
 *                +    *
 *                  *
 *              +*
 *            *
 *         *  + - locally stretched values
 *      * +
 *   *
 *
 * Requires : <assert.h>, <stdio.h>, "jl_defs.h".
 * Calls: assert, _MAXX, _MINN, printf, getch.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: September 15, 1995
 */
void LocalStretchTable8 (int *table,int tablesize,double offset,double change)
{
	int i;
	int lo_index,hi_index;
	double step,lo_val,hi_val,new_lo_val,new_hi_val,new_val;
    extern int Debug;

    if (change == 0.0 || offset == 0.0) return;
	assert((table != NULL) && (tablesize == 256));

	lo_index = (tablesize/2) - offset;
	hi_index = (tablesize/2) + offset;
    lo_val = table[lo_index];
	hi_val = table[hi_index];
    new_lo_val = _MAXX(_MINN(lo_val-change,128.0),0.0);
    new_hi_val = _MAXX(_MINN(hi_val+change,255.0),128.0);
	if (Debug) printf("lo_index=%d  hi_index=%d  new_lo_val=%g  new_hi_val=%g\n",
                      lo_index,hi_index,new_lo_val,new_hi_val);
    new_val = table[0];
    step = (new_lo_val - new_val) / lo_index;
    if (Debug) printf("1st step = %g\n",step);
	for (i=0; i<lo_index; i++)
    {   table[i] = _MAXX(_MINN(new_val,new_lo_val),table[0]);
        new_val += step;
	}
    step = (new_hi_val - new_lo_val) / (hi_index - lo_index);
    if (Debug) printf("2nd step = %g\n",step);
    new_val = new_lo_val;
    for (i=lo_index; i<=hi_index; i++)
    {   table[i] = _MAXX(_MINN(new_val,new_hi_val),new_lo_val);
        new_val += step;
    }
	step = (table[tablesize-1] - new_hi_val) / (tablesize-1 - hi_index+1);
	if (Debug) printf("3rd step = %g\n",step);
    new_val = new_hi_val;
	for (i=hi_index+1; i<tablesize; i++)
	{   table[i] = _MAXX(_MINN(new_val,table[tablesize-1]),new_hi_val);
        new_val += step;
    }

    if (Debug)
	{   puts("Local Stretch Table[] = ");
		for (i=0; i<tablesize; i++) printf("%d ",table[i]);
        puts("");
		getch();
    }
}
/****************************** ReverseTable8() *****************************/
/* Reverse a look-up table for 8-bit grayscale display that has
 * been previously initialized.
 *
 * Parameter list:
 *  int *table     - array to be used for grayscale
 *  int tablesize  - number of members in table[]; should be 256 for this function
 *
 * Requires : <assert.h>, <stdio.h>, "jl_defs.h".
 * Calls: assert, _MAXX, _MINN, printf, getch.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: April 27, 1994
 */
void ReverseTable8 (int *table,int tablesize)
{
    int i,j,itemp;
    extern int Debug;

	assert((table != NULL) && (tablesize > 0));
	for (i=0,j=tablesize-1; i<(tablesize/2); i++,j--)
	{   itemp    = table[i];
        table[i] = table[j];
        table[j] = itemp;
    }
    if (Debug)
    {   puts("Reverse Table[] = ");
		for (i=0; i<tablesize; i++) printf("%d ",table[i]);
        puts("");
		getch();
    }
}
/****************************** SquareTable8() ******************************/
/* Square then rescale look-up table for 8-bit grayscale display that has
 * been previously initialized.
 *
 * Parameter list:
 *  int *table     - array to be used for grayscale
 *  int tablesize  - number of members in table[]; should be 256 for this function
 *
 * Requires : <assert.h>, <stdio.h>, "jl_defs.h".
 * Calls: assert, _MAXX, _MINN, printf, getch.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: June 22, 1994
 */
void SquareTable8 (int *table,int tablesize)
{
    int i,sign;
    extern int Debug;

	assert((table != NULL) && (tablesize > 0));
	for (i=0; i<tablesize; i++)
    {   sign = 1;
        table[i] -= 128;          /* set mean to 0 */
		if (table[i] < 0) sign = -1;
        table[i] *= table[i];     /* square the value */
        table[i] >>= 7;           /* divide by 128 */
		if (sign == -1) table[i] *= -1;
		table[i] = _MAXX(_MINN(table[i]+128,255),0);  /* restore to unsigned char range */
	}

	if (Debug)
	{   puts("Square Table[] = ");
		for (i=0; i<tablesize; i++) printf("%d ",table[i]);
        puts("");
		getch();
    }
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
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 3, 1994
 */
static char *TrimStr (char *p)
{
    int end,i,len,trail=0,lead=0;

    /* return if NULL string */
    if ((end=(len=strlen(p))) == 0)  return p;

	/* count blanks etc. at end */
	while ((end>0) && (p[--end]<0x21 || p[end]>0x7E))  trail++;

    /* strip off blanks etc. by inserting terminating 0 */
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
