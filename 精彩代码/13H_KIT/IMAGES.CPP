//**************************************************************************
// IMAGES.CPP -source file for basic video operations in mode 0x13.        *
// Copyright 1991 by the Gamers Programming Workshop, a function of the    *
// GAMERS forum, Compuserve. For more info e-mail 76605,2346.              *
//                                                                         *
// License is granted for use or modification of this code as long as      *
// this notice remains intact, and all improvements are listed in the      *
// version history below, and uploaded to the GAMERS forum. This code      *
// may not be used for any commercial purpose.                             *
//                                                                         *
// PCX stuff adapted from "BitMapped Graphics"  by Steve Rimmer (1990      *
// Windcrest)                                                              *
//**************************************************************************

//**************************************************************************
// Version history:                                                        *
//                                                                         *
// Version 1.0                                                             *
// Developed: May 2, 1991                                                  *
// Author:    Mark Betz, 76605, 2346                                       *
// Last update: July 5, 1991                                               *
//**************************************************************************

// INCLUDES ****************************************************************

#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <alloc.h>
#include <io.h>
#include <fcntl.h>
#include "images.hpp"
#include "keyboard.hpp"

// *************************************************************************

// local constants *********************************************************

const int DacWrite = 0x3C8;        // DAC write index register
const int DacRead  = 0x3C7;        // DAC read index register
const int DacData  = 0x3C9;        // DAC data register
const int input_status_1 = 0x3DA;  // Port addr. of VGA input status 1 reg.
const int vbi_mask = 0x8;          // test bit 3 of input_status_1

// *************************************************************************

// Variables ***************************************************************

static unsigned int GRAPH_SEG = 0xA000; // segment for video operations
static p_rec current;                   // palette record for fades

// *************************************************************************

// *************************************************************************
// *************************************************************************
// NOTE:
//        The assembler functions in this module do not save any of the
//        registers they use, with the exception of di and si, which are
//        handled by the standard stack frame. When calling them expect
//        register values to be altered.
//
// *************************************************************************
// *************************************************************************

// *************************************************************************
// reporterr() should be called anytime one of the routines in this module
// returns a code other that NoErr. Pass it the error code returned by the
// offending subroutine, and a max. 30 character string describing the loc-
// ation of the error.
// *************************************************************************

void reporterr(char type, char mess[30])
	{
	settextmode();
	sound(300);
	delay(200);
	sound(250);
	delay(150);
	nosound();
	switch (type)
		{
		case MemErr        : printf("An error occured allocating memory...");
							 break;
		case FileReadErr   : printf("An error occured reading a file...");
							 break;
		case FileWriteErr  : printf("An error occured writing a file...");
							 break;
		case FileMakeErr   : printf("An error occured creating a file...");
							 break;
		case FileOpenErr   : printf("An error occured opening a file...");
							 break;
		case FileFormatErr : printf("A file format error has occured...");
							 break;
		case SecondaryErr  : printf("An error has occured in a subroutine...");
							 break;
		case UnknownErr    : printf("An unknown error has occured...");
		}
	printf(" error occured in ");
	printf(mess);
	return;
	}

// *************************************************************************
// UnpackPcx() unpacks the RLE encoded PCX data from a file if pcx != NULL,
// or the source buffer if pcx==NULL, to the Dest buffer. Unpacks NumBytes
// bytes of decompressed data. (max 64000) This is a pretty dumb routine.
// It's fast, but it expects you to know the uncompressed size of the data
// (the NumBytes parameter), and you have to do all of the file and buffer
// set-up elsewhere. Basically it's a core decoding engine. The InitPcxFile()
// function above is provided for opening a PCX file and saving the header
// and palette in data. For buffering and decoding of pcx's with various
// fade types see the fadeinpcx() function below.
// *************************************************************************

// *************************************************************************
// NOTE: this function isn't called by the PVC class objects when they dis-
// play themselves. It is an earlier version of the algrotihm, and they have
// their own version of it. I left this in in case anyone wanted to use it.
// **************************************************************************

void  unpackpcx(FILE *pcx, const char far *source, char far *dest,
				unsigned int num_bytes)
	{
	unsigned int     bytes=0;       // counts unpacked bytes
	unsigned char    c;             // byte being processed
	unsigned int     src_cntr=0;    // pointer into source buffer
	unsigned int     runlen;        // length of packet

	do
		{
		// get a key byte
		if (pcx!=NULL)
		   c=fgetc(pcx);
		else
		   c=source[src_cntr++] & 0xff;
		// check if it's a packet
		if ((c & 0xc0) == 0xc0)
			{
			// and off the high bits
			runlen = (c & 0x3f);
			// get the run value
			if (pcx!=NULL)
				c=fgetc(pcx);
			else
				c = source[src_cntr++];
			// run it out
			while(runlen--) (dest[bytes++]=c);
			}
		else
			dest[bytes++]=c;
		} while (bytes<num_bytes);
	}

// *************************************************************************
// sets the segment address that will be used in all graphics operations
// within this module. Can be used to operate on a buffer offscreen. This
// is a fairly powerfull feature, as any of the functions provided which
// write or read video memory will act on the segment contained in GRAPH_
// SEG. If you allocate a 64k buffer, and call _setgraphseg() with it's
// segment you can then create the screen there, and block copy it to
// video ram when you're finished.
// *************************************************************************

void setgraphseg(unsigned int newseg)
	{
	GRAPH_SEG = newseg;
	}

// *************************************************************************
// call bios interrupt 0x10, function 13, subfunction 00 to set up mode
// 0x13 graphics.
// *************************************************************************

void setgraphmode()
	{
	asm	{
		xor ah, ah          // zero out ah, subfunction 00
		mov al, 0x13        // function 0x13
		int 0x10            // interrupt 0x10
		}
	}

// *************************************************************************
// call bios interrupt 0x10 in same manner as previous function to restore
// text mode
// *************************************************************************

void settextmode()
	{
	asm	{
		xor ah, ah
		mov al, 0x03
		int 0x10
		}
	}

// **************************************************************************
// wait_vbi() waits twice for the vertical blanking interval. Once to make
// sure any current vbi is completed, and once for the start of the next vbi
// **************************************************************************

void wait_vbi()
	{
	asm mov dx, input_status_1;
	test_1:
	asm	{
		in al, dx
		test al,0x8
		jnz test_1
		}
	test_2:
	asm	{
		in al, dx
		test al,0x8
		jz test_2
		}
	}


// *************************************************************************
// loads the vga dac by direct method via the DAC access ports. Disables
// interrupts during load of each triplet. Does not do snow checking. Call
// with start = first register to load, number = number of registers to
// load, and palette pointing to a structure of type PRec containing the
// palette data. If start or number out of range, or palette is null then
// the function returns.
// *************************************************************************

void loadpalette(int start, int number, const p_rec palette)
	{
	unsigned int i;
	if ((start>256) || (start<0) || ((start+number)>256))
		return;
	for (i=start;i<(start+number);i++)
		{
		asm cli;
		outportb(DacWrite,i);
		outportb(DacData,palette[i][Red]);
		outportb(DacData,palette[i][Green]);
		outportb(DacData,palette[i][Blue]);
		asm sti;
		}
	}

// *************************************************************************
// same as previous function only reads the dac data into the supplied
// palette structure.
// *************************************************************************

