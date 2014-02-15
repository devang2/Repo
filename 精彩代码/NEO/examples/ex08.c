/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   这个程序演示了NEO的各种基本绘图函数的使用
*/

#include "neo.h"

main()
{
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*进入640X480，8位图形模式*/

   while (!keypressed())
   {  /*以随机坐标画点*/
      dot(random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画线*/
      line(random(639), random(479), random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画框*/
      rect(random(639), random(479), random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画面*/
      rectfill(random(639), random(479), random(639), random(479), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画圆*/
      circle(random(639), random(479), random(320), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画椭圆*/
      ellipse(random(639), random(479), random(300), random(300), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画实心圆*/
      circlefill(random(639), random(479), random(99), random(255));
   }
   readkey();
   clear();
   while (!keypressed())
   {  /*以随机坐标画实心椭圆*/
      ellipsefill(random(639), random(479), random(99), random(99), random(255));
   }
}
