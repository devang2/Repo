//--------------------------------------------------------------------
//
//	QRmakerPro.ocx Sample Program
//
//  Copyright (C) DENSO CORPORATION 2000-2004 all rights reserved
//--------------------------------------------------------------------
// testOCX2View.cpp : implementation of the CTestOCX2View class
//
#include "stdafx.h"
#include "testOCX2.h"

#include "testOCX2Doc.h"
#include "CntrItem.h"
#include "testOCX2View.h"
#include "picture.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2View

IMPLEMENT_DYNCREATE(CTestOCX2View, CFormView)

BEGIN_MESSAGE_MAP(CTestOCX2View, CFormView)
	//{{AFX_MSG_MAP(CTestOCX2View)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	ON_BN_CLICKED(IDC_BUTTON_COPY_PICTURE, OnButtonCopyPicture)
	ON_EN_CHANGE(IDC_EDIT_INPUTDATA, OnChangeEditInputdata)
	ON_BN_CLICKED(ID_APP_EXIT, OnAppExit)
	ON_BN_CLICKED(IDC_BUTTON_GETHANDLE, OnButtonGethandle)
	ON_EN_CHANGE(IDC_EDIT_PITCH, OnChangeEditPitch)
	ON_EN_CHANGE(IDC_EDIT_MODELNO, OnChangeEditModelno)
	ON_EN_CHANGE(IDC_EDIT_QZONE, OnChangeEditQzone)
	ON_EN_CHANGE(IDC_EDIT_ADJUST, OnChangeEditAdjust)
	ON_BN_CLICKED(IDC_BUTTON_BINARY, OnButtonBinary)
	ON_CBN_CLOSEUP(IDC_COMBO_ECC, OnCloseupComboEcc)
	ON_CBN_CLOSEUP(IDC_COMBO_UNIT, OnCloseupComboUnit)
	ON_BN_CLICKED(IDC_BUTTON_DRAWQRIMAGE, OnButtonDrawqrimage)
	//}}AFX_MSG_MAP
	// Standard Print Command
	ON_COMMAND(ID_FILE_PRINT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CFormView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2View construction/destruction

CTestOCX2View::CTestOCX2View()
	: CFormView(CTestOCX2View::IDD)
{
	//{{AFX_DATA_INIT(CTestOCX2View)
	m_EditInputData = _T("");
	//}}AFX_DATA_INIT
	m_pSelection = NULL;
	// TODO: add construction code here
	m_hmf = NULL;

}

CTestOCX2View::~CTestOCX2View()
{
	if (m_hmf != NULL) DeleteMetaFile(m_hmf);
}

void CTestOCX2View::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	try {
		// Execute some code that might throw an exception.
		//{{AFX_DATA_MAP(CTestOCX2View)
	DDX_Control(pDX, IDC_COMBO_UNIT, m_Combo_Unit);
		DDX_Control(pDX, IDC_COMBO_ECC, m_Combo_Ecc);
		DDX_Control(pDX, IDC_SPIN_ADJUST, m_SpinAdjust);
		DDX_Control(pDX, IDC_EDIT_ADJUST, m_EditAdjust);
		DDX_Control(pDX, IDC_SPIN_QZONE, m_SpinQZone);
		DDX_Control(pDX, IDC_EDIT_QZONE, m_EditQZone);
		DDX_Control(pDX, IDC_EDIT_MODELNO, m_EditModelNo);
		DDX_Control(pDX, IDC_SPIN_MODELNO, m_SpinModelNo);
		DDX_Control(pDX, IDC_SPIN_PITCH, m_SpinPitch);
		DDX_Control(pDX, IDC_EDIT_PITCH, m_EditPitch);
		DDX_Text(pDX, IDC_EDIT_INPUTDATA, m_EditInputData);
	DDX_Control(pDX, IDC_QRMAKERPROCTRL1, m_QRmaker);
	//}}AFX_DATA_MAP
	}
	catch( CException* e ) {
		// Handle the exception here.
		// "e" contains information about the exception.
		e->ReportError();
		e->Delete();
		abort();
	}
}

