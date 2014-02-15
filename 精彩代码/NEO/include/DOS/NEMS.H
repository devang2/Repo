/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nmem.h
  摘    要 : 本头文件包含了NEO SDK里有关扩充内存操作的各种函数、结构、全局
             变量的声明及定义
  当前版本 : V0.61
  作    者 : 董凯 
  完成日期 : 2005.12.12

  取代版本 : V0.61
  原 作 者 : 董凯 
  完成日期 : 2005.12.12
*/

#ifndef __NEMS_H__
#define __NEMS_H__

#ifndef NEO_ems_unused

#define   EMS   0x67
char far *g_ems_base;

char ems_init(void);
unsigned ems_avail(void);
long ems_alloc(unsigned pages);
int  ems_map(unsigned handle, int physical_page, int logical_page);
void ems_move(int page, void *str, int n);
void ems_get(int page, void *str, int n);
char map_save(unsigned handle);
char map_load(unsigned handle);
char get_handle_nums(void);
int  get_handle_pages(unsigned handle);
char ems_free(unsigned handle);


char ems_init(void)
{
   /*通过尝试打开文件emsXXXX0来测试扩充内存是否存在*/
   union REGS regs;
   struct SREGS sregs;
   unsigned handle;
   int error;

   /*尝试打开驱动文件emsXXXX0*/
   regs.x.ax = 0x3d00;
   regs.x.dx = (int)"emsXXXX0";
   sregs.ds = _DS;
   intdosx(&regs,&regs,&sregs);
   handle = regs.x.ax;
   error = regs.x.cflag;
   if (!error)
   {
      regs.h.ah = 0x3e;
      regs.x.bx = handle;
      intdos(&regs,&regs);
      /*g_routines |= 32;*/
   }
   /*检查扩充内存管理程序是否正确响应*/
   regs.h.ah = 0x40;
   int86(EMS, &regs, &regs);
   if (regs.h.ah)
      return FALSE;
   /*取得物理映射页面的段地址*/
   regs.h.ah = 0x41;
   int86(EMS, &regs, &regs);
   if (regs.h.ah)
      return FALSE;

   g_ems_base = (char far *)MK_FP(regs.x.bx, 0);
   g_routines |= 32;
   return TRUE;
}


unsigned ems_avail(void)
{
   /*成功返回可用扩充内存的页数(每页16KB);错误返回FALSE*/
   _AH = 0x42;
   __int__(EMS);

   return _AH? FALSE : _BX;
}


long ems_alloc(unsigned pages)
{
   /*申请指定页数的扩充内存,成功返回被分配扩充内存的操作句柄;否则返回-1*/
   _AH = 0x43;
   _BX = pages;
   __int__(EMS);

   return _AH?-1:/*regs.x.dx*/_DX;
}


int ems_map(unsigned handle, int physical_page, int logical_page)
{
  /*映射一块从扩充内存到基本内存的物理页,用来实现扩充内存和基本内存之间的数据交换*/

   if (physical_page >=0 && physical_page < 4)
   {
      _AH = 0x44;
      _AL = physical_page;
      _BX = logical_page;
      _DX = handle;
      __int__(EMS);
      return (_AH == 0);
   }
   else
      return FALSE;
}


void ems_move(int page, void *str, int n)
{
   /*从常规内存拷贝指定字节数的数据到扩充内存*/
   if (n <= 16384)
   {
      char far *ptr;

      ptr = g_ems_base + page * 16384;
      movedata(FP_SEG(str), FP_OFF(str), FP_SEG(ptr), FP_OFF(ptr), n);
   }
}


void ems_get(int page, void *str, int n)
{
   /*从扩充内存拷贝指定字节数的数据到常规内存*/
   if (n <= 16384)
   {
      char far *ptr;

      ptr = g_ems_base + page * 16384;
      movedata(FP_SEG(ptr), FP_OFF(ptr), FP_SEG(str), FP_OFF(str), n);
   }
}


/*一些TSR, ISR或设备驱动程序要使用EMS, 必须在操作前保存映射关系并在完成后恢复*/
char map_save(unsigned handle)
{
   _AH = 0x47;
   _DX = handle;
   __int__(EMS);

   return _AH? _AH : 0;
}


char map_load(unsigned handle)
{
   _AH = 0x48;
   _DX = handle;
   __int__(EMS);

   return _AH? _AH : 0;
}


char get_handle_nums(void)
{
   _AH = 0x4b;
   __int__(EMS);
   /*返回句柄数0~255*/
   return _AH? -1:_BX;
}


int get_handle_pages(unsigned handle)
{

   _AH = 0x4c;
   _DX = handle;
   __int__(EMS);

   return _AH? -1:_BX;
}


char ems_free(unsigned handle)
{
   /*释放以handle句柄指向的扩充扩充内存*/

   _AH = 0x45;
   _DX = handle;
   __int__(EMS);
   g_routines &= 223;
   return _AH?FALSE : TRUE;
}
#endif

#endif
