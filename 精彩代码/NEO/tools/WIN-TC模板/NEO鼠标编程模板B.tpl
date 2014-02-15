/*WIN-TC&NEO SDK高级鼠标编程模板(不加载时钟例程)
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init();

   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_keyboard();
   install_mouse();

   while(!keypressed()) /*程序主循环*/
   {
      mouse_refresh();
      /*插入用户自己的代码段
        ......*/
   }
}
