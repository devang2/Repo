//-------------------------------------------------------------
//                        XMS.CPP
//      The interface of XMS(eXtended Memory Specification)
// driver.This file include the class XMSDriver implement .
//          Write by TanWenHong 1996.12.20 Chen Du
//-------------------------------------------------------------
#ifndef __STDIO_H
	#include <stdio.h>
#endif
#ifndef _XMS_HPP
	#include "xms.hpp"
#endif

XMSDriver xms ;

static int XMBList[MAX_XMB_HANDLE];

int XMSDriver::AllocXmb(int sizeKb)
{
	int i=0;
	while(XMBList[i]!=0){
		i++;
		if(i>=MAX_XMB_HANDLE)	return 0;
	}
	XMBList[i]=alloc_xmem(sizeKb);
	return(XMBList[i]);
}

int XMSDriver::FreeXmb(int xmb_hd)
{
	for(int i = 0 ;i<MAX_XMB_HANDLE;i++){
		if(XMBList[i]==xmb_hd){
			XMBList[i]=0;
			break ;
		}
	}
	return(free_xmem(xmb_hd));
}

XMSDriver::~XMSDriver()
{
	for(int i=0;i<MAX_XMB_HANDLE;i++)
		if(XMBList[i])
			free_xmem(XMBList[i]);
}

XMSDriver::XMSDriver()
{
	if(!init_xms()){
		printf("XMS driver not present,please check your config.sys.");
	}else {
		version = xms_ver();
		printf("XMS version %d.%d\n",version>>8,version&0x000f);
		printf("Total free extended memory %dKb\n",totalfree_xmb());
		printf("Max block extended memory %dKb\n",maxfree_xmb());
	}
}

int XMSDriver::XmbMovetoCmb(int xmb_hd,long xmb_off,void far * dest,long size)
/* Move extended memory to conventional memory.
	NOTE:	The size of the memory block will be moved must times 2 bytes,
	otherwise this function will be fail. */
{
	XMOVE x;
	x.length = size ;
	x.sourceH = xmb_hd ;
	x.sourceoff = xmb_off ;
	x.destH = 0L ;
	x.destoff = Ptr2Long(dest);
	return move_xmem((XMOVE *)&x);
}

int XMSDriver::CmbMovetoXmb(void far * source,int xmb_hd,long xmb_off,long size)
/* Move conventional memory to extended memory .
	NOTE:	The size of the memory block will be moved must times 2 bytes,
	otherwise this function will be fail. */
{
	XMOVE x;
	x.length = size ;
	x.sourceH = 0 ;
	x.sourceoff = Ptr2Long(source) ;
	x.destH = xmb_hd ;
	x.destoff = xmb_off ;
	return move_xmem((XMOVE *)&x);
}

char * XMSDriver::ErrorMess(void)
/* This function return the error message if funtion of XMS
	runtime erron .*/
{
	int errno = xms_errno();
	switch(errno)
	{
	case 0x80 :
		return("No effect function");
	case 0x81 :
		return "Exist VDISK";
	case 0x82 :
		return "A20 bus error";
	case 0x8b :
		return "Invalid UMB segment address";
	case 0x94 :
		return "A20 bus can used";
	case 0x90 :
		return "HMA not exist";
	case 0x91 :
		return "HMA already alloc";
	case 0xa0 :
		return "All extended memory already allco";
	case 0xa1 :
		return "No extended handle can be used";
	case 0xa2 :
		return "Invalid XMB handle";
	case 0xa3 :
		return "Invalid XMB souce handle";
	case 0xa4 :
		return "Invalid XMB souce offset";
	case 0xa5 :
		return "Invalid XMB dest handle";
	case 0xa6 :
		return "Invalid XMB dest offset";
	case 0xa7 :
		return "Invalid XMB size";
	case 0xa8 :
		return "Invalid XMB move for covered";
	case 0xa9 :
		return "P/O check error";
	case 0xaa :
		return "XMB block no lock";
	case 0xab :
		return "XMB handle locked";
	case 0xac :
		return "XMB block locker counter overfloat";
	case 0xad :
		return "XMB lock fail";
	case 0xb0 :
		return "Valid UMB size smaller than needs";
	case 0xb1 :
		return "No UMB can use";
	}
	return 0;
}
