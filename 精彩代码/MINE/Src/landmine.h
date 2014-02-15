/************* 加入头文件 ************/
#ifndef TYPE_H
   #define TYPE_H
   #include "type.h"
#endif

#ifndef CGRAPH_H
   #define CGRAPH_H
   #include "CGraph.h"
#endif

#ifndef CTIMER_H
   #define CTIMER_H
   #include "CTimer.h"
#endif

#ifndef CMOUSE_H
   #define CMOUSE_H
   #include "CMouse.h"
#endif

#ifndef FUNCTION_H
   #define FUNCTION_H
   #include "function.h"
#endif
/**************************************/








/************** 游戏中使用到的常量 *******************/
#define TIME_OUT            999  /* 超时时间         */
#define REFRESH_FREQ        100  /* 鼠标刷新频率     */
#define DOUBLE_CLICK_DELAY  35   /* 双击延迟时间     */

#define ROWS        34           /* 雷区的列数       */
#define LINES       17           /* 雷区的行数       */
#define START_X     9            /* 雷区的开始横坐标 */
#define START_Y     59           /* 雷区的开始纵坐标 */
#define UNIT_WIDTH  9            /* 雷区单元的宽度   */
#define UNIT_HEIGHT 8            /* 雷区单元的高度   */

#define GAME_LEVEL  9            /* 游戏难度等级为9  */
/*****************************************************/








/**************** 图片句柄结构体 *****************/
typedef struct Handle
{
   UINT * buffer;
   CZone  zone;
} CHandle;
/*************************************************/






/******************* 游戏参数 ********************/

/* 1、"游戏"菜单正常显示所使用的句柄和显示的区域 */
CHandle nGame;     
CZone   nGZone={7,17,28,28};

/* 2、鼠标停留在"游戏"菜单上，菜单显示所使用的句柄 */
CHandle mGame;     
CZone   mGZone={72,17,93,28};

/* 3、"帮助"菜单正常显示所使用的句柄和显示的区域 */
CHandle nHelp;     
CZone   nHZone={40,17,62,28};

/* 4、鼠标停留在"帮助"菜单上，菜单显示所使用的句柄 */
CHandle mHelp;
CZone   mHZone={97,17,119,28};

/* 5、"关闭"按钮正常显示所使用的句柄和显示的区域 */
CHandle nClose;
CZone   nCZone={306,2,317,13};

/* 6、鼠标停留在"关闭"按钮上，按钮显示所使用的句柄 */
CHandle mClose;    
CZone   mCZone={263,2,274,13};

/* 7、"帮助对话框"显示所用的句柄，以及对话框上按钮的区域 */
CHandle helpDialog;
CZone   HDZone={119,100,185,139};
CZone   OKZone={166,130,180,135};
CZone   HDCZone={180,101,184,105};

/* 8、"重试对话框"显示使用的句柄，以及对话框上按钮的区域 */
CHandle TryAgainDialog;
CZone   TADZone={119,147,185,186};
CZone   YesZone={131,173,148,180};
CZone   NoZone ={157,173,174,180};

/* 9、雷区单元的正常显示所使用的句柄和显示的区域 */
CHandle normal;
CZone   nZone={ START_X,  START_Y,  START_X+UNIT_WIDTH-1,  START_Y+UNIT_HEIGHT-1 };

/* 10、雷区单元显示地雷所使用的图片句柄和显示的区域 */
CHandle bomb;  
CZone   bZone={ START_X+UNIT_WIDTH*10, START_Y, START_X+UNIT_WIDTH*11-1, START_Y+UNIT_HEIGHT-1 };   

