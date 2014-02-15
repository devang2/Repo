// MainGUI.C
//
// This file include menu functions and the core code of WYMGUI.
// 
// Copyright (c) 1995.8 ~ 1998.4, Mr. Wei Yongming.
//
// Last modified date: 1998.04.15.

#include <stdio.h>
#include <conio.h> 
#include <io.h>
#include <dos.h>
#include <bios.h>
#include <process.h>
#include <time.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>
#include <memory.h>
#include <Vmemory.h>
#include <string.h>
#include <direct.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include "Common.h"
#include "Support.h"
#include "MainGUI.h"
#include "Dialog.h"

extern STATUSBAR  SB[4];
extern struct _videoconfig vc;

const char szMiniGUI[] = "MiniGUI";
const char szDefaultLogo[] = "APP.BMP";

static PNORMALMENUITEM GetNormalMenuItem(PGUIINFO pGUIInfo, WORD wID);
static void RedrawPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iPrev, int iCur);
static void RedrawNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iPrev, int iCur, int x, int y, int iWidth);
static void CreateHiliteStringOfPopupMenu(PNORMALMENUITEM pNormalMenuItem, char* achHilite);
static void CreateHiliteStringOfMenuBar(PPOPUPMENUITEM pPopupMenuItem, char* achHilite);
static char GetHiliteAscii(PSTR pStr);
static int GetPositionOfPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iIndex);
static int GetPositionOfNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex, int y);
static int GetNextPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex);
static int GetPreviousPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex);
static int GetNumOfPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem);
static int GetNextNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex);
static int GetPreviousNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex);
static int GetNumOfNormalMenuItem(PNORMALMENUITEM pNormalMenuItem);
static int WhichItemMouseIn(PNORMALMENUITEM pNormalMenuItem, int top, int y);

/*
 * Function: PGUIINFO GUIPAI CreateMainGUI(char* spCaption, PPOPUPMENUITEM pPopupMenuItem,
                      PACCELTAB pAccelTab,
                      int (FAR * MainProc)(PGUIINFO, UINT, WPARAM, LPARAM));
 *      This function create main GUI:
 *      (1) Create the message queue;
 *      (2) Paint the menu bar and no client area;
 *      (3) Create the status bar.
 *
 * Parameters:
 *      spCaption: the caption of the GUI;
 *      pPopupMenuItem: pointer to the popupmenu;
 *      pAccelTab: pointer to the accelerator table;
 *      MainProc: the call back procedure that process message.
 * Return:
 *      success-----the pointer to the structure GUIINFO.
 *      failure-----NULL.
 *
 *  1995.8.9.AM.
 *
 */
PGUIINFO GUIAPI CreateMainGUI(const char* spCaption, 
                              PPOPUPMENUITEM pPopupMenuItem,
                              PACCELTAB pAccelTab,
                              int (FAR * MainProc)(PGUIINFO, UINT, WPARAM, LPARAM))
{
    PGUIINFO pGUIInfo;
    
    // Create the mesage queue
    if(!(pGUIInfo = (PGUIINFO)malloc(sizeof(GUIINFO))))
        return NULL;
    pGUIInfo->MQInfo.pMsg = NULL;
    if(!CreateMsgQueue(pGUIInfo, MAXQUEUELENGTH))
    {
        free(pGUIInfo);
        return NULL;
    }
    
    // set the GUIINFO structure.
    strncpy( pGUIInfo->spCaption, spCaption, CAPTIONLENGTH );
    pGUIInfo->pPopupMenuItem = pPopupMenuItem;
    pGUIInfo->pAccelTab = pAccelTab;
    pGUIInfo->MainProc = MainProc;
    
    SetPtrVis(SHOW);

    PostMessage(pGUIInfo, MSG_NCCREATE, (WPARAM)pGUIInfo, 0L);
    PostMessage(pGUIInfo, MSG_NCPAINT, 0, 0L);
    
    PostMessage(pGUIInfo, MSG_CREATE, (WPARAM)pGUIInfo, 0L);

    PostMessage(pGUIInfo, MSG_ERASEBKGND, (WPARAM)(&pGUIInfo->dc.clientrect), 0L);
    
    return pGUIInfo;
}

/*
 * Function: void GUIPAI DestroyMainGUI( PGUIINFO pGUIInfo );
 *      This function Destroy main GUI:
 *      (1) Release the message queue;
 *      (2) Release the space that was allocated to GUIINFO structure.
 *
 * Parameters:
 *      pGUIInfo: the pointer to the GUIINFO structure.
 * Return:
 *      None
 *
 *  1995.8.9.AM.
 *
 */
void GUIAPI DestroyMainGUI( PGUIINFO pGUIInfo )
{
    CreateMsgQueue(pGUIInfo, 0);
    free(pGUIInfo);
    SetPtrVis(HIDE);
}

int FAR DefaultGUIProc(PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static unsigned int uTimes = 767;
    
    switch(uMsg)
    {
        case MSG_NCCREATE:
            uTimes = 767;
            break;
            
        case MSG_NCPAINT:
            // Draw Menu
            DrawMenuBar(pGUIInfo->pPopupMenuItem);
            DrawNCArea(pGUIInfo);
            DrawStatusBar();
            break;
        
        case MSG_DESTROY:
            PostMessage(pGUIInfo, MSG_KILLFOCUS, 0, 0L);
            PostMessage(pGUIInfo, MSG_DESTROYPANES, 0, 0L);

            do
            {
                MSG msg;
                
                if(GetMessageFromQueue(pGUIInfo, &msg))
                {
                    TranslateAccelerator(pGUIInfo, &msg);
                    DisptchMessage(pGUIInfo, &msg);
                }
                else
                    break;
            }while(TRUE);
    
            PostMessage(pGUIInfo, MSG_QUIT, TRUE, 0L);
            break;
        
        case MSG_SETFOCUS: 
            DrawCaption(pGUIInfo, TRUE);
            break;
            
        case MSG_KILLFOCUS:
            DrawCaption(pGUIInfo, FALSE);
            break;
            
        case MSG_ERASEBKGND:
            {
                PRECT pRect = (PRECT)wParam;
                memcpy(&pGUIInfo->dc.cliprect, pRect, sizeof(RECT));
                
                SetPtrVis(HIDE);
                _setcolor(COLOR_darkcyan);
                _rectangle( _GFILLINTERIOR, pRect->left, pRect->top, 
                            pRect->right, pRect->bottom);
                SetPtrVis(SHOW);
                
                PostMessage(pGUIInfo, MSG_PAINT, (WPARAM)(&pGUIInfo->dc), 0L);
            }
            break;
            
        case MSG_IDLE:
            if(SB[0].uTimes != uTimes)
            {
                STATUSBARDATA SBData;
                SBData.fgcolor = 0;
                SBData.lpStr = "菜单：F10，<Alt+加速键>；主控界面：Alt+F4。";
                SetStatusBarInfo(0, &SBData);

                uTimes = SB[0].uTimes;
            }
            break;

        case MSG_RBUTTONDOWN:
            break;
        
        case MSG_LBUTTONDOWN:
            break;

        case MSG_CHAR:
            break;
        
        default:
            break;
    }
    
    return 0;
}

/* Function: void GUIAPI GetMessage(PMSG pMsg)
 *        This function get ascii code and key code.
 * Parameters: 
 *        pKeySqn: pointer to KEYSEQUENCE struct.
 * Return:
 *      None.
 *
 *  1995.8.9.AM.
 *
 */
