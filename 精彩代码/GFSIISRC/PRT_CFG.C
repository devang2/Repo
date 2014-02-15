#include <graphics.h>
#include <bios.h>
#include <conio.h>

#include <key.inc>
#include <bio.inc>
#include <menu.inc>
#include "feedef.h"

/* function  : read system config from file. If not exist,set to default value
 * called by : main(), set_sys_config()
 * date      : 1993.9.24
 */
void get_sys_config(void)
{
    FILE  *fp;
    UC i;

    fp = fopen("gfsdata\\system.cfg","rb");
    if(fp != NULL)
    {
	fread(&Sys_mode,  sizeof(SYS_MODE),1,fp);
	fread(&Intern_prn,sizeof(UC),1,fp);
	fread(&Nation_prn,sizeof(UC),1,fp);
	fread(&Local_prn, sizeof(UC),1,fp);
	fread(&Cashflg,   sizeof(UC),2,fp);
	fread(&Usr_nam,   sizeof(UC),31,fp);
	fread(&Usr_typ,   sizeof(UC),1,fp);
	fread(&Delay,     sizeof(DELAY_STRUCT),1,fp);
        fread(&Dsp_typ,   sizeof(UC),1,fp);
        fread(&Dsp_clr,   sizeof(COLOR_STRUCT),1,fp);
        fread(&Page_len,  sizeof(UC),1,fp);
        fread(&Ldd_fflg,  sizeof(UC),1,fp);
	fclose(fp);
    }
    else
    {
//	Sys_mode.pabx_m    = NTM;          /* LQ1600 */
//	Sys_mode.pabx_m    = HAX1001;      /* LQ1600 */
	Sys_mode.prt_m     = LX800;        /* LQ1600 */
	Sys_mode.prt_w     = N_WIDTH;      /* 80 col paper*/
	Sys_mode.prtflg    = OUTLIB;       /* 80 col paper*/
        for(i=0; i<PORT_NUM; i++)
	    Sys_mode.com_m[i]  = 0;         /* Unused  */
        Page_len           = 38;
	Delay.delay_idd    = 0;
	Delay.delay_ddd    = 0;
	Delay.delay_ldd    = 0;
	Intern_prn         = TRUE;
	Nation_prn         = TRUE;
	Local_prn          = FALSE;
	Cashflg[PHONE_CASH]= FALSE;
	Cashflg[AUTH_CASH] = FALSE;
        Usr_typ            = NORMAL_USR;
        Usr_nam[0]         = '\0';
        Dsp_typ            = COLOR;
        Dsp_clr.fng_clr    = LIGHTGREEN;
        Ldd_fflg           = YES;
    }
    return;
}

/* function  : save the system config to file.
 * called by :
 * date      : 1996.3.17
 */
void save_sys_config()
{
    FILE *fp;

    fp = fopen("gfsdata\\system.cfg","wb");

    fwrite(&Sys_mode,   sizeof(SYS_MODE),1,fp);
    fwrite(&Intern_prn, sizeof(UC), 1, fp);
    fwrite(&Nation_prn, sizeof(UC), 1, fp);
    fwrite(&Local_prn,  sizeof(UC), 1, fp);
    fwrite(&Cashflg,    sizeof(UC), 2, fp);
    fwrite(&Usr_nam,    sizeof(UC), 31,fp);
    fwrite(&Usr_typ,    sizeof(UC), 1, fp);
    fwrite(&Delay,      sizeof(DELAY_STRUCT),1,fp);
    fwrite(&Dsp_typ,    sizeof(UC),1,fp);
    fwrite(&Dsp_clr,    sizeof(COLOR_STRUCT),1,fp);
    fwrite(&Page_len,   sizeof(UC),1,fp);
    fwrite(&Ldd_fflg,   sizeof(UC),1,fp);

    fclose(fp);
}


/* function  : set the printer configeration
 * called by : set_data()
 * date      : 1993.9.22
 */
void set_prn_cfg(void)
{
    TABLE_STRUCT  prn_cfg_tbl = {H_BX+10, H_BY-40, 40, 40, 2, 2, {130, 100+64}, 7};

    message_disp(8,"↓↑ 选择  Enter 切换");     /* Up,Dn: move,Enter: switch */
    pop_back(H_BX, H_BY-45, H_BX+250+64, H_BY+100, 7);
    draw_table(&prn_cfg_tbl);
    disp_prn_cfg();

    set_finger_color(Dsp_clr.fng_clr);
    locate_finger(0, 0);

    input_prn_cfg();

    rid_pop();
    message_end();
    return;
}

