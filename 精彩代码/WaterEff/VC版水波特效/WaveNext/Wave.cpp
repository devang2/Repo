/*****************************************************************************************
ģ �� ����Wave.cpp
˵    ����ˮ��Ч��ʵ�ֺ���
�� �� �ˣ�ħ��
��    ����Wave VC�汾�������Ʊ�Ļ������д,
	ʵ��ˮ����Ч�Ĵ������ӣ��ο��˲������ϴ��룬�ر�ο������ְ����
	VB�汾
��    ����V1.0.0
*****************************************************************************************/

/*****************************************************************************************
 ��Դ��������Ҫ include Wave.h
 Ȼ�����²�����ã�
********************************************************************
1������ˮ������
Ҫ��һ�����ڽ��л滭������Ҫ����һ��ˮ�����󣨱���������һЩ��������
WaveInit(LPWAVEOBJECT lpWaveObject, HWND hWnd, HBITMAP hBmp, UINT dwSpeed, UINT dwType)
	lpWaveObject --> ָ��һ���յ� WAVE_OBJECT �ṹ
	hWnd --> Ҫ�滭ˮ��Ч���Ĵ��ڣ���Ⱦ���ͼƬ���������ڵĿͻ�����
	hBmp --> ����ͼƬ���滭�ķ�Χ��Сͬ����ͼƬ��С
	dwTime --> ˢ�µ�ʱ���������룩������ֵ��10��30
	dwType --> =0 ��ʾԲ��ˮ����=1��ʾ��Բ��ˮ��������͸��Ч����
	����ֵ��eax = 0���ɹ������󱻳�ʼ������eax = 1��ʧ�ܣ�
********************************************************************
 2����� WaveInit �������سɹ�������󱻳�ʼ���������󴫸����и��ֺ���
 ����ʵ�ָ���Ч�������溯���е� lpWaveObject ����ָ���� WaveInit ����
 �г�ʼ���� WAVEOBJECT �ṹ��

	(1) ��ָ��λ�á���ʯͷ��������ˮ��
	WaveDropStone(LPWAVEOBJECT lpWaveObject, int dwPosX, int dwPosY, int dwStoneSize,int dwStoneWeight);
		dwPosX,dwPosY --> ����ʯͷ��λ��
		dwStoneSize --> ʯͷ�Ĵ�С������ʼ��Ĵ�С������ֵ��0��5
		dwStoneWeight --> ʯͷ�������������˲������ɢ�ķ�Χ��С������ֵ��10��1000

	(2) �Զ���ʾ��Ч
	WaveEffect(LPWAVEOBJECT lpWaveObject, UINT dwType,int dwParam1,int dwParam2,int dwParam3)
		dwParam1,dwParam2,dwParam3 --> Ч���������Բ�ͬ����Ч���Ͳ������岻ͬ
		dwEffectType --> ��Ч����
		0 --> �ر���Ч
		1 --> ���꣬Param1���ܼ��ٶȣ�0���ܣ�Խ��Խϡ�裩������ֵ��0��30
			Param2��������ֱ��������ֵ��0��5
                        Param3������������������ֵ��50��250
		2 --> ��ͧ��Param1���ٶȣ�0������Խ��Խ�죩������ֵ��0��8
                        Param2������С������ֵ��0��4
                        Param3��ˮ����ɢ�ķ�Χ������ֵ��100��500
		3 --> ���ˣ�Param1���ܶȣ�Խ��Խ�ܣ�������ֵ��50��300
                        Param2����С������ֵ��2��5
                        Param3������������ֵ��5��10

	(3) ���ڿͻ���ǿ�Ƹ��£������ڴ��ڵ�WM_PAINT��Ϣ��ǿ�Ƹ��¿ͻ��ˣ�
	WaveUpdateFrame(LPWAVEOBJECT lpWaveObject, HDC hDc, BOOL bIfForce);
********************************************************************
 3���ͷ�ˮ������
ʹ����Ϻ󣬱��뽫ˮ�������ͷţ����������ͷ�����Ļ������ڴ����Դ��
	WaveFree(LPWAVEOBJECT lpWaveObject)
	lpWaveObject --> ָ�� WAVE_OBJECT �ṹ

ʵ���ϵ�ϸ��˵����

1�� ˮ����������
   (1) ��ɢ��ÿһ��Ĳ�����ɢ�������ܵ�λ����
   (2) ˥����ÿ����ɢ����ʧ��������������ˮ��������ֹͣ������ȥ��

2�� Ϊ�˱�������ʱ���е������ֲ�ͼ�������ж���2��������Wave1��Wave2
��������lpWave1��lpWave2ָ��Ļ������ڣ���Wave1Ϊ��ǰ���ݣ�Wave2
Ϊ��һ֡�����ݣ�ÿ����Ⱦʱ�����������2����������Wave1�����ݼ������
�����ֱ�ͼ�󣬱��浽Wave2�У�Ȼ�����Wave1��Wave2����ʱWave1����
���µ����ݡ�
	����ķ���Ϊ��ĳ����������������ܵ��ϴ�������ƽ��ֵ * ˥��ϵ��ȡ��
�ܵ�ƽ��ֵ��������չ����������˥��ϵ��������˥��������
      �ⲿ�ִ�����WaveSpread ������ʵ�֡�

 3�� ������ lpDIBitsSource �б�����ԭʼλͼ�����ݣ�ÿ����Ⱦʱ����ԭʼλ
 ͼ�����ݸ���Wave1�б���������ֲ����ݲ����µ�λͼ�����Ӿ��Ͽ���ĳ����
 ������Խ��ˮ��Խ�󣩣�����������ԽԶ���ĳ�����
	�㷨Ϊ�����ڵ㣨x,y������Wave1���ҳ��õ㣬��������ڵ�Ĳ��ܲ�
��Dx��Dy�������ݣ�������λͼ���أ�x,y����ԭʼλͼ���أ�x+Dx,y+Dy������
�㷨������������СӰ�������������ƫ�ƴ�С��
      �ⲿ�ִ����� WaveRender �ӳ�����ʵ�֡�

 4�� ��ʯͷ���㷨�ܺ���⣬����Wave1�е�ĳ���������ֵ��Ϊ��0ֵ����ֵԽ
 �󣬱�ʾ���µ�ʯͷ������Խ��ʯͷ�Ƚϴ��򽫸õ����ܵĵ�ȫ����Ϊ��0ֵ��

	�����򲹳�����ϼ������ļ�resource.txt��
	�����еĻ���������һά���飬ע��Ϊ�˷�����ö�ά���顣��Ȼ����������
�ö�ά�������������⡣
*****************************************************************************************/

