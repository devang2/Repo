#if !defined(AFX_WATERMARKDLG_H__A6666B6D_1B85_407C_987A_3817EEEC32DC__INCLUDED_)
#define AFX_WATERMARKDLG_H__A6666B6D_1B85_407C_987A_3817EEEC32DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WatermarkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWatermarkDlg dialog

class CWatermarkDlg : public CDialog
{
// Construction
public:
	CWatermarkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWatermarkDlg)
	enum { IDD = IDD_WATERMARK };
	CString	m_char_wk;
	CString	m_qr_image;
	float	m_chaos;
	CString	m_qr_image_wm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWatermarkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWatermarkDlg)
	afx_msg void OnOk();
	afx_msg void OnQrimagePath();
	afx_msg void OnQrimageWm();
	afx_msg void OnChangeCharWk();
	afx_msg void OnChangeChaos();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WATERMARKDLG_H__A6666B6D_1B85_407C_987A_3817EEEC32DC__INCLUDED_)
