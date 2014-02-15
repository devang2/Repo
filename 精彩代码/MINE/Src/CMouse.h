/************************************************************
*  版    权：NEU SUN SOFT.Co.,Ltd                           *
*************************************************************
*  文 件 名：CMouse.h                                       *
*************************************************************
*  作    者：余付平      版本：1.0     完成日期：2005/8/16  *
*************************************************************
*  联系作者：E-mail:walkac@qq.com                           *
*************************************************************
*  描    述：在该文件中封装了C语言中鼠标器的一系列操作。可  *
*            判断双击、单击、鼠标释放、鼠标按键状态、更改   *
*            鼠标指针、设置双击延迟时间、设置光标显示与隐藏 *
*            设置鼠标活动范围、移动鼠标到某个特定位置等功能 *
*************************************************************/



/**** 加入头文件 ****/
#ifndef TYPE_H
    #define TYPE_H
    #include "type.h"
#endif

#ifndef CGRAPH_H
    #define CGRAPH_H
    #include "CGraph.h"
#endif





/**** 定义 CMouse.h 中用到的数据类型、常量和全局变量 ****/

typedef int MouseState;    /* 鼠标的状态 */
typedef int ClickInfor;    /* 击键的信息 */


enum mouseState            /* 鼠标状态常量,共8种状态 */
{ 
    NO_BUTTON_IS_PRESSED=0,
    LEFT_BUTTON_IS_PRESSED,
    RIGHT_BUTTON_IS_PRESSED,
    LEFT_AND_RIGHT_BUTTONS_ARE_PRESSED,
    CENTER_BUTTON_IS_PRESSED,
    LEFT_AND_CENTER_BUTTONS_ARE_PRESSED,
    RIGHT_AND_CENTER_BUTTONS_ARE_PRESSED,
    LEFT_AND_RIGHT_AND_CENTER_BUTTONS_ARE_PRESSED
};


enum clickInfor            /* 鼠标击键信息常量*/
{ 
    NO_BUTTON_CLICK,       /* 没有击键信息*/
    LEFT_CLICK,            /* 左键单击 */
    RIGHT_CLICK,           /* 右键单击 */
    CENTER_CLICK,          /* 中键单击 */
    DOUBLE_CLICK,          /* 左键双击 */
    LEFT_RELEASED,         /* 左键释放 */
    RIGHT_RELEASED         /* 右键释放 */
};


typedef struct CursorICO            /* 鼠标光标图标类 */
{
    int *  m_cursor;       /* 光标图标数据数组,大小是 width×height。显示鼠标光标图标时,横向将数组中数据写入显存 */
    int *  m_backGround;   /* 用于保存光标背景，数组大小是 width×height */
    int    m_width;        /* 光标图标的宽度     */
    int    m_height;       /* 光标图标的高度     */
	Bool   m_visible;      /* 鼠标指针是否可见   */
	Bool   m_hasBackGround;/* 是否保存了背景     */
    CPoint m_position;     /* 当前显示光标的位置 */

} CCursorICO;


typedef struct MouseInfor           /* 记录鼠标信息 */
{
    int        m_buttons;           /* 鼠标按钮数 */ 
    int        m_doubleClickDelay;  /* m_doubleClickDelay 表示双击延迟时间是刷新鼠标图标的 m_doubleClickDelay 倍 */
    CPoint     m_position;          /* 鼠标的当前位置     */
    MouseState m_mouseState;        /* 鼠标的当前状态     */
    ClickInfor m_clickInfor;        /* 鼠标当前的击键信息 */

} CMouseInfor;


typedef struct Mouse
{
    CCursorICO  m_cursorICO;
    CMouseInfor m_mouseInfor;

} CMouse;


CMouse myMouse;         /* 全局变量，一个应用程序中只允许一个鼠标变量 */
Bool   hasMouse=false;  /* 是否存在鼠标，初始化成功后赋值为 true */





/**** 函数声明 ****/
Bool installMouse();        /* 安装鼠标器，在使用鼠标之前调用，并且只调用一次，如果初始化鼠标失败，则返回false */
void removeMouse();         /* 清除鼠标器，如果决定在当前应用中不再使用鼠标器，可以调用此函数清除鼠标器，并释放资源。
                               清除完后不能在安装鼠标！所以该函数一般是在程序结束时调用，如果要隐藏鼠标，可调用另一个函数: setVisible(false);*/

