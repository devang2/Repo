/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nvesa.h
  摘    要 : 本头文件包含了NEO SDK里有关显卡硬件加速及显存、屏幕底层处理的函数、结构、全局
             变量的声明及定义
  当前版本 : 2.22
  作    者 : 董凯
  完成日期 : 2006.01.18

  取代版本 : 2.21
  原 作 者 : 董凯
  完成日期 : 2006.01.16
*/
#ifndef  __NVESA_H__
#define  __NVESA_H__


#if defined NEO_color_depth_16_unused && defined NEO_color_depth_8_unused
#undef NEO_color_depth_8_unused
#undef NEO_color_depth_16_unused
#endif


#define  VBE320X200X256   0X13
#define  VBE640X400X256   0X100
#define  VBE640X480X256   0X101
#define  VBE800X600X256   0X103
#define  VBE1024X768X256  0X105
#define  VBE1280X1024X256 0X107

#define  VBE320X200X32K   0X10D
#define  VBE640X480X32K   0X110
#define  VBE800X600X32K   0X113
#define  VBE1024X768X32K  0X116
#define  VBE1280X1024X32K 0X119

#define  VBE320X200X64K   0X10E
#define  VBE640X480X64K   0X111
#define  VBE800X600X64K   0X114
#define  VBE1024X768X64K  0X117
#define  VBE1280X1024X64K 0X11A

#define  SCREEN_W      g_screen_h
#define  SCREEN_H      g_screen_v

struct vbe_info_t
{
   char  VESA_signature[4];      /* 'VESA' 4 byte signature */
   short VBE_version;            /* VBE version number */
   char  far *OEM_string_ptr;    /* Pointer to OEM string */
   long  capabilities;           /* Capabilities of video card */
   unsigned far *video_mode_ptr;    /* Pointer to supported modes */
   short total_memory;        /* Number of 64kb memory blocks */
   char  reserved[492];       /* Pad to 492 byte block size */
};

/*typedef struct CRTCInfoBlock_T
{
   unsigned short horizontal_total;
   unsigned short horizontal_sync_start;
   unsigned short horizontal_sync_end;
   unsigned short vertical_total;
   unsigned short vertical_sync_start;
   unsigned short vertical_sync_end;
   Uint8  flags;
   unsigned long pixel_clock;
   unsigned short refresh_rate;
   char  reserved[40]; 
}CRTC_T;*/

typedef struct ModeInfoBlock_T
{
   /* Mandatory information for all VBE revisions */
unsigned short mode_attrib; /* Mode attributes */
Uint8  win_a_attrib; /* Window A attributes */
Uint8  win_b_attrib; /* Window B attributes */
unsigned short win_granularity; /* Window granularity in k */
unsigned short win_size; /* Window size in k */
unsigned short win_a_seg; /* Window A segment */
unsigned short win_b_seg; /* Window B segment */
void (far *win_func_ptr)(void); /* Pointer to window function */
unsigned short bytes_per_scan_line; /* Bytes per scanline */

/* Mandatory information for VBE 1.2 and above */
unsigned short x_resolution; /* Horizontal resolution */
unsigned short y_resolution; /* Vertical resolution */
Uint8  x_char_size; /* Character cell width */
Uint8  y_char_size; /* Character cell height */
Uint8  number_of_planes; /* Number of memory planes */
Uint8  bits_per_pixel; /* Bits per pixel */
Uint8  number_of_banks; /* Number of CGA style banks */
Uint8  memory_model; /* Memory model type */
Uint8  bank_size; /* Size of CGA style banks */
Uint8  number_of_image_pages; /* Number of images pages */
Uint8  res1; /* Reserved */
/* Direct Color fields (required for direct/6 and YUV/7 memory models) */
Uint8  red_mask_size; /* Size of direct color red mask */
Uint8  red_field_pos; /* Bit posn of lsb of red mask */
Uint8  green_mask_size; /* Size of direct color green mask */
Uint8  green_field_pos; /* Bit posn of lsb of green mask */
Uint8  blue_mask_size; /* Size of direct color blue mask */
Uint8  blue_field_pos; /* Bit posn of lsb of blue mask */
Uint8  rsvd_mask_size; /* Size of direct color res mask */
Uint8  rsvd_field_pos; /* Bit posn of lsb of res mask */
Uint8  direct_color_mode_info; /* Direct color mode attributes */

/* Mandatory information for VBE 2.0 and above*/
void far *phys_base_ptr;
void far *res2;
unsigned short res3;

/* Mandatory information for VBE 3.0 and above*/
unsigned short bytes_per_scanline;
Uint8  bnk_num_of_imagepages;
Uint8  lin_num_of_imagepages;
Uint8  lin_red_mask_size;
Uint8  lin_red_field_pos;
Uint8  lin_green_mask_size;
Uint8  lin_green_field_pos;
Uint8  lin_blue_mask_size;
Uint8  lin_blue_field_pos;
Uint8  lin_rsvd_mask_size;
Uint8  lin_rsvd_field_pos;
void far *max_pixel_clock;

Uint8  res4[189]; /* Pad to 256 byte block size */
}mode_info_t;

