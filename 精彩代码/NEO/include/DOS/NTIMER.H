/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : ntimer.h
  摘    要 : 本头文件包含了NEO SDK里有关时钟部分函数、结构、全局变量的声明及定义
  当前版本 : V2.81
  作    者 : 董凯
  完成日期 : 2007.1.18

  取代版本 : V2.80
  原 作 者 : 董凯
  完成日期 : 2006.12.18
*/

#ifndef  __NTIMER_H__
#define  __NTIMER_H__

#define  TIMERS_PER_SECOND     1193180L

/*时钟类型定义*/
struct sub_timer_t
{
   int    cycle;
   Uint16 timer;
   void   (*proc)();
   struct sub_timer_t *next;
}g_sub_timer;

typedef void (*T_func_ptr)();
extern int mouse_refresh(void);

int _READ(int fp, void *buf, int count);
int _FREAD(void *buf, int size, int count, FILE *fp);
int _FWRITE(void *buf, int size, int count, FILE *fp);
void timer_store(void);
void timer_restore(void);

#ifndef  NEO_timer_unused

volatile long g_time_counter = 0;
int      g_timer_fre = 18;
char     i_love_bill;
char     NEO_TIMER = 0x1c;
char     g_timers; /*加载子时钟例程的总数*/

void     interrupt (*g_old_1ch_int)(__CPPARGS);
void     interrupt (*g_new_1ch_int)(__CPPARGS) = 0L;
void     g_temp_func();
void     time_counter(void);
void     interrupt new_1ch_int(__CPPARGS);
long     _clock(void);
void     install_timer(void interrupt (*intfunc)(__CPPARGS));
void     change_timer(unsigned hz);
void     remove_timer(void);
int      install_int(void (*func)(), int speed);
int      install_int_ex(void (*func)(), int speed);
void     remove_int(void (*func)());
void     rest_callback(Uint16 n_delay, void (*callback)());

#define  _CLK_TCK         (double)g_timer_fre
#define  _install_timer() install_timer(new_1ch_int)
#define  neo_clock()      (_clock() / _CLK_TCK)
#define  clock_reset()
#define  LOOP_END_OF_TIMER()
#define  read_timer()     g_timer_fre
#define  NEO_SYS_TIMER    new_1ch_int
#define  NEO_USER_TIMER   g_new_1ch_int
#define  use_virtual_timer() (NEO_TIMER = 0x28)
#define  use_hard_timer()    (NEO_TIMER = 0x1c)
#define  is_virtual_timer()  (NEO_TIMER == 0x28 ? TRUE : FALSE)
#define  SECS_TO_TIMER(secs) (g_timer_fre * secs)
#define  BPS_TO_TIMER(bps)   (g_timer_fre / bps)
#define  BPM_TO_TIMER(bpm)   (g_timer_fre * 60 / bpm)

/*int is_dos_free(void)
{
   int far *tem;
   _AH = 0x34;
   __int__(0x21);
   tem = MK_FP(_ES, _BX);
   return *tem;
}*/

void time_counter(void)
{
   g_time_counter = (g_time_counter >= 0 ? g_time_counter + 1 : 0); /*累加时钟计算变量*/
}


void interrupt new_1ch_int(__CPPARGS)
{
   struct sub_timer_t *tp;

   disable();
   if (common_flags & 0x40) /*检查NEO时钟忙标志位*/
   {
      return;
   }
   else
   {
      common_flags |= 0x40; /*置位NEO时钟忙标志*/
   } 
   tp = &g_sub_timer;
   while (tp != NULL)
   {
      if (tp->cycle > 0) /*触发周期大于0方才执行*/
      {
         if (++tp->timer >= tp->cycle)
         {
            #ifndef NEO_mouse_adv_unused
            if (tp->proc == (void (*)())mouse_refresh)
            {
               if (!(common_flags & 0x8)) /*避免高级鼠标模块重入*/
                  (*tp->proc)();
            }
            else
            #endif
            {
               (*tp->proc)();
            }

            tp->timer = 0;
         }
      }

      tp = tp->next; /*处理下一个子时钟例程*/
   }

   if (g_new_1ch_int) /*兼容老版本中的统一时钟子例程*/
      {g_new_1ch_int();}
   else
      {g_new_1ch_int = g_old_1ch_int;}
   
   common_flags &= (~0x40); /*清零时钟忙标志位(0xFFBF)*/
   enable();
}

long _clock(void)
{
   if (_timer_installed)
   {
      return g_time_counter;
   }
   else
   {
      return clock();
   }
}



