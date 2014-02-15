//
// The head file mainGUI.h.
//
// Copyright (c) 1995.8 ~ 1998.4, Mr. Wei Yongming.
//
// Last modified date: 1998.04.15.

/* Type definitions */
typedef struct
{
    UINT      uMsg;
    WPARAM    wParam;
    LPARAM    lParam;
    PSTR      pStr;
    POINT     pt;
}MSG;
typedef MSG * PMSG;
typedef MSG FAR * LPMSG;

typedef struct
{
    WORD fItemFlags;
    int iMenuID;
    char* spItemText;
    char* spPromptInfo;
}NORMALMENUITEM;
typedef NORMALMENUITEM* PNORMALMENUITEM;
typedef NORMALMENUITEM FAR * LPNORMALMENUITEM;

typedef struct
{
    WORD fItemFlags;
    char* spItemText;
    char* spPromptInfo;
    PNORMALMENUITEM pNormalMenuItem;
}POPUPMENUITEM;
typedef POPUPMENUITEM* PPOPUPMENUITEM;
typedef POPUPMENUITEM FAR * LPPOPUPMENUITEM;

typedef struct
{
    PMSG      pMsg;                     // pointer to the message queue
    int       cMsg;                     // longth of message queue
    int       iFront;                   // front of message queue
    int       iRear;                    // rear of message  queue
}MSGQUEUEINFO;

typedef struct
{
    BYTE    fFlags;                     // accelerator flags
    BYTE    bEvent;                     // identify scan code or ascii code
    WORD    wID;                        // accelerator value
}ACCELTAB;
typedef ACCELTAB * PACCELTAB;
typedef ACCELTAB FAR * LPACCELTAB;

#define MAXQUEUELENGTH   16
#define CAPTIONLENGTH    50

typedef struct
{
    RECT clientrect;
    RECT cliprect;
}DC;
typedef DC* PDC;
typedef DC FAR * LPDC;

typedef struct
{
    char spCaption[CAPTIONLENGTH + 1];
    PPOPUPMENUITEM pPopupMenuItem;
    PACCELTAB pAccelTab;
    int (FAR *MainProc)();
    MSGQUEUEINFO MQInfo;
    DC  dc;
}GUIINFO;
typedef GUIINFO* PGUIINFO;
typedef GUIINFO FAR * LPGUIINFO;

/* Macro definitions */
// definition of keybord and mouse input event.
#define NULLINPUT         0
#define KB_KEYDOWN        10
#define KB_KEYUP          11
#define ME_LBUTTONDOWN    20
#define ME_LBUTTONUP      21
#define ME_RBUTTONDOWN    22
#define ME_RBUTTONUP      23
#define ME_LBUTTONDBLCLK  24
#define ME_RBUTTONDBLCLK  25
#define ME_MOVE           26

// definitions of message.
    // Group 1 from 0x0000 to 0x001f
#define MSG_LBUTTONDOWN     0x0001
#define MSG_LBUTTONUP       0x0002
#define MSG_LBUTTONDBLCLK   0x000a
#define MSG_MOUSEMOVE       0x0003
#define MSG_RBUTTONDOWN     0x0004
#define MSG_RBUTTONUP       0x0005
#define MSG_RBUTTONDBLCLK   0x000b
#define MSG_KEYDOWN         0x0006
#define MSG_KEYUP           0x0007
#define MSG_CHAR            0x000c
#define MSG_SYSKEYDOWN      0x0008
#define MSG_SYSKEYUP        0x0009
#define MSG_SYSCHAR         0x000d
#define MSG_NCLBUTTONDOWN   0x000e
#define MSG_NCLBUTTONUP     0x000f
#define MSG_NCLBUTTONDBLCLK 0x0010
#define MSG_NCRBUTTONDOWN   0x0011
#define MSG_NCRBUTTONUP     0x0012
#define MSG_NCRBUTTONDBLCLK 0x0013
    // Group 2 from 0x0020 to 0x005f
#define MSG_SETCURSOR       0x0020
    // Group 3 from 0x0060 to 0x009f
#define MSG_CREATE          0x0060
#define MSG_NCCREATE        0x0061
#define MSG_ERASEBKGND      0x0062
#define MSG_SETFOCUS        0x0064
#define MSG_KILLFOCUS       0x0065
#define MSG_PAINT           0x0067
#define MSG_NCPAINT         0x0068
#define MSG_INITPANES       0x0069
#define MSG_DESTROYPANES    0x006a
#define MSG_DESTROY         0x006e
#define MSG_NCDESTROY       0x006f

    // Group 4 from 0x00a0 to 0x00af
