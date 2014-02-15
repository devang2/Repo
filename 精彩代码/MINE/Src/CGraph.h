/* 加入头文件 */
#ifndef DOS_H
    #define DOS_H
    #include<dos.h>
#endif

#ifndef CONIO_H
    #define CONIO_H
    #include<conio.h>
#endif

#ifndef STDIO_H
   #define STDIO_H
   #include<stdio.h>
#endif

#ifndef FCNTL_H
   #define FCNTL_H
   #include <fcntl.h> 
#endif

#ifndef TYPE_H
    #define TYPE_H
    #include "type.h"
#endif

#ifndef FUNCTION_H
   #define FUNCTION_H
   #include "function.h"
#endif


enum pause_ops{     /* 暂停屏幕时,对屏幕画面的操作类型 */
    GRAY,
    DARK,
    REVERSE
};


enum turn_scr_ops{     /*翻转屏幕的操作类型*/
    HORIZONTAL,
    VERTICAL
};


enum screen_WR_ops{   
    WRITE=0x0001,     /*二进制数00000000 00000001写操作控制*/
    READ =0x0000,     /*二进制数00000000 00000000读操作控制*/
    LINE =0x0000,     /*二进制数00000000 00000000按行绘制*/
    ROW  =0x0002      /*二进制数00000000 00000010按列绘制*/ 
};
/*屏幕写或读操作,可自由组合成: 

     WRITE：按行写
     READ ：按行读
     LINE ：按行读
     ROW  ：按列读
WRITE|LINE：按行写
WRITE|ROW ：按列写
READ |LINE：按行读
READ |ROW ：按列读

这八种之外的组合将导致歧义,推荐使用后四种组合之一*/


enum putimage_ops {     /* BitBlt operators for putimage */
    COPY_PUT,       /* MOV */
    XOR_PUT,        /* XOR */
    OR_PUT,         /* OR  */
    AND_PUT,        /* AND */
    NOT_PUT         /* NOT */
};


enum move_screen_ops{
    MOVLEFT,
    MOVRIGHT,
    MOVUP,
    MOVDOWN
};





/*——定义各种显示模式，有待扩充——*/
#define VGA_320X200X256_MODE 0x13
#define TEXT_MODE 0x03


/*在已定义的各种显示模式中,默认的图形模式是320×200×256色,关闭图形模式后默认进入文本模式。*/
/*如果想改变默认的图形模式,可以修改下面的值*/
#define GRAPHIC_MODE   VGA_320X200X256_MODE           /*1、默认的图形模式是320×200×256色 */
#define MAX_X          319                            /*2、默认的屏幕 x 坐标的最大值为 319 */
#define SCREEN_WIDTH   320                            /*3、默认的屏幕宽度为 320 象素       */
#define MAX_Y          199                            /*4、默认的屏幕 y 坐标的最大值为 199 */
#define SCREEN_HEIGHT  200                            /*5、默认的屏幕高度为 200 象素       */
#define MAX_COLOR      255                            /*6、最大颜色值，不得修改            */
#define COLORS         256                            /*7、共256色，不可修改!              */
#define MAX_RGB        63                             /*8、R、G、B的最大值为63,即6位色深   */


/*———重要的全局变量，用来记录当前屏幕的信息，请勿自行修改—————*/
int  CURR_X;                                    /*当前象素点 x 坐标 —*/
int  CURR_Y;                                    /*当前象素点 y 坐标 —*/
BYTE CURR_COLOR;                                /*当前默认前景颜色——*/
BYTE far *videoBuffer=(char far *)0xA0000000L;  /*图形模式显存起始位置*/


/* 函数声明 */






/*——函数实现——*/

/*设置显示模式*/
void setmode(BYTE mode)          
{
    union REGS inregs,outregs;
    inregs.h.ah=0;
    inregs.h.al=mode;
    int86(0x10,&inregs,&outregs);   /* 调用10H号中断的0号子功能 */
}

/*初始化图形模式，参数没有实际意义，只为与C语言原图形库函数保持一致风格*/
void initgraph(int * gdriver, int * gmode, char * path)
{
    setmode(GRAPHIC_MODE);
    CURR_X=0;
    CURR_Y=0;                           /*初始坐标为(0,0) */
    CURR_COLOR=MAX_COLOR;               /*初始前景色为白色*/
}

/*关闭图形模式，进入字符模式*/
void closegraph(viod)
{
    setmode(TEXT_MODE);
}


/*获取和设置屏幕信息*/
int getmaxx(void)
{
   return MAX_X;
}

int getmaxy(void)
{
   return MAX_Y;
}

int screenwidth(void)
{
   return SCREEN_WIDTH;
}

int screenheight(void)
{
   return SCREEN_HEIGHT;
}

int getx(void)
{
   return CURR_X;
}

int gety(void)
{
   return CURR_Y;
}

Bool moveto(int x, int y)
{
    if(0>x || MAX_X<x || 0>y || MAX_Y<y )
    {
        return false;
    }
    CURR_X=x;
    CURR_Y=y;  
    return true;
}

