/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neo.coderlife.net
   ckerhome@yahoo.com.cn

   ����һ���� Borland Graphics ����ֲ������С��Ϸ������˹���顱
*/

#define NEO_hi_graphics_h_used /*������Borland Graphics�Ľӿڼ���*/
#define NEO_sys_pal_used       /*ʹ��NEO��ϵͳ��ɫ��*/
#include "neo.h"

#define MINBOXSIZE 18  /* ��С����ĳߴ� */
#define BGCOLOR     6  /* ������ɫ */
#define GX  200
#define GY  10
#define LEVEL 2500    /* ÿ����Ҵ�5000�ֵȼ���һ���ٶ�*/

/* ������*/
#define VK_SPACE  32

/* �������˹����ķ��򣨶���Ϊ4�֣�*/
#define F_DONG 0
#define F_NAN  1
#define F_XI   2
#define F_BEI  3

#define NEXTCOL 20  /* Ҫ������һ�������������*/
#define NEXTROW 12  /* Ҫ������һ������ĺ�ӱ�*/
#define MAXROW 14   /* ��Ϸ��Ļ��С*/
#define MAXCOL 20
#define SCCOL 50    /*��Ϸ��Ļ����ʾ���ϵ����λ��*/
#define SCROW 30

#define X_Z    0   /* ���巽�����״�������֣�*/
#define X_REZ  1
#define X_7    2
#define X_RE7  3
#define X_I    4
#define X_REI  5
#define X_TIAN 6
#define X_T    7


int gril[22][16]; /* ��Ϸ��Ļ����*/
int col=1,row=7;  /* ��ǰ����ĺ�������*/
int boxfx=0,boxgs=0; /* ��ǰ�¿����׳�ͷ���*/
int  nextboxfx,nextboxgs,maxcol=22, nextboxco = 0;/*��һ���������״�ͷ�����ɫ*/
int minboxcolor,nextminboxcolor;
long num=0; /*��Ϸ��*/
int level=0,gamelevel[10]={18,16,14,12,10,8,6,4,2,1};/* ��Ϸ�ȼ�*/
char box_style = FALSE; /*��ɫ���*/
struct all{
   int x,y;
   int fangxiang;
   int gesu;
   };
/* ����������һ��3ά��������¼����������״�ͷ���*/
int boxstr[8][4][4]={{
        {0,0,0,0},
        {0,0,0,0},
        {0,1,1,0},
        {0,0,1,1}},
       {
       {0,0,0,0},
       {0,0,0,0},
       {0,0,1,1},
       {0,1,1,0}},
       {
       {0,0,0,0},
       {0,0,1,1},
       {0,0,0,1},
       {0,0,0,1}},
       {
       {0,0,0,0},
       {0,0,1,1},
       {0,0,1,0},
       {0,0,1,0}},
       {
       {0,0,0,1},
       {0,0,0,1},
       {0,0,0,1},
       {0,0,0,1}},
       {
       {0,0,0,0},
       {0,0,0,0},
       {0,0,0,0},
       {1,1,1,1}},
       {
       {0,0,0,0},
       {0,0,0,0},
       {0,0,1,1},
       {0,0,1,1}},
       {
       {0,0,0,0},
       {0,0,0,0},
       {0,1,1,1},
       {0,0,1,0}}
       };

/* ����õ���ǰ�������һ���������״�ͷ���*/
void boxrad(){
     minboxcolor=nextminboxcolor;
     boxfx=nextboxfx;
     boxgs=nextboxgs;
     nextminboxcolor=random(16);
     if(nextminboxcolor==BGCOLOR||nextminboxcolor==7)
       nextminboxcolor=9;
     nextboxfx=random(4);
     nextboxgs=random(8);
     nextboxco = random(15)<<4;
     }

/*��С����Ļ���*/
void minbox(int asc,int bsc,int color){
     int a=0,b=0,i;
     if (box_style)
     nextboxco= random(15)<<4;
     a=SCCOL+asc;
     b=SCROW+bsc;
     rectfill(a+1,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE,color);
     if(color!=BGCOLOR){
        for (i = 0; i < MINBOXSIZE-1; ++i){
        hline(a+1,b+1+i,a+1 + MINBOXSIZE-2,i + nextboxco );}
       setcolor(15);
       line(a+1,b+1,a-1+MINBOXSIZE,b+1);
       line(a+1,b+1,a+1,b-1+MINBOXSIZE);
       setcolor(0);
       line(a-1+MINBOXSIZE,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE);
       line(a+1,b-1+MINBOXSIZE,a-1+MINBOXSIZE,b-1+MINBOXSIZE);
       }
    }
/*��С����ĸ߼�����*/
void box(int a,int b,int color){
     rectfill(a+1,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE,color);
     setcolor(15);
     line(a+1,b+1,a-1+MINBOXSIZE,b+1);
     line(a+1,b+1,a+1,b-1+MINBOXSIZE);
     setcolor(0);
     line(a-1+MINBOXSIZE,b+1,a-1+MINBOXSIZE,b-1+MINBOXSIZE);
     line(a+1,b-1+MINBOXSIZE,a-1+MINBOXSIZE,b-1+MINBOXSIZE);
    }

