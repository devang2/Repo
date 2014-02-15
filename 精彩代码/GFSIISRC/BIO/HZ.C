/* ------------------------------------------------------------------------
	FILENAME:	HZ.C
	FUNCTION:	Includes some basic HanZi I/O routines.
	AUTHOR:		Bob Kong.
	DATE:		?
	MODIFIED:	1995/2/16
   ----------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include <alloc.h>
#include <graphics.h>

#include <def.inc>
#include <func.inc>

UI EMB_HZ_handle = 0xFFFF;

static UC far *Rom_char_lib;
static UC Hzk_file_j[64] = "\\C&C08A\\DATA\\CCLIBJ.DOT";
static UC Hzk_file_f[64] = "\\C&C08A\\DATA\\CCLIBF.DOT";
static UC hz_str_type=1;        /* 16 pixel */
//#define		WIN_LEFT		  8
#define		WIN_RIGHT		632
int 		winleft,left,right;
UC   		win_prn_flag;
/* ============================= load_char_lib =========================== */
void load_char(void)
{
static UI keep_es1,keep_es2,keep_bp1,keep_bp2;

	keep_es1 = _ES;
	keep_bp1 = _BP;
	_AX = 0x1130;
	_BH = 0x06;
	geninterrupt(0x10);
	keep_bp2 = _BP;
	keep_es2 = _ES;
	_BP = keep_bp1;
	_ES = keep_es1;
	Rom_char_lib = MK_FP(keep_es2, keep_bp2);
}

void set_hzk_file(UC *file_j, UC *file_f)
{
	if (file_j!=NULL && *file_j!='\0')
		strcpy(Hzk_file_j, file_j);

	if (file_f!=NULL && *file_f!='\0')
		strcpy(Hzk_file_f, file_f);
}
/* ------------------------------------------------------------------------
	FUNCTION:	Load the Chinese characters' library to the extened memory.
	CALLS:
	CALLED BY:
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		0 -- Load successfully.
				1 -- No enough extened memory.
				2 -- File \C&C08A\DATA\CCLIB.DOT not exist.
				3 -- No enough temp memory.
				4 -- Error reading file \C&C08A\DATA\CCLIB.DOT .
				5 -- Error accessing extened memory.
   ------------------------------------------------------------------------- */
UC load_hzk(UC hz_style)
{
UC * tmp_buf;
UI tmp_handle;
UI bytes;
UC loopi;

	if ( EMB_size()<250 )
		return 1;		/* Not enough EMB memory */
	EMB_HZ_handle = EMB_alloc(250);
	if (EMB_HZ_handle==0xFFFF)
		return 1;		/* Not enough EMB memory */

	if (hz_style == 1)
		tmp_handle = open(Hzk_file_j, O_BINARY|O_RDONLY);
	else
		tmp_handle = open(Hzk_file_f, O_BINARY|O_RDONLY);

	if (tmp_handle==0xffff)
		return 2;

	tmp_buf = (UC *)farmalloc(65536L);
	if (tmp_buf==NULL)
	{
		close(tmp_handle);
		return 3;
	}

	bytes = 10*94*32;
	for (loopi=0; loopi<8; loopi++)
	{
		if (read(tmp_handle, (UC *)tmp_buf, bytes)==-1)
		{
			farfree(tmp_buf);
			close(tmp_handle);
			return 4;
		}

		if (EMB_write(EMB_HZ_handle, (UL)bytes*loopi, tmp_buf, bytes)!=0)
		{
			farfree(tmp_buf);
			close(tmp_handle);
			return 5;
		}
	}

	bytes = 94*32;
	if (read(tmp_handle, (UC *)tmp_buf, bytes)==-1)
	{
		farfree(tmp_buf);
		close(tmp_handle);
		return 4;
	}

	if (EMB_write(EMB_HZ_handle, (UL)bytes*80L, tmp_buf, bytes)!=0)
	{
		farfree(tmp_buf);
		close(tmp_handle);
		return 5;
	}

	farfree(tmp_buf);
	close(tmp_handle);
	return 0;
}
/* =========================== unload_hzk() ============================= */
void unload_hzk(void)
{
	if ( EMB_HZ_handle!=0xFFFF )
		EMB_free(EMB_HZ_handle);
}

