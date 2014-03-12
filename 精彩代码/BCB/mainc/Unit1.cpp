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
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//asprintf代替printf,显示信息
void asprintf(AnsiString s){
    Form1->Label1->Caption = s;
}
void bsprintf(AnsiString s){
    Form1->Label3->Caption = s;
}

AnsiString instr=""; //用于接收输入的数值
void editClear(void){//把输入框Edit1清空
    Form1->Edit1->Clear(); instr=""; bsprintf("");
}
void __fastcall TForm1::Button1Click(TObject *Sender){
    instr=Form1->Edit1->Text; //Edit1是输入框,接受输入数据放到instr中
}

int sum,xz,count=0,coin,num[50],i=1; //居然有人把i当全局变量
int _password2,_number2;
int pass_word(int *p,int *q){//你的益达
    int password1;
    srand((int)time(0));
    password1=rand()%(9999-1000+1)+1000;
    p[i]=password1;
    bsprintf("您的密码为:"+IntToStr(password1)+" ,储物柜号为:"+IntToStr(i++));
    return 0;
}

short vStep=1; //当前执行到第几步
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    switch(vStep){
        case 1: //第1屏界面
            asprintf("请先输入储物箱总数：\n");
            if(instr!=""){//有输入
                sum=StrToInt(instr);//变量sum为输入框内容
                editClear(); //清空输入框以便下一次输入
                vStep=2; //跳到第2屏
            }
            break;
        case 2: //第2屏界面
            asprintf("请选择您所需要的服务：1存包；2取包；0退出\n");
            if(instr!=""){//有输入
                xz=StrToInt(instr);//变量
                if(xz==0)Close(); //关闭退出
                if(xz==1)vStep=3; //跳到第3屏:存包
                if(xz==2)vStep=4; //跳到第3屏:取包
                editClear(); //清空输入框以便下一次输入
                if(xz!=1 && xz!=2)bsprintf("对不起，服务请求未查到，请重新选择");
            }
            break;
        case 3://存包
            if(count>=sum)asprintf("对不起，本柜已满\n");
            else asprintf("请投入硬币\n");
            if(instr!=""){//有输入
                coin=StrToInt(instr);//变量coin
                editClear(); //清空输入框以便下一次输入
                if(coin==1){
                    pass_word(num,&i);
                    count++;
                    vStep=2; //跳到第2屏
                }
                else{
                    bsprintf("请重新投币\n");
                }
            }
            break;
        case 4://取包
            asprintf("请输入您的储物柜号\n");
            if(instr!=""){//有输入
                _number2=StrToInt(instr);//变量
                editClear(); //清空输入框以便下一次输入
                vStep=41; //跳到第2屏
            }
            break;
        case 41://取包后续
            asprintf("请输入您的密码\n");
            if(instr!=""){//有输入
                _password2=StrToInt(instr);//变量
                editClear(); //清空输入框以便下一次输入
                if(num[_number2]==_password2){
                    bsprintf("第"+IntToStr(_number2)+"个储物柜已打开，请取走您的物品\n");
                    count--;
                    vStep=2; //跳到第2屏
                }
                else{
                    bsprintf("对不起,密码不正确,请重新输入,密码丢失请联系管理员\n");
                    vStep=4;
                }
            }
            break;
    }
    //trace
    Label2->Caption="储物箱数量:"+IntToStr(count)+"/"+IntToStr(sum);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