/* 11、雷区单元显示周围地雷数目(0~9)所使用的图片句柄和显示区域 */
CHandle number[10];    
CZone   numZone[10]={ { START_X+UNIT_WIDTH*11,  START_Y,   START_X+UNIT_WIDTH*12-1,  START_Y+UNIT_HEIGHT-1 }, 
                      { START_X+UNIT_WIDTH*1,   START_Y,   START_X+UNIT_WIDTH*2-1,   START_Y+UNIT_HEIGHT-1 }, 
                      { START_X+UNIT_WIDTH*2,   START_Y,   START_X+UNIT_WIDTH*3-1,   START_Y+UNIT_HEIGHT-1 }, 
                      { START_X+UNIT_WIDTH*3,   START_Y,   START_X+UNIT_WIDTH*4-1,   START_Y+UNIT_HEIGHT-1 },                       
					  { START_X+UNIT_WIDTH*4,   START_Y,   START_X+UNIT_WIDTH*5-1,   START_Y+UNIT_HEIGHT-1 },   
					  { START_X+UNIT_WIDTH*5,   START_Y,   START_X+UNIT_WIDTH*6-1,   START_Y+UNIT_HEIGHT-1 },   
					  { START_X+UNIT_WIDTH*6,   START_Y,   START_X+UNIT_WIDTH*7-1,   START_Y+UNIT_HEIGHT-1 },   
					  { START_X+UNIT_WIDTH*7,   START_Y,   START_X+UNIT_WIDTH*8-1,   START_Y+UNIT_HEIGHT-1 },   
					  { START_X+UNIT_WIDTH*8,   START_Y,   START_X+UNIT_WIDTH*9-1,   START_Y+UNIT_HEIGHT-1 },   
					  { START_X+UNIT_WIDTH*9,   START_Y,   START_X+UNIT_WIDTH*10-1,  START_Y+UNIT_HEIGHT-1 }  
					};

/* 12、时间显示的区域 */
CZone timeZone={11,36,36,48};

/* 13、reset按钮显示的区域 */
CZone resetZone={154,36,168,50};

/* 14、得分显示的区域 */
CZone scoreZone={284,36,309,50};

/* 15、雷区的区域 */
CZone mineField={ START_X, START_Y, START_X+UNIT_WIDTH*ROWS-1, START_Y+UNIT_HEIGHT*LINES-1 };

/* 16、二维数组用来记录雷区地雷的分布情况 */
Bool bombs[LINES][ROWS];

/* 17、玩家的得分 */
int  myScore=0;
/*************************************************/








/************* 游戏中使用到的函数 ****************/

/* 1、获取单个图片的句柄，保存在handle所指向的句柄，图像的区域为zone */
Bool getHandle(CHandle * handle, CZone zone)
{
   long size;
   int  x1, y1, x2, y2;

   x1=zone.leftUp.x;
   y1=zone.leftUp.y;
   x2=zone.rightDown.x;
   y2=zone.rightDown.y;

   size = imagesize(x1,y1,x2,y2);
   (*handle).buffer = (UINT *)malloc(size*sizeof(UINT));

   if( !(*handle).buffer )
   {
      return false;
   }
   else
   {
      (*handle).zone=zone;
      return getimage(x1,y1,x2,y2, (*handle).buffer );
   }
}


/* 2、获取游戏中需要用到的所有的图片的句柄 */
Bool getPicHandles()
{
	return ( getHandle(&nGame,nGZone)           &&  getHandle(&mGame,mGZone)          &&
             getHandle(&nHelp,nHZone)           &&  getHandle(&mHelp,mHZone)          &&
             getHandle(&nClose,nCZone)          &&  getHandle(&mClose,mCZone)         &&
             getHandle(&normal,nZone)           &&  getHandle(&number[0],numZone[0])  &&
             getHandle(&number[1],numZone[1])   &&  getHandle(&number[2],numZone[2])  &&
             getHandle(&number[3],numZone[3])   &&  getHandle(&number[4],numZone[4])  &&
             getHandle(&number[5],numZone[5])   &&  getHandle(&number[6],numZone[6])  &&
             getHandle(&number[7],numZone[7])   &&  getHandle(&number[8],numZone[8])  &&
             getHandle(&number[9],numZone[9])   &&  getHandle(&bomb,bZone)            &&
             getHandle(&helpDialog,HDZone)      &&  getHandle(&TryAgainDialog,TADZone)
			 );
}


