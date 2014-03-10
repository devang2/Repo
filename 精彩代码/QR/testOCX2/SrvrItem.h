//--------------------------------------------------------------------
//
//	QRmakerPro.ocx使用する C++ のサンプルプログラム
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// SrvrItem.h : CTestOCX2SrvrItem クラスの宣言およびインターフェイスの定義をします。
//

#if !defined(AFX_SRVRITEM_H__2B9C9991_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_SRVRITEM_H__2B9C9991_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestOCX2SrvrItem : public CDocObjectServerItem
{
	DECLARE_DYNAMIC(CTestOCX2SrvrItem)

// コンストラクタ
public:
	CTestOCX2SrvrItem(CTestOCX2Doc* pContainerDoc);

// アトリビュート
	CTestOCX2Doc* GetDocument() const
		{ return (CTestOCX2Doc*)CDocObjectServerItem::GetDocument(); }

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTestOCX2SrvrItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	~CTestOCX2SrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // ドキュメント I/O に対してオーバーライドされます。
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SRVRITEM_H__2B9C9991_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
