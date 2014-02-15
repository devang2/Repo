/*NEO SDK V2.1.41 For DOS
  Copyleft Cker Home 2003-2005.

  Open Source Obey NEO_PL.TXT.
  http://neo.coderlife.net
  ckerhome@yahoo.com.cn

  文件名称 : ndraw.h
  摘    要 : 本头文件包含了NEO SDK里有关显卡硬件加速及显存、屏幕底层处理的函数、结构、全局
             变量的声明及定义
  当前版本 : 4.24
  作    者 : 董凯
  完成日期 : 2005.6.29

  取代版本 : 4.22
  原 作 者 : 董凯
  完成日期 : 2005.6.19
*/
#ifndef  NDRAW_H
#define  NDRAW_H

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
#define  _UP_ROLL      0
#define  _DOWN_ROLL    1
#define  _LEFT_ROLL    2
#define  _RIGHT_ROLL   3

#define  _WHITE   0xf
#define  _BLACK   0

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

typedef struct CRTCInfoBlock_T
{
   unsigned short horizontal_total;
   unsigned short horizontal_sync_start;
   unsigned short horizontal_sync_end;
   unsigned short vertical_total;
   unsigned short vertical_sync_start;
   unsigned short vertical_sync_end;
   unsigned char  flags;
    char    far  *pixel_clock;
   unsigned short refresh_rate;
   char  reserved[40]; 
}CRTC_T;

typedef struct ModeInfoBlock_T
{
   /* Mandatory information for all VBE revisions */
unsigned short mode_attributes; /* Mode attributes */
unsigned char  win_a_attributes; /* Window A attributes */
unsigned char  win_b_attributes; /* Window B attributes */
unsigned short win_granularity; /* Window granularity in k */
unsigned short win_size; /* Window size in k */
unsigned short win_a_segment; /* Window A segment */
unsigned short win_b_segment; /* Window B segment */
void (far *win_func_ptr)(void); /* Pointer to window function */
unsigned short bytes_per_scan_line; /* Bytes per scanline */

/* Mandatory information for VBE 1.2 and above */
unsigned short x_resolution; /* Horizontal resolution */
unsigned short y_resolution; /* Vertical resolution */
unsigned char  x_char_size; /* Character cell width */
unsigned char  y_char_size; /* Character cell height */
unsigned char  number_of_planes; /* Number of memory planes */
unsigned char  bits_per_pixel; /* Bits per pixel */
unsigned char  number_of_banks; /* Number of CGA style banks */
unsigned char  memory_model; /* Memory model type */
unsigned char  bank_size; /* Size of CGA style banks */
unsigned char  number_of_image_pages; /* Number of images pages */
unsigned char  res1; /* Reserved */
/* Direct Color fields (required for direct/6 and YUV/7 memory models) */
unsigned char  red_mask_size; /* Size of direct color red mask */
unsigned char  red_field_position; /* Bit posn of lsb of red mask */
unsigned char  green_mask_size; /* Size of direct color green mask */
unsigned char  green_field_position; /* Bit posn of lsb of green mask */
unsigned char  blue_mask_size; /* Size of direct color blue mask */
unsigned char  blue_field_position; /* Bit posn of lsb of blue mask */
unsigned char  rsvd_mask_size; /* Size of direct color res mask */
unsigned char  rsvd_field_position; /* Bit posn of lsb of res mask */
unsigned char  direct_color_mode_info; /* Direct color mode attributes */

/* Mandatory information for VBE 2.0 and above*/
void far *phys_base_ptr;
void far *res2;
unsigned short res3;

/* Mandatory information for VBE 3.0 and above*/
unsigned short bytes_per_scanline;
unsigned char  bnk_num_of_imagepages;
unsigned char  lin_num_of_imagepages;
unsigned char  lin_red_mask_size;
unsigned char  lin_red_field_pos;
unsigned char  lin_green_mask_size;
unsigned char  lin_green_field_pos;
unsigned char  lin_blue_mask_size;
unsigned char  lin_blue_field_pos;
unsigned char  lin_rsvd_mask_size;
unsigned char  lin_rsvd_field_pos;
void far *max_pixel_clock;

unsigned char  res4[189]; /* Pad to 256 byte block size */
}mode_info_t;

typedef union
{
   unsigned char c1;
   unsigned char c2;
   unsigned color;
}RGB16;

