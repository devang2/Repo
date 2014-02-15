
/************************************************************************

    文件名:   to_prn.c
    功能:     打印报表
    日期:     1993年6月15日
    作者:     刘毅 费敏

************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <ctype.h>
#include <mem.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>
#include <def.inc>
#include <func.inc>

UC	prn_status;

/************************************************************************

	      UC init0_prn(void)
	      功能: 初始化打印机
	      入口: 无
	      出口: 无
	      返回: 打印机状态

************************************************************************/

UC	init0_prn(void)
{
	prn_status=prnf("%c%c",0x1b,0x40);				/*	initial	*/
	if	(prn_status)	prn_status=prnf("%c",0x11); /*	strobe	*/
	if	(prn_status)	prn_status=prnf("%c%c",0x1c,0x26);	/* chin_mode*/
	if	(prn_status)	prn_status=prnf("%c%c%c",0x1c,0x76,0x01);/*close*/
	if	(prn_status)	prn_status=prnf("%c%c%c",0x1b,0x55,0x01);/*one_dir*/
	return	prn_status;
}

/************************************************************************

	    void set_str(UC *flag, UC *dest, UC *str)
	      功能: 设置一个新串, 新串包括flag str的长度和str
	      入口: flag, str
	      出口: dest
	      返回: 无

************************************************************************/

void	set_str(UC	*flag,	UC	*dest,	UC	*str)
{
	UC	len,	slen[4];

	strcpy(dest,flag);
	strcat(dest," ");

	len=strlen(str);
	itoa(len,slen,10);

	strcat(dest,slen);
	strcat(dest," ");
	strcat(dest,str);

	strcat(dest,"\x0d\x0a");
}

/************************************************************************

	    void set_chr(UC *flag, UC *dest, UC chr)
	      功能: 设置一个新串, 新串包括flag '1' 和chr
	      入口: flag, chr
	      出口: dest
	      返回: 无

************************************************************************/

void	set_chr(UC	*flag,	UC	*dest,	UC	chr)
{
	UC	str[2];

	strcpy(dest,flag);
	strcat(dest," ");

	strcat(dest,"1");
	strcat(dest," ");

	str[0]=chr;
	str[1]='\0';
	strcat(dest,str);

	strcat(dest,"\x0d\x0a");
}

/************************************************************************

	    void set_val(UC *flag, UC *dest, UL val)
	      功能: 将val转换为串, 设置到新串中, 新串包括flag, val的串
                    长度, val的串
	      入口: flag, val
	      出口: dest
	      返回: 无

************************************************************************/

void	set_val(UC	*flag,	UC	*dest,	UL	val)
{
	UC	len,	str[32],	slen[4];

	strcpy(dest,flag);
	strcat(dest," ");

	ltoa(val,str,10);
	len=strlen(str);
	itoa(len,slen,10);

	strcat(dest,slen);
	strcat(dest," ");

	strcat(dest,str);
	strcat(dest,"\x0d\x0a");
}

/************************************************************************

	    void set_fee(UC *flag, UC *dest, UL fee)
	      功能: 将以分为单位的钱数转换为以元为单位的串, 设置到新串
                    中, 新串中包括flag, 转换的串长度和转换的串
	      入口: flag, fee,
	      出口: dest
	      返回: 无

************************************************************************/

void	set_fee(UC	*flag,	UC	*dest,	UL	val)
{
	UC	len,	str[32],	slen[4];

	strcpy(dest,flag);
	strcat(dest," ");

	ltoa(val/100,str,10);
	len=strlen(str);
	itoa(len+3,slen,10);

	strcat(dest,slen);
	strcat(dest," ");

	strcat(dest,str);
	strcat(dest,".");
	if ((val%100)>9)	ltoa(val%100,str,10);
	else
	{
		str[0]='0';
		ltoa(val%100,str+1,10);
	}
	strcat(dest,str);
	strcat(dest,"\x0d\x0a");
}


