#ifndef _WINMAN_HPP
#define _WINMAN_HPP

#ifndef _WINDOWS_HPP
	#include <windows.hpp>
#endif

class WinManStack
{
	DbList winStack ;
public :
	void PushWin(Windows * win);
	void PopWin();
};

void WinManStack::PushWin(Windows * win)
{

