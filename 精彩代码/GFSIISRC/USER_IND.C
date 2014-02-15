#include "feedef.h"

#define NOFRESH  0
#define REFRESH  1

/* function  : construct the user unit index
 * called by : set_user_unit()
 * input     : user_fblk  -- file block information for users.dat
 *             maint_fblk -- file block information for charge.pho
 * date      : 1993.11.9
 * note      : if the date and time of users.dat & charge.pho do not change,
 *             we will not work; otherwise, we will reconstruct the user
 *             unit index file
 */
void set_user_index(struct ffblk *user_fblk, struct ffblk *maint_fblk)
{
    int    i,j,flag;
    FILE   *fp;
    UI     pnum=0;
    struct ffblk fblk;
    USERS_STRUCT *user;
    PHONE_STRUCT *phone;

    /*if the date and time of the two files do no change, do not reconstruct*/
    flag = findfirst(ChargeDataFile[PHONE_CASH],&fblk,FA_RDONLY);
    if(flag == -1)           /* charge.pho does not exist */
    {
	message(PHO_FILE_ERR);
	return;
    }
    if((fblk.ff_fdate==maint_fblk->ff_fdate) && (fblk.ff_ftime==maint_fblk->ff_ftime))
    {
	flag = findfirst("gfsdata\\dept.dat",&fblk,FA_RDONLY);
	if(flag == -1)
	{
	    message(USER_FILE_ERR);
	    return;
	}
	if((fblk.ff_fdate==user_fblk->ff_fdate) && (fblk.ff_ftime==user_fblk->ff_ftime))
	    return;
    }

    load_user_tbl();
    UNewlen = user_tbl_len();
    if(UNewlen == 0)
	return;

    load_phone_tbl();
    Newlen = phone_tbl_len();

    fp = fopen("gfsdata\\phones.idx","wb");
    user = User_top;
    for(i=0;i<UNewlen;i++)
    {
	phone = Phone_top;
	pnum = 0;

	for(j=0;j<Newlen;j++)
	{
	    if(phone->user_no == user->user_no)
	    {
		fwrite(phone->phone_no,sizeof(A_PHONE),1,fp);
		pnum++;
	    }
	    phone++;
	}

	user->phones = pnum;
	user++;
    }
    fclose(fp);

    unload_phone_tbl();
    save_user_tbl(NOFRESH);
    unload_user_tbl();

    return;
}
