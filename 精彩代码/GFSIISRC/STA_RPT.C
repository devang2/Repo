
#include "feedef.h"
#include <bio.inc>

/* function  : statistic
 * calls     :
 * called by : main_menu_handle (main.c)
 * date      : 1993.9.8
 */
void statistic(void)
{
    UC *title = "注意:";
    UC *warn2  = "非酒店用户不能使用话费结算功能!";
    UC flag;

    if(Usr_typ == NORMAL_USR)
        if(Menu_path == 3000 || Menu_path == 3001 || (Menu_path >= 3010 && \
	   Menu_path <= 3013) || (Menu_path >= 30200 && Menu_path <= 30314) )
        {
             warn_mesg(title, warn2);
             return;
        }

    switch(Menu_path)
    {
	case 3000:
            acount_one(PHONE_FEE);
	    break;

	case 3001:
            acount_one(AUTH_CD);
	    break;

	case 3010:
            acc_proc(BAN_ACC);
	    break;
	case 3011:
            acc_proc(DAY_ACC);
	    break;
	case 3012:
            acc_proc(MON_ACC);
	    break;
	case 3013:
            acc_proc(YEAR_ACC);
	    break;

	case 30200:
            inq_acc_fee(FEE_ACC);
	    break;
	case 30201:
            inq_acc_fee(BAN_ACC);
	    break;
	case 30202:
            inq_acc_fee(DAY_ACC);
	    break;
	case 30203:
            inq_acc_fee(MON_ACC);
	    break;
	case 30204:
            inq_acc_fee(YEAR_ACC);
	    break;

	case 30210:
            inq_acc_fee(HFEE_ACC);
            break;
	case 30211:
            inq_acc_fee(HBAN_ACC);
	    break;
	case 30212:
            inq_acc_fee(HDAY_ACC);
	    break;
	case 30213:
            inq_acc_fee(HMON_ACC);
	    break;
	case 30214:
            inq_acc_fee(HYEAR_ACC);
	    break;

	case 30300:
            prn_acc_fee(FEE_ACC);
	    break;
	case 30301:
            prn_acc_fee(BAN_ACC);
	    break;
	case 30302:
            prn_acc_fee(DAY_ACC);
	    break;
	case 30303:
            prn_acc_fee(MON_ACC);
	    break;
	case 30304:
            prn_acc_fee(YEAR_ACC);
	    break;

	case 30310:
            prn_acc_fee(HFEE_ACC);
            break;
	case 30311:
            prn_acc_fee(HBAN_ACC);
	    break;
	case 30312:
            prn_acc_fee(HDAY_ACC);
	    break;
	case 30313:
            prn_acc_fee(HMON_ACC);
	    break;
	case 30314:
            prn_acc_fee(HYEAR_ACC);
	    break;

	case 310:             /* total statitic table       */
            overflow_cash(INQUIRE, PHONE_CASH, 0, 0);
            overflow_cash(INQUIRE, AUTH_CASH, 0, 0);
	    break;
	case 311:             /* total statitic table       */
	    overflow_cash(PRINT, PHONE_CASH, 0, 0);
	    overflow_cash(PRINT, AUTH_CASH, 0, 0);
	    break;

	case 320:                /* cash pledge set up        */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // correct password
  	        set_csh_swtch();
	    break;
  	case 321:                /* cash pledge set up for phones    */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // correct password
	        set_cash_pledge(PHONE_CASH);
	    break;
       case 322:                /* inq cash pledge of phones    */
            inquire_cash_pledge(PHONE_CASH);
	    break;
	case 323:                /* cash pledge set up for auth    */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // correct password
	        set_cash_pledge(AUTH_CASH);
	    break;
       case 324:                /* inq cash pledge of auth    */
            inquire_cash_pledge(AUTH_CASH);
	    break;
	case 330:
	    set_sort_time();
	    break;
	case 331:             /* calculate total fee  */
	    cal_total_fee();
	    break;

	case 34000:
            inq_fee_part();
            break;
	case 34001:
	    inq_fee_all();
	    break;

	case 34010:            /* invoice information of all user units  */
	    inq_ufee_part();
	    break;
	case 34011:            /* invoice information of all user units  */
	    inq_ufee_all();
	    break;

	case 34020:            /* invoice information of all user units  */
	    inq_afee_part();
	    break;
	case 34021:            /* invoice information of all user units  */
	    inq_afee_all();
	    break;

	case 34100:
	    process_fee_one(INQUIRE, 0);
	    break;
	case 34101:
	    process_fee_part(INQUIRE, 0);
	    break;

	case 34110:            /* invoice information of a user unit     */
	    inq_one_usr();
	    break;
	case 34111:            /* invoice information of all user units  */
	    inq_part_usr();
	    break;

	case 34120:            /* invoice information of a user unit     */
	    process_afee_one(INQUIRE, 0);
	    break;
	case 34121:            /* invoice information of all user units  */
	    process_afee_part(INQUIRE, 0);
	    break;

	case 3500:            /* invoice of specified phone */
	    process_fee_one(PRINT, INV);
	    break;
	case 3501:            /* invoice of all      phones */
	    process_fee_all(INV);
	    break;
	case 3502:            /* invoice of part phones     */
	    process_fee_part(PRINT, INV);
	    break;

	case 3510:            /* invoice information of a user unit     */
	    inv_one_usr(INV);
	    break;
	case 3511:            /* invoice information of all user units  */
	    inv_all_usr();
	    break;
	case 3512:            /* invoice information of part user units */
	    inv_part_usr();
	    break;

	case 3520:            /* invoice information of a auth code     */
	    process_afee_one(PRINT, INV);
	    break;
	case 3521:            /* invoice of all      phones */
	    process_afee_all(INV);
	    break;
	case 3522:            /* invoice information of all auth code  */
	    process_afee_part(PRINT, INV);
	    break;

	case 3600:            /* detail of specified phone */
	    process_fee_one(PRINT, DETL);
	    break;
	case 3601:            /* detail of all      phones */
	    prtfee_all(PHONE_FEE);
	    break;
	case 3602:            /* detail of part phones     */
	    prtfee_part(PHONE_FEE);
	    break;

	case 3610:            /* detail information of a user unit     */
	    inv_one_usr(DETL);
	    break;
	case 3611:            /* detail information of all user units  */
            prtfee_all(DEPT_FEE);
	    break;
	case 3612:            /* detail information of part user units */
            prtfee_part(DEPT_FEE);
	    break;

	case 3620:            /* detail of specified phone */
	    process_afee_one(PRINT, DETL);
	    break;
	case 3621:            /* detail of all      phones */
	    prtafee_all();
	    break;
	case 3622:            /* detail of part phones     */
	    prtafee_part();
	    break;

	case 370:             /* statiting total fee       */
	    stats_fee();
	    break;
	case 371:             /* inquiring total fee       */
	    inq_total_fee();
	    break;
	case 372:             /* printing total fee        */
	    prt_total_fee();
	    break;
        case 380:                /* input phone number  */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // not press the ESC
                set_phone_num(SET_PCODE);
            break;
        case 390:		/* set authority head */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // not press the ESC
                set_auth_head();
            break;
        case 391:               /* set authority code */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // not press the ESC
                process_auth_code(SET_AUTH);
            break;
        case 392:               /* inq authority code */
            process_auth_code(INQ_AUTH);
            break;

        case 400:               /* set user unit             */
	    flag = valid_pass(CASH_PWD);
	    if(flag == FALSE)
		message(PASS_ERR);
            else if(flag == TRUE)   // not press the ESC
                set_user_unit();
            break;
        case 401:               /* inquire user unit         */
            inquire_user_unit();
            break;
        case 402:               /* print the user unit       */
            print_user_unit();
            break;
    }

    return;
}
