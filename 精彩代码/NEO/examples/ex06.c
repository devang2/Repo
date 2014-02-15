/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ�����ͨ������ʱ��������ʹ�����
   ��������ŵ�����˫�����һ������Ĳ�����������ʲôЧ��
*/

#include "neo.h"

main()
{
   neo_init();
   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_mouse(); /*��װ�߼��������*/
   _install_timer(); /*����ʱ�����̣����������״̬ˢ�º���*/
   change_timer(40);/*�޸�ʱ���жϵĴ���Ƶ��Ϊ40HZ*/

   clear_to_color(_DARKGRAY); /*�Ի�ɫ����*/
   text_mode(_DARKGRAY);      /*�������ֱ���ɫ*/
   set_str_color(_YELLOW); /*����������ʾ��ɫΪ��ɫ*/

   textout("The mouse test, press any key to exit...", 5, 5);
   while (!kbhit()) /*��������˳�ѭ��*/
   {
      /*�����尴ť*/
      line(300, 219, 340, 219, 15);
      line(299, 220, 299, 260, 15);
      line(341, 220, 341, 260, 20);
      line(300, 261, 340, 261, 20);
      switch (get_click_info())
      {
         case 1: /*�㵥�������*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTBLUE);
         textout("You click the left button", 5, 5);
         unscare_mouse();
         break;
         case 2: /*�㵥�����Ҽ�*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTGREEN);
         textout("You click the right button", 5, 5);
         unscare_mouse();
         break;
         case 3: /*ͬʱ�������Ҽ�*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTCYAN);
         textout("You click the left&right button", 5, 5);
         unscare_mouse();
         break;
         case 5: /*˫��*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTMAGENTA);
         textout("You click double the left button", 5, 5);
         unscare_mouse();
         break;
         default:
         break;
      }
      LOOP_END_OF_TIMER(); /*����ɱ�©��Ŷ~*/
   }
}
