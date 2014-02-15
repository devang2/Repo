/* --------------------------------------------------------------------
	FILENAME:	gethz_1.c
	FUNCTION:	This modula offers some routines for get_hz_str().
				These routines are about the prompt-window.
	DATE:		1993.7.28
	AUTHOR:		Bob Kong.
   -------------------------------------------------------------------- */
#include <stdio.h>
#include <graphics.h>
#include <alloc.h>
#include <string.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>
#include <menufunc.inc>

#define MAX_PY_KIND 402

#define PROMPT1_LEN	64			/* length of the title of the input method */

#define PROMPT2_LEN 60			/* length of the code-input window */
#define PROMPT2_SX	( Prompt_sx + PROMPT1_LEN + 1 )

#define PROMPT4_LEN 61          /* length of the next page prompt window */
#define PROMPT4_SX	( Prompt_ex - PROMPT4_LEN + 1 - 9 )

#define LEAST_PROMPT3_LEN	324
#define PROMPT3_SX  ( PROMPT2_SX + PROMPT2_LEN + 8 )
#define PROMPT3_EX	( PROMPT4_SX - 8 - 1 )

#define LEAST_LEN	( PROMPT1_LEN + PROMPT2_LEN + LEAST_PROMPT3_LEN \
					  + PROMPT4_LEN + 8 + 8 + 9 )

#define PROMPT_SX	LD_LEFT+1
#define PROMPT_SY   LD_BOTTOM-22
#define PROMPT_EX   LD_RIGHT-1
#define PROMPT_EY   LD_BOTTOM-1

extern	UC Get_bk_color;
extern	UC Get_cursor_color;
typedef struct
	{
		UI hz_pos;
		UC py[6];
		UC len;
		UC hz_num;
	} PYM_STRUCT;
extern PYM_STRUCT *Pym;

static UI Prompt_sx;
static UI Prompt_sy;
static UI Prompt_ex;
static UI Prompt_ey;
static UC Prompt_set = FALSE;

static UI Prompt_char_sy;

static UC *Keep_prompt_buf;
/* --------------------------------------------------------------------
	FUNCTION:	Initial the Pym structure array. It is a global variable.
	CALLS:
	CALLED BY:  get_hz_str().
	INPUT:		None.
	OUTPUT:     None.
	RETURN:		0 -- if OK,
				1 -- if failed.
   --------------------------------------------------------------------- */
UC init_pym_table(void)
{
FILE *fp_table;

	Pym = (PYM_STRUCT *) mem_alloc( MAX_PY_KIND*sizeof(PYM_STRUCT) );

	fp_table = fopen( "gfsdata\\pyno.dat", "rb" );
	if ( fp_table==NULL )
		return 1;

	fread( Pym, sizeof(PYM_STRUCT), MAX_PY_KIND, fp_table);
	fclose(fp_table);
	return 0;
}

/* -----------------------------------------------------------------------
	FUNCTION:	Set the get-HanZi paramenters about the prompt-window.
				Set the input value to the global static variables:
					(Prompt_sx, Prompt_sy, Prompt_ex, Prompt_ey)
				If the input values are invalid, use the default values:
					(LD_LEFT+1, LD_BOTTOM-22, LD_RIGHT-1, LD_BOTTOM-1)
	CALLS:
	CALLED BY:	This is an general-purposed routine used by the application
				programs.
	INPUT:		(prompt_sx, prompt_sy, prompt_ex, prompt_ey):
					The range of the prompt-window.
				request:	width>=LEAST_LEN && heigth>=22.
	OUTPUT:		None.
	RETURN:		None.
   ----------------------------------------------------------------------- */
void set_gethz_para(UI prompt_sx, UI prompt_sy, UI prompt_ex, UI prompt_ey)
{
	if (prompt_ex-prompt_sx+1 < LEAST_LEN)
		return;
	if (prompt_ey-prompt_sy+1 < 22)
		return;

	Prompt_sx = prompt_sx;
	Prompt_sy = prompt_sy;
	Prompt_ex = prompt_ex;
	Prompt_ey = prompt_ey;
	Prompt_set = TRUE;
}

/* ------------------------------------------------------------------------
	FUNCTION:	Initial the global static variables of get-HanZi-string
				function.
	CALLS:      None.
	CALLED BY:	get_hz_str().
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
void init_gethz_para(void)
{

	if ( !Prompt_set )
	{
		Prompt_sx = PROMPT_SX;
		Prompt_sy = PROMPT_SY;
		Prompt_ex = PROMPT_EX;
		Prompt_ey = PROMPT_EY;
	}
	Prompt_char_sy = ( Prompt_sy + Prompt_ey - 15 )>>1;
	Prompt_set = FALSE;
}

/* ----------------------------------------------------------------------
	FUNCTION:	Save the screen where the prompt-window occupied.
	CALLS:		None.
	CALLED BY:	get_hz_str()
   ---------------------------------------------------------------------- */
