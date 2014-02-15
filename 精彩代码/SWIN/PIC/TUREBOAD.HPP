class TrueBoard
{
	int x1,y1 ;
public :
	TrueBoard(int ,int );
	void Draw(int rx,int ry,int color);
};

inline TrueBoard::TrueBoard(int X1,int Y1)
{
	x1 = X1 ; y1 = Y1 ;
}

inline void TrueBoard::Draw(int rx,int ry,int color)
{
	putpixel(x1+rx,y1+ry,color);
}

TrueBoard  trueBoard(TRUE_BOARD_X,TRUE_BOARD_Y);