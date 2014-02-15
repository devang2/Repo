// Dialog.c The implementation file of dialog.
//
// This file include functions that handle
// dialog box procedures, message box procedure etc.
//
// Copyright (c) 1994.8 ~ 1998.4, Mr. Wei Yongming.
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

extern struct _videoconfig vc;
extern BOOL fNewKb;
extern void (__interrupt __far *oldvect)();

static MSGBOXDATA MsgBoxData;
static int CtrlIndex;
static BOOL fAlreadyUp;
    
static void CheckCtrl( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData );
static void PushButton(PCTRLDATA pCurCtrl, BOOL uCheck);
static void DrawBox( short lx, short ty, short rx, short by );
static void DrawFocus(PCTRLDATA pCtrlData);
static void CheckButtonDown(BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg);
static void CheckButtonUp(BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg, int CtrlIndex);

static BOOL StartDialog( PDLGHEADER pDlgHeader );
static BOOL CreateDialog( PDLGHEADER pDlgHeader, int (FAR *DlgProc)() );
static void EndDialog( PDLGHEADER pDlgHeader );

static PCTRLDATA GetNextTabstop( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData );
static PCTRLDATA GetLastTabstop( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData );
static PCTRLDATA GetNextItemInGroup( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData );
static PCTRLDATA GetLastItemInGroup( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData );

static BOOL HaveCancelButton( PDLGHEADER pDlgHeader );

static void DrawListBoxItem( PCTRLDATA pCtrlData, int iOffset, int iIndex, BOOL fSel );
static void ScrollListBox(PCTRLDATA pCtrlData, WORD uAction);
static void SetVScrollPosition(PCTRLDATA pCtrlData, BOOL fErase);

// 系统“有关……”对话框
static DLGHEADER  SysAboutDlg = {WS_BORDER|WS_POPUP|WS_CAPTION, 17, 70,100,490,274, NULL, "有关MINIGUI--图形用户界面支持系统"};
// 系统文件打开/保存对话框
static DLGHEADER  OpenFileDlg = {WS_BORDER|WS_POPUP|WS_CAPTION, 8, 100,100,416,230, NULL, NULL};
// 系统简单文件打开/保存对话框，此对话框只能完成在特定目录下的文件打开和保存功能。
static DLGHEADER  SimpleOpenFileDlg = {WS_BORDER|WS_POPUP|WS_CAPTION, 7, 70, 80, 354, 244, NULL, "选择要打开的文件"};

static CTRLDATA   SysDlgCtrl[]={
        // About Box's controls
        {SS_OWNER, 12, 10,84,84, IDC_ICON, STATIC, NULL, NULL},
        {SS_LEFT, 106, 8, 340, 16, IDC_STATIC, STATIC, "MINIGUI--图形用户界面支持系统  版本3.0", NULL},
        {SS_LEFT, 106, 26, 370, 16, IDC_STATIC, STATIC, "版权 (C) 1995~1998 清华大学魏永明", NULL},
        {SS_LEFT, 106, 44, 370, 16, IDC_STATIC, STATIC, "部分版权 (C) 清华大学李铁民", NULL},
        {SS_LEFT, 106, 72, 340, 16, IDC_STATIC, STATIC, "本支持系统的合法使用者是", NULL},
        {SS_LEFT, 106, 96, 340, 16, IDC_STATIC, STATIC, "清华大学", NULL},
        {SS_LEFT, 106, 116, 340, 16, IDC_STATIC, STATIC, "精仪系科协", NULL},
        {SS_BLACKRECT, 106, 138, 374, 2, IDC_STATIC, STATIC, NULL,NULL},
        {SS_LEFT, 106, 152, 130, 16, IDC_STATIC, STATIC, "近堆中可用内存：", NULL},
        {SS_LEFT, 106, 174, 130, 16, IDC_STATIC, STATIC, "远堆最大内存块：", NULL},
        {SS_LEFT, 106, 196, 114, 16, IDC_STATIC, STATIC, "数学协处理器：", NULL},
        {SS_LEFT, 106, 218, 114, 16, IDC_STATIC, STATIC, "剩余磁盘空间：", NULL},
        {SS_OWNER, 250, 152, 114, 16, IDC_MEMORY, STATIC, NULL, NULL},
        {SS_OWNER, 250, 174, 114, 16, IDC_FARMEMORY, STATIC, NULL, NULL},
        {SS_OWNER, 250, 196, 114, 16, IDC_MATH, STATIC, NULL, NULL},
        {SS_OWNER, 250, 218, 114, 16, IDC_DISK, STATIC, NULL, NULL},
        {BS_PUSHBUTTON | WS_TABSTOP, 390,167,80,30, IDOK, BUTTON, "确定", NULL},

        // Open and Save As Box's controls
        {SS_LEFT, 14, 16, 92, 18, IDC_STATIC, STATIC, "文件名:", NULL},
        {SS_LEFT, 160, 16, 160, 18, IDC_STATIC, STATIC, "目录/驱动器:", NULL},
        {ES_LEFT | WS_TABSTOP,  14,40,127,20, IDC_FILENAME, EDIT, NULL, NULL},
        {SS_OWNER, 160, 42, 240, 18, IDC_DIRECTORY, STATIC, NULL, NULL},
        {WS_VSCROLL | WS_BORDER | WS_TABSTOP, 14, 74, 128, 112, IDC_FILE, LISTBOX, NULL, NULL},
        {WS_VSCROLL | WS_BORDER | WS_TABSTOP, 160, 74, 128, 112, IDC_DIR, LISTBOX, NULL, NULL},
        {BS_PUSHBUTTON | WS_TABSTOP, 300,108,100,28, IDOK, BUTTON, "确定", NULL},
        {BS_PUSHBUTTON | WS_TABSTOP, 300,150,100,28, IDCANCEL, BUTTON, "取消", NULL},
        
        // Simple Open and Save file dialog
        {SS_LEFT,  14,12,132,18, IDC_STATIC, STATIC, "当前目录/驱动器:", NULL},
        {SS_OWNER, 14,36,328,18, IDC_DIRECTORY, STATIC, NULL, NULL},
        {SS_LEFT,  14,60,68,18, IDC_STATIC, STATIC, "文件名:", NULL},
        {ES_LEFT | WS_TABSTOP,  84,60,134,18, IDC_FILENAME, EDIT, NULL, NULL},
        {WS_VSCROLL | WS_BORDER | WS_TABSTOP, 84,94,134,112, IDC_FILE, LISTBOX, NULL, NULL},
        {BS_PUSHBUTTON | WS_TABSTOP, 240,56,100,28, IDOK, BUTTON, "确定", NULL},
        {BS_PUSHBUTTON | WS_TABSTOP, 240,94,100,28, IDCANCEL, BUTTON, "取消", NULL},
    };

PDLGHEADER GUIAPI GetSysDlgHeader(int iID)
{
    int j;
    switch(iID)
    {
        case IDD_SYSABOUTDLG:
            if(SysAboutDlg.pCtrlData == NULL)
            {
                SysAboutDlg.pCtrlData = SysDlgCtrl;

                SysAboutDlg.rx += SysAboutDlg.lx;
                SysAboutDlg.by += SysAboutDlg.ty;
                for(j=0; j<SysAboutDlg.NumOfItems; j++)
                {
                    (SysAboutDlg.pCtrlData + j)->lx = (SysAboutDlg.pCtrlData + j)->lx
                        + SysAboutDlg.lx;
                    (SysAboutDlg.pCtrlData + j)->rx = (SysAboutDlg.pCtrlData + j)->rx
                        + (SysAboutDlg.pCtrlData + j)->lx;
                    (SysAboutDlg.pCtrlData + j)->ty = (SysAboutDlg.pCtrlData + j)->ty
                        + SysAboutDlg.ty + 25;
                    (SysAboutDlg.pCtrlData + j)->by = (SysAboutDlg.pCtrlData + j)->by
                        + (SysAboutDlg.pCtrlData + j)->ty;
                }
            }
            return &SysAboutDlg;
            
        case IDD_OPENFILEDLG:
            if(OpenFileDlg.pCtrlData == NULL)
            {
                OpenFileDlg.pCtrlData = SysDlgCtrl + 17;

                OpenFileDlg.rx += OpenFileDlg.lx;
                OpenFileDlg.by += OpenFileDlg.ty;
                for(j=0; j<OpenFileDlg.NumOfItems; j++)
                {
                    (OpenFileDlg.pCtrlData + j)->lx = (OpenFileDlg.pCtrlData + j)->lx
                        + OpenFileDlg.lx;
                    (OpenFileDlg.pCtrlData + j)->rx = (OpenFileDlg.pCtrlData + j)->rx
                        + (OpenFileDlg.pCtrlData + j)->lx;
                    (OpenFileDlg.pCtrlData + j)->ty = (OpenFileDlg.pCtrlData + j)->ty
                        + OpenFileDlg.ty + 25;
                    (OpenFileDlg.pCtrlData + j)->by = (OpenFileDlg.pCtrlData + j)->by
                        + (OpenFileDlg.pCtrlData + j)->ty;
                }
            }
            return &OpenFileDlg;
            
        case IDD_SIMPLEOPENFILEDLG:
            if(SimpleOpenFileDlg.pCtrlData == NULL)
            {
                SimpleOpenFileDlg.pCtrlData = SysDlgCtrl + 25;

                SimpleOpenFileDlg.rx += SimpleOpenFileDlg.lx;
                SimpleOpenFileDlg.by += SimpleOpenFileDlg.ty;
                for(j=0; j<SimpleOpenFileDlg.NumOfItems; j++)
                {
                    (SimpleOpenFileDlg.pCtrlData + j)->lx = (SimpleOpenFileDlg.pCtrlData + j)->lx
                        + SimpleOpenFileDlg.lx;
                    (SimpleOpenFileDlg.pCtrlData + j)->rx = (SimpleOpenFileDlg.pCtrlData + j)->rx
                        + (SimpleOpenFileDlg.pCtrlData + j)->lx;
                    (SimpleOpenFileDlg.pCtrlData + j)->ty = (SimpleOpenFileDlg.pCtrlData + j)->ty
                        + SimpleOpenFileDlg.ty + 25;
                    (SimpleOpenFileDlg.pCtrlData + j)->by = (SimpleOpenFileDlg.pCtrlData + j)->by
                        + (SimpleOpenFileDlg.pCtrlData + j)->ty;
                }
            }
            return &SimpleOpenFileDlg;
    }
    
    return NULL;
}

/*
 * Function: int FAR SysAboutBoxProc( PDLGHEADER pDlgHeader, UINT uAction, PCTRLDATA pCtrlData, LONG lParam ).
 *      This function process the system about dialog box.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure.
 *      uAction: the Message given by DialogBoxParam function;
 *      pCtrlData: the control that have input focus;
 *      lParam: the LONG type parameter.
 * Return: 
 *      DEFAULT: for current action, to handle with a default procdure;
 *      OWNER  : for current action, not to handle with default;
 *      COLSE  : close dialog;
 *      NOCLOSE: do not close dialog.
 */
int FAR SysAboutBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam )
{
    char strBuffer[40];
    unsigned maxsize;
    
    switch(uAction)
    {
        case INITDIALOG:
        return DEFAULT;
        
        case COMMAND:
            switch(wID)
            {
                case IDOK:
                case IDCANCEL:
                return CLOSE;
                
                default:
                return DEFAULT;
            }
        break;

        case DRAWCTRL:
            if(wID == IDC_ICON)
            {
                Bitmap((LPSTR)".\\bin\\minigui1.bmp", 14, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->by);
                Bitmap((LPSTR)".\\bin\\minigui2.bmp", 8, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->by);
                return OWNER;
            }
        return DEFAULT;
        
        case DRAWTEXT:
            switch( wID )
            {
                case IDC_MEMORY:
                    sprintf(strBuffer, "%d Kb", _memavl()/1024);
                    cc_wt16(strBuffer, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->ty);
                    return OWNER;
                    
                case IDC_FARMEMORY:
                    _dos_allocmem(0xFFFF, &maxsize);
                    sprintf(strBuffer, "%d Kb", maxsize/64);
                    cc_wt16(strBuffer, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->ty);
                    return OWNER;
                    
                case IDC_MATH:
                    if(_bios_equiplist() & 0x0002)
                        strcpy(strBuffer, "已安装");
                    else
                        strcpy(strBuffer, "未安装");
                    cc_wt16(strBuffer, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->ty);
                    return OWNER;
                    
                case IDC_DISK:
                {
                    // fill disk free information
                    struct _diskfree_t diskfree;
                    if (_dos_getdiskfree(_getdrive(), &diskfree) == 0)
                    {
                        sprintf(strBuffer, "%ld Kb",
                            (DWORD)diskfree.avail_clusters *
                            (DWORD)diskfree.sectors_per_cluster *
                            (DWORD)diskfree.bytes_per_sector / (DWORD)1024L);
                    }
                    else
                        strcpy(strBuffer, "无可用空间");
                    cc_wt16(strBuffer, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->ty);
                    return OWNER;
                }
                
            }
        return DEFAULT;
            
        default:
        return DEFAULT;
    }
    return DEFAULT;
}

// static functions used by OpenBoxProc
static void FillFileListBox(PDLGHEADER pDlgHeader, WORD wID, PSTR pFileName)
{
    struct find_t c_file;
    
    if(_dos_findfirst(pFileName, _A_NORMAL, &c_file) == 0)
    {
        if(!(c_file.attrib & _A_SUBDIR))
            AddListBoxItem(pDlgHeader, wID, c_file.name);
    }
    else
        return;

    while( _dos_findnext(&c_file) == 0)
        if(!(c_file.attrib & _A_SUBDIR))
            AddListBoxItem(pDlgHeader, wID, c_file.name);
}

static void FillDirListBox(PDLGHEADER pDlgHeader, WORD wID)
{
    struct find_t c_file;
    
    if(_dos_findfirst("*.*", _A_SUBDIR, &c_file) == 0)
    {
        if(c_file.attrib & _A_SUBDIR)
            if(strcmp(c_file.name, ".") != 0)
                AddListBoxItem(pDlgHeader, wID, c_file.name);
    }
    else
        goto adddrive;
    
    while( _dos_findnext(&c_file) == 0)
    {
        if(c_file.attrib & _A_SUBDIR)
            AddListBoxItem(pDlgHeader, wID, c_file.name);
    }
    
adddrive:
    AddListBoxItem(pDlgHeader, IDC_DIR, "[D]");
    AddListBoxItem(pDlgHeader, IDC_DIR, "[C]");
}

static BOOL CheckFileName(BOOL fOpen, PSTR filename)
{
    if(fOpen)
    {
        if(_access(filename, 0) == -1)
        {
            MessageBox(NULL, "无法找到给定的文件！", "文件打开", MB_OK);
            return FALSE;
        }
    }
    else
    {
        if(_access(filename, 0) == 0)
        {
            if(MessageBox(NULL, "给定的文件已存在，是否替换？", "文件另存为", 
                    MB_YESNO | MB_ICONQUESTION) == IDNO)
            return FALSE;
        }
        if(strpbrk(filename," #%;/\\") != NULL)
        {
            MessageBox(NULL, "由于给定的文件名中有歧义字符，\n无法用给定的文件名创建文件！", "文件另存为", 
                    MB_OK);
            return FALSE;
        }
        {
            char* spExtName;
            int iWholeL, iExtL, iNameL;
                            
            iWholeL = strlen(filename);
            spExtName = strchr(filename, '.');
            if(spExtName == NULL)
            {
                iExtL = 0;
                iNameL = iWholeL - iExtL;
            }
            else
            {
                iExtL = strlen(spExtName + 1);
                iNameL = iWholeL - iExtL - 1;
            }
                
            if(iExtL > 3 || iNameL > 8)
            {
                MessageBox(NULL, "给定的文件名(或后缀名)太长！", "文件另存为",
                    MB_OK);
                return FALSE;
            }
        }
    }
    
    return TRUE;
}

/*
 * Function: int FAR OpenBoxProc( PDLGHEADER pDlgHeader, UINT uAction, PCTRLDATA pCtrlData, LONG lParam ).
 *      This function Process the open file dialog box.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure.
 *      uAction: the Message given by DialogBoxParam function;
 *      pCtrlData: the control that have input focus;
 *      lParam: the LONG type parameter.
 * Return: 
 */