Bool initMouse(int buttons);                              /* 初始化鼠标器,由 installMouse() 函数调用，用户不许自己调用它来初始化鼠标！因为初始化只进行一次 */
Bool setCursorICO(int * cursorICO,int height,int width);  /* 设置鼠标指针 */
void showMouse();                                         /* 显示鼠标指针，用户不需要自己调用，在 refreshMouse() 函数中被调用，如果想让隐藏的鼠标指针显示出来，可以调用：setVisible(true); */
void setVisible( Bool visible );       /* 设置鼠标是否可见 */
int  getButtons();                     /* 获得鼠标按键个数 */
int  getDoubleClickDelay();            /* 获得鼠标双击延迟时间 */
void setDoubleClickDelay(int doubleClickDelay);       /* 设置鼠标双击延迟时间 */
void setActiveArea(UINT x1,UINT y1,UINT x2,UINT y2);  /* 设置鼠标活动区域 */
void setMouseActiveZone(CZone zone);   /* 设置鼠标活动区域 */
void moveTo(UINT x, UINT y);           /* 把鼠标移至坐标(x，y) */
void moveMouseTo(CPoint position);     /* 把鼠标移至坐标position处 */
void refreshMouse();                   /* 刷新鼠标信息，包括鼠标指针和鼠标状态、击键信息 */
CPoint     getMousePosition();         /* 获取鼠标当前位置坐标 */
MouseState getMouseState();            /* 获取鼠标当前状态 */
ClickInfor fetchClickInfor();          /* 获取鼠标击键信息 */




/**** 函数实现 ****/

Bool installMouse()  /* 安装鼠标器 */
{
    static Bool firstCall = true;     /* 记录是否是第一次调用这个函数，防止多次调用 */

    if( firstCall )
    {
        UINT result=0, buttons=0;

        firstCall = false;

        asm MOV AX,00H;
        asm INT 33H;
        asm MOV result, AX;
        asm MOV buttons,BX; 
        
        if(0xFFFF==result)
        {
            hasMouse = true;

            if( initMouse(buttons) )
    		{
                return true;
    		}
        	else /* 初始化鼠标器失败 */
            {                
				printf("Error: System has a mouse with %u buttons, but I failed to initialize it\n", buttons);
                return false;
    		}
        }
        else /* 安装鼠标失败 */
        {
            return false;
        }

    }
    else /* 重复调用是不允许的，返回false */
    {
        return false;
    }
}


void removeMouse()
{
	int * cursor = myMouse.m_cursorICO.m_cursor;
    int * bg = myMouse.m_cursorICO.m_backGround;

	if(!hasMouse)  /* 如果不存在鼠标器，则不需要清除鼠标 */
	{
		return;
	}

	if( bg && myMouse.m_cursorICO.m_hasBackGround )  /* 如果鼠标还保存有背景，还得清除鼠标图标，恢复背景 */
	{
		int   i,j,width,height,x,y;
		long  index, tmp;

		x=myMouse.m_cursorICO.m_position.x;
		y=myMouse.m_cursorICO.m_position.y;
		
		width  = myMouse.m_cursorICO.m_width;
		height = myMouse.m_cursorICO.m_height;

		index=0;
		for(i=0; i<height; i++)
		{
			long  tmp=index;
			
			for(j=0; j<width; j++)
			{
				putpixel(x+j, y+i, bg[tmp] );
				tmp++;
			}

			index+=width;
		}
	}

	if(cursor)   /* 释放鼠标指针占有的内存 */
	{
		free(cursor);
		myMouse.m_cursorICO.m_cursor=NULL;
	}

	if(bg)       /* 释放鼠标背景占有的内存 */
	{
		free(bg);
		myMouse.m_cursorICO.m_backGround=NULL;
	}
}


