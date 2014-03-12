//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void msg(AnsiString s){Form2->Memo1->Lines->Add(s);  }

void __fastcall TForm2::Button1Click(TObject *Sender)
{
    int w,h;
    AnsiString s;

    w = GetSystemMetrics(SM_CXSCREEN);
    h = GetSystemMetrics(SM_CYSCREEN);

    s="w:"+IntToStr(w)+",h:"+IntToStr(h);
    msg(s);
}
//---------------------------------------------------------------------------

void __fastcall TForm2::Button2Click(TObject *Sender)
{
    Form1->Close();
}
//---------------------------------------------------------------------------

