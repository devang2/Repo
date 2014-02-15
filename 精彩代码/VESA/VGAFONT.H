#ifndef FONT_H
	#define FONT_H

#include <stdio.h>

class VGABASE;

enum font_type {
	ASCfont = 0,
	TXfont  = 1,
	HZfont  = 2 };

union FONThandle {
	void *mem;
	class XMS *xms;
	FILE *file;
	} ;

class FONT {
public:
	int OK;
	int where;
	long leng;
	union FONThandle handle;

	FONT(char *filename,int whe,VGABASE *v);
	~FONT();
	void setvga(VGABASE *v);
	void setsize(int hi,int wi,int xxi=0);
	void setsizeOnly(int hi,int wi,int xxi=0);
	void join(FONT *ftp1,FONT *ftp2=0);
	void cut(FONT *ftp);
	int gettexthigh();
	virtual int gettextwide(char *str);
	virtual void outtextxy(int x,int y,char *string);
	void putxy(int x,int y);
	void getxy(int &x,int &y);
	void outtext(char *string);
	void printf(char *fmt, ...);
	void sethollow(int k) { hollow=k; }
	virtual int getcharwide(unsigned char c);
	virtual int outchar(int x,int y,unsigned char *cp) = 0;


protected:
	static int TEXT_X,TEXT_Y;
	int type;
	int org_high,org_wide;
	int high,wide,xi;
	FONT *fonts[3];
	VGABASE *vga;
	int hollow;

	void *getbuf(long offset,int ll);
	void freebuf(void *buf);
	long getoffsetSmall(unsigned char *cp,int isize);
	} ;

class FONT_ASC : public FONT
	{
public:
	FONT_ASC(char *name,int whe,VGABASE *v);
	int gettextwide(char *string);
	void outtextxy(int x,int y,char *string);
	} ;


class ASC16 : public FONT_ASC
	{
public:
	ASC16(char *filename,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
	} ;

class ASC24 : public FONT_ASC
	{
public:
	ASC24(char *filename,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
	} ;

class HZ16 : public FONT
	{
public:
	HZ16(char *filename,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
protected:
	virtual void *getdata(unsigned char *cp);
	} ;

class HZ16_S : public HZ16
	{
public:
	HZ16_S(char *name,int whe,VGABASE *v) : HZ16(name,whe,v) { }
protected:
	void *getdata(unsigned char *cp)
		{ return getbuf( getoffsetSmall(cp,32), 32); }
	};

class HZ24 : public FONT
	{
public:
	HZ24(char *filename,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
protected:
	virtual void *getdata(unsigned char *cp);
	} ;

class TX24 : public HZ24
	{
public:
	TX24(char *name,int whe,VGABASE *v) : HZ24(name,whe,v)
		{ type=TXfont; }
protected:
	void *getdata(unsigned char *cp);
	} ;

class HZ24_S : public HZ24
	{
public:
	HZ24_S(char *name,int whe,VGABASE *v) : HZ24(name,whe,v)
		{ fonts[1]=this; }
protected:
	void *getdata(unsigned char *cp)
		{ return getbuf( getoffsetSmall(cp,72), 72); }
	} ;

class HZ32 : public FONT
	{
public:
	HZ32(char *filename,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
protected:
	virtual void *getdata(unsigned char *cp);
	} ;

class TX32 : public HZ32
	{
public:
	TX32(char *name,int whe,VGABASE *v) : HZ32(name,whe,v)
		{ type=TXfont; }
protected:
	void *getdata(unsigned char *cp);
	} ;

class HZ32_S : public HZ32
	{
public:
	HZ32_S(char *name,int whe,VGABASE *v) : HZ32(name,whe,v)
		{ fonts[1]=this; }
protected:
	void *getdata(unsigned char *cp)
		{ return getbuf( getoffsetSmall(cp,128), 128); }
	} ;

class HZ40 : public FONT
	{
public:
	HZ40(char *name,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
protected:
	virtual void *getdata(unsigned char *cp);
	} ;

class TX40 : public HZ40
	{
public:
	TX40(char *name,int whe,VGABASE *v) : HZ40(name,whe,v)
		{ type=TXfont; }
protected:
	void *getdata(unsigned char *cp);
	} ;

class HZ40_S : public HZ40
	{
public:
	HZ40_S(char *name,int whe,VGABASE *v) : HZ40(name,whe,v)
		{ fonts[1]=this; }
protected:
	void *getdata(unsigned char *cp)
		{ return getbuf( getoffsetSmall(cp,180), 180); }
	} ;

class ASCBC : public FONT_ASC
	{
public:
	ASCBC(char *filename,int whe,VGABASE *v);
	int gettextwide(char *string);
	int getcharwide(unsigned char c);
	int outchar(int x,int y,unsigned char *cp);
protected:
	void *getdata(int &wi,unsigned char c);
	} ;

class ASCSL : public FONT_ASC
	{
public:
	ASCSL(char *filename,int whe,VGABASE *v);
	int gettextwide(char *string);
	int getcharwide(unsigned char c);
	int outchar(int x,int y,unsigned char *cp);
protected:
	void *getdata(int &wi,unsigned char c);
	} ;

class HZUC : public FONT
	{
public:
	HZUC(char *filename,int whe,VGABASE *v);
	int outchar(int x,int y,unsigned char *cp);
protected:
	int *getdata(unsigned char *cp);
	int *translate(int n,unsigned char *data);
	void enlarge(int x0,int y0,int *xy);
	virtual long getoffsetUC(unsigned char *cp);
	} ;

class HZUC_S :public HZUC
	{
public:
	HZUC_S(char *name,int whe,VGABASE *v) : HZUC(name,whe,v) { }
protected:
	long getoffsetUC(unsigned char *cp)
		{ return getoffsetSmall(cp,6); }
	} ;

class HZUCK : public HZUC
	{
public:
	HZUCK(char *name,int whe,VGABASE *v) : HZUC(name,whe,v) { }
	int outchar(int x,int y,unsigned char *cp);
	} ;

class HZUCK_S :public HZUCK
	{
public:
	HZUCK_S(char *name,int whe,VGABASE *v) : HZUCK(name,whe,v) { }
protected:
	long getoffsetUC(unsigned char *cp)
		{ return getoffsetSmall(cp,6); }
	} ;


class TXUC : public HZUCK
	{
public:
	TXUC(char *name,int whe,VGABASE *v);
protected:
	long getoffsetUC(unsigned char *cp);
	} ;

class TXUC_S :public TXUC
	{
public:
	TXUC_S(char *name,int whe,VGABASE *v) : TXUC(name,whe,v) { }
protected:
	long getoffsetUC(unsigned char *cp)
		{ return getoffsetSmall(cp,6); }
	} ;


#endif

