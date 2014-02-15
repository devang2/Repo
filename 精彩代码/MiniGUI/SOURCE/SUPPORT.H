//
// This head file definition functions that process displaying of 
// bitmap, cursor, caret, icon, etc. Used by surport.c
//
// Copyright (c) 1994.8 ~ 1998.4, Mr. Wei Yongming.
//
// Last modified date: 1998.04.15.

/* Type definitions */
typedef struct
{
    short xOffset;
    short Width;
    unsigned int uTimes;
}STATUSBAR;
typedef STATUSBAR* PSTATUSBAR;

typedef struct
{
    short fgcolor;
    LPCSTR lpStr;
}STATUSBARDATA;
typedef STATUSBARDATA* PSTATUSBARDATA;

typedef struct
{
    _vmhnd_t Handle[5];
}CFONTINFO;

#define MAXCOUNTE 20
typedef struct
{
    UINT  count;
    short x;
    short y;
    BOOL  fDisplay;
    BOOL  fShowOrHide;
    
    UINT  uCount;
    UINT  uCountForChange;
}CARETINFO;

/* Mouse event structure */
typedef struct _EVENT
{
    short     hotx;
    short     hoty;
    unsigned  fsBtn;
} EVENT;

/* Mouse pointer shape union containing structures for graphics and text */
typedef union _PTRSHAPE
{
    struct
    {
        unsigned char atScreen;
        unsigned char chScreen;
        unsigned char atCursor;
        unsigned char chCursor;
    } t;
    struct
    {
        unsigned xHot, yHot;
        unsigned afsPtr[32];
    } g;
} PTRSHAPE;

typedef struct
{
    int      fExist, fInit, fGraph;
    short    xVirtual,  yVirtual;
    short    xActual,   yActual;
    short    xLast,     yLast;
    unsigned fsBtnLast, cBtn;
}MOUSEINFO;

/* Values for SetPtrVis function */
typedef enum _PTRVIS { SHOW = 1, HIDE } PTRVIS;

/* Action codes for getkey */
enum WAITACTION { NO_WAIT, WAIT, CLEAR_WAIT, CLEAR };

/* Mouse events */
#define LEFT_DOWN   0x01        /* 0000 0010 Left button pressed    */
#define RIGHT_DOWN  0x02        /* 0000 1000 Right button pressed   */
#define MIDDLE_DOWN 0x04        /* 0010 0000 Middle button pressed  */

/* Mouse Shap */
#define MOUSE_ARROW   0x10
#define MOUSE_WAIT    0x11

#define RGB(r,g,b) (((long) ((b) << 8 | (g)) << 8) | (r))

/* Color definition */
#define COLOR_black         0
#define COLOR_blue          1
#define COLOR_cyan          2
#define COLOR_green         3
#define COLOR_yellow        4
#define COLOR_red           5
#define COLOR_magenta       6
#define COLOR_lightgray     7
#define COLOR_darkblue      8
#define COLOR_darkcyan      9
#define COLOR_darkgreen     10
#define COLOR_darkyellow    11
#define COLOR_darkred       12
#define COLOR_darkmagenta   13
#define COLOR_darkgray      14
#define COLOR_lightwhite    15

#define SB_HEIGHT     24

/* Function definitions */
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL FAR LockFont(void);
void FAR UnlockFont(void);

BOOL GUIAPI InitVirtualMemory(  unsigned int iMinKByte, unsigned int iMaxKByte );
BOOL GUIAPI InitGUI();
void GUIAPI TerminateGUI( void );
void GUIAPI DosShell(void);
void GUIAPI ExecuteExternProcFromGUI( PSTR cmdname );

BOOL GUIAPI Bitmap( LPCSTR lpFileName, short color, short lx, short by );
void GUIAPI cc_printf( LPCSTR lpCStr, short x, short y );
void GUIAPI cc_wt16( LPCSTR lpCStr, short x, short y );
void GUIAPI SetGrayTextMask();
void GUIAPI SetDisplayMask(BYTE* cMask);
void GUIAPI RemoveDisplayMask();
int GUIAPI GetOutWidth( LPCSTR lpCStr );
void GUIAPI set_cliprgn(short lx, short ty, short rx, short by);
void GUIAPI disp_byte(short x, short y, BYTE by);
void GUIAPI set_color(short color);
void GUIAPI def_GDC_reg( void );
void GUIAPI move_image(short srcx, short srcy, short srcw, short srch, short destx, short desty);

_vmhnd_t GUIAPI SaveImageUseVirtualMem(int x, int y, int iWidth, int iHeight);
void GUIAPI RestoreImageUseVirtualMem(_vmhnd_t hdlImage, int x, int y);

// about icon
_vmhnd_t GUIAPI GetIconImageHandler( void );
void GUIAPI DisplayIcon( LPSTR lpImage, short x, short y );

// about status bar
void GUIAPI DrawStatusBar( void );
void GUIAPI SetStatusBarInfo( int iSBIndex, PSTATUSBARDATA pSBData );

// about caret
BOOL GUIAPI IsCaretDisplaying( int* x, int* y );
void GUIAPI UndisplayCaret( void );
void GUIAPI SetCaretPos( short x, short y );
void GUIAPI FlashCaret();

// dos support              
void __interrupt __far Clock( void );
void __far CriticalErrorHandler( unsigned ax, unsigned di, unsigned __far* header);

//void __interrupt __far KBoard( void );
unsigned GUIAPI GetKey( int fWait );
void GUIAPI Beep( int frequency, int duration );
void GUIAPI Sleep( clock_t wait );

// about mouse functions
BOOL GUIAPI IsMouseAvailable();
int GUIAPI GetMouseEvent( EVENT _far *pEvent );
int GUIAPI GetPtrPos( EVENT _far *pEvent );
int GUIAPI SetPtrPos( short x, short y );
int GUIAPI SetPtrVis( PTRVIS pv );
int GUIAPI SetPtrShape( PTRSHAPE _far *ps );

#ifdef __cplusplus
}
#endif  /* __cplusplus */
