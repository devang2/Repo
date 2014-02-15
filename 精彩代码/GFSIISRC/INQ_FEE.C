#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <string.h>

#include <bio.inc>
#include <key.inc>
#include <menu.inc>
#include "feedef.h"

/* function  :
 * called by :
 * date      : 1993.12.11
 */
void inq_fee_all(void)
{
    UI   i, key;
    UI   position=0;
    FILE          *fp;
    PHONE_STRUCT  *phone;
    TABLE_STRUCT   phnfee_tbl = { 3, 97, 40, 18, 16, 10,
                                {40, 72, 64, 76, 44, 64, 64, 60, 64, 76},
				GREEN
			       };

    load_phone_tbl();

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
        unload_phone_tbl();
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_inqfee_head(PHONE_FEE);
    draw_table(&phnfee_tbl);

    Newlen = phone_tbl_len();
    phone  = Phone_top;

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    for(i=0; i<Newlen; i++)
    {
	disp_a_phnfee(phone, position, i);
	position++;
	if( position >= 16 )
	{
            while(1)
            {
	        key = get_key1();
	        if(key == ESC)
	        {
	            fclose(fp);
                    unload_phone_tbl();
	            recover_screen(2);
	            message_end();
	            return;
	        }
	        else if(key == ENTER)
	        {
	            brush_tbl(10, 16, 7);
	            position = 0;
                    break;
	        }
            }
	}
	phone++;
    }

    key = get_key1();
    unload_phone_tbl();
    recover_screen(2);
    message_end();
    return;
}

/* function  :
 * called by :
 * date      : 1993.12.11
 */
