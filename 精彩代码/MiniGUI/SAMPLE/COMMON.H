/* COMMON.H
 *
 * This File include definitions and typedefs that used by MINIGUI Support System.
 *
 *             Version 1.0.0
 *             Copyright (c) 1994 ~ 1998 Mr. Wei Yongming.
 *             Last modified date: 1998.04.15.
 */
 
/******* Common definitions and typedefs ***********************************/

#define VOID          void

#define FAR           _far
#define NEAR          _near
#define PASCAL        _pascal
#define CDECL         _cdecl

#define GUIAPI        _far _pascal

/****** Simple types & common helper macros *********************************/

typedef int        BOOL;
#define FALSE         0
#define TRUE          1
#define FAILURE       -1

typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long       DWORD;

typedef unsigned int     UINT;

#define LONG long

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((UINT)(w) >> 8) & 0xFF))

#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))

/****** Common pointer types ************************************************/

#ifndef NULL
#define NULL          0
#endif

typedef VOID NEAR*          PVOID;
typedef char NEAR*          PSTR;
typedef char NEAR*          NPSTR;
typedef const char NEAR*    PCSTR;

typedef char FAR*           LPSTR;
typedef const char FAR*     LPCSTR;

typedef BYTE NEAR*       PBYTE;
typedef BYTE FAR*     LPBYTE;

typedef int NEAR*     PINT;
typedef int FAR*      LPINT;

typedef WORD NEAR*          PWORD;
typedef WORD FAR*           LPWORD;

typedef long NEAR*       PLONG;
typedef long FAR*     LPLONG;

typedef DWORD NEAR*         PDWORD;
typedef DWORD FAR*          LPDWORD;

typedef void FAR*           LPVOID;

/* Types use for passing & returning polymorphic values */
typedef UINT WPARAM;
typedef LONG LPARAM;

typedef struct tagRECT
{
    int left;
    int top;
    int right;
    int bottom;
} RECT;
typedef RECT*      PRECT;
typedef RECT NEAR* NPRECT;
typedef RECT FAR*  LPRECT;

typedef struct tagPOINT
{
    int x;
    int y;
} POINT;
typedef POINT*       PPOINT;
typedef POINT NEAR* NPPOINT;
typedef POINT FAR*  LPPOINT;

typedef struct tagSIZE
{
    int cx;
    int cy;
} SIZE;
typedef SIZE*       PSIZE;
typedef SIZE NEAR* NPSIZE;
typedef SIZE FAR*  LPSIZE;

/****** Color support *******************************************************/

#define GetRValue(rgb)      ((BYTE)(rgb))
#define GetGValue(rgb)      ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb)      ((BYTE)((rgb)>>16))

/****** Bitmap support ******************************************************/

#ifndef NOBITMAP

typedef struct tagRGBQUAD
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD FAR* LPRGBQUAD;

typedef struct tagBITMAPINFOHEADER
{
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} BITMAPINFOHEADER;
typedef BITMAPINFOHEADER*      PBITMAPINFOHEADER;
typedef BITMAPINFOHEADER FAR* LPBITMAPINFOHEADER;

typedef struct tagBITMAPINFO
{
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD      bmiColors[1];
} BITMAPINFO;
typedef BITMAPINFO*     PBITMAPINFO;
typedef BITMAPINFO FAR* LPBITMAPINFO;

