/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nshow.h
  摘    要 : 本头文件中包含了NEO SDK里有关各种图形文件处理,显示的函数、结构、全局变量的声明定义
             目前支持格式: 标准微软位图BMP(Bitmap)文件

  当前版本 : V2.42
  作    者 : 董凯
  完成日期 : 2006.03.12

  取代版本 : V2.41
  原 作 者 : 董凯
  完成日期 : 2006.12.06
*/

#ifndef __NBMP_H__
#define __NBMP_H__

#define BMP_MAGIC          19778
#define BI_RGB             0
#define BI_RLE8            1
#define BI_RLE4            2
#define BI_BITFIELDS       3
#define OS2INFOHEADERSIZE  12
#define WININFOHEADERSIZE  40

#ifndef NEO_bmp_support_unused
typedef struct bitmap_file_head
{
   int   bf_type;       /*  位图文件的类型，必须为BM */
   long  bf_size;       /*  位图文件的大小，以字节为单位 */
   int   bf_reserved1;  /*  位图文件保留字，必须为0 */
   int   bf_reserved2;  /*  位图文件保留字，必须为0 */
   long  bf_offset;     /*  位图数据的起始位置，以相对于位图 */
/*  文件头的偏移量表示，以字节为单位 */
}bitmap_file_head_t;

typedef struct bitmap_info_head  /* size: 40 */
{
   long  size;            /*  本结构所占用字节数一般为28h */
   long  width;           /*  位图的宽度，以像素为单位 */
   long  height;          /*  位图的高度，以像素为单位 */
   int   planes;          /*  目标设备的级别，必须为1 */
   int   bitCount;        /*  每个像素所需的位数，必须是1(双色), */
                            /*  4(16色)，8(256色)或24(真彩色)之一 */

   long  compression;     /*  位图压缩类型，必须是 0(不压缩), */
                          /*  1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 */
   long  sizeImage;       /*  位图的大小，以字节为单位 */
   long  XpelsPerMeter;   /*  位图水平分辨率，每米像素数 */
   long  YpelsPerMeter;   /*  位图垂直分辨率，每米像素数 */
   long  clrUsed;         /*  位图实际使用的颜色表中的颜色数 */
   long  clrImportant;    /*  位图显示过程中重要的颜色数 */
}bitmap_info_head_t;

#endif /*NEO_bmp_support_unused*/
/* Used for both OS/2 and Windows BMP. 
 * Contains only the parameters needed to load the image 
 */
typedef struct
{
   long  biWidth;       /*  位图的宽度，以像素为单位 */
   long  biHeight;      /*  位图的高度，以像素为单位 */
   short biBitCount;    /*  每个像素所需的位数 */
   long  biCompression; /*  位图压缩类型 */
} BITMAPINFOHEADER;


typedef struct BITMAP
{
   /*long     ems_handle;
   unsigned long vram_offset;
   unsigned xms_handle;*/
   int  w;
   int  h;
   int  fp;
   char bpp;
   char flag;   /*小于零表示使用基本内存;等于零表示使用虚拟内存(硬盘空间);为1时为使用扩展内存;
                  为2时为使用扩充内存;为3时使用显存 */
   char *fname; /*不用 "FILE *" 是因为DOS有文件打开数目限制*/
}BITMAP;


#if defined NEO_bmp_support_unused || defined NEO_color_depth_8_unused
#else
char g_last_bmp[PATH_LENGTH] = {'\0'};
#endif
/*char g_map_file[PATH_LENGTH];*/

char g_palette_changed;
char g_enable_palette  = TRUE;

char get_pal_from_bmp(char *filename, PALETTE output);
void set_pal_with_bmp(char *filename);

