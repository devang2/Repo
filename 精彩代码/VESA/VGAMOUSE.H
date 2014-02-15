#ifndef VGAMOUSE_H
	#define VGAMOUSE_H

class VGABASE;

struct MouseEvent {
	unchar buttons;
	int x,y;
	int keystate;
	} ;

struct MessageEvent {
	unsigned command;
	union {
		void far *infoPtr;
		long infoLong;
		unsigned int infoWord;
		int infoInt;
		unsigned char infoByte[2];
		char infoChar[2];
		};
	};

class Event
	{
public:
	unsigned what;
	union {
		int key;
		MessageEvent message;
		MouseEvent mouse;
		};
	void getevent(void);
	void getevent(unsigned mask);
	void clear(void);
	void postmessage(void);
	void clearpostbox(void);
private:
	void getMouseEvent();
	void getKeyEvent();
	void getMessageEvent();
	};


class MOUSE {
	friend class MouseMap;
public:
	static int OK;
	static void getevent( Event& );
	static int init(void);
	static void hide(void);
	static void show(void);
	static void getxy(int &x,int &y);
	static void putxy(int x,int y);
	static void putcentre(void);
	static int inarea(int left,int top,int right,int bottom);
	static void setregion(int left,int top,int right,int bottom);
	static void restoreregion(void);
	static void setspeed(int n);
	static void setdoubletime(int n) { doubleInterval=n; }
	static void setautotime(int n) { autoInterval=n; }
private:
	static class MouseMap *mmap;
	static int showState;
	static int max_X,max_Y;
	static int centre_X,centre_Y;

	static long lastDownTime,lastDownTimeR,lastDownTimeM;
	static int doubleInterval,autoInterval;

	static void getstate( struct MouseState& );
	static void mouse_intr(int&,int&,int&,int&);
	static void mouse_intr(unsigned,unsigned,unsigned,unsigned,unsigned);

} ;

class MouseMap {
	friend class MOUSE;
public:
	MouseMap(VGABASE *v) { vga=v; }
	~MouseMap();
	void setup(void);
protected:
	VGABASE *vga;
	void *buf;
	int wide,high;
	int centre_X,centre_Y;
	int x1,y1;
	unsigned activMask;

	void putback(void);
	void show(int x0,int y0);
	virtual void active(Event evt) { }
	virtual void init(void) { }
	virtual void draw(int x0,int y0)=0;
	} ;

class MouseMap16 : public MouseMap
	{
public:
	MouseMap16(VGABASE *v);
protected:
	void draw(int x0,int y0);
	} ;


class MouseMap256 : public MouseMap
	{
public:
	MouseMap256(VGABASE *v);
protected:
	class VGA256 *v256;
	void init(void);
	void active(Event evt);
	void draw(int x0,int y0);
	} ;


class MouseMap16M : public MouseMap
	{
public:
	MouseMap16M(VGABASE *v);
protected:
	class VGA16M *vt;
	void draw(int x0,int y0);
	} ;

class MouseMapHigh : public MouseMap
	{
public:
	MouseMapHigh(VGABASE *v);
	~MouseMapHigh();
protected:
	int *xy;
	void draw(int x0,int y0);
	} ;

void setup_key_mouse(void);
void del_key_mouse(void);


#endif

