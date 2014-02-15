#include <dos.h>

#include <def.inc>
#include <func.inc>

extern UL *Tick_cnt_ptr;

/* --------------------------------------------------------------------
	FUNCTION:   Suspends execution for interval.
	CALLS:		None.
	CALLED BY:	This is a General-purposed routine with event flags
				processing inside.
	INPUT:		nticks -- suspending period(Unit: TICKS=55ms).
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void delay_tick(UI nticks)
{
UL time1;
UI time;

	time1 = *Tick_cnt_ptr;
	time  = (UI) (*Tick_cnt_ptr - time1);
	while ( time<nticks )
	{
		check_event_flag();
		time = (UI) (*Tick_cnt_ptr - time1);
	}
}

/* --------------------------------------------------------------------
	FUNCTION:   Suspends execution for interval.
	CALLS:		delay();
	CALLED BY:	This is a General-purposed routine with event flags
				processing inside.
	INPUT:		n_10ms -- suspending period(Unit: 10ms).
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void delay_10ms(UI n_10ms)
{
UI loop;

	for (loop=0; loop<n_10ms; loop++)
	{
		check_event_flag();
		delay(10);
	}
}
