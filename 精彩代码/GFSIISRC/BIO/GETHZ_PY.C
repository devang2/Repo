#include <string.h>
#include <stdio.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>

#define MAX_PY_KIND 402
	/* GETHZ_0.C */
typedef struct
	{
		UI hz_pos;
		UC py[6];
		UC len;
		UC hz_num;
	}  PYM_STRUCT;
extern PYM_STRUCT *Pym;
extern UC *Pyhz_buf;
extern FILE *Fp_lspy;
	/* GETHZ_1.C */
extern void draw_prompt(UC kind);
extern void disp_pyhz_prompt(UC *hz_list, UC *page_no);
extern void clr_pyhz_prompt(void);
extern write_xxm(UC *xxm);
extern disp_input_cursor(UC xxm_len);
	/* GETHZ_2.C */
extern void ins_hz(UC qh, UC wh);

static UC Py_str[7];		/* The input py string, fix lenth string. */
static UI Pym_pos; 		/* Position of this pym in the pyno.dat	*/
static UI Hz_pos;			/* Position of the 1st hz of this pym in the lspy.dat */
static UC Hz_num;			/* The hz number of this kind of pym */
static UC Page_no; 		/* The current display page NO.  */

/* ------------------------------------------------------------------------
	FUNCTION:	Make and display the HanZi selected information, include
				page No.
	CALLS:		disp_pyhz_prompt().
	CALLED:		py_input().
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   ------------------------------------------------------------------------ */
static void pyhz_prompt(void)
{
UC hz_left;
UC pos;
UC *hz_pos;
UC hz_number;
UC prompt_hz_list[41];
UC prompt_page_no[8];

	memset(prompt_hz_list, 0, 41);
	strcpy(prompt_page_no, "[00,00]");

	hz_left = Hz_num-Page_no*10;
	hz_number = 0;
	while (hz_number<10 && hz_number<hz_left)
	{
		pos = hz_number*4;
		prompt_hz_list[pos++] = ( (hz_number+1)%10 ) + 0x30;
		hz_pos = Pyhz_buf+( (hz_number+Page_no*10)<<1 );
		prompt_hz_list[pos++] = *hz_pos;
		prompt_hz_list[pos++] = *(hz_pos+1);
		prompt_hz_list[pos++] = ' ';

		hz_number++;
	}

	prompt_page_no[1] = (Page_no+1)/10+0x30;
	prompt_page_no[2] = (Page_no+1)%10+0x30;
	if (Hz_num>Page_no*10+10)
	{
		prompt_page_no[4] = 161;         /* right arrow */
		prompt_page_no[5] = 250;

	}
	else
	{
		prompt_page_no[4] = 161;         /* down arrow  */
		prompt_page_no[5] = 253;
	}

	disp_pyhz_prompt(prompt_hz_list, prompt_page_no);
}

/* -----------------------------------------------------------------------
	FUNCTION:	This function locate the position of the complete or NOT
				complete PYM in the pym_table.
				The input PYM is in Py_str.
	CALLS:		None.
	CALLED BY:	py_input().
	INPUT:		len -- indicate the length of input PYM in Py_str.
	OUTPUT:		None.
	RETURN:		0 -- If can't locate (No such input PYM).
				1 -- If can locate, but the input PYM is not complete.
					 (The input PYM is a part of a PYM in Pym_table, and it
					  can't be a complete PYM.)
				2 -- If can locate and the input PYM is complete.
					 (The input PYM can be a complete PYM, although it may be
					  a part of another PYM in the Pym_table.)
   ----------------------------------------------------------------------- */
static UC pym_locate(UC len)
{
UI low, high, cur;
UC search_ok;
SI cmp_resoult;

	low  = 0;
	high = MAX_PY_KIND-1;
	search_ok = FALSE;
	if ( memcmp( (Pym+low)->py, Py_str, len )==0 )
	{
		if ( len==(Pym+low)->len )
		{
			Pym_pos = low;
			return 2;
		}
		search_ok = TRUE;
	}
	if ( memcmp( (Pym+high)->py, Py_str, len )==0 )
	{
		if ( len==(Pym+high)->len )
		{
			Pym_pos = high;
			return 2;
		}
		search_ok = TRUE;
	}

	cur = (low+high)>>1;
	while ( high>low+1 )
	{
		cmp_resoult = memcmp( (Pym+cur)->py, Py_str, len );
		if ( cmp_resoult==0 )
		{
			if ( (Pym+cur)->len==len )
			{
				Pym_pos = cur;
				return 2;
			}
			if ( (Pym+cur)->len>len )
				cmp_resoult = 1;
			else
				cmp_resoult = -1;
			search_ok = TRUE;
		}
		if ( cmp_resoult>0 )
			high = cur;
		else
			low = cur;
		cur = (low+high)>>1;
	}

	if ( search_ok )
		return 1;
	else
		return 0;
}

/* ------------------------------------------------------------------------
	FUNCTION:	If a PYM is located, read in the HanZis of it from disk to
				Pyhz_buf, and initate some global variables for HanZi selecting.
	CALLS:		None.
	CALLED:		py_input();
   ------------------------------------------------------------------------ */
