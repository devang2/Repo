/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : ndraw.h
  摘    要 : 本头文件包含了NEO SDK里的各种基本图形函数、结构、全局变量的声明及定义
  当前版本 : 3.13
  作    者 : 董凯
  完成日期 : 2006.10.18

  取代版本 : 3.11
  原 作 者 : 董凯
  完成日期 : 2006.02.09
*/
#ifndef  __NDRAW_H__
#define  __NDRAW_H__


/*使用中精度圆形填充时，如果圆的半径大于230像素会出现填充“裂缝”问题。此时的填充速度比较适中*/
#define CF_DETAIL 0.0001

/*使用高精度圆形填充时，如果圆的半径大于298像素才会出现填充“裂缝”问题。此时的填充速度最慢*/
#ifdef  N_CF_HIGH_DETAIL
#undef  CF_DETAIL
#define CF_DETAIL 0.00001
#endif

/*使用低精度圆形填充时，如果圆的半径大于65像素就会出现填充“裂缝”问题。但此时的填充速度最快*/
#ifdef  N_CF_LOW_DETAIL
#undef  CF_DETAIL
#define CF_DETAIL 0.001
#endif

/*如果启用兼容Borland graphics.h功能则执行下列操作*/
#ifdef  NEO_hi_graphics_h_used

/*检查是否已经包含Borland graphics.h*/
#ifdef  USER_CHAR_SIZE
#if     VERT_DIR
#error  Please don't include graphics.h!
#endif
#endif

/*启用绘图模式*/
#define NEO_draw_mode_used

#define VGAHI                      0x101
#define VGA                        1
#define EGAVGA_driver              1
#define registerbgidriver(driver)  neo_init()
#define closegraph()               set_vga_mode(0x3)
#define getgraphmode()             get_vbe_mode()
#define setgraphmode(mode)         set_vbe_mode(mode)
#define setviewport(l, t, r, b,c)  c?screen(l, t, r, b):0
#define getmaxx()                  max_x()
#define getmaxy()                  max_y()
#define setwritemode(mode)         set_draw_mode(mode)
#define setcolor(color)            (g_frt_color = color)
#define setbkcolor(color)          clear_to_color(color)
#define getcolor(color)            g_frt_color
#define setrgbpalette(cn, r, g,b)  set_color(cn, r, g, b)
#define putpixel(x, y, color)      dot(x, y, color)
#define getpixel(x, y)             get_dot(x, y)
#define bar(x0, y0, x1, y1)        rectfill(x0-1, y0-1, x1-1, y1-1, g_frt_color)
#define fillellipse(cx, cy, a, b)  ellipsefill(cx, cy, a, b, g_frt_color)
#define rectangle(x0, y0, x1, y1)  rect(x0, y0, x1, y1, g_frt_color)
#define textcolor(color)           set_str_color(color)
#define outtextxy(x, y, str)       textout(str, x, y)
#define setactivepage(page_num)    set_work_surface(page_num)
#define setvisualpage(page_num)    flip_surface(page_num)
#define setfillstyle(pattern, color)
#define cleardevice()              clear()
#define initgraph(driver, mode, path) set_vbe_mode(0x101)
#endif

int g_vtotal, g_htotal;  /*滚屏例程使用*/

/*======================================================*
 *              函数声明(Function declare)              *
 *======================================================*/
void putpixel(int x,int y,int color);
int  get_dot(int x, int y);

void ellipsefill(int center_x, int center_y, int stalk_long,int stalk_short, int fill_color);

#ifndef  NEO_hi_graphics_h_used
void line(int x1, int y1, int x2, int y2, int color);
void drawpoly(int n, int *points, int color);
void circle(int c_x, int c_y, int r, int color);
void ellipse(int x0, int y0, int a, int b, int color);
#else
void line(int x1, int y1, int x2, int y2);
void drawpoly(int n, int *points);
void circle(int c_x, int c_y, int r);
void ellipse(int x0, int y0, int a, int b);
#endif
void circlefill(int x0,int y0,int r,int color);
char hline(int x1, int y, int x2, int color);
void vline(int x, int y1, int y2, int color);
void rect(int x0, int y0, int x1, int y1, int color);
void tri(int xa, int ya, int xb, int yb, int xc, int yc, int col);
void rectfill(int x1, int y1, int x2, int y2, int color);

