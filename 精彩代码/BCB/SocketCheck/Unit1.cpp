//---------------------------------------------------------------------------
// Designed by himameng. 2007
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

//---------------------------------------------------------------------------
//客户端
//---------------------------------------------------------------------------
char locip[30];
void GetLocalIP(int t){//取本机地址; 可能有多个,0为第一个
	WSADATA wsaData;
	hostent *p;
	char s[128],*p2,da[128],r;
    AnsiString as;
	WSAStartup(MAKEWORD(1, 1), &wsaData);
    gethostname(s,128);;
	p = gethostbyname(s);
	p2 = inet_ntoa(*((in_addr *)&p->h_addr[t]));
    strcpy(locip,p2);//把本地IP装入本地IP中
	WSACleanup();
}

void __fastcall TForm1::Button1Click(TObject *Sender)
{   int i;
    Memo1->Clear();
    cs1->Close();
    cs1->Address=Edit1->Text;
    cs1->Port=StrToInt(Edit2->Text);
    cs1->Open();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{   int i;
    Memo2->Clear();
    ss1->Close();
    //ss1->Address=Edit1->Text;
    ss1->Port=StrToInt(Edit4->Text);
    ss1->Open();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{   int i;
    AnsiString s;
    cs1->Close();
    s="已经关闭";
    Memo1->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cs1Connect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="已经连接";
    Memo1->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cs1Disconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="连接断开";
    Memo1->Lines->Add(s);
}
char mbuf[200000];
void __fastcall TForm1::cs1Read(TObject *Sender, TCustomWinSocket *Socket)
{   char bb[10];
    int len;
    len=Socket->ReceiveLength();
    Socket->ReceiveBuf(mbuf,len); mbuf[len]=0;
    Label3->Caption = IntToStr(len);
    Memo1->Lines->Add(AnsiString(mbuf));
}
//---------------------------------------------------------------------------

void __fastcall TForm1::cs1Error(TObject *Sender, TCustomWinSocket *Socket,
      TErrorEvent ErrorEvent, int &ErrorCode)
{   ErrorCode=0;

}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender)
{   char bb[20];
    strcpy(bb,"hello,世界");
    cs1->Socket->SendBuf(bb,sizeof(bb));
}

//---------------------------------------------------------------------------
//服务端
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1ClientConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="客户端连接上了";
    Memo2->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1ClientDisconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="客户端连接断开了";
    Memo2->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1Listen(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="Listen";
    Memo2->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1Accept(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="Accept";
    Memo2->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1ClientRead(TObject *Sender,
      TCustomWinSocket *Socket)
{
    char bb[1000];
    int n;
    n=Socket->ReceiveLength();
    Socket->ReceiveBuf(bb,n); bb[n]=0;
    Memo2->Lines->Add(AnsiString(bb));
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button5Click(TObject *Sender)
{
    char bb[100];
    int count;
    strcpy(bb,"第[0]个客户端.");
    count=ss1->Socket->ActiveConnections;
    if(count>0)ss1->Socket->Connections[0]->SendBuf(bb,strlen(bb));
    Memo2->Lines->Add("客户端数量:"+IntToStr(count));
}


//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    int i;
    GetLocalIP(0);
    Edit1->Text = AnsiString(locip);
}
//---------------------------------------------------------------------------

