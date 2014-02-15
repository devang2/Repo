/*WIN-TC&NEO SDK高级鼠标编程模板(加载了时钟例程)
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 60, 0); /*进入640X480，8位图形模式*/
   install_keyboard();
   _install_timer(); /*安装NEO时钟中断程序，此时时钟中断触发频率为18次/秒*/
   /*change_timer(60);*/ /*修改时钟触发频率，用户可通过去掉注释设置成相应的值*/
   install_mouse(); /*安装高级鼠标例程*/

   /*****此部分添加你自己的代码
   一般是游戏大循环之类的,如：
   while(!keypressed())
   {
      用户代码
   }
   等等*****/
}