int FAR OpenBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam )
{
    char filename[_MAX_PATH + 1];
    int iIndex, drive;
    PCTRLDATA pCtrlData;
    static LPOPENFILEBOX lpOpenFileBox;

    switch(uAction)
    {
        case INITDIALOG:
            lpOpenFileBox = (LPOPENFILEBOX)lParam;

            _chdrive(lpOpenFileBox->iInitDrive);
            _fstrcpy(filename, lpOpenFileBox->lpInitDir);
            _chdir(filename);

            SetEditBoxText(pDlgHeader, IDC_FILENAME, lpOpenFileBox->lpExt);
            _fstrcpy(filename, lpOpenFileBox->lpExt);
            FillFileListBox(pDlgHeader, IDC_FILE, filename);

            FillDirListBox(pDlgHeader, IDC_DIR);
        return DEFAULT;
        
        case FOCUSCOMMAND:
            if(wID == IDC_FILE)
            {
                iIndex = GetSelectedItem(pDlgHeader, IDC_FILE);
                GetListBoxItem(pDlgHeader, IDC_FILE, filename, iIndex);
                SetEditBoxText(pDlgHeader, IDC_FILENAME, filename);
                pCtrlData = GetControlData(pDlgHeader, IDC_FILENAME);
                OutFieldEditText(pCtrlData, FALSE);
                return DEFAULT;
            }
        break;                
            
        case COMMAND:
            switch(wID)
            {
                case IDOK:
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    _fstrcpy(lpOpenFileBox->lpFileName, filename);
                    GetEditBoxText(pDlgHeader, IDC_FILENAME, filename, 13);
                    if(strchr(filename, '*') != NULL
                        || strchr(filename, '?') != NULL)
                    {
                        pCtrlData = GetControlData(pDlgHeader, IDC_FILE);
                        ReleaseListBoxBuff(pCtrlData);
                        
                        FillFileListBox(pDlgHeader, IDC_FILE, filename);
                        SetFocus(NULL);
                        FillListBox(pCtrlData);
                        return DEFAULT;
                    }
                    else if(!CheckFileName(lpOpenFileBox->fOpen, filename))
                        return DEFAULT;

                    // get path name
                    if(_fstrlen(lpOpenFileBox->lpFileName) > 3)
                        _fstrcat(lpOpenFileBox->lpFileName, "\\");
                    _fstrcat(lpOpenFileBox->lpFileName, filename);
                    
                     // set directory to the default directory.
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                     return CLOSE;
                
                case IDCANCEL:
                    // set directory to the default directory.
                    *(lpOpenFileBox->lpFileName) = '\0';
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                return CLOSE;
                
                case IDC_FILENAME:
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    _fstrcpy(lpOpenFileBox->lpFileName, filename);
                    GetEditBoxText(pDlgHeader, IDC_FILENAME, filename, 13);
                    if(strchr(filename, '*') != NULL
                        || strchr(filename, '?') != NULL)
                    {
                        pCtrlData = GetControlData(pDlgHeader, IDC_FILE);
                        ReleaseListBoxBuff(pCtrlData);
                        
                        FillFileListBox(pDlgHeader, IDC_FILE, filename);
                        SetFocus(NULL);
                        FillListBox(pCtrlData);
                        return DEFAULT;
                    }
                    else if(!CheckFileName(lpOpenFileBox->fOpen, filename))
                        return DEFAULT;

                    // get path name
                    if(_fstrlen(lpOpenFileBox->lpFileName) > 3)
                        _fstrcat(lpOpenFileBox->lpFileName, "\\");
                    _fstrcat(lpOpenFileBox->lpFileName, filename);
                    
                    // set directory to the default directory.
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                    return CLOSE;
                    
                case IDC_FILE:
                    if(GetListItemNum(pDlgHeader, IDC_FILE) < 1)
                        return DEFAULT;
                        
                    // get path name
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    _fstrcpy(lpOpenFileBox->lpFileName, filename);
                    iIndex = GetSelectedItem(pDlgHeader, IDC_FILE);
                    GetListBoxItem(pDlgHeader, IDC_FILE, filename, iIndex);

                    if(!lpOpenFileBox->fOpen)
                    {
                        if(MessageBox(NULL, "给定的文件已存在，是否替换？", "文件另存为", 
                                MB_YESNO | MB_ICONQUESTION) == IDNO)
                            return DEFAULT;
                    }
                    if(_fstrlen(lpOpenFileBox->lpFileName) > 3)
                        _fstrcat(lpOpenFileBox->lpFileName, "\\");
                    _fstrcat(lpOpenFileBox->lpFileName, filename);

                    // set directory to the default directory.
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                    return CLOSE;
                break;
                
                case IDC_DIR:
                    iIndex = GetSelectedItem(pDlgHeader, IDC_DIR);
                    drive = GetListItemNum(pDlgHeader, IDC_DIR) - iIndex;
                    if(drive < 3)
                    {
                        drive += 2;
                        _chdrive(drive);
                    }
                    else
                    {
                        GetListBoxItem(pDlgHeader, IDC_DIR, filename, iIndex);
                        _chdir(filename);
                    }
                    
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    DrawTextInStaticItem(pDlgHeader, IDC_DIRECTORY, filename );

                    pCtrlData = GetControlData(pDlgHeader, IDC_DIR);
                    ReleaseListBoxBuff(pCtrlData);
                    FillDirListBox(pDlgHeader, IDC_DIR);
                    SetFocus(NULL);
                    FillListBox(pCtrlData);

                    GetEditBoxText(pDlgHeader, IDC_FILENAME, filename, 13);
                    pCtrlData = GetControlData(pDlgHeader, IDC_FILE);
                    ReleaseListBoxBuff(pCtrlData);
                    FillFileListBox(pDlgHeader, IDC_FILE, filename);
                    SetFocus(NULL);
                    FillListBox(pCtrlData);
                break;

                default:
                break;
            }
        break;

        case DRAWTEXT:
            if( wID == IDC_DIRECTORY)
            {
                _getdcwd(_getdrive(), filename, _MAX_PATH);
                cc_wt16(filename, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->ty);
                return OWNER;
            }
        return DEFAULT;
        
        default:
        return DEFAULT;
    }
    return DEFAULT;
}

/*
 * Function: int FAR SimpleOpenBoxProc( PDLGHEADER pDlgHeader, UINT uAction, PCTRLDATA pCtrlData, LONG lParam ).
 *      This function Process the simple open file dialog box.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure.
 *      uAction: the Message given by DialogBoxParam function;
 *      pCtrlData: the control that have input focus;
 *      lParam: the LONG type parameter.
 * Return: 
 */
int FAR SimpleOpenBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam )
{
    char filename[_MAX_PATH + 1];
    PCTRLDATA pCtrlData;
    static LPOPENFILEBOX lpOpenFileBox;
    int iIndex;

    switch(uAction)
    {
        case INITDIALOG:
            lpOpenFileBox = (LPOPENFILEBOX)lParam;

            _chdrive(lpOpenFileBox->iInitDrive);
            _fstrcpy(filename, lpOpenFileBox->lpInitDir);
            _chdir(filename);

            SetEditBoxText(pDlgHeader, IDC_FILENAME, lpOpenFileBox->lpExt);
            _fstrcpy(filename, lpOpenFileBox->lpExt);
            FillFileListBox(pDlgHeader, IDC_FILE, filename);
        return DEFAULT;
        
        case FOCUSCOMMAND:
            if(wID == IDC_FILE)
            {
                iIndex = GetSelectedItem(pDlgHeader, IDC_FILE);
                GetListBoxItem(pDlgHeader, IDC_FILE, filename, iIndex);
                SetEditBoxText(pDlgHeader, IDC_FILENAME, filename);
                pCtrlData = GetControlData(pDlgHeader, IDC_FILENAME);
                OutFieldEditText(pCtrlData, FALSE);
                return DEFAULT;
            }
        break;                
            
        case COMMAND:
            switch(wID)
            {
                case IDOK:
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    _fstrcpy(lpOpenFileBox->lpFileName, filename);
                    GetEditBoxText(pDlgHeader, IDC_FILENAME, filename, 13);
                    if(strchr(filename, '*') != NULL
                        || strchr(filename, '?') != NULL)
                    {
                        pCtrlData = GetControlData(pDlgHeader, IDC_FILE);
                        ReleaseListBoxBuff(pCtrlData);
                        
                        FillFileListBox(pDlgHeader, IDC_FILE, filename);
                        SetFocus(NULL);
                        FillListBox(pCtrlData);
                        return DEFAULT;
                    }
                    else if(!CheckFileName(lpOpenFileBox->fOpen, filename))
                        return DEFAULT;

                    // get path name
                    if(_fstrlen(lpOpenFileBox->lpFileName) > 3)
                        _fstrcat(lpOpenFileBox->lpFileName, "\\");
                    _fstrcat(lpOpenFileBox->lpFileName, filename);
                    
                     // set directory to the default directory.
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                     return CLOSE;
                
                case IDCANCEL:
                    // set directory to the default directory.
                    *(lpOpenFileBox->lpFileName) = '\0';
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                return CLOSE;
                
                case IDC_FILENAME:
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    _fstrcpy(lpOpenFileBox->lpFileName, filename);
                    GetEditBoxText(pDlgHeader, IDC_FILENAME, filename, 13);
                    if(strchr(filename, '*') != NULL
                        || strchr(filename, '?') != NULL)
                    {
                        pCtrlData = GetControlData(pDlgHeader, IDC_FILE);
                        ReleaseListBoxBuff(pCtrlData);
                        
                        FillFileListBox(pDlgHeader, IDC_FILE, filename);
                        SetFocus(NULL);
                        FillListBox(pCtrlData);
                        return DEFAULT;
                    }
                    else if(!CheckFileName(lpOpenFileBox->fOpen, filename))
                        return DEFAULT;

                    // get path name
                    if(_fstrlen(lpOpenFileBox->lpFileName) > 3)
                        _fstrcat(lpOpenFileBox->lpFileName, "\\");
                    _fstrcat(lpOpenFileBox->lpFileName, filename);
                    
                    // set directory to the default directory.
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                    return CLOSE;
                    
                case IDC_FILE:
                    if(GetListItemNum(pDlgHeader, IDC_FILE) < 1)
                        return DEFAULT;
                        
                    // get path name
                    _getdcwd(_getdrive(), filename, _MAX_PATH);
                    _fstrcpy(lpOpenFileBox->lpFileName, filename);
                    iIndex = GetSelectedItem(pDlgHeader, IDC_FILE);
                    GetListBoxItem(pDlgHeader, IDC_FILE, filename, iIndex);

                    if(!lpOpenFileBox->fOpen)
                    {
                        if(MessageBox(NULL, "给定的文件已存在，是否替换？", "文件另存为", 
                                MB_YESNO | MB_ICONQUESTION) == IDNO)
                            return DEFAULT;
                    }
                    if(_fstrlen(lpOpenFileBox->lpFileName) > 3)
                        _fstrcat(lpOpenFileBox->lpFileName, "\\");
                    _fstrcat(lpOpenFileBox->lpFileName, filename);

                    // set directory to the default directory.
                    _chdrive(lpOpenFileBox->iInitDrive);
                    _fstrcpy(filename, lpOpenFileBox->lpInitDir);
                    _chdir(filename);
                    return CLOSE;
                break;
                
                default:
                break;
            }
        break;

        case DRAWTEXT:
            if( wID == IDC_DIRECTORY)
            {
                _getdcwd(_getdrive(), filename, _MAX_PATH);
                cc_wt16(filename, ((PCTRLDATA)lParam)->lx, ((PCTRLDATA)lParam)->ty);
                return OWNER;
            }
        return DEFAULT;
        
        default:
        return DEFAULT;
    }
    return DEFAULT;
}

/*
 * Function: BOOL GUIAPI DialogBoxParam( PGUIINFO pGUIInfo, int Index, BOOL (FAR *DlgProc)(),LPSTR lpStr );
 *      This function create dialog box, and manage dialog. This is common
 *   funtion.
 * Parameters:
 *      Index: index of dialog box;
 *      DlgProc: a pointer to the dialog process function;
 *      pStr: the parameter that give the DlgProc function, 
 *            it often is a pointer.
 * Return: 
 *      success----TRUE;
 *      failure----FALSE, maybe cannot create swape file.
 */
