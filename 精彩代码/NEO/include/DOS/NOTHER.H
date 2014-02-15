/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nother.h
  摘    要 : 本头文件中包含NEO SDK里附加功能的各种函数、结构、全局变量的声明及定义
  当前版本 : V0.30
  作    者 : 董凯 
  完成日期 : 2005.6.15

  取代版本 : V0.28
  原 作 者 : 董凯 
  完成日期 : 2005.6.5
*/

#ifndef NOTHER_H
#define NOTHER_H

void adv_delay(long delay);
void reset(void);
char total_cdrom(void);
char first_cdrom(void);
char copy_file(char *scr_file, char *des_file);


#ifndef NEO_other_unused

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

#ifndef __cplusplus
void reset(void)
{
   union REGS r;
   int far *res = (int far *)0x0472;
   void interrupt(*_reset)();
   
   puts("Press any key to reset your computer...");
   r.h.ah = 0;
   int86(0x16, &r, &r);

   _reset = MK_FP(0xffff, 0);
   *res = 0x1234;
   (*_reset)();
}
#else
void reset(void){}
#endif


/*---------------------------------------------------*
 *函数功能: 查询用户计算机一共有多少个CD-ROM.　      *
 *参数说明: 无.                                      *
 *返回说明: 返回计算机上的CD-ROM个数.                *
 *备    注: CD-ROM控制函数.用户程序可直接调用.　     *
 *---------------------------------------------------*/
char total_cdrom(void)
{
   union REGS reg;
   reg.x.ax = 0x1500;
   reg.x.bx = 0;
   int86(0x2f,&reg,&reg);

   return reg.x.bx;
}


/*---------------------------------------------------*
 *函数功能: 查询计算机上第一个CD-ROM驱动器的驱动号.  *
 *参数说明: 无.                                      *
 *返回说明: 计算机上第一个CD-ROM驱动器的驱动号.比如:G*
 *备    注: CD-ROM控制函数.用户程序可直接调用.　     *
 *---------------------------------------------------*/
char first_cdrom(void)
{
   union REGS reg;
   reg.x.ax = 0x1500;
   reg.x.bx = 0;
   int86(0x2f,&reg,&reg);

   return (reg.x.cx+65);
}


char copy_file(char *scr_file, char *des_file)
{
   FILE *fin,*fout;

   if ((fin=fopen(scr_file, "rb")) == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"copy_file", SOURCE_FILE_BAD, 1};
      throw_error(error);
      #endif
      return -1;
   }
   if ((fout=fopen(des_file, "wb")) == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"copy_file", DEST_FILE_BAD, 1};
      throw_error(error);
      #endif
      return -1;
   }

   while (!feof(fin))
   {
      fputc(fgetc(fin), fout);
   }
   fclose(fout);
   fclose(fin);
   return 0;
}

#endif

#endif
