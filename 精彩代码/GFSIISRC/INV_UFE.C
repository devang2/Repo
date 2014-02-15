#include <graphics.h>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : print the "yong hu dian xin fei shou ju " for one user unit
 * called by : statistic()
 * date      : 1993.11.9
 */
void inv_one_usr(UC opflg3)
{
    UC   flag;
    UI   pnum=0;
    UL   input_no=0;
    PHONE_STRUCT phone = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};
    USERS_STRUCT user;

    flag = check_prn();      /* check printer is ready or not */
    if(flag == FALSE)        /* not ready */
	return;

    flag = input_user(&input_no, PRINT);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the user no does not exist */
    {
	message(UNO_NOT_EXIST);
        return;
    }

    load_phone_tbl();

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    search_user((UI)input_no, &pnum, &user);
    cal_usr_fee(pnum, &user, &phone);

    if(inv_one(&phone, user.user_name, DEPT_FEE, SINGLE, opflg3) == FALSE)
    {
        message_end();
        unload_phone_tbl();
        return;
    }

    unload_phone_tbl();
    message_end();

    return;
}


/* function  : print the "yong hu dian xin fei shou ju" for all user unit
 * called by : statistic()
 * date      : 1993.11.9
 */
void inv_all_usr(void)
{
    FILE *fpu;
    UC    flag;
    UI    pnum=0;
    USERS_STRUCT user;
    PHONE_STRUCT phone = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};

    flag = check_prn();      /* check printer is ready or not */
    if(flag == FALSE)        /* not ready */
	return;

    load_phone_tbl();

    fpu = fopen("gfsdata\\dept.dat","rb");
    if(fpu == NULL)
    {
	unload_phone_tbl();
	message(USER_FILE_ERR);
	return;
    }

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    pnum = 0;
    while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
    {
        clr_ufc(&phone, 0);

        cal_usr_fee(pnum, &user, &phone);
        if(inv_one(&phone, user.user_name, DEPT_FEE, MULTY, INV) == FALSE)
        {
            fclose(fpu);
            message_end();
            unload_phone_tbl();
            return;
        }
	pnum += user.phones;
    }
    fclose(fpu);
    unload_phone_tbl();
    message_end();
    return;
}


/* function  : print the "yong hu dian xin fei shou ju" from the user unit
 *             input to the end
 * date      : 1993.12.10
 */
void inv_part_usr(void)
{
    FILE *fpu;
    UC   flag;
    UI   pnum=0, lenth, i;
    UL   input_no=0;
    USERS_STRUCT user;
    PHONE_STRUCT phone = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};

    flag = check_prn();      /* check printer is ready or not */
    if(flag == FALSE)        /* not ready */
	return;

    flag = inp_phid_num(&input_no, &lenth, PRINT, DEPT_NO);
    if(flag == 1)
	return;

    load_phone_tbl();

    fpu = fopen("gfsdata\\dept.dat","rb");
    if(fpu == NULL)
    {
	unload_phone_tbl();
	message(USER_FILE_ERR);
	return;
    }

    message_disp(8,"正在打印，请稍候...");   /* printing, please wait... */

    pnum = 0;
    while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
    {
	if(user.user_no != (UI)input_no)
	    pnum += user.phones;
	else
	    break;
    }

    for(i = 0; i < lenth; i++)
    {
        clr_ufc(&phone, 0);
        cal_usr_fee(pnum, &user, &phone);
        if(inv_one(&phone, user.user_name, DEPT_FEE, MULTY, INV) == FALSE)
        {
            fclose(fpu);
            message_end();
            unload_phone_tbl();
            return;
        }

	pnum += user.phones;
        if(fread(&user, sizeof(USERS_STRUCT), 1, fpu) != 1) break;
    }

    fclose(fpu);
    unload_phone_tbl();
    message_end();

    return;
}
/* function  : print the "yong hu dian xin fei shou ju " for one user unit
 * called by : statistic()
 * date      : 1993.11.9
 */
void inq_one_usr(void)
{
    UC   flag;
    UI   pnum=0;
    UL   input_no=0;
    PHONE_STRUCT phone = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};
    USERS_STRUCT user;

    flag = input_user(&input_no, INQUIRE);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the user no does not exist */
    {
	message(UNO_NOT_EXIST);
        return;
    }

    load_phone_tbl();

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    search_user((UI)input_no, &pnum, &user);
    cal_usr_fee(pnum, &user, &phone);

    inq_part_fee(&phone, user.user_name, DEPT_FEE);

    unload_phone_tbl();
    message_end();

    return;
}



/* function  : print the "yong hu dian xin fei shou ju" from the user unit
 *             input to the end
 * date      : 1993.12.10
 */
