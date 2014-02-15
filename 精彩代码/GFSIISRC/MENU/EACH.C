#include <string.h>
#include <stdlib.h>

#include <def.inc>
#include <func.inc>
#include <helpfunc.inc>
#include <menufunc.inc>

extern UI Menu_path;
extern UC Privi;
extern UC Opt_no;
static UC *common_buf = (UC far *)0x004000F0L;

/* ---------------------------------------------------------------------- */
void default_process(void)
{
UC msg[80];
UC path[6];

	strcpy(msg, "本模块尚未做完, 其调用入口路经是: ");
	ultoa(Menu_path, path, 10);
	strcat(msg, path);
	strcat(msg, ".\n请按鼠标或任意键返回···");
	sound_alarm();
	msg_win(208, 348, "", msg, 0);
}

/* ---------------------------------------------------------------------- */
void no_such_privi(UC need_privi)
{
UC msg[80];

	strcpy(msg, "本操作需在系统");
	if (need_privi==1)
		strcat(msg, "设定权限下才能执行。\n请按鼠标或任意键返回···");
	else
		strcat(msg, "管理权限下才能执行。\n请按鼠标或任意键返回···");
	sound_alarm();
	msg_win(208, 348, "", msg, 0);
}

/* --------------------------------------------------------------------
	Byte 	Meaning				Byte	Meaning
	0 	Privilege
---------------------------------------------------------------------- */
void set_global_data()
{
UC dat_buf[0x10];

	memset(dat_buf, 0, 0x10);
	dat_buf[0] = Privi;
	dat_buf[1] = Opt_no;

	memcpy(common_buf, dat_buf, 0x10);
}

void get_global_data()
{
UC dat_buf[0x0F];

	memcpy(dat_buf, common_buf, 0x10);
	Privi = dat_buf[0];
	Opt_no = dat_buf[1];
}