void readpalette(int start, int number, p_rec palette)
	{
	unsigned int i;
	if ((start>256) | (start<0) | ((start+number)>256))
		return;
	for (i=start;i<(start+number);i++)
		{
		asm cli;
		outportb(DacRead,i);
		palette[i][Red] = inportb(DacData);
		palette[i][Green] = inportb(DacData);
		palette[i][Blue] = inportb(DacData);
		asm sti;
		}
	}

// **************************************************************************
// clears a range of palette registers to zero
// **************************************************************************

void clrpalette(int start, int number)
	{
	unsigned int i;
	if ((start>256) | (start<0) | ((start+number)>256))
		return;
	for (i=start;i<(start+number);i++)
		{
		asm cli;
		outportb(DacWrite,i);
		outportb(DacData,0);
		outportb(DacData,0);
		outportb(DacData,0);
		asm sti;
		}
	}

// **************************************************************************
// fadepalettein() fades the dac palette from 0 (black) to the default pal-
// ette values passed in palette structure. Fade is done in 64 passes, and
// is very smooth. Operates on count registers starting with register start.
// NOTE: this funtion expect the dac palette registers to be zero'd before
// it is called. Call clrpalette() to clear the registers.
// **************************************************************************

void fadepalettein(int start, int count, const p_rec palette)
	{
	int i, j, cycle;
	memset((void *)current,0,sizeof(p_rec));
	for (cycle=0;cycle<64;cycle++)
		{
		i=start;
		wait_vbi();
		outportb(DacWrite,i);
		asm cli;
		for (;i<((start+count)/2);i++)
			{
			for (j=0;j<3;j++)
				{
				if ((64-cycle)<=palette[i][j])
					current[i][j]++;
				outportb(DacData,current[i][j]);
				}
			}
		asm sti;
		wait_vbi();
		asm cli;
		for (;i<(start+count);i++)
			{
			for (j=0;j<3;j++)
				{
				if ((64-cycle)<=palette[i][j])
					current[i][j]++;
				outportb(DacData,current[i][j]);
				}
			}
		asm sti;
		}
	}

// **************************************************************************
// fadepaletteout() fades the current palette to black. It operates on count
// registers starting with register start. NOTE: this function destroys the
// current contents of temp_pal (images.cpp internal palette structure).
// **************************************************************************

void fadepaletteout(int start, int count)
	{
	int i, j, cycle;
	readpalette(0,256,current);
	for (cycle=64;cycle>0;cycle--)
		{
		i=start;
		wait_vbi();
		outportb(DacWrite,i);
		asm cli;
		for (;i<((start+count)/2);i++)
			{
			for (j=0;j<3;j++)
				{
				if (current[i][j]!=0)
					current[i][j]--;
				outportb(DacData,current[i][j]);
				}
			}
		asm sti;
		wait_vbi();
		asm cli;
		for (;i<(start+count);i++)
			{
			for (j=0;j<3;j++)
				{
				if (current[i][j]!=0)
					current[i][j]--;
				outportb(DacData,current[i][j]);
				}
			}
		asm sti;
		}
	clearscr(0);
	}

// *************************************************************************
// clear the graphics screen to the color provided
// *************************************************************************

void clearscr(int color)
	{
	asm {
		mov es, GRAPH_SEG
		xor di, di
		mov ax, color
		mov cx, 32000
		rep stosw
		}
	}

// *************************************************************************
// fill the rectangular area bounded by (tlx,tly)(brx,bry) with color.
// tlx = top left x, tly = top left y, brx = bottom right x, bry = bottom
// right y.
//
// This function needs to be recoded in assembler
// *************************************************************************

void barfill(int tlx, int tly, int brx, int bry, int color)
	{
	int row;
	void far *line;
	for (row=tly;row<bry;row++)
		{
		line = MK_FP(GRAPH_SEG,((row*320)+tlx));
		_fmemset(line,color,(brx-tlx));
		}
	}

// *************************************************************************
// set the pixel at coordinate x,y to color
// This could be translated into assembler, but is probably fast enough
// *************************************************************************

void writepixel(int x, int y, int color)
	{
	*(char far *)MK_FP(GRAPH_SEG,(y*320)+x)=color;
	}

// *************************************************************************
// read the color of the pixel at coordinate x,y
// Could also be done in assembler, but may be fast enough as is
// *************************************************************************

char readpixel(int x, int y)
	{
	return(*(char far *)MK_FP(GRAPH_SEG,(y*320)+x));
	}

// *************************************************************************
// returns a void type pointer to video memory at pixel coordinate x,y
// *************************************************************************

void far *xy_to_ptr(int x, int y)
	{
	return((char *)MK_FP(GRAPH_SEG,(y*320)+x));
	}

// *************************************************************************
// The following three functions make up the line drawing routine
//
// Draws a line in vga mode 13h only from (x0,y0) to (x1,y1) in color color.
// Drawing code adapted from Power Graphics Programming, Michael Abrash
// *************************************************************************

// *************************************************************************
// Octant0() and Octant1() are support functions for line()
// Adapted from "Power Graphics Programming", Michael Abrash.
// *************************************************************************

void static near Octant0(unsigned int x0,unsigned int y0,unsigned int deltaX,
			 unsigned int deltaY, int Xdirection, int color)
	{
	int deltaYx2;
	int deltaYx2minusdeltaXx2;
	int errorterm;

	deltaYx2 = deltaY*2;
	deltaYx2minusdeltaXx2=deltaYx2-(int)(deltaX*2);
	errorterm=deltaYx2-(int)deltaX;

	writepixel(x0,y0,color);
	while (deltaX--)
		{
		if (errorterm>=0)
			{
			y0++;
			errorterm+=deltaYx2minusdeltaXx2;
			}
		else
			errorterm+=deltaYx2;
		x0+=Xdirection;
		writepixel(x0,y0,color);
		}
	}

void static near Octant1(unsigned int x0,unsigned int y0,unsigned int deltaX,
			 unsigned int deltaY, int Xdirection, int color)
	{

	int deltaXx2;
	int deltaXx2minusdeltaYx2;
	int errorterm;

	deltaXx2=deltaX*2;
	deltaXx2minusdeltaYx2=deltaXx2-(int)(deltaY*2);
	errorterm=deltaXx2-(int)deltaY;

	writepixel(x0,y0,color);
	while (deltaY--)
		{
		if (errorterm>=0)
			{
			x0+=Xdirection;
			errorterm+=deltaXx2minusdeltaYx2;
			}
		else
			errorterm+=deltaXx2;
		y0++;
		writepixel(x0,y0,color);
		}
	}

// **************************************************************************
// takes the initial call and manages the line drawing
// Adapted from "Power Graphics Programming", Michael Abrash
// **************************************************************************

void line(int x0, int y0, int x1, int y1, int color)
	{
	int deltaX, deltaY;
	int temp;

	if (y0>y1)
		{
		temp=y0;
		y0=y1;
		y1=temp;
		temp=x0;
		x0=x1;
		x1=temp;
		}

	deltaX=x1-x0;
	deltaY=y1-y0;

	if (deltaX > 0)
		{
		if (deltaX>deltaY)
			Octant0(x0,y0,deltaX,deltaY,1,color);
		else
			Octant1(x0,y0,deltaX,deltaY,1,color);
		}
	else
		{
		deltaX= -deltaX;
		if (deltaX > deltaY)
			Octant0(x0,y0,deltaX,deltaY,-1,color);
		else
			Octant1(x0,y0,deltaX,deltaY,-1,color);
		}
	}

