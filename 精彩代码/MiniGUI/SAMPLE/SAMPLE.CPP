#include <afxcoll.h>
#include <stdio.h>
#include <conio.h> 
#include <io.h>
#include <dos.h>
#include <bios.h>
#include <time.h>
#include <stddef.h>
#include <stdlib.h>
#include <malloc.h>
#include <graph.h>
#include <memory.h>
#include <vmemory.h>
#include <direct.h>
#include <string.h>
#include <math.h>
#include <sys\stat.h>

#include "common.h"
#include "support.h"
#include "mainGUI.h"
#include "dialog.h"

#include "sample.h"
#include "resource.h"
#include "sample.rc"

#define MIN_VIRTMEM_INKB    64
#define MAX_VIRTMEM_INKB    128

extern POPUPMENUITEM PopupMenuItem[];
extern ACCELTAB AccelTab[];
extern NORMALMENUITEM NormalMenuItem[];

const char szMainCaption[]="WYMGUI演示程序";
const char szMsgCaption[]="WYMGUI演示程序";

int iWorkDrive;
char szWorkDir[_MAX_PATH + 1];


/*
 * Function: void GUIAPI InitRes( void );
 *      This Function Initialize resources, such as
 *   dialog, message box, status bar.
 *      For diffrent application the function must diffrent.
 * Parameters:
 *      None.
 * Return: 
 *      None.
 */
BOOL GUIAPI InitRes( void )
{
    int i;
    
    // Initialize Menu
    PopupMenuItem[0].pNormalMenuItem = &(NormalMenuItem[0]);
    PopupMenuItem[1].pNormalMenuItem = &(NormalMenuItem[7]);
    PopupMenuItem[2].pNormalMenuItem = &(NormalMenuItem[14]);
    PopupMenuItem[3].pNormalMenuItem = &(NormalMenuItem[16]);
    PopupMenuItem[4].pNormalMenuItem = &(NormalMenuItem[19]);
    
    // Initialize dialog
    ErrorDlgHeader.pCtrlData = CtrlData;

	ErrorDlgHeader.rx += ErrorDlgHeader.lx;
	ErrorDlgHeader.by += ErrorDlgHeader.ty;
	
	for(i=0; i<ErrorDlgHeader.NumOfItems; i++)
	{
	    (ErrorDlgHeader.pCtrlData + i)->lx = (ErrorDlgHeader.pCtrlData + i)->lx
			+ ErrorDlgHeader.lx;
	    (ErrorDlgHeader.pCtrlData + i)->rx = (ErrorDlgHeader.pCtrlData + i)->rx
			+ (ErrorDlgHeader.pCtrlData + i)->lx;
	    (ErrorDlgHeader.pCtrlData + i)->ty = (ErrorDlgHeader.pCtrlData + i)->ty
			+ ErrorDlgHeader.ty + 25;
	    (ErrorDlgHeader.pCtrlData + i)->by = (ErrorDlgHeader.pCtrlData + i)->by
			+ (ErrorDlgHeader.pCtrlData + i)->ty;
	}
    
    return TRUE;
}

void FAR TerminateSample( void )
{
    TerminateGUI();
    _vheapterm();
    exit(1);
}

BOOL FAR ReinitGUI(PGUIINFO pGUIInfo)
{
    InitVirtualMemory(MIN_VIRTMEM_INKB, MAX_VIRTMEM_INKB);

    _chdrive(iWorkDrive);
    _chdir(szWorkDir);

    if(!InitGUI())
    {
    	if(pGUIInfo)
    	{
			PostMessage(pGUIInfo, MSG_DESTROY, TRUE, 0L);
			return FALSE;
		}
    }
    
    if(pGUIInfo)
    {
	    PostMessage(pGUIInfo, MSG_NCPAINT, 0, 0L);
	    PostMessage(pGUIInfo, MSG_ERASEBKGND, (WPARAM)(&pGUIInfo->dc.clientrect), 0L);
	}
	
	return TRUE;
}

void main( void )
{
    // See if is running
    {
	char* GUIvar;
	GUIvar = getenv("WYMGUI");
	if(strcmp("RUNNING", GUIvar) == 0)
	{
	    _settextposition(24, 1);
	    _settextcolor( 14 );
	    _outtext("WYMGUI is running. Please type ");
	    _settextcolor( 30 );
	    _outtext("EXIT");
	    _settextcolor( 14 );
	    _outtext(" to return.");
	    exit(1);
	}
    }
    _putenv("WYMGUI=RUNNING");
    
    if(!InitVirtualMemory(MIN_VIRTMEM_INKB, MAX_VIRTMEM_INKB))
    {
		exit(1);
    }

    // Initialize GUI.
    if(!InitGUI())
		TerminateSample();
    
    // Get work drive and directory.
    iWorkDrive = _getdrive();
    _getcwd(szWorkDir, _MAX_PATH);

    // Initialize resources.
    if(!InitRes())
    {
		printf( "Resource files were damaged! Can not run this file. \n" );
		TerminateSample();
    }
    
    SampleMain();
}

