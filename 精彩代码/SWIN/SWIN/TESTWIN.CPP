#include "windows.hpp"
#include "button.hpp"
#include "popmenu.hpp"
#include "pullmenu.hpp"
#include "icobut.hpp"
#include "textbut.hpp"
#include "ico.hpp"

class CapWin:public Windows
{
	DbList menuList ;
public :
	CapWin(int x,int y,int w,int h,char * caption,int objnum=1);
	void AddMenu(Box * menu);
	void Show();
	int Run();
	void Setxy(int x,int y);
};

CapWin::CapWin(int x,int y,int w,int h,char * caption,int objnum)
	:Windows(x,y,w,h,caption,objnum)
{}

void CapWin::AddMenu(Box * menu)
{
	menu->Setxy(wx+menu->x(),wy+menu->y());
	menuList.AtachToTail(menu);
}

void CapWin::Setxy(int x,int y)
{
	int dx = x - wx ;
	int dy = y - wy ;
	Windows::Setxy(x,y);
	if(!menuList.IsEmpty()) {
		Node * skipNode = menuList.ListHead ;
		do{
			skipNode->element->Setxy(dx+skipNode->element->x(),
											 dy+skipNode->element->y());
			skipNode = skipNode->next ;
		}while(skipNode != menuList.ListHead);
	}
}

void CapWin::Show()
{
	Windows::Show();
	unsigned on_off = mouse.display ;
	if(on_off == ON) mouse.Display(OFF);
//---------------------------------------------------------------
	gr.Box(wx+20,wy+70,360,200,SOLID_FILL,WHITE);
	gr.Rect3d(wx+20,wy+70,360,200,0);
	gr.Rect3d(wx+20,wy+280,360,20,0);

	ShowIco(wx+30,wy+80,"ico\\setup.ico");
	ShowIco(wx+70,wy+80,"ico\\printer.ico");
	ShowIco(wx+110,wy+80,"ico\\clock.ico");

	ShowIco(wx+150,wy+80,"ico\\date.ico");
	ShowIco(wx+190,wy+80,"ico\\control.ico");
	ShowIco(wx+230,wy+80,"ico\\camara.ico");
	ShowIco(wx+270,wy+80,"ico\\box2.ico");
	ShowIco(wx+310,wy+80,"ico\\caculor.ico");

	ShowIco(wx+30,wy+120,"ico\\write.ico");
	ShowIco(wx+70,wy+120,"ico\\taper.ico");
	ShowIco(wx+110,wy+120,"ico\\paper.ico");

//--------------------------------------------------------
	gr.Box(wx+5,wy+5,ww-10,20,SOLID_FILL,BLUE);
	gr.SetColor(WHITE);
	gr.Outtext(wx+(ww-gr.TextWidth(title))/2,
					wy+5+(20-gr.TextHeight(title))/2,title);
	menuList.ShowAllObj() ;
	mouse.Display(on_off);
}

int CapWin::Run()
{
	if(buttList.IsEmpty()&&menuList.IsEmpty()) return 0 ;
	CapWin::Show();
	buttList.Locate(_objnum);
	while(exitcode != EXIT){
		while(!event.Present());
		if(event.type == KEYBD){
			switch(event.key)
			{
			case 0x0f09 :buttList.SkipNext();continue;
			case 0x0f00 :buttList.SkipPrev();continue;
			case 0x011b :exitcode = EXIT ;continue ;
			}
		}
		exitcode = buttList.CheckEvent();
		if(exitcode == NOCLICK)
			exitcode = menuList.CheckEvent();
		if(exitcode != NOCLICK){
			gr.SetColor(BLACK);
			gr.Box(wx+22,wy+282,360-4,16,SOLID_FILL,LIGHTGRAY);
			gr.Outtext(wx+22,wy+282,"结果:选择对象的返回值为:%d",exitcode);
		}
	}
	Windows::Hide();
	return exitcode ;
}

MenuItem * pop0[3] ,* pop1[3],* pop2[3],* pop3[3];

PullMenu * pullmenu1;

int About()
{
	Windows win(200,150,240,200,"关于");
	win.AddButton(new Button(90,150,60,26,"确定",EXIT));
	win.Show();
	gr.Rect3d(220,170,200,120,0);
	gr.SetColor(WHITE);
	gr.Outtext(236,201,"Supper Windows 作者:");
	gr.SetColor(BLUE);
	gr.Outtext(235,200,"Supper Windows 作者:");
	ShowIco(300,240,"ico\\tanh.ico");
	win.Run();
	return 1 ;
}

void main()
{
	gr.Box(0,0,640,480,SOLID_FILL,BLUE);
	MenuItem * fileitem[] = {
		new MenuItem(0,4,120,20,"  新建文件  ",11),
		new MenuItem(0,28,120,20,"  打开文件  ",12),
		new MenuItem(0,50,120,0,"",COMMENT),
		new MenuItem(0,54,120,20,"  退    出  ",EXIT)
	};


	MenuItem * diskitem[] = {
		new MenuItem(0,4,120,20,"  格式化  ",21),
		new MenuItem(0,28,120,20,"  编  辑  ",22),
		new MenuItem(0,52,120,20,"  建目录  ",23)
	};
	diskitem[1]->Disable(true);

	MenuItem * disk[] ={
		new MenuItem(0,2,120,20,"  HardDisk   ",311),
		new MenuItem(0,22,120,20,"  FloatDisk  ",312),
		new MenuItem(0,42,120,20,"  Unknow     ",313)
	};
	PopMenu * disksub = new PopMenu(120,0,120,64,disk,3);

	MenuItem * reportitem[] = {
		new MenuItem(0,4,120,20,"  磁  盘\x10 ",31,disksub),
		new MenuItem(0,28,120,20,"  内  存  ",32),
		new MenuItem(0,50,120,0,"",COMMENT),
		new MenuItem(0,54,120,20,"  中  断  ",33)
	};

	PopMenu * filepop=new PopMenu(0,20,120,78,fileitem,4);
	PopMenu * diskpop=new PopMenu(60,20,120,76,diskitem,3);
	PopMenu * reportpop=new PopMenu(120,20,120,78,reportitem,4);

	pop0[0] = new MenuItem(0,0,60,20,"F.文件",1,filepop,0x2100);
	pop0[1] = new MenuItem(60,0,60,20,"D.磁盘",2,diskpop,0x2000);
	pop0[2] = new MenuItem(120,0,60,20,"R.报告",3,reportpop,0x1300);

	pullmenu1 = new PullMenu(10,30,180,20,pop0,3);
	CapWin win(10,10,400,400,"下拉菜单演示");
	win.AddMenu(pullmenu1);
	win.AddButton(new IcoButton(40,305,40,40,"ico\\setup.ico",1));
	win.AddButton(new Button(100,310,80,26,"A.关于",2,0x1e01,About));
	win.AddButton(new Button(200,310,80,26,"E.退出",EXIT,0x1205));
	win.AddButton(new IcoButton(300,305,40,40,"ico\\camara.ico",3));
	win.AddButton(new Text(20,350,44));
	win.Setxy(100,40);

	win.Run();
}