// **************************************************************************
// getimage() grabs the pixel values in the rectangle marked by (x0,y0) on
// the top left, and (x1,y1) on the bottom right. The data is placed in buff.
//
// NOTE: This function needs translating into assembler
// **************************************************************************

void getimage(int x0, int y0, int x1, int y1, char far *buff)
	{
	int i;
	int deltaY=(y1-y0);
	int xdim=(x1-x0);

	for (i=0;i<=deltaY;i++)
		{
		_fmemcpy((void *)&buff[i*xdim],xy_to_ptr(x0,(y0+i)),xdim);
		}
	}

// **************************************************************************
// putimage() copies the data in *buff to a rectangular area of the screen
// marked by (x0,y0) on the top left, and (x1,y1) on the bottom right.
//
// NOTE: This function needs translating into assembler
// **************************************************************************

void putimage(int x0, int y0, int x1, int y1, char far *buff)
	{
	int i;
	int deltaY=(y1-y0);
	int xdim=(x1-x0);

	for (i=0;i<=deltaY;i++)
		{
		_fmemcpy(xy_to_ptr(x0,(y0+i)),(void *)&buff[i*xdim],xdim);
		}
	}

// *************************************************************************
// copyimage copies a rectangular image bounded by (x0, y0) at top left, and
// (x1, y1) at bottom right, to the video buffer at (putx, puty). The func-
// tion is very fast, suitable for copying blocks of a virtual screen into
// the physical screen for animation. It does no clipping, or checking for
// the vertical blanking interval. If using it to copy from system ram to
// the video buffer, make sure to pass a pointer to the buffer in src_buff,
// and set GRAPH_SEG = to the video buffer before the call.
// *************************************************************************

void copyimage(int x0, int y0, int x1, int y1, int putx, int puty,
			   void far *src_buf)
	{
	int deltaY=(y1-y0);
	int xdim=(x1-x0);
	asm	{
		mov es, GRAPH_SEG           // establish GRAPH_SEG as destination seg
		mov bx, 320                 // increment value to move down one line
		mov ax, puty                // calculate the start of the first line
		mul bx                      // for the destination
		add ax, putx
		mov di, ax
		mov ax, y0                  // caluclate the start of the first line
		mul bx                      // for the source
		add ax, x0
		push ds
		lds si, src_buf
		add si, ax
		mov dx, deltaY              // set dx up as loop counter
		cld                         // clear the direction flag
		}
	mainloop:                       // frame drawing loop
	asm	{
		mov cx, xdim                // cx == frame width in pixels
		push si                     // save the line pointers
		push di
		shr cx, 1                   // optimized copy
		rep movsw
		rcl cx, 1
		rep movsb
		pop di                      // restore the line pointers
		pop si
		add di, 320                 // point to next line in source, dest
		add si, 320
		dec dx                      // adjust the loop counter
		jnz mainloop
		pop ds
		}
	}

// *************************************************************************
// doSplitVerticalWipe causes the picture in pic_buf to be displayed by
// wiping from left and right to the center. If called with a null pointer
// it will clear the screen using palette register 0. Waits for the vertical
// blanking interval, producing a smooth motion. This function really doesn't
// need to be done in assembler, as the vbi test is the limiting factor (see
// below)
// *************************************************************************

void  doSplitVerticalWipe(void far *pic_buf)
	{
	unsigned leftx = 0;
	unsigned rightx = 319;
	unsigned run_y;                    // counts rows
	char far *lsrc_ptr;                // pointer to left column in source
	char far *ldst_ptr;                // pointer to left column in dest
	char far *rsrc_ptr;                // pointer to right column in source
	char far *rdst_ptr;                // pointer to right column in dest

	if (pic_buf != NULL)
		{
		while (rightx > leftx)
			{
			ldst_ptr = (char far *)xy_to_ptr(leftx, 0);
			lsrc_ptr = (char far *)pic_buf+leftx;
			rdst_ptr = (char far *)xy_to_ptr(rightx, 0);
			rsrc_ptr = (char far *)pic_buf+rightx;
			wait_vbi();
			for (run_y = 0; run_y < 200; run_y++)
				{
				*ldst_ptr = *lsrc_ptr;
				*rdst_ptr = *rsrc_ptr;
				ldst_ptr += 320;
				lsrc_ptr += 320;
				rdst_ptr += 320;
				rsrc_ptr += 320;
				}
			leftx++;
			rightx--;
			}
		}
	else
		{
		while (rightx > leftx)
			{
			ldst_ptr = (char far *)xy_to_ptr(leftx, 0);
			rdst_ptr = (char far *)xy_to_ptr(rightx, 0);
			wait_vbi();
			for (run_y = 0; run_y < 200; run_y++)
				{
				*ldst_ptr = 0;
				*rdst_ptr = 0;
				ldst_ptr += 320;
				rdst_ptr += 320;
				}
			leftx++;
			rightx--;
			}
		}
	}

// **************************************************************************
// doSplitHorizWipe displays the picture in pic_buf by wiping it down from
// top and bottom. If it's called with a null pointer it clears the screen
// with palette register 0. Waits for vertical blanking interval, producing
// a smooth wipe. This function really didn't need to be done in assembler.
// I realized after I did it that the test for the vertical blanking interval
// was the limiting factor, not the execution time. Lessons learned! <g>.
//
// This function moves the image data by copying lines to the top and bottom
// of the screen alternately. The key to this process is the value in the
// unsigned integer adjust. In the setup es:di and ds:si are set up to point
// to the first line of the dest and source memory areas respectively. The
// adjust variable is intialized to 64000, which, when added to the offset of
// the first line, yields the offset of the last. Then adjust is decremented,
// and subtracted from the offset of the last line to get the offset of the
// second, and so on. The value of adjust is not used directly, but is loaded
// into the bx register. Keep an eye on si and di, as they are saved and
// restored quite often in all of these dissolve functions.
// **************************************************************************

void  doSplitHorizWipe(void far *pic_buf)
	{
	unsigned adjust = 64000U;    // used to adjust di and si
	asm {
		mov dx, input_status_1   // used to test for vbi later
		mov cx, 200              // line counter
		mov bx, adjust           // bx stores adjust value
		mov es, GRAPH_SEG        // else get video segment into es
		xor di, di               // zero out the offset
		push ds
		lds si, pic_buf          // get pic_buf into es:di
		mov ax, ds               // check for a null pointer
		cmp ax,0
		je clearscr              // if it's null clear the screen
		cld                      // clear the direction flag
		}
	mainloop1:                   // outer line loop
	asm {
		push cx                  // save loop counter
		mov cx, 160              // move 160 words
		push si                  // save offsets
		push di
		}
	test_11:                     // these lines wait for vbi
	asm	{
		in al, dx                // first test, out of vbi
		test al,0x8
		jnz test_11
		}
	test_12:
	asm	{
		in al, dx                // second test, in vbi
		test al,0x8
		jz test_12

		rep movsw                // copy words to video segment
		pop di                   // restore the original offsets
		pop si
		sub bx, 320              // take one line off of bx
		add si, bx               // add it to the offsets
		add di, bx
		mov cx, 160              // move 160 more words
		push si                  // save the offsets
		push di
		rep movsw                // copy the data to video seg
		pop di                   // restore the offsets
		pop si
		sub bx, 320              // take 1 line off of bx
		sub si, bx               // subtract it from the offsets
		sub di, bx
		pop cx                   // restore the outer loop counter
		loop mainloop1           // do next two lines
		jmp done                 // skip the next part
		}
	clearscr:
	asm cld;                     // clear the direction flag

	mainloop2:
	asm {
		push cx                  // save the loop counter
		mov cx, 160              // move 160 words
		push di                  // save the offset
		}
	test_21:                     // these lines test for vbi
	asm	{
		in al, dx                // first test, out of vbi
		test al,0x8
		jnz test_21
		}
	test_22:
	asm	{
		in al, dx                // second test, in vbi
		test al,0x8
		jz test_22
		xor ax, ax               // clear ax, value to be stored==0
		rep stosw                // clear the line
		pop di                   // restore the offset
		sub bx, 320              // take one line from bx
		add di, bx               // add it to the offset
		mov cx, 160              // mov 160 more words
		push di                  // save the offset
		rep stosw                // copy the data to video buff
		pop di                   // restore the offset
		sub bx, 320              // take one line from bx
		sub di, bx               // subtract it from the offset
		pop cx                   // restore the loop counter
		loop mainloop2
		}
	done:
	asm pop ds;
	}