typedef struct RGB
{
   unsigned char r;
   unsigned char g;
   unsigned char b;
}RGB24, RGB;

typedef struct PIC_RGB
{
   unsigned char blue;
   unsigned char green;
   unsigned char red;
   unsigned char reserved;
}PIC_RGB, *PIC_RGB_ptr;

enum MASK_COLOR
{
   TPT8  = 0X0,
   TPT15 = 0X7C1F,
   TPT16 = 0XF81F,
   /*TPT24 = 0XFF00FFL,
   TPT32 = 0XFF00FFL,*/
};

    
enum DRAWIMAGEOPS
{
   COPY_PUT,
   XOR_PUT,
   OR_PUT,
   AND_PUT,
   NOT_PUT
};

/*===============NEO的私有全局变量(请勿擅自俢改,以免造成不可预料的错误!)================*/
char far *g_videoptr = (char far *)0xa0000000;
char g_page_num    = 0;
char g_green_mask  = 0;
char g_green_bit   = 0;
char g_color_depth = 0;    /*记录当前图形模式的色深，为0表示非图形模式*/
char g_color_byte  = 0;    /*记录当前图形模式每个点占用的字节数*/
int  g_screen_h;           /*当前分辨率下的屏幕宽度*/
int  g_screen_v;           /*当前分辨率下的屏幕高度*/
int  g_clear_vram  = 0;    /*清除显存标志，0表示每次显示模式的改变都同步清除显存原有内容*/
int  g_cur_vbe_page= 0;    /*当前所处的显示页*/
long g_screen_size;        /*记录当前图形模式的屏幕面积*/
/*char g_last_surface = 0;   换页前所在页号*/
char g_work_surface = 0;   /*当前工作页*/
char g_look_surface = 0;   /*当前可视页*/
char g_vbe_version = -1;   /*VBE的主版本号*/
char g_vbe_sub_ver =  0;   /*VBE的副版本号*/
char g_total_surfaces =0;  /*记录能创建的后台页的总量，视显存大小而定*/
char g_alloc_surface = 0;  /*已经申请的后台页数*/
char g_reserved_page = 1;  /*保留页面张数*/
char g_flip_flag = 0;      /*换页状态标志，用来与鼠标例程通讯*/
unsigned g_sl_offset  = 0;
unsigned g_total_vram = 0; /*总显存数*/
unsigned g_blit_fix;       /*NEO目前用于blit()函数的临时变量*/
/*=======================================================================================*/
int  g_frt_color = 1;
int  g_tptcolor  = 0;     /*本开发包指定透明色(Transparent color)*/
char g_draw_mode = 0;     /*绘图模式,分为COPY_PUT,XOR_PUT,OR_PUT,AND_PUT,NOT_PUT,与TC中Graphics的绘图模式意义相同*/
/*以下4个变量用来支持裁剪输出,分别代表裁剪区域的左上角及右下角坐标（用户通过screen()函数间接设置）*/
int  g_rect_left;
int  g_rect_right;
int  g_rect_top;
int  g_rect_bottom;
int  g_temp_top;
int  g_temp_left;
int  g_temp_right;
int  g_temp_bottom;
double  g_time_total  = 0.0;
char g_dac_size_fix = 2;    /*记录DAC调色板宽度修正值，由8-当前DAC宽度得来，默认为2*/

#ifndef _MASK_COLOR
#define _MASK_COLOR   g_tptcolor
#endif

#define retrace_count g_time_total
#define _PAL_SIZE     256
typedef RGB24 PALLETE[_PAL_SIZE];
#define PALETTE       PALLETE

#ifndef NEO_palette_unused
PALETTE g_black_palette;
#endif

/*======================================================*
 *              函数声明(Function declare)              *
 *======================================================*/
char set_vbe_mode(int mode);
int  get_vbe_mode(void);
char get_page_num(void);
void set_display_start(unsigned fir_scan_dot, unsigned fir_scan_line);

char screen(int left, int top, int right, int bottom);
void rect_store(void);
void rect_restore(void);

void set_vbe_page(int page);

extern void rectfill(int x1, int y1, int x2, int y2, int color);
char surface_alloc(unsigned sur_num, int clear_flag);
char set_work_surface(unsigned sur_num);
char flip_surface(unsigned sur_num);
void flip(void);

void set_draw_mode(int draw_mode);

