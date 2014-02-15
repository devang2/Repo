/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn
   
   这个程序显示了NEO的系统调色板中所有的颜色
   NEO的系统调色板是由经典游戏"仙剑奇侠传I"的调色板经过分类排序等修改
   而来的，选色专业，颜色齐全，覆盖性好(有王婆卖瓜之嫌哦)一般匹配其他
   图片可以达到90%以上的近似效果
*/

/*通过下面编译开关开启对NEO系统调色板的支持*/
#define NEO_sys_pal_used

#include "neo.h"

/*填充函数*/
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


/*生成调色盘,一个Demo函数,用来显示所有的颜色.参数high为显示的高度*/
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
   palette(80);/*列出DOS的系统调色板所有颜色*/
   readkey();

   clear();
   set_neo_color();/*将DOS的调色板转换为NEO的系统调色板*/
   set_str_color(_WHITE);
   textout("The palette of NEO.", 10, 10);
   palette(80);/*列出NEO自己的调色板*/
   readkey();
}