/* =============================== hz_24 ================================ */
/*
void hz_24(UC hz_No,UI pox,UI poy,UC color)
{
UC i,k;
UC dot_byte,count;
UC far *dot_here;
static UC bit_map[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	dot_here = hz24k_buf+hz_No*72;
	set_dot_mode(color);
	count=3;

	for (i=0;i<72;i++)
	{
		dot_byte = *(dot_here++);
		for (k=0;k<8;k++)
		{
			if ( (dot_byte & bit_map[k]) )
			pixel(pox,poy);
			poy++;
		}
		if (--count==0)
		{
			pox++;
			poy-=24;
			count=3;
		}
	}
	reset_dot_mode();
}
*/

	  /*************************************************************
	   *       hz_enlarge(...) handle a 32-pots chinese            *
	   *       UC hz_enlarge(UC *hz_dot,UC *hz_dot0,UC loop0)      *
       *       CALLS     : none                                    *
       *       CALLED BY : hz_16(...)                              *
       *       AUTHOR    : LI CHENG                                *
	   *       DATE      : 20/09/94                                *
       *************************************************************/

void hz_enlarge(UC *hz_dot, UC *hz_dot0, UC loop0)
{
UC dot0,  dot1;
UC loop1, loop2;

	  memset(hz_dot,0,32);
	  for(loop2=0;loop2<8;loop2++)
	  {
		for(loop1=0;loop1<4;loop1++)
		{
		 switch(loop0)
		  {
		   case 0:
			  dot0=(( hz_dot0[loop2*2]>>(loop1+4))& 0x01)<<loop1*2;
			  dot1=(( hz_dot0[loop2*2]>> loop1)   & 0x01)<<loop1*2;
				  break;
		   case 1:
			  dot0=(( hz_dot0[16+loop2*2]>>(loop1+4))& 0x01)<<loop1*2;
			  dot1=(( hz_dot0[16+loop2*2]>> loop1)   & 0x01)<<loop1*2;
			  break;
		   case 2:
			  dot0=(( hz_dot0[loop2*2+1]>>(loop1+4))& 0x01)<<loop1*2;
			  dot1=(( hz_dot0[loop2*2+1]>> loop1)   & 0x01)<<loop1*2;
			  break;
		   case 3:
			  dot0=(( hz_dot0[16+loop2*2+1]>>(loop1+4))& 0x01)<<loop1*2;
			  dot1=(( hz_dot0[16+loop2*2+1]>> loop1)   & 0x01)<<loop1*2;
			  break;
		   default :
			  break;
		  }
		 dot0 *= 0x03;
		 hz_dot[loop2*4] |= dot0;

		 dot1 *= 0x03;
		 hz_dot[loop2*4+1] |= dot1;
      	}
       hz_dot[loop2*4+2] = hz_dot[loop2*4];
       hz_dot[loop2*4+3] = hz_dot[loop2*4+1];
      }

}

	  /*************************************************************
       *       str_enlarge(...) handle a 32-pots chinese           *
	   *       UC hz_enlarge(UC *str_dot,UC *str_dot0,UC loop0)    *
	   *       CALLS     : none                                    *
	   *       CALLED BY : str_16(...)                             *
	   *       AUTHOR    : LI CHENG                                *
	   *       DATE      : 20/09/94                                *
	   *************************************************************/

void str_enlarge(UC *str_dot, UC *str_dot0, UC loop0)
{
UC dot0;
UC loop1, loop2;

	  memset(str_dot,0,16);
	  for(loop2=0;loop2<8;loop2++)
	   {
		for(loop1=0;loop1<4;loop1++)
		 {
		  switch(loop0)
           {
		   case 0:
			  dot0=(( str_dot0[loop2]>>(loop1+4))  & 0x01) << loop1*2;
			  break;
		   case 1:
	   		  dot0=(( str_dot0[8+loop2]>>(loop1+4))& 0x01) << loop1*2;
              break;
           case 2:
			  dot0=(( str_dot0[loop2]>> loop1)   & 0x01) << loop1*2;
			  break;
		   case 3:
			  dot0=(( str_dot0[8+loop2]>> loop1) & 0x01) << loop1*2;
			  break;
		   default :
			  break;
           }

		 dot0 *= 0x03;
		 str_dot[loop2*2] |= dot0;
      	}
	   str_dot[loop2*2+1] = str_dot[loop2*2];
	  }

}


      /*************************************************************
	   *       hz_16 dsplay a 16-pots or 32-pots chinese.          *
       *       UC hz_16(UC asc_code, UI pox, UI poy,UC color)      *
	   *       CALLS     : hz_enlarge(UC*,UC*,UC)                  *
       *       CALLED BY :                                         *
       *       AUTHOR    :                                         *
       *       DATE      :                                         *
	   *       MODIFY    : LI CHENG                                *
       *       DATE      : 20/09/94                                *
	   *************************************************************/

