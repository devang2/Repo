#include <graphics.h>
#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : get the add fee for every phone record from file
 * called by : main(), set_record_add()
 * date      : 1993.9.21
 */
void get_record_add(void)
{
    FILE  *fp;

    fp = fopen("gfsdata\\addfee.dat","rb");
    if(fp != NULL)
    {
	fread(&Intern_add,sizeof(ADD_STRUCT),1,fp);
	fread(&Honkon_add,sizeof(ADD_STRUCT),1,fp);
	fread(&Nation_add,sizeof(ADD_STRUCT),1,fp);
	fread(&Local_add, sizeof(ADD_STRUCT),1,fp);
	fclose(fp);
    }
    else
    {
	Intern_add.add_percent = 0;
	Intern_add.add_record  = 0;
	Intern_add.add_rate    = 0;
	Intern_add.add_time    = 0;

	Honkon_add.add_percent = 0;
	Honkon_add.add_record  = 0;
	Honkon_add.add_rate    = 0;
	Honkon_add.add_time    = 0;

	Nation_add.add_percent = 0;
	Nation_add.add_record  = 0;
	Nation_add.add_rate    = 0;
	Nation_add.add_time    = 0;

	Local_add.add_percent  = 0;
	Local_add.add_record   = 0;
	Local_add.add_rate     = 0;
	Local_add.add_time     = 0;
     }

    return;
}

/* function  : set the add fee for every phone record
 * called by : set_data() (set_data.c)
 * input     : UI mode -- LDD. DDD. IDD
 * date      : 1993.9.21
 */
