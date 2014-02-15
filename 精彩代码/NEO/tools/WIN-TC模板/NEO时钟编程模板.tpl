/*WIN-TC&NEO SDK时钟编程模板
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO初使化*/

   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/
   install_keyboard();
   _install_timer(); /*安装NEO时钟中断程序，此时时钟中断触发频率为18次/秒*/
   change_timer(60); /*修改时钟触发频率，用户设置成相应的值*/

   /*****此部分添加你自己的代码
   一般是游戏大循环之类的,如：
   while(!keypressed())
   {
      用户代码
   }
   等等*****/
}
