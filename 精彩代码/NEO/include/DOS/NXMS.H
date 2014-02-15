/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nmem.h
  摘    要 : 本头文件包含了NEO SDK里有关扩展内存操作的各种函数、结构、全局
             变量的声明及定义
  当前版本 : V0.73
  作    者 : 董凯 
  完成日期 : 2006.03.16

  取代版本 : V0.69
  原 作 者 : 董凯 
  完成日期 : 2006.01.20
*/

#ifndef __NXMS_H__
#define __NXMS_H__

#ifndef NEO_xms_unused

#if defined (__TINY__) || (__HUGE__)
#ifndef NEO_xms_unused
#error nmem.h will can not used in this compile model!
#endif
#endif

#ifndef XMS_SWAB_BLOCK

#ifndef MIN_BLOCK_SIZE
#define MIN_BLOCK_SIZE 0x4000  /* 16L * 1024L = 16KB */
#endif

#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 0xffff  /* 64L * 1024L = 64KB */
#endif

/*我们将在常规内存中申请XMS_SWAB_BLOCK大小的缓冲区用来与扩展内存进行数据交换
  它的大小必须是1024的倍数,因为xms管理程序是以1K为单位分配扩展内存的.*/
#if defined (__SMALL__) || (__MEDIUM__)
#define XMS_SWAB_BLOCK MIN_BLOCK_SIZE
#endif
     
#if defined (__COMPACT__) || (__LARGE__)
#define XMS_SWAB_BLOCK MAX_BLOCK_SIZE
#define HIGH_COMPILE_MODE  1
#endif

#endif /*XMS_SWAB_BLOCK*/

/*g_xms_handle_t是定义用来实现常规内存与扩展内存之间数据交换的结构体.*/
typedef struct xms_handle_t
{
   /*blocklength是常规内存与扩展内存之间要传递数据块的长度(单位:字节)*/
   unsigned long blocklength;
     
   /*sourcehandle是数据传递时源内存块的操作句柄:
     若为0 : 表示下面定义的sourceptr是一个16:16的常规内存指针;
     否则sourceptr是从 sourcehandle所指向的扩展内存缓冲区起始处开始的偏移量.*/
   unsigned int sourcehandle;
   void far *sourceptr;
     
   /*desthandle是数据传递时目的内存块的操作句柄:
     若为0 :  表示下面定义的destptr是一个16:16的常规内存指针;
     否则destptr是从 desthandle所指向的扩展内存缓冲区起始处开始的偏移量.*/
   unsigned int desthandle;
   void far *destptr;
}xms_handle_t;
xms_handle_t g_xms_handle_t;
     
void far (*xms_func)(void) = 0L; /*用来调用扩展内存管理程序(himem.sys)*/
char *g_xms_buf = NULL;          /*常规内存中用来与扩展内存之间进行数据交换的交换页*/
/*#define  NEO_xms_used   TRUE*/
#define  XMS_SWAB_PTR   g_xms_buf
#define  xms_swab_del() { \
   if (g_xms_buf)         \
   free(g_xms_buf);       \
   g_xms_buf = NULL;      \
}
#define  xms_swab_rem() g_xms_buf = (char *)malloc(XMS_SWAB_BLOCK)
#define  xms_swab_new(size) g_xms_buf = (char *)malloc(size)
#endif

/*char xms_init(void);
int  xms_avail_block(int flag);
unsigned xms_alloc(unsigned long bytes);
char xms_realloc(unsigned handle, unsigned resize);
void xms_free(unsigned xms_handle);
unsigned umb_alloc(unsigned *size);
char umb_free(unsigned umb_segment);
char xms_write(unsigned xms_handle, unsigned long loc, void *val, unsigned long length);
void *xms_read(unsigned xms_handle, unsigned long loc,unsigned long length);
void xms_kill(void);*/
#define largest_xms_block() xms_avail_block(FALSE)
#define xms_avail()  xms_avail_block(TRUE)

#ifndef NEO_xms_unused

/*返回值说明:
  返回0  : 扩展内存管理程序(himem.sys)被加载;
  返回1  : 扩展内存管理程序(himem.sys)未被加载;
  返回-1 : 无法在常规内存中分配交换页.*/
