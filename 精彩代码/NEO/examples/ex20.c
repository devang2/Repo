/* 
   Example program for the NEO SDK, by Dong Kai.
   http://www.codelife.net
   ckerhome@yahoo.com.cn

   ��ʾ�������ʱ�ӳ������б�����û��Լ��Ĵ�����ʹ�䰴ָ������Ƶ�ʷ���ִ��
*/
#include "neo.h"

void bar1()
{
   static int i;
   i++;
   rectfill(i - 1, 100, i - 1 + 50, 150, _BLACK); /*����ԭ���ķ���*/
   rectfill(i, 100, i + 50, 150, _WHITE);         /*�����·���*/
}

void bar2()
{
   static int i;
   i++;
   rectfill(i - 1, 200, i - 1 + 50, 250, _BLACK); /*����ԭ���ķ���*/
   rectfill(i, 200, i + 50, 250, _WHITE);         /*�����·���*/
}

main()
{
   neo_init();
   install_keyboard();
   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/

   _install_timer(); /*��װNEOʱ������*/
   change_timer(1000); /*���ô���Ƶ��Ϊ1000����ʱ��ÿ�봥��1000���δ�ÿ���δ�ռ1����*/

   install_int_ex((T_func_ptr)bar1, 50); /*������bar1()����ʱ�ӳ������б�ִ���ٶȣ�ÿ100���δ�ִ��һ��*/
   install_int_ex((T_func_ptr)bar2, 200); /*������bar2()����ʱ�ӳ������б�ִ���ٶȣ�ÿ500���δ�ִ��һ��*/

   _getch();
}
