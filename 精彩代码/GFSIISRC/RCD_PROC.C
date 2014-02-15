#include "feedef.h"
#include <bio.inc>

/* function  : the second menu item implement
 * calls     :
 * called by : main_menu_proc (main.c)
 * date     :  1993.9.8
 */
void phone_record_proc(void)
{
    UC suc;

    switch(Menu_path)
    {
	case 200:       /* take the immediate records */
	    suc = take_all_record();
	    if(suc == 1)     /* successful */
		take_immediate_record();
	    break;
	case 2010:       /* immediate print selection  */
	    set_imm_print();
            break;
	case 2011:       /* immediate print selection  */
            set_phone_num(SET_IMPRN);
	    break;

	case 210:             /* set sorting time     */
	    set_sort_time();
	    break;
	case 2110:      /* inquire all the local records */
	    inquire_all_local(WHOLE);
	    break;
	case 21110:      /* inquire local records of one telephone  */
	    inquire_one_local();
	    break;
	case 21111:      /* inquire all local records of all telephone */
	    inquire_all_local(PHONE_NO);
	    break;
	case 21120:      /* inquire local records of one user unit  */
	    inquire_user_local();
	    break;
	case 21121:      /* inquire all local records of all user */
	    inquire_all_local(PHONE_NO);
	    break;
	case 21130:      /* inquire local records of a auth code user  */
            inquire_auth_local();
	    break;
	case 21131:      /* inquire all local records of all auth code user */
	    inquire_all_local(AUTH_CD);
	    break;

	case 2120:      /* inquire all the long records */
	    inquire_all_long(WHOLE);
	    break;
	case 21210:      /* inquire long records of one telephone  */
	    inquire_one_long();
	    break;
	case 21211:      /* inquire all long records of all telephone */
	    inquire_all_long(PHONE_NO);
	    break;
	case 21220:      /* inquire long records of one user unit  */
	    inquire_user_long();
	    break;
	case 21221:      /* inquire all long records of all user */
	    inquire_all_long(PHONE_NO);
	    break;
	case 21230:      /* inquire long records of a auth code user  */
	    inquire_auth_long();
	    break;
	case 21231:      /* inquire all long records of all auth code user */
	    inquire_all_long(AUTH_CD);
	    break;

	case 2130:       /* inquire all the records of all the phone */
	    inquire_all_record(NORMAL);
	    break;
	case 2131:       /* print all the records                 */
	    inquire_one_phone();
	    break;
	case 2132:       /* print all the records                 */
	    inquire_one_user();
	    break;
        case 2133:
	    inquire_one_authcd();
            break;

	case 220:             /* set sorting time     */
	    set_sort_time();
	    break;
	case 2210:      /* print all the local records */
	    print_all_local(WHOLE);
	    break;
	case 22110:      /* print local records of one telephone  */
	    print_one_local();
	    break;
	case 22111:      /* print all local records of all telephone */
	    print_all_local(PHONE_NO);
	    break;
	case 22120:      /* print local records of one user unit  */
	    print_user_local();
	    break;
	case 22121:      /* print all local records of all user */
	    print_all_local(PHONE_NO);
	    break;
	case 22130:      /* print local records of a auth code user  */
	    print_auth_local();
	    break;
	case 22131:      /* print all local records of all auth code user */
	    print_all_local(AUTH_CD);
	    break;

	case 2220:      /* print all the long records */
	    print_all_long(WHOLE);
	    break;
	case 22210:      /* print long records of one telephone  */
	    print_one_long();
	    break;
	case 22211:      /* print all long records of all telephone */
	    print_all_long(PHONE_NO);
	    break;
	case 22220:      /* print long records of one user unit  */
	    print_user_long();
	    break;
	case 22221:      /* print all long records of all user */
	    print_all_long(PHONE_NO);
	    break;
	case 22230:      /* print long records of a auth code user  */
  	    print_auth_long();
	    break;
	case 22231:      /* print all long records of all auth code user */
	    print_all_long(AUTH_CD);
	    break;

	case 2230:       /* print all the records                 */
	    print_all(NORMAL);
	    break;
	case 2231:       /* print all the records                 */
	    print_one_phone();
	    break;
	case 2232:       /* print all the records                 */
	    print_one_user();
	    break;
	case 2233:       /* print all the records                 */
	    print_one_authcd();
            break;

	case 2300:       /* delete records of one phone       */
	    delete_all_record(PHONE_NO);
	    break;

	case 2301:       /* delete records of one auth code       */
	    delete_all_record(AUTH_CD);
	    break;

	case 2302:       /* delete all authh code records                        */
	    delete_all_record(WHOLE);
	    break;

	case 23100:        /* out_of_range records                  */
	    inquire_all_record(OUTRNG);
	    break;
	case 23101:        /* out_of_range records                  */
	    print_all(OUTRNG);
	    break;
	case 23102:        /* out_of_range records                  */
	    out_of_range();
	    break;

	case 23110:        /* out_of_range records                  */
	    inquire_all_record(NONFEE);
	    break;
	case 23111:        /* out_of_range records                  */
	    print_all(NONFEE);
	    break;
	case 23112:        /* out_of_range records                  */
	    recharge_all_record();
	    break;

	case 2320:       /* backup the old telephone records      */
	    backup_old_rec();
	    break;

	case 23210:       /* inquire the old telephone records     */
	    inquire_old_rec(PHONE_NO);
	    break;
	case 23211:       /* inquire the old telephone records     */
	    inquire_old_rec(AUTH_CD);
	    break;
	case 23212:       /* inquire the old telephone records     */
	    inquire_old_rec(WHOLE);
	    break;

	case 24:
 	    clr_rcd();
	    break;
	default:
	    deft_process();
	    break;
    }                   /* end of "switch(Menu_path)"            */

    return;
}

void warn_mesg(UC *title, UC *warn)
{
    sound_alarm();
    set_msg_color(7, 0, 0);
    msg_win(210, 160, title, warn, 100);
}