#include "Wave.h"
#include "targetver.h"

/*****************************************************************************************
����������ӳ���
 ���룺Ҫ����������������ֵ������������
 ���ݣ�
 1. ��ѧ��ʽ Rnd=(Rnd*I+J) mod K ѭ���ش����� K �����ڲ��ظ���
    α���������K,I,J����Ϊ����
 2. 2^(2n-1)-1 �ض�Ϊ��������2�������η���1��
*****************************************************************************************/
DWORD WaveRandom16 (LPWAVEOBJECT lpWaveObject)
{
	DWORD dwRnd;
	dwRnd = ((lpWaveObject->dwRandom & 0xFFFF) * 0x7FF + 0x7F) % 0x7FFF;
	lpWaveObject->dwRandom = dwRnd;
	return (dwRnd & 0xFF);
}

DWORD WaveRandom (LPWAVEOBJECT lpWaveObject, DWORD dwMax)
{
	DWORD lRnd1, lRnd2, lRnd;
	lRnd1 = WaveRandom16 (lpWaveObject);
	lRnd2 = WaveRandom16 (lpWaveObject);

	if (dwMax == 0)
		lRnd = 0;
	else
		lRnd = (((lRnd2<<16)+lRnd1) % dwMax);

	return lRnd;
	
}

/*****************************************************************************************
 ������ɢ
 �㷨��
��Բ��ɢ
 Wave2(x,y) = ((Wave1(x+1,y)+Wave1(x-1,y)+Wave1(x+2,y)+Wave1(x-2,y))*3+Wave1(x+3,y)*2+Wave1(x-3,y)*2+ Wave1(x,y+1)*8+Wave1(x,y-1)*8)\16-Wave2(x,y)
Բ����ɢ
 Wave2(x,y) = (Wave1(x+1,y)+Wave1(x-1,y)+Wave1(x,y+1)+Wave1(x,y-1))\2-Wave2(x,y)
 Wave2(x,y) = Wave2(x,y) - Wave2(x,y) >> 5
 ���� Wave1,Wave2
*****************************************************************************************/
void WaveSpread (LPWAVEOBJECT lpWaveObject)
{
	PINT lpWave1, lpWave2;
	int dwBmpWidth, dwBmpHeight;
	int i;

	if ((lpWaveObject->dwFlag & F_WO_ACTIVE) == 0)
		return;
	
	lpWave1 = lpWaveObject->lpWave1;
	lpWave2 = lpWaveObject->lpWave2;
	dwBmpWidth = lpWaveObject->dwBmpWidth;
	dwBmpHeight = lpWaveObject->dwBmpHeight;

	for (i = dwBmpWidth; i < (dwBmpHeight-1)*dwBmpWidth; i++) {
		if (lpWaveObject->dwFlag & F_WO_ELLIPSE) {//��Բ��ɢ
			lpWave2[i] = ((lpWave1[i+1] + lpWave1[i-1] + lpWave1[i+2] + lpWave1[i-2])*3 + (lpWave1[i+3] + lpWave1[i-3])*2 + (lpWave1[i+dwBmpWidth] + lpWave1[i-dwBmpWidth])*8)/16 - lpWave2[i];
		} else {//Բ����ɢ
			lpWave2[i] = (lpWave1[i+1] + lpWave1[i-1] + lpWave1[i+dwBmpWidth] + lpWave1[i-dwBmpWidth])/2 - lpWave2[i];
		}
		if (lpWave2[i] == 0) lpWave2[i] = 0;
		lpWave2[i] -= (lpWave2[i] >> 5);
	}

	lpWaveObject->lpWave1 = lpWave2;
	lpWaveObject->lpWave2 = lpWave1;
}

