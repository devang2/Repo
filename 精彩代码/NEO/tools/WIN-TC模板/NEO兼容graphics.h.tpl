/*WIN-TC&NEO SDK兼容graphics.h模板
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/
/*启用NEO兼容graphics.h模式*/
#define NEO_hi_graphics_h_used

#include "neo.h"
main()
{
   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_keyboard();

   /*****此部分添加你自己的代码
   比如主循环之类的等等*****/

   _getch(); /* 暂停一下，看看前面代码的运行结果 */
}
