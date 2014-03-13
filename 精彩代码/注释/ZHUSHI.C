/* 删除c程序里的所有注释部分 */

#include <stdio.h>
#include <conio.h>

void main(char argc, char* argv[])
{
    /* 
        程序执行时, 命令行参数的个数记录在 argc 里
        命令行第一个参数(字符串)的地址被赋给argv[0]
        命令行第二个参数(字符串)的地址被赋给argv[1]
        命令行第三个参数(字符串)的地址被赋给argv[2]
    */
    FILE *fp1, *fp2;                         // 定义文件指针
    char c1,c2,i;

    if(argc != 3)
    {
        printf("\n USAGE: zhushi.exe *.c _*.c \n");
        getch();
        return;
    }
    if((fp1=fopen(argv[1],"rb"))==NULL)
    {
        printf("\n 读取的文件不存在");
        getch();
        return;                              // 读取一个文件, 文件名由argv[1]所指字符串决定, 文件指针fp1定位于文件的开头
    }
    if((fp2=fopen(argv[2],"rb"))!=NULL)
    {
        printf("\n 请备份或删除目标文件");
        getch();
        return;
    }
    if((fp2=fopen(argv[2],"wb"))==NULL)
    {
        printf("\n 不能建立目标文件");
        getch();
        return;
    }
    i=0;
    while(1)
    {
        c1 = fgetc(fp1);
        if (feof(fp1)) break;                // 读文件结束, 退出时i==0 表示不正常
        if (c1 == '/')
        {
            c2 = fgetc(fp1);
            if (feof(fp1)) break;            // 读文件结束, 非正常退出001, 文件最后有一个独立的字符'/'
            else if (c2 == '*')
            {
                do //以下字符为注释
                {
                    c1 = fgetc(fp1);
                    if(feof(fp1)) break;     // 读文件结束, 非正常退出002, 文件最后有两个连续字符'/'和'*',但缺少'*'和'/'
                    if (c1 == '*')
                    {
                        c2 = fgetc(fp1);
                        if (feof(fp1)) break;// 读文件结束, 非正常退出003, 文件最后的注释段结束有一个字符'*', 但缺少一个'/'
                        if (c2 == '/') break;
                    }
                } while(1);
    	    }
    	    else if (c2 == '/')
    	    {
                do //以下字符为注释
                {
                    c1 = fgetc(fp1);
                    if(feof(fp1)) break;     // 读文件结束, 正常退出, 最后一行是注释, 但未回车换行
                    if (c1 == 0x0d)
                    {
                        c2 = fgetc(fp1);
                        if (feof(fp1)) break;// 读文件结束, 非正常退出004, 最后一行是注释, 但是有回车没换行
                        if (c2 == 0x0a)
                        {
                            fputc(0x0d,fp2);
                            fputc(0x0a,fp2);
                            break;
                        }
                    }
                } while(1);
            }
            else {
    	        fputc(c1,fp2);
    	        fputc(c2,fp2);
            }
    	}
    	else {
    	    fputc(c1,fp2);
    	}
    	i=1;
    }
    if(i==0)
    {
        printf("读取的文件内容为空");
        getch();
    }
    fclose(fp1);
    fclose(fp2);
}