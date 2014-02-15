/*插件类别：移植插件 
插件版本：V0.32
最后更新：2006/03/03
创建日期：2005/09/25
插件描述：将ALLEGRO进行了一次轻量级封装，让ALLEGRO可以直接编译运行NEO程序
推荐在DEV-CPP + ALLEGRO下使用*/

#ifndef  NEO_H
#define  NEO_H

#ifdef __cplusplus
#include <mem.h>
#include <string.h>
#include <dir.h>
#endif

#include <conio.h>
#include <dos.h>
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <allegro.h>


typedef struct                 /* can be either */
{
   int height;
   int width_ex;
   int height_ex;
   FILE *font;
}Font;
Font *g_font;

typedef struct Errinfo_T
{  /*错误位置*/
   char *errloc; /*出错时程序流程所在函数的函数名.*/
   /*错误备注*/
   char *errtag;
   char  handle; /*为1时程序继续执行，否则程序退出*/
}Errinfo_t;


char set_vbe_mode(int mode);
char screen_rect(int left, int top, int right, int bottom);
void rect_store(void);
void rect_restore(void);
int  surface_alloc(unsigned sur_num, int clear_flag);
int  surface_realloc(unsigned sur_num, int flag);
BITMAP *work_surface_handle(void);
int  set_work_surface(unsigned sur_num);
int  flip_surface(unsigned sur_num);
void flip(void);
void set_neo_color(void);
BITMAP *get_str_bitmap(char *string, int fc, int bc);
void (*text_callback)() = NULL;


/* ===================================== NEO INIT ===================================== */
#ifndef  NEO_error_unused
#ifndef  NEO_chinese_err_info_unused
#define  ERR_LOG_FILE       "错误日志.err"
/*用户可以在这里加上自己定义的错误信息（中文）*/
/*You can add your own error information at here(in chinese)*/
#define  N_NEO_NOT_INIT     "NEO未初始化"
#define  NO_MEMORY          "内存不足"
#define  CREAT_FILE_FALSE   "创建文件失败"
#define  NOT_INIT_ADVMOUSE  "高级鼠标未初始化"
#define  ERR_VIDEO_MODE     "不合适的显示模式"
#define  ERR_FILE_TYPE      "错误文件类型"
#define  NO_FILE            "找不到文件"
#define  NOT_INIT_MOUSE     "鼠标未安装"
#define  SOURCE_FILE_BAD    "源文件错误"
#define  DEST_FILE_BAD      "目标文件错误"
#define  WRONG_FORMAT       "错误文件格式"
#define  WRONG_SB_SETTING   "声卡配置错误"
#define  N_NOT_VESA_CARD    "非VESA标准显卡"
#define  UNDEFINED_ERROR    "未定义错误"

#else
#define  ERR_LOG_FILE       "neoerror.err"

/*用户可以在这里加上自己定义的错误信息（英文）*/
/*You can add your own error information at here(in english)*/
#define  N_NEO_NOT_INIT     "NEO not init"
#define  NO_MEMORY          "No mem"
#define  CREAT_FILE_FALSE   "Can't Creat file"
#define  NOT_INIT_ADVMOUSE  "Adv-mouse has not ready"
#define  ERR_VIDEO_MODE     "Not suitable video mode"
#define  ERR_FILE_TYPE      "Err file type"
#define  NO_FILE            "Can't find file"
#define  NOT_INIT_MOUSE     "Mouse was not installed"
#define  SOURCE_FILE_BAD    "source file bad"
#define  DEST_FILE_BAD      "dest file bad"
#define  WRONG_FORMAT       "Wrong file format"
#define  WRONG_SB_SETTING   "Wrong SB settings"
#define  N_NOT_VESA_CARD    "NOT VESA Standard VideoCard"
#define  UNDEFINED_ERROR    "Undefined error"
#endif
#endif

#define  NEO_VERSION       2
#define  NEO_BETA          0
#define  NEO_SUB_VERSION   52
#define  NEO_VERSION_STR   "2.0.52 for DEV"
#define  NEO_DATE_STR      "2006/01/04"
                           /*yyyymmdd*/
#define  NEO_DATE          20060104L

char neo_id[] = NEO_VERSION_STR;
char neo_date[] = NEO_DATE_STR;

#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#endif
#ifndef MID
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif

