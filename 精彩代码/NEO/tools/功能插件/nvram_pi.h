/*http://neo.coderlife.net*/
 /*ckerhome@yahoo.com.cn*/
 /* Plug-Ins of NEO SDK */
 /*显存直接读写插件 V0.1*/
 /* 创建日期:07/02/2005 */

#ifndef  VRAM_PI_H
#define  VRAM_PI_H

#define _VM_FIR_PAGE 0

void bm_vm_copy(void *bm_handle, unsigned long vm_off, unsigned length, char mode)
{
   unsigned offset = vm_off % 0xffff;
   unsigned page_bef = length, page_aft = 0, i;
   int  page_1 = (int)(vm_off >> 16);
   int  page_2   = (int)((vm_off + length) >> 16);
   int  page_bak = g_cur_vbe_page;
   char far *vpoint = g_videoptr + offset;
   int  far *_vpoint = (int far *)vpoint;
   int  far *_bm_handle = (int far *)bm_handle;

   if (page_1 != page_2)
   {
      page_aft = (vm_off + (long)length) % 0xffff;
      page_bef = length - page_aft;
   }
   set_vbe_page(page_1 + _VM_FIR_PAGE);

   if (mode != 0) /*显存到基本内存*/
   {  /*改变主从关系*/
      _vpoint = (int far *)bm_handle;
      _bm_handle = (int far *)vpoint;
   }

   for (i = 0; i < page_bef; i += 2)
   {
      *_vpoint++ = *_bm_handle++;
   }
   if (page_2 != page_1)
   {
      set_vbe_page(page_2 + _VM_FIR_PAGE);
      if (mode != 0) /*显存到基本内存*/
         _bm_handle = (int far *)g_videoptr;
      else
         _vpoint = (int far *)g_videoptr;
      for (i = page_bef; i < page_bef + page_aft; i += 2)
      {
         *_vpoint++ = *_bm_handle++;
      }
   }
   set_vbe_page(page_bak);
}


char set_scanline(BITMAP *bm, void *buf, int n_line)
{
   unsigned long offset;
   if (bm->flag == 0) return FALSE;

   if (bm->flag == 3)
   {
      if (n_line >= 0 && n_line < bm->height)
      {
         bm_vm_copy(buf, bm->vram_offset + (long)n_line * (long)(bm->width * (bm->bpp >> 3)), bm->width * (bm->bpp >> 3), 0);
      }
   }
   return TRUE;
}


char get_scanline(BITMAP *bm, void *buf, int n_line)
{

   if (bm->flag == 0) return FALSE;

   if (bm->flag == 3)
   {
      if (n_line >= 0 && n_line < bm->height)
      {
         bm_vm_copy(buf, bm->vram_offset + (long)n_line * (long)(bm->width * (bm->bpp >> 3)), bm->width * (bm->bpp >> 3), 1);
      }
   }
   return TRUE;
}


BITMAP *load_bmp(char *filename, PALETTE pal)
{
   BITMAPINFOHEADER bmpheader;
   unsigned bmp_wid;
   unsigned bmpheight;
   unsigned char wid_fix, bmpbits;

   if (read_win_bminfoheader(filename, &bmpheader) <= 0)
      return 0;
   /*bmp_wid = bmpheader.biWidth;
   bmpheight = bmpheader.biHeight;
   bmpbits = bmpheader.biBitCount;*/
}


BITMAP *create_bitmap(int width, int height)
{
   BITMAP *bm = malloc(sizeof(BITMAP));
   if (bm  == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"create_bitmap", NO_MEMORY, 0};
      throw_error(error);
      #endif
   }

   if (1)
   {
      bm->flag = 3;
      bm->width = width;
      bm->height= height;
      bm->vram_offset = _VM_FIR_PAGE << 16;
      bm->bpp = g_color_depth;
   }

   return bm;
}

#endif
