/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ���Դ����βü������λͼ��ʾ����
   bmp_masked_blit()����Ķ���Ч����bmp_masked_blit()
   ��ר��Ϊ��Ϸ�о�����ʾ׼����,��Allegro��masked_blit()
   �Ĺ�������
*/

#include "neo.h"

char main()
{
   int i, j;

   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/

   while (!keypressed())
   {
      for (i = 0; i < 400; i += 100)
      {
         for (j = 0; j < 1000; j += 100)
         {
            vsync();
            rectfill(270, 190, 370, 290, 0);
            bmp_masked_blit("man.bmp", j, i, 270, 190, 100, 100);
            delay(1000);
         }
      }
   }
}
