/*NEO SDK V2.1.41 For DOS
  Copyleft Cker Home 2003-2005.

  Open Source Obey NEO_PL.TXT.
  http://neo.coderlife.net
  ckerhome@yahoo.com.cn

  文件名称 : nerror.h
  摘    要 : 本头文件中包含了NEO SDK里有关错误的处理函数、全局变量的声明及定义
  当前版本 : V2.64
  作    者 : 董凯
  完成日期 : 2005.7.2

  取代版本 : V2.61
  原 作 者 : 董凯
  完成日期 : 2005.6.22
*/

#ifndef NERROR_H
#define NERROR_H

#define FALSE 0
#define TRUE  !FALSE

/* NEO生成的可执行文件的路径深度，如果你的程序安装的路径比较深，请适当增大这个值 */
#ifndef PATH_LENGTH
#define PATH_LENGTH  80
#endif
#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#endif
#ifndef MID
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif

#ifndef ABS
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#endif
#ifndef SGN
#define SGN(x)       (((x) >= 0) ? 1 : -1)
#endif

#ifndef  NEO_error_unused
#ifndef  NEO_chinese_err_info_unused
#define  ERR_LOG_FILE       "错误日志.err"

/*用户可以在这里加上自己定义的错误信息（中文）*/
/*You can add your own error information at here(in chinese)*/
#define  N_NEO_NOT_INIT     "NEO未初始化"
#define  NO_MEMORY          "内存不足"
#define  CREAT_FILE_FALSE   "创建文件失败"
#define  NOT_INIT_ADVMOUSE  "高级鼠标未初始化"
#define  ERR_VIDEO_MODE     "不合适的显示模式"
#define  ERR_FILE_TYPE      "错误文件类型"
#define  NO_FILE            "找不到文件"
#define  NOT_INIT_MOUSE     "鼠标未安装"
#define  SOURCE_FILE_BAD    "源文件错误"
#define  DEST_FILE_BAD      "目标文件错误"
#define  WRONG_FORMAT       "错误文件格式"
#define  WRONG_SB_SETTING   "声卡配置错误"
#define  N_NOT_VESA_CARD    "非VESA标准显卡"
#define  UNDEFINED_ERROR    "未定义错误"

#else
#define  ERR_LOG_FILE       "neoerror.err"

/*用户可以在这里加上自己定义的错误信息（英文）*/
/*You can add your own error information at here(in english)*/
#define  N_NEO_NOT_INIT     "NEO not init"
#define  NO_MEMORY          "No mem"
#define  CREAT_FILE_FALSE   "Can't Creat file"
#define  NOT_INIT_ADVMOUSE  "Adv-mouse has not ready"
#define  ERR_VIDEO_MODE     "Not suitable video mode"
#define  ERR_FILE_TYPE      "Err file type"
#define  NO_FILE            "Can't find file"
#define  NOT_INIT_MOUSE     "Mouse was not installed"
#define  SOURCE_FILE_BAD    "source file bad"
#define  DEST_FILE_BAD      "dest file bad"
#define  WRONG_FORMAT       "Wrong file format"
#define  WRONG_SB_SETTING   "Wrong SB settings"
#define  N_NOT_VESA_CARD    "NOT VESA Standard VideoCard"
#define  UNDEFINED_ERROR    "Undefined error"

#endif
#endif

#ifdef __TINY__
#define COMPILE_MODE "微模式(Tiny)"
#endif

#ifdef __SMALL__
#define COMPILE_MODE "小模式(Small)"
#endif

#ifdef __MEDIUM__
#define COMPILE_MODE "中模式(Medium)"
#endif

#ifdef __COMPACT__
#define COMPILE_MODE "紧凑模式(compact)"
#endif

#ifdef __LARGE__
#define COMPILE_MODE "大模式(Large)"
#endif

#ifdef __HUGE__
#define COMPILE_MODE "巨模式(Huge)"
#endif



/*这个变量请勿修改!*/
char g_routines = 0;     /*最低位为1:处于图形模式;第2位为1:高级鼠标例程被加载;第3位为1:时钟例程被加载;第4位为1:键盘
　　　　　　　　　　　　　例程被加载;第5位为1:XMS例程被加载;第6位为1:EMS例程被加载;第7位为1:音效例程被加载*/

typedef struct Errinfo_T
{  /*错误位置*/
   char *errloc; /*出错时程序流程所在函数的函数名.*/
   /*错误备注*/
   char *errtag;
   char  handle; /*为1时程序继续执行，否则程序退出*/
}Errinfo_t;
#if defined NEO_error_unused
#define NEO_sys_report_error_unused
#endif


#define get_err_method() err_method(-1)
char err_method(int method);
void throw_error(Errinfo_t err);

/*处理方法:该参数低3位被使用:第1位为1:不处理;
                             第2位为1:将错误信息写入日志;
                             第3位为1:在屏幕上显示错误信息;
  默认情况下一般错误是将method的第1,2位置1即method=3.*/
#ifndef NEO_error_unused
char err_method(int method)
{
   static int oldmeth1 = 3;
   if (method >= 0)
   {
      oldmeth1 = method;
   }

   return oldmeth1;
}
#endif


/*---------------------------------------------------*
 *函数功能: 抛出一个错误,并在显示错误信息后结束程    *
 *          序.                                      *
 *参数说明: 错误代号,在本头文件中都有宏定义.         *
 *返回说明: 无.                                      *
 *备    注: 致命错误处理函数.用户可直接调用.         *
 *---------------------------------------------------*/
#ifndef NEO_error_unused
void throw_error(Errinfo_t err)
{
   FILE *errlog_p;
   static char fir_run;
   char method = err_method(-1);

   if (method & 2) /*写入错误日志*/
   {
      errlog_p = fopen(ERR_LOG_FILE, "a");

      if (!fir_run)
      {
         fprintf(errlog_p, "      {ERROR LOG}\n");
         fprintf(errlog_p, "{NEO SDK V2.1.41 For DOS}\n");
#ifndef MSVC15
         fprintf(errlog_p, "{Compile   Mode} : %s.\n", COMPILE_MODE);
#endif
         fprintf(errlog_p, ">>RUN START\n");
         fir_run = 10;
      }
      fprintf(errlog_p, ">>[Error   in] : %s().\n", err.errloc);
      fprintf(errlog_p, ">>[Error info] : %s.\n\n", err.errtag);
      fclose(errlog_p);
   }
   if (method & 4) /*在屏幕上显示*/
   {
      fprintf(stderr, "[Error   in] : %s().\n", err.errloc);
      fprintf(stderr, "[Error info] : %s.\n\n", err.errtag);
   }
   if (err.handle != 1)
   {
      exit(err.handle);
   }
}
#endif


#endif
