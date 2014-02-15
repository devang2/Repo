class Palette:public Box
{
public :
	Palette(int x,int y,int Color);
	void Draw();
	int Run();
};

inline Palette::Palette(int x,int y,int Color)
	:Box(x,y,PALETTE_H,PALETTE_H)
{
	color = Color;
}

inline void Palette::Draw()
{
	gr.Box(bx,by,bw,bh,SOLID_FILL,color);
	gr.Rectangle(bx,by,bw,bh,WHITE);
}

inline int Palette::Run()
{
	if(mouse.buttons&LEFT){
		FrColor.SetCurColor(color);
		FrColor.Draw();
	} else if(mouse.buttons&RIGHT)
	{
		BkColor.SetCurColor(color);
		BkColor.Draw();
	}
	return 1 ;
}