/* 3、释放游戏中用到的所有的图片的句柄 */
void freePicHandles()
{
    int i;

    if(nGame.buffer)  {  free(nGame.buffer);   nGame.buffer=0;   }
    if(mGame.buffer)  {  free(mGame.buffer);   mGame.buffer=0;   }
    if(nHelp.buffer)  {  free(nHelp.buffer);   nHelp.buffer=0;   }
    if(mHelp.buffer)  {  free(mHelp.buffer);   mHelp.buffer=0;   }
    if(nClose.buffer) {  free(nClose.buffer);  nClose.buffer=0;  }
    if(mClose.buffer) {  free(mClose.buffer);  mClose.buffer=0;  }
    if(normal.buffer) {  free(normal.buffer);  normal.buffer=0;  }
	if(bomb.buffer)   {  free(bomb.buffer);    bomb.buffer=0;    }
	if(helpDialog.buffer)      {  free(helpDialog.buffer);  helpDialog.buffer=0; }
	if(TryAgainDialog.buffer)  {  free(TryAgainDialog.buffer);  TryAgainDialog.buffer=0; }

    for(i=0; i<10; i++)
    {
       if(number[i].buffer)
       {
           free(number[i].buffer);
           number[i].buffer=0;
       }
    }    
}



/* 4、在雷区的(row,line)处画地雷或提示雷数的数字，row的取值范围[0, ROWS-1]，line 的取值范围[0, LINES-1] */
Bool drawBoard(CHandle handle, int row, int line)
{
   int x,y;

   if( !handle.buffer )
   {
	   return false;
   }

   if( row<0 || row>ROWS-1 || line<0 || line>LINES-1 )
   {
	   return false;
   }

   x = START_X + row*UNIT_WIDTH ;
   y = START_Y + line*UNIT_HEIGHT;

   putimage( x, y, handle.buffer, COPY_PUT );

   return true;
}


/* 5、在显示时钟处显示时钟计数值，如果超时，返回false */
Bool showTime(Bool reset)
{
    static int count=0;
    static int time =0;

    char timeInStr[3];
    int  i;

    if(reset)
    {
        count=0;
        time =0;
    }

    count++;
    if( count>=REFRESH_FREQ )
    {
       count=0;
       time++;

       timeInStr[0]='0'+(time%10);         /* 个位数值 */
       timeInStr[1]='0'+( (time%100)/10 ); /* 十位数值 */
       timeInStr[2]='0'+(time/100);        /* 百位数值 */

       clearZone( timeZone.leftUp.x, timeZone.leftUp.y, timeZone.rightDown.x, timeZone.rightDown.y, 0 );

       for(i=2; i>=0; i--)  /* 先输出百位，然后输出十位，最后输出个位 */
       {
          outCharInRom( timeInStr[i],255, timeZone.leftUp.x+2+(2-i)*8, timeZone.leftUp.y+3, 1);
       }
    }

	/* 判断是否超时，如果超时，返回false，否则返回true */
    if( time< TIME_OUT )
    {
        return true;
    }
    else
    {
        return false;
    }
}


/* 6、在成绩显示处显示成绩score */
void showScore(int score)
{
	char scoreInStr[3];
	int  i;

	scoreInStr[0]='0'+(score%10);         /* 个位数值 */
	scoreInStr[1]='0'+( (score%100)/10 ); /* 十位数值 */
	scoreInStr[2]='0'+(score/100);        /* 百位数值 */

	clearZone( scoreZone.leftUp.x, scoreZone.leftUp.y, scoreZone.rightDown.x, scoreZone.rightDown.y, 0 );
	
	for(i=2; i>=0; i--)  /* 先输出百位，然后输出十位，最后输出个位 */
	{
		outCharInRom( scoreInStr[i],255, scoreZone.leftUp.x+2+(2-i)*8, scoreZone.leftUp.y+3, 1);
	}

}


