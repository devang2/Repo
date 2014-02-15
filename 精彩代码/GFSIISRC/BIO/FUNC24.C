#include	<alloc.h>
#include        <stdio.h>
#include        <string.h>
#include       	<graphics.h>

#include        <def.inc>
#include 	<func.inc>

#define MAXNUM      255
#define HZK24_NAME	"c:\\charge\\hzk24.dot"

struct  cctab   {
		unsigned  cc;
		unsigned char no;
};

static UC Total ;
static struct cctab * Cc24tab ;
static FILE    *Libfp ;

/* ============================== init24() ============================== */
void init24 ()
{
UI i ;

  Libfp = fopen ( HZK24_NAME , "rb" ) ;
  if (Libfp==NULL )
	exit_scr(1,"\n\rFUNC24.C/init24(): Can not open the file: %s",HZK24_NAME );

  Total = getw(Libfp);
  Cc24tab = (struct cctab *)mem_alloc( Total*sizeof(struct cctab) );

  fseek( Libfp, 2, SEEK_SET );
  i = fread( Cc24tab, sizeof(struct cctab), Total, Libfp);
  if( i != Total )
	exit_scr(1,"\n\rFUNC24.C/init24(): Read file %s error.",HZK24_NAME );
}

UI search( UI k, UI low, UI up, struct cctab *cctab )
{
UI i;
UI cc;

	if( up==0 ) return -1;
	do {
		i = (up+low)/2;
		cc = cctab[i].cc;
		if( cc>k )
			up=i-1;
		else if( cc<k )
			low=i+1;
		else
			return i;
	} while( low<=up );
	return -1;
}

void  hz_24( UC qh, UC wh, UI x , UI y , UC color ,UC zt)
/*    zt == 0    --------   clibs.dot   */
/*    zt == 1    --------   clibh.dot   */
/*    zt == 2    --------   clibf.dot   */
/*    zt == 3    --------   clibg.dot   */
{
UI   i , j , k ;
UI   cc , x0 , y0 , mask ;
UL   pos ;
UC   bitbuf[72];
UC   *p;

   if (qh==0||wh==0) return;
   if (qh>87||wh>94) return;

   cc = ( (qh +0xa0 )<<8 ) + ( wh + 0xa0 ) ;
   i = search ( cc , 0 , Total , Cc24tab ) ;

   pos = 2+MAXNUM*sizeof(struct cctab)+(long)Cc24tab[i].no*72*4 +zt *72 ;

   fseek (Libfp , pos , SEEK_SET ) ;
   i = fread (bitbuf , 1 , 72 , Libfp ) ;
   if (i!=72) putchar ( 7 ) ;
   p = bitbuf ;
   x0 = x ; y0 = y ;
   set_dot_mode ( color ) ;
   for( i=0; i<24; i++ ) {
      for ( j=0;j<3;j++) {
            mask = 0x80;
            for( k=0; k<8; k++ ) {
                if( (*p)&mask )
                    pixel( x0, y0 );
                y0 ++;
                mask >>= 1;
            } ;
            p ++ ;
      } ;
   x0 ++ ; y0 = y ;
   }
   reset_dot_mode() ;
}

void  hz24_disp(UI x ,UI y , UC hz_str[50] , UC dist, UC color , UC zt )
{
UC hz1 , hz2 ;
	  while ( (hz1 = * hz_str) != 0 )
	  {
		if ( hz1<0xa0 )  hz_str ++ ;

		else {
			hz2 = * (++ hz_str) ;
			if ( hz2 < 0xa0 ) break ;
			hz_24 ( hz1 - 0xa0 , hz2 - 0xa0 , x , y , color , zt ) ;
			x += 24+dist ;
			hz_str ++ ;
		}  ;
    }
}     /* the place is overwritten (not cleaned ) by this string !  */

void disp_str24 (UI x ,UI y , UC hz_str[50],
					UC dist, UC bk_color , UC color ,UC zt )
{
UI length ;

	  length = ( strlen(hz_str)>>1 );
	  length = ( length-1 )*dist + length*24;

      setfillstyle ( SOLID_FILL , bk_color ) ;
      bar (x ,y , x + length , y + 24 ) ;

	  hz24_disp( x , y , hz_str , dist, color , zt ) ;
}     /* the place is covered by this string ! */

void    exit24()
{
    if( Cc24tab ) free( Cc24tab );
	fclose( Libfp );
}

