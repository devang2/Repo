// xlRWDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xlRW.h"
#include "xlRWDlg.h"

#include "biff.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXlRWDlg dialog

CXlRWDlg::CXlRWDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXlRWDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CXlRWDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CXlRWDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXlRWDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXlRWDlg, CDialog)
	//{{AFX_MSG_MAP(CXlRWDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXlRWDlg message handlers

BOOL CXlRWDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXlRWDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXlRWDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXlRWDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CXlRWDlg::OnOK() 
{
	CFile f;
	CFileException e;

	// 打开文件
	if (!f.Open("D:\\Book1.xls", CFile::modeRead, &e))
	{
		TCHAR szError[1024];
		e.GetErrorMessage(szError, 1024);
		AfxMessageBox(szError);
		return;
	}

	DWORD dwLen = f.GetLength();
	DWORD dwPos = f.GetPosition();
	WORD RecNo, RecLen, StrLen;
	UINT nRead;
	
	// 读取版本
	while (dwPos < dwLen)
	{
		nRead = f.Read((void*)&RecNo, 2);
		if (RecNo == XL_BOF)
		{
			WORD Ver, Type;
			f.Read((void*)&RecLen, 2);
			f.Read((void*)&Ver, 2);
			f.Read((void*)&Type, 2);
			f.Seek(RecLen, CFile::current);

			int ver = 0;
			switch (Ver)
			{
			case BIFF7:
				ver = 7;
				break;
			case BIFF8:
				ver = 8;
				AfxMessageBox("Biff8");
				break;
			}

			int type = 0;
			switch (Type)
			{
			case WORKBOOK:
				type = 5;
				AfxMessageBox("Workbook");
				break;
			case WORKSHEET:
				type = 16;
				AfxMessageBox("Worksheet");
				break;
			case CHART:
				type = 32;
				AfxMessageBox("Chart");
				break;
			}

			break;
		}
		dwPos = f.GetPosition();
	}
	
	f.SeekToBegin();
	dwPos = f.GetPosition();
	// 读表格数据
	while (dwPos < dwLen)
	{
		nRead = f.Read((void*)&RecNo, 2);
		switch (RecNo)
		{
		case XL_BOF:
			{
				f.Read((void*)&RecLen, 2);
				AfxMessageBox("Bof");
			}
			break;
		case XL_BOUNDSHEET:
			{
				DWORD	temp;
				BYTE	visi;
				BYTE	type;
				TCHAR	name;
				
				f.Read((void*)&RecLen, 2);
				f.Read((void*)&temp, 4);
				f.Read((void*)&visi, 1);
				f.Read((void*)&type, 1);
				f.Read((void*)&StrLen, 2);
				f.Read((void*)&name, StrLen);
				
				char buf[128];
				memset(buf, 0x0, 128);
				strncpy(buf, &name, StrLen);
				
				AfxMessageBox(buf);

			}
			break;
		case XL_DIMENSION:
			f.Read((void*)&RecLen, 2);
			f.Seek(RecLen, CFile::current);
			AfxMessageBox("Dimension");
			break;
		case 0xE2:	// INTERFACED
			f.Read((void*)&RecLen, 2);
			AfxMessageBox("e2");
			break;
		case XL_SST:
			f.Read((void*)&RecLen, 2);
			f.Seek(RecLen, CFile::current);
			AfxMessageBox("SST");
			break;
		case XL_NUMBER:
			f.Read((void*)&RecLen, 2);
			AfxMessageBox("Number");
			break;
		case XL_STRING:
			f.Read((void*)&RecLen, 2);
			AfxMessageBox("String");
			break;
		case XL_RK:
			f.Read((void*)&RecLen, 2);
			AfxMessageBox("RK");
			break;
		case XL_LABEL:
			{
				f.Read((void*)&RecLen, 2);
				AfxMessageBox("Label");
			}
			break;
		case 0xD6:
			f.Read((void*)&RecLen, 2);
			AfxMessageBox("RString");
			break;
		case XL_EOF:
			dwPos = dwLen;
			AfxMessageBox("Eof");
			break;
		default:
			nRead = f.Read((void*)&RecLen, 2);
			if (nRead == 0)
				dwPos = dwLen;
			break;
		}
	}

	// 关闭文件
	f.Close();
}
