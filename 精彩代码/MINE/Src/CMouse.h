/************************************************************
*  ��    Ȩ��NEU SUN SOFT.Co.,Ltd                           *
*************************************************************
*  �� �� ����CMouse.h                                       *
*************************************************************
*  ��    �ߣ��ึƽ      �汾��1.0     ������ڣ�2005/8/16  *
*************************************************************
*  ��ϵ���ߣ�E-mail:walkac@qq.com                           *
*************************************************************
*  ��    �����ڸ��ļ��з�װ��C�������������һϵ�в�������  *
*            �ж�˫��������������ͷš���갴��״̬������   *
*            ���ָ�롢����˫���ӳ�ʱ�䡢���ù����ʾ������ *
*            ���������Χ���ƶ���굽ĳ���ض�λ�õȹ��� *
*************************************************************/



/**** ����ͷ�ļ� ****/
#ifndef TYPE_H
    #define TYPE_H
    #include "type.h"
#endif

#ifndef CGRAPH_H
    #define CGRAPH_H
    #include "CGraph.h"
#endif





/**** ���� CMouse.h ���õ����������͡�������ȫ�ֱ��� ****/

typedef int MouseState;    /* ����״̬ */
typedef int ClickInfor;    /* ��������Ϣ */


enum mouseState            /* ���״̬����,��8��״̬ */
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


enum clickInfor            /* ��������Ϣ����*/
{ 
    NO_BUTTON_CLICK,       /* û�л�����Ϣ*/
    LEFT_CLICK,            /* ������� */
    RIGHT_CLICK,           /* �Ҽ����� */
    CENTER_CLICK,          /* �м����� */
    DOUBLE_CLICK,          /* ���˫�� */
    LEFT_RELEASED,         /* ����ͷ� */
    RIGHT_RELEASED         /* �Ҽ��ͷ� */
};


typedef struct CursorICO            /* �����ͼ���� */
{
    int *  m_cursor;       /* ���ͼ����������,��С�� width��height����ʾ�����ͼ��ʱ,��������������д���Դ� */
    int *  m_backGround;   /* ���ڱ����걳���������С�� width��height */
    int    m_width;        /* ���ͼ��Ŀ��     */
    int    m_height;       /* ���ͼ��ĸ߶�     */
	Bool   m_visible;      /* ���ָ���Ƿ�ɼ�   */
	Bool   m_hasBackGround;/* �Ƿ񱣴��˱���     */
    CPoint m_position;     /* ��ǰ��ʾ����λ�� */

} CCursorICO;


typedef struct MouseInfor           /* ��¼�����Ϣ */
{
    int        m_buttons;           /* ��갴ť�� */ 
    int        m_doubleClickDelay;  /* m_doubleClickDelay ��ʾ˫���ӳ�ʱ����ˢ�����ͼ��� m_doubleClickDelay �� */
    CPoint     m_position;          /* ���ĵ�ǰλ��     */
    MouseState m_mouseState;        /* ���ĵ�ǰ״̬     */
    ClickInfor m_clickInfor;        /* ��굱ǰ�Ļ�����Ϣ */

} CMouseInfor;


typedef struct Mouse
{
    CCursorICO  m_cursorICO;
    CMouseInfor m_mouseInfor;

} CMouse;


CMouse myMouse;         /* ȫ�ֱ�����һ��Ӧ�ó�����ֻ����һ�������� */
Bool   hasMouse=false;  /* �Ƿ������꣬��ʼ���ɹ���ֵΪ true */





/**** �������� ****/
Bool installMouse();        /* ��װ���������ʹ�����֮ǰ���ã�����ֻ����һ�Σ������ʼ�����ʧ�ܣ��򷵻�false */
void removeMouse();         /* ������������������ڵ�ǰӦ���в���ʹ������������Ե��ô˺����������������ͷ���Դ��
                               ���������ڰ�װ��꣡���Ըú���һ�����ڳ������ʱ���ã����Ҫ������꣬�ɵ�����һ������: setVisible(false);*/

