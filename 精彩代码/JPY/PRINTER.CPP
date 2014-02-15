#include <dos.h>
union REGS inregs,outregs;
char msg[]="Message for printer.\n";
int main()
  {
  int i;
  i=1;
  while(msg[i]!=0){
    inregs.h.ah=0x01;
    inregs.h.al=msg[i];
    inregs.x.dx=1;
    int86(0x17,&inregs,&outregs);
    ++i;
    }
  return 0;
  }