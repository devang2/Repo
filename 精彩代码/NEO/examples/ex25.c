/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   屏幕裁剪结合位图亮度调整实现的局部光线动态渐变：日出效果
*/

#define NEO_bmp_bright_used /*允许使用位图亮度调整功能*/
#include "neo.h"

main()
{
   int i;
   neo_init();
   install_keyboard();
   _install_timer();
   set_video_mode(320, 240, 16, 65, 0);
   set_cn_font(12, "hzk12");
   show_bmp("hill.bmp", 0, 0);
   string_out("局部光线动态渐变：日出效果", 0, 0);
   string_out("你将可以看到日出时远方的天空慢慢亮起来", 0, 16);
   _getch();
   clear();
   for (i = 0; i < 200; ++i)
   {
      screen_rect(i, 0, SCREEN_W - i, SCREEN_H - i);
      show_bmp_ex("hill.bmp", 0, 0, i-40, BMP_BRIGHT);
      rest(80);
   }
   screen_rect(0, 0, SCREEN_W, SCREEN_H);
   string_out("处理完毕，任意键退出", 0, 0);
   _getch();
}
