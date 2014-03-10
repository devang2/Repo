//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "cspin.h"
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *Button1;
	TMemo *memo;
	TImage *Img;
	TCSpinEdit *ScaleXY;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TCheckBox *chkAutoVersion;
	TComboBox *cboCorrectLevel;
	TCSpinEdit *spinModal;
	TComboBox *cboVersion;
	TComboBox *cboMaskingNo;
	TLabel *lbl_Pixcels;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ScaleXYChange(TObject *Sender);
	void __fastcall memoChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
