#include <dos.h>
#include <graphics.h>
#include <alloc.h>

#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : the first menu item implement
 * calls     :
 * called by : main_menu_proc (main.c)
 * date      : 1993.9.15
 */
void set_data(void)
{
        UC *title = "注意:";
        UC *warn3 = "非酒店型用户不能选择市话是否参与结算!";

        switch(Menu_path)
        {
                case 100:                /* change the password */
                        modi_sys_pass(SYS_PWD);
                        break;
                case 101:                /* change the password */
                        modi_sys_pass(DEL_PWD);
                        break;
                case 102:                /* change the password */
                        modi_sys_pass(CLR_PWD);
                        break;
                case 103:                /* change the password */
                        modi_sys_pass(CASH_PWD);
                        break;
                case 110:
                        set_sio_cfg();
                        break;
                case 111:               /* system config       */
                        set_prn_cfg();
                        break;
                case 112:               /* system config       */
                        set_dsp_cfg();
                        break;
                case 113:               /* system config       */
                        set_delay();
                        break;
                case 114:
                        calib_time();
                        echo_date(get_date());
                        echo_time();
                        break;
                case 1200:              /* local call charge rate    */
                        set_rate(LDD);
                        break;
                case 1201:              /* national call charge rate */
                        set_rate(DDD);
                        break;
                case 1202:              /* intern call charge rate   */
                        set_rate(IDD);
                        break;
                case 1210:              /* add fee for local call    */
                        set_record_add(LDD);
                        break;
                case 1211:              /* add fee for nation call   */
                        set_record_add(DDD);
                        break;
                case 1212:               /* add fee for intern call   */
                        set_record_add(IDD);
                        break;
                case 1213:               /* add fee for intern call   */
                        set_record_add(HDD);
                        break;
                case 122:               /*new service fee etc.*/
                        set_month_add();
                        break;
                case 123:               /* half fee set up           */
                        set_half_fee();
                        break;
                case 124:               /* print the charge rate     */
                        prn_rate();
                        break;
                case 130:
                        set_usr_cfg();
                        break;
                case 131:
                        if(Usr_typ == HOTEL_USR)
                            set_ldd_cfg();
                        else
		            warn_mesg(title, warn3);
                        break;
                case 140:	/* 设置农话 */
                case 141:	/* 设置信息台 */
                case 142:	/* 设置禁拨号码 */
                case 143:	/* 设置紧急呼叫*/
                	set_special_call(Menu_path-140);
                        break;

                default:
                        deft_process();
                        break;
        }

        return;
}
