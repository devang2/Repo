// xlRWDlg.h : header file
//

#if !defined(AFX_XLRWDLG_H__EC401FC2_2E4E_4B7D_B6BA_2FB9F18E1BC2__INCLUDED_)
#define AFX_XLRWDLG_H__EC401FC2_2E4E_4B7D_B6BA_2FB9F18E1BC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CXlRWDlg dialog

class CXlRWDlg : public CDialog
{
// Construction
public:
	CXlRWDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CXlRWDlg)
	enum { IDD = IDD_XLRW_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXlRWDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CXlRWDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XLRWDLG_H__EC401FC2_2E4E_4B7D_B6BA_2FB9F18E1BC2__INCLUDED_)