char read_win_bminfoheader(PACKFILE *file, BITMAPINFOHEADER *bmpheader);
char bmp_color_depth(PACKFILE *file);
void read_scan_line(int fp, void *buffer, int count/*, int line*/);
char show_bmp(PACKFILE *bmpfile, int x, int y);
char show_bmp_ex(PACKFILE *bmpfile, int x, int y, int param, long flags);
char bmp_masked_blit(char *bmpname, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
char bmp_blit(char *bmpname, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
#define set_bmp_mask(maskcolor)      g_tptcolor = maskcolor
#define _show_bmp(bmp, x, y)         show_bmp_ex(bmp, x, y, 0, BMP_MASK_C)
#define show_bmp_v_flip(bmp, x, y)   show_bmp_ex(bmp, x, y, 0, BMP_V_FLIP)
#define _show_bmp_v_flip(bmp, x, y)  show_bmp_ex(bmp, x, y, 0, BMP_MASK_C|BMP_V_FLIP)
#define show_bmp_h_flip(bmp, x, y)   show_bmp_ex(bmp, x, y, 0, BMP_H_FLIP)
#define _show_bmp_h_flip(bmp, x, y)  show_bmp_ex(bmp, x, y, 0, BMP_MASK_C|BMP_H_FLIP)
#define show_bmp_vh_flip(bmp, x, y)  show_bmp_ex(bmp, x, y, 0, BMP_H_FLIP|BMP_V_FLIP)
#define _show_bmp_vh_flip(bmp, x, y) show_bmp_ex(bmp, x, y, 0, BMP_MASK_C|BMP_H_FLIP|BMP_V_FLIP)
#define show_bmp_hv_flip(bmp, x, y)  show_bmp_vh_flip(bmp, x, y)
#define _show_bmp_hv_flip(bmp, x, y) _show_bmp_vh_flip(bmp, x, y)

#define save_screen(bmpfile)         rect_save_screen(bmpfile, 0, 0, SCREEN_W - 1, SCREEN_H - 1)
#define scan_line_define scan_line_throw
BITMAP *get_image(int x0, int y0, int x1, int y1);
void put_image_ex(BITMAP *image, int x, int y, int param, long flags);
BITMAP *load_bmp(char *file, PALETTE pal);
void destroy_bitmap(BITMAP *bitmap);

/*已经不被支持的函数*/
#define _load_bmp(filename)   0
#define blit(xms_handle)      0
#define free_bmp( xms_handle) 0

#ifndef NEO_bmp_support_unused

char read_win_bminfoheader(PACKFILE *file, BITMAPINFOHEADER *bmpheader)
{
   FILE *fp;
   int bmpmark;

   fp = fopen(file, "rb");
   if(fp == NULL)
   {
      return FALSE;
   }
   _FREAD(&bmpmark, 2, 1, fp);

   if (bmpmark != BMP_MAGIC)
   {
      return -1;
   }
   fseek(fp, 0X1C,SEEK_SET);
   _FREAD(&bmpheader->biBitCount, 2, 1, fp);
   _FREAD(&bmpheader->biCompression, 4, 1, fp);
   fseek(fp, 0X12, SEEK_SET);
   _FREAD(&bmpheader->biWidth, 4, 1, fp);
   _FREAD(&bmpheader->biHeight, 4, 1, fp);
   fclose(fp);
   return (char)bmpheader->biBitCount;
}


#ifndef NEO_color_depth_8_unused
#ifndef NEO_palette_unused
char get_bmp_palette(int fp, int bpp, PALETTE output) /*show_bmp()函数专用*/
{
   unsigned i = 0;
   unsigned char pix24b[4];

   lseek(fp, 54, SEEK_SET);
   switch (bpp)
   {
      case 8:
      for(; i < 256; ++i)
      {
         _READ(fp, &pix24b, 4);
         output[i].r = pix24b[2] >> g_dac_size_fix;
         output[i].g = pix24b[1] >> g_dac_size_fix;
         output[i].b = pix24b[0] >> g_dac_size_fix;
      }
      break;
      /*case 4:
      for(; i < 16; ++i)
      {
         _READ(fp, &pix24b, 4);
         output[i].r = pix24b[2] >> g_dac_size_fix;
         output[i].g = pix24b[1] >> g_dac_size_fix;
         output[i].b = pix24b[0] >> g_dac_size_fix;
      }   
      break;*/
      default:
      make_332_palette(output);
      break;
   }

   return TRUE;
}

char get_pal_from_bmp(char *bmpfile, PALETTE outpal)
{
   BITMAPINFOHEADER bmpheader;
   int fp;
   if (read_win_bminfoheader(bmpfile, &bmpheader) <= 0)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"get_bmp_palette", NO_FILE, 1};
      throw_error(error);
      #endif
      return FALSE;
   }
   fp = open(bmpfile, O_RDONLY | O_BINARY);
   get_bmp_palette(fp, bmpheader.biBitCount, outpal);
   close(fp);
   return bmpheader.biBitCount;
}


void set_pal_with_bmp(char *filename)
{
   PALETTE pal;
   get_pal_from_bmp(filename, pal);
   #ifndef NEO_draw_smooth_unused
   _set_palette_range(pal, 0, _PAL_SIZE - 1, 1);
   #else
   _set_palette_range(pal, 0, _PAL_SIZE - 1, 0);
   #endif
}
#endif /*NEO_palette_unused*/
#endif /*NEO_color_depth_8_unused*/


char bmp_color_depth(PACKFILE *file)
{
   BITMAPINFOHEADER bmpheader;

   return read_win_bminfoheader(file, &bmpheader);
}
   

#ifndef NEO_showbmp_unused

void read_scan_line(int fp, void *buffer, int count/*, int line*/)
{
   _READ(fp, buffer, count);

   if (g_bmp_flags)
   {
      if (g_h_flip_) /*进行扫描线水平翻转*/
      {
         scan_line_h_flip(buffer, count/*, line*/);
      }
      if (g_stead_) /*颜色替换应该放在前面，以保证颜色信息在不被其它特效干扰*/
      {
         scan_line_stead(buffer, count/*, line*/);
      }
      if (g_bright_)
      {
         scan_line_bright(buffer, count/*, line*/);
      }
      if (g_vivid_)
      {
         scan_line_vivid(buffer, count/*, line*/);
      }
      if (g_roloc_)
      {
         scan_line_roloc(buffer, count/*, line*/);
      }
      if (g_wublur_)
      {
         scan_line_wublur(buffer, count/*, line*/);
      }

      if (g_channl_) /*去颜色通道也要放在后面，否则会被其它算法干扰*/
      {
         scan_line_throw(buffer, count/*, line*/);
      }
      if (g_jmplin_) /*这两个一定要放在最后，否则无法产生透明效果*/
      {
         scan_line_jump(buffer, count/*, line*/);
      }
      if (g_broken_)
      {
         scan_line_broken(buffer, count/*, line*/);
      }
      if (g_define_)
      {
         scan_line_define(buffer, count/*, line*/);
      }
   }
}


