
					/* time_fun.c */

/*-----------------------------------------------------------------
	FILENAME:	time_fun.c
	FUNCTION:	offering a set of functions about time.
	DATE:		1993.5.6
	AUTHOR:		Fei min
	OTHER:
------------------------------------------------------------------*/

#include	  <conio.h>
#include	  <string.h>
#include 	  <time.h>
#include 	  <stdio.h>
#include 	  <stdlib.h>
#include 	  <def.inc>

UC week_day   (UC *);
UC last_day   (UC *,UC *);
UC next_day   (UC *,UC *);
UC date_right (UC *);
UC time_right (UC *);
UC leap_year  (UI);
UC start_time (UC *, UC *, UL);

/* --------------------------------------------------------------------
	FUNCTION:	Caculate weekday of a certain date.
	   CALLS:	date_right(),
				leap_year ();
   CALLED BY:	This is a general-purposed routine.
	   INPUT:	*date -- specify a certain date;
						 its format is: year, month, day.
	  OUTPUT:	None.
	  RETURN:	0..6 -- stands for Sunday..Saturday.
				7    -- stands for invalid input of date.
---------------------------------------------------------------------- */
UC	week_day (UC *date)
{
UC      year,month,day;
UI	org_year,temp_year;
UC	start_data[3]={93,1,1};        /* 93.1.1. is friday */
UC	month_day[12]={31,28,31,30,31,30,31,31,30,31,30,31};
UI      total_day,total_month_day,total_year_day;

	if (date_right (date) ==FALSE)
		return 7;

	year         =date[0];
	month        =date[1];
	day          =date[2];

	if (year < 93)
		org_year = year +2000;
	else
		org_year = year +1900;
	temp_year = org_year;

	total_year_day  =0;
	while (temp_year > (start_data[0] +1900))
	{
		temp_year--;
		total_year_day += (leap_year(temp_year) ==TRUE)? 366:365;
	}

	total_month_day	= 0;
	while (month >1)
	{
		month--;
		if (month ==2)
			total_month_day += (leap_year (org_year))? 29:28;
		else
			total_month_day += month_day[month-1];
	}
	total_day =total_year_day +total_month_day +day +5 -start_data[2];
	return (total_day % 7);
}
/* -------------------------------------------------------------------
	FUNCTION:	Calculate last day of a specified day.
	   CALLS:	date_right(),
				leap_year ();
       CALLED BY:	This is an general-purposed routine.
	   INPUT:	*now_date:	the date of the specified day.
							format: year, month day.
	  OUTPUT:	*last_date: the date of the last day.
							same format as now_date.
	  RETURN:	TRUE --- if the input date is available.
				FALSE -- if the input date invalid.
--------------------------------------------------------------------- */
UC last_day (UC *now_date,UC *last_date)
{
UI temp_year;
UC year,month,day;
UC month_day[12] ={31,28,31,30,31,30,31,31,30,31,30,31};

	if (date_right (now_date) ==FALSE)
		return FALSE;

	year     =now_date[0];
	month    =now_date[1];
	day      =now_date[2];

	if ( year < 93 )
		temp_year = year +2000;
	else
		temp_year = year +1900;

	if (day ==1)
	{
		month--;
		if (month ==0)
		{
			month =12;
			temp_year--;
		}
		if ((month ==2) && (leap_year(temp_year) ==TRUE))
			day =29;
		else
			day =month_day[month-1];
	}
	else
		day--;
	last_date[0] = (UC)(temp_year %100);
	last_date[1] = month;
	last_date[2] = day;
	return TRUE;
}
/* -------------------------------------------------------------------
	FUNCTION:	Calculate next day of a specified day.
	   CALLS:	date_right ();
				leap_year  ();
       CALLED BY:	This is an general-purposed routine.
	   INPUT:	*now_date:	the date of the specified day.
							format: year, month day.
	  OUTPUT:   *last_date: the date of the next day.
							same format as now_date.
	  RETURN:	TRUE --- if the input date correct.
				FALSE -- if the input date incorrect.
------------------------------------------------------------------- */
UC next_day (UC *now_date,UC *next_date)
{
UI temp_year;
UC year,month,day;
UC month_day[12] ={31,28,31,30,31,30,31,31,30,31,30,31};

	if (date_right (now_date) ==FALSE)
		return FALSE;

	year     =now_date[0];
	month    =now_date[1];
	day      =now_date[2];

	if (year <93)
		temp_year = year +2000;
	else
		temp_year = year +1900;

	if ((day ==month_day[month-1]) || ((month ==2) && (day ==29)))
	{
		if ((month ==2) && (day ==28) && (leap_year(temp_year) ==TRUE))
			day++;
		else
		{
			day =1;
			month++;
		}
		if (month ==13)
		{
			month =1;
			temp_year++;
		}
	}
	else
		day++;
	next_date[0] = (UC)(temp_year %100);
	next_date[1] = month;
	next_date[2] = day;
	return TRUE;
}
/* -------------------------------------------------------------------
	FUNCTION:	Check whether the input date is available or not.
	   CALLS:	leap_year();
       CALLED BY:	This is an general-purposed routine.
	   INPUT:	*date -- the input date,
						 its format is: year, month, day.
	  OUTPUT:	None.
	  RETURN:	TRUE:  if the date is correct.
				FALSE: if not.
--------------------------------------------------------------------- */
UC date_right (UC *date)
{
UI	temp_year;
UC  year,month,day;
UC  month_day[12] ={31,29,31,30,31,30,31,31,30,31,30,31};

	year     =date[0];
	month    =date[1];
	day      =date[2];
	if  (  year  >99
		|| month >12 || month <1
		|| day   >month_day[month-1] || day <1 )
		return FALSE;

	if (year < 93)
		temp_year = year +2000;
	else
		temp_year = year +1900;

	if ((month == 2) && (leap_year(temp_year) ==FALSE) && (day >28))
		return FALSE;
	return TRUE;
}
/* -------------------------------------------------------------------
	FUNCTION:	Check whether the input time is invalid or not.
	   CALLS:	None.
       CALLED BY:	This is an general-purposed routine.
	   INPUT:	*time -- The input time-string;
						 its format is hour, minute, second.
	  OUTPUT:	None.
	  RETURN:	TRUE:  if the input time is correct.
				FALSE: if the input time is invalid.
----------------------------------------------------------------------- */
UC time_right (UC *time)
{
UC hour,minute,second;

	hour    =time[0];
	minute  =time[1];
	second  =time[2];

	if (hour >23 || minute >59 || second >59)
		return FALSE;
	return TRUE;
}
/* -------------------------------------------------------------------
	FUNCTION:	Check whether the input year is a leap year.
	   CALLS:	None.
       CALLED BY:	This is an general-purposed routine.
	   INPUT:	year --	the input year.
	  OUTPUT:	None.
	  RETURN:	TRUE:  if the input year is a leap year.
				FALSE: if NOT.
---------------------------------------------------------------------- */
UC leap_year (UI year)
{
	if ((year %4 ==0) && (year %100 !=0) || (year %400 ==0))
		return TRUE;
	return FALSE;
}
/* --------------------------------------------------------------------
	FUNCTION:	Caculate the start_time via end time & timelong.
	   CALLS:	date_right ();
				time_right ();
				last_day   ();
       CALLED BY:	bills_seek ();
	   INPUT:	*end_time --- the input end time; its format is
							  year, month, day, hour, minute, second.
				timelong  --- the lenght of time between start_time & end_time.
	  OUTPUT:	*start_time.
	  RETURN:	TRUE:  if the input data is correct.
				FALSE: if NOT.
---------------------------------------------------------------------- */
UC start_time (UC *start_time, UC *end_time, UL timelong)
{
UL start_timel;
UL end_timel;
UC start_date[3];
UC end_date[3];
UC end_tm[3];
UL days,i;

	end_tm[0] =end_time[3];
	end_tm[1] =end_time[4];
	end_tm[2] =end_time[5];
	memcpy (end_date,end_time,3);

	if ((date_right(end_time)==FALSE) || (time_right(end_tm)==FALSE)
									  || (timelong==0))
		return FALSE;

	end_timel   =(UL)end_time[3]*3600 +(UL)end_time[4]*60 +(UL)end_time[5];
	days        =(timelong -end_timel) /86400L;
	start_timel =(timelong -end_timel) %86400L;

	if ((days !=0) && (start_timel ==0))
	{
		days--;
		start_timel =86400L;
	}
	if (end_timel >=timelong)
	{
		start_time[0] =end_time[0];
		start_time[1] =end_time[1];
		start_time[2] =end_time[2];

		end_timel    -=timelong;
		start_time[3] =end_timel/3600;
		start_time[4] =(end_timel%3600)/60;
		start_time[5] =end_timel%60;
		return TRUE;
	}
	for (i=0; i<=days; i++)
	{
		last_day (end_date,start_date);
		memcpy   (end_date,start_date,3);
	}
	start_time[0] =start_date[0];
	start_time[1] =start_date[1];
	start_time[2] =start_date[2];

	start_timel   =86400L -start_timel;
	start_time[3] =start_timel/3600;
	start_time[4] =(start_timel%3600)/60;
	start_time[5] =start_timel%60;
	return TRUE;
}
/* ---------- testing start_time ----------------------- */
/*
void main (void)
{
UC s_time[6];
UC e_time[6]={93,8,29,13,4,4};
UL time     =30*86400L +13*3600L +4*60L +4L;
	start_time (s_time,e_time,time);
	getch ();
}
*/
/* ------------ testing nextday & lastday -------------- */
/*
void main (void)
{
UI	temp_year;
UC	now_date[3],last_date[3],next_date[3];
UC	month[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

	randomize ();
	while (1)
	{
		now_date[0] =random (100);
		now_date[1] =random (12) +1;
		now_date[2] =month[now_date[1]];

		if ( now_date[0] < 93 )
			temp_year = now_date[0] +2000;
		else
			temp_year = now_date[0] +1900;
		if ((now_date[1] == 2) && (leap_year(temp_year) ==TRUE))
			now_date[2] =29;

		next_day (now_date, next_date);
		last_day (next_date,last_date);
		if ((now_date[0] != last_date[0]) ||
			(now_date[1] != last_date[1]) || (now_date[2] != last_date[2]))
			break;
		printf ("%02u%02u%02u    %02u%02u%02u\n",
				now_date[0], now_date[1], now_date[2],
				next_date[0],next_date[1],next_date[2]);
	}
}
*/
/* ------------ testing nextday & lastday -------------- */
/*
void main (void)
{
UI	i=0;
UC	now_date[3],last_date[3],next_date[3],mode;
UC	month[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

	randomize ();
	while (1)
	{
		now_date[0] =random (100);
		now_date[1] =random (12) +1;
		now_date[2] =random (month[now_date[1]])+1;
		mode =next_day (now_date, next_date);
		mode =last_day (next_date,last_date);
		if ((now_date[0] != last_date[0]) ||
			(now_date[1] != last_date[1]) || (now_date[2] != last_date[2]))
			break;
		printf ("%02u%02u%02u    %02u%02u%02u\n",
				now_date[0], now_date[1], now_date[2],
				next_date[0],next_date[1],next_date[2]);
	}
}
*/
/* ----------- for test week_day() ---------------- */
/*
void main (void)
{
UC	date[3] ={92,9,9},what_date,i=0;
UC	month[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

	randomize ();
	while ((i++)<250)
	{
		date[0] =random (100);
		date[1] =random (12) +1;
		date[2] =random (month[date[1]])+1;
		what_date =week_day (date);
		printf ("%02u%02u%02u     %u\n",date[0],date[1],date[2],what_date);
	}
}
*/
