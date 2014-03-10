//--------------------------------------------------------------------
//
//	QRmakerPro.ocx使用する C++ のサンプルプログラム
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// testOCX2.h : TESTOCX2 アプリケーションのメイン ヘッダー ファイル
//

#if !defined(AFX_TESTOCX2_H__2B9C9986_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_TESTOCX2_H__2B9C9986_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2App:
// このクラスの動作の定義に関しては testOCX2.cpp ファイルを参照してください。
//

class CTestOCX2App : public CWinApp
{
public:
	CTestOCX2App();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTestOCX2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// インプリメンテーション
	COleTemplateServer m_server;
		// ドキュメントを作成するためのサーバー オブジェクト

	//{{AFX_MSG(CTestOCX2App)
	afx_msg void OnAppAbout();
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TESTOCX2_H__2B9C9986_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
