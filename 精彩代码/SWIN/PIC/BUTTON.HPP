class Button:public Box
{
	int exitcode ;
	char * caption ;
public :
	Button(int x,int y,int w,int h,char * Caption,int exitCode);
	void Draw();
	int Run();
};

inline Button::Button(int x,int y,int w,int h,char *Caption,int exitCode=1)
	:Box(x,y,w,h)
{
	caption = new char[strlen(Caption)+1];
	strcpy(caption,Caption);
	exitcode = exitCode ;
};

void Button::Draw()
{
	gr.Box(bx+4,by+4,bw,bh,SOLID_FILL,BLACK);
	gr.Box(bx,by,bw+2,bh+2,SOLID_FILL,BLUE);
	gr.Box(bx,by,bw,bh,SOLID_FILL,RED);
	gr.SetColor(BLACK);
	gr.Outtext(bx+(bw-gr.TextWidth(caption))/2,
					by+(bh-gr.TextHeight(caption))/2,caption);
}

int Button::Run()
{
	int on_off = mouse.display;
	int runit = NORUN ;
	mouse.Display(OFF);

	char * buff = new char[imagesize(bx,by,bx+bw,by+bh)];
	getimage(bx,by,bx+bw,by+bh,buff);
	gr.Box(bx,by,bw+2,bh+2,SOLID_FILL,BLUE);
	putimage(bx+2,by+2,buff,XOR_PUT);
	while(event.Present());
	if(Box::ClickIn()) runit = exitcode ;
	else runit = CLICK_ONLY;
	mouse.Display(OFF);
	gr.Box(bx,by,bw+2,bh+2,SOLID_FILL,BLUE);
	putimage(bx,by,buff,COPY_PUT);

	mouse.Display(on_off);
	delete buff ;
	return runit;
}

