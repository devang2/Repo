// WNDPanes.c : Implementation file of Window Pane.
// 
// Window Pane works as a element of WYMGUI.
// Using Window Pane, you can define a lot of static panes 
// in the main client window of WYMGUI, 
// and for any individual pane, you can define its display
// that is different from others.
//
// Copyright (c) 1997.7 ~ 1998.3, Mr. Wei Yongming.
//
// Last modified date: 1998.03.04.

#include <afxcoll.h>

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
#include "WndPanes.h"
#include "Helper.h"

PWINDOWPANE pWndPane[MAX_PANENUMBER];
static HWNDPANE GetValidHandle()
{
    int i=0;
    while(pWndPane[i])
        i++;
    
    return (HWNDPANE)(i+1);
}

static BOOL SetPaneRect(HWNDPANE hWndPane, LPRECT lpRect)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return FALSE;
    
    pPane->sLeft = lpRect->left;
    pPane->sTop = lpRect->top;
    pPane->sRight = lpRect->right;
    pPane->sBottom = lpRect->bottom;
    return TRUE;
}

void GUIAPI DestroyPane(HWNDPANE hWndPane)
{
    delete pWndPane[hWndPane-1];
    pWndPane[hWndPane-1] = NULL;
}

void GUIAPI DestroyAllPanes()
{
    int i=0;
    while(pWndPane[i])
    {
        DestroyPane(i + 1);
        i++;
    }
}

HWNDPANE GUIAPI CreateParentPane(PGUIINFO pGUIInfo, short sFGColor, short sBKColor, LPCSTR szCaption, void (FAR *PaintProc)(HWNDPANE hWndPane))
{
    memset(pWndPane, 0, sizeof(PWINDOWPANE)*MAX_PANENUMBER);
    PWINDOWPANE pPane = new WINDOWPANE;
    
    if(pPane == NULL)
        return 0;
    
    pPane->pGUIInfo = pGUIInfo;
    pPane->sLeft = pGUIInfo->dc.clientrect.left;
    pPane->sTop = pGUIInfo->dc.clientrect.top;
    pPane->sRight = pGUIInfo->dc.clientrect.right;
    pPane->sBottom = pGUIInfo->dc.clientrect.bottom;
    pPane->PaintProc = PaintProc;
    pPane->sFGColor = sFGColor;
    pPane->sBKColor = sBKColor;
    pPane->lpData = NULL;
    pPane->nLineNum = 0;
    pPane->nCurLineNO = -1;
    pPane->lpClipRect = NULL;
    _fmemcpy(pPane->szCaption, szCaption, MAX_LENGTH_CAPTION + 1);
    pPane->szCaption[MAX_LENGTH_CAPTION] = '\0';
    pWndPane[0] = pPane;
    
    PostMessage(pGUIInfo, MSG_INITPANES, 1, 0L);
    return 1;
}

HWNDPANE GUIAPI CreateChildPane(HWNDPANE hParentPane, int iType, int iTopOrLeft, short sFGColor, short sBKColor, LPCSTR szCaption, void (FAR *PaintProc)(HWNDPANE hWndPane))
{
    if(iType != PANE_VERTICAL && iType != PANE_HORIZONTAL)
        return 0;

    PWINDOWPANE pPane = new WINDOWPANE;
    
    if(pPane == NULL)
        return 0;
    
    pPane->pGUIInfo = pWndPane[hParentPane - 1]->pGUIInfo;
    pPane->PaintProc = PaintProc;
    pPane->sBKColor = sBKColor;
    pPane->sFGColor = sFGColor;
    pPane->lpData = NULL;
    pPane->nLineNum = 0;
    pPane->lpClipRect = NULL;
    _fmemcpy(pPane->szCaption, szCaption, MAX_LENGTH_CAPTION + 1);
    pPane->szCaption[MAX_LENGTH_CAPTION] = '\0';
    HWNDPANE hWndPane = GetValidHandle();
    pWndPane[hWndPane-1] = pPane;
    
    
    RECT parentRect;
    RECT childRect;
    GetPaneRect(hParentPane, &parentRect);
    if(iType == PANE_VERTICAL)
    {
        childRect.left = iTopOrLeft;
        childRect.top = parentRect.top;
        childRect.right = parentRect.right;
        childRect.bottom = parentRect.bottom;
        parentRect.right = iTopOrLeft;
    }
    else
    {
        childRect.left = parentRect.left;
        childRect.top = iTopOrLeft;
        childRect.right = parentRect.right;
        childRect.bottom = parentRect.bottom;
        parentRect.bottom = iTopOrLeft;
    }
    SetPaneRect(hParentPane, &parentRect);
    SetPaneRect(hWndPane, &childRect);
    
    PostMessage(pPane->pGUIInfo, MSG_INITPANES, hWndPane, 0L);
    return hWndPane;
}

