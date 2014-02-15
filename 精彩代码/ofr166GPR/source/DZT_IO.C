/* DZT_IO.C - source code for functions in GPR_IO.LIB
 *
 * Jeff Lucius
 * U.S. Geological Survey
 * Box 25046  DFC  MS 964
 * Denver, CO  80225-0046
 * 303-236-1413 (office), 303-236-1212 (secretary), 303-236-1425 (fax)
 * email: lucius@musette.cr.usgs.gov
 *
 * To compile for use in GPR_IO.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c DZT_IO.C
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /c compiles only, no linking.
 *
 * Functions in this module:
 *   ConvertProcHist2
 *   EditDztHeader
 *   GetDztChSubGrid8
 *   GetDztChSubGrid16
 *   GetDztChSubImage8
 *   GetDztFile
 *   PrintOneDztHeader
 *   ReadOneDztHeader
 *   SaveDztFile
 *   SetDztHeader
 *   SetDzt5xHeader
 *
 * Revision: March 17, 2000; April 11, 2000;
 */

/* Include header files */
#include "gpr_io.h"

/* Declare globals */
const char *month_abbr[] =
{   "no date",    /* 0 */
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", /* 1-12 */
	"error",NULL, /* 13-14 */
} ;
const char *ant_number[] =
{   "0000",
	"3112","3224","3207","3020","3110","3105",
    "3205","3102","3101","3100","4208","4205",
    "5103","5106",NULL,
} ;
const char *ant_name[] =
{   "unknown",
	"80 MHz","15/45/80 MHz","100 MHz","120 MHz","120 MHz","300 MHz",
	"300 MHz","500 MHz","900 MHz","1000 MHz","1 GHz","2.5 GHz",
    "400 MHz","200 MHz",NULL,
} ;
const char *GetDztChSubGrid8Msg[] =
{   "GetDztChSubGrid8(): No error.",
	"GetDztChSubGrid8() Error: fopen() failed.",
	"GetDztChSubGrid8() Error: Short count from fread().",
	"GetDztChSubGrid8() Error: Inconsistency -- first scan sample not 0xFF.",
	"GetDztChSubGrid8() Error: Number scan samples exceed number of grid rows.",
	"GetDztChSubGrid8() Error: EOF encountered -- grid cols exceed number of scans.",
	"GetDztChSubGrid8() Error: Unable to allocate temporary storage.",
	"GetDztChSubGrid8() Error: Data do not consist of 8-bit samples.",
	"GetDztChSubGrid8() Error: Error value returned by ReadFirstDztHeader().",
	"GetDztChSubGrid8() Error: Invalid channel selected.",
    "GetDztChSubGrid8() Error: Number traces exceeds number of grid columns.",
} ;
const char *GetDztChSubGrid16Msg[] =
{   "GetDztChSubGrid16(): No error.",
	"GetDztChSubGrid16() Error: fopen() failed.",
	"GetDztChSubGrid16() Error: Short count from fread().",
	"GetDztChSubGrid16() Error: Inconsistency -- first scan sample not 0xFFFF.",
	"GetDztChSubGrid16() Error: Number scan samples exceed number of grid rows.",
	"GetDztChSubGrid16() Error: EOF encountered -- grid cols exceed number of scans.",
	"GetDztChSubGrid16() Error: Unable to allocate temporary storage.",
	"GetDztChSubGrid16() Error: Data do not consist of 16-bit samples.",
	"GetDztChSubGrid16() Error: Error value returned by ReadFirstDztHeader().",
	"GetDztChSubGrid16() Error: Invalid channel selected.",
    "GetDztChSubGrid16() Error: Number traces exceeds number of grid columns.",
} ;
const char *GetDztChSubImage8Msg[] =
{   "GetDztChSubImage8(): No error.",
	"GetDztChSubImage8() Error: Unable to open input DZT file.",
	"GetDztChSubImage8() Error: Short count from fread().",
	"GetDztChSubImage8() Error: Inconsistency -- first trace sample not 0xFFFF.",
	"GetDztChSubImage8() Error: requested trace samples exceed number of image rows.",
	"GetDztChSubImage8() Error: EOF encountered; image cols exceed number of traces.",
	"GetDztChSubImage8() Error: Unable to allocate temporary storage.",
	"GetDztChSubImage8() Error: Datatype not 8, 16, or 32 bits in size.",
	"GetDztChSubImage8() Error: Invalid function argument.",
	"GetDztChSubImage8() Error: Requested channel not present in file.",
	"GetDztChSubImage8() Error: Error value returned by ReadFirstDztHeader().",
} ;
const char *PrintOneDztHeaderMsg[] =
{   "PrintOneDztHeader(): No errors.",
	"PrintOneDztHeader(): ERROR: Unable to assign/open output stream.",
} ;
const char *ReadOneDztHeaderMsg[] =
{   "ReadOneDztHeader(): No errors.",
    "ReadOneDztHeader() ERROR: Function argument list contains a NULL pointer.",
    "ReadOneDztHeader() ERROR: fopen() failed.",
    "ReadOneDztHeader() ERROR: Short count from fread().",
	"ReadOneDztHeader() ERROR: Invalid channel selected.",

	"ReadOneDztHeader() ERROR: Invalid DZT header format - rh_bits not 8,16,32.",
	"ReadOneDztHeader() ERROR: Invalid DZT header format - rh_nsamp not 2**i.",
	"ReadOneDztHeader() ERROR: Invalid DZT header format - rh_nchan not 1,2,3,4.",
	"ReadOneDztHeader() ERROR: Invalid DZT header format - last 50 bytes != 0.",
	"ReadOneDztHeader() WARNING: Header ID is incorrect (not 0x0nFF).",

	"ReadOneDztHeader() WARNING: Possible header corruption -- checksum is wrong.",
	"ReadOneDztHeader() WARNING: Possible modified DZT header - rh_nsamp not 2**i.",
	"ReadOneDztHeader() WARNING: rh_nsamp not 2**i and header ID is incorrect.",
	"ReadOneDztHeader() WARNING: rh_nsamp not 2**i and checksum is wrong.",
	"ReadOneDztHeader() WARNING: header ID is incorrect and checksum is wrong.",

	"ReadOneDztHeader() WARNING: rh_nsamp not 2**i, header ID incorrect, checksum wrong.",
} ;
const char *SaveDztFileMsg[] =
{   "SaveDztFile(): No errors.",
	"SaveDztFile() Error: Function argument list contains a NULL pointer.",
	"SaveDztFile() Error: Function argument list contains an invalid value.",
	"SaveDztFile() Error: Problem opening output file.",
	"SaveDztFile() Error: Short count from fwrite saving header(s).",
	"SaveDztFile() Error: Short count from fwrite saving data.",
	"SaveDztFile() Error: Unable to allocate temporary storage.",
} ;
/* The following strings are used by function ConvertProcHist()2 */
const char *ph_viirl = "\tVert. IIR low pass  N=%d F=%g\n";
const char *ph_viirh = "\tVert. IIR high pass N=%d F=%g\n";
const char *ph_vfirl = "\tVert. FIR low pass  N=%d F=%g\n";
const char *ph_vfirh = "\tVert. FIR high pass N=%d F=%g\n";
const char *ph_vboxl = "\tVert. boxcar low pass    N=%d\n";
const char *ph_vboxh = "\tVert. boxcar high pass   N=%d\n";
const char *ph_vtril = "\tVert. triangle low pass  N=%d\n";
const char *ph_vtrih = "\tVert. triangle high pass N=%d\n";
const char *ph_vtcl  = "\tVert. low pass  TC=%g\n";
const char *ph_vtch  = "\tVert. high pass TC=%g\n";
const char *ph_hiirl = "\tHoriz. IIR low pass  N=%d F=%g\n";
const char *ph_hiirh = "\tHoriz. IIR high pass N=%d F=%g\n";
const char *ph_hfirl = "\tHoriz. FIR low pass  N=%d F=%g\n";
const char *ph_hfirh = "\tHoriz. FIR high pass N=%d F=%g\n";
const char *ph_hboxl = "\tHoriz. boxcar stacking   N=%d\n";
const char *ph_htril = "\tHoriz. triangle stacking N=%d\n";
const char *ph_htcl  = "\tHoriz. IIR stacking TC=%g\n";
const char *ph_hboxh = "\tHoriz. boxcar background removal N=%d\n";
const char *ph_htrih = "\tHoriz. triangle background removal N=%d\n";
const char *ph_htch  = "\tHoriz. IIR background removal TC=%g\n";
const char *ph_hsts  = "\tStatic stacking N=%d\n";

/**************************** GetDztChSubImage8() ****************************/
/* Read a section of GPR traces from a DZT file.  The traces are stored in an
 * unsigned char 2D image starting at first_samp and copying num_rows of
 * samples.  The image array is organized row by row.  Only 8-, 16-, or 32-bit
 * data can be used at this time (there is no 64 bit integer type in C at
 * present).  Short and long types are scaled down to character size.
 *
 * This program opens the file, moves past the header(s), moves the file
 * file pointer to the first trace to read, copies the section of traces from
 * the requested channel into the image, then closes the file.
 *
 * Note: This function differs from earlier versions in that it doesn't
 *       require the first sample of the first trace to have all bits set.
 *
 * Parameters:
 *  char *filename   - pointer to string, name of the DZT file
 *  int channel      - channel number, indexed from 0
 *  long first_samp  - first sample of each trace to copy, indexed from 0
 *  long first_trace - first trace to copy, indexed from 0
 *  long last_trace  - last trace to copy, indexed from 0
 *  long skip_traces - number of traces to skip for every one read in
 *  long num_cols    - number of columns (traces) in image[][]
 *  long num_rows    - number of rows (samples) in image[][]
 *  unsigned char **image - [num_rows][num_cols], pointer to 2D array
 *
 * NOTE: Storage for filename[] and image[][] must have been previously
 *       allocated.
 *
 * Requires: <stdio.h>, <stdlib.h>, "gpr_io.h".
 * Calls: fopen, setvbuf, fclose, fseek, fread, malloc, free, feof,
 *        ReadOneDztHeader.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *GetDztChSubImage8Msg[] =
{   "GetDztChSubImage8(): No error.",
    "GetDztChSubImage8() Error: Unable to open input DZT file.",
    "GetDztChSubImage8() Error: Short count from fread().",
    "GetDztChSubImage8() Error: Inconsistency -- first trace sample not 0xFFFF.",
    "GetDztChSubImage8() Error: requested trace samples exceed number of image rows.",
*5* "GetDztChSubImage8() Error: EOF encountered; image cols exceed number of traces.",
    "GetDztChSubImage8() Error: Unable to allocate temporary storage.",
    "GetDztChSubImage8() Error: Datatype not 8, 16, or 32 bits in size.",
    "GetDztChSubImage8() Error: Invalid function argument.",
    "GetDztChSubImage8() Error: Requested channel not present in file.",
*10*"GetDztChSubImage8() Error: Error value returned by ReadFirstDztHeader().",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: June 19, 1996
 */
