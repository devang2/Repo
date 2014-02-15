#include <bio.inc>
#include "feedef.h"

/* function  : judge the end time of calling from the starting time
 * called by :
 * date      : 1993.9.27
 */
void just_date(void)
{

    if( (temp_list.hour2 - temp_list.hour1) < 0)
	temp_list.day2 = temp_list.day2+1;

    switch(temp_list.mon2)
    {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	     if(temp_list.day2 > 31)
	     {
		  temp_list.day2 = 1;
		  temp_list.mon2 ++;
	     }
	     break;
	case 4:
	case 6:
	case 9:
	case 11:
	     if(temp_list.day2 > 30)
	     {
		 temp_list.day2 = 1;
		 temp_list.mon2 ++;
	      }
	      break;
	case 2:
	     if(leap_year(temp_list.year2))
	     {
		  if(temp_list.day2 > 29)
		  {
		     temp_list.day2 = 1;
		     temp_list.mon2 = 3;
		  }
	     }
	     else
	     {
		 if(temp_list.day2 > 28)
		 {
		     temp_list.day2 = 1;
		     temp_list.mon2 = 3;
		  }
	     }
	     break;
	case 12:
	     if(temp_list.day2 > 31)
	     {
		 temp_list.day2  = 1;
		 temp_list.mon2  = 1;
		 temp_list.year2 ++;
	     }
	     break;
	default:
	     break;
	 }

    return;
}

/* function  : judge whether it is two continuous day
 *             when temp_list.mon1 != temp_list.mon2, in the same year  OR
 *             when temp_list.year1 != temp_list.year2
 * called by : charge_hour()
 * date      : 1993.9.29
 */
UC is_cont_day(void)
{
    UC flag;
    UC day;

    switch(temp_list.mon1)
    {
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
            if((temp_list.day1-temp_list.day2) == 30)
		flag = TRUE;
	    else
		flag = FALSE;
	    break;
	case 4:
	case 6:
	case 9:
	case 11:
	    if ((temp_list.day1-temp_list.day2)==29)
		flag = TRUE;
	    else
		flag = FALSE;
	    break;
	case 2:
            if(leap_year(temp_list.year1))
		day = 29;
	    else
		day = 28;

	    if(temp_list.day1 == day)
		if(((temp_list.day1-temp_list.day2)==27) ||
		   ((temp_list.day1-temp_list.day2)==28)   )
		    flag = TRUE;
	    else
		    flag = FALSE;
	    break;
	case 12:
	    if((temp_list.year2-temp_list.year1) == 1)
	       if(temp_list.mon2==1)
		  if((temp_list.day1==31) && (temp_list.day2==1))
		     flag = TRUE;
	    else
		     flag = FALSE;
	    break;
    }    /* end of switch(temp_list.mon1) */

    return(flag);
}

/* function  : calculate the week day of year:month:day
 * called by : all_record_transfer(), imm_record_transfer()
 * input     : year:month:day -- date
 * output    : week day
 * date      : 1993.9.28
 */
UC weekday(UI year, UC month, UC day)
{
    UC day0;         /* February days */
    UL days;         /* interval to 1991,1,1 */
    UC Fix_week=2;   /* 1991,1,1 Tue */
    UC week;         /* return value */
    UI year_D=0;
    int i;

    for(i=1991;i<year;i++)
    {
	if (((i%4==0) && (i%100!=0)) || (i%400==0))   /* leap year */
	{
		year_D=year_D+366;
		year_D=year_D%7;
	}
	else
	{
		year_D=year_D+365;
		year_D=year_D%7;
	}
    }
    if (((year%4==0) && (year%100!=0)) || (year%400==0))
	day0=29;
    else
	day0=28;      /* February days */

    if      (month==1)  days=year_D+day;
    else if (month==2)  days=year_D+31+day;
    else if (month==3)  days=year_D+31+day0+day;
    else if (month==4)  days=year_D+31+day0+31+day;
    else if (month==5)  days=year_D+31+day0+31+30+day;
    else if (month==6)  days=year_D+31+day0+31+30+31+day;
    else if (month==7)  days=year_D+31+day0+31+30+31+30+day;
    else if (month==8)  days=year_D+31+day0+31+30+31+30+31+day;
    else if (month==9)  days=year_D+31+day0+31+30+31+30+31+31+day;
    else if (month==10) days=year_D+31+day0+31+30+31+30+31+31+30+day;
    else if (month==11) days=year_D+31+day0+31+30+31+30+31+31+30+31+day;
    else if (month==12) days=year_D+31+day0+31+30+31+30+31+31+30+31+30+day;

    week = (Fix_week+days-1)%7;
    return (week);
}

