/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : ncommon.h
  摘    要 : 本头文件中包含了NEO SDK里有关公共变量及宏的声明及定义
  当前版本 : V0.10
  作    者 : 董凯
  完成日期 : 2006.02.08

  取代版本 : V0.09
  原 作 者 : 董凯
  完成日期 : 2006.02.03
*/
#ifndef __NCOMMON_H__
#define __NCOMMON_H__

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    !FALSE
#endif

#ifndef N_ERORR
#define N_ERORR 0
#endif

#ifndef N_OK
#define N_OK    !N_ERORR
#endif

#ifndef BOOL
#define BOOL    char
#endif

/*用 typedef 可能引起移植时的命名冲突，所以仍采用了宏*/
#ifndef Uint32
#define Uint32 unsigned long
#endif

#ifndef Uint16
#define Uint16 unsigned short
#endif

#ifndef Uint8
#define Uint8  unsigned char
#endif

#ifndef Int32
#define Int32 long
#endif

#ifndef Int16
#define Int16 short
#endif

#ifndef Int8
#define Int8  char
#endif

/* NEO生成的可执行文件的路径深度，如果你的程序安装的路径比较深，请适当增大这个值 */
#ifndef PATH_LENGTH
#define PATH_LENGTH  80
#endif

#ifndef NEO_MIN
#define NEO_MIN(x,y)     (((x) < (y)) ? (x) : (y))
#endif
#ifndef NEO_MAX
#define NEO_MAX(x,y)     (((x) > (y)) ? (x) : (y))
#endif
#ifndef NEO_MID
#define NEO_MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif

#ifndef NEO_ABS
#define NEO_ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#endif
#ifndef NEO_SGN
#define NEO_SGN(x)       (((x) >= 0) ? 1 : -1)
#endif

#ifndef FP_OFF
#define FP_OFF(fp)   ((unsigned)(fp))
#endif
#ifndef FP_SEG
#define FP_SEG(fp)   ((unsigned)((unsigned long)(fp) >> 16))
#endif

/*#define MASK_LINEAR(addr)     ((addr) & 0x000FFFFF)
#define RM_OFFSET(addr)       ((addr) & 0xF)
#define RM_SEGMENT(addr)      (((addr) >> 4) & 0xFFFF)*/

#ifndef  NEO_error_unused

#ifndef  NEO_just_error_code_unused
#define  ERR_LOG_FILE       "错误日志.err"
#define  N_NEO_NOT_INIT     "0"
#define  NO_MEMORY          "1"
#define  CREAT_FILE_FALSE   "2"
#define  NOT_INIT_ADVMOUSE  "3"
#define  ERR_VIDEO_MODE     "4"
#define  ERR_FILE_TYPE      "5"
#define  NO_FILE            "6"
#define  NOT_INIT_MOUSE     "7"
#define  SOURCE_FILE_BAD    "8"
#define  DEST_FILE_BAD      "9"
#define  WRONG_SB_SETTING   "10"
#define  N_NOT_VESA_CARD    "11"
#define  ROUTINE_UNACTIVE   "12"
#define  UNDEFINED_ERROR    "-1"
#define  N_MEM_MAY_OUT      "13"
#define  N_KEYB_NOT_INIT    "14"

#else

#ifndef  NEO_chinese_err_info_unused

#define  ERR_LOG_FILE       "错误日志.err"
/*You can add your own error information at here(in chinese)*/
#define  N_NEO_NOT_INIT     "0 NEO未初始化"
#define  NO_MEMORY          "1 内存不足"
#define  CREAT_FILE_FALSE   "2 创建文件失败"
#define  NOT_INIT_ADVMOUSE  "3 高级鼠标未初始化"
#define  ERR_VIDEO_MODE     "4 显示模式不当"
#define  ERR_FILE_TYPE      "5 文件类型错误"
#define  NO_FILE            "6 找不到文件"
#define  NOT_INIT_MOUSE     "7 鼠标未安装"
#define  SOURCE_FILE_BAD    "8 源文件错误"
#define  DEST_FILE_BAD      "9 目标文件错误"
#define  WRONG_SB_SETTING   "10 声卡配置错误"
#define  N_NOT_VESA_CARD    "11 非VESA标准显卡"
#define  ROUTINE_UNACTIVE   "12 功能未激活"
#define  N_MEM_MAY_OUT      "13 内存可能泄漏"
#define  N_KEYB_NOT_INIT    "14 键盘模块未安装"
#define  UNDEFINED_ERROR    "-1 未定义错误"

#else

