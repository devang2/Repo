class DrawBoard:public Box
{
	int rx,ry ;
public :
	DrawBoard(int rx,int ry,int x1,int y1,int color);
	void Draw();
	int Run();
};

inline DrawBoard::DrawBoard(int Rx,int Ry,int x1,int y1,int Color)
	:Box(x1,y1,POINT_SIZE,POINT_SIZE)
{
	color = Color ;
	rx = Rx ; ry = Ry ;
}

inline void DrawBoard::Draw()
{
	int on_off = mouse.display ;
	mouse.Display(OFF);
	trueBoard.Draw(rx,ry,color);
	gr.Box(bx,by,bw,bh,SOLID_FILL,color);
	gr.Rectangle(bx,by,bw,bh,WHITE);
	mouse.Display(on_off);
}

int DrawBoard::Run()
{
	if(mouse.buttons&LEFT){
		if(FrColor.GetCur() == color) return 0 ;
		color = FrColor.GetCur() ;
	} else if(mouse.buttons&RIGHT) {
		if(BkColor.GetCur() == color) return 0 ;
		color = BkColor.GetCur() ;
	}
	DrawBoard::Draw();
	return 1;
}
