#ifndef FILLBASE_H
	#define FILLBASE_H

class FILL {
public:
	static void setstyle(int fstyle);
	static int getstyle(void);
	static void setpattern(unsigned char *p);
	static void scanlinestyle(int x1,int x2,int y);
	void line(int x1,int y1,int x2,int y2);
	virtual void inspole(int x,int y) { }
	static int dataptr;

protected:
	static unsigned char STD[96];
	static int STDN;
	static VGABASE *vga;

	FILL(VGABASE *v) { vga=v; }
	} ;


struct LFS {
	LFS *next,*prev;
	int x,y;

	LFS(int x0,int y0) { x=x0; y=y0; }
	void insbefore(LFS *lfs);
	void del();
	int operator>(LFS lp) {
		if(y>lp.y || (y==lp.y&&x>lp.x) ) return(1);
		else return(0); }
	int operator<(LFS lp) {
		if( y<lp.y || (y==lp.y&&x<lp.x) ) return(1);
		else return(0); }
	int operator>=(LFS lp) {
		if(y>lp.y || (y==lp.y&&x>=lp.x) ) return(1);
		else return(0); }
	int operator<=(LFS lp) {
		if( y<lp.y || (y==lp.y&&x<=lp.x) ) return(1);
		else return(0); }
	} ;

class LFILL : public FILL {
	LFS *beg,*end;
	LFS *cur;

public:
	LFILL(VGABASE *v);
	~LFILL();
	void inspole(int x,int y);
	void inspole(LFS *lp);
	void add(LFILL *lfp);
	int ifincl(LFILL *lfp);
	void poly(int n,int *border);
	void draw();
	void save(void);
	} ;


struct YFS {
	int n;
	int x[4];
	} ;

class YFILL : public FILL {
	YFS *yfs;
	int Y1,Y2;

public:
	YFILL(VGABASE *v,int y1,int y2);
	~YFILL();
	void inspole(int x,int y);
	void supple(void);
	void draw();
	void save(void);
	} ;

#endif

