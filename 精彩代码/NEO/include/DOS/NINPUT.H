/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : ninput.h
  摘    要 : 本头文件中包含NEO SDK里有关键盘、鼠标控制的各种函数、结构、全局变量
             的声明及定义
  当前版本 : V5.13
  作    者 : 董凯
  完成日期 : 2006.12.18

  取代版本 : V5.129
  原 作 者 : 董凯
  完成日期 : 2006.12.05
*/

#ifndef NINPUT_H
#define NINPUT_H

/*======================================================*
 *　　　 宏和结构的定义(Macro & type definitions)  　　 *
 *======================================================*/
#ifdef  NEO_mouse_bas_unused
#define NEO_mouse_exp_unused
#endif

    /*双击延时量(单位:秒*18):FAST_DBLCLICK为最短延时;SLOW_DBLCLICK为最长延时;MIDDLE_DBLCLICK为中
      速延时*/
#define FAST_DBLCLICK    0x1L
#define MID_DBLCLICK     0x3L
#define SLOW_DBLCLICK    0x5L

#define MOUSE_L_DOWN     0x1
#define MOUSE_R_DOWN     0x2
#define MOUSE_LR_DOWN    0x3
#define MOUSE_M_DOWN     0xff
#define MOUSE_LDBL_CLICK 0x5
#define MOUSE_RDBL_CLICK 0x6
#define MOUSE_L_UP       0x8
#define MOUSE_R_UP       0x10

#ifndef KEY_BUF_SIZE
#define KEY_BUF_SIZE     0x10
#endif

#define SCAN_ALT 56
#define SCAN_CTRL 29
#define SCAN_caps 58
#define SCAN_LEFTSHIFT 42
#define SCAN_RIGHTSHIFT 54
#define SCAN_SHIFT (keyflag[SCAN_RIGHTSHIFT]||keyflag[SCAN_LEFTSHIFT])
/*     键盘 ASCII 码     */
#define KEY_ESC 27
#define KEY_END 255
#define KEY_LEFT 254
#define KEY_RIGHT 253
#define KEY_PAGEUP 252
#define KEY_UP 251
#define KEY_DOWN 250
#define KEY_HOME 249
#define KEY_CTRLBREAK 248
#define KEY_F1 247
#define KEY_F2 246
#define KEY_F3 245
#define KEY_F4 244
#define KEY_F5 243
#define KEY_F6 242
#define KEY_F7 241
#define KEY_F8 240
#define KEY_F9 239
#define KEY_F10 238
#define KEY_PAGEDOWN 237
#define KEY_INSERT 236
#define KEY_DELETE 235
#define KEY_LEFTALT 234
#define KEY_RIGHTALT 233
#define KEY_RIGHTCTRL 232
#define KEY_LEFTCTRL 231
#define KEY_caps 230
#define KEY_F11 229
#define KEY_F12 228
#define KEY_PRINTSCREEN 228
#define KEY_NUMLOCK 227
#define KEY_SCROLLLOCK 226
#define KEY_LEFTSHIFT 225
#define KEY_RIGHTSHIFT 224
#define KEY_WINDOWS 223

#ifndef NEO_keyboard_unused
unsigned char g_pause_key;
unsigned char g_ascii_key;
unsigned char g_scan_key;
unsigned char keyflag[128];
unsigned char g_keypress;
/*unsigned char g_wheel_flag = FALSE;*/
unsigned g_key;
unsigned g_key_filter = 0xffff;
unsigned g_combine_keys_delay = 400;
unsigned char g_caps;
unsigned char g_cur_code,g_key_code;
#endif

void interrupt far (*old_key_int)(__CPPARGS);