#ifndef ABS
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#endif
#ifndef SGN
#define SGN(x)       (((x) >= 0) ? 1 : -1)
#endif 
#define neo_exit()  allegro_exit()
#define get_err_method()    err_method(-1)
#define max_x()     SCREEN_W
#define max_y()     SCREEN_H
#define clear_vram_on()
#define clear_vram_off()
#define get_current_page()  0
#define get_total_vram()    64
#define get_total_surfaces()    g_total_surfaces
#define get_vbe_mode()  0
#define get_page_num()  0
#define set_display_start(fir_scan_dot, fir_scan_line)
#define set_draw_mode(draw_mode)
#define n_screen(left, top, right, bottom) screen_rect(left, top, right, bottom)
#define set_neo_palette() set_neo_color()
#define _set_palette_range(pal_buf, from, to, vsync_flag)   set_palette_range(pal_buf, from, to, vsync_flag)
#define _get_palette_range(pal_buf, from, to)   get_palette_range(pal_buf, from, to)
#define _set_palette(pal_buf)   set_palette(pal_buf)
#define _get_palette(pal_buf)   get_palette(pal_buf)
#define set_dac_size(size)
#define get_dac_size()  6
#define _fade_in(p, speed)  fade_in(p, speed)
#define _fade_out(speed)    fade_out(speed)
#define set_text_blank(blank)  g_blank  = blank
#define text_mode(mode)        g_text_bg = mode
#define set_str_color(color)   g_str_color = color
#define get_str_color()        g_str_color
#define cn_s16_out(str, x, y)   set_cn_size(S16X16);put_chinese(str, x, y, g_str_color, 1)
#define cn_s24_out(str, x, y)   set_cn_size(S24X24);put_chinese(str, x, y, g_str_color, 1)
#define get_cn_size()   g_ilimit
#define en_out(c, cx, cy)   asc_out(c, cx, cy, g_str_color)
#define install_timer(new_1ch_int)  install_timer()
#define _install_timer()    install_timer()
#define change_timer(hz)
#define LOOP_END_OF_TIMER()
#define hide_mouse()    show_mouse(NULL);
#define get_mouse_x()   mouse_x
#define get_mouse_y()   mouse_y
#define get_mouse_z()   mouse_z
#define get_click()  mouse_b
#define set_mouse_focus(x, y)   set_mouse_sprite_focus(x, y)
#define mouse_act() get_click() == 0?0 : 1
#define _getch()    readkey()
#define kbhit() keypressed()
#define get_asc() readkey()&&0XFF
#define get_scan()  readkey()>>8
#define _get_asc() readkey()&&0XFF
#define _get_scan()  readkey()>>8
#define play_sample_ex play_sample

/* ============================Graphics modes of NEO SDK ============================ */
#define  VBE320X200X256   0X13
#define  VBE320X240X256   0X14
#define  VBE640X400X256   0X100
#define  VBE640X480X256   0X101
#define  VBE800X600X256   0X103
#define  VBE1024X768X256  0X105
#define  VBE1280X1024X256 0X107

#define  VBE320X200X32K   0X10D
#define  VBE320X240X32K   0X15
#define  VBE640X480X32K   0X110
#define  VBE800X600X32K   0X113
#define  VBE1024X768X32K  0X116
#define  VBE1280X1024X32K 0X119

#define  VBE320X200X64K   0X10E
#define  VBE320X240X64K   0X16
#define  VBE640X480X64K   0X111
#define  VBE800X600X64K   0X114
#define  VBE1024X768X64K  0X117
#define  VBE1280X1024X64K 0X11A

#define  VBE320X200X16M   0X10F
#define  VBE320X240X16M   0X17
#define  VBE640X480X16M   0X112
#define  VBE800X600X16M   0X115
#define  VBE1024X768X16M  0X118
#define  VBE1280X1024X16M 0X11B

#define  g_screen_h SCREEN_W      
#define  g_screen_v SCREEN_H

#define  _UP_ROLL      0
#define  _DOWN_ROLL    1
#define  _LEFT_ROLL    2
#define  _RIGHT_ROLL   3

#define  _WHITE   0xf
#define  _BLACK   0

#ifndef N_MAX_SURFACE_NUM
#define N_MAX_SURFACE_NUM   64
#endif

#define N_FULLSCREEN  TRUE
#define N_WINDOWED    FALSE
BITMAP *g_surface_ptr[N_MAX_SURFACE_NUM]; 

#define  S12X12   12
#define  S16X16   16
#define  S24X24   24
#define  S32X32   32
#define  S40X40   40
#define  S48X48   48

#define  N_TEXT_L_SLANT 1
#define  N_TEXT_R_SLANT -1

#ifndef _FONT_PATH
#define _FONT_PATH ""
#endif

#ifndef _FONT_12
#define _FONT_12 "hzk12"
#endif

#ifndef _FONT_16
#define _FONT_16 "hzk16"
#endif

#ifndef _FONT_24
#define _FONT_24 "hzk24"
#endif

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

#if defined NEO_error_unused
#define NEO_sys_report_error_unused
#endif     
/*===============NEO的私有全局变量(请勿擅自俢改,以免造成不可预料的错误!)================*/
char g_green_mask  = 0;
char g_green_bit   = 0;
char g_color_depth = 0;    /*记录当前图形模式的色深，为0表示非图形模式*/
char g_color_byte  = 0;    /*记录当前图形模式每个点占用的字节数*/
int  g_clear_vram  = 0;    /*清除显存标志，0表示每次显示模式的改变都同步清除显存原有内容*/
int  g_cur_vbe_page= 0;    /*当前所处的显示页*/
long g_screen_size;        /*记录当前图形模式的屏幕面积*/
char g_work_surface = 0;   /*当前工作页*/
char g_look_surface = 0;   /*当前可视页*/
char g_vbe_version =  3;   /*VBE的主版本号*/
char g_vbe_sub_ver =  0;   /*VBE的副版本号*/
char g_total_surfaces =N_MAX_SURFACE_NUM;  /*记录能创建的后台页的总量*/
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

char g_slant = 0;
int  g_str_color = 0xff15;
int  g_text_bg = -1;
int  g_ilimit = 16;
int  g_jlimit = 2;
int  g_blank = 1;
int  g_hz_size = 32;       /*一个字占用的字节数*/
int  g_hz_width= 16;
char g_start = 0;          /*汉字在字库中的起始区号*/
char *g_hz_font = _FONT_16; /*字库名*/

int  g_mode = TRUE; /*全屏标志，为FALSE时使用窗口模式，否则使用全屏模式*/

void abort_on_error(const char *message){
   if (screen != NULL)
   set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

   allegro_message("%s.\nLast Allegro error `%s'\n",
                    message, allegro_error);
   exit(-1);
}