void hz_16 (UC qh, UC wz, UI pox, UI poy, UC color)
{
UC loop,  loop0;
UI     pox0,poy0;
UC inner_pos;
UC dot_byte[2];
UC hz_dot0[32],hz_dot[32];

	if (qh==0 || wz==0) return;
	if (qh>87 || wz>94) return;
	if (qh>9  && qh<16) return;
	if ( qh>15 ) qh -= 6;

	EMB_read(hz_dot0, EMB_HZ_handle, (--qh)*94*32L+(--wz)*32L, 32);
	set_dot_mode(color);

	for ( loop0=0; loop0<4; loop0++)
	 {
	  if ( hz_str_type == 2)

		   hz_enlarge(hz_dot,hz_dot0,loop0);
	  else
		   memcpy(hz_dot,hz_dot0,32);

	  pox0 = pox + (loop0/2)*16;
	  poy0 = poy + (loop0%2)*16;
	  inner_pos = pox0 - ( (pox0 >> 3) << 3);
	  for (loop=0; loop<16; loop++)
	  {
		  dot_byte[0] = hz_dot[loop*2] >> inner_pos;
		  dot_byte[1] = hz_dot[loop*2] << (8-inner_pos);

		  if  (win_prn_flag)
		  {
			   if  ((pox0-left*8) >= winleft &&
					(pox0-left*8) <= (WIN_RIGHT-8))
				   pixels(pox0-left*8, poy0+loop, dot_byte, 2);
			   else break;
		  }
		  else
			  pixels(pox0, poy0+loop, dot_byte, 2);
	  }

	  for (loop=0; loop<16; loop++)
	  {
		  dot_byte[0] = hz_dot[loop*2+1] >> inner_pos;
		  dot_byte[1] = hz_dot[loop*2+1] << (8-inner_pos);
		  if  (win_prn_flag)
		  {
			   if  ((pox0-left*8+8) >= winleft &&
					(pox0-left*8+8) <= (WIN_RIGHT-8))
				   pixels(pox0-left*8+8, poy0+loop, dot_byte, 2);
			   else break;
		  }
		  else
			  pixels(pox0+8, poy0+loop, dot_byte, 2);
	  }

	  if ( hz_str_type == 1 )
		   break;
	}
	reset_dot_mode();
}

/*
void hz_16(UC qh, UC wz, UI pox, UI poy, UC color)
{
UC loop;
UC temp;
UI ttt;
UC inner_pos;
UC dot_byte[3];
UC hz_dot[32];

	if (qh==0 || wz==0) return;
	if (qh>87 || wz>94) return;
	if (qh>9  && qh<16) return;
	if ( qh>15 ) qh -= 6;

	EMB_read(hz_dot, EMB_HZ_handle, (--qh)*94*32L+(--wz)*32L, 32);
	set_dot_mode(color);

	inner_pos = pox - ( (pox>>3)<<3);
	for (loop=0; loop<16; loop++)
	{
		temp = loop<<1;
		dot_byte[0] = hz_dot[temp]>>inner_pos;

		ttt = (hz_dot[temp]<<8) | hz_dot[temp+1];
		ttt <<= 8-inner_pos;
		dot_byte[1] = ttt >> 8;
		dot_byte[2] = ttt & 0x00FF;

		pixels(pox, poy+loop, dot_byte, 3);
	}
	reset_dot_mode();
}
*/

	  /*************************************************************
	   *       char_16 dsplay report and output printer            *
	   *       UC char_16(UC asc_code, UI pox, UI poy,UC color)    *
	   *       CALLS     : none                                    *
	   *       CALLED BY :                                         *
	   *       AUTHOR    :                                         *
	   *       DATE      :                                         *
	   *       MODIFY    : LI CHENG                                *
	   *       DATE      : 20/09/94                                *
	   *************************************************************/