#ifndef NEO_keyboard_unused
unsigned char asciis[] =
{

   0,27,'1','2','3','4','5','6','7','8','9','0','-','=',8,9,
   'q','w','e','r','t','y','u','i','o','p','[',']',13,KEY_LEFTCTRL,'a','s',
   'd','f','g','h','j','k','l',';',39,'`',0,92,'z','x','c','v',
   'b','n','m',',','.','/',0,'*',KEY_LEFTALT,' ',KEY_caps,KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,
   KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NUMLOCK,KEY_SCROLLLOCK,'7','8','9','-','4','5','6','+','1',
   '2','3','0',127,0,0,'\\',KEY_F11,KEY_F12,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#endif


#ifndef NEO_keyboard_unused
unsigned char shifts[] =
{
   0,27,'!','@','#','$','%','^','&','*','(',')','_','+',8,9,
   'Q','W','E','R','T','Y','U','I','O','P','{','}',13,1,'A','S',
   'D','F','G','H','J','K','L',':',34,'~',KEY_LEFTSHIFT,'|','Z','X','C','V',
   'B','N','M','<','>','?',KEY_RIGHTSHIFT,'*',1,' ',0,0,0,0,0,0,
   0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
   '2','3','0',127,0,0,'|',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#endif


#ifndef NEO_keyboard_unused
char specials[] =
{
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,13,KEY_RIGHTCTRL,0,0,
   0,0,0,0,0,0,0,0,0,0,KEY_WINDOWS,0,0,0,0,0,
   0,0,0,0,0,'/',0,KEY_PRINTSCREEN,KEY_RIGHTALT,0,0,0,0,0,0,0,
   0,0,0,0,0,0,KEY_CTRLBREAK,KEY_HOME,KEY_UP,KEY_PAGEUP,0,KEY_LEFT,0,KEY_RIGHT,0,KEY_END,
   KEY_DOWN,KEY_PAGEDOWN,KEY_INSERT,KEY_DELETE,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
#endif


#ifndef NEO_mouse_exp_unused
typedef struct
{
   unsigned int cursor[32];

   char hotx;
   char hoty;
}Hardcur_T;

Hardcur_T g_arrow_t=
{
   {
       0x3fff,0x1fff,0x0fff,0x07ff,
       0x03ff,0x01ff,0x00ff,0x007f,
       0x003f,0x00ff,0x01ff,0x10ff,
       0x30ff,0xf87f,0xf87f,0xfc3f,
       0x0000,0x7c00,0x6000,0x7000,
       0x7800,0x7c00,0x7e00,0x7f00,
       0x7f80,0x7e00,0x7c00,0x4600,
       0x0600,0x0300,0x0300,0x0180
   },
   0,0,
};
#endif

#ifndef NEO_keyboard_unused
   unsigned g_key_buf[KEY_BUF_SIZE];
   unsigned char g_head_count;
   unsigned char g_tail_count;
#endif

#ifndef NEO_mouse_adv_unused
typedef struct
{
   int x;         /*当前鼠标光标 x坐标*/
   int y;         /*当前鼠标光标 y坐标*/
   /*int z;*/     /*当前鼠标滚轮偏移量*/
   #ifndef NEO_mouse_exp_unused
   char logic_x;  /*鼠标光标贴图的逻辑坐标(相对于以上的两个物理坐标,可以为负值)*/
   char logic_y;
   #endif
   long dblclickdelay;   /*双击延时变量.当第二次按下鼠标键与第一次的时间间隔小于该变量时,被
                          认为是双击鼠标键*/

   char advclickinfo;                  /*当前鼠标的按键状态,
                                         clickinfo=0 : 没动作;
                                         clickinfo=1 : 左键被按下;
                                         clickinfo=2 : 右键被按下;
                                         clickinfo=3 : 两键同时按下;
                                         clickinfo=5 : 左键被双击; */
   int oldclickinfo;
   clock_t start;
   clock_t end;                  /*这几个变量是为实现记录左键双击而设的“私有”变量*/

   #ifdef NEO_color_depth_16_unused
   char overlay_buf[CUR_SIDE];/*覆盖缓冲区:保存被光标覆盖的屏幕,以在光标移开后恢复*/
   #else
   int overlay_buf[CUR_SIDE];
   #endif

   Cursor iconbuffer;          /*光标缓冲区:保存被光标(文件)的位图信息,用以显示光标*/
   Cursor_ptr iconbuffer_p;    /*指向上面光标缓冲区的指针，用作调用相关函数的参数*/
   char mouse_b;
}Mouse_t;
Mouse_t *g_mouse_tp;
#endif

/*======================================================*
 *              函数声明(Function declare)              *
 *======================================================*/

#define keyboard_callback(key_filter) (g_key_filter = (key_filter))
#define _get_asc() (g_keypress == 1?g_ascii_key : 0)
#define _get_scan() (g_keypress == 1?g_scan_key : 0)
void clear_keybuf(void);
void interrupt new_key_int(__CPPARGS);
unsigned readkey(void);
void simulate_keypress(unsigned  key);
void install_keyboard(void);
void remove_keyboard(void);
unsigned char _getch(void);
unsigned char get_asc(void);
unsigned char get_scan(void);
         /*NEO的键盘例程提供的函数不能与TC标准库的键盘处理函数混用（如getch();）否则将无法正常结束程序*/
char keypressed(void);
char scan_asc_key(unsigned char scankey, unsigned char asckey);

#ifndef NEO_mouse_bas_unused
/*判断鼠标是否发生按键动作.*/
#define mouse_act() get_click() == 0?0 : 1

int init_mouse(void);     /*初始化鼠标器.*/
int get_mouse_x(void);     /*获取当前鼠标光标X坐标.*/
int get_mouse_y(void);     /*获取当前鼠标光标Y坐标.*/
char get_click(void);  /*获取当前鼠标按钮状态.*/
                     /*获取按钮按下状态.*/
unsigned get_press_info(int key, int *x, int *y, int *count);
                     /*获取按钮放开状态.*/
unsigned get_release_info(int key, int *x, int *y, int *count);
void locate_mouse(int x,int y);
                     /*显示鼠标光标.*/
void show_hard_cur(void);
                     /*隐藏鼠标光标.*/
void hide_hard_cur(void);
                     /*设置光标的有效活动范围.*/
void set_mouse_range(int xmin,int xmax,int ymin,int ymax);
#endif

#ifndef NEO_mouse_exp_unused
void mouse_shape(Hardcur_T cur);
void set_mouse_speed(int vx,int vy);
                     /*设置光标速度加速下限.*/
void speed_limit(int ms);
                       /*设置鼠标光标焦点*/
void set_mouse_focus(char f_x, char f_y);
#endif

#ifndef NEO_mouse_adv_unused
char install_mouse(void);   /*鼠标器高级初始化.*/
                     /*设置光标坐标.*/
void position_mouse(int x,int y);
void show_mouse(void);     /*鼠标图形光标显示.*/
void hide_mouse(void);     /*鼠标图形光标隐藏.*/
void cursor_refresh(void);   /*鼠标图形光标的更新显示.*/
void scare_mouse_area(int x, int y, int w, int h);  /*设置光标的隐藏区域*/
void scare_mouse(void);    /*暂时隐藏光标*/
void unscare_mouse(void);  /*取消光标隐藏*/
int  mouse_refresh(void);  /*鼠标图形光标的刷新.*/
                          /*鼠标双击判定延时设置.*/
void set_dbl_click_delay(int userdelay);
                          /*改变鼠标光标的形状.*/
int  set_mouse_icon(char *p_iconfile);
int  get_click_info(void);/*获得当前的鼠标高级按键状态.*/
void remove_mouse(void);     /*鼠标关闭函数.*/

#endif

/*======================================================*
 *            函数定义(Function definitions)            *
 *======================================================*/
#ifndef NEO_keyboard_unused
void clear_keybuf(void)
{
   if (!_keyboard_installed)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"clear_keybuf", N_KEYB_NOT_INIT, 0};
      throw_error(error);
      #endif
   }
   else
   {
      g_key = 0;
      memset(keyflag, 0, sizeof(keyflag)); /*键盘映射缓冲归零*/
      memset(g_key_buf, 0, KEY_BUF_SIZE);  /*键盘队列缓冲归零*/
      #ifndef NEO_key_buffer_unused
      g_tail_count = g_head_count;
      #endif
   }
}


void interrupt new_key_int(__CPPARGS)
{
   static unsigned char specialflag;
   static char rec_specials;
   unsigned char k, c, temp;

   k = inp(0x60);
   temp = inp(0x61) | 0x80;
   outportb(0x61,temp);

   if (k == 0xe0) specialflag = 1;
   else if (k == 0xe1) g_pause_key = 1;
   else
   {
      if (k & 0x80)
      {
         k &= 0x7f;
         keyflag[k] = 0;
         g_keypress = -1;
      }
      else
      {
         g_key_code = g_cur_code;
         g_cur_code = g_scan_key = k;
         keyflag[k] = 1;
         if (specialflag) /*防止特殊键被重复判断*/
         {
            c = specials[k];
            if (rec_specials == -33)
            {
               if (c) g_ascii_key = c;
               specialflag = rec_specials = 0;
               outportb(0x20,0x20);
               return;
            }
            rec_specials = -33;
         }
         else
         {
            if (k == SCAN_caps)
            {
               g_caps = (~g_caps) & 1;
            }
            if (SCAN_SHIFT)
            {
               c = shifts[k];
               if ((c >= 'A')&&(c <= 'Z') && g_caps)
               c += 'a'- 'A';
            }
            else
            {
               c=asciis[k];
               if (( c >= 'a') && (c <= 'z') && g_caps)
               c -= 'a' - 'A';
            }
         }
         if (c) g_ascii_key = c;
         g_keypress = 1;
      }
      specialflag=0;
   }
   outportb(0x20,0x20);

   #ifndef NEO_key_buffer_unused
   /*往键盘缓冲区放入相应的键值*/
   if (g_keypress == 1 && (g_key = (g_scan_key << 8) + g_ascii_key) != 10975)
   {
      if (g_key_filter == 0xffff)
      {
         g_key_buf[g_head_count] = g_key & g_key_filter;
      }
      else if (g_key_filter == 0)
      { /*按键不被加入缓冲队列*/ }
      else
      {
         g_key_buf[g_head_count] = g_key_filter;
      }
      if ( !((++g_head_count) < KEY_BUF_SIZE) )
      {
         g_head_count = 0;
      }

      if (g_head_count == g_tail_count)
      {
         if( !((++g_tail_count) < KEY_BUF_SIZE) )
         {
            g_tail_count = 0;
         }
      }
   }
   #else
   g_key = (g_scan_key << 8) + g_ascii_key;
   #endif
}


unsigned readkey(void)
{
   #ifndef NEO_key_buffer_unused
   unsigned key;

   while (g_head_count == g_tail_count); /*缓冲区为空时等待*/

   key = g_key_buf[g_tail_count];
   if ( !((++g_tail_count) < KEY_BUF_SIZE) )
   {
      g_tail_count = 0;
   }
   return key;

   #else
   unsigned key = g_key;
   if (g_keypress == 1)
   {
      g_key = 0;
   }
   return key;
   #endif
}


void simulate_keypress(unsigned  key)
{
   #ifndef NEO_key_buffer_unused
   if ((g_key = key) != 10975)
   {
      g_key_buf[g_head_count] = g_key;
      if ( !((++g_head_count) < KEY_BUF_SIZE) )
      {
         g_head_count = 0;
      }

      if (g_head_count == g_tail_count)
      {
         if( !((++g_tail_count) < KEY_BUF_SIZE) )
         {
            g_tail_count = 0;
         }
      }
   }
   #else
   g_key = key;
   #endif
}


void install_keyboard(void)
{
   g_scan_key = g_ascii_key = g_key = 0;
   memset(keyflag,0,sizeof(keyflag));
   g_head_count = g_tail_count = 0;

   old_key_int=getvect(9);
   setvect(9,new_key_int);
   g_routines |= 8;
}


void remove_keyboard(void)
{
   setvect(9,old_key_int);
   g_routines &= 247;
}


unsigned char _getch(void)
{
   if (_keyboard_installed)
   {
      while (!keypressed());
      return get_asc();
   }
   else
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"_getch", N_KEYB_NOT_INIT, 1};
      throw_error(error);
      #endif
      return getch();
   }
}