#define  ERR_LOG_FILE       "neoerror.err"
/*You can add your own error information at here(in english)*/
#define  N_NEO_NOT_INIT     "0 NEO not init"
#define  NO_MEMORY          "1 No mem"
#define  CREAT_FILE_FALSE   "2 Can't Creat file"
#define  NOT_INIT_ADVMOUSE  "3 Adv-mouse has not ready"
#define  ERR_VIDEO_MODE     "4 Not suitable video mode"
#define  ERR_FILE_TYPE      "5 Err file type"
#define  NO_FILE            "6 Can't find file"
#define  NOT_INIT_MOUSE     "7 Mouse was not installed"
#define  SOURCE_FILE_BAD    "8 source file bad"
#define  DEST_FILE_BAD      "9 dest file bad"
#define  WRONG_SB_SETTING   "10 Wrong SB settings"
#define  N_NOT_VESA_CARD    "11 NOT VESA Standard VideoCard"
#define  ROUTINE_UNACTIVE   "12 Routine was turn off"
#define  UNDEFINED_ERROR    "-1 Undefined error"
#define  N_MEM_MAY_OUT      "13 Memory may be leak"
#define  N_KEYB_NOT_INIT    "14 Keyboard not init"

#endif  /*NEO_chinese_err_info_unused*/
#endif  /*NEO_just_err_number_used*/
#endif  /*NEO_error_unused*/

#ifdef __TINY__
#define _COMPILE_MODE "Tiny"
#endif

#ifdef __SMALL__
#define _COMPILE_MODE "Small"
#endif

#ifdef __MEDIUM__
#define _COMPILE_MODE "Medium"
#endif

#ifdef __COMPACT__
#define _COMPILE_MODE "compact"
#endif

#ifdef __LARGE__
#define _COMPILE_MODE "Large"
#endif

#ifdef __HUGE__
#define _COMPILE_MODE "Huge"
#endif

#define _mouse_installed    (g_routines & 2)
#define _timer_installed    (g_routines & 4)
#define _keyboard_installed (g_routines & 8)
#define _xms_installed      (g_routines & 16)
#define _ems_installed      (g_routines & 32)
#define _digi_installed     (g_routines & 64)

#ifdef  __cplusplus
#define __CPPARGS ...
#else
#define __CPPARGS   void
#endif

#define PACKFILE    char

#define _LEFT_ROLL  0x2
#define _RIGHT_ROLL 0x4
#define _UP_ROLL    0x8
#define _DOWN_ROLL  0x16

#define CUR_SIDE    1024

#if defined NEO_bmp_support_unused
#define NEO_showbmp_unused
#define NEO__showbmp_unused
#endif

/*==============================================这些变量请勿修改!===============================================*/
Uint16 g_routines;      /*最低位为1:处于图形模式;第2位为1:高级鼠标例程被加载;第3位为1:时钟例程被加载;第4位为1:键盘
　　　　　　　　　　　　  例程被加载;第5位为1:XMS例程被加载;第6位为1:EMS例程被加载;第7位为1:音效例程被加载*/

Uint16 common_flags = 0; /*公共状态标志：
                           D0用于判别当前系统是否使用332伪真彩调色板(0x1)
                           D1为冻结鼠标标志(0x2)
                           D2为隐藏光标标志(0x4)
                           D3为防止鼠标刷新重入标志(0x8)
                           D4为左键双击标志(0x10)
                           D5保留
                           D6为时钟重入标志
                         */

Int8  freeze_mouse_flag;
#ifndef NEO_show_bmp_unused
Int16 g_bmp_param;       /*位图操作参数，为show_bmp_ex()函数服务*/
Int32 g_bmp_flags;       /*位图操作标志，从最高位开始依次是透明(mask)显示/垂直翻转/水平翻转 标志*/
#define g_define_    (g_bmp_flags & 0x800)
#define g_wublur_    (g_bmp_flags & 0x400)
#define g_stead_     (g_bmp_flags & 0x200)
#define g_vivid_     (g_bmp_flags & 0x100)
#define g_bright_    (g_bmp_flags & 0x80)
#define g_jmplin_    (g_bmp_flags & 0x40)
#define g_roloc_     (g_bmp_flags & 0x20)
#define g_channl_    (g_bmp_flags & 0x10)
#define g_broken_    (g_bmp_flags & 0x8)
#define g_h_flip_    (g_bmp_flags & 0x4)
#define g_v_flip_    (g_bmp_flags & 0x2)
#define g_mask_flag_ (g_bmp_flags & 0x1)
#define BMP_MASK_C 0x1
#define BMP_V_FLIP 0x2
#define BMP_H_FLIP 0x4
#define BMP_MUSLIN 0x8
#define BMP_CHANNL 0x10
#define BMP_RUOLOC 0x20
#define BMP_JMPLIN 0x40
#define BMP_BRIGHT 0x80
#define BMP_VIVIDC 0x100
#define BMP_STEADM 0x200
#define BMP_WUBLUR 0x400
#define BMP_DEFINE 0x800

#endif /*NEO_show_bmp_unused*/

/*void _RETF();
void _RETF()
{
   return;
}*/

#endif
