/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   用鼠标点击每张图片可观看到相应的图片显示特效
*/

/*打开对所有特效的支持*/
#define NEO_bmp_all_effect_used

#include "neo.h"
main()
{
   int  i;
   int  flags[9] = {0,0,0,0,0,0,0,0,0};

   neo_init();
   set_video_mode(640, 480, 16, 0, 0); /*切换到640X480，16位色深模式*/
   clear_to_color(_DARKGRAY);
   install_keyboard();
   _install_timer();

   for (i = 40; i <= 440; i+=200)
   {
      show_bmp("xp.bmp", i,  20);
      show_bmp("xp.bmp", i, 180);
      show_bmp("xp.bmp", i, 340);
   }

   install_mouse();

   while (get_asc() != 27)
   {
      /*mouse_refresh();*/
      if (get_click_info() == 8)
      {
         if (get_mouse_x() > 40 && get_mouse_x() < 200 && get_mouse_y() > 20 && get_mouse_y() < 140)
         {
            flags[0] = flags[0] == 0 ? 0xffff : 0;
            vsync();
            scare_mouse();
            show_bmp_ex("xp.bmp", 40, 20, 0, (BMP_H_FLIP | BMP_V_FLIP) & flags[0]);
            unscare_mouse();
         }
         else if (get_mouse_x() > 240 && get_mouse_x() < 400 && get_mouse_y() > 20 && get_mouse_y() < 140)
         {
            flags[1] = flags[1] == 0 ? 0xffff : 0;
            vsync();
            scare_mouse();
            show_bmp_ex("xp.bmp", 240, 20, 1, BMP_CHANNL & flags[1]);
            unscare_mouse();
         }
         else if (get_mouse_x() > 440 && get_mouse_x() < 600 && get_mouse_y() > 20 && get_mouse_y() < 140)
         {
            flags[2] = flags[2] == 0 ? 0xffff : 0;
            vsync();
            scare_mouse();
            show_bmp_ex("xp.bmp", 440, 20, 35, BMP_BRIGHT & flags[2]);
            unscare_mouse();
         }

         else if (get_mouse_x() > 40 && get_mouse_x() < 200 && get_mouse_y() > 180 && get_mouse_y() < 300)
         {
            flags[3] = flags[3] == 0 ? 0xffff : 0;
            vsync();
            scare_mouse();
            show_bmp_ex("xp.bmp", 40, 180, 0, BMP_RUOLOC & flags[3]);
            unscare_mouse();
         }
         else if (get_mouse_x() > 240 && get_mouse_x() < 400 && get_mouse_y() > 180 && get_mouse_y() < 300)
         {
            flags[4] = flags[4] == 0 ? 0xffff : 0;
            scare_mouse();
            vsync();
            show_bmp_ex("xp.bmp", 240, 180, 7, BMP_CHANNL & flags[4]);
            unscare_mouse();
         }
         else if (get_mouse_x() > 440 && get_mouse_x() < 600 && get_mouse_y() > 180 && get_mouse_y() < 300)
         {
            flags[5] = flags[5] == 0 ? 0xffff : 0;
            vsync();
            scare_mouse();
            show_bmp_ex("xp.bmp", 440, 180, 35, BMP_VIVIDC & flags[5]);
            unscare_mouse();
         }

         else if (get_mouse_x() > 40 && get_mouse_x() < 200 && get_mouse_y() > 340 && get_mouse_y() < 460)
         {
            flags[6] = flags[6] == 0 ? 0xffff : 0;
            scare_mouse();
            rectfill(40, 340, 200, 460, _DARKGRAY);
            vsync();
            show_bmp_ex("xp.bmp", 40, 340, 0, BMP_MASK_C | BMP_MUSLIN & flags[6]);
            unscare_mouse();
         }
         else if (get_mouse_x() > 240 && get_mouse_x() < 400 && get_mouse_y() > 340 && get_mouse_y() < 460)
         {
            flags[7] = flags[7] == 0 ? 0xffff : 0;
            scare_mouse();
            rectfill(240, 340, 400, 460, _DARKGRAY);
            vsync();
            show_bmp_ex("xp.bmp", 240, 340, 0, BMP_MASK_C | BMP_JMPLIN & flags[7]);
            unscare_mouse();
         }
         else if (get_mouse_x() > 440 && get_mouse_x() < 600 && get_mouse_y() > 340 && get_mouse_y() < 460)
         {
            flags[8] = flags[8] == 0 ? 0xffff : 0;
            vsync();
            scare_mouse();
            show_bmp_ex("xp.bmp", 440, 340, 0, BMP_WUBLUR & flags[8]);
            unscare_mouse();
         }
      }
   }
}