/*��Ϸ�г��ֵ�����*/
void print(int a,int b,char *txt,int color)
{
   set_str_color(color);
   textout(txt, a, b);
}

/*window����*/
void win(int a,int b,int c,int d,int bgcolor,char *text,int textcolor){
       rectfill(a,b,c,d,7);
       rectfill(a+3,b+24,c-3,d-3,bgcolor);
       rectfill(a+3,b+3,c-3,b+20,9);
       setcolor(15);
       line(a,b,c,b);
       line(a,b,a,d);
       line(a+2,b+21,c-2,b+21);
       line(c-2,b+2,c-2,b+21);
       line(a+2,d-2,c-2,d-2);
       line(c-2,b+23,c-2,d-2);
       print(a+5,b+8,text,textcolor);
       setcolor(0);
       line(a,d,c,d);
       line(c,b,c,d);
       line(a+3,b+2,c-3,b+2);
       line(a+2,b+2,a+2,b+20);
       line(a+2,b+24,a+2,d-2);
       line(a+2,b+23,c-3,b+23);
       }
/*�ѷ���ķ�����תfx��90��(fx������3)*/
void re90(int boxxy[4][4],int fx){
     int i,j,k,zero;
     int a[4][4];
   for(k=0;k<fx;k++){
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  a[i][j]=boxxy[3-j][i];
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  boxxy[i][j]=a[i][j];
     for(i=0;i<4;i++){
       zero=1;
       for(j=0;j<4;j++)
 if(boxxy[j][3]!=0)
   zero=0;
 if(zero)
   for(j=0;j<4;j++){
   boxxy[j][3]=boxxy[j][2];boxxy[j][2]=boxxy[j][1];boxxy[j][1]=boxxy[j][0];boxxy[j][0]=0;
   }
 }
     }
   }
/* ��ǰ����Ļ���*/
void funbox(int a,int b,int color){
     int i,j;
     int boxz[4][4];
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  boxz[i][j]=boxstr[boxgs][i][j];
     re90(boxz,boxfx);
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  if(boxz[i][j]==1)
    minbox((j+row+a)*MINBOXSIZE,(i+col+b)*MINBOXSIZE,color);
   }
/*��һ������Ļ���*/
void nextfunbox(int a,int b,int color){
     int i,j;
     int boxz[4][4];
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  boxz[i][j]=boxstr[nextboxgs][i][j];
     re90(boxz,nextboxfx);
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  if(boxz[i][j]==1)
    minbox((j+a)*MINBOXSIZE,(i+b)*MINBOXSIZE,color);
   }

/*������Ϸ�Ĺ�������������С�����һ��*/
void delcol(int a){
     int i,j;
     for(i=a;i>1;i--)
       for(j=1;j<15;j++){
  minbox(j*MINBOXSIZE,i*MINBOXSIZE,BGCOLOR);
  gril[i][j]=gril[i-1][j];
  if(gril[i][j]==1)
     minbox(j*MINBOXSIZE,i*MINBOXSIZE,minboxcolor);
   }
      }

/*�������ж�����С�������*/
void _delete(){
     int i,j,zero,delgx=0;
     for(i=1;i<21;i++){
       zero=0;
       for(j=1;j<15;j++)
 if(gril[i][j]==0)
   zero=1;
 if(zero==0){
   delcol(i);
   delgx++;
   }
       }
     num += delgx*delgx*10;
     level=num/LEVEL;
     rectfill(456,140,504,200,BGCOLOR);
     print(456,141,"Level :",15);
     neo_printf(457, 153, "%d", level);
     print(456,173,"Number:",15);
     neo_printf(457, 185, "%ld", num);
    }

/* ���Ե�ǰ�����Ƿ����������*/
int downok(){
    int i,j,k=1,a[4][4];
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
 a[i][j]=boxstr[boxgs][i][j];
    re90(a,boxfx);
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
       if(a[i][j] && gril[col+i+1][row+j])
  k=0;
    return(k);
    }
/* ���Ե�ǰ�����Ƿ����������*/
int leftok(){
    int i,j,k=1,a[4][4];
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
 a[i][j]=boxstr[boxgs][i][j];
    re90(a,boxfx);
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
       if(a[i][j] && gril[col+i][row+j-1])
  k=0;
    return(k);
    }
/* ���Ե�ǰ�����Ƿ����������*/
int rightok(){
    int i,j,k=1,a[4][4];
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
 a[i][j]=boxstr[boxgs][i][j];
    re90(a,boxfx);
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
       if(a[i][j] && gril[col+i][row+j+1])
  k=0;
    return(k);
    }
