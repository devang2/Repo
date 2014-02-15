#define NEO_sys_pal_used        /*将系统调色板转换为NEO SDK推荐的逻辑调色板。*/

#include "neo.h"

void  box(int x_start, int y_start, int length, int width, int thickness, int color);
void fillbox(int x0, int y0, int length, int width, int color);
void diamond(int x_center, int y_center,int length, int width, int color);

main()
{
   int v,h,color=0,key;     /* v-vertical  h-horizontall */
   neo_init(); /*NEO初始化*/
   set_vbe_mode(VBE640X480X256); /*设置图形模式*/
   set_332_palette();      /*设置调色版*/

   while(!kbhit())
    {
     box(random(639), random(479), random(639), random(479), 10, random(255));
    }
   getch();
   fillbox(0,0,640,480,0);
   while(!kbhit())
    {
     diamond(random(639), random(479), random(639), random(479), random(255));
     delay(10000);   /*为便于察看效果而设置此延迟*/
    }
   getch();
   fillbox(0,0,640,480,0);
   neo_printf(100, 200, "Press Any Key to Show The NEO's Palette.");
   neo_printf(60, 230, "Then if you want to exit, just press any key or");
   neo_printf(60, 240, "click your mouse.");
   getch();
   fillbox(0,0,640,480,198);
   neo_printf(250, 2, "The NEO's Palette");

   for(v=0;v<16;v++)
    for(h=0;h<16;h++)
        {
         fillbox(h*37+25, v*27+25, 35, 25, color);   /*256个色块*/
         set_str_color(color==4?2:4);
         neo_printf(h*37+30, v*27+33, "%-3d", color);     /*输出序号*/
         color++;
         if(color>255)break;
        }

   install_mouse();       /*鼠标器高级初始化*/
   position_mouse(320,240);     /*将鼠标移动到屏幕的指定位置*/
   while(!mouse_act()&&!kbhit())    /*有鼠标点击或键击动作则退出*/
    {
     mouse_refresh();
    }
}

void box(int x_start, int y_start, int length, int width, int thickness, int color)
{
  /*############################################################################
    # 函数名：    box
    # 功能描述：  图形模式下绘制自定义边框厚度的方框/矩形(for NEO SDK)
    # 参数描述：
    #             x_start   －  方框左上角（点）x坐标值
    #             y_start   －  方框左上角（点）y坐标值
    #             length    －  方框长度
    #             width     －  方框宽度
    #             thickness －  方框边框厚度
    #             color     －  方框的填充色
    #
    # 返回值：    无
    # 原作者：    yonken   (yonken@163.com)
    # 修改者：    -
    # 改动部分：  -
    # 改动原因：  -
    # 备注：      可考虑把fillbox()函数与此函数整合为一；
    # 更新日期：  2006/01/25
    ############################################################################*/
   int i,maxthick;

   maxthick=length>width?width:length;          /*定义最大厚度值*/

   if(thickness>maxthick)thickness=maxthick;    /*防止厚度超出方框*/

   fillbox(x_start, y_start, length, thickness, color);       /*填充上面的矩形*/
   fillbox(x_start, y_start+width-thickness, length, thickness, color);        /*下面*/
   fillbox(x_start, y_start+thickness, thickness, width-2*thickness, color);   /*左边*/
   fillbox(x_start+length-thickness, y_start+thickness, thickness, width-2*thickness, color);   /*右边*/
}

void fillbox(int x_start, int y_start, int length, int width, int color)
{
  /*############################################################################
    # 函数名：    fillbox
    # 功能描述：  图形模式下绘制填充的方框/矩形(for NEO SDK)
    # 参数描述：
    #             x_start   －  方框左上角（点）x坐标值
    #             y_start   －  方框左上角（点）y坐标值
    #             length    －  方框长度
    #             width     －  方框宽度
    #             color     －  方框的填充色
    #
    # 返回值：    无
    # 原作者：    yonken   (yonken@163.com)
    # 修改者：    -
    # 改动部分：  -
    # 改动原因：  -
    # 备注：      -
    # 更新日期：  2006/01/25
    ############################################################################*/
   int ytemp,x1,y1;

   x1=x_start+length-1;     /*方框右下角（点）的横坐标*/
   y1=y_start+width-1;      /*方框右下角（点）的纵坐标*/

   for (ytemp=y_start; ytemp<=y1; ytemp++)
   {
      hline(x_start, ytemp, x1, color);     /*利用水平线绘制函数填充方框，与rectfill函数一致，基本未作改动*/
   }
}

void diamond(int x_center, int y_center,int length, int width, int color)
{
  /*############################################################################
    # 函数名：    diamond
    # 功能描述：  图形模式下绘制菱形(for NEO SDK)
    # 参数描述：
    #             x_center   －  菱形左上角（点）x坐标值
    #             y_center   －  菱形左上角（点）y坐标值
    #             length     －  菱形长度
    #             width      －  菱形宽度
    #             color      －  菱形的边框颜色
    #
    # 返回值：    无
    # 原作者：    yonken   (yonken@163.com)
    # 修改者：    -
    # 改动部分：  -
    # 改动原因：  -
    # 备注：      -
    # 更新日期：  2006/01/25
    ############################################################################*/
 int x1,y1,x2,y2,x3,y3,x4,y4;
 x1=x2=x_center;
 x3=x_center-length/2;
 x4=x_center+length/2;
 y1=y_center-width/2;
 y2=y_center+width/2;
 y3=y4=y_center;

 line(x1,y1,x3,y3,color);
 line(x1,y1,x4,y4,color);
 line(x2,y2,x3,y3,color);
 line(x2,y2,x4,y4,color);
}
