/*WIN-TC&NEO SDKʱ�ӱ��ģ��
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init(); /*NEO��ʹ��*/

   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_keyboard();
   _install_timer(); /*��װNEOʱ���жϳ��򣬴�ʱʱ���жϴ���Ƶ��Ϊ18��/��*/
   change_timer(60); /*�޸�ʱ�Ӵ���Ƶ�ʣ��û����ó���Ӧ��ֵ*/

   /*****�˲���������Լ��Ĵ���
   һ������Ϸ��ѭ��֮���,�磺
   while(!keypressed())
   {
      �û�����
   }
   �ȵ�*****/
}
