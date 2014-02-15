/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nshow.h
  摘    要 : 本头文件中包含了NEO SDK里有关滚屏的函数、结构、全局变量的声明定义

  当前版本 : V2.02
  作    者 : 董凯 
  完成日期 : 2006.02.25

  取代版本 : V2.01
  原 作 者 : 董凯 
  完成日期 : 2005.01.03
*/

#ifndef __NSCROLL_H__
#define __NSCROLL_H__

#ifndef NEO_scroll_unused

char g_map_file[PATH_LENGTH];
int g_map_wid;
int g_map_hei;
int g_map_h_o;
int g_map_v_o;

char big_map(char *map_file, int x, int y);
void scroll_in_step(unsigned h_step, unsigned v_step, int direction);
int  lc2wc_x(int x);

char big_map(char *map_file, int x, int y)
{
   char reserved_page;
   int  color_temp;
   strcpy(g_map_file, map_file);
   g_map_h_o = x < 0?x : 0;
   g_map_v_o = y < 0?y : 0;
   g_vtotal = g_htotal = 0;
   #ifndef NEO_showbmp_unused
   show_bmp(map_file, g_map_h_o, g_map_v_o);
   #endif
   g_map_wid = g_bmp_wid;

   g_map_h_o = -g_map_h_o;
   g_map_v_o = -g_map_v_o;

   /*适应换页机制*/                           /*g_color_depth/7为适应15位色*/
   g_map_hei = g_bmpheight < (color_temp = (int)(8192 / g_color_byte))?g_bmpheight : color_temp;
   reserved_page = (char)(g_map_hei / SCREEN_H);
   /*处理g_map_hei / SCREEN_H除不尽的情况*/
   reserved_page = g_map_hei / SCREEN_H > reserved_page?++reserved_page : reserved_page;
   if (reserved_page > 0 && g_total_surfaces > 0)
   {
      g_total_surfaces = (g_reserved_page = reserved_page) < g_total_surfaces?g_total_surfaces - reserved_page:0;
      g_alloc_surface  = reserved_page < g_alloc_surface ?g_alloc_surface - reserved_page :0;
      g_reserved_page  = reserved_page;
   }
   return reserved_page;
}

void scroll_in_step(unsigned h_step, unsigned v_step, int direction)
{
   unsigned offset_bak = g_sl_offset; /*备份g_sl_offset*/
   int h_delta;
   #ifndef NEO_rect_unused
   rect_store();
   #endif
   screen(0, 0, SCREEN_W, SCREEN_H);
   h_delta = g_htotal % SCREEN_W;

   if (direction & _UP_ROLL)
   {
      if (g_map_hei > g_vtotal + v_step + SCREEN_H)
      {
         set_display_start(g_htotal, g_vtotal += v_step);
         g_sl_offset = g_vtotal;
         bmp_blit(g_map_file, g_htotal,SCREEN_H + g_vtotal - v_step, h_delta,SCREEN_H-v_step, SCREEN_W-h_delta,v_step);       /*.~SW*/
         bmp_blit(g_map_file, g_htotal + SCREEN_W - h_delta,SCREEN_H + g_vtotal - v_step, 0,SCREEN_H-v_step, h_delta,v_step); /*0~.*/
      }
   }
   else if (direction & _DOWN_ROLL)
   {
      set_display_start(g_htotal, g_vtotal = (g_vtotal >= v_step?g_vtotal -= v_step : 0));
      g_sl_offset = g_vtotal;
      bmp_blit(g_map_file, g_htotal,g_vtotal, h_delta,0, SCREEN_W-h_delta,v_step);
      bmp_blit(g_map_file, g_htotal + SCREEN_W - h_delta,g_vtotal, 0,0, h_delta,v_step);
   }

   if (direction & _LEFT_ROLL)
   {
      if (g_map_wid > g_htotal + h_step + SCREEN_W)
      {
         g_sl_offset = g_vtotal;
         g_sl_offset += g_htotal / SCREEN_W + 1; /*起微调作用*/
         set_display_start(g_htotal + h_step, g_vtotal);
         bmp_blit(g_map_file, SCREEN_W + g_htotal,g_vtotal, h_delta,0, h_step,SCREEN_H);
         g_htotal += h_step;
      }
   }
   else if (direction & _RIGHT_ROLL)
   {
      g_sl_offset = g_vtotal;
      g_sl_offset += g_htotal / SCREEN_W; /*起微调作用*/
      set_display_start(g_htotal = (g_htotal >= h_step?g_htotal -= h_step : 0), g_vtotal);
      bmp_blit(g_map_file, g_htotal,g_vtotal, h_delta,0, h_step,SCREEN_H);
   }

   g_sl_offset = offset_bak; /*还原g_sl_offset*/
   #ifndef NEO_rect_unused
   rect_restore();
   #endif
}

/*---------------------------------------------------*
 *函数功能: 本地坐标系转换为世界坐标系(X轴).         *
 *参数说明: 将本地坐标（又称视窗坐标）的X轴分量转换为*
 *          世界坐标（大地图坐标）的X轴分量。通过这个*
 *          函数可以不受滚屏影响在屏幕上画图。       *
 *返回说明: 返回世界坐标系(X轴)坐标，可直接用于dot() *
 *          line()等作图函数，这样一来就可以不受滚屏 *
 *          操作的干扰定点作图                       *
 *备    注: 服务于滚屏功能的辅助函数，可直接调用     *
 *---------------------------------------------------*/
int lc2wc_x(int x)
{
   int h_delta = g_htotal % SCREEN_W;
   return x <= SCREEN_W - h_delta?h_delta + x : x + h_delta - SCREEN_W;
}
#endif


#endif