void GUIAPI GetMessage(PMSG pMsg)
{
    UINT   uKey;
    EVENT  meEvent;
    static BOOL fLBtnDown = FALSE;
    static BOOL fRBtnDown = FALSE;
    
    if(uKey = GetKey(NO_WAIT))
    {
        pMsg->uMsg = KB_KEYDOWN;
        pMsg->wParam = uKey;
    }
    else if(GetMouseEvent(&meEvent))
    {   
        if( meEvent.fsBtn & LEFT_DOWN )
        {
            fLBtnDown = TRUE;
            pMsg->uMsg = ME_LBUTTONDOWN;
            pMsg->pt.x = meEvent.hotx;
            pMsg->pt.y = meEvent.hoty;
            return;
        }
        else if( fLBtnDown && !(meEvent.fsBtn & LEFT_DOWN) )
        {
            fLBtnDown = FALSE;
            pMsg->uMsg = ME_LBUTTONUP;
            pMsg->pt.x = meEvent.hotx;
            pMsg->pt.y = meEvent.hoty;
            return;
        }
        
        if( meEvent.fsBtn & RIGHT_DOWN )
        {
            fRBtnDown = TRUE;
            pMsg->uMsg = ME_RBUTTONDOWN;
            pMsg->pt.x = meEvent.hotx;
            pMsg->pt.y = meEvent.hoty;
            return;
        }
        else if( fRBtnDown && !(meEvent.fsBtn & RIGHT_DOWN) )
        {
            fRBtnDown = FALSE;
            pMsg->uMsg = ME_RBUTTONUP;
            pMsg->pt.x = meEvent.hotx;
            pMsg->pt.y = meEvent.hoty;
            return;
        }

        pMsg->uMsg = ME_MOVE;
        pMsg->pt.x = meEvent.hotx;
        pMsg->pt.y = meEvent.hoty;
        return;
    }
    else
        pMsg->uMsg = NULLINPUT;
    
    return;
}

/*
 * Function: BOOL GUIAPI CreateMsgQueue( PGUIINFO pGUIInfo, int cMsg )
 *      This function create a new message queue.
 * Parameters:
 *      cMsg: the longth of message queue.
 * Return:
 *      success-----TRUE.
 *      failure-----FALSE.
 *
 *  1995.8.9.AM.
 *
 */
BOOL GUIAPI CreateMsgQueue( PGUIINFO pGUIInfo, int cMsg )
{
    if(pGUIInfo->MQInfo.pMsg)
        free(pGUIInfo->MQInfo.pMsg);
    if(cMsg == 0)
        return TRUE;
        
    if(!(pGUIInfo->MQInfo.pMsg = (PMSG)malloc(cMsg*(sizeof(MSG)))))
        return FALSE;               // No enogh memory.
    
    pGUIInfo->MQInfo.cMsg  = cMsg;
    pGUIInfo->MQInfo.iFront = 0;
    pGUIInfo->MQInfo.iRear = 0;   // a empty queue.
    
    return TRUE;
}

/*
 * Function: BOOL GUIAPI PostMessage( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam )
 *      This function posts(places) a message in the message queue.
 *  queue.
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure.
 *      uMsg:   message to post
 *      wParam: first message parameter 
 *      lParam: seconde message parameter
 * Return:  
 *      success-----TRUE.
 *      failure-----FALSE.
 *
 *  1995.8.9.AM.
 *
 */
BOOL GUIAPI PostMessage( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    int iTemp;
    
    iTemp = pGUIInfo->MQInfo.iRear;
    pGUIInfo->MQInfo.iRear = (pGUIInfo->MQInfo.iRear + 1) % pGUIInfo->MQInfo.cMsg;
    if(pGUIInfo->MQInfo.iRear == pGUIInfo->MQInfo.iFront)
    {
        pGUIInfo->MQInfo.iRear = iTemp;
        return FALSE;               // The message queue is full.
    }
    
    (pGUIInfo->MQInfo.pMsg + pGUIInfo->MQInfo.iRear)->uMsg = uMsg;
    (pGUIInfo->MQInfo.pMsg + pGUIInfo->MQInfo.iRear)->wParam = wParam;
    (pGUIInfo->MQInfo.pMsg + pGUIInfo->MQInfo.iRear)->lParam = lParam;
    
    return TRUE;
}

/*
 * Function: BOOL GUIAPI GetMessageFromQueue( PGUIINFO pGUIInfo, PMSG pMsg )
 *      Using this function, you can get the message from the message queue.
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure.
 *      pMsg:    pinter to the message structure.
 * Return:  
 *      success-----TRUE.
 *      failure-----FALSE.  // No message in the queue.
 *
 *  1995.8.9.AM.
 *
 */
BOOL GUIAPI GetMessageFromQueue( PGUIINFO pGUIInfo, PMSG pMsg )
{
    if(pGUIInfo->MQInfo.iFront == pGUIInfo->MQInfo.iRear)
        return FALSE;               // There is no message in queue.
        
    pGUIInfo->MQInfo.iFront = (pGUIInfo->MQInfo.iFront + 1) % pGUIInfo->MQInfo.cMsg;
    pMsg->uMsg    = (pGUIInfo->MQInfo.pMsg + pGUIInfo->MQInfo.iFront)->uMsg;
    pMsg->wParam  = (pGUIInfo->MQInfo.pMsg + pGUIInfo->MQInfo.iFront)->wParam;
    pMsg->lParam  = (pGUIInfo->MQInfo.pMsg + pGUIInfo->MQInfo.iFront)->lParam;
    
    return TRUE;
}

/*
 * Function: BOOL GUIAPI PreProcMessage( PGUIINFO pGUIInfo, PMSG pMsg )
 *      This function pre-process the message:
 *      (1) If some event active the menu, then call TrackMenu function;
 *      (2) If message is other keyboard message, do nothing;
 *      (3) If message is other mouse message, Translate the mouse message.
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure.
 *      pMsg:    pinter to the message that will be processed.
 * Return:
 *      processed -----TRUE.
 *      have not processed-----FALSE.
 *
 *  1995.8.9.AM.
 *
 */
