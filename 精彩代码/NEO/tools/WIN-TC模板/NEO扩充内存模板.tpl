/*WIN-TC&NEO SDK�����ڴ�ģ��
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/

#define NEO_ems_used

#include "neo.h"
main()
{
   long ems_handle; /*����һ��αָ��*/

   neo_init(); /*NEO��ʹ��*/

   set_video_mode(640, 480, 8, 75, 0); /*����640X480��8λͼ��ģʽ*/
   install_keyboard();
   if(!ems_init())
   {
      neo_printf(0, 0, "Expanded memory is not present\n");
   }
   ems_handle = ems_alloc(10); /*����10ҳ��160K����չ�ڴ�,�û�������Ҫ�޸�*/
   /*****�˲���������Լ��Ĵ���
   ������ѭ��֮��ĵȵ�*****/

   _getch(); /* ��ͣһ�£�����ǰ���������н�� */
}
