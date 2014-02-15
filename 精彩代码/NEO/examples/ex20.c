/* 
   Example program for the NEO SDK, by Dong Kai.
   http://www.codelife.net
   ckerhome@yahoo.com.cn

   演示了如何向时钟程序处理列表加入用户自己的代码以使其按指定触发频率反复执行
*/
#include "neo.h"

void bar1()
{
   static int i;
   i++;
   rectfill(i - 1, 100, i - 1 + 50, 150, _BLACK); /*擦除原来的方块*/
   rectfill(i, 100, i + 50, 150, _WHITE);         /*绘制新方块*/
}

void bar2()
{
   static int i;
   i++;
   rectfill(i - 1, 200, i - 1 + 50, 250, _BLACK); /*擦除原来的方块*/
   rectfill(i, 200, i + 50, 250, _WHITE);         /*绘制新方块*/
}

main()
{
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/

   _install_timer(); /*安装NEO时钟例程*/
   change_timer(1000); /*设置触发频率为1000，即时钟每秒触发1000个滴答，每个滴答占1毫秒*/

   install_int_ex((T_func_ptr)bar1, 50); /*将函数bar1()加入时钟程序处理列表，执行速度：每100个滴答执行一次*/
   install_int_ex((T_func_ptr)bar2, 200); /*将函数bar2()加入时钟程序处理列表，执行速度：每500个滴答执行一次*/

   _getch();
}
