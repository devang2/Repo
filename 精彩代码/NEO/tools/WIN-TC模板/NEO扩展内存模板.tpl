/*WIN-TC&NEO SDK扩展内存模板
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   unsigned xms_ptr; /*定义一个伪指针*/

   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_keyboard();
   if (xms_init()) /*初始化扩展内存*/
   neo_printf(0, 0, "Himem.sys load false.\n");

   xms_ptr = xms_alloc(1024); /*申请1024K即1M的扩展内存,用户根据需要修改*/
   /*****此部分添加你自己的代码
   比如主循环之类的等等*****/

   _getch(); /* 暂停一下，看看前面代码的运行结果 */
}
