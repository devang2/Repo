#define  SET_PALETTE

#define RESET			7
#define ABOUT			6
#define HELP			5
#define LOAD			4
#define SAVE			3
#define CLICK_ONLY	2
#define RUN				1
#define NORUN			0
#define EXIT			-1

#define COLX			32
#define ROWY			32

#define BOARD_X		20
#define BOARD_Y		20
#define POINT_SIZE	10

#define PALETTE_X		10
#define PALETTE_Y 	400
#define PALETTE_H		20
#define PALETTE_W		20

#define FR_COLOR_X	10
#define FR_COLOR_Y	390

#define CUR_COLOR_H	5
#define CUR_COLOR_W  170

#define BK_COLOR_X	188
#define BK_COLOR_Y	390

#define TRUE_BOARD_X	400
#define TRUE_BOARD_Y	300
#define TRUE_BOARD_W	COLX
#define TRUE_BOARD_H	ROWY
#include <stdlib.h>
#include "gui.hpp"
#include "event.hpp"
#include "box.hpp"
#include "curcolor.hpp"
#include "palette.hpp"
#include "tureboad.hpp"
#include "drawboad.hpp"
#include "window.hpp"
#include "button.hpp"
#include "list.hpp"
#include "input.hpp"

struct PicHeader
{
	char picid[4];					//allway "PIC"
	int width ;
	int height ;
};

void PutIco(int x,int y,char * filename)
{
	PicHeader picheader ;
	FILE * fp = fopen(filename,"rb");
	if(fp==NULL) return ;
	fread((char *)&picheader,1,sizeof(PicHeader),fp);
	if(strcmp(picheader.picid,"ICO")!=0){
		fclose(fp);
		return ;
	}
	int size = picheader.width*picheader.height;
	char * buff = new char[size];
	fread(buff,1,size,fp);
	fclose(fp);
	for(int i = 0 ;i<picheader.height;i++)
		for(int j=0;j<picheader.width;j++){
			int color = buff[i*picheader.height+j];
			if(color) putpixel(x+j,y+i,color);
		}
	delete buff ;
}
//-------------------------------------------------------
class Screen
{
	char picname[13] ;
	List ObjList ;
public :
	Screen();
	void Draw();
	int Run();
	void AddObj(Box * obj);
	void Save();
	void Load();
	void Reset();
	void Help();
	void About();
};

void Screen::Help()
{
	Window * about = new Window(100,100,322,322,1,5) ;
	about->Draw();
	setviewport(102,102,100+320,100+320,1);
	gr.SetColor(YELLOW);
	gr.Outtext(0,10,"    这个程序是谭文鸿在一个寂莫的晚上编写\n");
	gr.Outtext("的。本来想找个姑娘玩玩，可是没一个愿意和\n");
	gr.Outtext("我玩，于是干脆不去跳舞，坐下来写点程序，\n");
	gr.Outtext("经过一个晚上的异想天开，终于完成了。\n\n");
	gr.Outtext("     谢谢！\n\n\n\n");
	gr.Outtext("                       1996.12.28");
	PutIco(hz.Getx()-60,hz.Gety()-32,"ico\\printer.ico");
	while(!event.Present());
	setviewport(0,0,getmaxx(),getmaxy(),1);
	delete about ;
}

void Screen::About()
{
	Window * about = new Window(100,100,322,322,1,5) ;
	about->Draw();
	setviewport(102,102,100+320,100+320,1);
	gr.SetColor(YELLOW);
	gr.Outtext(0,10,"    这个程序是谭文鸿在一个寂莫的晚上编写\n");
	gr.Outtext("的。本来想找个姑娘玩玩，可是没一个愿意和\n");
	gr.Outtext("我玩，于是干脆不去跳舞，坐下来写点程序，\n");
	gr.Outtext("经过一个晚上的意想天开，终于完成了。\n");
	gr.Outtext("    不过这个程序写得比较粗糙，只是一个测\n"
				  "试版容错功能较差，如果你对该程序有兴趣，\n");
	gr.Outtext("可以与本人联系：\n");
	gr.Outtext("广西农工商职业大学计算中心  谭文鸿\n"
				  "邮编：５３０２６７ 电话：(0771-)4217172");
	gr.Outtext("     谢谢！\n\n\n\n");
	gr.Outtext("                    1996.12.28");
	PutIco(hz.Getx()-60,hz.Gety()-32,"ico\\control.ico");
	while(!event.Present());
	setviewport(0,0,getmaxx(),getmaxy(),1);
	delete about ;
}

