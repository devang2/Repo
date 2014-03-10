// RightView.cpp : CRightView クラス インプリメンテーションファイル
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#include "stdafx.h"
#include "QR_Image.h"
#include "QR_Encode.h"
#include "MainFrm.h"
#include "RightView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CRightView

IMPLEMENT_DYNCREATE(CRightView, CFormView)

CRightView::CRightView()
	: CFormView(CRightView::IDD)
{
	//{{AFX_DATA_INIT(CRightView)
	//}}AFX_DATA_INIT

	m_bInitControl = FALSE;
}

CRightView::~CRightView()
{
}

void CRightView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRightView)
	DDX_Control(pDX, IDC_COMBOLEVEL, m_comboLevel);
	DDX_Control(pDX, IDC_COMBOVERSION, m_comboVersion);
	DDX_Control(pDX, IDC_STATICVERSION, m_staticVersion);
	DDX_Control(pDX, IDC_CHECKAUTOEXTENT, m_checkAutoExtent);
	DDX_Control(pDX, IDC_COMBOMASKINGNO, m_comboMaskingNo);
	DDX_Control(pDX, IDC_STATICMASKINGNO, m_staticMaskingNo);
	DDX_Control(pDX, IDC_EDITMODULESIZE, m_editModuleSize);
	DDX_Control(pDX, IDC_SPINMODULESIZE, m_spinModuleSize);
	DDX_Control(pDX, IDC_STATICBMPSIZE, m_staticBmpSize);
	DDX_Control(pDX, IDC_BUTTONCOPY, m_buttonCopy);
	DDX_Control(pDX, IDC_BUTTONSAVE, m_buttonSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRightView, CFormView)
	//{{AFX_MSG_MAP(CRightView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_CBN_SELCHANGE(IDC_COMBOLEVEL, OnSelChangeLevel)
	ON_CBN_SELCHANGE(IDC_COMBOVERSION, OnSelChangeVersion)
	ON_BN_CLICKED(IDC_CHECKAUTOEXTENT, OnAutoExtent)
	ON_CBN_SELCHANGE(IDC_COMBOMASKINGNO, OnSelChangeMaskNo)
	ON_EN_CHANGE(IDC_EDITMODULESIZE, OnChangeDrawSize)
	ON_EN_KILLFOCUS(IDC_EDITMODULESIZE, OnKillFocusDrawSize)
	ON_BN_CLICKED(IDC_BUTTONCOPY, OnCopy)
	ON_BN_CLICKED(IDC_BUTTONSAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRightView 診断

#ifdef _DEBUG
void CRightView::AssertValid() const
{
	CFormView::AssertValid();
}

void CRightView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CRightView メッセージ ハンドラ

void CRightView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (! m_bInitControl)
	{
		m_comboLevel.SetCurSel(1);

		m_comboVersion.SetCurSel(0);
		m_staticVersion.SetWindowText("");
		m_checkAutoExtent.SetCheck(1);
		m_checkAutoExtent.EnableWindow(FALSE);

		m_comboMaskingNo.SetCurSel(0);
		m_staticMaskingNo.SetWindowText("");

		m_editModuleSize.SetWindowText("4");
		m_spinModuleSize.SetRange(1, 20);
		m_staticBmpSize.SetWindowText("");

		m_buttonCopy.EnableWindow(FALSE);
		m_buttonSave.EnableWindow(FALSE);

		m_bInitControl = TRUE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// メニュー項目更新

void CRightView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;

	if (GetFocus() != NULL && IsWindow(GetFocus()->m_hWnd) && GetFocus()->GetDlgCtrlID() == IDC_EDITMODULESIZE)
	{
		if (m_editModuleSize.CanUndo())
		{
			bEnable = TRUE;
		}
	}

	pCmdUI->Enable(bEnable);
}

void CRightView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	if (GetFocus() != NULL && IsWindow(GetFocus()->m_hWnd) && GetFocus()->GetDlgCtrlID() == IDC_EDITMODULESIZE)
	{
		int nStartChar, nEndChar;

		m_editModuleSize.GetSel(nStartChar, nEndChar);

		if (nStartChar != nEndChar)
		{
			bEnable = TRUE;
		}
	}

	pCmdUI->Enable(bEnable);
}

void CRightView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	if (GetFocus() != NULL && IsWindow(GetFocus()->m_hWnd) && GetFocus()->GetDlgCtrlID() == IDC_EDITMODULESIZE)
	{
		int nStartChar, nEndChar;

		m_editModuleSize.GetSel(nStartChar, nEndChar);

		if (nStartChar != nEndChar)
		{
			bEnable = TRUE;
		}
	}

	pCmdUI->Enable(bEnable);
}

void CRightView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	BOOL bEnable = FALSE;

	if (GetFocus() != NULL && IsWindow(GetFocus()->m_hWnd) && GetFocus()->GetDlgCtrlID() == IDC_EDITMODULESIZE)
	{
		bEnable = TRUE;
	}

	pCmdUI->Enable(bEnable);
}


/////////////////////////////////////////////////////////////////////////////
// メニュー実行

void CRightView::OnEditUndo() {m_editModuleSize.Undo();}
void CRightView::OnEditCut() {m_editModuleSize.Cut();}
void CRightView::OnEditCopy() {m_editModuleSize.Copy();}
void CRightView::OnEditPaste() {m_editModuleSize.Paste();}
void CRightView::OnEditSelectAll() {m_editModuleSize.SetSel(0, -1);}


/////////////////////////////////////////////////////////////////////////////
// 入力更新後処理

void CRightView::OnSelChangeLevel() 
{
	if (m_bInitControl)
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ShowImage();
}

void CRightView::OnSelChangeVersion() 
{
	if (m_bInitControl)
	{
		m_checkAutoExtent.EnableWindow(m_comboVersion.GetCurSel() != 0 && m_comboVersion.GetCurSel() < 40);

		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ShowImage();
	}
}

void CRightView::OnAutoExtent() 
{
	if (m_bInitControl)
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ShowImage();
}

void CRightView::OnSelChangeMaskNo() 
{
	if (m_bInitControl)
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ShowImage();
}

void CRightView::OnChangeDrawSize() 
{
	if (m_bInitControl)
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ShowBitmapSize();
}

void CRightView::OnKillFocusDrawSize()
{
	if (m_bInitControl)
	{
		int nDrawSize = GetDlgItemInt(IDC_EDITMODULESIZE);

		if (nDrawSize <= 0)
		{
			nDrawSize = 1;
		}
		else if (nDrawSize > 20)
		{
			nDrawSize = 20;
		}

		SetDlgItemInt(IDC_EDITMODULESIZE, nDrawSize);

		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->ShowBitmapSize();
	}
}


/////////////////////////////////////////////////////////////////////////////
// コマンドボタン処理

void CRightView::OnCopy() 
{
	((CMainFrame*)(AfxGetApp()->m_pMainWnd))->CopyImage();
}

void CRightView::OnSave() 
{
	if (((CMainFrame*)(AfxGetApp()->m_pMainWnd))->m_strSavePath.IsEmpty())
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->SaveAsImage(); // 名前を付けて保存
	else
		((CMainFrame*)(AfxGetApp()->m_pMainWnd))->SaveImage(); // 上書き保存
}