/*****************************************************************************************
' ��Ⱦ���������µ�֡������Ⱦ�� lpDIBitsRender ��
 �㷨��
 posx = Wave1(x-1,y)-Wave1(x+1,y)+x
 posy = Wave1(x,y-1)-Wave1(x,y+1)+y
 SourceBmp(x,y) = DestBmp(posx,posy)
*****************************************************************************************/
void WaveRender (LPWAVEOBJECT lpWaveObject)
{
	int dwPosX, dwPosY, dwPtrSource, dwPtrDest;
	UINT dwFlag;
	PINT lpWave1;
	UINT LineIdx = lpWaveObject->dwBmpWidth;
	int dwBmpWidth = lpWaveObject->dwBmpWidth;
	PBYTE DIBitsSource = NULL;
	PBYTE DIBitsRender = NULL;
	int i, j, k;

	dwFlag = 0;
	if ((lpWaveObject->dwFlag & F_WO_ACTIVE) == 0) 
		return;
	lpWaveObject->dwFlag |= F_WO_NEED_UPDATE;
	lpWave1 = lpWaveObject->lpWave1;
	DIBitsSource = lpWaveObject->lpDIBitsSource;
	DIBitsRender = lpWaveObject->lpDIBitsRender;
	for (i = 1; i < lpWaveObject->dwBmpHeight-1; i++) {
		for (j = 0; j < lpWaveObject->dwBmpWidth; j++) {
			//����ƫ����
			//PosY=i+������1����-������1����
			//PosX=j+������1����-������1����
			dwPosY = i + lpWave1[LineIdx-dwBmpWidth] - lpWave1[LineIdx+dwBmpWidth];
			dwPosX = j + lpWave1[LineIdx-1] - lpWave1[LineIdx+1];

			//�ж������Ƿ��ڴ��ڷ�Χ��
			if (dwPosX < 0 || dwPosY < 0) goto Continue;
			if (dwPosX >= lpWaveObject->dwBmpWidth || dwPosY >= lpWaveObject->dwBmpHeight) goto Continue;

			//�����ƫ�����غ�ԭʼ���ص��ڴ��ַƫ����
			dwPtrSource = dwPosY*lpWaveObject->dwDIByteWidth + dwPosX*3;
			dwPtrDest = i*lpWaveObject->dwDIByteWidth + j*3;

			//��Ⱦ���� [ptrDest] = ԭʼ���� [ptrSource]
			if (dwPtrSource != dwPtrDest) {
				dwFlag |= 1;	//�������Դ���غ�Ŀ�����ز�ͬ�����ʾ���ڻ״̬
				for (k = 0; k < 3; k++) {
					DIBitsRender[dwPtrDest+k] = DIBitsSource[dwPtrSource+k];
					//DIBitsRender[dwPtrDest+k] = (DIBitsSource[dwPtrSource+k]*4 + (DIBitsSource[dwPtrSource+k-1] + 
					//	DIBitsSource[dwPtrSource+k+1] + DIBitsSource[dwPtrSource+k-lpWaveObject->dwDIByteWidth] +
					//	DIBitsSource[dwPtrSource+k+lpWaveObject->dwDIByteWidth])*3)/16;
				}
			} else {
				for (k = 0; k < 3; k++) {
					DIBitsRender[dwPtrDest+k] = DIBitsSource[dwPtrSource+k];
				}
			}

		Continue:
			LineIdx++;
		}
	}
	SetDIBits (lpWaveObject->hDcRender, lpWaveObject->hBmpRender, 0, lpWaveObject->dwBmpHeight, lpWaveObject->lpDIBitsRender, &lpWaveObject->stBmpInfo, DIB_RGB_COLORS);
	if (dwFlag = 0)
		lpWaveObject->dwFlag &= ~F_WO_ACTIVE;
}