void Screen::Reset()
{
	int on_off = mouse.display ;
	mouse.Display(OFF);
	ObjList.LocatePicStart();
	for(int i = ObjList.picStart ;i<=ObjList.picEnd;i++){
		ObjList.CurrNode->item->color = BkColor.GetCur();
		ObjList.CurrNode->item->Draw();
		ObjList.CurrNode=ObjList.CurrNode->next ;
	}
	mouse.Display(on_off);
}


Screen::Screen()
{
	picname[0] = 0 ;
	Screen::AddObj(
		new Window(BOARD_X-3,BOARD_Y-2,COLX*POINT_SIZE+6,
				ROWY*POINT_SIZE+6,0,10)
	);
	Screen::AddObj(
		new Window(TRUE_BOARD_X-2,TRUE_BOARD_Y-2,COLX+3,ROWY+3,0,3)
	);
	Screen::AddObj(new Button(10,440,60,20,"退出",EXIT));
	Screen::AddObj(new Button(72,440,60,20,"清除",RESET));
	Screen::AddObj(new Button(134,440,60,20,"存盘",SAVE));
	Screen::AddObj(new Button(196,440,60,20,"读入",LOAD));
	Screen::AddObj(new Button(258,440,60,20,"帮助",HELP));
	Screen::AddObj(new Button(320,440,60,20,"关于",ABOUT));


	ObjList.picStart = ObjList.totalNode+1;
	for(int y = 0; y < ROWY; y++){
		for(int x = 0 ; x < COLX ; x++){
			Screen::AddObj(
				new DrawBoard(x,y,BOARD_X+x*POINT_SIZE,
						BOARD_Y+y*POINT_SIZE,BkColor.GetCur())
			);
		}
	}
	ObjList.picEnd = ObjList.totalNode;
	for(int i = 0 ;i < 16 ;i++)
		Screen::AddObj(new Palette(PALETTE_X+i*(PALETTE_H+2),PALETTE_Y,i));
}

void Screen::Save()
{
	Input * save = new Input(380,100,200,80,"文件名:",12);
	save->Draw();
	if(save->Run()==RUN){
		FILE * savefp = fopen(save->GetText(),"wb");
		if(!savefp) {
			save->Display("不能打开该文件。");
			delay(1000);
		}else {
			PicHeader ph ;
			strcpy(ph.picid,"ICO");
			ph.width = COLX ;
			ph.height = ROWY ;
			fwrite((char *)&ph,1,sizeof(PicHeader),savefp);
			ObjList.LocatePicStart();
			for(int i = ObjList.picStart ;i<=ObjList.picEnd;i++){
				fputc(ObjList.CurrNode->item->color,savefp);
				ObjList.CurrNode=ObjList.CurrNode->next ;
			}
			fclose(savefp);
		}
	}
	delete save ;
}

void Screen::Load()
{
	Input * load = new Input(380,100,200,80,"文件名:",12);
	load->Draw();
	if(load->Run()==RUN){
		FILE * loadfp = fopen(load->GetText(),"rb");
		if(!loadfp) {
			load->Display("不能打开此文件。");
			delay(1000);
		}else {
			PicHeader ph ;
			fread((char *)&ph,1,sizeof(PicHeader),loadfp);
			if((strcmp(ph.picid,"ICO")!=0)||
				ph.width!=COLX||ph.height!=ph.height)
			{
				load->Display("不是ICO格式文件");
				delay(1000);
			} else {
				ObjList.LocatePicStart();
				int on_off = mouse.display ;
				mouse.Display(OFF);
				for(int i = ObjList.picStart ;i<=ObjList.picEnd;i++){
					ObjList.CurrNode->item->color=getc(loadfp);
					ObjList.CurrNode->item->Draw();
					ObjList.CurrNode=ObjList.CurrNode->next ;
				}
				mouse.Display(on_off);
			}
			fclose(loadfp);
		}
	}
	delete load ;
}

void Screen::AddObj(Box * obj)
{
	Node * addNode = new Node ;
	addNode->item = obj ;
	if(!ObjList.ListHead){
		ObjList.ListHead = ObjList.ListTail = addNode ;
	} else {
		ObjList.ListTail->next = addNode ;
		ObjList.ListTail = addNode ;
	}
	ObjList.ListTail->next = 0 ;
	ObjList.totalNode++;
}


