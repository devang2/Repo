/*WIN-TC&NEO SDK扩充内存模板
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#define NEO_ems_used

#include "neo.h"
main()
{
   long ems_handle; /*定义一个伪指针*/

   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_keyboard();
   if(!ems_init())
   {
      neo_printf(0, 0, "Expanded memory is not present\n");
   }
   ems_handle = ems_alloc(10); /*申请10页即160K的扩展内存,用户根据需要修改*/
   /*****此部分添加你自己的代码
   比如主循环之类的等等*****/

   _getch(); /* 暂停一下，看看前面代码的运行结果 */
}
