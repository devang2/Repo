/************* ����ͷ�ļ� ************/
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








/************** ��Ϸ��ʹ�õ��ĳ��� *******************/
#define TIME_OUT            999  /* ��ʱʱ��         */
#define REFRESH_FREQ        100  /* ���ˢ��Ƶ��     */
#define DOUBLE_CLICK_DELAY  35   /* ˫���ӳ�ʱ��     */

#define ROWS        34           /* ����������       */
#define LINES       17           /* ����������       */
#define START_X     9            /* �����Ŀ�ʼ������ */
#define START_Y     59           /* �����Ŀ�ʼ������ */
#define UNIT_WIDTH  9            /* ������Ԫ�Ŀ��   */
#define UNIT_HEIGHT 8            /* ������Ԫ�ĸ߶�   */

#define GAME_LEVEL  9            /* ��Ϸ�Ѷȵȼ�Ϊ9  */
/*****************************************************/








/**************** ͼƬ����ṹ�� *****************/
typedef struct Handle
{
   UINT * buffer;
   CZone  zone;
} CHandle;
/*************************************************/






/******************* ��Ϸ���� ********************/

/* 1��"��Ϸ"�˵�������ʾ��ʹ�õľ������ʾ������ */
CHandle nGame;     
CZone   nGZone={7,17,28,28};

/* 2�����ͣ����"��Ϸ"�˵��ϣ��˵���ʾ��ʹ�õľ�� */
CHandle mGame;     
CZone   mGZone={72,17,93,28};

/* 3��"����"�˵�������ʾ��ʹ�õľ������ʾ������ */
CHandle nHelp;     
CZone   nHZone={40,17,62,28};

/* 4�����ͣ����"����"�˵��ϣ��˵���ʾ��ʹ�õľ�� */
CHandle mHelp;
CZone   mHZone={97,17,119,28};

/* 5��"�ر�"��ť������ʾ��ʹ�õľ������ʾ������ */
CHandle nClose;
CZone   nCZone={306,2,317,13};

/* 6�����ͣ����"�ر�"��ť�ϣ���ť��ʾ��ʹ�õľ�� */
CHandle mClose;    
CZone   mCZone={263,2,274,13};

/* 7��"�����Ի���"��ʾ���õľ�����Լ��Ի����ϰ�ť������ */
CHandle helpDialog;
CZone   HDZone={119,100,185,139};
CZone   OKZone={166,130,180,135};
CZone   HDCZone={180,101,184,105};

/* 8��"���ԶԻ���"��ʾʹ�õľ�����Լ��Ի����ϰ�ť������ */
CHandle TryAgainDialog;
CZone   TADZone={119,147,185,186};
CZone   YesZone={131,173,148,180};
CZone   NoZone ={157,173,174,180};

/* 9��������Ԫ��������ʾ��ʹ�õľ������ʾ������ */
CHandle normal;
CZone   nZone={ START_X,  START_Y,  START_X+UNIT_WIDTH-1,  START_Y+UNIT_HEIGHT-1 };

/* 10��������Ԫ��ʾ������ʹ�õ�ͼƬ�������ʾ������ */
CHandle bomb;  
CZone   bZone={ START_X+UNIT_WIDTH*10, START_Y, START_X+UNIT_WIDTH*11-1, START_Y+UNIT_HEIGHT-1 };   

/* 11��������Ԫ��ʾ��Χ������Ŀ(0~9)��ʹ�õ�ͼƬ�������ʾ���� */
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

/* 12��ʱ����ʾ������ */
CZone timeZone={11,36,36,48};

/* 13��reset��ť��ʾ������ */
CZone resetZone={154,36,168,50};

/* 14���÷���ʾ������ */
CZone scoreZone={284,36,309,50};

/* 15������������ */
CZone mineField={ START_X, START_Y, START_X+UNIT_WIDTH*ROWS-1, START_Y+UNIT_HEIGHT*LINES-1 };

/* 16����ά����������¼�������׵ķֲ���� */
Bool bombs[LINES][ROWS];

/* 17����ҵĵ÷� */
int  myScore=0;
/*************************************************/








