#pragma once
#include "stdafx.h"

#define F_WO_ACTIVE	0001h
#define F_WO_NEED_UPDATE 0002h
#define F_WO_EFFECT	0004h
#define F_WO_ELLIPSE 0008h

typedef struct _WAVE_OBJECT
{
	HWND hWnd;
	DWORD dwFlag;
	DWORD	hDcRender;
	DWORD	hBmpRender;
	DWORD	lpDIBitsSource;
	DWORD   lpDIBitsRender;// 用于显示到屏幕的像素数据
	DWORD 	lpWave1;       // 水波能量数据缓冲1
	DWORD	lpWave2;       // 水波能量数据缓冲2
	//********************************************************************
	DWORD	dwBmpWidth;
	DWORD	dwBmpHeight;
	DWORD	dwDIByteWidth;// = (dwBmpWidth * 3 + 3) and ~3
	DWORD	dwWaveByteWidth;// = dwBmpWidth * 4
	DWORD	dwRandom;
	//********************************************************************
	// 特效参数
	//********************************************************************
	DWORD	dwEffectType;
	DWORD	dwEffectParam1;
	DWORD	dwEffectParam2;
	DWORD	dwEffectParam3;
	//********************************************************************
	// 用于行船特效
	//********************************************************************
	DWORD	dwEff2X;
	DWORD	dwEff2Y;
	DWORD	dwEff2XAdd;
	DWORD	dwEff2YAdd;
	DWORD	dwEff2Flip;
	//********************************************************************
	BITMAPINFO	stBmpInfo;// BITMAPINFO <>
} WAVE_OBJECT,*LPWAVE_OBJECT;

extern "C" void __stdcall xWaveInit(LPWAVE_OBJECT lpWaveObject,HWND hWnd,HBITMAP hBmp,DWORD dwSpeed,DWORD dwType);
extern "C" void __stdcall xWaveDropStone(LPWAVE_OBJECT lpWaveObject,DWORD dwPosX,DWORD dwPosY,DWORD dwStoneSize,DWORD dwStoneWeight);
extern "C" void __stdcall xWaveUpdateFrame(LPWAVE_OBJECT lpWaveObject,HDC hDc,DWORD bIfForce);
extern "C" void __stdcall xWaveFree(LPWAVE_OBJECT lpWaveObject);
extern "C" void __stdcall xWaveEffect(LPWAVE_OBJECT lpWaveObject,DWORD dwType,DWORD dwParam1,DWORD dwParam2,DWORD dwParam3);
