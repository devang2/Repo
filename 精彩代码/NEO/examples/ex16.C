/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   这个程序演示了通过NEO的基本绘图函数实现的粒子系统——瀑布
   效果。
*/

#include <stdlib.h>
#include "neo.h"

#define particle_num 950

typedef struct particle_sys /*粒子系统*/
{
   float cx;    /*X座标*/
   float cy;    /*Y座标*/
   float vx;    /*X轴速度*/
   float vy;    /*Y轴速度*/
   float vyacc; /*加速度*/
   float life;  /*生命期*/
   float decay; /*衰减量*/
}PARTICLE, *LPPARTICLE;

PARTICLE g_drop[particle_num];

void  init_drop(int x, int y);
void  move_drop(void);

void init_drop(int x, int y)
{
   int i;
   
   for (i = 0; i < particle_num; i++)
   {
      if (g_drop[i].life <= 0)
      {
          g_drop[i].cx = (float)x;
          g_drop[i].cy = (float)y;
          g_drop[i].vx = (float)(0.0007 * (random(500) + 500));
          g_drop[i].vy = (float)(0.1 * random(10));
          g_drop[i].vyacc = 0.05f;
          g_drop[i].life  = 1.0f;
          g_drop[i].decay = (float)(0.0005 * (random(250)));
      }
   }
}

void move_drop(void)
{
   int i;
      
   for (i = 0; i < particle_num; i++)
   {
      if (g_drop[i].life > 0)
      {
         g_drop[i].cx += g_drop[i].vx;
         g_drop[i].cy += g_drop[i].vy;
         g_drop[i].vy += g_drop[i].vyacc;
         g_drop[i].life -= g_drop[i].decay;
         if (g_drop[i].cx >= 310)
            g_drop[i].vx *= (-1);
         if ((g_drop[i].cy <= 10) || (g_drop[i].cy >= 190))
         {
            g_drop[i].vy *= (-1);
            g_drop[i].decay += 0.1f;
            g_drop[i].vy += 0.1f;
         }
      }
   }
}

void main()
{
   int i,x,y,color;
   
   neo_init();
   install_keyboard();
   set_vbe_mode(0x13);
   for (i=0; i<=128; i++)
   {
      set_color(i, 0, i>>3, i>>1);
   }
   
   while (!keypressed())
   {
      init_drop (130, 25);
      move_drop();

      for (i = 0; i < particle_num; i++)
      {
         dot(g_drop[i].cx, g_drop[i].cy, 127);
      }
      
      for (y = 25; y < 200; y++)
      {
         for (x = 130; x < 215; x++)
         {           
            color = (get_dot(x, y-1) +
                     get_dot(x+1, y) +
                     get_dot(x, y+1) +
                     get_dot(x-1, y)) >> 2;
            if (color > 2)
               dot(x, y, color - 2);
            else
               dot(x, y, 0);
         }
      }
   }
}