// **************************************************************************
// doSlideVerticalWipe causes the image in pic_buf to be displayed by sliding
// it in from both sides toward the center. Does not check pic_buff for a non
// null address. Waits for vertical blanking interval between writes, produc-
// ing a smooth image motion.
//
// This function basically captures frames from the left and right sides of
// the source image and displays them in the video buffer so that the image
// appears to be sliding onto the screen. This is done by starting with very
// narrow slices, which are grabbed from the center of the source, and disp-
// layed at the outside edges of the screen. As the slices get wider they re-
// main rooted to the edges of the screen, so that they appear to slide into
// the center. The outer loop iterates 160 times, or half the screen width.
// On each iteration the width of the frame (i_wid) is increased by one, and
// es:di, ds:si point to the top left corner of the source frame.
// **************************************************************************

void doSlideVerticalWipe(void far *pic_buf)
	{
	int i_wid, x0, putx;
	char ctrl;                   // controls branching

	asm	{
		mov i_wid, 0             // zero out the outer loop counter
		push ds                  // save the data segment
		mov es, GRAPH_SEG        // get the video segment into es
		}
	lefthalf:
	asm {
		mov ax, 160              // get center line into ax
		sub ax, i_wid            // i_wid is the offset from center
		mov x0, ax               // x0 is the top left of the source block
		mov putx, 0              // putx is the top left of the dest block
		mov ctrl, 0
		jmp doframe              // display the frame
		}

	righthalf:
	asm {
		mov ax, 160              // mostly same as above except for
		mov x0, ax               // right side
		mov ax, 319
		sub ax, i_wid
		mov putx, ax
		mov ctrl, 1
		jmp doframe
		}
	checkloop:
	asm {
		inc i_wid
		cmp i_wid, 160           // have 160 iterations
		jna lefthalf
		pop ds                   // restore data segment
		}
	return;                      // ** exit point **

	doframe:
	asm {
		mov di, putx             // di == top left corner of destination
		mov dx, input_status_1   // used to test for vbi below
		lds si, pic_buf          // get picture pointe into ds:si
		add si, x0               // set si == top left corner of source block
		}
	test_1:                      // these lines test for the vbi
	asm	{
		in al, dx                // first test, out of vbi
		test al,0x8
		jnz test_1
		}
	test_2:
	asm	{
		in al, dx                // second test, in vbi
		test al,0x8
		jz test_2
		mov dx, 200              // set dx up as loop counter
		cld                      // clear the direction flag
		}
	mainloop:                    // frame drawing loop
	asm	{
		mov cx, i_wid            // i_wid is equal to the block width
		push si                  // save the offsets
		push di
		shr cx, 1                // optimized copy technique, cx==number of
		rep movsw                // number of bytes/2
		rcl cx, 1                // now, if cx was odd, cx == 1, else cx ==0
		rep movsb                // catches any odd byte  (thanks to Gus S.)
		pop di                   // restore the offsets
		pop si
		add di, 320              // point to the next line
		add si, 320
		dec dx                   // adjust the outer loop counter
		jnz mainloop

		cmp ctrl, 0              // if last copy was lefthalf do righthalf
		je righthalf
		jmp checkloop
		}
	}

// **************************************************************************
// doSlideHorizWipe operates almost exactly like doSlideVerticalWipe above,
// except that the image slides in from top and bottom instead of from left
// and right. Like the other wipes it uses the wipe to clear the screen if
// called with pic_buf == NULL.
//
// This function operates almost exactly like the vertical version directly
// above.
// **************************************************************************

void  doSlideHorizWipe(void far *pic_buf)
	{
	int i_wid, y0, puty;
	char ctrl;                   // controls branching

	asm	{
		mov i_wid, 1             // zero out the outer loop counter
		push ds                  // save the data segment
		mov es, GRAPH_SEG        // get the video segment into es
		}
	tophalf:
	asm {
		mov ax, 100              // get center line into ax
		sub ax, i_wid            // i_wid is the offset from center
		mov bx, 320
		mul bx
		mov y0, ax               // x0 is the top left of the source block
		mov puty, 0              // putx is the top left of the dest block
		mov ctrl, 0
		jmp doframe              // display the frame
		}

	bottomhalf:
	asm {
		mov ax, 99               // mostly same as above except for
		mov bx, 320
		mul bx
		mov y0, ax               // right side
		mov ax, 199
		sub ax, i_wid
		mul bx
		mov puty, ax
		mov ctrl, 1
		jmp doframe
		}
	checkloop:
	asm {
		inc i_wid
		cmp i_wid, 100           // have 160 iterations
		jna tophalf
		pop ds                   // restore data segment
		}
	return;                      // ** exit point **

	doframe:
	asm {
		mov di, puty             // di == top left corner of destination
		mov dx, input_status_1   // used to test for vbi below
		lds si, pic_buf          // get picture pointe into ds:si
		add si, y0               // set si == top left corner of source block
		}
	test_1:                      // these lines test for the vbi
	asm	{
		in al, dx                // first test, out of vbi
		test al,0x8
		jnz test_1
		}
	test_2:
	asm	{
		in al, dx                // second test, in vbi
		test al,0x8
		jz test_2
		mov dx, i_wid            // set dx up as loop counter
		cld                      // clear the direction flag
		}
	mainloop:                    // frame drawing loop
	asm	{
		mov cx, 160              // i_wid is equal to the block width
		push si                  // save the offsets
		push di
		rep movsw                // number of bytes/2
		pop di                   // restore the offsets
		pop si
		add di, 320              // point to the next line
		add si, 320
		dec dx                   // adjust the outer loop counter
		jnz mainloop

		cmp ctrl, 0              // if last copy was lefthalf do righthalf
		je bottomhalf
		jmp checkloop
		}
	}

// *************************************************************************
// doVerticalDissolve causes the image stored in pic_buf to be displayed by
// fading in multiple vertical slices. It is the vertical equivalent of the
// doHorizDissolve function. When called with a null pointer it clears the
// screen.
//
// The function moves the image data to the screen by iterating through
// 16 vertical slices. On each iteration two lines are drawn, at an equal
// distance on either side of the center mark for that section. The outer-
// most loop counts the lines; it takes 10 iterations to fill one slice. The
// innermost loop cycles through the 16 slices. So, the first slice gets two
// lines, the second gets two, on up to the 16th, and then the first gets
// two more, and so on. Keep an eye on si and di.
// *************************************************************************

