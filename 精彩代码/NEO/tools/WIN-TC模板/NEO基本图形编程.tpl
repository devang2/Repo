/*WIN-TC & NEO SDK����ͼ�α��ģ��V1.1
  http://neo.coderlife.net
  E-MAIL:ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO��ʹ��*/

   set_video_mode(640, 480, 8, 75, 0);  /*����ͼ��ģʽ*/
   install_keyboard();

   /*****�˲���������Լ��Ĵ��룬����
   line(25, 25, 220, 220, 15);
   circle(100,100,50, 12);
   �ȵ�*****/

   _getch(); /* ��ͣһ�£�����ǰ���ͼ��������н�� */
}
