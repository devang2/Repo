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
{   this->DoubleBuffered = true;
}
//---------------------------------------------------------------------------
#include <math.h>
char start=0;
float ctime,ofx,ofy,bottom;
//---------------------------------------------------------------------------
void pix(int x,int y){
    if(x<0||y<0)return;
    Form1->Image1->Canvas->Pixels[x][y]=clBlack;
}
void line(int x1,int y1,int x2,int y2,unsigned int c){
    Form1->Image1->Canvas->Pen->Color=(TColor)c;
    Form1->Image1->Canvas->MoveTo(x1,y1);
    Form1->Image1->Canvas->LineTo(x2,y2);
}
void Arrow(int x,int y,float a){
    float fx[6],fy[6], rx[6],ry[6], nx,ny;
    int i;
    fx[0]=-15; fy[0]=5;  //箭簇
    fx[1]=-10; fy[1]=0;
    fx[2]=-15; fy[2]=-5;

    fx[3]=5;  fy[3]=5;  //箭头
    fx[4]=10; fy[4]=0;
    fx[5]=5;  fy[5]=-5;

    for(i=0;i<6;i++){
        nx=fx[i]*cos(a)+fy[i]*sin(a)+x;
        ny=fy[i]*cos(a)-fx[i]*sin(a)+y;
        rx[i]=nx; ry[i]=ny;
    }

    line(rx[0],ry[0], rx[1],ry[1], 0);
    line(rx[2],ry[2], rx[1],ry[1], 0);
    line(rx[3],ry[3], rx[4],ry[4], 0);
    line(rx[5],ry[5], rx[4],ry[4], 0);
    line(rx[1],ry[1], rx[4],ry[4], 0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    int v;
    //清屏
    Image1->Canvas->Pen->Mode = pmCopy;
    Image1->Canvas->Rectangle(0,0,Image1->Width,Image1->Height);
    Image1->Canvas->Pen->Mode = pmNotXor;
    //画坐标原点
    ofx=100; ofy=200; //坐标点
    line(ofx,0, ofx,Image1->Height, RGB(0,0,255));//y轴
    line(0,ofy, Image1->Width,ofy, RGB(0,0,255));//x轴
    Image1->Canvas->TextOutA(ofx+5,ofy+5,"原点(0,0)"); 
    //画地平一
    v=StrToInt(Edit3->Text); bottom=v;
    line(ofx,v, Image1->Width,v, RGB(255,0,0));
    Image1->Canvas->TextOutA(ofx+5,v+5,"地平线"); 
}

//---------------------------------------------------------------------------
void DrawCurve(float v,float angle) //x,y抛物线公式,根据计算机显示进行了修改
{
    float alpha,x,y,g;
    g=9.8; //重力加速度
    alpha=angle*3.14/180; //把180度转成3.14
    x=v*cos(alpha)*ctime; //水平位移=力度*cos角度*t(x坐标)
    y=v*sin(alpha)*ctime-(g*ctime*ctime/2);  //力度*sin角度*t-1/2重力加速度*t^2（y坐标）
    x=x+ofx; y=ofy-y;
    pix(x,y);
    if(y>bottom){start=0;} //设定地平线在300位置
}
//---------------------------------------------------------------------------
float ax,ay,aa; //保留旧的位置以便计算角度
void DrawArrow(float v,float angle) //x,y抛物线公式,根据计算机显示进行了修改
{
    float alpha,x,y,g,a;
    g=9.8; //重力加速度
    alpha=angle*3.14/180; //把180度转成3.14
    x=v*cos(alpha)*ctime; //水平位移=力度*cos角度*t(x坐标)
    y=v*sin(alpha)*ctime-(g*ctime*ctime/2);  //力度*sin角度*t-1/2重力加速度*t^2（y坐标）
    x=x+ofx; y=ofy-y;
    if(x!=ax){a=-atan((y-ay)/(x-ax)); } else a=0;
    if(ctime>0){
        if(ax!=ofx)Arrow(ax,ay, aa);//搽掉前面画的
        Arrow(x,y, a); //画箭
    }
    ax=x; ay=y; aa=a;
    if(y>bottom){start=0;} //设定地平线在300位置
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
    //计时器开始
    ctime=0; start=1;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
    //计时器开始
    ctime=0; start=2;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    float v,a;
    if(start>0){
        v=StrToFloat(Edit1->Text);  //力度
        a=StrToFloat(Edit2->Text);  //角度
        if(a<0||a>90){start=0; ShowMessage("只能在0-90度之间射箭,要想朝左你将程序中的x变成-x"); }
    }
    if(start==1){
        DrawCurve(v,a);
        ctime+=0.1; //时间增量,加大可以让箭更快
    }
    if(start==2){
        DrawArrow(v,a);
        ctime+=0.1; //时间增量,加大可以让箭更快
    }
}
//---------------------------------------------------------------------------
