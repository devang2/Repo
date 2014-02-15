#include <def.inc>
#include <func.inc>
#include <menufunc.inc>

extern UC Sec_chg;
/*extern UC Rec_flag;*/

/* ========================== check_event_flag =========================== */
void check_event_flag(void)
{
	if (Sec_chg)
	{
		Sec_chg = FALSE;
		echo_time();
	}
/*	if (Rec_flag)
		buffer_manage();

	echo_commstate();
*/
}

