/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn

   ���������ʾ����Ƶ�������̵ļ�ʹ��
*/

/*Ҫʹ����Ƶ���̱�������±�������뿪��*/
#define NEO_sound_used

#include "neo.h"

main()
{
   SAMPLE *wav;
   neo_init();
   install_keyboard();
   install_sound();  /*��װ��Ƶ����*/
   wav = load_wav("win.wav"); /*����һ����Ƶ����*/
   printf("start play the wave!");
   play_sample_ex(wav, 0,0,0,0);   /*���β�����Ƶ����*/
   readkey();
   stop_sample(wav); /*��ֹ��Ƶ����*/
}