char show_bmp(PACKFILE *bmpfile, int x, int y)
{
   BITMAPINFOHEADER bmpheader;
   Uint32 cnt = 0, addr1;
   register int   i, j, k;
   int   length, len, len2, real_hig;
   int   rect_scr_h = g_rect_right + 1;
   int   fp;
   int   offset = 0, color, fix = 0;
   int   xx = x, yy, start = -1, temp, masklen;
   char  page1, cpr;
   Uint8 wid_fix = 0;
   Uint8 *buffer;

   if (read_win_bminfoheader(bmpfile, &bmpheader) <= 0)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"show_bmp", NO_FILE, 1};
      throw_error(error);
      #endif
      return 0;
   }

   fp = open(bmpfile, O_RDONLY | O_BINARY);

   g_bmp_wid  = (int)bmpheader.biWidth;
   g_bmpheight= (int)bmpheader.biHeight;
   g_bmpbits  = (char)bmpheader.biBitCount;
   cpr        = (char)bmpheader.biCompression;

   if ( (y > g_rect_bottom) || (y + g_bmpheight < g_rect_top) || cpr == BI_RLE4 || cpr == BI_RLE8)
   {
      close(fp);
      return -1;
   }
   real_hig = --g_bmpheight;

   /*计算图片剪裁*/
   if (x >= 0 && x < rect_scr_h)
   {
      if (rect_scr_h - x >= g_bmp_wid) /*图象完全显示*/
      {
         fix = (x <= g_rect_left? (x >> (g_color_depth == 8)) : (g_rect_left >> (g_color_depth == 8)) );
         masklen = length = len = g_bmp_wid;
      }
      else                            /*图象右端溢出*/
      {
         if (x > g_rect_left)
         {
            x -= g_rect_left;
            start = 0;
         }
         else fix = (x >> (g_color_depth == 8));
         masklen = length = len = rect_scr_h - x;
      }
   }
   else if (x < 0 && x > (-g_bmp_wid))
   {
      offset = x;
      if (g_bmp_wid + x >= rect_scr_h)  /*图象水平两端溢出*/
      {
         length = len = rect_scr_h;
         masklen = rect_scr_h - x;
      }
      else                             /*图象左边溢出*/
      {
         len = (masklen = length = g_bmp_wid) + x;
      }
      x = 0;
   }
   else
   {
      close(fp);
      return -1;
   }

   if (g_color_depth == 8) /*在8位图形模式下*/
   {
      #ifndef NEO_color_depth_8_unused
      int far *s_tmp;
      int origin;

      len >>= 1;length -= (length & 1);
      origin = (g_rect_left >> 1) - fix;
      if (start < 0) start = origin;
      temp = start;

      if (g_bmpbits == 8) /*打开的是8位256色位图*/
      {
         #ifndef NEO_8bit_bmp_unused
         if ((buffer = (unsigned char *)malloc(len2 = g_bmp_wid + (wid_fix = (g_bmp_wid & 3)?(4 - (g_bmp_wid & 3)) : 0))) == NULL)
         {
            #ifndef NEO_sys_report_error_unused
            Errinfo_t error = {"show_bmp", NO_MEMORY, 0};
            close(fp);
            throw_error(error);
            #endif
         }
         s_tmp = (int far *)buffer;
         (char far *)s_tmp  -= offset;
         if (g_enable_palette && ((strcmp(g_last_bmp, bmpfile) != 0) || g_palette_changed))
         {
            PALETTE pal;
            get_bmp_palette(fp, 8, pal);

            #ifndef NEO_draw_smooth_unused
            _set_palette_range(pal, 0, 255, 1);
            #else
            _set_palette_range(pal, 0, 255, 0);
            #endif
            strcpy(g_last_bmp,bmpfile);
         }
         else lseek(fp, 1078, SEEK_SET);

         for(j = real_hig; j >= 0; --j)
         {
            if (((yy = y + (g_v_flip_?real_hig - j : j) ) < g_rect_top) || (yy > g_rect_bottom))
            {
               cnt++;
               continue; /*图象垂直溢出*/
            }
            else if (cnt)
            {
               lseek(fp, cnt * len2, SEEK_CUR);
               cnt = 0;
            }
            read_scan_line(fp, buffer, len2);
            page1 = (char)((addr1 = ((Uint32)yy + g_sl_offset) * g_screen_h + x) >> 16);

            if (page1 == ((addr1 + length) >> 16))
            {
               int far *d_tmp = (int far *)(g_videoptr + (unsigned)(addr1 & 0xffff));
               set_vbe_page(page1);

                /*#if HIGH_COMPILE_MODE && NEO_draw_optimization_beused
                  memcpy(d_tmp, s_tmp, (len - origin)<<1);
                  #else*/
               if (!g_mask_flag_)
               {
                  for (start = temp, k = origin; k < len; ++k, ++start)
                  {
                     #ifndef NEO_draw_mode_unused
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : d_tmp[k] = s_tmp[start]; break;
                        case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ s_tmp[start]; break;
                        case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT   : d_tmp[k] = d_tmp[k] | s_tmp[start]; break;
                        case AND_PUT  : d_tmp[k] = d_tmp[k] & s_tmp[start]; break;
                     }
                     #else
                     d_tmp[k] = s_tmp[start];
                     /*movedata(s_tmp, start, d_tmp, k, (len - origin)<<1);*/
                     #endif
                  }
                  /*#endif*/
               }
               else
               {
                  char far *dd = (char far *)d_tmp;
                  char *ss = (char *)s_tmp;

                  for (start = (temp << 1), k = (origin << 1); k < (len << 1); ++k, ++start)
                  {
                     if (ss[start] != MASK_COLOR_)
                     #ifndef NEO_draw_mode_unused
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : dd[k] = ss[start]; break;
                        case XOR_PUT  : dd[k] = dd[k] ^ ss[start]; break;
                        case NOT_PUT  : dd[k] = ~dd[k]; break;
                        case OR_PUT   : dd[k] = dd[k] | ss[start]; break;
                        case AND_PUT  : dd[k] = dd[k] & ss[start]; break;
                     }
                     #else
                     dd[k] = ss[start];
                     #endif
                  }
               }
            }
            else
            {
               i = 0;
               if (!g_mask_flag_)
               {
                  for(; i < masklen; ++i)
                  {
                     dot(i + xx, yy, buffer[i]);
                  }
               }
               else
               {
                  for(; i < masklen; ++i)
                  {
                     if (buffer[i] != MASK_COLOR_)
                     dot(i + xx, yy, buffer[i]);
                  }
               }
            }
         }
         free(buffer);
         close(fp);
         return 1;
         #endif
      }
      else if (g_bmpbits == 24) /*打开的是24位真彩位图*/
      {
         #ifndef NEO_24bit_bmp_unused
         RGB24 *buffer, *b_tmp;

         lseek(fp, 54, SEEK_SET);
         if ((buffer=(RGB24 *)malloc(len2 = g_bmp_wid*sizeof(RGB24) + (wid_fix = (g_bmp_wid & 3))))==NULL)
         {
            #ifndef NEO_sys_report_error_unused
            Errinfo_t error = {"show_bmp", NO_MEMORY, 0};
            close(fp);
            throw_error(error);
            #endif
         }
         b_tmp = buffer - offset;
         len <<= 1;
         for(j = real_hig; j >= 0; j--)
         {
            if (((yy = y + (g_v_flip_?real_hig - j : j) )<g_rect_top) || (yy > g_rect_bottom))
            {
               cnt++;
               continue; /*图象垂直溢出*/
            }
            else if (cnt)
            {
               lseek(fp, cnt * len2, SEEK_CUR);
               cnt = 0;
            }
            read_scan_line(fp, buffer, len2);
            page1 = (Uint8)((addr1 = ((Uint32)yy + g_sl_offset) * g_screen_h + x) >> 16);

            if (page1 == ((addr1 + length) >> 16))
            {
               char far *d_tmp = (char far *)(g_videoptr+ (unsigned)(addr1 & 0xffff));
               start = temp; k = origin;
               set_vbe_page(page1);
               if (!g_mask_flag_)
               {
                  for (; k < len; ++start ,++k)
                  {
                     #ifndef NEO_draw_mode_unused
                     color = makecol(b_tmp[start].b, b_tmp[start].g, b_tmp[start].r);
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : d_tmp[k] = color; break;
                        case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ color; break;
                        case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT   : d_tmp[k] = d_tmp[k] | color; break;
                        case AND_PUT  : d_tmp[k] = d_tmp[k] & color; break;
                     }
                     #else
                     d_tmp[k] = makecol(b_tmp[start].b, b_tmp[start].g, b_tmp[start].r);
                     #endif
                  }
               }
               else
               {
                  for (; k < len; ++start ,++k)
                  {
                     if (!(b_tmp[start].b == 0xff && b_tmp[start].r == b_tmp[start].b && b_tmp[start].g == 0))
                     #ifndef NEO_draw_mode_unused
                     {
                        color = makecol(b_tmp[start].b, b_tmp[start].g, b_tmp[start].r);
                        switch(g_draw_mode)
                        {
                           case COPY_PUT : d_tmp[k] = color; break;
                           case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ color; break;
                           case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                           case OR_PUT   : d_tmp[k] = d_tmp[k] | color; break;
                           case AND_PUT  : d_tmp[k] = d_tmp[k] & color; break;
                        }
                     }
                     #else
                     d_tmp[k] = makecol(b_tmp[start].b, b_tmp[start].g, b_tmp[start].r);
                     #endif
                  }
               }
            }
            else /*处理发生换页的扫描线*/
            {
               i = 0;
               if (!g_mask_flag_)
               {
                  for(; i < masklen; ++i)
                  {
                     if (!(buffer[start].b == 0xff && buffer[start].r == buffer[start].b && buffer[start].g == 0))
                     dot(xx + i, yy, makecol(buffer[i].b, buffer[i].g, buffer[i].r));
                  }
               }
               else
               {
                  for(; i < masklen; ++i)
                  {
                     dot(xx + i, yy, makecol(buffer[i].b, buffer[i].g, buffer[i].r));
                  }
               }
            }
         }

         free(buffer);
         close(fp);
         return 1;
         #endif
      }
      else
      {
         close(fp);
         return -2;
      }
      #else
      wid_fix = x = y = xx = 0;
      bmpfile += 0;
      #endif
   }

   else if (g_color_depth == 15 || g_color_depth == 16) /*在高彩(16位色)模式下*/
   {
      #ifndef NEO_color_depth_16_unused
      if (start < 0) start = g_rect_left - fix;
      temp = start;

      lseek(fp, 54, SEEK_SET);

      if (g_bmpbits == 24) /*打开的是24位真彩位图*/
      {
         #ifndef NEO_24bit_bmp_unused
         RGB24 *buffer, *b_tmp;

         if ((buffer=(RGB24 *)malloc(len2 = g_bmp_wid * sizeof(RGB24) + (wid_fix = (g_bmp_wid & 3))))==NULL)
         {
            #ifndef NEO_sys_report_error_unused
            Errinfo_t error = {"show_bmp", NO_MEMORY, 0};
            close(fp);
            throw_error(error);
            #endif
         }
         b_tmp = buffer-offset;

         for(j = real_hig; j >= 0; j--)
         {
            if (((yy = y + (g_v_flip_?real_hig - j : j) )<g_rect_top) || (yy > g_rect_bottom))
            {
               cnt++;
               continue; /*图象垂直溢出*/
            }
            else if (cnt)
            {
               lseek(fp, cnt * len2, SEEK_CUR);
               cnt = 0;
            }
            read_scan_line(fp, buffer, len2);
            page1 = (char)((addr1 = ((Uint32)yy + g_sl_offset) * g_screen_h + x) >> 15);
         
            if (page1 == ((addr1 + length) >> 15))
            {
               int far *d_tmp = (int far *)(g_videoptr+ (unsigned)((addr1<<1) & 0xffff));
               start = temp; k = g_rect_left - fix;
               set_vbe_page(page1);
               if (!g_mask_flag_)
               {
                  for (; k < len; ++start ,++k)
                  {
                     #ifndef NEO_draw_mode_unused
                     color = (b_tmp[start].r>>3)|((b_tmp[start].g>>g_green_bit)<<5)|((b_tmp[start].b>>3)<<11);
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : d_tmp[k] = color; break;
                        case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ color; break;
                        case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT   : d_tmp[k] = d_tmp[k] | color; break;
                        case AND_PUT  : d_tmp[k] = d_tmp[k] & color; break;
                     }
                     #else
                     d_tmp[k] = (b_tmp[start].r>>3)|((b_tmp[start].g>>g_green_bit)<<5)|((b_tmp[start].b>>3)<<11);
                     #endif
                  }
               }
               else
               {
                  for (; k < len; ++start ,++k)
                  {
                     if ((color = (b_tmp[start].r>>3)|((b_tmp[start].g>>g_green_bit)<<5)|((b_tmp[start].b>>3)<<11)) != MASK_COLOR_)
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
            }
            else
            {
               i = 0;
               if (!g_mask_flag_)
               {
                  for(; i < masklen; ++i)
                  {
                     color = ((((buffer[i].b>>3)<<g_green_mask)+(buffer[i].g>>g_green_bit))<<5)+(buffer[i].r>>3);
                     dot(xx + i, yy, color);
                  }
               }
               else
               {
                  for(; i < masklen; ++i)
                  {
                     color = ((((buffer[i].b>>3)<<g_green_mask)+(buffer[i].g>>g_green_bit))<<5)+(buffer[i].r>>3);
                     if (color != MASK_COLOR_)
                        dot(xx+i, yy, color);
                  }   
               }
            }
         }

         free(buffer);
         close(fp);
         return 1;
         #endif
      }
      else if (g_bmpbits == 16) /*打开的是16位高彩位图*/
      {
         #ifndef NEO_16bit_bmp_unused
         RGB16 *buffer, *b_tmp;
         if ((buffer=(RGB16 *)malloc(len2 = g_bmp_wid*sizeof(RGB16) + (wid_fix = ((g_bmp_wid & 1) << 1) )))==NULL)
         {
            #ifndef NEO_sys_report_error_unused
            Errinfo_t error = {"show_bmp", NO_MEMORY, 0};
            close(fp);
            throw_error(error);
            #endif
         }
         b_tmp = buffer-offset;

         if (cpr == 3)/*非555格式*/
         {
            lseek(fp, 16, SEEK_CUR);
         }

         for(j = real_hig; j >= 0; j--)
         {
            if (((yy = y + (g_v_flip_?real_hig - j : j) ) < g_rect_top) || (yy > g_rect_bottom))
            {
               cnt++;
               continue; /*图象垂直溢出*/
            }
            else if (cnt)
            {
               lseek(fp, cnt * len2, SEEK_CUR);
               cnt = BI_RGB;
            }
            read_scan_line(fp, buffer, len2);
            page1 = (char)((addr1 = ((Uint32)yy + g_sl_offset) * g_screen_h + x) >> 15);
            if (page1 == ((addr1 + length) >> 15))
            {
               int far *d_tmp = (int far *)(g_videoptr+ (unsigned)((addr1<<1) & 0xffff));
               start = temp; k = g_rect_left - fix;/*neo_printf(500,400,"%d",offset);*/
               set_vbe_page(page1);
               if (!g_mask_flag_)
               {
                  for (; k < len; /*++start ,*/++k)
                  {
                     #ifndef NEO_draw_mode_unused
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : d_tmp[k] = b_tmp[k].color; break;
                        case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ b_tmp[k].color; break;
                        case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT   : d_tmp[k] = d_tmp[k] | b_tmp[k].color; break;
                        case AND_PUT  : d_tmp[k] = d_tmp[k] & b_tmp[k].color; break;
                     }
                     #else
                     d_tmp[k] = b_tmp[k].color;
                     #endif
                  }
               }
               else
               {
                  for (; k < len; /*++start ,*/++k)
                  {
                     if (b_tmp[k].color != MASK_COLOR_)
                     #ifndef NEO_draw_mode_unused
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : d_tmp[k] = b_tmp[k].color; break;
                        case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ b_tmp[k].color; break;
                        case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT   : d_tmp[k] = d_tmp[k] | b_tmp[k].color; break;
                        case AND_PUT  : d_tmp[k] = d_tmp[k] & b_tmp[k].color; break;
                     }
                     #else
                     d_tmp[k] = b_tmp[k].color;
                     #endif
                  }
               }
            }
            else
            {
               i = 0;
               if (!g_mask_flag_)
               {
                  for(; i < masklen; ++i)
                  {
                     dot(xx + i, yy, buffer[i].color);
                  }
               }
               else
               {
                  for(; i < masklen; ++i)
                  {
                     if (buffer[i].color != MASK_COLOR_)
                        dot(xx+i, yy, buffer[i].color);
                  }   
               }
            }
         }

         free(buffer);
         close(fp);
         return 1;
         #endif
      }
      else if (g_bmpbits == 8) /*打开的是8位256色位图*/
      {
         #ifndef NEO_8bit_bmp_unused
         unsigned color256[256];
         unsigned char *buffer, *b_tmp;
         unsigned char pix24b[8];

         wid_fix = (g_bmp_wid & 3) ? (4 - (g_bmp_wid & 3)) : 0;
         if ((buffer = (unsigned char *)malloc(len2=(g_bmp_wid + wid_fix)))==NULL)
         {
            #ifndef NEO_sys_report_error_unused
            Errinfo_t error = {"show_bmp", NO_MEMORY, 0};
            close(fp);
            throw_error(error);
            #endif
         }
         for(i = 0; i < 256; ++i)
         {
            _READ(fp, &pix24b, 8);
            color256[i]   = ((((pix24b[2]>>3)<<g_green_mask)+(pix24b[1]>>g_green_bit))<<5)+(pix24b[0]>>3);
            color256[++i] = ((((pix24b[6]>>3)<<g_green_mask)+(pix24b[5]>>g_green_bit))<<5)+(pix24b[4]>>3);
         }
         b_tmp = buffer - offset;

         for(j = real_hig; j >= 0; j--)
         {
            if (((yy = y + (g_v_flip_?real_hig - j : j) )<g_rect_top) || (yy > g_rect_bottom))
            {
               cnt++;
               continue; /*图象垂直溢出*/
            }
            else if (cnt)
            {
               lseek(fp, cnt * len2, SEEK_CUR);
               cnt = 0;
            }
            read_scan_line(fp, buffer, len2);
            page1 = (char)((addr1 = ((Uint32)yy + g_sl_offset) * g_screen_h + x) >> 15);

            if (page1 == ((addr1 + length) >> 15))
            {
               int far *d_tmp = (int far *)(g_videoptr+ (unsigned)((addr1<<1) & 0xffff));
               start = temp; k = g_rect_left - fix;
               set_vbe_page(page1);
               if (!g_mask_flag_)
               {
                  for (; k < len; ++start ,++k)
                  {
                     #ifndef NEO_draw_mode_unused
                     switch(g_draw_mode)
                     {
                        case COPY_PUT : d_tmp[k] = color256[b_tmp[start]]; break;
                        case XOR_PUT  : d_tmp[k] = d_tmp[k] ^ color256[b_tmp[start]]; break;
                        case NOT_PUT  : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT   : d_tmp[k] = d_tmp[k] | color256[b_tmp[start]];break;
                        case AND_PUT  : d_tmp[k] = d_tmp[k] & color256[b_tmp[start]];break;
                     }
                     #else
                     d_tmp[k] = color256[b_tmp[start]];
                     #endif
                  }
               }
               else
               {
                  for (; k < len; ++start ,++k)
                  {
                     if (b_tmp[start] != MASK_COLOR_8)
                     #ifndef NEO_draw_mode_unused
                     switch(g_draw_mode)
                     {
                        case COPY_PUT: d_tmp[k] = color256[b_tmp[start]]; break;
                        case XOR_PUT : d_tmp[k] = d_tmp[k] ^ color256[b_tmp[start]]; break;
                        case NOT_PUT : d_tmp[k] = ~d_tmp[k]; break;
                        case OR_PUT  : d_tmp[k] = d_tmp[k] | color256[b_tmp[start]];break;
                        case AND_PUT : d_tmp[k] = d_tmp[k] & color256[b_tmp[start]];break;
                     }
                     #else
                     d_tmp[k] = color256[b_tmp[start]];
                     #endif
                  }
               }
            }
            else
            {
               i = 0;
               if (!g_mask_flag_)
               {
                  for(; i < masklen; ++i)
                  {
                     dot(xx+i, yy, color256[buffer[i]]);
                  }
               }
               else
               {
                  for (; i < masklen; ++i)
                  {
                     if (buffer[i] != MASK_COLOR_8)
                        dot(xx+i, yy, color256[buffer[i]]);
                  }
               }
            }
         }

         free(buffer);
         close(fp);
         return 1;
         #endif
      }
      else
      {
         close(fp);
         return -3;
      }
      #else
      wid_fix += 0;
      bmpfile += 0; x += 0; y+=0;
      #endif
   }
   else
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"show_bmp", ERR_VIDEO_MODE, 0};
      close(fp);
      throw_error(error);
      #endif
      wid_fix += 0; masklen += 0; temp += 0; xx += 0; offset += 0;real_hig += 0; cnt += 0;
   }
   return -3;
}