Bool moverel(int dx, int dy)
{
   int x,y;

   x=CURR_X+dx;
   y=CURR_Y+dy;
   return moveto(x,y);
}

int getmaxcolor(void)
{
   return MAX_COLOR;
}

int colors(void)         /*COLORS为256，BYTE放置不下，所以返回值为UINT型*/
{
    return COLORS;
}

int getcolor(void)
{
    return CURR_COLOR;
}

Bool getbkcolor(RGB * color)
{
    return getPaletteRegister(0,color);
}

Bool setbkcolor(RGB color)                               
{
    return setPaletteRegister(0,color);
}

Bool setcolor(int color)
{
   if( 0>color || MAX_COLOR<color )
   {
       return false;       
   }
   else
   {
       CURR_COLOR=color;
       return true;    
   }
}


/*——设置和读取调色板寄存器的颜色值——*/
/*进行调色板进行操作的操作码*/
#define PALETTE_MASK          0x3c6
#define PALETTE_REGISTER_RD   0x3c7
#define PALETTE_REGISTER_WR   0x3c8
#define PALETTE_DATA          0x3c9


Bool setPaletteRegister(int index,RGB color)  /*如果index在[0～MAX_COLORS]之外，则返回false*/
{
    if(index<0 || index>MAX_COLOR || color.red>MAX_RGB || color.green>MAX_RGB || color.blue>MAX_RGB )
    {   
        return false;
    }
    outp(PALETTE_MASK,0xff);
    outp(PALETTE_REGISTER_WR,index);
    outp(PALETTE_DATA,color.red);
    outp(PALETTE_DATA,color.green);
    outp(PALETTE_DATA,color.blue);

    return true;
}


Bool getPaletteRegister(int index,RGB * color)             /*如果index在[0～MAX_COLORS]之外，则返回false*/
{    
    if(index<0 || index>MAX_COLOR)
    {   
        return false;
    }
    outp(PALETTE_MASK,0xff);
    outp(PALETTE_REGISTER_RD,index);
    color->red  = inp(PALETTE_DATA);
    color->green= inp(PALETTE_DATA);
    color->blue = inp(PALETTE_DATA);

    return true;
}


Bool setRegisters(int start,int num,RGB * colors)   /*如果[start,start+num-1]不在[0～MAX_COLORS]之内,则返回false*/
{
    int i,j;
    int end=start+num-1;                           /*因为start从0开始计数，所以要减1 */
    
    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR || num<1 || num>COLORS)
    {   
        return false;
    }

    for(i=0,j=start; j<=end; i++,j++)                 
    {
        if( !setPaletteRegister(j,colors[i]))
        {
            return false;
        }
    }

    return true;
}


Bool getRegisters(int start,int num,RGB* colors)    /*如果[start,start+num-1]不在[0～MAX_COLORS]之内,则返回false*/
{
    RGB cl;
    int i,j;
    int end=start+num-1;                            /*因为start从0开始计数，所以要减1 */

    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR || num<1 || num>COLORS)
    {   
        return false;
    }

    for(i=0,j=start; j<=end; i++,j++)     
    {
        if( getPaletteRegister(j, &cl) )           /*获取第j个调色板寄存器成功*/
        {
            colors[i]=cl;
        }
        else
        {
            return false;
        }
    }

    return true;
}


/* 以下通过调色板技术产生特殊画面效果 */
Bool grayRegisters(int start,int end)   /*使调色板寄存器代表颜色由彩色变成灰色*/
{
    RGB  color;
    BYTE r,g,b,f;
    int  i;


    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR)
    {   
        return false;
    }

    if(start>end)
    {
        int t;
        t=start;   
        start=end;  
        end=t;
    }

    for(i=start;i<=end;i++)
    {
        if( !getPaletteRegister(i,&color) )
        {
            return false;
        }
        
        r=color.red;   
        g=color.green;   
        b=color.blue;

        f=0.299*r+0.587*g+0.114*b;    /*灰度变换公式*/

        color.red=f;
        color.green=f;
        color.blue=f;

        if( !setPaletteRegister(i,color) )
        {
            return false;
        }
    }

    return true;
}


Bool grayScreen(Bool grayBack)
{
    if(grayBack)
    {
        return grayRegisters(0,MAX_COLOR);
    }
    else
    {
        return grayRegisters(1,MAX_COLOR);
    }
}


Bool blackRegisters(int start,int end)
{
    RGB color;
    int i;

    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR)
    {  
        return false;
    }

    if(start>end)
    {   
        int t;

        t=start; 
        start=end;
        end=t;
    }

    color.red=0;
    color.green=0;
    color.blue=0;

    for(i=start; i<=end; i++)
    {
        if( !setPaletteRegister(i,color))
        {
            return false;
        }
    }

    return true;

}


Bool blackScreen(Bool blackBack)
{
    if(blackBack)
    {
        return blackRegisters(0,MAX_COLOR);
    }
    else
    {
        return blackRegisters(1,MAX_COLOR);
    }
}


