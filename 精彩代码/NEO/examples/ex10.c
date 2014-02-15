/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   这个程序演示了通过矩形裁剪函数结合底层鼠标函数
   制造探照灯效果。
*/

#include "neo.h"

main()
{
   int x = 0;
   int y = 0;
   neo_init();
   install_keyboard();
   init_mouse(); /*底层鼠标函数初使化*/
   /*切换到320X200X256模式下，相当于set_vbe_mode(VBE320X200X256);*/
   set_vbe_mode(0x13);
   /*设置鼠标移动范围*/
   set_mouse_range(0, 0, SCREEN_W, SCREEN_H);

   show_bmp("cs.bmp", 0, 0);
   set_str_color(1);
   textout("Click any mouse button to continue...", 15, 170);
   while (!mouse_act() ); /*等待鼠标按键*/
   clear();
   while (!keypressed()) /*按任意键退出*/
   {
      while (x == get_mouse_x() && y == get_mouse_y()); /*鼠标未移动时等待*/

      screen(0, 0, SCREEN_W, SCREEN_H);
      vsync(); /*等待屏幕回扫，消除屏闪问题*/
      rectfill(x - 1 ,y, x + 61, y + 46, 0); /*用背景色覆盖原先的显示区域*/
      x = get_mouse_x();
      y = get_mouse_y();
      screen(x, y, x + 60, y + 46);
      show_bmp("cs.bmp", 0, 0); /*在新的位置开辟一个显示区域*/
   }
}