void set_neo_color(void);
void set_palette_range(char *pal_buf, int from, int to, int vsync_flag);
void _set_palette_range(PALETTE pal_buf, int from, int to, int vsync_flag);
void get_palette_range(char *pal_buf, int from, int to);
void _get_palette_range(PALETTE pal_buf, int from, int to);
char set_color(int index, char red,char green, char blue);
void set_dac_size(unsigned char size);
char get_dac_size(void);
void set_palette(char *pal_buf);
void _set_palette(PALETTE pal_buf);
void get_palette(char *pal_buf);
void _get_palette(PALETTE pal_buf);
unsigned char get_red(int index);
unsigned char get_green(int index);
unsigned char get_blue(int index);

void (*fade_callback)() = NULL;
void fade_interpolate(PALETTE source, PALETTE dest, PALETTE output, int pos, int from, int to);
void fade_from_range(PALETTE source, PALETTE dest, int speed, int from, int to);
void fade_in_range(PALETTE p, int speed, int from, int to);
void fade_out_range(int speed, int from, int to);
void _fade_in(PALETTE p, int speed);
void fade_in(PALETTE p, unsigned char dest_color, int speed);
void _fade_out(int speed);
void fade_out(unsigned char dest_color, int speed);
void vsync(void);

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
#define makecol16(r,g,b)     ((((((r)>>3)<<g_green_mask)+((g)>>g_green_bit))<<5)+((b)>>3))
#define getr8(c)             get_red(c)
#define getg8(c)             get_green(c)
#define getb8(c)             get_blue(c)
#define getr15(c)            (((c)&0x7c00)>>10)
#define getg15(c)            (((c)&0x3e0) >>5)
#define getb15(c)            ((c)&0x1f)
#define getr16(c)            (((c)&0xf800)>>11)
#define getg16(c)            (((c)&0x7e0) >>5)
#define getb16(c)            ((c)&0x1f)
#define getr24(c)            (((c)&0xff0000)>>16)
#define getg24(c)            (((c)&0xff00)>>8)
#define getb24(c)            (c&0xff)
#define getr32(c)            getr24(c)
#define getg32(c)            getg24(c)
#define getb32(c)            getg24(c)

/*void far (*g_win_func)();*/
/*int g_refresh_rate = 6000;*/
/*設置SVGA的显示模式*/
char set_vbe_mode(int mode)
{
   mode_info_t mode_info;
   CRTC_T crtc_info;
   struct SREGS segs;
   union  REGS r;
   int    real_mode;
   char   far *modeinfo = (char far *)&mode_info;
   char   far *crtcinfo = (char far *)&crtc_info;
   /*char   p_c = 65;*/
   if (g_vbe_version < 0)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"set_vbe_mode", N_NEO_NOT_INIT, 0};
      throw_error(error);
      #endif     
   }
  /*crtc_info.refresh_rate = g_refresh_rate;
   crtc_info.flags = 3;
   crtc_info.pixel_clock =  &p_c;
   crtc_info.horizontal_total = 1360;crtc_info.vertical_total = 802;*/

   real_mode = (mode & 0x1ff) + g_clear_vram;
   /*线性寻址real_mode |= 0x4000;*/
   /*real_mode |= 0x800;*//*4800*/
   /*切换到指定图形模式*/
   r.x.ax = 0x4f02;
   r.x.bx = real_mode;
   r.x.di = FP_OFF(crtcinfo);
   segs.es = FP_SEG(crtcinfo);
   int86x(0x10, &r, &r, &segs);

   if (mode >= 0x100)
   {
      r.x.ax = 0x4f01;
      r.x.cx = mode;
      r.x.di = FP_OFF(modeinfo);
      segs.es = FP_SEG(modeinfo);
      int86x(0x10, &r, &r, &segs);
      if (r.x.ax != 0x4F) return FALSE; /*显卡不支持VESA标准*/
   }
   else
   {
      if (mode == 0x13)/*该模式不支持硬件换页机制*/
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
         g_tptcolor = TPT8;
         screen(0, 0, g_rect_right, g_rect_bottom);
         return TRUE;
      }
      else
         return FALSE; /* 忽略非VBE模式*/
   }

   #ifndef NEO_bmp_masked_blit_unused
   switch (mode) /*临时使用*/
   {
      case 0x101:
      g_blit_fix = 45056;
      break;
      case 0x103:
      g_blit_fix = 21248;
      break;
      case 0x105:
      g_blit_fix = 65534;
      break;
      default:
      g_blit_fix = 0;
      break;
   }
   #endif
     /* g_win_func = mode_info.win_func_ptr;*/
   g_green_mask  = mode_info.green_mask_size;
   g_color_depth = mode_info.bits_per_pixel;
   g_color_byte  = (char)(g_color_depth / 7);
   g_rect_left   = g_rect_top = 0;
   g_rect_right  = (g_screen_h = mode_info.x_resolution) - 1;
   g_rect_bottom = (g_screen_v = mode_info.y_resolution) - 1;
   g_screen_size = g_screen_h * g_screen_v;
   screen(0, 0, g_rect_right, g_rect_bottom);
   g_total_surfaces = mode_info.number_of_image_pages << 1;
   if (mode == 0x101) g_total_surfaces += 3;
   else if (mode == 0x116 || mode == 0x117) g_total_surfaces = 3; /*处理特殊情况*/
   else if (mode == 0x10e || mode == 0x111 || mode == 0x103) g_total_surfaces += 1;

   switch (g_color_depth)
   {
      case 8:
      g_tptcolor = TPT8;
      break;
      case 15:
      g_tptcolor = TPT15;
      g_green_bit= 3;
      break;
      case 16:
      g_tptcolor = TPT16;
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
   g_page_num = get_page_num();
   return TRUE;
}


