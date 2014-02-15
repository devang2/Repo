#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : inquire the telephone records of all phones
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void inquire_all_record(UC opflg)
{
    UC flag;

    TIME_STRUCT  in_stime={0, 0, 0, 0}, in_etime={0, 0, 0, 24};

    TABLE_STRUCT record_tbl = {0, 99, 20, 18, 16, 9,\
			       {44, 96, 70, 175, 44, 44, 44, 52, 60},\
			       GREEN};

    if(opflg == OUTRNG)
    {
	if( set_in_time(&in_stime, &in_etime) == 1)	     /* ESC */
	    return;
    }

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
	message(RATE_FILE_ERR);
	return;
    }

    flag  = load_authcd_tbl(0);
    if(flag != TRUE)
	Authflg = OFF;
    else
	Authflg = ON;

    clr_DialWin(2);

    draw_table(&record_tbl);

    disp_record_head();
    message_disp(8,"按任意键继续");        /* press any key */

    hz16_disp(240,80,"国 际 长 途 话 单",BLACK);  /* International long call */

    if(disp_a_kind_rcd(IDD, opflg, in_stime, in_etime)==FALSE)
    {
	recover_screen(2);
	message_end();
	unload_rate_tab();
	if(Authflg == ON)
	    unload_authcd_tbl();
	return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"国 内 长 途 话 单",BLACK);  /* national long call */

    if(disp_a_kind_rcd(DDD, opflg, in_stime, in_etime)==FALSE)
    {
	recover_screen(2);
	message_end();
	unload_rate_tab();
	if(Authflg == ON)
	    unload_authcd_tbl();
	return;
    }

    clr_scr(240,80,440,97,0,7);
    hz16_disp(240,80,"市   话   话   单",BLACK);   /* local long call */

    disp_a_kind_rcd(LDD, opflg, in_stime, in_etime);

    recover_screen(2);
    message_end();
    unload_rate_tab();
    if(Authflg == ON)
	unload_authcd_tbl();
    return;
}


UC  disp_a_kind_rcd(UC mode, UC opflg, TIME_STRUCT in_stime, TIME_STRUCT  in_etime)
{
    FILE *fp;
    UC	 position=0 , auth_name[9];
    UI	 key, num=0, i;
    char *a_name="    ";
    RATE_STRUCT huge *rate_tmp;

    for(i=0; i<2; i++)
    {
        if(i == 0)
        {
            if(mode == IDD)
	        fp = fopen("idd.dbf","rb");
            else if(mode == DDD)
 	        fp = fopen("ddd.dbf","rb");
            else
 	        fp = fopen("ldd.dbf","rb");
        }
	else
        {
            if(mode == IDD)
	        fp = fopen("idd.hst","rb");
            else if(mode == DDD)
 	        fp = fopen("ddd.hst","rb");
            else
 	        fp = fopen("ldd.hst","rb");
        }

	if(fp != NULL)
	{
	    while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
	    {
		if(opflg == NORMAL)
		{
		    if(!is_in_range())	       /* not in the sorting time range */
		    {
			continue;
		    }
		}
		else if(opflg == NONFEE)
		{
		    if( (temp_list.charge-temp_list.add) != 0 )
			continue;
		}
		else if(opflg == OUTRNG)
		{
		    if(in_time_range(in_stime,in_etime) )
			continue;
		}

                if(mode == IDD)
                {
		    rate_tmp = get_rate(&temp_list.callee[2],IDD);	 /* get charge rate */
                }
                else if(mode == DDD)
                {
		    rate_tmp = get_rate(&temp_list.callee[1],DDD);	 /* get charge rate */
                }
                else
                {
		    rate_tmp = get_rate(temp_list.callee,LDD);	 /* get charge rate */
                }

	        if(rate_tmp == NULL)		  /* the area No. does not exist    */
		    strcpy(rate1.area_name, a_name);
		else
		    rate1 = *rate_tmp;

		num++;

		auth_usr_proc(auth_name);

		disp_a_record(auth_name, position,num);
		position++;
		if( (position>=16) && (!EndofRecFile(fp)) )
		{
		    key = get_key1();
		    if(key == ESC)
		    {
			fclose(fp);
			return FALSE;
		    }
		    else
		    {
			brush_tbl(9, 16, 7);
			position = 0;
		    }
		}
	    }	       /* end of "while(fread(...fp)==1)" */
	    fclose(fp);
	}
    }	      /* end of "if(fp != NULL)"        */

    key = get_key1();

    if(mode != LDD)
    {
        if(key == ESC)
        {
	    return FALSE;
        }
        else
        {
	    brush_tbl(9, 16, 7);
	    position = 0;
	    num = 0;
        }
    }
    return TRUE;
}