void inq_afee_all(void)
{
    UI   i, key;
    UC   flag;
    UI   position=0;
    FILE          *fp;
    AUTHCD_STRUCT *authcd;
    TABLE_STRUCT ausfee_tbl = { 0, 97, 40, 18, 16, 10,
                                {42, 106, 66, 42, 66, 42, 66, 58, 66, 75},
				GREEN
			       };

    flag = load_authcd_tbl(0);
    if(flag == 0)                 /* charge.aus does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return;
    }

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
        unload_authcd_tbl();
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_inqfee_head(AUTH_FEE);
    draw_table(&ausfee_tbl);

    ANewlen = authcd_tbl_len();
    authcd  = Auth_cd_top;

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    for(i=0; i<ANewlen; i++)
    {
	disp_a_ausfee(authcd, position, i);

	position++;
	if( position >= 16 )
	{
            while(1)
            {
	        key = get_key1();
	        if(key == ESC)
	        {
                    unload_authcd_tbl();
	            recover_screen(2);
	            message_end();
	            return;
	        }
	        else if(key == ENTER)
	        {
	            brush_tbl(10, 16, 7);
	            position = 0;
                    break;
	        }
            }
	}
	authcd++;
    }

    key = get_key1();
    unload_authcd_tbl();
    recover_screen(2);
    message_end();
    return;
}



/* function  :
 * called by :
 * date      : 1993.12.11
 */
void inq_ufee_all(void)
{
    UI   position=0;
    FILE *fp, *fpu;
    UI   pnum=0, i, key;
    USERS_STRUCT user;
    PHONE_STRUCT phone = {"",0, 0,0,{0,0,0,0,0},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};
    TABLE_STRUCT usrfee_tbl = { 0, 97, 40, 18, 16, 11,
                                {38, 71, 39, 63, 71, 39, 63, 55, 55, 63, 71},
				GREEN
			       };

    load_phone_tbl();

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
        unload_phone_tbl();
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_inqfee_head(DEPT_FEE);
    draw_table(&usrfee_tbl);

    fpu = fopen("gfsdata\\dept.dat","rb");
    if(fpu == NULL)
    {
	unload_phone_tbl();
	message(USER_FILE_ERR);
        recover_screen(2);
	return;
    }

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    pnum = 0;
    i = 0;
    while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
    {
        clr_ufc(&phone, 0);
        cal_usr_fee(pnum, &user, &phone);

	disp_a_usrfee(&phone, user, position, i);

	position++;
	if( position >= 16 )
	{
            while(1)
            {
	        key = get_key1();
	        if(key == ESC)
	        {
	            fclose(fpu);
                    unload_phone_tbl();
	            recover_screen(2);
	            message_end();
	            return;
	        }
	        else if(key == ENTER)
	        {
	            brush_tbl(10, 16, 7);
	            position = 0;
                    break;
	        }
            }
	}

        i++;
	pnum += user.phones;
    }

    key = get_key1();
    fclose(fpu);
    unload_phone_tbl();
    message_end();
    recover_screen(2);
    return;
}

/* function  :
 * called by :
 * date      : 1993.12.11
 */
void inq_fee_part(void)
{
    UI   i, idx, key;
    UC   flag;
    UI   position=0;
    UI   lenth;
    UC   phoneno[10];
    UL   phone_no;
    FILE          *fp;
    PHONE_STRUCT  *phone;
    TABLE_STRUCT phnfee_tbl = { 3, 97, 40, 18, 16, 10,
                                {40, 72, 64, 76, 44, 64, 64, 60, 64, 76},
				GREEN
			       };

    flag = inp_phid_num(&phone_no, &lenth, INQUIRE, PHONE_NO);
    if(flag == 1)
	return;

    load_phone_tbl();

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
        unload_phone_tbl();
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_inqfee_head(PHONE_FEE);
    draw_table(&phnfee_tbl);

    i = 0;
    Newlen = phone_tbl_len();
    phone  = Phone_top;

    ltoa(phone_no,phoneno,10);
    while(strcmp(phone->phone_no,phoneno) != 0)
    {
	i++;
	phone++;
    }

    idx = i;
    if( (idx+lenth) >= Newlen )
	 idx = Newlen;
    else
	 idx += lenth;

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    for(; i<idx; i++)
    {
	disp_a_phnfee(phone, position, i);

	position++;
	if( position >= 16 || (i == (idx-1) ) )
	{
            while(1)
            {
	        key = get_key1();
	        if(key == ESC)
	        {
	            fclose(fp);
                    unload_phone_tbl();
	            recover_screen(2);
	            message_end();
	            return;
	        }
	        else if(key == ENTER)
	        {
	            brush_tbl(10, 16, 7);
	            position = 0;
                    break;
	        }
            }
	}
	phone++;
    }

    unload_phone_tbl();
    recover_screen(2);
    message_end();
    return;
}

/* function  :
 * called by :
 * date      : 1993.12.11
 */
void inq_afee_part(void)
{
    UI   i, idx, key;
    UC   flag;
    UI   position=0;
    UI   lenth;
    UL   auth_cd;
    FILE          *fp;
    AUTHCD_STRUCT *authcd;
    TABLE_STRUCT ausfee_tbl = { 0, 97, 40, 18, 16, 10,
                                {42, 106, 66, 42, 66, 42, 66, 58, 66, 75},
				GREEN
			       };

    flag = inp_phid_num(&auth_cd, &lenth, INQUIRE, AUTH_CD);
    if(flag == 1)
	return;

    flag = load_authcd_tbl(0);
    if(flag == FALSE)                 /* charge.pho does not exist */
    {
       message(AUTHCD_FILE_ERR);
       return;
    }

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
        unload_authcd_tbl();
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_inqfee_head(AUTH_FEE);
    draw_table(&ausfee_tbl);

    i = 0;
    ANewlen = authcd_tbl_len();
    authcd  = Auth_cd_top;

    while(authcd->auth_code != auth_cd)
    {
	i++;
	authcd++;
    }

    idx = i;
    if( (idx+lenth) >= ANewlen )
	 idx = ANewlen;
    else
	 idx += lenth;

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */

    for(; i<idx; i++)
    {
	disp_a_ausfee(authcd, position, i);

	position++;
	if( position >= 16 || (i == (idx-1) ) )
	{
            while(1)
            {
	        key = get_key1();
	        if(key == ESC)
	        {
	            fclose(fp);
                    unload_authcd_tbl();
	            recover_screen(2);
	            message_end();
	            return;
	        }
	        else if(key == ENTER)
	        {
	            brush_tbl(10, 16, 7);
	            position = 0;
                    break;
	        }
            }
	}
	authcd++;
    }

    fclose(fp);
    unload_authcd_tbl();
    recover_screen(2);
    message_end();
    return;
}



/* function  :
 * called by :
 * date      : 1993.12.11
 */
void inq_ufee_part(void)
{
    UI   position=0;
    FILE *fp, *fpu;
    UC   flag;
    UI   pnum=0, lenth, i, key;
    UL   input_no=0;
    USERS_STRUCT user;
    PHONE_STRUCT phone = {"",0,0,0,{1,1,1,1,1},0,0,0,0,0,0,0,0,0,0,0,0,0,{0,0,0,0,0}};

    TABLE_STRUCT usrfee_tbl = { 0, 97, 40, 18, 16, 11,
                                {38, 71, 39, 63, 71, 39, 63, 55, 55, 63, 71},
				GREEN
			       };

    flag = inp_phid_num(&input_no, &lenth, INQUIRE, DEPT_NO);
    if(flag == 1)
	return;

    load_phone_tbl();

    fp = fopen(TotTimeFileName,"rb");
    if(fp == NULL)
    {
	message(TFEE_FILE_ERR);
        unload_phone_tbl();
	return;
    }
    fread(&Ps_time, sizeof(TIME_STRUCT), 1, fp);
    fread(&Pe_time, sizeof(TIME_STRUCT), 1, fp);
    fclose(fp);

    clr_DialWin(2);
    disp_inqfee_head(DEPT_FEE);
    draw_table(&usrfee_tbl);

    fpu = fopen("gfsdata\\dept.dat","rb");
    if(fpu == NULL)
    {
	unload_phone_tbl();
	message(USER_FILE_ERR);
        recover_screen(2);
	return;
    }

    pnum = 0;
    while(fread(&user,sizeof(USERS_STRUCT),1,fpu) == 1)
    {
	if(user.user_no != (UI)input_no)
	    pnum += user.phones;
	else
	    break;
    }

    message_disp(8,"Enter 继续    Esc 终止");        /* press any key */


    for(i = 0; i < lenth; i++)
    {
        clr_ufc(&phone, 0);
        cal_usr_fee(pnum, &user, &phone);

	disp_a_usrfee(&phone, user, position, i);

	position++;
	if( position >= 16 )
	{
            while(1)
            {
	        key = get_key1();
	        if(key == ESC)
	        {
	            fclose(fpu);
                    unload_phone_tbl();
	            recover_screen(2);
	            message_end();
	            return;
	        }
	        else if(key == ENTER)
	        {
	            brush_tbl(10, 16, 7);
	            position = 0;
                    break;
	        }
            }
	}
	pnum += user.phones;
        if(fread(&user, sizeof(USERS_STRUCT), 1, fpu) != 1) break;
    }
    get_key1();

    fclose(fpu);
    unload_phone_tbl();
    recover_screen(2);
    message_end();
    return;
}

/* function  : display head for inquring record table
 * called by :
 * date      : 1993.10.5
 */
void disp_inqfee_head(UC opflg)
{
    if(opflg == PHONE_FEE)
    {
        outf(0, 75, 7, 0, "                      分  机  话  费  摘  要  报  告");

        outf(67,  102, 7,0, "分机");
        outf(136, 102, 7,0, "市话");
        outf(207, 102, 7,0, "长话");
        outf(268, 102, 7,0, "长话");
        outf(323, 102, 7,0, "附加");
        outf(388, 102, 7,0, "月租");
        outf(453, 102, 7,0, "维修");

        outf(10 , 120, 7,0, "序号");
        outf(67 , 120, 7,0, "号码");
        outf(136, 120, 7,0, "话费");
        outf(207, 120, 7,0, "话费");
        outf(268, 120, 7,0, "次数");
        outf(331, 120, 7,0, "费");
        outf(396, 120, 7,0, "费");
        outf(461, 120, 7,0, "费");
        outf(512, 120, 7,0, "杂费");
        outf(568, 120, 7,0, "费用合计");
    }
    else if(opflg == DEPT_FEE)
    {
        outf(0, 75, 7, 0, "                      分  户  话  费  摘  要  报  告");

        outf(12,  102, 7,0, "序");
        outf(114, 102, 7,0, "分机");
        outf(156+12, 102, 7,0, "市话");
        outf(220+12, 102, 7,0, "长话");
        outf(292, 102, 7,0, "长话");
        outf(332+12, 102, 7,0, "附加");
        outf(396+8, 102, 7,0, "月租");
        outf(452+8, 102, 7,0, "维修");

        outf(12,  120, 7,0, "号");
        outf(52 , 120, 7,0, "户头名");
        outf(116, 120, 7,0, "数量");
        outf(156+12, 120, 7,0, "话费");
        outf(220+12, 120, 7,0, "话费");
        outf(292, 120, 7,0, "次数");
        outf(340+12, 120, 7,0, "费");
        outf(404+8, 120, 7,0, "费");
        outf(460+8, 120, 7,0, "费");
        outf(508+12, 120, 7,0, "杂费");
        outf(572, 120, 7,0, "费用合计");
    }
    else
    {
        outf(0, 75, 7, 0, "                      授 权 用 户 话 费 摘 要 报 告");

        outf(65,  102, 7,0, "授权用户");
        outf(168, 102, 7,0, "市话");
        outf(223, 102, 7,0, "市话");
        outf(278, 102, 7,0, "长话");
        outf(333, 102, 7,0, "长话");
        outf(388, 102, 7,0, "附加");
        outf(451, 102, 7,0, "月租");
        outf(514, 102, 7,0, "服务");

        outf(6 ,  120, 7,0, "序号");
        outf(81 , 120, 7,0, "名称");
        outf(168, 120, 7,0, "话费");
        outf(223, 120, 7,0, "次数");
        outf(278, 120, 7,0, "话费");
        outf(333, 120, 7,0, "次数");
        outf(396, 120, 7,0, "费");
        outf(459, 120, 7,0, "费");
        outf(522, 120, 7,0, "费");
        outf(570, 120, 7,0, "费用合计");
    }
    return;
}

/* function  :
 * called by :
 * input     : pos -- display position in the table
 *             num -- sequence number
 * date      : 1993.10.5
 */
void disp_a_usrfee(PHONE_STRUCT *phone, USERS_STRUCT usr, UC pos, UI num)
{
    int    i, j;
    UI     x, y;
    double total_f;
    UNIT_STRUCT  cur_unit;
    UC usr_nam_buf[9];

    for(i=0; i<8; i++)
	usr_nam_buf[i] = usr.user_name[i];
    usr_nam_buf[i] = '\0';
    total_f = (double)(phone->local_charge)/100+\
	      ((double)(phone->nation_charge)+(double)(phone->intern_charge))/100+\
	      (double)(phone->addfee[4])/100+\
	      (double)(phone->month_lease)/100+\
	      (double)(phone->addfee[2])/100+\
	      ((double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3]))/100;

    cur_unit.unit_x = pos;
    for(j=0;j<11;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+3;
	y = cur_unit.dot_sy+1;
	switch(j)
	{
	    case 0:          /* sequence number */
		outf(x,y,7,0,"%-4u",num);
		break;
	    case 1:
		outf(x,y,7,0,"%-s",usr_nam_buf);
		break;
	    case 2:
		outf(x,y,7,0,"%-2u",usr.phones);
		break;
	    case 3:
		outf(x,y,7,0,"%7.2f",(double)(phone->local_charge)/100);
		break;
	    case 4:
                outf(x,y,7,0,"%8.2f",(double)(phone->nation_charge+phone->intern_charge)/100);
		break;
	    case 5:
		outf(x,y,7,0,"%4u", (phone->nation_count +phone->intern_count));
		break;
	    case 6:
		outf(x,y,7,0,"%7.2f",(double)(phone->addfee[4])/100);
		break;
	    case 7:
		outf(x, y, 7, 0,"%6.2f",((double)phone->month_lease)/100);
		break;
	    case 8:
		 outf(x,y,7,0,"%6.2f", (double)(phone->addfee[2])/100);
		 break;
	    case 9:
		outf(x,y,7,0,"%7.2f", (double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3])/100);
		break;
	    case 10:
		outf(x,y,7,0,"%8.2f",total_f);
		break;
	}     /* end of "switch(j)"        */
    }         /* end of "for(j=0;j<8;j++)" */

    return;
}


