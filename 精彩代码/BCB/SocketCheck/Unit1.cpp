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
//�ͻ���
//---------------------------------------------------------------------------
char locip[30];
void GetLocalIP(int t){//ȡ������ַ; �����ж��,0Ϊ��һ��
	WSADATA wsaData;
	hostent *p;
	char s[128],*p2,da[128],r;
    AnsiString as;
	WSAStartup(MAKEWORD(1, 1), &wsaData);
    gethostname(s,128);;
	p = gethostbyname(s);
	p2 = inet_ntoa(*((in_addr *)&p->h_addr[t]));
    strcpy(locip,p2);//�ѱ���IPװ�뱾��IP��
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
    s="�Ѿ��ر�";
    Memo1->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cs1Connect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="�Ѿ�����";
    Memo1->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::cs1Disconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="���ӶϿ�";
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
    strcpy(bb,"hello,����");
    cs1->Socket->SendBuf(bb,sizeof(bb));
}

//---------------------------------------------------------------------------
//�����
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1ClientConnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="�ͻ�����������";
    Memo2->Lines->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ss1ClientDisconnect(TObject *Sender,
      TCustomWinSocket *Socket)
{
    AnsiString s;
    s="�ͻ������ӶϿ���";
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
    strcpy(bb,"��[0]���ͻ���.");
    count=ss1->Socket->ActiveConnections;
    if(count>0)ss1->Socket->Connections[0]->SendBuf(bb,strlen(bb));
    Memo2->Lines->Add("�ͻ�������:"+IntToStr(count));
}


//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate(TObject *Sender)
{
    int i;
    GetLocalIP(0);
    Edit1->Text = AnsiString(locip);
}
//---------------------------------------------------------------------------