Bool initMouse(int buttons)    /* 初始化鼠标器 */
{
    /* 设置鼠标光标图标 */  
    int cursorICO[16][11]=
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
     1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    if( !hasMouse ) /* 如果不存在鼠标器，则不进行初始化 */
    {
        return false;
    }

    myMouse.m_cursorICO.m_cursor     = NULL;
    myMouse.m_cursorICO.m_backGround = NULL;
    myMouse.m_cursorICO.m_width   = 0;
    myMouse.m_cursorICO.m_height  = 0;
	myMouse.m_cursorICO.m_visible = true;
	myMouse.m_cursorICO.m_hasBackGround = false;

    if( !setCursorICO(cursorICO, 16, 11) )
    {
        return false;
    }
        		
    /* 设置鼠标信息 */
    myMouse.m_mouseInfor.m_buttons = buttons;
    myMouse.m_mouseInfor.m_doubleClickDelay = 2;
    myMouse.m_mouseInfor.m_mouseState = NO_BUTTON_IS_PRESSED;
    myMouse.m_mouseInfor.m_clickInfor = NO_BUTTON_CLICK;

    return true;
}


/* 设置鼠标指针 */
Bool setCursorICO( int * cursorICO, int height, int width )
{
    int * cursor = myMouse.m_cursorICO.m_cursor;
    int * bg = myMouse.m_cursorICO.m_backGround;    	
    int   i,j;
	int   x,y;
    long  index;

    if( !hasMouse || height<=0 || width<=0 )
    {
        return false;
    }

	x=myMouse.m_cursorICO.m_position.x;
	y=myMouse.m_cursorICO.m_position.y;

    if( NULL!=cursor )   /* 如果鼠标指针已经存在,则修改当前鼠标指针 */
    {
        int oldWidth  = myMouse.m_cursorICO.m_width;
        int oldHeight = myMouse.m_cursorICO.m_height;

        if( oldWidth==width && oldHeight==height )   /* 如果指针大小和原来的一样，则只需改变指针的图标即可，而不需重新申请内存 */
    	{
            index=0;
            for(i=0; i<height; i++)
    		{
                long tmp=index;

                for(j=0; j<width; j++)
        		{
                    cursor[tmp] = cursorICO[tmp];
            		tmp++;
        		}
                index+=width;
    		}
            return true;
    	}
        else  /* 指针大小和原来的不一样，需重新分配内存，用来保存新的指针图标 */
    	{            
			int * p;

            /* 1、先申请新内存 */
            cursor = malloc( width*height*sizeof(int) );		
            if(!cursor)	 /* 如果申请内存失败，则不进行修改指针的操作 */
        	{	
                return false; 
    		}

            bg = malloc( width*height*sizeof(int) );
            if(!bg)      /* 如果申请内存失败，则不进行修改指针的操作 */
    		{
                if(cursor)
        		{
                    free(cursor);
                    cursor=NULL;
        		}
    			
                return false;
    		}
    		
            /* 2、然后，如果鼠标保存有背景，则恢复鼠标背景并释放旧内存 */ 
			p = myMouse.m_cursorICO.m_backGround;

            if( p!=NULL && myMouse.m_cursorICO.m_hasBackGround )  /* 恢复鼠标背景 */
    		{
                index=0;
                for(i=0; i<oldHeight; i++)
        		{
                    long  tmp=index;
        			
                    for(j=0; j<oldWidth; j++)
        			{
                        putpixel( x+j, y+i, p[tmp] );
            			tmp++;
        			}

                    index+=oldWidth;
        		}
				myMouse.m_cursorICO.m_hasBackGround = false;                
    		}

			/* 释放鼠标占有的内存 */
			if(myMouse.m_cursorICO.m_cursor)
    		{
                free(myMouse.m_cursorICO.m_cursor);
                myMouse.m_cursorICO.m_cursor=NULL;
    		}

			if(myMouse.m_cursorICO.m_backGround)
			{
				free(myMouse.m_cursorICO.m_backGround);	
                myMouse.m_cursorICO.m_backGround=NULL;
			}

            /* 3、最后对鼠标赋值新的光标 */
            index=0;
			if(myMouse.m_cursorICO.m_visible)
			{
				for(i=0; i<height; i++)
				{
					long tmp=index;

					for(j=0; j<width; j++)
					{
						cursor[tmp] = cursorICO[tmp];

						bg[tmp]=getpixel(x+j, y+i);

						if( 0!=cursor[tmp] )
						{
							putpixel(x+j, y+i, cursor[tmp]);
						}
						tmp++;
					}
					index+=width;
				}
				myMouse.m_cursorICO.m_hasBackGround = true;
			}
			else
			{
				for(i=0; i<height; i++)
				{
					long tmp=index;

					for(j=0; j<width; j++)
					{
						cursor[tmp] = cursorICO[tmp];
						tmp++;
					}
					index+=width;
				}
			}
            myMouse.m_cursorICO.m_cursor = cursor;
            myMouse.m_cursorICO.m_backGround = bg;
            myMouse.m_cursorICO.m_width  = width;
            myMouse.m_cursorICO.m_height = height;	
			
            return true;
    	}
    }
    else  /* 如果鼠标指针不存在，则新建一个鼠标指针 */
    {   	
        cursor = malloc( width*height*sizeof(int) );    	
        if(!cursor) 
        {	
            printf("error: allocate memery failed when create a new cursor !\n");
            return false;
        }

        bg = malloc( width*height*sizeof(int) );
        if(!bg)
        {
            if(cursor)
        	{
                free(cursor);
                cursor=NULL;
        	}
            printf("error: allocate memery failed when create a new cursor !\n");
            return false;
        }   	

        index=0;
        for(i=0; i<height; i++)
        {
            long tmp=index;

            for(j=0; j<width; j++)
        	{
                cursor[tmp] = cursorICO[tmp];
            	tmp++;
        	}
            index+=width;
        }
        myMouse.m_cursorICO.m_cursor = cursor;
        myMouse.m_cursorICO.m_backGround = bg;
        myMouse.m_cursorICO.m_width  = width;
        myMouse.m_cursorICO.m_height = height;

        return true;
    }
}



