// MainFrm.cpp : CMainFrame 僋儔僗偺摦嶌掕媊
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#include "stdafx.h"
#include "QR_Image.h"

#include "MainFrm.h"
#include "QR_Encode.h"
#include "ImageView.h"
#include "RightView.h"
#include "BottomView.h"
#include "WatermarkDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CX_SPLITTER	7
#define CY_SPLITTER	7
#define CX_MARGIN	2
#define CY_MARGIN	2

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_GETMINMAXINFO()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_WATERMARK_EMBED, OnWatermarkEmbed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame 僋儔僗偺峔抸/徚柵

CMainFrame::CMainFrame()
{
	m_bInitSplit = FALSE;

	m_bDataEncoded = FALSE;
	m_pSymbleDC = NULL;

	m_strSavePath.Empty();
}

CMainFrame::~CMainFrame()
{
	if (m_pSymbleDC != NULL)
	{
		m_pSymbleDC->SelectObject(m_pOldBitmap);

		delete m_pSymbleBitmap;
		delete m_pSymbleDC;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCreate

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (! m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		! m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}

	if (! m_wndStatusBar.Create(this) ||
		! m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;
	}

	// 僗僥乕僞僗僶乕僀儞僕働乕僞偑惓偟偔昞帵偝傟傞傛偆愭摢儁僀儞暆傪曄峏
	m_wndStatusBar.SetPaneInfo(0, 0, SBPS_NOBORDERS | SBPS_STRETCH, 0);

	// 嵟彫僒僀僘偱昞帵
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	rcWindow.BottomRight() = rcWindow.TopLeft();
	MoveWindow(rcWindow);

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::PreCreateWindow

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame 僋儔僗偺恌抐

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame 儊僢僙乕僕 僴儞僪儔

/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnSetFocus

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	SetActiveView((CView*)(m_wndSplitter1.GetPane(1, 0)));
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCmdMsg

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCreateClient

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// 暘妱僂傿儞僪僂攝抲
//  +----------+----------+
//  |          |          |
//  |昞帵儁僀儞|愝掕儁僀儞|
//  |          |          |
//  +----------+----------+
//  |僨乕僞擖椡儁僀儞     |
//  +---------------------+
	m_wndSplitter1.CreateStatic(this, 2, 1);
	m_wndSplitter1.CreateView(1, 0, RUNTIME_CLASS(CBottomView), CSize(0, 0), pContext);

	m_wndSplitter2.CreateStatic(&m_wndSplitter1, 1, 2);
	m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CImageView), CSize(0, 0), pContext);
	m_wndSplitter2.CreateView(0, 1, RUNTIME_CLASS(CRightView), CSize(0, 0), pContext);

	m_pwndBottomView = ((CBottomView*)m_wndSplitter1.GetPane(1, 0));
	m_pwndRightView  = ((CRightView*)m_wndSplitter2.GetPane(0, 1));

	// 婯掕僒僀僘庢摼
	m_sizeBottomView = ((CFormView*)m_wndSplitter1.GetPane(1, 0))->GetTotalSize();
	m_sizeRightView  = ((CFormView*)m_wndSplitter2.GetPane(0, 1))->GetTotalSize();

	CRect rcClient;
	GetClientRect(rcClient);

	m_wndSplitter1.SetRowInfo(0, (rcClient.Height() - (CY_SPLITTER + CY_MARGIN * 2)) - m_sizeBottomView.cy, 0);
	m_wndSplitter2.SetColumnInfo(0, (rcClient.Width() - (CX_SPLITTER + CX_MARGIN * 2)) - m_sizeRightView.cx, 0);

	m_bInitSplit = TRUE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::RecalcLayout

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	if (m_bInitSplit)
	{
		CRect rcClient;
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, &rcClient);

		int cy0, cy1, cyMin;

		m_wndSplitter1.GetRowInfo(0, cy0, cyMin);
		m_wndSplitter1.GetRowInfo(1, cy1, cyMin);

		if (cy0 >= 0 && cy1 >= 0)
		{
			if ((cy1 > m_sizeBottomView.cy) && (rcClient.Height() < cy0 + cy1 + (CY_SPLITTER + CY_MARGIN * 2)))
			{
				cy1 = max(m_sizeBottomView.cy, (rcClient.Height() - (CY_SPLITTER + CY_MARGIN * 2)) - cy0);
			}

			cy0 = (rcClient.Height() - (CY_SPLITTER + CY_MARGIN * 2)) - cy1;

			m_wndSplitter1.SetRowInfo(0, cy0, 0);
			m_wndSplitter1.RecalcLayout();
		}

		int cx0, cx1, cxMin;

		m_wndSplitter2.GetColumnInfo(0, cx0, cxMin);
		m_wndSplitter2.GetColumnInfo(1, cx1, cxMin);

		if (cx0 >= 0 && cx1 >= 0)
		{
			if ((cx1 > m_sizeRightView.cx) && (rcClient.Width() < cx0 + cx1 + (CX_SPLITTER + CX_MARGIN * 2)))
			{
				cx1 = max(m_sizeRightView.cx, (rcClient.Width() - (CX_SPLITTER + CX_MARGIN * 2)) - cx0);
			}

			cx0 = (rcClient.Width() - (CX_SPLITTER + CX_MARGIN * 2)) - cx1;

			m_wndSplitter2.SetColumnInfo(0, cx0, 0);
			m_wndSplitter2.RecalcLayout();
		}
	}

	CFrameWnd::RecalcLayout(bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnGetMinMaxInfo

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	static BOOL bRestore = FALSE; // 嵟彫壔偐傜偺暅尦帪偼張棟偟側偄

	if (IsWindow(m_wndSplitter1.m_hWnd) && ! bRestore)
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);

		CRect rcClient;
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, &rcClient);

		lpMMI->ptMinTrackSize.y = m_sizeRightView.cy + m_sizeBottomView.cy + CX_SPLITTER + CX_MARGIN * 2 + (rcWindow.Height() - rcClient.Height());
		lpMMI->ptMinTrackSize.x = m_sizeBottomView.cx + CY_MARGIN * 2 + (rcWindow.Width() - rcClient.Width());
	}

	bRestore = IsIconic();

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

