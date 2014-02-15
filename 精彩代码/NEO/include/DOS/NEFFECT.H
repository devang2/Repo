/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : neffect.h
  摘    要 : 本头文件中包含了NEO SDK里有关各种图形文件特效处理的函数、结构、全局变量的声明定义

  当前版本 : V0.36
  作    者 : 董凯 
  完成日期 : 2006.01.17

  取代版本 : V0.29
  原 作 者 : 董凯 
  完成日期 : 2006.01.17
*/

#ifndef __NEFFECT_H__
#define __NEFFECT_H__

#ifndef NEO_bmp_support_unused
#ifndef NEO_showbmp_unused

int  g_bmp_wid;
int  g_bmpheight;
char g_bmpbits;

void scan_line_broken(void *buffer, int count/*, int line*/);
void scan_line_h_flip(void *buffer, int count/*, int line*/);
void scan_line_wublur(void *buffer, int count/*, int line*/);
void scan_line_stead(void *buffer, int count/*, int line*/);
void scan_line_vivid(void *buffer, int count/*, int line*/);
void scan_line_bright(void *buffer, int count/*, int line*/);
void scan_line_jump(void *buffer, int count/*, int line*/);
void scan_line_roloc(void *buffer, int count/*, int line*/);
void scan_line_throw(void *buffer, int count/*, int line*/);
/*void scan_line_hue(void *buffer, int count);*/

/*扫描线虚线化*/
void scan_line_broken(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_muslin_unused
   static int temp;
   int    t;
   int    i;

   if (g_bmpbits == 8)
   {
      #ifndef NEO_8bit_bmp_unused
      char *buf = (char *)buffer;
      int  mask = (get_color_depth() == 8? MASK_COLOR_ : MASK_COLOR_8);

      for (i = temp; i < count; i+=2)
      {
         buf[i] = mask;
      }
      #endif
   }
   #ifndef NEO_24bit_bmp_unused
   else if (g_bmpbits == 24)
   {
      RGB24 *buf = (RGB24 *)buffer;

      t = 5 - g_green_bit;
      count /= sizeof(RGB24); /*调整count的数值，以避免缓冲区溢出*/
      for (i = temp; i < count; i+=2)
      {
         buf[i].r = (MASK_COLOR_ & 0xf800) >> 8;
         buf[i].g = (MASK_COLOR_ & 0x7e0) >> t;
         buf[i].b = (MASK_COLOR_ & 0x1f) << 3;
      }
   }
   #endif
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      RGB16 *buf = (RGB16 *)buffer;
      count >>= 1; /*比count /= sizeof(RGB16)快*/
      for (i = temp; i < count; i+=2)
      {
         buf[i].color = MASK_COLOR_;
      }
   }
   #endif

   if (!g_bmp_param) /*如果g_bmp_param被置位，则产生隔列扫描效果*/
   {
      temp = (temp ? 0 : 1); /*交错系数*/
   }
   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"broken", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线反色效果*/