/* 显示鼠标指针 */
void showMouse()
{
    static Bool firstCall = true;

    int * bg     = myMouse.m_cursorICO.m_backGround;
    int * cursor = myMouse.m_cursorICO.m_cursor;
    int   width  = myMouse.m_cursorICO.m_width;
    int   height = myMouse.m_cursorICO.m_height;
    int   x, y, newX, newY;
    int   i,j;
    long  index;

    /* 如果没有鼠标，或鼠标不可见则不进行鼠标图标的绘制 */
    if( !hasMouse || !myMouse.m_cursorICO.m_visible )
    {
        return;
    }

    /* 先获取鼠标新的坐标，如果和指针当前显示的坐标相同，且不是第一次调用这个函数，则不需要更新显示 */
    x = myMouse.m_cursorICO.m_position.x;
    y = myMouse.m_cursorICO.m_position.y;
    newX = myMouse.m_mouseInfor.m_position.x;
    newY = myMouse.m_mouseInfor.m_position.y;

    if( !firstCall && x==newX && y==newY )
    {
        return;
    }

    /* 1、如果是第一次调用这个函数，则可以跳过恢复原背景，否则需先恢复背景 */
    if(firstCall)
    {
        firstCall = false;
    }
    else
    { 
        if(NULL != bg && myMouse.m_cursorICO.m_hasBackGround )
        {   		
            index=0;
            for(i=0; i<height; i++)
    		{
                long  tmp=index;
    			
                for(j=0; j<width; j++)
        		{
					putpixel( x+j, y+i, bg[tmp] );
            		tmp++;
        		}

                index+=width;
    		}

			myMouse.m_cursorICO.m_hasBackGround = false;
    	}
    }

    /* 2、先保存背景，然后显示鼠标指针 */
    if( NULL!=bg && NULL!=cursor && myMouse.m_cursorICO.m_visible )
    {   
        index=0;
        for(i=0; i<height; i++)
    	{
            long  tmp=index;
    		
            for(j=0; j<width; j++)
    		{
				int pixel=cursor[tmp];

                bg[tmp]=getpixel( newX+j, newY+i );
				
				if(pixel>0)
				{
					putpixel( newX+j, newY+i, pixel );
				}

            	tmp++;
    		}

            index+=width;
        }  

		myMouse.m_cursorICO.m_hasBackGround = true;
		myMouse.m_cursorICO.m_position.x = newX;
		myMouse.m_cursorICO.m_position.y = newY;
    }
}