/* function  : in the same day, and have half fee
 * called by : nation_charge()
 * date      : 1993.9.28
 */
void holiday_all_half(void)
{
    semi_hour += temp_list.hour2-temp_list.hour1;
    semi_min  += temp_list.min2-temp_list.min1;
    semi_sec  += temp_list.sec2-temp_list.sec1;

    return;
}

/* function  : in the same day, judge half_fee hours and all_fee hours
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void nation_same_day_hour(void)
{
    if(temp_list.hour1 >= half_fee.AM_hour)      /* past up-half time-zone */
    {
	if(temp_list.hour1 >= half_fee.PM_hour)  /*all in dn-half time-zone*/
	{
	    semi_hour += temp_list.hour2-temp_list.hour1;
	    semi_min  += temp_list.min2-temp_list.min1;
	    semi_sec  += temp_list.sec2-temp_list.sec1;
	}
	else                 /* between the two time-zone */
	{
	    if(temp_list.hour2 >= half_fee.PM_hour)   /* in dn-half zone   */
	    {
		hour += half_fee.PM_hour-temp_list.hour1;
		min  += 0-temp_list.min1;
		sec  += 0-temp_list.sec1;
		semi_hour += temp_list.hour2-half_fee.PM_hour;
		semi_min  += temp_list.min2;
		semi_sec  += temp_list.sec2;
	    }
	    else             /* all in all-fee time-zone */
	    {
		hour += temp_list.hour2-temp_list.hour1;
		min  += temp_list.min2-temp_list.min1;
		sec  += temp_list.sec2-temp_list.sec1;
	    }
	}     /* end of "else if(temp_list.hour1 < half_fee.PM_hour)" */
    }         /* end of "if(temp_list.hour1 >= half_fee.AM_hour)"     */
    else      /* hour1 in the up-half time-zone */
    {
	if(temp_list.hour2 < half_fee.AM_hour)
	{
	    semi_hour += temp_list.hour2-temp_list.hour1;
	    semi_min  += temp_list.min2-temp_list.min1;
	    semi_sec  += temp_list.sec2-temp_list.sec1;
	}
	else if(temp_list.hour2 < half_fee.PM_hour)
	{
	    semi_hour += half_fee.AM_hour-temp_list.hour1;
	    semi_min  += 0-temp_list.min1;
	    semi_sec  += 0-temp_list.sec1;
	    hour += temp_list.hour2-half_fee.AM_hour;
	    min  += temp_list.min2;
	    sec  += temp_list.sec2;
	}
	else
	{
	    semi_hour += half_fee.AM_hour-temp_list.hour1+  \
			 temp_list.hour2-half_fee.PM_hour;
	    semi_min  += temp_list.min2-temp_list.min1;
	    semi_sec  += temp_list.sec2-temp_list.sec1;
	    hour      += half_fee.PM_hour-half_fee.AM_hour;
	}
    }    /* end of "else if(temp_list.hour1 < half_fee.AM_hour)" */

    return;
}

/* function  : hour1 are in holiday half time-zone
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void holiday_up_half(void)
{
    semi_hour += 24-temp_list.hour1;
    semi_min  += 0-temp_list.min1;
    semi_sec  += 0-temp_list.sec1;

    return;
}

/* function  : in two continuous days, judge half-fee hours and all-fee hours
 *             for temp_list.hour1
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void nation_two_day_up(void)
{
    if(temp_list.hour1 >= half_fee.AM_hour)      /* out of up-half zone */
    {
	if(temp_list.hour1 >= half_fee.PM_hour)  /* in dn-half zone     */
	{
	    semi_hour += 24-temp_list.hour1;
	    semi_min  += 0-temp_list.min1;
	    semi_sec  += 0-temp_list.sec1;
        }
	else       /* in no half fee zone */
	{
	    hour += half_fee.PM_hour-temp_list.hour1;
	    min  += 0-temp_list.min1;
	    sec  += 0-temp_list.sec1;
	    semi_hour += 24-half_fee.PM_hour;
        }
    }         /* end of "if(temp_list.hour1 >= half_fee.AM_hour)" */
    else      /* in up-half zone */
    {
	hour += half_fee.PM_hour-half_fee.AM_hour;
	semi_hour += 24-half_fee.PM_hour+half_fee.AM_hour-temp_list.hour1;
	semi_min  += 0-temp_list.min1;
	semi_sec  += 0-temp_list.sec1;
    }

    return;
}

