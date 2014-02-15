/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   ����NEO�ľ��βü�����������Ҷ��Ч����ʾλͼ
*/
#include "neo.h"

void line_cls(int color, char mode);

/*���������ú����������Ҷ��ʽ������Ч��
  ���в���color��ָ�����������ɫ����
  modeΪ��ʱ��ʾ����������Ϊ����ʾ��������*/
void line_cls(int color, char mode)
{
   int i;
   if (mode > 0) /*��������*/
   {
      for (i = 0; i <= g_rect_right; i += 2)
      {
         delay(500);
         line(i, 0, i, g_rect_bottom, color);
         line(g_rect_right - i, 0, g_rect_right - i, g_rect_bottom, color);
      }
   }
   else if (mode < 0) /*��������*/
   {
      for (i = 0; i <= g_rect_bottom; i += 2)
      {
         delay(500);
         hline(0, i, g_rect_right, color);
         hline(0, g_rect_bottom - i, g_rect_right, color);
      }
   }
}

main()
{
   int i;
   neo_init();
   install_keyboard();
   /*�л���640X480X64Kģʽ�£��൱��set_vbe_mode(VBE640X480X64K);*/
   set_video_mode(640, 480, 16, 75, 0); /*����640X480��16λͼ��ģʽ*/

   set_str_color(0xfef0);
   show_bmp("neosdk.bmp", 0, 0); /*��ʾLOGOλͼ*/
   textout("Press any key to continue...", 210, 290);
   readkey();
   clear();

   for (i = 0; i < 240; i += 2)
   {  /*���þ��βü�����ķ�Χ(�����÷�Χ����ʾ��������)*/
      screen(0, i * 2, 640, (i + 1) * 2);
      _show_bmp("world.bmp", 0, 0);
   }
   for (i = 239; i > 0; i -= 2)
   {
      screen(0, i * 2, 640, (i + 1) * 2);
      _show_bmp("world.bmp", 0, 0);
   }
   /*���ü�����ķ�Χ��λΪ������Ļ*/
   screen(0, 0, SCREEN_W, SCREEN_H); /*��SCREEN_W��SCREEN_H��NEO���Ѷ���*/
   line_cls(0, 1); /*�����Ҷ��ʽ������*/

   for (i = 0; i < 320; i += 2)
   {
      screen(i * 2, 0, (i + 1) * 2, 640);
      show_bmp("world.bmp", 0, 0);
   }
   for (i = 319; i > 0; i -= 2)
   {
      screen(i * 2, 0, (i + 1) * 2, 640);
      show_bmp("world.bmp", 0, 0);
   }
   /*���ü�����ķ�Χ��λΪ������Ļ*/
   screen(0, 0, SCREEN_W, SCREEN_H);
   line_cls(0, -1); /*�����Ҷ��ʽ������*/

   show_bmp("neosdk.bmp", 0, 0);
   textout("Press any key to exit...", 220, 290);
   readkey();
}