/************************************************************************

	    void prttab(UC *fdat, UC *ftab, UC resetprn)
	      功能: 由数据表文件和相应的表原型文件生成报表送打印机
	      入口: fdat数据表文件名指针,
                fdat原型表文件名指针
                line_distance打印机行距(0: 1/8英寸, 1: 1/6英寸)
	      出口: 无
	      返回: 无

************************************************************************/

void prttab(UC	*fdat,	UC	*ftab,	UC	line_distance)
{
FILE 	*fp_tab,*fp_dat;
UC far *data;
UC far *table;
UI length_tab;
UI length_dat;
UI i,j;
UC len,	flag[40],	*fptr;

	if ((fp_tab =fopen (ftab,"rb")) ==NULL)
	{
		printf("error\n");
		exit (0);
	}
	fseek(fp_tab,0L,SEEK_END);
	length_tab =(UI)ftell(fp_tab);
	table =(UC *)farmalloc (length_tab+2);

	if (table==NULL)
	{
		printf("heap is not enough\n");
		exit (1);
	}

	fseek (fp_tab,0L,SEEK_SET);
	fread (table,length_tab,1,fp_tab);
	fclose (fp_tab);

	if ((fp_dat =fopen (fdat,"rb")) ==NULL)
	{
		printf("error\n");
		exit (0);
	}
	fseek(fp_dat,0L,SEEK_END);
	length_dat =(UI)ftell(fp_dat);
	data =(UC *)farmalloc (length_dat+2);

	if (data==NULL)
	{
		printf("heap is not enough\n");
		exit (1);
	}

	fseek (fp_dat,0L,SEEK_SET);
	fread (data,length_dat,1,fp_dat);
	fclose (fp_dat);

	for (i=0; i<length_tab; i++)
	{
		if (table[i] == '@')
		{
			j=0;
			while (	(table[i+1+j]!=' ')&&(table[i+1+j]<0xa0)	)
			{
				flag[j]=table[i+1+j];
				j++;
			}
			flag[j]='\0';

			if	((fptr=strstr(data,flag))==NULL)
			{
				printf("flag %s isn't exist\n",flag);
				getch();
			}
			else
			{
				fptr=fptr+strlen(flag)+1;
				len=(*fptr)&0x0f;

				if (isdigit(*(fptr+1)))
				{
					len=len*10;
					len+=(*(fptr+1))&0x0f;
					fptr+=3;
					memmove(table+i,fptr,	len);
				}
				else
				{
					if	(len<3)
					{
						if (table[i+2]<0xa0)	memset(table+i,' ',3);
						else	memset(table+i,' ',2);
					}
					fptr+=2;
					memmove(table+i,fptr,len);
				}
			}
		}
	}
/*------------------------------------------*/
	prn_status=init0_prn();
	if	(prn_status)
		if (line_distance)
		{
			prn_status=prnf("%c%c",0x1b,0x32);	/*	1/6	inch per line	*/
		}
		else	prn_status=prnf("%c%c",0x1b,0x30);	/*	1/8	ipl	*/

	if	(prn_status)
	{
			i=0;
			while (i<length_tab)
			{
				if (table[i]==0x92)
				{
					if	(table[i+1]==0x80)
					{
						i+=2;
						if (prn_status)
						{   /*	set big	chinese	word*/
							prn_status=prnf("%c%c%c",0x1c,0x57,0x01);
						}
					}
					else	if	(table[i+1]==0x84)
					{
						i+=2;
						if (prn_status)
							prn_status = prnf("%c%c%c", 0x1c, 0x57, 0x00);
					}
				}
				if (prn_status)
					prn_status=prn_ch(table[i]);
				i++;
			}
	}

	if (prn_status)
	{
		prn_status=prnf("%c%c",0x1b,0x32);	/*	1/6	inch per line	*/
	}
/*------------------------------------------*/
	farfree (table);
	farfree (data);
}