/* 7、重启游戏，使游戏的参数还原为初始值 */
void reset()
{
	int line,row,i;
	RGB colors[256];

	/* 设置成绩为0 */
	myScore=0;

	/* 暂时使屏幕变黑，这样可以避免出现画面的闪烁 */
	getRegisters(0,256,colors);
	blackScreen(false);

	for(line=0; line<LINES; line++)
	for(row=0 ; row<ROWS  ; row++ )
	{
		i=rand()%GAME_LEVEL;   /* 根据不同的难度等级，雷区的每个单元格出现地雷的概率不一样 */

		if(0==i)
		{
			bombs[line][row]=true;
		}
		else
		{
			bombs[line][row]=false;
		}

		drawBoard( normal, row, line );
	}
	
	/* 此时雷区的所有图片都已经更新，应该更新鼠标的背景 */
	changeBackGround(mineField);

	/* reset时间显示 */
	showTime(true);

	/* 显示当前成绩 */
	showScore(myScore);

	/* 回复图像的正常显示 */	
	setRegisters(0,256,colors);

}


/* 8、初始化游戏( 包括游戏主界面和游戏参数 ) */
Bool initGame()
{
   int  feedBack;
   RGB  colors[256];

   /* 显示游戏主界面 */
   feedBack = showBMP256("picture\\main.bmp",0,0);

   /* 先保存图片的调色板寄存器 */
   getRegisters(0,256,colors);

   /* 使屏幕暂时变黑，在获取关键图片句柄之后，再恢复正常图片显示，这样可以消除屏幕的闪动 */
   blackScreen(false);

   if(true!=feedBack)
   {
	   return false;
   }

   /* 获取各个关键图片的句柄 */
   if ( getPicHandles() )
   {
       BYTE pixel;
       int  row, line;

       pixel=getpixel( mGame.zone.leftUp.x-1, mGame.zone.leftUp.y+1 );
       clearZone( mGame.zone.leftUp.x, mGame.zone.leftUp.y, mGame.zone.rightDown.x,mGame.zone.rightDown.y,pixel );
       clearZone( mHelp.zone.leftUp.x, mHelp.zone.leftUp.y, mHelp.zone.rightDown.x,mHelp.zone.rightDown.y,pixel );

       pixel=getpixel( mClose.zone.leftUp.x-1, mClose.zone.leftUp.y+1 );
       clearZone( mClose.zone.leftUp.x, mClose.zone.leftUp.y, mClose.zone.rightDown.x, mClose.zone.rightDown.y, pixel );

       reset();

       setRegisters(0,256,colors);
       return true;
   }
   else
   {
	   return false;
   }
}


