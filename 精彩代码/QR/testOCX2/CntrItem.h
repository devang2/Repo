// CntrItem.h : CTestOCX2CntrItem クラスの宣言およびインターフェイスの定義をします。
//

#if !defined(AFX_CNTRITEM_H__2B9C9995_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
#define AFX_CNTRITEM_H__2B9C9995_DCF0_11D1_8D3B_A05802C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CTestOCX2Doc;
class CTestOCX2View;

class CTestOCX2CntrItem : public COleClientItem
{
	DECLARE_SERIAL(CTestOCX2CntrItem)

// コンストラクタ
public:
	CTestOCX2CntrItem(CTestOCX2Doc* pContainer = NULL);
		// メモ: pContainer を NULL にすると IMPLEMENT_SERIALIZE を行うことができます。
		//  IMPLEMENT_SERIALIZE を行うためにはクラス内に引数のないコンストラクタが必要です。
		//  通常、OLE アイテムは NULL でないドキュメント ポインタで組み込まれています。
		//

// アトリビュート
public:
	CTestOCX2Doc* GetDocument()
		{ return (CTestOCX2Doc*)COleClientItem::GetDocument(); }
	CTestOCX2View* GetActiveView()
		{ return (CTestOCX2View*)COleClientItem::GetActiveView(); }

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CTestOCX2CntrItem)
	public:
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
	protected:
	virtual void OnGetItemPosition(CRect& rPosition);
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
	virtual BOOL CanActivate();
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	~CTestOCX2CntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	virtual void Serialize(CArchive& ar);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CNTRITEM_H__2B9C9995_DCF0_11D1_8D3B_A05802C10000__INCLUDED_)
