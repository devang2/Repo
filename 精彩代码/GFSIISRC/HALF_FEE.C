#include <conio.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : get half_fee setting from file.  If the file does not exist,
 *             set to default values
 * called by : main(), set_half_fee()
 * date      : 1993.9.24
 */
void get_half_fee(void)
{
    FILE  *fp;

    fp = fopen("gfsdata\\halfee.dat","rb");
    if(fp != NULL)      /* the file exists  */
    {
	fread(&half_fee,sizeof(HALF_FEE),1,fp);
	fread(holiday,sizeof(HOLIDAY),7,fp);
	fclose(fp);
     }
    else           /* set to default values */
    {
	half_fee.PM_hour = 24;
	half_fee.AM_hour = 0;
	half_fee.sunday  = FALSE;
	half_fee.saturday  = FALSE;

	holiday[0].mon = 1;
	holiday[0].day = 1;
	holiday[0].flag = FALSE;

	holiday[1].mon = 5;
	holiday[1].day = 1;
	holiday[1].flag = FALSE;

	holiday[2].mon = 10;
	holiday[2].day = 1;
	holiday[2].flag = FALSE;

	holiday[3].mon = 10;
	holiday[3].day = 2;
	holiday[3].flag = FALSE;

	holiday[4].mon = 0;
	holiday[4].day = 0;
	holiday[4].flag = FALSE;

	holiday[5].mon = 0;
	holiday[5].day = 0;
	holiday[5].flag = FALSE;

	holiday[6].mon = 0;
	holiday[6].day = 0;
	holiday[6].flag = FALSE;
    }

    return;
}

/* function  : set half fee flag for every. Sunday and holiday
 * called by : set_data()
 * date      : 1993.9.24
 */
void set_half_fee(void)
{
    TABLE_STRUCT  half_tbl  = {260,130,0,30,9,2,{90,50},7};
    TABLE_STRUCT  spring_day= {260,180,30,30,3,2,{60,60},7};

    get_half_fee();

    message_disp(8,"↓↑ 移动  Enter 输入");     /* Up,Dn: move, Enter: input*/
    pop_back(250,90,440,427,7);
    draw_table(&half_tbl);
    disp_day();

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);

    input_half_fee();
    rid_pop();

    if(holiday[4].flag != FALSE)  /* set to half fee */
    {
	pop_back(240,140,400,350,7);
	draw_table(&spring_day);
	disp_spring_day();

	set_finger_color(Dsp_clr.fng_clr);
	locate_finger(0,0);
	input_spring_day();
        rid_pop();
    }

    message_end();

    return;
}

/* function  : display all the HZ for half fee input
 * called by : set_half_fee()
 * date      : 1993.9.24
 */
void disp_day(void)
{
    UI  left=270;

    hz16_disp(300,110,"半 费 设 定",BLACK);        /* half_fee setting */
    hz16_disp(left,145,"？ → ２４",BLACK);        /* ? -- 24          */
    draw_back(355,140,415,164,11);
    outf(360,146,11,BLACK,"%d",half_fee.PM_hour);
    hz16_disp(left,175,"０  →  ？",BLACK);        /* 0 -- ?           */
    draw_back(355,170,415,194,11);
    outf(360,176,11,BLACK,"%d",half_fee.AM_hour);
    hz16_disp(left,205," 星 期 六",BLACK);         /* saturay           */
    if(half_fee.saturday)
	chang_key3(355, 203, 375, 223, "", 15);
    else
	draw_key(355, 203, 375, 223, "", 7, 0);

    hz16_disp(left,235," 星 期 日",BLACK);         /* sunday           */
    if(half_fee.sunday)
	chang_key3(355, 233, 375, 253, "", 15);
    else
	draw_key(355, 233, 375, 253, "", 7, 0);

    hz16_disp(left,265," 一月一日",BLACK);         /* 1.1  */
    if(holiday[0].flag)
	chang_key3(355, 263, 375, 283, "", 15);
    else
	draw_key(355, 263, 375, 283, "", 7,0);

    hz16_disp(left,295," 五月一日",BLACK);        /* 5.1  */
    if(holiday[1].flag)
	chang_key3(355, 293, 375, 313, "", 15);
    else
	draw_key(355, 293, 375, 313, "", 7,0);

    hz16_disp(left,325," 十月一日",BLACK);        /* 10.1 */
    if(holiday[2].flag)
	chang_key3(355, 323, 375, 343, "", 15);
    else
	draw_key(355, 323, 375, 343, "", 7,0);

    hz16_disp(left,355," 十月二日",BLACK);        /* 10.2 */
    if(holiday[3].flag)
	chang_key3(355, 353, 375, 373, "", 15);
    else
	draw_key(355, 353, 375, 373, "", 7,0);

    hz16_disp(left,385," 春    节" ,BLACK);        /* spring festival */
    if(holiday[4].flag)
	chang_key3(355, 383, 375, 403, "", 15);
    else
	draw_key(355, 383, 375, 403, "", 7,0);

    return;
}

