/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ�����ͨ���������ز�ͬ��ICOͼ�������̬���Ч��
   �ó���������Ҫ��ͼ���ļ�flash.icoϵ����ͬ���ļ�����
*/

/*ͨ�����뿪�ؿ����Ե��������֧��*/
#define NEO_fade_pal_used
#include "neo.h"
#define  DIFFER 0.1f
main()
{
   float time_differ, time_cnt;
   char  i = 0;
   char  *ico[6] = {"flash1.ico", "flash2.ico", "flash3.ico", "flash4.ico", "flash5.ico", "flash6.ico"};
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_mouse();              /*��װ�߼��������*/
   surface_alloc(1, FALSE);
   set_work_surface(1);
   show_bmp("neosdk.bmp", 0, 0);
   flip();
   install_timer(new_1ch_int);   /*����ʱ�����̣����������״̬ˢ�º���*/
   change_timer(60);             /*�޸�ʱ���жϵĴ���Ƶ��Ϊ60HZ*/

   time_cnt = neo_clock();
   while (!keypressed())          /*�����������˳�*/
   {
      time_differ = neo_clock() - time_cnt;
      if (time_differ >= DIFFER)
      {
         i += (i < 5?1 : -5);
         set_mouse_icon(ico[i]);
         time_cnt = neo_clock();
      }
   }
   fade_out(216, 5);
}
