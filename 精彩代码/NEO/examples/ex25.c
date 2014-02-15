/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   ��Ļ�ü����λͼ���ȵ���ʵ�ֵľֲ����߶�̬���䣺�ճ�Ч��
*/

#define NEO_bmp_bright_used /*����ʹ��λͼ���ȵ�������*/
#include "neo.h"

main()
{
   int i;
   neo_init();
   install_keyboard();
   _install_timer();
   set_video_mode(320, 240, 16, 65, 0);
   set_cn_font(12, "hzk12");
   show_bmp("hill.bmp", 0, 0);
   string_out("�ֲ����߶�̬���䣺�ճ�Ч��", 0, 0);
   string_out("�㽫���Կ����ճ�ʱԶ�����������������", 0, 16);
   _getch();
   clear();
   for (i = 0; i < 200; ++i)
   {
      screen_rect(i, 0, SCREEN_W - i, SCREEN_H - i);
      show_bmp_ex("hill.bmp", 0, 0, i-40, BMP_BRIGHT);
      rest(80);
   }
   screen_rect(0, 0, SCREEN_W, SCREEN_H);
   string_out("������ϣ�������˳�", 0, 0);
   _getch();
}