BOOL GUIAPI DialogBoxParam( PGUIINFO pGUIInfo, PDLGHEADER pDlgHeader, int (FAR *DlgProc)(PDLGHEADER, UINT, WORD, LONG), LPSTR lpStr)
{   
    MSG Msg;
    _vmhnd_t hdlImage;
    
    SetPtrVis(HIDE);
    SetFocus(NULL);
    if(pGUIInfo)
    {
        do
        {
            if(GetMessageFromQueue(pGUIInfo, &Msg))
            {
                TranslateAccelerator(pGUIInfo, &Msg);
                DisptchMessage(pGUIInfo, &Msg);
            }
            else
                break;
        }while(TRUE);
        
        Msg.uMsg = MSG_KILLFOCUS;
        Msg.wParam = 0;
        Msg.lParam = 0L;
        DisptchMessage(pGUIInfo, &Msg);
    }
    else
        hdlImage = SaveImageUseVirtualMem(
                            pDlgHeader->lx, 
                            pDlgHeader->ty,
                            pDlgHeader->rx - pDlgHeader->lx, 
                            pDlgHeader->by - pDlgHeader->ty);

    CtrlIndex = 0;
    fAlreadyUp = TRUE;
    
    if(!StartDialog(pDlgHeader))
    {
        SetPtrVis(SHOW);
        return FALSE;
    }
    DrawBox(pDlgHeader->lx, pDlgHeader->ty, pDlgHeader->rx, pDlgHeader->by);
    (*DlgProc)(pDlgHeader, INITDIALOG, (WORD)NULL, (LONG)lpStr);
    CreateDialog(pDlgHeader, DlgProc);
    SetPtrVis(SHOW);
    
    if( CLOSE == (*DlgProc)(pDlgHeader, INITIALIZED, 0, 0L))
        goto enddlg;
    
    Msg.uMsg = SETFOCUS;
    Msg.pStr = (PSTR)(GetNextTabstop(pDlgHeader, pDlgHeader->pCtrlData));
    
    GetKey(CLEAR);
    do
    {
        if( Msg.uMsg == SETFOCUS 
                || Msg.uMsg == FOCUSCOMMAND 
                || Msg.uMsg == COMMAND
                || Msg.uMsg == CANCEL) 
        {
            SetFocus(((PCTRLDATA)Msg.pStr));
            if(((PCTRLDATA)Msg.pStr)->bClass == EDIT)
                ProcEdit(pDlgHeader, ((PCTRLDATA)Msg.pStr), &Msg);
            else if(((PCTRLDATA)Msg.pStr)->bClass == LISTBOX)
                ProcListBox(pDlgHeader, ((PCTRLDATA)Msg.pStr), &Msg);
            else
            {
                do
                {
                    GetMessage(&Msg);
                }while(Msg.uMsg == NULLINPUT);
                TranslateBoxMsg(TRUE, pDlgHeader, &Msg);
            }
        }
        else
        {
            do
            {
                GetMessage(&Msg);
            }while(Msg.uMsg == NULLINPUT);
            TranslateBoxMsg(TRUE, pDlgHeader, &Msg);
        }
        
        switch(Msg.uMsg)
        {
            case NEXTTAB:
                Msg.uMsg = SETFOCUS;
                Msg.pStr = (PSTR)GetNextTabstop(pDlgHeader, ((PCTRLDATA)Msg.pStr));
                if(((PCTRLDATA)Msg.pStr)->lStyle & WS_GROUP)
                {
                    PCTRLDATA pCtrlBuff;
                    pCtrlBuff = ((PCTRLDATA)Msg.pStr);
                    
                    while(TRUE)
                    {   
                        pCtrlBuff = GetNextItemInGroup(pDlgHeader, pCtrlBuff);
                        
                        if( pCtrlBuff->lStyle & WS_CHECKED)
                            break;
                        if( pCtrlBuff == ((PCTRLDATA)Msg.pStr) )
                            break;
                    }
                    Msg.pStr = (PSTR)pCtrlBuff;
                }
            break;
            
            case LASTTAB:
                Msg.uMsg = SETFOCUS;
                if(!(((PCTRLDATA)Msg.pStr)->lStyle & WS_TABSTOP))
                    Msg.pStr = (PSTR)GetLastTabstop(pDlgHeader, ((PCTRLDATA)Msg.pStr));
                Msg.pStr = (PSTR)GetLastTabstop(pDlgHeader, ((PCTRLDATA)Msg.pStr));
                if(((PCTRLDATA)Msg.pStr)->lStyle & WS_GROUP)
                {
                    PCTRLDATA pCtrlBuff;
                    pCtrlBuff = ((PCTRLDATA)Msg.pStr);
                    
                    while(TRUE)
                    {   
                        pCtrlBuff = GetLastItemInGroup(pDlgHeader, pCtrlBuff);
                        
                        if( pCtrlBuff->lStyle & WS_CHECKED)
                            break;
                        if( pCtrlBuff == ((PCTRLDATA)Msg.pStr) )
                            break;
                    }
                    Msg.pStr = (PSTR)pCtrlBuff;
                }
            break;
            
            case NEXTGROUP:
                Msg.uMsg = FOCUSCOMMAND;
                do
                {
                    Msg.pStr = (PSTR)GetNextItemInGroup(pDlgHeader, ((PCTRLDATA)Msg.pStr));
                }while(((PCTRLDATA)Msg.pStr)->lStyle & WS_DISABLED);
                CheckCtrl(pDlgHeader, ((PCTRLDATA)Msg.pStr));
            break;
            
            case LASTGROUP:
                Msg.uMsg = FOCUSCOMMAND;
                do
                {
                    Msg.pStr = (PSTR)GetLastItemInGroup(pDlgHeader, ((PCTRLDATA)Msg.pStr));
                }while(((PCTRLDATA)Msg.pStr)->lStyle & WS_DISABLED);
                CheckCtrl(pDlgHeader, ((PCTRLDATA)Msg.pStr));
            break;
            
            case CANCEL:
                if(HaveCancelButton(pDlgHeader))
                {
                    Msg.uMsg = COMMAND;
                    Msg.pStr = (PSTR)GetControlData(pDlgHeader, IDCANCEL);
                }
                else
                    break;
            break;
            
            case CHECK:
                Msg.uMsg = COMMAND;
                if(((((PCTRLDATA)Msg.pStr)->lStyle & 0x0000000fL) == BS_PUSHBUTTON) &&
                    (((PCTRLDATA)Msg.pStr)->bClass == BUTTON))
                {
                    PushButton(((PCTRLDATA)Msg.pStr), TRUE);
                    Sleep(100);
                    PushButton(((PCTRLDATA)Msg.pStr), FALSE);
                }
                else
                    CheckCtrl(pDlgHeader, ((PCTRLDATA)Msg.pStr));
            break;
            
            case OK:
                Msg.uMsg = COMMAND;
                if(((((PCTRLDATA)Msg.pStr)->lStyle & 0x0000000fL) == BS_PUSHBUTTON) &&
                    (((PCTRLDATA)Msg.pStr)->bClass == BUTTON))
                {
                    PushButton(((PCTRLDATA)Msg.pStr), TRUE);
                    Sleep(100);
                    PushButton(((PCTRLDATA)Msg.pStr), FALSE);
                }
                else
                {
                    Msg.pStr = (PSTR)GetControlData(pDlgHeader, IDOK);
                    SetFocus(((PCTRLDATA)Msg.pStr));
                }
                break;
                
            case BEEP:
                Msg.uMsg = NONE;
                Beep(700, 50);
                break;
                
            case MOUSEFOCUS:
                Msg.uMsg = SETFOCUS;
                if(((((PCTRLDATA)Msg.pStr)->lStyle & 0x0000000fL) == BS_PUSHBUTTON) &&
                    (((PCTRLDATA)Msg.pStr)->bClass == BUTTON))
                    PushButton(((PCTRLDATA)Msg.pStr), TRUE);
                break;
                
            case MOUSECHECK:
                if(((((PCTRLDATA)Msg.pStr)->lStyle & 0x0000000fL) == BS_PUSHBUTTON) &&
                    (((PCTRLDATA)Msg.pStr)->bClass == BUTTON))
                    PushButton(((PCTRLDATA)Msg.pStr), FALSE);
                if(Msg.wParam)
                {
                    if(((PCTRLDATA)Msg.pStr)->lStyle & WS_DISABLED)
                    {
                        Msg.uMsg = NONE;
                        break;
                    }
                    Msg.uMsg = COMMAND;
                    CheckCtrl(pDlgHeader, ((PCTRLDATA)Msg.pStr));
                }
                else
                    Msg.uMsg = NONE;
                break;
                
            case FOCUSCOMMAND:
            case COMMAND:
                break;
                
            default:
                Msg.uMsg = NONE;
                break;
        }
        
        if(Msg.pStr == NULL)
            continue;

        if( CLOSE == (*DlgProc)(pDlgHeader, Msg.uMsg, ((PCTRLDATA)Msg.pStr)->wID, (LONG)NULL))
            break;
            
    }while(TRUE);

enddlg:
    SetFocus(NULL);
    EndDialog(pDlgHeader);
    
    SetPtrVis(HIDE);
    if(pGUIInfo)
    {
        RECT rect;
        
        rect.left   = pDlgHeader->lx;
        rect.top    = pDlgHeader->ty;
        rect.right  = pDlgHeader->rx;
        rect.bottom = pDlgHeader->by;

        SendMessage(pGUIInfo, MSG_ERASEBKGND, (WPARAM)(&rect), 0L);

        PostMessage(pGUIInfo, MSG_SETFOCUS, 0, 0L);
    }
    else if(hdlImage != _VM_NULL)
        RestoreImageUseVirtualMem(hdlImage, pDlgHeader->lx, pDlgHeader->ty);

    SetPtrVis(SHOW);
    
    return TRUE;
}

/*
 * Function: static BOOL StartDialog( PDLGHEADER pDlgHeader );
 *      This function draw dialog box. This is a common function.
 * Parameters:
 *      pDlgHeader: a pointer to DLGHEADER struct;
 * Return: 
 *      success----TRUE;
 *      failure----FALSE.
 */
static BOOL StartDialog( PDLGHEADER pDlgHeader )
{
    int i;
    
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {
        // Allocate memory for special controls.
        switch((pDlgHeader->pCtrlData + i)->bClass)
        {
            case LISTBOX:
            {
                LPLISTBOXBUFF lpBuff;
                
                lpBuff = (LPLISTBOXBUFF)_fmalloc(sizeof(LISTBOXBUFF));
                if(lpBuff == NULL)
                    return FALSE;
                else
                {
                    lpBuff->iWndOffset = 0;
                    lpBuff->iCurSel = -1;
                    lpBuff->iCount = 0;
                    lpBuff->pHead = NULL;
                    (pDlgHeader->pCtrlData + i)->slpAdd = (LPSTR)lpBuff;
                }
                break;
            }
            case EDIT:
            {
                LPEDITBUFF lpBuff;
                
                lpBuff = (LPEDITBUFF)_fmalloc(sizeof(EDITBUFF));
                if(lpBuff == NULL)
                    return FALSE;
                else
                {
                    _fmemset(lpBuff, 0, sizeof(EDITBUFF));
                    (pDlgHeader->pCtrlData + i)->slpAdd = (LPSTR)lpBuff;
                }
                break;
            }
            default:
                break;
        }
    }
}

/*
 * Function: void GUIAPI EndDialog( PDLGHEADER pDlgHeader );
 *      This function draw dialog box. This is a common function.
 * Parameters:
 *      pDlgHeader: a pointer to DLGHEADER struct;
 * Return: 
 *      success----TRUE;
 *      failure----FALSE.
 */
static void EndDialog( PDLGHEADER pDlgHeader )
{
    int i;
    
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {
        if((pDlgHeader->pCtrlData + i)->bClass == LISTBOX)
        {
            ReleaseListBoxBuff(pDlgHeader->pCtrlData + i);
            _ffree((pDlgHeader->pCtrlData + i)->slpAdd);
        }
        else if((pDlgHeader->pCtrlData + i)->bClass == EDIT)
            _ffree((pDlgHeader->pCtrlData + i)->slpAdd);
    }
}

/*
 * Function: BOOL GUIAPI CreateDialog( PDLGHEADER pDlgHeader, int (FAR *DlgProc)());
 *      This function draw dialog box. This is a common function.
 * Parameters:
 *      pDlgHeader: a pointer to DLGHEADER struct;
 *      DlgProc   : a pointer to dialog callback function.
 * Return: 
 *      success----TRUE;
 *      failure----FALSE.
 */
static BOOL CreateDialog( PDLGHEADER pDlgHeader, int (FAR *DlgProc)() )
{
    int i;
    
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {
        if((*DlgProc)(pDlgHeader, DRAWCTRL, (pDlgHeader->pCtrlData + i)->wID, (LPSTR)(pDlgHeader->pCtrlData + i)) == OWNER)
            continue;
    
        DrawCtrl((pDlgHeader->pCtrlData + i)->bClass,
                 (pDlgHeader->pCtrlData + i)->lStyle,
                 (pDlgHeader->pCtrlData + i)->lx,
                 (pDlgHeader->pCtrlData + i)->ty,
                 (pDlgHeader->pCtrlData + i)->rx,
                 (pDlgHeader->pCtrlData + i)->by);
    }
    
    // Draw copyright information.

    if( !Bitmap(szDefaultLogo, 5, pDlgHeader->lx + 8, pDlgHeader->ty + 22))
    {
        set_color( 5 );
        cc_wt16(szMiniGUI, pDlgHeader->lx + 8, pDlgHeader->ty + 6);
    }

    set_color(15);
    cc_wt16(pDlgHeader->spCaption, 
                (pDlgHeader->lx + pDlgHeader->rx)/2 - GetOutWidth(pDlgHeader->spCaption)/2,
                pDlgHeader->ty + 8);
                
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {
        if((*DlgProc)(pDlgHeader, DRAWTEXT, (pDlgHeader->pCtrlData + i)->wID, (LPSTR)(pDlgHeader->pCtrlData + i)) == OWNER)
            continue;
        
        DrawCtrlClientArea(pDlgHeader->pCtrlData + i);
    }
    
    return TRUE;
}

/*
 * Function: BOOL HaveCancelButton()
 *      This function determine that if the dialog box have a button of IDCANCEL
 * Parameters:
 *      none.
 * Return: 
 *      TRUE: have a button of IDCANCEL;
 *      FALSE: have none button of IDCANCEL;
 */
static BOOL HaveCancelButton( PDLGHEADER pDlgHeader )
{
    short i;
    PCTRLDATA pCtrlData;
    
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {   
        pCtrlData = pDlgHeader->pCtrlData + i;
        if((pCtrlData->wID) == IDCANCEL)
            return TRUE;
    }
    
    return FALSE;
}

/*
 * Function: PCTRLDATA GUIAPI GetControlData( PDLGHEADER pDlgHeader, WORD wID ).
 *      This function get the pointer to CONTROLDATA struct 
 *     according to control's ID.
 * Parameters:
 *      wID: ID of control;
 * Return: 
 *      success----the pointer to CONTROLDATA struct;
 *      failure----NULL.
 */
PCTRLDATA GUIAPI GetControlData( PDLGHEADER pDlgHeader, WORD wID )
{
    short i;
    PCTRLDATA pCtrlData;
    
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {   
        pCtrlData = pDlgHeader->pCtrlData + i;
        if((pCtrlData->wID) == wID)
            return pCtrlData;
    }
    
    return NULL;
}

void GUIAPI GetControlRect( PDLGHEADER pDlgHeader, WORD wID, LPRECT lpRect)
{
    PCTRLDATA pCtrlData = GetControlData(pDlgHeader, wID);
    
    if(pCtrlData == NULL)
        return;
    
    lpRect->left = pCtrlData->lx;
    lpRect->top = pCtrlData->ty;
    lpRect->right = pCtrlData->rx;
    lpRect->bottom = pCtrlData->by;
}

/*
 * Function: static PCTRLDATA GetNextTabstop( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData ).
 *      This function get the next control with style of WS_TABSTOP.
 * Parameters:
 *      pCtrlData: given control's pointer;
 * Return: 
 *      success----pointer to expect control;
 *      failure----given pointer.
 */
static PCTRLDATA GetNextTabstop( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData )
{
    short i;
    PCTRLDATA pFirst = NULL;
            
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {   
        if(((pDlgHeader->pCtrlData + i)->lStyle) & WS_TABSTOP)
        {
            pFirst = pDlgHeader->pCtrlData + i;
            break;
        }
    }
    if(pFirst == NULL)
        return pCtrlData;
    
    while(pCtrlData != (pDlgHeader->pCtrlData + pDlgHeader->NumOfItems-1))
    {
        pCtrlData++;
        if((pCtrlData->lStyle) & WS_TABSTOP)
            return  pCtrlData;
            
    }
    
    return pFirst;
}
/*
 * Function: static PCTRLDATA GetLastTabstop( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData ).
 *      This function get the previous control with style of WS_TABSTOP.
 * Parameters:
 *      pCtrlData: given control's pointer;
 * Return: 
 *      success----pointer to expect control;
 *      failure----pointer to given pointer.
 */
static PCTRLDATA GetLastTabstop( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData )
{
    short i;
    PCTRLDATA pLast = NULL;
            
    for(i=0; i<pDlgHeader->NumOfItems; i++)
    {   
        if(((pDlgHeader->pCtrlData + i)->lStyle) & WS_TABSTOP)
            pLast = pDlgHeader->pCtrlData + i;
    }
    if(pLast == NULL)
        return pCtrlData;
    
    while( pCtrlData != pDlgHeader->pCtrlData )
    {
        pCtrlData--;
        if((pCtrlData->lStyle) & WS_TABSTOP)
            return  pCtrlData;
    }
    
    return pLast;
}
/*
 * Function: static PCTRLDATA GetNextItemInGroup( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData ).
 *      This function get the next control that is in a group with 
 *     given control.
 * Parameters:
 *      pCtrlData: given control's pointer;
 * Return: 
 *      success----pointer to expect control;
 *      failure----given pointer.
 */
static PCTRLDATA GetNextItemInGroup( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData )
{
    short i = 0;
    PCTRLDATA pFirstInGrp = NULL;
                     
    do
    {
        if(((pDlgHeader->pCtrlData+i)->lStyle) & WS_GROUP)
            pFirstInGrp = pDlgHeader->pCtrlData + i;
        i++;
    }while((pDlgHeader->pCtrlData+i) != pCtrlData);
    
    if(pCtrlData->lStyle & WS_GROUP)
        pFirstInGrp = pCtrlData;
        
    if(pFirstInGrp == NULL)
        return pCtrlData;
        
    pCtrlData++;
    if(pCtrlData == (pDlgHeader->pCtrlData + pDlgHeader->NumOfItems))
        return pFirstInGrp;
        
    if(!(pCtrlData->lStyle & WS_GROUP))
        return pCtrlData;
    
    return pFirstInGrp;
}

/*
 * Function: static PCTRLDATA GetLastItemInGroup( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData ).
 *      This function get the previous control that is a group with 
 *     given control.
 * Parameters:
 *      pCtrlData: given control's pointer;
 * Return: 
 *      success----pointer to expect control;
 *      failure----given pointer 
 */
static PCTRLDATA GetLastItemInGroup( PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData )
{
    short i = 1;
    PCTRLDATA pLastInGrp = NULL;
                     
    while((pCtrlData + i) != (pDlgHeader->pCtrlData + pDlgHeader->NumOfItems))
    {
        if(((pCtrlData + i)->lStyle) & WS_GROUP)
        {
            pLastInGrp = pCtrlData + i - 1;
            break;
        }
        i++;
    }
    
    if(pLastInGrp == NULL)
        return pCtrlData;
        
    if(pCtrlData->lStyle & WS_GROUP)
        return pLastInGrp;
    else
        return --pCtrlData;
}

/*
 * Function: void GUIAPI GrayDlgItem( PDLGHEADER pDlgHeader, WORD wID ).
 *      This function gray/disable the given control. 
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure
 *      wID: given control's ID;
 * Return: 
 *      None.
 */
void GUIAPI GrayDlgItem( PDLGHEADER pDlgHeader, WORD wID)
{    
    PCTRLDATA pCtrlData;
    
    if(!(pCtrlData = GetControlData(pDlgHeader, wID)))
        return;
    
    if(pCtrlData->lStyle & WS_DISABLED)
        return;
    
    if(pCtrlData->lStyle & WS_CHECKED)
    {
        pCtrlData->lStyle &= 0xffffefffL;
        _setcolor(7);
        _ellipse(_GFILLINTERIOR, pCtrlData->lx + 4, pCtrlData->ty + 4,pCtrlData->lx + 12,pCtrlData->ty + 12);
                    pCtrlData->lStyle &= 0xffffefffL;
    }       
    pCtrlData->lStyle |= WS_DISABLED;
    
    _setcolor(COLOR_lightgray);
    _rectangle(_GFILLINTERIOR, pCtrlData->lx - 1, pCtrlData->ty - 1, pCtrlData->rx + 1, pCtrlData->by + 1);
    
    DrawCtrl(pCtrlData->bClass, pCtrlData->lStyle, 
        pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);
    DrawCtrlClientArea(pCtrlData);
}

