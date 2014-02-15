#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

static UC old_cursor_stat;
extern UC Cursor_on;
extern UL far *Tick_cnt_ptr;

/* function  : display message in the small status win according to mode
 * calls     : message_win
 * called by : this is a general purposed routine
 * input     : mode -- information kind
 * date      : 1993.9.20
 */
void message(int mode)
{
  UC back_in=8,echo_time=30;

  if(mode != MAIN_MES)
      sound_alarm();

  switch(mode)
  {
      case MAIN_MES:    /* -><- move, ENTER retify   */
	  message_win(back_in, "←↓→↑ 选择   Enter 选中",50);
	  break;
      case PASS_ERR:    /* error in password input   */
	  message_win(back_in,"口令输入错误，请重新输入",echo_time);
	  break;
      case RE_PASS:     /* pleae Re-enter the password */
	  message_win(back_in,"请再输入一次新口令",echo_time);
	  break;
      case RE_PASS_ERR: /* error in Re-entern the password */
	  message_win(back_in,"两次输入不同，请重新输入",echo_time);
	  break;
      case NEW_PASS_EN: /* the new password has been valid */
	  message_win(back_in,"口令变更成功, 新口令已经生效",echo_time);
	  break;
      case TIME_OUT:         /* connecting time is out */
	  message_win(back_in,"打印机联机错误",echo_time);
	  break;
      case IO_ERR:           /* printer I/O error      */
	  message_win(back_in,"打印机I/O错误",echo_time);
	  break;
      case NO_PAPER:         /* printer no paper       */
	  message_win(back_in,"打印机无纸",echo_time);
	  break;
      case PRN_ERR:          /* some error in printer  */
	  message_win(back_in,"打印机错误",echo_time);
	  break;
      case PHO_FILE_ERR:     /* input phone number first */
	  message_win(back_in,"应先进行分机号码设定",echo_time);
	  break;
      case SPRING_DAY:       /* spring festival days must be input    */
	  message_win(back_in,"必须输入春节日期",echo_time);
	  break;
      case CONN_ERR:         /* connection error in SIO communication */
	  message_win(back_in,"联机错误",echo_time);
	  break;
      case REC_TIMEOUT:      /* timeout in receiving records or no B8 */
	  message_win(back_in,"接收错误",echo_time);
	  break;
      case RATE_FILE_ERR:    /* input charge rate first */
	  message_win(back_in,"费率文件出错",echo_time);
	  break;
      case NO_NOT_EXIST:     /* the phone number input does not exist */
	  message_win(back_in,"该分机号码不存在",echo_time);
	  break;
      case CASH_FILE_ERR:
	  message_win(back_in,"应先进行押金设定",echo_time);
	  break;
      case USER_FILE_ERR:
	  message_win(back_in,"应先进行户头设定",echo_time);
	  break;
      case PCODE_FILE_ERR:
          message_win(back_in,"应先设定立即打印分机",echo_time);
	  break;
      case PCODE_ERR:
	  message_win(back_in,"存在没设定分机内码",60);
	  break;
      case UNO_NOT_EXIST:
	  message_win(back_in,"该户头号码不存在",echo_time);
	  break;
      case TFEE_FILE_ERR:
	  message_win(back_in,"应先进行话费统计",echo_time);
	  break;
      case DTIME_FILE_ERR:
	  message_win(back_in,"应先进行汇总统计",echo_time);
	  break;
      case CANCEL_INPUT:
	  message_win(back_in,"取消刚才输入项目",echo_time);
	  break;
      case INVALID_INPUT:
	  message_win(back_in,"无效输入项目, 输入取消",echo_time);
	  break;
      case AUTHCD_FILE_ERR:
	  message_win(back_in,"应先进行授权码设定",echo_time);
	  break;
      case AUTHCD_NOT_EXIST:
	  message_win(back_in,"该授权卡号不存在",echo_time);
	  break;
      case ACCTIME_FILE_ERR:
	  message_win(back_in,"日期文件出错",echo_time);
	  break;
      case ACCFEE_FILE_ERR:
	  message_win(back_in,"无话费结算文件",echo_time);
	  break;
      case BANACC_FILE_ERR:
	  message_win(back_in,"无单班营业文件",echo_time);
	  break;
      case DAYACC_FILE_ERR:
	  message_win(back_in,"无日营业文件",echo_time);
	  break;
      case MONACC_FILE_ERR:
	  message_win(back_in,"无月营业文件",echo_time);
	  break;
      case YEARACC_FILE_ERR:
	  message_win(back_in,"无年营业文件",echo_time);
	  break;
      case HACCFEE_FILE_ERR:
	  message_win(back_in,"无话费结算历史文件",echo_time);
	  break;
      case HBANACC_FILE_ERR:
	  message_win(back_in,"无单班营业历史文件",echo_time);
	  break;
      case HDAYACC_FILE_ERR:
	  message_win(back_in,"无日营业历史文件",echo_time);
	  break;
      case HMONACC_FILE_ERR:
	  message_win(back_in,"无月营业历史文件",echo_time);
	  break;
      case HYEARACC_FILE_ERR:
	  message_win(back_in,"无年营业历史文件",echo_time);
	  break;
      case NULL_ACCOUNT_ERR:
	  message_win(back_in,"该住客话费结算为零",echo_time);
	  break;
      case NULL_CASH_ERR:
	  message_win(back_in,"无超限或透支分机",echo_time);
	  break;
   }

   return;
}