unsigned char get_asc(void)
{
   #ifndef NEO_key_buffer_unused
   unsigned key;
   if (g_head_count != g_tail_count) /*缓冲区非空时*/
   {
      key = g_key_buf[g_tail_count];
      if ( !((++g_tail_count) < KEY_BUF_SIZE) )
      {
         g_tail_count = 0;
      }
      return key & 0xff;
   }
   return 0;
   #else
   unsigned curkey = 0;

   if (g_keypress == 1)
   {
      curkey = g_ascii_key;
      g_ascii_key = 0;
   }
   return curkey;
   #endif
}


unsigned char get_scan(void)
{
   #ifndef NEO_key_buffer_unused
   unsigned key;
   if (g_head_count != g_tail_count) /*缓冲区非空时*/
   {
      key = g_key_buf[g_tail_count];
      if ( !((++g_tail_count) < KEY_BUF_SIZE) )
      {
         g_tail_count = 0;
      }
      return (key & 0xff00) >> 8;
   }
   return 0;
   #else
   unsigned curkey = 0;
   if (g_keypress == 1)
   {
      curkey = g_scan_key;
      g_scan_key = 0;
   }
   return curkey;
   #endif
}


char keypressed(void)
{
   if (_keyboard_installed)
   {
      #ifndef NEO_key_buffer_unused
      return g_head_count != g_tail_count? TRUE : FALSE;
      #else
      if (g_keypress == 1)
      {
         g_keypress = 0;
         return TRUE;
      }
      return FALSE;
      #endif
   }
   else return kbhit();
}