void scan_line_roloc(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_roloc_unused
   int i;

   if (g_bmpbits == 8)
   {
      #ifndef NEO_8bit_bmp_unused
      #ifndef NEO_color_depth_8_unused
      char *buf = (char *)buffer;

      for (i = 0; i < count; ++i)
      {
         buf[i] = ~buf[i];
      }
      #endif
      #endif
   }
   #ifndef NEO_24bit_bmp_unused
   else if (g_bmpbits == 24)
   {
      RGB24 *buf = (RGB24 *)buffer;

      count /= sizeof(RGB24);
      for (i = 0; i < count; ++i)
      {
         buf[i].r = ~buf[i].r;
         buf[i].g = ~buf[i].g;
         buf[i].b = ~buf[i].b;
      }
   }
   #endif
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      RGB16 *buf = (RGB16 *)buffer;

      count >>= 1; /*比count /= sizeof(RGB16)快*/
      for (i = 0; i < count; ++i)
      {
         buf[i].color = ~buf[i].color;
      }
   }
   #endif

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"roloc", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*隔行扫描*/
void scan_line_jump(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_jmp_lin_unused
   static int temp;
   int        i, t;

   if (temp)
   if (g_bmpbits == 8)
   {
      #ifndef NEO_8bit_bmp_unused
      char *buf = (char *)buffer;
      int  mask = (get_color_depth() == 8? MASK_COLOR_ : MASK_COLOR_8);

      for (i = 0; i < count; ++i)
      {
         buf[i] = mask;
      }
      #endif
   }
   #ifndef NEO_24bit_bmp_unused
   else if (g_bmpbits == 24)
   {
      RGB24 *buf = (RGB24 *)buffer;

      t = 5 - g_green_bit;
      count /= sizeof(RGB24);
      for (i = 0; i < count; ++i)
      {
         buf[i].r = (MASK_COLOR_ & 0xf800) >> 8;
         buf[i].g = (MASK_COLOR_ & 0x7e0) >> t;
         buf[i].b = (MASK_COLOR_ & 0x1f) << 3;
      }
   }
   #endif
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      RGB16 *buf = (RGB16 *)buffer;

      count >>= 1; /*比count /= sizeof(RGB16)快*/
      for (i = 0; i < count; ++i)
      {
         buf[i].color = MASK_COLOR_;
      }
   }
   #endif

   temp = (temp ? 0 : 1); /*交错系数*/
   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"jump", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线亮度调节*/