void  doVerticalDissolve(void far *pic_buf)
	{
	int base_row;                //  base_row stores the line offset from x
	asm {
		mov es, GRAPH_SEG        // get the video segment into es
		mov base_row, 0          // start out drawing on the x pixel
		mov cx, 11               // outer loop, 11 pixels to each side of
		cld                      // the x pixel (in si and di) will be drawn
		push ds                  // save the ds register value
		lds si, pic_buf          // get the picture pointer into ds:si
		mov ax, ds               // check for a NULL pointer and jmp to
		cmp ax, 0                // clearscr if found
		je clearscr
		}
	mainloop:
	asm {
		push cx                  // save the outer loop counter
		push si                  // save original offset to byte 0 of pic
		mov cx, 16               // process 16 vertical slices
		add si, 9                // first x mark is on 9,0
		mov di, 9
		}
	drawloop:
	asm {
		push cx                  // save the inner loop counter
		push si                  // and the x mark, source and dest
		push di
		add di, base_row         // add in the offset from x for first line
		add si, base_row
		mov cx, 200              // plot 200 pixels
		}
	lineloop1:                   // this loop draws a vertical line
	asm {
		movsb                    // move one byte
		add si, 319              // move to next row
		add di, 319
		loop lineloop1           // do another

		pop di                   // restore the starting points
		pop si
		push si                  // save them again
		push di
		sub di, base_row         // drawing on the other side this time
		sub si, base_row
		mov cx, 200              // plot 200 pixels
		}
	lineloop2:
	asm {
		movsb                    // same as above
		add si, 319
		add di, 319
		loop lineloop2

		pop di                   // restore the starting point
		pop si
		pop cx                   // restore the inner loop counter
		add si, 20               // adjust pointer to the next slice
		add di, 20
		loop drawloop            // process it
		inc base_row             // increment the drawing offset
		pop si                   // restore the offset to byte 0 of pic
		pop cx                   // restore the outer loop counter
		loop mainloop            // do it all again
		jmp done
		}
	clearscr:                    // this block functions exactly as the
	asm {                        // previous one but clears the screen using
		push cx                  // palette register 0
		mov cx, 16
		mov di, 9
		}
	drawloop2:
	asm {
		push cx
		push di
		add di, base_row
		xor ax, ax
		mov cx, 200
		}
	lineloop21:
	asm {
		stosb
		add di, 319
		loop lineloop21

		pop di
		push di
		sub di, base_row
		xor ax, ax
		mov cx, 200
		}
	lineloop22:
	asm {
		stosb
		add di, 319
		loop lineloop22
		pop di
		pop cx
		add di, 20
		loop drawloop2
		inc base_row
		pop cx
		loop clearscr
		}
	done:
	asm pop ds;                  // restore the data segment
	}

// **************************************************************************
// doHorizDissolve causes the image in pic_buf to be displayed by dissolving
// it in in 10 horizontal bands. If called with pic_buf==NULL it clears the
// screen by the same method.
//
// This function processes the data much like the Vertcal Dissolve funtion
// above. The screen in this case is divided into 10 horizontal slices. The
// center line starting points of these slices begin at 2880 bytes into the
// video buffer, and every 6400 bytes after that. Like the function above it
// cycles through the slices drawing two lines at each, progressively further
// from the center line with each iteration of the outer loop. Like the func-
// tion above, you have to keep a carefull eye on di and si to understand it.
// **************************************************************************

void  doHorizDissolve(void far *pic_buf)
	{
	int base_line;
	asm {
		mov base_line, 0          // base_line is an offset from each line
		mov es, GRAPH_SEG         // get video segment into es
		mov bx, 320               // bx * base_line == adjust value for si,di
		mov cx, 20                // outer loop 20 iterations
		cld                       // clear the direction flag
		push ds                   // save the ds register value
		lds si, pic_buf           // get pic_buf into ds:si
		mov ax, ds                // if pic_buf == NULL jump to clearscr
		cmp ax, 0
		je clearscr
		}
	mainloop1:                    // main drawing loop, 20 iterations...
	asm {                         // the number of offset lines to draw
		push cx                   // save the counter
		mov cx, 10                // inner loop, 10 iterations, draws lines
		push si                   // save the offset of the pic_buf pointer
		add si, 60480             // 60480, address of first line processed
		mov di, 60480
		mov dx, input_status_1    // used below to test for vbi
		}
	test_11:                      // these lines test for the vbi
	asm	{
		in al, dx                 // first test, out of vbi
		test al,0x8
		jnz test_11
		}
	test_12:
	asm	{
		in al, dx                 // second test, in vbi
		test al,0x8
		jz test_12
		}
	drawloop1:                    // drawloop processes the 10 base lines,
	asm {                         // drawing 2 lines at offset ax from base
		push cx                   // save the counter
		push si                   // save the offset to the current base
		push di
		mov ax, base_line         // calculate the pointer offset from base
		mul bx                    // by multiplying base_line * 320
		sub si, ax                // subtract the offset from source and
		sub di, ax                // dest base pointers, and
		mov cx, 160               // draw the first line
		rep movsw
		pop di                    // restore the base pointers
		pop si
		push si                   // save em again
		push di
		add si, ax                // add the offset to the source and dest
		add di, ax                // base pointers and
		mov cx, 160               // draw the second line
		rep movsw
		pop di                    // restore the base pointers
		pop si
		pop cx                    // restore the inner loop counter
		sub si, 6400              // adjust base pointers to point to the
		sub di, 6400              // next base, 20 lines up the screen
		loop drawloop1            // process the next section
		inc base_line             // increment the base offset factor
		pop si                    // restore the original pic_buf offset
		pop cx                    // restore the outer loop counter
		loop mainloop1            // do it all over again
		jmp done                  // finished
		}
	clearscr:                     // this section executes if pic_buf==NULL
	asm {
		push cx                   // save the outer loop counter
		mov cx, 10
		mov di, 60480             // we can ignore ds:si since no source
		mov dx, input_status_1    // used to test for vbi below
		}
	test_21:                      // these lines test for the vbi
	asm	{
		in al, dx                 // first test, out of vbi
		test al,0x8
		jnz test_21
		}
	test_22:
	asm	{
		in al, dx                 // second test, in vbi
		test al,0x8
		jz test_22
		}
	drawloop2:
	asm {
		push cx                  // save the inner loop counter
		push di                  // save the base offset
		mov ax, base_line        // calculate the drawing offset from base
		mul bx
		sub di, ax               // subtract the offset from the base pointer
		push ax                  // save the offset
		xor ax, ax               // clear ax for use in stosw instruction
		mov cx, 160
		rep stosw                // clear the first line
		pop ax                   // restore the offset
		pop di                   // restore the base pointer
		push di                  // save it again
		add di, ax               // add the offset to the base pointer
		xor ax, ax               // clear ax for use in stosw instruction
		mov cx, 160
		rep stosw                // clear the second line
		pop di                   // restore the base pointer
		pop cx                   // restore inner loop counter
		sub di, 6400             // adjust base pointer to 20 lines up screen
		loop drawloop2           // process the next section
		inc base_line            // increment the offset factor
		pop cx                   // restore the outer loop counter
		loop clearscr
		}
	done:                        // finished
	asm pop ds;                  // restore ds value
	}



void  doSparkleDissolve(void far *pic_buf)
	{
	}



// *************************************************************************
// member function definitions for class font, declared in images.hpp
// *************************************************************************