void _dot(int x,int y,int color);
void clear(void);
void clear_to_color(int color);


/*通用画点函数 17FPS*/
#ifndef alpha_debug_mode
void dot(int x,int y,int color)
#else
void putpixel(int x,int y,int color)
#endif
{
   long addr;
   int  page;

   if ((x<g_rect_left) || (y<g_rect_top) || (x>g_rect_right) || (y>g_rect_bottom)) return;

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 16);

      set_vbe_page(page);
      #ifndef NEO_draw_mode_unused
      switch(g_draw_mode)
      {
         case COPY_PUT : *(g_videoptr+(Uint16)(addr&0xffff))= color;break;
         case XOR_PUT  : *(g_videoptr+(Uint16)(addr&0xffff))= *(g_videoptr+(Uint16)(addr& 0xffff))^(char)color;break;
         case NOT_PUT  : *(g_videoptr+(Uint16)(addr&0xffff))=~*(g_videoptr+(Uint16)(addr& 0xffff));break;
         case OR_PUT   : *(g_videoptr+(Uint16)(addr&0xffff))= *(g_videoptr+(Uint16)(addr& 0xffff))|(char)color;break;
         case AND_PUT  : *(g_videoptr+(Uint16)(addr&0xffff))= *(g_videoptr+(Uint16)(addr& 0xffff))&(char)color;break;
      }
      #else
      *(g_videoptr + (Uint16)(addr & 0xffff)) = color;
      #endif

      #else
      x += 0; y+=0; color+=0;
      #endif
   }
   else /*if (g_color_depth == 16)*/ /*由于NEO目前只支持256和64K色,所以暂时注释左边一句*/
   {
      #ifndef NEO_color_depth_16_unused
      int far *videoptr16 = (int far *)MK_FP(0xa000, 0);
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);

      set_vbe_page(page);
      #ifndef NEO_draw_mode_unused
      switch(g_draw_mode)
      {
         case COPY_PUT : *(videoptr16+(Uint16)(addr&0xffff)) = color;break;
         case XOR_PUT  : *(videoptr16+(Uint16)(addr&0xffff))= *(videoptr16+(Uint16)(addr& 0xffff)) ^ color;break;
         case NOT_PUT  : *(videoptr16+(Uint16)(addr&0xffff))=~*(videoptr16+(Uint16)(addr& 0xffff)); break;
         case OR_PUT   : *(videoptr16+(Uint16)(addr&0xffff))= *(videoptr16+(Uint16)(addr& 0xffff)) | color;break;
         case AND_PUT  : *(videoptr16+(Uint16)(addr&0xffff))= *(videoptr16+(Uint16)(addr& 0xffff)) & color;break;
      }
      #else
      *(videoptr16 + (Uint16)(addr & 0xffff)) = color;
      #endif

      #else
      x += 0; y+=0; color+=0;
      #endif
   }
}


int  get_dot(int x, int y)
{
   long addr;
   int  page;
   if ((x<0) || (y<0) || (x>g_screen_h) || (y>g_screen_v)) return -1;

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 16);
      set_vbe_page(page);
      return (peekb(0xa000, (unsigned)(addr & 0xFFFF) ));
      #else
      x += 0; y+=0;
      #endif
   }
   else
   {
      #ifndef NEO_color_depth_16_unused
      int far *videoptr = (int far *)MK_FP(0xa000, 0);
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);

      set_vbe_page(page);
      return *(videoptr + (unsigned)(addr & 0xffff));
      #else
      x += 0; y+=0;
      #endif
   }
}


/*以下均上基本绘图函数*/
#ifndef NEO_basic_draw_unused
#ifndef NEO_hi_graphics_h_used

/*快速的半透明混合的函数*/
int quick_alpha(Uint16 c1, Uint16 c2)
{
   /*对不同的颜色深度进行分别处理*/
   switch(get_color_depth())
   {
      /*8位色的快速Alpha混合*/
      case 8:
         c1 = (((c1 & 0xda) + (c2 & 0xda)) >> 1) + ((((c1 & 0x25) + (c2 & 0x25)) >> 1) & 0x25);
      break;
      /*15位色的快速Alpha混合*/
      case 15:
         c1 = (((c1 & 0xfbde) + (c2 & 0xfbde)) >> 1) + ((((c1 & 0x0421) + (c2 & 0x0421)) >> 1) & 0x0421);
      break;
      /*16位色的快速Alpha混合*/
      case 16:
         c1 = (((c1 & 0xf7deL) + (c2 & 0xf7deL)) >> 1) + ((((c1 & 0x0821) + (c2 & 0x0821)) >> 1) & 0x0821);
      break;

      default:
      break;
   }

   /*绘制混合以后的点*/
   return (c1);
}

