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
    if(ADOTable1->Active==false){msg("���ݿⶼû�򿪶���?");return;}
    s=ADOTable1->FieldByName("�ֶ�5")->AsString;
    msg(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{   AnsiString s;
    s="12345";
    if(ADOTable1->Active==false){msg("���ݿⶼû��д��?");return;}
    ADOTable1->Edit();edited=1;//�༭ģʽ
    ADOTable1->FieldByName("�ֶ�5")->Text =s;
    msg("д12345�����ݿ�");
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button4Click(TObject *Sender)
{
    if(ADOTable1->Active==false){msg("���ݿⶼû�򿪱���ɶ?");return;}
    if(edited)ADOTable1->Post();//�����޸��򱣴�
    ADOTable1->Close();//�ر�
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
    if(ADOTable1->Active==false){msg("���ݿⶼû�򿪲�ѯɶ?");return;}

    ADOTable1->Active=false;
    ADOTable1->Filter="�ֶ�1='"+Edit1->Text+"'";
    ADOTable1->Active=true;
    ADOTable1->Filtered=true;
}
//---------------------------------------------------------------------------