short GUIAPI GetPaneBKColor(HWNDPANE hWndPane)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return 0;
    
    return pPane->sBKColor;
}

LPCSTR GUIAPI GetPaneCaption(HWNDPANE hWndPane)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return NULL;
    
    return (LPCSTR)pPane->szCaption;
}

short GUIAPI GetPaneFGColor(HWNDPANE hWndPane)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return 0;
    
    return pPane->sFGColor;
}

BOOL GUIAPI GetPaneRect(HWNDPANE hWndPane, LPRECT lpRect)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return FALSE;
    
    lpRect->left = pPane->sLeft;
    lpRect->top = pPane->sTop;
    lpRect->right = pPane->sRight;
    lpRect->bottom = pPane->sBottom;
    return TRUE;
}

BOOL GUIAPI GetPaneClientRect(HWNDPANE hWndPane, LPRECT lpRect)
{
    if(!GetPaneRect(hWndPane, lpRect))
        return FALSE;
    
    if(_fstrlen(GetPaneCaption(hWndPane)) == 0)
        lpRect->top += 3;
    else
        lpRect->top += 20;
    lpRect->left += 3;
    lpRect->right -= 3;
    lpRect->bottom -= 3;
    return TRUE;
}

PWINDOWPANE GUIAPI GetPaneContent(HWNDPANE hWndPane)
{
    return pWndPane[hWndPane-1];
}

void GUIAPI PaintPanes(PGUIINFO pGUIInfo, PDC pDC)
{
    for(int i=0; i<MAX_PANENUMBER; i++)
    {
        if(pWndPane[i])
        {
            PGUIINFO pGUIInfo = pWndPane[i]->pGUIInfo;
            short buttonx = pWndPane[i]->sLeft;
            short buttony = pWndPane[i]->sTop;
            short buttonw = pWndPane[i]->sRight;
            short buttonh = pWndPane[i]->sBottom;

            // Paint client area.
            RECT rcClip = {buttonx, buttony, buttonw, buttonh};
            if(!IntersectRect(&rcClip, &pDC->cliprect))
                continue;
            
            set_cliprgn(rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);

            // Erase pane.
            _setcolor(pWndPane[i]->sBKColor);
            _rectangle(_GFILLINTERIOR, buttonx + 2, buttony + 2,
                buttonw - 2, buttonh - 2);

            // Draw caption and border here.
            _setcolor(COLOR_lightgray);
            _rectangle(_GBORDER, buttonx, buttony, buttonw, buttonh);
            _setcolor(0);
            _moveto(buttonx + 1, buttonh - 1);
            _lineto(buttonx + 1, buttony + 1);
            _lineto(buttonw - 1, buttony + 1);
            _setcolor(14);
            _moveto(buttonx + 2, buttonh - 2);
            _lineto(buttonx + 2, buttony + 2);
            _lineto(buttonw - 2, buttony + 2);
            _setcolor(15);
            _moveto(buttonx + 1, buttonh - 1);
            _lineto(buttonw - 1, buttonh - 1);
            _lineto(buttonw - 1, buttony + 1);
            _setcolor(7);
            _moveto(buttonx + 2, buttonh - 2);
            _lineto(buttonw - 2, buttonh - 2);
            _lineto(buttonw - 2, buttony + 2);
            
            set_color(pWndPane[i]->sFGColor);
            cc_wt16(pWndPane[i]->szCaption, buttonx + 4, buttony + 4);

            pWndPane[i]->lpClipRect = &rcClip;
            pWndPane[i]->PaintProc((HWNDPANE)(i+1));
            pWndPane[i]->lpClipRect = NULL;
        }
    }
}

