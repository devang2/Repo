//--------------------------------------------------------------------
//
//	QRmakerPro.ocx使用する C++ のサンプルプログラム
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// testOCX2View.h : CTestOCX2View クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////
#include "qrmakerpro.h"

#if !defined(AFX_TESTOCX2VIEW_H__2B9C998E_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_TESTOCX2VIEW_H__2B9C998E_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestOCX2CntrItem;

class CTestOCX2View : public CFormView
{
protected: // シリアライズ機能のみから作成します。
	CTestOCX2View();
	DECLARE_DYNCREATE(CTestOCX2View)

public:
	//{{AFX_DATA(CTestOCX2View)
	enum { IDD = IDD_TESTOCX2_FORM };
	CComboBox	m_Combo_Unit;
	CComboBox	m_Combo_Ecc;
	CSpinButtonCtrl	m_SpinAdjust;
	CEdit	m_EditAdjust;
	CSpinButtonCtrl	m_SpinQZone;
	CEdit	m_EditQZone;
	CEdit	m_EditModelNo;
	CSpinButtonCtrl	m_SpinModelNo;
	CSpinButtonCtrl	m_SpinPitch;
	CEdit	m_EditPitch;
	CString	m_EditInputData;
	CQRmakerPro	m_QRmaker;
	//}}AFX_DATA

// アトリビュート
public:
	CTestOCX2Doc* GetDocument();
	//   m_pSelection は現在の CTestOCX2CntrItem に対する選択を保持しています。
	//  多くのアプリケーションでこのようなメンバ変数は、複数の選択や CTestOCX2CntrItem
	//  オブジェクトではないオブジェクトの選択を十分に表現できません。
	//  この機構は選択機能を応用しようとするプログラマの理解を助けるた
	//  めに組み込まれています。

	// TODO: この選択方法をアプリケーションに適した方法に置き換えてください。
	CTestOCX2CntrItem* m_pSelection;

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTestOCX2View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo*);
	virtual BOOL IsSelected(const CObject* pDocItem) const;// OLE コンテナ サポート
	virtual void OnDraw(CDC* pDC);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	int m_Height;
	HMETAFILE m_hmf;
	virtual ~CTestOCX2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CTestOCX2View)
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnButtonCopyPicture();
	afx_msg void OnChangeEditInputdata();
	afx_msg void OnAppExit();
	afx_msg void OnButtonGethandle();
	afx_msg void OnChangeEditPitch();
	afx_msg void OnChangeEditModelno();
	afx_msg void OnChangeEditQzone();
	afx_msg void OnChangeEditAdjust();
	afx_msg void OnButtonBinary();
	afx_msg void OnCloseupComboEcc();
	afx_msg void OnCloseupComboUnit();
	afx_msg void OnButtonDrawqrimage();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // testOCX2View.cpp ファイルがデバッグ環境の時使用されます。
inline CTestOCX2Doc* CTestOCX2View::GetDocument()
   { return (CTestOCX2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TESTOCX2VIEW_H__2B9C998E_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
