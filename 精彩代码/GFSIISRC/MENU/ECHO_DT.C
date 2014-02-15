/* ----------------------------------------------------------
	Source file:	ECHO_DT.C
	Author:		Zhang RuiJun.
	Modified:	Bob Kong, 1993/3/26
------------------------------------------------------------- */
#include <dos.h>

#include <def.inc>
#include <func.inc>
#include <menufunc.inc>

extern UL Seconds;
extern UC Cursor_on;

#define	DATE_X		13
#define	DATE_Y 		6
#define	DATE_BC 	0
#define	DATE_CC 	14
#define	WEEK_X 		22
#define	WEEK_Y 		26
#define	WEEK_BC 	0
#define	WEEK_CC 	14
#define	TIME_X 		13
#define	TIME_Y 		46
#define	TIME_BC 	0
#define	TIME_CC 	14

/* --------------------------------------------------------------------
	FUNCTION:	Echo the current time at the very place defined
				at the top.
	CALLS:		None.
	CALLED:		check_event_flag().
	INPUT:		None.
	OUTPUT:		the current time at the very place.
	RETURN:		None.
   -------------------------------------------------------------------- */
void echo_time(void)
{
static UC t_str[9] = {0x20,0x20,':',0x20,0x20,':',0x20,0x20,0};
static UI minutes = 0;
UI minute;
UC sec,min,hour;
UC keep_cursor_stat;

	keep_cursor_stat = Cursor_on;
	cursor_off();

	sec = Seconds%60;
	minute =(UI)(Seconds/60);
	t_str[6] = sec/10+0x30;
	t_str[7] = sec%10+0x30;
	if (minute==minutes)
	{
		disp_str(TIME_X+48, TIME_Y, t_str+6, TIME_BC, TIME_CC);
	}
	else
	{
		minutes = minute;
		min = minute%60;
		t_str[3] = min/10+0x30;
		t_str[4] = min%10+0x30;
		hour = minute/60;
		t_str[0] = hour/10+0x30;
		t_str[1] = hour%10+0x30;
		disp_str(TIME_X, TIME_Y, t_str, TIME_BC, TIME_CC);

		if (minutes==0)
			echo_date(get_date());
	}
	if (keep_cursor_stat==TRUE)
		cursor_on();
}

/* --------------------------------------------------------------------
	FUNCTION:	Echo the current date at the very place defined
				at the top.
	CALLS:		None.
	CALLED:		echo_time(), main(), and everyday-work modula.
	INPUT:		None.
	OUTPUT:		the current time at the very place.
	RETURN:		None.
   -------------------------------------------------------------------- */
void echo_date(UC *date)
{
static UC *week[7] ={"星期日","星期一","星期二","星期三",
				 "星期四","星期五","星期六",};
static UC dat[9] = {0x20,0x20,'.',0x20,0x20,'.',0x20,0x20,0};
UC keep_cursor_stat;

	keep_cursor_stat = Cursor_on;
	cursor_off();
	dat[0] = (date[0]/10)%10+0x30;
	dat[1] = date[0]%10+0x30;
	dat[3] = (date[1]/10)%10+0x30;
	dat[4] = date[1]%10+0x30;
	dat[6] = (date[2]/10)%10+0x30;
	dat[7] = date[2]%10+0x30;
	disp_str(DATE_X,DATE_Y,dat,DATE_BC,DATE_CC);
	disp_str(WEEK_X,WEEK_Y,week[date[3]],WEEK_BC,WEEK_CC);
	if (keep_cursor_stat==TRUE)
		cursor_on();
}
