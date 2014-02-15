#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : backup the old telephone records and delete them in *.dat
   called by : phone_rec_proc() (record.c)
   date      : 1993.11.5
 */

void backup_old_rec(void)
{
    UL    mon=0;
    UC    month[3];
    UC    suc, ret_val, flag, iflg1=0, dflg1=0, lflg1=0;
    UC    iflg2=0, dflg2=0, lflg2=0;

    flag = valid_pass(DEL_PWD);
    if(flag == 2)                 /* ESC */
        return;
    if(flag == FALSE)
    {
        message(PASS_ERR);
        return;
    }

    suc = input_month(BACKUP,&mon);         /* input the month for records */
    if(suc == FALSE)                        /* unsuccessful */
        return;
    ltoa(mon,month,10);

    message_disp(8,"正在备份,请稍候...");   /* backing up, please wait.... */

    if(Usr_typ == NORMAL_USR)
        iflg1=rcd_file_filter(IDD, month);
    else
        iflg2=rcd_file_filter(IDD, month);

    if(Usr_typ == NORMAL_USR)
        dflg1=rcd_file_filter(DDD, month);
    else
        dflg2=rcd_file_filter(DDD, month);

    if(Usr_typ == NORMAL_USR)
        lflg1=rcd_file_filter(LDD, month);
    else
        lflg2=rcd_file_filter(LDD, month);

    message_end();

    ret_val = select_1in2(1);
    if(ret_val == 1)
    {
        ret_val = select_1in2(2);
        if(ret_val == 2)
        {
            message_disp(8,"正在作删除操作, 请稍候...");  /* calculating, please wait... */
            if(iflg1 == 1)
            {
                fcopy("idd1.del","idd.dbf");        /* .del to .dat */
                fcopy("idd1.del","idd.tmp");        /* .del to .tmp */
            }

            if(iflg2 == 1)
            {
                fcopy("idd2.del","idd.hst");        /* .del to .dat */
            }

            if(dflg1 == 1)
            {
                fcopy("ddd1.del","ddd.dbf");        /* .del to .dat */
                fcopy("ddd1.del","ddd.tmp");        /* .del to .tmp */
            }

            if(dflg2 == 1)
            {
                fcopy("ddd2.del","ddd.hst");        /* .del to .dat */
            }

            if(lflg1 == 1)
            {
                fcopy("ldd1.del","ldd.dbf");        /* .del to .dat */
                fcopy("ldd1.del","ldd.tmp");        /* .del to .tmp */
            }

            if(lflg2 == 1)
            {
                fcopy("ldd2.del","ldd.hst");        /* .del to .dat */
            }
            message_end();
        }
    }

    if(iflg1 == 1)
        remove("idd1.del");
    if(dflg1 == 1)
        remove("ddd1.del");
    if(lflg1 == 1)
        remove("ldd1.del");
    if(iflg2 == 1)
        remove("idd2.del");
    if(dflg2 == 1)
        remove("ddd2.del");
    if(lflg2 == 1)
        remove("ldd2.del");

    return;
}


UC  rcd_file_filter(UC mode, UC month[3])
{
    UL mon=0;
    UC flag = 0;
    UC backed_file[15]="", back_file[15]="", dist_file[15]="";
    FILE *fps, *fpd, *fpback;
    struct date dd;

    getdate(&dd);
    mon = atol(month);
    if(mode == IDD)
    {
        strcpy(backed_file,"idd");
        strcpy(back_file,  "idd");
        strcpy(dist_file,  "idd");
    }
    else if(mode == DDD)
    {
        strcpy(backed_file,"ddd");
        strcpy(back_file,  "ddd");
        strcpy(dist_file,  "ddd");
    }
    else
    {
        strcpy(backed_file,"ldd");
        strcpy(back_file,  "ldd");
        strcpy(dist_file,  "ldd");
    }

    if(Usr_typ == NORMAL_USR)
        strcat(backed_file,".dbf");
    else
        strcat(backed_file,".hst");

    fps = fopen(backed_file,"rb");
    if(fps != NULL)
    {
        flag = 1;
        strcat(back_file,month);
        strcat(back_file,".dbf");

        if(Usr_typ == NORMAL_USR)
            strcat(dist_file,"1.del");            /* construct the file name */
        else
            strcat(dist_file,"2.del");            /* construct the file name */

        fpd    = fopen(dist_file,"wb");
        fpback = fopen(back_file,"wb");

        while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fps) == 1)
        {
            if( ( ( (UL)temp_list.mon2 == mon ) || \
                  ( (UL)temp_list.mon2 <  mon  && temp_list.year2 == dd.da_year ) || \
                  ( (UL)temp_list.mon2 >  mon  && temp_list.year2 == dd.da_year-1 ) ) )

                fwrite(&temp_list,sizeof(RECORD_STRUCT),1,fpback);
            else
                fwrite(&temp_list,sizeof(RECORD_STRUCT),1,fpd);
        }
        fclose(fpback);
        fclose(fpd);
        fclose(fps);
    }
    return flag;
}


/* function  : input the month for backup,inquire and delete the old
 *             telephone records
 * called by : backup_old_rec(),inquire_old_rec(),delete_old_rec()
 * input     : mode -- BACKUP,INQUIRE,DELETE
 * output    : TRUE  -- successful
 *             FALSE -- unsuccessful
 *             mon -- the month input
 * date      : 1993.11.5
 */
UC input_month(UC mode,UL *mon)
{
    UC   back_in=5, res=0;

    message_disp(8," 输入月份   Enter 确认");      /*month.*/
    pop_back(H_BX-40,H_BY-11,H_BX+240,H_BY+35,7); /* big frame */
    draw_back(H_BX+132,H_BY,H_BX+230,H_BY+25,11);

    if(mode == BACKUP)
        hz16_disp(H_BX-33,H_BY+5,"请输入欲备份话单月份",BLACK);
    if(mode == INQUIRE)
        hz16_disp(H_BX-33,H_BY+5,"请输入欲查询话单月份",BLACK);
    if(mode == DELETE)
        hz16_disp(H_BX-33,H_BY+5,"请输入欲删除话单月份",BLACK);
    do
    {
        res = get_dec(H_BX+138,H_BY+5,20,50,back_in,2,mon,0x00);
    }while(res && (*mon<1 || *mon>12));
    rid_pop();

    message_end();
    if(*mon>=1 && *mon<=12)
        return(TRUE);
    else
        return(FALSE);
}
