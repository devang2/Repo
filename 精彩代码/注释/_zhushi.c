

#include <stdio.h>
#include <conio.h>

void main(char argc, char* argv[])
{
    
    FILE *fp1, *fp2;                         
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
        return;                              
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
        if (feof(fp1)) break;                
        if (c1 == '/')
        {
            c2 = fgetc(fp1);
            if (feof(fp1)) break;            
            else if (c2 == '*')
            {
                do 
                {
                    c1 = fgetc(fp1);
                    if(feof(fp1)) break;     
                    if (c1 == '*')
                    {
                        c2 = fgetc(fp1);
                        if (feof(fp1)) break;
                        if (c2 == '/') break;
                    }
                } while(1);
    	    }
    	    else if (c2 == '/')
    	    {
                do 
                {
                    c1 = fgetc(fp1);
                    if(feof(fp1)) break;     
                    if (c1 == 0x0d)
                    {
                        c2 = fgetc(fp1);
                        if (feof(fp1)) break;
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