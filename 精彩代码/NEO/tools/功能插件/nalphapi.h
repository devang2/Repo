/*http://neo.coderlife.net*/
 /*ckerhome@yahoo.com.cn*/
 /* Plug-Ins of NEO SDK */
 /*Alpha透明处理插件 V0.1*/
 /* 创建日期:07/02/2005 */

/*混合一个点的函数*/
void  alpha_dot(int x, int y, int c, int n)
{
    Uint8 r1;   /*前景色的R分量*/
    Uint8 g1;   /*前景色的G分量*/
    Uint8 b1;   /*前景色的B分量*/
    Uint8 r2;   /*背景色的R分量*/
    Uint8 g2;   /*背景色的G分量*/
    Uint8 b2;   /*背景色的B分量*/

    /*获取前景色的RGB分量*/
    r1 = getr(c); g1 = getg(c); b1 = getb(c);
    c = get_dot(x, y);
    r2 = getr(c); g2 = getg(c); b2 = getb(c);
    /*对不同的颜色深度进行分别处理*/
    switch(get_color_depth())
    {
       /*8位色模式的Alpha混合*/
       case 8:
       r2 = r1 + n * (r2 - r1) / 8;
       g2 = g1 + n * (g2 - g1) / 8;
       b2 = b1 + n * (b2 - b1) / 8;
       break;
       /*15位色和16位色的Alpha混合*/
       case 15:
       case 16:
       r2 = r1 + n * (r2 - r1) / 64;
       g2 = g1 + n * (g2 - g1) / 64;
       b2 = b1 + n * (b2 - b1) / 64;
       break;

       default:
       break;
    }

    /*绘制混合以后的点*/
    dot(x, y, makecol(r2,g2,b2));
}

/*快速的半透明混合的函数*/
void  quick_alpha_dot(int x, int y, int c)
{
   int bc = get_dot(x, y);

   /*对不同的颜色深度进行分别处理*/
   switch(get_color_depth())
   {
      /*8位色的快速Alpha混合*/
      case 8:
         bc = (((bc & 0xda) + (c & 0xda)) >> 1) + ((((bc & 0x25) + (c & 0x25)) >> 1) & 0x25);
      break;
      /*15位色的快速Alpha混合*/
      case 15:
         bc = (((bc & 0xfbde) + (c & 0xfbde)) >> 1) + ((((bc & 0x0421) + (c & 0x0421)) >> 1) & 0x0421);
      break;
      /*16位色的快速Alpha混合*/
      case 16:
         bc = (((bc & 0xf7deL) + (c & 0xf7deL)) >> 1) + ((((bc & 0x0821) + (c & 0x0821)) >> 1) & 0x0821);
      break;

      default:
      break;
   }

    /*绘制混合以后的点*/
    dot(x, y, bc);
}


/*绘制一个Alpha Bar的函数*/
void  alpha_bar(int x, int y, int w, int h, int c, int n)
{
   int i;
   int j;

   if (n == 32)
   {
      for(i = 0; i < h; i++)
         for(j = 0; j < w; j++)
            quick_alpha_dot(x+j, y+i, c);
   }
   else
   {
      for(i = 0; i < h; i++)
         for(j = 0; j < w; j++)
            alpha_dot(x + j, y + i, c, n);
   }
}

/*main()
{
   neo_init();
   set_video_mode(640, 480, 16, 75, 0);
   show_bmp("_yue.bmp", 0, 0);
   getch();
   alpha_bar(0, 0, 320, 240, 0, 32);
   getch();
} */
