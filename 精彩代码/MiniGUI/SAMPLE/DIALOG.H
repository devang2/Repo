//
// This head file definition functions that process dialog box 
//
// Copyright (c) 1994.8 ~ 1998.4, Mr. Wei Yongming.
//
// Last modified date: 1998.04.15.

/* Type definitions */
typedef struct
{
   DWORD lStyle;
   short  lx;
   short  ty;
   short  rx;
   short  by;
   WORD  wID;
   BYTE  bClass;
   char* spItemText;
   LPSTR slpAdd;
}CTRLDATA;
typedef CTRLDATA* PCTRLDATA;
typedef CTRLDATA FAR * LPCTRLDATA;
   
typedef struct
{
   DWORD lStyle;
   short  NumOfItems;
   short  lx;
   short  ty;
   short  rx;
   short  by;
   PCTRLDATA pCtrlData;
   LPCSTR spCaption;
}DLGHEADER;
typedef DLGHEADER* PDLGHEADER;
typedef DLGHEADER FAR * LPDLGHEADER;

typedef struct
{
    char pBuff[256];
    int iEditPos;
    int iDataEnd;
    short CaretOffset;
}EDITBUFF;
typedef EDITBUFF* PEDITBUFF;
typedef EDITBUFF FAR * LPEDITBUFF;

typedef struct node
{
    LPSTR pStr;
    struct node FAR* pNext;
}NODE;
typedef NODE* PNODE;
typedef NODE FAR* LPNODE;

typedef struct 
{
    int iWndOffset;
    int iCurSel;
    int iCount;
    LPNODE pHead;
}LISTBOXBUFF;
typedef LISTBOXBUFF* PLISTBOXBUFF;
typedef LISTBOXBUFF FAR* LPLISTBOXBUFF;

typedef struct
{
    DWORD           lStyle;
    short           lx, ty, rx, by;
    short           xfirst;
    int             NumOfCtrl;
    PCTRLDATA       pCtrlData;
    LPCSTR           lpMsg;
    LPCSTR           lpCaption;
}MSGBOXDATA;
typedef MSGBOXDATA* PMSGBOXDATA;
typedef MSGBOXDATA FAR * LPMSGBOXDATA;

typedef struct
{
    BOOL fOpen;
    int iInitDrive;
    LPCSTR lpInitDir;
    LPSTR lpFileName;
    LPCSTR lpExt;
}OPENFILEBOX;
typedef OPENFILEBOX* POPENFILEBOX;
typedef OPENFILEBOX FAR * LPOPENFILEBOX;

// the identifiers of System Dialogs
#define IDD_SYSABOUTDLG         1
#define IDD_OPENFILEDLG         2
#define IDD_SIMPLEOPENFILEDLG   3

// the identifiers that used by System About Box
#define IDC_ICON                        500
#define IDC_MEMORY                      501
#define IDC_MATH                        502
#define IDC_DISK                        503
#define IDC_FARMEMORY                   504

// the identifiers that used by Open File Box
#define IDC_FILENAME                    504
#define IDC_DIRECTORY                   505
#define IDC_FILE                        506
#define IDC_DIR                         507

// definition of FIELD EDIT key and mouse message
#define LB_NULL           30
#define LB_BEEP           32
#define LB_ASCII          33
#define LB_DOWN           34
#define LB_UP             35
#define LB_FRONT          36
#define LB_END            37
#define LB_PAGEDOWN       38
#define LB_PAGEUP         39

// definition of FIELD EDIT key and mouse message
#define ED_NULL           30
#define ED_ABORT          31
#define ED_BEEP           32
#define ED_ASCII          33
#define ED_LEFT           34
#define ED_RIGHT          35
#define ED_FRONT          36
#define ED_END            37
#define ED_DELETE         38
#define ED_BACKSPACE      39
#define ED_INSTOGGLE      40
#define ED_TRANSMIT       41
#define ED_DELALL         42
                          
// definition of BOX key and mouse message
#define INVALID           50
#define NEXTTAB           51
#define LASTTAB           52
#define ABORT             53
#define RETRY             54
#define IGNORE            55
#define CANCEL            56
#define OK                57
#define YES               58
#define NO                59
#define CHECK             70
#define BEEP              71
#define NEXTGROUP         72
#define LASTGROUP         73
#define MOUSEFOCUS        75
#define MOUSECHECK        76
    
