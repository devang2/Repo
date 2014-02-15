/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nword.h
  摘    要 : 本头文件中包含了NEO SDK里有关文字输出的各种函数、结构、全局变量的声明及定义
  当前版本 : V1.31
  作    者 : 董凯 
  完成日期 : 2006.12.29

  取代版本 : V1.30
  原 作 者 : 董凯 
  完成日期 : 2006.02.13
*/

#ifndef  NWORD_H
#define  NWORD_H

#define  S12X12   12
#define  S16X16   16
#define  S24X24   24
#define  S32X32   32
#define  S40X40   40
#define  S48X48   48

#define  N_TEXT_L_SLANT 1
#define  N_TEXT_R_SLANT -1

unsigned  g_text_bg = 0;
int  g_str_color = 0xff0f; /*默认字体颜色*/
int  g_ilimit = 16;
int  g_jlimit = 2;
int  g_blank = 0;          /*字符间距*/
int  g_hz_size = 32;
char g_slant = 0;          /*斜体标志*/
char g_start = 0;          /*汉字在字库中的起始区号*/
char *g_hz_font = "HZK16"; /*字库名*/

#define set_text_blank(blank)  g_blank  = blank
#define text_mode(mode)        g_text_bg = mode
#define set_str_color(color)   g_str_color = color
#define set_text_color(c)      g_str_color = c
#define get_str_color()        g_str_color
#define get_text_color()       g_str_color
#define cn_s16_out(str, x, y)  set_cn_size(S16X16);put_chinese(str, x, y, g_str_color, 1)
#define cn_s24_out(str, x, y)  set_cn_size(S24X24);put_chinese(str, x, y, g_str_color, 1)
#define get_cn_size()          g_ilimit
#define en_out(c, cx, cy)      asc_out(c, cx, cy, g_str_color)

unsigned char far *get_asc_rom(unsigned int bx);
unsigned char far *cur_asc_rom(int *chose);
void (*text_callback)() = NULL;
void en_slant(char slant);
void asc_out(char c, int cx, int cy, int color); /*显示速度:每秒约45000字*/
int  textout(char *str, int sx, int sy);   /*显示速度:每秒约45000字*/
int  put_chinese(char *string, int x, int y, int color, int blank);
void set_cn_font(char cn_size, char *cn_font);
void set_cn_size(char cn_size);
int  string_out(char *str, int x, int y); /*显示速度:每秒约6000字*/
int  neo_printf(int x, int y, char *format, ...);


/*获得ROM字符集首地址*/
unsigned char far *get_asc_rom(unsigned int bx)
{
    struct REGPACK reg;
    reg.r_ax = 0x1130;
    reg.r_bx = bx;
    intr(0x10, &reg);
    return (unsigned char far *)MK_FP(reg.r_es, reg.r_bp);
}


/*选择要使用的ASCII ROM字符集*/
unsigned char far *cur_asc_rom(int *chose)
{
   static char fault_ch = 8;
   static unsigned char far *romaddr = (unsigned char far *)MK_FP(0xf000, 0xfa6e);

   if (*chose>0)
   {
      switch (fault_ch = *chose)
      {
         case 8 : /*获得8*8  ASCII ROM字符集首地址*/
            return romaddr = get_asc_rom(0x0300);
         case 14: /*获得14*8  ASCII ROM字符集首地址*/
            return romaddr = get_asc_rom(0x0200);

         case 16: /*获得16*8  ASCII ROM字符集首地址*/
            return romaddr = get_asc_rom(0x0600);

         default:
            romaddr = get_asc_rom(0x0300);
            *chose = fault_ch = 8;
            return romaddr;
      }
   }
   *chose = fault_ch;
   return romaddr;
}


void en_slant(char slant)
{
   if (slant != 0)   
      g_slant = slant > 0?1 : -1;
   else
      g_slant = 0;
}


#ifndef NEO_english_out_unused
/*显示单个English字符*/
void asc_out(char c, int cx, int cy, int color)
{
   int  x, y, num = 0;
   unsigned char bit = 1;
   char slant = 0;
   int  ascrom = -1;
   unsigned char far *romchar_p = cur_asc_rom(&ascrom);
   unsigned char far *temp_p;
   temp_p = romchar_p + c * (ascrom);

   for (y = 0; y < ascrom; ++y)
   {
      bit = 1;slant += g_slant;
      for (x = 0; x < 8; ++x)
      {
         num = bit & (*temp_p);
         num >>= x;
         if (num == 1)
         {
            dot(cx + 8 - x + slant, cy + y, color);
         }
         else if (g_text_bg != 0)
         {
            dot(cx + 8 - x + slant, cy + y, g_text_bg);
         }
         bit <<= 1;
      }
      ++temp_p;
   }
}


