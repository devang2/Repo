//---------------------------------------------------------------------------
#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <FileCtrl.hpp>
#include <stdlib.h>
#include <ComCtrls.hpp>
#include <ToolWin.hpp> //itoa()
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TMemo *Memo1;
    TStaticText *StaticText1;
    TPanel *Panel1;
    TDriveComboBox *DriveComboBox1;
    TDirectoryListBox *DirectoryListBox1;
    TFileListBox *FileListBox1;
    TToolBar *ToolBar1;
    TButton *Button1;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