/* 9、显示管理，用于管理"游戏"菜单、"帮助"菜单和"关闭"按钮的显示 */
enum MouseOnObject { NONE, GAME_MENU, HELP_MENU, CLOSE_BUTTON };
typedef int MouseOnObject;
void displayManage( MouseOnObject object )
{	
	static Bool gameMenuIsNormal=true;
	static Bool helpMenuIsNormal=true;
	static Bool closeIsNormal=true;

	switch(object)
	{
	    case  GAME_MENU     :    if(!helpMenuIsNormal)
								 {
									 putimage( nHZone.leftUp.x, nHZone.leftUp.y, nHelp.buffer, COPY_PUT );
									 changeBackGround(nHZone);
									 helpMenuIsNormal=true;
								 }

			                     if(!closeIsNormal)
								 {
									 putimage( nCZone.leftUp.x, nCZone.leftUp.y, nClose.buffer, COPY_PUT );
									 changeBackGround(nCZone);
									 closeIsNormal=true;
								 }

								 if(gameMenuIsNormal)
								 {
									 putimage( nGZone.leftUp.x, nGZone.leftUp.y, mGame.buffer, COPY_PUT );
									 changeBackGround(nGZone);
									 gameMenuIsNormal=false;
								 }
								 break;

		case  HELP_MENU     :    if(!gameMenuIsNormal)
								 {
									 putimage( nGZone.leftUp.x, nGZone.leftUp.y, nGame.buffer, COPY_PUT );
									 changeBackGround(nGZone);
									 gameMenuIsNormal=true;
								 }

			                     if(!closeIsNormal)
								 {
									 putimage( nCZone.leftUp.x, nCZone.leftUp.y, nClose.buffer, COPY_PUT );
									 changeBackGround(nCZone);
									 closeIsNormal=true;
								 }

								 if(helpMenuIsNormal)
								 {
									 putimage( nHZone.leftUp.x, nHZone.leftUp.y, mHelp.buffer, COPY_PUT );
									 changeBackGround(nHZone);
									 helpMenuIsNormal=false;
								 }
								 break;

		case  CLOSE_BUTTON  :    if(!gameMenuIsNormal)
								 {
									 putimage( nGZone.leftUp.x, nGZone.leftUp.y, nGame.buffer, COPY_PUT );
									 changeBackGround(nGZone);
									 gameMenuIsNormal=true;
								 }
			 
			                     if(!helpMenuIsNormal)
								 {
									 putimage( nHZone.leftUp.x, nHZone.leftUp.y, nHelp.buffer, COPY_PUT );
									 changeBackGround(nHZone);
									 helpMenuIsNormal=true;
								 }

								 if(closeIsNormal)
								 {
									 putimage( nCZone.leftUp.x, nCZone.leftUp.y, mClose.buffer, COPY_PUT );
									 changeBackGround(nCZone);
									 closeIsNormal=false;
								 }
								 break;

		default             :    if(!gameMenuIsNormal)
								 {
									 putimage( nGZone.leftUp.x, nGZone.leftUp.y, nGame.buffer, COPY_PUT );
									 changeBackGround(nGZone);
									 gameMenuIsNormal=true;
								 }

	                             if(!helpMenuIsNormal)
								 {
									 putimage( nHZone.leftUp.x, nHZone.leftUp.y, nHelp.buffer, COPY_PUT );
									 changeBackGround(nHZone);
									 helpMenuIsNormal=true;
								 }
                          
								 if(!closeIsNormal)
								 {
									 putimage( nCZone.leftUp.x, nCZone.leftUp.y, nClose.buffer, COPY_PUT );
									 changeBackGround(nCZone);
									 closeIsNormal=true;
								 }
								 break;
	}

}


