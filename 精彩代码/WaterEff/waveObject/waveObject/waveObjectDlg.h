// waveObjectDlg.h : 头文件
//

#pragma once
#include "waveHead.h"
// CwaveObjectDlg 对话框
class CwaveObjectDlg : public CDialog
{
// 构造
public:
	CwaveObjectDlg(CWnd* pParent = NULL);	// 标准构造函数

	WAVE_OBJECT wb;
	HBITMAP pic;
// 对话框数据
	enum { IDD = IDD_WAVEOBJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnClose();
};
