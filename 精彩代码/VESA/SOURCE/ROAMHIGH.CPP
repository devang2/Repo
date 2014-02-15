#include "\vga\vgabase.h"
#include "\vga\vga16.h"
#include "\vga\vga256.h"
#include "\vga\vgahigh.h"
#include "\vga\vgaroam.h"

_320_200_32k_roam::_320_200_32k_roam(int vwi,int vhi,int fixedhi)
{
orgWide=320;
orgHigh=200;
minMoveWide=2;
minSetWide=4;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE*2;
PAGEN=16;
VESAmodeNo=0x010d;
}

void _320_200_32k_roam::setmode()
{
VGABASE::setmode();
setScreenWide(WIDE);
cutScreen(0);
setbxy(0,0);
cls0();
}

_512_480_32k_roam::_512_480_32k_roam(int vwi,int vhi,int fixedhi)
{
orgWide=512;
orgHigh=480;
minMoveWide=2;
minSetWide=4;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE*2;
PAGEN=16;
VESAmodeNo=0x0170;
}

void _512_480_32k_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}


_320_200_64k_roam::_320_200_64k_roam(int vwi,int vhi,int fixedhi)
{
orgWide=320;
orgHigh=200;
minMoveWide=2;
minSetWide=4;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE*2;
PAGEN=16;
VESAmodeNo=0x010e;
}

void _320_200_64k_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}


_512_480_64k_roam::_512_480_64k_roam(int vwi,int vhi,int fixedhi)
{
orgWide=512;
orgHigh=480;
minMoveWide=2;
minSetWide=4;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE*2;
PAGEN=16;
VESAmodeNo=0x0171;
}

void _512_480_64k_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}