typedef union
{
   Uint16 color;
   Uint16 c;
}RGB16;

typedef struct
{
   Uint8 r;
   Uint8 g;
   Uint8 b;
}RGB24, RGB;

typedef struct
{
   Uint8 blue;
   Uint8 green;
   Uint8 red;
   Uint8 reserved;
}PIC_RGB;


enum MASK_COLOR
{
   MASK_COLOR_8  = 0X0,
   MASK_COLOR_15 = 0X7C1F,
   MASK_COLOR_16 = 0XF81F,
};

/*#define  MASK_COLOR_24  0XFF00FFL*/
#define  TPT8   MASK_COLOR_8
#define  TPT15  MASK_COLOR_15
#define  TPT16  MASK_COLOR_16

enum DRAWIMAGEOPS
{
   COPY_PUT,
   XOR_PUT,
   OR_PUT,
   AND_PUT,
   NOT_PUT
};

#define _PAL_SIZE     256
typedef RGB24 PALLETE[_PAL_SIZE];
#define PALETTE       PALLETE

#ifndef NEO_sys_pal_bak_unused
PALETTE g_sys_palette;
#define NEO_SYS_PALLETE g_sys_palette
#endif

/*===============NEO的私有全局变量(请勿擅自俢改,以免造成不可预料的错误!)================*/
long g_screen_size;        /*记录当前图形模式的屏幕面积*/
int  g_screen_h;           /*当前分辨率下的屏幕宽度*/
int  g_screen_v;           /*当前分辨率下的屏幕高度*/
int  g_clear_vram;    /*清除显存标志，0表示每次显示模式的改变都同步清除显存原有内容*/
int  g_cur_vbe_page;    /*当前所处的显示页*/
char g_work_surface;   /*当前工作页*/
char g_look_surface;   /*当前可视页*/
char g_vbe_version = -1;   /*VBE的主版本号*/
char g_vbe_sub_ver;   /*VBE的副版本号*/
char g_total_surfaces;  /*记录能创建的后台页的总量，视显存大小而定*/
char g_alloc_surface;  /*已经申请的后台页数*/
char g_reserved_page = 1;  /*保留页面张数*/
char far *g_videoptr = (char far *)0xa0000000L;
char g_page_num;
char g_green_mask;
char g_green_bit;
char g_color_depth;    /*记录当前图形模式的色深，为0表示非图形模式*/
char g_color_byte;    /*记录当前图形模式每个点占用的字节数*/
Uint16 g_sl_offset;
Uint16 g_total_vram;  /*总显存数*/
/*=======================================================================================*/
int  g_frt_color = 1;
int  g_tptcolor;     /*本开发包指定透明色(Transparent color)*/
char g_draw_mode;     /*绘图模式,分为COPY_PUT,XOR_PUT,OR_PUT,AND_PUT,NOT_PUT,与TC中Graphics的绘图模式意义相同*/
/*以下4个变量用来支持裁剪输出,分别代表裁剪区域的左上角及右下角坐标（用户通过screen()函数间接设置）*/
int  g_rect_left;
int  g_rect_right;
int  g_rect_top;
int  g_rect_bottom;
int  g_temp_top;
int  g_temp_left;
int  g_temp_right;
int  g_temp_bottom;

/*======================================================*
 *              函数声明(Function declare)              *
 *======================================================*/
int  get_mode_info(int mode, char far *modeinfo);
char set_video_mode(int w, int h, int bpp, int refresh, int flags);
int  get_vbe_mode(void);
void set_vga_mode(int mode);
char get_page_num(void);
void set_display_start(unsigned fir_scan_dot, unsigned fir_scan_line);