BOOL GUIAPI PreProcMessage( PGUIINFO pGUIInfo, PMSG pMsg, BOOL bNoTrackMenu )
{
    int iID;
    int iIndex;
    BOOL bCaretDisplaying;
    int iCaretX, iCaretY;
    
    bCaretDisplaying = IsCaretDisplaying(&iCaretX, &iCaretY);
    switch(pMsg->uMsg)
    {
        case KB_KEYDOWN:
            // See weather F10 key was pressed.
            if(pMsg->wParam == 0x0144 && !bNoTrackMenu)
            {
                if(bCaretDisplaying)
                    UndisplayCaret();
                    
                SendMessage(pGUIInfo, MSG_ACTIVEMENU, 0, 0L);
                set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);
                iID = TrackMenu(pGUIInfo->pPopupMenuItem, 0, FALSE);
                set_cliprgn(pGUIInfo->dc.clientrect.left, 
                    pGUIInfo->dc.clientrect.top,
                    pGUIInfo->dc.clientrect.right,
                    pGUIInfo->dc.clientrect.bottom);
                
                if( iID > 0)
                {
                    pMsg->uMsg = MSG_COMMAND;
                    pMsg->wParam = (WPARAM)iID;
                    pMsg->lParam = 0L;
                }
                else
                    pMsg->uMsg = MSG_NULL;

                if(bCaretDisplaying)
                    SetCaretPos(iCaretX, iCaretY);
            }
            // See if Alt + x key has pressed.
            else if((HIBYTE(pMsg->wParam)) == 0x04 && !bNoTrackMenu)
            {
                iIndex = CanActiveMenuByAltKey(pGUIInfo->pPopupMenuItem, (UINT)pMsg->wParam);
    
                if(bCaretDisplaying)
                    UndisplayCaret();
                    
                if(iIndex >= 0)
                {
                    SendMessage(pGUIInfo, MSG_ACTIVEMENU, iIndex, 0L);
                    set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);
                    iID = TrackMenu(pGUIInfo->pPopupMenuItem, iIndex, TRUE);
                    set_cliprgn(pGUIInfo->dc.clientrect.left, 
                        pGUIInfo->dc.clientrect.top,
                        pGUIInfo->dc.clientrect.right,
                        pGUIInfo->dc.clientrect.bottom);
                    if( iID > 0)
                    {
                        pMsg->uMsg = MSG_COMMAND;
                        pMsg->wParam = (WPARAM)iID;
                        pMsg->lParam = 0L;
                    }
                    else
                        pMsg->uMsg = MSG_SYSCHAR;
                }
                else
                    pMsg->uMsg = MSG_SYSCHAR;

                if(bCaretDisplaying)
                    SetCaretPos(iCaretX, iCaretY);
            }
            else
            {
                pMsg->uMsg = MSG_CHAR;
            }
            break;

        case ME_LBUTTONDOWN:
            iIndex = CanActiveMenu(pGUIInfo->pPopupMenuItem, &(pMsg->pt));
            if(iIndex >= 0)
            {
                if(bCaretDisplaying)
                    UndisplayCaret();
                    
                SendMessage(pGUIInfo, MSG_ACTIVEMENU, 0, 0L);
                set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);
                iID = TrackMenu(pGUIInfo->pPopupMenuItem, iIndex, TRUE);
                set_cliprgn(pGUIInfo->dc.clientrect.left, 
                    pGUIInfo->dc.clientrect.top,
                    pGUIInfo->dc.clientrect.right,
                    pGUIInfo->dc.clientrect.bottom);
                if( iID > 0)
                {
                    pMsg->uMsg = MSG_COMMAND;
                    pMsg->wParam = (WPARAM)iID;
                    pMsg->lParam = 0L;
                }
                else
                {
                    if(IsInClientArea(pGUIInfo, pMsg->pt))
                        pMsg->uMsg = MSG_LBUTTONDOWN;
                    else
                        pMsg->uMsg = MSG_NCLBUTTONDOWN;
                    pMsg->lParam = (LPARAM)(MAKELONG(pMsg->pt.x, pMsg->pt.y));
                }
            }
            else
            {
                if(IsInClientArea(pGUIInfo, pMsg->pt))
                    pMsg->uMsg = MSG_LBUTTONDOWN;
                else
                    pMsg->uMsg = MSG_NCLBUTTONDOWN;
                pMsg->lParam = (LPARAM)(MAKELONG(pMsg->pt.x, pMsg->pt.y));
            }
            break;

        case ME_LBUTTONUP:
            if(IsInClientArea(pGUIInfo, pMsg->pt))
                pMsg->uMsg = MSG_LBUTTONUP;
            else
                pMsg->uMsg = MSG_NCLBUTTONUP;
            pMsg->lParam = (LPARAM)(MAKELONG(pMsg->pt.x, pMsg->pt.y));
            break;
        
        case ME_RBUTTONUP:
            if(IsInClientArea(pGUIInfo, pMsg->pt))
                pMsg->uMsg = MSG_RBUTTONUP;
            else
                pMsg->uMsg = MSG_NCRBUTTONUP;
            pMsg->lParam = (LPARAM)(MAKELONG(pMsg->pt.x, pMsg->pt.y));
            break;
        
        case ME_RBUTTONDOWN:
            if(IsInClientArea(pGUIInfo, pMsg->pt))
                pMsg->uMsg = MSG_RBUTTONDOWN;
            else
                pMsg->uMsg = MSG_NCRBUTTONDOWN;
            pMsg->lParam = (LPARAM)(MAKELONG(pMsg->pt.x, pMsg->pt.y));
            break;
        
        case ME_MOVE:
            pMsg->uMsg = MSG_MOUSEMOVE;
            pMsg->lParam = (LPARAM)(MAKELONG(pMsg->pt.x, pMsg->pt.y));
            PostMessage(pGUIInfo, MSG_SETCURSOR, pMsg->wParam, pMsg->lParam);
            break;

        default:
            pMsg->uMsg = MSG_IDLE;
            pMsg->wParam = 0;
            pMsg->lParam = 0L;
            break;
            
    }

    PostMessage(pGUIInfo, pMsg->uMsg, pMsg->wParam, pMsg->lParam);
    return TRUE;
}

/*
 * Function: BOOL GUIAPI IsInClientArea( PGUIINFO pGUIInfo, POINT pt )
 *      This function determines weather the specified ponit pt is 
 *  in the client area.
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure;
 *      pt:       specified the point.
 * Return:
 *      TRUE: the point is in the client;
 *      FALSE: the point is not in the client.
 *
 *  1995.8.9.AM.
 *
 */
BOOL GUIAPI IsInClientArea( PGUIINFO pGUIInfo, POINT pt )
{
    return ((pt.x >= pGUIInfo->dc.clientrect.left) && 
                (pt.x <  pGUIInfo->dc.clientrect.right) &&
                (pt.y >= pGUIInfo->dc.clientrect.top) &&
                (pt.y <  pGUIInfo->dc.clientrect.bottom));
}

/*
 * Function: int  GUIAPI TranslateAccelerator( PGUIINFO pGUIInfo, PMSG pMsg);
 *      This function translate the accelerator to a MSG_COMMAND.
 *  
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure.
 *      pMsg:    pinter to the message that will be translated.
 *      
 * Return:
 *      if there is no message translated, return 0.
 *
 *  1995.8.9.AM.
 *
 */
int GUIAPI TranslateAccelerator( PGUIINFO pGUIInfo, PMSG pMsg)
{
    int  i = 0;
    BOOL fTranslated = FALSE;
    
    if(pMsg->uMsg == MSG_CHAR || pMsg->uMsg == MSG_SYSCHAR)
    {
        do
        {
            if((pGUIInfo->pAccelTab+i)->bEvent == LOBYTE(pMsg->wParam) &&
                ((pGUIInfo->pAccelTab+i)->fFlags & 0x0f) == HIBYTE(pMsg->wParam))
            {
                PNORMALMENUITEM pNormalMenuItem;

                pNormalMenuItem = GetNormalMenuItem(pGUIInfo, (pGUIInfo->pAccelTab+i)->wID);
                if(pNormalMenuItem)
                {
                    fTranslated = TRUE;
                    if(pNormalMenuItem->fItemFlags & MF_DISABLED)
                        break;
                }
                else
                    break;
                    
                pMsg->uMsg = MSG_COMMAND;
                pMsg->wParam = (pGUIInfo->pAccelTab+i)->wID;
                break;
            }
            if((pGUIInfo->pAccelTab+i)->bEvent & AF_END)
                break;
            i++;
        }while(TRUE);
    }
    
    if(fTranslated)
        return 1;
    return 0;
}

/*
 * Function: void GUIAPI DisptchMessage( PGUIINFO pGUIInfo, PMSG pMsg )
 *      This function disptch the specified message to the call back function.
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure.
 *      pMsg:    pinter to the message that will be disptched.
 * Return:
 *      None;
 *
 *  1995.8.9.AM.
 *
 */
void GUIAPI DisptchMessage( PGUIINFO pGUIInfo, PMSG pMsg )
{
    (*pGUIInfo->MainProc)(pGUIInfo, pMsg->uMsg, pMsg->wParam, pMsg->lParam);
}

/*
 * Function: void GUIAPI SendMessage( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam LPARAM lParam )
 *      This function send the specified message directly to the call back function.
 * Parameters:
 *      pGUIInfo: the pionter to the GUIINFO structure.
 *      uMsg:    the message that will be sent.
 *      wParam:  the WORD parameters of the message.
 *      lParam:  the LONG parameters of the message.
 * Return:
 *      None;
 *
 *  1997.7.7.AM.
 *
 */
void GUIAPI SendMessage( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    (*pGUIInfo->MainProc)(pGUIInfo, uMsg, wParam, lParam);
}

