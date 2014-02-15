/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : ndraw.h
  摘    要 : 本头文件包含了NEO SDK里有关显卡硬件加速及显存、屏幕底层处理的函数、结构、全局
             变量的声明及定义
  当前版本 : 2.46
  作    者 : 董凯
  完成日期 : 2006.12.29

  取代版本 : 2.45
  原 作 者 : 董凯
  完成日期 : 2006.12.05
*/
#ifndef  __NCOLOR_H__
#define  __NCOLOR_H__

/*#define NEO_INT_MAX   2147483647L
#define NEO_INT_MIN   (-NEO_INT_MAX - 1)*/

/* dark colors */
#define _BLACK        makecol(0, 0, 0)
#define _BLUE         makecol(0, 0, 128)
#define _GREEN        makecol(0, 128, 0)
#define _CYAN         makecol(0, 128, 128)
#define _RED          makecol(128, 0, 0)
#define _MAGENTA      makecol(128, 0, 128)
#define _BROWN        makecol(128, 0, 64)
#define _LIGHTGRAY    makecol(192, 192, 192)
#define _DARKGRAY     makecol(128, 128, 128)
/* light colors */
#define _LIGHTBLUE    makecol(0, 0, 255)
#define _LIGHTGREEN   makecol(0, 255, 0)
#define _LIGHTCYAN    makecol(0, 255, 255)
#define _LIGHTRED     makecol(255, 0, 0)
#define _LIGHTMAGENTA makecol(255, 0, 255)
#define _YELLOW       makecol(255, 255, 0)
#define _WHITE        makecol(255, 255, 255)
/*#define _WHIIE        makecol(255, 255, 255)*/
#ifndef MASK_COLOR_
#define MASK_COLOR_   g_tptcolor
#endif

#define retrace_count neo_clock()

char g_dac_size_fix = 2;    /*记录DAC调色板宽度修正值，由8-当前DAC宽度得来，默认为2*/

Uint8 get_rgb(int index, int flags);
void set_neo_color(void);
void make_332_palette(PALETTE pal);
void set_332_palette(void);
char palette_cmp(PALETTE pal1, PALETTE pal2);
char is_332_palette(void);
Uint8 bestfit_color(PALETTE pal, int r, int g, int b);
void set_palette_range(char *pal_buf, int from, int to, int vsync_flag);

void set_color(int index, char red, char green, char blue);
void get_color(int index, char *red, char *green, char *blue);
void set_dac_size(Uint8 size);
char get_dac_size(void);
void set_palette(char *pal_buf);
void _set_palette(PALETTE pal_buf);
void get_palette(char *pal_buf);
void _get_palette(PALETTE pal_buf);
Uint8 makecol8(int r, int g, int b);

void (*fade_callback)() = NULL;
void fade_interpolate(PALETTE source, PALETTE dest, PALETTE output, int pos, int from, int to);
void fade_from_range(PALETTE source, PALETTE dest, int speed, int from, int to);
void fade_in_range(PALETTE p, int speed, int from, int to);
void fade_out_range(int speed, int from, int to);
void _fade_in(PALETTE p, int speed);
void fade_in(PALETTE p, Uint8 dest_color, int speed);
void _fade_out(int speed);
void fade_out(Uint8 dest_color, int speed);
int  makecol(int r, int g, int b);
#define bestfit_col8(pal, r, g, b) bestfit_color(pal, r, g, b)
#define bestfit_col16(pal, r, g, b) bestfit_color(pal, (r>>2), (g>>2), (b>>2))
#define makecol16(r, g, b)  ((((((r)>>3)<<g_green_mask)+((g)>>g_green_bit))<<5)+((b)>>3))
#define makecol15(r, g, b)  makecol16(r,g,b)

#ifndef NEO_color_depth_8_unused
#ifndef NEO_palette_unused

#define _set_palette_range(pal, from, to, vsync_f) set_palette_range((char *)pal, from, to, vsync_f)
#define makecol8(r, g, b)   bestfit_col16(g_sys_palette, r, g, b)
#define get_red(i)          (g_sys_palette[i].r)
#define get_green(i)        (g_sys_palette[i].g)
#define get_blue(i)         (g_sys_palette[i].b)
#endif

