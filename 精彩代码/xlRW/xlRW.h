// xlRW.h : main header file for the XLRW application
//

#if !defined(AFX_XLRW_H__84F21BDB_0126_4249_B43C_6FDB1D19C84D__INCLUDED_)
#define AFX_XLRW_H__84F21BDB_0126_4249_B43C_6FDB1D19C84D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXlRWApp:
// See xlRW.cpp for the implementation of this class
//

class CXlRWApp : public CWinApp
{
public:
	CXlRWApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXlRWApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXlRWApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XLRW_H__84F21BDB_0126_4249_B43C_6FDB1D19C84D__INCLUDED_)