int GetDztChSubImage8 (char *filename,int channel,long first_samp,
        long first_trace,long last_trace,long skip_traces,long num_cols,
        long num_rows,unsigned char **image)
{
    int  error;               /* scratch variables and counters */
    int  header_size,num_hdrs;
    long traces_read;         /* number of traces read from file */
    long num_traces;          /* number of traces per channel */
    long col,row,samp,ltemp;  /* scratch variables and counters */
    long disk_trace_size;     /* number of bytes in one trace */
    long disk_chunk_size;     /* number of bytes in one chunk of traces */
    long offset;              /* offset in bytes to start of requested trace
                                 in multichannel files */
    void *trace_chunk = NULL; /* hold one set of scans in multichannel files */
    unsigned char  *ucp;      /* will point to offset in trace_chunk */
    unsigned short *usp;      /* will point to offset in trace_chunk */
    unsigned long  *ulp;      /* will point to offset in trace_chunk */
    FILE *infile;
    struct DztHdrStruct hdr;
    #if defined(_INTELC32_)
      char *buffer = NULL;
      size_t vbufsize = 64512;  /* 63 KB */
    #endif
    extern int Debug;

    /* Verify function arguments */
    if (filename == NULL || image == NULL || channel < 0 || num_cols < 1 ||
        num_rows < 1) return 8;
    if (skip_traces < 0) skip_traces = 0;

    /* Get the first header from the file */
    error = ReadOneDztHeader(filename,&num_hdrs,&num_traces,channel,&header_size,&hdr);
    if (error)
    {   if (Debug) printf("ReadOneDztHeader() returned %d\n",error);
        if (error < 9)  return 10;
    }

    /* Verify DZT file contains a data type we can handle */
    switch (hdr.rh_bits)
    {   case 8: case 16: case 32:      break;
        default:                       return 7;
    }

    /* Verify requested channel is present */
    if (channel >= hdr.rh_nchan)  return 9;  /* rh_nchan indexed from 1 */

    /* Verify last scan sample does not exceed number of rows in the grid */
    if ((first_samp + num_rows - 1) >= hdr.rh_nsamp)  return 4;

    /* Open DZT file */
    if ((infile = fopen(filename,"rb")) == NULL) return 1;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(buffer,vbufsize);
      if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
      else         setvbuf(infile,NULL,_IOFBF,vbufsize);
    #endif

    /* Allocate storage for one chunk of scans */
    disk_trace_size = hdr.rh_nsamp * (hdr.rh_bits/8);   /* 8 bits per byte */
    disk_chunk_size = hdr.rh_nchan * disk_trace_size;
    trace_chunk = (void *)malloc((size_t)disk_chunk_size);
    if (trace_chunk == NULL)
    {   fclose(infile);
        #if defined(_INTELC32_)
          realfree(buffer);
        #endif
        return 6;
    }

    /* Determine offset to channel 0 of "first_trace" and move file pointer */
    offset = (hdr.rh_nchan * header_size) + (first_trace * disk_chunk_size);
    fseek(infile,offset,SEEK_SET);

    /* Get requested traces from file one at a time */
    offset = channel * hdr.rh_nsamp;             /* number of samples */
    traces_read = 0;   /* keep track of number of chunks sucesfully read in */
    for (col=0; col<num_cols; col++)
    {   /* reset error and get a group of traces */
        error = 0;
        if (fread(trace_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
        {   error = 2;
            if (feof(infile)) error = 5;
            if (Debug)
            {   printf("fread error - get trace loop. col=%ld error=%d traces_read=%ld\n",
                        col,error,traces_read);
                getch();
            }
            break;  /* out of for loop reading traces from file */
        }
        traces_read++;
        switch (hdr.rh_bits)
        {   case 8:
                ucp = (unsigned char *)trace_chunk;
                /* Copy selected channel into grid */
                ucp += offset;
                samp = first_samp;
                for (row=0; row<num_rows; row++,samp++)
                    image[row][col] = ucp[samp];
            break;
        case 16:
                usp = (unsigned short *)trace_chunk;
                /* Copy selected channel into grid */
                usp += offset;
				samp = first_samp;
                for (row=0; row<num_rows; row++,samp++)
                    image[row][col] = usp[samp]>>8;
            break;
        case 32:
                ulp = (unsigned long *)trace_chunk;
                /* Copy selected channel into grid */
                ulp += offset;
                samp = first_samp;
                for (row=0; row<num_rows; row++,samp++)
                    image[row][col] = ulp[samp]>>24;
            break;
        }   /* end of switch (hdr.rh_bits) block */

        /* skip traces if requested */
        for (ltemp = 0; ltemp<skip_traces; ltemp++)
        {   if (traces_read >= last_trace)
            {   if (Debug)
                {   printf("traces_read = %ld  last_trace = %ld\n",traces_read,last_trace);
                    getch();
                }
                break;  /* out of for loop getting skipped traces */
            } else
            {   if (fread(trace_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
                {   if (Debug)
                    {   printf("fread error - skip loop. col=%ld ltemp=%ld traces_read=%ld\n",
                                col,ltemp,traces_read);
                        getch();
                    }
                    if (feof(infile))  /* back up to get last trace */
                    {    /* should never get here ! */
                         rewind(infile);  /* clear EOF indicator */
                         fseek(infile,last_trace * disk_chunk_size,SEEK_SET);
                         if (Debug)
                         {  puts("File rewound and fseeked to (last_trace * disk_chunk_size)");
                            printf("EOF found - skip loop; skip counter=%ld ftell=%ld bytes traces_read=%ld\n",
                                     ltemp,ftell(infile),traces_read);
                             getch();
                         }
                    }
                    break;  /* out of for loop getting skipped traces */
                }
                traces_read++;
            }
        }   /* end of for loop reading skipped traces from file */
	}   /* end of for loop reading traces from file */

    /* Clean up and return */
    fclose(infile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    free(trace_chunk);
    return error;
}
/**************************** ReadOneDztHeader() ****************************/
/* Open a file, reads the selected DZT header, and then close the file.
 * This function differs from earlier versions in that it attempts to read
 * the header as a 512-byte old-style one if there is a format error trying
 * to read it as a newer 1024-byte header.
 *
 * Parameters:
 *  char *filename  - pointer to string containing DZT filename
 *  int *num_hdrs   - address of variable, will hold number of headers on return
 *  long *num_traces - address of variable, will hold number of GPR traces per
 *                   channel on return.  For multi-channel files, the total
 *                   number of traces in the file is num_traces * num_hdrs.
 *  int *header_bytes - 512 (old-style) or 1024 (new_style) on return
 *  int channel     - channel number to get header for, INDEXED FROM 0
 *  struct DztHdrStruct *hdrPtr - address of header structure
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <math.h>, <stdio.h>, <string.h>, "gpr_io.h", "assertjl.h".
 * Calls: assert, fopen, fclose, fread, fseek, ftell, memset, pow, rewind,
 *        printf, getch.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *ReadOneDztHeaderMsg[] =
{   "ReadOneDztHeader(): No errors.",
    "ReadOneDztHeader() ERROR: Function argument list contains a NULL pointer.",
    "ReadOneDztHeader() ERROR: fopen() failed.",
    "ReadOneDztHeader() ERROR: Short count from fread().",
	"ReadOneDztHeader() ERROR: Invalid channel selected.",

	"ReadOneDztHeader() ERROR: Invalid DZT header format - rh_bits not 8,16,32.",
	"ReadOneDztHeader() ERROR: Invalid DZT header format - rh_nsamp not 2**i.",
	"ReadOneDztHeader() ERROR: Invalid DZT header format - rh_nchan not 1,2,3,4.",
	"ReadOneDztHeader() ERROR: Invalid DZT header format - last 50 bytes != 0.",
	"ReadOneDztHeader() WARNING: Header ID is incorrect (not 0x0nFF).",

	"ReadOneDztHeader() WARNING: Possible header corruption -- checksum is wrong.",
	"ReadOneDztHeader() WARNING: Possible modified DZT header - rh_nsamp not 2**i.",
	"ReadOneDztHeader() WARNING: rh_nsamp not 2**i and header ID is incorrect.",
	"ReadOneDztHeader() WARNING: rh_nsamp not 2**i and checksum is wrong.",
	"ReadOneDztHeader() WARNING: header ID is incorrect and checksum is wrong.",

	"ReadOneDztHeader() WARNING: rh_nsamp not 2**i, header ID incorrect, checksum wrong.",
} ;
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: November 17, 1995;
 * Revisions: August 8, 1997  - allow number of samps to not be power of 2
 *            March 17, 2000  - func used to end with no return value!
 */
int ReadOneDztHeader (char *filename,int *num_hdrs,long *num_traces,int channel,
					  int *header_bytes,struct DztHdrStruct *hdrPtr)
{
	unsigned char uctemp;
	int i,error,error2, error3;
	unsigned short ustemp,checksum;
	long last_byte,num_bytes;
	FILE *infile = NULL;
	extern int Debug;

/***** Check for NULL pointers in parameter list *****/
	if (hdrPtr == NULL || filename == NULL || num_hdrs == NULL ||
		num_traces == NULL) return 1;

/***** Attemp to open input file *****/
	if ((infile = fopen(filename,"rb")) == NULL) return 2;

/***** Determine if a valid (recognized) DZT header *****/
	/* Get header as if 1024-bytes long */
	*header_bytes = sizeof(struct DztHdrStruct);  /* 1024 */
	assert(*header_bytes == 1024);                /* double check */
	if (fread((void *)hdrPtr,(size_t)*header_bytes,(size_t)1,infile) < 1)
	{   fclose(infile);
		return 3;
	}

	/* Check essential information */
	if ( (hdrPtr->rh_bits !=  8) && (hdrPtr->rh_bits != 16) &&
		 (hdrPtr->rh_bits != 32)
	   )
		{   fclose(infile) ;
			return 5;           /* sample size incorrect */
		}
	error = 11;
	for (i=1; i<=16; i++)
	{   if (hdrPtr->rh_nsamp == pow(2,(double)i))
		{   error = 0;
			break;   /* out of for loop */
		}
	}

	/* Check required information */
	*num_hdrs = hdrPtr->rh_nchan;
	if (*num_hdrs < 1 || *num_hdrs > 4)  /* as of 11/17/95 only 1 to 4 channels */
	{   fclose(infile);
		return 7;               /* invalid number of channels */
	}
	if (channel >= *num_hdrs || channel < 0)
	{   fclose(infile);
		return 4;               /* selected channel not there */
	}

/***** Determine if old (512-byte) or new (1024-byte) header *****/
	error2 = 0;
	/* is tag correct? */
	if (hdrPtr->rh_tag != 0x00FF) error2 = 9;

	/* calculate check sum */
	ustemp = hdrPtr->rh_chksum;
	hdrPtr->rh_chksum = 0;
	checksum = 0;
	for (i=0; i<512; i++)
		checksum += *((unsigned short *)hdrPtr + i);
	hdrPtr->rh_chksum = ustemp;

	/* is check sum correct? */
	error3 = 0;
	if (checksum != ustemp) error3 = 10;

	/* check last 50 bytes in header to see if all zeroes -- highly unlikely
	   that processing/comment area is full or that data will be all 0's
	 */
	fseek(infile,(long)(*num_hdrs * 1024) - 51L,SEEK_SET);
	for (i=0; i<50; i++)
	{   if (fread((void *)&uctemp,(size_t)1,(size_t)1,infile) < 1)
		{   fclose(infile);
			return 3;
		}
		if (uctemp != 0)
		{  *header_bytes = 512;
			fseek(infile,(long)(*num_hdrs * 512) - 51L,SEEK_SET);
			for (i=0; i<50; i++)
			{   if (fread((void *)&uctemp,(size_t)1,(size_t)1,infile) < 1)
				{   fclose(infile);
					return 3;
				}
				if (uctemp != 0)
				{   fclose(infile);
					return 8;
				}
		   }
		   break;  /* out of for loop */
		}
	}

/***** Clear buffer and get selected header *****/
	if (channel > 0)
	{   memset((void *)hdrPtr,0x00,sizeof(struct DztHdrStruct));
		fseek(infile,(long)(channel * *header_bytes),SEEK_SET);
		if (fread((void *)hdrPtr,(size_t)(*header_bytes),(size_t)1,infile) < 1)
		{   fclose(infile);
			return 3;
		}
	}

/***** Determine number of GPR scans in file *****/
	fseek(infile, 0L, SEEK_END);       /* go to end of file */
	last_byte = ftell(infile);         /* get last location (byte) */
	num_bytes = last_byte - (*num_hdrs * *header_bytes);
	*num_traces = num_bytes / (hdrPtr->rh_nsamp * (hdrPtr->rh_bits / 8));
	*num_traces /= *num_hdrs;

	fclose(infile);
	if (Debug)
	{   printf("\nReadOneDztHeader: num_hdrs=%ld num_traces=%ld header_bytes=%ld\n",
				*num_hdrs,*num_traces,*header_bytes);
		getch();
	}
	if (error2 && !error && !error3) return 9;
	if (error3 && !error && !error2) return 10;
	if (error && !error2 && !error3) return 11;
	if (error && error2 && !error3)  return 12;
	if (error && error3 && !error2)  return 13;
	if (error2 && error3 && !error) return 14;
	if (error2 && error3 && error)  return 15;

    return 0;
}
/****************************** SetDztHeader() ******************************/
/* Set all members for one header structure.
 *
 * Parameters:
 *  int hdr_num                 - 1, 2, 3, or 4; the header number in multi-channel files
 *  unsigned short rh_nsamp     - samples per scan
 *  unsigned short rh_bits      - bits per data word
 *  short          rh_zero      - binary offset (mid-point of data)
 *  float          rh_sps       - scans per second
 *  float          rh_spm       - scans per meter
 *  float          rh_mpm       - meters per mark
 *  float          rh_position  - position (ns)
 *  float          rh_range     - range (ns)
 *  unsigned short rh_npass     - scans per pass for 2D files
 *  unsigned short rh_nchan     - number of channels
 *  float          rh_epsr      - average dielectric constant
 *  float          rh_top       - top position in meters
 *  float          rh_depth     - range in meters
 *  char          *rh_antname   - [14], antenna name
 *  char          *rh_name      - [12], this file name
 *  char          *text         - [rh_ntext-1], comments
 *  unsigned short rg_breaks    - number of range gain break points
 *  float         *rg_values    - [rg_breaks], range gain at break points in decibels
 *  unsigned short rh_nproc     - size of processing history
 *  unsigned char *proc_hist    - [rh_nproc], coded processing history
 *  struct DztDateStruct *create_date - address of time/date structure
 *  struct DztHdrStruct *hdrPtr - address of header structure
 *
 * Requires: <string.h>, <dos.h>, "gpr_io.h".
 * Calls: memset, strncpy, strlen, strcpy, .
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: December 6, 1995
 * Revisions: March 17, 2000 - switched to different get time/date function
 */
void SetDztHeader (int hdr_num,unsigned short rh_nsamp,unsigned short rh_bits,
        short rh_zero,float rh_sps,float rh_spm,float rh_mpm,
        float rh_position,float rh_range,unsigned short rh_npass,
        unsigned short rh_nchan,float rh_epsr,float rh_top,float rh_depth,
		char *rh_antname,char *rh_name,char *text,unsigned short rg_breaks,
        float *rg_values,unsigned short rh_nproc,
        unsigned char *proc_hist,struct DztDateStruct *create_date,
        struct DztHdrStruct *hdrPtr)
{
    char           *cp  = (char *)hdrPtr;
    unsigned char  *ucp = (unsigned char *)hdrPtr;
    unsigned short *usp = (unsigned short *)hdrPtr;
    unsigned short checksum = 0;
    int i;
    time_t long_time;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
    extern int Debug;

    if (Debug)
    {
      printf("hdr_num = %d rh_nsamp = %hu rh_bits = %hu rh_zero = %hd\n\
rh_sps = %g rh_spm = %g rh_mpm = %g,float rh_position = %g\n\
rh_range = %g rh_npass = %hu rh_nchan = %hu rh_epsr = %f\n\
rh_top = %f rh_depth = %g rh_antname = %s rh_name = %s\n\
text = %s rg_breaks = %hu rg_values = %g rh_nproc = %hu\n\
proc_hist = %s hdrPtr = %p\n",hdr_num,rh_nsamp,rh_bits,
        rh_zero,rh_sps,rh_spm,rh_mpm,rh_position,rh_range,rh_npass,
        rh_nchan,rh_epsr,rh_top,rh_depth,rh_antname,rh_name,text,
        rg_breaks,rg_values,rh_nproc,proc_hist,hdrPtr);
        getch();
    }
    memset((void *)hdrPtr,0x00,sizeof(struct DztHdrStruct));
    switch (hdr_num)
    {   case 1:  hdrPtr->rh_tag = 0x00FF;  break;
        case 2:  hdrPtr->rh_tag = 0x01FF;  break;
        case 3:  hdrPtr->rh_tag = 0x02FF;  break;
        case 4:  hdrPtr->rh_tag = 0x03FF;  break;
    }
    hdrPtr->rh_data     = sizeof(struct DztHdrStruct) * rh_nchan;
    hdrPtr->rh_nsamp    = rh_nsamp;
    hdrPtr->rh_bits     = rh_bits;
    hdrPtr->rh_zero     = rh_zero;
    hdrPtr->rh_sps      = rh_sps;
    hdrPtr->rh_spm      = rh_spm;
    hdrPtr->rh_mpm      = rh_mpm;
    hdrPtr->rh_position = rh_position;
    hdrPtr->rh_range    = rh_range;
    hdrPtr->rh_npass    = rh_npass;
    hdrPtr->rh_create.sec2  = create_date->sec2;
	hdrPtr->rh_create.min   = create_date->min;
    hdrPtr->rh_create.hour  = create_date->hour;
    hdrPtr->rh_create.day   = create_date->day;
    hdrPtr->rh_create.month = create_date->month;
    hdrPtr->rh_create.year  = create_date->year;
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
	hdrPtr->rh_modif.sec2   = dostime.second/2;   /* second/2  (0-29) */
	hdrPtr->rh_modif.min    = dostime.minute;     /* minute    (0-59) */
	hdrPtr->rh_modif.hour   = dostime.hour;       /* hour      (0-23) */
	hdrPtr->rh_modif.day    = dosdate.day;        /* day       (1-31) */
	hdrPtr->rh_modif.month  = dosdate.month;      /* month     (1-12; 1=Jan, 2=Feb, etc. */
	hdrPtr->rh_modif.year   = dosdate.year-1980;  /* year-1980 (0-127; 1980-2107) */
    /* hdrPtr->rh_modif remains as zeroes */
    hdrPtr->rh_nchan    = rh_nchan;
    hdrPtr->rh_epsr     = rh_epsr;
    hdrPtr->rh_top      = rh_top;
    hdrPtr->rh_depth    = rh_depth;
    /* hdrPtr->reserved[32] not currently used (all zeroes) */
    if (rh_antname[0])
        strncpy(hdrPtr->rh_antname,rh_antname,13);
    switch (rh_nchan)
    {   case 1:  hdrPtr->rh_chanmask = 0x0801;  break;    /* was 0x530x */
        case 2:  hdrPtr->rh_chanmask = 0x0803;  break;
        case 3:  hdrPtr->rh_chanmask = 0x0807;  break;
        case 4:  hdrPtr->rh_chanmask = 0x080F;  break;
    }
    if (rh_name[0])
        strncpy(hdrPtr->rh_name,rh_name,8);
    hdrPtr->rh_rgain    = 128;
    hdrPtr->rh_nrgain   = 2 + 4*(rg_breaks);
    *(unsigned short *)(cp + hdrPtr->rh_rgain) = rg_breaks;
    if (rg_breaks > 0)
        for (i=0; i<rg_breaks; i++)
            *(float *)(cp + hdrPtr->rh_rgain + 2 + 4*i) = rg_values[i];
    hdrPtr->rh_text     = hdrPtr->rh_rgain + hdrPtr->rh_nrgain;
    if (text && text[0])
    {   hdrPtr->rh_ntext = strlen(text) + 1;
        strcpy(cp + hdrPtr->rh_text,text);
    }
    else
        hdrPtr->rh_ntext = 0;
    hdrPtr->rh_proc     = hdrPtr->rh_text + hdrPtr->rh_ntext;
    hdrPtr->rh_nproc    = rh_nproc;
    if (proc_hist && proc_hist[0])
		for (i=0; i<rh_nproc; i++)
            *(ucp + hdrPtr->rh_proc + i) = proc_hist[i];
    for (i=0; i<512; i++)  checksum += *(usp + i);
    hdrPtr->rh_chksum = checksum;
}
/**************************** PrintOneDztHeader() ***************************/
/* Print out the contents of one DZT file header to an output device
 * (stdout, stderr, a disk file, etc.)..
 *
 * Parameters:
 *   int header_bytes    - number of actual bytes in file header, 512 or 1024
 *   int num_traces     - number of radar scans, <=0 if unknown
 *   char *out_filename - "stdout", "stderr", or a DOS filename
 *   char *dzt_filename - DOS name of the DZT file header(s) belong to
 *   struct DztHdrStruct *hdrPtr1 - addresses of the header
 *
 * Requires: <conio.h>, <stdio.h>, <string.h>, "gpr_io.h".
 * Calls: fopen, fprintf, getch, strstr, ConvertProcHist2.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: February 5, 1996
 */
int PrintOneDztHeader (int header_bytes,long num_traces,char *out_filename,
                       char *dzt_filename,struct DztHdrStruct *hdrPtr)
{
    char ant_freq[10];
    unsigned short rg_breaks = 0;
    unsigned short checksum;
    unsigned short ustemp;
    int i,rg_break_delta;
    FILE *stream = NULL;

    static char big_buff[1024];           /* removes from stack and places in data segment */
    extern const char *ant_number[];      /* in "dzt_io.h" */
    extern const char *ant_name[];        /* in "dzt_io.h" */
    extern const char *month_abbr[];      /* in "dzt_io.h" */

    /* Assign or open stream */
    if      (strstr(out_filename,"stdout"))  stream = stdout;
    else if (strstr(out_filename,"stderr"))  stream = stderr;
    else                                     stream = fopen(out_filename,"wt");
    if (stream == NULL) return 1;

	/* Print the header information */
    fprintf(stream,"SIR-10A header for file %s:\n",dzt_filename);
    fprintf(stream,"number of headers    = %d\n",hdrPtr->rh_nchan);
    fprintf(stream,"file header size     = %d\n",header_bytes);
    if (num_traces > 0)  fprintf(stream,"number of GPR traces = %d\n",num_traces);
    fprintf(stream,"SIR-10A header ID    = 0x%04x\n",hdrPtr->rh_tag);
    fprintf(stream,"offset to data       = %hd\n",hdrPtr->rh_data);
    fprintf(stream,"samples per scan     = %hu\n",hdrPtr->rh_nsamp);
    fprintf(stream,"bits per data word   = %hu\n",hdrPtr->rh_bits);
    fprintf(stream,"binary offset        = %hd (0x%04X)\n",hdrPtr->rh_zero,(unsigned short)hdrPtr->rh_zero);
    fprintf(stream,"scans per second     = %f\n",hdrPtr->rh_sps);
    fprintf(stream,"scans per meter      = %f\n",hdrPtr->rh_spm);
    fprintf(stream,"meters per mark      = %f\n",hdrPtr->rh_mpm);
    fprintf(stream,"position (ns)        = %f\n",hdrPtr->rh_position);
    fprintf(stream,"range (ns)           = %f\n",hdrPtr->rh_range);
    fprintf(stream,"scans per pass       = %hu\n",hdrPtr->rh_npass);
    if (hdrPtr->rh_create.month == 0)
        fprintf(stream,"create date          = no date\n");
    else
        fprintf(stream,"create date          = %s %d, %d (%d:%02d:%02d)\n",
            month_abbr[hdrPtr->rh_create.month],hdrPtr->rh_create.day,hdrPtr->rh_create.year+1980,
            hdrPtr->rh_create.hour,hdrPtr->rh_create.min,hdrPtr->rh_create.sec2*2);
    if (hdrPtr->rh_modif.month == 0)
        fprintf(stream,"modification date    = no date\n");
    else
        fprintf(stream,"modification date    = %s %d, %d (%d:%02d:%02d)\n",
            month_abbr[hdrPtr->rh_modif.month],hdrPtr->rh_modif.day,hdrPtr->rh_modif.year+1980,
            hdrPtr->rh_modif.hour,hdrPtr->rh_modif.min,hdrPtr->rh_modif.sec2*2);
    fprintf(stream,"offset to range gain = %hu\n",hdrPtr->rh_rgain);
    fprintf(stream,"size of range gain   = %hu\n",hdrPtr->rh_nrgain);
    fprintf(stream,"offset to text       = %hu\n",hdrPtr->rh_text);
    fprintf(stream,"size of text         = %hu\n",hdrPtr->rh_ntext);
    fprintf(stream,"offset to proc.hist. = %hu\n",hdrPtr->rh_proc);
    fprintf(stream,"size of proc.hist.   = %hu\n",hdrPtr->rh_nproc);
    if (stream==stdout || stream==stderr)
    {   fprintf(stream,"Press a key to continue ...");
        getch();
        /* fprintf(stream,"\r                                \r");*/
        puts("");
    }
    fprintf(stream,"number of channels   = %hu\n",hdrPtr->rh_nchan);
    fprintf(stream,"ave. diel. constant  = %f\n",hdrPtr->rh_epsr);
    fprintf(stream,"top pos. in meters   = %f\n",hdrPtr->rh_top);
    fprintf(stream,"range in meters      = %f\n",hdrPtr->rh_depth);
    fprintf(stream,"reserved             = %s\n",hdrPtr->reserved);
    if (hdrPtr->rh_dtype == 0 && hdrPtr->rh_bits != 8)
    {        /* rh_dtype not used */
    } else   /* rh_dtype may be used */
    {  switch (hdrPtr->rh_dtype)
        {   case  0x00:
                fprintf(stream,"data element type    = unsigned 8-bit int\n");
                break;
            case  0x01:
                fprintf(stream,"data element type    = signed 8-bit int\n");
                break;
            case 0x02:
                fprintf(stream,"data element type    = unsigned 16-bit int\n");
                break;
            case 0x03:
                fprintf(stream,"data element type    = signed 16-bit int\n");
                break;
            case 0x04:
                fprintf(stream,"data element type    = signed 32-bit int\n");
                break;
            case 0x05:
                fprintf(stream,"data element type    = unsigned 32-bit int\n");
                break;
            case 0x10:
                fprintf(stream,"data element type    = 32-bit float\n");
                break;
            case 0x20:
                fprintf(stream,"data element type    = 64-bit float (USGS)\n");
                break;
            default:
                fprintf(stream,"data element type    = unknown (0x%02X)\n",hdrPtr->rh_dtype);
                break;
        }
    }
    ant_freq[0] = 0;
    for (i=0; ant_number[i]; i++)
    {   if (strstr(hdrPtr->rh_antname,ant_number[i]))
        {   strcpy(ant_freq,ant_name[i]);
            break;  /* out of for loop */
        }
    }
    fprintf(stream,"antenna name         = %s",hdrPtr->rh_antname);
    if (ant_freq[0]) fprintf(stream," (%s)",ant_freq);
    fprintf(stream,"\n");
    fprintf(stream,"active channels mask = 0x%04X\n",hdrPtr->rh_chanmask);
    fprintf(stream,"this file name       = %s\n",hdrPtr->rh_name);
    if (hdrPtr->rh_chksum != 0)
    {    fprintf(stream,"checksum for header  = %hu (0x%04X)\n",hdrPtr->rh_chksum,hdrPtr->rh_chksum);
        ustemp = hdrPtr->rh_chksum;
        hdrPtr->rh_chksum = 0;
        checksum = 0;
        for (i=0; i<512; i++)
            checksum += *((unsigned short *)hdrPtr + i);
        fprintf(stream,"calculated checksum  = %hu (0x%04X) [header %s corrupted]\n",
            checksum,checksum,(checksum==ustemp) ? "is not" : "is");
        hdrPtr->rh_chksum = ustemp;
    }
    if (hdrPtr->rh_rgain != 0 && hdrPtr->rh_nrgain != 0)
    {   rg_breaks = *(unsigned short *) ((char *)hdrPtr + hdrPtr->rh_rgain);
        fprintf(stream,"number of rg breaks  = %hd\n",rg_breaks);
        fprintf(stream,"range gain: scan sample     db\n");
        rg_break_delta = (hdrPtr->rh_nsamp-1) / (rg_breaks-1);
        for (i=0; i<rg_breaks; i++)
        {   fprintf(stream,"               %3d       %7.3f\n",
                rg_break_delta*i,*(float *)((char *)hdrPtr + hdrPtr->rh_rgain + 2 + 4*i));
        }
    }
    if (stream==stdout || stream==stderr)
    {   fprintf(stream,"Press a key to continue ...");
        getch();
        /* fprintf(stream,"\r                                \r");*/
        puts("");
    }
    if (hdrPtr->rh_text)
        fprintf(stream,"comments =\n%s\n",(char *)hdrPtr + hdrPtr->rh_text);
    else
        fprintf(stream,"no comments\n");
    if (hdrPtr->rh_nproc)
    {   ConvertProcHist2((int)sizeof(big_buff),big_buff,(int)hdrPtr->rh_nproc,
                                 ((char *)hdrPtr + hdrPtr->rh_proc));
        fprintf(stream,"Processing History:\n");
        if (strlen(big_buff)) fprintf(stream,"%s\n",big_buff);
        else                  fprintf(stream,"\tnone\n\n");
    }
    else
        fprintf(stream,"no processing history\n");
    if      (strstr(out_filename,"stdout"))     ;
    else if (strstr(out_filename,"stderr"))     ;
    else                                        fclose(stream);
    return 0;
}
/**************************** ConvertProcHist2() ****************************/
/* This function converts the coded processing history into a long ASCII
 * string.  This code is modified from that written by GSSI, contact Leo
 * Galinovsky.  This function differs from ConvertProcHist() in that the
 * function arguments do not contain the structure pointer but the
 * processing history buffer and its size.
 *
 * Parameters:
 *   int bufsiz      - size of buffer pointed to by "buff"
 *   char *buff      - pointer to string (1024-byte length recommended)
 *   int hist_size   - bytes in processign history buffer
 *   char *proc_hist - pointer to string containing coded processing history
 *
 * Requires: <stdio.h>, <string.h>, "gpr_io.h".
 * Calls: strcpy, sprintf, strlen.
 * Returns: char *buff.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: November 14, 1995
 */
char *ConvertProcHist2 (int bufsiz,char *buff,int hist_size,char *proc_hist)
{
    char *b,*p,*proc;
    int nproc;
    extern const char *ph_viirl,*ph_viirh,*ph_vfirl,*ph_vfirh,*ph_vboxl;
    extern const char *ph_vboxh,*ph_vtril,*ph_vtrih,*ph_vtcl,*ph_vtch;
    extern const char *ph_hiirl,*ph_hiirh,*ph_hfirl,*ph_hfirh,*ph_hboxl;
    extern const char *ph_htril,*ph_htcl,*ph_hboxh,*ph_htrih,*ph_htch,*ph_hsts;

    b = buff;
    b[0] = 0;
    p = proc = proc_hist;
    nproc = hist_size;
    while (p-proc < nproc)
    {   switch (*p)
        {   /* first two case to make header compatible with old tapes */
            case PR_LPF:
                p += 2 + sizeof(float);
                break;
            case PR_HPF:
                p += 2 + sizeof(float);
                break;
            case PR_VIIRL:
                sprintf(b,ph_viirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VIIRH:
                sprintf(b,ph_viirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VFIRL:
                sprintf(b,ph_vfirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VFIRH:
                sprintf(b,ph_vfirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VBOXL:
                sprintf(b,ph_vboxl,(int)*(p+1));
                p += 2;
                break;
            case PR_VBOXH:
                sprintf(b,ph_vboxh,(int)*(p+1));
                p += 2;
                break;
            case PR_VTRIL:
                sprintf(b,ph_vtril,(int)*(p+1));
                p += 2;
                break;
            case PR_VTRIH:
                sprintf(b,ph_vtrih,(int)*(p+1));
                p += 2;
                break;
            case PR_VTCL:
                sprintf(b,ph_vtcl,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_VTCH:
                sprintf(b,ph_vtch,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HIIRL:
                sprintf(b,ph_hiirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HIIRH:
                sprintf(b,ph_hiirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HFIRL:
                sprintf(b,ph_hfirl,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HFIRH:
                sprintf(b,ph_hfirh,(int)*(p+1),*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HBOXL:
                sprintf(b,ph_hboxl,(int)*(p+1));
                p += 2;
                break;
            case PR_HTRIL:
                sprintf(b,ph_htril,(int)*(p+1));
                p += 2;
                break;
            case PR_HTCL:
                sprintf(b,ph_htcl,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_HBOXH:
                sprintf(b,ph_hboxh,(int)*(p+1));
                p += 2;
                break;
            case PR_HTRIH:
                sprintf(b,ph_htrih,(int)*(p+1));
                p += 2;
                break;
            case PR_HTCH:
                sprintf(b,ph_htch,*(float *)(p+2));
                p += 2 + sizeof(float);
                break;
            case PR_STS:  case PR_STS_NEW:
                sprintf(b,ph_hsts,(int)*(p+1));
                p += 2;
                break;
            default:
                return buff;
        }
        if (strlen(buff) + strlen(b) > bufsiz) break;
        b += strlen(b);
    }
    return buff;
}
/**************************** GetDztChSubGrid8() ****************************/
/* Read a section of GPR scans from a DZT file.  The scans must have 8 bits
 * per sample.  The scans are stored in an unsigned char 2D grid scan by
 * scan (grid is organized column by column).
 *
 * This program opens the file, moves past the header(s), moves the file
 * file pointer to the first scan to read, copies the section of scans from
 * the requested channel into the grid, then closes the file.
 *
 * Note: This function differs from earlier versions in that it doesn't
 *       require the first sample of the first trace to have all bits set.
 *
 * Parameters:
 *  char *filename  - pointer to string, name of DZT file to copy scans from
 *  int channel     - channel number, indexed from 0
 *  long first_scan  - address of variable, first scan to copy, indexed from 0
 *  long num_cols    - number of columns (scans) in grid[][]
 *  long num_rows    - number of rows (samples per scan) in grid[][]
 *  unsigned char **grid - pointer to 2D array [num_cols][num_rows]
 *
 * NOTE: Storage for filename[] and grid[][] must have been previously
 *       allocated.
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_io.h".
 * Calls: fopen, setvbuf, fclose, fseek, fread, malloc, free, memcpy, printf,
 *        ReadOneDztHeader.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *GetDztChSubGrid8Msg[] =
{   "GetDztChSubGrid8(): No error.",
    "GetDztChSubGrid8() Error: fopen() failed.",
    "GetDztChSubGrid8() Error: Short count from fread().",
    "GetDztChSubGrid8() Error: Inconsistency -- first scan sample not 0xFFFF.",
    "GetDztChSubGrid8() Error: Number scan samples exceed number of grid rows.",
*5*"GetDztChSubGrid8() Error: EOF encountered -- grid cols exceed number of scans.",
    "GetDztChSubGrid8() Error: Unable to allocate temporary storage.",
    "GetDztChSubGrid8() Error: Data do not consist of 8-bit samples.",
    "GetDztChSubGrid8() Error: Error value returned by ReadFirstDztHeader().",
    "GetDztChSubGrid8() Error: Invalid channel selected.",
    "GetDztChSubGrid8() Error: Number traces exceeds number of grid columns.",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: November 17, 1995; August 4, 1999;
 */
int GetDztChSubGrid8 (char *filename,int channel,long first_scan,long num_cols,
              long num_rows,unsigned char **grid)
{
    int error,num;                             /* scratch variables and counters */
    int header_size,num_hdrs;
    long num_traces;
    long disk_scan_size,offset,disk_chunk_size; /* measured in bytes */
    unsigned char *scan_chunk = NULL;          /* hold one set of scans */
    char *cp;                                  /* will point to offset in scan_chunk */
    FILE *infile;
    struct DztHdrStruct hdr;
    #if defined(_INTELC32_)
        char *buffer = NULL;
        size_t vbufsize = 65536;
    #endif
    extern int Debug;

    /* Get the first header from the file */
    error = ReadOneDztHeader(filename,&num_hdrs,&num_traces,channel,&header_size,&hdr);
    if (error)
    {   if (Debug) printf("ReadOneDztHeader() return %d\n",error);
        if (error < 9)  return 8;
    }

    /* Verify we have 8-bit data */
    if (hdr.rh_bits != 8)         return 7;

    /* Make sure requested channel is there */
    if (channel < 0 || channel >= hdr.rh_nchan)  /* rh_nchan indexed from 1 */
        return 9;

    /* Verify number of trace samples do not exceed number of rows in the grid */
    if (hdr.rh_nsamp > num_rows)  return 4;

    /* Verify the first scan plus the number of columns (scans) does not exceed
       number of traces */
    if ( (first_scan + num_cols) > num_traces) return 10;

    /* Open DZT file */
    if ((infile = fopen(filename,"rb")) == NULL) return 1;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
        realmalloc(buffer,vbufsize);
        if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
        else         setvbuf(infile,NULL,_IOFBF,vbufsize);
    #endif

    /* Allocate storage for one chunk of scans */
    disk_scan_size = hdr.rh_nsamp * sizeof(unsigned char);
    disk_chunk_size = disk_scan_size * hdr.rh_nchan;
    scan_chunk = (unsigned char *)malloc((size_t)disk_chunk_size);
    if (scan_chunk == NULL)
    {   fclose(infile);
        #if defined(_INTELC32_)
            realfree(buffer);
        #endif
        return 6;
    }

    /* Determine offset to channel 0 of "first_scan" and move file pointer */
    offset = (hdr.rh_nchan * header_size) + (first_scan * disk_chunk_size);
    fseek(infile,offset,SEEK_SET);

    /* Get first chunk of scans */
    if (fread((void *)scan_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
    {   fclose(infile);
        #if defined(_INTELC32_)
            realfree(buffer);
        #endif
        free(scan_chunk);
        return 2;
    }

    /* Verify first sample has all bits set */
    #ifdef VERIFY_SYNC
        if (scan_chunk[0] != 0xFF)
        {   fclose(infile);
            #if defined(_INTELC32_)
                realfree(buffer);
            #endif
            free(scan_chunk);
            return 3;
        }
    #endif

    /* Copy selected channel into grid */
    offset = channel * disk_scan_size;
    cp = (char *)scan_chunk + offset;
    memcpy((void *)(grid[0]),(void *)cp,(size_t)disk_scan_size);

    /* Get requested scans from file one at a time */
    error = 0;
    for (num=1; num<num_cols; num++)
    {   if (fread((void *)scan_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
        {   error = 2;
            if (feof(infile)) error = 5;
            break;
        }
        /* Copy selected channel into grid */
        memcpy((void *)(grid[num]),(void *)cp,(size_t)disk_scan_size);
    }

    /* Clean up and return */
    fclose(infile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    free(scan_chunk);
    return error;
}
/**************************** GetDztChSubGrid16() ***************************/
/* Read a section of GPR scans from a DZT file.  The scans must have 16 bits
 * per sample.  The scans are stored in an unsigned short 2D grid scan by
 * scan (grid is organized column by column).
 *
 * This program opens the file, moves past the header(s), moves the file
 * file pointer to the first scan to read, copies the section of scans from
 * the requested channel into the grid, then closes the file.
 *
 * Note: This function differs from earlier versions in that it doesn't
 *       require the first sample of the first trace to have all bits set.
 *
 * Parameters:
 *  char *filename  - pointer to string, name of DZT file to copy scans from
 *  int channel     - channel number, indexed from 0
 *  long first_scan  - address of variable, first scan to copy, indexed from 0
 *  long num_cols    - number of columns (scans) in grid[][]
 *  long num_rows    - number of rows (samples per scan) in grid[][]
 *  unsigned short **grid - pointer to 2D array [num_cols][num_rows]
 *
 * NOTE: Storage for filename[] and grid[][] must have been previously
 *       allocated.
 *
 * Requires: <stdio.h>, <stdlib.h>, <string.h>, "gpr_io.h".
 * Calls: fopen, setvbuf, fclose, fseek, fread, malloc, free, memcpy,
 *        ReadOneDztHeader.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *GetDztChSubGrid16Msg[] =
{   "GetDztChSubGrid16(): No error.",
    "GetDztChSubGrid16() Error: fopen() failed.",
    "GetDztChSubGrid16() Error: Short count from fread().",
    "GetDztChSubGrid16() Error: Inconsistency -- first scan sample not 0xFFFF.",
    "GetDztChSubGrid16() Error: Number scan samples exceed number of grid rows.",

    "GetDztChSubGrid16() Error: EOF encountered -- grid cols exceed number of scans.",
    "GetDztChSubGrid16() Error: Unable to allocate temporary storage.",
    "GetDztChSubGrid16() Error: Data do not consist of 16-bit samples.",
    "GetDztChSubGrid16() Error: Error value returned by ReadFirstDztHeader().",
    "GetDztChSubGrid16() Error: Invalid channel selected.",

    "GetDztChSubGrid16() Error: Number traces exceeds number of grid columns.",
}
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: November 17, 1995; August 4, 1999;
 */
int GetDztChSubGrid16 (char *filename,int channel,long first_scan,long num_cols,
                      long num_rows,unsigned short **grid)
{
    int error,num;                             /* scratch variables and counters */
    int header_size,num_hdrs;
    long num_traces;
    long disk_scan_size,offset,disk_chunk_size; /* measured in bytes */
    unsigned short *scan_chunk = NULL;         /* hold one set of scans */
    char *cp;                                  /* will point to offset in scan_chunk */
    FILE *infile;
    struct DztHdrStruct hdr;
    #if defined(_INTELC32_)
        char *buffer = NULL;
        size_t vbufsize = 65536;
    #endif
    extern int Debug;

    /* Get the first header from the file */
    error = ReadOneDztHeader(filename,&num_hdrs,&num_traces,channel,&header_size,&hdr);
    if (error)
    {   if (Debug) printf("ReadOneDztHeader() return %d\n",error);
        if (error < 9)  return 8;
    }

    /* Verify we have 16-bit data */
    if (hdr.rh_bits != 16)         return 7;

    /* Make sure requested channel is there; rh_nchan indexed from 1 */
    if (channel < 0 || channel >= hdr.rh_nchan)  return 9;

    /* Verify number of trace samples do not exceed number of rows in the grid */
    if (hdr.rh_nsamp > num_rows)   return 4;

    /* Verify the first scan plus the number of columns (scans) does not exceed
       number of traces */
    if ( (first_scan + num_cols) > num_traces) return 10;

    /* Open DZT file */
    if ((infile = fopen(filename,"rb")) == NULL) return 1;

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
        realmalloc(buffer,vbufsize);
        if (buffer)  setvbuf(infile,buffer,_IOFBF,vbufsize);
        else         setvbuf(infile,NULL,_IOFBF,vbufsize);
    #endif

    /* Allocate storage for one chunk of scans */
    disk_scan_size = hdr.rh_nsamp * sizeof(unsigned short);
    disk_chunk_size = disk_scan_size * hdr.rh_nchan;
    scan_chunk = (unsigned short *)malloc((size_t)disk_chunk_size);
    if (scan_chunk == NULL)
    {   fclose(infile);
        #if defined(_INTELC32_)
            realfree(buffer);
        #endif
        return 6;
    }

    /* Determine offset to channel 0 of "first_scan" and move file pointer */
    offset = (hdr.rh_nchan * header_size + (first_scan * disk_chunk_size));
    fseek(infile,offset,SEEK_SET);

    /* Get first chunk of scans */
    if (fread((void *)scan_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
    {   fclose(infile);
        #if defined(_INTELC32_)
            realfree(buffer);
        #endif
        free(scan_chunk);
        return 2;
    }

    /* Verify first sample has all bits set */
    #ifdef VERIFY_SYNC
        if (scan_chunk[0] != 0xFFFF)
        {   fclose(infile);
            #if defined(_INTELC32_)
                realfree(buffer);
            #endif
        free(scan_chunk);
        return 3;
        }
    #endif

    /* Copy selected channel into grid */
    offset = channel * disk_scan_size;
    cp = (char *)scan_chunk + offset;
    memcpy((void *)(grid[0]),(void *)cp,(size_t)disk_scan_size);

    /* Get requested scans from file one at a time */
    error = 0;
    for (num=1; num<num_cols; num++)
    {   if (fread((void *)scan_chunk,(size_t)disk_chunk_size,(size_t)1,infile) < 1)
        {   error = 2;
            if (feof(infile)) error = 5;
            break;
        }
        /* Copy selected channel into grid */
        memcpy((void *)(grid[num]),(void *)cp,(size_t)disk_scan_size);
    }

    /* Clean up and return */
    fclose(infile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    free(scan_chunk);
    return error;
}
/******************************* SaveDztFile() ******************************/
/* Write the DZT header(s) and the data to a disk file.
 *
 * NOTE: There is no file overwrite protection.
 *       This function differs from earlier versions in that it forces the
 *       number of samples to be a power of two.
 *
 * Parameters:
 *  char *filename - name of disk file to store DZT data/info to
 *  long num_traces - number of columns in grid
 *  long num_samps  - number of rows in grid
 *  int num_bits   - number of bits per trace sample
 *  int num_hdrs   - number of headers
 *  struct DztHdrStruct *hdrPtrArray[] - array of pointers to header
 *                                       structures; [num_hdrs]
 *  void **grid    - pointer to grid storage areas; [num_cols][num_rows]
 *
 * NOTE: Storage for variables pointed to in parameters list must have been
 *       allocated prior to calling this function.
 *
 * Requires: <math.h>, <stdio.h>, <stdlib.h>, "gpr_io.h".
 * Calls: fopen, fclose, fwrite, setvbuf, malloc, free, pow.
 * Returns: 0 on success,
 *         >0 on error (offset into an array of strings).
 *
const char *SaveDztFileMsg[] =
{   "SaveDztFile(): No errors.",
    "SaveDztFile() Error: Function argument list contains a NULL pointer.",
    "SaveDztFile() Error: Function argument list contains an invalid value.",
    "SaveDztFile() Error: Problem opening output file.",
    "SaveDztFile() Error: Short count from fwrite saving header(s).",
    "SaveDztFile() Error: Short count from fwrite saving data.",
    "SaveDztFile() Error: Unable to allocate temporary storage.",
} ;
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: March 29, 1995
 */
int SaveDztFile (char *filename,long num_traces,long num_samps,int num_bits,
                 int num_hdrs,struct DztHdrStruct *hdrPtrArray[],void **grid)
{
    void *filler = NULL;
    int i,error;
    long trace,trace_bytes,filler_bytes,num_fillers;
    unsigned long mid_val;
    FILE *outfile;

    #if defined(_INTELC32_)
      char *buffer = NULL;
      size_t vbufsize = 65536;
    #endif

    /* Verify parameter list */
    if (hdrPtrArray == NULL || grid == NULL || filename == NULL) return 1;
    if (num_hdrs < 1 || num_traces < 1 || num_samps < 2 || num_bits < 8 ||
        num_samps > 65536L)
        return 2;

    /* Check that num_samps is a power of 2 */
    filler_bytes = 0L;
    num_fillers = 0L;
    for (i=1; i<16; i++)
    {   if (num_samps > pow(2,(double)i) && num_samps < pow(2,(double)(i+1)))
        {   int j;

            num_fillers = pow(2,(double)(i+1)) - num_samps;
            filler = (void *)malloc(num_fillers * (num_bits/8));
            if (filler == NULL) return 6;
            mid_val = 0x00000001UL << (num_bits - 1);  /* 128, 32768, or 2147483648 */
            switch (num_bits)
            {   case 8:
                   filler_bytes = num_fillers * 1;   /* bytes */
                   for (j=0; j<num_fillers; j++)
                       ((unsigned char *)filler)[j] = mid_val;
                   break;
                case 16:
                   filler_bytes = num_fillers * 2;   /* bytes */
                   for (j=0; j<num_fillers; j++)
                       ((unsigned short *)filler)[j] = mid_val;
                   break;
                case 32:
                   filler_bytes = num_fillers * 4;   /* bytes */
                   for (j=0; j<num_fillers; j++)
                       ((unsigned long *)filler)[j] = mid_val;
                   break;
            }
            break; /* out of for loop */
        }
    }

    /* Open output file */
    if ((outfile = fopen(filename,"wb")) == NULL)
    {   if (num_fillers) free(filler);
        return 3;
    }

    /* Speed up disk access by using large (conventional memory) buffer */
    #if defined(_INTELC32_)
      realmalloc(buffer,vbufsize);
      if (buffer)  setvbuf(outfile,buffer,_IOFBF,vbufsize);
      else         setvbuf(outfile,NULL,_IOFBF,vbufsize);
    #endif

    /* Write header(s) to disk */
    error = 0;
    for (i=0; i<num_hdrs; i++)
    {   if (fwrite((void *)hdrPtrArray[i],sizeof(struct DztHdrStruct),(size_t)1,outfile) < 1)
        {   error = 4;
            break;
        }
    }

    /* Write data to disk */
    if (!error)
    {   trace_bytes = num_samps * (num_bits/8);    /* 8 bits per byte */
        for (trace=0; trace<num_traces; trace++)
        {   switch (num_bits)
            {   case 8:
                    ((unsigned char **)grid)[trace][0] = 0xFFU;
                    if (((unsigned char **)grid)[trace][1] != 0xE8U)
                        ((unsigned char **)grid)[trace][1] = 0xF1U;
                    break;
                case 16:
                    ((unsigned short **)grid)[trace][0] = 0xFFFFU;
                    if (((unsigned short **)grid)[trace][1] != 0xE800U)
                        ((unsigned short **)grid)[trace][1] = 0xF100U;
                    break;
                case 32:
                    ((unsigned long **)grid)[trace][0] = 0xFFFFFFFFUL;
                    if (((unsigned long **)grid)[trace][1] != 0xE8000000UL)
                        ((unsigned long **)grid)[trace][1] = 0xF1000000UL;
                    break;
            }
            if (fwrite(grid[trace],(size_t)trace_bytes,(size_t)1,outfile) < 1)
            {   error = 5;
                break;
            }
            if (num_fillers)
            {   if (fwrite(filler,(size_t)filler_bytes,(size_t)1,outfile) < 1)
                {   error = 5;
                    break;
                }
            }
        }
    }

    /* Close file, release buffer and return */
    fclose(outfile);
    #if defined(_INTELC32_)
      realfree(buffer);
    #endif
    if (num_fillers) free(filler);
    return error;
}
/****************************** EditDztHeader() *****************************/
/* Edit one field of a DZT header structure.
 *
 * Parameters:
 *   int                  field_id  - see below; number representing which field
 *                                    to change; fields in square brackets cannot
 *                                    be changed -- they can be specified but a
 *                                    hardwired solution is used.
 *   void                *field_val - address of variable to be type cast according
 *                                    to field_id
 *   struct DztHdrStruct *hdrPtr    - address of structure
 *
 * field_id    field_type       field_name        field_description
 *
 *    0      unsigned short       rh_tag;         select 0 to 3, the header number
 *   [1]     unsigned short       rh_data;        offset to data in file or could be
 *                                                  the size of the header structure
 *                                                  (always 1024)
 *    2      unsigned short       rh_nsamp;       samples per scan
 *    3      unsigned short       rh_bits;        bits per data word
 *   [4]     unsigned short       rh_zero;        binary offset (mid-point of data)
 *                                                  (determined from rh_bits)
 *    5      float                rh_sps;         scans per second
 *    6      float                rh_spm;         scans per meter
 *    7      float                rh_mpm;         meters per mark
 *    8      float                rh_position;    position (ns)
 *    9      float                rh_range;       range (ns)
 *   10      unsigned short       rh_npass;       scans per pass for 2D files
 *   11      struct DztDateStruct rh_create;      date created
 *   12      struct DztDateStruct rh_modif;       date modified
 *  [13]     unsigned short       rh_rgain;       offset to range gain function
 *                                                (always 128)
 *  [14]     unsigned short       rh_nrgain;      size of range gain function
 *                                                (determined from field_id 28)
 *  [15]     unsigned short       rh_text;        offset to text
 *                                                (determined from rh_rgain and rh_nrgain)
 *  [16]     unsigned short       rh_ntext;       size of text
 *                                                (determined from field_id 30)
 *  [17]     unsigned short       rh_proc;        offset to processing history
 *                                                (determined from rh_text and rh_ntext)
 *  [18]     unsigned short       rh_nproc;       size of processing history
 *                                                (determined from field_id 31)
 *   19      unsigned short       rh_nchan;       number of channels
 *   20      float                rh_epsr;        average dielectric constant
 *   21      float                rh_top;         top position in meters
 *   22      float                rh_depth;       range in meters
 *  [23]     char                 reserved[32];   (always zeroes; for now)
 *   24      char                 rh_antname[14]; antenna name (actually its number)
 *  [25]     unsigned short       rh_chanmask;    active channels mask
 *                                                (determined from rh_nchan; 0x5301, 0x5303, 0x5307, 0x530F)
 *   26      char                 rh_name[12];    this file name
 *   --      unsigned short       rh_chksum;      checksum for header (always recalculated)
 *      128 bytes to here
 *  [27]     char                 variable[896];  range gain, comments, processing history
 *      below are not part of structure except as stored in variable[]
 *   28      unsigned short       num_rg_breaks   number of range gain break points
 *   29      float *              rg_values       the range gain values in db
 *                                                (be sure to set num_rg_breaks first !!!)
 *   30      char *               text            comments
 *   31      char *               proc            this is the coded processing history
 *
 *
 * Requires: <string.h>, "gpr_io.h".
 * Calls: strlen, strncpy.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: October 20, 1995
 * Revisions:
 *   August 11, 1997   - fixed problem in case 4 - nsamps was subed for bits.
 *   April 11, 2000    - fixed problem in text field (#30) because some RADAN
 *                       and SIR-2 headers do not set rh_text!
 *
 */
void EditDztHeader (int field_id,void *field_val,struct DztHdrStruct *hdrPtr)
{
	char                 *c_ptr;
	char                 *cp_hdr = (char *)hdrPtr;
	unsigned short       *us_ptr, us_val, checksum;
	int                   i, old1, new1;
	float                *f_ptr, *f_ptr2, f_val;
	size_t                new_len;

	switch (field_id)
	{   case 0:
			us_val = *(unsigned short *)field_val;
			if (us_val <= 3)
			   hdrPtr->rh_tag = (us_val << 8) | 0x00FF;
			break;
		case 1:
			hdrPtr->rh_data = 1024;
			break;
		case 2:
			us_val = *(unsigned short *)field_val;
			/* if (us_val == 256 || us_val == 512 || us_val == 1024)  hdrPtr->rh_nsamp = us_val; */
			hdrPtr->rh_nsamp = us_val;
			break;
		case 3:
			us_val = *(unsigned short *)field_val;
			if (us_val == 8 || us_val == 16)
			{   hdrPtr->rh_bits = us_val;
				hdrPtr->rh_zero = 1 << (us_val-1);
			}
			break;
		case 4:
			if (hdrPtr->rh_bits == 8 || hdrPtr->rh_bits == 16)
				hdrPtr->rh_zero = 1 << (hdrPtr->rh_bits-1);
			break;
		case 5:
			f_val = *(float *)field_val;
			if (f_val >= 0.10 && f_val <= 128.0)  hdrPtr->rh_sps = f_val;
			break;
		case 6:
			hdrPtr->rh_spm = *(float *)field_val;
			break;
		case 7:
			hdrPtr->rh_mpm = *(float *)field_val;
			break;
		case 8:
			hdrPtr->rh_position = *(float *)field_val;
			break;
		case 9:
			hdrPtr->rh_range = *(float *)field_val;
			break;
		case 10:
			us_val = *(unsigned short *)field_val;
			if (us_val >= 1)  hdrPtr->rh_npass = us_val;
			break;
		case 11:
			hdrPtr->rh_create = *(struct DztDateStruct *)field_val;
			break;
		case 12:
			hdrPtr->rh_modif = *(struct DztDateStruct *)field_val;
			break;
		case 13:
			hdrPtr->rh_rgain = 128;
            break;
        case 14:
            break;
        case 15:
            if (hdrPtr->rh_rgain > 0 && hdrPtr->rh_nrgain > 0)
               hdrPtr->rh_text = hdrPtr->rh_rgain + hdrPtr->rh_nrgain;
            break;
		case 16:
            break;
        case 17:
            if (hdrPtr->rh_text > 0 && hdrPtr->rh_ntext > 0)
               hdrPtr->rh_text = hdrPtr->rh_text + hdrPtr->rh_ntext;
			break;
        case 18:
            break;
        case 19:
            us_ptr = (unsigned short *)field_val;
            if (*us_ptr >= 1  && *us_ptr <= 4)
            {   hdrPtr->rh_nchan = *us_ptr;
                switch (hdrPtr->rh_nchan)
                {   case 1:  hdrPtr->rh_chanmask = 0x5301;  break;
                    case 2:  hdrPtr->rh_chanmask = 0x5303;  break;
                    case 3:  hdrPtr->rh_chanmask = 0x5307;  break;
                    case 4:  hdrPtr->rh_chanmask = 0x530F;  break;
                }
            }
            break;
		case 20:
            hdrPtr->rh_epsr = *(float *)field_val;
            break;
        case 21:
            hdrPtr->rh_top = *(float *)field_val;
			break;
        case 22:
            hdrPtr->rh_depth = *(float *)field_val;
            break;
        case 23:
            break;
        case 24:
            strncpy(hdrPtr->rh_antname,(char *)field_val,13);
            if (hdrPtr->rh_antname[8] == '(') hdrPtr->rh_antname[13] = ')';
            else                              hdrPtr->rh_antname[13] = 0;
            break;
        case 25:
            switch (hdrPtr->rh_nchan)
            {   case 1:  hdrPtr->rh_chanmask = 0x5301;  break;
                case 2:  hdrPtr->rh_chanmask = 0x5303;  break;
				case 3:  hdrPtr->rh_chanmask = 0x5307;  break;
                case 4:  hdrPtr->rh_chanmask = 0x530F;  break;
            }
            break;
        case 26:
			strncpy(hdrPtr->rh_name,(char *)field_val,8);
            break;
        case 27:
            break;
        case 28:
            us_ptr = (unsigned short *)field_val;
            hdrPtr->rh_rgain = 128;
            /* get old value */
            us_val = *(unsigned short *)(cp_hdr + hdrPtr->rh_rgain);
            /* is new value in valid range? */
            if (*us_ptr >= 2 && *us_ptr <= 8)
            {   /* move remainder of variable[] if neccesary */
                if (*us_ptr > us_val)
                {   new1 = sizeof(struct DztHdrStruct) - 1;
                    old1 = new1 - ((*us_ptr - us_val) * sizeof(float));
					while (old1 >= hdrPtr->rh_rgain+2+us_val*sizeof(float))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1--;  old1--;
                    }
                } else if (*us_ptr < us_val)
				{   old1 = hdrPtr->rh_rgain + 2 + (us_val * sizeof(float));
                    new1 = old1 - ((us_val - *us_ptr) * sizeof(float));
                    while (old1 < sizeof(struct DztHdrStruct))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1++;  old1++;
                    }
                }
                *(unsigned short *)(cp_hdr + hdrPtr->rh_rgain) = *us_ptr;
                hdrPtr->rh_nrgain = 2 + *us_ptr * sizeof(float);
                /* adjust offsets to remaining fields */
                hdrPtr->rh_text = hdrPtr->rh_rgain + hdrPtr->rh_nrgain;
                hdrPtr->rh_proc = hdrPtr->rh_text + hdrPtr->rh_ntext;
            }
            break;
        case 29:
			/* get number of breaks from header */
            us_val = *(unsigned short *)(cp_hdr + hdrPtr->rh_rgain);
            /* type cast array pointer */
            f_ptr = (float *)field_val;
            /* point to start of float "array" in variable[] */
			f_ptr2 = (float *)(cp_hdr + hdrPtr->rh_rgain + 2);
            /* copy values */
            for (i=0; i<us_val; i++)  f_ptr2[i] = f_ptr[i];
            break;
        case 30:
            c_ptr = (char *)field_val;
            new_len = strlen(c_ptr) + 1;
            /* is new value in valid range? */
            if (hdrPtr->rh_text == 0)  /* some RADAN and SIR-2 files */
                hdrPtr->rh_text = hdrPtr->rh_rgain + hdrPtr->rh_nrgain;
            if (hdrPtr->rh_text == 0)  /* still 0? */
                hdrPtr->rh_text = 128;
            if (new_len > 1)
            {   /* move remainder of variable[] if neccesary */
                if (new_len > hdrPtr->rh_ntext)
                {   new1 = sizeof(struct DztHdrStruct) - 1;
                    old1 = new1 - (new_len - hdrPtr->rh_ntext);
                    while (old1 >= hdrPtr->rh_text + hdrPtr->rh_ntext)
                    {   cp_hdr[new1] = cp_hdr[old1];
						new1--;  old1--;
                    }
                } else if (new_len < hdrPtr->rh_ntext)
                {   old1 = hdrPtr->rh_text + hdrPtr->rh_ntext;
                    new1 = old1 - (hdrPtr->rh_ntext - new_len);
					while (old1 < sizeof(struct DztHdrStruct))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1++;  old1++;
                    }
                }
                hdrPtr->rh_ntext = new_len;
                /* adjust offsets to remaining fields */
                hdrPtr->rh_proc = hdrPtr->rh_text + hdrPtr->rh_ntext;
                /* copy replacement text string */
                strncpy(cp_hdr + hdrPtr->rh_text,c_ptr,new_len - 1);
                *(cp_hdr + hdrPtr->rh_text + hdrPtr->rh_ntext - 1) = 0;  /* be sure NULL terminated */
            }
            break;
        case 31:
            c_ptr = (char *)field_val;
			new_len = strlen(c_ptr) + 1;
            /* is new value in valid range? */
            if (new_len > 1)
            {   /* move remainder of variable[] if neccesary */
                if (new_len > hdrPtr->rh_nproc)
				{   new1 = sizeof(struct DztHdrStruct) - 1;
                    old1 = new1 - (new_len - hdrPtr->rh_nproc);
                    while (old1 >= hdrPtr->rh_proc + hdrPtr->rh_nproc)
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1--;  old1--;
                    }
                } else if (new_len < hdrPtr->rh_nproc)
                {   old1 = hdrPtr->rh_proc + hdrPtr->rh_nproc;
                    new1 = old1 - (hdrPtr->rh_nproc - new_len);
                    while (old1 < sizeof(struct DztHdrStruct))
                    {   cp_hdr[new1] = cp_hdr[old1];
                        new1++;  old1++;
                    }
                }
                hdrPtr->rh_nproc = new_len;
				/* copy replacement string */
                strncpy(cp_hdr + hdrPtr->rh_proc,c_ptr,new_len - 1);
                *(cp_hdr + hdrPtr->rh_proc +  - 1) = 0;  /* be sure NULL terminated */
            }
            break;
	}
    /* now update checksum ! */
    hdrPtr->rh_chksum = 0;
    checksum = 0;
    for (i=0; i<512; i++)
        checksum += *((unsigned short *)hdrPtr + i);
	hdrPtr->rh_chksum = checksum;
}
/***************************** SetDzt5xHeader() *****************************/
/* Set the values in a ver 5.x DZT header structure.
 * New version incorporates some 1/94 changes by GSSI to version 5.x.
 *
 * Note: DO NOT send char pointers from hdr through this function!
 *       SetDzt5xHeader() clears structure arrays before writing to
 *       them! Other members are OK.
 *
 * Requires: <conio.h>, <string.h>, <stdio.h>, <time.h>, "gpr_io.h".
 * Calls: printf, getch, memset, time, localtime, strncpy, strcpy.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: September 17, 1996
 * Revisions: March 17, 2000 - switched to different get time/date function
 */
void SetDzt5xHeader (int hdr_num,unsigned short rh_nsamp,unsigned short rh_bits,
		short rh_zero,float rh_sps,float rh_spm,float rh_mpm,
		float rh_position,float rh_range,unsigned short rh_npass,
		unsigned short rh_nchan,float rh_epsr,float rh_top,float rh_depth,
		int rh_dtype,char *antname,char *rh_name,char *text,
		unsigned short rg_breaks,float *rg_values,unsigned short rh_nproc,
		unsigned char *proc_hist,struct DztDateStruct *create_date,
		struct DztHdrStruct *hdrPtr)
{
	char           *cp  = (char *)hdrPtr;
	unsigned char  *ucp = (unsigned char *)hdrPtr;
	unsigned short *usp = (unsigned short *)hdrPtr;
	unsigned short checksum = 0;
	int i,hdr_size;
    struct dosdate_t dosdate;
    struct dostime_t dostime;
	extern int Debug;

	if (Debug)
	{   printf("hdr_num = %d rh_nsamp = %hu rh_bits = %hu rh_zero = %hd\n",
			hdr_num,rh_nsamp,rh_bits,rh_zero);
		printf("rh_sps = %g rh_spm = %g rh_mpm = %g,rh_position = %g\n",
            rh_sps,rh_spm,rh_mpm,rh_position);
		printf("rh_range = %g rh_npass = %hu rh_nchan = %hu rh_epsr = %f\n",
            rh_range,rh_npass,rh_nchan,rh_epsr);
        printf("rh_top = %f rh_depth = %g rh_dtype = %x antname = %s rh_name = %s\n",
			rh_top,rh_depth,(char)rh_dtype,antname,rh_name);
		printf("text = %s rg_breaks = %hu rg_values = %p\n",
            text,rg_breaks,rg_values);
        printf("rh_nproc = %hu  proc_hist = %s  hdrPtr = %p\n",
            rh_nproc,proc_hist,hdrPtr);
        getch();
    }

	memset((void *)hdrPtr,0x00,sizeof(struct DztHdrStruct));
	switch (hdr_num)
	{   case 1:  hdrPtr->rh_tag = 0x00FF;  break;
		case 2:  hdrPtr->rh_tag = 0x01FF;  break;
		case 3:  hdrPtr->rh_tag = 0x02FF;  break;
        case 4:  hdrPtr->rh_tag = 0x03FF;  break;
    }
	hdrPtr->rh_data         = sizeof(struct DztHdrStruct) * rh_nchan;
	hdrPtr->rh_nsamp        = rh_nsamp;
    hdrPtr->rh_bits         = rh_bits;
    hdrPtr->rh_zero         = rh_zero;
    hdrPtr->rh_sps          = rh_sps;
	hdrPtr->rh_spm          = rh_spm;
	hdrPtr->rh_mpm          = rh_mpm;
    hdrPtr->rh_position     = rh_position;
    hdrPtr->rh_range        = rh_range;
    hdrPtr->rh_npass        = rh_npass;
    hdrPtr->rh_create.sec2  = create_date->sec2;
    hdrPtr->rh_create.min   = create_date->min;
	hdrPtr->rh_create.hour  = create_date->hour;
	hdrPtr->rh_create.day   = create_date->day;
	hdrPtr->rh_create.month = create_date->month;
	hdrPtr->rh_create.year  = create_date->year;
    _dos_getdate(&dosdate);
    _dos_gettime(&dostime);
	hdrPtr->rh_modif.sec2   = dostime.second/2;   /* second/2  (0-29) */
	hdrPtr->rh_modif.min    = dostime.minute;     /* minute    (0-59) */
	hdrPtr->rh_modif.hour   = dostime.hour;       /* hour      (0-23) */
	hdrPtr->rh_modif.day    = dosdate.day;        /* day       (1-31) */
	hdrPtr->rh_modif.month  = dosdate.month;      /* month     (1-12; 1=Jan, 2=Feb, etc. */
	hdrPtr->rh_modif.year   = dosdate.year-1980;  /* year-1980 (0-127; 1980-2107) */
	hdrPtr->rh_rgain        = 128;                                  /* offset */
	hdrPtr->rh_nrgain       = 2 + 4*(rg_breaks);                    /* size */
	hdrPtr->rh_text         = hdrPtr->rh_rgain + hdrPtr->rh_nrgain; /* offset */
	hdrPtr->rh_ntext        = 0;                                    /* size */
	if (text && text[0])
		hdrPtr->rh_ntext    = strlen(text) + 1;
	hdrPtr->rh_proc         = hdrPtr->rh_text + hdrPtr->rh_ntext; /* offset */
	hdrPtr->rh_nproc        = rh_nproc;                           /* size */

	/* check we don't exceed max header length */
	hdr_size = sizeof(struct DztHdrStruct);
	if ((hdrPtr->rh_proc + hdrPtr->rh_nproc) > hdr_size)
	{   /* we have problem */
		if ((hdrPtr->rh_text + hdrPtr->rh_ntext) > hdr_size)
		{   /* we have bigger problem */
			/* truncate text field, text offset OK, adjust proc offset */
			hdrPtr->rh_ntext = hdr_size - hdrPtr->rh_text - 2;
			hdrPtr->rh_proc  = hdr_size - 1;
			hdrPtr->rh_nproc = 0;  /* eliminate proc field */
		} else
		{   hdrPtr->rh_nproc = hdr_size - hdrPtr->rh_proc - 2;
		}
	}

	hdrPtr->rh_nchan        = rh_nchan;
	hdrPtr->rh_epsr         = rh_epsr;
	hdrPtr->rh_top          = rh_top;
	hdrPtr->rh_depth        = rh_depth;
	hdrPtr->rh_dtype        = rh_dtype;
	if (antname && antname[0])
        strncpy(hdrPtr->rh_antname,antname,13);
    switch (rh_nchan)
    {   case 1:  hdrPtr->rh_chanmask = 0x0801;  break; /* was 0x530x !! */
        case 2:  hdrPtr->rh_chanmask = 0x0803;  break;
        case 3:  hdrPtr->rh_chanmask = 0x0807;  break;
		case 4:  hdrPtr->rh_chanmask = 0x080F;  break;
	}
	if (rh_name && rh_name[0])
		strncpy(hdrPtr->rh_name,rh_name,8);
	hdrPtr->rh_chksum       = 0;   /* must be zero before calculation */

	/* gain, text and proc hist reside in hdrPtr->variable[] */
	*(unsigned short *)(cp + hdrPtr->rh_rgain) = rg_breaks;
	if (rg_breaks > 0)
		for (i=0; i<rg_breaks; i++)
			*(float *)(cp + hdrPtr->rh_rgain + 2 + 4*i) = rg_values[i];
	if (text && text[0])
		for (i=0; i<hdrPtr->rh_ntext; i++)
			*(cp + hdrPtr->rh_text + i) = text[i];
	if (proc_hist && proc_hist[0])
		for (i=0; i<hdrPtr->rh_nproc; i++)
			*(ucp + hdrPtr->rh_proc + i) = proc_hist[i];

	/* calculate checksum and store */
	for (i=0; i<512; i++)  checksum += *(usp + i);
	hdrPtr->rh_chksum = checksum;
}
/***************************** end of DZT_IO.C ******************************/