//��һ��ʯͷ
void __stdcall WaveDropStone (LPWAVEOBJECT lpWaveObject, int dwX, int dwY, int dwStoneSize, int dwStoneWeight)
{
	int dwSize, dwX1, dwY1, dwX2, dwY2, dwY2_t;
	PINT lpWave1;

	//���㷶Χ
	dwSize = dwStoneSize / 2;
	dwX1 = dwX + dwSize;
	dwX2 = dwX - dwSize;
	if (lpWaveObject->dwFlag & F_WO_ELLIPSE) dwSize /= 2;
	dwY1 = dwY + dwSize;
	dwY2 = dwY - dwSize;
	dwSize = dwStoneSize;
	if (dwSize == 0) dwSize = 1;
	
	//�жϷ�Χ�ĺϷ���
	if (dwX1+1 >= lpWaveObject->dwBmpWidth || dwX2 < 1 || dwY1+1 >= lpWaveObject->dwBmpHeight || dwY2 < 1) return;

	//����Χ�ڵĵ��������Ϊ dwStoneWeight
	while (dwX2 <= dwX1) {
		dwY2_t = dwY2;
		while (dwY2 < dwY1) {
			if (((dwX2-dwX)*(dwX2-dwX) + (dwY2-dwY)*(dwY2-dwY))
				<= dwSize*dwSize) {
			lpWave1 = lpWaveObject->lpWave1 + (dwY2*lpWaveObject->dwBmpWidth + dwX2);
			*lpWave1 = dwStoneWeight;
			}
			dwY2++; 
		}
		dwX2++; dwY2 = dwY2_t;
	}
	lpWaveObject->dwFlag |= F_WO_ACTIVE;
}

void __stdcall  WaveUpdateFrame (LPWAVEOBJECT lpWaveObject, HDC hDc, BOOL bIfForce)
{
	if (bIfForce || (lpWaveObject->dwFlag & F_WO_NEED_UPDATE)) {
		BitBlt (hDc, 0, 0, lpWaveObject->dwBmpWidth, lpWaveObject->dwBmpHeight, lpWaveObject->hDcRender, 0, 0, SRCCOPY);
		lpWaveObject->dwFlag &= ~F_WO_NEED_UPDATE;
	}
}

