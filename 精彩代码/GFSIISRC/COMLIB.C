#define COMINT       0x14
#define COMINIT         0
#define COMSEND         1
#define COMRECEIVE      2
#define COMSTAT         3
#define COMBUFSTAT      4
#define COMSETPORT      5
#define COMSETVECTOR    6
#define COMSETACTIVE    7
#define COMSETMCR       8
#define NONE            0
#define ODD             1
#define EVEN            2
#define COM1            0
#define COM2            1
#define COM3            2
#define COM4            3
#define COM5            4
#define COM6            5
#define COM7            6
#define COM8            7
#define TSRID         232
#define XON            17
#define XOFF           19
#define DTR             1
#define RTS             2
#define OUT1            4
#define OUT2            8

#if     !defined(__DOS)
#include        <dos.h>
#endif

int comtxch(irqno,portno,ch)
int irqno,portno;
char ch;
{
  union REGS reg;

  reg.h.dh = irqno;
  reg.h.dl = portno;
  reg.h.ah = COMSEND;
  reg.h.al = ch;
  int86(0x14,&reg,&reg);

  return (reg.x.ax);
}

int comrxch(irqno,portno,ch)
int irqno,portno;
char *ch;
{
  union REGS reg;

  reg.h.dh = irqno;
  reg.h.dl = portno;
  reg.h.ah = COMRECEIVE;
  int86(0x14,&reg,&reg);
  *ch = reg.h.al;

  return (reg.h.ah);
}

