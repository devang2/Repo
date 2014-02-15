/*NEO SDK V3.1.00 For DOS
  Copyleft Cker Home 2003-2007.

  Open Source Obey NEO_PL.TXT.
  http://www.ds0101.com/neosdk
  ckerhome@yahoo.com.cn

  文件名称 : nsound.h
  摘    要 : 本头文件中包含了NEO SDK里有关音频播放的函数、结构、全局变量的声明定义
             目前支持格式:标准波形文件wav(PCM 8位 单/双声道)
  当前版本 : V0.67
  作    者 : 董凯
  完成日期 : 2006.01.18

  取代版本 : V0.66
  原 作 者 : 董凯
  完成日期 : 2005.12.29
*/

#ifndef  __NSOUND_H__
#define  __NSOUND_H__

#ifndef NEO_sound_unused

#define PCM_HEAD_LONG  50
#define PLAY_DELAY     100


typedef struct WaveData
{
   FILE           *fp;
   long           pos; /*当前WAV文件播放进度*/
   unsigned short sample_lenth; /*音频缓冲块的尺寸*/
   unsigned short rate;
   unsigned short channels;
   unsigned char  time_constant;
   char           loop;

   char           *sample;
}SAMPLE;

struct HeaderType
{
   long           riff;       /*RIFF类资源文件头部*/
   unsigned long  file_len;   /*文件长度*/
   char           wave[4];    /*"WAVE"标志*/
   char           fmt [4];    /*"fmt"标志*/
   char           NI1 [4];    /*过渡字节*/
   unsigned short format_type;/*格式类别(10H为PCM形式的声音数据)*/
   unsigned short Channels;   /*Channels 1 = 单声道; 2 = 立体声*/
   long           frequency;  /*采样频率*/
   long           trans_speed;/*音频数据传送速率*/
   char           NI2 [2];    /*过渡字节*/
   short          sample_bits;/*样本的数据位数(8/16)*/
   char           data[4];    /*数据标记符"data"*/
   unsigned long  wav_len;    /*语音数据的长度*/
   char           NI3 [4];    /*过渡字节*/
};

/*char g_last_wav[PATH_LENGTH] = {'\0'}; */

unsigned short g_base;        /*声卡基址*/
unsigned short g_port;
/*long sb_hw_dsp_ver = -1;*/

SAMPLE *g_sample;

void   write_dsp( unsigned char value );
short  ResetDSP ( unsigned short Test );
void   _sb_set_mixer(short index, char value);
short  install_sound(void);
short  play_sample_ex(SAMPLE *spl, short vol, short pan, short freq, short loop);
SAMPLE *load_wav(char *filename);
char   play_back  (SAMPLE *wave);
void   stop_sample(SAMPLE *spl);
void   destroy_sample(SAMPLE *spl);
void   remove_sound(void);

/*为兼容以前版本而增置*/
#define reset_dsp() ResetDSP(g_port)
#define play_sample(mode) play_sample_ex(SAMPLE *g_sample, 255, 255, 0, mode)
#define clear_digi_buffer()
#define set_port(port)
#define set_irq(irq)
#define set_dma(dma)

short install_sound(void)
{
   /*if (digi_card)
   {*/
      if ( ResetDSP (0x220) )
      {
         /*基址为220h*/
         g_port = 0x220;
      }
      else if (ResetDSP (0x230))
      {
         /*基址为230h*/
         g_port = 0x230;
      }
      else if (ResetDSP (0x240))
      {
         /*基址为240h*/
         g_port = 0x240;
      }
      else
      {
         /*基址检测失败*/
         g_port = -1;
         return 0;
      }
   /*}
   if (midi_card)
   {}*/

   g_routines |= 64;
   return 1;
}


/****************************************************************************
 检查一个声卡基址是否存在，如果存在则将声卡复位                             *
****************************************************************************/
short ResetDSP(unsigned short Test)
{
   /*重置DSP*/
   outportb (Test + 0x6, 1);
   delay(50);
   outportb (Test + 0x6, 0);
   delay(50);
   /*如果重置成功则检查*/
   if (( (inportb(Test + 0xE) & 0x80) == 0x80) && (inportb(Test + 0xA) == 0xAA))
   {
      /*DSP被找到*/
      g_base = Test;
      return (1);
   }
   else
   {  /*找不到DSP*/
      return (0);
   }
}


/****************************************************************************
** 发送一个字节到声卡的DSP(数字信号处理芯片：Digital Signal Processor)     **
****************************************************************************/
void write_dsp(unsigned char value)
{
   /*等待DSP接收一个字节*/
   while ((inportb(g_base + 0xC) & 0x80) == 0x80);
   /*发送字节*/
   outportb (g_base + 0xC, value);
}


