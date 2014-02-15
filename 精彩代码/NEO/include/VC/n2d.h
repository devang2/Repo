/*NEO SDK V2.1.41 For DOS
  Copyleft Cker Home 2003-2005.

  Open Source Obey NEO_PL.TXT.
  http://neo.coderlife.net
  ckerhome@yahoo.com.cn

  文件名称 : n2d.h
  摘    要 : 本头文件包含了NEO SDK里的各种基本图形函数、结构、全局变量的声明及定义
  当前版本 : 2.77
  作    者 : 董凯
  完成日期 : 2005.6.28

  取代版本 : 2.76
  原 作 者 : 董凯
  完成日期 : 2005.6.22
*/
#ifndef  N2D_H
#define  N2D_H

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

#define closegraph()              set_vbe_mode(0x3)
#define getgraphmode()            get_vbe_mode()
#define setgraphmode(mode)        set_vbe_mode(mode)
#define setviewport(l, t, r, b,c) c?screen(l, t, r, b):0
#define getmaxx()                 max_x()
#define getmaxy()                 max_y()
#define setwritemode(mode)        set_draw_mode(mode)
#define setcolor(color)           (g_frt_color = color)
#define setbkcolor(color)         clear_to_color(color)
#define getcolor(color)           g_frt_color
#define setrgbpalette(cn, r, g,b) set_color(cn, r, g, b)
#define putpixel(x, y, color)     dot(x, y, color)
#define getpixel(x, y)            get_dot(x, y)
#define bar(x0, y0, x1, y1)       rectfill(x0-1, y0-1, x1-1, y1-1, g_frt_color)
#define rectangle(x0, y0, x1, y1) rect(x0, y0, x1, y1, g_frt_color)
#define textcolor(color)          set_str_color(color)
#define outtextxy(x, y, str)      textout(str, x, y)
#define setactivepage(page_num)   set_work_surface(page_num)
#define setvisualpage(page_num)   flip_surface(page_num)
#define setfillstyle(pattern, color)
#define cleardevice()             clear()
#endif

int g_vtotal, g_htotal;  /*滚屏例程使用*/

/*======================================================*
 *              函数声明(Function declare)              *
 *======================================================*/
void dot(int x,int y,int color);
int  get_dot(int x, int y);
#ifndef  NEO_hi_graphics_h_used
void line(int x1, int y1, int x2, int y2, unsigned color);
void circle(int c_x, int c_y, int r, unsigned char color);
void ellipse(int x0, int y0, int a, int b, unsigned char color);
#else
void line(int x1, int y1, int x2, int y2);
void circle(int c_x, int c_y, int r);
void ellipse(int x0, int y0, int a, int b);
#endif
char hline(int x, int y, int width, unsigned color);
void vline(int x, int y, int height,unsigned color);
void rect(int x0, int y0, int x1, int y1, int color);
void tri(int xa, int ya, int xb, int yb, int xc, int yc, int col);
void rectfill(int x1, int y1, int x2, int y2, int color);
void circlefill(int x, int y, int r, int c);
void _dot(int x,int y,int color);
void clear(void);
void clear_to_color(int color);


/*通用画点函数,速度:17FPS*/
void dot(int x,int y,int color)
{
   long addr;
   int page;

   if ((x<g_rect_left) || (y<g_rect_top) || (x>g_rect_right) || (y>g_rect_bottom)) return;

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 16);

      if (g_cur_vbe_page != page)
      {
		#ifndef MSVC15
         _BX = 0;
         _DX = g_cur_vbe_page = page;
         _AX = 0x4f05;
         __int__(0x10);
		#else
		 g_cur_vbe_page = page;
		 _asm
		  {
		  mov bx,0
		  mov dx,g_cur_vbe_page 
		  mov ax,4f05h
		  int 10h
		  }
	    #endif
      }
      #ifndef NEO_draw_mode_unused
      switch(g_draw_mode)
      {
         case COPY_PUT : *(g_videoptr+(addr&0xffff))= color;break;
         case XOR_PUT  : *(g_videoptr+(addr&0xffff))= *(g_videoptr+(addr& 0xffff))^(char)color;break;
         case NOT_PUT  : *(g_videoptr+(addr&0xffff))=~*(g_videoptr+(addr& 0xffff));break;
         case OR_PUT   : *(g_videoptr+(addr&0xffff))= *(g_videoptr+(addr& 0xffff))|(char)color;break;
         case AND_PUT  : *(g_videoptr+(addr&0xffff))= *(g_videoptr+(addr& 0xffff))&(char)color;break;
      }
      #else
      *(g_videoptr+(addr& 0xffff)) = (unsigned char)color;
      #endif

      #else
      x += 0; y+=0; color+=0;
      #endif
   }
   else /*if (g_color_depth == 16)*/ /*由于NEO目前只支持256和64K色,所以暂时注释左边一句*/
   {
      #ifndef NEO_color_depth_16_unused
      int far *videoptr16 = (int far *)0xa0000000;
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);

      if (g_cur_vbe_page != page)
      {
		#ifndef MSVC15
         _BX = 0;
         _DX = g_cur_vbe_page = page;
         _AX = 0x4f05;
         __int__(0x10);
		#else
		 g_cur_vbe_page = page;
		 _asm
		  {
		  mov bx,0
		  mov dx,g_cur_vbe_page 
		  mov ax,4f05h
		  int 10h
		  }
	     #endif
      }
      #ifndef NEO_draw_mode_unused
      switch(g_draw_mode)
      {
         case COPY_PUT : *(videoptr16+(addr&0xffff)) = color;break;
         case XOR_PUT  : *(videoptr16+(addr&0xffff))= *(videoptr16+(addr& 0xffff)) ^ color;break;
         case NOT_PUT  : *(videoptr16+(addr&0xffff))=~*(videoptr16+(addr& 0xffff)); break;
         case OR_PUT   : *(videoptr16+(addr&0xffff))= *(videoptr16+(addr& 0xffff)) | color;break;
         case AND_PUT  : *(videoptr16+(addr&0xffff))= *(videoptr16+(addr& 0xffff)) & color;break;
      }
      #else
      *(videoptr16 + (addr & 0xffff)) = color;
      #endif

      #else
      x += 0; y+=0; color+=0;
      #endif
   }
}