#define getr8(c)            get_rgb(i, 0)
#define getg8(c)            get_rgb(i, 1)
#define getb8(c)            get_rgb(i, 2)
#endif

#define getr15(c)           (((c)&0x7c00)>>10)
#define getg15(c)           (((c)&0x3e0) >>5)
#define getb15(c)           ((c)&0x1f)
#define getr16(c)           (((c)&0xf800)>>11)
#define getg16(c)           (((c)&0x7e0) >>5)
#define getb16(c)           ((c)&0x1f)
#define getr24(c)           (((c)&0xff0000)>>16)
#define getg24(c)           (((c)&0xff00)>>8)
#define getb24(c)           (c&0xff)
#define getr(i)             get_rgb(i, 0)
#define getg(i)             get_rgb(i, 1)
#define getb(i)             get_rgb(i, 2)


/*flags标志：为0返回红色分量，为1返回绿色分量，为2返回蓝色分量*/
Uint8 get_rgb(int index, int flags)
{
   Uint8 rgb[3];

   /*对不同的颜色深度进行分别处理*/
   switch(get_color_depth())
   {
      case 8:
      #ifndef NEO_color_depth_8_unused
      #ifndef NEO_palette_unused
         get_palette_range((char *)rgb, index, index);
      #endif
      #endif
      break;
      case 15:
         rgb[0] = getr15(index);
         rgb[1] = getg15(index);
         rgb[2] = getb15(index);
      break;
      case 16:
           /* fall through */
      default:
         rgb[0] = getr16(index);
         rgb[1] = getg16(index);
         rgb[2] = getb16(index);
      break;
   }

   return rgb[flags];
}


int makecol(int r, int g, int b)
{
   switch (get_color_depth())
   {
      case 8:
      #ifndef NEO_color_depth_8_unused
      return makecol8(r, g, b);
      #endif
      case 15:
           /* fall through */
      case 16:
           /* fall through */
      default:
      return makecol16(r, g, b);
   }
}


#ifndef NEO_color_depth_8_unused
#ifndef NEO_palette_unused

#define set_palette(pal_buf) set_palette_range(pal_buf, 0, 255, TRUE)
#define _set_palette(pal)    _set_palette_range(pal, 0, 255, TRUE)
#define get_palette(pal_buf) get_palette_range(pal_buf, 0, 255)
#define _get_palette(pal)   _get_palette_range(pal, 0, 255)

/* make_332_palette:
 *  Used when loading a truecolor image into an 8 bit bitmap, to generate
 *  a 3.3.2 RGB palette.Just adapt a 6 bits DAC.
 */
void make_332_palette(PALETTE pal)
{
   int r, g, b, k = 0;

   for(r = 0; r < 64; r += 9)
   {
      for(g = 0; g < 64; g += 9)
      {
         for(b = 0; b < 64; b += 21)
         {
            pal[k].r = r;
            pal[k].g = g;
            pal[k].b = b;
            k++;
         }
      }
   }
}


char palette_cmp(PALETTE pal1, PALETTE pal2)
{
   int *x = (int *)pal1;
   int *y = (int *)pal2;
   int i;

   for (i = 0; i < 384; ++i)
   {
      if (x[i] != y[i])
      {
         return FALSE;
      }
   }
   return TRUE;
}


char is_332_palette(void)
{
   PALETTE pal332;

   make_332_palette(pal332);
   if (palette_cmp(pal332, g_sys_palette))
   {
      return (common_flags |= 0x1);
   }
   else
   {
      return (common_flags &= 0xfffe);
   }
}


void set_palette_range(char *pal_buf, int from, int to, int vsync_flag)
{
   struct REGPACK reg;
   int    i;
   char   *syspal = (char *)g_sys_palette;

   if (from <= to && from >= 0 && to < 256)
   {
      /*同步到内存中的调色板映射*/
      if (from == 0 && to == 255)
      {
         memcpy((char *)g_sys_palette, (char *)pal_buf, 768);
      }
      else
      {
         for (i = from * 3; i <= to * 3; i += 3)
         {
            syspal[i] = pal_buf[i];
            syspal[i + 1] = pal_buf[i + 1];
            syspal[i + 2] = pal_buf[i + 2];
         }
      }
      if (vsync_flag) vsync();
      reg.r_ax = 0x1012;
      reg.r_bx = from;
      reg.r_cx = to - from + 1;
      reg.r_es = FP_SEG(pal_buf);
      reg.r_dx = FP_OFF(pal_buf);
      intr(0x10, &reg);

      is_332_palette();
   }
}


