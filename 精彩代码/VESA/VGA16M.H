#ifndef VGA16M_H
	#define VGA16M_H

class VGA16M : public VGABASE
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

	void en_putpixel(int x,int y,unchar enl,unchar redu);
	void en_scanline(int x1,int x2,int y,unchar enl,unchar redu);
	void en_bar(int x1,int y1,int x2,int y2,unchar enl,unchar redu);

	void re_putpixel(int x,int y);
	void re_scanline(int x1,int x2,int y);
	void re_bar(int x1,int y1,int x2,int y2);
	void de_putpixel(int x,int y);
	void de_scanline(int x1,int x2,int y);
	void de_bar(int x1,int y1,int x2,int y2);

	void re_putpixel(int x,int y,COLOR col)
		{ CUR_COLOR=col; re_putpixel(x,y); }
	void re_scanline(int x1,int x2,int y,COLOR col)
		{ CUR_COLOR=col; re_scanline(x1,x2,y); }
	void re_bar(int x1,int y1,int x2,int y2,COLOR col)
		{ CUR_COLOR=col; re_bar(x1,y1,x2,y2); }
	void de_putpixel(int x,int y,COLOR col)
		{ CUR_COLOR=col; de_putpixel(x,y); }
	void de_scanline(int x1,int x2,int y,COLOR col)
		{ CUR_COLOR=col; de_scanline(x1,x2,y); }
	void de_bar(int x1,int y1,int x2,int y2,COLOR col)
		{ CUR_COLOR=col; de_bar(x1,y1,x2,y2); }
	};

class _320_200_16M : public VGA16M
	{
public:
	_320_200_16M();
	};

class _640_480_16M : public VGA16M
	{
public:
	_640_480_16M();
	};

#endif