char show_bmp_ex(PACKFILE *bmpfile, int x, int y, int param, long flags)
{
   char retrn;
   g_bmp_param = param;
   g_bmp_flags = flags;
   retrn = show_bmp(bmpfile, x, y);
   g_bmp_flags = g_bmp_param = 0x0;
   return retrn;
}

#endif /*NEO_showbmp_unused*/


#ifndef NEO_bmp_blit_unused
char bmp_blit(char *bmpname, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
   int xx = dest_x + width, yy = dest_y + height;
   #ifndef NEO_rect_unused
   rect_store();
   #endif
   if (dest_x > g_rect_right || xx < g_rect_left || dest_y > g_rect_bottom || yy < g_rect_top)
   {
      return FALSE;
   }

   if (dest_x > g_rect_left) g_rect_left = dest_x;
   if (dest_y > g_rect_top)  g_rect_top  = dest_y;
   if (xx < g_rect_right)    g_rect_right = xx;
   if (yy < g_rect_bottom)   g_rect_bottom = yy;

   #ifndef NEO_showbmp_unused
   show_bmp(bmpname, dest_x - source_x, dest_y - source_y);
   #endif

   #ifndef NEO_rect_unused
   rect_restore();
   #endif
   return TRUE;
}
#endif /*NEO_bmp_blit_unused*/


