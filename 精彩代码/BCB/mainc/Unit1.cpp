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
//asprintf����printf,��ʾ��Ϣ
void asprintf(AnsiString s){
    Form1->Label1->Caption = s;
}
void bsprintf(AnsiString s){
    Form1->Label3->Caption = s;
}

AnsiString instr=""; //���ڽ����������ֵ
void editClear(void){//�������Edit1���
    Form1->Edit1->Clear(); instr=""; bsprintf("");
}
void __fastcall TForm1::Button1Click(TObject *Sender){
    instr=Form1->Edit1->Text; //Edit1�������,�����������ݷŵ�instr��
}

int sum,xz,count=0,coin,num[50],i=1; //��Ȼ���˰�i��ȫ�ֱ���
int _password2,_number2;
int pass_word(int *p,int *q){//������
    int password1;
    srand((int)time(0));
    password1=rand()%(9999-1000+1)+1000;
    p[i]=password1;
    bsprintf("��������Ϊ:"+IntToStr(password1)+" ,������Ϊ:"+IntToStr(i++));
    return 0;
}

short vStep=1; //��ǰִ�е��ڼ���
void __fastcall TForm1::Timer1Timer(TObject *Sender)
{
    switch(vStep){
        case 1: //��1������
            asprintf("�������봢����������\n");
            if(instr!=""){//������
                sum=StrToInt(instr);//����sumΪ���������
                editClear(); //���������Ա���һ������
                vStep=2; //������2��
            }
            break;
        case 2: //��2������
            asprintf("��ѡ��������Ҫ�ķ���1�����2ȡ����0�˳�\n");
            if(instr!=""){//������
                xz=StrToInt(instr);//����
                if(xz==0)Close(); //�ر��˳�
                if(xz==1)vStep=3; //������3��:���
                if(xz==2)vStep=4; //������3��:ȡ��
                editClear(); //���������Ա���һ������
                if(xz!=1 && xz!=2)bsprintf("�Բ��𣬷�������δ�鵽��������ѡ��");
            }
            break;
        case 3://���
            if(count>=sum)asprintf("�Բ��𣬱�������\n");
            else asprintf("��Ͷ��Ӳ��\n");
            if(instr!=""){//������
                coin=StrToInt(instr);//����coin
                editClear(); //���������Ա���һ������
                if(coin==1){
                    pass_word(num,&i);
                    count++;
                    vStep=2; //������2��
                }
                else{
                    bsprintf("������Ͷ��\n");
                }
            }
            break;
        case 4://ȡ��
            asprintf("���������Ĵ�����\n");
            if(instr!=""){//������
                _number2=StrToInt(instr);//����
                editClear(); //���������Ա���һ������
                vStep=41; //������2��
            }
            break;
        case 41://ȡ������
            asprintf("��������������\n");
            if(instr!=""){//������
                _password2=StrToInt(instr);//����
                editClear(); //���������Ա���һ������
                if(num[_number2]==_password2){
                    bsprintf("��"+IntToStr(_number2)+"��������Ѵ򿪣���ȡ��������Ʒ\n");
                    count--;
                    vStep=2; //������2��
                }
                else{
                    bsprintf("�Բ���,���벻��ȷ,����������,���붪ʧ����ϵ����Ա\n");
                    vStep=4;
                }
            }
            break;
    }
    //trace
    Label2->Caption="����������:"+IntToStr(count)+"/"+IntToStr(sum);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
