/*****************************************************************************************
ģ �� ����Wave.h
˵    ����ˮ��Ч��ʵ�ֺ�����ͷ�ļ�
�� �� �ˣ�ħ��
��    ����V1.0.0
*****************************************************************************************/

#include <windows.h>

#define	F_WO_ACTIVE	0X0001
#define	F_WO_NEED_UPDATE	0x0002
#define	F_WO_EFFECT	0x0004
#define	F_WO_ELLIPSE	0x0008

typedef struct _WAVEOBJECT
{
	HWND hWnd;
	UINT dwFlag;			//�� F_WO_xxx ���
	HDC hDcRender;
	HBITMAP hBmpRender;
	PBYTE lpDIBitsSource;
	PBYTE   lpDIBitsRender;	// ������ʾ����Ļ����������
	int * lpWave1;			// ˮ���������ݻ���1
	int * lpWave2;			// ˮ���������ݻ���2

	int dwBmpWidth;
	int dwBmpHeight;
	UINT dwDIByteWidth;		// = (dwBmpWidth * 3 + 3) and ~3
	UINT dwWaveByteWidth;	// = dwBmpWidth * 4
	UINT dwRandom;

	// ��Ч����
	UINT dwEffectType;
	int dwEffectParam1;
	int dwEffectParam2;
	int dwEffectParam3;

	// �����д���Ч
	int dwEff2X;
	int dwEff2Y;
	int dwEff2XAdd;
	int dwEff2YAdd;
	int dwEff2Flip;
	//********************************************************************
	BITMAPINFO stBmpInfo;
} WAVEOBJECT,*LPWAVEOBJECT;

extern "C" UINT __stdcall WaveInit(LPWAVEOBJECT lpWaveObject, HWND hWnd, HBITMAP hBmp, UINT dwSpeed, UINT dwType);

extern "C" void __stdcall WaveDropStone(LPWAVEOBJECT lpWaveObject, int dwPosX, int dwPosY, int dwStoneSize,int dwStoneWeight);

extern "C" void __stdcall WaveUpdateFrame(LPWAVEOBJECT lpWaveObject, HDC hDc, BOOL bIfForce);

extern "C" void __stdcall WaveFree(LPWAVEOBJECT lpWaveObject);

extern "C" void __stdcall WaveEffect(LPWAVEOBJECT lpWaveObject, UINT dwType,int dwParam1,int dwParam2,int dwParam3);

