//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <mmsystem.h>

#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
TForm1 *Form1;
void MyLoadIcon(char *iconame);
int playsoundto(short n,int start,int end);
int getsoundlen(short n);
void volumedec(short n);
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
    this->DoubleBuffered = true;
}

//---------------------------------------------------------------------------
// ȫ�ֺ��������
//---------------------------------------------------------------------------
extern "C" char read_png(char *file_name, unsigned char *mem, int *wid,int *hei);
//---------------------------------------------------------------------------
#define uchar unsigned char
//���ǽ���PNG����Ļ
WNDCLASSA Wnd ;
HWND Hnd; //��ʾ���
HDC  hdc; //ͼ�����
TForm *ComponentForm ;
char LayeredName[20];
struct STRARGB{uchar a,r,g,b;}apx;
uchar *pngbuf;
Graphics::TBitmap *bmpStand,*bmpAttack,*bmpPaper ; //װpngͼ�Ļ�����
HRGN RegA; //Form1�Ͽؼ�ռ�������
BLENDFUNCTION blend ; //Form͸������
int scrw,scrh;
//---------------------------------------------------------------------------
char v_ton=0,v_show=0;
char v_cur=0;
unsigned int v_tCur,v_tStand,v_tAttack,v_alarm=0;
//---------------------------------------------------------------------------
void Frm2Show(void){char st[20];
    playsoundto(0, 0,getsoundlen(0));
    //Form2->Left = scrw-350;
    //Form2->Top = scrh-470;
    //Form2->Show();
    v_ton=2;
    v_cur=0;           
}

