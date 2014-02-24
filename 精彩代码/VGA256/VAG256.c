#include "dos.h"
#include "conio.h"
#include "stdio.h"

void InitScr();                                                                 /* 初始化VGA256 */
void RstScr();                                                                  /* 关闭VGA256 */
void PutPoint(int x, int y, int Color);                                         /* VGA256打点 */
void RectE(int x1, int y1, int x2, int y2, int Color);                          /* VGA256空矩形 */
void RectF(int x1, int y1, int x2, int y2, int Color1,int Color2);              /* VGA实矩形 */
void LineV(int x1, int y1, int x2, int y2, int Color);                          /* VGA256画线 */

int main()                                                                      /* 主函数 */
{
    int x1, y1, x2, y2, i, j,c=0;

    InitScr();
    for( i=0; i<256; i+=16)
        for( j=0; j<256; j+=16)
            {
                RectF( i/2, j/2, i/2+8, j/2+8, c, c);
                c++;
            }

    getch();
    RstScr();
}

void InitScr()                                                                  /* 初始化VGA256 */
{
    union REGS In;
    In.x.ax = 0x13;/*进入13H模式  */
    int86(0x10, &In, &In);
}

void RstScr()                                                                   /* 关闭VGA256 */
{
    union REGS In;
    In.x.ax = 0x03;/* 退出13H模式 */
    int86(0x10, &In, &In);
}

                                                                                /* 直接写视频缓冲区 */
void PutPoint(int x, int y, int Color)                                          /* VGA256画点函数 */
{
   char far *p;
   p = (char far *) (0x0a0000000L);
   * (x+y*320+p) = Color;
}

/* 利用VGA BIOS中断在屏幕上画点, 速度慢
void PutPoint(int x, int y, int Color)
{
   union REGS  In;
   In.h.ah = 0x0C;
   In.h.al = Color;
   In.x.cx = x;
   In.x.dx = y;
   In.h.bh = 0;
   int86(0x10, &In, &In);
}
*/

void LineV(int x1, int y1, int x2, int y2, int Color)                           /* VGA256画一垂直线 */
{
   int i;
   float a,b;

    if(x2-x1!=0)
        {
            for (i = x1; i <= x2; i++)
                {
                    a=i*(y2-y1)/(x2-x1)+(y2*x1-y1*x2)/(x2-x1);
                    PutPoint(x1, a, Color);
                }
        }
    else
        {
            if(y2>=y1)
                {   for (i=y1; i <= y2; i++)
                        {
                            PutPoint(x1, a, Color);
                        }
                }
        }
}

void RectE(int x1, int y1, int x2, int y2, int Color)                           /* VGA256画空一矩形*/
{
   int i;
   for(i = x1; i <= x2; i++)
   {
      PutPoint(i, y1, Color);
      PutPoint(i, y2, Color);
   }
   for(i = y1; i <= y2; i++)
   {
      PutPoint(x1, i, Color);
      PutPoint(x2, i, Color);
   }
}

void RectF(int x1, int y1, int x2, int y2, int Color1, int Color2)              /* VGA256画空一矩形*/
{
   int i, j;
   for(i = x1; i <= x2; i++)
        for(j = y1; j <= y2; j++)
            PutPoint(i, j, Color1);

   for(i = x1; i <= x2; i++)
        {
            PutPoint(i, y1, Color2);
            PutPoint(i, y2, Color2);
        }
   for(i = y1; i <= y2; i++)
        {
            PutPoint(x1, i, Color2);
            PutPoint(x2, i, Color2);
        }
}