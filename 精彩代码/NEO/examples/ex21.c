/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   ���������ʾ��ʹ��Ӳ������ʵ�ֵġ�����Ч��
*/

/*����Ӳ����������*/
#define NEO_scroll_used
#include "neo.h"

main()
{
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 16, 75, 0); /*����640X480��16λͼ��ģʽ*/
   big_map("swa.bmp", 0, 0);
   readkey();
   while (!keypressed())
   {
      scroll_in_step(1, 1, random(32));
   }
}