LRESULT WndAProc(HWND Wnd, UINT uMsg, WPARAM wPar, LPARAM lPar)
{
	RECT Rect ;
	switch(uMsg){
		case WM_DESTROY:PostQuitMessage(0);break;
		case WM_LBUTTONDOWN:SendMessage(Wnd, WM_SYSCOMMAND, SC_MOVE+2, 0);break;
		case WM_MBUTTONUP:  Form1->Close(); break;
		case WM_RBUTTONUP:	Frm2Show();	break;
		case WM_MOVING:
		case WM_MOVE:if(GetWindowRect(Wnd, &Rect)){SetWindowPos(Form1->Handle, 0, Rect.left, Rect.top, 0, 0, SWP_NOSIZE); }break;
		default:break;
	}
	return DefWindowProc(Wnd, uMsg, wPar, lPar);
}
extern LRESULT WndBProc(HWND Wnd, UINT uMsg, WPARAM wPar, LPARAM lPar);
//--------------------------------
void LoadAPNG(char *pngname,Graphics::TBitmap *bmp,HRGN rgn)
{//��һ��png�ŵ�bmp��
    int w,h,x,y,a,r,g,b,n,p=0;
	PRGBQuad Linia32 ;

    //����װ��png����
    pngbuf=new uchar [2200*300*4];
    read_png(pngname,pngbuf,&w,&h);
	bmp->Width = w;
	bmp->Height = h;
	bmp->PixelFormat = pf32bit;

    for(y=0;y<h;y++){
        Linia32 = (PRGBQuad)bmp->ScanLine[y];//ֱ��дͼ�ķ�ʽ
        for(x=0;x<w;x++){
            memcpy(&apx,&pngbuf[p],4);p+=4;
            if(apx.a==0)a=0; else a=apx.a;
            if( PtInRegion(rgn, x, y))a=0;//���пؼ��ĵط��ճ���
            if(a==0)memset(&apx,0,4);
			Linia32->rgbRed   = apx.r;
			Linia32->rgbGreen = apx.g;
			Linia32->rgbBlue  = apx.b;
			Linia32->rgbReserved = a;
			Linia32++;
        }
    }
    delete[] pngbuf;
}
//-----FormA��Form��,pngname��png��
HWND CreateLayered(TForm *FormA, HRGN rgn)
{
	HRGN ComponentRegion ;
    HWND hw;
	ComponentForm = FormA;
    ComponentForm->BorderStyle = bsNone;

	int iCompCount = ComponentForm->ControlCount;
    for(int i = 0 ;i<iCompCount;i++){
    	if( ComponentForm->Controls[i]->Visible){
    		ComponentRegion = CreateRectRgn(ComponentForm->Controls[i]->Left,
											ComponentForm->Controls[i]->Top,
	    									ComponentForm->Controls[i]->Width+ComponentForm->Controls[i]->Left,
		    								ComponentForm->Controls[i]->Height+ComponentForm->Controls[i]->Top);
    		CombineRgn(rgn, rgn, ComponentRegion, RGN_OR);
			DeleteObject(ComponentRegion);
    	}
    }

    Wnd.lpfnWndProc =(WNDPROC)WndAProc;
    strcpy(LayeredName,"WndAClass");
    Wnd.lpszClassName = LayeredName;
	Wnd.hInstance = HInstance;
	Wnd.hbrBackground = (HBRUSH)COLOR_WINDOW;
    Wnd.hCursor = LoadCursor(NULL,IDC_ARROW);//���ﲻ��������ʾ©��
	RegisterClass(&Wnd);

    hw = CreateWindowEx(WS_EX_LAYERED, LayeredName,
    					 ComponentForm->Caption.c_str(),
						 WS_POPUP|WS_VISIBLE, //WS_POPUP|
	    				 ComponentForm->Left,
		    			 ComponentForm->Top,
			    		 ComponentForm->Width,
				    	 ComponentForm->Height,
					     ComponentForm->Handle,
    					 0,
						 HInstance,
	    				 NULL);
    hdc=GetDC(0);
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = 255;
	//UpdateLayeredWindow(hw,hdc,NULL,&S,bmp->Canvas->Handle,&P,0,&blend,ULW_ALPHA);
    //��From����
	SetWindowRgn(ComponentForm->Handle, rgn, TRUE);
	//DeleteObject(Region);
    return(hw);
}
void ReleaseAll(void){
    DeleteObject(RegA);
    delete bmpStand;
    delete bmpAttack;
}
//----------------
void UpdateFrame(int n, int w,int h,Graphics::TBitmap *tbmp){//��ʾ��n/t֡
    TPoint P ;
	TSize S ;
    int x,j;
    j=v_cur; x=j*w;
	P = Point(x, 0);
    S.cx = w; S.cy = h; //137,156
    UpdateLayeredWindow(Hnd,hdc,NULL,&S,tbmp->Canvas->Handle,&P,0,&blend,ULW_ALPHA);
	SetWindowRgn(Form1->Handle, RegA, TRUE);
}
//----------------
#include <dos.h>
struct FILET{int ymd,hms;}vft,vtt;
void GetCurTime(void){//�õ���ǰʱ��
    struct date d; struct time t; int yy,mo,dd, hh,mm,ss;
    getdate(&d); yy=d.da_year;mo=d.da_mon;dd=d.da_day;
    gettime(&t); hh=t.ti_hour;mm=t.ti_min;ss=t.ti_sec;  //v=h*100+m;//ʱ��
    vtt.ymd=yy*10000+(mo*100)+dd; vtt.hms=(hh*10000)+(mm*100)+ss;
}
void msw(AnsiString s){Form1->Label1->Caption = s;}
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    char st[20];
    int a,b;
    v_tCur=GetTickCount();
    if(v_show==0){ShowWindow(Application->Handle, SW_HIDE);v_show=1;}
    if(v_ton==1){
        if((v_tCur-v_tStand)>100){//վ��֡ʱ��
            UpdateFrame(v_cur++, 137,156, bmpStand);
            if(v_cur>=10)v_cur=0; //��ǰ֡<��֡��
            v_tStand=v_tCur; //ʱ���¼
        }
    }
    else if(v_ton==2){
        if((v_tCur-v_tAttack)>70){//����֡ʱ��
            UpdateFrame(v_cur++, 146,180, bmpAttack);
            if(v_cur>=15){v_ton=1; v_cur=0;} //����ֻ��һ�غ�
            v_tAttack=v_tCur; //ʱ���¼
        }
    }

    //Alarm
    GetCurTime();
    sprintf(st,"Cur:%d,on:%d.", vtt.ymd, vtt.hms);
    msw(st);
    if(v_alarm==0 && vtt.hms==203001){playsoundto(1, 0,getsoundlen(1));v_alarm=1; }

}