char scan_asc_key(unsigned char scankey, unsigned char asckey)
{
   int i;
   if (_get_scan() == scankey)
   {
      for (i = 0; i < g_combine_keys_delay; ++i);
      return (_get_asc() == asckey?1 : 0);
   }
   else return 0;
}
#endif


#ifndef NEO_mouse_bas_unused
/*函数功能: 初始化鼠标器.                            *
 *参数说明: 无.                                      *
 *返回说明: 成功则返回鼠标器按键个数;否则返回0.      *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     */
int init_mouse(void)
{
   _AX=0x00;
   __int__(0x33);
   if (_AX)
   {
      return (_BX);
   }
   else
   {
      return (0);
   }
}


/*---------------------------------------------------*
 *函数功能: 获取当前鼠标光标X坐标.                   *
 *参数说明: 无.                                      *
 *返回说明: 返回光标X坐标.                           *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
int get_mouse_x(void)
{
   _AX=0x03;
   __int__(0x33);
   return _CX;
}


/*---------------------------------------------------*
 *函数功能: 获取当前鼠标光标Y坐标.                   *
 *参数说明: 无.                                      *
 *返回说明: 返回光标Y坐标.                           *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
int get_mouse_y(void)
{
   _AX=0x03;
   __int__(0x33);
   return _DX;
}


/*---------------------------------------------------*
 *函数功能: 获取当前鼠标按钮状态.                    *
 *参数说明: 无.                                      *
 *返回说明: 返回按钮状态标志                         *
 *          返回0: 表示左右按键都没按下;             *
 *          返回1: 表示左键按下;                     *
            返回2: 表示右键按下;                     *
            返回3: 表示左右键均被按下.               *
 *备    注: 底层鼠标控制函数.用户通过高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
char get_click(void)
{
   _AX = 0x03;
   __int__(0x33);
   return _BX;
}


/*---------------------------------------------------*
 *函数功能: 获取按钮按下状态.                        *
 *参数说明: 变量key指定检查哪个键:等于0(或LEFT_KEY)时*
 *          表示检查左键;等于1(或RIGHT_KEY)时检查右键*
 *          整型指针型变量*x,*y用来记录最后一次按下指*
 *          定键时的光标坐标;*count将记录调用该功能以*
 *          来指定按键压下的总次数,超过65535溢出即置0*
 *返回说明: 返回当前鼠标按钮状态(同get_click功能) *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
unsigned get_press_info(int key, int *p_x, int *p_y, int *p_count)
{
   _BX = key;
   _AX = 0x05;
   __int__(0x33);
   *p_x = _CX;       /*指定键最后一次按下时的x坐标*/
   *p_y = _DX;       /*指定键最后一次按下时的y坐标*/
   *p_count = _BX; /*返回调用该功能以来指定按键压下的总次数(超过65535溢出,即置0)*/
   return _AX;
}


/*---------------------------------------------------*
 *函数功能: 获取按钮放开状态.                        *
 *参数说明: 变量key指定检查哪个键:等于0(或LEFT_KEY)时*
 *          表示检查左键;等于1(或RIGHT_KEY)时检查右键*
 *          整型指针型变量*x,*y用来记录最后一次按下指*
 *          定键时的光标坐标;*count将记录调用该功能以*
 *          来指定按键放开的总次数,超过65535溢出即置0*
 *返回说明: 返回当前鼠标按钮状态(同get_click功能)    *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
unsigned get_release_info(int key, int *p_x, int *p_y, int *p_count)
{
   _BX = key;
   _AX = 0x06;
   __int__(0x33);
   *p_x = _CX;       /*指定键最后一次松开时的x坐标*/
   *p_y = _DX;       /*指定键最后一次松开时的y坐标*/
   *p_count = _BX; /*返回调用该功能以来指定按键松开的总次数(超过65535溢出,即置0)*/
   return _AX;
}


void locate_mouse(int x,int y)
{
   _CX=x;
   _DX=y;
   _AX=0x04;
   __int__(0x33);
}


