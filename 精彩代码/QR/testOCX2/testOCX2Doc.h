//--------------------------------------------------------------------
//
//	QRmakerPro.ocx使用する C++ のサンプルプログラム
//
//  Copyright (C) DENSO CORPORATION 2000 all rights reserved
//--------------------------------------------------------------------
// testOCX2Doc.h : CTestOCX2Doc クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTOCX2DOC_H__2B9C998C_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_TESTOCX2DOC_H__2B9C998C_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CTestOCX2SrvrItem;

class CTestOCX2Doc : public COleServerDoc
{
protected: // シリアライズ機能のみから作成します。
	CTestOCX2Doc();
	DECLARE_DYNCREATE(CTestOCX2Doc)

// アトリビュート
public:
	CTestOCX2SrvrItem* GetEmbeddedItem()
		{ return (CTestOCX2SrvrItem*)COleServerDoc::GetEmbeddedItem(); }

// オペレーション
public:

//オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTestOCX2Doc)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CTestOCX2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite);

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CTestOCX2Doc)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_TESTOCX2DOC_H__2B9C998C_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
