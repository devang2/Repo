// GETFONT.CPP
// small program to display and grab font bitmap data from a PCX file.

#include <dos.h>
#include <io.h>
#include <stdio.h>
#include <conio.h>
#include <alloc.h>
#include <mem.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "keyboard.hpp"
#include "images.hpp"

// fill in the constants below as applicable to the font you're capturing

struct header {                     // font description structure
	char  font_name[8];             // font name, no file extension
	char  fram_wid;                 // width of character frame in pixels
	char  fram_hit;                 // height of character frame in pixels
	char  base_ofs;                 // baseline y offset from top
	char  ascii_first;              // number of first character defined
	char  ascii_last;               // number of last character defined
	char f_color;          // original font foreground color

} HDR={"CSFONT1",6,6,5,0x20,0x7F,15};  // <<-----FILL IN HERE

char width[128] = {4,2,3,5,5,5,5,2,2,2,2,4,2,2,2,5,4,2,4,4,4,4,4,4,4,
				   4,2,2,5,4,5,4,5,4,4,4,4,4,4,4,4,3,4,4,4,5,5,4,4,4,
				   4,4,4,4,4,5,5,4,4,2,5,2,5,5,2,5,4,4,4,4,3,4,4,3,4,
				   4,2,5,4,4,4,4,4,4,3,4,4,5,4,4,4,2,2,2,4,5};

char cursor_mask[6][6]= {0,0,1,0,0,0,
						 0,1,0,1,0,0,
						 1,0,0,0,1,0,
						 0,1,0,1,0,0,
						 0,0,1,0,0,0,
						 0,0,0,0,0,0};


const short x_origin=20;            // top left x of first frame in each row
const short row1_y_origin=24;       // top left y of first frame in row #
const short row2_y_origin=31;
const short row3_y_origin=38;
const short chars_per_row=32;          // number of frames in each row

char pcxfile[80]="FONT1.PCX";    // source pcx file
char fntfile[80]="CSFONT1.FNT";  // destination font file name
char result;

const void far *vbuff=MK_FP(0xA000,0);

unsigned int i,x,y;

// **************************************************************************
// grabchar() gets the character at the passed x,y coord and writes it to
// the file indicated by the global handle.
// **************************************************************************

void grabchar(int x, int y) {
	int j;
	for(j=y;j<(y+HDR.fram_hit);j++) {
		write(handle,xy_to_ptr(x,j),HDR.fram_wid);
	}
}


void main() {
	_setgraphmode();

	// init the pcx file

	pcx font("font1.pcx");
	if (font.getstatus() != NoErr) {
		reporterr(font.getstatus(),"main()");
		return;
	}
	result = font.load(Bestfit);
	if ((result != NoErr) && (result != MemErr)) {
		reporterr(result,"font.load()");
		return;
		}
	font.display(SnapWipe);

	// create the font file in the current directory

	if((handle=open(fntfile,O_CREAT|O_BINARY,S_IREAD|S_IWRITE))==-1) {
		_settextmode();
		printf("error creating font file\n");
		_settextmode();
		return;
	}

	// write the font header, character width table, and cursor mask to
	// the new file

	write(handle,&HDR,sizeof(HDR));
	write(handle,&width[0],sizeof(width));
	write(handle,&cursor_mask[0][0],sizeof(cursor_mask));

	// capture row 1

	for (i=0;i<chars_per_row;i++) {
		x=(i*HDR.fram_wid)+x_origin;
		grabchar(x,row1_y_origin);
	}

	// capture row 2

	for (i=0;i<chars_per_row;i++) {
		x=(i*HDR.fram_wid)+x_origin;
		grabchar(x,row2_y_origin);
	}

	// capture row 3

	for (i=0;i<chars_per_row;i++) {
		x=(i*HDR.fram_wid)+x_origin;
		grabchar(x,row3_y_origin);
	}

	// cleanup

	sound(100);
	delay(200);
	nosound();
	while(!kbhit());
	_settextmode();
	return;
}

