/*WIN-TC & NEO SDK基本图形编程模板V1.1
  http://neo.coderlife.net
  E-MAIL:ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 75, 0);  /*设置图形模式*/
   install_keyboard();

   /*****此部分添加你自己的代码，例如
   line(25, 25, 220, 220, 15);
   circle(100,100,50, 12);
   等等*****/

   _getch(); /* 暂停一下，看看前面绘图代码的运行结果 */
}
