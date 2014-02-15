/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : neo.h
  摘    要 : 本头文件包含了NEO SDK里有关系统初使化函数、结构、全局变量的声明及定义
  当前版本 : V0.50
  作    者 : 董凯
  完成日期 : 2007.08.17

  取代版本 : V0.48
  原 作 者 : 董凯
  完成日期 : 2006.02.01
*/

#ifndef  __NEO_H__
#define  __NEO_H__

#ifndef  NEO_H
#define  NEO_H
#endif

#ifdef __cplusplus
/*语言平台相关代码*/
#endif

#include <conio.h>
#include <dir.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <math.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/*以下一部分是NEO默认禁用的一些功能例程，NEO禁用它们并不代表不支持这些功能
  而是出于对最后生成执行文件大小的考虑。所以用“编译开关”将一些不常用或较耗
  内存的部分功能禁用了，比如不常用的鼠标扩展功能、较占内存的NEO系统调色板、
  尚未完成的音频播放等等。当然，若你想恢复对它们的支持以便使用也是非常简单的
  你只要在#include <neo.h>之前加上开启该功能的“编译开关”。比方说如果你想写
  的不是普通图形程序而是象游戏之类的“高级”图形程序，你可能就需要播放音频文
  件的功能，而音频例程是NEO默认禁用的，你可以在#include "neo.h"前边加上启用
  音频例程的“编译开关”#define NEO_sound_used来开启对其的支持。更多的编译开关
  可以参考NEO说明文档中的“减小你的可执行文件体积”一节*/

/*NEO默认不使用绘图模式(譬如XOR_PUT等等)*/
#ifndef NEO_draw_mode_used
#define NEO_draw_mode_unused
#endif
/*NEO默认不载入NEO的系统调色板*/
#ifndef NEO_sys_pal_used
#define NEO_sys_pal_unused
#endif
/*默认停用8位色下的淡出淡入例程*/
#ifndef NEO_fade_pal_used
#define NEO_fade_pal_unused
#endif
/*NEO默认不使用鼠标扩展功能函数*/
#ifndef NEO_mouse_exp_used
#define NEO_mouse_exp_unused
#endif
/*默认不使用XMS扩展内存函数*/
#ifndef NEO_xms_used
#define NEO_xms_unused
#endif
/*默认不使用UMB内存函数*/
#ifndef NEO_umb_used
#define NEO_umb_unused
#endif
/*默认不使用EMS扩充内存函数*/
#ifndef NEO_ems_used
#define NEO_ems_unused
#endif
/*默认不使用音频播放*/
#ifndef NEO_sound_used
#define NEO_sound_unused
#endif
/*默认不使用硬件滚屏系统*/
#ifndef NEO_scroll_used
#define NEO_scroll_unused
#endif
/*默认不使用时钟自适应功能*/
#ifndef NEO_timer_adapt_used
#define NEO_timer_adapt_unused
#endif
/*默认不使用时钟自适应功能*/
#ifndef NEO_other_used
#define NEO_other_unused
#endif

/*默认不使用全部的特效*/
#ifdef  NEO_bmp_all_effect_used
#define NEO_bmp_muslin_used
#define NEO_bmp_channel_used
#define NEO_bmp_roloc_used
#define NEO_bmp_jmp_lin_used
#define NEO_bmp_bright_used
#define NEO_bmp_vivid_used
#define NEO_bmp_stead_used
#define NEO_bmp_wublur_used
#endif
/*默认不使用位图缕纱效果*/
#ifndef NEO_bmp_muslin_used
#define NEO_bmp_muslin_unused
#endif
/*默认不使用位图通道效果*/
#ifndef NEO_bmp_channel_used
#define NEO_bmp_channel_unused
#endif
/*默认不使用位图反色效果*/
#ifndef NEO_bmp_roloc_used
#define NEO_bmp_roloc_unused
#endif
/*默认不使用隔行扫描效果*/
#ifndef NEO_bmp_jmp_lin_used
#define NEO_bmp_jmp_lin_unused
#endif
/*默认不使用亮度调整功能*/
#ifndef NEO_bmp_bright_used
#define NEO_bmp_bright_unused
#endif
/*默认不使用颜色加强效果*/
#ifndef NEO_bmp_vivid_used
#define NEO_bmp_vivid_unused
#endif
/*默认不使用颜色替换功能*/
#ifndef NEO_bmp_stead_used
#define NEO_bmp_stead_unused
#endif
/*默认不使用位图模糊效果*/
#ifndef NEO_bmp_wublur_used
#define NEO_bmp_wublur_unused
#endif

/*包含所有的NEO头文件*/
#include "ncommon.h"/* 公共变量 */
#include "ntimer.h" /* 时钟模块 */
#include "nerror.h" /* 错误报告及抛出功能 */
#include "nvesa.h"  /* 显卡硬件底层处理   */
#include "ncolor.h" /* 调色板处理   */
#include "ndraw.h"  /* 基本图形函数 */
#include "nxms.h"   /* 扩展内存操作 */
#include "nems.h"   /* 扩充内存操作 */
#include "neffect.h"/* 图形显示特效 */
#include "nbmp.h"   /* 位图文件处理 */
#include "nicon.h"  /* 光标文件处理 */
#include "nscroll.h"/* 滚屏处理 */
#include "ninput.h" /* 键盘、鼠标   */
#include "nword.h"  /* 文字输出 */

#include "nsound.h" /*音频播放 */
#include "nother.h" /*附加功能 */

#define  NEO_VERSION       3
#define  NEO_BETA          1
#define  NEO_SUB_VERSION   0
#define  NEO_VERSION_STR   "3.1.00 +"
#define  NEO_DATE_STR      "2007/8/17"
                           /*yyyymmdd*/
#define  NEO_DATE          20070817L

extern char neo_id[] = NEO_VERSION_STR;
extern char neo_date[] = NEO_DATE_STR;

char neo_init(void);
void neo_exit(void);


char neo_init(void)
{
   union  REGS  in,out;
   struct SREGS segs;
   struct vbe_info_t vbe_info;
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

   atexit(neo_exit);
   #ifndef NEO_temp_dir_unused
   mkdir("neotemp");
   #endif
   return 0;
}


void neo_exit(void)
{
   /*char near *memzero = (int near *)0X0;*/

   #ifndef NEO_color_depth_8_unused
   #ifndef NEO_palette_unused
   set_dac_size(6);
   #endif
   #endif

   /*set_vbe_mode(3);*/
   #ifndef NEO_mouse_adv_unused
   if (_mouse_installed)
   {
      remove_mouse();
   }
   #endif
   #ifndef NEO_timer_unused
   if (_timer_installed)
   {
      remove_timer();
   }
   #endif
   #ifndef NEO_keyboard_unused
   if (_keyboard_installed)
   {
      remove_keyboard();
   }
   #endif
   #ifndef NEO_xms_unused
   if (_xms_installed)
   {
      xms_kill();
   }
   #endif
   #ifndef NEO_sound_unused
   if (_digi_installed)
   {
      remove_sound();
   }
   #endif

   /*进行收尾工作*/
   fcloseall();
   set_vga_mode(0x3);

   printf("Clear temp file...\n");
   #ifndef NEO_temp_dir_unused
   system("del NEOTEMP\\*.$$$");
   #else
   system("del *.$$$");
   #endif
   system("cls");

   /*检查用户程序在运行过程中是否发生内存泄漏*/
   /*if (*memzero)  
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"neo_exit", N_MEM_MAY_OUT, 0};
      throw_error(error);
      #endif
   }*/
}
   
#endif
