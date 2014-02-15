#include <dos.h>
#include <def.inc>

UI scasw(UI far *word_str, UI word_unit, UI range)
{
UI tmp;

	asm mov cx, range;
	asm les di, word_str;
	asm mov dx, di;
	asm mov ax, word_unit;
	asm cld;
	asm repne scasw;
	asm sub di, dx;
	asm mov dx, di;
	asm shr dx, 1;

	tmp = _DX;
	if (tmp==range)
	{
		tmp--;
		if (word_str[tmp]==word_unit)
			return(tmp);
		else
			return(0xFFFF);
	}
	else
		return(--tmp);
}