Bool initMouse(int buttons);                              /* ��ʼ�������,�� installMouse() �������ã��û������Լ�����������ʼ����꣡��Ϊ��ʼ��ֻ����һ�� */
Bool setCursorICO(int * cursorICO,int height,int width);  /* �������ָ�� */
void showMouse();                                         /* ��ʾ���ָ�룬�û�����Ҫ�Լ����ã��� refreshMouse() �����б����ã�����������ص����ָ����ʾ���������Ե��ã�setVisible(true); */
void setVisible( Bool visible );       /* ��������Ƿ�ɼ� */
int  getButtons();                     /* �����갴������ */
int  getDoubleClickDelay();            /* ������˫���ӳ�ʱ�� */
void setDoubleClickDelay(int doubleClickDelay);       /* �������˫���ӳ�ʱ�� */
void setActiveArea(UINT x1,UINT y1,UINT x2,UINT y2);  /* ����������� */
void setMouseActiveZone(CZone zone);   /* ����������� */
void moveTo(UINT x, UINT y);           /* �������������(x��y) */
void moveMouseTo(CPoint position);     /* �������������position�� */
void refreshMouse();                   /* ˢ�������Ϣ���������ָ������״̬��������Ϣ */
CPoint     getMousePosition();         /* ��ȡ��굱ǰλ������ */
MouseState getMouseState();            /* ��ȡ��굱ǰ״̬ */
ClickInfor fetchClickInfor();          /* ��ȡ��������Ϣ */




/**** ����ʵ�� ****/

Bool installMouse()  /* ��װ����� */
{
    static Bool firstCall = true;     /* ��¼�Ƿ��ǵ�һ�ε��������������ֹ��ε��� */

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
        	else /* ��ʼ�������ʧ�� */
            {                
				printf("Error: System has a mouse with %u buttons, but I failed to initialize it\n", buttons);
                return false;
    		}
        }
        else /* ��װ���ʧ�� */
        {
            return false;
        }

    }
    else /* �ظ������ǲ�����ģ�����false */
    {
        return false;
    }
}


void removeMouse()
{
	int * cursor = myMouse.m_cursorICO.m_cursor;
    int * bg = myMouse.m_cursorICO.m_backGround;

	if(!hasMouse)  /* ��������������������Ҫ������ */
	{
		return;
	}

	if( bg && myMouse.m_cursorICO.m_hasBackGround )  /* �����껹�����б���������������ͼ�꣬�ָ����� */
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

	if(cursor)   /* �ͷ����ָ��ռ�е��ڴ� */
	{
		free(cursor);
		myMouse.m_cursorICO.m_cursor=NULL;
	}

	if(bg)       /* �ͷ���걳��ռ�е��ڴ� */
	{
		free(bg);
		myMouse.m_cursorICO.m_backGround=NULL;
	}
}


Bool initMouse(int buttons)    /* ��ʼ������� */
{
    /* ���������ͼ�� */  
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

    if( !hasMouse ) /* �����������������򲻽��г�ʼ�� */
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
        		
    /* ���������Ϣ */
    myMouse.m_mouseInfor.m_buttons = buttons;
    myMouse.m_mouseInfor.m_doubleClickDelay = 2;
    myMouse.m_mouseInfor.m_mouseState = NO_BUTTON_IS_PRESSED;
    myMouse.m_mouseInfor.m_clickInfor = NO_BUTTON_CLICK;

    return true;
}