/* ============================= char_16 ================================ */
void char_16(UC asc_code, UI pox, UI poy, UC color)
{
UI pox0,poy0;
UC loop, loop0;
UC inner_pos;
UC dot_byte[2];
UC str_dot[16],str_dot0[16];

	memcpy(str_dot0, Rom_char_lib+(asc_code<<4), 16);

	set_dot_mode(color);

	for ( loop0=0; loop0 < 4; loop0++)
	{
	  if ( hz_str_type == 2)
		   str_enlarge ( str_dot, str_dot0, loop0);
	  else
		   memcpy ( str_dot, str_dot0, 16);

	  pox0 = pox + (loop0/2)* 8;
	  poy0 = poy + (loop0%2)*16;
	  inner_pos = pox0 - ( (pox0 >> 3) << 3);
	  for (loop=0; loop<16; loop++)
	  {
		  dot_byte[0] = str_dot[loop] >> inner_pos;
		  dot_byte[1] = str_dot[loop] << (8-inner_pos);

		  if  (win_prn_flag)
		  {
               if  ((pox0-left*8) >= winleft &&
					(pox0-left*8) <= (WIN_RIGHT-8))
				   pixels(pox0-left*8, poy0+loop, dot_byte, 2);
			   else break;
		  }
		  else
			  pixels(pox0, poy0+loop, dot_byte, 2);

	  }
	  if ( hz_str_type == 1)
		   break;
	}
	reset_dot_mode();
}

/* ============================= hz16_disp =============================== */
void hz16_disp(UI x, UI y, UC *hz_str, UC color)
{
UC han1, han2, i;

	i = 0;
	while ( (han1=hz_str[i]) != 0)
	{
		if ( han1<0xa0 )
		{
			char_16(han1, x, y+1, color);
			x += 8;
			i++;
		}
		else
		{
			han2 = hz_str[++i];
			if (han2<160)
				return;
			hz_16((han1-160), (han2-160), x, y, color);
			x += 16;
			i++;
		}
	}
}

/* ============================= hz16_fill =============================== */
void hz16_fill(UI x, UI y, UC *hz_str, UC fill_len, UC color)
{
UC han1, han2, i;

	i = 0;
	while ( i<fill_len && (han1=hz_str[i])!=0)
	{
		if ( han1<160 )
		{
			char_16(han1, x, y+1, color);
			x += 8;
			i++;
		}
		else
		{
			if (i>=fill_len)
				return;
			han2 = hz_str[++i];
			if (han2<160)
				return;
			hz_16((han1-160), (han2-160), x, y, color);
			x += 16;
			i++;
		}
	}
}


/* ============================= topic_disp ============================== */
void topic_disp(UI x, UI y, UC *hz_str, UC hz_color, UC char_color)
{
UC han1,han2,i;

	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			char_16(han1, x, y+1, char_color);
			x += 8;
			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160) return;
			hz_16((han1-160),(han2-160),x,y,hz_color);
			x += 16;
			i++;
		}
	}
}

	  /***************************************************************
	   *       disp_str dislay a hz or str on the definited position.*
	   *       void disp_str(UI x,UI y,UC *hz_str,UC bk_color...)    *
	   *       CALLS     : hz_16(...),char_16(...)                   *
	   *       CALLED BY :                                           *
	   *       AUTHOR    :                                           *
	   *       DATE      :                                           *
	   *       MODIFY    : LI CHENG                                  *
	   *       DATE      : 20/09/94                                  *
	   ***************************************************************/
/* ============================== disp_str ============================== */
void disp_str(UI x, UI y, UC *hz_str, UC bk_color, UC char_color)
{
UC han1,han2;
int i;
struct fillsettingstype old_setting;

	i = strlen(hz_str);
	if (i==0 || i>=181) return;
	if (win_prn_flag && i<=left) return;
	getfillsettings(&old_setting);
	setfillstyle(SOLID_FILL, bk_color);
	if ( win_prn_flag)
	   bar(x,y,x+(min(i,right)-left)*8-1,y+15);
	else
	   bar(x, y, x+(i<<3)-1, y+15);

	i=0;
	hz_str_type = 1;
	while ( (han1=hz_str[i]) != 0 )
	{
		if ( han1 < 0x80 )
		{
			 if (win_prn_flag && i<(left-1) )
				 ;
			 else
				char_16(han1, x, y+1, char_color);
			 x += 8*hz_str_type;
			 hz_str_type = 1;
		}

		else if ( han1==0x80 )
				  hz_str_type = 2;
		else
		{
			 han2 = hz_str[++i];

			 if ( hz_str_type==2)
				  if ( han2==0x80 )
					   han2 = hz_str[++i];
				  else return;

			 if ( han2 < 0xa0 )
//				  return;
				  continue;
			 if (win_prn_flag && i<(left-3) )
				 ;
			 else
				 hz_16((han1-160),(han2-160),x,y,char_color);
			 x += 16*hz_str_type;
			 hz_str_type = 1;
		}
		if (win_prn_flag && i>right)
			break;
		i++;
	}
	setfillstyle(old_setting.pattern,old_setting.color);
}

