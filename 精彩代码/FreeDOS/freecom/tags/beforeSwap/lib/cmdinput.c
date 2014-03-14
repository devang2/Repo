/*
 *  CMDINPUT.C - handles command input (tab completion, history, etc.)
 */

#include "../config.h"

#include <assert.h>
#include <conio.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

#include "../include/command.h"
#include "../include/keys.h"
#include "../include/misc.h"

static unsigned orgx, orgy;		/* start of current line */

/* Print a character to current cursor position
	Updates cursor postion
 */
static void outc(char c)
{
	putchar(c);
}
/* Print a blank to current cursor postion
	Updates cursor position
 */
static void outblank(void)
{	outc(' ');
}

/* Print string to current cursor position
	Updates cursor position
 */
static void outs(const char * const s)
{	assert(s);
	fputs(s, stdout);
}
/* Print string to cursor position and append one blank
	Updates cursor postion
 */
static void outsblank(const char * const s)
{	outs(s);
	outblank();
}

/* read in a command line */
void readcommandEnhanced(char * const str, const int maxlen)
{
	unsigned char insert = 1;
	unsigned ch;
#ifdef FEATURE_FILENAME_COMPLETION
	unsigned lastch = 0;
#endif
#ifdef FEATURE_HISTORY
	int histLevel = 0;
	char prvLine[MAX_INTERNAL_COMMAND_SIZE];
#endif
	unsigned curx;
	unsigned cury;
	int count;
	unsigned current = 0;
	unsigned charcount = 0;

	assert(str);

	/* if echo off, don't print prompt */
	if(echo)
		printprompt();

	orgx = wherex();
	orgy = wherey();
	memset(str, 0, maxlen);

	_setcursortype(_NORMALCURSOR);

#ifdef FEATURE_HISTORY
	histGet(histLevel - 1, prvLine, sizeof(prvLine));
#endif

	do {
		ch = cgetchar();

		if(cbreak)
			ch = KEY_CTL_C;

		switch(ch) {
		case KEY_BS:               /* delete character to left of cursor */

			if(current > 0 && charcount > 0) {
			  if(current == charcount) {     /* if at end of line */
				str[current - 1] = 0;
				if (wherex() != 1)
				  outs("\b \b");
				else
				{
				  goxy(MAX_X, wherey() - 1);
				  outblank();
				  goxy(MAX_X, wherey() - 1);
				}
			  }
			  else
			  {
				for (count = current - 1; count < charcount; count++)
				  str[count] = str[count + 1];
				if (wherex() != 1)
				  goxy(wherex() - 1, wherey());
				else
				  goxy(MAX_X, wherey() - 1);
				curx = wherex();
				cury = wherey();
				outsblank(&str[current - 1]);
				goxy(curx, cury);
			  }
			  charcount--;
			  current--;
			}
			break;

		case KEY_INSERT:           /* toggle insert/overstrike mode */
			insert ^= 1;
			if (insert)
			  _setcursortype(_NORMALCURSOR);
			else
			  _setcursortype(_SOLIDCURSOR);
			break;

		case KEY_DELETE:           /* delete character under cursor */

			if (current != charcount && charcount > 0)
			{
			  for (count = current; count < charcount; count++)
				str[count] = str[count + 1];
			  charcount--;
			  curx = wherex();
			  cury = wherey();
			  outsblank(&str[current]);
			  goxy(curx, cury);
			}
			break;

		case KEY_HOME:             /* goto beginning of string */

			if (current != 0)
			{
			  goxy(orgx, orgy);
			  current = 0;
			}
			break;

		case KEY_END:              /* goto end of string */

			if (current != charcount)
			{
			  goxy(orgx, orgy);
			  outs(str);
			  current = charcount;
			}
			break;

#ifdef FEATURE_FILENAME_COMPLETION
		case KEY_TAB:		 /* expand current file name */
			if(current == charcount) {      /* only works at end of line */
			  if(lastch != KEY_TAB) { /* if first TAB, complete filename */
				complete_filename(str, charcount);
				charcount = strlen(str);
				current = charcount;

				goxy(orgx, orgy);
				outs(str);
				if ((strlen(str) > (MAX_X - orgx)) && (orgy == MAX_Y + 1))
				  orgy--;
			  } else {                 /* if second TAB, list matches */
				if (show_completion_matches(str, charcount))
				{
				  printprompt();
				  orgx = wherex();
				  orgy = wherey();
				  outs(str);
				}
			  }
			}
			else
			  beep();
			break;
#endif

		case KEY_ENTER:            /* end input, return to main */

#ifdef FEATURE_HISTORY
			if(str[0])
			  histSet(0, str);      /* add to the history */
#endif

			outc('\n');
			break;

		case KEY_CTL_C:       		/* ^C */
		case KEY_ESC:              /* clear str  Make this callable! */

			clrcmdline(str, maxlen, orgx, orgy);
			current = charcount = 0;

			if(ch == KEY_CTL_C && !echo) {
			  /* enable echo to let user know that's this
				is the command line */
			  echo = 1;
			  printprompt();
			}
			break;

		case KEY_RIGHT:            /* move cursor right */

			if (current != charcount)
			{
			  current++;
			  if (wherex() == MAX_X)
				goxy(1, wherey() + 1);
			  else
				goxy(wherex() + 1, wherey());
				break;
			}
			/* cursor-right at end of string grabs the next character
				from the previous line */
			/* FALL THROUGH */

#ifndef FEATURE_HISTORY
			break;
#else
		case KEY_F1:       /* get character from last command buffer */
			  if (current < strlen(prvLine)) {
				 outc(str[current] = prvLine[current]);
				 charcount = ++current;
			  }
			  break;
			  
		case KEY_F3:               /* get previous command from buffer */
			if(charcount < strlen(prvLine)) {
				outs(strcpy(&str[charcount], &prvLine[charcount]));
			   current = charcount = strlen(str);
		   }
		   break;

		case KEY_UP:               /* get previous command from buffer */
			if(!histGet(--histLevel, prvLine, sizeof(prvLine)))
				++histLevel;		/* failed -> keep current command line */
			else {
				clrcmdline(str, maxlen, orgx, orgy);
				strcpy(str, prvLine);
				current = charcount = strlen(str);
				outs(str);
				histGet(histLevel - 1, prvLine, sizeof(prvLine));
			}
			break;

		case KEY_DOWN:             /* get next command from buffer */
			if(histLevel) {
				clrcmdline(str, maxlen, orgx, orgy);
				strcpy(prvLine, str);
				histGet(++histLevel, str, sizeof(str));
				current = charcount = strlen(str);
				outs(str);
			}
			break;

		case KEY_F5: /* keep cmdline in F3/UP buffer and move to next line */
			strcpy(prvLine, str);
			clrcmdline(str, maxlen, orgx, orgy);
			outc('@');
			if(orgy >= MAX_Y) {
				outc('\n');			/* Force scroll */
				orgy = MAX_Y;
			} else {
				++orgy;
			}
			goxy(orgx, orgy);
			current = charcount = 0;

			break;

#endif

		case KEY_LEFT:             /* move cursor left */

			if (current > 0)
			{
			  current--;
			  if (wherex() == 1)
				goxy(MAX_X, wherey() - 1);
			  else
				goxy(wherex() - 1, wherey());
			}
#if 0
			else
				   beep();
#endif
			break;

		default:                 /* insert character into string... */

			if ((ch >= 32 && ch <= 255) && (charcount != (maxlen - 2)))
			{
			  if (insert && current != charcount)
			  {
				for (count = charcount; count > current; count--)
				  str[count] = str[count - 1];
				str[current++] = ch;
				curx = wherex() + 1;
				cury = wherey();
				outs(&str[current - 1]);
				if ((strlen(str) > (MAX_X - orgx)) && (orgy == MAX_Y + 1))
				  cury--;
				goxy(curx, cury);
				charcount++;
			  }
			  else
			  {
				if (current == charcount)
				  charcount++;
				str[current++] = ch;
				outc(ch);
			  }
			  if ((strlen(str) > (MAX_X - orgx)) && (orgy == MAX_Y + 1))
				orgy--;
			}
			else
			  beep();
			break;
		}
#ifdef FEATURE_FILENAME_COMPLETION
		lastch = ch;
#endif
	} while(ch != KEY_ENTER);

	_setcursortype(_NORMALCURSOR);
}
