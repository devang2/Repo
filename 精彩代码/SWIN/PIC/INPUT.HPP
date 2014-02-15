class Input:public Box
{
	char * bkbuff;
	int textlen ;
	int maxchars ;
	char * caption ;
	char * text ;
public :
	Input(int x,int y,int w,int h,char * Caption,int maxtext);
	~Input();
	void Draw();
	int Run();
	char * GetText(){return text ;}
	void Display();
	void Display(char * msg);
};

Input::Input(int x,int y,int w,int h,char * Caption,int maxtext)
	:Box(x,y,w,h)
{
	bkbuff = 0 ;
	text = new char[maxtext+1] ;
	if(!text) printf("text no alloc");
	text[0]= 0 ;
	textlen = 0 ;
	maxchars = maxtext ;
	caption = new char[strlen(Caption)+1];
	if(!caption) printf("caption error");
	strcpy(caption,Caption);
}

Input::~Input()
{
	if(caption)
		delete caption ;
	if(text)
		delete text ;
	if(bkbuff){
		putimage(bx,by,bkbuff,COPY_PUT);
		delete bkbuff ;
	}
}

void Input::Display()
{
	gr.Box(bx+2,by+2,bw-4,bh-4,9,RED);
	gr.SetColor(YELLOW);
	gr.Outtext(bx+(bw-gr.TextWidth(caption)-maxchars*gr.TextWidth("H"))/2,
						by+(bh-gr.TextHeight("H"))/2,
				"%s%s%c",caption,text,'_');
}

void Input::Display(char * msg)
{
	gr.Box(bx+2,by+2,bw-4,bh-4,9,RED);
	gr.SetColor(YELLOW);
	gr.Outtext(bx+(bw-gr.TextWidth(msg))/2,by+(bh-gr.TextHeight("H"))/2,msg);
}

void Input::Draw()
{
	bkbuff = new char[imagesize(bx,by,bw+bx+3,bh+by+3)];
	if(!bkbuff) {
		printf("memory not enough");
		exit (1);
	}
	getimage(bx,by,bx+bw+3,by+bh+3,bkbuff);
	gr.Box(bx+3,by+3,bw,bh,SOLID_FILL,BLACK);
	gr.Box(bx,by,bw,bh,SOLID_FILL,GREEN);
	Input::Display();
}

int Input::Run()
{
	while(1){
		while(!event.Present());
		if(event.type == KEYBD)
		{
			switch(event.key)
			{
			case 0x0e08 :
				if(textlen){
					textlen --;
					text[textlen] = 0;
					Input::Display();
				}
				break;
			case 0x011b :
				return NORUN ;
			case 0x1c0d :
				return RUN ;
			default :
				char c = event.key ;
				if( c > ' ' && c<'~'){
					if(textlen < Input::maxchars){
						text[textlen] = c ;
						textlen ++;
						text[textlen] = 0 ;
						Input::Display();
					}
				}
			}
		}
	}
}