static void hz_read(void)
{

	Hz_pos = (Pym+Pym_pos)->hz_pos;
	Hz_num = (Pym+Pym_pos)->hz_num;

	fseek(Fp_lspy, Hz_pos<<1, SEEK_SET);
	if ( fread(Pyhz_buf, 2, Hz_num, Fp_lspy)!=Hz_num )
		exit_scr(1, "\n\rGETHZ_PY.C/prompt(): Read file lspy.dat error.");

	Page_no = 0;
}

/* -------------------------------------------------------------------------
	FUNCTION:	Select one HanZi from those which list in the prompt-window.
	CALLS:		ins_hz();
	CALLED BY:	Function: py_input(), get_hz_str().
	INPUT:		no -- indicates the one to be select.
					  0 .. 9 releax to HanZi No 1 .. No 10.
	OUTPUT:     None.
	RETURN:		None.
   ------------------------------------------------------------------------- */
void select_hz(UC no)
{
UC cur_pos;

	if ( (cur_pos = Page_no*10+no) >= Hz_num )
	{
		sound_alarm();
		return;
	}
	cur_pos = cur_pos<<1;

	ins_hz(*(Pyhz_buf+cur_pos)-160, *(Pyhz_buf+cur_pos+1)-160);
}

/* -------------------------------------------------------------------------
	FUNCTION:	List HanZis of the last page.
	CALLS:		None.
	CALLED BY:	Function: py_input(), get_hz_str().
   ------------------------------------------------------------------------- */
void fore_page(void)
{
	if (Page_no==0)
	{
		sound_alarm();
		return;
	}
	Page_no--;
	pyhz_prompt();
}

/* -------------------------------------------------------------------------
	FUNCTION:	List HanZis of the next page.
	CALLS:		None.
	CALLED BY:	Function: py_input(), get_hz_str().
   ------------------------------------------------------------------------- */
void next_page(void)
{
	if (Hz_num<=Page_no*10+10)
	{
		sound_alarm();
		return;
	}
	Page_no++;
	pyhz_prompt();
}

/* -------------------------------------------------------------------------
	FUNCTION:   Read the PYM input, and select the HanZi needed.
	CALLS:		Many.
	CALLED BY:	get_hz_str().
	INPUT:      keyin -- The first letter of the PYM input.
	OUTPUT:
	RETURN:     0..3 -- Return stat is PY, GB, English, DB.
				0xFD -- Return stat is PY, and continuously selection is vailable.
				0xFE -- <Esc> pressed, will escape from get_hz_str after return.
				0xFF -- <Enter> pressed, will escape from get_hz_str() after return.
   ------------------------------------------------------------------------- */
UC py_input(UI keyin)
{
UC pym_len;
UI key;
UC res;
UC can_select;

	memset(Py_str, 0, 7);
	Py_str[0] = keyin;

	pym_len = 1;
	res = pym_locate(pym_len);
	switch (res)
	{
	case 0:
		sound_bell();
		return 0;

	case 1:
		write_xxm(Py_str);
		clr_pyhz_prompt();
		can_select = FALSE;
		break;

	case 2:
		write_xxm(Py_str);
		hz_read();
		pyhz_prompt();
		can_select = TRUE;
		break;
	}

	while ( TRUE )
	{
		clr_keybuf();
		while ( keybuf_nul() )
			disp_input_cursor(pym_len);

		key = get_key0();
		switch(key)
		{
		case PAGEUP:
			draw_prompt(3);
			return 3;

		case PAGEDOWN:
			draw_prompt(1);
			return 1;

		case ENTER:
			return 0xFF;

		case ESC:
			return 0xFE;

		case BACKSPACE:
			if (pym_len==1)
			{
				write_xxm("");
				clr_pyhz_prompt();
				return(0);
			}
			Py_str[--pym_len] = 0;
			write_xxm(Py_str);
			res = pym_locate( pym_len );
			switch (res)
			{
			case 0:
				sound_alarm();
				break;

			case 1:
				clr_pyhz_prompt();
				can_select = FALSE;
				break;

			case 2:
				hz_read();
				pyhz_prompt();
				can_select = TRUE;
				break;
			}
			break;

		case MINUS:
			if (!can_select)
				break;
			fore_page();
			break;

		case PLUS:
			if (!can_select)
				break;
			next_page();
			break;

		default:
			if (key==SPACE)
				key = 0x0031;
			key &=0x00ff;
			if (can_select && key>='0' && key<='9')
			{
				key -=0x30;
				if ( key==0 )
					key = 10;
				key --;
				select_hz(key);
				write_xxm("");
				return 0xFD;
			}
			else
			{
				if (pym_len==6)
				{
					sound_alarm();
					break;
				}
				if (key<'a' || key>'z')
				{
					sound_bell();
					break;
				}
				Py_str[pym_len++] = key;
				res = pym_locate( pym_len );
				switch (res)
				{
				case 0:
					Py_str[--pym_len] = 0;
					sound_alarm();
					break;

				case 1:
					write_xxm(Py_str);
					clr_pyhz_prompt();
					can_select = FALSE;
					break;

				case 2:
					write_xxm(Py_str);
					hz_read();
					pyhz_prompt();
					can_select = TRUE;
					break;
				}
			}
			break;
		}	/* end of switch(key) */
	}		/* end of while(1) */
}			/* end of py_input()  */
