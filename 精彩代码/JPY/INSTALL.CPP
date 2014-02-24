#include <dir.h>
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <bios.h>
#include <io.h>
#include <stat.h>
#include <fcntl.h>
#define IGNORE 0
#define RETRY 1
#define ABORT 2
static char *err_msg[]={
  "write protect",
  "unknown unit",
  "drive not ready",
  "unknown command",
  "data error(CRC)",
  "bad request",
  "seek error",
  "unknown media type",
  "sector not found",
  "printer out of paper",
  "write fault",
  "read fault",
  "general failure",
  "reserved",
  "reserved",
  "invalid disk change"
  };
void error_win(char *msg)
  {
  printf("%s",msg);//cputs
  }
int handler(int errval,int ax,int bp,int si)
  {
  static char msg[80];
  unsigned di;
  int drive;
  int errorno;
  di=_DI;
  if(ax<0){
    error_win("Device error");
    hardretn(ABORT);
    }
  drive=ax&0x00ff;
  errorno=di&0x00ff;
  hardretn(ABORT);
  sprintf(msg,"Error:%s on drive %c\r\n",err_msg[errorno],'A'+drive);
  error_win(msg);
  return ABORT;
  }
void install()
  {
  system("echo off");
  system("c:\\");
  system("cd\\");
  system("md jpy");
  system("cd\\jpy");
  system("copy a:\\*.*");
  system("lharc e jpy.lzh");
  system("copy c:\\jpy\\*.lnk c:\\windows\\desktop");
  system("copy c:\\jpy\\*.pif c:\\windows\\desktop");
  system("copy c:\\jpy\\jpy.bat c:\\");
  system("copy c:\\jpy\\ini_jpy.exe c:\\");
  system("ini_jpy.exe");
  }
#pragma warn + par
int main()
  {
  harderr(handler);
  install();
  return(0);
  }