BOOL CTestOCX2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	return CFormView::PreCreateWindow(cs);
}

void CTestOCX2View::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_pSelection = NULL;    // Initialize Selection

	m_SpinPitch.SetRange(1, 100);
	m_SpinModelNo.SetRange(1, 3);
	m_SpinQZone.SetRange(0, 50);

	int pitch = m_QRmaker.GetCellPitch();
	char buf[20];
	sprintf(buf, "%d", pitch);
	m_EditPitch.SetWindowText(buf);		// Set initial value

	m_SpinAdjust.SetRange(-pitch+1, pitch-1);

	int adj = m_QRmaker.GetCellAdjust();
	sprintf(buf, "%d", adj);
	m_EditAdjust.SetWindowText(buf);		// Set initial value

	m_QRmaker.SetCellUnit(-1);				// Get resolution of default printer
	int unit = m_QRmaker.GetCellUnit();
	sprintf(buf, "%d", unit);
	m_Combo_Unit.SetWindowText(buf);		// Set initial value

	int model = m_QRmaker.GetModelNo();
	sprintf(buf, "%d", model);
	m_EditModelNo.SetWindowText(buf);		// Set initial value

	int qzone = m_QRmaker.GetQuietZone();
	sprintf(buf, "%d", qzone);
	m_EditQZone.SetWindowText(buf);		// Set initial value

	#if 1	// vc++6.0
		USES_CONVERSION;
	#else
		int _convert;
	#endif
	CString tmp;
	COleVariant vaTemp;
	VARIANT newValue = m_QRmaker.GetInputData();
	vaTemp.ChangeType(VT_BSTR, (COleVariant*)&newValue);
	tmp = W2CA(vaTemp.bstrVal);					// Entering Text
	m_EditInputData = tmp;

	m_QRmaker.SetAutoRedraw(1);		// Set flag of AutoRedraw

	int j = m_QRmaker.GetEccLevel();
	m_Combo_Ecc.SetCurSel(j);
	
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2View Printing

BOOL CTestOCX2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Prepare Printing
	return DoPreparePrinting(pInfo);
}

void CTestOCX2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO:
}

void CTestOCX2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 
}

void CTestOCX2View::OnPrint(CDC* pDC, CPrintInfo*)
{
	// TODO: 
}

void CTestOCX2View::OnDestroy()
{
	// 
   CFormView::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////
// Support/Command of OLE Client

BOOL CTestOCX2View::IsSelected(const CObject* pDocItem) const
{
	// 

	return pDocItem == m_pSelection;
}

void CTestOCX2View::OnInsertObject()
{
	// 
	COleInsertDialog dlg;
	if (dlg.DoModal() != IDOK)
		return;

	BeginWaitCursor();

	CTestOCX2CntrItem* pItem = NULL;
	TRY
	{
		// 
		CTestOCX2Doc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new CTestOCX2CntrItem(pDoc);
		ASSERT_VALID(pItem);

		// 
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  
		ASSERT_VALID(pItem);

		// 
		if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);

		ASSERT_VALID(pItem);

		// 
		m_pSelection = pItem;   // 
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

//
void CTestOCX2View::OnCancelEditCntr()
{
	// 
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

// 
void CTestOCX2View::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		//
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // 
			return;
		}
	}

	CFormView::OnSetFocus(pOldWnd);
}

void CTestOCX2View::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server Support

void CTestOCX2View::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2View diagnostics

#ifdef _DEBUG
void CTestOCX2View::AssertValid() const
{
	CFormView::AssertValid();
}

