//*************************************************************************
// IMAGES.HPP -Header file for basic graphics operations and PCX          *
//               image decoding in graphics mode 13h                      *
// 								                                     	  *
// Copyright 1991 by the Gamers Programming Workshop, a function of the   *
// GAMERS forum, Compuserve. For more info e-mail 76605,2346.             *
//                                                                        *
// License is granted for use or modification of this code as long as     *
// this notice remains intact, and all improvements are listed in the     *
// version history below, and uploaded to the GAMERS forum. This code     *
// may not be used for any commercial purpose.                            *
//*************************************************************************

//*************************************************************************
// Version history:                                                       *
//                                                                        *
// Version 1.0                                                            *
// Developed: May 2, 1991                                                 *
// Author:    Mark Betz, 76605,2346                                       *
// Last update: July 5, 1991                                              *
//*************************************************************************

// ************************************************************************
// The functions and class structures declared in this header provide some
// basic tools for 256 color mode 13h. Tools are provided for initializing
// a pcx graphic file, unpacking PCX encoded data, setting text and graphics
// modes, loading the dac palette, reading the dac palette, writing a pixel
// reading a pixel, bar fills, and fast line drawing. In addition, the class
// font provides methods for displaying and reading strings in a bitmapped
// font. See images.doc for info on images tools, and font.doc for info on
// using the font class.
// ************************************************************************

// ************************************************************************
// this is the 128 byte header at the front of all PCX files. In 256 color
// mode 0x13 the palette structure is ignored, and the palette can be found
// in the last 768 bytes of the file.
// ************************************************************************

struct pcx_hdr {
	char Mfg;				// manufacturer, always 0xa0
	char Ver;				// encoder version number
	char Enc;				// encoding code, always 1
	char Bpp;				// bits per pixel, 8 in mode 0x13
	int	 Xmin,Ymin;			// image origin, usually 0,0
	int	 Xmax,Ymax;			// image dimensions
	int	 Hres;				// horizontal resolution value
	int	 Vres;				// vertical resolution value
	char Pal[48];			// palette (not in mode 0x13)
	char Reserved;       	// who knows?
	char ClrPlanes;	    	// number of planes, 1 in mode 0x13
	int	 Bpl;				// bytes per line, 80 in mode 0x13
	int	 PalType;			// Grey or Color palette flag
	char Filler[58];		// Zsoft wanted a 128 byte header
};

// **************************************************************************
// p_rec is the primary rgb dac palette structure type
// **************************************************************************

typedef char p_rec[256][3];  	   // basic rgb palette structure type

// **************************************************************************
// this enum is used to specify in several places how the background will be
// treated.
// **************************************************************************

enum opacity {trans,opaque};

// **************************************************************************
// declaration of font class (see font.doc). All methods define in images.cpp
// **************************************************************************

class font {                              // generic font class
	public:
	opacity see_thru;                     // background treatment

	font(char *f_spec);                   // constructor, pass file name
	void setstyle(int f_grnd, int b_grnd,
				  short char_tab, opacity o,
				  char space_wid);
	int getforecolor();                   // get current text color
	int getbackcolor();                   // get current background color
	void show(int x, int y,char *str);    // show a string at x,y
	void readstr(int x, int y,char *str,
				 char length, char mask); // read a string from keyboard
	int installed();	    			  // returns 1 if font installed
	~font();                              // destructor frees font memory

	private:
	struct font_hdr {              // font description structure
		char  font_name[8];        // font name, no file extension
		char  fram_wid;            // width of character frame in pixels
		char  fram_hit;            // height of character frame in pixels
		char  base_ofs;            // baseline y offset from top
		char  ascii_first;         // number of first character defined
		char  ascii_last;          // number of last character defined
		char f_color;              // original font foreground color
	} f_hdr;
	char width[128];               // character width table
	char spacing;                  // pixels between characters
	char far *font_ptr;            // pointer to font in memory
	char far *cursor_mask;         // pointer to cursor mask in memory
	int fore_color;                // holds current text color
	int back_color;                // holds current background color
	int status;                    // non 0 if installation successful

	void put_char(int x, int y,    // display a single character
				  char c);
	char get_width(char c);
};

// *************************************************************************
// The i_stack class was originally envisioned as a pointer stack for the
// use of a window management class that would create cascaded windows by
// allocating pointers to win class objects and pushing them onto the
// pointer stack. A close() method would close the last opened window. For
// now this is on the back burner. If you see a use for the pointer stack
// be aware that I haven't tested it at all. Should work <g>.
// *************************************************************************


class i_stack {
	public:
	i_stack();                     // constructor
	char push_ptr(void far *ptr);  // push a pointer on to stack
	void *pop_ptr();               // pop a pointer from the stack

	private:
	void far *ptr_array[20];       // pointer container
	char ptr_index;                // index to next open element
};

// *************************************************************************
// Class win is a pop up window class. A win object allocates memory for,
// and saves it's own background. So consecutive windows can be opened, and
// each will restore the background as it goes out of scope, or as it is
// deleted(), depending on how it was opened. See WIN.DOC for a brief des-
// cription.
// *************************************************************************

