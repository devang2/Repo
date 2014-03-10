

/*

QR编解码参考了 Hank Wallace写的"Using The Golay Error Detection And
Correction Code"。编解码实现的是DMR协议中的QR(16,7,6)。编解码是在
QR(17,9,5)基础上实现的，即在数据低2位补零+监督位高位加一位奇偶校验位，
从而实现了QR(16,7,6)编解码。QR(16,7,6)可以纠正2为错误，检查出部分3位
错误。
                 
				                             LiuHui, 27th March 2009
*/


#include "stdio.h"
#include "conio.h"

#define POLY  0x139  /*QR(16,7,6)用到的生成多项式 */

/* ====================================================== */

int parity(unsigned long cw)
/* 对码字cw进行奇偶校验，cw有奇数个1返回1，否则返回0. */
{
  unsigned char p;

  /* 对每一字节进行异或*/
  p=*(unsigned char*)&cw;
  p^=*((unsigned char*)&cw+1);
  p^=*((unsigned char*)&cw+2);

  /* 对每一位异或 */
  p=p ^ (p>>4);
  p=p ^ (p>>2);
  p=p ^ (p>>1);

  return(p & 1);
}

/***********************************************************
 函数名称：QR
 函数功能：对指定数据进行QR编码.   
 入口参数：cw 待编码数据，低7位有效.
 返回值  ：编码后数据，低16位有效[监督位(9),数据位(7)].
 ***********************************************************/

unsigned long QR(unsigned long cw)
{
  int i;
  unsigned long c;
  cw&=0x7fl;
  c=cw; //保存原始数据
  for (i=1; i<=7; i++)  //计算监督位
    {
      if (cw & 1)        
        cw^=POLY;       
      cw>>=1;            
    }
  cw=(cw<<7)|c;//数据和监督位合并
  cw=cw^((parity(cw)<<15)&0x8000);//数据、监督位和奇偶校验位合并
  return(cw);    
}


/* ====================================================== */

unsigned long syndrome(unsigned long cw)
/* 计算返回QR(16,7,6)的伴随式 */
{
  int i;
  cw&=0x1ffffl;
  for (i=1; i<=7; i++)  // 检查每一位
    {
      if (cw & 1)        
        cw^=POLY;       
      cw>>=1;            
    }
  return(cw<<7);        //返回伴随式
}

/* ====================================================== */

int weight(unsigned long cw)
/* 计算15位码字码重(16位码字去掉1位奇偶校验位) */
{
  int bits,k;

  const char wgt[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};//半字码重表

  bits=0; //码重计数器
  k=0;
  /*计算码重，最多5个半字节 */
  while ((k<5) && (cw))
    {
      bits=bits+wgt[cw & 0xf];
      cw>>=4;
      k++;
    }

  return(bits);//返回码重
}

/* ====================================================== */

unsigned long rotate_left(unsigned long cw, int n)
/* 对17bit码字循环左移n位. */
{
  int i;

  if (n != 0)
    {
      for (i=1; i<=n; i++)
        {
          if ((cw & 0x10000l) != 0)
            cw=(cw << 1) | 1;
          else
            cw<<=1;
        }
    }

  return(cw & 0x1ffffl);
}

/* ====================================================== */

unsigned long rotate_right(unsigned long cw, int n)
/* 对17bit码字循环右移n位. */
{
  int i;

  if (n != 0)
    {
      for (i=1; i<=n; i++)
        {
          if ((cw & 1) != 0)
            cw=(cw >> 1) | 0x10000l;
          else
            cw>>=1;
        }
    }

  return(cw & 0x1ffffl);
}

/***********************************************************
 函数名称：correct
 函数功能：对指定数据进行纠错.   
 入口参数：cw 待纠错数据，低15位有效，
           *errs错误个数指针.
 返回值  ：纠错过的数据.
 ***********************************************************/

unsigned long correct(unsigned long cw, int *errs)
{
  unsigned char
    w;                 // 伴随式重量, 2 或 3 
  unsigned long
    mask;             // 用于屏蔽位的存储器 
  int
    i,j;              /* index */
  unsigned long
    s,                //伴随式
    cwsaver;          //原始数据暂存器

  cwsaver=cw;         //保存原始数据
  *errs=0;
  w=2;                //初始化伴随式码重
  j=-1;               //-1 = 第一次循环没有要检验的位
  mask=1;
  while (j<17) 
    {
      if (j != -1) 
        {
          if (j>0) //恢复上一位
            {
              cw=cwsaver ^ mask;
              mask+=mask; //指向下一位
            }
          cw=cwsaver ^ mask; 
          w=1; //最低码重
        }

      s=syndrome(cw); //查看有没错误
      if (s) //错误存在
        {
          for (i=0; i<17; i++) //循环移位并检查伴随式码重
            {
              if ((*errs=weight(s)) <= w) //伴随式符合错误类型
                {
                  cw=cw ^ s;              //纠正错误
                  cw=rotate_right(cw,i);  //循环右移数据
                  if(j!=-1) 
					  (*errs)++;
				  return(s=cw);
                }
              else
                {
                  cw=rotate_left(cw,1);   //移位到下一个错误图样
                  s=syndrome(cw);         //计算新的伴随式
                }
            }
          j++; 
        }
      else
        return(cw); //返回正确码字
    }

  return(cwsaver); //无错误返回原始码字
} 

/***********************************************************
 函数名称：QR_decode
 函数功能：对指定数据进行QR解码.   
 入口参数：correct_mode 纠错模式，为1纠错，为0只检错不纠错.
           *errs错误个数指针.
		   *cw待解码数据指针，低16位有效.
 返回值  ：correct_mode=1模式时，返回0代表无错误，返回1代表存在奇偶错误
           correct_mode=0模式时，返回0代表无错误，返回1代表存在奇偶错误，返回2代表码字存在错误
 ***********************************************************/

int QR_decode(int correct_mode, int *errs, unsigned long *cw)
{
  unsigned long parity_bit;

  if (correct_mode)                //纠错模式
    {
      parity_bit=*cw & 0x8000l; //保存奇偶校验位
      *cw&=~0x8000l;            //去除奇偶校验位

      *cw=correct(*cw, errs);     //纠错
      *cw|=parity_bit;            //还原奇偶校验位

      /* 检查第3个错误 */
      if (parity(*cw))            //奇偶校验位错
        return(1);
        return(0); //无错误
    }
  else //只检错
    {
      *errs=0;
      if (parity(*cw)) //奇偶校验位错
        {
          *errs=1;
          return(1);
        }
      if (syndrome(*cw))
        {
          *errs=1;
          return(2);
        }
      else
        return(0); //无错误
    }
} 


