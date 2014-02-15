#ifndef VGA256_H
	#define VGA256_H

class VGA256 : public VGAindirect
	{
public:
	void putpixel(int x,int y);
	union COLOR getpixel(int x,int y);
	void cls(void);
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
	};

class _320_200_256 : public VGA256
	{
public:
	_320_200_256();
	};

class _640_400_256 : public VGA256
	{
public:
	_640_400_256();
	};

class _640_480_256 : public VGA256
	{
public:
	_640_480_256();
	};

class _800_600_256 : public VGA256
	{
public:
	_800_600_256();
	};

class _1024_768_256 : public VGA256
	{
public:
	_1024_768_256();
	};

#endif