/*---------------------------------------------------*
 *函数功能: 显示鼠标光标.                            *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 底层鼠标控制函数.不推荐用户使用.原因是它 *
 *          在Win2000/WinXP下可能无法显示光标.       * 
 *---------------------------------------------------*/
void show_hard_cur(void)
{
   _AX = 0x01;
   __int__(0x33);
}


/*---------------------------------------------------*
 *函数功能: 隐藏鼠标光标.当鼠标光标改变了位置,要先调 *
 *          用它来隐藏原来的光标,再调用show_hard_cur()来显 *
            示新的光标,否则会破坏屏幕的显示内容.     *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 底层鼠标控制函数.由于与高版本的Windows兼 *
 *          容性不好而不推荐用户使用.                *
 *---------------------------------------------------*/
void hide_hard_cur(void)
{
   _AX = 0x02;
   __int__(0x33);
}


/*---------------------------------------------------*
 *函数功能: 设置光标的有效活动范围.                  *
 *参数说明: xmin:水平方向活动的最小坐标.             *
 *          xmax:水平方向活动的最大坐标.             *
 *          ymin:垂直方向活动的最小坐标.             *
 *          ymax:垂直方向活动的最小坐标.             *
 *返回说明: 无.                                      *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
void set_mouse_range(int xmin, int ymin, int xmax, int ymax)
{
   _AX = 7;
   _CX = xmin;
   _DX = xmax;
   __int__(0x33);

   _AX = 8;
   _CX = ymin;
   _DX = ymax;
   __int__(0x33);
}
#endif


/*char check_wheel(void)
{
   _AX=0x11;
   __int__(0x33);
   return g_wheel_flag = (_AX == 0x574D);
}*/


/*改变鼠标光标形状
  Hardcur_T cur:为以上定义的形状 */
#ifndef NEO_mouse_exp_unused
void mouse_shape(Hardcur_T cur)
{
   _BX = cur.hotx;
   _CX = cur.hoty;
   _ES = FP_SEG(&cur.cursor);
   _DX = FP_OFF(&cur.cursor);
   _AX = 0x09;
   __int__(0x33);
}


/*---------------------------------------------------*
 *函数功能: 设置鼠标的移动速度.                      *
 *参数说明: vx,vy的缺省值:vx=8,vy=8.值越大速度越慢   *
 *返回说明: 无.                                      *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
void set_mouse_speed(int vx,int vy)
{
   _CX = vx;
   _DX = vy;
   _AX = 0x0f;
   __int__(0x33);
}


/*---------------------------------------------------*
 *函数功能: 设置光标速度加速下限.当光标移动速度超过设*
            置的下限后,光标速度将加倍.               *
 *参数说明: ms为每秒移动的下限单位(1ms约为1/1200英寸)*
 *          ms默认值为64/S.                          *
 *返回说明: 无.                                      *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
void speed_limit(int ms)
{
   _DX = ms;
   _AX = 0x13;
   __int__(0x33);
}


void set_mouse_focus(char f_x, char f_y)
{
   if (g_mouse_tp)
   {
      g_mouse_tp->logic_x = -f_x;
      g_mouse_tp->logic_y = -f_y;
   }
   else
   {
      /*to be continue...*/
   }
}
#endif


/*---------------------------------------------------*
 *函数功能: 基于NEO_SDK鼠标器高级初始化.             *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 高级鼠标控制函数.用户程序必须在使用其它高*
 *          级鼠标控制函数之前首先调用.              *
 *---------------------------------------------------*/
#ifndef NEO_mouse_adv_unused
char install_mouse(void)
{
   int h, v;
   int cur_x;
   int cur_y;
   char keys = init_mouse();

   if (_mouse_installed) return FALSE;

   /*底层鼠标控制初始化*/
   if (!keys)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"install_mouse", NOT_INIT_MOUSE, 1};

      throw_error(error);
      return FALSE;
      #endif
   }

   if (!g_color_depth)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"install_mouse", ERR_VIDEO_MODE, 0};

      throw_error(error);
      #endif
   }

   if ((g_mouse_tp = (Mouse_t *)malloc( sizeof (Mouse_t) )) == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"install_mouse", NO_MEMORY, 0};

      throw_error(error);
      #endif
   }
   g_mouse_tp->iconbuffer_p = &g_mouse_tp->iconbuffer;

   /*将鼠标的各项属性取默认值*/
   #ifndef NEO_mouse_exp_unused
   g_mouse_tp->advclickinfo = g_mouse_tp->logic_x = g_mouse_tp->logic_y = 0;
   #else
   g_mouse_tp->advclickinfo = 0;
   #endif
   cur_x = g_mouse_tp->x = (int)((g_rect_right-g_rect_left)>>1)-32;
   cur_y = g_mouse_tp->y = (int)((g_rect_bottom-g_rect_top)>>1)-32;
   g_mouse_tp->dblclickdelay = MID_DBLCLICK;/*当两次按下左键间隔小于MID_DBLCLICK即认为是双击*/
   g_mouse_tp->oldclickinfo = -1;
   common_flags &= 0xFFFB;

   set_mouse_range(0, 0, g_rect_right, g_rect_bottom);
   locate_mouse(g_mouse_tp->x, g_mouse_tp->y);

   for (v = 0; v<32; v+=1)
   {
      for (h = 0; h<32; ++h)
      {
         /*保存光标将要覆盖的屏幕*/
         g_mouse_tp->overlay_buf[v * 32 + h] = get_dot(cur_x + h, cur_y + v);
      }
   }
   set_mouse_icon(NULL); /*设置成默认指针光标*/
   for (v = 0; v<32; ++v)
   {
      for (h = 0; h<32; ++h)
      {
         dot(cur_x + h , cur_y + v, g_mouse_tp->overlay_buf[v * 32 + h]);
      }
   }
   common_flags &= 0xFFFD;
   g_routines |= 2;

   #ifndef  NEO_timer_unused
   #ifndef NEO_mouse_bind_timer_unused
   if (!(common_flags & 0x2))
   {
      install_int_ex((void (*)())mouse_refresh, 1); /*将鼠标的刷新工作交给时钟*/
   }
   #endif
   #endif

   return keys;
}


