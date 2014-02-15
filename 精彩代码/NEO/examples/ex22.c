/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   ����������ú�ɫ�����Ͱ�ɫ������˶��Ա���ʾ��ֱ����ͼ��
   �ȴ���ֱ��ɨ�ͻ�ҳ���Ƶ������ص�
*/

#define NEO_key_buffer_unused /*���ü��̻�����*/
#include "neo.h" /*����NEO SDKͷ�ļ�*/

main()
{
   /*�ڴ˲����������*/
   int i;

   neo_init(); /*NEO��ʼ��*/
   err_method( 2 ); /*ָ����������*/
   install_keyboard(); /*��װ���̴������*/
   set_video_mode(320, 200, 16, 75, 0); /*����320X200��16λͼ��ģʽ*/
   clear_to_color(makecol16(255, 255, 255));
   surface_alloc(1, FALSE);

   _getch();
   textout("No Vsync & No Fliping & Delay(1000)", 10, 10);
   for (i = 0; i < 320; i++)
   {
      line(i - 51, 75, i - 51, 125, makecol16(255, 255, 255));
      delay(1000);
      rectfill(i - 50, 75, i, 125, makecol16(0, 0, 0));
   }

   clear_to_color(makecol16(255, 255, 255));
   _getch();
   textout("Vsync & No Fliping & No Delay", 10, 10);
   for (i = 0; i < 320; i++)
   {
      line(i - 51, 75, i - 51, 125, makecol16(255, 255, 255));
      vsync();
      rectfill(i - 50, 75, i, 125, makecol16(0, 0, 0));
   }

   clear_to_color(makecol16(255, 255, 255));
   _getch();
   set_work_surface(1);
   rectfill(0, 0, SCREEN_W, SCREEN_H, makecol16(255, 255, 255));
   textout("Vsync & Fliping & No Delay", 10, 10);
   for (i = 0; i < 320; i+=2)
   {
      rectfill(i - 52, 75, i-50, 125, makecol16(255, 255, 255));
      rectfill(i - 50, 75, i, 125, makecol16(0, 0, 0));
      flip_surface(1);
   }
}