void install_timer(void interrupt (*intfunc)(__CPPARGS))
{
   if (!_timer_installed)
   {
      g_time_counter = 0;
      g_timer_fre  = 18;
      g_old_1ch_int= getvect(NEO_TIMER);

      /*将第一个时钟处理列表程序设置为计算器*/
      g_sub_timer.cycle = 1;
      g_sub_timer.timer = 0;
      g_sub_timer.proc = (void (*)())time_counter;

      disable();
      setvect(NEO_TIMER, intfunc);
      enable();
      g_routines |= 4;
      if (i_love_bill)
      {
         i_love_bill = FALSE; /*将i_love_bill设置为假以调用change_timer()函数*/
         change_timer(200);   /*开启WINDOWS友好模式，用以兼容WIN 3.1及WIN 95等低版本的WINDOWS系统*/
         i_love_bill = TRUE;  /*恢复i_love_bill为真*/
      }
   }
}


void change_timer(unsigned hz)
{
   unsigned cnt = (unsigned)(TIMERS_PER_SECOND / hz);

   if (hz >= 18 && (!i_love_bill))
   {
      g_timer_fre = hz;
      outportb(0x43, 0x3c);
      outportb(0x40, cnt & 0x00ff);
      outportb(0x40, (cnt >> 8) & 0x00ff);
   }
}


void remove_timer(void)
{
   timer_store(); /*恢复时间频率*/

   disable();
   setvect(NEO_TIMER, g_old_1ch_int); /*恢复系统中断程序*/
   enable();
   g_routines &= 251; /*恢复标志*/
   #undef NEO_timer_install
}


void rest_callback(Uint16 n_delay, void (*callback)())
{
   if (_timer_installed)
   {
      double time_differ = neo_clock();
      while((neo_clock() - time_differ) * 1000 < n_delay)
      {
         if (callback)
            (*callback)();
      }
   }
   else
   {
      delay(n_delay);
   }
}

int  install_int(void (*func)(), int speed)
{
   struct sub_timer_t *add = (struct sub_timer_t *)malloc(sizeof(struct sub_timer_t));
   struct sub_timer_t *tp;

   if (add == NULL) return -1;

   add->proc = func;
   add->cycle= speed;
   add->timer= 0;

   tp = g_sub_timer.next;
   g_sub_timer.next = add;
   add->next = tp;

   return ++g_timers;
}


int  install_int_ex(void (*func)(), int speed)
{
   struct sub_timer_t *tp = &g_sub_timer;
   struct sub_timer_t *temp;

   while (tp != NULL)
   {
      temp = tp->next;
      if(temp->proc == func) /*若该函数已经加载则调整其调用速度*/
      {
         temp->cycle = speed;
         return 0;
      }
      tp = tp->next;
   }
   return install_int(func, speed);
}


/*删除时钟*/
void remove_int(void (*func)())
{
   struct sub_timer_t *t1, *t2;
   t1 = t2 = &g_sub_timer;

   while(t1 != NULL)
   {
      t1 = t1->next;
      if(t1->proc == func)
      {
         t2->next = t1->next;
         free(t1);
         --g_timers;
         break;
      }
      t2 = t1;
   }
}

#define rest(delay)  rest_callback(delay, NULL)

#endif

void timer_store(void)
{
   #ifndef  NEO_timer_unused
   if (_timer_installed)
   {
      outportb(0x43, 0x3c);
      outportb(0x40, 0xffff & 0x00ff);
      outportb(0x40, (0xffff >> 8) & 0x00ff); /*恢复系统默认时钟触发频率*/
   }
   #else
   /*#ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"Timer", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif*/

   #endif
}


void timer_restore(void)
{
   #ifndef  NEO_timer_unused
   if (_timer_installed)
   {
      change_timer(g_timer_fre);
   }
   #endif
}


int _READ(int fp, void *buf, int count)
{
   int retrn;
   #ifndef NEO_timer_adapt_unused
   timer_store();
   #endif

   retrn = _read(fp, buf, count);

   #ifndef NEO_timer_adapt_unused
   timer_restore();
   #endif
   return retrn;
}

int _FREAD(void *buf, int size, int count, FILE *fp)
{
   int retrn;
   #ifndef NEO_timer_adapt_unused
   timer_store();
   #endif

   retrn = fread(buf, size, count, fp);

   #ifndef NEO_timer_adapt_unused
   timer_restore();
   #endif
   return retrn;
}


int _FWRITE(void *buf, int size, int count, FILE *fp)
{
   int retrn;
   #ifndef NEO_timer_adapt_unused
   timer_store();
   #endif

   retrn = fwrite(buf, size, count, fp);

   #ifndef NEO_timer_adapt_unused
   timer_restore();
   #endif
   return retrn;
}

#endif