char screen_rect(int left, int top, int right, int bottom);
void rect_store(void);
void rect_restore(void);

void set_vbe_page(int page);

extern void rectfill(int x1, int y1, int x2, int y2, int color);
char surface_alloc(unsigned sur_num, int clear_flag);
char set_work_surface(unsigned sur_num);
char flip_surface(unsigned sur_num);
void flip(void);

void set_draw_mode(int draw_mode);
void vsync(void);

void get_palette_range(char *pal_buf, int from, int to);
extern void show_mouse(void);
extern void hide_mouse(void);
extern void scare_mouse(void);
extern void unscare_mouse(void);

/*======================================================*
 *            函数定义(Function definitions)            *
 *======================================================*/
#define max_x()              g_screen_h
#define max_y()              g_screen_v
#define get_color_depth()    g_color_depth
#define clear_vram_on()      g_clear_vram = 0
#define clear_vram_off()     g_clear_vram = 0x8000
#define get_current_page()   g_cur_vbe_page
#define get_total_vram()     (g_total_vram >> 4)
#define get_total_surfaces() g_total_surfaces
#define surface_realloc(sur_num,flag) surface_alloc(sur_num,flag)
#define set_vbe_mode(mode)   set_video_mode(0, 0, 0, 0, mode << 7)
#define screen(x1, y1, x2, y2) screen_rect(x1, y1, x2, y2)

#ifndef NEO_color_depth_8_unused
#ifndef NEO_palette_unused
#define _get_palette_range(pal, from, to) get_palette_range((char *)pal, from, to)
#define update_sys_pal()                 _get_palette_range(g_sys_palette, 0, 255)
#endif
#endif
int get_mode_info(int mode, /*mode_info_t **/char far *modeinfo)
{
   union REGS in,out;
   struct SREGS segs;

   if (mode < 0x100)
   {
      return mode == 0x13 ? TRUE : FALSE;
   }
   in.x.ax = 0x4F01;
   in.x.cx = mode;
   in.x.di = FP_OFF(modeinfo);
   segs.es = FP_SEG(modeinfo);
   int86x(0x10, &in, &out, &segs);
   return (out.x.ax == 0x4F);
}


/*#define HPOS         0
#define HNEG         (1 << 2)
#define VPOS         0
#define VNEG         (1 << 3)
#define INTERLACED   (1 << 1)
#define DOUBLESCAN   (1 << 0)
static int current_refresh_rate = 0;
int _refresh_rate_request;
extern unsigned long __tb;*/

/* request_refresh_rate:
 *  Requests that the next call to set_gfx_mode() use the specified refresh
 *  rate.
 */
/*void request_refresh_rate(int rate)
{
   _refresh_rate_request = rate;
}*/

/* _set_current_refresh_rate:
 *  Sets the current refresh rate.
 *  (This function must be called by the gfx drivers)
 */
/*void _set_current_refresh_rate(int rate)
{
   if ((rate<40) || (rate>200))
      rate = 0;

   current_refresh_rate = rate;
}*/

/* calc_crtc_timing:
 *  Calculates CRTC mode timings.
 */
/*static void calc_crtc_timing(CRTC_T *crtc, int xres, int yres, int xadjust, int yadjust)
{
   int HTotal, VTotal;
   int HDisp, VDisp;
   int HSS, VSS;
   int HSE, VSE;
   int HSWidth, VSWidth;
   int SS, SE;
   int doublescan = FALSE;

   if (yres < 400){
      doublescan = TRUE;
      yres *= 2;
   }

   HDisp = xres;
   HTotal = (int)(HDisp * 1.27) & ~0x7;
   HSWidth = (int)((HTotal - HDisp) / 5) & ~0x7;
   HSS = HDisp + 16;
   HSE = HSS + HSWidth;
   VDisp = yres;
   VTotal = VDisp * 1.07;
   VSWidth = (VTotal / 100) + 1;
   VSS = VDisp + ((int)(VTotal - VDisp) / 5) + 1;
   VSE = VSS + VSWidth;

   SS = HSS + xadjust;
   SE = HSE + xadjust;

   if (xadjust < 0) {
      if (SS < (HDisp + 8)) {
    SS = HDisp + 8;
    SE = SS + HSWidth;
      }
   }
   else {
      if ((HTotal - 24) < SE) {
    SE = HTotal - 24;
    SS = SE - HSWidth;
      }
   }

   HSS = SS;
   HSE = SE;

   SS = VSS + yadjust;
   SE = VSE + yadjust;

   if (yadjust < 0) {
      if (SS < (VDisp + 3)) {
    SS = VDisp + 3;
    SE = SS + VSWidth;
      }
   }
   else {
      if ((VTotal - 4) < SE) {
    SE = VTotal - 4;
    SS = SE - VSWidth;
      }
   }

   VSS = SS;
   VSE = SE;

   crtc->horizontal_total     = HTotal;
   crtc->horizontal_sync_start = HSS;
   crtc->horizontal_sync_end   = HSE;
   crtc->vertical_total       = VTotal;
   crtc->vertical_sync_start   = VSS;
   crtc->vertical_sync_end     = VSE;
   crtc->flags               = HNEG | VNEG;

   if (doublescan)
      crtc->flags |= DOUBLESCAN;
}*/


