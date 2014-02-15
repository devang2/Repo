#include <dos.h>		/* geninterrupt MK_FP en/disable */
#include  <graphics.h>

#include  <def.inc>
#include  <key.inc>
#include  <func.inc>

#define DEFAULT_FREQENCE	450;
#define DEFAULT_CYCLE		5;

void 	interrupt far (*old_timer)(void);

UL far 	*Tick_cnt_ptr = (UL far *)0x0040006cL;
UL 	Seconds = 0;
UC      Sec_chg = FALSE;

static  UC Timer_load = FALSE;

static  UC Bell = FALSE;
static  UI Bell_cycle = DEFAULT_CYCLE;
static  UI Freqence = DEFAULT_FREQENCE;
static  UC Have_sound;
static  UC Bell_count;

static  UC Flash_set = FALSE;
static  UC Flash = FALSE;
static  UI Pos_x, Pos_y;
static  UI Bk_color, Finger_color;
static  UI Flash_cycle;
static	UC Have_finger;
static  UC Flash_count;

/* ============================= set_bell() ============================ */
void set_bell(UI freqence, UC cycle)
{
	if (freqence!=0)
		Freqence = freqence;

	if (cycle!=0)
		Bell_cycle = cycle;

	sound(Freqence);  			/* begin sound */
	Have_sound = TRUE;
	Bell_count = 0;

	Bell = TRUE;
}

/* ============================ reset_bell() =========================== */
void reset_bell(void)
{
	Freqence = DEFAULT_FREQENCE;
	Bell_cycle = DEFAULT_CYCLE;
	nosound();
	Bell = FALSE;
}

/* =========================== flash_cursor() ========================== */
void set_flash(UI x, UI y, UC bk_color, UC finger_color, UC cycle)
{
	Pos_x = x;
	Pos_y = y;
	Bk_color = bk_color;
	Finger_color = finger_color;
	Flash_cycle = cycle;

	h_finger(Pos_x, Pos_y, Finger_color);
	Flash_count = 0;
	Have_finger = TRUE;
	Flash_set = TRUE;
	Flash = TRUE;
}

/* =========================== reset_flash() ========================== */
void reset_flash(void)
{
	Flash_set = FALSE;
	Flash = FALSE;
	h_finger(Pos_x, Pos_y, Bk_color);
}

/* ========================== reflash_finger() ========================= */
void flash_continue(void)
{
	if (Flash_set)
		Flash = TRUE;
}

/* ========================== noflash_finger() ========================= */
void flash_pause(void)
{
	Flash = FALSE;
}


/* ============================== TIMER =============================== */
void interrupt far timer()
{
UL tmp_seconds;
static UL old_tick_cnt = 0;

	(*old_timer)();
	disable();

	if (Bell)
	{
		if (Have_sound)
		{
			if ( ++Bell_count==Bell_cycle )
			{
				nosound();
				Bell_count = 0;
				Have_sound = FALSE;
			}
		}
		else
		{
			if ( ++Bell_count==Bell_cycle )
			{
				sound(Freqence);
				Bell_count = 0;
				Have_sound = TRUE;
			}
		}
	}

	if (Flash)
	{
		if (Have_finger)
		{
			if ( ++Flash_count==Flash_cycle )
			{
				h_finger(Pos_x, Pos_y, Bk_color);
				Flash_count = 0;
				Have_finger = FALSE;
			}
		}
		else
		{
			if ( ++Flash_count==Flash_cycle )
			{
				h_finger(Pos_x, Pos_y, Finger_color);
				Flash_count = 0;
				Have_finger = TRUE;
			}
		}
	}			/* end of if (Flash) */

	/* BIO calender stimulates 18.2 times in a second, and Tick_cnt_ptr
	 * is the pointer to BIO calender, so if(....)<17, then it is not
	 * 1 second, and not change the time on the main menu
	 */
	if ( (*Tick_cnt_ptr-old_tick_cnt)<17 )
	{
		enable();
		return;
	}
	/* almost gains 1 second                   */
	tmp_seconds = (*Tick_cnt_ptr<<11)/37287U;
	if ( tmp_seconds==Seconds)
	{
		enable();
		return;
	}
	Seconds = tmp_seconds;
	old_tick_cnt = *Tick_cnt_ptr;
	Sec_chg = TRUE;
	enable();
}
/*
 ========================== Justify_watch() ===========================
justify_watch()
{
static UC t_str[9] = {0x20,0x20,':',0x20,0x20,':',0x20,0x20,0};
static UI minutes = 0;
struct fillsettingstype old_setting;
UI minute;
UC sec,min,hour;

	sec = Seconds%60;
	minute = Seconds/60;

	t_str[6] = sec/10+0x30;
	t_str[7] = sec%10+0x30;

	getfillsettings(&old_setting);
	setfillstyle(SOLID_FILL,0);
	if (minute==minutes)
	{
		bar(593,442,610,458);     		    610 : 608+2
		hz16_disp(593,442,t_str+6,7);
	}
	else
	{
		minutes = minute;
		min = minute%60;
		t_str[3] = min/10+0x30;
		t_str[4] = min%10+0x30;
		hour = minute/60;
		t_str[0] = hour/10+0x30;
		t_str[1] = hour%10+0x30;
		bar(542,442,610,458);	                542: 545-3
		hz16_disp(545,442,t_str,7);
	}
	setfillstyle(old_setting.pattern,old_setting.color);
}
*/
/* ============================= load_timer ============================== */
void load_timer(void)
{
	if (!Timer_load)
	{
		old_timer = getvect(0x08);
		setvect(0x08,timer);
		Timer_load = TRUE;
	}
}

/* ============================ unload_timer ============================= */
void unload_timer(void)
{
	if (Timer_load)
	{
		setvect(0x08,old_timer);
		Timer_load = FALSE;
	}
}