/* function  : in two continuous days, temp_list.hour2 is in holiday half fee
 *             time zone
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void holiday_dn_half(void)
{
    semi_hour += temp_list.hour2;
    semi_min  += temp_list.min2;
    semi_sec  += temp_list.sec2;

    return;
}

/* function  : in two continuous days, judge half-fee hours and all-fee hours
 *             for temp_list.hour2
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void nation_two_day_dn(void)
{
    if(temp_list.hour2 < half_fee.AM_hour)
    {
	semi_hour += temp_list.hour2;
	semi_min  += temp_list.min2;
	semi_sec  += temp_list.sec2;
    }
    else if(temp_list.hour2 < half_fee.PM_hour)
    {
	semi_hour += half_fee.AM_hour;
	hour += temp_list.hour2-half_fee.AM_hour;
	min  += temp_list.min2;
	sec  += temp_list.sec2;
    }
    else
    {
	semi_hour += half_fee.AM_hour+temp_list.hour2-half_fee.PM_hour;
	semi_min  += temp_list.min2;
	semi_sec  += temp_list.sec2;
	hour += half_fee.PM_hour-half_fee.AM_hour;
    }

    return;
}

/* function  : day1 and day2 are in the same day, calculate half fee time
 *             (semi_hour,semi_min & semi_sec) and all fee time(hour,min & sec)
 *             respectively
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void nation_same_day_charge(void)
{
    int i;

    for(i=0;i<7;i++)        /* judge half fee day */
        if((temp_list.day1==holiday[i].day) &&
           (temp_list.mon1==holiday[i].mon))
            break;
    if(i<7)
    {
        if( (holiday[i].flag) ||
	  ( (week1==0) && half_fee.sunday) ||
	  ( (week1==6) && half_fee.saturday) ) /* effect */
            holiday_all_half();
        else
	    nation_same_day_hour();
    }
    else if( ( (week1==0) && half_fee.sunday) ||
             ( (week1==6) && half_fee.saturday) )
        holiday_all_half();
    else
	nation_same_day_hour();

    return;
}

/* function  : day1 and day2 are two continuous day, calculate half fee time
 *             (semi_hour,semi_min & semi_sec) and all fee time(hour,min & sec)
 *             respectively
 * called by : charge_hour()
 * date      : 1993.9.28
 */
void nation_cont_day_charge(void)
{
    int i;

    /* judge temp_list.hour1           */
    for(i=0;i<7;i++)
        if((temp_list.day1 == holiday[i].day) &&
           (temp_list.mon1 == holiday[i].mon))
            break;
    if(i<7)
    {
        if( (holiday[i].flag==1) ||
	  ( (week1==0) && half_fee.sunday) ||
	  ( (week1==6) && half_fee.saturday) )
            holiday_up_half();
        else
	    nation_two_day_up();
    }
    else if( ( (week1==0) && half_fee.sunday) ||
    	     ( (week1==6) && half_fee.saturday) )
        holiday_up_half();
    else
	nation_two_day_up();

    /* judge temp_list.hour2 */
    for(i=0;i<7;i++)
    {
        if((temp_list.day2 == holiday[i].day) &&
           (temp_list.mon2 == holiday[i].mon))
            break;
    }
    if(i<7)
    {
        if( (holiday[i].flag == 1) ||
	  ( (week2==0) && half_fee.sunday ) ||
	  ( (week2==6) && half_fee.saturday ) )
            holiday_dn_half();
        else
	    nation_two_day_dn();
    }
    else if( ( (week2==0) && half_fee.sunday) ||
             ( (week2==6) && half_fee.saturday) )
        holiday_dn_half();
    else
	nation_two_day_dn();

    return;
}