int  neo_init(){
   /*char *tem = (char *)malloc(strlen(_FONT_PATH) + strlen(_FONT_16) + 2); 
  
   g_font = (Font *)malloc(sizeof(Font));
   if (g_font && tem)
   {
      strcpy(tem, _FONT_PATH);
      g_hz_font = strcat(tem, _FONT_16);
      g_font->height = 8;
      g_font->width_ex  = 16;
      g_font->height_ex = 16;
      g_font->font = fopen(g_hz_font, "rb+");      
   } */      
   return allegro_init();
}

char err_method(int method){
   static int oldmeth1 = 3;
   if (method >= 0)
   {
      oldmeth1 = method;
   }

   return oldmeth1;
}

void neo_warn(void){
    printf("\a");
}

void throw_error(Errinfo_t err){
   FILE *errlog_p;
   static char fir_run;
   char method = err_method(-1);

   if (method & 1)   /*声音报警*/
   {
      neo_warn();
   }
   if (method & 2) /*写入错误日志*/
   {
      errlog_p = fopen(ERR_LOG_FILE, "a");

      if (!fir_run)
      {
         fprintf(errlog_p, "      {ERROR LOG}\n");
         fprintf(errlog_p, "{NEO SDK V2.1.45 To Allegro}\n");
         fprintf(errlog_p, ">>RUN START\n");
         fir_run = 10;
      }
      fprintf(errlog_p, ">>[Error   in] : %s().\n", err.errloc);
      fprintf(errlog_p, ">>[Error info] : %s.\n\n", err.errtag);
      fclose(errlog_p);
   }
   if (method & 4) /*在屏幕上显示*/
   {
      fprintf(stderr, "[Error   in] : %s().\n", err.errloc);
      fprintf(stderr, "[Error info] : %s.\n\n", err.errtag);
   }
   if (err.handle != 1)
   {
      exit(err.handle);
   }
}

void set_window_mode(int mode){
    g_mode = mode;
}

int get_window_mode(void){
    return g_mode? GFX_AUTODETECT : GFX_AUTODETECT_WINDOWED;
}

char set_video_mode(int w, int h, int bpp, int refresh, int flags){
    set_color_depth(bpp);
    request_refresh_rate(refresh);
    if (set_gfx_mode(get_window_mode(), w, h, 0, 0) != 0)
    {
       abort_on_error("Couldn't set graphic mode!");
       return FALSE;
    }
    g_rect_left  = g_rect_top   = 0;
    g_rect_right = g_screen_h - 1;
    g_rect_bottom= g_screen_v - 1;
    g_surface_ptr[0] = screen;   
    return TRUE;
}

char set_vbe_mode(int mode){
    int  screen_wid;
    int  screen_hig;
    char color_dpt;
    switch (mode)
    {
        case VBE320X200X256:   
            color_dpt = 8;
            screen_wid = 320;
            screen_hig = 200;
            break;
        case VBE320X240X256:        
            color_dpt = 8;
            screen_wid = 320;
            screen_hig = 240;
            break;            
        case VBE640X400X256:        
            color_dpt = 8;
            screen_wid = 640;
            screen_hig = 400;
            break;
        case VBE640X480X256:        
            color_dpt = 8;
            screen_wid = 640;
            screen_hig = 480;
            break;
        case VBE800X600X256:        
            color_dpt = 8;
            screen_wid = 800;
            screen_hig = 600;
            break;
        case VBE1024X768X256:        
            color_dpt = 8;
            screen_wid = 1024;
            screen_hig = 768;
            break;
        case VBE1280X1024X256:        
            color_dpt = 8;
            screen_wid = 1280;
            screen_hig = 1024;
            break;

        case VBE320X200X32K:        
            color_dpt = 15;
            screen_wid = 320;
            screen_hig = 200;
            break;
        case VBE320X240X32K:        
            color_dpt = 15;
            screen_wid = 320;
            screen_hig = 240;
            break;
        case VBE640X480X32K:        
            color_dpt = 15;
            screen_wid = 640;
            screen_hig = 480;
            break;
        case VBE800X600X32K:        
            color_dpt = 15;
            screen_wid = 800;
            screen_hig = 600;
            break;
        case VBE1024X768X32K:        
            color_dpt = 15;
            screen_wid = 1024;
            screen_hig = 768;
            break;
        case VBE1280X1024X32K:        
            color_dpt = 15;
            screen_wid = 1280;
            screen_hig = 1024;
            break;

        case VBE320X200X64K:        
            color_dpt = 16;
            screen_wid = 320;
            screen_hig = 200;
            break;
        case VBE320X240X64K:        
            color_dpt = 16;
            screen_wid = 320;
            screen_hig = 240;
            break;            
        case VBE640X480X64K:        
            color_dpt = 16;
            screen_wid = 640;
            screen_hig = 480;
            break;
        case VBE800X600X64K:        
            color_dpt = 16;
            screen_wid = 800;
            screen_hig = 600;
            break;
        case VBE1024X768X64K:        
            color_dpt = 16;
            screen_wid = 1024;
            screen_hig = 768;
            break;            
        case VBE1280X1024X64K:        
            color_dpt = 16;
            screen_wid = 1280;
            screen_hig = 1024;
            break;                  

        case VBE320X200X16M:        
            color_dpt = 24;
            screen_wid = 320;
            screen_hig = 200;
            break;
        case VBE320X240X16M:        
            color_dpt = 24;
            screen_wid = 320;
            screen_hig = 240;
            break;
        case VBE640X480X16M:        
            color_dpt = 24;
            screen_wid = 640;
            screen_hig = 480;
            break;
        case VBE800X600X16M:        
            color_dpt = 24;
            screen_wid = 800;
            screen_hig = 600;
            break;
        case VBE1024X768X16M:        
            color_dpt = 24;
            screen_wid = 1024;
            screen_hig = 768;
            break;
        case VBE1280X1024X16M:        
            color_dpt = 24;
            screen_wid = 1280;
            screen_hig = 1024;
            break;
        default:            
            return FALSE;     
    }
    set_color_depth(color_dpt);
    set_gfx_mode(get_window_mode(), screen_wid, screen_hig, 0, 0);    
    g_rect_left  = g_rect_top   = 0;
    g_rect_right = g_screen_h - 1;
    g_rect_bottom= g_screen_v - 1;
    g_surface_ptr[0] = screen;/*create_bitmap(SCREEN_W, SCREEN_H);
    clear_bitmap(g_surface_ptr[0]); */ 
    return TRUE;
}