/*
 * Function: void GUIAPI UngrayDlgItem( PDLGHEADER pDlgHeader, WORD wID ).
 *      This function ungray/enable the given control. 
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 * Return: 
 *      None.
 */
void GUIAPI UngrayDlgItem( PDLGHEADER pDlgHeader, WORD wID)
{    
    PCTRLDATA pCtrlData;
    
    if(!(pCtrlData = GetControlData(pDlgHeader, wID)))
        return;
    
    if(!(pCtrlData->lStyle & WS_DISABLED))
        return;
        
    pCtrlData->lStyle &= 0xf7ffffffL;
    
    DrawCtrl(pCtrlData->bClass, pCtrlData->lStyle, 
        pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);
    DrawCtrlClientArea(pCtrlData);
}

void GUIAPI ShowDlgItem( PDLGHEADER pDlgHeader, WORD wID )
{
    PCTRLDATA pCtrlData;
    
    if(!(pCtrlData = GetControlData(pDlgHeader, wID)))
        return;
    
    pCtrlData->lStyle &= 0xf7ffffffL;
    
    DrawCtrl(pCtrlData->bClass, pCtrlData->lStyle, 
        pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);
    DrawCtrlClientArea(pCtrlData);
}

void GUIAPI HideDlgItem( PDLGHEADER pDlgHeader, WORD wID )
{
    PCTRLDATA pCtrlData;
    
    if(!(pCtrlData = GetControlData(pDlgHeader, wID)))
        return;
    
    pCtrlData->lStyle |= WS_DISABLED;
    
    _setcolor(COLOR_lightgray);
    _rectangle(_GFILLINTERIOR, pCtrlData->lx - 1, pCtrlData->ty - 1, pCtrlData->rx + 1, pCtrlData->by + 1);
}

/*
 * Function: void GUIAPI DrawTextInStaticItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr )
 *      This function output text in the specified static control.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 *      lpStr: the text that want to output;
 * Return: 
 *      None.
 */
void GUIAPI DrawTextInStaticItem( PDLGHEADER pDlgHeader, WORD wID, LPCSTR lpStr )
{
    PCTRLDATA pCtrlData;
    short buttonx, buttony;
    
    if(!(pCtrlData = GetControlData(pDlgHeader, wID)))
        return;
    
    set_cliprgn(pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);

    _setcolor(7);
    _rectangle(_GFILLINTERIOR, pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);
    
    buttony = (pCtrlData->ty
            + pCtrlData->by)/2
            - 8;
    switch((pCtrlData->lStyle) & 0x0000000fL)
    {
        case ES_RIGHT:
            buttonx = pCtrlData->rx
                    - _fstrlen(lpStr) * 8;
        break;
        
        case ES_LEFT:
            buttonx = pCtrlData->lx;
        break;
        
        case ES_CENTER:
            buttonx = (pCtrlData->lx
                        + pCtrlData->rx)/2
                    - _fstrlen(lpStr) * 4;
        break;
        
        default:
            buttonx = pCtrlData->lx;
        break;
        
    }
    
    set_color(0);
    if(pCtrlData->lStyle & WS_DISABLED)
        SetGrayTextMask();
    cc_printf(lpStr, buttonx, buttony);
    if(pCtrlData->lStyle & WS_DISABLED)
        RemoveDisplayMask();
        
    set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);
}

static BOOL bIsSysMsgBox = FALSE;
/*
 * Function: int GUIAPI SysMessageBox( PGUIINFO pGUIInfo, int Index, LPSTR lpImpMsg).
 *      This function manage a system message box. 
 * Parameters:
 *      Index: the index of MessageBox;
 *      lpImpMsg: the pointer to important message string.
 * Return: 
 *      the choice of MessageBox, such as OK, CANCEL etc..
 */
int GUIAPI SysMessageBox( PGUIINFO pGUIInfo, LPCSTR lpszText, LPCSTR lpszCaption, DWORD lStyle)
{
    int iRet;
    
    bIsSysMsgBox = TRUE;
    iRet = MessageBox(pGUIInfo, lpszText, lpszCaption, lStyle);
    bIsSysMsgBox = FALSE;
    
    return iRet;
}

/*
 * Function: int GUIAPI MessageBox( PGUIINFO pGUIInfo, int Index, LPSTR lpImpMsg).
 *      This function manage a message box. 
 * Parameters:
 *      Index: the index of MessageBox;
 *      lpImpMsg: the pointer to important message string.
 * Return: 
 *      the choice of MessageBox, such as OK, CANCEL etc..
 */
int GUIAPI MessageBox( PGUIINFO pGUIInfo, LPCSTR lpszText, LPCSTR lpszCaption, DWORD lStyle)
{
    int          i;
    int          iNumOfRow, iMaxCol, iCol;
    short        Width, Height;
    _vmhnd_t     hdlImage;
    int          iRetCtrl = 0;
    int          CtrlRet[3];
    int          RetValue;
    MSG          Msg;
    BOOL         fEND = FALSE;
    CTRLDATA     CtrlData[3] = {
                    {BS_PUSHBUTTON | WS_TABSTOP,
                     0, 0, 0, 0, 1, BUTTON, NULL, NULL},
                    {BS_PUSHBUTTON | WS_TABSTOP,
                     0, 0, 0, 0, 1, BUTTON, NULL, NULL},
                    {BS_PUSHBUTTON | WS_TABSTOP,
                     0, 0, 0, 0, 1, BUTTON, NULL, NULL}
                 };

    Width  = 80;
    Height = 140;
    
    MsgBoxData.lStyle = lStyle;
    MsgBoxData.lpMsg = lpszText;
    if(lpszCaption)
        MsgBoxData.lpCaption = lpszCaption;
    else
        MsgBoxData.lpCaption = pGUIInfo->spCaption;
    MsgBoxData.pCtrlData = &CtrlData[0];
    
    switch(MsgBoxData.lStyle&0x000f)
    {
        case MB_OK:
            MsgBoxData.NumOfCtrl = 1;
            CtrlRet[0] = IDOK;
            CtrlData[0].spItemText = "确定";
        break;
        
        case MB_OKCANCEL:
            Width += 100;
            MsgBoxData.NumOfCtrl = 2;
            CtrlRet[0] = IDOK;
            CtrlRet[1] = IDCANCEL;
            CtrlData[0].spItemText = "确定";
            CtrlData[1].spItemText = "取消";
            iRetCtrl = 1;
        break;                   
        
        case MB_YESNO:
            Width += 100;
            MsgBoxData.NumOfCtrl = 2;
            CtrlRet[0] = IDYES;
            CtrlRet[1] = IDNO;
            CtrlData[0].spItemText = "是(Y)";
            CtrlData[1].spItemText = "否(N)";
            iRetCtrl = 1;
        break;
        
        case MB_RETRYCANCEL:
            Width += 100;
            MsgBoxData.NumOfCtrl = 2;
            CtrlRet[0] = IDRETRY;
            CtrlRet[1] = IDCANCEL;
            CtrlData[0].spItemText = "重试(R)";
            CtrlData[1].spItemText = "取消";
        break;
        
        case MB_ABORTRETRYIGNORE:
            Width += 200;
            MsgBoxData.NumOfCtrl = 3;
            CtrlRet[0] = IDABORT;
            CtrlRet[1] = IDRETRY;
            CtrlRet[2] = IDIGNORE;
            CtrlData[0].spItemText = "中止(A)";
            CtrlData[1].spItemText = "重试(R)";
            CtrlData[2].spItemText = "忽略(I)";
        break;
        
        case MB_YESNOCANCEL:
            Width += 200;
            MsgBoxData.NumOfCtrl = 3;
            CtrlRet[0] = IDYES;
            CtrlRet[1] = IDNO;
            CtrlRet[2] = IDCANCEL;
            CtrlData[0].spItemText = "是(Y)";
            CtrlData[1].spItemText = "否(N)";
            CtrlData[2].spItemText = "取消";
        break;
    }
    
    i = 0;
    iNumOfRow = 1;
    iMaxCol = 0;
    iCol = 0;
    while(*(MsgBoxData.lpMsg + i) != '\0')
    {
        if(*(MsgBoxData.lpMsg + i) == '\n')
        {
            iNumOfRow ++;
            if(iCol > iMaxCol)
            {
                iMaxCol = iCol;
                iCol = 1;
            }
        }
        
        if(*(MsgBoxData.lpMsg + i) == '\t')
            iCol += 4;
        iCol++;
        
        i++;
    }
    
    if(iCol > iMaxCol)
        iMaxCol = iCol;
        
    if((iMaxCol*8) > (Width - 20))
        Width = iMaxCol*8 + 20;
    
    if((size_t)Width < (_fstrlen(lpszCaption)*8 + 50))
        Width = _fstrlen(lpszCaption)*8 + 50;
        
    Height += iNumOfRow*16;
    
    if((MsgBoxData.lStyle)&0x00f0)
        Width += 100;
    else
        Width += 70;
        
    MsgBoxData.lx = (vc.numxpixels - Width)/2;
    MsgBoxData.ty = (vc.numypixels - Height)/2;
    MsgBoxData.rx = MsgBoxData.lx + Width;
    MsgBoxData.by = MsgBoxData.ty + Height;
    
    SetPtrVis(HIDE);
    SetFocus(NULL);
    if(pGUIInfo)
    {
        do
        {
            if(GetMessageFromQueue(pGUIInfo, &Msg))
            {
                TranslateAccelerator(pGUIInfo, &Msg);
                DisptchMessage(pGUIInfo, &Msg);
            }
            else
                break;
        }while(TRUE);
        
        Msg.uMsg = MSG_KILLFOCUS;
        Msg.wParam = 0;
        Msg.lParam = 0L;
        DisptchMessage(pGUIInfo, &Msg);
    }
    else
        hdlImage = SaveImageUseVirtualMem(MsgBoxData.lx, MsgBoxData.ty,
                            Width, Height);

    CreateMsgBox(&MsgBoxData);
    
    if((MsgBoxData.lStyle)&0x00f0)
        DisplayIcon(LoadMsgIcon(MsgBoxData.lStyle), MsgBoxData.lx + 20, MsgBoxData.ty + 70);
    SetPtrVis(SHOW);
    
    for(i=0; i<MsgBoxData.NumOfCtrl; i++)
    {
        CtrlData[i].lx = MsgBoxData.xfirst + i*100;
        CtrlData[i].ty = MsgBoxData.by - 42;
        CtrlData[i].by = MsgBoxData.by - 17;
        CtrlData[i].rx = MsgBoxData.xfirst + i*100 + 80;
    }
    
    SetFocus(&CtrlData[iRetCtrl]);
    
    fAlreadyUp = TRUE;
    GetKey(CLEAR);
    while(!fEND)
    {
        MSG Msg;
        
        GetMessage(&Msg);
        
        TranslateBoxMsg(FALSE, NULL, &Msg);
        
        switch(Msg.uMsg)
        {
            case NEXTGROUP:
            case NEXTTAB:
                iRetCtrl++;
                if(iRetCtrl == MsgBoxData.NumOfCtrl)
                    iRetCtrl = 0;
                SetFocus(&CtrlData[iRetCtrl]);
            break;
            
            case LASTGROUP:
            case LASTTAB:
                iRetCtrl--;
                if(iRetCtrl == -1)
                    iRetCtrl = MsgBoxData.NumOfCtrl - 1;
                SetFocus(&CtrlData[iRetCtrl]);
            break;
            
            case ABORT:
                switch(MsgBoxData.lStyle & 0x000f)
                {
                    case MB_ABORTRETRYIGNORE:
                        RetValue = IDABORT;
                        fEND = TRUE;
                    break;
                }
                break;
        
            case RETRY:
                switch(MsgBoxData.lStyle & 0x000f)
                {
                    case MB_RETRYCANCEL:
                    case MB_ABORTRETRYIGNORE:
                        RetValue = IDRETRY;
                        fEND = TRUE;
                    break;
                }
                break;
            
            case IGNORE:
                switch(MsgBoxData.lStyle & 0x000f)
                {
                    case MB_ABORTRETRYIGNORE:
                        RetValue = IDIGNORE;
                        fEND = TRUE;
                    break;
                }
                break;
            
            case CANCEL:
                switch(MsgBoxData.lStyle & 0x000f)
                {
                    case MB_OKCANCEL:
                    case MB_RETRYCANCEL:
                    case MB_YESNOCANCEL:
                        RetValue = IDCANCEL;
                        fEND = TRUE;
                    break;
                }
                break;
            
            case YES:
                switch(MsgBoxData.lStyle & 0x000f)
                {
                    case MB_YESNO:
                    case MB_YESNOCANCEL:
                        RetValue = IDYES;
                        fEND = TRUE;
                    break;
                }
                break;
        
            case NO:
                switch(MsgBoxData.lStyle & 0x000f)
                {
                    case MB_YESNO:
                    case MB_YESNOCANCEL:
                        RetValue = IDNO;
                        fEND = TRUE;
                    break;
                }
                break;
        
            case CHECK:
                PushButton(&CtrlData[iRetCtrl], TRUE);
                Sleep(100);
                PushButton(&CtrlData[iRetCtrl], FALSE);
            case OK:
                RetValue = CtrlRet[iRetCtrl];
                fEND = TRUE;
            break;
        
            case MOUSEFOCUS:
                SetFocus(((PCTRLDATA)Msg.pStr));
                PushButton(((PCTRLDATA)Msg.pStr), TRUE);
            break;
                
            case MOUSECHECK:
                PushButton(((PCTRLDATA)Msg.pStr), FALSE);
                if(Msg.wParam)
                {
                    RetValue = CtrlRet[Msg.wParam - 1];
                    fEND = TRUE;
                    break;
                }
                else
                    break;
                
            case BEEP:
                Beep(700, 50);
                break;
            default:
                break;
        }
    }
    
    SetFocus(NULL);
    SetPtrVis(HIDE);
    if(pGUIInfo)
    {
        RECT rect;
        
        rect.left   = MsgBoxData.lx;
        rect.top    = MsgBoxData.ty;
        rect.right  = MsgBoxData.rx;
        rect.bottom = MsgBoxData.by;

        SendMessage(pGUIInfo, MSG_ERASEBKGND, (WPARAM)(&rect), 0L);

        PostMessage(pGUIInfo, MSG_SETFOCUS, 0, 0L);
    }
    else if(hdlImage != _VM_NULL)
        RestoreImageUseVirtualMem(hdlImage, MsgBoxData.lx, MsgBoxData.ty);
    SetPtrVis(SHOW);
    
    return RetValue;
}

/*
 * Function: static BOOL CreateMsgBox( PMSGBOXDATA pMsgBoxData ).
 *      This function Create a message box.
 * Parameters:
 *      pMsgBoxData: the pointer to MSGBOXDATA struct;
 * Return: 
 *      success-----TRUE;
 *      failure-----FALSE.
 */