/* function  :
 * called by :
 * input     : pos -- display position in the table
 *             num -- sequence number
 * date      : 1993.10.5
 */
void disp_a_phnfee(PHONE_STRUCT *phone, UC pos, UI num)
{
    int    j;
    UI     x, y;
    double total_f;
    UNIT_STRUCT  cur_unit;


    total_f = (double)(phone->local_charge)/100+\
	      ((double)(phone->nation_charge)+(double)(phone->intern_charge))/100+\
	      (double)(phone->addfee[4])/100+\
	      (double)(phone->month_lease)/100+\
	      (double)(phone->addfee[2])/100+\
	      ((double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3]))/100;

    cur_unit.unit_x = pos;
    for(j=0;j<10;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+4;
	y = cur_unit.dot_sy+1;
	switch(j)
	{
	    case 0:          /* sequence number */
		outf(x,y,7,0,"%-4u",num);
		break;
	    case 1:
		outf(x,y,7,0,"%-s",phone->phone_no);
		break;
	    case 2:
		outf(x,y,7,0,"%7.2f",(double)(phone->local_charge)/100);
		break;
	    case 3:
                outf(x+2,y,7,0,"%8.2f",(double)(phone->nation_charge+phone->intern_charge)/100);
		break;
	    case 4:
		outf(x+2,y,7,0,"%4u", (phone->nation_count +phone->intern_count));
		break;
	    case 5:
		outf(x+2,y,7,0,"%7.2f",(double)(phone->addfee[4])/100);
		break;
	    case 6:
		outf(x+2, y, 7, 0,"%6.2f",((double)phone->month_lease)/100);
		break;
	    case 7:
		 outf(x+2,y,7,0,"%6.2f", (double)(phone->addfee[2])/100);
		 break;
	    case 8:
		outf(x,y,7,0,"%7.2f", (double)(phone->addfee[0]+phone->addfee[1]+phone->addfee[3])/100);
		break;
	    case 9:
		outf(x-3,y,7,0,"%9.2f",total_f);
		break;
	}     /* end of "switch(j)"        */
    }         /* end of "for(j=0;j<8;j++)" */

    return;
}

