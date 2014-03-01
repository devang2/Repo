#ifndef __CRIPPLE_H__
#define __CRIPPLE_H__

#include <Windows.h>

/**
 *ˮ����Ч��
 */
class CRipple
{
	//[���졢��������]
public:
	CRipple();
	virtual ~CRipple();

	//[��Ա����]
public:
	//��ȡˮ��λͼ��
	int GetRippleWidth(){return m_iBmpWidth;}
	//��ȡˮ��λͼ��
	int GetRippleHeight(){return m_iBmpHeight;}
	//��ʼ��ˮ������
	bool InitRipple(HWND hWnd, HBITMAP hBmp, UINT uiSpeed);
	//�ͷ�ˮ������
	void FreeRipple();
	//ˢ�µ���ĻDC
	void UpdateFrame(HDC hDc);
	//��ʯ�ӣ��趨��Դ��
	void DropStone(int x, int y, int stoneSize, int stoneWeight);
	//ˮ����ɢ
	void WaveSpread();
	//���ݲ�����Ⱦˮ��
	void WaveRender();

	//[��Ա����]
private:
	HWND				m_hWnd;						//���ھ��
	HDC					m_hRenderDC;				//�ڴ���ȾDC���
	HBITMAP				m_hRenderBmp;				//��Ⱦλͼ���
	int					m_iBmpWidth;				//ˮ������ͼƬ��
	int					m_iBmpHeight;				//ˮ������ͼƬ��
	int					m_iBytesPerWidth;			//λͼÿһ�е��ֽ���
	int				   *m_pWaveBuf1;				//���ܻ�����1
	int				   *m_pWaveBuf2;				//���ܻ�����2
	BYTE			   *m_pBmpSource;				//ˮ��λͼԭʼ����
	BYTE			   *m_pBmpRender;				//ˮ��λͼ��Ⱦ����
	BITMAPINFO			m_stBitmapInfo;			    //λͼ��Ϣ�ṹ
};

#endif	/*__CRIPPLE_H__*/