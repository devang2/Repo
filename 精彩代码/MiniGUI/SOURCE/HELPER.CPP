// Helper.cpp : implementation file of helper functions.
//
// Copyright (c) 1997.8 ~ 1998.3, Mr. Wei Yongming.
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

BOOL FAR DoPromptFileName( PGUIINFO pGUIInfo, CString& fileName, CString strCaption, BOOL bOpenFileDialog, CString strFilter)
{
	char szOldPath[_MAX_PATH];
	char szFileName[_MAX_PATH];
	
	_getcwd(szOldPath, _MAX_PATH);
	int iOldDrive = _getdrive();

    OPENFILEBOX OpenFileBox;
    
    OpenFileBox.fOpen = bOpenFileDialog;
    OpenFileBox.iInitDrive = iOldDrive;
    OpenFileBox.lpInitDir = (LPSTR)szOldPath;
    OpenFileBox.lpExt = (LPCSTR)strFilter;
    OpenFileBox.lpFileName = (LPSTR)szFileName;
    
    PDLGHEADER pDlgHeader = GetSysDlgHeader(IDD_OPENFILEDLG);
    pDlgHeader->spCaption = (LPCSTR)strCaption;
    DialogBoxParam(pGUIInfo, pDlgHeader, OpenBoxProc, (LPSTR)(&OpenFileBox));
	fileName = szFileName;
	
    _chdrive(iOldDrive);
    _chdir(szOldPath);
	
	if(szFileName[0] == '\0')
		return FALSE;
	
	return TRUE;
}

BOOL FAR DoPromptSimpleFileName( PGUIINFO pGUIInfo, int iDefDrive, LPCSTR lpDefDir, CString& fileName, CString strCaption, BOOL bOpenFileDialog, CString strFilter)
{
	char szOldPath[_MAX_PATH];
	char szFileName[_MAX_PATH];
	
	_getcwd(szOldPath, _MAX_PATH);
	int iOldDrive = _getdrive();

    OPENFILEBOX OpenFileBox;
    
    OpenFileBox.fOpen = bOpenFileDialog;
    OpenFileBox.iInitDrive = iDefDrive;
    OpenFileBox.lpInitDir = lpDefDir;
    OpenFileBox.lpExt = (LPCSTR)strFilter;
    OpenFileBox.lpFileName = (LPSTR)szFileName;
    
    PDLGHEADER pDlgHeader = GetSysDlgHeader(IDD_SIMPLEOPENFILEDLG);
    pDlgHeader->spCaption = (LPCSTR)strCaption;
    DialogBoxParam(pGUIInfo, pDlgHeader, SimpleOpenBoxProc, (LPSTR)(&OpenFileBox));
	fileName = szFileName;
	
    _chdrive(iOldDrive);
    _chdir(szOldPath);
	
	if(szFileName[0] == '\0')
		return FALSE;
	
	return TRUE;
}

void FAR InitDialogRes(PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData)
{
    pDlgHeader->pCtrlData = pCtrlData;
    pDlgHeader->rx += pDlgHeader->lx;
    pDlgHeader->by += pDlgHeader->ty;
    for(int i=0; i<pDlgHeader->NumOfItems; i++)
    {
        (pDlgHeader->pCtrlData + i)->lx = (pDlgHeader->pCtrlData + i)->lx
            + pDlgHeader->lx;
        (pDlgHeader->pCtrlData + i)->rx = (pDlgHeader->pCtrlData + i)->rx
            + (pDlgHeader->pCtrlData + i)->lx;
        (pDlgHeader->pCtrlData + i)->ty = (pDlgHeader->pCtrlData + i)->ty
            + pDlgHeader->ty + 25;
        (pDlgHeader->pCtrlData + i)->by = (pDlgHeader->pCtrlData + i)->by
            + (pDlgHeader->pCtrlData + i)->ty;
    }
    
}

void FAR NormalizeRect(LPRECT lpRect)
{
	int iTemp;
	
	if(lpRect->left > lpRect->right)
	{
		iTemp = lpRect->left;
		lpRect->left = lpRect->right;
		lpRect->right = iTemp;
	}
	
	if(lpRect->top > lpRect->bottom)
	{
		iTemp = lpRect->top;
		lpRect->top = lpRect->bottom;
		lpRect->bottom = iTemp;
	}
}

BOOL FAR IntersectRect(LPRECT lpFRect, LPRECT lpSRect)
{
	NormalizeRect(lpFRect);
	NormalizeRect(lpSRect);
	
	lpFRect->left = (lpFRect->left > lpSRect->left) ? lpFRect->left : lpSRect->left;
	lpFRect->top = (lpFRect->top > lpSRect->top) ? lpFRect->top : lpSRect->top;
	lpFRect->right = (lpFRect->right < lpSRect->right) ? lpFRect->right : lpSRect->right;
	lpFRect->bottom = (lpFRect->bottom < lpSRect->bottom) ? lpFRect->bottom : lpSRect->bottom;
	
	if(lpFRect->left > lpFRect->right)
		return FALSE;
	if(lpFRect->top > lpFRect->bottom)
		return FALSE;
		
	return TRUE;
}