typedef struct tagBITMAPFILEHEADER
{
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;
typedef BITMAPFILEHEADER*      PBITMAPFILEHEADER;
typedef BITMAPFILEHEADER FAR* LPBITMAPFILEHEADER;

#endif /*NOBITMAP*/

/* Standard dialog button IDs */
#define IDC_STATIC    0
#define IDOK          1
#define IDCANCEL      2
#define IDABORT       3
#define IDRETRY       4
#define IDIGNORE      5
#define IDYES         6
#define IDNO          7

/****** Window creation/destroy *********************************************/

/* Window Styles */
#ifndef NOWINSTYLES

/* Basic window types */
#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP      0x80000000L
#define WS_CHILD      0x40000000L

/* Clipping styles */
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L

/* Generic window states */
#define WS_VISIBLE       0x10000000L
#define WS_DISABLED      0x08000000L
#define WS_FOCUS         0x00002000L
#define WS_CHECKED       0x00001000L

/* Main window states */
#define WS_MINIMIZE      0x20000000L
#define WS_MAXIMIZE      0x01000000L

/* Main window styles */
#define WS_CAPTION       0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER        0x00800000L
#define WS_DLGFRAME      0x00400000L
#define WS_VSCROLL       0x00200000L
#define WS_HSCROLL       0x00100000L
#define WS_SYSMENU       0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L

/* Control types */
#define BUTTON              0x80
#define EDIT                0x81
#define STATIC              0x82
#define LISTBOX             0x83
#define SCROLLBAR           0x84
#define COMBOBOX            0x85

/* Control window styles */
#define WS_GROUP      0x00020000L
#define WS_TABSTOP       0x00010000L

#endif //NOWINSTYLES

/****** control style ******************************************************/

#ifndef NOCTLMGR

/* Static Control Styles */
#define SS_LEFT       0x00000000L
#define SS_CENTER     0x00000001L
#define SS_RIGHT      0x00000002L
#define SS_ICON       0x00000003L
#define SS_BLACKRECT     0x00000004L
#define SS_GRAYRECT      0x00000005L
#define SS_WHITERECT     0x00000006L
#define SS_BLACKFRAME       0x00000007L
#define SS_GRAYFRAME     0x00000008L
#define SS_WHITEFRAME       0x00000009L
#define SS_SIMPLE     0x0000000BL
#define SS_LEFTNOWORDWRAP   0x0000000CL
#define SS_NOPREFIX         0x00000080L
#define SS_OWNER         0x000000FL

/* Button Control Styles */
#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX      0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE     0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX      0x00000007L
#define BS_USERBUTTON       0x00000008L
#define BS_AUTORADIOBUTTON  0x00000009L
#define BS_OWNERDRAW     0x0000000BL
#define BS_LEFTTEXT      0x00000020L

/* Edit control styles */
#define ES_LEFT       0x00000000L
#define ES_CENTER     0x00000001L
#define ES_RIGHT      0x00000002L
#define ES_MULTILINE     0x00000004L
#define ES_UPPERCASE     0x00000008L
#define ES_LOWERCASE     0x00000010L
#define ES_PASSWORD         0x00000020L
#define ES_AUTOVSCROLL      0x00000040L
#define ES_AUTOHSCROLL      0x00000080L
#define ES_NOHIDESEL     0x00000100L
#define ES_OEMCONVERT       0x00000400L

/* Listbox styles */
#define LBS_NOTIFY        0x0001L
#define LBS_SORT          0x0002L
#define LBS_NOREDRAW          0x0004L
#define LBS_MULTIPLESEL       0x0008L
#define LBS_OWNERDRAWFIXED    0x0010L
#define LBS_OWNERDRAWVARIABLE 0x0020L
#define LBS_HASSTRINGS        0x0040L
#define LBS_USETABSTOPS       0x0080L
#define LBS_NOINTEGRALHEIGHT  0x0100L
#define LBS_MULTICOLUMN       0x0200L
#define LBS_WANTKEYBOARDINPUT 0x0400L
#define LBS_EXTENDEDSEL       0x0800L

#endif  /* NOCTLMGR */

/****** Message Box support *************************************************/

#ifndef NOMB

#define MB_OK           0x0000
#define MB_OKCANCEL     0x0001
#define MB_YESNO        0x0002
#define MB_RETRYCANCEL      0x0003
#define MB_ABORTRETRYIGNORE 0x0004
#define MB_YESNOCANCEL      0x0005
#define MB_TYPEMASK     0x000F

#define MB_ICONHAND     0x0010
#define MB_ICONQUESTION     0x0020
#define MB_ICONEXCLAMATION  0x0030
#define MB_ICONASTERISK     0x0040
#define MB_ICONMASK     0x00F0

#define MB_ICONINFORMATION  MB_ICONASTERISK
#define MB_ICONSTOP         MB_ICONHAND

#define MB_DEFBUTTON1       0x0000
#define MB_DEFBUTTON2       0x0100
#define MB_DEFBUTTON3       0x0200
#define MB_DEFMASK      0x0F00

#define MB_APPLMODAL        0x0000
#define MB_SYSTEMMODAL      0x1000
#define MB_TASKMODAL        0x2000

#define MB_NOFOCUS      0x8000

#endif  /* NOMB */

/* Menu flags */
#ifndef NOMENU

#define MF_DISABLED     0x0002
#define MF_CHECKED      0x0008
#define MF_POPUP        0x0010
#define MF_MENUBARBREAK     0x0020
#define MF_MENUBREAK        0x0040
#define MF_END          0x0080

#endif // NOMENU

/* Accelerator flags */
#ifndef NOACCL

#define AF_ASCII        0x00
#define AF_FUNCTION     0x01
#define AF_SHIFT        0x02
#define AF_CTRL         0x03
#define AF_ALT          0x04
#define AF_END          0x80

#define SCAN_F1         0x3B
#define SCAN_F2         0x3C
#define SCAN_F3         0x3D
#define SCAN_F4         0x3E
#define SCAN_F5         0x3F
#define SCAN_F6         0x40
#define SCAN_F7         0x41
#define SCAN_F8         0x42
#define SCAN_F9         0x43
#define SCAN_F10        0x44
#define SCAN_F11        0x85
#define SCAN_F12        0x86

#define SCAN_SHIFTF1    0x54
#define SCAN_SHIFTF2    0x55
#define SCAN_SHIFTF3    0x56
#define SCAN_SHIFTF4    0x57
#define SCAN_SHIFTF5    0x58
#define SCAN_SHIFTF6    0x59
#define SCAN_SHIFTF7    0x5A
#define SCAN_SHIFTF8    0x5B
#define SCAN_SHIFTF9    0x5C
#define SCAN_SHIFTF10   0x5D
#define SCAN_SHIFTF11   0x87
#define SCAN_SHIFTF12   0x88

#define ASCII_CTRLA     0x01
#define ASCII_CTRLB     0x02
#define ASCII_CTRLC     0x03
#define ASCII_CTRLD     0x04
#define ASCII_CTRLE     0x05
#define ASCII_CTRLF     0x06
#define ASCII_CTRLG     0x07
#define ASCII_CTRLH     0x08
#define ASCII_CTRLI     0x09
#define ASCII_CTRLJ     0x0A
#define ASCII_CTRLK     0x0B
#define ASCII_CTRLL     0x0C
#define ASCII_CTRLM     0x0D
#define ASCII_CTRLN     0x0E
#define ASCII_CTRLO     0x0F
#define ASCII_CTRLP     0x10
#define ASCII_CTRLQ     0x11
#define ASCII_CTRLR     0x12
#define ASCII_CTRLS     0x13
#define ASCII_CTRLT     0x14
#define ASCII_CTRLU     0x15
#define ASCII_CTRLV     0x16
#define ASCII_CTRLW     0x17
#define ASCII_CTRLX     0x18
#define ASCII_CTRLY     0x19
#define ASCII_CTRLZ     0x1A

#define SCAN_ALTF1      0x68
#define SCAN_ALTF2      0x69
#define SCAN_ALTF3      0x6A
#define SCAN_ALTF4      0x6B
#define SCAN_ALTF5      0x6C
#define SCAN_ALTF6      0x6D
#define SCAN_ALTF7      0x6E
#define SCAN_ALTF8      0x6F
#define SCAN_ALTF9      0x70
#define SCAN_ALTF10     0x71
#define SCAN_ALTF11     0x8B
#define SCAN_ALTF12     0x8C

#define SCAN_ALT1       0x78
#define SCAN_ALT2       0x79
#define SCAN_ALT3       0x7A
#define SCAN_ALT4       0x7B
#define SCAN_ALT5       0x7C
#define SCAN_ALT6       0x7D
#define SCAN_ALT7       0x7E
#define SCAN_ALT8       0x7F
#define SCAN_ALT9       0x80
#define SCAN_ALT0       0x81

#define SCAN_HOME       0x47
#define SCAN_UPARROW    0x48
#define SCAN_PAGEUP     0x49
#define SCAN_LEFTARROW  0x4B
#define SCAN_END        0x4F
#define SCAN_DOWNARROW  0x50
#define SCAN_PAGEDOWN   0x51
#define SCAN_INSERT     0x52
#define SCAN_DELETE     0x53

#endif // NOACCL
