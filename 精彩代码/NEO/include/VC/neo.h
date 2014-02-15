/*NEO SDK V2.1.41 For DOS
  Copyleft Cker Home 2003-2005.

  Open Source Obey NEO_PL.TXT.
  http://neo.coderlife.net
  ckerhome@yahoo.com.cn

  �ļ����� : neo.h
  ժ    Ҫ : ��ͷ�ļ�������NEO SDK���й�ϵͳ��ʹ���������ṹ��ȫ�ֱ���������������
  ��ǰ�汾 : V0.39
  ��    �� : ����
  ������� : 2005.7.2

  ȡ���汾 : V0.37
  ԭ �� �� : ����
  ������� : 2004.5.16
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

/*����һ������NEOĬ�Ͻ��õ�һЩ�������̣�NEO�������ǲ�������֧����Щ����
  ���ǳ��ڶ��������ִ���ļ���С�Ŀ��ǡ������á����뿪�ء���һЩ�����û�Ϻ�
  �ڴ�Ĳ������̽����ˣ����粻���õ����Ӳ����ꡢ��ռ�ڴ��NEOϵͳ��ɫ�塢
  ��δ��ɵ�λͼ����ȵȡ���Ȼ��������ָ������ǵ�֧���Ա�ʹ��Ҳ�Ƿǳ��򵥵�
  ��ֻҪ��#include "neo.h"֮ǰ���Ͽ����ù��ܵġ����뿪�ء����ȷ�˵�������д
  �Ĳ�����ͨͼ�γ����������Ϸ֮��ġ��߼���ͼ�γ�������ܾ���Ҫ������Ƶ��
  ���Ĺ��ܣ�����Ƶ������NEOĬ�Ͻ��õģ��������#include "neo.h"ǰ�߼�������
  ��Ƶ���̵ġ����뿪�ء�#define NEO_sound_used�����������֧�֡�������Բο�
  NEO��ʹ���ĵ��еġ���С��Ŀ�ִ���ļ������һ��*/

/*NEOĬ�ϲ�ʹ�û�ͼģʽ(Ʃ��XOR_PUT�ȵ�)*/
#ifndef NEO_draw_mode_used
#define NEO_draw_mode_unused
#endif
/*NEOĬ�ϲ�����NEO��ϵͳ��ɫ��*/
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

/*�������е�NEOͷ�ļ�*/
#include "nerror.h" /*���󱨸漰�׳�����*/
#include "ndraw.h"  /*�Կ�Ӳ���ײ㴦��*/
#include "n2d.h"    /*����ͼ�κ���*/
#include "nword.h"  /*�������*/

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
/*��������: ��Ӳ�������޹صĸ߼���ʱ����.            *
 *����˵��: delay: ��ʱ��          *
 *����˵��: ��.                                    *
 *��    ע: ���ڴ˺����ܲ��������Ӳ�������޹ص�ʱ�� *��
 *          ���,����ǿ⺯��delay()�����������.    */
void adv_delay(long delay)
{
   unsigned int far *clock = (unsigned int far *)0x0000046CL;
   unsigned int now;
   now = *clock;
   while (abs(*clock - now) < delay);
}   
#endif