/////////////////////////////////////////////////////////////////////////////
// 儊僯儏乕崁栚峏怴

void CMainFrame::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bDataEncoded);
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bDataEncoded);
}


/////////////////////////////////////////////////////////////////////////////
// 儊僯儏乕幚峴

void CMainFrame::OnFileSave() 
{
	m_strSavePath.IsEmpty() ? SaveAsImage() : SaveImage();
}

void CMainFrame::OnFileSaveAs() 
{
	SaveAsImage();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::ShowImage
// 梡  搑丗俻俼僐乕僪僀儊乕僕傪嶌惉
// 旛  峫丗幚嵺偺昤夋偼CImageView::OnDraw

void CMainFrame::ShowImage()
{
	int nLevel = m_pwndRightView->m_comboLevel.GetCurSel();
	int nVersion = m_pwndRightView->m_comboVersion.GetCurSel();
	bool bAutoExtent = (m_pwndRightView->m_checkAutoExtent.GetCheck() != 0);
	int nMaskingNo = m_pwndRightView->m_comboMaskingNo.GetCurSel() - 1;

	CString strEncodeData;
	m_pwndBottomView->m_editSoureData.GetWindowText(strEncodeData);

	int i, j;

	CQR_Encode* pQR_Encode = new CQR_Encode;

	m_bDataEncoded = pQR_Encode->EncodeData(nLevel, nVersion, bAutoExtent, nMaskingNo, strEncodeData);

	if (m_bDataEncoded)
	{
	if (m_pSymbleDC != NULL)
		{
			m_pSymbleDC->SelectObject(m_pOldBitmap);

			delete m_pSymbleBitmap;
			delete m_pSymbleDC;
		}

		m_nSymbleSize = pQR_Encode->m_nSymbleSize + (QR_MARGIN * 2);

		m_pSymbleBitmap = new CBitmap;
		m_pSymbleBitmap->CreateBitmap(m_nSymbleSize, m_nSymbleSize, 1, 1, NULL);
		m_pSymbleDC = new CDC;
		m_pSymbleDC->CreateCompatibleDC(NULL);

		m_pOldBitmap = m_pSymbleDC->SelectObject(m_pSymbleBitmap);

		// 夋柺徚嫀
		m_pSymbleDC->PatBlt(0, 0, m_nSymbleSize, m_nSymbleSize, WHITENESS);

		// 僪僢僩昤夋
		for (i = 0; i < pQR_Encode->m_nSymbleSize; ++i)
		{
			for (j = 0; j < pQR_Encode->m_nSymbleSize; ++j)
			{
				if (pQR_Encode->m_byModuleData[i][j])
				{
					m_pSymbleDC->SetPixel(i + QR_MARGIN, j + QR_MARGIN, RGB(0, 0, 0));
				}
			}
		}

/**************************************************************************************************/  
		//int nPixelSize=1;
	    m_nSymbleSize = pQR_Encode->m_nSymbleSize + (QR_MARGIN * 2);
		int nPixelSize = m_nSymbleSize * m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);//size
		int width=m_nSymbleSize*nPixelSize;
		int height=m_nSymbleSize*nPixelSize;
		int bu_width=width+32-(width%32);
		int length=(bu_width*height)/8;
		unsigned char image_head[62];
		memset(image_head,0,62);
		image_head[0]=66;
		image_head[1]=77;
		image_head[2]=(length+62)%256;
		image_head[3]=(length+62)/256;
		image_head[4]=(length+62)/(256*256);
		image_head[5]=(length+62)/(256*256*256);
		image_head[10]=62;
		image_head[14]=40;
		image_head[18]=width%256;
		image_head[19]=width/256;
		image_head[22]=height%256;
		image_head[23]=height/256;
		image_head[26]=1;
		image_head[28]=1;
		image_head[34]=length%256;
		image_head[35]=length/256;
		image_head[36]=length/(256*256);
		image_head[37]=length/(256*256*256);
		image_head[38]=196;
		image_head[39]=14;
		image_head[42]=196;
		image_head[43]=14;
		image_head[58]=255;
		image_head[59]=255;
		image_head[60]=255;
		image_head[61]=0;
		
		unsigned char *pixels;
		pixels=(unsigned char *)malloc(length*8*sizeof(unsigned char *));
		memset(pixels,0x01,length*8);
/*		for(i=0;i<height;i++)
		{
			for(j=0;j<bu_width;j++)
			{
				*(pixels+i*width+j)=0;
			}
		}
*/
		for(i=0;i<pQR_Encode->m_nSymbleSize;i++)
			for(j=0;j<pQR_Encode->m_nSymbleSize;j++)
			{
				if (pQR_Encode->m_byModuleData[i][j])
				{
					int x,y,z;
					x=i+QR_MARGIN;
					y=j+QR_MARGIN;
					//z=y*bu_width+x;
					int m,n;
					for(m=0;m<nPixelSize;m++)
						for(n=0;n<nPixelSize;n++)
						{
							z=bu_width*(y*nPixelSize+n)+x*nPixelSize+m;
                            *(pixels+z)=0;
						}
				}
			}
    	unsigned char *pixels_by_turn;
		pixels_by_turn=(unsigned char *)malloc(length*8*sizeof(unsigned char*));
		//memset(pixels_by_turn,0x00,length*8);
		for(i=height-1;i>=0;i--)
		{
			memcpy(pixels_by_turn+((height-1)-i)*bu_width,pixels+i*bu_width,bu_width);
		}
 		unsigned char *pixels_by_byte;
		pixels_by_byte=(unsigned char *)malloc(length*sizeof(unsigned char *));
		j=0;
		unsigned char q;
		for(i=0;i<length;i++)
		{
			q=0;
			q+=pixels_by_turn[j++]<<7;
			q+=pixels_by_turn[j++]<<6;
			q+=pixels_by_turn[j++]<<5;
			q+=pixels_by_turn[j++]<<4;
			q+=pixels_by_turn[j++]<<3;
			q+=pixels_by_turn[j++]<<2;
			q+=pixels_by_turn[j++]<<1;
			q+=pixels_by_turn[j++];
			pixels_by_byte[i]=q;
		}
       FILE *fp;
		fp=fopen("QR.bmp","w+");
		if(fp==NULL)
		{
			printf("open error!");
		}
		fseek(fp,0,0);
		fwrite(image_head,62,1,fp);
		fseek(fp,62,0);
		fwrite(pixels_by_byte,length,1,fp);
		free(pixels);
		free(pixels_by_turn);
		free(pixels_by_byte);
		fclose(fp);
/**/
/******************************************************************************************/
//	}
/*
		// 昤夋宆斣昞帵
		CString strWork;

		if (nVersion != pQR_Encode->m_nVersion)
			strWork.Format("(%d)", pQR_Encode->m_nVersion);
		else
			strWork.Empty();

		m_pwndRightView->m_staticVersion.SetWindowText(strWork);

		// 帺摦儅僗僉儞僌慖戰帪偺儅僗僉儞僌斣崋昞帵
		if (nMaskingNo == -1)
			strWork.Format("(%d)", pQR_Encode->m_nMaskingNo);
		else
			strWork.Empty();

		m_pwndRightView->m_staticMaskingNo.SetWindowText(strWork);

		// 儃僞儞巊梡壜
		m_pwndRightView->m_buttonCopy.EnableWindow();                      //使其可以拷贝
		m_pwndRightView->m_buttonSave.EnableWindow();                      //使其可以保存
	}
	else
	{
		// 昞帵僋儕傾
		m_pwndRightView->m_staticVersion.SetWindowText("");
		m_pwndRightView->m_staticMaskingNo.SetWindowText("");

		// 儃僞儞巊梡晄壜
		m_pwndRightView->m_buttonCopy.EnableWindow(FALSE);
		m_pwndRightView->m_buttonSave.EnableWindow(FALSE);
*/	
}

	delete pQR_Encode;

	// 曐懚帪夋憸僒僀僘昞帵
//	ShowBitmapSize();

	// 昞帵儁僀儞傪柍岠壔偟昤夋
//	m_wndSplitter1.GetPane(0, 0)->InvalidateRect(NULL, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::ShowBitmapSize
// 梡  搑丗曐懚帪夋憸僒僀僘偺昞帵

void CMainFrame::ShowBitmapSize()
{
	CString strWork;
	strWork.Empty();

	if (m_bDataEncoded)
	{
		int nModuleSize = m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);

		if (nModuleSize >= 1 && nModuleSize <= 20) 
		{
			int nSize = m_nSymbleSize * nModuleSize;
			strWork.Format("%d x %d", nSize, nSize);
		}
	}

	m_pwndRightView->m_staticBmpSize.SetWindowText(strWork);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CopyImage
// 梡  搑丗俻俼僐乕僪僀儊乕僕傪僋儕僢僾儃乕僪偵僐僺乕
// 堷  悢丗COleDataSource 傾僪儗僗乮僨僼僅儖僩=NULL乯
// 旛  峫丗COleDataSource 偑巜掕偝傟偨応崌偼OLE僪儔僢僌&僪儘僢僾丄柍巜掕帪偼僋儕僢僾儃乕僪僐僺乕
//		   僼僅乕儅僢僩偼DIB宍幃傪巊梡

void CMainFrame::CopyImage(COleDataSource* pOleDataSource)
{
	int nPixelSize = m_nSymbleSize * m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);

	// 價僢僩僨乕僞僒僀僘庢摼
	int nBmpSize = ((nPixelSize + 31) / 32) * 32 / 8;
	nBmpSize *= nPixelSize;

	// 僌儘乕僶儖儊儌儕庢摼
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize);

	if (hGlobal == NULL)
	{
		MessageBox("儊儌儕晄懌偱偡丅", NULL, MB_ICONEXCLAMATION);
		return;
	}

	LPBYTE lpbyBits = (LPBYTE)GlobalLock(hGlobal);

	// 峔憿懱傾僪儗僗妱晅
	LPBITMAPINFO pbmi = (LPBITMAPINFO)lpbyBits;
	LPVOID       pbdt = lpbyBits + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	// LPBITMAPINFO->BITMAPINFOHEADER
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= nPixelSize;
	pbmi->bmiHeader.biHeight		= nPixelSize;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 1;
	pbmi->bmiHeader.biCompression	= BI_RGB;
	pbmi->bmiHeader.biSizeImage		= nBmpSize;
	pbmi->bmiHeader.biXPelsPerMeter = 3780;
	pbmi->bmiHeader.biYPelsPerMeter = 3780;
	pbmi->bmiHeader.biClrUsed		= 0;
	pbmi->bmiHeader.biClrImportant	= 0;

	CDC* pWorkDC = new CDC;
	pWorkDC->CreateCompatibleDC(NULL);

	CBitmap* pWorkBitmap = new CBitmap;
	pWorkBitmap->CreateBitmap(nPixelSize, nPixelSize, 1, 1, NULL);

	CBitmap* pOldBitmap = pWorkDC->SelectObject(pWorkBitmap);
	pWorkDC->StretchBlt(0, 0, nPixelSize, nPixelSize, m_pSymbleDC, 0, 0, m_nSymbleSize, m_nSymbleSize, SRCCOPY);

	GetDIBits(pWorkDC->m_hDC, (HBITMAP)*pWorkBitmap, 0, nPixelSize, pbdt, pbmi, DIB_RGB_COLORS);

	GlobalUnlock(hGlobal);

	pWorkDC->SelectObject(pOldBitmap);
	delete pWorkBitmap;
	delete pWorkDC;

	if (pOleDataSource == NULL)
	{
		// 僋儕僢僾儃乕僪偵僐僺乕
		OpenClipboard();
		EmptyClipboard();
		SetClipboardData(CF_DIB, hGlobal);
		CloseClipboard();
	}
	else
	{
		// OLE僪儔僢僌&僪儘僢僾 僨乕僞揮憲
		pOleDataSource->CacheGlobalData(CF_DIB, hGlobal);
		pOleDataSource->DoDragDrop(DROPEFFECT_COPY);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::SaveAsImage
// 梡  搑丗俻俼僐乕僪僀儊乕僕傪價僢僩儅僢僾僼傽僀儖偵曐懚

void CMainFrame::SaveAsImage()
{
	// CFileDialog 僋儔僗峔抸
	CFileDialog* pFileDialog = new CFileDialog(FALSE, "bmp", m_strSavePath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
											   "位图 (*.bmp)|*.bmp|任意文件 (*.*)|*.*||", this);

	pFileDialog->m_ofn.lpstrTitle = "QR Code";

	CString strFileName;

	//乽僼傽僀儖曐懚乿僟僀傾儘僌昞帵
	if (pFileDialog->DoModal() == IDOK)
		strFileName = pFileDialog->GetPathName();

	delete 	pFileDialog;

	if (strFileName.IsEmpty())
		return;

	m_strSavePath = strFileName;

	SaveImage();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::SaveImage
// 梡  搑丗俻俼僐乕僪僀儊乕僕傪價僢僩儅僢僾僼傽僀儖偵曐懚

void CMainFrame::SaveImage()
{
	int nPixelSize = m_nSymbleSize * m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);

	// 價僢僩僨乕僞僒僀僘庢摼
	int nBmpSize = ((nPixelSize + 31) / 32) * 32 / 8;
	nBmpSize *= nPixelSize;

	// 儊儌儕乕儅僢僾僪僼傽僀儖嶌惉
	HANDLE hFile = CreateFile(m_strSavePath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
							  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		m_strSavePath.Empty();
		MessageBox("僼傽僀儖僆乕僾儞僄儔乕", "QR Code", MB_ICONSTOP);
		return;
	}

	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_COMMIT, 0, 
						  sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize, NULL);
	LPBYTE lpbyMapView = (LPBYTE)MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, 0);

	ZeroMemory(lpbyMapView, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize);

	// 峔憿懱傾僪儗僗妱晅
	LPBITMAPFILEHEADER pbfh = (LPBITMAPFILEHEADER)lpbyMapView;
	LPBITMAPINFO       pbmi = (LPBITMAPINFO)(lpbyMapView + sizeof(BITMAPFILEHEADER));
	LPVOID             pbdt = lpbyMapView + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	// BITMAPFILEHEADER
	pbfh->bfType      = (WORD) (('M' << 8) | 'B'); // "BM"
	pbfh->bfSize      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize;
	pbfh->bfReserved1 = 0;
	pbfh->bfReserved2 = 0;
	pbfh->bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	// LPBITMAPINFO->BITMAPINFOHEADER
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= nPixelSize;
	pbmi->bmiHeader.biHeight		= nPixelSize;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 1;
	pbmi->bmiHeader.biCompression	= BI_RGB;
	pbmi->bmiHeader.biSizeImage		= nBmpSize;
	pbmi->bmiHeader.biXPelsPerMeter = 3780;
	pbmi->bmiHeader.biYPelsPerMeter = 3780;
	pbmi->bmiHeader.biClrUsed		= 0;
	pbmi->bmiHeader.biClrImportant	= 0;

	CDC* pWorkDC = new CDC;
	pWorkDC->CreateCompatibleDC(NULL);

	CBitmap* pWorkBitmap = new CBitmap;
	pWorkBitmap->CreateBitmap(nPixelSize, nPixelSize, 1, 1, NULL);

	CBitmap* pOldBitmap = pWorkDC->SelectObject(pWorkBitmap);
	pWorkDC->StretchBlt(0, 0, nPixelSize, nPixelSize, m_pSymbleDC, 0, 0, m_nSymbleSize, m_nSymbleSize, SRCCOPY);

	GetDIBits(pWorkDC->m_hDC, (HBITMAP)*pWorkBitmap, 0, nPixelSize, pbdt, pbmi, DIB_RGB_COLORS);

	pWorkDC->SelectObject(pOldBitmap);
	delete pWorkBitmap;
	delete pWorkDC;

	UnmapViewOfFile(lpbyMapView);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
}


void CMainFrame::OnWatermarkEmbed() 
{
	// TODO: Add your command handler code here
	CWatermarkDlg dlg;
	dlg.DoModal();
}