/*void far (*g_win_func)();*/
/*設置SVGA的显示模式*/
char set_video_mode(int w, int h, int bpp, int refresh, int flags)
{
   mode_info_t mode_info;

   struct   SREGS segs;
   union    REGS r;
   int      mode = 0;
   int      temp;
   unsigned min_mode = 0x100;
   unsigned max_mode = 0x1ff;
   unsigned far *p = &min_mode;
   char   far *modeinfo = (char far *)&mode_info;

   refresh += 0; /*该参数暂时未使用*/
   if (g_vbe_version < 0)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"set_video_mode", N_NEO_NOT_INIT, 0};
      throw_error(error);
      #endif     
   }

   temp = (flags & 0xff80) >> 7;
   if (temp == 0)
   {
      for (; *p < max_mode; (*p)++)
      {
         if (get_mode_info(*p, modeinfo))
         {
            if (w == mode_info.x_resolution && h == mode_info.y_resolution && bpp == mode_info.bits_per_pixel)
            {
               mode = *p;
               break;
            }
         }
      }
      if (mode == 0)
      {
         #ifndef NEO_sys_report_error_unused
         Errinfo_t error = {"set_video_mode", ERR_VIDEO_MODE, 1};
         throw_error(error);
         return FALSE;
         #endif
      }
   }
   else
   {
      if (!get_mode_info(mode = temp, modeinfo))
      {
         return FALSE;
      }
   }
   /*线性寻址  real_mode |= 0x4000;*/
   /*调整刷新率real_mode |= 0x0800;*/
   /*{
      CRTC_T   crtc;
      unsigned long vclk;
      double f0;
      _refresh_rate_request = refresh;
      calc_crtc_timing(&crtc, w, h, 0, 0);

      vclk = (double)crtc.horizontal_total * crtc.vertical_total * _refresh_rate_request;
      vclk = get_closest_pixel_clock(vesa_mode, vclk);

      if (vclk != 0) {
    f0 = (double)vclk / (crtc.horizontal_total * crtc.vertical_total);

    _set_current_refresh_rate((int)(f0 + 0.5));

    crtc.pixel_clock  = vclk;
    crtc.refresh_rate = _refresh_rate_request * 100;

    r.x.di = FP_OFF(&crtc);
    segs.es = FP_SEG(&crtc);
      }
   }*/

   if (mode >= 0x100)
   {  /*mode |= 0x0800;*/
      mode |= g_clear_vram;
      /*切换到指定图形模式*/
      r.x.ax = 0x4f02;
      r.x.bx = mode;
      int86x(0x10, &r, &r, &segs);
      if (r.x.ax != 0x4F) return FALSE; /*切换失败*/
   }
   else
   {
      if (mode == 0x13)/*该模式不支持硬件换页机制等特性*/
      {
         r.h.ah=0;
         r.h.al=mode;
         int86(0x10,&r,&r);
         g_color_depth = 8;
         g_color_byte  = (char)(g_color_depth / 7);
         g_rect_left   = g_rect_top = 0;
         g_rect_right  = (g_screen_h = 320) - 1;
         g_rect_bottom = (g_screen_v = 200) - 1;
         g_screen_size = g_screen_h * g_screen_v;
         g_tptcolor    = MASK_COLOR_8;
         screen(0, 0, g_rect_right, g_rect_bottom);
         #ifndef NEO_color_depth_8_unused
         #ifndef NEO_palette_unused
         update_sys_pal();
         #endif
         #endif
         return TRUE;
      }
      else
         return FALSE; /* 忽略其它非VESA标准模式*/
   }

   g_videoptr    = (char far *)MK_FP(0xa000, 0);
   g_green_mask  = mode_info.green_mask_size;
   g_color_depth = mode_info.bits_per_pixel;
   g_color_byte  = (char)(g_color_depth / 7);
   g_rect_left   = g_rect_top = 0;
   g_rect_right  = (g_screen_h = mode_info.x_resolution) - 1;
   g_rect_bottom = (g_screen_v = mode_info.y_resolution) - 1;
   g_screen_size = g_screen_h * g_screen_v;
   g_page_num    = get_page_num();
   screen(0, 0, g_rect_right, g_rect_bottom);
   g_total_surfaces = mode_info.number_of_image_pages * 2;
   if (mode == 0x101) g_total_surfaces += 3;
   else if (mode == 0x116 || mode == 0x117) g_total_surfaces = 3; /*处理特殊情况*/
   else if (mode == 0x10e || mode == 0x111 || mode == 0x103) g_total_surfaces += 1;

   if (g_page_num == -1)
   {
      g_page_num = g_screen_size / (bpp * 0xffff) + 1;
   }
   switch (g_color_depth)
   {
      case 8:
      g_tptcolor = MASK_COLOR_8;
      #ifndef NEO_color_depth_8_unused
      #ifndef NEO_palette_unused
      update_sys_pal();
      #endif
      #endif
      break;
      case 15:
      g_tptcolor = MASK_COLOR_15;
      g_green_bit= 3;
      break;
      case 16:
      g_tptcolor = MASK_COLOR_16;
      g_green_bit= 2;
      break;
      /*case 24:
      g_tptcolor = TPT24;
      break;
      case 32:
      g_tptcolor = TPT32;
      break;*/
      default:
      g_tptcolor = TPT8;
      break;
   }
   g_routines |= 1;

   return TRUE;
}

