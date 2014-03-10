// QR_Image.cpp : アプリケーション用クラスの機能定義
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#include "stdafx.h"
#include "QR_Image.h"

#include "QR_Encode.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CQR_ImageApp

BEGIN_MESSAGE_MAP(CQR_ImageApp, CWinApp)
	//{{AFX_MSG_MAP(CQR_ImageApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CQR_ImageApp クラスの構築

CQR_ImageApp::CQR_ImageApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// 唯一の CQR_ImageApp オブジェクト

CQR_ImageApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CQR_ImageApp クラスの初期化

BOOL CQR_ImageApp::InitInstance()
{
	// OLE 初期化
	if (! AfxOleInit())
	   return FALSE;

// メモリリーク検出用チェックポイント設定（デバッグ時のみ）
// ※ToolTip 関連のリークは問題なし
#ifdef _DEBUG
	mStart.Checkpoint();
#endif //_DEBUG

#if _MFC_VER <= 0x0600
	#ifdef _AFXDLL
		Enable3dControls();
	#else
		Enable3dControlsStatic();
	#endif
#endif

	// メインウインドウ作成
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// フレームをリソースからロード
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW, NULL, NULL);

	// アプリケーションアイコンセット
	HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	pFrame->SetIcon(hIcon, TRUE);

	// メインウィンドウ表示、更新
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}

int CQR_ImageApp::ExitInstance() 
{
// メモリーリーク情報表示（デバッグ時のみ）
#ifdef _DEBUG
	mEnd.Checkpoint();

	if (mDiff.Difference(mStart, mEnd))
	{
		TRACE("\n-------- Detected memory leaks. DUMP(DumpStatistics) Start --------\n");
		mDiff.DumpStatistics();
		TRACE("-------- Detected memory leaks. DUMP(DumpStatistics) End --------\n");

		TRACE("\n-------- Detected memory leaks. DUMP(DumpAllObjectsSince) Start --------\n");
		mDiff.DumpAllObjectsSince();
		TRACE("-------- Detected memory leaks. DUMP(DumpAllObjectsSince) End --------\n\n");

//		AfxMessageBox("Detected memory leaks.", MB_OK | MB_ICONEXCLAMATION);
	}
	else
	{
		TRACE("\n-------- No detected memory leaks. OK !! --------\n\n");
	}
#endif //_DEBUG

	return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	HCURSOR m_hCurArrow, m_hCurWait, m_hCurHand;
	HFONT   m_hFont;  // URL/MailTo 表示文字列フォント

	// 再描画抑制のためのフラグ
	BOOL m_bPointOnUrl;
	BOOL m_bPointOnMailTo;

	// マウスボタン押上時にコマンド実行するためのフラグ
	BOOL m_bBtnDownUrl;
	BOOL m_bBtnDownMailTo;

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーションコマンド
void CQR_ImageApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_hCurArrow = LoadCursor(NULL, IDC_ARROW);
	m_hCurWait  = LoadCursor(NULL, IDC_WAIT);
	m_hCurHand  = AfxGetApp()->LoadCursor(IDC_RHAND);

	m_bPointOnUrl = m_bPointOnMailTo = FALSE;
	m_bBtnDownUrl = m_bBtnDownMailTo = FALSE;

	// フォント作成（アンダーライン付加）
	LOGFONT lf;
	lf.lfHeight			= 12;
	lf.lfWidth			= 0;
	lf.lfEscapement		= 0;
	lf.lfOrientation	= 0;
	lf.lfWeight			= FW_DONTCARE;
	lf.lfItalic			= FALSE;
	lf.lfUnderline		= TRUE;
	lf.lfStrikeOut		= FALSE;
	lf.lfCharSet		= SHIFTJIS_CHARSET;
	lf.lfOutPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= PROOF_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	lstrcpy(lf.lfFaceName, "ＭＳ Ｐゴシック");

	m_hFont = CreateFontIndirect(&lf);
	GetDlgItem(IDC_STATICURL)->SendMessage(WM_SETFONT, (WPARAM)m_hFont, TRUE);
	GetDlgItem(IDC_STATICMAILTO)->SendMessage(WM_SETFONT, (WPARAM)m_hFont, TRUE);

	return TRUE;
}

void CAboutDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
	// 使用オブジェクト破棄
	DeleteObject(m_hFont);
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	int nID = pWnd->GetDlgCtrlID();

	if (nID == IDC_STATICURL)
		pDC->SetTextColor(m_bPointOnUrl ? RGB(255, 128, 0) : RGB(0, 0, 128));

	if (nID == IDC_STATICMAILTO)
		pDC->SetTextColor(m_bPointOnMailTo ? RGB(255, 128, 0) : RGB(0, 0, 128));

	return hbr;
}

void CAboutDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// マウス座標位置チェック
	ClientToScreen(&point);

	CRect rectUrl;
	GetDlgItem(IDC_STATICURL)->GetWindowRect(rectUrl);

	CRect rectMailTo;
	GetDlgItem(IDC_STATICMAILTO)->GetWindowRect(rectMailTo);

	// カーソル変更
	SetCursor(rectUrl.PtInRect(point) || rectMailTo.PtInRect(point) ? m_hCurHand : m_hCurArrow);

	if (m_bPointOnUrl != rectUrl.PtInRect(point))
	{
		// URL 表示テキスト色変更
		GetDlgItem(IDC_STATICURL)->InvalidateRect(NULL);
		m_bPointOnUrl = rectUrl.PtInRect(point);

		if (m_bPointOnUrl)
			SetCapture();
		else
			ReleaseCapture();
	}

	if (m_bPointOnMailTo != rectMailTo.PtInRect(point))
	{
		// mailto 表示テキスト色変更
		GetDlgItem(IDC_STATICMAILTO)->InvalidateRect(NULL);
		m_bPointOnMailTo = rectMailTo.PtInRect(point);

		if (m_bPointOnMailTo)
			SetCapture();
		else
			ReleaseCapture();
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// マウス座標位置チェック
	ClientToScreen(&point);

	CRect rectUrl;
	GetDlgItem(IDC_STATICURL)->GetWindowRect(rectUrl);

	CRect rectMailTo;
	GetDlgItem(IDC_STATICMAILTO)->GetWindowRect(rectMailTo);

	// カーソル変更
	SetCursor(rectUrl.PtInRect(point) || rectMailTo.PtInRect(point) ? m_hCurHand : m_hCurArrow);

	m_bBtnDownUrl    = rectUrl.PtInRect(point);
	m_bBtnDownMailTo = rectMailTo.PtInRect(point);

	CDialog::OnLButtonDown(nFlags, point);
}

void CAboutDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bBtnDownUrl)
	{
		// マウス座標位置チェック
		ClientToScreen(&point);

		CRect rectUrl;
		GetDlgItem(IDC_STATICURL)->GetWindowRect(rectUrl);

		if (rectUrl.PtInRect(point))
		{
			SetCursor(m_hCurWait);

			// ブラウザ起動
			ShellExecute(m_hWnd, "open", "http://www.psytec.co.jp/", NULL, "", SW_SHOW);

			// 本ダイアログが隠される場合を考慮しテキストを再描画
			m_bPointOnUrl = FALSE;
			GetDlgItem(IDC_STATICURL)->InvalidateRect(NULL);

			SetCursor(m_hCurArrow);
		}

		m_bBtnDownUrl = FALSE;
	}
	else if (m_bBtnDownMailTo)
	{
		// マウス座標位置チェック
		ClientToScreen(&point);

		CRect rectMailTo;
		GetDlgItem(IDC_STATICMAILTO)->GetWindowRect(rectMailTo);

		if (rectMailTo.PtInRect(point))
		{
			SetCursor(m_hCurWait);

			// メーラ起動
			ShellExecute(m_hWnd, "open", "mailto:info@psytec.co.jp", NULL, "", SW_SHOW);

			// 本ダイアログが隠される場合を考慮しテキストを再描画
			m_bPointOnMailTo = FALSE;
			GetDlgItem(IDC_STATICMAILTO)->InvalidateRect(NULL);

			SetCursor(m_hCurArrow);
		}

		m_bBtnDownMailTo = FALSE;
	}

	CDialog::OnLButtonUp(nFlags, point);
}