// *************************************************************************
// constructor takes a char * filepath. It opens the fontfile, gets the
// font header into the member structure font_hdr, then allocates memory
// and reads the font data into it. Errors cause the constructor set the
// font status to 0, then return.  The status can be checked via the
// font::getstatus() method.
// *************************************************************************

font::font(char *f_spec)
	{
	int handle;
	int readsize;
	int bytes_read=0;
	char cursor_size;

	if ((handle=open(f_spec,O_RDONLY|O_BINARY))==-1)
		{
		status = FileOpenErr;
		return;
		}
	bytes_read+=(read(handle,&f_hdr,sizeof(f_hdr)));
	bytes_read+=(read(handle,&width[0],sizeof(width)));
	cursor_size=(f_hdr.fram_wid*f_hdr.fram_hit);
	if (((void far *)cursor_mask=malloc(cursor_size)) ==NULL)
		{
		status = MemErr;
		return;
		}
	bytes_read+=(read(handle,(void far*)cursor_mask,cursor_size));
	readsize=(filelength(handle)-bytes_read);
	if (((void far *)font_ptr=malloc(readsize)) ==NULL)
		{
		free(cursor_mask);
		status = MemErr;
		return;
		}
	read(handle,(void far *)font_ptr,readsize);
	close(handle);
	fore_color=0;
	setstyle(0,15,1,opaque,4);
	status=1;
}

// *************************************************************************
// destructor disposes of the memory allocated to the font
// *************************************************************************

font::~font()
	{
	free(font_ptr);
	font_ptr=NULL;
	free(cursor_mask);
	cursor_mask=NULL;
	}

// *************************************************************************
// get_width() returns the proportional width in pixels of char c
// *************************************************************************

char font::get_width(char c)
	{
	if ((c>=f_hdr.ascii_first)&&(c<=f_hdr.ascii_last))
		return(width[c-f_hdr.ascii_first]);
	else
		return(0);
	}

// *************************************************************************
// put_char displays one bitmapped character at the passed x,y location
// *************************************************************************

void font::put_char(int x, int y, char c)
	{
	int j;
	int i;
	unsigned char src,dst;
	char far *p;
	int char_ofs;
	char_ofs=(c-f_hdr.ascii_first)*((f_hdr.fram_wid*f_hdr.fram_hit));
	(void far *)p=MK_FP(FP_SEG(font_ptr),FP_OFF(font_ptr)+char_ofs);
	for(j=0;j<f_hdr.fram_hit;j++)
		{
		for (i=0;i<f_hdr.fram_wid;i++)
			{
			src=p[(j*f_hdr.fram_wid)+i];
			if (src==f_hdr.f_color)
				{
				dst=fore_color;
				writepixel(x+i,y+j,dst);
				}
			else
				{
				if (see_thru==opaque)
					{
					dst=back_color;
					writepixel(x+i,y+j,dst);
					}
				}
			}
		}
	}

// *************************************************************************
// returns the current foreground color setting for text output
// *************************************************************************

int font::getforecolor()
	{
	return(fore_color);
	}

// *************************************************************************
// returns the current background color setting for text output
// *************************************************************************

int font::getbackcolor()
	{
	return(back_color);
	}

// *************************************************************************
// call this function to set the style parameters for text output. f_grnd=
// foreground color, b_grnd=background color, char_tab=pixels between
// characters, opacity={trans,opaque}, when opacity=trans the background
// pixels in the font are ingored, when opacity=opaque the background pixels
// are set to the color value passed in b_grnd, space_wid=width in pixels
// of ascii character 32 (20h), the space character.
// *************************************************************************

void font::setstyle(int f_grnd,int b_grnd,short char_tab,
					opacity o, char space_wid)
	{
	spacing=char_tab;
	see_thru=o;
	fore_color=f_grnd;
	back_color=b_grnd;
	width[32-f_hdr.ascii_first]=space_wid;
	}

// *************************************************************************
// ouput the string pointed to by str at the given x,y coordinate.
// *************************************************************************

void font::show(int x, int y, char *str)
	{
	int i;
	int j;
	for (j=0;str[j]!=0;j++);   // one line gets length of string
	for (i=0;i<j;i++)
		{
		put_char(x,y,str[i]);
		x += get_width(str[i])+spacing;
		}
	}

// *************************************************************************
// read a string from the keyboard, echo to screen at x,y. Reads until it
// encouters a carriage return <ascii 13>, or the string reaches width
// bytes in length, whichever occurs first. Displays a blinking cursor using
// the cursor mask loaded with the font. Backspace enabled.
// *************************************************************************

void font::readstr(int x, int y, char *str, char length, char mask)
	{
	char c='\0';                   // used in call to getfilteredkey()
	extnd scan=NO_EXT;             // ditto
	int cursor_x=x;                // cursor position/next letter position
	int cursor_y=y;                // ditto
	char cur_toggle=0;             // cursor display state
	unsigned int tick;             // last clock reading
	volatile unsigned int *clock=(unsigned int *)MK_FP(0x0040,0x006C);
	int str_len=0;                 // input string length
	int i,j;                       // generic counters

	str[str_len]='\0';
	while((c!=13)&&(str_len<length))
		{
		do
			{
			tick= *clock;
			while ((*clock<(tick+3))&&(!kbhit()));
			cur_toggle = !cur_toggle;
			for(j=0;j<f_hdr.fram_hit;j++)
				{
				for (i=0;i<(f_hdr.fram_wid-1);i++)
					{
					if ((cur_toggle==0)||(cursor_mask[(j*f_hdr.fram_wid+i)]==0))
						writepixel(cursor_x+i,cursor_y+j,back_color);
					else
						writepixel(cursor_x+i,cursor_y+j,fore_color);
					}
				}
			} while ((!kbhit())||(cur_toggle!=0));
		if (getfilteredkey(mask,&c,&scan))
			{
			if ((c!=13)&&(c!=8)&&(c!=9)&&(c!=27))
				{
				str[str_len++]=c;
				str[str_len]='\0';
				put_char(cursor_x,cursor_y,c);
				cursor_x+=(get_width(c)+spacing);
				}
			else
				{
				if ((c==8)&&(str_len!=0))
					{
					cursor_x-=(get_width(str[str_len-1])+spacing);
					str[--str_len]='\0';
					}
				}
			}
		}
	}

// **************************************************************************
// call installed() immediately after initializing an instance of class font.
// if it returns non-zero (true) the installation was successful. 0 indicates
// a file access or memory error during installation.
// **************************************************************************

int font::installed()
	{
	return(status);
	}

// **************************************************************************
// member function definitions for i_stack pointer stack class
//
// these don't have any specific place in a graphics module, except that they
// were going to be used in a window manager, and I was too lazy to make them
// a module to live in.
// **************************************************************************

// **************************************************************************
// constructor clears the pointer stack to NULL
// **************************************************************************

i_stack::i_stack()
	{
	int i;

	ptr_index=0;
	for (i=0;i<20;i++)
		{
		ptr_array[i]=NULL;
		}
	}

// **************************************************************************
// push a pointer on to the stack. Returns 0 if stack full, 1 if success
// **************************************************************************

char i_stack::push_ptr(void far *ptr)
	{
	if (ptr_index!=20)
		{
		ptr_array[ptr_index++]=ptr;
		return(1);
		}
	else
		{
		return(0);
		}
	}

// **************************************************************************
// pop a pointer from the stack. Returns NULL if stack empty.
// **************************************************************************

void far *i_stack::pop_ptr()
	{
	if (ptr_index!=0)
		return(ptr_array[--ptr_index]);
	else
		return(NULL);
	}

