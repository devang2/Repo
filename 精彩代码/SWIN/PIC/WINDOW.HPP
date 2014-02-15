class Window:public Box
{
	int shadow ;
	int savebk ;
	IMG * bkimg ;
public :
	Window(int x,int y,int w,int h,int savebk,int shadow);
	~Window();
	void Draw();
	int Run(){ return 0 ;}
};

inline Window::Window(int x,int y,int w,int h,int Savebk,int Shadow)
	:Box(x,y,w,h)
{
	shadow = Shadow ; savebk = Savebk ;
}

void Window::Draw()
{
	int on_off = mouse.display ;
	mouse.Display(OFF);
	bkimg = new IMG ;
	if(shadow){
		if(savebk)	gr.GetImage(bx,by,bw+shadow,bh+shadow,bkimg);
		gr.Box(bx+shadow,by+shadow,bw,bh,CLOSE_DOT_FILL,BLUE);
//		gr.Rectangle(bx+shadow,by+shadow,bw,bh,YELLOW);
	} else {
		if(savebk) gr.GetImage(bx,by,bw,bh,bkimg);
	}
	gr.Box(bx,by,bw,bh,SOLID_FILL,GREEN);
	gr.Box(bx+2,by+2,bw-4,bh-4,9,RED);
	mouse.Display(on_off);
}

inline Window::~Window()
{
	if(savebk){
		int on_off = mouse.display ;
		mouse.Display(OFF);
		gr.PutImage(bx,by,bkimg,COPY_PUT);
		mouse.Display(on_off);
		delete bkimg;
	}
}