/*---------------------------------------------------*
 *函数功能: 设置光标坐标.                            *
 *参数说明: 整型参数x,y为光标的新位置坐标.           *
 *返回说明: 无.                                      *
 *备    注: 底层鼠标控制函数.用户通过nadmouse.h中的高*
 *          鼠标控制函数间接调用                     *
 *---------------------------------------------------*/
void position_mouse(int x,int y)
{
   if (g_mouse_tp != NULL)
   {
      hide_mouse();
      locate_mouse(x, y);
      show_mouse();
   }
   else
   {
      #ifndef NEO_mouse_bas_unused
      hide_hard_cur();
      locate_mouse(x, y);
      show_hard_cur();
      #endif
   }

}


/*---------------------------------------------------*
 *函数功能: 以鼠标结构中保留的坐标值在屏幕相应的位置 *
 *          绘制光标                                 *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 高级鼠标控制函数.用户通过mouse_refresh() *
            间接调用.                                *
 *---------------------------------------------------*/
void show_mouse(void)
{
   int h, v;
   unsigned offs = g_sl_offset;
   char  work = g_work_surface;
   #ifndef NEO_mouse_exp_unused
   int cur_x = (g_mouse_tp->x = get_mouse_x()) + g_mouse_tp->logic_x;
   int cur_y = (g_mouse_tp->y = get_mouse_y()) + g_mouse_tp->logic_y;
   #else
   int cur_x = g_mouse_tp->x = get_mouse_x();
   int cur_y = g_mouse_tp->y = get_mouse_y();
   #endif
   common_flags |= 0x8;
   #ifndef NEO_surface_unused
   if (g_look_surface != g_work_surface)
      set_work_surface(g_look_surface);
   #endif

   for (v = 0; v < 32; v += 2)
   {
      for (h = 0; h < 32; ++h)
      {
         /*保存光标将要覆盖的屏幕*/
         g_mouse_tp->overlay_buf[(v << 5) + h] = get_dot(cur_x + h, cur_y + v);
         g_mouse_tp->overlay_buf[((v + 1) << 5) + h] = get_dot(cur_x + h, cur_y + v + 1);
      }
   }
   #ifndef NEO_icon_support_unused
   blit_icon(g_mouse_tp->iconbuffer_p, cur_x, cur_y);
   #endif
   g_work_surface =  work;
   g_sl_offset = offs;
   common_flags &= 0xfff7;
}


/*---------------------------------------------------*
 *函数功能: 鼠标图形光标隐藏.　　　　　　　　        *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 高级鼠标控制函数.用户通过cursor_refresh()*
 *          间接调用.                                *
 *---------------------------------------------------*/
void hide_mouse(void)
{
   int h, v;
   unsigned offs = g_sl_offset;
   char  work = g_work_surface;
   #ifndef NEO_mouse_exp_unused
   int x = g_mouse_tp->x + g_mouse_tp->logic_x, y = g_mouse_tp->y + g_mouse_tp->logic_y;
   #else
   int x = g_mouse_tp->x, y = g_mouse_tp->y;
   #endif

   #ifndef NEO_surface_unused
   if (g_look_surface != g_work_surface)
      set_work_surface(g_look_surface);
   #endif

   #ifndef NEO_draw_smooth_unused
   vsync();
   #endif
   for (v = 0; v < 32; ++v)
   {
      for (h = 0; h < 32; ++h)
      {
         dot(x + h , y + v, g_mouse_tp->overlay_buf[(v << 5) + h]);
      }
   }
   g_work_surface =  work;
   g_sl_offset = offs;
}


/*---------------------------------------------------*
 *函数功能: 鼠标图形光标的更新显示.                  *
 *          当鼠标光标可能被覆盖时调用来重绘光标.    *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 高级鼠标控制函数.用户可直接调用.         *
 *---------------------------------------------------*/
/*void cursor_refresh(void)
{
   if (!(common_flags & 0x4) && (!(common_flags & 0x2)))
   {
      common_flags |= 0x8;
      hide_mouse();
      common_flags &= 0xfff7;
      show_mouse();
   }
}*/
/*出于时钟中断执行效率的考虑，将上面函数换成宏*/
#define cursor_refresh() { \
   if (!(common_flags & 0x4) && (!(common_flags & 0x2))) \
   { \
      common_flags |= 0x8; /*防止重入标志*/ \
      hide_mouse();   /*隐藏原来的光标*/ \
      common_flags &= 0xfff7; \
      show_mouse();   /*绘制新的光标.*/ \
   } \
}


void scare_mouse_area(int x, int y, int w, int h)
{
   if ((g_mouse_tp->x > x) && (g_mouse_tp->y > y) && (g_mouse_tp->x < x + w) && (g_mouse_tp->y < y + h))
   {
      scare_mouse();
   }
   else
   {
      common_flags |= 0x2;
   }
}


