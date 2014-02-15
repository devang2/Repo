#ifndef VGABASE_H
	#define VGABASE_H

typedef unsigned char unchar;

#define Select_Page(page) Select_Page_VESAint(page)
//or
//#define Select_Page(page) Select_Page_Trident(page)
#define G_SEGMENT 0a000h

union COLOR {
	long dword;
	int word;
	unsigned char byte;
	unsigned char rgb[3];
	};

enum WHERE {
	inNONE  = 0,
	inMEM = 1,
	inXMS = 2,
	inHD  = 3
	};

struct IMAGE {
	int where;
	union IMGhandle {
		void *mem;
		class XMS *xms;
		char *filename;
		} handle;
	int x1,y1,xn,yn;
	~IMAGE();
	} ;

class VGABASE {
public:
	int OK;
	COLOR CUR_COLOR;
	int WIDE,HIGH;
	int VESAmodeNo;

	VGABASE() { OK=0; }
	virtual int init(void);
	void close(void);
	void display_off(void);
	void display_on(void);
	void setcolor(union COLOR color) { CUR_COLOR=color; }

	virtual void cls0(void);
	virtual void setcolor(unchar color);
	virtual COLOR setcolorto(unchar color);
	virtual void setcolor(unchar r,unchar g,unchar b);
	virtual COLOR setcolorto(unchar r,unchar g,unchar b);

	virtual void putpixel(int x,int y)=0;
	virtual union COLOR getpixel(int x,int y)=0;
	virtual void scanline(int x1,int x2,int y)=0;
	virtual void cls(void)=0;
	virtual void getscanline(int x1,int y,int n,void *buf)=0;
	virtual void putscanline(int x1,int y,int n,void *buf)=0;
	virtual int scanlinesize(int x1,int x2)=0;

	void line(int x1,int y1,int x2,int y2);
	void moveto(int x,int y);
	void lineto(int x,int y);
	void rectangle(int x1,int y1,int x2,int y2);
	void poly(int n,int *border);
	void poly(int *border);
	void circle(int x0,int y0,int r);
	void sector(int x0,int y0,int r,int stangle,int endangle);
	void ellipse(int x0,int y0,long r1,long r2);

	void setfillstyle(int fst);
	int getfillstyle(void);
	void setfillpattern(unsigned char *s);
	void fillarea(int x0,int y0,COLOR bordercolor);
	void bar(int x1,int y1,int x2,int y2);
	void polyfill(int n,int *border);
	void polyfill(int *border);
	void circlefill(int x0,int y0,int r);
	void sectorfill(int x0,int y0,int r,int stangle,int endangle);
	void ellipsefill(int x0,int y0,long r1,long r2);

	long imagesize(int x1,int y1,int x2,int y2);
	IMAGE *getimage(int x1,int y1,int x2,int y2,int where=inMEM);
	void putimage(int x1,int y1,IMAGE *img);
	void putimage(IMAGE *img);
	void getimageMEM(int x1,int y1,int x2,int y2,void *buf);
	void putimageMEM(int x1,int y1,int xn,int yn,void *buf);

protected:
	int CUR_X,CUR_Y;
	int PAGEN;
	int PN;
	int CUR_PAGE,SCANLENG;

	virtual void setmode(void);
	void Select_Page_VESAcall(int page);
	void Select_Page_VESAcallG(int page);
	void Select_Page_VESAint(int page);
	void Select_Page_VESAintG(int page);
	void Select_Page_Trident(int page);

	class XMS *getimageXMS(int x1,int y1,int x2,int y2);
	void putimageXMS(int x1,int y1,int xn,int yn,class XMS *xms);
	char *getimageHD(int x1,int y1,int x2,int y2);
	void putimageHD(int x1,int y1,int xn,int yn,char *filename);

private:
	void getpagepar(void);

public:
	virtual void putpixel(int x,int y,COLOR color)=0;
	virtual void scanline(int x1,int x2,int y,COLOR color)=0;
	virtual void cls(COLOR color)=0;
	void line(int x1,int y1,int x2,int y2,COLOR color)
		{ CUR_COLOR=color; line(x1,y1,x2,y2); }
	void lineto(int x,int y,COLOR color)
		{ CUR_COLOR=color; lineto(x,y); }
	void rectangle(int x1,int y1,int x2,int y2,COLOR color)
		{ CUR_COLOR=color; rectangle(x1,y1,x2,y2); }
	void poly(int n,int *border,COLOR color)
		{ CUR_COLOR=color; poly(n,border); }
	void poly(int *border,COLOR color)
		{ CUR_COLOR=color; poly(border); }
	void circle(int x0,int y0,int r,COLOR color)
		{ CUR_COLOR=color; circle(x0,y0,r); }
	void sector(int x0,int y0,int r,int stagl,int endagl,COLOR color)
		{ CUR_COLOR=color; sector(x0,y0,r,stagl,endagl); }
	void ellipse(int x0,int y0,long r1,long r2,COLOR color)
		{ CUR_COLOR=color; ellipse(x0,y0,r1,r2); }
	void fillarea(int x0,int y0,COLOR bordercolor,COLOR color)
		{ CUR_COLOR=color; fillarea(x0,y0,bordercolor); }
	void bar(int x1,int y1,int x2,int y2,COLOR color)
		{ CUR_COLOR=color; bar(x1,y1,x2,y2); }
	void polyfill(int n,int *border,COLOR color)
		{ CUR_COLOR=color; polyfill(n,border); }
	void polyfill(int *border,COLOR color)
		{ CUR_COLOR=color; polyfill(border); }
	void circlefill(int x0,int y0,int r,COLOR color)
		{ CUR_COLOR=color; circlefill(x0,y0,r); }
	void sectorfill(int x0,int y0,int r,int stagl,int endagl,COLOR color)
		{ CUR_COLOR=color; sectorfill(x0,y0,r,stagl,endagl); }
	void ellipsefill(int x0,int y0,long r1,long r2,COLOR color)
		{ CUR_COLOR=color; ellipsefill(x0,y0,r1,r2); }
	};


class VGAindirect : public VGABASE {
public:
	void setdac(unchar idx,unchar r,unchar g,unchar b);
	void getdac(unchar idx,unchar &r,unchar &g,unchar &b);
	} ;

double Lsin(int angle);
double Lcos(int angle);

#endif