void changeBackGround(CZone zone)
{
	int * bg     = myMouse.m_cursorICO.m_backGround;
    int   width  = myMouse.m_cursorICO.m_width;
    int   height = myMouse.m_cursorICO.m_height;
    int   i,j,x,y;
    long  index;
	CZone cursorZone;

    x = myMouse.m_cursorICO.m_position.x;
    y = myMouse.m_cursorICO.m_position.y;

	cursorZone.leftUp.x=x;
	cursorZone.leftUp.y=y;
	cursorZone.rightDown.x=x+width -1;
	cursorZone.rightDown.y=y+height-1;

	if( !publicZone(zone, cursorZone, &cursorZone) )
	{
		return ;
	}

	if( NULL!=bg )
    {   
        index=0;
        for(i=0; i<height; i++)
    	{
            long  tmp=index;
    		
            for(j=0; j<width; j++)
    		{
				CPoint p;

				p.x=x+j;
				p.y=y+i;

				if( inZone(cursorZone,p) )
				{
					bg[tmp]=getpixel( p.x, p.y );
				}

				tmp++;
    		}

            index+=width;
        }  
		myMouse.m_cursorICO.m_hasBackGround = true;
    }
}


void setVisible( Bool visible )
{
	if(!hasMouse)  /* 如果不存在鼠标器，则不进行任何操作 */
	{
		return ;
	}

	if( visible )  /* 设置鼠标可见 */
	{
		myMouse.m_cursorICO.m_visible=true;
		return;
	}
	else  /* 设置鼠标不可见 */
	{
		if( myMouse.m_cursorICO.m_visible )  
		{
			/* 先要隐藏鼠标指针 */
			int * bg = myMouse.m_cursorICO.m_backGround;

			if( bg && myMouse.m_cursorICO.m_hasBackGround )  /* 如果鼠标可见，还得清除鼠标图标 */
			{
				int   i,j,width,height,x,y;
				long  index, tmp;

				x=myMouse.m_cursorICO.m_position.x;
				y=myMouse.m_cursorICO.m_position.y;
				
				width = myMouse.m_cursorICO.m_width;
				height= myMouse.m_cursorICO.m_height;

				index=0;
				for(i=0; i<height; i++)
				{
					long  tmp=index;
					
					for(j=0; j<width; j++)
					{
						putpixel(x+j, y+i, bg[tmp] );
						tmp++;
					}
					
					index+=width;
				}
				myMouse.m_cursorICO.m_hasBackGround=false;
			}

			/* 然后设置鼠标指针不可见 */
			myMouse.m_cursorICO.m_visible=false;
		}
		else
		{
			return ;
		}
	}
}



int getButtons()
{
    if( hasMouse )
    {
        return myMouse.m_mouseInfor.m_buttons;
    }
    else
    {
        return 0;
    }
}


int getDoubleClickDelay()
{
    if(hasMouse)
    {
        return myMouse.m_mouseInfor.m_doubleClickDelay;
   }
    else
    {
        return 0;
    }
}


void setDoubleClickDelay(int doubleClickDelay)
{
    if(hasMouse)
    {
        myMouse.m_mouseInfor.m_doubleClickDelay = doubleClickDelay;
    }
}


CPoint getMousePosition()   /* 获取鼠标当前位置坐标 */
{
    CPoint point;
    point.x=0;
    point.y=0;

    if(hasMouse)
    {
        point = myMouse.m_mouseInfor.m_position;
    }

    return point;
}


MouseState getMouseState()
{   
    if(hasMouse)
    {
        return myMouse.m_mouseInfor.m_mouseState;
    }
    else
    {
        return NO_BUTTON_IS_PRESSED;
    }
}


ClickInfor fetchClickInfor()
{
    ClickInfor infor = NO_BUTTON_CLICK;
    if(hasMouse)
    {
        infor = myMouse.m_mouseInfor.m_clickInfor;
        myMouse.m_mouseInfor.m_clickInfor = NO_BUTTON_CLICK; 
    }

    return infor;
}


void setActiveArea( UINT x1, UINT y1, UINT x2, UINT y2 )
{
	if(!hasMouse)
	{
		return;
	}

	asm mov ax,07h;   
	asm mov cx,x1;
	asm mov dx,x2;
	asm int 33h;

	asm mov ax,08h;
	asm mov cx,y1;
	asm mov dx,y2;
	asm int 33h;
}