char screen_rect(int left, int top, int right, int bottom){
   if(left>g_screen_h||top>g_screen_v||right<0||bottom<0||right<=left||bottom<=top)
      {return -1;}
   g_rect_left  = left;
   g_rect_right = right;
   g_rect_top   = top;
   g_rect_bottom= bottom;   
   set_clip_rect(g_surface_ptr[g_work_surface], left,  top, right, bottom);
   return 0;
}

void rect_store(void){
   g_temp_top = g_rect_top;
   g_temp_left= g_rect_left;
   g_temp_right = g_rect_right;
   g_temp_bottom= g_rect_bottom;   
}

void rect_restore(void){
   g_rect_top = g_temp_top;
   g_rect_left= g_temp_left;
   g_rect_right = g_temp_right;
   g_rect_bottom= g_temp_bottom;
   set_clip_rect(g_surface_ptr[g_work_surface], g_rect_left, g_rect_top, g_rect_right, g_rect_bottom);
}

int surface_alloc(unsigned sur_num, int clear_flag){
    unsigned num;
    int i;
    g_alloc_surface = sur_num >= N_MAX_SURFACE_NUM? N_MAX_SURFACE_NUM : sur_num;

    for (i = 1; i <= g_alloc_surface; ++i)
    {
        g_surface_ptr[i] = create_bitmap(SCREEN_W, SCREEN_H);
        if (clear_flag) clear_bitmap(g_surface_ptr[i]);
    }
    
    /*if (mouse_driver)
    {
       scare_mouse();
    }
    blit(screen, g_surface_ptr[0], 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    if (mouse_driver)
    {
       unscare_mouse();
    }*/
         
    return g_alloc_surface;
}

int surface_realloc(unsigned sur_num, int flag){
    unsigned num;
    int i;
    for (i = 1; i <= g_alloc_surface; ++i)
        destroy_bitmap(g_surface_ptr[i]);    
        
    return surface_alloc(sur_num, flag);
}

BITMAP *work_surface_handle(void){
    return g_surface_ptr[g_work_surface];
}    

int set_work_surface(unsigned sur_num){
    if (g_alloc_surface)
        return g_work_surface = (sur_num <= g_alloc_surface && sur_num > 0)? sur_num : 0;
    else
        return 0;
}       