/* 10、鼠标在雷区活动的处理函数 */
Bool inBoardZone(CPoint point, ClickInfor infor)
{
	displayManage(NONE);

	if( LEFT_CLICK==infor )  /* 在雷区有单击事件 */
    {
		int row,line;
		CZone z;

		/* 通过鼠标单击的坐标，来计算雷区中单元格的位置 */
		row  = (point.x-START_X)/UNIT_WIDTH;
		line = (point.y-START_Y)/UNIT_HEIGHT;		
		
		if( bombs[line][row])   /* 恭喜你，踩到地雷了!此时把所有的地雷都显示出来 */
		{
			for(line=0; line<LINES; line++)
			for(row=0;  row<ROWS;   row++)
			{
                if( bombs[line][row])
				{
					drawBoard(bomb,row,line);
					
					/* 同时更新鼠标的背景 */
					z.leftUp.x = START_X + UNIT_WIDTH*row ;
					z.leftUp.y = START_Y + UNIT_HEIGHT*line;
					z.rightDown.x=z.leftUp.x+UNIT_WIDTH-1;
					z.rightDown.y=z.leftUp.y+UNIT_HEIGHT-1;

					changeBackGround(z);
				}
			}			
			return false;
		}
		else
		{
			int bombNum=0;
			int lineTemp,rowTemp;

			for( lineTemp=line-1; lineTemp<=line+1; lineTemp++ )
            for( rowTemp =row-1 ; rowTemp<=row+1  ; rowTemp++  )
            {
                if( lineTemp>=0 && lineTemp<LINES && rowTemp>=0 && rowTemp<ROWS )
                {
					if(bombs[lineTemp][rowTemp])
					{
						bombNum++;
					}
                }
            }

			drawBoard( number[bombNum], row, line );

			/* 同时更新鼠标的背景 */			
			z.leftUp.x = START_X + UNIT_WIDTH*row ;
			z.leftUp.y = START_Y + UNIT_HEIGHT*line;
			z.rightDown.x=z.leftUp.x+UNIT_WIDTH-1;
			z.rightDown.y=z.leftUp.y+UNIT_HEIGHT-1;
				
			changeBackGround(z);

            /* 成绩增加 */
			myScore++;
			showScore(myScore);

			return true;
		}
	}
	else
	{
		return true;
	}
}


/* 11、鼠标在"关闭"按钮处活动 */
Bool inCloseZone(CPoint point, ClickInfor infor)
{
	displayManage(CLOSE_BUTTON);

	if(LEFT_CLICK==infor)
	{
		return false;
	}
	else
	{
		return true;
	}
}


/* 12、鼠标在"reset"按钮上活动 */
Bool inResetZone(CPoint point, ClickInfor infor)
{
	displayManage(NONE);

	if( LEFT_CLICK==infor )
	{
		reset();
	}

	return true;
}


/* 13、鼠标在"游戏"菜单上活动 */
Bool inGameZone(CPoint point, ClickInfor infor)
{
	displayManage(GAME_MENU);

	if( LEFT_CLICK==infor )
	{
		reset();
	}

	return true;
}


/* 14、鼠标在"帮助"菜单上活动 */
Bool inHelpZone(CPoint point, ClickInfor infor)
{	
	displayManage(HELP_MENU);

	if( LEFT_CLICK==infor )
	{
		long size=0;
		int  x1,y1,x2,y2;
		UINT * buffer=0;

		x1=HDZone.leftUp.x;
		y1=HDZone.leftUp.y;
		x2=HDZone.rightDown.x;
		y2=HDZone.rightDown.y;

		size  =imagesize(x1,y1,x2,y2);
        buffer=(UINT *)malloc(size*sizeof(UINT));

		if( !buffer )    /* 如果申请内存失败，则不显示帮助对话框，直接返回 */
		{
			return true;
		}
		else
		{
			Bool notQuit=true;
			CPoint      pt;
			ClickInfor  ci;

			getimage(x1,y1,x2,y2, buffer );
			putimage(x1,y1,helpDialog.buffer, COPY_PUT );
			changeBackGround(HDZone);

			/* "帮助对话框"是"模式对话框"，因此需要自己的鼠标事件监听机制 */
			while( notQuit )
			TIME_SLICE_BEGIN(1)
			{
				refreshMouse();

				if( !showTime( false ) )  /* 如果超时了，则返回false */
				{  
					return false;
				}

				ci = fetchClickInfor();
				pt = getMousePosition();

				if( LEFT_CLICK==ci)
				{
					if( inZone(OKZone,pt) || inZone(HDCZone,pt))
					{
						putimage(x1,y1,buffer,COPY_PUT );
						changeBackGround(HDZone);
						free(buffer);
						return true;
					}
				}
			}
			TIME_SLICE_END()
		}		
	}

	return true;
}