/* ============================= disp_20dot ============================== */
void disp_20d(UI x, UI y, UC *hz_str, UC bk_color, UC char_color)
{
UC han1,han2;
UI i;
struct fillsettingstype old_setting;

	i = strlen(hz_str);
	if (i==0 || i>=80) return;

	getfillsettings(&old_setting);
	setfillstyle(SOLID_FILL,bk_color);
	bar(x-1, y-2, x+(i<<3), y+17);

	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			char_16(han1, x, y+1, char_color);
			x += 8;
			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160) return;
			hz_16((han1-160), (han2-160), x, y, char_color);
			x += 16;
			i++;
		}
	}
	setfillstyle(old_setting.pattern,old_setting.color);
}

/* ========================== support view port ========================== */
/* ------------------------------- hz1_16 -------------------------------- */
void hz1_16(UC qh,UC wz,UI pox,UI poy,UC color)
{
UC i,k;
UC dot_byte;
UC hz_dot[32];
static UC bit_map[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	if (qh==0||wz==0) return;
	if (qh>87||wz>94) return;
	if ( qh>9 ) qh -= 6;

	EMB_read(hz_dot, EMB_HZ_handle, (--qh)*94*32L+(--wz)*32L, 32);

	for (i=0;i<32;i++)
	{
		dot_byte = hz_dot[i];
		for (k=0;k<8;k++)
		{
			if ((dot_byte & bit_map[k]) != 0)
				putpixel(pox,poy,color);
			pox++;
		}
		if ( i&0x01 )
		{
			poy++;
			pox -= 16;
		}
	}
}

/* ============================= char_16 ================================ */
void char1_16(UC asc_code, UI pox, UI poy, UC color)
{
UC i,k,dot_byte;
UC far *dot_here;
static UC bit_map[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	if (asc_code>127) return;
	dot_here = Rom_char_lib+(asc_code<<4);
	for (i=0;i<16;i++)
	{
		dot_byte = *(dot_here++);
		for (k=0;k<8;k++)
		{
			if ((dot_byte & bit_map[k]) != 0)
				putpixel(pox,poy,color);
			pox++;
		}
		poy++;
		pox-=8;
	}
}

/* ============================= hz16_disp =============================== */
void hz16_disp1(UI x,UI y,UC hz_str[80],UC color)
{
UC han1,han2,i;

	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			char1_16(han1, x, y+1, color);
			x += 8;
			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160) return;
			hz1_16((han1-160),(han2-160),x,y,color);
			x += 16;
			i++;
		}
	}
}

	   /*************************************************************
	   *       char_16 dsplay report and output printer             *
	   *       UC char_16(UC asc_code, UI pox, UI poy,UC color)     *
	   *       CALLS      : none                                    *
	   *       CALLED BY  :                                         *
	   *       AUTHOR     :                                         *
	   *       DATE       :                                         *
	   *       MODIFY     : LI CHENG                                *
	   *       DATE       : 11/10/94				    			*
	   *	   DESCRIPTION: what is str? its index least than 0xb0  *
	   *************************************************************/
/* ============================= topic_disp ============================== */
void topic_disp1(UI x,UI y,UC hz_str[80],UC hz_color,UC char_color)
{
UC han1,han2,i;

	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			char1_16(han1, x, y+1, char_color);
			x += 8;
			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160) return;
			if (han1<0xb0)
				hz1_16((han1-160),(han2-160),x,y,char_color);
			else
				hz1_16((han1-160),(han2-160),x,y,hz_color);
			x += 16;
			i++;
		}
	}
}
/* ============================== disp_str ============================== */
void disp_str1(UI x,UI y,UC hz_str[80],UC bk_color,UC char_color)
{
UC han1,han2;
UI i;
struct fillsettingstype old_setting;

	i=0;
	while(i<80 && hz_str[i]!=0) i++;
	if (i==0 || i==80) return;

	getfillsettings(&old_setting);
	setfillstyle(SOLID_FILL,bk_color);
	bar(x,y,x+(i<<3)-1,y+15);

	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			char1_16(han1, x, y+1, char_color);
			x += 8;
			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160) return;
			hz1_16((han1-160),(han2-160),x,y,char_color);
			x += 16;
			i++;
		}
	}
	setfillstyle(old_setting.pattern,old_setting.color);
}

