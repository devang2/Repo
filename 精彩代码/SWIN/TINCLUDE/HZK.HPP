//---------------------------------------------------------
//							HZK.HPP
//			Load the chinese font liberary to extended
//	memory or set read buffers of hzk .
//			Write by TanWenHong 1996.12.20 Chen Du
//---------------------------------------------------------
#ifndef _HZK_HPP
#define _HZK_HPP

#ifndef  _XMS_HPP
	#include "xms.hpp"
#endif

#ifndef	__STDIO_H
	#include <stdio.h>
#endif

#define	IN_XMS		1			//HZK put in extended memory
#define 	IN_HD			2			//HZK put in hard disk

class HZK
{
protected :
	char hzk_file[81] ;			//Filename of liberary.
	int putin ;						//Where the liberary loaded to.
										// 1 = put in extended memory
										// 2 = put in hard disk
	int xms_handle ;				//XMB hanle of hzk,if hzk had be loaded to XMS,
										//it get the XMB handle of hzk,else it get 0;
	FILE * hzk_fp ;				//File point of hzk,if hzk could't be load to
										//XMS it get the file point of hzk,else get NULL
private :
	int PutToXmb();				//Attemp to put hzk in XMS;
public :
	HZK(const char * hzk);
	~HZK();
	void GetFont16(void * hzfont,int& qm,int& wm);
};

#endif	//_HZK_HPP
