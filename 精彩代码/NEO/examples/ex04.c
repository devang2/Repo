/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   这个有趣的程序也是从Allegro V3.12的例程包中移植过来的
   它显示了如何进行显存操作，它使用的是读点函数get_dot()
   和画点函数dot()，而它最终的效果是一团冉冉升起的火焰。
*/

#include <stdlib.h>
#include "neo.h"

/* The fire is formed from several 'hotspots' which are moved randomly
 * across the bottom of the screen.
 */
#define FIRE_HOTSPOTS   48

int hotspot[FIRE_HOTSPOTS]; 

/* This function updates the bottom line of the screen with a pattern
 * of varying intensities which are then moved upwards and faded out
 * by the code in main().
 */
void draw_bottom_line_of_fire()
{
   int c, c2;
   unsigned char temp[320];

   /* zero the buffer */
   for (c=0; c<SCREEN_W; c++)
      temp[c] = 0;

   for (c=0; c<FIRE_HOTSPOTS; c++) { 
      /* display the hotspots */
      for (c2=hotspot[c]-20; c2<hotspot[c]+20; c2++)
    if ((c2 >= 0) && (c2 < SCREEN_W))
       temp[c2] = min(temp[c2] + 20-abs(hotspot[c]-c2), 192);

      /* move the hotspots */
      hotspot[c] += (random(350) & 7) - 3;
      if (hotspot[c] < 0)
    hotspot[c] += SCREEN_W;
      else
    if (hotspot[c] >= SCREEN_W)
       hotspot[c] -= SCREEN_W;
   }
   /* display the buffer */
   for (c=0; c<SCREEN_W; c++)
      dot(c, SCREEN_H-1, temp[c]);
}

int main()
{
   char pallete[256][3];
   int c;
   int x, y;
   unsigned long address;
   unsigned char temp[320];

   neo_init();
   install_keyboard(); 
   set_vbe_mode(VBE320X200X256);

   for (c=0; c<FIRE_HOTSPOTS; c++)
      hotspot[c] = random(320) % SCREEN_W;

   /* fill our pallete with a gradually altering sequence of colors */
   for (c=0; c<64; c++) {
      pallete[c][0] = c;
      pallete[c][1] = 0;
      pallete[c][2] = 0;
   }
   for (c=64; c<128; c++) {
      pallete[c][0] = 63;
      pallete[c][1] = c-64;
      pallete[c][2] = 0;
   }
   for (c=128; c<192; c++) {
      pallete[c][0] = 63;
      pallete[c][1] = 63;
      pallete[c][2] = c-192;
   }
   for (c=192; c<256; c++) {
      pallete[c][0] = 63;
      pallete[c][1] = 63;
      pallete[c][2] = 63;
   }

   set_palette(*pallete);
   set_str_color(100);
   textout("Using get/dot()", 0, 10);

   /* using get_dot(() and dot() is slow :-) */
   while (!keypressed()) {
      draw_bottom_line_of_fire();

      for (y=64; y<SCREEN_H-1; y++) {
    /* read line */
    for (x=0; x<SCREEN_W; x++)
       temp[x] = get_dot(x, y+1);

    /* adjust it */
    for (x=0; x<SCREEN_W; x++)
       if (temp[x] > 0)
          temp[x]--;

    /* write line */
    for (x=0; x<SCREEN_W; x++)
       dot(x, y, temp[x]);
      }
   }

   clear_keybuf();
}
