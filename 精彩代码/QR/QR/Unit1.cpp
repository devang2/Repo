//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
#include "QR_Encode.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
	//擦除屏幕
	Img->Canvas->Brush->Color = clWhite;
	Img->Canvas->Rectangle(0,0,Img->Width,Img->Height);
	//一个点占N个象素宽/高
	int OnePoint_NPixels = ScaleXY->Value;
	TQR_Encode *pQR_Encode = new TQR_Encode();
	int m_bDataEncoded = 0;
	try
	{
	m_bDataEncoded = pQR_Encode->EncodeData(cboCorrectLevel->Items->IndexOf(cboCorrectLevel->Text),
					   cboVersion->Items->IndexOf(cboVersion->Text),
					   chkAutoVersion->Checked,
					   cboMaskingNo->Items->IndexOf(cboMaskingNo->Text)-1,
					   memo->Text.t_str());
	}
	catch(...)
    {}
	if(m_bDataEncoded)
	{
		lbl_Pixcels->Caption = IntToStr(pQR_Encode->m_nSymbleSize*OnePoint_NPixels)+" x " +IntToStr(pQR_Encode->m_nSymbleSize*OnePoint_NPixels);
		//点阵绘图
		for (int i = 0; i < pQR_Encode->m_nSymbleSize; ++i)
		{
			for (int j = 0; j < pQR_Encode->m_nSymbleSize; ++j)
			{
				if (pQR_Encode->m_byModuleData[i][j])
				{   //根据点的放大倍数描点
					for(int k=0;k<OnePoint_NPixels;k++)
					{
						for(int m=0;m<OnePoint_NPixels;m++)
						{
							Img->Canvas->Pixels[i*OnePoint_NPixels+k][j*OnePoint_NPixels+m]=clRed;
						}
					}
				}
			}
		}
	}
	else
	{
		lbl_Pixcels->Caption = "";
	}
	delete pQR_Encode;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::ScaleXYChange(TObject *Sender)
{
	Button1Click(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::memoChange(TObject *Sender)
{
	Button1Click(Sender);
}
//---------------------------------------------------------------------------