/************* ��Ϸ��ʹ�õ��ĺ��� ****************/

/* 1����ȡ����ͼƬ�ľ����������handle��ָ��ľ����ͼ�������Ϊzone */
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


/* 2����ȡ��Ϸ����Ҫ�õ������е�ͼƬ�ľ�� */
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


/* 3���ͷ���Ϸ���õ������е�ͼƬ�ľ�� */
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



/* 4����������(row,line)�������׻���ʾ���������֣�row��ȡֵ��Χ[0, ROWS-1]��line ��ȡֵ��Χ[0, LINES-1] */
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


/* 5������ʾʱ�Ӵ���ʾʱ�Ӽ���ֵ�������ʱ������false */
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

       timeInStr[0]='0'+(time%10);         /* ��λ��ֵ */
       timeInStr[1]='0'+( (time%100)/10 ); /* ʮλ��ֵ */
       timeInStr[2]='0'+(time/100);        /* ��λ��ֵ */

       clearZone( timeZone.leftUp.x, timeZone.leftUp.y, timeZone.rightDown.x, timeZone.rightDown.y, 0 );

       for(i=2; i>=0; i--)  /* �������λ��Ȼ�����ʮλ����������λ */
       {
          outCharInRom( timeInStr[i],255, timeZone.leftUp.x+2+(2-i)*8, timeZone.leftUp.y+3, 1);
       }
    }

	/* �ж��Ƿ�ʱ�������ʱ������false�����򷵻�true */
    if( time< TIME_OUT )
    {
        return true;
    }
    else
    {
        return false;
    }
}


/* 6���ڳɼ���ʾ����ʾ�ɼ�score */
void showScore(int score)
{
	char scoreInStr[3];
	int  i;

	scoreInStr[0]='0'+(score%10);         /* ��λ��ֵ */
	scoreInStr[1]='0'+( (score%100)/10 ); /* ʮλ��ֵ */
	scoreInStr[2]='0'+(score/100);        /* ��λ��ֵ */

	clearZone( scoreZone.leftUp.x, scoreZone.leftUp.y, scoreZone.rightDown.x, scoreZone.rightDown.y, 0 );
	
	for(i=2; i>=0; i--)  /* �������λ��Ȼ�����ʮλ����������λ */
	{
		outCharInRom( scoreInStr[i],255, scoreZone.leftUp.x+2+(2-i)*8, scoreZone.leftUp.y+3, 1);
	}

}