#define MN_BKCOLOR      15      // The background color of normal menu item.
#define MN_FGCOLOR      0       // The foreground color of normal menu item.
#define MA_BKCOLOR      8       // The background color of actived menu item.
#define MA_FGCOLOR      15      // The foreground color of actived menu item.
#define MD_FGCOLOR      14      // The foreground color of disabled menu item.
#define MN_HEIGHT       18      // The height of normal menu item.
#define MB_HEIGHT       4       // The height of menu break.
#define MB_WIDTH        20      // The width of menu bar break.
#define MB_START_WIDTH  4
#define MP_START_WIDTH  8       // The width before start bar menu item.
#define MP_END_WIDTH    8       // The width after end bar menu item.
#define MN_START_WIDTH  16      // The width before start normal menu item.
#define MN_END_WIDTH    16      // The width after end normal menu item.

/*
 * Function: void GUIAPI DrawNCArea( PGUIINFO pGUIInfo )
 *      This function Draw the none clietn area, and fill the client rect of
 *  pGUIInfo
 * Parameters:
 * Return:
 *      None;
 *
 *  1995.8.9.AM.
 *
 */
void GUIAPI DrawNCArea( PGUIINFO pGUIInfo )
{
    pGUIInfo->dc.clientrect.left = 0;
    pGUIInfo->dc.clientrect.right = vc.numxpixels - 1;
    pGUIInfo->dc.clientrect.top = CP_HEIGHT + MN_HEIGHT + 2;
    pGUIInfo->dc.clientrect.bottom = vc.numypixels - SB_HEIGHT - 1;

    PostMessage(pGUIInfo, MSG_SETFOCUS, 0, 0L);
}

/*
 * Function: void GUIAPI DrawCaption( PGUIINFO pGUIInfo, BOOL fSetFocus)
 *      This function disptch the specified message to the call back function.
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
void GUIAPI DrawCaption( PGUIINFO pGUIInfo, BOOL fSetFocus)
{
    int bkcolor;
    int fgcolor;
    int x;

    bkcolor = fSetFocus?CP_BKCOLOR:CD_BKCOLOR;
    fgcolor = fSetFocus?CP_FGCOLOR:CD_FGCOLOR;

    // Draw caption
    set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);

	SetPtrVis(HIDE);
    _setcolor(bkcolor);
    _rectangle(_GFILLINTERIOR, 0, 0, vc.numxpixels - 1, CP_HEIGHT - 1);

    // Draw copyright information.
    if( !Bitmap(szDefaultLogo, 5, 16, 16) )
    {
        set_color( 5 );
        cc_wt16(szMiniGUI, 16, 0);
    }

    set_color(fgcolor);
    x = (vc.numxpixels - GetOutWidth(pGUIInfo->spCaption))/2;
    cc_wt16(pGUIInfo->spCaption, x, 0);

    _setcolor(0);
    _moveto(0, CP_HEIGHT - 1);
    _lineto(vc.numxpixels - 1, CP_HEIGHT - 1);
	SetPtrVis(SHOW);

    set_cliprgn(pGUIInfo->dc.clientrect.left,
                pGUIInfo->dc.clientrect.top,
                pGUIInfo->dc.clientrect.right,
                pGUIInfo->dc.clientrect.bottom);
}
    
/*
 * Function: void GUIAPI DrawMenuBar( PPOPUPMENUITEM pPopupMenuItem )
 *      This function disptch the specified message to the call back function.
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
void GUIAPI DrawMenuBar( PPOPUPMENUITEM pPopupMenuItem )
{
    int x = MB_START_WIDTH;
    PPOPUPMENUITEM pBuffer = pPopupMenuItem;
    
    SetPtrVis(HIDE);

    // Fill background.
    _setcolor(MN_BKCOLOR);
    _rectangle( _GFILLINTERIOR, 0, CP_HEIGHT, vc.numxpixels, CP_HEIGHT + MN_HEIGHT);
    _setcolor(0);
    _moveto(0, CP_HEIGHT + MN_HEIGHT + 1);
    _lineto(vc.numxpixels - 1, CP_HEIGHT + MN_HEIGHT + 1);
    
    // Draw bar menu item.
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBARBREAK)
            x += MB_WIDTH;
        else
        {
            int iWidth = GetOutWidth(pBuffer->spItemText) 
                    + MP_START_WIDTH + MP_END_WIDTH;
            DrawMenuItem(pBuffer->spItemText, FALSE, pBuffer->fItemFlags, x, CP_HEIGHT, iWidth);
            x += iWidth;
        }
        
        if(pBuffer->fItemFlags & MF_END)
            break;
            
        pBuffer++;
        
    }while(TRUE);
        
    SetPtrVis(SHOW);
}

#define KB_LEFT     0x014b
#define KB_RIGHT    0x014d
#define KB_UP       0x0148
#define KB_DOWN     0x0150
#define KB_ENTER    0x000d
#define KB_ESC      0x001b
#define KB_ALT      0x0800
#define MAXITEM     20

#define NEXTPOPUPMENU       -1
#define PREVIOUSPOPUPMENU   -2
#define MOUSEDETERMINE      -3
#define INVALIDCHOICE       -4
#define NULLCHOICE          -5
#define ENDTRACKMENU        -6

/*
 * Function: int GUIAPI TrackMenu( PPOPUPMENUITEM pPopupMenuItem, int iIndex, BOOL fPopup )
 *      This function track menu by user.
 * Parameters:
 *      
 * Return:
 *      >= 0: give the select menu item's identification;
 *      <  0: none menu item or invalid menu item was selected.
 */