/* function  : preserve the old message and display new message in the
 *             small status win.  If anykey is pressed or the time is
 *             longer than "echo_time", the old message is poped back
 * called by : message()
 * input     : back_in -- from what position in the status window to
 *                        display the information
 *             msg     -- the new information
 *             echo_time -- the longest displaying time
 * date      : 1993.9.20
 */
void message_win(UC back_in, UC *msg, UC echo_time)
{
    UC old_cursor_stat;
    UL start_time;
    MOUSE_BAND_STRUCT  mouse_band;

    pop_back(M_EX+2,D_BOTTOM+1,MAX_X-S_XAD-2,MAX_Y,LIGHTRED);
    hz16_disp(MES_LEFT+back_in,MES_TOP+1, msg, YELLOW);

    old_cursor_stat = Cursor_on;
    cursor_on();
    get_mouse_band(&mouse_band);
    clr_keybuf();
    start_time = *Tick_cnt_ptr;
    while (1)
    {
	if ( !keybuf_nul() )
	{
	    get_key0();
	    break;
	}
	if ( left_pressed() )
	    break;
	if (echo_time)
	{
	    if (*Tick_cnt_ptr-start_time>echo_time)
	    break;
	}
    }

    cursor_off();
    rid_pop();
    set_mouse_band(&mouse_band);
    if (old_cursor_stat==TRUE)
	cursor_on();

    return;
}

/* function  : display  "msg" in the status window. The only method to
 *             retrieve the old message is to call "message_end", that
 *             is displaying time of "msg" is not limited by echo_time
 * called by : set_record_add()   (set_add.c)
 * input     : back_in, msg -- the same as those in "message_win()"
 * date      : 1993.9.22.
 */
void message_disp(UC back_in, UC *msg)
{
    pop_back(M_EX,D_BOTTOM+1,MAX_X-S_XAD-2,MAX_Y,11);
    hz16_disp(MES_LEFT+back_in,MES_TOP+1, msg,0);

    old_cursor_stat = Cursor_on;
    cursor_on();
    return;
}

/* function  : to retrieve the old message preserved by "message_disp()"
 * called by : message_disp()
 * date      : 1993.9.22
 */
void message_end(void)
{
    cursor_off();
    rid_pop();
    if (old_cursor_stat==TRUE)
	cursor_on();

    return;
}
