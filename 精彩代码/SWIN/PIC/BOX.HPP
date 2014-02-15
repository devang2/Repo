class Box
{
public :
	int color ;
protected:
	int bx,by,bw,bh ;
public :
	Box(int x,int y,int w,int h);
	int ClickIn();
	virtual void Draw() = 0;
	virtual int Run() = 0;
};

inline Box::Box(int x,int y,int w,int h)
{
	bx = x  ; by = y  ; bw = w  ; bh = h  ;
}

inline int Box::ClickIn()
{
	if( (mouse.curx > bx && mouse.curx < (bx+bw) )
		&&(mouse.cury > by && mouse.cury < (by+bh) ))
	{
		return 1;
	}
	return 0;
}