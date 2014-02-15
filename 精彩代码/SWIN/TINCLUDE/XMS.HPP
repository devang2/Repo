#ifndef	_XMS_HPP
#define	_XMS_HPP


#define MAX_XMB_HANDLE		30

//------- The struct of XMB to move -------------
typedef struct
{
	long length;					//The length of memory block be moved ,
										//must be times of 2 .
	unsigned int sourceH;		//Source memory block handle,it equate 0
										//if the source memory block in conventional memory.
	long sourceoff;				//Source memory block offset.
	unsigned int destH;			//Dest memory block handle,it equate 0
                              //if the dest memory block in conventional memory.
	long destoff;					//The dest memory block offset.
}XMOVE;


//--------- In XMSA.ASM ------------
#ifdef __cplusplus
extern "C" {
#endif
	int far init_xms(void);
	int far xms_ver(void);
	int far xms_errno(void);
	int far free_xmem(int handle);
	int far alloc_xmem(int KBytes);
	int far move_xmem(XMOVE * p);
	long far Ptr2Long(void far * p);
	void far * far alloc_umb(int Per16byte);
	int far free_umb(void far * umb_prt);
	int far maxfree_xmb(void);
	int far totalfree_xmb(void);
#ifdef __cplusplus
}
#endif

//--------- C L A S S  XMSDriver -----------------

class XMSDriver
{
	int version ;
public :
	XMSDriver();
	~XMSDriver();
	int AllocXmb(int sizeKb);
	int FreeXmb(int xms_hd);
	int XmbMovetoCmb(int xmb_hd,long xmb_off,void far * dest,long size);
	int CmbMovetoXmb(void far * sourc,int xmb_hd,long xmb_off,long size);
	void far * AllocUmb(int size16Byte);
	int FreeUmb(void far * umb_ptr);
	char * ErrorMess(void);
};

inline void far * XMSDriver::AllocUmb(int size16Bytes)
{
	return(alloc_umb(size16Bytes));
}

inline int XMSDriver::FreeUmb(void far * umb_ptr)
{
	return(free_umb(umb_ptr));
}

extern XMSDriver xms;
#endif
