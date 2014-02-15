/*-----------------------------------------------------------
						OBJECT.HPP
		This file clarm Object class,it's importance.
			Write by TanWenHong	1996.12.27 Cheng Du
-----------------------------------------------------------*/
#ifndef _OBJECT_HPP
#define _OBJECT_HPP

#define	CLICK_ONLY	3
#define	RUNIT			1
#define  OK				1

#define	NOCLICK		0

#define	EXIT			-1
#define  CANCEL		-2

enum Bool {false = 0,true = 1};

class Object
{
public :
	Bool disable ;
private :
	int objno ;
protected :
	Bool active ;
public :
	Object(){ active = false ;disable = false ; objno = 0 ;};
	virtual void Show(){};
	virtual int Run(){return 0;}	//If this function return -1,will quit this
											//Object grouph.
	virtual int Clicked(){return 0;} ;
											//Tell event present on this object or not.
											//0:Event no present in it.
											//1:Event present and run this object
											//2:mouse clicked in but not run this object
	virtual void Active(Bool status){} ;	//Active/Deactive this object.
	virtual void Disable(Bool status){} ;//This object disable or not.
	Bool IsActive(void){ return active ;}	//This object actived or not.

	friend DbList ;
};

#endif