Bool reverseRegisters(int start,int end)  /*把相应的调色板寄存器的颜色取反色*/
{
    RGB  color;
    int i;

    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR)
    {   
        return false;
    }

    if(start>end)
    {   
        int t;

        t=start;  
        start=end; 
        end=t;
    }

    for(i=start; i<=end; i++)
    {
        if( !getPaletteRegister(i,&color) )
        {
            return false;
        }

        color.red  = MAX_RGB-color.red;
        color.green= MAX_RGB-color.green;
        color.blue = MAX_RGB-color.blue;

        if( !setPaletteRegister(i,color) )
        {
            return false;
        }
    }

    return true;

}


Bool reverseScreen(Bool reverseBack)
{
    if(reverseBack)
    {
        return reverseRegisters(0,MAX_COLOR);
    }
    else
    {
        return reverseRegisters(1,MAX_COLOR);
    }
}


Bool LAORegisters(int start,int end,float value) /*  LAORegisters代表: light adjuest on registers 调节亮度,value是调节值*/
{
    RGB color;
    int i;
    float r,g,b,maxRGB,maxValue;  /* 为了保证颜色不失真,应设置最高阀值maxValue,如果函数参数value在 [0～maxValue] 则放大value倍  
                                     如果value大于maxValue则放大maxValue倍,maxValue 取255/red、255/green、255/blue三个值的最小值 */

    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR)
    {
        return false;
    }

    if(start>end)
    {   
        int t;

        t=start;    
        start=end;  
        end=t;
    }

    maxRGB=MAX_RGB; 
    
    for(i=start; i<=end; i++)
    {
        if( !getPaletteRegister(i,&color) )
        {           
            return false;
        }

        maxValue=256;        r=color.red;       g=color.green;      b=color.blue;       

        if(r>0)
        {   maxValue=maxRGB/r;
        }

        if( g>0 && (maxRGB/g)<maxValue )
        {   maxValue=maxRGB/g;         
        }

        if(b>0 && (maxRGB/b)<maxValue )
        {
            maxValue=maxRGB/b;
        }

        if(value>maxValue)
        {
            value=maxValue;
        }
        else if(value<0)     
        {
            value=0;
        }

        color.red*=value;
        color.green*=value;
        color.blue*=value;

        if( !setPaletteRegister(i,color))
        {
            return false;
        }
    }

    return true;

}



Bool LAOScreen(float value,Bool LAOBack)
{
    if(LAOBack)
    {
        return LAORegisters(0,MAX_COLOR,value);
    }
    else
    {
        return LAORegisters(1,MAX_COLOR,value);
    }
}


Bool pauseScreen(int operate)
{
    RGB colors[COLORS];

    if( !getRegisters(0,COLORS,colors) )   
    {  
        return false;
    }

    switch(operate)
    {
    case GRAY    :   if(! grayScreen(true))
                         return false;
                     else 
                         break;

    case REVERSE :   if( !reverseScreen(false))
                         return false;
                     else
                         break;

    case DARK    :

    default      :   if( !LAOScreen(0.6,true))     /*调节为原先亮度的 3/5 */
                         return false;
                     else
                         break;

    }

    getch();

    return setRegisters(0,COLORS,colors);

}



/*3、以下是对显存的一系列操作，用来绘制用户所要求的图形，属于底层操作，因此对执行效率要求非常高*/
Bool  putpixel(int x,int y,int color)                   /*如果(x,y)不在屏幕区域或color不在颜色可以取值的范围内，返回false；若操作成功，返回true*/    
{
    if(0>x || MAX_X<x || 0>y || MAX_Y<y || 0>color || MAX_COLOR<color )
    {
        return false;
    }

    videoBuffer[y*SCREEN_WIDTH+x]=(BYTE)color;

    CURR_X=x;
    CURR_Y=y;
    CURR_COLOR=(BYTE)color;
    return true;
}


BYTE  getpixel(int x, int y)             
{
    if(0>x || MAX_X<x || 0>y || MAX_Y<y )
    {
        return 0;
    }

    return videoBuffer[y*SCREEN_WIDTH+x];     
}


Bool clearZone(int x1,int y1,int x2,int y2,int color)
{
    int  x,y,pixelsInALine,i,j;
    long t;
    BYTE c;
    UINT ct;
    UINT far * bufferTemp;

    /*对数据进行调整*/
    if(x1>x2)
    {   x=x1;   x1=x2;  x2=x;
    }
    if(y1>y2)
    {
        y=y1;   y1=y2;  y2=y;
    }

    /*先对参数进行“过滤”*/
    if(color<0 || color>MAX_COLOR || x1>MAX_X || y1>MAX_Y || x2<0 || y2<0 )
    {
        return false;
    }

    if(x1<0)       x1=0;
    if(y1<0)       y1=0;
    if(x2>MAX_X)   x2=MAX_X;
    if(y2>MAX_Y)   y2=MAX_Y;

    pixelsInALine=x2-x1+1;
    j=pixelsInALine/2;

    c=color;
    ct=(color<<8)|c;           /*使ct的高字节和低字节的值都等于c*/
    
    t=y1*SCREEN_WIDTH;      /*当用BYTE指针类型来表示显存地址时，每行的下标的增量为SCREEN_WIDTH */   

    for(y=y1; y<=y2; y++)
    {       
        bufferTemp=(UINT far *)(videoBuffer+t+x1);  
        for(i=0; i<j; i++)
        {
            bufferTemp[i]=ct;
        }

        if(1==pixelsInALine %2)
        {
            videoBuffer[t+x2]=c;
        }
        t+=SCREEN_WIDTH;
    }

    return true;

}


