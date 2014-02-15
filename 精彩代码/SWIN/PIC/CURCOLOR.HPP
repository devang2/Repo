class CurColor
{
	int x1, y1;
	int color ;
public :
	CurColor(int,int,int color);
	int GetCur(){return color;}
	void SetCurColor(int Color){ color = Color; }
	void Draw();
};

inline CurColor::CurColor(int X1,int Y1,int Color)
{
	x1 = X1 ; y1 = Y1 ;
	color = Color ;
}

inline void CurColor::Draw()
{
	gr.Box(x1,y1,CUR_COLOR_W,CUR_COLOR_H,SOLID_FILL,color);
	gr.Rectangle(x1-1,y1-1,CUR_COLOR_W+2,CUR_COLOR_H+2,RED);
}

CurColor FrColor(FR_COLOR_X,FR_COLOR_Y,BLACK);
CurColor BkColor(BK_COLOR_X,BK_COLOR_Y,BLACK);
