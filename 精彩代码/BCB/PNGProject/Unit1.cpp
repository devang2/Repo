//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdio.h>
#include <shellapi.hpp>

#pragma hdrstop

#include "Unit1.h"
#include "Unit2.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
TForm1 *Form1;
void MyLoadIcon(char *iconame);
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
    this->DoubleBuffered = true;
}

//---------------------------------------------------------------------------
// 全局函数与变量
//---------------------------------------------------------------------------
extern "C" char read_png(char *file_name, unsigned char *mem, int *wid,int *hei);
//---------------------------------------------------------------------------
#define uchar unsigned char
//这是建立PNG的屏幕
WNDCLASSA Wnd ;
HWND Hnd; //显示句柄
HDC  hdc; //图画句柄
TForm *ComponentForm ;
char LayeredName[20];
struct STRARGB{uchar a,r,g,b;}apx;
uchar *pngbuf;
Graphics::TBitmap *bmpStand,*bmpAttack,*bmpPaper ; //装png图的缓冲区
HRGN RegA; //Form1上控件占领的区域
BLENDFUNCTION blend ; //Form透明参数
int scrw,scrh;
//---------------------------------------------------------------------------
char v_ton=0,v_show=0;
char v_cur=0;
unsigned int v_tCur,v_tStand,v_tAttack;
//---------------------------------------------------------------------------
void Frm2Show(void){char st[20];
    Form2->Left = scrw-350;
    Form2->Top = scrh-470;
    Form2->Show();
    
    v_ton=2;
    v_cur=0;           
}
LRESULT WndAProc(HWND Wnd, UINT uMsg, WPARAM wPar, LPARAM lPar)
{
	RECT Rect ;
	switch(uMsg){
		case  WM_DESTROY:{
			PostQuitMessage(0);
			break;
		}
		case  WM_LBUTTONDOWN:{
			SendMessage(Wnd, WM_SYSCOMMAND, SC_MOVE+2, 0);
			break;
		}
		case  WM_RBUTTONUP:{
			Frm2Show();
			break;
		}
		case WM_MOVING:
		case WM_MOVE:{
			if (GetWindowRect(Wnd, &Rect)){
			 	SetWindowPos(Form1->Handle, 0, Rect.left, Rect.top, 0, 0, SWP_NOSIZE);
			}
			break;
		}
		default:{
        	break;
		}
	}
	return DefWindowProc(Wnd, uMsg, wPar, lPar);
}
extern LRESULT WndBProc(HWND Wnd, UINT uMsg, WPARAM wPar, LPARAM lPar);
//--------------------------------
void LoadAPNG(char *pngname,Graphics::TBitmap *bmp,HRGN rgn)
{//读一个png放到bmp中
    int w,h,x,y,a,r,g,b,n,p=0;
	PRGBQuad Linia32 ;

    //这里装入png数据
    pngbuf=new uchar [2200*300*4];
    read_png(pngname,pngbuf,&w,&h);
	bmp->Width = w;
	bmp->Height = h;
	bmp->PixelFormat = pf32bit;

    for(y=0;y<h;y++){
        Linia32 = (PRGBQuad)bmp->ScanLine[y];//直接写图的方式
        for(x=0;x<w;x++){
            memcpy(&apx,&pngbuf[p],4);p+=4;
            if(apx.a==0)a=0; else a=apx.a;
            if( PtInRegion(rgn, x, y))a=0;//把有控件的地方空出来
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
//-----FormA是Form名,pngname是png名
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
    Wnd.hCursor = LoadCursor(NULL,IDC_ARROW);//这里不加则总显示漏斗
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
    //设From区域
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
void UpdateFrame(int n, int w,int h,Graphics::TBitmap *tbmp){//显示第n/t帧
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
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    char st[20];
    int a,b;
    v_tCur=GetTickCount();
    if(v_show==0){ShowWindow(Application->Handle, SW_HIDE);v_show=1;}
    if(v_ton==1){
        if((v_tCur-v_tStand)>100){//站立帧时间
            UpdateFrame(v_cur++, 137,156, bmpStand);
            if(v_cur>=10)v_cur=0; //当前帧<总帧数
            v_tStand=v_tCur; //时间记录
        }
    }
    else if(v_ton==2){
        if((v_tCur-v_tAttack)>70){//攻击帧时间
            UpdateFrame(v_cur++, 146,180, bmpAttack);
            if(v_cur>=15){v_ton=1; v_cur=0;} //攻击只有一回合
            v_tAttack=v_tCur; //时间记录
        }
    }
    //sprintf(st,"Cur:%d,on:%d", v_cur,v_ton);
    //msw(st);
}


//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    int w,h;
    scrw =w= GetSystemMetrics(SM_CXSCREEN);
    scrh =h= GetSystemMetrics(SM_CYSCREEN);
    Form1->Left = w-190;
    Form1->Top  = h-190;

    bmpStand = new Graphics::TBitmap();//分配
    bmpAttack = new Graphics::TBitmap();//分配
    bmpPaper = new Graphics::TBitmap();//分配
    RegA = CreateRectRgn(0, 0, 0, 0);

    Hnd=CreateLayered(Form1,RegA);
    LoadAPNG("jtStand.png", bmpStand,RegA);
    LoadAPNG("jtAttack.png",bmpAttack,RegA);
    LoadAPNG("jtPaper.png",bmpPaper,RegA);
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