void inq_part_usr(void)
{
    FILE *fpu;
    UC   flag;
    UI   pnum=0, lenth, i;
    UL   input_no=0;
    USERS_STRUCT user;
    PHONE_STRUCT phone = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};

    flag = inp_phid_num(&input_no, &lenth, INQUIRE, DEPT_NO);
    if(flag == 1)
	return;

    load_phone_tbl();

    fpu = fopen("gfsdata\\dept.dat","rb");
    if(fpu == NULL)
    {
	unload_phone_tbl();
	message(USER_FILE_ERR);
	return;
    }

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    pnum = 0;
    while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
    {
	if(user.user_no != (UI)input_no)
	    pnum += user.phones;
	else
	    break;
    }

    for(i = 0; i < lenth; i++)
    {
        clr_ufc(&phone, 0);

        cal_usr_fee(pnum, &user, &phone);
        if(inq_part_fee(&phone, user.user_name, DEPT_FEE) == FALSE) break;

	pnum += user.phones;
        if(fread(&user, sizeof(USERS_STRUCT), 1, fpu) != 1) break;
    }

    fclose(fpu);
    unload_phone_tbl();
    message_end();

    return;
}


/* function  : calculate the total fee of all kinds for a user unit
 * input     : pnum -- the phone number before this user unit in phones.ind
 *             user -- the user unit to be calculated
 * output    : phone -- total fee
 *             mon   -- total month lease
 * date      : 1993.11.9
 */
void cal_usr_fee(UI pnum, USERS_STRUCT *user, PHONE_STRUCT *phone)
{
    int    i;
    FILE  *fpp;
    UC     phone_no[10];
    UI     num=0;
    PHONE_STRUCT  *tmp;

    fpp = fopen("gfsdata\\phones.idx", "rb");
    fseek(fpp, pnum*sizeof(A_PHONE), SEEK_SET);
    while(num < user->phones)
    {
	fread(phone_no, sizeof(A_PHONE), 1, fpp);
	tmp = binary_search(phone_no);
	num++;
	if(tmp == NULL)
	    continue;

	phone->intern_time  += tmp->intern_time;
	phone->intern_charge+= tmp->intern_charge;
	phone->intern_count += tmp->intern_count;

	phone->nation_time  += tmp->nation_time;
	phone->nation_charge+= tmp->nation_charge;
	phone->nation_count += tmp->nation_count;

	phone->local_time   += tmp->local_time;
	phone->local_charge += tmp->local_charge;
	phone->local_count  += tmp->local_count;

	phone->month_lease  += tmp->month_lease;

	for(i=0; i<5; i++)
	    phone->addfee[i] += tmp->addfee[i];
    }
    fclose(fpp);
    return;
}

void clr_ufc(PHONE_STRUCT *phone, UC opflg)
{
     UI i;

     phone->intern_time   = 0;
     phone->intern_charge = 0;
     phone->intern_count  = 0;

     phone->nation_time   = 0;
     phone->nation_charge = 0;
     phone->nation_count  = 0;

     phone->local_time    = 0;
     phone->local_charge  = 0;
     phone->local_count   = 0;
     if(opflg != DELETE)
     {
         phone->month_lease   = 0;

         for(i=0; i<4; i++)
             phone->addfee[i] = 0;
     }
     phone->addfee[4] = 0;

     return;
}

/* function  : input the user unit no to be printed
 * called by : detail_one_user()
	       detail_part_user()
 * input     : no -- user unit no
 * output    : TRUE  -- the user unit no exists
 *             FALSE -- the user unit no does not exist
 *             2     -- does not input the number
 * date      : 1993.10.4
 */
UC input_user(UL *no, UC opflg)
{
    USERS_STRUCT *tbl;
    UC  result;
    UC  back_in=5;

    /* input the user  number */
    message_disp(8," 输入户头号   Enter 确认");    /*user no.*/
    pop_back(H_BX-40,H_BY-11,H_BX+240,H_BY+35,7); /* big frame */
    draw_back(H_BX+132,H_BY,H_BX+230,H_BY+25,11);
    if(opflg != INQUIRE)
        hz16_disp(H_BX-30,H_BY+5,"请输入欲打印户头号码",BLACK);   /* input user num */
    else
        hz16_disp(H_BX-30,H_BY+5,"请输入欲查询户头号码",BLACK);   /* input user num */

    result = get_dec(H_BX+134,H_BY+5,18, 60,back_in,4,no,0x00);

    message_end();
    rid_pop();

    if(!result)      /* IF USER TYPE "Esc", RETRUN. */
	return(2);

    load_user_tbl();
    UNewlen = user_tbl_len();
    if(UNewlen == 0)
    {
       unload_user_tbl();
       message(USER_FILE_ERR);
       return(2);
    }

    /*  judge whether the user unit number exists or not          */
    tbl = binary_user(*no);
    unload_user_tbl();
    if(tbl == NULL)          /* can not find the user unit number */
	return(FALSE);
    else
	return(TRUE);
}
