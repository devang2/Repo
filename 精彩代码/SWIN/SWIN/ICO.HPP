#ifndef _ICO_HPP
#define _ICO_HPP

struct IcoHeader
{
	char icoId[4];
	int width ;
	int height ;
};

void ShowIco(int x,int y,char * icofile);

#endif
