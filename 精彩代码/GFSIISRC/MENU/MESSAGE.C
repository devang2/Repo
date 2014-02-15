#include <graphics.h>

#include <bio.inc>
#include <menu.inc>

#include "feedef.h"

extern UC Cursor_on;
extern UL far *Tick_cnt_ptr;
static UC old_cursor_stat;

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
	  hz16_disp(MES_LEFT+8,MES_TOP+1, "←↓→↑ 移动   Enter 确认",0);
	  break;
      case PASS_ERR:    /* error in password input   */
	  message_win(back_in,"输入错误，请重新输入!",echo_time);
	  break;
      case RE_PASS:     /* pleae Re-enter the password */
	  message_win(back_in,"请再输入一遍新密码!",echo_time);
	  break;
      case RE_PASS_ERR: /* error in Re-entern the password */
	  message_win(back_in,"两次输入不同，请重来!",echo_time);
	  break;
      case NEW_PASS_EN: /* the new password has been valid */
	  message_win(back_in,"新密码已经生效!",echo_time);
	  break;
      case TIME_OUT:         /* connecting time is out */
	  message_win(back_in,"打印机联机错误!",echo_time);
	  break;
      case IO_ERR:           /* printer I/O error      */
	  message_win(back_in,"打印机I/O错误!",echo_time);
	  break;
      case NO_PAPER:         /* printer no paper       */
	  message_win(back_in,"打印机无纸!",echo_time);
	  break;
      case PRN_ERR:          /* some error in printer  */
	  message_win(back_in,"打印机错误!",echo_time);
	  break;
      case PHO_FILE_ERR:     /* input phone number first */
	  message_win(back_in,"请先输入话机号!",echo_time);
	  break;
      case SPRING_DAY:       /* spring festival days must be input    */
	  message_win(back_in,"必须输入春节日期!",echo_time);
	  break;
      case CONN_ERR:         /* connection error in SIO communication */
	  message_win(back_in,"联机错误!",echo_time);
	  break;
      case REC_TIMEOUT:      /* timeout in receiving records or no B8 */
	  message_win(back_in,"接收错误!",echo_time);
	  break;
      case RATE_FILE_ERR:    /* input charge rate first */
	  message_win(back_in,"请先输入费率!",echo_time);
	  break;
      case NO_NOT_EXIST:     /* the phone number input does not exist */
	  message_win(back_in,"该电话号码不存在!",echo_time);
	  break;
      case CASH_FILE_ERR:
	  message_win(back_in,"请先进行押金设定!",echo_time);
	  break;
      case USER_FILE_ERR:
	  message_win(back_in,"请先进行户头设定!",echo_time);
	  break;
      case UNO_NOT_EXIST:
	  message_win(back_in,"该户头号不存在!",echo_time);
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

    pop_back(D_LEFT,D_BOTTOM,MAX_X-S_XAD,MAX_Y,11);
    hz16_disp(MES_LEFT+back_in,MES_TOP+1, msg,0);

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
    pop_back(D_LEFT,D_BOTTOM,MAX_X-S_XAD,MAX_Y,11);
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
