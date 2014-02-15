// Helper.h : header file for the helper.cpp
//
// Copyright (c) 1997.8 ~ 1998.3, Mr. Wei Yongming.
//
// Last modified date: 1998.03.04.

BOOL FAR DoPromptFileName( PGUIINFO pGUIInfo, CString& fileName, CString strCaption, BOOL bOpenFileDialog, CString strFilter);
BOOL FAR DoPromptSimpleFileName( PGUIINFO pGUIInfo, int iDefDrive, LPCSTR lpDefDir, CString& fileName, CString strCaption, BOOL bOpenFileDialog, CString strFilter);
void FAR InitDialogRes(PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData);
BOOL FAR IntersectRect(LPRECT lpFRect, LPRECT lpSRect);