void CTestOCX2View::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTestOCX2Doc* CTestOCX2View::GetDocument() //
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestOCX2Doc)));
	return (CTestOCX2Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestOCX2View message handlers

void CTestOCX2View::OnChangeEditInputdata() 
{
	CString buf;
	CEdit* edit = (CEdit*)GetDlgItem(IDC_EDIT_INPUTDATA);
	edit->GetWindowText(buf);
	#if 1	// vc++6.0
		USES_CONVERSION;
	#else
		int _convert;
	#endif
	CString tmp = A2W(buf);		// 
	m_QRmaker.SetInputData(COleVariant(tmp).Detach());
}

void CTestOCX2View::OnDraw(CDC* pDC) 
{
	int ofsetX = 180, ofsetY = 10;
	int xLongPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	int newSize = m_Height * xLongPixPerInch / 2540;

	// Draw by MetaFileHandle
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(m_Height, -m_Height);
	pDC->SetViewportExt(newSize, newSize);
	//HMETAFILE hmf = (HMETAFILE)pict.GetHandle();
	//pDC->PlayMetaFile(hmf);			// 
	pDC->PlayMetaFile(m_hmf);
}


void CTestOCX2View::OnAppExit() 
{
	CFrameWnd* frm = (CFrameWnd*)GetParentOwner();	
	frm->PostMessage(WM_CLOSE);
}

void CTestOCX2View::OnButtonCopyPicture() 
{
	//CPicture pict;
	//LPDISPATCH lpdisp;
	//pict = m_QRmaker.GetPicture();
	//lpdisp = pict.m_lpDispatch;
	//m_pictHolder.SetPictureDispatch((LPPICTUREDISP)lpdisp);

	CDC *pDC = GetDC();
	int ofsetX = 180, ofsetY = 10;
	int xLongPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);

	CPicture pict = m_QRmaker.GetPicture();
	LPDISPATCH lpdisp = pict.m_lpDispatch;
	CPictureHolder pictHolder;
	int height = pict.GetHeight();		// HIMETRIC Unit
	int newSize = height * xLongPixPerInch / 2540;

	// Draw by Render()
	CRect rect(ofsetX, ofsetY, newSize+ofsetX, newSize+ofsetY);
	pictHolder.SetPictureDispatch((LPPICTUREDISP)lpdisp);
	pictHolder.Render(pDC, rect, rect);

	// if you draw by OnDraw... 
	//static int oldSize = 0;
	//if (newSize > oldSize)				// 
	//	oldSize = newSize;
	//CRect rect(ofsetX, ofsetY, oldSize+ofsetX, oldSize+ofsetY);
	//oldSize = newSize;
	//InvalidateRect(rect);

	ReleaseDC(pDC);			// Don't forget it!
}

void CTestOCX2View::OnButtonGethandle() 
{
	CDC *pDC = GetDC();
	CPicture pict = m_QRmaker.GetPicture();
	HMETAFILE hmf = NULL;
	int type;
	int xLongPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	m_Height = pict.GetHeight();		// HIMETRIC Unit
	int newSize = m_Height * xLongPixPerInch / 2540;
	static int oldSize = 0;

	//	Get MetaFileHandle from CPicture Type
	type = pict.GetType();			// Picture Type
	if (type == PICTYPE_METAFILE) {	// MetaFile
		hmf = (HMETAFILE)pict.GetHandle();
		if (m_hmf != NULL) DeleteMetaFile(m_hmf);
		m_hmf = CopyMetaFile(hmf, NULL);

		
		//LPDISPATCH lpdisp;
		//CPictureHolder pictHolder;
		//lpdisp = pict.m_lpDispatch;
		//pictHolder.SetPictureDispatch((LPPICTUREDISP)lpdisp);
		//type = pictHolder.GetType();	// Picture Type 
		//pictHolder.m_pPict->get_Handle((OLE_HANDLE*)&hmf);	//Receives a pointer to GDI handle 

//		pDC->SetMapMode(MM_ANISOTROPIC);
//		pDC->SetWindowExt(height, -height);
//		pDC->SetViewportExt(newSize, newSize);
		//
		//	int ofsetX = 10, ofsetY = 10;
		//	pDC->SetWindowOrg(ofsetX, -ofsetY);

//		pDC->PlayMetaFile(hmf);
	}
	if (newSize > oldSize)				// Renew drawing area
		oldSize = newSize;
	CRect rect(0, 0, oldSize, oldSize);
	oldSize = newSize;

	InvalidateRect(rect);
	ReleaseDC(pDC);			// Don't forget it!
	
}

