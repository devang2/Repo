#ifndef VGAROAM_H
	#define VGAROAM_H

class ROAM
	{
public:
	static int orgWide,orgHigh;
	static int activHigh,fixedHigh;
	static int begx,begy;
	static int endx,endy;

	static void roam(int dx,int dy);
	static void setbxy(int bx,int by);
	static void setexy(int ex,int ey);
	static void autoroam(int x0,int y0,int xn,int yn);
	void setup_key_roam(int d=8);
	void del_key_roam(void);
protected:
	static int minMoveY,maxMoveY;
	static int maxMoveX;
	static int minMoveWide,minSetWide;
	static int VWIDE,VHIGH;
	static int KEY_OK,dxy;

	void checkpar(void);
	void setScreenWide(int wi);
	void cutScreen(int hi);
	void setmode();

	static void interrupt kbint0x09_roam(void);
	};

class _640_400_256_roam : public VGA256, public ROAM
	{
public:
	_640_400_256_roam(int vwi,int vhi,int fhi=0);
protected:
	void setmode(void);
	};

class _640_480_256_roam : public VGA256, public ROAM
	{
public:
	_640_480_256_roam(int vwi,int vhi,int fhi=0);
protected:
	void setmode(void);
	};

class _800_600_256_roam : public VGA256, public ROAM
	{
public:
	_800_600_256_roam(int vwi,int vhi,int fhi=0);
protected:
	void setmode(void);
	};

class _1024_768_256_roam : public VGA256, public ROAM
	{
public:
	_1024_768_256_roam(int vwi,int vhi,int fhi=0);
protected:
	void setmode(void);
	};


class _640_480_16_roam : public VGA16, public ROAM
	{
public:
	_640_480_16_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

class _800_600_16_roam : public VGA16, public ROAM
	{
public:
	_800_600_16_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

class _1024_768_16_roam : public VGA16, public ROAM
	{
public:
	_1024_768_16_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

class _320_200_32k_roam : public VGA32k, public ROAM
	{
public:
	_320_200_32k_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

class _512_480_32k_roam : public VGA32k, public ROAM
	{
public:
	_512_480_32k_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

class _320_200_64k_roam : public VGA64k, public ROAM
	{
public:
	_320_200_64k_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

class _512_480_64k_roam : public VGA64k, public ROAM
	{
public:
	_512_480_64k_roam(int vwi,int vhi,int fixedhi);
protected:
	void setmode(void);
	};

#endif

