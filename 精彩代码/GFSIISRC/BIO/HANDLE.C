#include <dos.h>
#include <process.h>

#include <def.inc>
#include <func.inc>
#include <menufunc.inc>

#define	IGNORE 0
#define	RETRY 1
#define ABORT 2

static char *infor[14] =
		{ "Ａ驱动器错误：目标盘已被写保护！",
		  "Ａ驱动器错误：非法的驱动器指定！",
		  "Ａ驱动器错误：驱动器未准备好！",
		  "软件错误：错误的设备驱动命令！",
		  "Ａ驱动器错误：磁盘数据校验错误！",
		  "软件错误：设备驱动命令格式错误！",
		  "Ａ驱动器错误：磁盘寻道错误！",
		  "Ａ驱动器错误：未知的磁盘介质！",
		  "Ａ驱动器错误：扇区未找到！",
		  "打印机错误：打印机无纸！",
		  "Ａ驱动器错误：一般性写错误！",
		  "Ａ驱动器错误：一般性读错误！",
		  "Ａ驱动器错误：一般性磁盘错误！",
		  "软件内部错误：建议重新启动计算机！"
		};
static UC *item[3] =
		{
		  "  重试  ",
		  "  忽略  ",
		  "  返回  "
		};
extern UC Cursor_on;

int  handler(int Err_code, int keep_ax, int keep_bp, int keep_si)
{
static old_cursor_stat;
UC drive;
UC answer;

	Err_code &= 0x00ff;
	if (Err_code>0x0C)
		Err_code = 0x0D;

	if ( keep_ax>0 )
	{
		drive = keep_ax&0x00FF;
		switch(Err_code)
		{
		case 3:
		case 5:
		case 9:
		case 13:
			break;
		default:
			infor[Err_code][1] = drive+193;
		}
	}

	old_cursor_stat = Cursor_on;
	cursor_off();
	flash_pause();
	set_bell(950, 4);
	answer = t_menu(3, infor[Err_code], item);
	reset_bell();
	flash_continue();
	if (old_cursor_stat==TRUE)
		cursor_on();

	switch(answer)
	{
	case 1:
		hardresume(RETRY);
	case 2:
		hardresume(IGNORE);
	default:
		hardretn(-1);
	}
	return 1;   /*This is added by Yu WeiHua to avoid compiling error.*/
}