void keep_prompt(void)
{
	Keep_prompt_buf = (UC *) mem_alloc
				( imagesize(Prompt_sx, Prompt_sy, Prompt_ex, Prompt_ey) );
	getimage(Prompt_sx, Prompt_sy, Prompt_ex, Prompt_ey, Keep_prompt_buf);
}

/* ----------------------------------------------------------------------
	FUNCTION:	Restore the screen where the prompt-window occupied.
				The screen is saved in the static variable: Keep_prompt_buf
				by function keep_prompt().
	CALLS:		None.
	CALLED BY:	get_hz_str()
   ---------------------------------------------------------------------- */
void restore_prompt(void)
{
	putimage(Prompt_sx, Prompt_sy, Keep_prompt_buf, COPY_PUT);
	farfree( Keep_prompt_buf );
}

/* ----------------------------------------------------------------------
	FUNCTION:	Draw the initial prompt window.
	CALLS:		None.
	CALLED BY:	get_hz_str()
   ---------------------------------------------------------------------- */
void draw_init_prompt(void)
{
	draw_back2(Prompt_sx, Prompt_sy, Prompt_ex, Prompt_ey, 7);
	chang_key2(PROMPT2_SX, Prompt_sy+2, PROMPT2_SX+PROMPT2_LEN-1, Prompt_ey-2,
			   "", 0);
	chang_key2(PROMPT3_SX, Prompt_sy+2, PROMPT3_EX, Prompt_ey-2,
			   "用<PgUp>、<PgDn>键，选择汉字输入的方式。", 0);
	chang_key2(PROMPT4_SX, Prompt_sy+2, PROMPT4_SX+PROMPT4_LEN-1, Prompt_ey-2,
			   "", 0);
	hz16_disp(Prompt_sx+8, Prompt_char_sy, "拼音码：", 0);

	setfillstyle(SOLID_FILL, Get_bk_color);
}

/* ----------------------------------------------------------------------
	FUNCTION:	Draw the varies kind of prompt window.
	CALLS:		None.
	CALLED BY:	get_hz_str(), py_input(), db_input(), gb_input()
	INPUT:      code -- Specify the kind of prompt window to be drew.
						0/1/2/3 = PY/GB/English/DB
	OUTPUT:		None.
	RETURN:		None.
   ---------------------------------------------------------------------- */
void draw_prompt(UC code)
{
	switch (code)
	{
	case 0:
		setfillstyle(SOLID_FILL, 7);
		bar(Prompt_sx+8, Prompt_char_sy, Prompt_sx+PROMPT1_LEN, Prompt_char_sy+15);
		hz16_disp(Prompt_sx+8, Prompt_char_sy, "拼音码：", 0);

		bar(PROMPT2_SX+1, Prompt_char_sy, PROMPT2_SX+PROMPT2_LEN-2, Prompt_char_sy+15);
		bar(PROMPT3_SX+1, Prompt_char_sy, PROMPT3_EX-1, Prompt_char_sy+15);
		bar(PROMPT4_SX+1, Prompt_char_sy, PROMPT4_SX+PROMPT4_LEN-2, Prompt_char_sy+15);
		setfillstyle(SOLID_FILL, Get_bk_color);
		break;

	case 1:
		setfillstyle(SOLID_FILL, 7);
		bar(Prompt_sx+8, Prompt_sy+2, Prompt_sx+PROMPT1_LEN, Prompt_ey-2);
		hz16_disp(Prompt_sx+8, Prompt_char_sy, "区位码：", 0);

		bar(PROMPT2_SX+1, Prompt_char_sy, PROMPT2_SX+PROMPT2_LEN-2, Prompt_char_sy+15);
		bar(PROMPT3_SX+1, Prompt_char_sy, PROMPT3_EX-1, Prompt_char_sy+15);
		bar(PROMPT4_SX+1, Prompt_char_sy, PROMPT4_SX+PROMPT4_LEN-2, Prompt_char_sy+15);
		setfillstyle(SOLID_FILL, Get_bk_color);
		break;

	case 2:
		setfillstyle(SOLID_FILL, 7);
		bar(Prompt_sx+8, Prompt_char_sy, Prompt_sx+PROMPT1_LEN, Prompt_char_sy+15);
		hz16_disp(Prompt_sx+8, Prompt_char_sy, "西　文: ", 0);

		bar(PROMPT2_SX+1, Prompt_char_sy, PROMPT2_SX+PROMPT2_LEN-2, Prompt_char_sy+15);
		bar(PROMPT3_SX+1, Prompt_char_sy, PROMPT3_EX-1, Prompt_char_sy+15);
		bar(PROMPT4_SX+1, Prompt_char_sy, PROMPT4_SX+PROMPT4_LEN-2, Prompt_char_sy+15);
		setfillstyle(SOLID_FILL, Get_bk_color);
		break;

	case 3:
		setfillstyle(SOLID_FILL, 7);
		bar(Prompt_sx+8, Prompt_char_sy, Prompt_sx+PROMPT1_LEN, Prompt_ey-2);
		hz16_disp(Prompt_sx+8, Prompt_char_sy, "电报码：", 0);

		bar(PROMPT2_SX+1, Prompt_char_sy, PROMPT2_SX+PROMPT2_LEN-2, Prompt_char_sy+15);
		bar(PROMPT3_SX+1, Prompt_char_sy, PROMPT3_EX-1, Prompt_char_sy+15);
		bar(PROMPT4_SX+1, Prompt_char_sy, PROMPT4_SX+PROMPT4_LEN-2, Prompt_char_sy+15);
		setfillstyle(SOLID_FILL, Get_bk_color);
		break;

	default:
		draw_back2(Prompt_sx, Prompt_sy, Prompt_ex, Prompt_ey, 7);

	}
}

