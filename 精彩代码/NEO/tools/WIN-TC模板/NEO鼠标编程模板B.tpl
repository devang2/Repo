/*WIN-TC&NEO SDK�߼������ģ��(������ʱ������)
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#include "neo.h"
main()
{
   neo_init();

   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_keyboard();
   install_mouse();

   while(!keypressed()) /*������ѭ��*/
   {
      mouse_refresh();
      /*�����û��Լ��Ĵ����
        ......*/
   }
}
