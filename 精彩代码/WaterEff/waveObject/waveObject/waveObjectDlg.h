// waveObjectDlg.h : ͷ�ļ�
//

#pragma once
#include "waveHead.h"
// CwaveObjectDlg �Ի���
class CwaveObjectDlg : public CDialog
{
// ����
public:
	CwaveObjectDlg(CWnd* pParent = NULL);	// ��׼���캯��

	WAVE_OBJECT wb;
	HBITMAP pic;
// �Ի�������
	enum { IDD = IDD_WAVEOBJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
};