void outCharInRom(char c,int color, int x, int y, int zoom)
{
    BYTE far * romCharSet=(BYTE far *)0xF000FA6EL;       /*DOS 下，ROM 中ASCII字符的字模码起始地址*/
    BYTE far * charStart = romCharSet + c*8;
    BYTE romCharMod[8];
    int  pixels[64];
    int  i,j,m,n;

    /* 先获取ROM中按位保存的字模码 */
    for(i=0; i<8; i++)
    {
        romCharMod[i]=charStart[i];
    }

    /* 将取出来的位字模码的每一位用一个BYTE来保存 */
    for(i=0; i<8; i++)
    {
        BYTE mod=romCharMod[i];

        for(j=0; j<8; j++)
        {
            if( mod & 0x80 )
               pixels[ i*8+j ]=color;
            else
               pixels[ i*8+j ]=0;

            mod<<=1;
        }
    }

    /* 最后将字模码显示出来 */
    for(i=0; i<8; i++)
    for(j=0; j<8; j++)
    {
        if(pixels[i*8+j])
        {
           for(m=0; m<zoom; m++)
           for(n=0; n<zoom; n++)
           {
               putpixel( x+j*zoom+n, y+i*zoom+m, pixels[i*8+j]);
           }
        }
    }

}



Bool WRScreen(CZone  zone,           /*在屏幕上绘图或读取数据的区域(以屏幕坐标为参考坐标系)，允许超出屏幕区域(这种情况下会自动剪切与屏幕相交的区域作为实际绘图的区域)，但如果与屏幕没有相交部分，将返回false!  */
             CPoint leftUp,         /*图像的左上角起始坐标，可以不在屏幕区域内，但如果参数不合适将导致图像与zone没有相交区域，此时返回false!*/                                    
             CSize  rectSize,       /*图像的大小，即长度和宽度*/
             BYTE * buffer,         /*绘图时图像数据源；读取屏幕时的数据存储区域*/ 
             long maskOrBufferSize, /*绘图时表示掩模码；读取屏幕数据时表示数据存储区域的大小*/
             int  op)               /*对屏幕进行写或读操作，还可以指定水平读写或垂直读写*/
{   
    int operate,drawSequence;
    CZone screen,       /*整个屏幕区域，以屏幕坐标作为参考坐标系*/ 
          picZone;      /*完整的图像需占用的区域，以屏幕坐标作为参考坐标系*/

    CZone realPicZone,  /*表示实际绘图或读取的数据区域，以图像坐标作为参考坐标系*/
          realScrInPic; /*用realScrInPi存储把zone坐标转换成以图像坐标作为参考坐标系得到的新坐标*/
    
    operate=op & 0x0001;    
    drawSequence= op & 0x0002;
/*—求实际将在屏幕上绘图或读取数据的屏幕区域，保存在zone中，以屏幕坐标作为参考坐标系*/
    screen.leftUp.x=0;              /*screen为整个屏幕区域，以屏幕坐标作为参考坐标系*/ 
    screen.leftUp.y=0;
    screen.rightDown.x=MAX_X;
    screen.rightDown.y=MAX_Y;

    picZone.leftUp=leftUp;          /*picZone为完整的图像需占用的区域，以屏幕坐标作为参考坐标系*/
    picZone.rightDown.x=leftUp.x+rectSize.width-1;
    picZone.rightDown.y=leftUp.y+rectSize.height-1;

    if(! publicZone(screen,zone,&zone))
    {
        return false;
    }
    if(! publicZone(zone,picZone,&zone))
    {
        return false;
    }

/*—求实际在图像数据中读取或写入的区域，保存在realPicZone中，以图像坐标作为参考坐标系*/ 
    realPicZone.leftUp.x=0;      /*完整图像在以图像坐标作为参考坐标系的坐标系中的表示*/
    realPicZone.leftUp.y=0;
    realPicZone.rightDown.x=rectSize.width-1;
    realPicZone.rightDown.y=rectSize.height-1;

    realScrInPic.leftUp.x=zone.leftUp.x-leftUp.x;              /*对zone进行坐标系转换*/
    realScrInPic.leftUp.y=zone.leftUp.y-leftUp.y;
    realScrInPic.rightDown.x=zone.rightDown.x-leftUp.x;
    realScrInPic.rightDown.y=zone.rightDown.y-leftUp.y;

    if(! publicZone(realPicZone,realScrInPic,&realPicZone))
    {
        return false;
    }
/*————*/
    if(WRITE==operate)
    {
        return draw(zone, realPicZone, rectSize, buffer, maskOrBufferSize, drawSequence);
    }
    else if(READ==operate)
    {
        return readFromVideo(zone, realPicZone, rectSize, buffer);
    }
    else
    {
        return false;
    }
}



