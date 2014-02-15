/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   这是一个很简单的程序——Hello, World!
   它告诉你如何切换到图形方式下并显示文字。
*/

/*包含neo.h来使用NEO SDK*/
#include "neo.h"

main()
{
   /* you should always do this at the start of NEO programs */
   neo_init();

   /* set VGA graphics mode 13h (sized 320x200, color depth 8 bits) */
   set_video_mode(320, 240, 8, 60, 0);
   install_keyboard();

   set_str_color(_LIGHTGREEN);
   /* write some text to the screen */
   /**/
   neo_printf(8, 8,  "The version of NEO: %d.%d %s.", NEO_VERSION, NEO_SUB_VERSION, NEO_BETA?"Release":"Beta");
   neo_printf(8, 16, "Issue date of NEO : %s.", neo_date);
   neo_printf(8, 24, "The author of NEO : DongKai.");
   neo_printf(8, 32, "Current resolution: %d x %d.", SCREEN_W, SCREEN_H);
   textout("Press any key to quit.", 8, 48);
   textout("Hello, world!", SCREEN_W/3, SCREEN_H/2);
   readkey();
}