/*获得SVGA的显示模式*/
int get_vbe_mode(void)
{
   _AX = 0x4f03;
   __int__(0x10); 

   return _BX;
}

void set_vga_mode(int mode)
{
   _AX = mode;  /*回到VGA模式下*/
   __int__(0x10);
}

char get_page_num(void)
{
   char pages[28] = {4, 5, 4, 8, 6, 12, 10, 20, -1, -1, -1, -1, -1, 2, 2, 3, 10, 10, 15,
                     15, 15,  22, 24, 24, 36, 40, 40, 60};
   int tmp;
   _AX = 0x4f03;
   __int__(0x10);   
   tmp = _BX;
   if (tmp >= 0x100 && tmp <= 0x11c)
   {
      return pages[tmp - 256];
   }
   else if (tmp == 0x13)
   {
      return 1;
   }
   return -1;
}


/*设置扫描线的起点*/
void set_display_start(unsigned fir_scan_dot, unsigned fir_scan_line)
{
   union REGS r;
   r.x.ax = 0x4f07;
   r.x.bx = 0;
   r.x.cx = fir_scan_dot;
   r.x.dx = fir_scan_line;
   int86(0x10, &r, &r);
}


/*指定剪切输出域(包括边线)*/
char screen_rect(int left, int top, int right, int bottom)
{
   if(left>g_screen_h||top>g_screen_v||right<0||bottom<0||right<=left||bottom<=top)
      {return -1;}
   #ifndef NEO_rect_unused
   if (left < 0)
      {left = 0;}
   if (top < 0)
      {top = 0;}
   if (right >= g_screen_h)
      {right =  g_screen_h - 1;}
   if (bottom >=g_screen_v)
      {bottom = g_screen_v - 1;}

   g_rect_left  = left;
   g_rect_right = right;
   g_rect_top   = top;
   g_rect_bottom= bottom;
   #else
   g_rect_left  = left = g_rect_top   = top  = 0;
   g_rect_right = right= g_screen_h - 1;
   g_rect_bottom= bottom=g_screen_v - 1;
   #endif
   return 0;
}