/* ------------------------------------------------------------------------
	FUNCTION:	Display the HanZi of one PYM to be selected, include the
				page No information.
	CALLS:      None.
	CALLED BY:  Function pyhz_prompt() in modula gethz_py.
	INPUT:		*hz_list -- The string includes HanZi to be selected.
				*page_no -- The string includes page No information.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
void disp_pyhz_prompt(UC *hz_list, UC *page_no)
{
	setfillstyle(SOLID_FILL, 7);

	bar(PROMPT3_SX+1, Prompt_char_sy, PROMPT3_EX-1, Prompt_char_sy+15);
	topic_disp(PROMPT3_SX+2, Prompt_char_sy, hz_list, 0, 1);

	bar(PROMPT4_SX+1, Prompt_char_sy, PROMPT4_SX+PROMPT4_LEN-2, Prompt_char_sy+15);
	hz16_disp(PROMPT4_SX+2, Prompt_char_sy, page_no, 12);

	setfillstyle(SOLID_FILL, Get_bk_color);
}

/* ----------------------------------------------------------------------
	FUNCTION:	Clears two parts of the prompt-window: the HanZi-selected
				area and the page No displayed area.
	CALLS:		None.
	CALLED BY:	Modula GETHZ_PY.
   ---------------------------------------------------------------------- */
void clr_pyhz_prompt(void)
{
	setfillstyle(SOLID_FILL, 7);
	bar(PROMPT3_SX+1, Prompt_char_sy, PROMPT3_EX-1, Prompt_char_sy+15);
	bar(PROMPT4_SX+1, Prompt_char_sy, PROMPT4_SX+PROMPT4_LEN-2, Prompt_char_sy+15);

	setfillstyle(SOLID_FILL, Get_bk_color);
}

/* ----------------------------------------------------------------------
	FUNCTION:	Display the PYM, DBM or GBM input in the prompt window.
	CALLS:		None.
	CALLED BY:	Function: py_input(), db_input(), gb_input().
	INPUT:		*xxm -- The string of PYM, DBM or GBM to be displayed.
	OUTPUT:
	RETURN:		None.
   ---------------------------------------------------------------------- */
void write_xxm(UC *xxm)
{
	setfillstyle(SOLID_FILL, 7);
	bar(PROMPT2_SX+1, Prompt_char_sy, PROMPT2_SX+PROMPT2_LEN-2, Prompt_char_sy+15);
	hz16_disp(PROMPT2_SX+2, Prompt_char_sy-1, xxm, 0);

	setfillstyle(SOLID_FILL, Get_bk_color);
}

/* ----------------------------------------------------------------------
	FUNCTION:	Display the flashing cursor at the next place to the
				inputed PYM, DBM or GBM.
	CALLS:		None.
	CALLED BY:	Function: py_input(), db_input(), gb_input().
	INPUT:		xxm_len -- The length of PYM, DBM or GBM. It indicates the
							place where the flashing cursor displayed.
	OUTPUT:
	RETURN:		None.
   ---------------------------------------------------------------------- */
void disp_input_cursor(UC xxm_len)
{
	disp_cursor(PROMPT2_SX+2+(xxm_len<<3), Prompt_char_sy+14, Get_bk_color, Get_cursor_color);
}

