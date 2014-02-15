// Support.c
// This file include functions that handle the display of 
// mono bitmap, caret, and lower layer functions.
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

STATUSBAR  SB[4] = {{5, 349, 0}, {359, 196, 0}, {559, 36, 0}, {599, 36, 0}};
struct _videoconfig vc;
RECT rcClipRgn;
void (__interrupt __far *oldClock)();
CARETINFO  CaretInfo = {0, 0, 0, FALSE, FALSE, 0, 0};
BOOL fNewKb = FALSE;

static CFONTINFO CFontInfo;
static char* pAFont;
static _vmhnd_t hdlImage = _VM_NULL;       // Image memory handle
static MOUSEINFO mi =
{
    1, 0, 0,
    0, 0,
    0, 0,
    0, 0,
    0, 0
};

static BOOL LoadFont( void );
static void UnloadFont( void );

static int MouseInit( void );

/*
 * Function: BOOL GUIAPI InitVirtualMemory( void );
 * Parameters:
 *      None.
 * Return:  success-----TRUE.
 *          failure-----FALSE.
 */
 
BOOL GUIAPI InitVirtualMemory( unsigned int iMinKByte, unsigned int iMaxKByte )
{
    // Allocate virtual memory and lock it.
    if ( !_vheapinit( 64*iMinKByte, 64*iMaxKByte, _VM_XMS ) )
    {
        printf( "Could not initialize virtual memory manager.\n\n" );
        _settextposition(24, 1);
        _settextcolor( 14 );
        _outtext("Please check the free conventional memory and extended memory manager of your computer!");
        return FALSE;
    }
    
    return TRUE;
}

/*
 * Function: BOOL LockFont( void );
 *      This function load the Chinese font(16*16) library into the virtual memory.
 * Parameters:
 *      None.
 * Return:  success-----TRUE.
 *          failure-----FALSE.
 */
BOOL FAR LockFont( void )
{
    return TRUE;
}

/*
 * Function: BOOL UnlockFont( void );
 *      This function load the Chinese font(16*16) library into the virtual memory.
 * Parameters:
 *      None.
 * Return:  success-----TRUE.
 *          failure-----FALSE.
 */
void FAR UnlockFont( void )
{
}

/*
 * Function: BOOL LoadFont( void );
 *      This function load the Chinese font(16*16) library into the virtual memory.
 * Parameters:
 *      None.
 * Return:  success-----TRUE.
 *          failure-----FALSE.
 */
static BOOL LoadFont( void )
{
    int i, j;
    FILE* fp;
    char pBuffer[1328];
    LPSTR lpStr;
    
    // Load Chinese font information to the virtual memory.
    printf( "Allocate virtual memory for the font library..." );
    for( i=0; i<5; i++)
    {
        if ( (CFontInfo.Handle[i] = _vmalloc( 60L*1024 )) == _VM_NULL )
        {
            printf( "failure.\n\n");
            _settextposition(24, 1);
            _settextcolor( 14 );
            _outtext("Your computer should have less than 2Mb memory!");
            return FALSE;
        }
    }
    printf( "done.\n" );

    // Open font file and read information to the virtual memory.
    printf( "Read and load font information from file..." );
    if(!(fp = fopen( ".\\bin\\hzk16", "rb")))
    {
        printf("failure.\n\n");
        _settextposition(24, 1);
        _settextcolor( 14 );
        _outtext("The font file ""hzk16"" can not be found!");
        return FALSE;
    }
        
    for(i=0; i<4; i++)
    {
        lpStr = (LPSTR)_vload( CFontInfo.Handle[i], _VM_DIRTY );
        fseek(fp, 0L + i*61440, SEEK_SET);
        for(j=0; j<60; j++)
        {
            fread(pBuffer, sizeof(char), 1024, fp);
            _fmemcpy(lpStr + j*1024L, pBuffer, 1024);
        }
    }
    
    lpStr = (LPSTR)_vload( CFontInfo.Handle[4], _VM_DIRTY );
    fseek(fp, 4L*61440, SEEK_SET);
    for(j=0; j<15; j++)
    {
        fread(pBuffer, sizeof(char), 1024, fp);
        _fmemcpy(lpStr + j*1024L, pBuffer, 1024);
    }
    fclose(fp);
    
    // Load ASSIC font information to the near memory.
    printf( "Load ASSIC font..." );
    pAFont = malloc(4096);
    if(!(fp = fopen( ".\\bin\\asc16", "rb")))
    {
        printf("failure.\n\n");
        _settextposition(24, 1);
        _settextcolor( 14 );
        _outtext("The font file ""asc16"" can not be found!");
        return FALSE;
    }
    fseek(fp, 0L, SEEK_SET);
    fread(pAFont, sizeof(char), 4096, fp);
    fclose(fp);
    printf( "done.\n" );
    
    // Alloc memory and load icon image
    printf( "Allocate virtual memory for icon image..." );
    if ( (hdlImage = _vmalloc( 3L*1328 )) == _VM_NULL )
    {
        printf( "failure.\n\n");
        _settextposition(24, 1);
        _settextcolor( 14 );
        _outtext("Your computer should have less than 1Mb extended memory!");
        return FALSE;
    }
    if ( (lpStr = (LPSTR)_vload( hdlImage, _VM_DIRTY )) == NULL )
    {
        printf( "failure.\n\n");
        _settextposition(24, 1);
        _settextcolor( 14 );
        _outtext("Insufficient memory!");
        return FALSE;
    }
    printf( "done.\n" );

    // Load icon image from file.
    printf( "Read icon image from file..." );
    if(!(fp = fopen( ".\\bin\\icon.img", "rb")))
    {
        printf("failure.\n\n");
        _settextposition(24, 1);
        _settextcolor( 14 );
        _outtext("The icon image file ""icon.img"" can not be found!");
        return FALSE;
    }
        
    fseek(fp, 0L, SEEK_SET);
    for(j=0; j<3; j++)
    {
        fread(pBuffer, sizeof(char), 1328, fp);
        _fmemcpy(lpStr + j*1328L, pBuffer, 1328);
    }
    fclose(fp);
    printf( "done.\n" );

    return TRUE;
}