#ifndef alpha_debug_mode
#define quick_alpha_dot(x, y, c) dot(x, y, quick_alpha(get_dot(x, y), c))
#else
#define quick_alpha_dot(x, y, c) putpixel(x, y, quick_alpha(get_dot(x, y), c))
#ifdef  NEO_quick_alpha_used
#define dot(x, y, c) quick_alpha_dot(x, y, c)
#else
#define dot(x, y, c) putpixel(x, y, c)
#endif
#endif


/*通用的画线函数，用color颜色由(x1,y1)画到(x2,y2);速度:(16FPS)*/
void line(int x1, int y1, int x2, int y2, int color)
{
   int  dt_x = x2 - x1, dt_y = y2 - y1, step_x, step_y, change;

   if (dt_x < 0)
   {
      dt_x = -dt_x;    /*从右向左画线*/
      step_x = -1;
   }
   else {step_x = 1;}  /*从左向右画线*/

   if (dt_y < 0)
   {
      dt_y = -dt_y;    /*从下向上画线*/
      step_y = -1;
   }
   else {step_y = 1;}
                       /*从上向下画线*/
   if (dt_x > dt_y)    /*x改变得比y快*/
   {
      change = dt_x >> 1;
      while (x1 != x2)
     {
        dot(x1, y1, color);
        x1 += step_x;
        change += dt_y;
        if(change > dt_x)
        {
           y1 += step_y;
           change -= dt_x;
        }
     }
   }
   else                 /*y改变得比x快*/
   {
      change = dt_y >> 1;
      while(y1 != y2)
      {
         dot(x1, y1, color);
         y1 += step_y;
         change += dt_x;
         if(change > dt_y)
         {
            x1 += step_x;
            change -= dt_y;
         }
      }
   }
   dot(x2, y2, color);
}
#else
void line(int x1, int y1, int x2, int y2)
{
   int  dt_x = x2 - x1, dt_y = y2 - y1, step_x, step_y, change;

   if (dt_x < 0)
   {
      dt_x = -dt_x;    /*从右向左画线*/
      step_x = -1;
   }
   else {step_x = 1;}  /*从左向右画线*/

   if (dt_y < 0)
   {
      dt_y = -dt_y;    /*从下向上画线*/
      step_y = -1;
   }
   else {step_y = 1;}
                        /*从上向下画线*/
   if (dt_x > dt_y)     /*x改变得比y快*/
   {
      change = dt_x >> 1;
      while (x1 != x2)
     {
        dot(x1, y1, g_frt_color);
        x1 += step_x;
        change += dt_y;
        if(change > dt_x)
        {
           y1 += step_y;
           change -= dt_x;
        }
     }
   }
   else                  /*y改变得比x快*/
   {
      change = dt_y >> 1;
      while(y1 != y2)
      {
         dot(x1, y1, g_frt_color);
         y1 += step_y;
         change += dt_x;
         if(change > dt_y)
         {
            x1 += step_x;
            change -= dt_y;
         }
      }
   }
   dot(x2, y2, g_frt_color);
}
#endif

/* x, y为正多边形的中心坐标 */
/* lng为正多边形的边长 */
/* cnt为多边形的边数 */
/* 返回值可供drawpoly()函数使用 */
int *polygon(int x, int y, unsigned cnt, unsigned lng)
{
   double art;
   const double PI = 3.1415926f;
   unsigned R=(int)(lng >> 1)/sin(PI / cnt);
   unsigned i;
   int *points;

   if (cnt < 2 || (points = malloc(cnt << 1)) == NULL)
   {
      return NULL;
   }

   if(cnt & 1) /*判断正多边形边数的奇偶*/
   {
      points[0] = 0 + x;
      points[1] = R + y;

      art = PI / 2.0f;
   }
   else
   {
      art = PI / 2.0f - PI / cnt;
      points[0] = (int)(R * cos(art)) + x;
      points[1] = (int)(R * sin(art)) + y;
   }


   for(i = 0; i < ((cnt - 1) << 1); i += 2)
   {
      art -= (PI * 2.0f) / cnt;
      points[2 + i] = (int)(R * cos(art)) + x;
      points[3 + i] = (int)(R * sin(art)) + y;
   }

   return points;
}