class win {
	public:
	char status;             // 1 if window opened successfully

	win(int tlx, int tly, int xwid, int ywid, char *bmap,
		   char brdrwid, char shdwwid, char bgrnd, char border);

	char installed(){return(status);}   // returns the status value
	~win();

	private:
	char far *backgrnd;      // pointer to background bitmap
	int origX;               // top left x coord of the window
	int origY;               // top left y coord of the window
	int xsize;               // window x dimension in pixels
	int ysize;               // window y dimension in pixels
	char bdrwid;             // border width, 0 if no border
	char shdwid;             // shadow width, 0 if no shadow
	char shdwon;             // 1 if shadow is on, 0 if not
	char b_grnd;             // window background color
	char brdr;               // border color, ignored if bdrwid=0;
};


class image {
	public:
	p_rec palette;                        // holds the default image palette
	char fpath[80];                       // stores the disk path to image
	char getstatus() {return(status);}    // returns status of last op
										  // 1 if successful, 0 if not
	protected:
	char in_ram;                          // 1 if image in ram, 0 if not
	char packed;                          // 1 if packed in ram, 0 if not
	char status;                          // returned by getstatus()
	char far *buffer;                     // pointer for ram buffer if any
};

class pcx : public image {
	public:
	pcx(char *fspec);                     // constructor
	char load(char method);               // load into ram packed or unpacked
	void unload();                        // unload from ram
	char display(char fadetype);          // display image using fadetype
	void remove(char fadetype);           // remove image using fadetype
	~pcx();								  // destructor
	private:
	char unpacktoram(char far *dest,      // internal, unpack to memory
					 unsigned numbytes);

};

// image fade constants ***************************************************

const char SnapWipe = 0;             // simple block copy to vram
const char SplitVerticalWipe = 1;    // fades from top and bottom
const char SplitHorizWipe = 2;       // fades from right and left
const char SlideVerticalWipe = 3;    // slides image in from right and left
const char SlideHorizWipe = 4;       // slides image in from top and bottom
const char VerticalDissolve = 5;     // fades multiple vertical slices
const char HorizDissolve = 6;        // fades multiple horizontal slices
const char SparkleDissolve = 7;      // fades with random pixel placement
const char SoftFade=8;	             // does a palette shifted fade

// error reporting constants **********************************************

const char NoErr = 0;            // no error occured
const char MemErr = 1;           // error occured allocating memory
const char FileReadErr = 2;      // error occured reading a file
const char FileWriteErr = 3;     // error occured writing a file
const char FileMakeErr = 4;      // error occured creating a file
const char FileOpenErr = 5;      // error occured opening file
const char FileFormatErr = 6;    // bad file format error
const char SecondaryErr = 7;     // error occured in subroutine during call
const char UnknownErr = 8;       // an unknown error occured

// For use with type PRec *************************************************

const short Red = 0;               // constants used with type PRec
const short Green = 1;             // example: PRec Palette;
const short Blue = 2;			   //          Palette[0][Green]=63;

// for use with call to pcx::load() ***************************************

const char Packed = 0;             // load image in compressed form
const char Unpacked = 1;           // load image in uncompressed form
const char Bestfit = 2;            // load compressed or uncompressed

// Function prototypes ****************************************************

extern void unpackpcx(FILE *pcx, const char far *source,
					  char far *dest, unsigned int num_bytes);

// **** System level graphics functions ***********************************

extern void setgraphmode();
extern void settextmode();
extern void wait_vbi();
extern void reporterr(char type, char where[30]);

// **** DAC palette functions *********************************************

extern void loadpalette(int start, int number, const p_rec palette);
extern void readpalette(int start, int number, p_rec palette);
extern void clrpalette(int start, int number);
extern void fadepalettein(int start, int count, const p_rec palette);
extern void fadepaletteout(int start, int count);

// **** Graphics segment redirection **************************************

extern void setgraphseg(unsigned newseg);

// **** Drawing primitives ************************************************

extern void clearscr(int color);
extern void barfill(int tlx, int tly, int brx, int bry, int color);
extern void writepixel(int x, int y, int color);
extern char readpixel(int x, int y);
extern void far *xy_to_ptr(int x, int y);
extern void line(int x0, int y0, int x1, int y1, int color);

// **** Image block copying functions *************************************

extern void getimage(int x0, int y0, int x1, int y1, char far *buff);
extern void putimage(int x0, int y0, int x1, int y1, char far *buff);
extern void copyimage(int x0, int y0, int x1, int y1, int putx, int puty,
					  void far *src_buf);

// **** Image fades and dissolve functions ********************************

extern void doSplitVerticalWipe(void far *pic_buf);
extern void doSplitHorizWipe(void far *pic_buf);
extern void doSlideVerticalWipe(void far *pic_buf);
extern void doSlideHorizWipe(void far *pic_buf);
extern void doVerticalDissolve(void far *pic_buf);
extern void doHorizDissolve(void far *pic_buf);
extern void doSparkleDissolve(void far *pic_buf);

// ***********************************************************************