/* 15、踩到地雷了，询问是否继续游戏 */
Bool haveAnotherTry()
{
	long size=0;
	int  x1,y1,x2,y2;
	UINT * buffer=0;

	x1=TADZone.leftUp.x;
	y1=TADZone.leftUp.y;
	x2=TADZone.rightDown.x;
	y2=TADZone.rightDown.y;

	size  =imagesize(x1,y1,x2,y2);
    buffer=(UINT *)malloc(size*sizeof(UINT));

	if( !buffer )    /* 如果申请内存失败，则不显示重试对话框，直接返回false */
	{
		return false;
	}
	else
	{
		Bool  notQuit=true;
		
		CZone dlg,YesButton,NoButton;
		int   excurX=0, excurY=-45;       /* 实际显示时，"重试对话框"的坐标偏移量 */		

		CPoint      pt;
		ClickInfor  ci;

		dlg.leftUp.x    = TADZone.leftUp.x+excurX;
		dlg.leftUp.y    = TADZone.leftUp.y+excurY;
		dlg.rightDown.x = TADZone.rightDown.x+excurX;
		dlg.rightDown.y = TADZone.rightDown.y+excurY;

		YesButton.leftUp.x    = YesZone.leftUp.x+excurX;
		YesButton.leftUp.y    = YesZone.leftUp.y+excurY;
		YesButton.rightDown.x = YesZone.rightDown.x+excurX;
		YesButton.rightDown.y = YesZone.rightDown.y+excurY;

		NoButton.leftUp.x    = NoZone.leftUp.x+excurX;
		NoButton.leftUp.y    = NoZone.leftUp.y+excurY;
		NoButton.rightDown.x = NoZone.rightDown.x+excurX;
		NoButton.rightDown.y = NoZone.rightDown.y+excurY;


		getimage(dlg.leftUp.x, dlg.leftUp.y, dlg.rightDown.x, dlg.rightDown.y, buffer );
		putimage(dlg.leftUp.x, dlg.leftUp.y, TryAgainDialog.buffer, COPY_PUT );
		changeBackGround(dlg);

		/* "帮助对话框"是"模式对话框"，因此需要自己的鼠标事件监听机制 */
		while( notQuit )
		TIME_SLICE_BEGIN(1)
		{
			refreshMouse();

			ci = fetchClickInfor();
			pt = getMousePosition();

			if( LEFT_CLICK==ci)
			{
				if( inZone(YesButton,pt) || inZone(NoButton,pt))
				{
					putimage(dlg.leftUp.x, dlg.leftUp.y,buffer,COPY_PUT );
					changeBackGround(dlg);
					free(buffer);
					
					if( inZone(YesButton,pt) )
					{
						reset();
						return true;
					}
					else
					{
						return false;
					}
				}
			}
		}
		TIME_SLICE_END()
	}		
}

/* 15、对鼠标事件进行分析，然后分配给不同的对象，各对象都有相应的鼠标事件相应函数 */
Bool analysis(CPoint point, ClickInfor infor)
{
    CZone  boardZone;
    static Bool closeNormal=true;    

    if( inZone( mineField, point) )        /* 鼠标指着在“雷区” */
    {
        if( !inBoardZone(point, infor) )
		{
			return haveAnotherTry();
		}
		else
		{
			return true;
		}
    }
    else if( inZone(nCZone, point) )       /* 鼠标指针指向窗口的“关闭”按钮 */
    {
        return inCloseZone(point, infor);
    }
	else if( inZone(resetZone, point) )    /* 鼠标指针指向“reset”按钮 */
	{
		return inResetZone(point, infor);
	}
	else if( inZone(nGZone, point) )       /* 鼠标指针指向“游戏”菜单 */
	{
		return inGameZone(point, infor);
	}
	else if( inZone(nHZone, point) )       /* 鼠标指针指向“帮助”菜单 */
	{
		if( !inHelpZone(point, infor) )
		{
			return haveAnotherTry();
		}
		else
		{
			return true;
		}
	}
	else 
	{
		displayManage(NONE);
		return true;
	}
    
}

/*************************************************/
