#include "Stdio.h"
#include "Conio.h"
#include "String.h"
#include "dir.h"  /* 等文件函数*/
#include "math.h"
#include "d:\edit\def.h"

 int main(void)
{

    int key;
    int shiftkey;


     /*设置文本显示模式 80×25 彩色*/
    textmode(C80);
    flag=0;  /*初始化为插入状态*/

    clrscr();
    initmenu();

    /*显示主菜单(占最上面一行)*/
    mainfram();
    NewFile();


   while(1)
    {
        key=bioskey(0);
        shiftkey=bioskey(2);
        TranslateKey(key,shiftkey);
    }
  return 0;
}


