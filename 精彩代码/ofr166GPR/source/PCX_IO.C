/* PCX_IO.C - source code for functions in PCX_IO.LIB
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
 * To compile for use in PCX.LIB:
 *  icc /F /xnovm /zmod486 /zfloatsync /c PCX.C
 *
 *     /F removes the floating point emulator, reduces binary size
 *     /xnovm specifies no virtual memory manager, disk access is too slow
 *     /zmod486 generates specific 80486 instructions, speeds up program.
 *     /zfloatsync insures the FPU is operand-synchronized with the CPU.
 *     /c compiles only, no linking.
 *
 * Functions in this module:
 *   PcxDecodeScanLine
 *   PcxEncodeScanLine
 *   SetPcxHeader
 */

/* Include header files */
#include "pcx_io.h"

/* Declare globals */

/**************************** PcxDecodeScanLine() ***************************/
/*  Decode (uncompress) a PCX image file scan line.
 *
 *  PcxDecodeScanLine() decodes a buffer containing scan line data encoded
 *  using the PCX run-length encoding (RLE) algorithm.  The encoded data is
 *  read from a FILE stream, decoded, and then written to a pointer to a
 *  buffer passed to this function.
 *
 *  The PCX specification states (in so many words) that the run-length
 *  encoding of a pixel run should stop at the end of each scan line.
 *  However, some PCX encoders may continue the encoding of a pixel run on
 *  to the beginning of the next scan line, if possible.  This code,
 *  therefore, assumes that any pixel run can span scan lines.
 *
 *  NOTE: To check for decoding errors, the value returned by this function
 *        should be the same as the value of BufferSize (the length of an
 *        uncompressed scan line).
 *
 * Parameter list:
 *  unsigned char *DecodedBuffer - pointer to buffer to hold decoded image data
 *  int   BufferSize    - size of DecodedBuffer
 *  FILE *infile        - FILE pointer to the open input PCX image file
 *
 * Requires: <stdio.h>, "pcx_jl.h".
 * Calls: fread,ferror,clearerr.
 * Returns: Total number of pixels decoded from compressed scan line,
 *          or EOF (usually -1) if a file stream error occured.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Modified from:
 *   James D. Murray and William vanRyper, 1994, Encyclopedia of Graphics
 *      File Formats: Sebastopol, CA, USA, O'Reilly & Assoc. Inc., 894 p. +
 *      1 CD-ROM.
 * Date: January 18, 1996
 */
int PcxDecodeScanLine (unsigned char *DecodedBuffer,int BufferSize,FILE *infile)
{
	int  index = 0;            /* index into compressed scan line buffer */
	int  total = 0;            /* running total of decoded pixel values  */
	unsigned char byte;        /* data byte read from PCX file           */
	static unsigned char runcount = 0;  /* length of decoded pixel run   */
	static unsigned char runvalue = 0;  /* value of decoded pixel run    */

	/* If there is any data left over from the previous scan line write it
	   to the beginning of this scan line, using static variables.
	 */
	do
	{   /* Write the pixel run to the buffer */
		for (total+=runcount; runcount&&(index<BufferSize); runcount--,index++)
            DecodedBuffer[index] = runvalue;    /* assign value to buffer  */

        if (runcount)           /* encoded run ran past end of scan line   */
		{   total -= runcount;  /* subtract count not written to buffer    */
            return(total);      /* return number of pixels decoded         */
		}

        /* Get the next encoded run packet.
		   Read a byte of data.  If the two MBSs are 1 then this byte
		   holds a count value (0 to 63) and the following byte is the
		   data value to be repeated.  If the two MSBs are 0 then the
		   count is one and the byte is the data value itself.
		 */
		fread((void *)&byte,(size_t)1,(size_t)1,infile); /* ignore errors for speed */
		if ((byte & 0xC0) == 0xC0)     /* 2-byte code */
		{   /* get run count    */
			runcount = byte & 0x3F;
			/* get pixel value  */
			if ((fread((void *)&runvalue,(size_t)1,(size_t)1,infile)) < 1)
				return(EOF);
		}  else                        /* 1-byte code */
		{   runcount = 1;              /* run count is one */
			runvalue = byte;           /* pixel value */
		}
	} while (index < BufferSize);      /* read until the end of the buffer */

	if (ferror(infile))     /* file stream error; EOF or fread short count */
	{   clearerr(infile);
        return(EOF);
    } else
        return(total);      /* return number of pixels decoded */
}
/**************************** PcxEncodeScanLine() ***************************/
/*  Encode (compress) a PCX image file scan line.
 *
 *  PcxEncodeScanLine() encodes a buffer containing raw scan line data
 *  using the PCX run-length encoding (RLE) algorithm.  The encoded
 *  data is written to a second buffer passed to the function.
 *
 *  The main problem when encoding a scan line is that we never know how
 *  long the resulting encoded scan line will be.  Typically it will be
 *  shorter than the unencoded line, however, this type of RLE encoding
 *  can make the resulting encoded data larger in size than the unencoded
 *  data depending upon the data being encoded.
 *
 *  NOTE: It is therefore important that the buffer used to hold the encoded
 *        scan line be larger than what will typically be needed.  A size of
 *        2048 bytes should be more than plenty for typical VGA images.  The
 *        size of the buffer can be trimmed down to the size of the encoded
 *        data using realloc() and the return value of PcxEncodeScanLine()
 *        function, if necessary.
 *
 * Parameter list:
 *  unsigned char *DecodedBuffer - pointer to buffer holding unencoded data
 *  unsigned char *EncodedBuffer - pointer to buffer to hold encodeded scan line
 *  int   DecodedSize   - size of DecodedBuffer
 *
 * Requires: <stdio.h>, "pcx_jl.h".
 * Calls: fread.
 * Returns: number of bytes in compressed (encoded) scan line.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Modified from:
 *   Encyclopedia of Graphics File Formats, 1994, James D. Murray and William
 *      vanRyper: Sebastopol, CA, O'Reilly & Assoc. Inc., 894 p. + 1 CD-ROM.
 * Date: January 10, 1996
 */