#ifndef  NEO_hi_graphics_h_used
/*绘制多边形的函数*/
void drawpoly(int n, int *points, int color)
{
   int i;

   if (points) /* points != NULL */
   {
      for (i = 0; i < n - 1; ++i)
      {
         line(points[i << 1], points[(i << 1) + 1], points[(i + 1) << 1], points[((i + 1) << 1) + 1], color);
      }
      line(points[0], points[1], points[(n - 1) << 1], points[((n - 1) << 1) + 1], color);
   }
}
#else
void drawpoly(int n, int *points)
{
   int i;   
   if (points) /* points != NULL */
   {
      for (i = 0; i < n - 1; ++i)
      {
         line(points[i << 1], points[(i << 1) + 1], points[(i + 1) << 1], points[((i + 1) << 1) + 1]);
      }
      line(points[0], points[1], points[(n - 1) << 1], points[((n - 1) << 1) + 1]);
   }
}
#endif

/*优化后的画水平线函数;速度: 8位65FPS; 16位32FPS*/
char hline(int x1, int y, int x2, int color)
{
   long addr;
   int  k = 0;
   int  xx = x1;
   int  rect_scr_h = g_rect_right + 1;
   int  len = 0, length/*直线有效长度*/ = 0;
   char page, odd;
   int  fix = 0;
   int  width;

   if (x1 > x2)
   {
      x1 ^= x2;
      x2 ^= x1;
      x1 ^= x2;
   }
   width = x2 - x1;

   if ( (y > g_rect_bottom) || (y < g_rect_top) || width < 0)
   {
      return -1;
   }
   if (x1 >= 0 && x1 < rect_scr_h)
   {
      if (rect_scr_h - x1 >= width) /*直线完全显示*/
      {
         fix = ( x1 <= g_rect_left? (x1 >> (g_color_depth == 8)) : (g_rect_left >> (g_color_depth == 8)) );
         len = length = width + 1;
      }
      else                         /*直线右边溢出*/
      {
         if (g_color_depth == 8)
         {
            if (x1 > g_rect_left)
            {
               x1 -= g_rect_left;
            }
            else fix = x1 >> 1;
         }
         len = length = rect_scr_h - x1 + 1;
      }
   }
   else if (x1 < 0 && x1 > (-width))
   {
      if (width + x1 >= rect_scr_h)  /*直线两端溢出*/
      {
         len = length = rect_scr_h + 1;
         xx = x1;
      }
      else                          /*直线左边溢出*/
      {
         len = (length = width + 1) + x1;
      }
      x1 = 0;
   }
   else
   {
      return -1;
   }

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      int origin = 0;
      unsigned char color8 = (unsigned char)color;
      unsigned int  color16 = color8 << 8;

      color16 += color8;
      len >>= 1;
      length -= (odd = length & 1); /* <=> length & 1*/
      origin = (g_rect_left >> 1) - fix;
      page = (char)((addr = ((long)y + g_sl_offset) * g_screen_h + x1) >> 16);
      if (page == ((addr + length) >> 16))
      {
         int far *d_tmp = (int far *)(g_videoptr + (unsigned)(addr & 0xffff));

         set_vbe_page(page);
         for (; origin < len; ++origin)
         {
            #ifndef NEO_draw_mode_unused
            switch(g_draw_mode)
            {
               case COPY_PUT : d_tmp[origin] = color16; break;
               case XOR_PUT  : d_tmp[origin] = d_tmp[origin] ^ color16; break;
               case NOT_PUT  : d_tmp[origin] = ~d_tmp[origin]; break;
               case OR_PUT   : d_tmp[origin] = d_tmp[origin] | color16; break;
               case AND_PUT  : d_tmp[origin] = d_tmp[origin] & color16; break;
            }
            #else
            d_tmp[origin] = color16;
            #endif
         }
      }
      else
      {
         for (k = g_rect_left; k < length; ++k)
            dot(k + xx, y,color8);
      }
      if (odd) dot(xx + length, y, color8); /*补齐*/
      #else
      x1 += 0; y += 0; x2 += 0; color += 0; fix += 0;
      #endif
      return 0;
   }
   else
   {
      #ifndef NEO_color_depth_16_unused
      page = (char)((addr = ((long)y + g_sl_offset) * g_screen_h + x1) >> 15);
      if (page == ((addr + length) >> 15))
      {
         int far *d_tmp = (int far *)(g_videoptr + (unsigned)((addr << 1) & 0xffff));

         set_vbe_page(page);
         for (k = g_rect_left; k < len; ++k)
         {
            #ifndef NEO_draw_mode_unused
            switch(g_draw_mode)
            {
               case COPY_PUT : d_tmp[k] = color; break;
               case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ color; break;
               case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
               case OR_PUT   : d_tmp[k] = d_tmp[k] | color; break;
               case AND_PUT  : d_tmp[k] = d_tmp[k] & color; break;
            }
            #else
            d_tmp[k] = color;
            #endif
         }
      }
      else
      {
         for (k = g_rect_left; k < length; ++k)
            dot(k + xx, y, color);
      }
      #else
      x1 += 0; y += 0; x2 += 0; color += 0;
      #endif
      return 0;
   }
}

