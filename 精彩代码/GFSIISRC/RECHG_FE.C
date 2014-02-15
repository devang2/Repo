#include <string.h>
#include <stdlib.h>
#include <mem.h>
#include <alloc.h>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

void recharge_all_record(void)
{
    UC   flag;
    UC   mode;

    flag  = load_rate_tab();
    if(flag != TRUE)
    {
        message(RATE_FILE_ERR);
        return;
    }

    for(mode=0; mode<2; mode++)
        if(Cashflg[mode])
        {
            flag = load_cash_tbl(mode);
            if(flag != 1)             /* cash.dat does not exist */
                Cashflg[mode] = 0;
            else
                Cash_len[mode] = cash_tbl_len(mode);
        }

    load_special_tbl(0);
    load_special_tbl(1);

    message_disp(8,"正在为缺价话单重新计价, 请稍候...");        /* press any key */

    recharge_a_kind_rcd(IDD);
    recharge_a_kind_rcd(DDD);
    recharge_a_kind_rcd(LDD);

    message_end();

    for(mode=0; mode<2; mode++)
        if(Cashflg[mode])
        {
            save_cash_tbl(mode);
            unload_cash_tbl(mode);
        }
    unload_special_tbl(0);
    unload_special_tbl(1);

    unload_rate_tab();
    return;
}


void recharge_a_kind_rcd(UC mode)
{
    FILE *fps, *fpd;
    UI   SpecLen[2];
    UC   f1, f2;
    UC   user[10];
    PHONE_STRUCT *phone;

    SpecLen[0] = special_tbl_len(0);
    SpecLen[1] = special_tbl_len(1);

    if(mode == IDD)
        fps = fopen("idd.dbf","rb");
    else if(mode == DDD)
        fps = fopen("ddd.dbf","rb");
    else
        fps = fopen("ldd.dbf","rb");
    if(fps != NULL)
    {
        if(mode == IDD)
            fpd = fopen("idd.rch","wb");
        else if(mode == DDD)
            fpd = fopen("ddd.rch","wb");
        else
            fpd = fopen("ldd.rch","wb");

        while(fread(&temp_list,sizeof(RECORD_STRUCT),1,fps) == 1)
        {
            if( (temp_list.charge-temp_list.add) != 0 )
                fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd);
            else
            {
                if(mode == IDD)
                    Intern_charge();
                else if(mode == DDD)
                    Nation_charge();
                else
                    Local_charge();

                fwrite(&temp_list, sizeof(RECORD_STRUCT), 1, fpd);

                if(temp_list.auth_code != 0xaaaaaa)
                    update_cash(AUTH_CASH);
                else
                {
                    ltoa(temp_list.caller2, user, 10);   /* int to ASCII string */
                    phone = binary_search(user);         /* locate the phone No */
                    if( phone != NULL)
		    {
		        if(  (mode == IDD && phone->feeflag[4])  ||
                             (mode == DDD && phone->feeflag[3]) )
                            update_cash(PHONE_CASH);
                        else 	/* ldd */
                        {
                            f1 = IsSpecialCall(temp_list.callee,SpecLen[0],0);
                            f2 = IsSpecialCall(temp_list.callee,SpecLen[1],1);

                            if(   (phone->feeflag[1] && f1)
                                ||(phone->feeflag[2] && f2)
                                ||(phone->feeflag[0] && !(f1 && f2) ))
 		                update_cash(PHONE_CASH);
                        }
                    }
                }
            }
        }          /* end of "while(fread(...fp)==1)" */
        fclose(fps);
        fclose(fpd);

        if(mode == IDD)
        {
            fcopy("idd.rch","idd.dbf");        /* .rch to .dat */
            fcopy("idd.rch","idd.tmp");        /* .rch to .tmp */
            remove("idd.rch");
        }
        else if(mode == DDD)
        {
            fcopy("ddd.rch","ddd.dbf");        /* .rch to .dat */
            fcopy("ddd.rch","ddd.tmp");        /* .rch to .tmp */
            remove("ddd.rch");
        }
        else
        {
            fcopy("ldd.rch","ldd.dbf");        /* .rch to .dat */
            fcopy("ldd.rch","ldd.tmp");        /* .rch to .tmp */
            remove("ldd.rch");
        }
    }
}