static BOOL CreateMsgBox( PMSGBOXDATA pMsgBoxData )
{   
    short buttonx;
    short buttony;
    BOOL  Have3 = FALSE;
    
    DrawBox(pMsgBoxData->lx, pMsgBoxData->ty, pMsgBoxData->rx, pMsgBoxData->by);
    
    buttonx = (pMsgBoxData->lx + pMsgBoxData->rx)/2 - 35;
    buttony = pMsgBoxData->by - 42;
    
    switch((pMsgBoxData->lStyle)&0x000f)
    {
        case MB_YESNOCANCEL:
        case MB_ABORTRETRYIGNORE:
            DrawCtrl(BUTTON, BS_PUSHBUTTON, buttonx, buttony, buttonx + 80, buttony + 25);
            buttonx += 100;
            Have3 = TRUE;
        case MB_RETRYCANCEL:
        case MB_YESNO:
        case MB_OKCANCEL:
            if(!Have3)  
                buttonx = (pMsgBoxData->lx + pMsgBoxData->rx)/2 + 10;
            DrawCtrl(BUTTON, BS_PUSHBUTTON, buttonx, buttony, buttonx + 80, buttony + 25);
            if(Have3)
                buttonx -=200;
            else
                buttonx -=100;
        case MB_OK:
            DrawCtrl(BUTTON, BS_PUSHBUTTON, buttonx, buttony, buttonx + 80, buttony + 25);
    }
    
    pMsgBoxData->xfirst = buttonx;
    
    buttonx += 40;
    buttony += 5;
                   
    set_color(0);
    switch((pMsgBoxData->lStyle)&0x000f)
    {
        case MB_YESNOCANCEL:
            cc_wt16(pMsgBoxData->pCtrlData->spItemText, buttonx - 20, buttony);
            buttonx += 100;
            cc_wt16((pMsgBoxData->pCtrlData + 1)->spItemText, buttonx - 20, buttony);
            buttonx +=100;                     
            cc_wt16((pMsgBoxData->pCtrlData + 2)->spItemText, buttonx - 16, buttony);
        break;
            
        case MB_ABORTRETRYIGNORE:
            cc_wt16((pMsgBoxData->pCtrlData)->spItemText, buttonx - 28, buttony);
            buttonx += 100;
            cc_wt16((pMsgBoxData->pCtrlData + 1)->spItemText, buttonx - 28, buttony);
            buttonx += 100;
            cc_wt16((pMsgBoxData->pCtrlData + 2)->spItemText, buttonx - 28, buttony);
        break;
            
        case MB_RETRYCANCEL:
            cc_wt16((pMsgBoxData->pCtrlData)->spItemText, buttonx - 28, buttony);
            buttonx += 100;
            cc_wt16((pMsgBoxData->pCtrlData + 1)->spItemText, buttonx - 16, buttony);
        break;
        
        case MB_YESNO:
            cc_wt16((pMsgBoxData->pCtrlData)->spItemText, buttonx - 20, buttony);
            buttonx += 100;
            cc_wt16((pMsgBoxData->pCtrlData + 1)->spItemText, buttonx - 20, buttony);
        break;
        
        case MB_OKCANCEL:
            cc_wt16((pMsgBoxData->pCtrlData)->spItemText, buttonx - 16, buttony);
            buttonx += 100;
            cc_wt16((pMsgBoxData->pCtrlData + 1)->spItemText, buttonx - 16, buttony);
        break;
        
        case MB_OK:
            cc_wt16((pMsgBoxData->pCtrlData)->spItemText, buttonx - 16, buttony);
        break;
    }

    // Draw copyright information.
    if(!bIsSysMsgBox || !Bitmap(szDefaultLogo, 5, pMsgBoxData->lx + 8, pMsgBoxData->ty + 22))
    {
        set_color( 5 );
        cc_wt16(szMiniGUI, pMsgBoxData->lx + 8, pMsgBoxData->ty + 6);
    }
    
    set_color(15);
    cc_wt16(pMsgBoxData->lpCaption, 
                (pMsgBoxData->lx + pMsgBoxData->rx)/2 - 4*_fstrlen(pMsgBoxData->lpCaption),
                pMsgBoxData->ty + 8);
    
    set_color(0);
    buttonx = pMsgBoxData->lx + ((pMsgBoxData->lStyle & 0x00f0)?70:40);
    buttony = pMsgBoxData->ty + 70;
    cc_printf(pMsgBoxData->lpMsg, buttonx, buttony);

    return TRUE;
}

/*
 * Function: viod GUIAPI DrawCtrl(BYTE bClass, DWORD lStyle, short buttonx, short buttony, short buttonw, short buttonh)
 *      This function draw control. 
 * Parameters:
 *      bClass: the class of given control;
 *      lStyle: the style of given control;
 *      buttonx, buttony, buttonw, buttony: give the position of control.
 * Return: 
 *      None.
 */
static BYTE bRadioGray[] = {0x03, 0x0C, 0x10, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x10};
static BYTE bRadioBlack[] = {0x03, 0x0C, 0x08, 0x10, 0x10, 0x10, 0x10, 0x08};
static BYTE bRadioWhite1[] = {0x03, 0x07, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x0C, 0x03};
static BYTE bRadioWhite2[] = {0xC8, 0xE8, 0xF4, 0xF4, 0xF4, 0xF4, 0xE8, 0xC8, 0x30, 0xC0};
static BYTE bCheckMask[] = {0x02, 0x06, 0x8E, 0xDC, 0xF8, 0x70, 0x20};
void GUIAPI DrawCtrl(BYTE bClass, DWORD lStyle, short buttonx, short buttony, short buttonw, short buttonh)
{
    int i;
    
    SetPtrVis(HIDE);
    switch(bClass)
    {
        case LISTBOX:
            _setcolor(15);
            _rectangle(_GFILLINTERIOR, buttonx, buttony, buttonw, buttonh);
            if(lStyle & WS_BORDER)
            {
                _setcolor(0);
                _moveto(buttonx - 2, buttonh + 2);
                _lineto(buttonx - 2, buttony - 2);
                _lineto(buttonw + 2, buttony - 2);
                _setcolor(14);
                _moveto(buttonx - 1, buttonh + 1);
                _lineto(buttonx - 1, buttony - 1);
                _lineto(buttonw + 1, buttony - 1);
                _setcolor(15);
                _moveto(buttonx - 2, buttonh + 2);
                _lineto(buttonw + 2, buttonh + 2);
                _lineto(buttonw + 2, buttony - 2);
                _setcolor(7);
                _moveto(buttonx - 1, buttonh + 1);
                _lineto(buttonw + 1, buttonh + 1);
                _lineto(buttonw + 1, buttony - 1);
            }
            if(lStyle & WS_VSCROLL)
            {
                _setcolor(7);
                _rectangle(_GFILLINTERIOR, buttonw - 15, buttony,
                                     buttonw, buttonh); 
                _setcolor(0);
                _rectangle(_GBORDER, buttonw - 15, buttony,
                                     buttonw, buttonh); 
            }
            break;
        case BUTTON:
            switch(lStyle & 0x0000000fL)
            {
            case BS_PUSHBUTTON:
                    _setcolor(7);
                    _rectangle(_GFILLINTERIOR, buttonx, buttony, buttonw, buttonh);
                    _setcolor(0);
                    _rectangle(_GBORDER, buttonx, buttony, buttonw, buttonh);
                    _setcolor(15);
                    _moveto(buttonx+1, buttonh-1);
                    _lineto(buttonx+1, buttony+1);
                    _lineto(buttonw-1, buttony+1);
                    _setcolor(14);
                    _moveto(buttonx+1, buttonh-1);
                    _lineto(buttonw-1, buttonh-1);
                    _lineto(buttonw-1, buttony+1);
                break;
                
                case BS_CHECKBOX:
                    _setcolor(15);
                    _rectangle(_GFILLINTERIOR, buttonx + 2,buttony + 2,buttonx + 14,buttony + 14);
                    _setcolor(14);
                    _moveto(buttonx + 2, buttony + 14);
                    _lineto(buttonx + 2, buttony + 2);
                    _lineto(buttonx + 14, buttony + 2);
                    _setcolor(0);
                    _moveto(buttonx + 3, buttony + 13);
                    _lineto(buttonx + 3, buttony + 3);
                    _lineto(buttonx + 13, buttony + 3);
                    _setcolor(7);
                    _moveto(buttonx + 3, buttony + 13);
                    _lineto(buttonx + 13, buttony + 13);
                    _lineto(buttonx + 13, buttony + 3);
                    if(lStyle & WS_CHECKED)
                    {
                        set_color(0);
                        for(i=0; i<7; i++)
                            disp_byte(buttonx+5, buttony+i+5, bCheckMask[i]);
                    }
                break;
                
                case BS_RADIOBUTTON:
                    set_color(14);
                    for(i=0; i<10; i++)
                        disp_byte(buttonx, buttony+i+2, bRadioGray[i]);
                    disp_byte(buttonx + 8, buttony + 2, 0xC0);
                    disp_byte(buttonx + 8, buttony + 3, 0x30);
                    set_color(0);
                    for(i=0; i<8; i++)
                        disp_byte(buttonx, buttony+i+3, bRadioBlack[i]);
                    set_color(15);
                    for(i=0; i<10; i++)
                        disp_byte(buttonx, buttony+i+4, bRadioWhite1[i]);
                    for(i=0; i<10; i++)
                        disp_byte(buttonx+8, buttony+i+4, bRadioWhite2[i]);
                    
                    if(lStyle & WS_CHECKED)
                    {
                        set_color(0);
                        disp_byte(buttonx + 6, buttony + 6, 0x60);
                        disp_byte(buttonx + 6, buttony + 7, 0xF0);
                        disp_byte(buttonx + 6, buttony + 8, 0xF0);
                        disp_byte(buttonx + 6, buttony + 9, 0x60);
                    }
                break;
                
                case BS_GROUPBOX:
                    _setcolor(15);
                    _rectangle(_GBORDER, buttonx + 1, buttony + 9, buttonw, buttonh);
                    _setcolor(14);
                    _rectangle(_GBORDER, buttonx, buttony + 8, buttonw-1, buttonh - 1);
                break;
            }
        break; 
        case STATIC:
            switch(lStyle & 0x0000000fL)
            {
                case SS_CENTER:
                case SS_LEFT:
                case SS_RIGHT:
                break;
                
                case SS_BLACKFRAME:
                    _setcolor(0);
                    _rectangle(_GBORDER, buttonx - 1, buttony - 1, buttonw + 1, buttonh + 1);
                    _setcolor(COLOR_lightwhite);
                    _rectangle(_GFILLINTERIOR, buttonx, buttony, buttonw, buttonh);
                break;
                
                case SS_BLACKRECT:
                    _setcolor(0);
                    _rectangle(_GFILLINTERIOR, buttonx, buttony, buttonw, buttonh);
                break;
            }
        break;
        case EDIT:
            switch(lStyle & 0x0000000fL)
            {
                case ES_LEFT:
                    _setcolor(15);
                    _rectangle(_GFILLINTERIOR, buttonx, buttony, buttonw, buttonh);
                    _setcolor(0);
                    _moveto(buttonx - 2, buttonh + 2);
                    _lineto(buttonx - 2, buttony - 2);
                    _lineto(buttonw + 2, buttony - 2);
                    _setcolor(14);
                    _moveto(buttonx - 1, buttonh + 1);
                    _lineto(buttonx - 1, buttony - 1);
                    _lineto(buttonw + 1, buttony - 1);
                    _setcolor(15);
                    _moveto(buttonx - 2, buttonh + 2);
                    _lineto(buttonw + 2, buttonh + 2);
                    _lineto(buttonw + 2, buttony - 2);
                    _setcolor(7);
                    _moveto(buttonx - 1, buttonh + 1);
                    _lineto(buttonw + 1, buttonh + 1);
                    _lineto(buttonw + 1, buttony - 1);
                break;
            }
        break;
    }

    SetPtrVis(SHOW);
}

void GUIAPI DrawCtrlClientArea(PCTRLDATA pCtrlData)
{
    short buttonx;
    short buttony;
    BOOL  fCW;

    SetPtrVis(HIDE);
    _setcolor(0);
    buttony = (pCtrlData->ty
            + pCtrlData->by)/2
            - 8;
    switch(pCtrlData->bClass)
    {
        case LISTBOX:
            FillListBox(pCtrlData);
            fCW = FALSE;
            break;
        case BUTTON:
            switch((pCtrlData->lStyle) & 0x0000000fL)
            { 
                case BS_CHECKBOX:
                case BS_RADIOBUTTON:
                    buttonx = pCtrlData->lx + 20;
                break;
                case BS_PUSHBUTTON:
                    buttonx = (pCtrlData->lx
                                + pCtrlData->rx)/2
                                - strlen(pCtrlData->spItemText) * 4;
                break;
                case BS_GROUPBOX:
                    buttony = pCtrlData->ty;
                    buttonx = pCtrlData->lx + 8;
                    _setcolor(7);
                    _rectangle(_GFILLINTERIOR, buttonx, buttony,
                                buttonx + strlen(pCtrlData->spItemText) * 8,
                                buttony + 16);
                    _setcolor(0);
                break;
            }
            fCW = TRUE;
        break;
        case EDIT:
            OutFieldEditText(pCtrlData, FALSE);
            fCW = FALSE;
        break;
        case STATIC:
            fCW = TRUE;
            switch((pCtrlData->lStyle) & 0x0000000fL)
            {
                case ES_RIGHT:
                    buttonx = pCtrlData->rx
                            - strlen(pCtrlData->spItemText) * 8;
                break;
                case ES_LEFT:
                    buttonx = pCtrlData->lx;
                break;
                case ES_CENTER:
                    buttonx = (pCtrlData->lx
                                + pCtrlData->rx)/2
                            - strlen(pCtrlData->spItemText) * 4;
                break;
                default:
                    fCW = FALSE;
                break;
            }
        break;
        default:
            fCW = FALSE;
        break;
    }
    
    if(fCW)
    {
        set_color(0);
        set_cliprgn(pCtrlData->lx,
             pCtrlData->ty,
             pCtrlData->rx,
             pCtrlData->by);
        if(pCtrlData->lStyle & WS_DISABLED)
            SetGrayTextMask();
            
        cc_printf(pCtrlData->spItemText, 
                buttonx,
                buttony);
        if(pCtrlData->lStyle & WS_DISABLED)
            RemoveDisplayMask();
                
        set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);
    }
    
    SetPtrVis(SHOW);
}

BOOL GUIAPI GetCheck( PDLGHEADER pDlgHeader, WORD wID )
{
    PCTRLDATA pCtrlData = GetControlData(pDlgHeader, wID);
    
    if(pCtrlData->lStyle & WS_CHECKED)
        return TRUE;
    else
        return FALSE;
}

void GUIAPI SetCheck( PDLGHEADER pDlgHeader, WORD wID, BOOL bCheck )
{
    PCTRLDATA pCtrlData = GetControlData(pDlgHeader, wID);
    
    if(GetCheck(pDlgHeader, wID) && !bCheck)
        CheckCtrl(pDlgHeader, pCtrlData);
    else if(!GetCheck(pDlgHeader, wID) && bCheck)
        CheckCtrl(pDlgHeader, pCtrlData);
}

/*
 * Function: static viod CheckCtrl(PDLGHEADER pDlgHeader, PCTRLDATA pCurCtrl)
 *      This function check control. 
 * Parameters:
 *      pDlgHeader: pointer to struct DLGHEADER;
 *      pCurCtrl:   pointer to struct CONTROLDATA;
 * Return: 
 *      None.
 */
static void CheckCtrl(PDLGHEADER pDlgHeader, PCTRLDATA pCurCtrl)
{
    int i;
    SetPtrVis(HIDE);
    
    switch(pCurCtrl->bClass)
    {
        case BUTTON:
            switch(pCurCtrl->lStyle & 0x0000000f)
            {
                case BS_PUSHBUTTON:
                    SetFocus(pCurCtrl);
                break;
                
                case BS_CHECKBOX:
                    if(pCurCtrl->lStyle & WS_CHECKED)
                    {
                        set_color(15);
                        pCurCtrl->lStyle &= 0xffffefffL;
                    }
                    else
                    {
                        set_color(0);
                        pCurCtrl->lStyle |= WS_CHECKED;
                    }
                    for(i=0; i<7; i++)
                        disp_byte(pCurCtrl->lx+5, pCurCtrl->ty+i+5, bCheckMask[i]);
                break;
                
                case BS_RADIOBUTTON:
                {
                    PCTRLDATA pCtrlData;
                    
                    pCurCtrl->lStyle |= WS_CHECKED;
                    pCtrlData = pCurCtrl;
                    
                        while(TRUE)
                        {
                            pCtrlData = GetNextItemInGroup(pDlgHeader, pCtrlData);
                            if(pCtrlData == pCurCtrl)
                                break;
                            if((pCtrlData != pCurCtrl) && (pCtrlData->lStyle & WS_CHECKED))
                            {
                                set_color(15);
                                disp_byte(pCtrlData->lx + 6, pCtrlData->ty + 6, 0x60);
                                disp_byte(pCtrlData->lx + 6, pCtrlData->ty + 7, 0xF0);
                                disp_byte(pCtrlData->lx + 6, pCtrlData->ty + 8, 0xF0);
                                disp_byte(pCtrlData->lx + 6, pCtrlData->ty + 9, 0x60);
                                pCtrlData->lStyle &= 0xffffefffL;
                            }
                            
                        }
                    
                        set_color(0);
                        disp_byte(pCurCtrl->lx + 6, pCurCtrl->ty + 6, 0x60);
                        disp_byte(pCurCtrl->lx + 6, pCurCtrl->ty + 7, 0xF0);
                        disp_byte(pCurCtrl->lx + 6, pCurCtrl->ty + 8, 0xF0);
                        disp_byte(pCurCtrl->lx + 6, pCurCtrl->ty + 9, 0x60);
                    }
                    break;
                
                case BS_GROUPBOX:
                break;
            }
        break;
        case STATIC:
        break;
        case EDIT:
        break;
    }

    SetPtrVis(SHOW);
}

/*
 * Function: static viod PushButton(PCTRLDATA pCurCtrl, BOOL uCheck)
 *      This function check control. 
 * Parameters:
 *      pCurCtrl: pointer to struct CONTROLDATA;
 *      uCheck:   TRUE----push down; FALSE----push up.  
 * Return: 
 *      None.
 */