/*绘制一条垂直线的函数*/
void vline(int x, int y1, int y2,int color)
{
   int i;

   if (y1 > y2)
   {
      y1 ^= y2;
      y2 ^= y1;
      y1 ^= y2;
   }

   for (i = y1; i <= y2; ++i)
      dot(x, i, color);
}


#ifndef  NEO_hi_graphics_h_used
void  rect(int x0, int y0, int x1, int y1, int color)
{
   hline(x0, y0, x1, color);
   hline(x0, y1, x1, color);
   vline(x0, y0, y1, color);
   vline(x1, y0, y1, color);
}
#else
void  rect(int x0, int y0, int x1, int y1, int color)
{
   hline(x0, y0, x1, color);
   hline(x0, y1, x1, color);
   vline(x0, y0, y1, color);
   vline(x1, y0, y1, color);

}
#endif


#ifndef  NEO_hi_graphics_h_used
void  tri(int xa, int ya, int xb, int yb, int xc, int yc, int col)
{
   line(xa, ya, xb, yb,col);
   line(xb, yb, xc, yc,col);
   line(xc, yc, xa, ya,col);
}
#else
void  tri(int xa, int ya, int xb, int yb, int xc, int yc, int col)
{
   setcolor(col);
   line(xa, ya, xb, yb);
   line(xb, yb, xc, yc);
   line(xc, yc, xa, ya);
}
#endif


void rectfill(int x1, int y1, int x2, int y2, int color)
{
   int temp;

   if (x1 > x2)
   {
      temp = x1; x1 = x2; x2 = temp;
   }
   if (y1 > y2)
   {
      temp = y1; y1 = y2; y2 = temp;
   }

   for (temp=y1; temp<=y2; ++temp)
   {
      hline(x1, temp, x2, color);
   }
}


void ellipsefill(int center_x, int center_y, int stalk_long,int stalk_short, int fill_color)
{
    double tread = CF_DETAIL/*Detail of circle fill functions*/, fist = 0.0;
    double sit_x, sit_y;
    int dx, dy, x, y, storage_x, storage_y;

    storage_x = storage_y = -1;

    while(fist < 1.5706)
    {
       sit_x = stalk_long * cos(fist) + center_x;
       sit_y = stalk_short * sin(fist) + center_y;

       dx = sit_x * 10;
       dy = sit_y * 10;
    
       if(dx % 10 < 5)
           x = dx / 10;
       else
           x = dx / 10 + 1;

       if(dy % 10 < 5)
           y = dy / 10;
       else
           y = dy / 10 + 1;


       if(x != storage_x || y != storage_y)
       {
         #ifndef  NEO_hi_graphics_h_used
         line(x, y, (center_x<<1) - x, (center_y<<1) - y, fill_color);
         line(x, (center_y<<1) - y, (center_x<<1) - x, y, fill_color);
         #else
         setcolor(fill_color);
         line(x, y, (center_x<<1) - x, (center_y<<1) - y);
         line(x, (center_y<<1) - y, (center_x<<1) - x, y);
         #endif
       }

       storage_x = x;
       storage_y = y;
    
       fist += tread;
    }
}


