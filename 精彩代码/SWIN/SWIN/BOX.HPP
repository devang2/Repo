/*-------------------------------------------------------------
							BOX.HPP
		This file clarm Box class baseed on Object class.
			Write by TanWenHong 1996.12.27 Cheng Du
-------------------------------------------------------------*/
#ifndef _BOX_HPP
#define _BOX_HPP

#ifndef  _EVENT_HPP
	#include "event.hpp"
#endif

#define	CLICK_ONLY	3
#define	RUNIT			1
#define  OK				1

#define  CLICKED 		1
#define	NOCLICK		0

#define  NORUN 		0

#define	EXIT			-1
#define  CANCEL		-2
#define  COMMENT		-3

enum Bool {false = 0,true = 1};

class Box
{
private :
	int objno ;
protected :
	Bool active ;
	int exitcode ;
	int wx , wy , ww , wh;		//Box region coording.
public :
	Bool disable ;
//------------------------------------------------------------------------
public :
	Box(int NewX,int NewY,int NewW,int NewH)
	{ 	disable = active = false ; objno = exitcode = 0 ;
		wx = NewX ; wy = NewY; ww = NewW ; wh = NewH ;}
//-----------------------------------------------------------------------
	virtual void Show(){};
	virtual int Run(){return exitcode;}	//If this function return -1,will quit this
											//Object grouph.
	virtual int Clicked(){return NOCLICK;} ;
											//Tell event present on this object or not.
											//0:Event no present in it.
											//1:Event present and run this object
											//2:mouse clicked in but not run this object
	virtual void Active(Bool status){active = status ;} ;	//Active/Deactive this object.
	virtual void Disable(Bool status){disable = status ;} ;//This object disable or not.
	virtual void Setxy(int x,int y){ wx = x ; wy = y ;}
//--------------------------------------------------------------
	int MouseIn();
	int ExitCode(){return exitcode;}
	int x(){return wx;}
	int y(){return wy;}
	int w(){return ww;}
	int h(){return wh;}
	void Setw(int w){ww = w ;}
	void Seth(int h){wh = h ;}
//---------------------------------------------------------
	friend DbList ;
};
#endif