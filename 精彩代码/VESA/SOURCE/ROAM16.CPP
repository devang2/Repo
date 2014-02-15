#include "\vga\vgabase.h"
#include "\vga\vga16.h"
#include "\vga\vga256.h"
#include "\vga\vgahigh.h"
#include "\vga\vgaroam.h"

_640_480_16_roam::_640_480_16_roam(int vwi,int vhi,int fixedhi)
{
long l;
orgWide=640;
orgHigh=480;
minMoveWide=8;
minSetWide=16;
l=(long)(vwi)*(long)vhi;
if(l>0x80000L)
	{
	l=480L*(long)vwi;
	if(l>0x80000L)
		{
		vwi=0x80000L/480L;
		vhi=480;
		}
	else
		vhi=0x80000L/vwi;
	}
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE/8;
PAGEN=1;
VESAmodeNo=0x0012;
}

void _640_480_16_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

_800_600_16_roam::_800_600_16_roam(int vwi,int vhi,int fixedhi)
{
orgWide=800;
orgHigh=600;
minMoveWide=8;
minSetWide=16;
if(vwi<1536)
	vwi=1024;
else
	vwi=2048;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE/8;
PAGEN=4;
VESAmodeNo=0x0102;
}

void _800_600_16_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

_1024_768_16_roam::_1024_768_16_roam(int vwi,int vhi,int fixedhi)
{
orgWide=1024;
orgHigh=768;
minMoveWide=8;
minSetWide=8;
vwi=1024;
VWIDE=vwi;
VHIGH=vhi;
fixedHigh=fixedhi;
checkpar();
WIDE=VWIDE;
HIGH=VHIGH;
SCANLENG=VWIDE/8;
PAGEN=4;
VESAmodeNo=0x0104;
}

void _1024_768_16_roam::setmode()
{
VGABASE::setmode();
ROAM::setmode();
cls0();
}

