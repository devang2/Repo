/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   ���������ʾ��NEO��ϵͳ��ɫ�������е���ɫ
   NEO��ϵͳ��ɫ�����ɾ�����Ϸ"�ɽ�������I"�ĵ�ɫ�徭������������޸�
   �����ģ�ѡɫרҵ����ɫ��ȫ�������Ժ�(����������֮��Ŷ)һ��ƥ������
   ͼƬ���Դﵽ90%���ϵĽ���Ч��
*/

/*ͨ��������뿪�ؿ�����NEOϵͳ��ɫ���֧��*/
#define NEO_sys_pal_used

#include "neo.h"

/*��亯��*/
void fillcolor(int x,int y,unsigned char color)
{
   int mx1,my1,my2,mx2,my3,my4,i,lup=0,ldown=0,rup=0,rdown=0,left=0,right=0;

   mx1=mx2=x;
   my1=my2=my3=my4=y;
   {
     dot(x,y,color);
   }
   while (lup==0||ldown==0)
   {
      if (get_dot(mx1,my1+1)==0)
      {
         my1++;
      }
      else
      {
         ldown=1;
      }

      if (get_dot(mx1,my2-1)==0)
      {
         my2--;
      }
      else
      {
         lup=1;
      }
      dot(mx1,my1,color);
      dot(mx1,my2,color);
   }

   mx1=mx2=x;
   while (left==0||right==0)
   {
      if (get_dot(mx1-1,y)!=0)
      {
         left=1;
      }
      else
      {
         mx1--;
      }

      if (left==0)
      {
         my1=my2=y;
         dot(mx1,y,color);
         lup=ldown=0;
         while (lup==0||ldown==0)
         {
            if (get_dot(mx1,(my1-1))==0)
            {
               my1--;
            }
            else
            {
               lup=1;
            }

            if (get_dot(mx1,(my2+1))==0)
            {
               my2++;
            }
            else
            {
               ldown=1;
            }
            dot(mx1,my1,color);
            dot(mx1,my2,color);
         }
      }

      if (get_dot(mx2+1,y)!=0)
      {
         right=1;
      }
      else
      {
         mx2++;
      }

      if (right==0)
      {
         my3=my4=y;
         dot(mx2,y,color);
         rup=rdown=0;
         while (rup==0||rdown==0)
         {
            if (get_dot(mx2,my3+1)==0)
            {
               my3++;
            }
            else
            {
               rdown=1;
            }

            if (get_dot(mx2,my4-1)==0)
            {
               my4--;
            }
            else
            {
               rup=1;
            }
            dot(mx2,my3,color);
            dot(mx2,my4,color);
         }
      }
   }
}


/*���ɵ�ɫ��,һ��Demo����,������ʾ���е���ɫ.����highΪ��ʾ�ĸ߶�*/
void palette(int high)
{
   int m,n;
   unsigned char color=0;

   for (m=0;m<5;m++)
   {
      line(0,m*4+high,319,m*4+high,35);
   }
   for (n=0;n<64;n++)
   {
      line(n*5,high,n*5,16+high,35);
   }
   line(319,high,319,17+high,35);
   for (m=0;m<4;m++)
   {
      for (n=0;n<64;n++)
      {
         fillcolor(n*5+2,m*4+high+2,color);
         color++;
      }
   }
}

main()
{
   neo_init();
   install_keyboard();
   set_vbe_mode(VBE320X200X256);
   set_str_color(_WHITE);
   textout("The palette of DOS. Any key...", 10, 10);
   palette(80);/*�г�DOS��ϵͳ��ɫ��������ɫ*/
   readkey();

   clear();
   set_neo_color();/*��DOS�ĵ�ɫ��ת��ΪNEO��ϵͳ��ɫ��*/
   set_str_color(_WHITE);
   textout("The palette of NEO.", 10, 10);
   palette(80);/*�г�NEO�Լ��ĵ�ɫ��*/
   readkey();
}
