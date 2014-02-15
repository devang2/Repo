/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ��ͨ�����βü�������ϵײ���꺯��
   ����̽�յ�Ч����
*/

#include "neo.h"

main()
{
   int x = 0;
   int y = 0;
   neo_init();
   install_keyboard();
   init_mouse(); /*�ײ���꺯����ʹ��*/
   /*�л���320X200X256ģʽ�£��൱��set_vbe_mode(VBE320X200X256);*/
   set_vbe_mode(0x13);
   /*��������ƶ���Χ*/
   set_mouse_range(0, 0, SCREEN_W, SCREEN_H);

   show_bmp("cs.bmp", 0, 0);
   set_str_color(1);
   textout("Click any mouse button to continue...", 15, 170);
   while (!mouse_act() ); /*�ȴ���갴��*/
   clear();
   while (!keypressed()) /*��������˳�*/
   {
      while (x == get_mouse_x() && y == get_mouse_y()); /*���δ�ƶ�ʱ�ȴ�*/

      screen(0, 0, SCREEN_W, SCREEN_H);
      vsync(); /*�ȴ���Ļ��ɨ��������������*/
      rectfill(x - 1 ,y, x + 61, y + 46, 0); /*�ñ���ɫ����ԭ�ȵ���ʾ����*/
      x = get_mouse_x();
      y = get_mouse_y();
      screen(x, y, x + 60, y + 46);
      show_bmp("cs.bmp", 0, 0); /*���µ�λ�ÿ���һ����ʾ����*/
   }
}