void scan_line_bright(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_bright_unused
   int i, temp;

   if (g_bmpbits == 24)
   {
      #ifndef NEO_24bit_bmp_unused
      RGB24 *buf = (RGB24 *)buffer;

      count /= sizeof(RGB24);
      if (g_bmp_param < 0)
      {
         for (i = 0; i < count; ++i)
         {
            temp = buf[i].r + g_bmp_param;
            buf[i].r = temp < 0 ? 0 : temp;
            temp = buf[i].g + g_bmp_param;
            buf[i].g = temp < 0 ? 0 : temp;
            temp = buf[i].b + g_bmp_param;
            buf[i].b = temp < 0 ? 0 : temp;
         }
      }
      else if (g_bmp_param > 0)
      {
         for (i = 0; i < count; ++i)
         {   
            temp = buf[i].r + g_bmp_param;
            buf[i].r = temp > 255 ? 255 : temp;
            temp = buf[i].g + g_bmp_param;
            buf[i].g = temp > 255 ? 255 : temp;
            temp = buf[i].b + g_bmp_param;
            buf[i].b = temp > 255 ? 255 : temp;
         }
      }
      #endif
   }
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      /*RGB16 *buf = (RGB16 *)buffer;
      int t;

      count >>= 1;
      if (g_bmp_param < 0)
      {
         for (i = 0; i < count; ++i)
         {
            temp = buf[i].color & 0x1f; t = 0x1 * (-g_bmp_param);
            buf[i].color -= temp <  t? temp : t;
            temp = buf[i].color & 0x7e0; t = 0x20 * (-g_bmp_param);
            buf[i].color -= temp <  t? temp : t;
            temp = buf[i].color & 0xf800; t = 0x800 * (-g_bmp_param);
            buf[i].color -= temp <  t? temp : t;
         }
         else if (g_bmp_param > 0)
         {
            temp = buf[i].color & 0x1f; t = 0x1 * (-g_bmp_param);
            buf[i].color += temp + t > 0x1f ? 0x1f - temp : t;
            temp = buf[i].color & 0x7e0; t = 0x20 * (-g_bmp_param);
            buf[i].color -= temp + t > 0x7e0? 0x1f - temp : t;
            temp = buf[i].color & 0xf800; t = 0x800 * (-g_bmp_param);
            buf[i].color -= temp + t > 0xf800L? 0x1f - temp : t;
         }
      }*/
   }
   #endif

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"bright", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线鲜艳效果*/
void scan_line_vivid(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_vivid_unused
   int i, temp, t2;

   if (g_bmpbits == 24)
   {
      #ifndef NEO_24bit_bmp_unused
      RGB24 *buf = (RGB24 *)buffer;

      count /= sizeof(RGB24);
      if (g_bmp_param > 0)
      {
         for (i = 0; i < count; ++i)
         {
            temp = NEO_MAX(buf[i].r, NEO_MAX(buf[i].g, buf[i].b));
            if (buf[i].r == temp)
            {
               t2 = buf[i].r + g_bmp_param;
               buf[i].r = t2 > 255 ? 255 : t2;
            }
            else if (buf[i].b == temp)
            {
               t2 = buf[i].b + g_bmp_param;
               buf[i].b = t2 > 255 ? 255 : t2;
            }
            else if (buf[i].g == temp)
            {
               t2 = buf[i].g + g_bmp_param;
               buf[i].g = t2 > 255 ? 255 : t2;
            }
         }
      }
      #endif
   }
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      /*RGB16 *buf = (RGB16 *)buffer;*/
      /*count >>= 1; */
   }
   #endif

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"vivid", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线颜色替换*/
void scan_line_stead(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_stead_unused
   int i, temp;

   if (g_bmpbits == 8)
   {
      #ifndef NEO_8bit_bmp_unused
      char *buf = (char *)buffer;
      temp = (get_color_depth() == 8? MASK_COLOR_ : MASK_COLOR_8);

      for (i = 0; i < count; ++i)
      {
         if (buf[i] == temp)
         {
            buf[i] = (char)g_bmp_param;
         }
      }
      #endif
   }
   #ifndef NEO_24bit_bmp_unused
   else if (g_bmpbits == 24)
   {
      RGB24 *buf = (RGB24 *)buffer;

      temp = 5 - g_green_bit;
      count /= sizeof(RGB24);
      for (i = 0; i < count; ++i)
      {
         if ( ((buf[i].r>>3)|((buf[i].g>>g_green_bit)<<5)|((buf[i].b>>3)<<11)) == MASK_COLOR_)
         {
            buf[i].b = (g_bmp_param & 0xf800) >> 8;
            buf[i].g = (g_bmp_param & 0x7e0) >> temp;
            buf[i].r = (g_bmp_param & 0x1f) << 3;
         }
      }
   }
   #endif
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      RGB16 *buf = (RGB16 *)buffer;

      count >>= 1; /*比count /= sizeof(RGB16)快*/
      for (i = 0; i < count; ++i)
      {
         if (buf[i].color == MASK_COLOR_)
         {
            buf[i].color = g_bmp_param;
         }
      }
   }
   #endif

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"stead", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线水平翻转*/
void scan_line_h_flip(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_h_flip_unused
   int i, temp;
   int r,g,b;

   if (g_bmpbits == 8)
   {
      #ifndef NEO_8bit_bmp_unused
      char *buf = (char *)buffer;
      r = count >> 1;

      for (i = 0; i < r; ++i)
      {
         temp = buf[i];
         buf[i] = buf[count - i];
         buf[count - i] = temp;
      }
      #endif
   }
   #ifndef NEO_24bit_bmp_unused
   else if (g_bmpbits == 24)
   {
      RGB24 *buf = (RGB24 *)buffer;

      temp = (int)(count / 3);
      count = temp >> 1;
      for (i = 0; i < count; ++i)
      {
         r = buf[i].r; g = buf[i].g; b = buf[i].b;
         buf[i].r = buf[temp - i].r;
         buf[i].g = buf[temp - i].g;
         buf[i].b = buf[temp - i].b;
         buf[temp - i].r = r; buf[temp - i].g = g; buf[temp - i].b = b;
      }

   }
   #endif
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      RGB16 *buf = (RGB16 *)buffer;

      r = (count >>= 1);
      count >>= 1;
      for (i = 0; i < count; ++i)
      {
         temp = buf[i].color;
         buf[i].color = buf[r - i].color;
         buf[r - i].color = temp;
      }
   }
   #endif

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"bmp_flip", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线模糊效果*/
void scan_line_wublur(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_wublur_unused
   int i;

   if (g_bmpbits == 24)
   {
      #ifndef NEO_24bit_bmp_unused
      RGB24 *buf = (RGB24 *)buffer;

      count /= sizeof(RGB24);
      for (i = 0; i < count - 1; ++i)
      {
            buf[i].r = (buf[i].r + buf[i + 1].r) >> 1;
            buf[i].g = (buf[i].g + buf[i + 1].g) >> 1;
            buf[i].b = (buf[i].b + buf[i + 1].b) >> 1;
      }
      #endif
   }
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      count >>= 1; /*比count /= sizeof(RGB16)快*/
   }
   #endif

   #else

   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"wu-blur", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}