// definition of box message
#define NONE              100
#define INITDIALOG        101
#define DRAWCTRL          102
#define DRAWTEXT          103
#define COMMAND           104
#define SETFOCUS          105
#define FOCUSCOMMAND      106
#define INITIALIZED       110

// definition of dialog procedure's return 
#define OWNER             1
#define DEFAULT           2
#define CLOSE             3
#define NOCLOSE           4

/* Function declaration */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
// Common functions
void GUIAPI DrawCtrl( BYTE bClass, DWORD lSytle, short buttonx, short buttony, short buttonw, short buttonh );
void GUIAPI DrawCtrlClientArea(PCTRLDATA pCtrlData);
void GUIAPI SetFocus(PCTRLDATA pCtrlData);
void GUIAPI TranslateBoxMsg( BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg );
void GUIAPI TranslateEditMsg(PDLGHEADER pDlgHeader, PMSG pMsg );
void GUIAPI TranslateListBoxMsg(PDLGHEADER pDlgHeader, PMSG pMsg );

// about dialog
PDLGHEADER GUIAPI GetSysDlgHeader(int iID);
int FAR SysAboutBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam );
int FAR OpenBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam );
int FAR SimpleOpenBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam );
BOOL GUIAPI DialogBoxParam( PGUIINFO pGUIInfo, PDLGHEADER pDlgHeader, int (FAR *DlgProc)(PDLGHEADER, UINT, WORD, LONG), LPSTR lpStr );
PCTRLDATA GUIAPI GetControlData( PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI GetControlRect( PDLGHEADER pDlgHeader, WORD wID, LPRECT lpRect);
void GUIAPI GrayDlgItem( PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI UngrayDlgItem( PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI ShowDlgItem( PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI HideDlgItem( PDLGHEADER pDlgHeader, WORD wID );

void GUIAPI DrawTextInStaticItem( PDLGHEADER pDlgHeader, WORD wID, LPCSTR lpStr );

// about Message Box
int GUIAPI MessageBox( PGUIINFO pGUIInfo, LPCSTR lpszText, LPCSTR lpszCaption, DWORD lStyle);
int GUIAPI SysMessageBox( PGUIINFO pGUIInfo, LPCSTR lpszText, LPCSTR lpszCaption, DWORD lStyle);
static BOOL CreateMsgBox( PMSGBOXDATA pMsgBoxData );
LPSTR GUIAPI LoadMsgIcon( DWORD lStyle );

// about button
BOOL GUIAPI GetCheck( PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI SetCheck( PDLGHEADER pDlgHeader, WORD wID, BOOL bCheck );

// about Field Edit Box
void GUIAPI ProcEdit(  PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData, PMSG pMsg );
void GUIAPI OutFieldEditText( PCTRLDATA pCtrlData, BOOL fEdit );
void GUIAPI SetEditBoxText(  PDLGHEADER pDlgHeader, WORD wID, LPCSTR lpStr );
void GUIAPI GetEditBoxText(  PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr, int iLength );

// about linked list
LPNODE GUIAPI CreateNewList(LPSTR lpStr);
BOOL GUIAPI AddNode(LPNODE pHead, LPSTR lpStr);
void GUIAPI DeleteList(LPNODE pHead);
LPNODE GUIAPI GetTrail(LPNODE pHead);
LPNODE GUIAPI DeleteNode(LPNODE pHead, int iIndex);
LPCSTR GUIAPI GetNodeValue(LPNODE pHead, int iIndex);
int GUIAPI GetListLength(LPNODE pHead);

// about ListBox
void GUIAPI ProcListBox(  PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData, PMSG pMsg );
BOOL GUIAPI AddListBoxItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr );
void GUIAPI DeleteListBoxItem( PDLGHEADER pDlgHeader, WORD wID, int iIndex );
int GUIAPI GetSelectedItem( PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI GetListBoxItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr, int iIndex );
int GUIAPI GetListItemNum(  PDLGHEADER pDlgHeader, WORD wID );
void GUIAPI ReleaseListBoxBuff( PCTRLDATA pCtrlData );
void GUIAPI FillListBox( PCTRLDATA pCtrlData );

#ifdef __cplusplus
}
#endif  /* __cplusplus */
