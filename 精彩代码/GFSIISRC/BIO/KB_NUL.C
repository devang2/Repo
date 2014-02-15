#include <def.inc>
#include <key.inc>
#include <func.inc>
#include <helpfunc.inc>

static UI Help_index = 0;
static UC Help_disabled = FALSE;

void enable_help(void)
{
	Help_disabled = FALSE;
}

void disable_help(void)
{
	Help_disabled = TRUE;
}

void set_help(UI index)
{
	Help_index = index;
}

UC kb_nul(void)
{
static UC Help_on = FALSE;
UI key;

	asm mov ah,1;
	asm int 0x16;
	asm jz no_key;
	key = _AX;

	if (key==F1)
	{
		if (Help_on)
			return FALSE;
		if (Help_disabled)
		{
			clr_kb();
			return TRUE;
		}
		Help_on = TRUE;
		help(Help_index);
		Help_on = FALSE;
		clr_kb();
		return TRUE;
	}
	return FALSE;

no_key:
	return TRUE;
}


