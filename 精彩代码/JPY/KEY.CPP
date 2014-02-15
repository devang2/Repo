#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
int main()
  {
  unsigned int i,cmos=0;
  FILE *fp;
  for(i=0x10;i<=0x2d;i++)
    {
    outportb(0x70,i);
    cmos+=inportb(0x71);
    }
    if((fp=fopen("user.chk","wb"))==NULL)
      {
      printf("\nuser.chk is not exsist!!!\07");
      return(0);
      }
    else printf("\nYour software is unlock, you can use!!!\07");
    printf("\nThe number is:%d\n",cmos);
    fwrite(&cmos,2,1,fp);
    fclose(fp);
  return(0);
  }