Bool draw(CZone scr, CZone pic, CSize picSize, BYTE * buffer, long mask, int drawSeq)
{
    int  ssx,ssy,sex,sey,sx,sy;
    int  psx,psy,pex,pey,px,py,pwidth,pheight;
    long t1,t2,t3,t4;
    BYTE c,bmask;
    Bool hasMask=false;
    UINT far * scrBuffer;
    UINT far * picBuffer;

    ssx=scr.leftUp.x;   ssy=scr.leftUp.y;   sex=scr.rightDown.x;    sey=scr.rightDown.y;
    psx=pic.leftUp.x;   psy=pic.leftUp.y;   pex=pic.rightDown.x;    pey=pic.rightDown.y;

    pwidth =picSize.width;  pheight=picSize.height;

    if(mask>=0 && mask<=MAX_COLOR)
    {
        hasMask=true;
        bmask=(BYTE)mask;
    }

    if(LINE==drawSeq)
    {
        if(hasMask)
        {
            t1=ssy*SCREEN_WIDTH;
            t2=psy*pwidth;
            for(sy=ssy,py=psy; sy<=sey && py<=pey; sy++,py++)
            {
                for(sx=ssx,px=psx; sx<=sex && px<=pex; sx++,px++)
                {
                    c=buffer[t2+px];
                    if(c!=bmask)
                    {
                        videoBuffer[t1+sx]=c;
                    }
                }
                t1+=SCREEN_WIDTH;
                t2+=pwidth;
            }
        }
        else
        {
            int i,j,pixelsInALine;

            pixelsInALine=sex-ssx+1;
            j=pixelsInALine/2;      /* 实际绘图宽度的一半 */
            
            t1=ssy*SCREEN_WIDTH;
            t2=psy*pwidth;

            for(sy=ssy,py=psy; sy<=sey && py<=pey; sy++,py++)
            {
                scrBuffer=(UINT far *)(videoBuffer+t1+ssx);
                picBuffer=(UINT far *)(buffer+t2+psx);
                
                for(i=0; i<j; i++)
                {
                    scrBuffer[i]=picBuffer[i];
                }
                if(1==pixelsInALine % 2)
                {
                    videoBuffer[t1+sex]=buffer[t2+pex];
                }

                t1+=SCREEN_WIDTH;
                t2+=pwidth;
            }
        }/* if(hasMask) else */
    }
    else if(ROW==drawSeq)
    {
        if(hasMask)
        {
            t3=ssy*SCREEN_WIDTH;
            t4=psy*pwidth;
            for(sx=ssx,px=psx; sx<=sex && px<=pex; sx++,px++)
            {
                t1=t3+sx;       
                t2=t4+px;               
                for(sy=ssy,py=psy; sy<=sey && py<=pey; sy++,py++)
                {
                    c=buffer[t2];
                    if(c!=bmask)
                    {
                        videoBuffer[t1]=c;
                    }
                    t1+=SCREEN_WIDTH;
                    t2+=pwidth;
                }
            }
        }
        else
        {
            t3=ssy*SCREEN_WIDTH;
            t4=psy*pwidth;
            for(sx=ssx,px=psx; sx<=sex && px<=pex; sx++,px++)
            {
                t1=t3+sx;       
                t2=t4+px;               
                for(sy=ssy,py=psy; sy<=sey && py<=pey; sy++,py++)
                {
                    videoBuffer[t1]=buffer[t2];
                    t1+=SCREEN_WIDTH;
                    t2+=pwidth;
                }
            }
        }/* if(hasMask) else */

    }/* else if(ROW==drawSeq) */
    else
    {
        return false;
    }

    return true;

}


Bool readFromVideo(CZone scr, CZone pic, CSize picSize, BYTE * buffer)
{
    int  ssx,ssy,sex,sey,sx,sy;
    int  psx,psy,pex,pey,px,py,pwidth,pheight;
    int  i,j,pixelsInALine;
    long t1,t2,t3,t4;
    UINT far * scrBuffer;
    UINT far * picBuffer;

    ssx=scr.leftUp.x;   ssy=scr.leftUp.y;   sex=scr.rightDown.x;    sey=scr.rightDown.y;
    psx=pic.leftUp.x;   psy=pic.leftUp.y;   pex=pic.rightDown.x;    pey=pic.rightDown.y;

    pwidth =picSize.width;  pheight=picSize.height;
    
    pixelsInALine=sex-ssx+1;
    j=pixelsInALine/2;     
    
    t1=ssy*SCREEN_WIDTH;
    t2=psy*pwidth;

    for(sy=ssy,py=psy; sy<=sey && py<=pey; sy++,py++)
    {
        scrBuffer=(UINT far *)(videoBuffer+t1+ssx);
        picBuffer=(UINT far *)(buffer+t2+psx);
        
        for(i=0; i<j; i++)
        {
            picBuffer[i]=scrBuffer[i];
        }
        if(1==pixelsInALine % 2)
        {
            buffer[t2+pex]=videoBuffer[t1+sex];
        }                   	
        t1+=SCREEN_WIDTH;
        t2+=pwidth;
    }

    return true;
}


