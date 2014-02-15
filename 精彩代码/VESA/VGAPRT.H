#ifndef VGAPRT_H
	#define VGAPRT_H

class prt_buf : public VGABASE
	{
public:
	prt_buf(int wi,int hi,int whe=inMEM);
	~prt_buf();
	int init(void) { return(OK);}
	void close(void) { }
	void setcolor(unchar col);
	COLOR setcolorto(unchar col);
	void putpixel(int x,int y);
	COLOR getpixel(int x,int y);
	void scanline(int x1,int x2,int y);
	void cls0();
	void cls();
	void getscanline(int x1,int y1,int n,void *buf);
	void putscanline(int x1,int y1,int n,void *buf);
	int scanlinesize(int x1,int x2);
	void putpixel(int x,int y,COLOR color)
		{ CUR_COLOR=color; putpixel(x,y); }
	void scanline(int x1,int x2,int y,COLOR color)
		{ CUR_COLOR=color; scanline(x1,x2,y); }
	void cls(COLOR color)
		{ CUR_COLOR=color; cls(); }


	unchar *getprtdata(int y);

	unsigned char *buffer;
	void getbuffer(int y);
protected:
	int where,widebyte;
	int bufy;
	union { unsigned char **mem;
			class XMS *xms;
			FILE *file;
			} handle;
	} ;


class PRINTER {
public:
	PRINTER(VGABASE *v);
	virtual void print(void)=0;
	virtual void outpaper(void)=0;
	void nextrow(void);
protected:
	VGABASE *vga;
	int byten;
	unchar *getprtdata(int y);
	void outcode(unsigned char *code);
	virtual void setdpi() { };
	virtual void setmode(int pxn) { }
	} ;

class HP_PRINTER : public PRINTER
	{
public:
	HP_PRINTER(VGABASE *v) : PRINTER(v) { }
	void print(void);
	void outpaper(void);
protected:
	void setbytenum(int n);
	void setgz();
	void setdx();
	void end();
	} ;

class HP_600 : public HP_PRINTER
	{
public:
	HP_600(VGABASE *v) : HP_PRINTER(v) { }
protected:
	void setdpi();
	} ;

class HP_300 : public HP_PRINTER
	{
public:
	HP_300(VGABASE *v) : HP_PRINTER(v) { }
protected:
	void setdpi();
	} ;

class HP_150 : public HP_PRINTER
	{
public:
	HP_150(VGABASE *v) : HP_PRINTER(v) { }
protected:
	void setdpi();
	} ;

class HP_100 : public HP_PRINTER
	{
public:
	HP_100(VGABASE *v) : HP_PRINTER(v) { }
protected:
	void setdpi();
	} ;

class HP_75 : public HP_PRINTER
	{
public:
	HP_75(VGABASE *v) : HP_PRINTER(v) { }
protected:
	void setdpi();
	} ;

class EPSON_PRINTER : public PRINTER
	{
public:
	EPSON_PRINTER(VGABASE *v) : PRINTER(v) { }
	void print(void);
	void outpaper(void);
protected:
	int dpi;
	void init(void);
	void setjg180(unchar c180=24);
	void setdx(void);
	void setmodeb(int pxn,unchar *code);
	} ;

class EPSON_360 : public EPSON_PRINTER
	{
public:
	EPSON_360(VGABASE *v) : EPSON_PRINTER(v) { dpi=360; }
protected:
	void setmode(int pxn);
	} ;

class EPSON_180 : public EPSON_PRINTER
	{
public:
	EPSON_180(VGABASE *v) : EPSON_PRINTER(v) { dpi=180; }
protected:
	void setmode(int pxn);
	} ;

class EPSON_60 : public EPSON_PRINTER
	{
public:
	EPSON_60(VGABASE *v) : EPSON_PRINTER(v) { dpi=60; }
protected:
	void setmode(int pxn);
	} ;

#endif

