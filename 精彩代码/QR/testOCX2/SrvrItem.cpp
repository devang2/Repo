//--------------------------------------------------------------------
//
//	QRmakerPro.ocx Sample Program
//
//  Copyright (C) DENSO CORPORATION 2000-2004 all rights reserved
//--------------------------------------------------------------------


#include "stdafx.h"
#include "testOCX2.h"

#include "testOCX2Doc.h"
#include "SrvrItem.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2SrvrItem 

IMPLEMENT_DYNAMIC(CTestOCX2SrvrItem, CDocObjectServerItem)

CTestOCX2SrvrItem::CTestOCX2SrvrItem(CTestOCX2Doc* pContainerDoc)
	: CDocObjectServerItem(pContainerDoc, TRUE)
{
	// 
}

CTestOCX2SrvrItem::~CTestOCX2SrvrItem()
{
	// 
}

void CTestOCX2SrvrItem::Serialize(CArchive& ar)
{
	// 
	if (!IsLinkedItem())
	{
		CTestOCX2Doc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL CTestOCX2SrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	//

	if (dwDrawAspect != DVASPECT_CONTENT)
		return CDocObjectServerItem::OnGetExtent(dwDrawAspect, rSize);

	//

	CTestOCX2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// 

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC unit

	return TRUE;
}

BOOL CTestOCX2SrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	//
	UNREFERENCED_PARAMETER(rSize);

	CTestOCX2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	// 

	POSITION pos = pDoc->GetStartPosition();
	CTestOCX2CntrItem* pItem = (CTestOCX2CntrItem*)pDoc->GetNextClientItem(pos);
	if (pItem != NULL)
		pItem->Draw(pDC, CRect(10, 10, 1010, 1010));
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2SrvrItem diagnostics

#ifdef _DEBUG
void CTestOCX2SrvrItem::AssertValid() const
{
	CDocObjectServerItem::AssertValid();
}

void CTestOCX2SrvrItem::Dump(CDumpContext& dc) const
{
	CDocObjectServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
