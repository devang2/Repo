#include "dos.h"
#include "conio.h"
#include "stdio.h"

void InitScr();                                                                 /* ��ʼ��VGA256 */
void RstScr();                                                                  /* �ر�VGA256 */
void PutPoint(int x, int y, int Color);                                         /* VGA256��� */
void RectE(int x1, int y1, int x2, int y2, int Color);                          /* VGA256�վ��� */
void RectF(int x1, int y1, int x2, int y2, int Color1,int Color2);              /* VGAʵ���� */
void LineV(int x1, int y1, int x2, int y2, int Color);                          /* VGA256���� */

int main()                                                                      /* ������ */
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

void InitScr()                                                                  /* ��ʼ��VGA256 */
{
    union REGS In;
    In.x.ax = 0x13;/*����13Hģʽ  */
    int86(0x10, &In, &In);
}

void RstScr()                                                                   /* �ر�VGA256 */
{
    union REGS In;
    In.x.ax = 0x03;/* �˳�13Hģʽ */
    int86(0x10, &In, &In);
}

                                                                                /* ֱ��д��Ƶ������ */
void PutPoint(int x, int y, int Color)                                          /* VGA256���㺯�� */
{
   char far *p;
   p = (char far *) (0x0a0000000L);
   * (x+y*320+p) = Color;
}

/* ����VGA BIOS�ж�����Ļ�ϻ���, �ٶ���
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

void LineV(int x1, int y1, int x2, int y2, int Color)                           /* VGA256��һ��ֱ�� */
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

void RectE(int x1, int y1, int x2, int y2, int Color)                           /* VGA256����һ����*/
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

void RectF(int x1, int y1, int x2, int y2, int Color1, int Color2)              /* VGA256����һ����*/
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