int PcxEncodeScanLine (unsigned char *DecodedBuffer,unsigned char *EncodedBuffer,int DecodedSize)
{
	int  decoded_index = 0;   /* index into uncompressed (decoded) buffer */
	int  encoded_index = 0;   /* index into compressed (endoded) buffer */
	unsigned char runcount;   /* length of encoded pixel run */
	unsigned char runvalue;   /* value of encoded pixel run */

    while (decoded_index < DecodedSize)
    {   /* Get the run count of the next pixel value run.
           Pixel value runs are encoded until a different pixel value
           is encountered, the end of the scan line is reached, or 63
           pixel values have been counted.
         */
        runvalue = DecodedBuffer[decoded_index];
        for ( runcount = 1;
             (runvalue == DecodedBuffer[decoded_index + runcount]) &&
             ((decoded_index + runcount) < DecodedSize) &&
             (runcount < 63);
              runcount++)
			;  /* do nothing */

        /* Encode the run into a one or two-byte code.
           Multiple pixel runs are stored in two-byte codes.  If a single
           pixel run has a value of less than 64 then it is stored in a
           one-byte code.  If a single pixel run has a value of 64 to 255
           then it is stored in a two-byte code.
         */
        if (runcount > 1)    /* multiple pixel run */
        {   EncodedBuffer[encoded_index++] = runcount | 0xC0;
            EncodedBuffer[encoded_index++] = runvalue;
        }  else              /* single pixel run */
        {   if (DecodedBuffer[decoded_index] < 64)  /* 1-byte code */
            {   EncodedBuffer[encoded_index++] = runvalue;
            } else                                  /* 2-byte code */
            {   EncodedBuffer[encoded_index++] = runcount | 0xC0;
                EncodedBuffer[encoded_index++] = runvalue;
            }
        }
		decoded_index += runcount;  /* jump ahead to next pixel run value */
    }
    return(encoded_index);      /* Return the number of bytes written to buffer */
}
/****************************** SetPcxHeader() ******************************/
/*  Set the members of a PCX header structure.
 *
 * Parameter list:
 *  int BitsPerPixel - bits per pixel (1, 2, 4 or 8)
 *  int X1           - left side of image (usually 0)
 *  int Y1           - top side of image  (usually 0)
 *  int X2           - right side of image (pixels)
 *  int Y2           - bottom side of image (pixels)
 *  int HorDpi       - horizontal pix/line or dots/in
 *  int VerDpi       - vertical pix/line or dots/in
 *  int NumBitPlanes - number of color planes
 *  int BytesPerLine - bytes in an unencoded scan line
 *  int PaletteType  - 1-monochrome; 2-gray scale
 *  int HorScrnSize  - horizontal Screen Size
 *  int VerScrnSize  - vertical screen size
 *  struct PcxHdrStruct *HdrPtr - pointer to header structure
 *
 * Requires: <string.h>, "pcx_jl.h".
 * Calls: memset.
 * Returns: void.
 *
 * Author: Jeff Lucius   USGS   Branch of Geophysics   Golden, CO
 * Date: January 18, 1996
 */
void SetPcxHeader (int BitsPerPixel,int X1,int Y1,int X2,int Y2,
                   int HorDpi,int VerDpi,int NumBitPlanes,int BytesPerLine,
                   int PaletteType,int HorScrnSize,int VerScrnSize,
                   struct PcxHdrStruct *HdrPtr)
{
    memset((void *)HdrPtr,0x00,sizeof(struct PcxHdrStruct));
    HdrPtr->ID           = 0x0A;
    HdrPtr->Version      = 0x05;
    HdrPtr->Encoding     = 0x01;
	HdrPtr->BitsPerPixel = (char)BitsPerPixel;
    HdrPtr->X1           = (short)X1;
    HdrPtr->Y1           = (short)Y1;
    HdrPtr->X2           = (short)X2;
    HdrPtr->Y2           = (short)Y2;
    HdrPtr->HorDpi       = (short)HorDpi;
    HdrPtr->VerDpi       = (short)VerDpi;
    HdrPtr->NumBitPlanes = (char)NumBitPlanes;
    HdrPtr->BytesPerLine = (short)BytesPerLine;
    HdrPtr->PaletteType  = (short)PaletteType;
    HdrPtr->HorScrnSize  = (short)HorScrnSize;
    HdrPtr->VerScrnSize  = (short)VerScrnSize;
}
