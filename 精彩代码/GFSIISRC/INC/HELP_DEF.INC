#define	LINE_LEN		50		/* Maximum of characters in one line */
#define TITLE_LINE_LEN		36		/* Maximum of characters in title line */
#define MENU_LINE_LEN		24		/* Maximum of characters in menu line */
#define MAX_LINES		12		/* Maximum of lines in one page	*/

#define BORDER			5
#define KEY_BORDER		3
#define BACK_IN			4
#define HELP_SX			199		/* x screen coordinate of help window */
#define	HELP_SY			120		/* y screen coordinate of help window */
#define LINE_HIGH		20  	/* High of one text line */
#define KEY_HIGH		22      /* 16 + 2*3 */
#define TITLE_HIGH		( LINE_HIGH + 6 )
#define SELE_AREA_HIGH		( KEY_HIGH + 2 )
#define EXPL_KEY_LEN		84      /* 5*16 + 2*3 + 2*3 */
#define SEARCH_KEY_LEN		45		/* 2*16 + 2*3 + 2*8 */
#define LEVEL_KEY_LEN		( MENU_LINE_LEN*8/3 )

#define HELP_EX			( HELP_SX + BORDER*2 + BACK_IN*2 + LINE_LEN*8 - 1 )
#define HELP_EY			( HELP_SY + BORDER*2 + TITLE_HIGH	\
						+ MAX_LINES*LINE_HIGH + SELE_AREA_HIGH - 1 )

#define TEXT_SX			( HELP_SX + BORDER + BACK_IN )
#define TEXT_EX                 ( HELP_EX - BORDER - BACK_IN )
#define	TEXT_SY			( HELP_SY + BORDER + TITLE_HIGH )
#define TEXT_EY			( HELP_EY - BORDER - SELE_AREA_HIGH )

#define KEY_SY			( TEXT_EY + 1 + ( (SELE_AREA_HIGH-KEY_HIGH)/2 )+1 )
#define KEY_DELAY		0

#define PROMPT_SX		( TEXT_EX-128 )		/*128 = 8*16 */


#define WIN_BKCOLOR	11	/* Background color of help window */
#define TITLE_BKCOLOR   7
#define TITLE_COLOR 	0
#define	TEXT_COLOR    	0	/* text's color of help text & help menu */
#define HL_BKCOLOR 	12      /* highlight background color of help menu */
#define	HL_TEXT_COLOR   14	/* highlight text's color of help menu */
#define WARN_COLOR	12	/* character's color of warn information */

#define MAX_HELP_LEVEL 	4
#define MAX_HELP_NODE 	999
#define MAX_PAGES	99

