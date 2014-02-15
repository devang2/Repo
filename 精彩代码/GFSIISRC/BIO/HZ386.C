#include <io.h>
#include <dos.h>
#include <string.h>
#include <fcntl.h>
#include <alloc.h>
#include <graphics.h>

#include <def.inc>
#include <func.inc>

static UC far *Rom_char_lib;

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

/* ============================= load_hz_lib ============================= */
void load_hzk(UC hz_style)
{
UC * tmp_buf;
UI tmp_handle;
UI bytes;
UC loopi;

	if (EM_amount()<256/*512*/)
		exit_scr(1, "\n\rHZ.C/load_hzk(): Not enough extened memory.");

	if (hz_style == 1)
		tmp_handle = open("DATA\\CCLIBJ.DOT",O_BINARY|O_RDONLY);
	else
		tmp_handle = open("DATA\\CCLIBF.DOT",O_BINARY|O_RDONLY);

	if (tmp_handle==0xffff)
		exit_scr(1, "\n\rHZ.C/load_hzk(): Can't open file CCLIB.DOT.");

	tmp_buf = (UC *)mem_alloc(65536L);

	bytes = 10*94*32;
	for (loopi=0; loopi<8; loopi++)
	{
		if (read(tmp_handle, (UC *)tmp_buf, bytes)==-1)
		{
		    farfree(tmp_buf);
		    exit_scr(1, "\n\rHZ.C/load_hzk(): Read file DATA\\CCLIB.DOT error.");
		}

		if (EM_write(0x100000L+(UL)bytes*loopi, tmp_buf, bytes)==FALSE)
		{
		    farfree(tmp_buf);
		    exit_scr(1, "\n\rHZ.C/load_hzk(): Extened memory access error.");
		 }
	}

	bytes = 94*32;
	if (read(tmp_handle, (UC *)tmp_buf, bytes)==-1)
	{
	    farfree(tmp_buf);
	    exit_scr(1, "\n\rHZ.C/load_hzk(): Read file DATA\\CCLIB.DOT error.");
	 }

	if (EM_write(0x100000L+(UL)bytes*80L, tmp_buf, bytes)==FALSE)
	{
	    farfree(tmp_buf);
	    exit_scr(1, "\n\rHZ.C/load_hzk(): Extened memory access error.");
	 }

	farfree(tmp_buf);
	close(tmp_handle);
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

/* ============================== hz_16 ================================ */
void hz_16(UC qh, UC wz, UI pox, UI poy, UC color)
{
	UC i,k;
	UC dot_byte;
	UC hz_dot[32];
	static UC bit_map[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	if (qh==0 || wz==0) return;
	if (qh>87 || wz>94) return;
	if (qh>9  && qh<16) return;
	if ( qh>15 ) qh -= 6;

	EM_read(hz_dot, 0x100000L+(--qh)*94*32L+(--wz)*32L, 32);

	set_dot_mode(color);
	for (i=0;i<32;i++)
	{
		dot_byte = hz_dot[i];
		for (k=0;k<8;k++)
		{
			if ((dot_byte & bit_map[k]) != 0)
				pixel(pox,poy);
			pox++;
		}
		if ( i&0x01 )
		{
			poy++;
			pox -= 16;
		}
	}
	reset_dot_mode();
}

/* ============================= char_16 ================================ */
void char_16(UC asc_code, UI pox, UI poy, UC color)
{
UC i,k,dot_byte;
UC far *dot_here;
static UC bit_map[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	if (asc_code>127) return;
	dot_here = Rom_char_lib+(asc_code<<4);
	set_dot_mode(color);
	for (i=0;i<16;i++)
	{
		dot_byte = *(dot_here++);
		for (k=0;k<8;k++)
		{
			if ((dot_byte & bit_map[k]) != 0)
				pixel(pox,poy);
			pox++;
		}
		poy++;
		pox-=8;
	}
	reset_dot_mode();
}

/* ============================= hz16_disp =============================== */
void hz16_disp(UI x, UI y, UC *hz_str, UC color)
{
UC han1, han2, i;

	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			char_16(han1, x, y+1, color);
			x += 8;
			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160)
				return;
			hz_16((han1-160),(han2-160),x,y,color);
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

/* ============================== disp_str ============================== */
void disp_str(UI x, UI y, UC *hz_str, UC bk_color, UC char_color)
{
UC han1,han2;
UI i;
struct fillsettingstype old_setting;

	i = strlen(hz_str);
	if (i==0 || i>=80) return;

	getfillsettings(&old_setting);
	setfillstyle(SOLID_FILL,bk_color);
	bar(x, y, x+(i<<3)-1, y+15);

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

	EM_read(hz_dot, 0x100000L+(--qh)*94*32L+(--wz)*32L, 32);

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