static void PushButton(PCTRLDATA pCurCtrl, BOOL uCheck)
{
    SetPtrVis(HIDE);

    uCheck?_setcolor(14):_setcolor(15);
    _moveto(pCurCtrl->lx + 1, pCurCtrl->by - 1);
    _lineto(pCurCtrl->lx + 1, pCurCtrl->ty + 1);
    _lineto(pCurCtrl->rx - 1, pCurCtrl->ty + 1);
    uCheck?_setcolor(15):_setcolor(14);
    _moveto(pCurCtrl->lx + 1, pCurCtrl->by - 1);
    _lineto(pCurCtrl->rx - 1, pCurCtrl->by - 1);
    _lineto(pCurCtrl->rx - 1, pCurCtrl->ty + 1);
    
    SetPtrVis(SHOW);
}

/*
 * Function: static viod DrawBox( short lx, short ty, short rx, short by)
 *      This function draw dialog box or mssage box. 
 * Parameters:
 *      fDialog: TRUE --- draw dialog box, FALSE --- draw messagebox;
 *      lx:   left x;
 *      ty:   top y;
 *      rx:   right x;
 *      by:   below y;
 * Return: 
 *      None.
 */
static void DrawBox(short lx, short ty, short rx, short by)
{
    SetPtrVis(HIDE);

    _setcolor(7);
    _rectangle(_GFILLINTERIOR, lx, ty, rx, by);
    _setcolor(14);
    _rectangle(_GBORDER, lx, ty, rx - 1, by - 1);
    _setcolor(0);
    _moveto(lx, by);
    _lineto(rx, by);
    _lineto(rx, ty);
    _setcolor(15);
    _moveto(lx + 1, by - 1);
    _lineto(lx + 1, ty + 1);
    _lineto(rx - 1, ty + 1);

    _setcolor(8);
    _rectangle(_GFILLINTERIOR, lx + 6, ty + 6 , rx - 6, ty + 25);
    _setcolor(14);
    _moveto(lx + 5, ty + 25);
    _lineto(lx + 5, ty + 5);
    _lineto(rx - 5, ty + 5);
    _setcolor(15);
    _moveto(lx + 5, ty + 25);
    _lineto(rx - 5, ty + 25);
    _lineto(rx - 5, ty + 5);

    SetPtrVis(SHOW);
}

/*
 * Function: void  GUIAPI SetFocus( PCTRLDATA pCtrlData)
 *      This function set input focus to new control 
 * Parameters:
 *      pCtrlData: pointer to struct CTRLDATA;
 * Return: 
 *      None;
 */
void GUIAPI SetFocus(PCTRLDATA pCtrlData)
{
    static CTRLDATA CtrlData;
    
    _setwritemode(_GXOR);
    _setlinestyle(0xaaaa);
    _setcolor(7);

    if(pCtrlData)
        DrawFocus(pCtrlData);
        
    if(CtrlData.bClass != 0x00)
        DrawFocus(&CtrlData);
        
    if(pCtrlData == NULL)
        CtrlData.bClass = 0x00;
    else
        CtrlData = *pCtrlData;
    
    _setlinestyle(0xffff);
    _setwritemode(_GPSET);
}

/*
 * Function: static void DrawFocus( PCTRLDATA pCtrlData)
 *      This function Draw input focus to new control 
 * Parameters:
 *      pCtrlData: pointer to struct CTRLDATA;
 * Return: 
 *      None;
 */
static void DrawFocus( PCTRLDATA pCtrlData)
{
    SetPtrVis(HIDE);
    
    switch(pCtrlData -> bClass)
    {
        case BUTTON:
            switch(pCtrlData -> lStyle & 0x0000000fL)
            {
            case BS_PUSHBUTTON:
                    _rectangle(_GBORDER, 
                            pCtrlData->lx + 3,
                            pCtrlData->ty + 3,
                            pCtrlData->rx - 3,
                            pCtrlData->by - 3);
                break;
                
                case BS_CHECKBOX:
                case BS_RADIOBUTTON:
                    _rectangle(_GBORDER, 
                            pCtrlData->lx + 20,
                            (pCtrlData->ty + pCtrlData->by)/2 - 9,
                            pCtrlData->lx + strlen(pCtrlData->spItemText)*8 + 21,
                            (pCtrlData->ty + pCtrlData->by)/2 - 8 + 17);
                break;
                
            }
        break;
        
        case LISTBOX:
        {
            int iOffset;
            
            iOffset = ((LPLISTBOXBUFF)pCtrlData->slpAdd)->iWndOffset*16;
            
            _rectangle(_GBORDER, 
                    pCtrlData->lx, 
                    pCtrlData->ty + iOffset,
                    (pCtrlData->lStyle & WS_VSCROLL)?(pCtrlData->rx - 16):pCtrlData->rx, 
                    pCtrlData->ty + iOffset + 16);
        }
        break;
        
        default:
        break;
    }
    
    SetPtrVis(SHOW);        
}

/*
 * Function: void GUIAPI TranslateEditMsg(PDLGHEADER pDlgHeader, PMSG pMsg )
 *      This function translate edit key. 
 * Parameters:
 * Return: 
 *      the action of key
 */
void GUIAPI TranslateEditMsg(PDLGHEADER pDlgHeader, PMSG pMsg )
{
    switch(pMsg->uMsg)
    {
        case KB_KEYDOWN:
            if(pMsg->wParam == (0x0200|SCAN_DELETE))
            {
                pMsg->uMsg = ED_DELALL;
                break;
            }
            else if(pMsg->wParam >> 8)
                switch(pMsg->wParam & 0x00ff)
                {
                    case 0x4b:
                        pMsg->uMsg = ED_LEFT;
                        break;
                    case 0x4d:
                        pMsg->uMsg = ED_RIGHT;
                        break;
                    case 0x47:
                        pMsg->uMsg = ED_FRONT;
                        break;
                    case 0x4f:
                        pMsg->uMsg = ED_END;
                        break;
                    case 0x52:
                        pMsg->uMsg = ED_INSTOGGLE;
                        break;
                    case 0x53:
                        pMsg->uMsg = ED_DELETE;
                        break;
                    case 0x0f:
                        pMsg->uMsg = LASTTAB;
                        break;
                    default:
                        pMsg->uMsg = ED_NULL;
                        break;
                }
            else if(pMsg->wParam < 32)
            switch(pMsg->wParam)
            {
                case 0x1b:
                    pMsg->uMsg = CANCEL;
                    break;
                case 0x0d:
                    pMsg->uMsg = COMMAND;
                    break;
                case 0x09:
                    pMsg->uMsg = NEXTTAB;
                    break;
                case 0x08:
                    pMsg->uMsg = ED_BACKSPACE;
                    break;
                default:
                    pMsg->uMsg = ED_NULL;
                    break;
            }
            else
                pMsg->uMsg = ED_ASCII;
            break;
        
        case ME_LBUTTONDOWN:
        case ME_LBUTTONUP:
            TranslateBoxMsg(TRUE, pDlgHeader, pMsg);
            if(pMsg->uMsg == INVALID)
                pMsg->uMsg = ED_NULL;
            break;
        
        default:
            pMsg->uMsg = ED_NULL;
            break;
    }
    
    return;
}

/*
 * Function: void GUIAPI TranslateBoxMsg( BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg  )
 *      This function translate box key. 
 * Parameters:
 * Return: 
 *      the action of key
 */
void GUIAPI TranslateBoxMsg( BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg  )
{
    switch(pMsg->uMsg)
    {
        case KB_KEYDOWN:
            if(pMsg->wParam >> 8)
            {
                switch(pMsg->wParam & 0x00ff)
                {
                    case 0x4d:
                    case 0x50:
                        pMsg->uMsg = NEXTGROUP;
                        break;
                    case 0x48:
                    case 0x4b:
                        pMsg->uMsg = LASTGROUP;
                        break;
                    case 0x0f:
                        pMsg->uMsg = LASTTAB;
                        break;
                    default:
                        pMsg->uMsg = NONE;
                        break;
                }
            }
            else
            {               
                switch(pMsg->wParam & 0x00ff)
                {
                    case 0x09:
                        pMsg->uMsg = NEXTTAB;
                    break;
                    case 0x61:
                    case 0x41:
                        pMsg->uMsg = ABORT;
                        break;
                    case 0x52:
                    case 0x72:
                        pMsg->uMsg = RETRY;
                        break;
                    case 0x49:
                    case 0x69:
                        pMsg->uMsg = IGNORE;
                        break;
                    case 0x1b:
                        pMsg->uMsg = CANCEL;
                        break;
                    case 0x0d:
                        pMsg->uMsg = OK;
                        break;
                    case 0x59:
                    case 0x79:
                        pMsg->uMsg = YES;
                        break;
                    case 0x4e:
                    case 0x6e:
                        pMsg->uMsg = NO;
                        break;
                    case 0x20:
                        pMsg->uMsg = CHECK;
                        break;
                    default:
                        pMsg->uMsg = NONE;
                        break;
                }
        }
        break;

    case ME_LBUTTONDOWN:
        if(fAlreadyUp)
        {
            CheckButtonDown(fDlg, pDlgHeader, pMsg);
            if(pMsg->wParam)
            {
                fAlreadyUp = FALSE;
                CtrlIndex = pMsg->wParam;
                pMsg->uMsg = MOUSEFOCUS;
            }
            else
                pMsg->uMsg = INVALID;
        }
        else
            pMsg->uMsg = INVALID;
        break;

    case ME_LBUTTONUP:
        if(CtrlIndex)
        {   
            fAlreadyUp = TRUE;
            pMsg->uMsg = MOUSECHECK;
            CheckButtonUp(fDlg, pDlgHeader, pMsg, CtrlIndex);
            CtrlIndex = 0;
        }
        else
            pMsg->uMsg = INVALID;
        break;

    default:
        pMsg->uMsg = NONE;
        break;
    }
    
    return;
}

static void CheckButtonDown(BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg)
{
    int i;
    
    pMsg->wParam = 0;
    
    if(fDlg)
    {
        for(i=0; i<pDlgHeader->NumOfItems; i++)
        {
            if( (pMsg->pt.x >= (pDlgHeader->pCtrlData + i)->lx) && 
                (pMsg->pt.x <  (pDlgHeader->pCtrlData + i)->rx) &&
                (pMsg->pt.y >= (pDlgHeader->pCtrlData + i)->ty) && 
                (pMsg->pt.y <  (pDlgHeader->pCtrlData + i)->by) )
            {
                if((((pDlgHeader->pCtrlData + i)->lStyle & 0x0000000fL) == BS_GROUPBOX) &&
                    ((pDlgHeader->pCtrlData + i)->bClass == BUTTON))
                    continue;
                pMsg->wParam = i + 1;
                pMsg->pStr = (PSTR)(pDlgHeader->pCtrlData + i);
                break;
            }
        }
    }
    else
    {
        for(i=0; i<MsgBoxData.NumOfCtrl; i++)
        {
            if( (pMsg->pt.x >= (MsgBoxData.pCtrlData + i)->lx) && 
                (pMsg->pt.x <  (MsgBoxData.pCtrlData + i)->rx) &&
                (pMsg->pt.y >= (MsgBoxData.pCtrlData + i)->ty) && 
                (pMsg->pt.y <  (MsgBoxData.pCtrlData + i)->by) )
            {
                pMsg->wParam = i + 1;
                pMsg->pStr = (PSTR)(MsgBoxData.pCtrlData + i);
                break;
            }
        }
    }
    
}

static void CheckButtonUp(BOOL fDlg, PDLGHEADER pDlgHeader, PMSG pMsg, int CtrlIndex)
{
    int i;
    
    pMsg->wParam = 0;
    
    if(fDlg)
    {
        for(i=0; i<pDlgHeader->NumOfItems; i++)
        {
            if( (pMsg->pt.x >= (pDlgHeader->pCtrlData + i)->lx) && 
                (pMsg->pt.x <  (pDlgHeader->pCtrlData + i)->rx) &&
                (pMsg->pt.y >= (pDlgHeader->pCtrlData + i)->ty) && 
                (pMsg->pt.y <  (pDlgHeader->pCtrlData + i)->by) )
            {
                if((((pDlgHeader->pCtrlData + i)->lStyle & 0x0000000fL) == BS_GROUPBOX) &&
                    ((pDlgHeader->pCtrlData + i)->bClass == BUTTON))
                    continue;
                pMsg->wParam = i + 1;
                break;
            }
        }
    }
    else
    {
        for(i=0; i<MsgBoxData.NumOfCtrl; i++)
        {
            if( (pMsg->pt.x >= (MsgBoxData.pCtrlData + i)->lx) && 
                (pMsg->pt.x <  (MsgBoxData.pCtrlData + i)->rx) &&
                (pMsg->pt.y >= (MsgBoxData.pCtrlData + i)->ty) && 
                (pMsg->pt.y <  (MsgBoxData.pCtrlData + i)->by) )
            {
                pMsg->wParam = i + 1;
                break;
            }
        }
    }
    
    if(CtrlIndex != (int)pMsg->wParam)
        pMsg->wParam = 0;
    
}

/*
 * Function: LPSTR GUIAPI LoadMsgIcon( DWORD lStyle )
 *      This function load a icon. 
 * Parameters:
 *      lStyle: the style of icon ;
 * Return: 
 *      success----pointer to image;
 *      failure----NULL.
 */
LPSTR GUIAPI LoadMsgIcon( DWORD lStyle )
{
    LPSTR lpStr;
    
    if ( (lpStr = (LPSTR)_vload( GetIconImageHandler(), _VM_DIRTY )) == NULL )
        return NULL;
    
    switch((lStyle & 0x00f0))
    {
        case MB_ICONASTERISK:
            return lpStr;
        case MB_ICONEXCLAMATION:
            return lpStr + 1328L;
        case MB_ICONQUESTION:
            return lpStr + 2656L;
    }
        
    return NULL;
}

/*
 * Function: void GUIAPI ProcEdit( int Index, PCTRLDATA pCtrlData ).
 *      This function manage a field edit. 
 * Parameters:
 *      pCrlData: the pointer to control;
 * Return: 
 *      the last input.
 */