/*获得SVGA的显示模式*/
int get_vbe_mode(void)
{
#ifndef MSVC15
   _AX = 0x4f03;
   __int__(0x10); 
   return _BX;
#else
   int _bx;
   _asm
	{
	mov ax,4f03h
	int 10h
	mov _bx,bx
	}
	return _bx;
#endif
}

char get_page_num(void)
{
   char pages[28] = {4, 5, 4, 8, 6, 12, 10, 20, -1, -1, -1, -1, -1, 2, 2, 3, 10, 10, 15,
                     15, 15,  22, 24, 24, 36, 40, 40, 60};
   int tmp;
#ifndef MSVC15
   _AX = 0x4f03;
   __int__(0x10);   
#else
   int _bx;
   _asm
	{
	mov ax,4f03h
	int 10h
	mov _bx,bx
	}
#endif

#ifndef MSVC15
   if ((tmp = _BX) >= 0x100)
#else
   if ((tmp = _bx) >= 0x100)
#endif
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
#ifndef MSVC15
   _AX = 0x4f07;
   _BH = 0;
   _BL = 0;
   _CX = fir_scan_dot;
   _DX = fir_scan_line;
   __int__(0x10);
#else
	_asm
	{
	mov ax,4f07h
	mov bh,0
	mov bl,0
	mov cx,fir_scan_dot
    mov dx,fir_scan_line
	int 10h
	}
#endif
}


