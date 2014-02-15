/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ��NEO�ĸ��ֻ�����ͼ������ʹ��
*/

#include "neo.h"

main()
{
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/

   while (!keypressed())
   {  /*��������껭��*/
      dot(random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭��*/
      line(random(639), random(479), random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭��*/
      rect(random(639), random(479), random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭��*/
      rectfill(random(639), random(479), random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭Բ*/
      circle(random(639), random(479), random(320), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭��Բ*/
      ellipse(random(639), random(479), random(300), random(300), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭ʵ��Բ*/
      circlefill(random(639), random(479), random(99), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*��������껭ʵ����Բ*/
      ellipsefill(random(639), random(479), random(99), random(99), random(255));
   }
}