/* function  : display the " chun jie ri qi ","yue" and "ri" in table, for
 *             spring day input
 * called by : set_half_fee()
 * date      : 1993.9.24
 */
void disp_spring_day(void)
{
    hz16_disp(288,160,"春节日期",BLACK);
    hz16_disp(283,190,"月",BLACK);
    hz16_disp(344,190,"日",BLACK);
    draw_back(263,215,317,239,11);
    outf(286,219,11,BLACK,"%d",holiday[4].mon);
    draw_back(324,215,378,239,11);
    outf(344,219,11,BLACK,"%d",holiday[4].day);
    draw_back(263,246,317,270,11);
    outf(286,250,11,BLACK,"%d",holiday[5].mon);
    draw_back(324,246,378,270,11);
    outf(344,250,11,BLACK,"%d",holiday[5].day);
    draw_back(263,277,317,301,11);
    outf(286,281,11,BLACK,"%d",holiday[6].mon);
    draw_back(324,277,378,301,11);
    outf(344,281,11,BLACK,"%d",holiday[6].day);

    return;
}

/* function  : input half fee hour and state
 * called by : set_half_fee()
 * date      : 1993.9.24
 */
void input_half_fee(void)
{
    UI input;
    UI back_in = 4;
    UL value;
    UC cur_x;
    UC result;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
		 if(holiday[4].flag == TRUE)    /* no half fee */
		     return;

		 result=select_1in3();
		 switch(result)
		 {
		     case 1:     /* DON'T QUIT */
			 break;

		     case 2:     /* SAVE AND QUIT */
			 save_half_fee();
			 return;

		     case 3:     /* QUIT AND DON'T SAVE */
			 return;
		 }
		 break;

	    case UP:    /* GO TO THE LAST ROW */
		 if(cur_unit.unit_x != 0)
		     move_finger(0,1);
		 else
		     move_finger(1,8);
		 break;

	    case DOWN:  /* GO TO THE NEXT ROW */
		 if(cur_unit.unit_x != 8)
		     move_finger(1,1);
		 else
		     move_finger(0,8);
		 break;

	    case ENTER:
		 hide_finger();
		 switch(cur_unit.unit_x)
		 {
		     case 0:      /* ? -- 24 */
			 do
			 {
			     result = get_dec(357,142,20,56,back_in,2,&value,0);
			 }while((value<21) || (value>24));

			 if(result)
			     half_fee.PM_hour = (UC)value;
			 break;
		     case 1:      /* 0 -- ? */
			 do
			 {
			     result = get_dec(357,172,20,56,back_in,1,&value,0);
			 }while(value > 7);

			 if(result)
			     half_fee.AM_hour = (UC)value;
			 break;
		     case 2:      /* saturday */
			 if(half_fee.saturday == FALSE)
			 {
			     half_fee.saturday = TRUE;
			     chang_key3(355, 203, 375, 223, "", 15);
			 }
			 else
			 {
			     half_fee.saturday = FALSE;
			     draw_key(355, 203, 375, 223, "", 7, 0);
			 }
			 break;
		     case 3:      /* sunday */
			 if(half_fee.sunday == FALSE)
			 {
			     half_fee.sunday = TRUE;
			     chang_key3(355, 233, 375, 253, "", 15);
			 }
			 else
			 {
			     half_fee.sunday = FALSE;
			     draw_key(355, 233, 375, 253, "", 7, 0);
			 }
			 break;
		     case 4:      /*  1.1 */
			 cur_x = cur_unit.unit_x;
			 if(holiday[cur_x-4].flag == FALSE)
			 {
			    holiday[cur_x-4].flag = TRUE;
			    chang_key3(355, 263, 375, 283, "", 15);			 }
			 else
			 {
			    holiday[cur_x-4].flag = FALSE;
			    draw_key(355, 263, 375, 283, "", 7,0);
			 }
			 break;
		     case 5:      /*  5.1 */
			 cur_x = cur_unit.unit_x;
			 if(holiday[cur_x-4].flag == FALSE)
			 {
			    holiday[cur_x-4].flag = TRUE;
			    chang_key3(355, 293, 375, 313, "", 15);
			 }
			 else
			 {
			    holiday[cur_x-4].flag = FALSE;
			    draw_key(355, 293, 375, 313, "", 7,0);
			 }
			 break;
		     case 6:      /* 10.1 */
			 cur_x = cur_unit.unit_x;
			 if(holiday[cur_x-4].flag == FALSE)
			 {
			    holiday[cur_x-4].flag = TRUE;
			    chang_key3(355, 323, 375, 343, "", 15);
			 }
			 else
			 {
			    holiday[cur_x-4].flag = FALSE;
			    draw_key(355, 323, 375, 343, "", 7,0);
			 }
			 break;
		     case 7:      /* 10.2 */
			 cur_x = cur_unit.unit_x;
			 if(holiday[cur_x-4].flag == FALSE)
			 {
			    holiday[cur_x-4].flag = TRUE;
			    chang_key3(355, 353, 375, 373, "", 15);
			 }
			 else
			 {
			    holiday[cur_x-4].flag = FALSE;
			    draw_key(355, 353, 375, 373, "", 7,0);
			 }
			 break;
		     case 8:      /* spring festival */
			 cur_x = cur_unit.unit_x;
			 if(holiday[cur_x-4].flag == FALSE)
			 {
			    holiday[cur_x-4].flag = TRUE;
			    chang_key3(355, 383, 375, 403, "", 15);
			 }
			 else
			 {
			    holiday[cur_x-4].flag = FALSE;
			    draw_key(355, 383, 375, 403, "", 7,0);
			 }
			 break;
		     default:
			 sound_alarm();
			 break;
		 }      /* end of "switch(cur_unit.unit_x)" */

		 echo_finger();
		 break;      /* ENTER */
	    default:
		    sound_bell();
		    break;
	}    /* end of switch(input)   */
    }        /* END OF WHILE */
}

