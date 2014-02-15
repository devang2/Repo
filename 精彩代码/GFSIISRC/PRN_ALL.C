#include <string.h>
#include <stdlib.h>
#include <bios.h>
#include <bio.inc>

#include "feedef.h"

/* function  : print all the telephone records
 * called by : phone_record_proc()
 * date      : 1993.10.5
 */
void print_all(UC opflg)
{
    UC	 flag;
    TIME_STRUCT  in_stime={0, 0, 0, 0}, in_etime={0, 0, 0, 24};

    flag = check_prn();
    if(flag == FALSE)	     /* not ready */
	return;

    if(opflg == OUTRNG)
    {
	if(set_in_time(&in_stime, &in_etime) == 1)	    /* ESC */
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

    message_disp(8,"正在打印，请稍候...");   /* printing */

    if(prn_a_kind_rcd(IDD, opflg, in_stime, in_etime) == FALSE)
    {
	message_end();
	unload_rate_tab();
	if(Authflg == ON)
	    unload_authcd_tbl();
	return;
    }

    if(prn_a_kind_rcd(DDD, opflg, in_stime, in_etime) == FALSE)
    {
	message_end();
	unload_rate_tab();
	if(Authflg == ON)
	    unload_authcd_tbl();
	return;
    }

    prn_a_kind_rcd(LDD, opflg, in_stime, in_etime);
    message_end();
    unload_rate_tab();
    if(Authflg == ON)
        unload_authcd_tbl();
    return;
}



UC  prn_a_kind_rcd(UC mode, UC opflg, TIME_STRUCT in_stime, TIME_STRUCT  in_etime)
{
    FILE *fp;
    UI	 num=0, i;
    UC	 pflg=FALSE;
    char *a_name="不明";
    char auth_name[9];
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
	    pflg=TRUE;
	    if(i == 0)
	    {
		if(print_rcdrpt_head(mode)==FALSE)
		{
		    fclose(fp);
		    return FALSE;
		}
	    }

	    while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fp) == 1)
	    {
		if(opflg == NORMAL)
		{
		    if(!is_in_range())	       /* not in the sorting time range */
			continue;
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

		if(print_rcdrpt_detail(auth_name, num, mode)==FALSE)
		{
		    fclose(fp);
		    return FALSE;
		}
	    }	       /* end of "while(!feof(fp))" */
	    fclose(fp);
	}
    }	      /* end of "if(fp != NULL)"        */

    if(pflg == TRUE)
	if(print_rcdrpt_tail()==FALSE)
	    return FALSE;

    return  TRUE;
}
