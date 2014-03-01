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
	DWORD   lpDIBitsRender;// ������ʾ����Ļ����������
	DWORD 	lpWave1;       // ˮ���������ݻ���1
	DWORD	lpWave2;       // ˮ���������ݻ���2
	//********************************************************************
	DWORD	dwBmpWidth;
	DWORD	dwBmpHeight;
	DWORD	dwDIByteWidth;// = (dwBmpWidth * 3 + 3) and ~3
	DWORD	dwWaveByteWidth;// = dwBmpWidth * 4
	DWORD	dwRandom;
	//********************************************************************
	// ��Ч����
	//********************************************************************
	DWORD	dwEffectType;
	DWORD	dwEffectParam1;
	DWORD	dwEffectParam2;
	DWORD	dwEffectParam3;
	//********************************************************************
	// �����д���Ч
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
