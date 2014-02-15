/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn

   16位高彩模式下的颜色渐变效果
*/

#include "neo.h"

main()
{
   unsigned int color; /*16位高彩颜色变量*/
   int y;

   neo_init();
   set_video_mode(320, 240, 16, 75, 0); /*进入320X240，16位图形模式*/

   /*红色分量在最高五位，递增量为2048：实现红色32级渐变*/
   for (color = 2048,y = 0; y < 32; color += 2048,++y)
   {
      hline(0, y, 319, color);
   }
   /*绿色分量在中间，递增量为32：实现绿色64级渐变*/
   for (color = 32,y = 32; y < 96; color += 32,++y)
   {
      hline(0, y, 319, color);
   }
   /*蓝色分量在最低五位，递增量为1：实现蓝色32级渐变*/
   for (color = 0,y = 96; y < 128; ++color,++y)
   {
      hline(0, y, 319, color);
   }
   /*以红分量递增步长为2048，绿色步长64，兰色步长1（总量2113），实现白色32级渐变*/
   for (color = 0, y = 128; y <160; ++y, color += 2113)
   {
      hline(0, y, 319, color);
   }

   getch();
}
