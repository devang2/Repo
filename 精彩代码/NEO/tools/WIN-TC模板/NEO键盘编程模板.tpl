/*WIN-TC&NEO SDK���̱��ģ��
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO��ʹ��*/

   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_keyboard(); /*����NEO�����жϴ������*/

   /*****�˲���������Լ��Ĵ���
   ������ѭ��֮��ĵȵ�*****/

   _getch(); /* ��ͣһ�£�����ǰ���������н�� */
}