#ifndef  NEO_hi_graphics_h_used
/*画圆函数，以(c_x, c_y)为中心画一半径为r的圆*/
void circle(int c_x, int c_y, int r, int color)
{
   int x = 0, y = r, d0, d1, d2, direction;
   /*x = 0; y = r;*/
   d0 = (1-r)<<1;

   while (y >= 0)
   {
      dot(c_x + x, c_y + y, color);
      dot(c_x - x, c_y - y, color);
      dot(c_x + x, c_y - y, color);
      dot(c_x - x, c_y + y, color);
      if (d0 < 0)
      {
    d1 = ((d0 + y) << 1) -1;
         if (d1 <= 0) direction = 1;
         else direction = 2;
      }
      else if (d0 > 0)
      {
    d2 = ((d0 - x) << 1) - 1;
         if (d2 <= 0) direction = 2;
         else direction = 3;
      }
      else direction = 2;

      switch(direction)
      {
        case 1 : ++x;
           d0 += (x<<1) + 1;
        break;

        case 2 : ++x;
           --y;
           d0+=(x-y+1)<<1;
        break;

        case 3 : --y;
           d0 += -(y<<1) + 1;
        break;
      }
   }
}
#else
void circle(int c_x, int c_y, int r)
{
   int x = 0, y = r, d0, d1, d2, direction;
   d0 = (1-r)<<1;

   while (y >= 0)
   {
      dot(c_x + x, c_y + y, g_frt_color);
      dot(c_x - x, c_y - y, g_frt_color);
      dot(c_x + x, c_y - y, g_frt_color);
      dot(c_x - x, c_y + y, g_frt_color);
      if (d0 < 0)
      {
         d1 = (d0 + y)<<1 -1;
         if (d1 <= 0) direction = 1;
         else direction = 2;
      }
      else if (d0 > 0)
      {
         d2 = (d0-x)<<1 - 1;
         if (d2 <= 0) direction = 2;
         else direction = 3;
      }
      else direction = 2;

      switch(direction)
      {
        case 1 : ++x;
           d0 += (x<<1) + 1;
        break;

        case 2 : ++x;
           --y;
           d0+=(x-y+1)<<1;
        break;

        case 3 : --y;
           d0 += -(y<<1) + 1;
        break;
      }
   }
}
#endif


/*填充的圆形,使用的是Midpoint算法*/
void circlefill(int x0, int y0, int r, int color)
{
   if (g_color_depth == 8)
   {
      int x;
      int y;
      int deltax;
      int deltay;
      int d;

      x = 0;
      y = r;
      deltax = 3;
      deltay = 2 - (r << 1);
      d = 1 - r;

      while (x <= y)
      {
        hline(x0-x, y0+y, x0+x, color);
        hline(x0-x, y0-y, x0+x, color);
        hline(x0-y, y0-x, x0+y, color);
        hline(x0-y, y0+x, x0+y, color);
        hline(x0-x, y0+y, x0+x, color);

        if(d<0)
        {
            d += deltax;
            deltax += 2;
            x++;
        }
        else
        {
            d += deltax+deltay;
            deltax += 2;
            deltay += 2;
            x++;
            y--;
         }
      }
   }
   else
   {
      ellipsefill(x0, y0, r, r, color);
   }
   
}


