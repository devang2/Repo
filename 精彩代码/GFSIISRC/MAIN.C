#define  MAIN_C
#include <graphics.h>
#include <alloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

UC       Quit = FALSE;        /* TRUE if quit is selected: ESC or mouse pressed */

/* function  :  main modula for HJD48 fee account system
 * calls     :  1. routines in the general-purpose library No 1 & No 2.
 *              2. all the devices & datas initiating routines.
 * called by :  None.
 * date      :  1993.9.8
 */
void main(int argc , char * argv[])
{
    UI key;         /* keep the key scan value get from keyboard  */
    UC leaf_node;   /* TRUE if current choice is a leaf node of the menu tree */
    UC port;
    struct textsettingstype save_text;                /* save fill style */

    if(argc >1 && strcmp(argv[1], "debug") == 0)
	    DEBUG = TRUE;
    else
	    DEBUG = FALSE;

    TimeOut = 182;   /* 18.2*10s */

    CashDataFile[PHONE_CASH] = "gfsdata\\cash.pho";
    CashDataFile[AUTH_CASH] = "gfsdata\\cash.aus";
    ChargeDataFile[AUTH_CASH] = "gfsdata\\charge.aus";
    ChargeDataFile[PHONE_CASH] = "gfsdata\\charge.pho";
    IDDRateFileName = "gfsdata\\idd.rat";
    DDDRateFileName = "gfsdata\\ddd.rat";
    LDDRateFileName = "gfsdata\\ldd.rat";
    TotTimeFileName = "gfsdata\\tottime.dat";

    /* initialize the global variables */
    Auth_cd_top = NULL;
    Phone_top = NULL;
    Cash_top[PHONE_CASH] = NULL;
    Cash_top[AUTH_CASH] = NULL;
    Call_top[0] = NULL;
    Call_top[1] = NULL;
    Call_top[2] = NULL;
    Call_top[3] = NULL;
    Tbltop = NULL;
    User_top = NULL;

    /* get system configration */
    get_sys_config();

    /* alloc the receive buffer for the serial ports */
    for(port=0; port<PORT_NUM; port++)  {
	SIORecBuf[port] = malloc(SIO_BUF_SIZE*sizeof(UC));
	CmdBuf[port] = malloc(CMD_BUF_SIZE*sizeof(UC));
	if(SIORecBuf[port] == NULL || CmdBuf[port] == NULL) {
	    printf("Not enough memory for GFSII\n");
	    exit(0);
	}
    }

/*    if ( XMS_init()!=TRUE )
	exit_scr(1, "MAIN.C/main(): XMS not installed");
*/
    /* set the pointer to ROM char fonts      */
    load_char();

    /* set the graphics mode and pallete      */
    init_scr();
    if( !init_menu1("gfsdata\\ntmenu.idx", "gfsdata\\ntmenu.txt", "gfsdata\\nthelp.txt", 4) )
	exit_scr(6, "Init menu ERROR. \n\nGFS system aborted.\n\n\n\n");

    gettextsettings(&save_text);
    settextstyle(3, HORIZ_DIR, 4);

    draw_screen(1);
    /* When debugging, you'd better turn the timer off.*/
    load_timer();
    echo_date( get_date() );
    echo_time();
    harderr(handler);
    cursor_on();
//    pop_head();

    /* initialize the serial port */
    SIOInit();

    calib_time();
    echo_date(get_date());
    echo_time();

    init_head_menu();

    /* the main loop */
    leaf_node = FALSE;
    while ( !Quit )
    {
	if ( leaf_node )
	{
	    main_menu_proc();
	    leaf_node = FALSE;
	}
	if ( left_pressed() )
	{
	    leaf_node = select_menu(0);
	    continue;
	}
	if ( !keybuf_nul() )
	{
	    key = get_key0();
	    clr_keybuf();
	    leaf_node = select_menu(key);
	}
    }

    /* restore the serial ports */
    SIOEnd();

    for(port=0; port<PORT_NUM; port++)  {
	free(SIORecBuf[port]);
	free(CmdBuf[port]);
    }

    exit_scr(0, "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nGFSII system normally shutdown!\n\n");
}

/* function  :  Call menu-explaining functions according to the menu-path
 *                stored in global variable "Menu_path".
 *                The current menu-path is set by the menu-process modula.
 * calls     :  All the menu-explaining routines of the "HT" sub-system.
 * called by :  The main function main() of "HT" sub-system.
 * date      :  1993.9.8
 */
void main_menu_proc(void)
{
    UI ui;

    if( (Menu_path/10000) != 0)
	ui = Menu_path/10000;
    else if( (Menu_path/1000) != 0)
	ui = Menu_path/1000;
    else if( (Menu_path/100) != 0)
	ui = Menu_path/100;
    else
	ui = Menu_path/10;

    switch (ui)
    {
	case 1:
	    set_data();
	    break;
	case 2:
	    get_half_fee();          /* get half fee setting  */
	    get_record_add();        /* get addfee for record */
	    if(e_time.year < 1993)
		get_default_time();  /* default time for sort */
	    phone_record_proc();
	    break;
	case 3:
	case 4:
	    get_half_fee();          /* get half fee setting  */
	    get_record_add();        /* get addfee for record */
	    if(e_time.year < 1993)
		get_default_time();  /* default time for sorting */
	    statistic();
	    break;
	default:
	    deft_process();
	    break;
    }

    return;
}