char xms_init(void)
{
   unsigned char status;
   _AX = 0x4300;
   geninterrupt(0x2F);
   status = _AL;

   if(status == 0x80)
   {
      _AX = 0x4310;
      geninterrupt(0x2F);
      xms_func = (void (far *)(void)) MK_FP(_ES, _BX);

      /*#ifdef __SMALL__
      g_xms_buf = (char *)malloc(XMS_SWAB_BLOCK);
      #endif
      #ifdef __MEDIUM__
      g_xms_buf = (char *)malloc(XMS_SWAB_BLOCK);
      #endif
      #ifdef __COMPACT__
      g_xms_buf = (char *)malloc(XMS_SWAB_BLOCK);
      #endif
      #ifdef __LARGE__
      g_xms_buf = (char *)malloc(XMS_SWAB_BLOCK);
      #endif*/
      /*g_xms_buf = (char far *)0xa0000000L;*/
      xms_swab_new(XMS_SWAB_BLOCK);
      if (g_xms_buf == NULL)
      {
         return -1;
      }

      g_routines |= 16;
      return 0;
   }
   return 1;
}
     

/*本函数将返回以KB为单位的可用扩展内存的大小或最大可用扩展内存块的大小*/
int xms_avail_block(int flag)
{
   _AH = 8;
   xms_func();

   return flag ? _AX : _DX;
}


/*申请bytes指定字节大小的扩展内存块，返回指向该内存块首址的“伪指针”*/
unsigned  xms_alloc(unsigned long bytes)
{
   _DX = (int)(bytes >> 10) + 1;
   _AH = 9;
   xms_func();
   if (_AX == 0)
   {
      return FALSE;
   }
   return /*g_xms_handle =*/ _DX;
}


char xms_realloc(unsigned handle, unsigned resize)
{
   _AH = 0x0f;
   _DX = handle;
   _BX = resize;
   xms_func();
   return _AX?0 : _BL;
}

     
void xms_free(unsigned xms_handle)
{
   /*释放申请到的扩展内存空间*/
   _DX = xms_handle;
   _AH = 0x0a;
   xms_func();

}


#ifndef NEO_umb_unused
/*请求分配UMB(upper memory block,即上位内存, 640K以上的非EMS内存)，
DX给入块的大小(单位:节, 即paragraphs, 为16个字节).DX返回块的实际容量*/
unsigned umb_alloc(unsigned *size)
{
   _AH = 0x10;
   _DX = *size;
   xms_func();
   *size=_DX;
   return _AX?_BX : FALSE;
}


char umb_free(unsigned umb_segment)
{
   _AH = 0x10;
   _DX = umb_segment;
   xms_func();
   return _AX?0 : FALSE;
}
#endif /*NEO_umb_unused*/

     
char xms_write(unsigned xms_handle, unsigned long loc, void *val, unsigned long length)
{
   length += length & 1; /*凑成偶数*/
   g_xms_handle_t.sourcehandle = 0;
   g_xms_handle_t.sourceptr = val;

   g_xms_handle_t.desthandle = xms_handle;
   g_xms_handle_t.destptr = (void far *)(loc);
   g_xms_handle_t.blocklength = length;

   _SI = FP_OFF(&g_xms_handle_t);
   _AH = 0x0b;
   xms_func();
   if (_AX == 0)
   {
      return FALSE;
   }
   return TRUE;
}


/*成功返回TRUE;否则返回FALSE*/
char xms_read_ex(unsigned xms_handle, unsigned long loc,unsigned long length, void *buffer)
{
   length -= length & 1; /*凑成偶数*/
   if (length <= XMS_SWAB_BLOCK - 1) /*当要读取的数据小于中转缓冲时执行以下代码*/
   {
      g_xms_handle_t.sourcehandle = xms_handle;
      g_xms_handle_t.sourceptr = (void far *)(loc);
      g_xms_handle_t.desthandle = 0;
      g_xms_handle_t.destptr = buffer;
      g_xms_handle_t.blocklength = length;

      _DS = FP_SEG(&g_xms_handle_t);/**/
      _SI = FP_OFF(&g_xms_handle_t);
      _AH = 0x0b;
      xms_func();
      if (_AX == 0)
      {
         return FALSE;
      }
      return TRUE;
   }
   return FALSE;
}


/*成功返回指向中转页(g_xms_buf)的指针;否则返回NULL*/
void *xms_read(unsigned xms_handle, unsigned long loc,unsigned long length)
{
   if (xms_read_ex(xms_handle, loc, length, g_xms_buf))
   {
      return g_xms_buf;
   }
   else
   {
      return NULL;
   }
}


void xms_kill(void)
{
   xms_swab_del();
   g_routines &= 239;
}

#endif /*NEO_xms_unused*/


#endif /*__NXMS_H__*/
