#include <dos.h>
#include <bios.h>

#include <bio.inc>

#include "feedef.h"

/* function  : print the charge rate of IDD, DDD & LDD
 * called by : set_data()
 * date      : 1993.9.20
 */
void prn_rate(void)
{
    UC  flag;

    flag = check_prn();
    if(flag == FALSE)
	return;

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    load_tbl(IDD);
    flag=print_rate(IDD);
    if(flag==FALSE)
    {
	 unload_tbl();
	 message_end();
	 return;
    }
    unload_tbl();

    load_tbl(DDD);
    flag=print_rate(DDD);
    if(flag==FALSE)
    {
	 unload_tbl();
	 message_end();
	 return;
    }
    unload_tbl();

    load_tbl(LDD);
    flag=print_rate(LDD);
    if(flag==FALSE)
    {
	 unload_tbl();
	 message_end();
	 return;
    }
    unload_tbl();

    message_end();
    return;
}

/* function  : print the charge rate
 * called by : prn_rate()
 * input     : mode -- IDD,DDD,LDD
 * date      : 1993.9.20
 */
UC print_rate(UC mode)
{
    UI num;
    RATE_STRUCT huge *tbl;

    tbl = Tbltop;

    if(print_ratrpt_head(mode) == FALSE) return(FALSE);

    num=0;
    while(tbl->length)
    {
        num++;
        if(print_ratrpt_detail(tbl, mode, num) == FALSE) return(FALSE);
	tbl++;
    }

    if(print_ratrpt_tail() == FALSE)
        return(FALSE);

    return(TRUE);
}

UC print_a_rattbl(RATE_STRUCT huge *tbl)
{
    UC flag;

    flag=prnf(" %-8s  ",tbl->area_no);
    if(flag==FALSE) return(FALSE);
    flag=prnf("%-9s  ",tbl->area_name);
    if(flag==FALSE) return(FALSE);
    flag=prnf("%6.2f    ",(double)(tbl->first_rate)/100);
    if(flag==FALSE) return(FALSE);
    flag=prnf("%3u       ",tbl->first_time);
    if(flag==FALSE) return(FALSE);
    flag=prnf("%6.2f    ",(double)(tbl->formal_rate)/100);
    if(flag==FALSE) return(FALSE);
    flag=prnf("%3u\n",tbl->formal_time);
    if(flag==FALSE) return(FALSE);

    return(TRUE);
}


UC print_ratrpt_head(UC isidd)
{
    UC *head[3] = {"       国际长话费率表",
	           "       国内长话费率表",
	           "       市内电话费率表"};

        if(isidd == IDD)
        {
            if(prnfd("\n%s\n\n", head[0]) == FALSE)
	        return(FALSE);
        }
        else if(isidd == DDD)
        {
            if(prnfd("\n%s\n\n", head[1]) == FALSE)
	        return(FALSE);
        }
        else if(isidd == LDD)
        {
            if(prnfd("\n%s\n\n", head[2]) == FALSE)
	        return(FALSE);
        }

        if(print_rattbl() == FALSE)
            return(FALSE);
        if(prnf(" 区号   地区       起始价  起始时长    正常价  正常时长\n") == FALSE)
            return(FALSE);
        if(print_rattbl() == FALSE)
            return(FALSE);
    return(TRUE);
}



UC print_ratrpt_tail(void)
{
    struct date now;

    getdate(&now);

    if(print_rattbl() == FALSE)
        return(FALSE);
    if(prnf(" 制表时间：%-4u年%2u月%2u日\n",now.da_year, now.da_mon, now.da_day )\
	 == FALSE)
        return(FALSE);

    if(new_page() == FALSE)
	return(FALSE);
    return(TRUE);
}

UC print_rattbl(void)
{
    if(prnf("━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n") == FALSE)
        return(FALSE);
    return(TRUE);
}

UC print_ratrpt_detail(RATE_STRUCT huge *tbl, UC isidd, UI num)
{
    if(num%Page_len !=0 )
    {
       if(print_a_rattbl(tbl) == FALSE)
           return(FALSE);
    }
    else
    {
       if(print_ratrpt_tail() == FALSE)
           return(FALSE);
       if(print_ratrpt_head(isidd) == FALSE)
           return(FALSE);
       if(print_a_rattbl(tbl) == FALSE)
           return(FALSE);
    }
    return(TRUE);
}


/* function  : check the printer whether it is ready or not
 * called by : this is general purposed routine
 * output    : 1 -- the printer is ready
 *             0 -- the printer is not ready
 * date      : 1993.9.20
 */
UC  check_prn(void)
{
    int status=0, i;
    UC  returnvalue;

    status = biosprint(2,0,PORT);      /* read the printer status */
    for( i =0; i<10; i++)
    {
        if((status!=0x90) && (status!=0x10)) /* the two types of printer are both 0x90 */
        {                                    /* when they are valid                    */
	    returnvalue = FALSE;
        }
        else
        {
	    returnvalue = TRUE;
            break;
        }
    }
    if(returnvalue == TRUE)
    {
        return(TRUE);
    }
    else
    {
       sound_alarm();
       message_win(8, "打印机出错或未开机", 5);
       return(FALSE);
    }
}