/*临时去掉鼠标光标*/
void scare_mouse(void)
{
   common_flags |= 0x4;
   hide_mouse();
}


/*恢复临时去掉的鼠标光标*/
void unscare_mouse(void)
{
   if (common_flags & 0x4)
   {
      common_flags &= 0xFFFB;
      show_mouse();
   }
   common_flags &= 0xFFFD;
}


char check_click(void)
{
   int current_x = get_mouse_x();
   int current_y = get_mouse_y();

   if ((current_x == g_mouse_tp->x) && (current_y == g_mouse_tp->y))
   {
      char click = get_click();

      /*检测左键是否双击*/
      if (click == 0 && (g_mouse_tp->advclickinfo != 0))
      {
         g_mouse_tp->start = _clock();
         if (common_flags & 0x10)
         {
            common_flags &= 0xFFEF;
            g_mouse_tp->start = -32768L;
         }

         if (g_mouse_tp->advclickinfo == 1)
         {
            g_mouse_tp->mouse_b = MOUSE_L_UP;/*单击左键*/
         }
         else if (g_mouse_tp->advclickinfo == 2)
         {
            g_mouse_tp->mouse_b = MOUSE_R_UP;/*单击右键*/
         }
         g_mouse_tp->oldclickinfo = g_mouse_tp->advclickinfo;
         g_mouse_tp->advclickinfo = 0;
      }
      #ifndef NEO_dbl_click_unused
      else if (click == 0 && (g_mouse_tp->oldclickinfo == 1) && (g_mouse_tp->advclickinfo == 0))
      {
         g_mouse_tp->mouse_b = g_mouse_tp->advclickinfo = 0;
      }
      else if (click == 1 && (g_mouse_tp->oldclickinfo == 1) && (g_mouse_tp->advclickinfo == 0))
      {
         g_mouse_tp->end = _clock();
         /*默认当dblclickdelay为MIDDLE_DBLCLICK即双击*/
         if (abs(g_mouse_tp->end - g_mouse_tp->start) < g_mouse_tp->dblclickdelay)
         {
            g_mouse_tp->mouse_b = g_mouse_tp->advclickinfo = MOUSE_LDBL_CLICK;
            common_flags |= 0x10;/*开启双击标志*/
         }
         else/*否则只视为按下*/
         {
            g_mouse_tp->mouse_b = g_mouse_tp->advclickinfo = click;
         }
         g_mouse_tp->oldclickinfo = 0;
      }
      #endif
      else
      {
         g_mouse_tp->mouse_b = g_mouse_tp->advclickinfo = click;
      }
      return FALSE;
   }
   else /*鼠标移动了*/
   {
      g_mouse_tp->mouse_b = g_mouse_tp->advclickinfo = get_click();
      return TRUE;
   }
}


/*---------------------------------------------------*
 *函数功能: 鼠标图形光标的刷新.放在用户程序的主循环中*
 *          用以刷新鼠标的各种主要状态.              *
 *参数说明: 无.                                      *
 *返回说明: 若鼠标移动返回0;否则返回1.               *
 *备    注: 高级鼠标控制函数.用户可直接调用.         *
 *---------------------------------------------------*/
int mouse_refresh(void)
{
   if (g_mouse_tp == NULL) /*检查高级鼠标例程是否已安装*/
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"mouse_refresh", NOT_INIT_ADVMOUSE, 1};
      throw_error(error);
      #endif
      return -1;
   }

   if (check_click())
   {
      cursor_refresh();
      return 0;
   }

   return  1;
}


/*---------------------------------------------------*
 *函数功能: 鼠标双击判定延时设置.当两次相邻的单击时间*
 *          间隔小于双击判定延时即被认为是双击.　　　*
 *参数说明: userdelay可以是:                         *
 *          FAST_DBLCLICK,MIDDLE_DBLCLICK            *
 *          或SLOW_DBLCLICK(用户亦可自定一个小于1.5的*
 *          延时)                                    *
 *返回说明: 无.                                      *
 *备    注: 高级鼠标控制函数.用户可直接调用.         *
 *---------------------------------------------------*/
void set_dbl_click_delay(int userdelay)
{
   if (g_mouse_tp==NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"set_dbl_click_delay", NOT_INIT_ADVMOUSE, 0};

      throw_error(error);
      #endif
   }
   (g_mouse_tp->dblclickdelay=userdelay);
}


/*---------------------------------------------------*
 *函数功能: 改变鼠标光标的形状.                      *
 *参数说明: p_iconfile:光标文件的文件名.             *
 *返回说明: 改变成功返回1;失败返回0,并改为默认的箭头 *
 *          形光标.                                  *
 *备    注: 高级鼠标控制函数.用户可直接调用.         *
 *---------------------------------------------------*/
