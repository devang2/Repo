// WatermarkDlg.cpp : implementation file
//

#include "stdafx.h"
#include "QR_Image.h"
#include "WatermarkDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatermarkDlg dialog


CWatermarkDlg::CWatermarkDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWatermarkDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWatermarkDlg)
	m_char_wk = _T("");
	m_qr_image = _T("");
	m_chaos = 0.01f;
	m_qr_image_wm = _T("");
	//}}AFX_DATA_INIT
}


void CWatermarkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWatermarkDlg)
	DDX_Text(pDX, IDC_CHAR_WK, m_char_wk);
	DDV_MaxChars(pDX, m_char_wk, 50);
	DDX_Text(pDX, ID_QRIMAGE_PATH, m_qr_image);
	DDX_Text(pDX, IDC_CHAOS, m_chaos);
	DDV_MinMaxFloat(pDX, m_chaos, 1.e-005f, 0.99999f);
	DDX_Text(pDX, IDC_QRIMAGE_WM_PATH, m_qr_image_wm);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWatermarkDlg, CDialog)
	//{{AFX_MSG_MAP(CWatermarkDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_QRIMAGE_PATH, OnQrimagePath)
	ON_BN_CLICKED(IDC_QRIMAGE_WM, OnQrimageWm)
	ON_EN_CHANGE(IDC_CHAR_WK, OnChangeCharWk)
	ON_EN_CHANGE(IDC_CHAOS, OnChangeChaos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatermarkDlg message handlers



void CWatermarkDlg::OnOk() 
{
	// TODO: Add your control notification handler code here
	if(m_qr_image.GetLength()==0)
	{
		MessageBox("请指定条码图像路径");
		return;
	}
	if(m_qr_image_wm.GetLength()==0)
	{
		MessageBox("请指定嵌入水印后的条码图像路径");
		return;
	}
	int i,j=0;
	int wm_byte_length=m_char_wk.GetLength();
	int wm_bit_length=wm_byte_length*8;
	unsigned char * wm_embed;
	wm_embed=(unsigned char *)malloc(wm_bit_length*sizeof(unsigned char));
	for(i=0;i<wm_byte_length;i++){
		unsigned char temp0,temp1,temp2,temp3,temp4,temp5,temp6,temp7;
		unsigned char q=m_char_wk.GetAt(i);
		temp0=q>>7;temp1=q>>6;temp1=temp1&0x01;
		temp2=q>>5;temp2=temp2&0x01;temp3=q>>4;temp3=temp3&0x01;
		temp4=q>>3;temp4=temp4&0x01;temp5=q>>2;temp5=temp5&0x01;
		temp6=q>>1;temp6=temp6&0x01;temp7=q&0x01;
		wm_embed[j]=temp0;j++;
		wm_embed[j]=temp1;j++;
		wm_embed[j]=temp2;j++;
		wm_embed[j]=temp3;j++;
		wm_embed[j]=temp4;j++;
		wm_embed[j]=temp5;j++;
		wm_embed[j]=temp6;j++;
		wm_embed[j]=temp7;j++;
	}
	
	char bmpfile[100];
	memset(bmpfile,0,100);
	strcpy(bmpfile, m_qr_image.GetBuffer(m_qr_image.GetLength()));
	FILE * fp;
	fp=fopen(bmpfile,"rb");
	fseek(fp,0,0);
	unsigned char image_head[62];
	fread(image_head,62,1,fp);
	int image_byte_length=image_head[34]+image_head[35]*256;
	int image_width=image_head[18]+image_head[19]*256;
	int image_height=image_head[22]+image_head[23]*256;
	unsigned char *pBmpBuf;
	pBmpBuf=(unsigned char *)malloc(image_byte_length*sizeof(unsigned char));
	fseek(fp,62,0);
    fread(pBmpBuf,image_byte_length,1,fp); 
	fclose(fp);
	float kx=m_chaos;
	BOOL * T;
	T=(BOOL *)malloc(image_byte_length*sizeof(BOOL));
	for(i=0;i<image_byte_length;i++)
	     T[i]=false;
	j=0;
	while(j<wm_bit_length){
		kx=4*kx*(1-kx);
		int k=(int)(image_byte_length*kx);
		if(T[k]==false){
			pBmpBuf[k]=(pBmpBuf[k]&0xfe)+wm_embed[j];//LSB算法
			T[k]=true;
			j++;
		}
		
	}
	memset(bmpfile,0,100);
	strcpy(bmpfile, m_qr_image_wm.GetBuffer(m_qr_image_wm.GetLength()));
	fp=fopen(bmpfile,"wb");
	fwrite(image_head,62,1,fp); 
    fwrite(pBmpBuf,image_byte_length,1,fp);
	fclose(fp);
	MessageBox("水印已嵌入");
	CDialog::OnOK();
}

void CWatermarkDlg::OnQrimagePath() 
{
	// TODO: Add your control notification handler code here
	CFileDialog MyDlg(TRUE, NULL,"*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "*.bmp", NULL );
	if (MyDlg.DoModal()==IDOK){
		m_qr_image=MyDlg.GetPathName();
	}
	UpdateData(FALSE);
}

void CWatermarkDlg::OnQrimageWm() 
{
	// TODO: Add your control notification handler code here
	CFileDialog MyDlg(FALSE, NULL,"*.bmp",OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "*.bmp", NULL );
	if (MyDlg.DoModal()==IDOK){
		m_qr_image_wm=MyDlg.GetPathName();
	}
	UpdateData(FALSE);
}

void CWatermarkDlg::OnChangeCharWk() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CWatermarkDlg::OnChangeChaos() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}