void turn180(void)   /*翻转180度*/
{
    int  x,y;
    long i,j,k,m,n;
    BYTE t;

    k=MAX_Y*SCREEN_WIDTH;
    for(y=0; y<(SCREEN_HEIGHT/2); y++)
    {
        m=y*SCREEN_WIDTH;
        n=k-m+MAX_X;
        for(x=0; x<=MAX_X; x++)
        {
            i=m+x;
            j=n-x;
            t=videoBuffer[i];
            videoBuffer[i]=videoBuffer[j];
            videoBuffer[j]=t;
        }
    }
    return true;
}


Bool turnScr(int op)
{
    int  x,y;
    long i,j,k,m,n;
    BYTE bt;

    k=MAX_Y*SCREEN_WIDTH;

    if(HORIZONTAL==op)
    {
        for(x=0; x<(SCREEN_WIDTH/2); x++)
        {       
            k=0;
            for(y=0; y<=MAX_Y; y++)
            {               
                i=k+x;
                j=k+MAX_X-x;
                bt=videoBuffer[i];
                videoBuffer[i]=videoBuffer[j];
                videoBuffer[j]=bt;
                k+=SCREEN_WIDTH;
            }
        }
    }
    else if(VERTICAL==op)
    {
        UINT ut;
        UINT far * buffer1;
        UINT far * buffer2;

        k=SCREEN_WIDTH/2;
        m=0;
        n=MAX_Y*SCREEN_WIDTH;

        for(y=0; y<(SCREEN_HEIGHT/2); y++)
        {
            buffer1=(UINT far * )(videoBuffer+m);
            buffer2=(UINT far * )(videoBuffer+n);       
            
            for(i=0; i<k; i++)
            {               
                ut=buffer1[i];
                buffer1[i]=buffer2[i];
                buffer2[i]=ut;
            }
            m+=SCREEN_WIDTH;
            n-=SCREEN_WIDTH;
        }
    }
    else
    {
        return false;
    }
    return true;
}



long  imagesize(int x1, int y1, int x2, int y2)
{
   long size;
   if(x1>x2)
   {  int temp;
      temp=x1;
      x1=x2;
      x2=temp;
   }

   if(y1>y2)
   {
       int temp;
       temp=y1;
       y1=y2;
       y2=temp;
   }

   size=(long)(x2-x1+1)*(y2-y1+1)+2; /*加2是为了用前两个元素里记录图片的宽和高*/

   return size; 
}



Bool getimage(int x1,int y1,int x2,int y2,unsigned int * buf)    /*如果（x,y）不在屏幕内，则转化到最近的屏幕内坐标处 */
{
   int x,y;
   long index;

   if(x1<0)  x1=0;
   else if(x1>MAX_X)  x1=MAX_X;

   if(y1<0)  y1=0;
   else if(y1>MAX_Y)  y1=MAX_Y;

   if(x2<0)  x2=0;
   else if(x2>MAX_X)  x2=MAX_X;

   if(y2<0)  y2=0;
   else if(y2>MAX_Y)  y2=MAX_Y;

   if(x1>x2)
   {  int temp;
      temp=x1;
      x1=x2;
      x2=temp;
   }

   if(y1>y2)
   {   int temp;
       temp=y1;
       y1=y2;
       y2=temp;
    }

   buf[0]=x2-x1+1;     /* record the width */
   buf[1]=y2-y1+1;     /* record the height*/
   index=2;
   for(y=y1;y<=y2;y++)         /* 一行一行地存储 */
      for(x=x1;x<=x2;x++)
      {   
		  buf[ index ]=videoBuffer[y*SCREEN_WIDTH+x];
          index++;
      }
}



