/*WIN-TC&NEO SDK键盘编程模板
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_keyboard(); /*加载NEO键盘中断处理程序*/

   /*****此部分添加你自己的代码
   比如主循环之类的等等*****/

   _getch(); /* 暂停一下，看看前面代码的运行结果 */
}
