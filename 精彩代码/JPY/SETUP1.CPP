#include <dir.h>
#include <dos.h>
#include <math.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <bios.h>
#include <io.h>
#include <stat.h>
#include <fcntl.h>
int main()
  {
  system("copy c:\\jpy\\jpy.bat c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\egavga.bgi c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\jpy.exe c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\viewbmp.exe c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\fm.bmp c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\hzk16 c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\mouse.* c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\install.exe c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\ini_jpy.exe c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\user.chk c:\\jpy1");
  delay(200);
  system("copy c:\\jpy\\key.exe c:\\jpy1");
  delay(200);
  system("cls");
  printf("setup finished!");
  return(0);
  }
