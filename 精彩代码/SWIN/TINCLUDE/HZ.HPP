#ifndef	_HZ_HPP
#define 	_HZ_HPP

#include "hzk.hpp"

#define TRUE		1
#define FALSE 		0

class HZ:public HZK
{
	int curx,cury;					//The current coord of display hz.
	int size;						//Size of hz character.
	int hj ;
	int zj ;
	char far * asc_8x16_fp;		//Point of 8x16 ascii character font.

public :
	HZ(const char * hzk="hzk16");
	int Getx(){return curx;}
	int Gety(){return cury;}
	void SetSize(int NewSize){size=NewSize;}
	void Outtext(char far *str);
	void Outtext(int x,int y,char far *str);

private :
	void Get_asc_8x16(void);
	void DispHz(int qh,int wh);
	void DispAsc(char asc);
};

inline HZ::HZ(const char * hzk):HZK(hzk)
{
	curx = cury = 0;
	size = 1;
	hj = 3 ;
	zj = 0 ;
	Get_asc_8x16();
}

inline void HZ::Outtext(int x,int y,char far *str)
{
	this->curx=x;
	this->cury=y;
	HZ::Outtext(str);
}

extern  HZ hz;

#endif	_HZ_HPP