void setMouseActiveZone(CZone zone)
{
   int x1,x2,y1,y2;

   if(!hasMouse)
   {
       return;
   }

   x1=zone.leftUp.x;
   y1=zone.leftUp.y;

   x2=zone.rightDown.x;
   y2=zone.rightDown.y;

   setActiveArea( x1, y1, x2, y2 );

}


void moveTo(UINT x, UINT y)
{
	if(!hasMouse)
    {
        return;
    }

	asm mov ax,04h;
    asm mov cx,x;
    asm mov dx,y;
    asm int 33h;
}


void moveMouseTo(CPoint position)
{
    UINT x,y;

    if(!hasMouse)
    {
        return;
    }

    x=position.x;
    y=position.y;

    moveTo( x, y );
}


void refreshMouse()
{
    /* point 用于记录鼠标当前位置 */
    CPoint point;
    volatile UINT   x=0, y=0;

    /* state 用于记录鼠标当前状态 */
    MouseState state = NO_BUTTON_IS_PRESSED;
	static     MouseState priorState = NO_BUTTON_IS_PRESSED;  /* "上一次"按键状态 */    
    UINT mask=0, buttons=0;

    /* clickInfor 用于记录鼠标击键信息 */
    static int count = 1;
	int        doubleClickDelay;
    ClickInfor clickInfor = NO_BUTTON_CLICK;


    if(!hasMouse)  /* 如果还没有安装，则不进行操作 */
    {
        return;
    }

    /* 1、获取鼠标当前位置 */
    asm MOV AX,03H;
    asm INT 33H;
    asm MOV x,CX;
    asm MOV y,DX;

    point.x=x;
    point.y=y;

    myMouse.m_mouseInfor.m_position = point;

    /* 2、获取鼠标当前按键状态 */
    buttons=myMouse.m_mouseInfor.m_buttons;
    switch(buttons)
    {
         case 1 :  mask=0x01;   break;
         case 2 :  mask=0x03;   break;
         case 3 :  mask=0x07;   break;
         default:  mask=0;
    }
    
    asm MOV AX,03H;
    asm INT 33H;
    asm MOV state,BX;
    
    state &= mask;
   
    myMouse.m_mouseInfor.m_mouseState=state;

    /* 3、获取鼠标击键信息 */
    doubleClickDelay = myMouse.m_mouseInfor.m_doubleClickDelay;

    if( count<doubleClickDelay )  /* 如果还未到达双击延迟时间，不进行击键信息的读取 */
    {
        count++;
    }
    else
    {
        int i;
        for( i=0; i<buttons; i++ )  /* 假设同一时刻只有一种击键事件发生 */
    	{
            asm MOV AX,05H;
            asm MOV BX,i;
            asm INT 33H;
            asm MOV count,BX;

            if(1==count)
    		{
                switch(i)
        		{
                    case  0 : clickInfor=LEFT_CLICK;     break;
                    case  1 : clickInfor=RIGHT_CLICK;    break;
                    case  2 : clickInfor=CENTER_CLICK;   break;
                    default : break;
        		}
    		}
            else if(count>1)
    		{
                switch(i)
        		{
                    case  0 : clickInfor=DOUBLE_CLICK;   break;  /* 只有左键才有双击事件 */
                    case  1 : clickInfor=RIGHT_CLICK;    break;
                    case  2 : clickInfor=CENTER_CLICK;   break;
                    default : break;
        		}
    		}
    	}        
        count=1;
    }

	/* 以下语句用来判断鼠标按键的释放动作 */
	if( NO_BUTTON_CLICK==clickInfor )
	{
		if( LEFT_BUTTON_IS_PRESSED==priorState && NO_BUTTON_IS_PRESSED==state )
		{
			clickInfor=LEFT_RELEASED;
		}
		else if( RIGHT_BUTTON_IS_PRESSED==priorState && NO_BUTTON_IS_PRESSED==state )
		{
			clickInfor=RIGHT_RELEASED;
		}
	}

	priorState=myMouse.m_mouseInfor.m_mouseState;
	myMouse.m_mouseInfor.m_clickInfor=clickInfor;	

    /* 4、最后在新的位置显示鼠标指针 */
    showMouse();

}