/* ���Ե�ǰ�����Ƿ���Ա���*/
int upok(){
    int i,j,k=1,a[4][4];
    for(i=0;i<4;i++)
      for(j=0;j<4;j++)
       a[i][j]=boxstr[boxgs][i][j];
    re90(a,boxfx+1);
    for(i=3;i>=0;i--)
      for(j=3;j>=0;j--)
       if(a[i][j] && gril[col+i][row+j])
  k=0;
    return(k);
    }
/*��ǰ��������֮�󣬸���Ļ���������*/
void setgril(){
     int i,j,a[4][4];
     funbox(0,0,minboxcolor);
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  a[i][j]=boxstr[boxgs][i][j];
     re90(a,boxfx);
     for(i=0;i<4;i++)
       for(j=0;j<4;j++)
  if(a[i][j])
    gril[col+i][row+j]=1;
    col=1;row=7;
    }
/*��Ϸ����*/
void gameover(){
     win(150, 100, 250, 144, 15,"Game Over!", 0);
     textout("U Lose:(", 154, get_cn_size()==16?126:128);
     }
/*����������*/
void call_key(int keyx){
     switch(keyx){
 case KEY_DOWN: {  /*�·�������������һ��*/
   if(downok()){
     col++;
     funbox(0,0,minboxcolor);}
     else{
     funbox(0,0,minboxcolor);
     setgril();
     nextfunbox(NEXTCOL,NEXTROW,BGCOLOR);
     boxrad();
     nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor);
     _delete();
     }
     break;
   }
 case KEY_UP:  { /*�Ϸ������������״��ת90��*/
                       if(upok())
    boxfx++;
         if(boxfx>3)
    boxfx=0;
    funbox(0,0,minboxcolor);
         break;
         }
 case KEY_LEFT:{ /*��������������һ*/
                       if(leftok())
    row--;
         funbox(0,0,minboxcolor);
         break;
         }
 case KEY_RIGHT:{ /*�ҷ�������������һ*/
                        if(rightok())
   row++;
   funbox(0,0,minboxcolor);
   break;
   }
 case VK_SPACE: /*�ո����ֱ���䵽�������䵽������*/
                        while(downok())
    col++;
         funbox(0,0,minboxcolor);
         setgril();
         nextfunbox(NEXTCOL,NEXTROW,BGCOLOR);
         boxrad();
         nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor);
         _delete();
         break;
  case 115:
     box_style = box_style?FALSE : TRUE;
     break;
 default: { win(423,53,620,95,15,"Enter Key Error!",15);
     print(428,80,"Plese Enter Anly Key AG!",4);
     _getch();
     rectfill(420,50,622,97,BGCOLOR);
     }
      }
}

/*ʱ���жϿ�ʼ*/
void timezd(void){
  int key;

  boxrad();
  nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor);
  for(;;){
    if(keypressed()){
      key=get_asc();
      funbox(0,0,BGCOLOR);
      if(key == 27)
 break;
      call_key(key);
      }
   if(g_time_counter>gamelevel[level]){
      g_time_counter=0;
      if(downok()){
 funbox(0,0,BGCOLOR);
 col++;
 funbox(0,0,minboxcolor);
 }
 else {
  if(col==1){
    gameover();
    _getch();
    break;
    }
  setgril();
  _delete();
  funbox(0,0,minboxcolor);
  col=1;row=7;
  funbox(0,0,BGCOLOR);
  nextfunbox(NEXTCOL,NEXTROW,BGCOLOR);
  boxrad();
  nextfunbox(NEXTCOL,NEXTROW,nextminboxcolor);
  }
      }
    }
  }

/*������ʼ*/
void main(void)
{
  int i,j,color=7;
  char *nm;
  neo_init();
  setgraphmode(VBE640X480X256);
  install_keyboard();
  set_neo_color();
  clear();
/*��Ļ�����ʼ��*/
  for(i=0;i<=MAXCOL+1;i++)
    for(j=0;j<=MAXROW+1;j++)
      gril[i][j]=0;
  for(i=0;i<=MAXCOL+1;i++) {
    gril[i][0]=1;
    gril[i][15]=1;
    }
  for(j=1;j<=MAXROW;j++){
    gril[0][j]=1;
    gril[21][j]=1;
    }
  rectfill(0,0,640,480,15);
  win(1,1,639,479,BGCOLOR,"Example of NEO SDK",15);

  setcolor(color);
  for(i=0;i<=21;i++){
    for(j=0;j<=15;j++) {
     if(gril[i][j]==1)
       box(j*MINBOXSIZE+SCCOL,i*MINBOXSIZE+SCROW,color);
    }
 }
  nextboxgs=random(8);
  nextboxfx=random(4);

  print(456,141,"Level :",15);
  neo_printf(457, 153, "%d", level);
  print(456,173,"Number:",15);
  neo_printf(457, 185, "%d", num);
  print(456,243,"Next Box:",15);
  _install_timer();
  change_timer(30);
  timezd();
  closegraph();
}
