#ifndef PCX_IO_H
#define PCX_IO_H 1.0
/*                                 PCX_IO.H
 *                               version 1.00
 *
 * This file contains the header structures for the PCX image file format
 * and prototypes for the functions in the source file PCX_IO.C.
 *
 * This header file should be included whenever the library PCX_IO.LIB is
 * linked with source/object code.
 *
 * Jeff Lucius  U.S. Geological Survey  Branch of Geophysics  Golden, CO
 * January 23, 1996
 *
 * References:
 *   James D. Murray and William vanRyper, 1994, Encyclopedia of Graphics
 *      File Formats: Sebastopol, CA, USA, O'Reilly & Assoc. Inc., 894 p. +
 *      1 CD-ROM.
 *   Dick Oliver, Scott Anderson, James McCord, Spyro Gumacs, and Bob Zigon,
 *      1993, Tricks of the Graphics Gurus: Carmel, IN, USA, SAMS Publishing,
 *      894 p. + 2 diskettes.
 *
 * The PCX graphics file storage format orginated with ZSoft's MS-DOS-based
 * PC Paintbrush program.  PCX files are organized into 2 or 3 sections:
 * header, data, and (optionally) VGA 256-color palette.  The first 128 bytes
 * of every PCX file is the header - a C structure - with the first 2 bytes
 * always set to 0x0A.  The image data follows, encoded using a simple
 * run-length scheme (RLE).  If a palette is present in the file (version 5
 * or later), it occupies the last 768 bytes, and at 769 bytes from the end
 * there must be the decimal 12 (hex 0x0C).
 *
\****************************************************************************/

/********************** Include required header files ***********************/
/* ANSI-compatible headers */
#include <stdio.h>    /* fread,ferror,clearerr */
#include <string.h>   /* memset */

/*************************** Manifest constants *****************************/
#define PCX_BUF_SIZE 2048  /* max size of compressed scanline buffer */

/****************************** New data types ******************************/
/* Make sure the following structures are packed on byte boundaries! */
#if defined(_INTELC32_)
    #pragma align (PcxHdrStruct=1)
    #pragma align (PcxVgaPaletteStruct=1)
#elif defined(__BORLANDC__)
    #pragma option -a-
#elif defined(_MSC_VER)
    #pragma pack(1)
#elif defined(__ZTC__)
    #pragma ZTC align 1
#elif defined(__WATCOMC__)
    #pragma pack (1)
#endif

/*  The PCX file header structure */
struct PcxHdrStruct
{   char   ID;                  /* PCX ID (always 0x0A)             */
    char   Version;             /* version number (0x05)            */
    char   Encoding;            /* encoding Scheme (0x01)           */
    char   BitsPerPixel;        /* bits per pixel (1, 2, 4 or 8)    */
         /* for 2-, 4-, 16- or 256-colors                           */
    short  X1;                  /* left side of image (usually 0)   */
    short  Y1;                  /* top side of image  (usually 0)   */
    short  X2;                  /* right side of image (pixels)     */
    short  Y2;                  /* bottom side of image (pixels)    */
         /* image size is (X2 - X1 + 1) pixels wide and
                          (Y2 - Y1 + 1) pixels high
            (X1,Y1) is upper left corner and (X2,Y2) is lower right */
    short  HorDpi;              /* horizontal pix/line or dots/in   */
    short  VerDpi;              /* vertical pix/column or dots/in     */
    char   EgaPalette[48];      /* 16-Color EGA palette             */
    char   Reserved1;           /* reserved (should be 0x00)        */
    char   NumBitPlanes;        /* number of color planes           */
                   /* color    bits/pixel   max       video         */
                   /* planes   per plane   colors     mode          */
                   /*   1          1           2   monochrome       */
                   /*   1          2           4   CGA              */
                   /*   3          1           8   EGA              */
                   /*   4          1          16   EGA & VGA        */
                   /*   1          8         256   Extended VGA     */
                   /*   3          8  16,777,216   Ext. VGA & XGA   */
             /* MaxNumColors = (1L << (BitsPerPixel * NumBitPlanes) */
    short  BytesPerLine;        /*  bytes in an unencoded scan line */
    short  PaletteType;         /*  1-monochrome; 2-gray scale      */
    /* following two added with ver. 4 so can usually ignore        */
    short  HorScrnSize;         /*  horizontal screen size          */
    short  VerScrnSize;         /*  vertical screen size            */
    char   Filler[54];          /*  reserved (should be all 0x00)   */
} ;   /* 128 bytes if tightly packed */

/*  PCX VGA palette */
struct PcxVgaPaletteStruct
{   unsigned char VgaPalette[768];     /* 256 VGA Color Palette  */
} ;

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

/********************* Global variables from pcx_pgms.c *********************/


/*************************  Function prototypes *****************************/
int  PcxDecodeScanLine(unsigned char *DecodedBuffer,int BufferSize,FILE *infile);
int  PcxEncodeScanLine(unsigned char *DecodedBuffer,unsigned char *EncodedBuffer,int DecodedSize);
void SetPcxHeader(int BitsPerPixel,int X1,int Y1,int X2,int Y2,int HorDpi,
        int VerDpi,int NumBitPlanes,int BytesPerLine,int PaletteType,
        int HorScrnSize,int VerScrnSize,struct PcxHdrStruct *HdrPtr);

#endif  /* PCX_IO_H */