void GUIAPI ProcEdit(PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData, PMSG pMsg )
{
    STATUSBARDATA SBData[3];
    BOOL fEdit = TRUE;
    BOOL IsIns;
    BOOL fOut = TRUE;
    LPEDITBUFF lpEditBuff;
    char PosBuff[3];
    int i;
    
    for(i=0; i<3; i++)
        SBData[i].fgcolor = 0;
    SBData[0].lpStr = (LPSTR)"插入";
    SBData[1].lpStr = (LPSTR)"替换";

    lpEditBuff = (LPEDITBUFF)pCtrlData->slpAdd;
    
    if(_bios_keybrd(fNewKb ? _NKEYBRD_SHIFTSTATUS : _KEYBRD_SHIFTSTATUS) & 0x0080)
    {
        SetStatusBarInfo(2, &SBData[0]);
        IsIns = TRUE;
    }
    else
    {
        SetStatusBarInfo(2, &SBData[1]);
        IsIns = FALSE;
    }

    SBData[2].lpStr = (LPSTR)PosBuff;
    
    SetCaretPos(pCtrlData->lx, pCtrlData->ty);    
    
    do
    {   
        if(fOut)
        {
            sprintf(PosBuff, "%.3d", lpEditBuff->iEditPos);
            SetStatusBarInfo(3, &SBData[2]);
        
            UndisplayCaret();
            OutFieldEditText(pCtrlData, fEdit);
        }
        
        do
        {
            FlashCaret();
            GetMessage( pMsg );
            
        }while(pMsg->uMsg == NULLINPUT);
        TranslateEditMsg(pDlgHeader, pMsg);

        switch(pMsg->uMsg)
        {
            case ED_NULL:
                fOut = FALSE;
                break;
            case CANCEL:
            case COMMAND:
                fEdit = FALSE;
                UndisplayCaret();
                OutFieldEditText(pCtrlData, fEdit);
            break;
            case ED_BEEP:
                Beep(700, 50);
                fOut = FALSE;
            break;
            case ED_ASCII:
                if(IsIns)
                {
                    if(lpEditBuff->iDataEnd == 256)
                    {
                        Beep(1200, 50);
                        fOut = FALSE;
                        break;
                    }
                    for(i=lpEditBuff->iDataEnd;i>lpEditBuff->iEditPos; i--)
                        (lpEditBuff->pBuff)[i] = (lpEditBuff->pBuff)[i-1];
                    *((lpEditBuff->pBuff) + lpEditBuff->iEditPos) = pMsg->wParam;
                    lpEditBuff->iEditPos++;
                    lpEditBuff->iDataEnd++;
                }
                else
                {
                    (lpEditBuff->pBuff)[lpEditBuff->iEditPos] = pMsg->wParam;
                    lpEditBuff->iEditPos++;
                    if(lpEditBuff->iDataEnd < lpEditBuff->iEditPos)
                        lpEditBuff->iDataEnd++;
                }
                lpEditBuff->CaretOffset++;
                if((lpEditBuff->CaretOffset*8)>(pCtrlData->rx - pCtrlData->lx))
                    lpEditBuff->CaretOffset = (pCtrlData->rx - pCtrlData->lx)/8;
                (lpEditBuff->pBuff)[lpEditBuff->iDataEnd] = '\0';
                fOut = TRUE;
            break;
            case ED_LEFT:
                if(lpEditBuff->iEditPos == 0)
                {
                    Beep(1200,50);
                    fOut = FALSE;
                    break;
                }
                lpEditBuff->iEditPos--;
                lpEditBuff->CaretOffset--;
                if(lpEditBuff->CaretOffset == 0 && lpEditBuff->iEditPos != 0)
                    lpEditBuff->CaretOffset = 1;
                else if(lpEditBuff->CaretOffset < 0)
                    lpEditBuff->CaretOffset = 0;
                fOut = TRUE;
            break;
            case ED_RIGHT:
                if(lpEditBuff->iEditPos == lpEditBuff->iDataEnd)
                {
                    Beep(1200,50);
                    fOut = FALSE;
                    break;
                }
                lpEditBuff->iEditPos++;
                lpEditBuff->CaretOffset++;
                if((lpEditBuff->CaretOffset*8)>(pCtrlData->rx - pCtrlData->lx))
                    lpEditBuff->CaretOffset = (pCtrlData->rx - pCtrlData->lx)/8;
                fOut = TRUE;
            break;
            case ED_FRONT:
                lpEditBuff->iEditPos = 0;
                lpEditBuff->CaretOffset = 0;
                fOut = TRUE;
            break;
            case ED_END:
                lpEditBuff->iEditPos = lpEditBuff->iDataEnd;
                if((lpEditBuff->iDataEnd*8)>(pCtrlData->rx-pCtrlData->lx))
                    lpEditBuff->CaretOffset = (pCtrlData->rx-pCtrlData->lx)/8;
                else
                    lpEditBuff->CaretOffset = lpEditBuff->iEditPos;
                fOut = TRUE;
            break;
            case ED_DELETE:
                if(lpEditBuff->iEditPos == lpEditBuff->iDataEnd|| lpEditBuff->iDataEnd == 0)
                {
                    Beep(1200,50);
                    fOut = FALSE;
                    break;
                }
                for(i=lpEditBuff->iEditPos; i<lpEditBuff->iDataEnd-1; i++)
                    (lpEditBuff->pBuff)[i] = (lpEditBuff->pBuff)[i+1];
                lpEditBuff->iDataEnd--;
                (lpEditBuff->pBuff)[lpEditBuff->iDataEnd] = '\0';
                fOut = TRUE;
            break;
            case ED_DELALL:
                Beep(1200,50);
                lpEditBuff->iEditPos = lpEditBuff->iDataEnd = 0;
                (lpEditBuff->pBuff)[0] = '\0';
                lpEditBuff->CaretOffset = 0;
                fOut = TRUE;
            break;
            case ED_BACKSPACE:
                if(lpEditBuff->iEditPos == 0 || lpEditBuff->iDataEnd == 0)
                {
                    Beep(1200,50);
                    fOut = FALSE;
                    break;
                }
                for(i=lpEditBuff->iEditPos; i<lpEditBuff->iDataEnd; i++)
                    (lpEditBuff->pBuff)[i-1] = (lpEditBuff->pBuff)[i];
                lpEditBuff->iEditPos--;
                lpEditBuff->iDataEnd--;
                (lpEditBuff->pBuff)[lpEditBuff->iDataEnd] = '\0';
                lpEditBuff->CaretOffset--;
                if(lpEditBuff->CaretOffset == 0 && lpEditBuff->iEditPos != 0)
                    lpEditBuff->CaretOffset = 1;
                else if(lpEditBuff->CaretOffset < 0)
                    lpEditBuff->CaretOffset = 0;
                fOut = TRUE;
            break;
            case ED_INSTOGGLE:
                SetStatusBarInfo(2, &SBData[IsIns?1:0]);
                if(IsIns)
                    IsIns = FALSE;
                else
                    IsIns = TRUE;
                fOut = FALSE;
            break;
            
            case NEXTTAB:
            case LASTTAB:
                fEdit = FALSE;
                UndisplayCaret();
                OutFieldEditText(pCtrlData, fEdit);
            break;
            case SETFOCUS:
            case MOUSEFOCUS:
                if(((PCTRLDATA)pMsg->pStr) == pCtrlData)
                {
                    short Offset;
                    
                    Offset = (pMsg->pt.x - pCtrlData->lx)/8;
                    if(Offset<0)Offset = 0;
                    
                    if((lpEditBuff->iEditPos + Offset - lpEditBuff->CaretOffset) > lpEditBuff->iDataEnd)
                    {
                        fOut = TRUE;
                        break;
                    }
    
                    lpEditBuff->iEditPos += (Offset - lpEditBuff->CaretOffset);
                    lpEditBuff->CaretOffset = Offset;
                    fOut = TRUE;
                }
                else
                {
                    fEdit = FALSE;
                    UndisplayCaret();
                    OutFieldEditText(pCtrlData, fEdit);
                }
            break;          
            default:
                    fOut = FALSE;
            break;
        }
    
    }while(fEdit);
    
    SetStatusBarInfo(2, NULL);
    SetStatusBarInfo(3, NULL);
}

/*
 * Function: void GUIAPI OutFieldEditText( PCTRLDATA pCtrlData ).
 *      This function out text to given field edit. 
 * Parameters:
 *      pCtrlData: the pointer to control;
 *      IsDispCur: flages display or not sisplay cursor;
 * Return: 
 *      None
 */
void GUIAPI OutFieldEditText(PCTRLDATA pCtrlData, BOOL fEdit)
{
    LPEDITBUFF lpEditBuff;
    int LimitOfChar;
    short StartOut;
    
    lpEditBuff = (LPEDITBUFF)pCtrlData->slpAdd;
    
    LimitOfChar = (pCtrlData->rx - pCtrlData->lx)/8;
    
    if(fEdit)
        StartOut = pCtrlData->lx + (lpEditBuff->CaretOffset - lpEditBuff->iEditPos) * 8;
    else
    StartOut = pCtrlData->lx;
        
    SetPtrVis(HIDE);
    _setcolor(15);
    _rectangle(_GFILLINTERIOR, pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);
    
    set_color(0);
    set_cliprgn(pCtrlData->lx, pCtrlData->ty, pCtrlData->rx, pCtrlData->by);
    cc_wt16(lpEditBuff->pBuff, StartOut+1, pCtrlData->ty);
    set_cliprgn(0, 0, vc.numxpixels, vc.numypixels);
    
    SetPtrVis(SHOW);
    
    if(fEdit)
        SetCaretPos(pCtrlData->lx + lpEditBuff->CaretOffset * 8, pCtrlData->ty);
}

/*
 * Function: void GUIAPI void GUIAPI SetEditBoxText(  PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr );
 *      This function set string to a field edit. 
 * Parameters:
 *      wID: control's ID;
 *      pStr:the pointer to string.
 * Return: 
 *      None.
 */
void GUIAPI SetEditBoxText(  PDLGHEADER pDlgHeader, WORD wID, LPCSTR lpStr )
{
    PCTRLDATA pCtrlData;
    LPEDITBUFF lpEditBuff;
    
    pCtrlData = GetControlData(pDlgHeader, wID);
    lpEditBuff = (LPEDITBUFF)pCtrlData->slpAdd;
                                              
    _fstrncpy(lpEditBuff->pBuff, lpStr, 255);
    lpEditBuff->iEditPos = 0;
    lpEditBuff->iDataEnd = _fstrlen(lpEditBuff->pBuff);
    lpEditBuff->CaretOffset = 0;
    
}

/*
 * Function: void GUIAPI GetEditBoxText( PCTRLDATA pCtrlData, LPSTR lpStr, int iLength ).
 *      This function set string to a field edit. 
 * Parameters:
 *      wID: control's ID;
 *      lpStr:the pointer to string;
 *      iLength: the length of string want to get.
 * Return: 
 *      None.
 */
void GUIAPI GetEditBoxText( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr, int iLength )
{   
    PCTRLDATA pCtrlData;
    pCtrlData = GetControlData(pDlgHeader, wID);
    
    if(!pCtrlData->slpAdd)
        return;
    _fstrncpy(lpStr, ((LPEDITBUFF)(pCtrlData->slpAdd))->pBuff, iLength);
}

/*
 * Function: TranslateListBoxMsg( int Index, PMSG pMsg )
 *      This function translate list box key. 
 * Parameters:
 * Return: 
 *      the action of key
 */
void GUIAPI TranslateListBoxMsg( PDLGHEADER pDlgHeader, PMSG pMsg )
{
    switch(pMsg->uMsg)
    {
        case KB_KEYDOWN:
            if(pMsg->wParam >> 8)
                switch(pMsg->wParam & 0x00ff)
                {
                    case 0x49:
                        pMsg->uMsg = LB_PAGEUP;
                        break;
                    case 0x51:
                        pMsg->uMsg = LB_PAGEDOWN;
                        break;
                    case 0x50:
                    case 0x4d:
                        pMsg->uMsg = LB_DOWN;
                        break;
                    case 0x48:
                    case 0x4b:
                        pMsg->uMsg = LB_UP;
                        break;
                    case 0x47:
                        pMsg->uMsg = LB_FRONT;
                        break;
                    case 0x4f:
                        pMsg->uMsg = LB_END;
                        break;
                    case 0x0f:
                        pMsg->uMsg = LASTTAB;
                        break;
                    default:
                        pMsg->uMsg = LB_NULL;
                        break;
                }
            else if(pMsg->wParam < 32)
            switch(pMsg->wParam)
            {
                case 0x1b:
                    pMsg->uMsg = CANCEL;
                    break;
                case 0x0d:
                    pMsg->uMsg = COMMAND;
                    break;
                case 0x09:
                    pMsg->uMsg = NEXTTAB;
                    break;
                default:
                    pMsg->uMsg = LB_NULL;
                    break;
            }
        else
            pMsg->uMsg = LB_ASCII;
            break;

        case ME_LBUTTONDOWN:
        case ME_LBUTTONUP:
            TranslateBoxMsg(TRUE, pDlgHeader, pMsg);
            if(pMsg->uMsg == INVALID)
                pMsg->uMsg = LB_NULL;
            break;

        default:
            pMsg->uMsg = LB_NULL;
            break;
    }
    
    return;
}

/*
 * Function: void GUIAPI ProcListBox( int Index, PCTRLDATA pCtrlData, PMSG pMsg )
 *      This function manage a List Box. 
 * Parameters:
 *      pCrlData: the pointer to control;
 * Return: 
 *      the last input.
 */
void GUIAPI ProcListBox(PDLGHEADER pDlgHeader, PCTRLDATA pCtrlData, PMSG pMsg )
{
    BOOL fRedraw = FALSE;
    BOOL fReturn = FALSE;
    LPLISTBOXBUFF lpListBoxBuff;
    int iMaxRows, iLimitRows;
    int iCount;
    
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;

    iCount = lpListBoxBuff->iCount;
    iMaxRows = (pCtrlData->by - pCtrlData->ty) / 16;
    (iCount < iMaxRows)?(iLimitRows = iCount):(iLimitRows = iMaxRows);
    
    do
    {
        do
        {
            GetMessage( pMsg );
        }while(pMsg->uMsg == NULLINPUT);

        TranslateListBoxMsg( pDlgHeader, pMsg );
        
        switch(pMsg->uMsg)
        {
            case LB_BEEP:
                Beep(700, 50);
            case LB_NULL:
                break;
    
            case CANCEL:
            case FOCUSCOMMAND:
            case COMMAND:
                fReturn = TRUE;
            break;
    
            case SETFOCUS:
            case MOUSEFOCUS:
                if(iCount == 0)
                {
                    fReturn = TRUE;
                    break;
                }
                if(((PCTRLDATA)pMsg->pStr) == pCtrlData)
                {
                    int iMouseOffset = (pMsg->pt.y - pCtrlData->ty)/16;
                    SetFocus(NULL);
                    if(lpListBoxBuff->iCurSel == -1)
                        lpListBoxBuff->iCurSel = 0;
                    else
                    {
                        SetVScrollPosition(pCtrlData, TRUE);
                        DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset,
                            lpListBoxBuff->iCurSel, FALSE);
                    }

                    if(lpListBoxBuff->iCurSel - lpListBoxBuff->iWndOffset + iMouseOffset >= iCount)
                        iMouseOffset = lpListBoxBuff->iWndOffset;
                        
                    lpListBoxBuff->iCurSel = lpListBoxBuff->iCurSel - lpListBoxBuff->iWndOffset + iMouseOffset;
                    lpListBoxBuff->iWndOffset = iMouseOffset;
                    SetVScrollPosition(pCtrlData, FALSE);
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                        lpListBoxBuff->iCurSel, TRUE);
                    pMsg->uMsg = FOCUSCOMMAND;
                    fReturn = TRUE;
                }
                else
                    fReturn = TRUE;
            break;          

            case LB_UP:
                if(iCount == 0)
                    break;
                if(lpListBoxBuff->iCurSel == 0)
                    break;
                SetFocus(NULL);
                if(lpListBoxBuff->iCurSel == -1)
                {
                    lpListBoxBuff->iCurSel = 0;
                }
                else if(lpListBoxBuff->iWndOffset == 0)
                {
                    SetVScrollPosition(pCtrlData, TRUE);
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                        lpListBoxBuff->iCurSel, FALSE);
                    lpListBoxBuff->iCurSel--;
                    ScrollListBox(pCtrlData, LB_UP);
                }
                else
                {
                    SetVScrollPosition(pCtrlData, TRUE);
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                        lpListBoxBuff->iCurSel, FALSE);
                    lpListBoxBuff->iCurSel--;
                    lpListBoxBuff->iWndOffset--;
                }
                SetVScrollPosition(pCtrlData, FALSE);
                DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                    lpListBoxBuff->iCurSel, TRUE);
                pMsg->uMsg = FOCUSCOMMAND;
                fReturn = TRUE;
            break;

            case LB_DOWN:
                if(iCount == 0)
                    break;
                if(lpListBoxBuff->iCurSel == (iCount - 1))
                    break;
                SetFocus(NULL);
                if(lpListBoxBuff->iCurSel == -1)
                {
                    lpListBoxBuff->iCurSel = 0;
                    
                }
                else if(lpListBoxBuff->iWndOffset == (iMaxRows - 1))
                {
                    SetVScrollPosition(pCtrlData, TRUE);
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset,
                        lpListBoxBuff->iCurSel, FALSE);
                    lpListBoxBuff->iCurSel++;
                    ScrollListBox(pCtrlData, LB_DOWN);
                }
                else
                {
                    SetVScrollPosition(pCtrlData, TRUE);
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                        lpListBoxBuff->iCurSel, FALSE);
                    lpListBoxBuff->iCurSel++;
                    lpListBoxBuff->iWndOffset++;
                }
                SetVScrollPosition(pCtrlData, FALSE);
                DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                    lpListBoxBuff->iCurSel, TRUE);
                pMsg->uMsg = FOCUSCOMMAND;
                fReturn = TRUE;
            break;

            case LB_ASCII:
            break;
    
            case LB_PAGEDOWN:
                if(iCount == 0)
                    break;
                if((lpListBoxBuff->iWndOffset + iCount - lpListBoxBuff->iCurSel) <= iLimitRows)
                    break;
                SetFocus(NULL);
                SetVScrollPosition(pCtrlData, TRUE);
                if(lpListBoxBuff->iCurSel == -1)
                    lpListBoxBuff->iCurSel = 0;
                lpListBoxBuff->iCurSel += iLimitRows;
                if(lpListBoxBuff->iCurSel > (iCount - 1))
                    lpListBoxBuff->iCurSel = iCount - 1;
                FillListBox(pCtrlData);
                SetVScrollPosition(pCtrlData, FALSE);
                DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                    lpListBoxBuff->iCurSel, TRUE);
                pMsg->uMsg = FOCUSCOMMAND;
                fReturn = TRUE;
            break;
            
            case LB_PAGEUP:
                if(iCount == 0)
                    break;
                if(lpListBoxBuff->iCurSel == lpListBoxBuff->iWndOffset)
                    break;
                SetFocus(NULL);
                SetVScrollPosition(pCtrlData, TRUE);
                if(lpListBoxBuff->iCurSel == -1)
                    lpListBoxBuff->iCurSel = 0;
                if((lpListBoxBuff->iCurSel - lpListBoxBuff->iWndOffset - iMaxRows) > 0)
                    lpListBoxBuff->iCurSel = lpListBoxBuff->iCurSel - iMaxRows;
                else
                    lpListBoxBuff->iCurSel = lpListBoxBuff->iWndOffset;
                FillListBox(pCtrlData);
                SetVScrollPosition(pCtrlData, FALSE);
                DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                    lpListBoxBuff->iCurSel, TRUE);
                pMsg->uMsg = FOCUSCOMMAND;
                fReturn = TRUE;
            break;
            
            case LB_FRONT:
                if(iCount == 0)
                    break;
                SetFocus(NULL);
                SetVScrollPosition(pCtrlData, TRUE);
                if(lpListBoxBuff->iCurSel == -1)
                    lpListBoxBuff->iCurSel = 0;
                if(lpListBoxBuff->iCurSel != lpListBoxBuff->iWndOffset)
                {
                    lpListBoxBuff->iCurSel = 0;
                    lpListBoxBuff->iWndOffset = 0;
                    FillListBox(pCtrlData);
                }
                else
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                        lpListBoxBuff->iCurSel, FALSE);
                lpListBoxBuff->iCurSel = 0;
                lpListBoxBuff->iWndOffset = 0;
                SetVScrollPosition(pCtrlData, FALSE);
                DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                    lpListBoxBuff->iCurSel, TRUE);
                pMsg->uMsg = FOCUSCOMMAND;
                fReturn = TRUE;
            break;
            case LB_END:
                if(iCount == 0)
                    break;
                SetFocus(NULL);
                SetVScrollPosition(pCtrlData, TRUE);
                if(lpListBoxBuff->iCurSel == -1)
                    lpListBoxBuff->iCurSel = 0;
                if(iCount >= (lpListBoxBuff->iCurSel + iLimitRows - lpListBoxBuff->iWndOffset))
                {
                    lpListBoxBuff->iCurSel = iCount - 1;
                    lpListBoxBuff->iWndOffset = iLimitRows - 1;
                    FillListBox(pCtrlData);
                }
                else
                {
                    DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                        lpListBoxBuff->iCurSel, FALSE);
                    lpListBoxBuff->iCurSel = iCount - 1;
                }
                SetVScrollPosition(pCtrlData, FALSE);
                DrawListBoxItem(pCtrlData, lpListBoxBuff->iWndOffset, 
                    lpListBoxBuff->iCurSel, TRUE);
                pMsg->uMsg = FOCUSCOMMAND;
                fReturn = TRUE;
            break;

            case NEXTTAB:
            case LASTTAB:
                fReturn = TRUE;
            break;

            default:
            break;
        }
    
    }while(!fReturn);
}