int  get_dot(int x, int y)
{
   long addr;
   int page;
   if ((x<0) || (y<0) || (x>g_screen_h) || (y>g_screen_v)) return -1;

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 16);
      if (g_cur_vbe_page != page)
      {
		#ifndef MSVC15
         _BX = 0;
         _DX = g_cur_vbe_page = page;
         _AX = 0x4f05;
         __int__(0x10);
		#else
		 g_cur_vbe_page = page;
		 _asm
		  {
		  mov bx,0
		  mov dx,g_cur_vbe_page 
		  mov ax,4f05h
		  int 10h
		  }
	     #endif
      }
      return (peekb(0xa000,addr & 0xFFFF));
      #else
      x += 0; y+=0;
      #endif
   }
   else
   {
      #ifndef NEO_color_depth_16_unused
      int far *videoptr = (int far *)0xa0000000;
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);

      if (g_cur_vbe_page != page)
      {
		#ifndef MSVC15
         _BX = 0;
         _DX = g_cur_vbe_page = page;
         _AX = 0x4f05;
         __int__(0x10);
		#else
		 g_cur_vbe_page = page;
		 _asm
		  {
		  mov bx,0
		  mov dx,g_cur_vbe_page 
		  mov ax,4f05h
		  int 10h
		  }
	     #endif
      }
      return *(videoptr + (addr & 0xffff));
      #else
      x += 0; y+=0;
      #endif
   }
}


#ifndef NEO_basic_draw_unused
#ifndef  NEO_hi_graphics_h_used
/*通用的画线函数，用color颜色由(x1,y1)画到(x2,y2);速度:(16FPS)*/
void line(int x1, int y1, int x2, int y2, unsigned color)
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

/*优化后的画水平线函数，用color颜色由(x,y)向右画长为width的水平线;速度: 8位65FPS; 16位32FPS*/
char hline(int x, int y, int width, unsigned color)
{
   long addr;
   int  k = 0;
   int  xx = x;
   int  rect_scr_h = g_rect_right + 1;
   int  len = 0, length/*直线有效长度*/ = 0;
   char page, odd;
   int  fix;

   if (width < 0)
   {
      x -= width;
      width = -width;
   }
   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      int origin = 0;
      unsigned char color8 = (unsigned char)color;
      unsigned int  color16 = color8 << 8;

      if ( (y > g_rect_bottom) || (y < g_rect_top) || width < 0)
      {
         return -1;
      }
      if (x >= 0 && x < rect_scr_h)
      {
         if (rect_scr_h - x >= width) /*直线完全显示*/
         {
            fix = (x <= g_rect_left? x>>1 : g_rect_left>>1);
            len = length = width + 1;
         }
         else                         /*直线右边溢出*/
         {
            if (x > g_rect_left)
            {
               x = x - g_rect_left;
            }
            else fix = x>>1;
            len = length = rect_scr_h - x + 1;
         }
      }
      else if (x < 0 && x > (-width))
      {
         if (width + x >= rect_scr_h)  /*直线两端溢出*/
         {
            len = length = rect_scr_h + 1;
            xx = x = 0;
         }
         else                          /*直线左边溢出*/
         {
            len = (length = width + 1) + x;
            x = 0;
         }
      }
      else
      {
         return -1;
      }

      color16 += color8;
      len >>= 1;
      length -= (odd = length % 2);
      origin = (g_rect_left >> 1) - fix;
      page = (char)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 16);
      if (page == (addr + length) >> 16)
      {
         int far *d_tmp = (int far *)(g_videoptr + (addr & 0xffff));

         if (g_cur_vbe_page != page)
         {
            set_vbe_page(page);
         }
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
         for (;k < length; ++k)
            dot(k + xx, y,color8);
      }
      if (odd) dot(xx + length, y, color8);
      #else
      x += 0; y += 0; width += 0; color += 0;
      #endif
      return 0;
   }
   else
   {
      #ifndef NEO_color_depth_16_unused
      if ( (y > g_rect_bottom) || (y < g_rect_top) || width < 0)
      {
         return -1;
      }
       if (x >= 0 && x < rect_scr_h)
      {
         if (rect_scr_h - x >= width) /*直线完全显示*/
         {
            len = length = width + 1;
         }
         else                         /*直线右边溢出*/
         {
            /*if (x > g_rect_left)
            {
               x = x - g_rect_left;
            }*/
            len = length = rect_scr_h - x + 1;
         }
      }
      else if (x < 0 && x > (-width))
      {
         if (width + x >= rect_scr_h)  /*直线两端溢出*/
         {
            len = length = rect_scr_h + 1;
            xx = x = 0;
         }
         else                          /*直线左边溢出*/
         {
            len = (length = width + 1) + x;
            x = 0;
         }
      }
      else
      {
         return -1;
      }

      page = (char)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);
      if (page == (addr + length) >> 15)
      {
         int far *d_tmp = (int far *)(g_videoptr + ((addr<<1) & 0xffff));

         if (g_cur_vbe_page != page)
         {
            set_vbe_page(page);
         }
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
         for (k = g_rect_left;k < length; ++k)
            dot(k + xx, y, color);
      }
      #else
      x += 0; y += 0; width += 0; color += 0;
      #endif
      return 0;
   }
}


