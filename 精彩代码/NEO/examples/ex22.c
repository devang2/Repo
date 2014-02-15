/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   这个程序利用黑色背景和白色方框的运动对比演示了直接作图、
   等待垂直回扫和换页机制的性能特点
*/

#define NEO_key_buffer_unused /*禁用键盘缓冲区*/
#include "neo.h" /*包含NEO SDK头文件*/

main()
{
   /*在此插入变量声明*/
   int i;

   neo_init(); /*NEO初始化*/
   err_method( 2 ); /*指定错误处理方案*/
   install_keyboard(); /*安装键盘处理程序*/
   set_video_mode(320, 200, 16, 75, 0); /*进入320X200，16位图形模式*/
   clear_to_color(makecol16(255, 255, 255));
   surface_alloc(1, FALSE);

   _getch();
   textout("No Vsync & No Fliping & Delay(1000)", 10, 10);
   for (i = 0; i < 320; i++)
   {
      line(i - 51, 75, i - 51, 125, makecol16(255, 255, 255));
      delay(1000);
      rectfill(i - 50, 75, i, 125, makecol16(0, 0, 0));
   }

   clear_to_color(makecol16(255, 255, 255));
   _getch();
   textout("Vsync & No Fliping & No Delay", 10, 10);
   for (i = 0; i < 320; i++)
   {
      line(i - 51, 75, i - 51, 125, makecol16(255, 255, 255));
      vsync();
      rectfill(i - 50, 75, i, 125, makecol16(0, 0, 0));
   }

   clear_to_color(makecol16(255, 255, 255));
   _getch();
   set_work_surface(1);
   rectfill(0, 0, SCREEN_W, SCREEN_H, makecol16(255, 255, 255));
   textout("Vsync & Fliping & No Delay", 10, 10);
   for (i = 0; i < 320; i+=2)
   {
      rectfill(i - 52, 75, i-50, 125, makecol16(255, 255, 255));
      rectfill(i - 50, 75, i, 125, makecol16(0, 0, 0));
      flip_surface(1);
   }
}
