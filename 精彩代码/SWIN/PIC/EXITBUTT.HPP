class ExitButt:public Button
{
public :
	ExitButt(int x,int y,int w,int h,char * caption);
	void Draw();
	int Run();
};

inline ExitButt::ExitButt(int x,int y,int w,int h,char * caption)
	:Button(x,y,w,h,caption)
{}

inline void ExitButt::Draw()
{
	Button::Draw();
}

inline int ExitButt::Run()
{
	if(Button::Run()) return -1;
	else return 1 ;
}