int GUIAPI TrackMenu( PPOPUPMENUITEM pPopupMenuItem, int iIndex, BOOL fPopup )
{
    unsigned    uKey;       // unsigned key code
    EVENT       meEvent;
    BOOL fReturn = FALSE;
    static BOOL fBtnDown = FALSE;
    int x, y;
    int iWidth;
    int iID = -1;
    int iPrev;
    int iCur;
    POINT pt;
    char        achHilite[MAXITEM];
    char*       pchT;      // Temporary character pointer
    
    CreateHiliteStringOfMenuBar(pPopupMenuItem, achHilite);
    
    // Track menu bar.
    iPrev = -1;
    iCur = iIndex;
    
    GetKey(CLEAR);
    
    while( TRUE )
    {
        if(iPrev != iCur) // We must redisplay the popup menu item.
        {
            RedrawPopupMenuItem(pPopupMenuItem, iPrev, iCur);
            iPrev = iCur;
        }
        
        if(!fPopup)
        {
            if( uKey = GetKey( NO_WAIT ) )
            {
                switch( uKey )
                {
                    case KB_UP:              // Up key
                    case KB_DOWN:            // Down key
                    case KB_ENTER:
                        fPopup = TRUE;
                        break;
                    case KB_RIGHT:              // Right key
                        iCur = GetNextPopupMenu(pPopupMenuItem, iCur);
                        break;
                    case KB_LEFT:            // Left key
                        iCur = GetPreviousPopupMenu(pPopupMenuItem, iCur);
                        break;
                    case KB_ALT:
                    case KB_ESC:
                        iCur = -1;
                        fReturn = TRUE;
                        break;
                    default:
                        if( uKey > 256 )    // Ignore unknown function key
                            break;
                        pchT = strchr( achHilite, (char)tolower( uKey ) );
                        if( pchT != NULL )  // If in highlight string, evaluate
                        {
                            iCur = pchT - achHilite;
                            if(!((pPopupMenuItem+iCur)->fItemFlags & MF_DISABLED))
                            {
                                RedrawPopupMenuItem(pPopupMenuItem, iPrev, iCur);
                                iPrev = iCur;
                                fPopup = TRUE;
                            }
                            break;
                        }
                        else
                            break;       // Ignore unkown ASCII key
                }
            }
            else if( GetMouseEvent( &meEvent ) )
            {
                pt.x = meEvent.hotx;
                pt.y = meEvent.hoty;
            
                if( meEvent.fsBtn & LEFT_DOWN )
                {
                    fBtnDown = TRUE;
                    iCur = CanActiveMenu(pPopupMenuItem, &pt);
                    if(iCur < 0)
                        fReturn = TRUE;
                    else
                        fPopup = TRUE;
                }
                else if( fBtnDown && !(meEvent.fsBtn & LEFT_DOWN) )
                {
                    fBtnDown = FALSE;
                }
            }
        }
        
        if(iPrev != iCur) // We must redisplay the popup menu item.
        {
            RedrawPopupMenuItem(pPopupMenuItem, iPrev, iCur);
            iPrev = iCur;
        }
        
        if(fPopup)
        {
            // Get the position of popup menu.
            x = GetPositionOfPopupMenuItem(pPopupMenuItem, iCur);
            iWidth = GetPopupMenuWidth(pPopupMenuItem + iCur);
            if((x + iWidth + 2) > vc.numxpixels)
                x = vc.numxpixels - iWidth - 2;
            y = CP_HEIGHT + MN_HEIGHT;

            iID = TrackPopupMenu(pPopupMenuItem, iCur, x, y, &pt);
            if( iID > 0)
            {
                iCur = -1;
                fReturn = TRUE;
            }
            else
            {
                switch( iID )
                {
                    case NEXTPOPUPMENU:
                        iCur = GetNextPopupMenu(pPopupMenuItem, iCur);
                        fPopup = TRUE;
                        break;
                    case PREVIOUSPOPUPMENU:
                        iCur = GetPreviousPopupMenu(pPopupMenuItem, iCur);
                        fPopup = TRUE;
                        break;
                    case MOUSEDETERMINE:
                        iCur = CanActiveMenu(pPopupMenuItem, &pt);
                        if(iCur < 0)
                            fReturn = TRUE;
                        else
                            fPopup = TRUE;
                        break;
                    case INVALIDCHOICE: // selected a disabled menu item.
                    case ENDTRACKMENU:       // Alt pressed
                        iCur = -1;
                        fReturn = TRUE;
                        break;
                    case NULLCHOICE:    // ESC pressed
                        fPopup = FALSE;
                        break;
                }
            }
        }
        
        if(fReturn)
        {
            RedrawPopupMenuItem(pPopupMenuItem, iPrev, iCur);
            return iID;
        }
    }
}
        
/*
 * Function: static void RedrawPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iPrev, int iCur)
 * Parameters:
 * Return:
 *      None;
 */
static void RedrawPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iPrev, int iCur)
{
    int x, iWidth;
    STATUSBARDATA SBData;
    
    SBData.fgcolor = 0;
    
    SetPtrVis(HIDE);

    if( iPrev >= 0)
    {
        iWidth = MP_START_WIDTH + MP_END_WIDTH;
        x = GetPositionOfPopupMenuItem(pPopupMenuItem, iPrev);
        iWidth += GetOutWidth((pPopupMenuItem + iPrev)->spItemText);
        DrawMenuItem((pPopupMenuItem + iPrev)->spItemText, 
                FALSE,
                (pPopupMenuItem + iPrev)->fItemFlags,
                x, CP_HEIGHT,
                iWidth) ;
    }

    if( iCur >= 0)
    {
        iWidth = MP_START_WIDTH + MP_END_WIDTH;
        x = GetPositionOfPopupMenuItem(pPopupMenuItem, iCur);
        iWidth += GetOutWidth((pPopupMenuItem + iCur)->spItemText);
        DrawMenuItem((pPopupMenuItem + iCur)->spItemText, 
                TRUE,
                (pPopupMenuItem + iCur)->fItemFlags,
                x, CP_HEIGHT,
                iWidth);
    }
    
    if(iCur >= 0)
    {
        SBData.lpStr = (LPSTR)((pPopupMenuItem + iCur)->spPromptInfo);
        SetStatusBarInfo( 0, &SBData );
    }
    else
        SetStatusBarInfo( 0, NULL );

    SetPtrVis(SHOW);
}

/*
 * Function: int GUIAPI TrackPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex, int x, int y, PPOINT pPt)
 * Parameters:
 * Return:
 *      None;
 */
int GUIAPI TrackPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex, int x, int y, PPOINT pPt)
{
    unsigned    uKey;       // unsigned key code
    int         iPrev;      // Previous index
    int         iCur;       // Current index
    EVENT       meEvent;
    static BOOL  fBtnDown = FALSE;
    static BOOL  fBtnDownA = FALSE;
    PNORMALMENUITEM pBuffer;
    int         iID;
    BOOL        fReturn = FALSE;
    int         iWidth;
    int         iHeight;
    char        achHilite[MAXITEM];
    char*       pchT;      // Temporary character pointer
    _vmhnd_t    hdlImage;
    
    pBuffer = (pPopupMenuItem + iIndex)->pNormalMenuItem;
    iWidth = GetPopupMenuWidth(pPopupMenuItem + iIndex);
    iHeight = GetPopupMenuHeight(pPopupMenuItem + iIndex);
    CreateHiliteStringOfPopupMenu(pBuffer, achHilite);
    hdlImage = SaveImageUseVirtualMem(x - 1, y - 1, iWidth + 3, iHeight + 3);
    
    // Draw popup menu.
    DrawPopupMenu(pPopupMenuItem + iIndex, x, y);
        
    iPrev = -1;
    iCur = 0;
    
    GetKey(CLEAR);
    
    while( TRUE )
    {
        // Redisplay current and previous if we get here through arrow
        // move or drag.
        if(iPrev != iCur)
        {
            RedrawNormalMenuItem(pBuffer, iPrev, iCur, x, y, iWidth);
            iPrev = iCur;
            
            if(iCur < 0)
            {
                STATUSBARDATA SBData;
    
                SetPtrVis(HIDE);
                SBData.fgcolor = 0;
                SBData.lpStr = (LPSTR)((pPopupMenuItem + iIndex)->spPromptInfo);

                SetStatusBarInfo( 0, &SBData );
                SetPtrVis(SHOW);
            }
        }

        if( uKey = GetKey( NO_WAIT ) )
        {
            switch( uKey )
            {
                case KB_UP:              // Up key
                    iCur = GetPreviousNormalMenuItem(pBuffer, iCur);
                    break;
                case KB_DOWN:            // Down key
                    iCur = GetNextNormalMenuItem(pBuffer, iCur);
                    break;
                case KB_RIGHT:              // Right key
                    iID = NEXTPOPUPMENU;
                    fReturn = TRUE;
                    break;
                case KB_LEFT:            // Left key
                    iID = PREVIOUSPOPUPMENU;
                    fReturn = TRUE;
                    break;
                case KB_ESC:
                    iID = NULLCHOICE;
                    fReturn = TRUE;
                    break;
                case KB_ALT:
                    iID = ENDTRACKMENU;
                    fReturn = TRUE;
                    break;
                case KB_ENTER:
                    if((pBuffer+iCur)->fItemFlags & MF_DISABLED)
                        iID = INVALIDCHOICE;
                    else
                        iID = (pBuffer+iCur)->iMenuID;
                    fReturn = TRUE;
                    break;
                default:
                    if( uKey > 256 )    // Ignore unknown function key
                        continue;
                    pchT = strchr( achHilite, (char)tolower( uKey ) );
                    if( pchT != NULL )  // If in highlight string, evaluate
                    {
                        iCur = pchT - achHilite;// and fall through
                        if((pBuffer+iCur)->fItemFlags & MF_DISABLED)
                            iID = INVALIDCHOICE;
                        else
                            iID = (pBuffer+iCur)->iMenuID;
                        fReturn = TRUE;
                    }
                    else
                        continue;       // Ignore unkown ASCII key
                break;
            }
        }
        else if( GetMouseEvent( &meEvent ) )
        {
            pPt->x = meEvent.hotx;
            pPt->y = meEvent.hoty;
            
            // If mouse is on the menu, respond to various events.
            if( (pPt->x >= x) && (pPt->x < (x + iWidth))
                &&(pPt->y >= y) && (pPt->y < (y + iHeight)) )
            {
                // If button is down, drag selection.
                if( meEvent.fsBtn & LEFT_DOWN )
                {
                    fBtnDown = TRUE;
                    iCur = WhichItemMouseIn(pBuffer, y, pPt->y);
                }
                // If button goes up from down, select current.
                else if( fBtnDown && !(meEvent.fsBtn & LEFT_DOWN) )
                {
                    fBtnDown = FALSE;
                    iCur = WhichItemMouseIn(pBuffer, y, pPt->y);
                    if((pBuffer+iCur)->fItemFlags & MF_DISABLED || iCur < 0)
                        iCur = 0;
                    else
                    {
                        iID = (pBuffer+iCur)->iMenuID;
                        fReturn = TRUE;
                    }
                }
                // Ignore if no button has been pushed.
                else
                    continue;
            }
            else
            {
                int lx = GetPositionOfPopupMenuItem(pPopupMenuItem, iIndex);
                int rx = lx + GetOutWidth((pPopupMenuItem + iIndex)->spItemText);
                rx += MP_START_WIDTH + MP_END_WIDTH;

                if( meEvent.fsBtn & LEFT_DOWN )
                {
                    fBtnDownA = TRUE;
                    if((pPt->x >= lx) && (pPt->x < rx)
                        && (pPt->y >= CP_HEIGHT) && (pPt->y < y) )
                        iCur = -1;
                    else if((pPt->y < y) && (pPt->y >= CP_HEIGHT))
                    {
                        iID = MOUSEDETERMINE;
                        fReturn = TRUE;
                    }
                    else
                        iCur = -1;
                }
                // If button goes up from down
                else if( fBtnDownA && !(meEvent.fsBtn & LEFT_DOWN) )
                {
                    fBtnDownA = FALSE;
                    if((pPt->x >= lx) && (pPt->x < rx)
                        && (pPt->y >= CP_HEIGHT) && (pPt->y < y) )
                        iCur = 0;
                    else
                    {
                        iID = INVALIDCHOICE;
                        fReturn = TRUE;
                    }
                }
                else
                    continue;
            }
        }
        else
            continue;
        
        if(fReturn)
            break;
    }

    if(hdlImage != _VM_NULL)
        RestoreImageUseVirtualMem(hdlImage, x - 1, y - 1);
    
    return iID;
}