/*显示English字符串*/
int textout(char *str, int sx, int sy)
{
   int index;
   for (index = 0; str[index] != 0; index++)
   {
      asc_out(str[index], sx + (index << 3) + g_blank, sy, g_str_color);
   }
   return index;
}
#else
void asc_out(char c, int cx, int cy, int color)
{
   cx+=0;cy+=0;c+=0;color+=0;
   #ifndef NEO_sys_report_error_unused
   {
     Errinfo_t error = {"asc_out", ROUTINE_UNACTIVE, 1};
     throw_error(error);
   }
   #endif
}
int  textout(char *str, int sx, int sy)
{
   sx+=0;sy+=0;str+=0;
   #ifndef NEO_sys_report_error_unused
   {
     Errinfo_t error = {"textout", ROUTINE_UNACTIVE, 1};
     throw_error(error);
   }
   #endif
}
#endif

unsigned int g_hzk_xms;
#ifndef NEO_cn_text_out_unused
int put_chinese(char *string, int x, int y, int color, int blank)
{
   FILE *hzk_p;
   long fpos;       /*汉字在字库文件中的偏移*/
   int  i, j, k, len = 0;
   int  asc_size = 0;/*用来或得当前英文字符的高*/
   char hz_buf[72]; /*汉字字模缓冲区，目前最多能支持24X24点阵字符的存放*/

   if ((hzk_p=fopen(g_hz_font,"rb+"))==NULL)/* 打开点阵字库文件 */
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"put_chinese", NO_FILE, 1};
      throw_error(error);
      #endif
      return -1;
   }
   cur_asc_rom(&asc_size);
   asc_size = abs(g_ilimit - asc_size);
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
         _FREAD(hz_buf, g_hz_size, 1, hzk_p);
         /*g = (char *)xms_read(g_hzk_xms, fpos, g_hz_size);
         memcpy(hz_buf, g, g_hz_size); */
         if (g_hz_size <= 32) /*16点阵以下的字库*/
         {
            for (i = 0; i < g_ilimit; ++i)
            {
               slant += g_slant;
               for (j = 0; j < g_jlimit; ++j)
               {
                  for (k = 0; k < 8; ++k)
                  {
                     if ((hz_buf[(i << 1) + j] >> (7 - k)) & 1)
                        dot(x + (j << 3) + k + slant, y + i, color);
                     else if (g_text_bg != 0)
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
           if ( (hz_buf[i * 3 + j] << (k - 1)) & 0x80 )
                        dot(x + i, y + (j << 3) + k - 1, color);
                     else if (g_text_bg != 0)
                        dot(x + i, y + (j << 3) + k - 1, g_text_bg);
                  }
               }
            }
         }
         x += (g_ilimit + blank);
         string += 2;
         ++len;
      }
      if (text_callback)
         (*text_callback)();
   }
   fclose(hzk_p);
   return len;
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
		break;
	case S16X16:
		g_hz_size= 32;
		g_hz_font= cn_font;
		break;
	case S24X24:
		g_jlimit = 3;
		g_hz_size= 72;
		g_start  = 15;
		g_hz_font= cn_font;
		break;
	case S40X40:
		g_jlimit = 4;
		g_hz_size= 200;
		g_start  = 15;
		g_hz_font= cn_font;
		break;
	case S48X48:
		g_jlimit = 5;
		g_hz_size= 288;
		g_start  = 15;
		g_hz_font= cn_font;
		break;
	default:
		g_ilimit = S16X16;
		g_hz_size= 32;
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
	case S48X48:
			set_cn_font(S48X48, "HZK48S");
			break;
	default:
		set_cn_font(S16X16, "HZK16");
		break;
	}
}
/*格式化输出字符串，支持中文，可变参数使用方式与printf相同，返回格式化后的字符串长度
  但有些控制位置的字符如"\n","\t"等无法正常工作，格式化后最长容纳255字节信息
  如使用neo_printf(10,20,"算式:%d-%d=%d",20,10,10);
  则在以屏幕点(10,20)为左上角输出字符串"算式:20-10=10"
*/
int neo_printf(int x, int y, char *format, ...)
{
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
   /*如果找不到汉字字库，则用textout()输出*/
   if (put_chinese(buffer, x, y, g_str_color, g_blank) == -1)
      textout(buffer, x, y);
   #ifndef NEO_error_unused
   err_method(err_m);
   #endif
   return (count);
}


int string_out(char *str, int x, int y)
{
   return put_chinese(str, x, y, g_str_color, g_blank);
}
#endif


#endif
