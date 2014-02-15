#include "\vga\vgabase.h"
#include "\vga\vga16.h"
#include "\vga\vga256.h"
#include "\vga\vgahigh.h"
#include "\vga\vgaroam.h"

_640_400_256_roam::_640_400_256_roam(int vwi,int vhi,int fixedhi)
{
orgWide=640;
orgHigh=400;
minMoveWide=8;
minSetWide=16;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE;
PAGEN=16;
VESAmodeNo=0x0100;
}

void _640_400_256_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

_640_480_256_roam::_640_480_256_roam(int vwi,int vhi,int fixedhi)
{
orgWide=640;
orgHigh=480;
minMoveWide=8;
minSetWide=16;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE;
PAGEN=16;
VESAmodeNo=0x0101;
}

void _640_480_256_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

_800_600_256_roam::_800_600_256_roam(int vwi,int vhi,int fixedhi)
{
orgWide=800;
orgHigh=600;
minMoveWide=8;
minSetWide=16;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE;
PAGEN=16;
VESAmodeNo=0x0103;
}

void _800_600_256_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

_1024_768_256_roam::_1024_768_256_roam(int vwi,int vhi,int fixedhi)
{
orgWide=1024;
orgHigh=768;
minMoveWide=8;
minSetWide=8;
//vwi=1024;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE;
PAGEN=16;
VESAmodeNo=0x0105;
}

void _1024_768_256_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