/* 7��������Ϸ��ʹ��Ϸ�Ĳ�����ԭΪ��ʼֵ */
void reset()
{
	int line,row,i;
	RGB colors[256];

	/* ���óɼ�Ϊ0 */
	myScore=0;

	/* ��ʱʹ��Ļ��ڣ��������Ա�����ֻ������˸ */
	getRegisters(0,256,colors);
	blackScreen(false);

	for(line=0; line<LINES; line++)
	for(row=0 ; row<ROWS  ; row++ )
	{
		i=rand()%GAME_LEVEL;   /* ���ݲ�ͬ���Ѷȵȼ���������ÿ����Ԫ����ֵ��׵ĸ��ʲ�һ�� */

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
	
	/* ��ʱ����������ͼƬ���Ѿ����£�Ӧ�ø������ı��� */
	changeBackGround(mineField);

	/* resetʱ����ʾ */
	showTime(true);

	/* ��ʾ��ǰ�ɼ� */
	showScore(myScore);

	/* �ظ�ͼ���������ʾ */	
	setRegisters(0,256,colors);

}


/* 8����ʼ����Ϸ( ������Ϸ���������Ϸ���� ) */
Bool initGame()
{
   int  feedBack;
   RGB  colors[256];

   /* ��ʾ��Ϸ������ */
   feedBack = showBMP256("picture\\main.bmp",0,0);

   /* �ȱ���ͼƬ�ĵ�ɫ��Ĵ��� */
   getRegisters(0,256,colors);

   /* ʹ��Ļ��ʱ��ڣ��ڻ�ȡ�ؼ�ͼƬ���֮���ٻָ�����ͼƬ��ʾ����������������Ļ������ */
   blackScreen(false);

   if(true!=feedBack)
   {
	   return false;
   }

   /* ��ȡ�����ؼ�ͼƬ�ľ�� */
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


/* 9����ʾ�������ڹ���"��Ϸ"�˵���"����"�˵���"�ر�"��ť����ʾ */
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


/* 10�������������Ĵ����� */
Bool inBoardZone(CPoint point, ClickInfor infor)
{
	displayManage(NONE);

	if( LEFT_CLICK==infor )  /* �������е����¼� */
    {
		int row,line;
		CZone z;

		/* ͨ����굥�������꣬�����������е�Ԫ���λ�� */
		row  = (point.x-START_X)/UNIT_WIDTH;
		line = (point.y-START_Y)/UNIT_HEIGHT;		
		
		if( bombs[line][row])   /* ��ϲ�㣬�ȵ�������!��ʱ�����еĵ��׶���ʾ���� */
		{
			for(line=0; line<LINES; line++)
			for(row=0;  row<ROWS;   row++)
			{
                if( bombs[line][row])
				{
					drawBoard(bomb,row,line);
					
					/* ͬʱ�������ı��� */
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

			/* ͬʱ�������ı��� */			
			z.leftUp.x = START_X + UNIT_WIDTH*row ;
			z.leftUp.y = START_Y + UNIT_HEIGHT*line;
			z.rightDown.x=z.leftUp.x+UNIT_WIDTH-1;
			z.rightDown.y=z.leftUp.y+UNIT_HEIGHT-1;
				
			changeBackGround(z);

            /* �ɼ����� */
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


/* 11�������"�ر�"��ť��� */
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


/* 12�������"reset"��ť�ϻ */
Bool inResetZone(CPoint point, ClickInfor infor)
{
	displayManage(NONE);

	if( LEFT_CLICK==infor )
	{
		reset();
	}

	return true;
}


/* 13�������"��Ϸ"�˵��ϻ */
Bool inGameZone(CPoint point, ClickInfor infor)
{
	displayManage(GAME_MENU);

	if( LEFT_CLICK==infor )
	{
		reset();
	}

	return true;
}


/* 14�������"����"�˵��ϻ */
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

		if( !buffer )    /* ��������ڴ�ʧ�ܣ�����ʾ�����Ի���ֱ�ӷ��� */
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

			/* "�����Ի���"��"ģʽ�Ի���"�������Ҫ�Լ�������¼��������� */
			while( notQuit )
			TIME_SLICE_BEGIN(1)
			{
				refreshMouse();

				if( !showTime( false ) )  /* �����ʱ�ˣ��򷵻�false */
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


/* 15���ȵ������ˣ�ѯ���Ƿ������Ϸ */
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

	if( !buffer )    /* ��������ڴ�ʧ�ܣ�����ʾ���ԶԻ���ֱ�ӷ���false */
	{
		return false;
	}
	else
	{
		Bool  notQuit=true;
		
		CZone dlg,YesButton,NoButton;
		int   excurX=0, excurY=-45;       /* ʵ����ʾʱ��"���ԶԻ���"������ƫ���� */		

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

		/* "�����Ի���"��"ģʽ�Ի���"�������Ҫ�Լ�������¼��������� */
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

/* 15��������¼����з�����Ȼ��������ͬ�Ķ��󣬸���������Ӧ������¼���Ӧ���� */
Bool analysis(CPoint point, ClickInfor infor)
{
    CZone  boardZone;
    static Bool closeNormal=true;    

    if( inZone( mineField, point) )        /* ���ָ���ڡ������� */
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
    else if( inZone(nCZone, point) )       /* ���ָ��ָ�򴰿ڵġ��رա���ť */
    {
        return inCloseZone(point, infor);
    }
	else if( inZone(resetZone, point) )    /* ���ָ��ָ��reset����ť */
	{
		return inResetZone(point, infor);
	}
	else if( inZone(nGZone, point) )       /* ���ָ��ָ����Ϸ���˵� */
	{
		return inGameZone(point, infor);
	}
	else if( inZone(nHZone, point) )       /* ���ָ��ָ�򡰰������˵� */
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