/*
 * Function: BOOL GUIAPI AddListBoxItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr )
 *      This function add item to the specified list box.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 *      lpStr: the item that want to add;
 * Return: 
 *      TRUE: add item successfully;
 *      FALSE: memory allocation error occured.
 */
BOOL GUIAPI AddListBoxItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr )
{
    PCTRLDATA pCtrlData;
    LPLISTBOXBUFF lpListBoxBuff;
    LPNODE pHead;

    pCtrlData = GetControlData(pDlgHeader, wID);
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    pHead = lpListBoxBuff->pHead;
    
    if(pHead)
    {
        if(!AddNode(pHead, lpStr))
            return FALSE;
    }
    else
    {
        lpListBoxBuff->pHead = CreateNewList(lpStr);
        if(lpListBoxBuff->pHead == NULL)
            return FALSE;
    }
    
    lpListBoxBuff->iCount++;
    
    return TRUE;
}

/*
 * Function: void GUIAPI DeleteListBoxItem( PDLGHEADER pDlgHeader, WORD wID, int iIndex )
 *      This function delete item in the specified list box.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 *      iIndex: the item's index that want delete;
 * Return: 
 *      None;
 */
void GUIAPI DeleteListBoxItem( PDLGHEADER pDlgHeader, WORD wID, int iIndex )
{
    PCTRLDATA pCtrlData;
    LPLISTBOXBUFF lpListBoxBuff;
    LPNODE pHead;

    pCtrlData = GetControlData(pDlgHeader, wID);
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    pHead = lpListBoxBuff->pHead;
    
    if(pHead)
        lpListBoxBuff->pHead = DeleteNode(pHead, iIndex);

    lpListBoxBuff->iCount--;
}

/*
 * Function: int GUIAPI GetSelectedItem( PDLGHEADER pDlgHeader, WORD wID )
 *      This function return the current selected item in the list box;
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 * Return: 
 *      -1: there is no selected item;
 *      >=0: the index of selected item.
 */
int GUIAPI GetSelectedItem( PDLGHEADER pDlgHeader, WORD wID )
{
    PCTRLDATA pCtrlData;
    LPLISTBOXBUFF lpListBoxBuff;

    pCtrlData = GetControlData(pDlgHeader, wID);
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    
    return lpListBoxBuff->iCurSel;
}

/*
 * Function: void GUIAPI GetListBoxItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr, int iIndex )
 *      This function give the content of specified item in list box;
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 *      lpStr: the content of specified item will be in this buffer;
 *      iIndex: the index of specified item.
 * Return: 
 *      None.
 */
void GUIAPI GetListBoxItem( PDLGHEADER pDlgHeader, WORD wID, LPSTR lpStr, int iIndex )
{
    PCTRLDATA pCtrlData;
    LPLISTBOXBUFF lpListBoxBuff;
    LPNODE pHead;

    pCtrlData = GetControlData(pDlgHeader, wID);
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    pHead = lpListBoxBuff->pHead;
    
    if(pHead)
    {
        LPCSTR pBuff;
        
        if((pBuff = GetNodeValue(pHead, iIndex)) != NULL)
            _fstrcpy(lpStr, pBuff);
    }
}

/*
 * Function: int GUIAPI GetListItemNum(  PDLGHEADER pDlgHeader, WORD wID )
 *      This function give the item count of specified list box.
 * Parameters:
 *      pDlgHeader: the pointer to DLGHEADER structure;
 *      wID: given control's ID;
 * Return: 
 *      The number.
 */
int GUIAPI GetListItemNum(  PDLGHEADER pDlgHeader, WORD wID )
{
    PCTRLDATA pCtrlData;
    LPLISTBOXBUFF lpListBoxBuff;

    pCtrlData = GetControlData(pDlgHeader, wID);
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    
    return lpListBoxBuff->iCount;
}

/*
 * Function: void GUIAPI FillListBox( PCTRLDATA pCtrlData )
 *      This function fill list box.
 * Parameters:
 *      pCtrlData: the pointer to control data of list box.
 * Return: 
 *      None;
 */
void GUIAPI FillListBox( PCTRLDATA pCtrlData )
{
    int i, j;
    int iStartRow, iEndRow;
    LPLISTBOXBUFF lpListBoxBuff;
    
    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    if(lpListBoxBuff->iCount == 0)
    {
        _setcolor(15);
        _rectangle(_GFILLINTERIOR,
                pCtrlData->lx,
                pCtrlData->ty,
                (pCtrlData->lStyle & WS_VSCROLL)?(pCtrlData->rx - 16):pCtrlData->rx,
                pCtrlData->by);
        return;
    }

    iStartRow = lpListBoxBuff->iCurSel - lpListBoxBuff->iWndOffset;
    if(iStartRow < 0)
        iStartRow = 0;
    iEndRow = (pCtrlData->by - pCtrlData->ty) / 16 + iStartRow;
    
    if(pCtrlData->lStyle & WS_VSCROLL)
    {
        int lx, ty, rx, by;
        
        lx = pCtrlData->rx - 14;
        ty = pCtrlData->ty + 2;
        rx = pCtrlData->rx - 1;
        by = pCtrlData->by - 2;
        _setcolor(7);
        _rectangle(_GFILLINTERIOR, lx, ty, rx, by);
    }
    
    for(i=iStartRow, j=0; i<iEndRow; i++, j++)
        DrawListBoxItem(pCtrlData, j, i, FALSE);
}

/*
 * Function: void GUIAPI ReleaseListBoxBuff( PCTRLDATA pCtrlData )
 *      This function release the buffer that save the contents of 
 *      list box's items,
 * Parameters:
 *      pCtrlData: the pointer to control data of list box.
 * Return: 
 *      None;
 */
void GUIAPI ReleaseListBoxBuff( PCTRLDATA pCtrlData )
{
    LPLISTBOXBUFF lpListBoxBuff;
    LPNODE pHead;

    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    pHead = lpListBoxBuff->pHead;
    
    DeleteList(pHead);
    lpListBoxBuff->iCurSel = -1;
    lpListBoxBuff->iWndOffset = 0;
    lpListBoxBuff->iCount = 0;
    lpListBoxBuff->pHead = NULL;
}

// The following static functions manage the drawing of list box;
#define LS_BKCOLOR      1
#define LB_BKCOLOR      15
#define LS_FGCOLOR      4
#define LB_FGCOLOR      0

static void DrawListBoxItem( PCTRLDATA pCtrlData, int iOffset, int iIndex, BOOL fSel )
{
    int lx, ty, rx, by;
    LPLISTBOXBUFF lpListBoxBuff;

    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    
    lx = pCtrlData->lx;
    ty = pCtrlData->ty + iOffset*16;
    rx = (pCtrlData->lStyle & WS_VSCROLL)?(pCtrlData->rx - 16):pCtrlData->rx;
    by = ty + 16;
    
    SetPtrVis(HIDE);

    if(fSel)
        _setcolor(LS_BKCOLOR);
    else
        _setcolor(LB_BKCOLOR);
    _rectangle(_GFILLINTERIOR, lx, ty, rx, by);

    if(lpListBoxBuff->iCount <= iIndex)
    {
        SetPtrVis(SHOW);
        return;
    }
        
    set_cliprgn(lx, ty, rx, by);
    if(fSel)
        set_color(LS_FGCOLOR);
    else
        set_color(LB_FGCOLOR);
    cc_wt16(GetNodeValue(lpListBoxBuff->pHead, iIndex), lx, ty);
    
    SetPtrVis(SHOW);
    set_cliprgn(0, 0, vc.numxpixels - 1, vc.numypixels - 1);
}

static void ScrollListBox(PCTRLDATA pCtrlData, WORD uAction)
{
    int i;
    int sx, sy;
    int sw;
    int dx, dy;
    int iRows = (pCtrlData->by - pCtrlData->ty) / 16 - 1;
     
    sx = dx = pCtrlData->lx;
    sw = (pCtrlData->rx - pCtrlData->lx);
    if(pCtrlData->lStyle & WS_VSCROLL)
        sw -= 16;
    
    SetPtrVis(HIDE);
    switch(uAction)
    {
        case LB_UP:
            for(i=iRows-1; i>=0; i--)
            {
                sy = pCtrlData->ty + i*16;
                dy = pCtrlData->ty + (i+1)*16;
                move_image(sx, sy, sw, 16, dx, dy);
            }
            break;
            
        case LB_DOWN:
            for(i=0; i<iRows; i++)
            {
                sy = pCtrlData->ty + (i+1)*16;
                dy = pCtrlData->ty + i*16;
                move_image(sx, sy, sw, 16, dx, dy);
            }
            break;
        default:
            return;
    }
    SetPtrVis(SHOW);
}

static void SetVScrollPosition(PCTRLDATA pCtrlData, BOOL fErase)
{
    int lx, rx, y;
    LPLISTBOXBUFF lpListBoxBuff;
    int iCurSel;
    int iCount;

    lpListBoxBuff = (LPLISTBOXBUFF)pCtrlData->slpAdd;
    
    iCurSel = lpListBoxBuff->iCurSel;
    iCount = (lpListBoxBuff->iCount == 0) ? 1 : lpListBoxBuff->iCount;
    lx = pCtrlData->rx - 14;
    rx = pCtrlData->rx;
    y = pCtrlData->ty + 12 + (int)((pCtrlData->by - pCtrlData->ty - 20.0)*iCurSel/iCount);

    if(iCount == 0)
        return;
    
    SetPtrVis(HIDE);
    if(fErase)
    {
        _setcolor(7);
        _moveto(lx, y + 8);
        _lineto(rx - 1, y + 8);
        
        _moveto(lx + 1, y + 7);
        _lineto(lx + 1, y - 7);
        _lineto(rx - 1, y - 7);
        _moveto(lx + 1, y + 7);
        _lineto(rx - 1, y + 7);
        _lineto(rx - 1, y - 7);
    }
    else
    {
        _setcolor(7);
        _moveto(lx, y + 8);
        _lineto(lx, y - 8);
        _lineto(rx, y - 8);
        _setcolor(0);
        _moveto(lx, y + 8);
        _lineto(rx, y + 8);
        _lineto(rx, y - 8);
        
        _setcolor(15);
        _moveto(lx + 1, y + 7);
        _lineto(lx + 1, y - 7);
        _lineto(rx - 1, y - 7);
        _setcolor(14);
        _moveto(lx + 1, y + 7);
        _lineto(rx - 1, y + 7);
        _lineto(rx - 1, y - 7);
    }
    
//    fErase?_setcolor(7):_setcolor(0);
//    _moveto(lx, y);
//    _lineto(rx, y);
//    _moveto(lx, y);
//    _lineto(lx + 2, y - 2);
    SetPtrVis(SHOW);
}

// Following functions manage the linked list. 
//      Copyright (C) by Mr.Wei Yongming.
//      1995.8.13.am.
//
// Modified by Mr. Wei Yongming, 1997.7.1.am.

LPNODE GUIAPI CreateNewList(LPSTR lpStr)
{
    LPNODE pHead;
    
    pHead = _fmalloc(sizeof(NODE));
    if(pHead == NULL)
        return NULL;
        
    pHead->pStr = _fmalloc(_fstrlen(lpStr) + 1);
    if(pHead->pStr == NULL)
    {
        _ffree(pHead);
        return NULL;
    }
    
    _fstrcpy(pHead->pStr, lpStr);
    pHead->pNext = NULL;
    
    return pHead;
}

BOOL GUIAPI AddNode(LPNODE pHead, LPSTR lpStr)
{
    LPNODE pNode = pHead;
    LPNODE pTail;

    pTail = pNode = GetTrail(pHead);
    pNode->pNext = _fmalloc(sizeof(NODE));

    pNode = pNode->pNext;
    if(pNode == NULL)
        return FALSE;
        
    pNode->pStr = _fmalloc(_fstrlen(lpStr) + 1);
    if(pNode->pStr == NULL)
    {
        _ffree(pNode);
        pTail->pNext = NULL;
        return FALSE;
    }
    
    _fstrcpy(pNode->pStr, lpStr);
    pNode->pNext = NULL;
    return TRUE;
}

void GUIAPI DeleteList(LPNODE pHead)
{
    LPNODE pNode = pHead;
    LPNODE pNodeNext;

    while(pNode)
    {
        pNodeNext = pNode->pNext;
        
        _ffree(pNode->pStr);
        _ffree(pNode);
        
        pNode = pNodeNext;
    }
}

LPNODE GUIAPI DeleteNode(LPNODE pHead, int iIndex)
{
    int i=0;
    LPNODE pNode1 = pHead;
    LPNODE pNode2;

    while(pNode1)
    {   
        if(i == iIndex)
            break;
        pNode2 = pNode1;
        pNode1 = pNode1->pNext;
        
        i++;
    }

    if(pNode1 == NULL)             // not found node or it is a null list
        return pHead;
    
    if(pNode1 == pHead)
        pHead = pNode1->pNext;
    else
        pNode2->pNext = pNode1->pNext;
        
    _ffree(pNode1->pStr);
    _ffree(pNode1);
    
    return pHead;
}

LPNODE GUIAPI GetTrail(LPNODE pHead)
{
    LPNODE pNode = pHead;
    
    while(pNode->pNext)
    {
        pNode = pNode->pNext;
    }
    
    return pNode;
}

int GUIAPI GetListLength(LPNODE pHead)
{
    int nCount = 0;
    LPNODE pNode = pHead;
    
    if(pHead)
        nCount++;
    else
        return 0;
    
    while(pNode->pNext)
    {
        pNode = pNode->pNext;
        nCount++;
    }
    
    return nCount;
}

LPCSTR GUIAPI GetNodeValue(LPNODE pHead, int iIndex)
{
    int i = 0;
    LPNODE pNode = pHead;

    if(iIndex >= GetListLength(pHead))
        return NULL;
    
    while(TRUE)
    {
        if(iIndex == i)
            break;
            
        pNode = pNode->pNext;
        i++;
    }
    
    return pNode->pStr;
}