/*指定剪切输出域(包括边线)*/
char screen(int left, int top, int right, int bottom)
{
   if(left>g_screen_h||top>g_screen_v||right<0||bottom<0||right<=left||bottom<=top)
      {return -1;}
   #ifndef NEO_rect_unused
   if (left < 0)
      {left = 0;}
   if (top < 0)
      {top = 0;}
   if (right >= g_screen_h)
      {right =  g_screen_h-1;}
   if (bottom >=g_screen_v)
      {bottom = g_screen_v-1;}

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
   union REGS r;
   if (g_cur_vbe_page != page)
   {
      r.x.ax=0x4f05;
      r.h.bh = 0; /* Set memory window */
      r.h.bl = 0; /* Window A */
      r.x.dx = g_cur_vbe_page = page;
      int86(0x10, &r, &r);
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
         rectfill(0, 0, g_rect_right, g_rect_bottom, _BLACK);
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


   set_display_start(0, temp);
   g_look_surface = (sur_num <= g_alloc_surface?sur_num : 0);

   return g_look_surface;
}


void flip(void)
{
   static unsigned last = 0;                 /*跳过系统保留页*/

   set_display_start(0, last = (last == 0?g_screen_v * g_reserved_page : 0));
   g_look_surface = last != 0?1 : 0;
}
#endif


#ifndef NEO_draw_mode_unused
void set_draw_mode(int draw_mode)
{
   if ((draw_mode>=COPY_PUT)&&(draw_mode<=NOT_PUT)) g_draw_mode=draw_mode;
}
#endif


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
   unsigned char rgb[256][3] = {
0,0,0,6,6,6,10,10,10,14,14,14,18,18,18,22,22,22,26,26,26,30,30,30,34,34,34,38,38,38,42,42,42,
46,46,46,50,50,50,54,54,54,59,59,59,63,63,63,20,0,0,23,0,0,28,0,0,33,1,1,38,2,2,47,4,3,54,6,
5,63,0,0,63,0,0,63,14,11,63,18,15,63,22,19,63,22,18,63,32,28,63,37,33,63,43,39,18,5,2,21,6,3,
24,6,3,27,6,3,31,10,4,36,15,7,40,20,9,44,25,11,48,30,15,53,36,18,56,41,21,60,46,24,63,50,28,
63,55,33,63,60,39,63,62,44,12,6,0,22,15,0,32,25,0,41,34,1,48,43,1,57,50,1,59,56,1,62,62,1,62,
62,21,63,63,0,63,63,10,63,63,19,63,63,28,63,63,41,63,63,49,63,63,60,5,3,2,7,5,3,10,7,5,13,10,
7,16,13,9,18,16,11,21,19,14,24,22,16,27,25,19,29,28,21,32,31,24,35,34,28,38,37,31,41,40,36,
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
   set_palette(*rgb);
#endif
}
#define set_neo_palette() set_neo_color()


void set_palette_range(char *pal_buf, int from, int to, int vsync_flag)
{
#ifndef MSVC15
   struct REGPACK reg;
#else
   int _cx,_es,_dx;
#endif
   if (from <= to && from >= 0 && from < 256 && to - from < 256)
   {
      if (vsync_flag)
         vsync();
#ifndef MSVC15
      reg.r_ax = 0x1012;
      reg.r_bx = from;
      reg.r_cx = to - from + 1;
      reg.r_es = FP_SEG(pal_buf);
      reg.r_dx = FP_OFF(pal_buf);
      intr(0x10,&reg);
#else
	  _asm
	   {
	   mov ax,1012h
	   mov bx,from
	   }
	   _cx=to-from+1;
	   _es= FP_SEG(pal_buf);
       _dx = FP_OFF(pal_buf);
	   _asm
	    {
		int 10h
		}
#endif
   }
}


void _set_palette_range(PALETTE pal_buf, int from, int to, int vsync_flag)
{
   set_palette_range((char *)pal_buf, from, to, vsync_flag);
}


void get_palette_range(char *pal_buf, int from, int to)
{
#ifndef MSVC15
   struct REGPACK reg;
#else
   int _cx,_es,_dx;
#endif
   if (from <= to && from >= 0 && from < 256 && to - from < 256)
   {
#ifndef MSVC15
      reg.r_ax = 0x1017;
      reg.r_bx = from;
      reg.r_cx = to - from + 1;
      reg.r_es = FP_SEG(pal_buf);
      reg.r_dx = FP_OFF(pal_buf);
      intr(0x10,&reg);
#else
	  _asm
	   {
	   mov ax,1017h
	   mov bx,from
	   }
	   _cx=to-from+1;
	   _es= FP_SEG(pal_buf);
       _dx = FP_OFF(pal_buf);
	   _asm
	    {
		int 10h
		}
#endif
   }
}

void _get_palette_range(PALETTE pal_buf, int from, int to)
{
   get_palette_range((char *)pal_buf, from, to);
}


/*---------------------------------------------------*
 *函数功能: 设置硬件调色板中指定颜色号的颜色分量.    *
 *参数说明: index为颜色号.                           *
 *          red,green,blue为此颜色号对应的红,绿,兰三 *
 *          原色分量,均应小于64)                     *
 *返回说明: 成功返回1;否则返回0.                     *
 *备    注: 颜色设置函数.用户程序可直接调用.  　     *
 *---------------------------------------------------*/
char set_color(int index, char red,char green, char blue)
{
   unsigned char color[3];

   color[0] = red;
   color[1] = green;
   color[2] = blue;
   set_palette_range(color, index, index, 0);
   return -1;
}


#ifndef NEO_palette_unused
/*---------------------------------------------------*
 *函数功能: 设置硬件调色板为逻辑调色板.              *
 *参数说明: pal_buf指向存放256种颜色分量数组的指针.  *
 *这个数组的形式一般可以有两种：pal[256][3]和pal[768]*
 *返回说明: 无.                                      *
 *备    注: 颜色设置函数.用户程序可直接调用.  　     *
 *---------------------------------------------------*/
void set_palette(char *pal_buf)
{
   #ifndef NEO_draw_smooth_unused
   set_palette_range(pal_buf, 0, 255, 1);
   #else
   set_palette_range(pal_buf, 0, 255, 0);
   #endif
}

void _set_palette(PALETTE pal_buf)
{
   #ifndef NEO_draw_smooth_unused
   _set_palette_range(pal_buf, 0, 255, 1);
   #else
   _set_palette_range(pal_buf, 0, 255, 0);
   #endif
}


void get_palette(char *pal_buf)/*获取系统调色板*/
{
   get_palette_range(pal_buf, 0, 255);
}

void _get_palette(PALETTE pal_buf)
{
   _get_palette_range(pal_buf, 0, 255);
}


unsigned char get_red(int index)
{
   unsigned char color[3];

   get_palette_range(color, index, index);
   return color[0];
}

unsigned char get_green(int index)
{
   unsigned char color[3];

   get_palette_range(color, index, index);
   return color[1];
}

unsigned char get_blue(int index)
{
   unsigned char color[3];

   get_palette_range(color, index, index);
   return color[2];
}


void set_dac_size(unsigned char size)
{
#ifndef MSVC15
   _AX = 0x4f08;
   _BL = 0;
   _BH = size;
   __int__(0x10);
#else
	char sz=(char)size;
	_asm
	{
	mov ax,4f08h
    mov bl,0h
    mov bh,sz
    int 10h
	}
#endif
   g_dac_size_fix = ((size == 6) || (size == 8)) ? 8 - size : 2;
}

char get_dac_size(void)
{
#ifndef MSVC15
   _AX = 0x4f08;
   _BL = 1;
   __int__(0x10);
   return 8 - (g_dac_size_fix = 8 - _BH);
#else
   char _bh;
   _asm
	{
	mov ax,4f08h
	mov bl,1
    int 10h
    mov _bh,bh
	}
   return 8 - (g_dac_size_fix = 8 - _bh);
#endif
}


#ifndef NEO_color_depth_8_unused
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


      speed = speed <= 0?1 : speed;
      for (c=0; c < 64; c += speed)
      {
         fade_interpolate(source, dest, temp, c, from, to);
         _set_palette_range(temp, from, to, TRUE);
         if (fade_callback) (*fade_callback)();
      }

   _set_palette_range(dest, from, to, TRUE);
}