#ifndef NEO_bmp_blit_unused
char bmp_masked_blit(char *bmpname, int source_x, int source_y, int dest_x, int dest_y, int width, int height)
{
   int flag;

   g_bmp_flags |= BMP_MASK_C; /*mask标志开*/
   flag = bmp_blit(bmpname, source_x, source_y, dest_x, dest_y, width, height);
   g_bmp_flags &= !BMP_MASK_C; /*mask标志关*/

   return flag;
}
#endif


#ifndef NEO_save_screen_unused
char rect_save_screen(char *bmp, int left, int top, int right, int bottom)
{
   FILE *bmp_fp;
   bitmap_file_head_t bmp_file_h;
   bitmap_info_head_t bmp_header;
   long addr;
   int  bmpwidth, len;
   int  bmpheight;
   int  *buf16;
   int  i, j;
   char page;
   char *buffer;
   char div, off;

   if (left >= right || top >= bottom)
      return FALSE;
   right  = right >= SCREEN_W?SCREEN_W - 1 : right;
   bottom = bottom>= SCREEN_H?SCREEN_H - 1 : bottom;
   bmp_fp = fopen(bmp, "wb");
   if (bmp_fp == NULL)
   {
      return FALSE;
   }
   bmpwidth = len = right - left + 1;
   bmpheight = bottom - top + 1;

   bmp_file_h.bf_type = BMP_MAGIC;
   bmp_file_h.bf_offset = bmp_file_h.bf_size = 54;
   bmp_header.clrImportant = bmp_header.compression = bmp_file_h.bf_reserved1 = bmp_file_h.bf_reserved2 = 0;

   bmp_header.size = 40;
   bmp_header.width= (long)bmpwidth;
   bmp_header.height= (long)bmpheight;
   bmp_header.planes= 1;
   bmp_header.YpelsPerMeter = bmp_header.XpelsPerMeter = 3780/*2835*/;

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      #ifndef NEO_palette_unused
      char pal_buf[256][3];
      char pal[256][4];
      len += (bmpwidth & 3) ? 4 - (bmpwidth & 3) : 0;
      bmp_file_h.bf_size += 1024 + (long)len * (long)bmpheight;
      bmp_file_h.bf_offset += 1024;
      bmp_header.bitCount = 8;
      bmp_header.clrUsed  = 256;
      bmp_header.sizeImage= (long)bmpwidth * (long)bmpheight;
      _FWRITE(&bmp_file_h, 14, 1, bmp_fp);
      _FWRITE(&bmp_header, 40, 1, bmp_fp);

      get_palette(*pal_buf);
      for (i = 0; i < 256; ++i)
      {
         pal[i][0] = pal_buf[i][2]<<g_dac_size_fix;
         pal[i][1] = pal_buf[i][1]<<g_dac_size_fix;
         pal[i][2] = pal_buf[i][0]<<g_dac_size_fix;
         pal[i][3] = 0;
      }
      _FWRITE(pal, 4, 256, bmp_fp);
      if ( (buffer = (char *)malloc(len)) == NULL) return FALSE;
      div = 0;
      off = 16;
      #endif
      #endif
   }
   else if (g_color_depth == 16 || g_color_depth == 15)
   {
      #ifndef NEO_color_depth_16_unused
      bmp_file_h.bf_size += (long)len * (long)bmpheight << 1;
      bmp_header.bitCount = 16;
      bmp_header.clrUsed  = 65536L;
      bmp_header.sizeImage= (long)bmpwidth * (long)bmpheight << 1;
      bmp_header.compression = (g_color_depth == 16) ? BI_BITFIELDS : BI_RGB;
      _FWRITE(&bmp_file_h, 14, 1, bmp_fp);
      _FWRITE(&bmp_header, 40, 1, bmp_fp);


      if (g_color_depth == 16)
      {
         Uint32 mask;
         /*fseek(bmp_fp, 16, SEEK_CUR);*/
         mask = 0xF800;
         _FWRITE(&mask, 4, 1, bmp_fp);
         mask = 0x07E0;
         _FWRITE(&mask, 4, 1, bmp_fp);
         mask = 0x001F;
         _FWRITE(&mask, 4, 1, bmp_fp);
         mask = 0x0;
         _FWRITE(&mask, 4, 1, bmp_fp);
      }
      div = 1;
      off = 15;
      buffer = (char *)malloc(len = (len << 1) + ((bmpwidth & 1) << 1) );
      if ( buffer == NULL) return FALSE;
      buf16 = (int *)buffer;
      #endif
   }
   else
   {
      fclose(bmp_fp);
      return FALSE;
   }

   for (j = bottom; j >= top; --j)
   {
      page = (char)((addr = ((long)j + g_sl_offset) * g_screen_h + left) >> off);
      if (page == ((addr + bmpwidth) >> off))
      {
         set_vbe_page(page);
         movedata(0xa000, (unsigned)((addr << div) & 0xffff), FP_SEG(buffer), FP_OFF(buffer), len);
      }
      else
      {
         for (i = 0; i < bmpwidth; ++i)
         {
            if (g_color_depth == 8)
               buffer[i] = get_dot(i + left, j);
            else
            {
               #ifndef NEO_color_depth_16_unused
               buf16[i] = get_dot(i + left, j);
               #endif
            }
         }
      }
      _FWRITE(buffer, 1, len, bmp_fp);
   }
   free(buffer);
   fclose(bmp_fp);
   return TRUE;
}