// **************************************************************************
// member function definitions for window class
// **************************************************************************

// **************************************************************************
// Constructor creates the window. See WIN.DOC for more information.
// **************************************************************************

win::win(int tlx, int tly, int xwid, int ywid, char far *bmap,
			   char brdrwid, char shdwwid, char bgrnd, char border)
	{
	if (shdwwid==0)
		{
		shdwon=0;
		if ((backgrnd=new char[xwid*ywid])==NULL)
			{
			status = 0;
			return;
			}
		getimage(tlx,tly,tlx+xwid,tly+ywid,backgrnd);
		}
	else
		{
		shdwon=1;
		if ((backgrnd = new char[(xwid+shdwwid)*(ywid+shdwwid)])==NULL)
			{
			status=0;
			return;
			}
		getimage(tlx,tly,tlx+xwid+shdwwid,tly+ywid+shdwwid,backgrnd);
		barfill(tlx+xwid,tly+shdwwid,tlx+xwid+shdwwid,tly+ywid+shdwwid,0);
		barfill(tlx+shdwwid,tly+ywid,tlx+xwid,tly+ywid+shdwwid,0);
		}
	if (bmap==NULL)
		barfill(tlx,tly,tlx+xwid,tly+ywid,bgrnd);
	else
		putimage(tlx,tly,tlx+xwid,tly+ywid,bmap);
	if (brdrwid!=0)
		{
		if (brdrwid>1)
			{
			barfill(tlx,tly,tlx+xwid,tly+brdrwid,border);
			barfill(tlx+(xwid-brdrwid),tly,tlx+xwid,tly+ywid,border);
			barfill(tlx,tly+(ywid-brdrwid),tlx+xwid,tly+ywid,border);
			barfill(tlx,tly,tlx+brdrwid,tly+ywid,border);
			}
		else
			{
			line(tlx,tly,tlx+xwid,tly,border);
			line(tlx+xwid,tly,tlx+xwid,tly+ywid,border);
			line(tlx,tly+ywid,tlx+xwid,tly+ywid,border);
			line(tlx,tly,tlx,tly+ywid,border);
			}
		}
	origX=tlx;
	origY=tly;
	xsize=xwid;
	ysize=ywid;
	bdrwid=brdrwid;
	shdwid=shdwwid;
	b_grnd=bgrnd;
	brdr=border;
	status=1;
	return;
	}

win::~win()
	{
	if (shdwon)
		putimage(origX,origY,origX+xsize+shdwid,origY+ysize+shdwid,backgrnd);
	else
		putimage(origX,origY,origX+xsize,origY+ysize,backgrnd);
	delete(backgrnd);
	return;
	}

// **************************************************************************
// member function defintions for pcx class
// **************************************************************************

// **************************************************************************
// constructor opens the file as binary, readonly. It reads the dac palette
// into the member p_rec structure palette, corrects it for shift, sets sev-
// eral status variables, and closes the file. On error it sets pcx.status to
// Failed, and reports the error in the images module error record imag_err.
// **************************************************************************

pcx::pcx(char *fspec)
	{
	char check_pal;
	int i,j;
	FILE *filptr;

	status = NoErr;

	if ((filptr = fopen(fspec,"rb"))==NULL)
		{
		status = FileOpenErr;
		return;
		}
	if (fseek(filptr,-769L,SEEK_END) != 0)
		{
		status= FileReadErr;
		fclose(filptr);
		return;
		}
	fread(&check_pal,1,1,filptr);

	// check for 256 color PCX palette tag 0x0C at filelength-769

	if (check_pal!=0x0C)
		{
		status = FileFormatErr;
		fclose(filptr);
		return;
		}

	// read in the palette and store in class member data palette

	if (fread(palette,1,768,filptr) != 768)
		{
		status = FileReadErr;
		fclose(filptr);
		return;
		}

	// shift the palette values right by two bits (pcx specific detail)

	for (i=0;i<256;i++)
		{
		for (j=0;j<3;j++)
			{
			palette[i][j]=palette[i][j]>>2;
			}
		}

	// leave the file pointer on the first byte of image data

	if (fseek(filptr,128,SEEK_SET) != 0)
		{
		status = FileReadErr;
		fclose(filptr);
		return;
		}
	fclose(filptr);
	buffer=NULL;                       // pcx:image.buffer
	for (j=0;fspec[j]!=0;j++);         // these two lines copy the path to
	memcpy(fpath,fspec,j+1);           // to storage in the object data
	in_ram = 0;                        // pcx.in_ram
	packed = 0;                        // pcx.packed
	return;
	}

// **************************************************************************
// unpacktoram unpacks the rle encoded into the buffer pointed to by dest.
// No size checking is performed on the buffer. The unpacking process cont-
// inues until num_bytes have been moved to dest. The function checks the
// state of the in_ram member variable. If the image is not in memory it is
// unpacked from the disk file. If the image is in memory in compressed mode
// it is unpacked from memory. If the image is in memory, but is already
// uncompressed the function returns UnknownErr.
// **************************************************************************

char pcx::unpacktoram(char far *dest, unsigned int num_bytes)
	{
	unsigned int bytes=0;           // counts unpacked bytes
	char    c;                      // byte being processed
	int     runlen;                 // length of packet
	unsigned int src_cntr = 0;      // counts through ram buffer if any
	char far *streambuf;
	FILE *filptr;

	if (!in_ram)
		{
		if ((filptr=fopen(fpath,"rb"))==NULL)
			return(FileOpenErr);
		if ((streambuf=new char[5000]) != NULL)
			{
			setbuf(filptr,streambuf);
			}
		if (fseek(filptr,128L,SEEK_SET) != 0)
			{
			fclose(filptr);
			if (streambuf != NULL)
				delete(streambuf);
			return(FileReadErr);
			}
		do
			{
			c=fgetc(filptr);
			if ((c & 0xc0) == 0xc0)
				{
				runlen = (c & 0x3f);
				c=fgetc(filptr);
				while(runlen--) (dest[bytes++]=c);
				}
			else
				dest[bytes++]=c;
			} while (bytes<num_bytes);
		fclose(filptr);
		if (streambuf!=NULL)
			delete(streambuf);
		}
	else
		{
		if (!packed)
			return(UnknownErr);
		else
			{
			do
				{
				c=buffer[src_cntr++];
				if ((c & 0xc0) == 0xc0)
					{
					runlen = (c & 0x3f);
					c=buffer[src_cntr++];
					while(runlen--) (dest[bytes++]=c);
					}
				else
					dest[bytes++]=c;
				} while (bytes<num_bytes);
			}
		}
	return(NoErr);
	}


// **************************************************************************
// the load() member function reads the pcx file into a memory buffer. The
// pointer to the buffer and the buffer size are internal to the class, and
// do not need to be taken into account by the programmer. The data is loaded
// based on the value of method. A set of three constants is defined for this
// value; Packed, Unpacked, and Bestfit. If method = packed the data will be
// read into the memory buffer in compressed PCX format, but the palette and
// header data will be stripped. If method = unpacked the data is uncompress-
// to memory using the unpacktoram() function. In either case, if there is
// not sufficient memory, the function returns MemErr. If method = Bestfit
// the function will unpack the data is possible, or load it packed if not,
// and finally returns MemErr if neither method can be used. If successfull
// the function sets internal switches so that other pcx member functions
// will know that the image data is already loaded, and in what form it has
// been loaded.
// **************************************************************************

