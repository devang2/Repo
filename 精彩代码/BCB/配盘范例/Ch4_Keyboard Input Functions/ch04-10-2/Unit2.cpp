//---------------------------------------------------------------------------
//目的：将默认关闭功能<Alt+F4>组合键失效
//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCloseQuery(TObject *Sender, bool &CanClose)
{//将默认关闭功能<Alt+F4>组合键失效
CanClose = !(::GetKeyState(VK_MENU) && ::GetKeyState(VK_F4));
}
//---------------------------------------------------------------------------
