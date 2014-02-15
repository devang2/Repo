/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   这个程序演示了如何通过轮流加载不同的ICO图标产生动态光标效果
   该程序运行需要和图标文件flash.ico系列在同个文件夹下
*/

/*通过编译开关开启对淡出淡入的支持*/
#define NEO_fade_pal_used
#include "neo.h"
#define  DIFFER 0.1f
main()
{
   float time_differ, time_cnt;
   char  i = 0;
   char  *ico[6] = {"flash1.ico", "flash2.ico", "flash3.ico", "flash4.ico", "flash5.ico", "flash6.ico"};
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_mouse();              /*安装高级鼠标例程*/
   surface_alloc(1, FALSE);
   set_work_surface(1);
   show_bmp("neosdk.bmp", 0, 0);
   flip();
   install_timer(new_1ch_int);   /*加载时钟例程，它绑定了鼠标状态刷新函数*/
   change_timer(60);             /*修改时钟中断的触发频率为60HZ*/

   time_cnt = neo_clock();
   while (!keypressed())          /*按任意鼠标键退出*/
   {
      time_differ = neo_clock() - time_cnt;
      if (time_differ >= DIFFER)
      {
         i += (i < 5?1 : -5);
         set_mouse_icon(ico[i]);
         time_cnt = neo_clock();
      }
   }
   fade_out(216, 5);
}