void Screen::Draw()
{
	FrColor.Draw();
	BkColor.Draw();
	ObjList.CurrNode = ObjList.ListHead;
	while(ObjList.CurrNode){
		ObjList.CurrNode->item->Draw();
		ObjList.CurrNode = ObjList.CurrNode->next ;
	}
	gr.SetTextStyle(TRIPLEX_FONT,HORIZ_DIR,6);
	gr.SetColor(BLACK);
	gr.Outtext(402,12,"ICO EDIT");
	gr.SetColor(YELLOW);
	gr.Outtext(400,10,"ICO EDIT");
	gr.SetTextStyle(HZ_FONT);
	gr.SetColor(BLACK);
	gr.Outtext(382, 82,"         Version 1.0a         ");
	gr.Outtext(382,102,"(c) Copyright Sprite Software,");
	gr.Outtext(382,122,"             Inc              ");
	gr.Outtext(382,142,"    Program by");
	gr.Outtext(381,232,"←-- 图形编辑板");
	gr.Outtext(452,312,"←-- 真实图形板");
	gr.Outtext(382,402,"←-- 调色板");
	gr.Outtext(62,367,"左键色");
	gr.Outtext(232,367,"右键色");
	gr.Outtext(402,442,"←-- 主菜单");
	gr.SetColor(YELLOW);
	gr.Outtext(380, 80,"         Version 1.0a         ");
	gr.Outtext(380,100,"(c) Copyright Sprite Software,");
	gr.Outtext(380,120,"             Inc              ");
	gr.Outtext(380,140,"    Program by");
	PutIco(hz.Getx()+8,hz.Gety()-8,"ico\\tanh.ico");
	gr.Outtext(380,230,"←-- 图形编辑板");
	gr.Outtext(450,310,"←-- 真实图形板");
	gr.Outtext(380,400,"←-- 调色板");
	gr.Outtext(60,365,"左键色");
	gr.Outtext(230,365,"右键色");
	gr.Outtext(400,440,"←-- 主菜单");
	PutIco(hz.Getx()+8,hz.Gety()-8,"ico\\setup.ico");
}

int Screen::Run()
{
	int exit = 0 ;
	while(exit >= 0){
		while(!event.Present());
		if(event.type == KEYBD){
			if(event.key == 0x011b) exit = EXIT;
			else if(event.key == 0x1372) exit = RESET ;
		} else if(event.type == MBUTTON)
		{
			ObjList.CurrNode = ObjList.ListHead;
			while(ObjList.CurrNode){
				if(ObjList.CurrNode->item->ClickIn()){
					exit = ObjList.CurrNode->item->Run();
					if(exit == SAVE)
						Screen::Save();
					else if(exit == EXIT)
						return EXIT ;
					else if(exit == LOAD)
						Screen::Load();
					else if(exit == RESET)
						Screen::Reset();
					else if(exit == ABOUT)
						Screen::About();
					else if(exit == HELP)
						Screen::Help();
					else if(exit == CLICK_ONLY)
						break ;
				}
				ObjList.CurrNode = ObjList.CurrNode->next ;
			}
		}
	}
	return exit ;
}

#ifdef SET_PALETTE

void SetPalette()
{
	FILE * palfp = fopen("swin.pal","rb");
	if(!palfp){
		printf("Can not found SWIN.PAL\n");
		return ;
	}
	char palette[48] ;
	fread(palette,1,48,palfp);
	fclose(palfp);
	union REGS regs ;
	struct SREGS sregs ;
	for( int i = 0 ; i< 16 ; i ++ )
	{
		regs.h.ah = 0x10 ;
		regs.h.al = 0x00 ;
		regs.h.bh = i ;
		regs.h.bl = i ;
		int86(0x10,&regs,&regs);
	}

	for( i = 0 ; i < 48 ; i ++ ){
		palette[i] = palette[i] >> 2 ;
	}

	regs.x.ax = 0x1012 ;
	regs.x.bx = 0 ;
	regs.x.cx = 16 ;
	regs.x.dx = FP_OFF(palette);
	sregs.es = FP_SEG(palette);
	int86x(0x10,&regs,&regs,&sregs);
}
#endif
void main()
{
	gr.Box(0,0,639,479,SOLID_FILL,BLUE);
#ifdef SET_PALETTE
	SetPalette();
#endif
	mouse.Display(OFF);
	Screen * screen = new Screen;
	screen->Draw();
	screen->Run();
	delete screen ;
}