int flip_surface(unsigned sur_num){                                                                                           /*跳过系统保留页*/
   if (mouse_driver)
   {
      scare_mouse();   /*隐藏Patch原来的光标*/
   }
   g_look_surface = (sur_num <= g_alloc_surface?sur_num : 0);
   rect_store();
   set_clip_rect(screen, 0,  0, SCREEN_W, SCREEN_H);
   blit(g_surface_ptr[g_look_surface], screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
   rect_restore();
   if (mouse_driver)
   {
      unscare_mouse();   /*绘制redraw新的光标*/
   }
   return g_look_surface;
}

void flip(void){
   static unsigned last = 0;                 /*跳过系统保留页*/

   g_look_surface = last != 0?1 : 0;
   flip_surface(last);
}

void set_neo_color(void){
   PALETTE rgb = {
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
   set_palette(rgb);
}

void set_color(int index, char red,char green, char blue){
    RGB p;
    p.r = red;
    p.g = green;
    p.b = blue;
    set_color(index, &p);
}

unsigned char get_red(int index){
    PALETTE p;
    get_palette_range(p, index, index);
    return p[index].r;
}

unsigned char get_green(int index){
    PALETTE p;
    get_palette_range(p, index, index);
    return p[index].g;
}

unsigned char get_blue(int index){
    PALETTE p;
    get_palette_range(p, index, index);
    return p[index].b;
}

void fade_in(PALETTE p, unsigned char dest_color, int speed){
   unsigned int i;
   RGB black_rbg = {0,0,0};
   RGB temp_rbg;
   temp_rbg.r = get_red(dest_color);
   temp_rbg.g = get_green(dest_color);
   temp_rbg.b = get_blue(dest_color);

   for (i=0; i<256; i++)
      black_palette[i] = temp_rbg;
   fade_in(p, speed);
   for (i=0; i<256; i++)
      black_palette[i] = black_rbg;
}

void fade_out(unsigned char dest_color, int speed){
   unsigned int i;
   RGB black_rbg = {0,0,0};
   RGB  temp_rbg;
   temp_rbg.r = get_red(dest_color);
   temp_rbg.g = get_green(dest_color);
   temp_rbg.b = get_blue(dest_color);

   for (i=0; i<256; i++)
      black_palette[i] = temp_rbg;
   fade_out(speed);
   for (i=0; i<256; i++)
      black_palette[i] = black_rbg;
}

char get_pal_from_bmp(char *filename, PALETTE output){
   BITMAP  *bmp;
   bmp = load_bitmap(filename, output);
   if (bmp != NULL && bitmap_color_depth(bmp) == 8)
   {
      destroy_bitmap(bmp);
      return 0;
   }
   destroy_bitmap(bmp);
   return -1;
}

void set_pal_with_bmp(char *filename){
   PALETTE pal;
   destroy_bitmap(load_bitmap(filename, pal));
   set_palette(pal);
}

/*====================================Drawing primitives=================================*/
void dot(int x, int y, int color){
    putpixel(g_surface_ptr[g_work_surface], x, y, color);
}

void _dot(int x,int y,int color){
    _putpixel(g_surface_ptr[g_work_surface], x, y, color);
}    

int  get_dot(int x, int y){
    return getpixel(g_surface_ptr[g_work_surface], x, y);
}

void line(int x1, int y1, int x2, int y2, unsigned color){
    line(g_surface_ptr[g_work_surface], x1, y1, x2, y2, color);
}

char hline(int x, int y, int width, unsigned color){
    hline(g_surface_ptr[g_work_surface], x, y, width, color);
}

void vline(int x, int y, int height,unsigned color){
    vline(g_surface_ptr[g_work_surface], x, y, height, color);
}

void  rect(int x0, int y0, int x1, int y1, int color){
    rect(g_surface_ptr[g_work_surface], x0, y0, x1, y1, color);
}

void  tri(int xa, int ya, int xb, int yb, int xc, int yc, int color){
   line(xa, ya, xb, yb,color);
   line(xb, yb, xc, yc,color);
   line(xc, yc, xa, ya,color);
}

void rectfill(int x1, int y1, int x2, int y2, int color){
    rectfill(g_surface_ptr[g_work_surface], x1, y1, x2, y2, color);
}

void ellipsefill(int center_x, int center_y, int stalk_long,int stalk_short, int fill_color){
    ellipsefill(g_surface_ptr[g_work_surface], center_x, center_y, stalk_long, stalk_short, fill_color);
}

void circlefill(int c_x, int c_y, int r, unsigned color){
    circlefill(g_surface_ptr[g_work_surface], c_x, c_y, r, color);
}

void circle(int c_x, int c_y, int r, unsigned color){
    circle(g_surface_ptr[g_work_surface], c_x, c_y, r, color);
}

void ellipse(int x0,int y0,int a,int b,unsigned color){
    ellipse(g_surface_ptr[g_work_surface], x0, y0, a, b, color);
}

void clear_to_color(int color){
    clear_to_color(g_surface_ptr[g_work_surface], color);
}

void clear(void){
   clear_to_color(0);
}

/*==================================Blitting and sprites=================================*/
#define CUR_SIDE   1024
enum MASK_COLOR{
   TPT8  = 0X0,
   TPT15 = 0X7C1F,
   TPT16 = 0XF81F,
   /*TPT24 = 0XFF00FFL,
   TPT32 = 0XFF00FFL,*/
};
typedef struct PIC_RGB{
   unsigned char blue;
   unsigned char green;
   unsigned char red;
   unsigned char reserved;
}PIC_RGB, *PIC_RGB_ptr;

typedef struct Cursor_type{
   PIC_RGB  palette[16];
   unsigned color64k[16];
   unsigned char fir_cur_col;
   unsigned char map[512];
   unsigned char mask[128];
   unsigned char buffer[CUR_SIDE];
}Cursor, *Cursor_ptr;

int fir_icon_color_pos(int fircol_index){
   static unsigned char colorindex = /*239*/240;
   /*if (!g_color_depth)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"fir_icon_color_pos", ERR_VIDEO_MODE, 0};
      throw_error(error);
      #endif
   }*/
   if ((fircol_index>=0) && (fircol_index<=240))
   {
      colorindex = fircol_index;
      return colorindex;
   }
   else if (fircol_index < 0)
   {
      return colorindex;
   }
   else
   {
      return -1;
   }
}

Cursor_ptr load_icon(char *filename){
   int k = 0;
   int x = 0;
   int y = 0;
   int i, j, offset;
   Cursor_ptr cursor_p = NULL;
   FILE *fp;

   if ((cursor_p = (Cursor_ptr)malloc( sizeof (Cursor) ))==NULL)
   {
      /*#ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"load_icon", NO_MEMORY, 0};
      throw_error(error);
      #endif*/exit(0);
   }

   if ((fp = fopen(filename, "rb")) == NULL)
   {
      /*#ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"load_icon", NO_FILE, 1};
      throw_error(error);
      #endif*/
      return NULL;
   }

   cursor_p->fir_cur_col = fir_icon_color_pos(-1);
   fseek(fp, 62, SEEK_SET);
   fread(&cursor_p->palette, sizeof(struct PIC_RGB), 16, fp);
   fread(&cursor_p->map, 512, 1, fp);
   fread(&cursor_p->mask, 128, 1, fp);
   fclose(fp);

   memset(cursor_p->color64k, 0, 32);
   for(offset=0; offset<16; ++offset)
   {
      set_color(cursor_p->fir_cur_col + offset, (cursor_p->palette[offset]).red>>g_dac_size_fix,
                                        (cursor_p->palette[offset]).green>>g_dac_size_fix,
                                         (cursor_p->palette[offset]).blue>>g_dac_size_fix );
      cursor_p->color64k[offset] += ((cursor_p->palette[offset]).red>>3);
      cursor_p->color64k[offset] <<= g_green_mask;
      cursor_p->color64k[offset] += ((cursor_p->palette[offset]).green>>g_green_bit);
      cursor_p->color64k[offset] <<= 5;
      cursor_p->color64k[offset] += (cursor_p->palette[offset]).blue>>3;
   }

   if (g_color_depth == 8)
   {
      for(i=31; i>=0; --i)
      {
         for(j=0; j<32; j+=2)
         {
            cursor_p->buffer[( i<<5 )+j] = (cursor_p->map[k]>>4) + cursor_p->fir_cur_col;
            cursor_p->buffer[( i<<5 )+j+1] = (cursor_p->map[k++]&0x0f) + cursor_p->fir_cur_col;
         }
      }
   }
   else if (g_color_depth == 15 || g_color_depth == 16)
   {
      for(i=31; i>=0; --i)
      {
         for(j=0; j<32; j+=2)
         {
            cursor_p->buffer[( i<<5 )+j] = (cursor_p->map[k]>>4);
            cursor_p->buffer[( i<<5 )+j+1] = (cursor_p->map[k++]&0x0f);
         }
      }
   }

   for(i=124; i>=0; i-=4)
   {
      for(j=i; j<=i+3; ++j)
      {
         unsigned char m = 0x80;

         for(k = 0; k < 8; ++k)
         {
            if(cursor_p->mask[j] & m)
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
   fir_icon_color_pos(cursor_p->fir_cur_col);

   return cursor_p;
}
 
void blit_icon(Cursor_ptr cursor_p, int x, int y){
   int i, j;
   unsigned int color;

   if (g_color_depth == 8)
   {
      for(i=0; i<32; ++i)
      {
         for(j=0; j<32; j+=2)
         {
            if ((color=cursor_p->buffer[(i<<5)+j])  != (TPT8 + 16)) dot(x + j, y + i, color);
            if ((color=cursor_p->buffer[(i<<5)+j+1])!=(TPT8 + 16)) dot(x + j + 1,y + i, color);
         }
      }
   }
   else if (g_color_depth == 15 || g_color_depth == 16)
   {
      for(i=0; i<32; ++i)
      {
         for(j=0; j<32; j+=1)
         {
            if ((cursor_p->buffer[(i<<5)+j]) != (TPT8 + 16))
            {
               dot(x + j, y + i, cursor_p->color64k[cursor_p->buffer[(i<<5)+j]]);
            }
         }
      }
   }
}

void show_icon(char *curfile, int x, int y){
   Cursor_ptr cur_p;

   cur_p = load_icon(curfile);
   blit_icon(cur_p,x,y);
   free(cur_p);
}

char bitmap_color_depth(char *bmp){
    PALLETE pal;
    char cd;
    BITMAP *b = load_bitmap(bmp, pal);
    cd = bitmap_color_depth(b);
    destroy_bitmap(b);
    return cd;
}        

char show_bmp(char *bmpfile, int x, int y)
{
    PALLETE pal;
    BITMAP *bmp = load_bitmap(bmpfile, pal);
    if (bmp == NULL) return FALSE;
    
    if (bitmap_color_depth(bmp) == 8) set_palette(pal);
    blit(bmp, g_surface_ptr[g_work_surface], 0, 0, x, y, bmp->w, bmp->h);
    destroy_bitmap(bmp);
    return TRUE;
}

char _show_bmp(char *bmpfile, int x, int y)
{
    PALLETE pal;
    BITMAP *bmp = load_bitmap(bmpfile, pal);
    if (bmp == NULL) return FALSE;
    
    if (bitmap_color_depth(bmp) == 8) set_palette(pal);
    masked_blit(bmp, g_surface_ptr[g_work_surface], 0, 0, x, y, bmp->w, bmp->h);
    destroy_bitmap(bmp);
    return TRUE;
}

char _show_bmp_v_flip(char *bmpfile, int x, int y)
{
    PALLETE pal;
    BITMAP *bmp = load_bitmap(bmpfile, pal);
    if (bmp == NULL) return FALSE;
    
    if (bitmap_color_depth(bmp) == 8) set_palette(pal);
    draw_sprite_v_flip(g_surface_ptr[g_work_surface], bmp, x, y);
    destroy_bitmap(bmp);
    return TRUE;
}

char bmp_blit(char *bmpname, int source_x, int source_y, int dest_x, int dest_y, int width, int height){
    PALLETE pal;
    BITMAP *bmp = load_bitmap(bmpname, pal);
    if (bmp == NULL) return FALSE;
    
    if (bitmap_color_depth(bmp) == 8) set_palette(pal);
    blit(bmp, g_surface_ptr[g_work_surface], source_x, source_y, dest_x, dest_x, width, height);
    destroy_bitmap(bmp);
    return TRUE;
} 

char bmp_masked_blit(char *bmpname, int source_x, int source_y, int dest_x, int dest_y, int width, int height){
    PALLETE pal;
    BITMAP *bmp = load_bitmap(bmpname, pal);
    if (bmp == NULL) return FALSE;
    
    if (bitmap_color_depth(bmp) == 8) set_palette(pal);
    masked_blit(bmp, g_surface_ptr[g_work_surface], source_x, source_y, dest_x, dest_x, width, height);
    destroy_bitmap(bmp);
    return TRUE;
}

BITMAP *get_image(unsigned x0, unsigned y0, unsigned x1, unsigned y1){
    int w = x1 - x0;
    int h = y1 - y0;
    BITMAP *image = create_bitmap(w, h);
    if (image == NULL || w <= 0 || h <= 0) return NULL;

    blit(g_surface_ptr[g_work_surface], image, x0, y0, 0, 0, w, h);
    return image;
}

void put_image(BITMAP *image, int x, int y){
    blit(image, g_surface_ptr[g_work_surface], 0, 0, x, y, image->w, image->h);
}    

BITMAP *_load_bmp(char *filename){
    RGB *pal;
    BITMAP *bmp = load_bmp(filename, pal);
    if (bmp == NULL) return NULL;
    return bmp;
}

char blit(BITMAP *bmp){
    blit(bmp, g_surface_ptr[g_work_surface], 0, 0, 0, 0, bmp->w, bmp->h);
    return TRUE;
}

void free_bmp(BITMAP *bitmap){
    destroy_bitmap(bitmap);
}

char rect_save_screen(char *bmpfile, unsigned left, unsigned top, unsigned right, unsigned bottom){
    BITMAP *bmp;
    PALETTE pal;

    get_palette(pal);
    bmp = create_sub_bitmap(screen, left, top, right - left, bottom - top);
    if (bmp == NULL) return FALSE;
    save_bitmap(bmpfile, bmp, pal);
    destroy_bitmap(bmp);
    return TRUE;
}

char save_screen(char *bmpfile) {
    return rect_save_screen(bmpfile, 0, 0, SCREEN_W, SCREEN_H);
}

void show_mouse(void){
    show_mouse(screen);
}

void mouse_refresh(void){
    //update_mouse();
    //draw_mouse(TRUE, FALSE);
}

void set_dbl_click_delay(int userdelay){}

int  set_mouse_icon(char *p_iconfile){
    BITMAP *sprite;
    PALETTE p;
    if (p_iconfile)
    {
        sprite = load_bitmap(p_iconfile, p);
        set_mouse_sprite(sprite);
        //destroy_bitmap(sprite);    
        return TRUE;
    }
    else
    set_mouse_sprite(NULL);    
    return FALSE;
}

int  get_click_info(void){
    return mouse_b;
}       

void en_slant(char slant){
   if (slant != 0)   
      g_slant = slant > 0?1 : -1;
   else
      g_slant = 0;
}

void asc_out(char c, int cx, int cy, int color){
    char str[2];
    str[0] = c;
    str[1] = '\0';
    textout_ex(g_surface_ptr[g_work_surface], font, str, cx, cy, color, g_text_bg);
}

void textout(const char *str, int sx, int sy){
   int index;
   for (index = 0; str[index] != 0; index++)
   {
      asc_out(str[index], sx + (index << 3) + g_blank, sy, g_str_color);
   }
}

char put_chinese(char *string, int x, int y, int color, int blank){
   FILE *hzk_p;
   long fpos;       /*汉字在字库文件中的偏移*/
   int  i, j, k, l;
   int asc_size = 0;/*用来或得当前英文字符的高*/
   char hz_buf[72]; /*汉字字模缓冲区，目前能支持24X24点阵字符的存放*/

   if ((hzk_p=fopen(g_hz_font,"rb+"))==NULL)/* 打开24点阵字库文件 */
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"put_chinese", NO_FILE, 1};
      throw_error(error);
      #endif
      return -1;
   }
   asc_size = abs(g_ilimit - text_height(font));
   while (*string != '\0')
   {
      if((*string & 0x80) == 0)/* 判断是否是扩展ASCII */
      {
         asc_out(*string, x, y + asc_size, color);
         x += 8 + blank;
         string++;
      }
      else
      {
         char slant = 0;
         fpos = (((unsigned char)*string-0xa1 - g_start)*94 + ((unsigned char)*(string+1)-0xa1))*(long)g_hz_size;
         fseek(hzk_p, fpos, SEEK_SET);
         fread(hz_buf, g_hz_size, 1, hzk_p);

         if (g_hz_size <= 32) /*16点阵以下的字库*/
         {
            for (i = 0; i < g_ilimit; ++i)
            {
               slant += g_slant;
               for (j = 0; j < g_jlimit; ++j)
               {
                  for (k = 0; k < 8; ++k)
                  {
                     if (((hz_buf[(i << 1) + j] >> 7 - k)) & 1)
                        dot(x + (j << 3) + k + slant, y + i, color);
                     else if (g_text_bg >= 0)
                        dot(x + (j << 3) + k + slant, y + i, g_text_bg);
                  }
               }
            }
         }
         else /*24点阵以上的字库*/
         {
            for (i = 0; i < g_ilimit; ++i)
            {
               for (j = 0; j < g_jlimit; ++j)
               {
                  for (k = 1; k <= 8; ++k)
                  {
                     if ( (hz_buf[i * 3 + j] << k - 1) & 0x80 )
                        dot(x + i, y + (j << 3) + k - 1, color);
                     else if (g_text_bg >= 0)
                        dot(x + i, y + (j << 3) + k - 1, g_text_bg);
                  }
               }
            }
         }
         x += (g_ilimit + blank);
         string += 2;
      }
      if (text_callback)
         text_callback();
   }
   fclose(hzk_p);
   return 0;
}

void set_cn_font(char cn_size, char *cn_font){
   g_jlimit = 2;
   g_start  = 0;

   g_ilimit = cn_size;
   switch(cn_size)
   {
      case S12X12:
      g_hz_size= 24;
      g_hz_font= cn_font;
      g_hz_width= 12;
      break;
      case S16X16:
      g_hz_size= 32;
      g_hz_font= cn_font;
      g_hz_width= 16;
      break;
      case S24X24:
      g_jlimit = 3;
      g_hz_size= 72;
      g_start  = 15;
      g_hz_width= 24;
      g_hz_font= cn_font;
      break;
      case S40X40:
		  g_jlimit = 4;
		  g_hz_size= 200;
		  g_start  = 15;
		  g_hz_width= 40;
		  g_hz_font= cn_font;
		  break;
      case S48X48:
		  g_jlimit = 5;
		  g_hz_size= 288;
		  g_start  = 15;
		  g_hz_width= 48;
		  g_hz_font= cn_font;
      break;
      default:
      g_ilimit = S16X16;
      g_hz_size= 32;
      g_hz_width= 16;
      g_hz_font= cn_font;
      break;
   }
}

void set_cn_size(char cn_size){
   switch(cn_size)
   {
      case S12X12:
      set_cn_font(S12X12, "HZK12");
      break;
      case S16X16:
      set_cn_font(S16X16, "HZK16");
      break;
      case S24X24:
      set_cn_font(S24X24, "HZK24S");
      break;
      case S40X40:
		  set_cn_font(S40X40, "HZK40S");
		  break;
      case S48X48
		  set_cn_font(S48X48, "HZK48S");
		  break;
      default:
      set_cn_font(S16X16, "HZK16");
      break;
   }
}

int neo_printf(int x, int y, char *format, ...){
   char buffer[256];
   va_list argptr;
   int count;
   char err_m;

   va_start(argptr, format);
   count = vsprintf(buffer, format, argptr);
   va_end(argptr);

   #ifndef NEO_error_unused
   err_m = get_err_method();
   err_method(0);
   #endif
   if (put_chinese(buffer, x, y, g_str_color, g_blank))
      textout(buffer, x, y);
   #ifndef NEO_error_unused
   err_method(err_m);
   #endif
   return (count);
}

BITMAP *get_str_bitmap(char *string, int fc, int bc){
   BITMAP *str_bitmap;
   FILE *hzk_p;
   long fpos;       /*汉字在字库文件中的偏移*/
   int  i, j, k, l, x = 0, y = 0;
   int asc_size = 0;/*用来或得当前英文字符的高*/
   int count;
   char hz_buf[72]; /*汉字字模缓冲区，目前能支持24X24点阵字符的存放*/

   if (count = strlen(string))
   {
      count % 2 ? count++ : 0;
      str_bitmap = create_bitmap((count >> 1) * (g_hz_width) + count, g_hz_width);
      if (str_bitmap)
         clear(str_bitmap);
      else
         return NULL;
   }   
      
   if ((hzk_p = fopen(g_hz_font, "rb+"))==NULL)/* 打开24点阵字库文件 */
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"put_chinese", NO_FILE, 1};
      throw_error(error);
      #endif
      return NULL;
   }
   asc_size = abs(g_ilimit - text_height(font));
   while (*string != '\0')
   {
      if((*string & 0x80) == 0)/* 判断是否是扩展ASCII */
      {
         char str[2];
         str[0] = *string;
         str[1] = '\0';
         textout_ex(str_bitmap, font, str, x, 0, fc, bc);         
         x += 8 + 1/*+ blank*/;
         string++;
      }
      else
      {
         char slant = 0;
         fpos = (((unsigned char)*string-0xa1 - g_start)*94 + ((unsigned char)*(string+1)-0xa1))*(long)g_hz_size;
         fseek(hzk_p, fpos, SEEK_SET);
         fread(hz_buf, g_hz_size, 1, hzk_p);

         if (g_hz_size <= 32) /*16点阵以下的字库*/
         {
            for (i = 0; i < g_ilimit; ++i)
            {
               slant += g_slant;
               for (j = 0; j < g_jlimit; ++j)
               {
                  for (k = 0; k < 8; ++k)
                  {
                     if (((hz_buf[(i << 1) + j] >> 7 - k)) & 1)
                        putpixel(str_bitmap, x + (j << 3) + k + slant, y + i, fc);
                     else/* if (g_text_bg >= 0)*/
                        putpixel(str_bitmap, x + (j << 3) + k + slant, y + i, bc);
                  }
               }
            }
         }
         else /*24点阵以上的字库*/
         {
            for (i = 0; i < g_ilimit; ++i)
            {
               for (j = 0; j < g_jlimit; ++j)
               {
                  for (k = 1; k <= 8; ++k)
                  {
                     if ( (hz_buf[i * 3 + j] << k - 1) & 0x80 )
                        putpixel(str_bitmap, x + i, y + (j << 3) + k - 1, fc);
                     else/* if (g_text_bg >= 0)*/
                        putpixel(str_bitmap, x + i, y + (j << 3) + k - 1, bc);
                  }
               }
            }
         }
         x += (g_ilimit + 1/*+ blank*/);
         string += 2;
      }
   }

   return str_bitmap;
} 

int string_out(char *str, int x, int y)
{
   return put_chinese(str, x, y, g_str_color, g_blank);
}

int random (int high){  /* get a random number between low and high, inclusively */
   if (high < 0) return (0); /* don't divide by zero */
   return (rand() % (high + 1));
}

void neo_message(char *message, ...)
{
   allegro_message(message);
}

#endif