//������ɢ���ݡ���Ⱦλͼ�����´��ڡ�������Ч�Ķ�ʱ������
void CALLBACK WaveTimerProc (HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	HDC hDc;
	int dwX, dwY, dwSize, dwWeight;
	int i;

	LPWAVEOBJECT lpWaveObject = (LPWAVEOBJECT) idEvent;
	WaveSpread (lpWaveObject);
	WaveRender (lpWaveObject);
	if (lpWaveObject->dwFlag & F_WO_NEED_UPDATE) {
		hDc = GetDC (lpWaveObject->hWnd);
		WaveUpdateFrame (lpWaveObject, hDc, FALSE);
		ReleaseDC (lpWaveObject->hWnd, hDc);
	}

	//��Ч����
	if ((lpWaveObject->dwFlag & F_WO_EFFECT) == 0)
		return;
	switch (lpWaveObject->dwEffectType) {
	case 1:	//Type = 1 ��㣬Param1���ٶȣ�0��죬Խ��Խ������Param2������С��Param3������
		if (!lpWaveObject->dwEffectParam1 || !WaveRandom (lpWaveObject, lpWaveObject->dwEffectParam1)) {
			dwX = WaveRandom(lpWaveObject, lpWaveObject->dwBmpWidth-2) + 1;
			dwY = WaveRandom (lpWaveObject, lpWaveObject->dwBmpHeight-2) + 1;
			dwSize = WaveRandom (lpWaveObject, lpWaveObject->dwEffectParam2) + 1;			
			dwWeight = WaveRandom (lpWaveObject, lpWaveObject->dwEffectParam3) + 50;
			WaveDropStone (lpWaveObject, dwX, dwY, dwSize, dwWeight);
		}
		break;
	case 2:	//Type = 2 �д���Param1���ٶȣ�0��죬Խ��Խ�죩��Param2����С��Param3������
		lpWaveObject->dwEff2Flip++;
		if ((lpWaveObject->dwEff2Flip & 1) != 0) return;
		dwX = lpWaveObject->dwEff2X + lpWaveObject->dwEff2XAdd;
		dwY = lpWaveObject->dwEff2Y + lpWaveObject->dwEff2YAdd;
		if (dwX < 1) {
			dwX = -(dwX-1);
			lpWaveObject->dwEff2XAdd = -lpWaveObject->dwEff2XAdd;
		}
		if (dwY < 1) {
			dwY = -(dwY-1);
			lpWaveObject->dwEff2YAdd = -lpWaveObject->dwEff2YAdd;
		}
		if (dwX > lpWaveObject->dwBmpWidth-1) {
			dwX = (lpWaveObject->dwBmpWidth-1)*2 - dwX;
			lpWaveObject->dwEff2XAdd = -lpWaveObject->dwEff2XAdd;
		}
		if (dwY > lpWaveObject->dwBmpHeight-1) {
			dwY = (lpWaveObject->dwBmpHeight-1)*2 - dwY;
			lpWaveObject->dwEff2YAdd = -lpWaveObject->dwEff2YAdd;
		}
		lpWaveObject->dwEff2X = dwX;
		lpWaveObject->dwEff2Y = dwY;
		WaveDropStone (lpWaveObject, dwX, dwY, lpWaveObject->dwEffectParam2, lpWaveObject->dwEffectParam3);
		break;
	case 3:	//Type = 3 ���ˣ�Param1���ܶȣ�Param2����С��Param3������
		for (i = 0; i <= lpWaveObject->dwEffectParam1; i++) {
		dwX = WaveRandom (lpWaveObject, lpWaveObject->dwBmpWidth-2) + 1;
		dwY = WaveRandom (lpWaveObject, lpWaveObject->dwBmpHeight-2) + 1;
		dwSize = WaveRandom (lpWaveObject, lpWaveObject->dwEffectParam2) + 1;
		dwWeight = WaveRandom (lpWaveObject, lpWaveObject->dwEffectParam3);
		WaveDropStone (lpWaveObject, dwX, dwY, dwSize, dwWeight);
		}
		break;
	}
}