/*
 * Function: void UnloadFont( void );
 *      This function unload the Chinese font(16*16) library 
 *      and free the vitual memory then terminate the vitual memory manager.
 * Parameters:
 *      None.
 * Return:
 *      None.
 */
static void UnloadFont( void )
{
    int i;
    
    for(i=0; i<5; i++)
    {
        if(CFontInfo.Handle[i] != _VM_NULL)
            _vfree(CFontInfo.Handle[i]);
    }
    
    free(pAFont);
    
    if(hdlImage != _VM_NULL)
        _vfree(hdlImage);
}

/*
 * Function: BOOL InitGUI( void );
 *      This Function Initializate tha graphics element, such as
 *    videomode, pallete etc.
 * Parameters:
 *      None.
 * Return:  success-----TRUE.
 *          failure-----FALSE.
 */
BOOL GUIAPI InitGUI()
{   
    // Color pallete.
    long pal[16]=
       {RGB( 0, 0, 0 ),         //black         --0
        RGB( 0, 0, 63 ),        //blue          --1
        RGB( 0, 63, 63 ),       //cyan          --2
        RGB( 0, 63, 0 ),        //green         --3
        RGB( 63, 63, 0 ),       //yellow        --4
        RGB( 63, 0, 0 ),        //red           --5
        RGB( 63, 0, 63 ),       //magenta       --6
        RGB( 48, 48, 48 ),      //light gray    --7
        RGB( 0, 0, 32 ),        //dark blue     --8
        RGB( 0, 32, 32 ),       //dark cyan     --9
        RGB( 0, 32, 0 ),        //dark green    --10
        RGB( 32, 32, 0 ),       //dark yellow   --11
        RGB( 32, 0, 0 ),        //dark red      --12
        RGB( 32, 0, 32 ),       //dark magenta  --13
        RGB( 32, 32, 32 ),      //dark gray     --14
        RGB( 63, 63, 63 )       //light white   --15
       };
        
    // Set new interrupt vector.
    _disable();
    oldClock = _dos_getvect(0x1c);
    _dos_setvect(0x1c, Clock);
    _enable();
    
    // Set new critical error handler.
    _harderr(&CriticalErrorHandler);
    
    // Load the Chinese font library and message box icon image to the vitual memory.
    if(!LoadFont())
        return FALSE;

    // Initialize the graphics adapter.
    if( !_setvideomode( _VRES16COLOR ) )
    {
        _setvideomode( _DEFAULTMODE );
        printf( "Invalid graphics adapter!\n" );
        return FALSE;
    }
    
    _getvideoconfig( &vc );
    rcClipRgn.left = rcClipRgn.top = 0;
    rcClipRgn.right = vc.numxpixels - 1;
    rcClipRgn.bottom = vc.numypixels - 1;

    if( !_remapallpalette( pal ) )
    {
        _setvideomode( _DEFAULTMODE );
        printf( "Palettes not available with this adapter!\n" );
        return FALSE;
    }
    _setbkcolor(0);
    
    // To determine weather the keyboad is ehanced.
    if( (*(BYTE FAR *)( 0x00400096 )) & 0x10 )
        fNewKb = TRUE;
        
    MouseInit();
    return TRUE;
}

/*
 * Function: void TerminateGUI( void );
 *      This Function Terminate this procedure.
 * Parameters:
 *      None.
 * Return:
 *      None.
 */

void GUIAPI TerminateGUI( void )
{ 
    _disable();
    _dos_setvect(0x1c, oldClock);
    _enable();
    
    if(vc.mode != 3)
        _setvideomode( _DEFAULTMODE );
    
    UnloadFont();
}

/*
 * Function: void DosShell(PGUIINFO pGUIInfo);
 *      This Function Terminate this procedure.
 * Parameters:
 *      None.
 * Return:
 *      None.
 */
void GUIAPI DosShell()
{ 
    short oldfgd;
    
    _disable();
    _dos_setvect(0x1c, oldClock);
    _enable();
    
    _setvideomode( _DEFAULTMODE );
    UnloadFont();
    _vheapterm();

    _flushall();
    
    oldfgd = _gettextcolor();
    _settextcolor( 14 );         // Set to yellow color.
    _settextposition(1, 1);
    _outtext("Û²±°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°±²Û");
    _settextposition(2, 1);
    _outtext("Û²±°                                                                        °±²Û");
    _settextposition(3, 1);
    _outtext("Û²±°    Type EXIT and press ENTER to quit this MS_DOS prompt and return.    °±²Û");
    _settextposition(4, 1);
    _outtext("Û²±°                                                                        °±²Û");
    _settextposition(5, 1);
    _outtext("Û²±°               (C) Copyright Mr. Wei Yongming 1996 ~ 1998.              °±²Û");
    _settextposition(6, 1);
    _outtext("Û²±°                                                                        °±²Û");
    _settextposition(7, 1);
    _outtext("Û²±°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°±²Û");
    _settextcolor(oldfgd);
    
    _spawnvp(_P_WAIT, "c:\\command.com", NULL);
    
    
}

/*
 * Function: void ExecuteExternProcFromGUI( PSTR cmdname )
 *      This Function Terminate this procedure.
 * Parameters:
 *      None.
 * Return:
 *      None.
 */
void GUIAPI ExecuteExternProcFromGUI( PSTR cmdname )
{ 
    _disable();
    _dos_setvect(0x1c, oldClock);
    _enable();
    
    _setvideomode( _DEFAULTMODE );
    UnloadFont();
    _vheapterm();

    _flushall();

    _spawnvp(_P_WAIT, cmdname, NULL);
}

