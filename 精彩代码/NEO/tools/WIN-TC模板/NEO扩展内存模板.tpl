/*WIN-TC&NEO SDK��չ�ڴ�ģ��
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   unsigned xms_ptr; /*����һ��αָ��*/

   neo_init(); /*NEO��ʹ��*/

   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_keyboard();
   if (xms_init()) /*��ʼ����չ�ڴ�*/
   neo_printf(0, 0, "Himem.sys load false.\n");

   xms_ptr = xms_alloc(1024); /*����1024K��1M����չ�ڴ�,�û�������Ҫ�޸�*/
   /*****�˲���������Լ��Ĵ���
   ������ѭ��֮��ĵȵ�*****/

   _getch(); /* ��ͣһ�£�����ǰ���������н�� */
}