void putimage(int x,int y,unsigned int * buf,int putmode)
{
    int x1,y1,index,x_temp;
    x1=x+buf[0]-1;
    y1=y+buf[1]-1;
    index=2;

    switch(putmode)
    {
         case XOR_PUT:       for(;y<=y1;y++)
                             for(x_temp=x;x_temp<=x1;x_temp++)
                             {  videoBuffer[ y*SCREEN_WIDTH + x_temp]^=buf[index];  index++; }
                             break; 
  
         case OR_PUT:        for(;y<=y1;y++)
                             for(x_temp=x;x_temp<=x1;x_temp++)
                             {  videoBuffer[ y*SCREEN_WIDTH + x_temp]|=buf[index];  index++; }
                             break;        
     
         case AND_PUT:       for(;y<=y1;y++)
                             for(x_temp=x;x_temp<=x1;x_temp++)
                             {  videoBuffer[ y*SCREEN_WIDTH + x_temp]&=buf[index];  index++; }
                             break;        

         case NOT_PUT:       for(;y<=y1;y++)
                             for(x_temp=x;x_temp<=x1;x_temp++)
                             {  videoBuffer[ y*SCREEN_WIDTH + x_temp]= ( buf[index]^0xff );  index++; }
                             break;        

         case COPY_PUT:      
         default:            for(;y<=y1;y++)
                             for(x_temp=x;x_temp<=x1;x_temp++)
                             {  videoBuffer[ y*SCREEN_WIDTH + x_temp]=buf[index];  index++; }
                             break;        
    }
    CURR_X=x1;
    CURR_Y=y1;
}



