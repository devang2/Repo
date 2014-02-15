// KEYBOARD.CPP ************************************************************
// module for low level filtered and unfiltered keyboard i/o.              *
// Copyright 1991 by the Gamers Programming Workshop, a function of the    *
// GAMERS forum, Compuserve. For more info e-mail 76605,2346.              *
//                                                                         *
// License is granted for use or modification of this code as long as      *
// this notice remains intact, and all improvements are listed in the      *
// version history below, and uploaded to the GAMERS forum. This code      *
// may not be used for any commercial purpose.                             *
//                                                                         *
//**************************************************************************

//**************************************************************************
// Version history:                                                        *
//                                                                         *
// Version 1.0                                                             *
// Developed: May 30, 1991                                                 *
// Author:    Mark Betz, 76605, 2346                                       *
// Last update: July 5, 1991                                               *
//**************************************************************************

#include <stdio.h>
#include <conio.h>
#include "keyboard.hpp"

// ************************************************************************
// getkey() is passed pointers to type char, for the ascii code, and type
// extnd, for the scan code. If either is NULL the funtion ignores it. The
// function waits if no key has been pressed, so test with kbhit() if you
// don't want to hang here.
// ************************************************************************

void getkey(char *key, extnd *scan) {
	asm {
		mov ah, 0x10
		int 0x16
	}
	if (key!=NULL)
		*key=_AL;
	if (scan!=NULL) {
		if (*key==0)
			*scan=(extnd)_AH;
		else
			*scan=NO_EXT;
	}
}

// ************************************************************************
// getfilteredkey() compares the next keycode combination retrieved from
// the buffer based on the filter in mask. If a key matches the function
// stores the key data in key and scan, and returns a boolean true. If no
// match exists for the key, the function returns false, and the values
// of scan and key are unchanged. NULL pointer arguments are not allowed.
// ************************************************************************

boolean getfilteredkey(char mask, char *key, extnd *scan) {
	char main,aux;
	asm {
		mov ah, 0x10
		int 0x16
	}
	main=_AL;
	aux =_AH;
	if (!((mask&UCASE)&&                  // filter for uppercase chars
	   (main>64) && (main<91))) {

	 if (!((mask&LCASE)&&                 // filter for lower case chars
		(main>96) && (main<123))) {

	  if (!((mask&BCASE)&&                // filter for chars of both cases
		 (((main>64) && (main<91))||
		 ((main>96) && (main<123))))) {

	   if (!((mask&NUMBER)&&               // filter for numeric chars
		  (main>47) && (main<58))) {

		if (!((mask&FUNCT)&&               // filter for keys f1-f10
		   (main==0)&&
		   ((aux>58) && (aux<69)) ||
		   ((aux>132) && (aux<135)))) {

		 if (!((mask&FUNCT)&&              // filter for shift f1- shift f10
			(main==0)&&
			(((aux>83) && (aux<94))||
			((aux>134) && (aux<137))))) {

		  if (!((mask&FUNCT)&&             // filter for ctrl f1- ctrl f10
			 (main==0)&&
			 (((aux>93) && (aux<104))||
			 ((aux>136) && (aux<139))))) {

		   if (!((mask&FUNCT)&&            // filter for alt f1- alt f10
			  (main==0)&&
			  (((aux>103) && (aux<114))||
			  ((aux>138) && (aux<141))))) {

			if (!((mask&CURSOR)&&          // filter for cursor command keys
			   ((main==0)||(main==224))&&  // source keypad or standalone
			   (((aux>69) && (aux<74))||   // home, up arrow, pageup
			   (aux==75)||(aux==77)||      // left arrow, right arrow
			   ((aux>78) && (aux<84))||    // end, dn arrw, pgdwn, ins, del
			   ((aux>113) && (aux<120))||  // ctrl-cursor combos
			   (aux==132)))) {

			 if (!((mask&PUNCT)&&          // filter for punctuation marks
				((main==33)||(main==34)||  // ! and "
				(main==39)||(main==44)||   // ' and ,
				(main==46)||(main==96)||   // . and `
				(main==13)||(main==8)||    // enter and backspace
				(main==32)||(main==9)))) { // space and tab

			  if (!((mask&ESC)&&(main==27))) {

					return(false);
			  }
			 }
			}
		   }
		  }
		 }
		}
	   }
	  }
	 }
	}
	*key=main;
	*scan=(extnd)aux;
	return(true);
}

// ************************************************************************
// stuffbuffer() places the passed key and scan codes in the keyboard
// buffer. stuffbuffer() does not check for a full keyboard buffer. It is
// recommended that flushbuffer() be called prior to using this service,
// and that this function be called a maximum of 15 times between keyboard
// reads.
// ************************************************************************

void stuffbuffer(char key, extnd scan) {
	char s;
	s=scan;
	asm {
		mov ah, 5
		mov cl, key
		mov ch, s
		int 0x16
	}
}

// ************************************************************************
// flushbuffer() flushes the keyboard buffer. If buf != NULL it flushes the
// key and scan codes to the memory buffer pointed to by buf, and returns
// the number of characters read. If buf=NULL, or no characters are waiting
// it returns 0.
// ************************************************************************

int flushbuffer(char *buf) {
	char c;
	extnd s;
	int i;

	if (buf!=NULL) {
		for (i=0;kbhit();) {
			getkey(&c,&s);
			buf[i++]=c;
			buf[i++]=s;
		}
		return(i/2);
	} else
		while (kbhit()) {
			getkey(&c,&s);
		}
	return(0);
}



