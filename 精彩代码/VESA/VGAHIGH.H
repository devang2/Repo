#ifndef VGAHIGH_H
	#define VGAHIGH_H

class VGAHIGH : public VGABASE
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
	} ;

class VGA32k : public VGAHIGH
	{
public:
	void setcolor(unchar r,unchar g,unchar b);
	void setcolor(unchar light)
		{ setcolor(light,light,light); }
	COLOR setcolorto(unchar r,unchar g,unchar b);
	COLOR setcolorto(unchar light)
		{ return setcolorto(light,light,light); }
	} ;

class VGA64k : public VGAHIGH
	{
public:
	void setcolor(unchar r,unchar g,unchar b);
	void setcolor(unchar light)
		{ setcolor(light,light,light); }
	COLOR setcolorto(unchar r,unchar g,unchar b);
	COLOR setcolorto(unchar light)
		{ return setcolorto(light,light,light); }
	} ;


class _320_200_32k : public VGA32k
	{
public:
	_320_200_32k();
	};

class _320_200_64k : public VGA64k
	{
public:
	_320_200_64k();
	};

class _512_480_32k : public VGA32k
	{
public:
	_512_480_32k();
	};

class _512_480_64k : public VGA64k
	{
public:
	_512_480_64k();
	};

class _640_480_32k : public VGA32k
	{
public:
	_640_480_32k();
	};

class _640_480_64k : public VGA64k
	{
public:
	_640_480_64k();
	};

class _800_600_32k : public VGA32k
	{
public:
	_800_600_32k();
	};

class _800_600_64k : public VGA64k
	{
public:
	_800_600_64k();
	};

#endif

