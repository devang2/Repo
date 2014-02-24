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
  system("copy c:\\jpy\\jpy.bat a:");
  system("copy c:\\jpy\\egavga.bgi a:");
  system("copy c:\\jpy\\jpy.exe a:");
  system("copy c:\\jpy\\viewbmp.exe a:");
  system("copy c:\\jpy\\fm.bmp a:");
  system("copy c:\\jpy\\hzk16 a:");
  system("copy c:\\jpy\\mouse.* a:");
  system("copy c:\\jpy\\install.exe a:");
  system("copy c:\\jpy\\user.chk a:");
  system("copy c:\\jpy\\key.exe a:");
  system("copy c:\\jpy\\jp.exe a:");
  system("cls");
  printf("setup finished!");
  return(0);
  }