/*
 * Function: BOOL Bitmap( LPSTR lpFileName, int x, int y);
 *      This Function display a color bitmap with information in the file
 * Parameters:
 *      lpFileNme: File name;
 *      x,y: the coordination of display vector.
 * Return:  success-----TRUE.
 *          failure-----FALSE.
 */
BOOL GUIAPI Bitmap( LPCSTR lpFileName, short color, short lx, short by)
{
    char FileName[13];
    BYTE *bitmapBuff, *bitmap;
    int byte;
    FILE *fp;
    int i,j;
    BITMAPFILEHEADER bmfh;
    BITMAPINFOHEADER bmih;
    long reset;
    
    _fstrcpy((char __far *)FileName, lpFileName);

    if(!(fp = fopen( FileName, "rb")))return FALSE;
   
    fseek(fp, 0L, SEEK_SET);
    fread(&bmfh, sizeof(char), sizeof(BITMAPFILEHEADER), fp);
    fread(&bmih, sizeof(char), sizeof(BITMAPINFOHEADER), fp);
    reset = (long)(bmih.biSizeImage/bmih.biHeight - bmih.biWidth/8);
    byte = (int)bmih.biWidth/8;
    
    set_color(color);
    
    if(!(bitmap = malloc((size_t)bmih.biSizeImage)))
    {
        fclose(fp);
        return FALSE;
    }
    bitmapBuff = bitmap;
    fseek(fp, (long)bmfh.bfOffBits, SEEK_SET);
    fread(bitmap, 1, (size_t)bmih.biSizeImage, fp);
    fclose(fp);

    for(i=0; i<(int)bmih.biHeight; i++)
    {
        for(j=0; j<byte; j++)
        {
            disp_byte(lx + j*8, by - i, (BYTE)(~(*bitmap)));
            bitmap++;
        }
        bitmap += reset;
    }
    
    free(bitmapBuff);
    return TRUE;
}

