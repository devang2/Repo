; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CWatermarkDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "QR_Image.h"
LastPage=0

ClassCount=7
Class1=CBottomView
Class2=CImageView
Class3=CMainFrame
Class4=CQR_ImageApp
Class5=CAboutDlg
Class6=CRightView

ResourceCount=9
Resource1=IDD_ABOUTBOX
Resource2=IDD_BOTTOMVIEW
Resource3=IDR_IMAGEMENU
Resource4=IDD_ABOUTBOX (Japanese)
Resource5=IDD_RIGHTVIEW
Resource6=IDR_MAINFRAME (Japanese)
Resource7=IDD_WATERMARK
Resource8=IDR_IMAGEMENU (Japanese)
Class7=CWatermarkDlg
Resource9=IDR_MAINFRAME

[CLS:CBottomView]
Type=0
BaseClass=CFormView
HeaderFile=BottomView.h
ImplementationFile=BottomView.cpp
LastObject=IDC_EDITSOURCEDATA
Filter=D
VirtualFilter=VWC

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
LastObject=ID_WATERMARK_EMBED
Filter=T
VirtualFilter=fWC

[CLS:CQR_ImageApp]
Type=0
BaseClass=CWinApp
HeaderFile=QR_Image.h
ImplementationFile=QR_Image.cpp
LastObject=CQR_ImageApp
Filter=N
VirtualFilter=AC

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=QR_Image.cpp
ImplementationFile=QR_Image.cpp
LastObject=CAboutDlg
Filter=D
VirtualFilter=dWC

[CLS:CRightView]
Type=0
BaseClass=CFormView
HeaderFile=RightView.h
ImplementationFile=RightView.cpp
Filter=D
VirtualFilter=VWC
LastObject=CRightView

[DLG:IDD_BOTTOMVIEW]
Type=1
Class=CBottomView
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDITSOURCEDATA,edit,1352732740

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=11
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308480
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342177283
Control6=IDOK,button,1342373889
Control7=IDC_STATIC,static,1342308481
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATICURL,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATICMAILTO,static,1342308352

[DLG:IDD_RIGHTVIEW]
Type=1
Class=CRightView
ControlCount=17
Control1=IDC_STATIC,button,1342177287
Control2=IDC_COMBOLEVEL,combobox,1342242819
Control3=IDC_STATIC,button,1342177287
Control4=IDC_COMBOVERSION,combobox,1344339971
Control5=IDC_STATICVERSION,static,1342308352
Control6=IDC_CHECKAUTOEXTENT,button,1342242819
Control7=IDC_STATIC,button,1342177287
Control8=IDC_COMBOMASKINGNO,combobox,1344339971
Control9=IDC_STATICMASKINGNO,static,1342308352
Control10=IDC_STATIC,button,1342177287
Control11=IDC_EDITMODULESIZE,edit,1350639874
Control12=IDC_SPINMODULESIZE,msctls_updown32,1342177334
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATICBMPSIZE,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_BUTTONCOPY,button,1342242816
Control17=IDC_BUTTONSAVE,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_SAVE
Command2=ID_EDIT_CUT
Command3=ID_EDIT_COPY
Command4=ID_EDIT_PASTE
Command5=ID_APP_ABOUT
CommandCount=5

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_SAVE
Command2=ID_FILE_SAVE_AS
Command3=ID_APP_EXIT
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_VIEW_TOOLBAR
Command9=ID_VIEW_STATUS_BAR
Command10=ID_WATERMARK_EMBED
Command11=ID_APP_ABOUT
CommandCount=11

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_SAVE
Command4=ID_EDIT_PASTE
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_NEXT_PANE
Command8=ID_PREV_PANE
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_EDIT_CUT
Command12=ID_EDIT_UNDO
CommandCount=12

[CLS:CImageView]
Type=0
HeaderFile=imageview.h
ImplementationFile=imageview.cpp
BaseClass=CView
LastObject=ID_FILE_SAVE
Filter=C
VirtualFilter=VWC

[MNU:IDR_IMAGEMENU]
Type=1
Class=?
Command1=ID_EDIT_COPY
CommandCount=1

[ACL:IDR_MAINFRAME (Japanese)]
Type=1
Class=?
Command1=ID_EDIT_SELECT_ALL
Command2=ID_EDIT_COPY
Command3=ID_FILE_SAVE
Command4=ID_EDIT_PASTE
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_NEXT_PANE
Command8=ID_PREV_PANE
Command9=ID_EDIT_COPY
Command10=ID_EDIT_PASTE
Command11=ID_EDIT_CUT
Command12=ID_EDIT_UNDO
CommandCount=12

[DLG:IDD_ABOUTBOX (Japanese)]
Type=1
Class=CAboutDlg
ControlCount=11
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308480
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342177283
Control6=IDOK,button,1342373889
Control7=IDC_STATIC,static,1342308481
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATICURL,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATICMAILTO,static,1342308352

[MNU:IDR_IMAGEMENU (Japanese)]
Type=1
Class=?
Command1=ID_EDIT_COPY
CommandCount=1

[DLG:IDD_WATERMARK]
Type=1
Class=CWatermarkDlg
ControlCount=12
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CHAOS,edit,1350631552
Control4=IDC_CHAR_WK,edit,1350631552
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=ID_QRIMAGE_PATH,edit,1350631552
Control9=IDC_QRIMAGE_PATH,button,1342242816
Control10=IDC_STATIC,static,1342308352
Control11=IDC_QRIMAGE_WM_PATH,edit,1350631552
Control12=IDC_QRIMAGE_WM,button,1342242816

[CLS:CWatermarkDlg]
Type=0
HeaderFile=WatermarkDlg.h
ImplementationFile=WatermarkDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_CHAOS
VirtualFilter=dWC

