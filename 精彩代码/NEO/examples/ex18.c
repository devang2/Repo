/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn

   这个程序演示了音频播放例程的简单使用
*/

/*要使用音频例程必须加上下边这个编译开关*/
#define NEO_sound_used

#include "neo.h"

main()
{
   SAMPLE *wav;
   neo_init();
   install_keyboard();
   install_sound();  /*安装音频例程*/
   wav = load_wav("win.wav"); /*读入一个音频采样*/
   printf("start play the wave!");
   play_sample_ex(wav, 0,0,0,0);   /*单次播放音频采样*/
   readkey();
   stop_sample(wav); /*中止音频采样*/
}
