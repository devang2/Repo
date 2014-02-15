/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nerror.h
  摘    要 : 本头文件中包含了NEO SDK里有关错误的处理函数、全局变量的声明及定义
  当前版本 : V2.68
  作    者 : 董凯
  完成日期 : 2006.02.07

  取代版本 : V2.66
  原 作 者 : 董凯
  完成日期 : 2006.02.03
*/

#ifndef __NERROR_H__
#define __NERROR_H__

char *g_error;

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
#define NEO_getError()   g_error
char err_method(int method);
void neo_warn(void);
void throw_error(Errinfo_t err);
FILE *FOPEN(char *filename, char *mode);

/*处理方法:该参数低3位被使用:第1位为1:发生错误则声音报警;
                             第2位为1:将错误信息写入日志;
                             第3位为1:在屏幕上显示错误信息;
  默认情况下一般错误是将method的第1,2位置1即method=3.*/

char err_method(int method)
{
   #ifndef NEO_error_unused
   static int oldmeth1 = 3;
   if (method >= 0)
   {
      oldmeth1 = method;
   }

   return oldmeth1;
   #else
   method += 0;
   #endif
}


void neo_warn(void)
{
   sound(500);
   rest(50);
   nosound();
}


/*---------------------------------------------------*
 *函数功能: 抛出一个错误,并在显示错误信息后结束程    *
 *          序.                                      *
 *参数说明: 错误代号,在本头文件中都有宏定义.         *
 *返回说明: 无.                                      *
 *备    注: 致命错误处理函数.用户可直接调用.         *
 *---------------------------------------------------*/
void throw_error(Errinfo_t err)
{
   #ifndef NEO_error_unused
   FILE *errlog_p;
   static char fir_run = TRUE;
   char method = err_method(-1);

   if (method & 1)   /*声音报警*/
   {
      neo_warn();
   }
   if (method & 2) /*写入错误日志*/
   {
      errlog_p = fopen(ERR_LOG_FILE, "a");

      if (fir_run)
      {
         struct date d;
         struct time t;
         getdate(&d);
         gettime(&t);
         fprintf(errlog_p, "\n      {ERROR LOG}\n");
         fprintf(errlog_p, "{NEO SDK V2.1.90 For DOS}\n");
         fprintf(errlog_p, "{Compile   Mode} %s\n", _COMPILE_MODE);
         fprintf(errlog_p, "{Run time} %2d:%2d:%2d %d-%d-%d\n\n", t.ti_hour, t.ti_min, t.ti_sec, d.da_year, d.da_mon, d.da_day);
         fir_run = FALSE;
      }
      #ifdef NEO_show_error_detail_used
      fprintf(errlog_p, ">>[Error from] %s()\n", err.errloc);
      fprintf(errlog_p, ">>[Error info] No.%s\n\n", g_error = err.errtag);
      #else
      fprintf(errlog_p, "Error No.%s in %s()\n\n", g_error = err.errtag, err.errloc);
      #endif
      fclose(errlog_p);
   }
   if (method & 4) /*在屏幕上显示*/
   {
      fprintf(errlog_p, "Error No.%s in %s()\n\n", g_error = err.errtag, err.errloc);
   }
   if (err.handle != 1)
   {
      exit(err.handle);
   }
   #else
   err.handle += 0;
   #endif
}

#endif