/****************************************************************************
** 播放内存中的部分音频流                                                  **
****************************************************************************/
char play_back (SAMPLE *wave)
{
   long         LinearAddress;
   unsigned short page, offset;

   if ((!wave) || (!wave->sample))
   {
      printf("no sample!");
      return 0;
   }
   /*开启声卡*/
   write_dsp( 0xD1 );

   write_dsp( 0x40 );                    /*DSP第40h号命令 ：设置采样频率*/
   write_dsp( wave -> time_constant );            /*Write time constant*/
   /*将音频流指针转换成线性地址*/
   LinearAddress = FP_SEG ( wave -> sample );
   LinearAddress = ( LinearAddress << 4 ) + FP_OFF ( wave->sample );
   page   = LinearAddress >> 16;      /*计算页*/
   offset = LinearAddress & 0xFFFF;   /*计算页偏移*/
   /*注意 ：这个操作只能工作于DMA的第一通道*/
   outportb (0x0A, 5);                /*Mask 锁DMA通道一*/
   outportb (0x0C, 0);                /*清除DMA内部翻转标志*/
   outportb (0x0B, 0x49);             /*设置成回（播）放模式*/
   /*
    模式由下面几项组成:
    0x49 = 二进制 01 00 10 01
                  |  |  |  |
                  |  |  |  +- DMA通道 01
                  |  |  +---- 读操作 (从内存到DSP)
                  |  +------- 单一周期方式
                  +---------- 块方式
   */
   outportb ( 0x02, offset & 0x100);  /*将偏移量写入DMA控制器*/
   outportb ( 0x02, offset >> 8);
   outportb ( 0x83, page);             /*将页面写入DMA控制器*/
   outportb ( 0x03, wave->sample_lenth & 0x100);
   outportb ( 0x03, wave->sample_lenth >> 8);
   outportb ( 0x0A, 1 );               /*激活DMA通道一*/
   write_dsp( 0x14 );                  /*DSP第14h号命令 ：单一周期回放*/
   write_dsp( wave -> sample_lenth & 0xFF );
   write_dsp( wave -> sample_lenth >> 8);
   return 1;
}


/****************************************************************************
** 将音频文件读入内存                                                      **
** 这个例程能操作标准的PCM文件头部                                         **
** 它覆行了许多错误检查                                                    **
****************************************************************************/
SAMPLE *load_wav(char *filename)
{
   struct HeaderType t_header;
   FILE   *wav_file;
   SAMPLE *voice;

   /*如果打不开文件...*/
   wav_file = fopen(filename, "rb");
   if (wav_file == NULL)
   {
         #ifndef NEO_sys_report_error_unused
         Errinfo_t error = {"load_wav", NO_FILE, 1};
         throw_error(error);
         #endif
      return (NULL);
   }

   voice = (SAMPLE *)malloc(sizeof(SAMPLE));
   if ( voice == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"load_wav", NO_MEMORY, 1};
      throw_error(error);
      #endif
      return NULL;
   }

   /*if ((strnicmp(g_last_wav, filename, strlen(bmpfile) ) != 0)
   {*/
      /*读取文件头部*/
      fread ( &t_header, sizeof(t_header), 1, wav_file );
      /*检查RIFF头*/
      if ( t_header.riff != 0x46464952L)
      {
         #ifndef NEO_sys_report_error_unused
         Errinfo_t error = {"load_wav", ERR_FILE_TYPE, 1};
         throw_error(error);
         #endif
         return ( NULL );
      }
      /*检查通道*/
      /*if ( t_header.Channels != 1 )
      {
         #ifndef NEO_sys_report_error_unused
         Errinfo_t error = {"load_wav", "Not a MONO wave file!", 1};
         throw_error(error);
         #endif
         return ( NULL );
      }*/
      /*检查采样位数*/
      if ( t_header.sample_bits != 8 )
      {
         #ifndef NEO_sys_report_error_unused
         Errinfo_t error = {"load_wav", "Not an 8 bits wav file!", 1};
         throw_error(error);
         #endif
         return ( NULL );
      }
      /*strcpy(g_last_wav,filename);
   }*/

   voice->sample_lenth = (unsigned short)(t_header.file_len - PCM_HEAD_LONG);
   voice->time_constant= 256 - (1000000L / ( (voice->rate=t_header.frequency) * (voice->channels=t_header.Channels) ));

   if ( t_header.file_len - PCM_HEAD_LONG >= 0xffff )
   {
      voice->sample_lenth = 0xfffe;
   }
   if (voice->sample_lenth == 0) voice->sample_lenth = 1;
   voice->sample = (char *) malloc ( voice->sample_lenth + 1);/*申请内存*/
   if ( voice->sample == NULL)
   {
      #ifndef NEO_sys_report_error_unused
      Errinfo_t error = {"load_wav", NO_MEMORY, 1};
      throw_error(error);
      #endif
      fclose ( wav_file ); /*关闭文件*/
      return NULL;
   }
   voice->pos = voice->sample_lenth;
   /*fseek(wav_file, 57L, SEEK_SET);*/
   /*读取采样数据流*/
   fread  ( voice->sample, voice->sample_lenth + 1, 1, wav_file );
   fclose ( wav_file ); /*关闭文件*/
   g_sample = voice;
   return voice;
}


short play_sample_ex(SAMPLE *spl, short vol, short pan, short freq, short loop)
{

   vol += 0;
   pan += 0;
   freq+= 0;
   spl->loop |= loop;

   if (spl) /*如果采样存在*/
   {
      /*开始回放*/
      play_back(spl);
      delay(PLAY_DELAY);
      /*停止 DMA 传送*/
      write_dsp(0xD0);
      return 1;
   }
   return 0;
}


void stop_sample(SAMPLE *spl)
{ spl+=0;
/*   SAMPLE stop;
   char   sam[10];

   stop.sample = sam;
   stop.sample_lenth = 9;
   stop.time_constant= 9;
   play_back(&stop);*/
}


void destroy_sample(SAMPLE *spl)
{
   if (spl)
   {
      free(spl->sample);
      spl->sample = NULL;
      free(spl);
      spl = NULL;
   }
}


void remove_sound(void)
{
   /*关闭声卡*/
   write_dsp(0xD3);
   /*重置DSP*/
   reset_dsp();
   g_routines &= 191;
}

#endif


#endif
