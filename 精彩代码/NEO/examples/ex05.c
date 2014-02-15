/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   这个程序演示了如何在程序主循环中使用鼠标刷新函数
   你可以试着单击、双击、右击按键的操作来看看有什么
   效果
*/

#include "neo.h"

main()
{
   neo_init();
   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_mouse(); /*安装高级鼠标例程*/
   install_keyboard();
   clear_to_color(_DARKGRAY); /*以灰色清屏*/
   text_mode(_DARKGRAY);      /*设置文字背景色*/
   set_str_color(_YELLOW); /*设置文字显示颜色为黄色*/

   textout("The mouse test, press any key to exit...", 5, 5);
   while (!keypressed()) /*按任意键退出循环*/
   {
      /*画立体按钮*/
      line(300, 219, 340, 219, 15);
      line(299, 220, 299, 260, 15);
      line(341, 220, 341, 260, 20);
      line(300, 261, 340, 261, 20);
      mouse_refresh(); /*鼠标状态刷新*/
      switch (get_click_info())
      {
         case 1: /*你单击了左键*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTBLUE);
         textout("You click the left button", 5, 5);
         unscare_mouse();
         break;
         case 2: /*你单击了右键*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTGREEN);
         textout("You click the right button", 5, 5);
         unscare_mouse();
         break;
         case 3: /*同时按下左右键*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTCYAN);
         textout("You click the left&right button", 5, 5);
         unscare_mouse();
         break;
         case 5: /*双击*/
         scare_mouse();
         rectfill(0, 0, 360, 16, _DARKGRAY);
         rectfill(300, 220, 340, 260, _LIGHTMAGENTA);
         textout("You click double the left button", 5, 5);
         unscare_mouse();
         break;
         default:
         break;
      }
   }
}