void CTestOCX2View::OnChangeEditPitch() 
{
	CString buf;
	if (m_EditPitch.m_hWnd) {				// Ignore the error while View constructing
		m_EditPitch.GetWindowText(buf);
		int pitch = atoi(buf);
		m_QRmaker.SetCellPitch(pitch);
	//	m_SpinAdjust.SetRange(-pitch+1, pitch-1);
	}
}

void CTestOCX2View::OnChangeEditModelno() 
{
	CString buf;
	if (m_EditModelNo.m_hWnd) {				// Ignore the error while View constructing
		m_EditModelNo.GetWindowText(buf);
		m_QRmaker.SetModelNo(atoi(buf));
	}
}

void CTestOCX2View::OnChangeEditQzone() 
{
	CString buf;
	if (m_EditQZone.m_hWnd) {				// Ignore the error while View constructing
		m_EditQZone.GetWindowText(buf);
		m_QRmaker.SetQuietZone(atoi(buf));
	}
}

void CTestOCX2View::OnChangeEditAdjust() 
{
	CString buf;
	if (m_EditAdjust.m_hWnd) {				// Ignore the error while View constructing
		m_EditAdjust.GetWindowText(buf);
		TRY {
			m_QRmaker.SetCellAdjust(atoi(buf));
		}
		CATCH(CException, e) {
			AfxMessageBox("Cell Adjust range over");
		}
		END_CATCH
	}
}

void CTestOCX2View::OnButtonBinary() 
{
	CFileDialog fdlg(TRUE, "bmp", NULL, OFN_FILEMUSTEXIST,
		"All Files (*.*)|*.*||", NULL );

	if (fdlg.DoModal() == IDOK) {
		CString fName = fdlg.GetPathName();

		TRY
		{
			CFile f( fName, CFile::modeRead | CFile::typeBinary);
			//
			CByteArray bary;
			char buf[2];
			int i, fileLen;
			fileLen = f.GetLength();
			if (fileLen > 2953) {
				AfxMessageBox("Too Large File !!");
			} else {
				for (i = 0; i < fileLen; i++) {
					if (f.Read(buf, 1) > 0) {
						bary.Add(buf[0]);
					}
				}
				//
				//
				m_QRmaker.SetInputDataB(COleVariant(bary).Detach());
			}
		}
		CATCH( CFileException, e )
		{
			#ifdef _DEBUG
				afxDump << "File could not be opened " << e->m_cause << "\n";
			#endif
			AfxMessageBox( "Can't Open " + fName, MB_OK | MB_ICONEXCLAMATION);	// MB_SYSTEMMODAL
		}
		END_CATCH
	}
}

void CTestOCX2View::OnCloseupComboEcc() 
{
	int j;

	if ((j = m_Combo_Ecc.GetCurSel()) >= 0) {
		m_QRmaker.SetEccLevel(j);
	}
}

void CTestOCX2View::OnCloseupComboUnit() 
{
 	CString buf;

 	if (m_Combo_Unit.m_hWnd) {				// Ignore the error while View constructing
		int i = m_Combo_Unit.GetCurSel();
		if (i >= 0) {
			m_Combo_Unit.GetLBText(i, buf);
 			m_QRmaker.SetCellUnit(atoi(buf));
 			m_QRmaker.Refresh();
 			// Renew CellPitch,CellAdjust because change the Unit
 			int pitch = m_QRmaker.GetCellPitch();
 			char tbuf[20];
 			sprintf(tbuf, "%d", pitch);
 			m_EditPitch.SetWindowText(tbuf);		// Initialize
 
 			m_SpinAdjust.SetRange(-pitch+1, pitch-1);
 
 			int adj = m_QRmaker.GetCellAdjust();
 			sprintf(tbuf, "%d", adj);
 			m_EditAdjust.SetWindowText(tbuf);		// Initialize
		}
 	}
}

//
// Direct draw by DrawQrImage 
//
void CTestOCX2View::OnButtonDrawqrimage() 
{
// 
	CClientDC dc(this);
	m_QRmaker.DrawQrImage((long)(dc.m_hDC), 0, 0);
}
