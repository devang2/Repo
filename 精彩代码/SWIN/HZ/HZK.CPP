//---------------------------------------------------------
//							HZK.CPP
//			Load the chinese font liberary to extended
//	memory or set read buffers of hzk .
//			Write by TanWenHong 1996.12.20 Chen Du
//---------------------------------------------------------
#ifndef	_XMS_HPP
	#include "xms.hpp"
#endif
#ifndef  _HZK_HPP
	#include "hzk.hpp"
#endif
#ifndef	__STDIO_H
	#include <stdio.h>
#endif
#ifndef  __STDLIB_H
	#include <stdlib.h>
#endif
#ifndef  __STRING_H
	#include <string.h>
#endif

HZK::HZK(const char * hzk)
{
	strcpy(hzk_file,hzk);
	HZK::xms_handle = PutToXmb();
	if(!HZK::xms_handle){
		if(!HZK::hzk_fp){
			printf("HZK load error %c\n",7);
			exit(0);
		} else HZK::putin = IN_HD ;
	}
}

HZK::~HZK()
{
	if(HZK::putin == IN_XMS)
		xms.FreeXmb(HZK::xms_handle);
	else if(HZK::putin == IN_HD){
		fclose(HZK::hzk_fp);
	}
}

int HZK::PutToXmb()
{
	HZK::hzk_fp = fopen(hzk_file,"rb");
	if(!HZK::hzk_fp){
		printf("Open file %s error\n",hzk_file);
		return 0;
	}
	char * buff = new char[1024];
	if(!buff){
		printf("Memory not enough.\n");
		return 0;
	}
	unsigned hzksize = 0;
	while(!feof(HZK::hzk_fp)){
		fread(buff,1,1024,HZK::hzk_fp);
		hzksize++;
	}
	HZK::xms_handle = xms.AllocXmb(hzksize);
	if(!HZK::xms_handle){
		printf("No enough XMB.\n");
		return 0;
	}
	fseek(HZK::hzk_fp,0L,SEEK_SET);
	printf("Loading Chinese font to XMS\n");
	for(unsigned long i=0;i<hzksize;i++){
		fread(buff,1,1024,HZK::hzk_fp);
		if(!(i%20))	putchar('.');
		if(!xms.CmbMovetoXmb(buff,HZK::xms_handle,i*1024,1024L)){
			printf("Load hzk to XMB fail.\n");
			printf("Error :%s.\n",xms.ErrorMess());
			xms.FreeXmb(HZK::xms_handle);
			return 0;
		}
	}

	HZK::putin = IN_XMS ;
	delete buff ;
	fclose(HZK::hzk_fp);
	return HZK::xms_handle ;
}

void HZK::GetFont16(void * hz_font,int& qm,int& wm)
{
	unsigned long hz_off=(unsigned long)(qm*94+wm)*32;
	if(HZK::putin == IN_XMS)
		xms.XmbMovetoCmb(HZK::xms_handle,hz_off,hz_font,32);
	else if(HZK::putin == IN_HD){
			fseek(HZK::hzk_fp,hz_off,SEEK_SET);
			fread(hz_font,32,1,hzk_fp);
	}
}
