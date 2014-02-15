/* --------------------------------------------------------------------
	FILENAME:	sound.c
	FUNCTION:	Offer a few functions on sound controlling.
	AUTHOR:		Bob Kong.
	DATE:		1993-03-20
   -------------------------------------------------------------------- */
#include <dos.h>
#include <def.inc>
#include <func.inc>

/* ========================== sound_one ============================= */
void sound_one(UI frequence, UI n_10ms)
{
	sound(frequence);
	delay_10ms(n_10ms);
	nosound();
}
/* ========================== sound_bell =============================== */
void sound_bell(void)
{
	sound(250);
	delay_10ms(3);
	nosound();
}
/* ========================== sound_alarm ================================ */
void sound_alarm(void)
{
	sound(540);
	delay_10ms(5);
	nosound();
}
