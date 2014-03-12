//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ScktComp.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
    TClientSocket *cs1;
    TServerSocket *ss1;
    TLabel *Label1;
    TLabel *Label2;
    TEdit *Edit1;
    TEdit *Edit2;
    TMemo *Memo1;
    TButton *Button1;
    TEdit *Edit4;
    TMemo *Memo2;
    TButton *Button2;
    TButton *Button3;
    TImage *Image1;
    TLabel *Label3;
    TButton *Button4;
    TLabel *Label4;
    TButton *Button5;
    TLabel *Label5;
    TPanel *Panel1;
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall cs1Connect(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall cs1Disconnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall ss1ClientConnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall ss1ClientDisconnect(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall ss1Listen(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall ss1Accept(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall cs1Read(TObject *Sender, TCustomWinSocket *Socket);
    void __fastcall cs1Error(TObject *Sender, TCustomWinSocket *Socket,
          TErrorEvent ErrorEvent, int &ErrorCode);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall ss1ClientRead(TObject *Sender,
          TCustomWinSocket *Socket);
    void __fastcall Button5Click(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