void get_palette_range(char *pal_buf, int from, int to)
{
   struct REGPACK regs;
   if (from <= to && from >= 0 && to < 256)
   {
      regs.r_ax = 0x1017;
      regs.r_bx = from;
      regs.r_cx = to - from + 1;
      regs.r_es = FP_SEG(pal_buf);
      regs.r_dx = FP_OFF(pal_buf);
      intr(0x10, &regs);
   }
}


/*---------------------------------------------------*
 *函数功能: 设置硬件调色板中指定颜色号的颜色分量.    *
 *参数说明: index为颜色号.                           *
 *          red,green,blue为此颜色号对应的红,绿,兰三 *
 *          原色分量,均应小于64)                     *
 *返回说明:                                          *
 *备    注: 颜色设置函数.用户程序可直接调用.  　     *
 *---------------------------------------------------*/
void set_color(int index, char red, char green, char blue)
{
   char color[3];

   g_sys_palette[index].r = color[0] = red;
   g_sys_palette[index].g = color[1] = green;
   g_sys_palette[index].b = color[2] = blue;
   set_palette_range(color, index, index, FALSE);
}


void get_color(int index, char *red, char *green, char *blue)
{
   char color[3];

   get_palette_range(color, index, index);
   *red  = color[0];
   *green= color[1];
   *blue = color[2];
}


void set_332_palette(void)
{
   PALETTE pal332;   
   make_332_palette(pal332);
   _set_palette_range(pal332, 0, 255, FALSE);
   common_flags |= 0x1;
}

/*从332调色板中匹配最佳色*/
#define SRGB332(r, g, b) ((((r) / 9) << 5) + (((g) / 9)<<2) + (b) / 21)
/*从332调色板中匹配最佳色（改进版）*/
#define ARGB332(r, g, b) ((((r + 4) / 9) << 5) + (((g + 4) / 9)<<2) + (b + 10) / 21)
#define GRGB332(i, pr, pg, pb) {              \
*(pr) = (((i>>5) % 8) << 3) + ((i>>5) % 8);   \
*(pg) = (((i>>2) % 8) << 3) + ((i>>2) % 8);   \
(*pb) = ((i % 4)<<4) + ((i % 4)<<2) + (i % 4);\
} 

/* bestfit_color:
 * 从指定调色板pal中搜索与给出颜色分量r,g,b最匹配的颜色号并将其返回
   参数r,g,b范围为0~63；
 */
Uint8 bestfit_color(PALETTE pal, int r, int g, int b)
{
    int mind;
    int newd;
    int color;
    int i;

   /*如果使用了332伪真彩调色板*/
   if (common_flags & 0x1)
   {
      return ARGB332(r, g, b);
   }
   else
   {
      color = 0;
      mind  = abs(pal[0].r - r) + abs(pal[0].g - g) + abs(pal[0].b - b);

      for (i=1; i<256; i++)
      {
         /*通过比较色差总和选择最佳匹配色*/
         newd = abs(pal[i].r - r) + abs(pal[i].g - g) + abs(pal[i].b - b);
         if (newd<mind)
         {
            mind  = newd;
            color = i;
         }
      }
   }

    return color;
}


void set_dac_size(Uint8 size)
{
   _AX = 0x4f08;
   _BL = 0;
   _BH = size;
   __int__(0x10);
   g_dac_size_fix = ((size == 6) || (size == 8)) ? 8 - size : 2;
}

char get_dac_size(void)
{
   _AX = 0x4f08;
   _BL = 1;
   __int__(0x10);
   return 8 - (g_dac_size_fix = 8 - _BH);
}


/*---------------------------------------------------*
 *函数功能: 将硬件调色板转换为本SDK推荐的逻辑调色板. *
 *参数说明: 无.                                      *
 *返回说明: 无.                　　                  *
 *备    注: 本SDK推荐的调色板是由经典游戏"仙剑奇侠传"*
 *          的调色板经过分类排序等修改而来的,选色专业*
 *          ,颜色齐,覆盖性好;一般匹配其他图片可以达到*
 *          90%以上的近似效果.所以本SDK将其定为推荐调*
 *          色盘.用户程序可直接调用.          　     *
 *---------------------------------------------------*/