void set_record_add(UC mode)
{
    int  key;
    UC   res1=0,res2=0,res3=0,res4=0, res5=0, esc=0;
    UI   back_in=8;
    UL   add_percent, add_record, add_rate, add_time, add_contry;
    FILE *fp;
    TABLE_STRUCT add_tbl   = {H_BX,H_BY-17,0,30,4,2,{150,120},7};
    TABLE_STRUCT add_tbl_d = {H_BX,H_BY-17,0,30,5,2,{150,120},7};
    UNIT_STRUCT  cur_unit;

    get_record_add();

    message_disp(8,"↓↑ 移动  Enter 输入");     /* Up,Dn: move,Enter: input */

    if(mode == DDD)
    {
        pop_back(H_BX-10,H_BY-55,H_BX+285,H_BY+95+60,7);    /* big frame */
        draw_table(&add_tbl_d);
    }
    else
    {
        pop_back(H_BX-10,H_BY-55,H_BX+285,H_BY+95+30,7);    /* big frame */
        draw_table(&add_tbl);
    }

    if(mode == IDD)          /* international call add fee */
	hz16_disp(H_BX+50,H_BY-40,"国 际 长 途 附 加 费",BLACK);
    else if(mode == HDD)     /* national call add fee      */
	hz16_disp(H_BX+50,H_BY-40,"港 澳 长 途 附 加 费",BLACK);
    else if(mode == DDD)     /* national call add fee      */
	hz16_disp(H_BX+50,H_BY-40,"国 内 长 途 附 加 费",BLACK);
    else if(mode == LDD)     /* local call add fee         */
	hz16_disp(H_BX+60,H_BY-40,"市  话  附  加  费 ",BLACK);

    hz16_disp(H_BX+20,H_BY-5, "取通话费的百分之",BLACK);   /* add_percent */
    hz16_disp(H_BX+20,H_BY+25,"每 张 话 单 加             元",BLACK); /* add_record*/
    hz16_disp(H_BX+20,H_BY+55,"附  加  费  率             元/次",BLACK); /* add_record*/
    hz16_disp(H_BX+20,H_BY+85,"附  加  时  长             秒",BLACK); /* add_time  */
    if(mode == DDD)
        hz16_disp(H_BX+20,H_BY+115,"农 话 过 线 费             元",BLACK); /* add_time  */

    draw_back(H_BX+153,H_BY-10,H_BX+225,H_BY+13, 11);
    draw_back(H_BX+153,H_BY+22,H_BX+225,H_BY+45, 11);
    draw_back(H_BX+153,H_BY+52,H_BX+225,H_BY+75, 11);
    draw_back(H_BX+153,H_BY+82,H_BX+225,H_BY+105,11);
    if(mode == DDD)
        draw_back(H_BX+153,H_BY+112,H_BX+225,H_BY+135,11);

    setcolor(BLACK);
    if(mode == IDD)          /* display the original values */
    {
       outf(H_BX+155+back_in,H_BY-6, 11,BLACK,"%u",Intern_add.add_percent);
       outf(H_BX+155+back_in,H_BY+26,11,BLACK,"%4.2f",((double)Intern_add.add_record)/100);
       outf(H_BX+155+back_in,H_BY+56,11,BLACK,"%4.2f",((double)Intern_add.add_rate)/100);
       outf(H_BX+155+back_in,H_BY+86,11,BLACK,"%u",Intern_add.add_time);
    }
    if(mode == HDD)          /* display the original values */
    {
       outf(H_BX+155+back_in,H_BY-6, 11,BLACK,"%u",Honkon_add.add_percent);
       outf(H_BX+155+back_in,H_BY+26,11,BLACK,"%4.2f",((double)Honkon_add.add_record)/100);
       outf(H_BX+155+back_in,H_BY+56,11,BLACK,"%4.2f",((double)Honkon_add.add_rate)/100);
       outf(H_BX+155+back_in,H_BY+86,11,BLACK,"%u",Honkon_add.add_time);
    }
    if(mode == DDD)          /* display the original values */
    {
       outf(H_BX+155+back_in,H_BY-6,  11,BLACK,"%u",Nation_add.add_percent);
       outf(H_BX+155+back_in,H_BY+26, 11,BLACK,"%4.2f",((double)Nation_add.add_record)/100);
       outf(H_BX+155+back_in,H_BY+56, 11,BLACK,"%4.2f",((double)Nation_add.add_rate)/100);
       outf(H_BX+155+back_in,H_BY+86, 11,BLACK,"%u",Nation_add.add_time);
    }
    if(mode == LDD)          /* display the original values */
    {
       outf(H_BX+155+back_in,H_BY-6, 11,BLACK,"%u",Local_add.add_percent);
       outf(H_BX+155+back_in,H_BY+26,11,BLACK,"%4.2f",((double)Local_add.add_record)/100);
       outf(H_BX+155+back_in,H_BY+56,11,BLACK,"%4.2f",((double)Local_add.add_rate)/100);
       outf(H_BX+155+back_in,H_BY+86,11,BLACK,"%u",Local_add.add_time);
    }

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0,1);

    for(;;)
    {
	key = get_key1();

	get_current(&cur_unit);
	switch(key)
	{
	    case ESC:
		esc = 1;
		break;
	    case UP:
		move_finger(0,1);
		break;
	    case DOWN:
		move_finger(1,1);            /* down a step */
		break;
	    case ENTER:
		hide_finger();
		switch(cur_unit.unit_x)
		{
		    case 0:     /* input the add percent */
			res1 = get_dec(H_BX+155,H_BY-7,20,60,back_in,2,\
					 &add_percent,0x00);
			if(res1)     /* valid input */
			    move_finger(1,1);
			break;
		    case 1:       /* input the add record */
		        set_get_color(11, 0, 12, 11, 0);
			res2 = get_money(H_BX+155,H_BY+25,20,60,back_in,1,\
					 &add_record,0x00);
			if(res2)     /* valid input */
			    move_finger(1,1);
			break;
		    case 2:       /* input the add record */
		        set_get_color(11, 0, 12, 11, 0);
			res3 = get_money(H_BX+155,H_BY+55,20,60,back_in,1,\
					 &add_rate,0x00);
			if(res3)     /* valid input */
			    move_finger(1,1);
			break;
		    case 3:       /* input the add time */
			res4 = get_dec(H_BX+155,H_BY+85,20,60,back_in,5,\
					 &add_time,0x00);
			if(res4 && mode != DDD)     /* valid input */
			    move_finger(0,3);
                        else if(res4 && mode == DDD)
                            move_finger(1,1);
			break;
		    case 4:       /* input the add time */
		        set_get_color(11, 0, 12, 11, 0);
			res5 = get_money(H_BX+155,H_BY+115,20,60,back_in,1,\
			                 &add_contry,0x00);
			if(res5)     /* valid input */
			    move_finger(0,4);
			break;
		    default:
			sound_alarm();
			break;
		}       /* end of "switch(cur_unit.unit_y)" */
		echo_finger();
		break;
	    default:
		sound_alarm();
		break;
	 }              /* end of "switch(key)" */

	 if(esc)
	    break;
    }                   /* end of "for(;;)"     */

    if(res1 || res2 || res3 || res4 || ( mode == DDD && res5 != 0) )      /* at least one input is valid */
    {
	switch(mode)
	{
	    case IDD:
		if(res1)
		    Intern_add.add_percent = (UI)add_percent;
		if(res2)
		    Intern_add.add_record  = (UI)add_record;
		if(res3)
		    Intern_add.add_rate  = (UI)add_rate;
		if(res4)
		    Intern_add.add_time    = (UI)add_time;
		break;
	    case HDD:
		if(res1)
		    Honkon_add.add_percent = (UI)add_percent;
		if(res2)
		    Honkon_add.add_record  = (UI)add_record;
		if(res3)
		    Honkon_add.add_rate  = (UI)add_rate;
		if(res4)
		    Honkon_add.add_time    = (UI)add_time;
		break;
	    case DDD:
		if(res1)
		    Nation_add.add_percent = (UI)add_percent;
		if(res2)
		    Nation_add.add_record  = (UI)add_record;
		if(res3)
		    Nation_add.add_rate  = (UI)add_rate;
		if(res4)
		    Nation_add.add_time    = (UI)add_time;
		break;
	    case LDD:
		if(res1)
		    Local_add.add_percent = (UI)add_percent;
		if(res2)
		    Local_add.add_record  = (UI)add_record;
		if(res3)
		    Local_add.add_rate  = (UI)add_rate;
		if(res4)
		    Local_add.add_time    = (UI)add_time;
		break;
	    default:
		sound_alarm();
		break;
	}     /* end of "switch(mode)"     */
    }         /* end of "if(res1 || res2)" */

    fp = fopen("gfsdata\\addfee.dat","wb");             /* save */
    fwrite(&Intern_add,sizeof(ADD_STRUCT),1,fp);
    fwrite(&Honkon_add,sizeof(ADD_STRUCT),1,fp);
    fwrite(&Nation_add,sizeof(ADD_STRUCT),1,fp);
    fwrite(&Local_add, sizeof(ADD_STRUCT),1,fp);
    fclose(fp);

    rid_pop();
    message_end();
    return;
}