#define MSG_ACTIVEMENU  0x00a0

    // Group 5 from 0x00b0 to 0x0100
    // Group 6 from 0x0100 to 0x013f
#define MSG_NULL            0x0100
#define MSG_QUIT            0x0101
#define MSG_COMMAND         0x0102
#define MSG_IDLE            0x0104

    // Group 6 from 0x0300 to 0x 03ff for user
#define MSG_USER            0x0300

// Parameters about caption
#define CP_HEIGHT       20      // Height of caption bar
#define CP_FGCOLOR      15      // The foreground color of caption
#define CP_BKCOLOR      8       // The background color of caption
#define CD_FGCOLOR      0      // The foreground color of caption
#define CD_BKCOLOR      7       // The background color of caption

/* Functions declaration */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */
// Function that manage the Main graphics user interface.
PGUIINFO GUIAPI CreateMainGUI(const char* spCaption, PPOPUPMENUITEM pPopupMenuItem,
                      PACCELTAB pAccelTab,
                      int (FAR * MainProc)(PGUIINFO, UINT, WPARAM, LPARAM));
void GUIAPI DestroyMainGUI(PGUIINFO pGUIInfo);
int FAR DefaultGUIProc(PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Functions that manage message.
void GUIAPI GetMessage( PMSG pMsg );
BOOL GUIAPI CreateMsgQueue( PGUIINFO pGUIInfo, int cMsg );
BOOL GUIAPI PostMessage( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL GUIAPI GetMessageFromQueue( PGUIINFO pGUIInfo, PMSG pMsg );
BOOL GUIAPI PreProcMessage( PGUIINFO pGUIInfo, PMSG pMsg, BOOL bNoTrackMenu );
int  GUIAPI TranslateAccelerator( PGUIINFO pGUIInfo, PMSG pMsg );
void GUIAPI DisptchMessage( PGUIINFO pGUIInfo, PMSG pMsg );
void GUIAPI SendMessage( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam );

// Functions that manage menu.
WORD GUIAPI GetPopupMenuItemFlags(PGUIINFO pGUIInfo, int iIndex);
WORD GUIAPI GetNormalMenuItemFlags(PGUIINFO pGUIInfo, int iPopupIndex, int iIndex);
void GUIAPI EnablePopupMenuItem(PGUIINFO pGUIInfo, int iIndex, BOOL fEnable);
void GUIAPI EnableNormalMenuItem(PGUIINFO pGUIInfo, int iPopupIndex, int iIndex, BOOL fEnable);
void GUIAPI CheckNormalMenuItem(PGUIINFO pGUIInfo, int iPopupIndex, int iIndex, BOOL fCheck);

void GUIAPI DrawCaption( PGUIINFO pGUIInfo, BOOL fSetFocus);
void GUIAPI DrawMenuBar( PPOPUPMENUITEM pPopupMenuItem );
int GUIAPI TrackMenu( PPOPUPMENUITEM pPopupMenuItem, int iIndex, BOOL fPopup );
int GUIAPI TrackPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex, int x, int y, PPOINT pPt);
void GUIAPI DrawPopupMenu( PPOPUPMENUITEM pPopupMenuItem, int x, int y );
int GUIAPI GetPopupMenuWidth( PPOPUPMENUITEM pPopupMenuItem );
int GUIAPI GetPopupMenuHeight( PPOPUPMENUITEM pPopupMenuItem );
void GUIAPI DrawMenuItem( PSTR pStr, BOOL fHilite, WORD fFlags, int x, int y, int iWidth );
int GUIAPI CanActiveMenu( PPOPUPMENUITEM pPopupMenuItem, PPOINT pPt );
int GUIAPI CanActiveMenuByAltKey(PPOPUPMENUITEM pPopupMenuItem, UINT uKey);

// Functions that manage paint of client area.
void GUIAPI BeginPaint(PDC pDC);
void GUIAPI EndPaint(PDC pDC);

// other functions.
void GUIAPI DrawNCArea( PGUIINFO pGUIInfo );
BOOL GUIAPI IsInClientArea( PGUIINFO pGUIInfo, POINT pt );

extern const char szMiniGUI[];
extern const char szDefaultLogo[];

#ifdef __cplusplus
}
#endif  /* __cplusplus */
