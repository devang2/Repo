/*http://neo.coderlife.net*/
 /*ckerhome@yahoo.com.cn*/
 /* Plug-Ins of NEO SDK */
 /* 最佳匹配色插件 V0.2 */
 /* 创建日期:06/30/2005 */

#ifndef  NBFIT_PI_H
#define  NBFIT_PI_H

#ifndef  __NEO_H__
/* neo.h必须在nbfit_pi.h之前被包含! */
#error neo.h must be included before nbfit_pi.h!
#endif

#define NEO_INT_MAX      2147483647
#define NEO_INT_MIN      (-NEO_INT_MAX - 1)

 /* 1.5k lookup table for color matching */
unsigned int npi_g_col_diff[3*128];

void npi_bestfit_init(void);
unsigned char npi_bestfit_color(PALETTE pal, int r, int g, int b);

/* npi_bestfit_init:
 *  Color matching is done with weighted squares, which are much faster
 *  if we pregenerate a little lookup table...
 */
void npi_bestfit_init(void)
{
   int i;

   for (i=1; i<64; i++)
   {
      int k = i * i;
      npi_g_col_diff[0  +i] = npi_g_col_diff[0  +128-i] = k * (59 * 59);
      npi_g_col_diff[128+i] = npi_g_col_diff[128+128-i] = k * (30 * 30);
      npi_g_col_diff[256+i] = npi_g_col_diff[256+128-i] = k * (11 * 11);
   }
}



/* npi_bestfit_color:
 * 从指定调色板pal中搜索与给出颜色分量r,g,b最匹配的颜色号并将其返回
   参数r,g,b范围为0~63；
 */
unsigned char npi_bestfit_color(PALETTE pal, int r, int g, int b)
{
   long coldiff, lowest;
   unsigned i, bestfit;
   if (npi_g_col_diff[1] == 0)
      npi_bestfit_init();

   bestfit = 0;
   lowest = NEO_INT_MAX;

   /* only the transparent (pink) color can be mapped to index 0 */
   if ((r == 63) && (g == 0) && (b == 63))
      i = 0;
   else
      i = 1;

   while (i<_PAL_SIZE)
   {
      RGB *rgb = &pal[i];
      coldiff = (npi_g_col_diff + 0) [ (rgb->g - g) & 0x7F ];
      if (coldiff < lowest)
      {
         coldiff += (npi_g_col_diff + 128) [ (rgb->r - r) & 0x7F ];
         if (coldiff < lowest)
         {
            coldiff += (npi_g_col_diff + 256) [ (rgb->b - b) & 0x7F ];
            if (coldiff < lowest)
            {
               bestfit = rgb - pal;    /* faster than `bestfit = i;' */
               if (coldiff == 0)
                  return bestfit;
               lowest = coldiff;
            }
         }
      }
      i++;
   }
   return bestfit;
}

#define npi_bestfit_col8(pal, r, g, b) npi_bestfit_color(pal, r, g, b)
#define npi_bestfit_col16(pal, r, g, b) npi_bestfit_color(pal, (r>>2), (g>>2), (b>>2))

unsigned char makecol8(int r, int g, int b)
{
   PALETTE pal;
   _get_palette(pal);
   return npi_bestfit_col16(pal, r, g, b);
}

#endif