/* �������ָ�� */
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

    if( NULL!=cursor )   /* ������ָ���Ѿ�����,���޸ĵ�ǰ���ָ�� */
    {
        int oldWidth  = myMouse.m_cursorICO.m_width;
        int oldHeight = myMouse.m_cursorICO.m_height;

        if( oldWidth==width && oldHeight==height )   /* ���ָ���С��ԭ����һ������ֻ��ı�ָ���ͼ�꼴�ɣ����������������ڴ� */
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
        else  /* ָ���С��ԭ���Ĳ�һ���������·����ڴ棬���������µ�ָ��ͼ�� */
    	{            
			int * p;

            /* 1�����������ڴ� */
            cursor = malloc( width*height*sizeof(int) );		
            if(!cursor)	 /* ��������ڴ�ʧ�ܣ��򲻽����޸�ָ��Ĳ��� */
        	{	
                return false; 
    		}

            bg = malloc( width*height*sizeof(int) );
            if(!bg)      /* ��������ڴ�ʧ�ܣ��򲻽����޸�ָ��Ĳ��� */
    		{
                if(cursor)
        		{
                    free(cursor);
                    cursor=NULL;
        		}
    			
                return false;
    		}
    		
            /* 2��Ȼ�������걣���б�������ָ���걳�����ͷž��ڴ� */ 
			p = myMouse.m_cursorICO.m_backGround;

            if( p!=NULL && myMouse.m_cursorICO.m_hasBackGround )  /* �ָ���걳�� */
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

			/* �ͷ����ռ�е��ڴ� */
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

            /* 3��������긳ֵ�µĹ�� */
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
    else  /* ������ָ�벻���ڣ����½�һ�����ָ�� */
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



/* ��ʾ���ָ�� */
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

    /* ���û����꣬����겻�ɼ��򲻽������ͼ��Ļ��� */
    if( !hasMouse || !myMouse.m_cursorICO.m_visible )
    {
        return;
    }

    /* �Ȼ�ȡ����µ����꣬�����ָ�뵱ǰ��ʾ��������ͬ���Ҳ��ǵ�һ�ε����������������Ҫ������ʾ */
    x = myMouse.m_cursorICO.m_position.x;
    y = myMouse.m_cursorICO.m_position.y;
    newX = myMouse.m_mouseInfor.m_position.x;
    newY = myMouse.m_mouseInfor.m_position.y;

    if( !firstCall && x==newX && y==newY )
    {
        return;
    }

    /* 1������ǵ�һ�ε����������������������ָ�ԭ�������������Ȼָ����� */
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

    /* 2���ȱ��汳����Ȼ����ʾ���ָ�� */
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
	if(!hasMouse)  /* �����������������򲻽����κβ��� */
	{
		return ;
	}

	if( visible )  /* �������ɼ� */
	{
		myMouse.m_cursorICO.m_visible=true;
		return;
	}
	else  /* ������겻�ɼ� */
	{
		if( myMouse.m_cursorICO.m_visible )  
		{
			/* ��Ҫ�������ָ�� */
			int * bg = myMouse.m_cursorICO.m_backGround;

			if( bg && myMouse.m_cursorICO.m_hasBackGround )  /* ������ɼ�������������ͼ�� */
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

			/* Ȼ���������ָ�벻�ɼ� */
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


CPoint getMousePosition()   /* ��ȡ��굱ǰλ������ */
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
    /* point ���ڼ�¼��굱ǰλ�� */
    CPoint point;
    volatile UINT   x=0, y=0;

    /* state ���ڼ�¼��굱ǰ״̬ */
    MouseState state = NO_BUTTON_IS_PRESSED;
	static     MouseState priorState = NO_BUTTON_IS_PRESSED;  /* "��һ��"����״̬ */    
    UINT mask=0, buttons=0;

    /* clickInfor ���ڼ�¼��������Ϣ */
    static int count = 1;
	int        doubleClickDelay;
    ClickInfor clickInfor = NO_BUTTON_CLICK;


    if(!hasMouse)  /* �����û�а�װ���򲻽��в��� */
    {
        return;
    }

    /* 1����ȡ��굱ǰλ�� */
    asm MOV AX,03H;
    asm INT 33H;
    asm MOV x,CX;
    asm MOV y,DX;

    point.x=x;
    point.y=y;

    myMouse.m_mouseInfor.m_position = point;

    /* 2����ȡ��굱ǰ����״̬ */
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

    /* 3����ȡ��������Ϣ */
    doubleClickDelay = myMouse.m_mouseInfor.m_doubleClickDelay;

    if( count<doubleClickDelay )  /* �����δ����˫���ӳ�ʱ�䣬�����л�����Ϣ�Ķ�ȡ */
    {
        count++;
    }
    else
    {
        int i;
        for( i=0; i<buttons; i++ )  /* ����ͬһʱ��ֻ��һ�ֻ����¼����� */
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
                    case  0 : clickInfor=DOUBLE_CLICK;   break;  /* ֻ���������˫���¼� */
                    case  1 : clickInfor=RIGHT_CLICK;    break;
                    case  2 : clickInfor=CENTER_CLICK;   break;
                    default : break;
        		}
    		}
    	}        
        count=1;
    }

	/* ������������ж���갴�����ͷŶ��� */
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

    /* 4��������µ�λ����ʾ���ָ�� */
    showMouse();

}