//�ͷŶ���
void __stdcall WaveFree (LPWAVEOBJECT lpWaveObject)
{
	if (lpWaveObject->hDcRender)
		DeleteDC (lpWaveObject->hDcRender);
	if (lpWaveObject->hBmpRender)
		DeleteObject (lpWaveObject->hBmpRender);
	if (lpWaveObject->lpDIBitsSource)
		GlobalFree (lpWaveObject->lpDIBitsSource);
	if (lpWaveObject->lpDIBitsRender)
		GlobalFree (lpWaveObject->lpDIBitsRender);
	if (lpWaveObject->lpWave1)
		GlobalFree (lpWaveObject->lpWave1);
	if (lpWaveObject->lpWave2)
		GlobalFree (lpWaveObject->lpWave2);
	KillTimer (lpWaveObject->hWnd, (UINT_PTR) lpWaveObject);
	RtlZeroMemory (lpWaveObject, sizeof WAVEOBJECT);	
}

UINT __stdcall WaveInit (LPWAVEOBJECT lpWaveObject, HWND hWnd, HBITMAP hBmp, UINT dwSpeed, UINT dwType)
{
	BITMAP stBmp;
	UINT dwReturn = 0;
	HDC hDc = NULL;
	HDC hDcRender = NULL;
	
	RtlZeroMemory (lpWaveObject, sizeof (WAVEOBJECT));
	//dwType��ˮ��ͼ�εĿ���
	//dwType��0ʱΪԲ��ˮ����dwType��1ʱΪ��Բ��ˮ��
	if (dwType)
		lpWaveObject->dwFlag = lpWaveObject->dwFlag | F_WO_ELLIPSE;
	
	//��ȡλͼ�ߴ�
	lpWaveObject->hWnd = hWnd;
	//GetTickCount���شӲ���ϵͳ������������������elapsed���ĺ�������
	//���ķ���ֵ��DWORD�� 
	lpWaveObject->dwRandom = GetTickCount ();
	
	if (!GetObject (hBmp, sizeof (BITMAP), &stBmp)) {
		dwReturn = 1;
		goto result;
	}	
	lpWaveObject->dwBmpHeight = stBmp.bmHeight;
	if (stBmp.bmHeight <= 3) 
		dwReturn = 1;
	lpWaveObject->dwBmpWidth = stBmp.bmWidth;
	if (stBmp.bmWidth <= 3) 
		dwReturn = 1;
		
	lpWaveObject->dwWaveByteWidth = stBmp.bmWidth * 4;
	lpWaveObject->dwDIByteWidth = (stBmp.bmWidth * 3 + 3) & (~3);

	//����������Ⱦ��λͼ
	hDc = GetDC (hWnd);
	//����һ����ָ���豸���ݵ��ڴ��豸�����Ļ���
	lpWaveObject->hDcRender = CreateCompatibleDC (hDc);
	//HBITMAP CreateCompatibleBitmap (HDC hDc, int cx, int cy); 
	//ָ���ߡ����ɫ�����������CreateCompatibleDC���õ���Ҫ
	//�ú���������ָ�����豸������ص��豸���ݵ�λͼ
	lpWaveObject->hBmpRender = CreateCompatibleBitmap (hDc, 
		lpWaveObject->dwBmpWidth, lpWaveObject->dwBmpHeight);
	
	//�ú���ѡ��һ����ָ�����豸�����Ļ����У����¶����滻��ǰ����ͬ���͵Ķ��� 
	SelectObject (lpWaveObject->hDcRender, lpWaveObject->hBmpRender);
		
	//���䲨�ܻ�����
	lpWaveObject->lpWave1 = (PINT) GlobalAlloc (GPTR, 
		lpWaveObject->dwWaveByteWidth * lpWaveObject->dwBmpHeight);
	lpWaveObject ->lpWave2 = (PINT) GlobalAlloc (GPTR, 
		lpWaveObject->dwWaveByteWidth * lpWaveObject->dwBmpHeight);

	//�������ػ�����
	lpWaveObject->lpDIBitsSource = (PBYTE) GlobalAlloc (GPTR, 
		lpWaveObject->dwDIByteWidth * lpWaveObject->dwBmpHeight);
	lpWaveObject->lpDIBitsRender = (PBYTE) GlobalAlloc (GPTR, 
		lpWaveObject->dwDIByteWidth * lpWaveObject->dwBmpHeight);

	//��ȡ����ԭʼ����
	lpWaveObject->stBmpInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	lpWaveObject->stBmpInfo.bmiHeader.biWidth = lpWaveObject->dwBmpWidth;
	lpWaveObject->stBmpInfo.bmiHeader.biHeight =   - lpWaveObject->dwBmpHeight;
	lpWaveObject->stBmpInfo.bmiHeader.biPlanes = 1;
	lpWaveObject->stBmpInfo.bmiHeader.biBitCount = 24;
	lpWaveObject->stBmpInfo.bmiHeader.biCompression = BI_RGB;
	lpWaveObject->stBmpInfo.bmiHeader.biSizeImage = 0;

	hDcRender = CreateCompatibleDC (hDc);
	SelectObject (hDcRender, hBmp);
	ReleaseDC (hWnd, hDc);

	//GetDIBits������ȡָ��λͼ����Ϣ����������ָ����ʽ���Ƶ�һ����������
	GetDIBits (hDcRender, hBmp, 0, lpWaveObject->dwBmpHeight, 
	lpWaveObject->lpDIBitsSource, &lpWaveObject->stBmpInfo, DIB_RGB_COLORS);
	GetDIBits (hDcRender, hBmp, 0, lpWaveObject->dwBmpHeight, 
	lpWaveObject->lpDIBitsRender, &lpWaveObject->stBmpInfo, DIB_RGB_COLORS);
	DeleteDC (hDcRender);

	if (!lpWaveObject->lpWave1 || !lpWaveObject->lpWave2 || !lpWaveObject->lpDIBitsSource || !lpWaveObject->lpDIBitsRender || !lpWaveObject->hDcRender) {
		MessageBox (hWnd, TEXT ("�ڴ�����ʧ��"), TEXT ("����"), MB_OK);
		WaveFree (lpWaveObject);
		dwReturn = 1;
	}

	SetTimer (hWnd, (UINT_PTR) lpWaveObject, dwSpeed, WaveTimerProc);

	lpWaveObject->dwFlag = lpWaveObject->dwFlag | F_WO_ACTIVE | F_WO_NEED_UPDATE;
	WaveRender (lpWaveObject);
	hDc = GetDC (hWnd);
	WaveUpdateFrame (lpWaveObject, hDc, TRUE);
	ReleaseDC  (hWnd, hDc);
result:
	return dwReturn;
}