/* function  : calculate half fee time(semi_hour,semi_min & semi_sec)
 *             and all fee time(hour,min & sec) respectively, for
 *             national call only
 * called by : nation_charge()
 * date      : 1993.9.28
 */
UC nation_charge_hour(void)
{
    int day;

    semi_hour = 0;
    semi_min = 0;
    semi_sec = 0;
    hour = 0;
    min = 0;
    sec = 0;

    if(temp_list.year2 == temp_list.year1)       /* the same year */
    {
	if(temp_list.mon2 == temp_list.mon1)     /* the same month*/
	{
            day = temp_list.day2-temp_list.day1;
	    if((day>=0) && (day<=1))
	    {
		if(day == 0)                /* the same day */
		{
		    nation_same_day_charge();     /* charge hour  */
		    return(TRUE);
		}
		else              /* in continuous two days */
		{
		    nation_cont_day_charge();
		    return(TRUE);
		}
	    }      /* end of "if((day>=0) && (day<=1))"     */

	    else   /* calling time is too long  */
		return(FALSE);

	}          /* end of "if(temp_list.mon2 == temp_list.mon1)"    */

	else if((temp_list.mon2-temp_list.mon1) == 1)  /*different mon */
	{
	   if(is_cont_day())      /* day1 and day2 are continuous days */
	   {
	       nation_cont_day_charge();
	       return(TRUE);
	   }
	   else    /* calling time is too long */
	       return(FALSE);
	}

	else       /* not continuous month, so calling time is too long */
	    return(FALSE);
    }    /* end of "if(temp_list.year2 == temp_list.year1)"             */

    else      /* different year */
    {
	if(is_cont_day())    /* day1 and day2 are still continuous days */
	{
	    nation_cont_day_charge();
	    return(TRUE);
	}
	else
	    return(FALSE);
    }
}

/* function  : day1 and day2 are in the same day, calculate calling time
 * called by : normal_charge_hour()
 * date      : 1993.9.29
 */
void normal_same_day_charge(void)
{
    hour = temp_list.hour2-temp_list.hour1;
    min  = temp_list.min2-temp_list.min1;
    sec  = temp_list.sec2-temp_list.sec1;

    return;
}

/* function  : day1 and day2 are two continuous days, calculate calling time
 * called by : normal_charge_hour()
 * date      : 1993.9.29
 */
void normal_cont_day_charge(void)
{
     hour = 24-temp_list.hour1;
     min  = 0-temp_list.min1;
     sec  = 0-temp_list.sec1;

     hour += temp_list.hour2;
     min  += temp_list.min2;
     sec  += temp_list.sec2;

    return;
}

/* function  : calculate calling time for international call and local call
 * called by : intern_charge(),local_charge()
 * date      : 1993.9.28
 */
UC normal_charge_hour(void)
{
    int day;

    hour = 0;
    min = 0;
    sec = 0;

    if(temp_list.year2 == temp_list.year1)       /* the same year */
    {
	if(temp_list.mon2 == temp_list.mon1)     /* the same month*/
	{
            day = temp_list.day2-temp_list.day1;
	    if((day>=0) && (day<=1))
	    {
		if(day == 0)                /* the same day */
		{
		    normal_same_day_charge();     /* charge hour  */
		    return(TRUE);
		}
		else              /* in continuous two days */
		{
		    normal_cont_day_charge();
		    return(TRUE);
		}
	    }      /* end of "if((day>=0) && (day<=1))"     */

	    else   /* calling time is too long  */
		return(FALSE);
	}          /* end of "if(temp_list.mon2 == temp_list.mon1)"    */

	else if((temp_list.mon2-temp_list.mon1) == 1)  /*different mon */
	{
	   if(is_cont_day())      /* day1 and day2 are continuous days */
	   {
	       normal_cont_day_charge();
	       return(TRUE);
	   }
	   else    /* calling time is too long */
	       return(FALSE);
	}

	else       /* not continuous month, so calling time is too long */
	    return(FALSE);
    }    /* end of "if(temp_list.year2 == temp_list.year1)"             */

    else      /* different year */
    {
	if(is_cont_day())    /* day1 and day2 are still continuous days */
	{
	    normal_cont_day_charge();
	    return(TRUE);
	}
	else
	    return(FALSE);
    }
}
