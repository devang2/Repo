//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
    : TForm(Owner)
{
}
void msg(AnsiString s){ Form1->Memo1->Lines->Add(s); }
//---------------------------------------------------------------------------
char edited=0;
void __fastcall TForm1::Button1Click(TObject *Sender)
{
"Provider=Microsoft.Jet.OLEDB.4.0;Data Source=psw.mdb;Persist Security Info=False;Jet OLEDB:Database Password=12345";
    AnsiString s;
    s="Provider=Microsoft.Jet.OLEDB.4.0;Data Source=Database1.mdb;Persist Security Info=False";
    ADOTable1->Close();
    ADOTable1->Filter="";
    ADOTable1->ConnectionString=s;
    ADOTable1->TableName ="Table1";     
    ADOTable1->Open();
    msg("ok");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{   AnsiString s;
    if(ADOTable1->Active==false){msg("数据库都没打开读哪?");return;}
    s=ADOTable1->FieldByName("字段5")->AsString;
    msg(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{   AnsiString s;
    s="12345";
    if(ADOTable1->Active==false){msg("数据库都没打开写哪?");return;}
    ADOTable1->Edit();edited=1;//编辑模式
    ADOTable1->FieldByName("字段5")->Text =s;
    msg("写12345到数据库");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
    if(ADOTable1->Active==false){msg("数据库都没打开保存啥?");return;}
    if(edited)ADOTable1->Post();//如有修改则保存
    ADOTable1->Close();//关闭
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
    if(ADOTable1->Active==false){msg("数据库都没打开查询啥?");return;}

    ADOTable1->Active=false;
    ADOTable1->Filter="字段1='"+Edit1->Text+"'";
    ADOTable1->Active=true;
    ADOTable1->Filtered=true;
}
//---------------------------------------------------------------------------