/* function  : display all the HZ for print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void disp_prn_cfg(void)
{
    int i,j;
    UI  x,y;
    UNIT_STRUCT  cur_unit;

    hz16_disp(H_BX+58+32, H_BY-25,"打 印 设 备 设 置",BLACK);

    j = 0;
    for(i=0; i<2; i++)
    {
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+5;
	y = cur_unit.dot_sy+8;

	switch(i)
	{
	    case 0:     /* printer device */
		draw_key(x, y, x+100, y+28, "打印机选择", 7, 0);
		break;
	    case 1:     /* paper width   */
		draw_key(x, y, x+100, y+28, "打印纸选择", 7, 0);
		break;
	}
    }

    j = 1;
    for(i=0; i<2; i++)
    {
	cur_unit.unit_x = i;
	cur_unit.unit_y = j;
	get_certain(&cur_unit);
	x = cur_unit.dot_sx+5;
	y = cur_unit.dot_sy+8;

	switch(i)
	{
	    case 0:     /* printer device */
		draw_back(x, y, x+90+64, y+28, 11);
		if(Sys_mode.prt_m == LQ1600 && Sys_mode.prtflg == OUTLIB )
		    outf(x+5, y+6, 11, BLACK,"24针打印机(无字库)");
		else if(Sys_mode.prt_m == LQ1600 && Sys_mode.prtflg == INLIB)
		    outf(x+5, y+6, 11, BLACK,"24针打印机(带字库)");
		else
		    outf(x+5, y+6, 11, BLACK," 9针打印机");
		break;
	    case 1:     /* paper width   */
		draw_back(x, y, x+90+64, y+28, 11);
		if(Sys_mode.prt_w == N_WIDTH)
		    outf(x+5, y+6, 11, BLACK,"窄行打印纸");
		else
		    outf(x+5, y+6, 11, BLACK,"宽行打印纸");
		break;
	}
    }
    return;
}

/* function  : input print configeration
 * called by : set_prn_cfg()
 * date      : 1993.11.5
 */
void input_prn_cfg(void)
{
    UI input, x, y;
    UNIT_STRUCT cur_unit;

    while(1)
    {
	input=get_key1();

	get_current(&cur_unit);

	switch(input)
	{
	    case ESC:   /* QUIT */
                save_sys_config();
		return;
	    case UP:    /* GO TO THE LAST ROW */
		move_finger(0,1);
		break;
	    case DOWN:  /* GO TO THE NEXT ROW */
		move_finger(1,1);
		break;
	    case ENTER:
		x = cur_unit.dot_sx+5;
		y = cur_unit.dot_sy+8;

		hide_finger();
		switch(cur_unit.unit_x)
		{
		     case 0:      /* printer device */
			chang_key(x, y, x+100, y+28, "打印机选择", 15);
			delay(200);
			draw_key(x, y, x+100, y+28,  "打印机选择", 7, 0);
			if(Sys_mode.prt_m == LQ1600 && Sys_mode.prtflg == OUTLIB)
			{
			     draw_back(x+131, y, x+221+64, y+28, 11);
			     Sys_mode.prt_m = LX800;
			     outf(x+136, y+6, 11, BLACK," 9针打印机");
                             Page_len = 38;
			     if(Sys_mode.prt_w == W_WIDTH)
			     {
				 Sys_mode.prt_w = N_WIDTH;
				 draw_back(x+131, y+41, x+221+64, y+41+28, 11);
				 outf(x+136, y+6+41, 11, BLACK,"窄行打印纸");
			     }
			}
			else if(Sys_mode.prt_m == LQ1600 && Sys_mode.prtflg == INLIB)
			{
			     draw_back(x+131, y, x+221+64, y+28, 11);
			     Sys_mode.prt_m = LQ1600;
			     Sys_mode.prtflg = OUTLIB;
			     outf(x+136, y+6, 11, BLACK,"24针打印机(无字库)");
                             Page_len = 72;
			}
			else
			{
			     draw_back(x+131, y, x+221+64, y+28, 11);
			     Sys_mode.prt_m = LQ1600;
			     Sys_mode.prtflg = INLIB;
			     outf(x+136, y+6, 11, BLACK,"24针打印机(带字库)");
                             Page_len = 56;
			}
			 break;
		     case 1:      /* paper width */
			chang_key(x, y, x+100, y+28, "打印纸选择", 15);
			delay(200);
			draw_key( x, y, x+100, y+28, "打印纸选择", 7, 0);
			if( (Sys_mode.prt_w == N_WIDTH ) &&\
			    (Sys_mode.prt_m == LQ1600 ) )
			{
                             ;
//			     draw_back(x+131, y, x+221+64, y+28, 11);
//			     Sys_mode.prt_w = W_WIDTH;
//			     outf(x+136, y+6, 11, BLACK,"宽行打印纸");
			}
			else
			{
			     Sys_mode.prt_w = N_WIDTH;
			     draw_back(x+131, y, x+221+64, y+28, 11);
			     outf(x+136, y+6, 11, BLACK,"窄行打印纸");
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
