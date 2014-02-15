#include <graphics.h>
#include <bio.inc>
#include <menu.inc>
#include <key.inc>
#include "feedef.h"

#define BK_CLR    BLUE
#define CHR_CLR   WHITE    /* white */
#define HEAD_CLR  WHITE    /* white */

/* function  : inquire cash pledge of all telephone numbers
 * called by : statistic()
 * date      : 1993.10.18
 */
void inquire_cash_pledge(UC mode)
{
    UC flag;
    UI pg;
    TABLE_STRUCT  cash_tbl = {115,99,20,18,16,5,{48,72,72,72,72},GREEN};
    CASH_PLEDGE   *tbl;

    flag = load_cash_tbl(mode);       /* load cash file to memory */
    if(flag == 0)                 /* the file does not exist  */
    {
	message(CASH_FILE_ERR);
	return;
    }
    Cash_len[mode] = cash_tbl_len(mode);

    clr_DialWin(1);

    set_finger_color(Dsp_clr.fng_clr);
    draw_table(&cash_tbl);         /* draw the table frame */
    disp_cash_head(INQ_CASH, mode);      /* display table head   */
    disp_cash_help(INQ_CASH, mode);      /* display the help information */

    message_disp(8,"指示灯变红表示押金现额已低于押金下限");

    Count = 1;
    tbl = Cash_top[mode];
    disp_cash_tbl(tbl,Count,INQ_CASH, mode);

    if(!(Cash_len[mode]%16) && Cash_len[mode])
	 pg = Cash_len[mode]/16;
    else
	 pg = (UI)(Cash_len[mode]/16)+1;
    outf(542,397,11,14,"%2u",pg);

    locate_finger(0,2);

    inq_cash_data(mode);

    unload_cash_tbl(mode);

    recover_screen(1);
    message_end();
    return;
}

/* function  : do as input keys
 * called by : set_cash_pledge()
 * date      : 1993.9.23
 */
void inq_cash_data(UC mode)
{
    UI input;
    UC *title = "注意:";
    UC *warn = "最多只能有这么多分机号码!";
    UC esc=0;
    UNIT_STRUCT cur_unit;
    UI maxlen;

    maxlen = (mode == PHONE_CASH)? MAX_USERS:MAX_AUTHCD;

    while(1)
    {
        input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
		esc = 1;
		break;
            case UP:    /* GO TO THE LAST ROW */
                move_finger(0,1);
                break;

            case DOWN:  /* GO TO THE NEXT ROW */
                 if(Count+cur_unit.unit_x == maxlen)
                 {
	              warn_mesg(title, warn);
                      break;
                 }
		 if((Count+cur_unit.unit_x) < Cash_len[mode])
		     move_finger(1,1);
                 else
		     sound_bell();
                 break;

            case PAGEUP:
		 pgup_cash_tbl(INQ_CASH, mode);
		 break;

	    case PAGEDOWN:
		 pgdn_cash_tbl(INQ_CASH, mode);
                 break;

	    case F6:    /* locate according to the phone No input */
		 loc_cash_tbl(mode);
                 break;

	    case F7:    /* PAGE No. LOCATE */
		 loc_cash_page(INQ_CASH, mode);
                 break;

            default:
		 sound_bell();
		 break;
	}     /* END OF SWITCH */

	if(esc == 1)
	    break;
    }    /* END OF WHILE */

    return;
}

