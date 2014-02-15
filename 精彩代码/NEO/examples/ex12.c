/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ��ͨ�����βü��������춯��Ч����ʾ
*/
#include "neo.h"

char main()
{
   int i, j;

   neo_init();
   _install_timer();
   install_keyboard();
   change_timer(30);
   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   screen(270, 190, 370, 290);

   while (!keypressed())
   {
      for (i = 0; i < 400; i += 100)
      {
         for (j = 0; j < 1000; j += 100)
         {
            vsync();
            show_bmp("man.bmp" , 270 - j, 190 - i);
         }
      }
   }
}