/*
 * Function: static void RedrawNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iPrev, int iCur, int x, int y, int iWidth)
 * Parameters:
 * Return:
 *      None;
 */
static void RedrawNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iPrev, int iCur, int x, int y, int iWidth)
{
    int iPos;
    STATUSBARDATA SBData;
    
    SBData.fgcolor = 0;
    SetPtrVis(HIDE);
    if( iPrev >= 0)
    {
        iPos = GetPositionOfNormalMenuItem(pNormalMenuItem, iPrev, y);
        DrawMenuItem((pNormalMenuItem + iPrev)->spItemText, FALSE, (pNormalMenuItem + iPrev)->fItemFlags, x, iPos, iWidth);
    }
    
    if( iCur >= 0)
    {
        iPos = GetPositionOfNormalMenuItem(pNormalMenuItem, iCur, y);
        DrawMenuItem((pNormalMenuItem + iCur)->spItemText, TRUE, (pNormalMenuItem + iCur)->fItemFlags, x, iPos, iWidth);
    }

    if(iCur >= 0)
    {
        SBData.lpStr = (LPSTR)((pNormalMenuItem + iCur)->spPromptInfo);
        SetStatusBarInfo( 0, &SBData );
    }

    SetPtrVis(SHOW);
}

/*
 * Function: static viod CreateHiliteStringOfPopupMenu(PNORMALMENUITEM pNormalMenuItem, char* achHilite)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static void CreateHiliteStringOfPopupMenu(PNORMALMENUITEM pNormalMenuItem, char* achHilite)
{
    int i = 0;
    PNORMALMENUITEM pBuffer = pNormalMenuItem;
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBREAK)
            *(achHilite + i) = '\v';
        else
            *(achHilite + i) = GetHiliteAscii(pBuffer->spItemText);
        
        if(pBuffer->fItemFlags & MF_END)
            break;

        pBuffer++;
        i++;
        
    }while(TRUE);

    *(achHilite + i + 1) = '\0';
}

/*
 * Function: static viod CreateHiliteStringOfMenuBar(PPOPUPMENUITEM pPopupMenuItem, char* achHilite)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static void CreateHiliteStringOfMenuBar(PPOPUPMENUITEM pPopupMenuItem, char* achHilite)
{
    int i = 0;
    PPOPUPMENUITEM pBuffer = pPopupMenuItem;
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBARBREAK)
            *(achHilite + i) = '\v';
        else
            *(achHilite + i) = GetHiliteAscii(pBuffer->spItemText);
        
        if(pBuffer->fItemFlags & MF_END)
            break;

        pBuffer++;
        i++;
    
    }while(TRUE);
    
    *(achHilite + i + 1) = '\0';
}

/*
 * Function: static char GetHiliteAscii(PSTR pStr)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static char GetHiliteAscii(PSTR pStr)
{
    int i = 0;
    char cBuff;
    
    do
    {
        if(*(pStr + i) == '(')
        {
            cBuff = *(pStr + i + 1);
            return tolower(cBuff);
        }
        
        if(*(pStr + i) == '\0')
            return '\0';

        i++;
    }while(TRUE);
}

/*
 * Function: static int GetPositionOfPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
 * Parameters:
 * Return:
 *      x;
 *
 * 1995.8.9.AM.
 *
 */
static int GetPositionOfPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
{
    int i = 0;
    int x = MB_START_WIDTH;
    PPOPUPMENUITEM pBuffer = pPopupMenuItem;
    
    do
    {
        if(i == iIndex)
            break;
        if(pBuffer->fItemFlags & MF_END)
            break;
            
        if(pBuffer->fItemFlags & MF_MENUBARBREAK)
            x += MB_WIDTH;
        else
        {
            x += MP_START_WIDTH;
            x += GetOutWidth(pBuffer->spItemText);
            x += MP_END_WIDTH;
        }
        
        pBuffer++;
        i++;
    }while(TRUE);
    
    return x;
}

/*
 * Function: static int GetPositionOfNormalMenuItem(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
 * Parameters:
 * Return:
 *      y
 *
 * 1995.8.9.AM.
 *
 */
static int GetPositionOfNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex, int y)
{
    int i = 0;
    PNORMALMENUITEM pBuffer = pNormalMenuItem;
    
    do
    {
        if(i == iIndex)
            break;
        if(pBuffer->fItemFlags & MF_END)
            break;
            
        if(pBuffer->fItemFlags & MF_MENUBREAK)
            y += MB_HEIGHT*2 + 1;
        else
            y += MN_HEIGHT;
        
        pBuffer++;
        i++;
    }while(TRUE);
    
    return y;
}

