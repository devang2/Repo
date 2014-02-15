#ifndef VGA16_H
	#define VGA16_H

class VGA16 : public VGAindirect
	{
public:
	void putpixel(int x,int y);
	union COLOR getpixel(int x,int y);
	void cls(void);
	void cls0(void);
	int scanlinesize(int x1,int x2);
	void scanline(int x1,int x2,int y);
	void getscanline(int x1,int y,int n,void *buf);
	void putscanline(int x1,int y,int n,void *buf);

	void putpixel(int x,int y,COLOR color)
		{ CUR_COLOR=color; putpixel(x,y); }
	void scanline(int x1,int x2,int y,COLOR color)
		{ CUR_COLOR=color; scanline(x1,x2,y); }
	void cls(COLOR color)
		{ CUR_COLOR=color; cls(); }

	void setpalette(unchar idx16,unchar idx256);
	unchar getpalette(unchar idx16);
	};

class _640_480_16 : public VGA16
	{
public:
	_640_480_16();
	};

class _800_600_16 : public VGA16
	{
public:
	_800_600_16();
	};

class _1024_768_16 : public VGA16
	{
public:
	_1024_768_16();
	};

#endif