int set_mouse_icon(char *p_iconfile)
{
   unsigned char *deft = NULL;
   Cursor_ptr tempcur;
   const char _BLACK_ = 0;
   const char _WHITE_ = 15;

   if (g_mouse_tp == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"set_mouse_icon", NOT_INIT_ADVMOUSE, 0};
      throw_error(error);
      #endif
   }

   #ifndef NEO_mouse_icon_unused
   if (p_iconfile!=NULL)
   {
      tempcur = load_icon(p_iconfile);
      if (tempcur)
      {
         memcpy(g_mouse_tp->iconbuffer_p, tempcur, sizeof(Cursor));
         free(tempcur);
         cursor_refresh();
         return 1;
      }
      else
      {
         #ifndef NEO_sys_report_error_unused
         Errinfo_t error = {"set_mouse_icon", NO_FILE, 1};
         throw_error(error);
         #endif
      }
   }
   #else
   p_iconfile += 0; /*加上此句只是为在条件编译时不报警*/
   #endif

   #ifndef NEO_mouse_cursor_unused
    /*当指定的光标文件找不到时,执行以下代码*/
    /*以下代码段是在光标缓冲区填入默认的箭头形光标*/
    memset(g_mouse_tp->iconbuffer_p->buffer, TPT8 + 16, 1024);
    /*以下代码服务于8位色深*/
    g_mouse_tp->iconbuffer_p->palette[_BLACK_].red = g_mouse_tp->iconbuffer_p->palette[_BLACK_].green = g_mouse_tp->iconbuffer_p->palette[_BLACK_].blue = 0;
    g_mouse_tp->iconbuffer_p->palette[_WHITE_].red = g_mouse_tp->iconbuffer_p->palette[_WHITE_].green = g_mouse_tp->iconbuffer_p->palette[_WHITE_].blue = 255;
    /*以下代码服务于15/16位色深*/
    g_mouse_tp->iconbuffer_p->color64k[_BLACK_] = 0x0000;
    g_mouse_tp->iconbuffer_p->color64k[_WHITE_] = 0xffff;

    deft = g_mouse_tp->iconbuffer_p->buffer;
    *deft = _BLACK_;
    *(deft += 32) = *(deft + 1) = _BLACK_;
    *(deft += 32) = *(deft + 2) = _BLACK_;
    *(deft + 1) = _WHITE_;
    *(deft += 32) = *(deft + 3) = _BLACK_;
    *(deft + 1) = *(deft + 2) = _WHITE_;
    *(deft += 32) = *(deft + 4) = _BLACK_;
    *(deft + 1) = *(deft + 2) = *(deft + 3) = _WHITE_;
    *(deft += 32) = *(deft + 5) = _BLACK_;
    *(deft + 1) = *(deft + 2) = *(deft + 3) = *(deft + 4) = _WHITE_;
    *(deft += 32) = *(deft + 6) = _BLACK_;
    *(deft + 1) = *(deft + 2) =*(deft + 3)=*(deft + 4)=*(deft + 5)=_WHITE_;
    *(deft += 32) = *(deft + 7) = _BLACK_;
    memset((deft + 1), _WHITE_, 6);
    *(deft += 32) = *(deft + 8) = _BLACK_;
    memset((deft + 1), _WHITE_, 7);
    *(deft += 32) = *(deft + 9) = _BLACK_;
    memset((deft + 1), _WHITE_, 8);
    *(deft += 32) = _BLACK_;
    memset((deft + 1), _WHITE_, 5); memset((deft + 6), 1, 5);
    *(deft += 32) = *(deft + 3) = *(deft + 6) = _BLACK_;
    *(deft + 1) = *(deft + 2) = *(deft + 4) = *(deft + 5) = _WHITE_;
    *(deft += 32) = *(deft + 2) = *(deft + 4) = *(deft + 7) = _BLACK_;
    *(deft + 1) = *(deft + 5) = *(deft + 6) = _WHITE_;
    *(deft += 32) = *(deft + 1) = *(deft + 4) = *(deft + 7) = _BLACK_;
    *(deft + 5) = *(deft + 6) = _WHITE_;
    *(deft += 32) = *(deft + 5) = *(deft + 8) = _BLACK_;
    *(deft + 6) = *(deft + 7) = _WHITE_;
    deft += 32;
    *(deft + 5) = *(deft + 8) = _BLACK_;
    *(deft + 6) = *(deft + 7) = _WHITE_;
    deft += 32;
    *(deft + 6) = *(deft + 9) = _BLACK_;
    *(deft + 7) = *(deft + 8) = _WHITE_;
    deft += 32;
    *(deft + 6) = *(deft + 9) = _BLACK_;
    *(deft + 7) = *(deft + 8) = _WHITE_;
    deft += 32;
    *(deft + 7) = *(deft + 8) = _BLACK_;
   #else
   deft += 0;
   #endif
   cursor_refresh();
   return 0;
}

int get_click_only()
{
   int click = (g_mouse_tp->mouse_b);
   g_mouse_tp->mouse_b = 0;
   return click;
}

/*---------------------------------------------------*
 *函数功能: 获得当前的鼠标高级按键状态.              *
 *备    注: 高级鼠标控制函数.用户可直接调用.         *
 *---------------------------------------------------*/
int get_click_info(void)
{
   #ifdef NEO_mouse_bind_timer_unused
   mouse_refresh();
   #else
   if (!_timer_installed)
   {
      mouse_refresh();
   }
   else
   {
      check_click();
   }
   #endif

   return g_mouse_tp->mouse_b;
}


/*---------------------------------------------------*
 *函数功能: 鼠标关闭函数,主要用于释放鼠标结构占用的内*
 *          存.                                      *
 *参数说明: 无.                                      *
 *返回说明: 无.                                      *
 *备    注: 由清理函数neo_exit();会自动调用所以用户一*
 *          般不用直接调用                           *
 *---------------------------------------------------*/
void remove_mouse(void)
{
   if (g_mouse_tp != NULL)
   {
      free(g_mouse_tp);
      g_mouse_tp = NULL;
   }
   g_routines &= 253;
}
#endif


#endif