/*
 * Function: static int GetNextPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int GetNextPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
{
    int iNum;
    iNum = GetNumOfPopupMenuItem(pPopupMenuItem);
    
    iIndex ++;
    if(iIndex == iNum)
        iIndex = 0;
    
    if((pPopupMenuItem + iIndex)->fItemFlags & MF_MENUBARBREAK)
        iIndex = GetNextPopupMenu(pPopupMenuItem, iIndex);

    return iIndex;
}

/*
 * Function: static int GetPreviousPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int GetPreviousPopupMenu(PPOPUPMENUITEM pPopupMenuItem, int iIndex)
{
    int iNum;
    iNum = GetNumOfPopupMenuItem(pPopupMenuItem);
    
    if(iIndex == 0)
        iIndex = iNum;

    iIndex--;
    
    if((pPopupMenuItem + iIndex)->fItemFlags & MF_MENUBARBREAK)
        iIndex = GetPreviousPopupMenu(pPopupMenuItem, iIndex);

    return iIndex;
}

/*
 * Function: static int GetNumOfPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem);
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int GetNumOfPopupMenuItem(PPOPUPMENUITEM pPopupMenuItem)
{
    int iNum = 0;
    PPOPUPMENUITEM pBuffer = pPopupMenuItem;
    
    do
    {
        iNum++;
        if(pBuffer->fItemFlags & MF_END)
            break;
            
        pBuffer++;
    }while(TRUE);
    
    return iNum;
}

/*
 * Function: static int GetNextNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int GetNextNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex)
{
    int iNum;
    iNum = GetNumOfNormalMenuItem(pNormalMenuItem);
    
    iIndex ++;
    if(iIndex == iNum)
        iIndex = 0;
    
    if((pNormalMenuItem + iIndex)->fItemFlags & MF_MENUBREAK)
        iIndex = GetNextNormalMenuItem(pNormalMenuItem, iIndex);

    return iIndex;
}

/*
 * Function: static int GetPreviousNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int GetPreviousNormalMenuItem(PNORMALMENUITEM pNormalMenuItem, int iIndex)
{
    int iNum;
    iNum = GetNumOfNormalMenuItem(pNormalMenuItem);
    
    if(iIndex == 0)
        iIndex = iNum;

    iIndex--;
    
    if((pNormalMenuItem + iIndex)->fItemFlags & MF_MENUBREAK)
        iIndex = GetPreviousNormalMenuItem(pNormalMenuItem, iIndex);

    return iIndex;
}

/*
 * Function: static int GetNumOfNormalMenuItem(PNORMALMENUITEM pNormalMenuItem);
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int GetNumOfNormalMenuItem(PNORMALMENUITEM pNormalMenuItem)
{
    int iNum = 0;
    PNORMALMENUITEM pBuffer = pNormalMenuItem;
    
    do
    {
        iNum++;
        if(pBuffer->fItemFlags & MF_END)
            break;
            
        pBuffer++;
    }while(TRUE);
    
    return iNum;
}

/*
 * Function: void GUIAPI DrawPopupMenu( PPOPUPMENUITEM pPopupMenuItem, int x, int y )
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
void GUIAPI DrawPopupMenu( PPOPUPMENUITEM pPopupMenuItem, int x, int y )
{
    int iWidth, iHeight;
    PNORMALMENUITEM pBuffer = pPopupMenuItem->pNormalMenuItem;
    
    iWidth = GetPopupMenuWidth(pPopupMenuItem);
    iHeight = GetPopupMenuHeight(pPopupMenuItem);

    SetPtrVis(HIDE);

    // Fill background and draw the border.
    _setcolor(MN_BKCOLOR);
    _rectangle(_GFILLINTERIOR, x, y, x + iWidth, y + iHeight);
    _setcolor(0);
    _rectangle(_GBORDER, x - 1, y - 1, x + iWidth + 1, y + iHeight + 1);
    _moveto(x, y + iHeight + 2);
    _lineto(x + iWidth + 2, y + iHeight + 2);
    _lineto(x + iWidth + 2, y);
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBREAK)
        {
            _setcolor(0);
            _moveto(x, y + MB_HEIGHT);
            _lineto(x + iWidth, y + MB_HEIGHT);
            y += MB_HEIGHT*2 + 1;
        }
        else
        {
            DrawMenuItem(pBuffer->spItemText, FALSE, pBuffer->fItemFlags, x, y, iWidth);
            y += MN_HEIGHT;
        }

        if(pBuffer->fItemFlags & MF_END)
            break;
            
        pBuffer++;
    }while(TRUE);
    
    SetPtrVis(SHOW);
}

/*
 * Function: int GUIAPI GetPopupMenuWidth( PPOPUPMENUITEM pPopupMenuItem )
 * Parameters:
 * Return:
 *      None.
 *
 * 1995.8.9.AM.
 *
 */
int GUIAPI GetPopupMenuWidth( PPOPUPMENUITEM pPopupMenuItem )
{
    int iWidth = 0;
    int x;
    PNORMALMENUITEM pBuffer = pPopupMenuItem->pNormalMenuItem;
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBREAK)
        {
            pBuffer++;
            continue;
        }
            
        x = GetOutWidth(pBuffer->spItemText);
        if(iWidth < x)
            iWidth = x;
        if(pBuffer->fItemFlags & MF_END)
            break;
        
        pBuffer++;
    }while(TRUE);
    
    return iWidth + MN_START_WIDTH + MN_END_WIDTH;
}

/*
 * Function: int GUIAPI GetPopupMenuHeight( PPOPUPMENUITEM pPopupMenuItem )
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
int GUIAPI GetPopupMenuHeight( PPOPUPMENUITEM pPopupMenuItem )
{
    int iHeight = 0;
    PNORMALMENUITEM pBuffer = pPopupMenuItem->pNormalMenuItem;
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBREAK)
            iHeight += MB_HEIGHT*2 + 1;
        else
            iHeight += MN_HEIGHT;

        if(pBuffer->fItemFlags & MF_END)
            break;
        
        pBuffer++;
    }while(TRUE);
    
    return iHeight;
}

/*
 * Function: void GUIAPI DrawMenuItem( PSTR pStr, WORD fFlags, int x, int y, int iWidth)
 *      This function draw the menu item in the specified position with
 *  the specified flags.
 * Parameters:
 *      pStr:   pointer to the item text;
 *      fFlags: menu item's flags;
 *      x, y:   specified the position;
 *      iWidth: the width of menu item.
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
void GUIAPI DrawMenuItem( PSTR pStr, BOOL fHilite, WORD fFlags, int x, int y, int iWidth )
{
    char buffer[5];
    
    if((fFlags & MF_MENUBARBREAK) || (fFlags & MF_MENUBREAK))
        return;
        
    if(fHilite)
    {
        // Fill actived menu item's background.
        _setcolor(MA_BKCOLOR);
        _rectangle(_GFILLINTERIOR, x, y, x + iWidth, y + MN_HEIGHT);
    }
    else
    {
        _setcolor(MN_BKCOLOR);
        _rectangle(_GFILLINTERIOR, x, y, x + iWidth, y + MN_HEIGHT);
    }
    
    
    if(fFlags & MF_DISABLED)
        set_color(MD_FGCOLOR);
    else if(fHilite)
        set_color(MA_FGCOLOR);
    else
        set_color(MN_FGCOLOR);
    
    if(fFlags & MF_POPUP)
        x += MP_START_WIDTH;
    else
        x += MN_START_WIDTH;
        
    if(fFlags & MF_DISABLED)
        SetGrayTextMask();
    cc_printf(pStr, x, y);
    if(fFlags & MF_DISABLED)
        RemoveDisplayMask();
    
    if(!(fFlags & MF_POPUP))
        if(fFlags & MF_CHECKED)
        {
            strcpy(buffer, "√");
            cc_wt16(buffer, x - MN_START_WIDTH, y);
        }
    
}

/*
 * Function: int GUIAPI CanActiveMenuByAltKey(PPOPUPMENUITEM pPopupMenuItem, UINT uKey)
 *      This function determines whether the specified Alt + x Key can 
 *  active the menu.
 * Parameters:
 *      pPopupMenuItem: the pionter to the POPUPMENUITEM structure;
 *      uKey:           the specified key ;
 * Return:
 *      < 0: can not;
 *      >= 0: can active menu and give the index of the popup menu item.
 *
 * 19956.12.5.pM.
 *
 */
