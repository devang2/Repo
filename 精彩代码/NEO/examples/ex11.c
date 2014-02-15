/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   运用NEO的矩形裁剪功能制作百叶窗效果显示位图
*/
#include "neo.h"

void line_cls(int color, char mode);

/*线清屏，该函数能制造百叶窗式的清屏效果
  其中参数color是指用它代表的颜色清屏
  mode为正时表示横向清屏，为负表示纵向清屏*/
void line_cls(int color, char mode)
{
   int i;
   if (mode > 0) /*横向清屏*/
   {
      for (i = 0; i <= g_rect_right; i += 2)
      {
         delay(500);
         line(i, 0, i, g_rect_bottom, color);
         line(g_rect_right - i, 0, g_rect_right - i, g_rect_bottom, color);
      }
   }
   else if (mode < 0) /*纵向清屏*/
   {
      for (i = 0; i <= g_rect_bottom; i += 2)
      {
         delay(500);
         hline(0, i, g_rect_right, color);
         hline(0, g_rect_bottom - i, g_rect_right, color);
      }
   }
}

main()
{
   int i;
   neo_init();
   install_keyboard();
   /*切换到640X480X64K模式下，相当于set_vbe_mode(VBE640X480X64K);*/
   set_video_mode(640, 480, 16, 75, 0); /*进入640X480，16位图形模式*/

   set_str_color(0xfef0);
   show_bmp("neosdk.bmp", 0, 0); /*显示LOGO位图*/
   textout("Press any key to continue...", 210, 290);
   readkey();
   clear();

   for (i = 0; i < 240; i += 2)
   {  /*设置矩形裁剪输出的范围(超出该范围的显示将被忽略)*/
      screen(0, i * 2, 640, (i + 1) * 2);
      _show_bmp("world.bmp", 0, 0);
   }
   for (i = 239; i > 0; i -= 2)
   {
      screen(0, i * 2, 640, (i + 1) * 2);
      _show_bmp("world.bmp", 0, 0);
   }
   /*将裁剪输出的范围复位为整个屏幕*/
   screen(0, 0, SCREEN_W, SCREEN_H); /*宏SCREEN_W和SCREEN_H在NEO中已定义*/
   line_cls(0, 1); /*横向百叶窗式的清屏*/

   for (i = 0; i < 320; i += 2)
   {
      screen(i * 2, 0, (i + 1) * 2, 640);
      show_bmp("world.bmp", 0, 0);
   }
   for (i = 319; i > 0; i -= 2)
   {
      screen(i * 2, 0, (i + 1) * 2, 640);
      show_bmp("world.bmp", 0, 0);
   }
   /*将裁剪输出的范围复位为整个屏幕*/
   screen(0, 0, SCREEN_W, SCREEN_H);
   line_cls(0, -1); /*纵向百叶窗式的清屏*/

   show_bmp("neosdk.bmp", 0, 0);
   textout("Press any key to exit...", 220, 290);
   readkey();
}