/* function  :
 * called by :
 * input     : pos -- display position in the table
 *             num -- sequence number
 * date      : 1993.10.5
 */
void disp_a_ausfee(AUTHCD_STRUCT *authcd, UC pos, UI num)
{
    int    i, j;
    UI     x, y;
    double total_f, sfee;
    UNIT_STRUCT  cur_unit;
    UC asr_nam_buf[13];


    for(i=0; i<10; i++)
	asr_nam_buf[i] = authcd->auth_usr_nam[i];
    asr_nam_buf[i] = '\0';

    sfee = 0;

    total_f = (double)(authcd->local_charge)/100+\
	      ((double)(authcd->nation_charge)+(double)(authcd->intern_charge))/100+\
	      (double)(authcd->addfee)/100+\
	      (double)(authcd->month_lease)/100;

    cur_unit.unit_x = pos;
    for(j=0;j<10;j++)
    {
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+4;
	y = cur_unit.dot_sy+1;
	switch(j)
	{
	    case 0:          /* sequence number */
		outf(x,y,7,0,"%-4u",num);
		break;
	    case 1:
		outf(x,y,7,0,"%-s",asr_nam_buf);
		break;
	    case 2:
		outf(x,y,7,0,"%7.2f",(double)(authcd->local_charge)/100);
		break;
	    case 3:
		outf(x,y,7,0,"%4u", authcd->local_count);
		break;
	    case 4:
                outf(x,y,7,0,"%7.2f",(double)(authcd->nation_charge+authcd->intern_charge)/100);
		break;
	    case 5:
		outf(x,y,7,0,"%4u", (authcd->nation_count +authcd->intern_count));
		break;
	    case 6:
		outf(x,y,7,0,"%7.2f",(double)(authcd->addfee)/100);
		break;
	    case 7:
		outf(x, y, 7, 0,"%6.2f",((double)authcd->month_lease)/100);
		break;
	    case 8:
		outf(x,y,7,0,"%7.2f", sfee);
		break;
	    case 9:
		outf(x,y,7,0,"%8.2f",total_f);
		break;
	}     /* end of "switch(j)"        */
    }         /* end of "for(j=0;j<8;j++)" */

    return;
}


/* function  : brushr the table item
 * called by :
 * date      : 1994.10.5
 */
void brush_tbl(UC xnum, UC ynum, UC bk_colr)
{
    UI i;

    for(i=0; i<(UI)ynum; i++)
        brush_one_row(i, xnum, bk_colr);
    return;
}


/* function  : clear one row the rate table
 * date      : 1993.11.19
 */
void brush_one_row(UC row, UC xnum, UC bk_colr)
{
    UI j;
    UI xs, ys, xe, ye;
    UNIT_STRUCT cur_unit;

    cur_unit.unit_x = row;
    for(j = 0; j < (UI)xnum; j++)
    {
         cur_unit.unit_y = j;
         get_certain(&cur_unit);
         xs = cur_unit.dot_sx;
         ys = cur_unit.dot_sy;
         xe = cur_unit.dot_ex;
         ye = cur_unit.dot_ey;
         setfillstyle(1, bk_colr);
         bar(xs, ys, xe, ye);
    }
    return;
}