Bool fastMoveScr(int direction,int pixels,Bool cycle)
{
    BYTE * backUp;
    int  dis,backUpSize;
    int  sX,sY,sStartX,sStartY,sEndX,sEndY;
    int  i;

    /*1、先进行合理性检验,对不合理的参数,返回错误*/
    if( pixels<=0 || !(MOVRIGHT==direction || MOVLEFT==direction || MOVUP==direction || MOVDOWN==direction) )
    {
        return false;
    }

    if(MOVRIGHT==direction || MOVLEFT==direction)
    {
        dis=pixels % SCREEN_WIDTH;
    }
    else if(MOVUP==direction || MOVDOWN==direction)
    {
        dis=pixels % SCREEN_HEIGHT;
    }

    /*2、如果是循环移动,则要保存被"移出"屏幕的图像*/
    if(cycle)    
    {
        /*视不同情况，计算应分配内存的大小*/
        if(MOVRIGHT==direction || MOVLEFT==direction)
        {           
            backUpSize=dis*SCREEN_HEIGHT;
        }
        else if(MOVUP==direction || MOVDOWN==direction)
        {           
            backUpSize=dis*SCREEN_WIDTH;
        }
        else   /*非法操作*/
        {
            return false;
        }

        /*进行内存分配*/
        backUp= (BYTE *) malloc( backUpSize*sizeof(BYTE) );
        if(!backUp)
        {
            return false;
        }

        /*视不同情况,计算从屏幕读取数据的区域*/
        switch(direction)
        {
            case MOVLEFT   :    sStartX=0;   sEndX=dis-1;
                                sStartY=0;   sEndY=MAX_Y;
                                break;
            case MOVRIGHT  :    sStartX=SCREEN_WIDTH-dis;
                                sEndX=MAX_X;
                                sStartY=0;   sEndY=MAX_Y;
                                break;
            case MOVUP     :    sStartX=0;   sEndX=MAX_X;
                                sStartY=0;   sEndY=dis-1;
                                break;
            case MOVDOWN   :    sStartX=0;   sEndX=MAX_X;
                                sStartY=SCREEN_HEIGHT-dis;
                                sEndY=MAX_Y;
                                break;
            default     :       return false;
                                break;
        }

        /*终于可以读取即将被"移出"屏幕的数据了*/
        for(i=0,sY=sStartY; sY<=sEndY; sY++)          /*记住它是从上往下,从左到右存储的，恢复的时候也要从上往下从左到右喔！*/
        for(sX=sStartX; (sX<=sEndX) && (i<backUpSize); sX++,i++)
        {
            backUp[i]=videoBuffer[sY*SCREEN_WIDTH+sX];                  	
        }
    }

    /*3、开始移动吧！动起来！*/
    switch(direction)
    {
        case MOVLEFT  :  for(sX=0; sX<=MAX_X-dis; sX++)   /*从左往右*/
                         for(sY=0;   sY<=MAX_Y  ; sY++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[sY*SCREEN_WIDTH+sX+dis];
                         }
                         break;
        case MOVRIGHT :  for(sX=MAX_X; sX>=dis; sX--)     /*从右往左*/
                         for(sY=0; sY<=MAX_Y  ; sY++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[sY*SCREEN_WIDTH+(sX-dis)];
                         }
                         break;
        case MOVUP    :  for(sY=0; sY<=MAX_Y-dis; sY++)   /*从上往下*/
                         for(sX=0; sX<=MAX_X; sX++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[(sY+dis)*SCREEN_WIDTH+sX];
                         }
                         break;
        case MOVDOWN  :  for(sY=MAX_Y; sY>=dis; sY--)     /*从下往上*/
                         for(sX=0;   sX<=MAX_X; sX++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[(sY-dis)*SCREEN_WIDTH+sX];
                         }
                         break;
        default       :  return false;
                         break;
    }

    /*4、如果是循环移动，还要记得把"移出"屏幕的部分还原喔*/
    if(cycle)
    {       
        switch(direction)
        {
            case MOVLEFT   :    for(i=0,sY=0; sY<=MAX_Y; sY++)     /*从上往下、从左往右*/
                             for(sX=MAX_X-dis+1; sX<=MAX_X; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            case MOVRIGHT  :    for(i=0,sY=0; sY<=MAX_Y; sY++)     /*从上往下、从左往右*/
                             for(sX=0; sX<=dis-1; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            case MOVUP     :    for(i=0,sY=MAX_Y-dis+1; sY<=MAX_Y; sY++)     /*从上往下、从左往右*/
                             for(sX=0;  sX<=MAX_X; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            case MOVDOWN   :    for(i=0,sY=0; sY<dis; sY++)     /*从上往下、从左往右*/
                             for(sX=0;  sX<=MAX_X; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            default     :    return false;
                              break;
        }

        free(backUp);

    }
    
    return true;
    
}



enum Error_Type
{ 
	OPEN_FILE_FAILED = -1,
	READ_FILE_FAILED = -2,
	NOT_BMP_FILE     = -3,
	COMPRESSED_BMP_FILE   = -4,
	NOT_256COLORS_PICTURE = -5,
    MEMERY_ALLOCATE_FAILED= -6,
};
Bool showBMP256(char * fileName, int startX, int startY)
{
    BITMAPFILEHEADER  fileHeader;
    BITMAPINFOHEADER  infoHeader;
    RGBQUAD           colorBoard[256];
	RGB               rgbColors[256];

	int handle, bytes=0;
	BYTE * buffer=NULL;

	int width=0, height=0, biWidth=0;
	int i, j, x, y;

    

	/* 1、先以只读方式打开图片文件 */
	handle=open( fileName, O_RDONLY|O_BINARY );
    if( -1==handle )
	{	
		return OPEN_FILE_FAILED;
	}


	/* 2、读取文件头部信息和图片头部信息 */
	bytes = read( handle, &fileHeader, sizeof(BITMAPFILEHEADER) );
	if( sizeof(BITMAPFILEHEADER)!=bytes )
	{
		close(handle);
		return READ_FILE_FAILED;
	}

	bytes = read( handle, &infoHeader, sizeof(BITMAPINFOHEADER) );
	if( sizeof(BITMAPINFOHEADER)!=bytes )
	{
		close(handle);
		return READ_FILE_FAILED;
	}
    
	/* 3、根据读取的文件头部信息和图片头部信息对文件进行检测，对非256色bmp图片不进行处理 */
	if( fileHeader.bfType!=0x4d42 )
	{
		close(handle);
		return NOT_BMP_FILE;
	}
	if( infoHeader.biCompression!=0 )
	{
		close(handle);
		return COMPRESSED_BMP_FILE;
	}
	if( infoHeader.biBitCount!=8 )
	{
		close(handle);
		return NOT_256COLORS_PICTURE;
	}

	/* 4、读取调色板数据 */
	for( i=0; i<256; i++ )
	{
		bytes = read( handle, &colorBoard[i], sizeof(RGBQUAD) );

		if( sizeof(RGBQUAD)!=bytes )
		{
			close(handle);
			return READ_FILE_FAILED;
		}
	}
 
	/* 5.1、先使屏幕变黑，等将图像数据全部写入显存后，再将图片的调色板数据写入系统调色板寄存器中，这样可以使用户看不见绘图的过程 */
	blackScreen(true);

	/* 5.2、读取图片的象素数据信息，写入显存 */
	width  = infoHeader.biWidth;
	height = infoHeader.biHeight;

	if( (height!=0) && (0==infoHeader.biSizeImage%height))
	{
		biWidth=infoHeader.biSizeImage/height;
	}
	else
	{
		close( handle );
		return false;
	}

	buffer=(BYTE *) malloc( biWidth*sizeof(BYTE) );
	if(!buffer)
	{
		close( handle );
		return MEMERY_ALLOCATE_FAILED;
	}

	for( i=0; i<height; i++ )
	{
		bytes = read( handle, buffer, biWidth*sizeof(BYTE) );

		if( biWidth*sizeof(BYTE)!=bytes )
		{
			close( handle );
			return READ_FILE_FAILED;
		}

		y=(startY+height-1)-i;   /* 从最后一行开始画图 */
		
		if( y<0 || y>MAX_Y)
		{
			continue;
		}

		for( x=startX, j=0; x<(startX+width) && j<biWidth; x++, j++)  /* 从左往右 */
		{
			if(x<0 || x>MAX_X)
			{
				continue;
			}
			videoBuffer[y*SCREEN_WIDTH+x]=buffer[j];
		}

	}

	/* 文件读取结束，关闭文件索引 */
	free(buffer);
	close(handle);

	/* 5.3、将图片的调色板数据写入调色板寄存器 */
	for(i=0; i<256; i++)
	{
		rgbColors[i].red=colorBoard[i].rgbRed>>2;
		rgbColors[i].green=colorBoard[i].rgbGreen>>2;
		rgbColors[i].blue=colorBoard[i].rgbBlue>>2;
	}

	if( !setRegisters(0,256,rgbColors) )
	{
		return false;
	}
	else
	{
		return true;
	}
}
