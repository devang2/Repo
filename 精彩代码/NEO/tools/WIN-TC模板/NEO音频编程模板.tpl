/*WIN-TC&NEO SDK音频编程模板
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
*/
/*启用NEO对音频播放的支持*/
#define NEO_sound_used

#include "neo.h"
main()
{
   SAMPLE *wav;
   neo_init();
   install_keyboard();
   install_sound();  /*安装音频例程*/
   wav = load_wav("wav.wav"); /*读入一个音频采样*/
   printf("start play the wave!");
   play_sample_ex(wav, 0,0,0,0);   /*单次播放音频采样*/
   _getch();
   stop_sample(wav); /*中止音频采样*/
}
