#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : print the telephones belonging to the user unit input
 * called by : set_data()
 * date      : 1993.11.10
 */
void print_user_unit(void)
{
    FILE *fpu,*fpp;
    UC   flag, phone_no[10];
    UC   *head="          户头分机关系一览表";
    UI   num=0;
    USERS_STRUCT user;
    struct date now;

    flag = check_prn();      /* check printer is ready or not */
    if(flag == FALSE)        /* not ready */
	return;

    message_disp(8,"正在打印，请稍候...");   /* printing */

        if(prnfd("\n%s\n", head)  == FALSE)
        {
            message_end();
            return;
        }

    fpu = fopen("gfsdata\\dept.dat","rb");
    if(fpu == NULL)
    {
        message_end();
	message(USER_FILE_ERR);
	return;
    }

    fpp = fopen("gfsdata\\phones.idx","rb");
    if(fpp != NULL)
    {
	while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
	{
            if(print_usrtbl() == FALSE)
            {
                message_end();
                fclose(fpu);
                fclose(fpp);
	        return;
            }
            if(prnf("户头名称: %-25s     户头号码: %-5u    分机数量: %-3u\n",user.user_name, user.user_no, user.phones)  == FALSE)
            {
                message_end();
                fclose(fpu);
                fclose(fpp);
	        return;
            }

            if(prnf("\n分机号码: ")  == FALSE)
            {
                message_end();
                fclose(fpu);
                fclose(fpp);
	        return;
            }

	    num = 0;
	    while(num < user.phones)
	    {
		fread(phone_no,sizeof(A_PHONE),1,fpp);
                if(prnf("%-8s", phone_no)  == FALSE)
                {
                    message_end();
                    fclose(fpu);
                    fclose(fpp);
	            return;
                }
		num++;
		if(num%8 == 0)
                {
                    if(prnf("\n          ")  == FALSE)
                    {
                        message_end();
                        fclose(fpu);
                        fclose(fpp);
	                return;
                    }
                }
	    }
	}
        if(print_usrtbl() == FALSE)
        {
	    message_end();
            fclose(fpu);
            fclose(fpp);
	    return;
        }
        getdate(&now);

        if(prnf("制表时间：%-4u年%2u月%2u日\n",\
		   now.da_year, now.da_mon, now.da_day )\
		   == FALSE)
        {
	    message_end();
            fclose(fpu);
            fclose(fpp);
	    return;
        }
        /* new page */
        if(new_page() == FALSE)
        {
            message_end();
            fclose(fpu);
            fclose(fpp);
	    return;
        }
	fclose(fpp);
    }
    fclose(fpu);
    message_end();
    return;
}

UC print_usrtbl(void)
{
    if(prnf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n")  == FALSE)
        return(FALSE);
    return(TRUE);
}