BOOL FAR SampleMain( void )
{ 
    PGUIINFO pGUIInfo;
    MSG msg;
    
    // Create the main graphics user interface,
    // that include drawing menu, client area and creating status bar etc.
    pGUIInfo = CreateMainGUI(szMainCaption,
                    PopupMenuItem,
                    AccelTab,
                    SampleMainGUIProc);
    if(pGUIInfo == NULL)
    {
        MessageBox(NULL, "出现致命错误，不能运行此程序", szMsgCaption, MB_OK | MB_ICONEXCLAMATION);
        TerminateSample();
    }
    
    do
    {
        if(GetMessageFromQueue(pGUIInfo, &msg))
        {
            if(msg.uMsg == MSG_QUIT)
                break;
            TranslateAccelerator(pGUIInfo, &msg);
            DisptchMessage(pGUIInfo, &msg);
        }
        else
        {
            GetMessage(&msg);
            PreProcMessage(pGUIInfo, &msg, FALSE);
        }
    }while(TRUE);
    
    TerminateSample();
    
    return FALSE;
    
}

int FAR ErrorBoxProc( PDLGHEADER pDlgHeader, UINT uAction, WORD wID, LONG lParam )
{
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

        default:
        return DEFAULT;
    }
    return DEFAULT;
}

/*
 * Function: int FAR SampleMainGUIProc( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam)
 *      The main procedure that process GUI messages.
 * Parameters:
 * Return: 
 */
int FAR SampleMainGUIProc( PGUIINFO pGUIInfo, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    char szFileName[_MAX_PATH + 1];
    OPENFILEBOX OpenFileBox;
    char szText[50];
    
    switch(uMsg)
    {
        case MSG_COMMAND:
            switch (wParam)
            {
                case IDM_EXIT:
                    if(MessageBox(pGUIInfo, "是否确定要退出？", szMsgCaption, MB_YESNO | MB_ICONQUESTION) == IDYES)
                        PostMessage(pGUIInfo, MSG_DESTROY, 0, 0L);
                    break;
                case IDM_SHELL:
                    DosShell();
                    ReinitGUI(pGUIInfo);
                    break;
                    
                case IDM_ABOUT:
                    DialogBoxParam( pGUIInfo, GetSysDlgHeader(IDD_SYSABOUTDLG), SysAboutBoxProc, NULL);
                    break;
                
                case IDM_OPENDATA:
                    OpenFileBox.fOpen = TRUE;
                    OpenFileBox.iInitDrive = iWorkDrive;
                    OpenFileBox.lpInitDir = szWorkDir;
                    OpenFileBox.lpExt = "*.dat";
                    OpenFileBox.lpFileName = (LPSTR)szFileName;
                    GetSysDlgHeader(IDD_OPENFILEDLG)->spCaption = "文件打开";
                    DialogBoxParam( pGUIInfo, GetSysDlgHeader(IDD_OPENFILEDLG), OpenBoxProc, (LPSTR)(&OpenFileBox));
                    // set directory to the default directory.
                    _chdrive(iWorkDrive);
                    _chdir(szWorkDir);

                    if(szFileName[0] == '\0')
                        break;
                    break;
                
                case IDM_SAVEAS:
                    OpenFileBox.fOpen = FALSE;
                    OpenFileBox.iInitDrive = iWorkDrive;
                    OpenFileBox.lpInitDir = szWorkDir;
                    OpenFileBox.lpExt = "*.dat";
                    OpenFileBox.lpFileName = (LPSTR)szFileName;
                    GetSysDlgHeader(IDD_OPENFILEDLG)->spCaption = "文件另存为";
                    DialogBoxParam( pGUIInfo, GetSysDlgHeader(IDD_OPENFILEDLG), OpenBoxProc, (LPSTR)(&OpenFileBox));
                    // set directory to the default directory.
                    _chdrive(iWorkDrive);
                    _chdir(szWorkDir);

                    if(szFileName[0] == '\0')
                        break;
                    break;
                
                case IDM_STOP:
                    break;
                    
                case IDM_COMPUTE:
                    break;
                    
                case IDM_EDITDATA:
                    ExecuteExternProcFromGUI("d:\\vam\\acc-9d.exe\\edit.com");
                    ReinitGUI(pGUIInfo);
                    break;
                    
                case IDM_ERROR:
                    DialogBoxParam( pGUIInfo, &ErrorDlgHeader, ErrorBoxProc, NULL);
                    break;
                
                case IDM_CUTTINGPARAM:
                case IDM_CONTENTS:
                case IDM_INDEX:
                default:
                    sprintf(szText, "You choose a menu item, IDM:%d", wParam);
                    MessageBox(pGUIInfo, szText, szMsgCaption, MB_OK);
                    break;
            }
            break;
       
        case MSG_PAINT:
            {
                DC dc = *((PDC)wParam);
                BeginPaint(&dc);

                // Draw client rect here;
                Bitmap((LPSTR)"bitmap1.bmp", 15, 120, 300);

                EndPaint(&dc);
            }
            break;
            
        case MSG_CHAR:
        {
        	char szChar[256];
        	sprintf(szChar, "Press a key: %u.", wParam);
			MessageBox(pGUIInfo, szChar, szMsgCaption, MB_OK | MB_ICONINFORMATION);
        	break;
        }
        default:
            break;
    }
            
    return DefaultGUIProc(pGUIInfo, uMsg, wParam, lParam);
}