//---------------------------------------------------------------------------
AnsiString AliasName[3],curdir;
//�õ����ļ���
int GetShortName(char *sLongName,char *sShortName){//ת��Ϊ����
    int  nShortNameLen;
    memset(sShortName,0,256);
    nShortNameLen =GetShortPathName(sLongName,sShortName,255 );
    if(nShortNameLen==0)return -1;
    return 0;
}
int opensound(AnsiString filename,short n) //���ļ�
{
  char bb[10],name[256];
  AnsiString cmd,ShortPathAndFile,typeDevice;
  int dwReturn;
  typeDevice="MPEGVideo";
  if(GetShortName(filename.c_str(),name)!=0)return (-1);
  ShortPathAndFile=AnsiString(name);
  cmd = "open "+ShortPathAndFile+" type "+typeDevice+" Alias "+AliasName[n];
  dwReturn = mciSendString(cmd.c_str(), NULL, 0, 0);
  return dwReturn;
}
int playsoundto(short n,int start,int end) //�����ļ�
{
  char bb[10];
  AnsiString cmd;
  int dwReturn;
  cmd = "play "+AliasName[n] + " from " + IntToStr(start) + " to " + IntToStr(end);
  dwReturn = mciSendString(cmd.c_str(), NULL, 0, 0);//����
  return dwReturn;
}
int getsoundlen(short n){
    char sPosition[256];
    AnsiString cmd;
    long lLength;
    cmd="Status "+AliasName[n]+" length";
    mciSendString(cmd.c_str(), sPosition, 255,0);
    lLength=strtol(sPosition, NULL, 10);
    return(lLength);
}
void volumedec(short n){//0-1000
    char chVolume[256]; long lVolume;
    TCHAR cmd[256],volume[256];
    wsprintf(cmd,"status Voice%d volume",n);
    mciSendString(cmd,volume, sizeof(volume), 0 );
    int v=atoi(volume);//���ַ�������תΪInternet�ͣ�Ҫinclude<stdlib.h>
    v-=500; if(v<0)v=0;
    wsprintf(cmd,"setaudio Voice%d volume to %i",n,v);//�޸�����
    mciSendString(cmd,"",0,NULL);
}
int closesound(short n) //�ر��ļ�
{
  char bb[10];
  AnsiString cmd;
  int dwReturn;
  cmd = "Close "+AliasName[n];
  dwReturn = mciSendString(cmd.c_str(), NULL, 0, 0);
  return dwReturn;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    int n,w,h;
    //��ַ
    for(n=0;n<3;n++)AliasName[n]="Voice"+IntToStr(n);
    curdir=ExtractFilePath(Application->ExeName);
    opensound("��.mp3",0);
    opensound("���ڷ�.mp3",1);
    volumedec(1);
    //�õ�������
    scrw =w= GetSystemMetrics(SM_CXSCREEN);
    scrh =h= GetSystemMetrics(SM_CYSCREEN);
    Form1->Left = w-190;
    Form1->Top  = h-190;
    //װ��ͼ
    bmpStand = new Graphics::TBitmap();//����
    bmpAttack = new Graphics::TBitmap();//����
    bmpPaper = new Graphics::TBitmap();//����
    RegA = CreateRectRgn(0, 0, 0, 0);
    //��PNG����
    Hnd=CreateLayered(Form1,RegA);
    LoadAPNG("jtStand.png", bmpStand,RegA);
    LoadAPNG("jtAttack.png",bmpAttack,RegA);
    LoadAPNG("jtPaper.png",bmpPaper,RegA);
    //���ڶ���
    v_ton=1;
    v_cur=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::open1Click(TObject *Sender)
{   char st[20];
    v_ton=2;
    v_cur=0;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::exit1Click(TObject *Sender)
{   char st[20];
    ReleaseAll();
    Form1->Close();
}
//---------------------------------------------------------------------------