void fade_in_range(PALETTE p, int speed, int from, int to)
{
   fade_from_range(g_black_palette, p, speed, from, to);
}

void fade_out_range(int speed, int from, int to)
{
   PALETTE temp;

   _get_palette(temp);
   fade_from_range(temp, g_black_palette, speed, from, to);
}

void _fade_in(PALETTE p, int speed)
{
   fade_in_range(p, speed, 0, _PAL_SIZE-1);
}

void _fade_out(int speed)
{
   fade_out_range(speed, 0, _PAL_SIZE-1);
}


void fade_in(PALETTE p, unsigned char dest_color, int speed)
{
   unsigned int i;
   RGB24  black_rbg = {0,0,0};
   RGB24  temp_rbg;
   temp_rbg.r = get_red(dest_color);
   temp_rbg.g = get_green(dest_color);
   temp_rbg.b = get_blue(dest_color);

   for (i=0; i<256; i++)
      g_black_palette[i] = temp_rbg;
   _fade_in(p, speed);
   for (i=0; i<256; i++)
      g_black_palette[i] = black_rbg;
}

void fade_out(unsigned char dest_color, int speed)
{
   unsigned int i;
   RGB24  black_rbg = {0,0,0};
   RGB24  temp_rbg;
   temp_rbg.r = get_red(dest_color);
   temp_rbg.g = get_green(dest_color);
   temp_rbg.b = get_blue(dest_color);

   for (i=0; i<256; i++)
      g_black_palette[i] = temp_rbg;
   _fade_out(speed);
   for (i=0; i<256; i++)
      g_black_palette[i] = black_rbg;
}
#endif
#endif

#endif



void vsync(void)
{
   while  ( inportb(0x3da)&0x08 ) ;
   while (!(inportb(0x3da)&0x08)) ;
}


#endif