char pcx::load(char method)
	{
	unsigned readsize;
	FILE *filptr;

	// make sure the image isn't already loaded

	if (!in_ram)
		{
		if (method == Bestfit)
			{

			// if enough ram for the image load unpacked

			if (coreleft() >= 64000U)
				{
				method = Unpacked;
				}
			else

			// if not enough ram open the file, compare it's size to the
			// available memory, and load it packed if there's enough room
			// or return a MemErr if there isn't.

				{
				if ((filptr=fopen(fpath,"rb"))==NULL)
					{
					return(FileOpenErr);
					}
				readsize = filelength(fileno(filptr))-(128+769);
				fclose(filptr);
				if (coreleft() >= readsize)
					method = Packed;
				else
					return(MemErr);
				}
			}

		// if method = packed then allocate a buffer the size of the image
		// file with the header and palette data stripped out. Zero the
		// buffer, then read the packed data into it. Finally, set in_ram
		// and packed = 1 (pcx class member data used by display() member
		// function)

		if (method == Packed)
			{
			if ((filptr=fopen(fpath,"rb"))==NULL)
				return(FileOpenErr);
			readsize = filelength(fileno(filptr))-(128+769);
			if ((buffer = new char[readsize]) == NULL)
				{
				fclose(filptr);
				return(MemErr);
				}
			_fmemset(buffer,0,readsize);
			if (fseek(filptr, 128L, SEEK_SET) != 0)
				{
				fclose(filptr);
				delete(buffer);
				return(FileReadErr);
				}
			if (fread(buffer, 1, readsize, filptr) != readsize)
				{
				fclose(filptr);
				delete(buffer);
				return(FileReadErr);
				}
			fclose(filptr);
			in_ram = 1;
			packed = 1;
			}
		else

		// if method = Unpacked then allocate a 64000 byte buffer, and
		// unpack the data into it.

			{
			if ((buffer = new char[64000U]) == NULL)
				return(MemErr);
			_fmemset(buffer,0,64000U);
			if(unpacktoram(buffer,64000U) != NoErr)
				{
				delete(buffer);
				in_ram = 0;
				return(SecondaryErr);
				}
			else
				{
				in_ram = 1;
				packed = 0;
				}
			}
		}
	return(NoErr);
	}

// **************************************************************************
// the unload() member function frees the memory allocated to the pcx data,
// and updates the internal switches so that other member functions will
// know that the image is no longer in memory.
// **************************************************************************

void pcx::unload()
	{
	if (in_ram)
		{
		delete(buffer);
		in_ram = 0;
		packed = 0;
		}
	}

// **************************************************************************
// pcx::display can be used to display a PCX class object to screen using
// various fade types, wipes, and dissolves. All fade types other than Soft-
// Fade and SnapWipe require a screen buffer of 64000 bytes. If that memory
// is not available the function returns MemErr. The PCX will be displayed
// whether it is: on disk, in ram packed, or in ram unpacked (in order of
// decreasing execution time).
//
// The following fade types are available:
//
//		SnapWipe           - snaps the pcx to screen, basic block mem move.
//		SplitVerticalWipe  - fades from the top and bottom toward center.
//		SplitHorizWipe     - fades from the right and left toward center.
//      SlideVerticalWipe  - slides pic in from left and right
//      SlideHorizWipe     - slides pic in from top and bottom
//		VerticalDissolve   - overlays multiple vertical slices
//		HorizDissolve      - overlays multiple horizontal slices
//		SparkleDissolve    - fades with random block placement
//		SoftFade           - dac palette intensity fade, 64 passes
// **************************************************************************

char pcx::display(char fadetype)
	{
	char far *do_buffer;
	char loaded_here = 0;

	// SoftFade (palette fade) requires the palette to be cleared.

	if (fadetype == SoftFade)
		{
		wait_vbi();
		clrpalette(0,256);
		}
	else

	// all other types require the new palette to be loaded

		{
		wait_vbi();
		loadpalette(0,256,palette);
		}

	// if the image is not in ram, or is in ram compressed, then it
	// needs to be unpacked, from either disk or memory, to either the
	// video buffer or a memory buffer depending on the fade type.

	if ((!in_ram) || ((in_ram) && (packed)))
		{

		// these two fade types simply require the data unpacked to vseg.

		if ((fadetype == SoftFade) || (fadetype == SnapWipe))
			{
			if (unpacktoram((char far *)MK_FP(0xa000,0), 64000U) != NoErr)
				return(SecondaryErr);
			}
		else

		// all the others require the data in a 64K memory buffer
		// so one is allocated, and the data is unpacked to it

			{
			if ((do_buffer = new char[64100U]) == NULL)
				return(MemErr);
			if (unpacktoram(do_buffer, 64000U) != NoErr)
				{
				delete(do_buffer);
				return(SecondaryErr);
				}
			loaded_here = 1;
			}
		}
	else

	// if the data is in ram uncompressed it's ready to go. The only
	// action necessary is to copy it to the video buffer for the two
	// simple fade types. The buffer identifier is member data of the
	// image class, and stores a pointer to the image buffer. In both
	// cases do_buffer is assigned the value of buffer, and then the
	// operations are performed on it. This simplifies the calls to
	// the fade functions below.

		{
		do_buffer = buffer;

		// for these two fade types copy the data over to vseg

		if ((fadetype == SoftFade) || (fadetype == SnapWipe))
			{
			wait_vbi();
			asm {
				push es
				push ds
				push cx
				push si
				push di
				mov es, GRAPH_SEG
				xor di, di
				lds si, do_buffer
				mov cx, 32000
				rep movsw
				pop di
				pop si
				pop cx
				pop ds
				pop es
				}
			}
		}
	switch (fadetype)
		{
		case SoftFade:           fadepalettein(0,256,palette); break;
		case SplitVerticalWipe:  doSplitVerticalWipe(do_buffer); break;
		case SplitHorizWipe:     doSplitHorizWipe(do_buffer); break;
		case SlideVerticalWipe:  doSlideVerticalWipe(do_buffer); break;
		case SlideHorizWipe:     doSlideHorizWipe(do_buffer); break;
		case VerticalDissolve:   doVerticalDissolve(do_buffer); break;
		case HorizDissolve:      doHorizDissolve(do_buffer); break;
		case SparkleDissolve:    doSparkleDissolve(do_buffer); break;
		}
	if (loaded_here)
		delete(do_buffer);
	return(NoErr);
	}

// **************************************************************************
// remove() clears the current image from the screen using the passed fade
// type. It will clear the screen regardless of what image is on it. No
// checking is performed to be certain this function is called after a call
// to display() has been made for the object.
// **************************************************************************

void pcx::remove(char fadetype)
	{
	switch (fadetype)
		{
		case SoftFade:            fadepaletteout(0,256); break;
		case SnapWipe:            clearscr(0); break;
		case SplitVerticalWipe:   doSplitVerticalWipe(NULL); break;
		case SplitHorizWipe:      doSplitHorizWipe(NULL); break;
		case SlideVerticalWipe:   break;
		case SlideHorizWipe:      break;
		case VerticalDissolve:    doVerticalDissolve(NULL); break;
		case HorizDissolve:       doHorizDissolve(NULL); break;
		case SparkleDissolve:     doSparkleDissolve(NULL); break;
		}
	return;
	}

// **************************************************************************
// destructor frees memory allocated to pcx file, if any
// **************************************************************************

pcx::~pcx()
	{
	if (in_ram)
		delete(buffer);
	}