void set_neo_color(void)
{
#ifndef NEO_sys_pal_unused
char rgb[768] = {
0,0,0,6,6,6,10,10,10,14,14,14,18,18,18,22,22,22,26,26,26,30,30,30,34,34,34,38,38,38,42,42,42,\
46,46,46,50,50,50,54,54,54,59,59,59,63,63,63,20,0,0,23,0,0,28,0,0,33,1,1,38,2,2,47,4,3,54,6, \
5,63,0,0,63,0,0,63,14,11,63,18,15,63,22,19,63,22,18,63,32,28,63,37,33,63,43,39,18,5,2,21,6,3,\
24,6,3,27,6,3,31,10,4,36,15,7,40,20,9,44,25,11,48,30,15,53,36,18,56,41,21,60,46,24,63,50,28, \
63,55,33,63,60,39,63,62,44,12,6,0,22,15,0,32,25,0,41,34,1,48,43,1,57,50,1,59,56,1,62,62,1,62,\
62,21,63,63,0,63,63,10,63,63,19,63,63,28,63,63,41,63,63,49,63,63,60,5,3,2,7,5,3,10,7,5,13,10,\
7,16,13,9,18,16,11,21,19,14,24,22,16,27,25,19,29,28,21,32,31,24,35,34,28,38,37,31,41,40,36,  \
45,44,40,49,46,43,0,0,15,0,0,19,1,1,27,2,2,32,3,3,37,2,2,41,3,3,46,0,3,51,0,0,58,0,0,63,7,7,
63,14,14,63,21,21,63,28,28,63,38,38,63,42,42,63,7,4,14,9,5,17,11,6,20,13,8,23,15,10,26,18,12,
29,21,14,32,24,17,35,27,20,38,31,23,41,34,27,44,38,31,47,42,35,50,46,40,53,50,44,56,54,49,59,
5,9,10,7,11,13,9,14,16,11,17,19,13,20,22,16,23,25,19,26,28,22,30,32,26,33,35,30,37,39,33,40,
42,38,44,46,42,48,50,47,52,53,52,56,57,57,60,61,0,4,2,0,8,5,0,11,8,1,15,11,2,19,15,4,23,19,6,
26,22,8,30,26,11,34,29,13,38,33,17,42,37,21,46,41,26,50,45,31,54,49,36,58,52,42,62,57,23,10,
6,28,14,8,33,18,10,36,23,13,40,28,16,43,32,19,45,33,21,47,33,24,49,33,27,50,36,30,52,39,33,
54,42,37,55,45,40,57,48,43,58,51,47,60,54,50,12,4,0,16,6,0,21,8,1,24,10,2,27,12,3,30,15,6,33,
18,8,36,21,11,39,25,14,42,29,17,45,33,21,48,37,25,51,41,30,55,46,35,59,50,41,63,56,48,9,3,1,
12,5,2,15,7,3,18,9,4,22,13,7,25,16,9,28,19,12,32,23,15,35,28,18,39,32,22,42,36,27,46,41,31,
49,45,36,53,50,42,57,55,48,61,60,55,0,7,0,0,9,0,1,12,0,2,15,1,4,19,2,6,24,3,8,31,5,11,39,7,
14,45,9,0,49,0,6,49,0,11,49,0,19,49,19,26,49,22,32,49,26,28,49,31,0,6,23,0,8,27,1,10,31,3,12,
35,5,15,40,7,17,44,10,20,48,13,23,50,17,27,52,21,31,55,25,35,56,30,39,58,35,43,59,39,47,60,
44,51,61,50,55,63,9,5,3,12,7,4,15,10,6,18,13,8,22,17,11,25,20,13,28,23,16,31,27,19,34,30,22,
37,34,26,40,37,30,44,41,34,47,45,38,50,49,42,53,52,46,56,55,50, 0,0,0,32,0,0,0,32,0,32,32,
0,0,0,32,32,0,32,0,32,32,32,32,32,48,48,48,63,0,0,0,63,0,63,63,0,0,0,63,63,0,63,0,63,
63,63,63,63
};
   set_palette(rgb);

#else
#ifndef NEO_sys_report_error_unused
{
   Errinfo_t error = {"set_neo_color", ROUTINE_UNACTIVE, 1};
   throw_error(error);
}
#endif

#endif
}
#define set_neo_palette() set_neo_color()

