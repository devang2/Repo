/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   这个例子是从Allegro V3.12的例程包中移植过来的,原文件为examples\ex4.c
   原作者是Shawn Hargreaves.这个程序示范怎样操作调色板,它把一套同
   心的圆画在屏幕上并且通过循环调色板产生奇妙的特效.
*/

#include "neo.h"

main()
{
   char black_pallete[768];
   PALETTE pallete;
   RGB temp;
   int c;

   neo_init();
   set_vbe_mode(VBE320X200X256);
   install_keyboard();
   puts("\nPress any key and wait a moment...");/*按任意键继续*/
   _getch();

   memset(black_pallete, 0, 768);
   /* first set the pallete to black to hide what we are doing */
   set_palette(black_pallete);

   /* draw some circles onto the screen */
   for (c=255; c>0; c--)
      circlefill(SCREEN_W/2, SCREEN_H/2, c, c);

   /* fill our pallete with a gradually altering sequence of colors */
   for (c=0; c<64; c++)
   {
      pallete[c].r = c;
      pallete[c].g = 0;
      pallete[c].b = 0;
   }
   for (c=64; c<128; c++)
   {
      pallete[c].r = 127-c;
      pallete[c].g = c-64;
      pallete[c].b = 0;
   }
   for (c=128; c<192; c++)
   {
      pallete[c].r = 0;
      pallete[c].g = 191-c;
      pallete[c].b = c-128;
   }
   for (c=192; c<256; c++)
   {
      pallete[c].r = 0;
      pallete[c].g = 0;
      pallete[c].b = 255-c;
   }

   /* animate the image by rotating the pallete */
   while (!keypressed())
   {
      temp = pallete[255];
      for (c=255; c>0; c--)
      pallete[c] = pallete[c-1];
      pallete[0] = temp;
      set_palette((char *)pallete);
   }
}