//��Ч����
void __stdcall WaveEffect (LPWAVEOBJECT lpWaveObject, UINT dwEffectType, int dwParam1, int dwParam2, int dwParam3)
{
	switch (dwEffectType) {
	case 0:
		lpWaveObject->dwFlag &= ~F_WO_EFFECT;
		lpWaveObject->dwEffectType = dwEffectType;
		break;
	case 2:
		lpWaveObject->dwEff2XAdd = dwParam1;
		lpWaveObject->dwEff2YAdd = dwParam1;
		lpWaveObject->dwEff2X = WaveRandom (lpWaveObject, lpWaveObject->dwBmpWidth-2) + 1;
		lpWaveObject->dwEff2Y = WaveRandom (lpWaveObject, lpWaveObject->dwBmpHeight-2) + 1;
		lpWaveObject->dwEffectType = dwEffectType;
		lpWaveObject->dwEffectParam1 = dwParam1;
		lpWaveObject->dwEffectParam2 = dwParam2;
		lpWaveObject->dwEffectParam3 = dwParam3;
		lpWaveObject->dwFlag |= F_WO_EFFECT;
		break;
	default:
		lpWaveObject->dwEffectType = dwEffectType;
		lpWaveObject->dwEffectParam1 = dwParam1;
		lpWaveObject->dwEffectParam2 = dwParam2;
		lpWaveObject->dwEffectParam3 = dwParam3;
		lpWaveObject->dwFlag |= F_WO_EFFECT;
	}
}