/*扫描线去色效果*/
void scan_line_throw(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_channel_unused
   int i, temp;

   if (g_bmpbits == 8)
   {
      #ifndef NEO_8bit_bmp_unused
      #ifndef NEO_color_depth_8_unused
      char *buf = (char *)buffer;

      if ((g_bmp_param & 0x7) == 0x7)
      {
         for (i = 0; i < count; ++i)
         {
            temp = (get_blue(buf[i]) * 3 + get_green(buf[i]) * 6 + get_red(buf[i])) / 10;
            buf[i] = (temp << 6) | (temp << 3) | temp;
         }
      }
      else
      {
         for (i = 0; i < count; ++i)
         {
            if (g_bmp_param & 0x4) buf[i] &= 0X1F;
            if (g_bmp_param & 0x2) buf[i] &= 0XE3;
            if (g_bmp_param & 0x1) buf[i] &= 0XFC;
         }
      }
      #endif
      #endif   
   }
   #ifndef NEO_24bit_bmp_unused
   else if (g_bmpbits == 24)
   {
      RGB24 *buf = (RGB24 *)buffer;

      count /= sizeof(RGB24);
      if ((g_bmp_param & 0x7) == 0x7)
      {
         for (i = 0; i < count; ++i)
         {
            if (buf[i].r == buf[i].g && buf[i].g == buf[i].b) continue;
            temp = (buf[i].r * 3 + buf[i].g * 6 + buf[i].b) / 10;
            buf[i].r = buf[i].g = buf[i].b = (Uint8)temp;
         }
      }
      else
      {
         for (i = 0; i < count; ++i)
         {
            if (g_bmp_param & 0x1) buf[i].r = 0;
            if (g_bmp_param & 0x2) buf[i].g = 0;
            if (g_bmp_param & 0x4) buf[i].b = 0;
         }
      }
   }
   #endif
   #ifndef NEO_16bit_bmp_unused
   else if (g_bmpbits == 16)
   {
      RGB16 *buf = (RGB16 *)buffer;

      count >>= 1; /*比count /= sizeof(RGB16)快*/
      if ((g_bmp_param & 0x7) == 0x7)
      {
         for (i = 0; i < count; ++i)
         {
            temp = (getb16(buf[i].color) * 3 + getg16(buf[i].color) * 6 + getr16(buf[i].color)) / 10;
            buf[i].color = (temp << 11) | (temp << 6) | temp;
         }
      }
      else
      {
         for (i = 0; i < count; ++i)
         {
            if (g_bmp_param & 0x1) buf[i].color &= 0XFFE0;
            if (g_bmp_param & 0x2) buf[i].color &= 0XF81F;
            if (g_bmp_param & 0x4) buf[i].color &= 0X07FF;
         }
      }
   }
   #endif

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"Channel", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}

void scan_line_define(void *buffer, int count/*, int line*/)
{
   #ifndef NEO_bmp_define_unused
   int i;

   if (g_bmpbits == 24)
   {
      #ifndef NEO_24bit_bmp_unused
      RGB24 *buf = (RGB24 *)buffer;

      count /= sizeof(RGB24);

      for (i = 0; i < count; ++i)
      {
         buf[i].r = abs(g_bmp_param - buf[i].r);
         buf[i].g = abs(g_bmp_param - buf[i].g);
         buf[i].b = abs(g_bmp_param - buf[i].b);
      }
      #endif
   }

   #else
   char *buf = (char *)buffer; *buf += count;
   #ifndef NEO_sys_report_error_unused
   {
      Errinfo_t error = {"Channel", ROUTINE_UNACTIVE, 1};
      throw_error(error);
   }
   #endif

   #endif
}

#endif /* NEO_bmp_support_unused */
#endif /* NEO_showbmp_unused */

#endif /*__NEFFECT_H__*/
