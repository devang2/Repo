/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nshow.h
  摘    要 : 本头文件中包含了NEO SDK里有关各种图形文件处理,显示的函数、结构、全局变量的声明定义
             目前支持格式:1.标准(即静态,766字节)的32x32x16(色)*.cur和*.ico光标、图标文件

  当前版本 : V2.10
  作    者 : 董凯 
  完成日期 : 2006.03.18

  取代版本 : V2.01
  原 作 者 : 董凯 
  完成日期 : 2006.02.01
*/

#ifndef  __NICO_H__
#define  __NICO_H__

/*======================================================*
 *　　　 宏和结构的定义(Macro & type definitions)  　　 *
 *======================================================*/
#if defined NEO_icon_support_unused
#define NEO_mouse_icon_unused
#endif


typedef struct
{
   PIC_RGB  palette[16];
   unsigned color64k[16];
   unsigned char buffer[CUR_SIDE];
}CURSOR, *Cursor_ptr, Cursor;


/*======================================================*
 *              函数声明(Function declare)              *
 *======================================================*/
void show_icon(char *curfile, int x, int y);
CURSOR *load_icon(char *filename);
void blit_icon(CURSOR *cur_p, int x, int y);
#define fir_icon_color_pos(fircol_index)/*已经被取消的函数*/
#define destroy_icon(i_ptr) free(i_ptr)

/*======================================================*
 *            函数定义(Function definitions)            *
 *======================================================*/
/*---------------------------------------------------*
 *函数功能: 定位光标调色盘的在系统调色盘中的位置.　  *
 *参数说明: fircol_index:光标调色盘第一个颜色在系 　 *
 *          统调色盘中的位置(默认在240号颜色处，即从 *
 *          240~255这16种颜色为光标显示所用)注意:　　*
 *          fircol_index必须为小于241的整数.当       *
 *       fircol_index为负数时只返回当前光标调色盘首颜*
 *          色在系统调色盘中的位置.                  *
 *返回说明: 返回当前光标调色盘首颜色在系统调色盘中的 *
 *          位置.            　　　　　　　　　　　  *
 *备    注: 用户程序可以直接调用.          　　　    *
 *---------------------------------------------------*/
#ifndef NEO_icon_support_unused

void show_icon(char *curfile, int x, int y)
{
   CURSOR *cur_p;

   cur_p = load_icon(curfile);
   if (cur_p) blit_icon(cur_p, x, y);
   destroy_icon(cur_p);
}


/*---------------------------------------------------*
 *函数功能: 将图标文件读入内存.　　　  　　　　　　　*
 *参数说明: filename:图标文件名.　                   *
 *          Cursor_ptr cur_p:结构Cursor_type声明的结 *
 *          构体cur_p                                *
 *返回说明: 无.              　　　　　　　　　　　  *
 *备    注: 用户程序可以直接调用.　　　　　　　　    *
 *---------------------------------------------------*/
CURSOR *load_icon(char *filename)
{
   Cursor_ptr cursor_p = NULL;
   FILE *fp;
   unsigned char map[512];
   unsigned char mask[128];
   int k = 0;
   int x = 0;
   int y = 0;
   int i, j, offset;

   fp = fopen(filename, "rb");
   if (fp == NULL)
      return NULL;

   if ((cursor_p = (CURSOR *)malloc( sizeof (CURSOR) ))==NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"load_icon", NO_MEMORY, 0};
      throw_error(error);
      #endif
   }

   fseek(fp, 62, SEEK_SET);
   _FREAD(cursor_p->palette, sizeof(PIC_RGB), 16, fp);
   _FREAD(map, 512, 1, fp);
   _FREAD(mask, 128, 1, fp);
   fclose(fp);

   memset(cursor_p->color64k, 0, 32);
   #ifndef NEO_color_depth_16_unused
   if (g_color_depth > 8)
   {
      for(offset=0; offset<16; ++offset)
      {
         cursor_p->color64k[offset] += ((cursor_p->palette[offset]).red>>3);
         cursor_p->color64k[offset] <<= g_green_mask;
         cursor_p->color64k[offset] += ((cursor_p->palette[offset]).green>>g_green_bit);
         cursor_p->color64k[offset] <<= 5;
         cursor_p->color64k[offset] += (cursor_p->palette[offset]).blue>>3;
      }
   }
   #endif

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      for(i = 31; i >= 0; --i)
      {
         for(j = 0; j < 32; j += 2)
         {
            cursor_p->buffer[( i<<5 ) + j] = (map[k] >> 4);
            cursor_p->buffer[( i<<5 ) + j + 1] = (map[k++] & 0x0f);
         }
      }
      #endif
   }
   else if (g_color_depth == 15 || g_color_depth == 16)
   {
      #ifndef NEO_color_depth_16_unused
      for(i = 31; i >= 0; --i)
      {
         for(j = 0; j < 32; j += 2)
         {
            cursor_p->buffer[( i<<5 ) + j] = (map[k] >> 4);
            cursor_p->buffer[( i<<5 ) + j + 1] = (map[k++] & 0x0f);
         }
      }
      #endif
   }

   for(i = 124; i >= 0; i -= 4)
   {
      for(j = i; j <= i + 3; ++j)
      {
         unsigned char m = 0x80;

         for(k = 0; k < 8; ++k)
         {
            if(mask[j] & m)
            {
               cursor_p->buffer[( y<<5 )+x] = (TPT8 + 16);
            }
            ++x;
            m >>= 1;
         }
      }
      ++y;
      x = 0;
   }

   return cursor_p;
}


/*---------------------------------------------------*
 *函数功能: 绘制(显示)已读入内存的光(图)标文件.      *
 *参数说明: Cursor_ptr cursor_p:指向                 *
 *          构体cursor_p                             *
 *          x,y:要显示的位置坐标                     *
 *返回说明: 无.              　　　　　　　　　　　  *
 *备    注: 用户程序可以直接调用.　　　    　　　    *
 *---------------------------------------------------*/
void blit_icon(CURSOR *cursor_p, int x, int y)
{
   int i, j;
   unsigned int index;

   if (g_color_depth == 8)
   {
      #ifndef NEO_color_depth_8_unused
      for(i = 0; i < 32; ++i)
      {
         for(j = 0; j < 32; j += 2)
         {
            if ((index = cursor_p->buffer[(i<<5)+j])  != (TPT8 + 16)) dot(x + j, y + i,
                makecol8((cursor_p->palette[index]).red,
               (cursor_p->palette[index]).green,
               (cursor_p->palette[index]).blue));
            if ((index = cursor_p->buffer[(i<<5)+j+1]) != (TPT8 + 16)) dot(x + j + 1, y + i,
                makecol8((cursor_p->palette[index]).red,
               (cursor_p->palette[index]).green,
               (cursor_p->palette[index]).blue));
         }
      }
      #else
      x += 0;
      y += 0;
      cursor_p += 0;
      #endif
   }
   #ifndef NEO_color_depth_16_unused
   else if (g_color_depth == 15 || g_color_depth == 16)
   {
      for(i = 0; i < 32; ++i)
      {
         for(j = 0; j < 32; ++j)
         {
            if ((cursor_p->buffer[(i << 5) + j]) != (TPT8 + 16))
            {
               dot(x + j, y + i, cursor_p->color64k[cursor_p->buffer[(i << 5) + j]]);
            }
         }
      }
   }
   #else
   x += 0;
   y += 0;
   cursor_p += 0;
   #endif
}
#endif


#endif