void GUIAPI UpdatePane(HWNDPANE hWndPane, LPRECT lpRect)
{
    RECT rc;
    GetPaneClientRect(hWndPane, &rc);
    
    RECT rcClip;
    memcpy(&rcClip, &rc, sizeof(RECT));
    
    if(lpRect != NULL)
    {
        IntersectRect(&rcClip, lpRect);
    }
    
    set_cliprgn(rcClip.left, rcClip.top, 
                rcClip.right, rcClip.bottom);
    
    // erase invalide rect.
    SetPtrVis(HIDE);
    _setcolor(pWndPane[hWndPane - 1]->sBKColor);
    _rectangle( _GFILLINTERIOR, rc.left, rc.top, 
                rc.right, rc.bottom);

    set_color(pWndPane[hWndPane - 1]->sFGColor);
    pWndPane[hWndPane - 1]->lpClipRect = &rcClip;
    pWndPane[hWndPane - 1]->PaintProc(hWndPane);
    pWndPane[hWndPane - 1]->lpClipRect = NULL;
    SetPtrVis(SHOW);

    // Restore clip rect.
    PGUIINFO pGUIInfo = pWndPane[hWndPane - 1]->pGUIInfo;
    set_cliprgn(pGUIInfo->dc.clientrect.left, pGUIInfo->dc.clientrect.top, 
        pGUIInfo->dc.clientrect.right, pGUIInfo->dc.clientrect.bottom);
}

void GUIAPI ScrollUpOnLine(HWNDPANE hWndPane)
{
    RECT rc;
    GetPaneClientRect(hWndPane, &rc);
    
    rc.left -= 3;
    rc.right += 3;
    int iWidth = rc.right - rc.left;
    int iHeight = rc.bottom - rc.top;
    if(iHeight < 16)
        return;
    
    iHeight -= 16;
    SetPtrVis(HIDE);
    move_image( rc.left, rc.top + 16,
                iWidth, iHeight,
                rc.left, rc.top );
    SetPtrVis(SHOW);
}

void GUIAPI AddTextLineToPane(HWNDPANE hWndPane, LPCSTR lpszText, BOOL bUpdate)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return;

    RECT rc;
    GetPaneClientRect(hWndPane, &rc);
    UINT nMaxLineNum = (UINT)((rc.bottom - rc.top)/16);
    if(nMaxLineNum <= (UINT)0)
        nMaxLineNum = (UINT)1;
    
    pPane->nLineNum++;
    LPTEXTLINE pOldTextLine = NULL;
    if(pPane->nLineNum > nMaxLineNum)
    {
        LPVOID lpData = pPane->lpData;
        pPane->lpData = ((LPTEXTLINE)(pPane->lpData))->pNextLine;
        pOldTextLine = (LPTEXTLINE)lpData;
    }
    
    if(!pPane->lpData)
    {
        if(pOldTextLine)
            pPane->lpData = pOldTextLine;
        else
            pPane->lpData = _fmalloc(sizeof(TEXTLINE));
        LPTEXTLINE lpTextLine = (LPTEXTLINE)(pPane->lpData);
        _fmemcpy(lpTextLine->szText, lpszText, MAX_LENGTH_LINE + 1);
        lpTextLine->szText[MAX_LENGTH_LINE] = '\0';
        lpTextLine->pNextLine = NULL;
        lpTextLine->nLineNo = pPane->nLineNum;
    }
    else
    {
        LPTEXTLINE lpTextLine = (LPTEXTLINE)(pPane->lpData);
        while(lpTextLine->pNextLine)
        {
            lpTextLine = lpTextLine->pNextLine;
        }
        
        if(pOldTextLine)
            lpTextLine->pNextLine = pOldTextLine;
        else
            lpTextLine->pNextLine = (LPTEXTLINE)_fmalloc(sizeof(TEXTLINE));
        lpTextLine = lpTextLine->pNextLine;
        _fmemcpy(lpTextLine->szText, lpszText, MAX_LENGTH_LINE + 1);
        lpTextLine->szText[MAX_LENGTH_LINE] = '\0';
        lpTextLine->pNextLine = NULL;
        lpTextLine->nLineNo = pPane->nLineNum;
    }
    
    if(bUpdate)
    {
        UINT nLine = pPane->nLineNum;
        if(nLine > nMaxLineNum)
        {
            nLine = nMaxLineNum;
            ScrollUpOnLine(hWndPane);
        }

        // Get line output rect
        RECT rcLine;
        memcpy(&rcLine, &rc, sizeof(RECT));
        rcLine.top = rc.top + 16*(nLine - 1);
        rcLine.bottom = rcLine.top + 16;
        
        UpdatePane(hWndPane, &rcLine);
    }
}

