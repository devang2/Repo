/*NEO SDK V2.1.41 For DOS
  Copyleft Cker Home 2003-2005.

  Open Source Obey NEO_PL.TXT.
  http://neo.coderlife.net
  ckerhome@yahoo.com.cn

  文件名称 : neo.h
  摘    要 : 本头文件包含了NEO SDK里有关系统初使化函数、结构、全局变量的声明及定义
  当前版本 : V0.39
  作    者 : 董凯
  完成日期 : 2005.7.2

  取代版本 : V0.37
  原 作 者 : 董凯
  完成日期 : 2004.5.16
*/

#ifndef  NEO_H
#define  NEO_H

#if 1
#define MSVC15
#endif

#ifdef __cplusplus

#ifdef MSVC15
#include <memory.h>
#define inportb(port) _asm{inb dx,port} 
#else
#include <mem.h>
#endif

#include <string.h>
#include <dir.h>
#endif

#include <conio.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

/*以下一部分是NEO默认禁用的一些功能例程，NEO禁用它们并不代表不支持这些功能
  而是出于对最后生成执行文件大小的考虑。所以用“编译开关”将一些不常用或较耗
  内存的部分例程禁用了，比如不常用的鼠标硬件光标、较占内存的NEO系统调色板、
  尚未完成的位图传输等等。当然，若你想恢复对它们的支持以便使用也是非常简单的
  你只要在#include "neo.h"之前加上开启该功能的“编译开关”。比方说如果你想写
  的不是普通图形程序而是象游戏之类的“高级”图形程序，你可能就需要播放音频文
  件的功能，而音频例程是NEO默认禁用的，你可以在#include "neo.h"前边加上启用
  音频例程的“编译开关”#define NEO_sound_used来开启对其的支持。详情可以参考
  NEO的使用文档中的“减小你的可执行文件体积”一节*/

/*NEO默认不使用绘图模式(譬如XOR_PUT等等)*/
#ifndef NEO_draw_mode_used
#define NEO_draw_mode_unused
#endif
/*NEO默认不载入NEO的系统调色板*/
#ifndef NEO_sys_pal_used
#define NEO_sys_pal_unused
#endif

#ifdef MSVC15
unsigned char inb(unsigned short port) 
{ unsigned char tmp; 
  _asm{
      push  dx;
      mov dx,port;
      xor ax,ax;
      in al,dx;
      mov tmp,al;
     }
  return tmp;
}
#define inportb(port) inb(port)
#define peekb(a,b)      (*((char far*)MK_FP((a),(b))))
#endif

/*包含所有的NEO头文件*/
#include "nerror.h" /*错误报告及抛出功能*/
#include "ndraw.h"  /*显卡硬件底层处理*/
#include "n2d.h"    /*基本图形函数*/
#include "nword.h"  /*文字输出*/

#define  NEO_VERSION       2
#define  NEO_BETA          1
#define  NEO_SUB_VERSION   41
#define  NEO_VERSION_STR   "2.1.41"
#define  NEO_DATE_STR      "2005/07/02"
                           /*yyyymmdd*/
#define  NEO_DATE          20050702L

extern char neo_id[] = NEO_VERSION_STR;
extern char neo_date[] = NEO_DATE_STR;

char neo_init(void);
void neo_exit(void);


char neo_init(void)
{
   union  REGS  in,out;
   struct SREGS segs;
   struct vbe_info_t vbe_info;
   #ifndef NEO_palette_unused
   RGB24  black_rbg = {0,0,0};
   int    i;
   #endif
   char far *vbeinfo = (char far *)&vbe_info;

   in.x.ax = 0x4F00;
   in.x.di = FP_OFF(vbeinfo);
   segs.es = FP_SEG(vbeinfo);
   int86x(0x10, &in, &out, &segs);

   if (stricmp(vbe_info.VESA_signature, "VESA") != 0)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"neo_init", N_NOT_VESA_CARD, 1};
      throw_error(error);
      #endif    
      return (g_vbe_sub_ver = -1);      
   }
   g_vbe_version = vbe_info.VBE_version >> 8;
   g_vbe_sub_ver = vbe_info.VBE_version & 0xF;
   g_total_vram  = vbe_info.total_memory;
   #ifndef NEO_palette_unused
   for (i=0; i<256; i++)
      g_black_palette[i] = black_rbg;
   #endif

   atexit(neo_exit);
   mkdir("neotemp");
   return 0;
}


void neo_exit(void)
{
   #ifndef NEO_palette_unused
   set_dac_size(6);
   #endif

   set_vbe_mode(3);
   
   /*puts("\nExiting.Please wait...");*/
   fcloseall();
   system("del NEOTEMP\\*.$$$");
   unlink("c:\\neo.vbs");
}
/*函数功能: 与硬件条件无关的高级延时函数.            *
 *参数说明: delay: 延时量          *
 *返回说明: 无.                                    *
 *备    注: 由于此函数能产生与具体硬件条件无关的时间 *　
 *          间隔,因此是库函数delay()的理想替代者.    */
void adv_delay(long delay)
{
   unsigned int far *clock = (unsigned int far *)0x0000046CL;
   unsigned int now;
   now = *clock;
   while (abs(*clock - now) < delay);
}   
#endif
