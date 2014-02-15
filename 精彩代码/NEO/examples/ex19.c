/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn

   16λ�߲�ģʽ�µ���ɫ����Ч��
*/

#include "neo.h"

main()
{
   unsigned int color; /*16λ�߲���ɫ����*/
   int y;

   neo_init();
   set_video_mode(320, 240, 16, 75, 0); /*����320X240��16λͼ��ģʽ*/

   /*��ɫ�����������λ��������Ϊ2048��ʵ�ֺ�ɫ32������*/
   for (color = 2048,y = 0; y < 32; color += 2048,++y)
   {
      hline(0, y, 319, color);
   }
   /*��ɫ�������м䣬������Ϊ32��ʵ����ɫ64������*/
   for (color = 32,y = 32; y < 96; color += 32,++y)
   {
      hline(0, y, 319, color);
   }
   /*��ɫ�����������λ��������Ϊ1��ʵ����ɫ32������*/
   for (color = 0,y = 96; y < 128; ++color,++y)
   {
      hline(0, y, 319, color);
   }
   /*�Ժ������������Ϊ2048����ɫ����64����ɫ����1������2113����ʵ�ְ�ɫ32������*/
   for (color = 0, y = 128; y <160; ++y, color += 2113)
   {
      hline(0, y, 319, color);
   }

   getch();
}
