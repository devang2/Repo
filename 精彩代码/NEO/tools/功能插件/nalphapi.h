/*http://neo.coderlife.net*/
 /*ckerhome@yahoo.com.cn*/
 /* Plug-Ins of NEO SDK */
 /*Alpha͸�������� V0.1*/
 /* ��������:07/02/2005 */

/*���һ����ĺ���*/
void  alpha_dot(int x, int y, int c, int n)
{
    Uint8 r1;   /*ǰ��ɫ��R����*/
    Uint8 g1;   /*ǰ��ɫ��G����*/
    Uint8 b1;   /*ǰ��ɫ��B����*/
    Uint8 r2;   /*����ɫ��R����*/
    Uint8 g2;   /*����ɫ��G����*/
    Uint8 b2;   /*����ɫ��B����*/

    /*��ȡǰ��ɫ��RGB����*/
    r1 = getr(c); g1 = getg(c); b1 = getb(c);
    c = get_dot(x, y);
    r2 = getr(c); g2 = getg(c); b2 = getb(c);
    /*�Բ�ͬ����ɫ��Ƚ��зֱ���*/
    switch(get_color_depth())
    {
       /*8λɫģʽ��Alpha���*/
       case 8:
       r2 = r1 + n * (r2 - r1) / 8;
       g2 = g1 + n * (g2 - g1) / 8;
       b2 = b1 + n * (b2 - b1) / 8;
       break;
       /*15λɫ��16λɫ��Alpha���*/
       case 15:
       case 16:
       r2 = r1 + n * (r2 - r1) / 64;
       g2 = g1 + n * (g2 - g1) / 64;
       b2 = b1 + n * (b2 - b1) / 64;
       break;

       default:
       break;
    }

    /*���ƻ���Ժ�ĵ�*/
    dot(x, y, makecol(r2,g2,b2));
}

/*���ٵİ�͸����ϵĺ���*/
void  quick_alpha_dot(int x, int y, int c)
{
   int bc = get_dot(x, y);

   /*�Բ�ͬ����ɫ��Ƚ��зֱ���*/
   switch(get_color_depth())
   {
      /*8λɫ�Ŀ���Alpha���*/
      case 8:
         bc = (((bc & 0xda) + (c & 0xda)) >> 1) + ((((bc & 0x25) + (c & 0x25)) >> 1) & 0x25);
      break;
      /*15λɫ�Ŀ���Alpha���*/
      case 15:
         bc = (((bc & 0xfbde) + (c & 0xfbde)) >> 1) + ((((bc & 0x0421) + (c & 0x0421)) >> 1) & 0x0421);
      break;
      /*16λɫ�Ŀ���Alpha���*/
      case 16:
         bc = (((bc & 0xf7deL) + (c & 0xf7deL)) >> 1) + ((((bc & 0x0821) + (c & 0x0821)) >> 1) & 0x0821);
      break;

      default:
      break;
   }

    /*���ƻ���Ժ�ĵ�*/
    dot(x, y, bc);
}


/*����һ��Alpha Bar�ĺ���*/
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
