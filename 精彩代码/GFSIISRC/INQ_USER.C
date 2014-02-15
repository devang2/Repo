#include <graphics.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

/* function  : display the telephones belonging to the user unit input
 * called by : set_data()
 * date      : 1993.11.9
 */
void inquire_user_unit(void)
{
    FILE *fp;
    UC   flag, phone_no[10];
    UL   input_no=0;
    UI   key, num=0;
    UI   pnum=0;
    TABLE_STRUCT phone_tbl = {130,99,20,18,16,5,{60,100,100,100,100},GREEN};
    USERS_STRUCT user;

    flag = input_user(&input_no, INQUIRE);
    if(flag == 2)
        return;
    else if(flag == FALSE)        /* the user no does not exist */
    {
	message(UNO_NOT_EXIST);
        return;
    }

    search_user((UI)input_no,&pnum,&user);
    message_disp(8,"按任意键继续");        /* press any key */

    clr_DialWin(1);
    draw_table(&phone_tbl);
    inq_user_head(&user);

    fp = fopen("gfsdata\\phones.idx","rb");
    flag = fseek(fp,pnum*sizeof(A_PHONE),SEEK_SET);
    while(num < user.phones)
    {
	fread(phone_no,sizeof(A_PHONE),1,fp);
	disp_a_phone(phone_no,num);
	num++;
	if(num%64 == 0)
	{
	    key = get_key1();
	    if(key == ESC)
	    {
		fclose(fp);
		recover_screen(1);
		message_end();
		return;
	    }
	    else
                brush_tbl(5, 16, 7);
	}          /* end of "if(num%64 == 0)" */
    }         /* end of "if(fp != NULL)"        */
    fclose(fp);
    get_key1();

    recover_screen(1);
    message_end();
    return;
}


/* function  : search the user unit no in users.dat
 * input     : input_no -- the user unit no
 * output    : pnum -- the phones before this user
 *             user -- the user unit found
 * date      : 1993.11.9
 */
void search_user(UI input_no, UI *pnum, USERS_STRUCT *user)
{
    FILE  *fp;

   *pnum = 0;
    fp = fopen("gfsdata\\dept.dat","rb");
    while(fread(user,sizeof(USERS_STRUCT),1,fp) == 1)
    {
	if(user->user_no != input_no)
	   *pnum += user->phones;
	else
	    break;
    }
    fclose(fp);

    return;
}

/* function  : display the head information for the inquire-user-unit table
 * called by : inquire_user_unit()
 * input     : user -- the user unit structure
 * date      : 1993.11.10
 */
void inq_user_head(USERS_STRUCT *user)
{
    outf(140,80,7,0,"户头号: %-4u   户头名称: %-s", \
			    user->user_no, user->user_name);
    outf(140,103,7,0,"序号");
    outf(215,103,7,0,"分机号");
    outf(315,103,7,0,"分机号");
    outf(415,103,7,0,"分机号");
    outf(515,103,7,0,"分机号");

    return;
}

/* function  :     Display a page of phone num table.
 * calls     :     None
 * called by :     inqire_user_unit()
 * input     :     no  -- the phone No to be displayed
 *                 num -- the sequence No. of the phone no
 * date      :     1993.11.10
 */
void disp_a_phone(UC *no, UI num)
{
    UI i, j;
    UI x, y;
    UNIT_STRUCT cer_unit;

    if(num%64 == 0)          /* the first item of this page */
    {
	for(i=0;i<16;i++)      /* DISPLAY No. IN THE TABLE */
	{
	    j = num+i*4+1;
	    if(j > MAX_USERS)
		break;
	    outf(140,i*19+123,7,0,"%-3u",j);
	}
    }

    cer_unit.unit_x = (num%64)/4;
    cer_unit.unit_y = num%4+1;
    get_certain(&cer_unit);
    x = cer_unit.dot_sx+20;
    y = cer_unit.dot_sy+1;
    outf(x,y,7,0,"%-7s",no);

    return;
}
