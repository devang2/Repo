/* --------------------------------------------------------------------
	FILENAME:	DATETIME.C
	FUNCTION:	Offers some functions on getting or setting the
				date and time.
	AUTHOR:		Bob Kong.
	DATE:       1993-05-24
   -------------------------------------------------------------------- */
#include <dos.h>

#include <def.inc>
#include <func.inc>

extern UL far 	*Tick_cnt_ptr;

/* --------------------------------------------------------------------
	FUNCTION:	Get the current date.
	CALLS:		None.
	CALLED:		This is a general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		The pointer to the date string.
			the date string format: Year, Month, Day, Weekday.
   -------------------------------------------------------------------- */
UC *get_date(void)
{
static	UC date[4];

	_AH = 0x2A;
	geninterrupt(0x21);
	date[1] = _DH;
	date[2] = _DL;
	date[3] = _AL;
	date[0] = _CX%100;

	return( date );
}

/* --------------------------------------------------------------------
	FUNCTION:	Get the current time.
	CALLS:		None.
	CALLED:		This is a general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		The pointer to the time string.
			    the time string format: Hour, Minute, Second.
   -------------------------------------------------------------------- */
UC *get_time(void)
{
UL	   seconds;
UI 	   minute;
static	UC time[3];

	seconds = (*Tick_cnt_ptr<<11)/37287U;
	minute = (UI)(seconds/60);

	time[2] = seconds%60;	    /* second */
	time[1] = minute%60;        /* minute */
	time[0] = minute/60;        /* hour   */

	return (time);
}

/* --------------------------------------------------------------------
	FUNCTION:	Set the current date.
	CALLS:		None.
	CALLED:		This is a general-purposed routine.
	INPUT:		date[3] -- store the NEW date to be set.
	OUTPUT:		None.
	RETURN:		TRUE -- if OK,
				FALSE -- if date invalid.
   -------------------------------------------------------------------- */
UC set_date(UC date[3])
{
UI year;
UC resoult;

	if ( date[0]>90 )
		year = date[0]+1900;
	else
		year = date[0]+2000;

	_CX = year;
	_DH = date[1];
	_DL = date[2];
	_AH = 0x2B;
	geninterrupt(0x21);
	resoult = _AL;

	if (resoult==0)
		return TRUE;
	else
		return FALSE;
}

/* --------------------------------------------------------------------
	FUNCTION:	Set the current time.
	CALLS:		None.
	CALLED:		This is a general-purposed routine.
	INPUT:		time[3] -- stores the time to be set.
	OUTPUT:		None.
	RETURN:		TRUE -- if OK,
				FALSE -- if time invalid.
   -------------------------------------------------------------------- */
UC set_time(UC time[3])
{
UC resoult;

	_CH = time[0];
	_CL = time[1];
	_DH = time[2];
	_DL = 0;
	_AH = 0x2D;
	geninterrupt(0x21);
	resoult = _AL;

	if (resoult==0)
		return TRUE;
	else
		return FALSE;

}

