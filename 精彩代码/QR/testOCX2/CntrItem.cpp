//--------------------------------------------------------------------
//
//	QRmakerPro.ocx Sample Program
//
//  Copyright (C) DENSO CORPORATION 2000-2004 all rights reserved
//--------------------------------------------------------------------
// CntrItem.cpp : CTestOCX2CntrItem 
//

#include "stdafx.h"
#include "testOCX2.h"

#include "testOCX2Doc.h"
#include "testOCX2View.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2CntrItem 

IMPLEMENT_SERIAL(CTestOCX2CntrItem, COleClientItem, 0)

CTestOCX2CntrItem::CTestOCX2CntrItem(CTestOCX2Doc* pContainer)
	: COleClientItem(pContainer)
{
	// 
	
}

CTestOCX2CntrItem::~CTestOCX2CntrItem()
{
	// 
	
}

void CTestOCX2CntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	// 

	GetDocument()->UpdateAllViews(NULL);
		//
}

BOOL CTestOCX2CntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	//

	if (!COleClientItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// 
	return TRUE;
}

void CTestOCX2CntrItem::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);

	//

	rPosition.SetRect(10, 10, 210, 210);
}

void CTestOCX2CntrItem::OnActivate()
{
    // 
    CTestOCX2View* pView = GetActiveView();
    ASSERT_VALID(pView);
    COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
    if (pItem != NULL && pItem != this)
        pItem->Close();
    
    COleClientItem::OnActivate();
}

void CTestOCX2CntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

    // 
    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void CTestOCX2CntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// 
	COleClientItem::Serialize(ar);

	// 
	if (ar.IsStoring())
	{
		//
	}
	else
	{
		// 
	}
}

BOOL CTestOCX2CntrItem::CanActivate()
{
	// 
	CTestOCX2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(COleServerDoc)));
	if (pDoc->IsInPlaceActive())
		return FALSE;

	// 
	return COleClientItem::CanActivate();
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2CntrItem diagnostics

#ifdef _DEBUG
void CTestOCX2CntrItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void CTestOCX2CntrItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