/*#ifndef NEO_color_depth_8_unused*/
#ifndef NEO_fade_pal_unused
void fade_interpolate(PALETTE source, PALETTE dest, PALETTE output, int pos, int from, int to)
{
   int c;

   for (c=from; c<=to; c++) { 
      output[c].r = ((int)source[c].r * (63-pos) + (int)dest[c].r * pos) >> 6;
      output[c].g = ((int)source[c].g * (63-pos) + (int)dest[c].g * pos) >> 6;
      output[c].b = ((int)source[c].b * (63-pos) + (int)dest[c].b * pos) >> 6;
   }
}

void fade_from_range(PALETTE source, PALETTE dest, int speed, int from, int to)
{
   PALETTE temp;
   int c, start, last;

   for (c=0; c<_PAL_SIZE; c++)
      temp[c] = source[c];

   #ifndef NEO_timer_unused
   if (_timer_installed)
   {
      start = retrace_count;
      last = -1;

      while ((c = (retrace_count - start) * (speed << 3)) < 64)
      {
         if (c != last)
         {
            fade_interpolate(source, dest, temp, c, from, to);
            #ifndef NEO_draw_smooth_unused
            _set_palette_range(temp, from, to, TRUE);
            #else
            _set_palette_range(temp, from, to, FALSE);
            #endif
            if (fade_callback) (*fade_callback)();
            last = c;
         }
      }
   }
   else
   {
      speed = speed <= 0?1 : speed;
      for (c=0; c < 64; c += speed)
      {
         fade_interpolate(source, dest, temp, c, from, to);
         #ifndef NEO_draw_smooth_unused
         _set_palette_range(temp, from, to, TRUE);
         #else
         _set_palette_range(temp, from, to, FALSE);
         #endif
         if (fade_callback) (*fade_callback)();
      }
   }
   #else
   speed = speed <= 0?1 : speed;
   for (c=0; c < 64; c += speed)
   {
      fade_interpolate(source, dest, temp, c, from, to);
      #ifndef NEO_draw_smooth_unused
      _set_palette_range(temp, from, to, TRUE);
      #else
      _set_palette_range(temp, from, to, FALSE);
      #endif
      if (fade_callback) (*fade_callback)();
   }
   #endif

   #ifndef NEO_draw_smooth_unused
   _set_palette_range(temp, from, to, TRUE);
   #else
   _set_palette_range(temp, from, to, FALSE);
   #endif
}


void fade_in_range(PALETTE p, int speed, int from, int to)
{
   PALETTE black_pal;
   memset(black_pal, 0, 768);
   fade_from_range(black_pal, p, speed, from, to);
}

void fade_out_range(int speed, int from, int to)
{
   PALETTE temp;
   PALETTE black_pal;

   _get_palette(temp);
   memset(black_pal, 0, 768);
   fade_from_range(temp, black_pal, speed, from, to);
}

#define _fade_in(pal, speed) fade_in_range(pal, speed, 0, _PAL_SIZE - 1)
#define _fade_out(speed)     fade_out_range(speed, 0, _PAL_SIZE - 1)


void fade_in(PALETTE p, Uint8 dest_color, int speed)
{
   PALETTE black_pal;
   RGB24  temp_rbg;
   unsigned int i;

   temp_rbg.r = getr(dest_color);
   temp_rbg.g = getg(dest_color);
   temp_rbg.b = getb(dest_color);

   for (i=0; i<256; i++)
      black_pal[i] = temp_rbg;
   _fade_in(p, speed);
}

void fade_out(Uint8 dest_color, int speed)
{
   PALETTE black_pal;
   RGB24  temp_rbg;
   unsigned int i;

   temp_rbg.r = getr(dest_color);
   temp_rbg.g = getg(dest_color);
   temp_rbg.b = getb(dest_color);

   for (i=0; i<256; i++)
      black_pal[i] = temp_rbg;
   _fade_out(speed);
}
#endif
#endif
#endif


#endif