BOOL GUIAPI IsRectVisible(HWNDPANE hWndPane, LPRECT lpRect)
{
    if(pWndPane[hWndPane - 1]->lpClipRect == NULL)
        return TRUE;
        
    RECT rcTemp;
    _fmemcpy(&rcTemp, pWndPane[hWndPane - 1]->lpClipRect, sizeof(RECT));
    
    return IntersectRect(&rcTemp, lpRect);
}

void GUIAPI OutPaneText(HWNDPANE hWndPane)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return;

    LPTEXTLINE lpTextLine = (LPTEXTLINE)(pPane->lpData);
    if(!lpTextLine)
        return;
    
    RECT rc;
    RECT rcLine;
    int iLine = 0;
    GetPaneClientRect(hWndPane, &rc);
    while(lpTextLine)
    {
        rcLine.left = rc.left;
        rcLine.top = rc.top + 16*iLine;
        rcLine.right = rc.right;
        rcLine.bottom = rcLine.top + 16;
        
        if(IsRectVisible(hWndPane, &rcLine))
        {
            char szLineNo[10];
            sprintf(szLineNo, "%06u", lpTextLine->nLineNo);
            cc_wt16(szLineNo, rc.left + 4, rc.top + 16*iLine);
            cc_wt16(lpTextLine->szText, rc.left + 56, rc.top + 16*iLine);
        }
        
        lpTextLine = lpTextLine->pNextLine;
        iLine++;
    }
}

void GUIAPI FlashLastLine(HWNDPANE hWndPane)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return;

    LPTEXTLINE lpTextLine = (LPTEXTLINE)(pPane->lpData);
    if(!lpTextLine)
        return;
    
    int iLine = 0;
    RECT rc;
    LPTEXTLINE lpLastLine;
    GetPaneClientRect(hWndPane, &rc);
    do
    {
        lpLastLine = lpTextLine;
        lpTextLine = lpTextLine->pNextLine;
        
        if(lpTextLine == NULL)
            break;
        
        iLine++;
    }while(TRUE);
    
    char szLineNo[10];
    sprintf(szLineNo, "%06u", lpLastLine->nLineNo);
    
    static BOOL bErase = TRUE;
    
    SetPtrVis(HIDE);
    if(bErase)
        set_color(pWndPane[hWndPane - 1]->sBKColor);
    else
        set_color(pWndPane[hWndPane - 1]->sFGColor);

    char szFlashLine[MAX_LENGTH_LINE + 1];
    int iFlashLineLength = __min((rc.right - rc.left - 4)/8, MAX_LENGTH_LINE);
    _fmemcpy(szFlashLine, lpLastLine->szText, iFlashLineLength);
    szFlashLine[iFlashLineLength] = '\0';
    cc_wt16(szFlashLine, rc.left + 56, rc.top + 16*iLine);
    SetPtrVis(SHOW);

    bErase = !bErase;
}

void GUIAPI DeleteWndPaneContent(HWNDPANE hWndPane)
{
    PWINDOWPANE pPane = pWndPane[hWndPane-1];
    if(!pPane)
        return;

    LPTEXTLINE lpTextLine = (LPTEXTLINE)(pPane->lpData);
    LPTEXTLINE pNextTextLine;
    while(lpTextLine)
    {
        pNextTextLine = lpTextLine->pNextLine;
        
        _ffree(lpTextLine);
        
        lpTextLine = pNextTextLine;
    }
    
    pPane->lpData = NULL;
}