BITMAP *get_image(int x0, int y0, int x1, int y1)
{
   BITMAP *image;
   char   dir[16];

   if ( (image = (BITMAP *)malloc(sizeof(BITMAP))) == NULL)
      return FALSE;

   if ( (image->fname = (char *)malloc(32)) == NULL)
   {
      free(image);
      return FALSE;
   }
   #ifndef NEO_temp_dir_unused
   strcpy(image->fname, "neotemp\\");
   #endif
   strcat(image->fname, tmpnam(dir));
   image->flag = 0;
   rect_save_screen(image->fname, x0, y0, x1, y1);
   return image;
}


void put_image_ex(BITMAP *image, int x, int y, int param, long flags)
{
   if (image != NULL)
   {
      if (image->flag == 0)
      {
         #ifndef NEO_showbmp_unused
         show_bmp_ex(image->fname, x, y, param, flags);
         #endif
      }
   }
}
#define put_image(image, x, y) put_image_ex(image, x, y, 0, 0)


/*加入扩展内存后该 #endif 往上挪位*/
#endif /*NEO_save_screen_unused*/

BITMAP *load_bmp(char *file, PALLETE pal)
{
   BITMAP *image;

   if ( (image = (BITMAP *)malloc(sizeof(BITMAP))) == NULL)
      return FALSE;
   if ( (image->fname = (char *)malloc(strlen(file) + 1)) == NULL)
   {
      free(image);
      return FALSE;
   }

   image->flag = 0;
   strcpy(image->fname, file);
   #if !(defined NEO_color_depth_8_unused)
   get_pal_from_bmp(file, pal);
   #else
   pal += 0;
   #endif
   return image;
}

void destroy_bitmap(BITMAP *bitmap)
{
   if (bitmap != NULL)
   {
      if (bitmap->flag == 0)
      {
         free(bitmap->fname); /*释放保存文件名的内存*/
      }
      free(bitmap);
      bitmap = NULL;
   }
}
#endif /*NEO_bmp_support_unused*/

#endif /*__NBMP_H__*/