static BYTE cGrayTextMask[]={0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
void GUIAPI SetGrayTextMask()
{
    SetDisplayMask(cGrayTextMask);
}

static BYTE cDisplayMask[]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
void GUIAPI SetDisplayMask(BYTE* cMask)
{
    memcpy(cDisplayMask, cMask, sizeof(BYTE)*8);
}

void GUIAPI RemoveDisplayMask()
{
    int i;
    for(i=0; i<8; i++)
        cDisplayMask[i] = 0xFF;
}

/*
 * Function: cc_wt16( LPSTR lpCStr, int x, int y )
 *       This Function display Chinese word with information in file.
 * Parameters:
 *       x,y: the coordination of display point.
 * Return:  None.
 */

void GUIAPI cc_wt16( LPCSTR lpCStr, short x, short y )
{
    BYTE KBuffer[32];
    BYTE bQM;
    BYTE bWM;
    BYTE byte;
    long lOffset;
    int i, j;
    int iCurIndex = 0;
    int iIndex = 0;
    LPSTR lpStr = NULL;
   
    i = j = 0;
    do
    {
        bQM = (BYTE)(*(lpCStr + i));
        bWM = (BYTE)(*(lpCStr + i + 1));
        
        if(bQM == '\0' || bQM == '\t' || bQM == '\n')
            break;
        
        if((bQM & 0x80) && (bWM & 0x80))    // This is a Chinese word
        {
            lOffset = ((bQM - 161)*94 + bWM - 161)*32L;
            
            iIndex = (int)(lOffset/61440L);
            if(lpStr == NULL || iCurIndex != iIndex)
            {
                iCurIndex = iIndex;
                lpStr = (LPSTR)_vload(CFontInfo.Handle[iIndex], _VM_CLEAN);
            }
            
            lOffset = lOffset%61440;
            _fmemcpy(KBuffer, lpStr + lOffset, 32);
            for(j=0; j<16; j++)
            {
                byte = (BYTE)(KBuffer[j*2] & cDisplayMask[(j<8)?j:(15-j)]);
                disp_byte(x, y + j, byte);
                byte = (BYTE)(KBuffer[j*2+1] & cDisplayMask[(j<8)?j:(15-j)]);
                disp_byte(x + 8, y + j, byte);
            }
            x += 16;
            i += 2;
        }
        else
        {
            lOffset = bQM * 16L;
            memcpy(KBuffer, pAFont + lOffset, 16);
            for(j=0; j<15; j++)
            {
                byte = (BYTE)(KBuffer[j] & cDisplayMask[(j<8)?j:(15-j)]);
                disp_byte(x, y + j + 1, byte);
            }
            x += 8;
            i += 1;
         
            if(bWM == '\0' || bWM == '\t' || bWM == '\n')
                break;
         
            if(bWM & 0x80)
                continue;
            
            lOffset = bWM * 16L;
            memcpy(KBuffer, pAFont + lOffset, 16);
            
            for(j=0; j<15; j++)
            {
                byte = (BYTE)(KBuffer[j] & cDisplayMask[(j<8)?j:(15-j)]);
                disp_byte(x, y + j + 1, byte);
            }
            x += 8;
            i += 1;
        }
    
    }while(TRUE);

}
   
/*
 * Function: cc_printf( LPSTR lpCStr, int x, int y )
 *       This Function display formatted Chinese word.
 * Parameters:
 *       x,y: the coordination of display point.
 * Return:  None.
 */

void GUIAPI cc_printf( LPCSTR lpCStr, short x, short y )
{
    int i;
    int iNewx, iNewy;
    
    i = 0;
    iNewx = x;
    iNewy = y;
    
    if(*lpCStr != '\0' && *lpCStr != '\t' && *lpCStr != '\n')
        cc_wt16(lpCStr, iNewx, iNewy);
        
    while(*(lpCStr+i) != '\0')
    {
        if(*(lpCStr+i) == '\t')
        {
            iNewx = x + ((iNewx - x)/32 + 1)*32;
            cc_wt16(lpCStr+i+1, iNewx, iNewy);
        }
        else if(*(lpCStr+i) == '\n')
        {
            iNewx = x;
            iNewy = iNewy + 16;
            cc_wt16(lpCStr+i+1, x, iNewy);
        }
        else
        {
            iNewx += 8;
        }
        
        i++;
    }
}

/*
 * Function: int GetOutWidth( LPSTR lpCStr )
 *      This function calculates the lpCStr's width that will be outputed by 
 *  function cc_printf.
 * Parameters:
 *      lpCStr: pointer to Chinese Words.
 * Return:  
 *      int:    the width that want to get.
 */

int GUIAPI GetOutWidth( LPCSTR lpCStr )
{
    int iWidth = 0;
    int i = 0;
    
    if(lpCStr == NULL)
        return 0;
    
    while(*(lpCStr+i) != '\0' && *(lpCStr+i) != '\n')
    {
        if(*(lpCStr+i) == '\t')
        {
            iWidth = (iWidth/32 + 1)*32;
        }
        else
            iWidth += 8;
        
        i++;
    }
    
    return iWidth;
}

/*
 * Function: static void DisplayIcon( LPSTR pImage, short x, short y ).
 *      This function display a icon. 
 * Parameters:
 *      ANDImage, XORImage;
 *      x,y. 
 * Return: 
 *      None
 */
void GUIAPI DisplayIcon(LPSTR lpImage, short x, short y )
{
    if(!lpImage)return;
    def_GDC_reg();
    _putimage(x, y, lpImage, _GAND);
    _putimage(x, y, lpImage+664, _GXOR);
}

/*
 * Function: void GUIAPI DrawStatusBar(  ).
 *      this function create status bar.
 * Parameters:                                                          
 *      None.
 * Return: 
 *      None
 */
void GUIAPI DrawStatusBar( void )
{   
    int i;
    
    _setcolor(7);
    
    _rectangle(_GFILLINTERIOR, 0, vc.numypixels - SB_HEIGHT, vc.numxpixels, vc.numypixels);
     
    _setcolor(15);
    _moveto(0, vc.numypixels - 24);
    _lineto(vc.numxpixels, vc.numypixels - 24);
    
    for(i=0; i<4; i++)
    {
        _moveto(SB[i].xOffset, vc.numypixels - 3);
        _lineto(SB[i].xOffset + SB[i].Width, vc.numypixels - 3);
        _lineto(SB[i].xOffset + SB[i].Width, vc.numypixels - 21);
    }
    
    _setcolor(14);
    for(i=0; i<4; i++)
    {
        _moveto(SB[i].xOffset, vc.numypixels - 3);
        _lineto(SB[i].xOffset, vc.numypixels - 21);
        _lineto(SB[i].xOffset + SB[i].Width, vc.numypixels - 21);
    }
}

/*
 * Function: void GUIAPI SetStatusBarInfo( int iSBIndex, PSTATUSBARDATA pSBData ).
 *      this function set or erase status bar according to 
 *      information in pSBData.
 * Parameters:                                                          
 *      iSBIndex: the index of child frame of status bar;
 *      pSBData: pointer to STATUSBARDATA struct.
 * Return: 
 *      None
 */
void GUIAPI SetStatusBarInfo( int iSBIndex, PSTATUSBARDATA pSBData )
{   
    SB[iSBIndex].uTimes ++;
    
    SetPtrVis(HIDE);
    set_cliprgn(SB[iSBIndex].xOffset + 1, vc.numypixels - 20, 
            SB[iSBIndex].xOffset + SB[iSBIndex].Width-1, vc.numypixels - 4);
    _setcolor( 7 );
    _rectangle(_GFILLINTERIOR, 
            SB[iSBIndex].xOffset + 1, vc.numypixels - 20, 
            SB[iSBIndex].xOffset + SB[iSBIndex].Width-1, vc.numypixels - 4);
            
    if(pSBData != NULL)
    {
        set_color(pSBData->fgcolor);
        cc_wt16(pSBData->lpStr, SB[iSBIndex].xOffset + 2, vc.numypixels - 20);
    }
    set_cliprgn(0, 0, vc.numxpixels, vc.numypixels);

    SetPtrVis(SHOW);
}

/*
 * Function: void __interrupt __far Clock( void ).
 *      this function is the new vector of INT 0x1c; 
 * Parameters:
 *      None.
 * Return: 
 *      None.
 */
void __interrupt __far Clock( void )
{   
    CaretInfo.uCountForChange++;
    
    (*oldClock)();

    if(!CaretInfo.fDisplay)
        return;
    
    CaretInfo.count++;
    CaretInfo.uCount++;
    if(CaretInfo.uCount > MAXCOUNTE)
    {
        CaretInfo.uCount = 0;
    }
}

/*
 * Function: void __far CriticalErrorHandler( unsigned ax, unsigned di, unsigned __far* header)
 *      this function is the new vector of INT 0x1c; 
 * Parameters:
 *      None.
 * Return: 
 *      None.
 */
void __far CriticalErrorHandler( unsigned ax, unsigned di, unsigned __far* header)
{
    DWORD   lStyle = MB_ABORTRETRYIGNORE;
    char szInfo[256];
    
    if((ax >> 8) & 128)
        szInfo[0] = '\0';
    else
        sprintf(szInfo, "Drive %c: ", 'A' + ax & 0xff);
        
    switch(di)
    {
        case 0:
            strcat(szInfo, "Attempt to write a write-protected disk.");
            lStyle = MB_RETRYCANCEL;
            break;
        case 1:
            strcat(szInfo, "Unkown unit.");
            lStyle = MB_RETRYCANCEL;
            break;
        case 2:
            strcat(szInfo, "Drive not ready.");
            lStyle = MB_RETRYCANCEL;
            break;
        case 3:
            strcat(szInfo, "Unkown command.");
            break;
        case 4:
            strcat(szInfo, "Cyclic-redundancy-check error in data.");
            break;
        case 5:
            strcat(szInfo, "Bad drive-request structure length.");
            break;
        case 6:
            strcat(szInfo, "Seek error.");
            break;
        case 7:
            strcat(szInfo, "Unkown media type.");
            break;
        case 8:
            strcat(szInfo, "Sector not found.");
            break;
        case 9:
            strcat(szInfo, "Printer out of paper.");
            break;
        case 10:
            strcat(szInfo, "Write fault.");
            break;
        case 11:
            strcat(szInfo, "Read fault.");
            break;
        case 12:
            strcat(szInfo, "General failure.");
            break;
        }
        
        switch(SysMessageBox(NULL, szInfo, "System Error", lStyle))
        {
            case IDCANCEL:
                _hardresume(_HARDERR_FAIL);
                break;
            case IDABORT:
                _hardresume(_HARDERR_ABORT);
                break;
            case IDRETRY:
                _hardresume(_HARDERR_RETRY);
                break;
            case IDIGNORE:
                _hardresume(_HARDERR_IGNORE);
                break;
        }
}

/* GetKey - Gets a key from the keyboard. This routine distinguishes
 * between ASCII keys and function or control keys with different shift
 * states. It also accepts a flag to return immediately if no key is
 * available.
 *
 * Params: fWait - Code to indicate how to handle keyboard buffer:
 *   NO_WAIT     Return 0 if no key in buffer, else return key
 *   WAIT        Return first key if available, else wait for key
 *   CLEAR_WAIT  Throw away any key in buffer and wait for new key
 *
 * Return: One of the following:
 *
 *   Keytype                                High Byte    Low Byte
 *   -------                                ---------    --------
 *   No key available (only with NO_WAIT)       0           0
 *   Only Shift key was pressed                 x           0
 *   ASCII value                                0        ASCII code
 *   Unshifted function or keypad               1        scan code
 *   Shifted function or keypad                 2        scan code
 *   CTRL function or keypad                    3        scan code
 *   ALT function or keypad                     4        scan code
 *
 * Note:   getkey cannot return codes for keys not recognized by BIOS
 *         int 16, such as the CTRL-UP or the 5 key on the numeric keypad.
 */
unsigned GUIAPI GetKey( int fWait )
{
    unsigned uKey, uShift;

    /* If CLEAR_WAIT, drain the keyboard buffer. */
    if( (fWait == CLEAR_WAIT) || (fWait == CLEAR) )
        while( _bios_keybrd( fNewKb ? _NKEYBRD_READY : _KEYBRD_READY ) )
            _bios_keybrd( fNewKb ? _NKEYBRD_READ : _KEYBRD_READ );
    
    if( fWait == CLEAR )
        return 0;
        
    /* If NO_WAIT, return 0 if there is no key ready. */
    if( !fWait && !_bios_keybrd( fNewKb ? _NKEYBRD_READY : _KEYBRD_READY ) )
        return 0;
    
    
    /* Get key code. */
    uKey = _bios_keybrd( fNewKb ? _NKEYBRD_READ : _KEYBRD_READ );

    /* If low byte is not zero, it's an ASCII key. Check scan code to see
     * if it's on the numeric keypad. If not, clear high byte and return.
     */
    if( uKey & 0x00ff )
        if( (uKey >> 8) < 69 )
            return( uKey & 0x00ff );

    /* For function keys and numeric keypad, put scan code in low byte
     * and shift state codes in high byte.
     */
    uKey >>= 8;
    uShift = _bios_keybrd( _KEYBRD_SHIFTSTATUS ) & 0x000f;
    switch( uShift )
    {
        case 0:
            return( 0x0100 | uKey );  /* None (1)    */
        case 1:
        case 2:
        case 3:
            return( 0x0200 | uKey );  /* Shift (2)   */
        case 4:
            return( 0x0300 | uKey );  /* Control (3) */
        case 8:
            return( 0x0400 | uKey );  /* Alt (4)     */
    }
}

/* Function: void GUIAPI Beep(int frequency, int duration)
 *        This function beep with a specified frequency 
 *      and duration time
 * Parameters: 
 *        frequency: the number of freanqency;
 *        duration : the micseconds number of duration time.
 * Return:
 *        None.
 */
void GUIAPI Beep( int frequency, int duration )
{
    int control;

    /* If frequency is 0, Beep doesn't try to make a sound. It
     * just sleeps for the duration.
     */
    if( frequency )
    {
        /* 75 is about the shortest reliable duration of a sound. */
        if( duration < 75 )
            duration = 75;

        /* Prepare timer by sending 10111100 to port 43. */
        _outp( 0x43, 0xb6 );

        /* Divide input frequency by timer ticks per second and
         * write (byte by byte) to timer.
         */
        frequency = (unsigned)(1193180L / frequency);
        _outp( 0x42, (char)frequency );
        _outp( 0x42, (char)(frequency >> 8) );

        /* Save speaker control byte. */
        control = inp( 0x61 );

        /* Turn on the speaker (with bits 0 and 1). */
        _outp( 0x61, control | 0x3 );
    }

    Sleep( (clock_t)duration );

    /* Turn speaker back on if necessary. */
    if( frequency )
        _outp( 0x61, control );

}

/*
 * Function: void GUIAPI Sleep( clock_t wait );
 *      This function pauses for a specified number of microseconds.
 * Parameters:
 *      wait: the number of microseconds.
 * Return:  
 *      None.
 */
void GUIAPI Sleep( clock_t wait )
{
    clock_t goal;

    goal = wait + clock();
    while( goal > clock() )
        ;
}

/*
 * Function: void GUIAPI set_color( short color );
 *      This function set color value.
 * Parameters:
 *      color: the color value.
 * Return:  
 *      None.
 */
void GUIAPI set_color(short color)
{
    _asm
    {
        mov     dx, 3ceh
        mov     ax, 0005h
        out     dx, al
        inc     dx
        xchg    ah, al
        out     dx, al
        dec     dx
        
        mov     al, 0
        out     dx, al
        inc     dx
        mov     ax, color
        out     dx, al
        dec     dx
        
        mov     ax, 0f01h
        out     dx, al
        inc     dx
        xchg    ah, al
        out     dx, al
        dec     dx
        
    }
}

/*
 * Function: void GUIAPI def_GDC_reg( void );
 *      This function set color value.
 * Parameters:
 *      color: the color value.
 * Return:  
 *      None.
 */
void GUIAPI def_GDC_reg( void )
{
    _asm
    {
        mov     dx, 3ceh
        mov     ax, 0
        out     dx, al
        inc     dx
        out     dx, al
        dec     dx
        
        mov     al, 0001
        out     dx, al
        inc     dx
        xchg    ah, al
        out     dx, al
        dec     dx
        
        mov     ax, 0ff08h
        out     dx, al
        inc     dx
        xchg    ah, al
        out     dx, al
        dec     dx
        
    }
}

/*
 * Function: void GUIAPI disp_byte( short x, short y, BYTE by );
 *      This function display a line.
 * Parameters:
 *      x, y: coordinate of point;
 *      by  : the byte that include the infomation of display line.
 * Return:  
 *      None.
 */
void GUIAPI disp_byte(short x, short y, BYTE by)
{
    if( ((rcClipRgn.left/8)*8 > x) || 
        ((rcClipRgn.right/8)*8 < x) ||
        ( rcClipRgn.top    > y) ||
        ( rcClipRgn.bottom < y) )
        return;
    
    _asm
    {
        push ds
        mov  ax, y
        mov  bx, x
        mov  dx, 80
        mul  dx
        mov  cl, 3
        shr  bx, cl
        push bx
        push ax
        shl  bx, cl
        mov  ax, x
        sub  ax, bx
        mov  cl, al
        pop  ax 
        pop  bx 
        add  bx, ax
        mov  ax, 0a000h
        mov  ds, ax
        mov  dx, 3ceh
        mov  al, 08h
        out  dx, al
        inc  dx
        mov  ah, by
        mov  al, 0
        shr  ax, cl
        xchg ah, al     
        out  dx, al
        or   [bx], al
        cmp  cl,0
        je   equ
        xchg ah, al
        out  dx, al
        inc  bx
        or   [bx], al
equ:    pop  ds
    }
}

void GUIAPI set_cliprgn(short lx, short ty, short rx, short by) 
{
    rcClipRgn.left   = lx;
    rcClipRgn.top    = ty;  
    rcClipRgn.right  = rx;
    rcClipRgn.bottom = by;
    
    _setcliprgn(lx, ty, rx, by);    
}

void GUIAPI move_image(short srcx, short srcy, short srcw, short srch, short destx, short desty)
{
    _asm
    {
        push    ds
        push    es
        mov     dx, 3ceh
        mov     ax, 0105h
        out     dx, al
        xchg    ah, al
        inc     dx
        out     dx, al

        mov     ax, srcy
        mov     si, srcx
        mov     dx, 80
        mul     dx
        mov     cl, 3
        shr     si, cl
        add     si, ax
        push    si
        
        mov     ax, desty
        mov     di, destx
        mov     dx, 80
        mul     dx
        mov     cl, 3
        shr     di, cl
        add     di, ax
        push    di

        mov     ax, 0a000h
        mov     ds, ax
        mov     es, ax
        
        mov     bx, srcw
        mov     cl, 3
        shr     bx, cl
        mov     dx, 80

        mov     ax, srch
move1:  mov     cx, bx
move2:  movsb
        loop    move2
        pop     di
        pop     si
        add     si, dx
        add     di, dx
        push    si
        push    di
        dec     ax
        cmp     ax, 0
        jne     move1
        
        pop     di
        pop     si
        
        mov     dx, 3ceh
        mov     ax, 0005h
        out     dx, al
        xchg    ah, al
        inc     dx
        out     dx, al

        pop     es
        pop     ds
    }
}

/* LPIMAGEMEM GUIAPI SaveImageUseVirtualMem(int x, int y, int iWidth, int iHeight)
 *
 * Params:  x, y:       the position of image that want to save.
 *          iWidth, iHeight:    the size of image that want to save.
 *                           
 * Return: Handle of virtual memory that save image.
 *
 *  1995.8.10.AM.
 *
 */
_vmhnd_t GUIAPI SaveImageUseVirtualMem(int x, int y, int iWidth, int iHeight)
{
    _vmhnd_t Handle;
    char FAR* lpImageMem;
    unsigned long size;
    
    SetPtrVis(HIDE);
    def_GDC_reg();
    size = (unsigned long)_imagesize(0, 0, iWidth, iHeight);
    Handle = _vmalloc(size);
    if(Handle == _VM_NULL)
        return _VM_NULL;
        
    lpImageMem = (char FAR*)_vload(Handle, _VM_DIRTY);
    _getimage(x, y, x + iWidth, y + iHeight, lpImageMem);
    SetPtrVis(SHOW);

    return Handle;
}

/* void GUIAPI RestoreImageUseVirtualMem( _vmhnd_t hdlImage, int x, int y )
 *
 * Params:  hdlImage:   the handle of image memory;
 *          x, y:       the position that image will be put.
 *
 * Return: None.
 *
 *  1995.8.10.AM.
 *
 */
void GUIAPI RestoreImageUseVirtualMem( _vmhnd_t hdlImage, int x, int y )
{
    char FAR* lpImageMem;
    
    lpImageMem = (char FAR*)_vload(hdlImage, _VM_CLEAN);
    
    SetPtrVis(HIDE);
    def_GDC_reg();
    _putimage(x, y, lpImageMem, _GPSET);
    SetPtrVis(SHOW);
    
    _vfree(hdlImage);
}

/* _vmhnd_t GUIAPI GetIconImageHandler( void )
 *
 * Params:  none.
 *
 * Return: the virtual memomry handler of icon image.
 *
 *  1997.7.3.AM.
 *
 */
_vmhnd_t GUIAPI GetIconImageHandler( void )
{
    return hdlImage;
}


/*
 * Function: void GUIAPI SetCaretPos( short x, short y ).
 *      This function set new caret position.
 * Parameters:
 *      x, y: newposition
 * Return: 
 *      None;
 * Note:
 *      Before call this function, undisplay caret first.
 */
void GUIAPI SetCaretPos( short x, short y )
{
    // Undisplay the old caret.
    if(CaretInfo.fShowOrHide)
    {
        SetPtrVis(HIDE);
        _setwritemode(_GXOR);
        _setcolor(COLOR_lightwhite);
        _moveto(CaretInfo.x, CaretInfo.y+2);
        _lineto(CaretInfo.x, CaretInfo.y+14);
        _moveto(CaretInfo.x + 1, CaretInfo.y+2);
        _lineto(CaretInfo.x + 1, CaretInfo.y+14);
        _setwritemode(_GPSET);
        CaretInfo.fShowOrHide = FALSE;
        SetPtrVis(SHOW);
    }
    
    // set new caret position.
    CaretInfo.x = x;
    CaretInfo.y = y;
    CaretInfo.fDisplay = TRUE;
    CaretInfo.count = 6;
    
    return;
}

/*
 * Function: void GUIAPI FlashCaret(void).
 *      This function flash caret in the current position.
 * Parameters:
 *      none.
 * Return: 
 *      None;
 * Note:
 *      Before call this function, undisplay caret first.
 */
void GUIAPI FlashCaret(void)
{
    if(CaretInfo.fDisplay && CaretInfo.count > 5)
    {
        SetPtrVis(HIDE);
        _setwritemode(_GXOR);
        _setcolor(COLOR_lightwhite);
        _moveto(CaretInfo.x, CaretInfo.y+2);
        _lineto(CaretInfo.x, CaretInfo.y+14);
        _moveto(CaretInfo.x + 1, CaretInfo.y+2);
        _lineto(CaretInfo.x + 1, CaretInfo.y+14);
        _setwritemode(_GPSET);
        SetPtrVis(SHOW);
        
        CaretInfo.count = 0;
        CaretInfo.fShowOrHide = !CaretInfo.fShowOrHide;
    }
}

/*
 * Function: void GUIAPI UndipslayCaret( void ).
 *      This function erase current caret .
 * Parameters:
 *      None.
 * Return: 
 *      None.
 * Note:
 *      This function always called before update position of caret.
 */
void GUIAPI UndisplayCaret( void )
{
    CaretInfo.fDisplay = FALSE;

    if(CaretInfo.fShowOrHide)
    {
        SetPtrVis(HIDE);
        _setwritemode(_GXOR);
        _setcolor(COLOR_lightwhite);
        _moveto(CaretInfo.x, CaretInfo.y+2);
        _lineto(CaretInfo.x, CaretInfo.y+14);
        _moveto(CaretInfo.x + 1, CaretInfo.y+2);
        _lineto(CaretInfo.x + 1, CaretInfo.y+14);
        _setwritemode(_GPSET);
        CaretInfo.fShowOrHide = FALSE;
        SetPtrVis(SHOW);
    }
}

BOOL GUIAPI IsCaretDisplaying(int* x, int* y)
{
    *x = CaretInfo.x;
    *y = CaretInfo.y;
    return CaretInfo.fDisplay;
}

/* MouseInit - Initialize mouse and turns on mouse pointer. Initializes
 * all internal variables used by other mouse functions. This function
 * should be called whenever a new video mode is set, since internal
 * variables are mode-dependent.
 *
 * Params: none
 *
 * Return: 0 if no mouse available, otherwise number of buttons available
 */
static int MouseInit()
{
//  struct videoconfig vc;
    char _far *pMode = (char _far *)0x00000449; /* Address for mode */

    /* Handle special case of Hercules graphics. To use mouse with video
     * page 0. assume mode 6. To use mouse with page 1, assume mode 5.
     * Since the mouse functions couldn't easily detect and adjust for
     * page changes anyway, this code assumes page 0. Note also that the
     * mouse for Hercules graphics must be set in text mono mode.
     */
    if( vc.mode == _HERCMONO )
    {   
        mi.fExist = 0;
        *pMode = 6;
    }

    mi.fInit = 1;
    _asm
    {
        sub     ax, ax              ; Mouse function 0, reset mouse
        int     33h
        mov     mi.fExist, ax       ; Set existence flag for future calls
        or      ax, ax              ; If AX = 0, there is no mouse
        jnz     exist
        ret                         ; so quit
exist:
        mov     mi.cBtn, bx         ; Save number of mouse buttons for return
    }

    /* Set graphics flag. */
    if( vc.numxpixels )
    {
        mi.fGraph = 1;
        mi.yActual = vc.numypixels - 1;
        mi.xActual = vc.numxpixels - 1;
    }
    else
        mi.fGraph = 0;

    /* The mouse works on a virtual screen of 640 x pixels by (8 * textrows)
     * vertical pixels. By default, it assumes 640 x 200 for 25-line mode.
     * You must call function 8 to adjust for other screen sizes.
     */
    mi.xVirtual = 639;
    if( mi.fGraph )
        mi.yVirtual = vc.numypixels - 1;
    else
        mi.yVirtual = (vc.numtextrows << 3) - 1;

    /* Reset Hercules graphics mode and reset the height. */
    if( vc.mode == _HERCMONO )
    {
        _setvideomode( _HERCMONO );
        mi.xVirtual = 719;
    }

    _asm
    {
        mov     ax, 8               ; Set minimum and maximum vertical
        sub     cx, cx              ; Minimum is 0
        mov     dx, mi.yVirtual     ; Maximum is 8 * rows (or rows SHL 3)
        int     33h                 ; Adjust for 25, 30, 43, 50, or 60 lines

        mov     ax, 1               ; Turn on mouse pointer
        int     33h

        mov     ax, 3               ; Get initial position and button status
        int     33h
        mov     mi.xLast, cx        ; Save internally
        mov     mi.yLast, dx
        mov     mi.fsBtnLast, bx
    }
    return mi.cBtn;                 /* Return the number of mouse buttons */
}

BOOL GUIAPI IsMouseAvailable()
{
    return mi.fExist;
}

/* GetMouseEvent - Check to see if there has been a mouse event. If event
 * occurred, update event structure.
 *
 * Params: pEvent - Pointer to event structure
 *
 * Return: 1 if event, 0 if no event
 */
int GUIAPI GetMouseEvent( EVENT _far *pEvent )
{
    int rtn;

    /* Make sure that mouse exists. */
    if( !mi.fExist )
        return 0;

    _asm
    {
        mov     ax, 3               ; Get Mouse position and button status
        int     33h
        sub     ax, ax              ; Assume no event

        cmp     cx, mi.xLast        ; Has column changed?
        jne     event
        cmp     dx, mi.yLast        ; Has row changed?
        jne     event
        cmp     bx, mi.fsBtnLast    ; Has button changed?
        je      noevent
event:
        mov     ax, 1               ; If something changed, event occurred
        mov     mi.xLast, cx        ; Update internal variables
        mov     mi.yLast, dx
        mov     mi.fsBtnLast, bx
noevent:
        mov     rtn, ax             ; Set return value
    }

    /* If event, put adjust values in structure. */
    if( rtn )
    {
        /* If graphics mode, adjust virtual mouse position to actual
         * screen coordinates.
         */
        if( mi.fGraph )
        {
            pEvent->hotx = (short)(((long)mi.xLast * mi.xActual) / mi.xVirtual);
            pEvent->hoty = (short)(((long)mi.yLast * mi.yActual) / mi.yVirtual);
        }
        /* If text mode, adjust virtual mouse position to 1-based
         * row/column.
         */
        else
        {
            pEvent->hotx = (mi.xLast >> 3) + 1;
            pEvent->hoty = (mi.yLast >> 3) + 1;
        }
        pEvent->fsBtn = mi.fsBtnLast;
    }
    return rtn;
}

/* GetPtrPos - Get mouse pointer position and button status regardless of
 * whether there was an event.
 *
 * Params: pEvent - Pointer to event structure
 *
 * Return: 0 if no mouse, otherwise 1
 */
int GUIAPI GetPtrPos( EVENT FAR *pEvent )
{
    /* Make sure that mouse exists. */
    if( !mi.fExist )
        return 0;

    _asm
    {
        mov     ax, 3               ; Get Mouse position and button status
        int     33h
        les     di, pEvent
        mov     es:[di].hotx, cx
        mov     es:[di].hoty, dx
        mov     es:[di].fsBtn, bx
    }

    /* If graphics mode, adjust virtual mouse position to actual
     * screen coordinates.
     */
    if( mi.fGraph )
    {
        pEvent->hotx = (short)(((long)pEvent->hotx * mi.xActual) / mi.xVirtual);
        pEvent->hoty = (short)(((long)pEvent->hoty * mi.yActual) / mi.yVirtual);
    }
    /* If text mode, adjust virtual mouse position to 1-based
     * row/column.
     */
    else
    {
        pEvent->hotx >>= 3;
        pEvent->hoty >>= 3;
        pEvent->hotx++;
        pEvent->hoty++;
    }
    return 1;
}

/* SetPtrVis - Set pointer visibility.
 *
 * Params: state - SHOW or HIDE
 *
 * Return: 0 if no mouse, otherwise 1
 */
int GUIAPI SetPtrVis( enum PTRVIS pv )
{
    /* Make sure that mouse exists. */
    if( !mi.fExist )
        return 0;

    _asm
    {
        mov ax, pv                  ; Show or hide mouse pointer
        int 33h
    }
}

/* SetPtrPos - Set mouse pointer position.
 *
 * Params: x - column position in text modes, actual x coordinate in graphics
 *         y - row position in text modes, actual y coordinate in graphics
 *
 * Return: 0 if no mouse, otherwise 1
 */
int GUIAPI SetPtrPos( short x, short y )
{
    /* Make sure that mouse exists. */
    if( !mi.fExist )
        return 0;

    /* If graphics, adjust actual coordinates to virtual coordinates. */
    if( mi.fGraph )
    {
        x = (short)(((long)x * mi.xActual) / mi.xVirtual);
        y = (short)(((long)y * mi.yActual) / mi.yVirtual);
    }
    /* If text, adjust row/column to 0-based virtual coordinates. */
    else
    {
        x--;
        y--;
        x <<= 3;
        y <<= 3;
    }

    _asm
    {
        mov     ax, 4               ; Set mouse position
        mov     cx, x
        mov     dx, y
        int     33h
    }
    return 1;
}

/* SetPtrShape - Set mouse pointer shape.
 *
 * Params: x - column position in text modes, actual x coordinate in graphics
 *         y - row position in text modes, actual y coordinate in graphics
 *
 * Return: 0 if no mouse, otherwise 1
 */
int GUIAPI SetPtrShape( PTRSHAPE FAR *ps )
{
    /* Make sure that mouse exists. */
    if( !mi.fExist )
        return 0;

    /* If graphics, use pointer shape bitmask array. */
    if( mi.fGraph )
    {
        _asm
        {
            les     di, ps
            mov     bx, es:[di].g.xHot      ; Load hot spot offsets
            mov     cx, es:[di].g.yHot
            mov     dx, di
            add     dx, 4

            mov     ax, 9                   ; Set graphics pointer
            int     33h
        }
    }
    /* If text, use pointer color/character values. */
    else
    {
        _asm
        {
            les     di, ps
            mov     bx, 0                   ; Use software cursor
            mov     cl, es:[di].t.chScreen
            mov     ch, es:[di].t.atScreen
            mov     dl, es:[di].t.chCursor
            mov     dh, es:[di].t.atCursor

            mov     ax, 10                  ; Set text pointer
            int     33h
        }
    }
    return 1;
}