int GUIAPI CanActiveMenuByAltKey(PPOPUPMENUITEM pPopupMenuItem, UINT uKey)
{
    char  achHilite[MAXITEM];
    char* pchT;
    char  zChar;
    BYTE  bScanCode;
    int   i, iIndex;
    BYTE  bAllScanCode[] ={ 0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24,
                            0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1F, 0x14,
                            0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C
                          };
    
    CreateHiliteStringOfMenuBar(pPopupMenuItem, achHilite);
    
    // Convete scan code to ASCII code.
    bScanCode = LOBYTE(uKey);
    iIndex = -1;
    for(i=0; i<26; i++)
    {
        if(bScanCode == bAllScanCode[i])
        {
            iIndex = i;
            break;
        }
    }
    
    if(iIndex == -1)
        return -1;
    
    zChar = (char)('a' + iIndex);
    pchT = strchr( achHilite, zChar);
    if( pchT != NULL )  // If in highlight string, evaluate
    {
        return (pchT - achHilite);
    }
    
    return -1;
}

/*
 * Function: int GUIAPI CanActiveMenu( PPOPUPMENUITEM pPopupMenuItem, PPOINT pPt )
 *      This function determines weather the specified ponit pt can 
 *  active the menu.
 * Parameters:
 *      pPopupMenuItem: the pionter to the POPUPMENUITEM structure;
 *      :       specified the point.
 * Return:
 *      < 0: can not;
 *      >= 0: can active menu and give the index of the popup menu item.
 *
 * 1995.8.9.AM.
 *
 */
int GUIAPI CanActiveMenu( PPOPUPMENUITEM pPopupMenuItem, PPOINT pPt )
{
    int left = 0, right = 0;
    int i = 0;
    PPOPUPMENUITEM pBuffer = pPopupMenuItem;
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBARBREAK)
            right += MB_WIDTH;
        else
        {
            right += MP_START_WIDTH;
            right += GetOutWidth(pBuffer->spItemText);
            right += MP_END_WIDTH;
        }
            
        if( (pPt->y > CP_HEIGHT) 
            && (pPt->y <= CP_HEIGHT + MN_HEIGHT)
            && (pPt->x > left)
            && (pPt->x <= right))
        {
            if(pBuffer->fItemFlags & MF_MENUBARBREAK)
                return FAILURE;
            return i;
        }

        if(pBuffer->fItemFlags & MF_END)
            break;
        
        left = right;
        
        pBuffer++;
        i++;
    }while(TRUE);
    
    return FAILURE;
}

/*
 * Function: static int WhichItemMouseIn(PNORMALMENUITEM pNormalMenuItem, int top, int y)
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.9.AM.
 *
 */
static int WhichItemMouseIn(PNORMALMENUITEM pNormalMenuItem, int top, int y)
{
    int bottom = top;
    int i = 0;
    PNORMALMENUITEM pBuffer = pNormalMenuItem;
    
    do
    {
        if(pBuffer->fItemFlags & MF_MENUBREAK)
            bottom += MB_HEIGHT*2 + 1;
        else
            bottom += MN_HEIGHT;
        
        if((y > top) && (y <= bottom))
        {
            if(pBuffer->fItemFlags & MF_MENUBREAK)
                return FAILURE;
            return i;
        }

        if(pBuffer->fItemFlags & MF_END)
            break;
        
        top = bottom;
        
        pBuffer++;
        i++;
    }while(TRUE);
    
    return FAILURE;
}

static PNORMALMENUITEM GetNormalMenuItem(PGUIINFO pGUIInfo, WORD wID)
{
    PPOPUPMENUITEM pPopupMenuItem;
    PNORMALMENUITEM pNormalMenuItem;
    
    pPopupMenuItem = pGUIInfo->pPopupMenuItem;
    while(!(pPopupMenuItem->fItemFlags & MF_END))
    {
        pNormalMenuItem = pPopupMenuItem->pNormalMenuItem;
        while(!(pPopupMenuItem->fItemFlags & MF_END))
        {
            if((WORD)(pNormalMenuItem->iMenuID) == wID)
                return pNormalMenuItem;
            pNormalMenuItem++;
        }
        pPopupMenuItem++;
    }
    
    return NULL;
}

WORD GUIAPI GetPopupMenuItemFlags(PGUIINFO pGUIInfo, int iIndex)
{
    PPOPUPMENUITEM pPopupMenuItem;
    
    pPopupMenuItem = pGUIInfo->pPopupMenuItem + iIndex;
    
    return pPopupMenuItem->fItemFlags;
}

WORD GUIAPI GetNormalMenuItemFlags(PGUIINFO pGUIInfo, int iPopupIndex, int iIndex)
{
    PPOPUPMENUITEM pPopupMenuItem;
    PNORMALMENUITEM pNormalMenuItem;
    
    pPopupMenuItem = pGUIInfo->pPopupMenuItem + iPopupIndex;
    pNormalMenuItem = pPopupMenuItem->pNormalMenuItem + iIndex;

    return pNormalMenuItem->fItemFlags;
}

void GUIAPI EnablePopupMenuItem(PGUIINFO pGUIInfo, int iIndex, BOOL fEnable)
{
    PPOPUPMENUITEM pPopupMenuItem;
    
    pPopupMenuItem = pGUIInfo->pPopupMenuItem + iIndex;
    
    if(fEnable)
        pPopupMenuItem->fItemFlags &= ~MF_DISABLED;
    else
        pPopupMenuItem->fItemFlags |= MF_DISABLED;
}

void GUIAPI EnableNormalMenuItem(PGUIINFO pGUIInfo, int iPopupIndex, int iIndex, BOOL fEnable)
{
    PPOPUPMENUITEM pPopupMenuItem;
    PNORMALMENUITEM pNormalMenuItem;
    
    pPopupMenuItem = pGUIInfo->pPopupMenuItem + iPopupIndex;
    pNormalMenuItem = pPopupMenuItem->pNormalMenuItem + iIndex;

    if(fEnable)
        pNormalMenuItem->fItemFlags &= ~MF_DISABLED;
    else
        pNormalMenuItem->fItemFlags |= MF_DISABLED;
}

void GUIAPI CheckNormalMenuItem(PGUIINFO pGUIInfo, int iPopupIndex, int iIndex, BOOL fCheck)
{
    PPOPUPMENUITEM pPopupMenuItem;
    PNORMALMENUITEM pNormalMenuItem;
    
    pPopupMenuItem = pGUIInfo->pPopupMenuItem + iPopupIndex;
    pNormalMenuItem = pPopupMenuItem->pNormalMenuItem + iIndex;

    if(!fCheck)
        pNormalMenuItem->fItemFlags &= ~MF_CHECKED;
    else
        pNormalMenuItem->fItemFlags |= MF_CHECKED;
}

/*
 * Function: void GUIAPI BeginPaint(PDC pDC);
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.10.AM.
 *
 */
void GUIAPI BeginPaint(PDC pDC)
{
    SetPtrVis(HIDE);
    set_cliprgn(pDC->cliprect.left, 
        pDC->cliprect.top,
        pDC->cliprect.right,
        pDC->cliprect.bottom);
}

/*
 * Function: void GUIAPI EndPaint(PDC pDC);
 * Parameters:
 * Return:
 *      None;
 *
 * 1995.8.10.AM.
 *
 */
void GUIAPI EndPaint(PDC pDC)
{
    set_cliprgn(pDC->clientrect.left, 
        pDC->clientrect.top,
        pDC->clientrect.right,
        pDC->clientrect.bottom);
    SetPtrVis(SHOW);
}

