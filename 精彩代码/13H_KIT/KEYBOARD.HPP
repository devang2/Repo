// KEYBOARD.HPP ************************************************************
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

enum boolean {false,true};

// keyboard constants for special keys *************************************
// prefixes: C_  : Ctrl and key
//           S_  : Shift and key
//           A_  : Alt and key      example: A_F2 = alt-f2
// *************************************************************************

enum extnd {F1=59,F2,F3,F4,F5,F6,F7,F8,F9,F10,
			F11=133,F12,
			S_F1=84,S_F2,S_F3,S_F4,S_F5,S_F6,S_F7,S_F8,S_F9,S_F10,
			S_F11=135,S_F12,
			C_F1=94,C_F2,C_F3,C_F4,C_F5,C_F6,C_F7,C_F8,C_F9,C_F10,
			C_F11=137,C_F12,
			A_F1=104,A_F2,A_F3,A_F4,A_F5,A_F6,A_F7,A_F8,A_F9,A_F10,
			A_F11=139,A_F12,
			S_TAB=15,
			HOME=71,UP_ARR,PG_UP,LT_ARR=75,RT_ARR=77,END=79,DN_ARR,
			PG_DN,INS,DEL,
			C_PRTSC=114,C_LT_ARR,C_RT_ARR,C_END,C_PG_DN,C_HOME,
			C_PG_UP=132, NO_EXT=0};

// *************************************************************************
// mask types passed to getfilteredkey() to select keymask. Masks operate
// as follows:  U_CASE   - returns only upper case characters
//				L_CASE   - returns only lower case characters
//				B_CASE   - returns characters of both cases
//				NUMBER   - returns only numbers
//				FUNCT    - returns only function keys
//				CURSOR   - returns only cursor positioning keys
//              PUNCT    - returns only punctuation marks
//              ESC      - returns only the escape key
//
// the masks can be combined with a logical OR in the call, for example:
//
// if(getfilteredkey(&this_event,U_CASE|NUMBER|FUNCT))
//
// will return true if the key pressed was an upper case character, a
// number, or a function key. If true, the key's ascii code and scan
// value are in struct key_event this_event.
// *************************************************************************

const char UCASE = 0x1;
const char LCASE = 0x2;
const char BCASE = 0x4;
const char NUMBER = 0x8;
const char FUNCT  = 0x10;
const char CURSOR = 0x20;
const char PUNCT  = 0x40;
const char ESC   = 0x80;

// function prototypes *****************************************************

extern void getkey(char *key, extnd *scan);
extern boolean getfilteredkey(char mask, char *key, extnd *scan);
extern void stuffbuffer(char key, extnd scan);
extern void flushBuffer(char *buf);
