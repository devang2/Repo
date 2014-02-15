//
// The head file WNDPanes.h
//
// Copyright (c) 1997.8 ~ 1998.3, Mr. Wei Yongming.
//
// Last modified date: 1998.03.06.

/* Type definitions */
#define MAX_LENGTH_LINE	49
typedef int HWNDPANE;
typedef struct tagTEXTLINE
{
    UINT nLineNo;
    char szText[MAX_LENGTH_LINE + 1];
    struct tagTEXTLINE FAR* pNextLine;
}TEXTLINE;
typedef TEXTLINE* PTEXTLINE;
typedef TEXTLINE FAR* LPTEXTLINE;
    
#define MAX_LENGTH_CAPTION	48
typedef struct tagWINDOWPANE
{
	PGUIINFO pGUIInfo;
    short sLeft;
    short sTop;
    short sRight;
    short sBottom;
    short sBKColor;
    short sFGColor;
    char szCaption[MAX_LENGTH_CAPTION + 1];
    void (FAR *PaintProc)(HWNDPANE hWndPane);
    LPVOID lpData;
    UINT nLineNum;
    UINT nCurLineNO;
    LPRECT lpClipRect;
}WINDOWPANE;
typedef WINDOWPANE* PWINDOWPANE;
typedef WINDOWPANE FAR * LPWINDOWPANE;

#define MAX_PANENUMBER  8
#define PANE_VERTICAL   1
#define PANE_HORIZONTAL 2

HWNDPANE GUIAPI CreateParentPane(PGUIINFO pGUIInfo, short sFGColor, short sBKColor, LPCSTR szCaption, void (FAR *PaintProc)(HWNDPANE hWndPane));
HWNDPANE GUIAPI CreateChildPane(HWNDPANE hParentPane, int iType, int iTopOrLeft, short sFGColor, short sBKColor, LPCSTR szCaption, void (FAR *PaintProc)(HWNDPANE hWndPane));

short GUIAPI GetPaneBKColor(HWNDPANE hWndPane);
short GUIAPI GetPaneFGColor(HWNDPANE hWndPane);
LPCSTR GUIAPI GetPaneCaption(HWNDPANE hWndPane);
BOOL GUIAPI GetPaneRect(HWNDPANE hWndPane, LPRECT lpRect);
BOOL GUIAPI GetPaneClientRect(HWNDPANE hWndPane, LPRECT lpRect);
PWINDOWPANE GUIAPI GetPaneContent(HWNDPANE hWndPane);

void GUIAPI AddTextLineToPane(HWNDPANE hWndPane, LPCSTR lpszText, BOOL bUpdate = TRUE);
void GUIAPI OutPaneText(HWNDPANE hWndPane);
void GUIAPI ScrollUpOnLine(HWNDPANE hWndPane);
void GUIAPI FlashLastLine(HWNDPANE hWndPane);

BOOL GUIAPI IsRectVisible(HWNDPANE hWndPane, LPRECT lpRect);
void GUIAPI UpdatePane(HWNDPANE hWndPane, LPRECT lpRect = NULL);
void GUIAPI PaintPanes(PGUIINFO pGUIInfo, PDC pDC);

void GUIAPI DestroyPane(HWNDPANE hWndPane);
void GUIAPI DestroyAllPanes();

void GUIAPI DeleteWndPaneContent(HWNDPANE hWndPane);
