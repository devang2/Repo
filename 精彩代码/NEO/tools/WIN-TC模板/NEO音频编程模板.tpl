/*WIN-TC&NEO SDK��Ƶ���ģ��
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/
/*����NEO����Ƶ���ŵ�֧��*/
#define NEO_sound_used

#include "neo.h"
main()
{
   SAMPLE *wav;
   neo_init();
   install_keyboard();
   install_sound();  /*��װ��Ƶ����*/
   wav = load_wav("wav.wav"); /*����һ����Ƶ����*/
   printf("start play the wave!");
   play_sample_ex(wav, 0,0,0,0);   /*���β�����Ƶ����*/
   _getch();
   stop_sample(wav); /*��ֹ��Ƶ����*/
}