/* function  : input the month and day for spring festival
 * called by : set_half_fee()
 * date      : 1993.9.24
 */
void input_spring_day(void)
{
    UI input,sy;
    UI back_in = 18;
    UL value;
    UC cur_x;
    UC result;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
		 if(holiday[4].flag == TRUE)
		     if((holiday[4].mon!=1 && holiday[4].mon!=2) ||
			(holiday[5].mon!=1 && holiday[5].mon!=2) ||
			(holiday[6].mon!=1 && holiday[6].mon!=2) )
		     {
			 message(SPRING_DAY);
			 break;
		     }
		 result=select_1in3();
		 switch(result)
		 {
		     case 1:     /* DON'T QUIT */
			 break;

		     case 2:     /* SAVE AND QUIT */
			 save_half_fee();
			 return;

		     case 3:     /* QUIT AND DON'T SAVE */
			 return;
		 }
		 break;

	    case UP:    /* GO TO THE LAST ROW */
		 move_finger(0,1);
		 break;

	    case DOWN:  /* GO TO THE NEXT ROW */
		 move_finger(1,1);
		 break;

	    case LEFT:
		 move_finger(2,1);
		 break;

	    case RIGHT:
		 move_finger(3,1);
		 break;

	    case ENTER:
		 sy = cur_unit.dot_sy;

		 hide_finger();
		 switch(cur_unit.unit_x)
		 {
		     case 0:
		     case 1:
		     case 2:
			 cur_x = cur_unit.unit_x;
			 switch(cur_unit.unit_y)
			 {
			 case 0:  /* month */

			     result = get_dec(273,sy+5,20,34,back_in,1,&value,0);

			     if(result)
			     {
				 move_finger(3,1);
				 holiday[cur_x+4].mon = (UC)value;
			     }
			     break;
			 case 1:  /* day   */
			     result = get_dec(326,sy+5,20,34,back_in,2,&value,0);

			     if(result)
			     {
				 move_finger(1,1);
				 move_finger(2,1);
				 holiday[cur_x+4].day = (UC)value;
			     }
			     break;
			 default:
			     sound_alarm();
			     break;
			 }       /* end of "switch(cur_unit.unit_y)" */
			 break;
		     default:    /* corresponding to case 0.1.2 */
			 sound_alarm();
			 break;
		 }               /* end of "swithc(cur_unit.unit_x)" */
		 echo_finger();
		 break;          /* end of ENTER */

	    default:             /* cooresponding to ENTER.UP.DOWN etc */
		 sound_alarm();
		 break;
	}        /* end of "switch(input)" */
    }            /* end of "while(1)"      */
}

/* function  : save half_fee setting to file
 * called by : set_half_fee()
 * date      : 1993.9.24
 */
void save_half_fee(void)
{
    FILE  *fp;

    if(holiday[4].flag == TRUE)
    {
       holiday[5].flag = TRUE;
       holiday[6].flag = TRUE;
    }
    else
    {
       holiday[5].flag = FALSE;
       holiday[6].flag = FALSE;
    }
    fp = fopen("gfsdata\\halfee.dat","wb");
    fwrite(&half_fee,sizeof(HALF_FEE),1,fp);
    fwrite(holiday,sizeof(HOLIDAY),7,fp);
    fclose(fp);

    return;
}