#ifndef  NEO_hi_graphics_h_used
/*以(x0,y0)为中心、a和b分别为水平和垂直半轴画一椭圆*/
void ellipse(int x0, int y0, int a, int b, int color)
{
   float d1, d2;
   int x, y;

   x=0; y=b;

   d1 = b * b + a * a * (-b + 0.25);
   dot(x0 + x, y0 + y, color);
   dot(x0 - x, y0 - y, color);
   dot(x0 - x, y0 + y, color);
   dot(x0 + x, y0 - y, color);

   while ((x + 1.0f) * b * b < (y - 0.5f) * a * a)
   {
      if (d1 < 0)
      {
         d1 += (2.0f * x + 3) * b * b;
         ++x;
      }
      else
      {
         d1 += (2.0f * x + 3) * b * b + (-2.0f * y + 2) * a * a;
         ++x; --y;
      }

      dot(x0 + x, y0 + y, color);
      dot(x0 - x, y0 - y, color);
      dot(x0 - x, y0 + y, color);
      dot(x0 + x, y0 - y, color);
   }
   d2 = (x + 0.5f) * b + (y - 1.0f) * a - a * b;
   while (y > 0)
   {
      if (d2 < 0)
      {
         d2 += (2.0f * x + 2) * b * b + (-2.0f * y + 3) * a * a;
         ++x; --y;
      }
      else
      {
         d2 += (-2.0f * y + 3) * a * a;
         --y;
      }

      dot(x0 + x, y0 + y, color);
      dot(x0 - x, y0 - y, color);
      dot(x0 - x, y0 + y, color);
      dot(x0 + x, y0 - y, color);
   }
}
#else
void ellipse(int x0, int y0, int a, int b)
{
   float d1, d2;
   int x, y;

   x=0; y=b;

   d1 = b * b + a * a * (-b + 0.25);
   dot(x0 + x, y0 + y, g_frt_color);
   dot(x0 - x, y0 - y, g_frt_color);
   dot(x0 - x, y0 + y, g_frt_color);
   dot(x0 + x, y0 - y, g_frt_color);

   while ((x + 1.0f) * b * b < (y - 0.5f) * a * a)
   {
      if (d1 < 0)
      {
         d1 += (2.0f * x + 3) * b * b;
         ++x;
      }
      else
      {
         d1 += (2.0f * x + 3) * b * b + (-2.0f * y + 2) * a * a;
         ++x; --y;
      }

      dot(x0 + x, y0 + y, g_frt_color);
      dot(x0 - x, y0 - y, g_frt_color);
      dot(x0 - x, y0 + y, g_frt_color);
      dot(x0 + x, y0 - y, g_frt_color);
   }
   d2 = (x + 0.5f) * b + (y - 1.0f) * a - a * b;
   while (y > 0)
   {
      if (d2 < 0)
      {
         d2 += (2.0f * x + 2) * b * b + (-2.0f * y + 3) * a * a;
         ++x; --y;
      }
      else
      {
         d2 += (-2.0f * y + 3) * a * a;
         --y;
      }

      dot(x0 + x, y0 + y, g_frt_color);
      dot(x0 - x, y0 - y, g_frt_color);
      dot(x0 - x, y0 + y, g_frt_color);
      dot(x0 + x, y0 - y, g_frt_color);
   }
}
#endif


void _dot(int x, int y, int color)
{
   long addr;
   int page;
   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 16);

      if (g_cur_vbe_page != page)
      {
         _BX=0;
         _DX=g_cur_vbe_page = page;
         _AX=0x4f05;
         __int__(0x10);
      }
      *(g_videoptr + (unsigned)(addr & 0xffff)) = (unsigned char)color;
      #else
      x = y = color;
      #endif
   }
   else /*if (g_v_mode == 1)*/
   {
      #ifndef NEO_color_depth_16_unused
      int far *videoptr = (int far *)MK_FP(0xa000, 0);
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);
      if (g_cur_vbe_page != page)
      {
         _BX=0;
         _DX=g_cur_vbe_page = page;
         _AX=0x4f05;
         __int__(0x10);
      }
      *(videoptr + (unsigned)(addr & 0xffff)) = color;
      #else
      x = y = color;
      #endif
   }
}
/*以上均上基本绘图函数*/
#endif


void clear_to_color(int color)
{
   #ifndef NEO_mouse_adv_unused
   if (_mouse_installed && (g_work_surface == g_look_surface))
   {
      scare_mouse();
   }
   #endif

   #ifndef NEO_rect_unused
   rect_store();
   #endif
   screen(0, 0, SCREEN_W, SCREEN_H);
   #ifndef NEO_basic_draw_unused
   rectfill(0, 0, SCREEN_W, SCREEN_H, color);
   #else
   {
      int x, y;
      for (y = 0; y < SCREEN_H; ++y)
         for (x = 0; x < SCREEN_W; ++x)
            dot(x, y, color);
   }
   #endif
   #ifndef NEO_rect_unused
   rect_restore();
   #endif

   #ifndef NEO_mouse_adv_unused
   if (_mouse_installed && (g_work_surface == g_look_surface))
   {
      unscare_mouse();
   }
   #endif
}


void clear(void)
{
   clear_to_color(0);
}


#endif
