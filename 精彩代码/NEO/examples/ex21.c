/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   这个程序演示了使用硬件滚屏实现的“地震”效果
*/

/*开启硬件滚屏功能*/
#define NEO_scroll_used
#include "neo.h"

main()
{
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 16, 75, 0); /*进入640X480，16位图形模式*/
   big_map("swa.bmp", 0, 0);
   readkey();
   while (!keypressed())
   {
      scroll_in_step(1, 1, random(32));
   }
}
