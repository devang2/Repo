/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   这个程序演示了自带矩形裁剪输出的位图显示函数
   bmp_masked_blit()制造的动画效果。bmp_masked_blit()
   是专门为游戏中精灵显示准备的,与Allegro的masked_blit()
   的功能相似
*/

#include "neo.h"

char main()
{
   int i, j;

   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/

   while (!keypressed())
   {
      for (i = 0; i < 400; i += 100)
      {
         for (j = 0; j < 1000; j += 100)
         {
            vsync();
            rectfill(270, 190, 370, 290, 0);
            bmp_masked_blit("man.bmp", j, i, 270, 190, 100, 100);
            delay(1000);
         }
      }
   }
}