#ifndef NEO_rect_unused
void rect_store(void)
{
   g_temp_top = g_rect_top;
   g_temp_left= g_rect_left;
   g_temp_right = g_rect_right;
   g_temp_bottom= g_rect_bottom;
}

void rect_restore(void)
{
   g_rect_top = g_temp_top;
   g_rect_left= g_temp_left;
   g_rect_right = g_temp_right;
   g_rect_bottom= g_temp_bottom;
}
#endif


/*显存换页函数*/
void set_vbe_page(int page)
{
   if (g_cur_vbe_page != page)
   {
      _BX = 0;
      _DX = g_cur_vbe_page = page;
      _AX = 0x4f05;
      __int__(0x10);
   }
}
/*下面这个函数功能与上面一样，但效率似乎更高些*/
/*void set_vbe_page(int page)
{
   if (g_cur_vbe_page != page)
   {
      _BX=0;
      _DX=g_cur_vbe_page = page;
      g_win_func();
   }
}*/


/*void set_logic_scanl_len(unsigned length)
{
   union REGS r;
   r.x.ax=0x4F06;*/
   /* BL = 00h Set Scan Line Length in Pixels
         = 01h Get Scan Line Length
         = 02h Set Scan Line Length in Bytes
         = 03h Get Maximum Scan Line Length */
   /*r.h.bl = 0;
   r.x.cx = length;
   int86(0x10, &r, &r);
}*/


#ifndef NEO_surface_unused
char surface_alloc(unsigned sur_num, int clear_flag)
{ 
   g_alloc_surface = (sur_num <= g_total_surfaces? sur_num : g_total_surfaces);

   if (clear_flag)
   {
      int i;
      #ifndef NEO_rect_unused
      rect_store();
      #endif
      screen(0, 0, g_screen_h - 1, g_screen_v - 1);
      for (i = 1; i <= g_alloc_surface; ++i)
      {
         set_work_surface(i);
         #ifndef NEO_basic_draw_unused
         rectfill(0, 0, g_rect_right, g_rect_bottom, 0);
         #endif
      }
      #ifndef NEO_rect_unused
      rect_restore();
      #endif
      set_work_surface(0);
   }
   return g_alloc_surface;
}


char set_work_surface(unsigned sur_num)
{
   if (sur_num <= g_alloc_surface && sur_num > 0)
   {
      g_sl_offset = g_screen_v * ((g_work_surface = sur_num) - 1) + (g_screen_v * g_reserved_page)/*跳过系统保留页*/;
   }
   else
   {
      g_work_surface = g_sl_offset = 0;
   }
   return g_work_surface;
}

       
char flip_surface(unsigned sur_num)
{                                                                                           /*跳过系统保留页*/
   unsigned temp = (sur_num <= g_alloc_surface && sur_num > 0)?g_screen_v * (sur_num - 1) + (g_screen_v * g_reserved_page) : 0;

   #ifndef NEO_draw_smooth_unused
   vsync();
   #endif
   if (g_routines & 2)
   {
      #ifndef NEO_mouse_adv_unused
      hide_mouse();   /*隐藏Patch原来的光标*/
      #endif
   }

   set_display_start(0, temp);
   g_look_surface = (sur_num <= g_alloc_surface?sur_num : 0);
   if (g_routines & 2)
   {
      #ifndef NEO_mouse_adv_unused
      show_mouse();   /*绘制redraw新的光标*/
      #endif
   }
   return g_look_surface;
}


void flip(void)
{
   static unsigned last = 0;                 /*跳过系统保留页*/

   #ifndef NEO_draw_smooth_unused
   vsync();
   #endif
   if (g_routines & 2)
   {
      #ifndef NEO_mouse_adv_unused
      hide_mouse();
      #endif
   }

   set_display_start(0, last = (last == 0?g_screen_v * g_reserved_page : 0));
   g_look_surface = last != 0?1 : 0;
   if (g_routines & 2)
   {
      #ifndef NEO_mouse_adv_unused
      show_mouse();
      #endif
   }
}
#endif


#ifndef NEO_draw_mode_unused
void set_draw_mode(int draw_mode)
{
   if ((draw_mode>=COPY_PUT)&&(draw_mode<=NOT_PUT)) g_draw_mode=draw_mode;
}
#endif

void vsync(void)
{
   while  ( inportb(0x3da)&0x08 ) ;
   while (!(inportb(0x3da)&0x08)) ;
}


#endif