#ifndef  NEO_hi_graphics_h_used
void  rect(int x0, int y0, int x1, int y1, int color)
{
   line(x0,y0,x0,y1,color);
   line(x0,y0,x1,y0,color);
   line(x0,y1,x1,y1,color);
   line(x1,y1,x1,y0,color);
}
#else
void  rect(int x0, int y0, int x1, int y1, int color)
{
   setcolor(color);
   line(x0, y0, x0, y1);
   line(x0, y0, x1, y0);
   line(x0, y1, x1, y1);
   line(x1, y1, x1, y0);

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
   int delta, temp;

   if (x1 > x2)
   {
      temp = x1; x1 = x2; x2 = temp;
   }
   if (y1 > y2)
   {
      temp = y1; y1 = y2; y2 = temp;
   }
   delta = x2 - x1;
   for (temp=y1; temp<=y2; ++temp)
   {
      hline(x1, temp, delta, color);
   }
}


#ifndef  NEO_hi_graphics_h_used
/*画圆函数，以(c_x, c_y)为中心画一半径为r的圆*/
void circle(int c_x, int c_y, int r, unsigned char color)
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


#ifndef  NEO_hi_graphics_h_used
void circlefill(int x, int y, int r, int c)
{
   int i;

   for (i = 0; i < r; ++i)
   {
      circle(x, y, i, c);
   }
}
#else
void circlefill(int x, int y, int r, int c)
{
   int i;
   setcolor(c);
   for (i = 0; i < r; ++i)
   {
      circle(x, y, i);
   }
}
#endif


#ifndef  NEO_hi_graphics_h_used
/*以(x0,y0)为中心、a和b分别为水平和垂直半轴画一椭圆*/
void ellipse(int x0,int y0,int a,int b,unsigned char color)
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
void ellipse(int x0,int y0,int a,int b)
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
		#ifndef MSVC15
         _BX = 0;
         _DX = g_cur_vbe_page = page;
         _AX = 0x4f05;
         __int__(0x10);
		#else
		 g_cur_vbe_page = page;
		 _asm
		  {
		  mov bx,0
		  mov dx,g_cur_vbe_page 
		  mov ax,4f05h
		  int 10h
		  }
	     #endif
      }
      *(g_videoptr+(addr& 0xffff)) = (unsigned char)color;
      #else
      x = y = color;
      #endif
   }
   else /*if (g_v_mode == 1)*/
   {
      #ifndef NEO_color_depth_16_unused
      int far *videoptr = (int far *)0xa0000000;
      page = (int)((addr = ((long)y + g_sl_offset) * g_screen_h + x) >> 15);
      if (g_cur_vbe_page != page)
      {
		#ifndef MSVC15
         _BX = 0;
         _DX = g_cur_vbe_page = page;
         _AX = 0x4f05;
         __int__(0x10);
		#else
		 g_cur_vbe_page = page;
		 _asm
		  {
		  mov bx,0
		  mov dx,g_cur_vbe_page 
		  mov ax,4f05h
		  int 10h
		  }
	     #endif
      }
      *(videoptr + (addr & 0xffff)) = color;
      #else
      x = y = color;
      #endif
   }
}
#endif


void clear_to_color(int color)
{


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

}


void clear(void)
{
   clear_to_color(0);
}


#endif
