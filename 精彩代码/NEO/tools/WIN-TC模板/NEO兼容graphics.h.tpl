/*WIN-TC&NEO SDK����graphics.hģ��
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/
/*����NEO����graphics.hģʽ*/
#define NEO_hi_graphics_h_used

#include "neo.h"
main()
{
   neo_init(); /*NEO��ʹ��*/

   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_keyboard();

   /*****�˲���������Լ��Ĵ���
   ������ѭ��֮��ĵȵ�*****/

   _getch(); /* ��ͣһ�£�����ǰ���������н�� */
}
