//--------------------------------------------------------------------
//
//	QRmakerPro.ocx Sample Program
//
//  Copyright (C) DENSO CORPORATION 2000-2004 all rights reserved
//--------------------------------------------------------------------
// testOCX2Doc.cpp : implementation of the CTestOCX2Doc class
//

#include "stdafx.h"
#include "testOCX2.h"

#include "testOCX2Doc.h"
#include "CntrItem.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2Doc

IMPLEMENT_DYNCREATE(CTestOCX2Doc, COleServerDoc)

BEGIN_MESSAGE_MAP(CTestOCX2Doc, COleServerDoc)
	//{{AFX_MSG_MAP(CTestOCX2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, COleServerDoc::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, COleServerDoc::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, COleServerDoc::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, COleServerDoc::OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, COleServerDoc::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, COleServerDoc::OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST, COleServerDoc::OnUpdateObjectVerbMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2Doc construction/destruction

CTestOCX2Doc::CTestOCX2Doc()
{
	// OLE Compound File
	EnableCompoundFile();

	// TODO: add one-time construction code here

}

CTestOCX2Doc::~CTestOCX2Doc()
{
}

BOOL CTestOCX2Doc::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2Doc serialization

COleServerItem* CTestOCX2Doc::OnGetEmbeddedItem()
{
	// 

	CTestOCX2SrvrItem* pItem = new CTestOCX2SrvrItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2Doc ActiveX Document Server

CDocObjectServer *CTestOCX2Doc::GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite)
{
	return new CDocObjectServer(this, pDocSite);
}



/////////////////////////////////////////////////////////////////////////////
// CTestOCX2Doc serialization


void CTestOCX2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		//
	}
	else
	{
		// 
	}

	// 
	COleServerDoc::Serialize(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2Doc 

#ifdef _DEBUG
void CTestOCX2Doc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CTestOCX2Doc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

//