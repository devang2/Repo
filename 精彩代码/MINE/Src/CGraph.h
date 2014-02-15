/* ����ͷ�ļ� */
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


enum pause_ops{     /* ��ͣ��Ļʱ,����Ļ����Ĳ������� */
    GRAY,
    DARK,
    REVERSE
};


enum turn_scr_ops{     /*��ת��Ļ�Ĳ�������*/
    HORIZONTAL,
    VERTICAL
};


enum screen_WR_ops{   
    WRITE=0x0001,     /*��������00000000 00000001д��������*/
    READ =0x0000,     /*��������00000000 00000000����������*/
    LINE =0x0000,     /*��������00000000 00000000���л���*/
    ROW  =0x0002      /*��������00000000 00000010���л���*/ 
};
/*��Ļд�������,��������ϳ�: 

     WRITE������д
     READ �����ж�
     LINE �����ж�
     ROW  �����ж�
WRITE|LINE������д
WRITE|ROW ������д
READ |LINE�����ж�
READ |ROW �����ж�

�����֮�����Ͻ���������,�Ƽ�ʹ�ú��������֮һ*/


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





/*�������������ʾģʽ���д����䡪��*/
#define VGA_320X200X256_MODE 0x13
#define TEXT_MODE 0x03


/*���Ѷ���ĸ�����ʾģʽ��,Ĭ�ϵ�ͼ��ģʽ��320��200��256ɫ,�ر�ͼ��ģʽ��Ĭ�Ͻ����ı�ģʽ��*/
/*�����ı�Ĭ�ϵ�ͼ��ģʽ,�����޸������ֵ*/
#define GRAPHIC_MODE   VGA_320X200X256_MODE           /*1��Ĭ�ϵ�ͼ��ģʽ��320��200��256ɫ */
#define MAX_X          319                            /*2��Ĭ�ϵ���Ļ x ��������ֵΪ 319 */
#define SCREEN_WIDTH   320                            /*3��Ĭ�ϵ���Ļ���Ϊ 320 ����       */
#define MAX_Y          199                            /*4��Ĭ�ϵ���Ļ y ��������ֵΪ 199 */
#define SCREEN_HEIGHT  200                            /*5��Ĭ�ϵ���Ļ�߶�Ϊ 200 ����       */
#define MAX_COLOR      255                            /*6�������ɫֵ�������޸�            */
#define COLORS         256                            /*7����256ɫ�������޸�!              */
#define MAX_RGB        63                             /*8��R��G��B�����ֵΪ63,��6λɫ��   */


/*��������Ҫ��ȫ�ֱ�����������¼��ǰ��Ļ����Ϣ�����������޸ġ���������*/
int  CURR_X;                                    /*��ǰ���ص� x ���� ��*/
int  CURR_Y;                                    /*��ǰ���ص� y ���� ��*/
BYTE CURR_COLOR;                                /*��ǰĬ��ǰ����ɫ����*/
BYTE far *videoBuffer=(char far *)0xA0000000L;  /*ͼ��ģʽ�Դ���ʼλ��*/


/* �������� */






/*��������ʵ�֡���*/

/*������ʾģʽ*/
void setmode(BYTE mode)          
{
    union REGS inregs,outregs;
    inregs.h.ah=0;
    inregs.h.al=mode;
    int86(0x10,&inregs,&outregs);   /* ����10H���жϵ�0���ӹ��� */
}

/*��ʼ��ͼ��ģʽ������û��ʵ�����壬ֻΪ��C����ԭͼ�ο⺯������һ�·��*/
void initgraph(int * gdriver, int * gmode, char * path)
{
    setmode(GRAPHIC_MODE);
    CURR_X=0;
    CURR_Y=0;                           /*��ʼ����Ϊ(0,0) */
    CURR_COLOR=MAX_COLOR;               /*��ʼǰ��ɫΪ��ɫ*/
}

/*�ر�ͼ��ģʽ�������ַ�ģʽ*/
void closegraph(viod)
{
    setmode(TEXT_MODE);
}


/*��ȡ��������Ļ��Ϣ*/
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

int colors(void)         /*COLORSΪ256��BYTE���ò��£����Է���ֵΪUINT��*/
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


/*�������úͶ�ȡ��ɫ��Ĵ�������ɫֵ����*/
/*���е�ɫ����в����Ĳ�����*/
#define PALETTE_MASK          0x3c6
#define PALETTE_REGISTER_RD   0x3c7
#define PALETTE_REGISTER_WR   0x3c8
#define PALETTE_DATA          0x3c9


Bool setPaletteRegister(int index,RGB color)  /*���index��[0��MAX_COLORS]֮�⣬�򷵻�false*/
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


Bool getPaletteRegister(int index,RGB * color)             /*���index��[0��MAX_COLORS]֮�⣬�򷵻�false*/
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


Bool setRegisters(int start,int num,RGB * colors)   /*���[start,start+num-1]����[0��MAX_COLORS]֮��,�򷵻�false*/
{
    int i,j;
    int end=start+num-1;                           /*��Ϊstart��0��ʼ����������Ҫ��1 */
    
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


Bool getRegisters(int start,int num,RGB* colors)    /*���[start,start+num-1]����[0��MAX_COLORS]֮��,�򷵻�false*/
{
    RGB cl;
    int i,j;
    int end=start+num-1;                            /*��Ϊstart��0��ʼ����������Ҫ��1 */

    if(start<0 || start>MAX_COLOR || end<0 || end>MAX_COLOR || num<1 || num>COLORS)
    {   
        return false;
    }

    for(i=0,j=start; j<=end; i++,j++)     
    {
        if( getPaletteRegister(j, &cl) )           /*��ȡ��j����ɫ��Ĵ����ɹ�*/
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


/* ����ͨ����ɫ�弼���������⻭��Ч�� */
Bool grayRegisters(int start,int end)   /*ʹ��ɫ��Ĵ���������ɫ�ɲ�ɫ��ɻ�ɫ*/
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

        f=0.299*r+0.587*g+0.114*b;    /*�Ҷȱ任��ʽ*/

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


Bool reverseRegisters(int start,int end)  /*����Ӧ�ĵ�ɫ��Ĵ�������ɫȡ��ɫ*/
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


Bool LAORegisters(int start,int end,float value) /*  LAORegisters����: light adjuest on registers ��������,value�ǵ���ֵ*/
{
    RGB color;
    int i;
    float r,g,b,maxRGB,maxValue;  /* Ϊ�˱�֤��ɫ��ʧ��,Ӧ������߷�ֵmaxValue,�����������value�� [0��maxValue] ��Ŵ�value��  
                                     ���value����maxValue��Ŵ�maxValue��,maxValue ȡ255/red��255/green��255/blue����ֵ����Сֵ */

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

    default      :   if( !LAOScreen(0.6,true))     /*����Ϊԭ�����ȵ� 3/5 */
                         return false;
                     else
                         break;

    }

    getch();

    return setRegisters(0,COLORS,colors);

}



/*3�������Ƕ��Դ��һϵ�в��������������û���Ҫ���ͼ�Σ����ڵײ��������˶�ִ��Ч��Ҫ��ǳ���*/
Bool  putpixel(int x,int y,int color)                   /*���(x,y)������Ļ�����color������ɫ����ȡֵ�ķ�Χ�ڣ�����false���������ɹ�������true*/    
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

    /*�����ݽ��е���*/
    if(x1>x2)
    {   x=x1;   x1=x2;  x2=x;
    }
    if(y1>y2)
    {
        y=y1;   y1=y2;  y2=y;
    }

    /*�ȶԲ������С����ˡ�*/
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
    ct=(color<<8)|c;           /*ʹct�ĸ��ֽں͵��ֽڵ�ֵ������c*/
    
    t=y1*SCREEN_WIDTH;      /*����BYTEָ����������ʾ�Դ��ַʱ��ÿ�е��±������ΪSCREEN_WIDTH */   

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
    BYTE far * romCharSet=(BYTE far *)0xF000FA6EL;       /*DOS �£�ROM ��ASCII�ַ�����ģ����ʼ��ַ*/
    BYTE far * charStart = romCharSet + c*8;
    BYTE romCharMod[8];
    int  pixels[64];
    int  i,j,m,n;

    /* �Ȼ�ȡROM�а�λ�������ģ�� */
    for(i=0; i<8; i++)
    {
        romCharMod[i]=charStart[i];
    }

    /* ��ȡ������λ��ģ���ÿһλ��һ��BYTE������ */
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

    /* �����ģ����ʾ���� */
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



Bool WRScreen(CZone  zone,           /*����Ļ�ϻ�ͼ���ȡ���ݵ�����(����Ļ����Ϊ�ο�����ϵ)����������Ļ����(��������»��Զ���������Ļ�ཻ��������Ϊʵ�ʻ�ͼ������)�����������Ļû���ཻ���֣�������false!  */
             CPoint leftUp,         /*ͼ������Ͻ���ʼ���꣬���Բ�����Ļ�����ڣ���������������ʽ�����ͼ����zoneû���ཻ���򣬴�ʱ����false!*/                                    
             CSize  rectSize,       /*ͼ��Ĵ�С�������ȺͿ��*/
             BYTE * buffer,         /*��ͼʱͼ������Դ����ȡ��Ļʱ�����ݴ洢����*/ 
             long maskOrBufferSize, /*��ͼʱ��ʾ��ģ�룻��ȡ��Ļ����ʱ��ʾ���ݴ洢����Ĵ�С*/
             int  op)               /*����Ļ����д���������������ָ��ˮƽ��д��ֱ��д*/
{   
    int operate,drawSequence;
    CZone screen,       /*������Ļ��������Ļ������Ϊ�ο�����ϵ*/ 
          picZone;      /*������ͼ����ռ�õ���������Ļ������Ϊ�ο�����ϵ*/

    CZone realPicZone,  /*��ʾʵ�ʻ�ͼ���ȡ������������ͼ��������Ϊ�ο�����ϵ*/
          realScrInPic; /*��realScrInPi�洢��zone����ת������ͼ��������Ϊ�ο�����ϵ�õ���������*/
    
    operate=op & 0x0001;    
    drawSequence= op & 0x0002;
/*����ʵ�ʽ�����Ļ�ϻ�ͼ���ȡ���ݵ���Ļ���򣬱�����zone�У�����Ļ������Ϊ�ο�����ϵ*/
    screen.leftUp.x=0;              /*screenΪ������Ļ��������Ļ������Ϊ�ο�����ϵ*/ 
    screen.leftUp.y=0;
    screen.rightDown.x=MAX_X;
    screen.rightDown.y=MAX_Y;

    picZone.leftUp=leftUp;          /*picZoneΪ������ͼ����ռ�õ���������Ļ������Ϊ�ο�����ϵ*/
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

/*����ʵ����ͼ�������ж�ȡ��д������򣬱�����realPicZone�У���ͼ��������Ϊ�ο�����ϵ*/ 
    realPicZone.leftUp.x=0;      /*����ͼ������ͼ��������Ϊ�ο�����ϵ������ϵ�еı�ʾ*/
    realPicZone.leftUp.y=0;
    realPicZone.rightDown.x=rectSize.width-1;
    realPicZone.rightDown.y=rectSize.height-1;

    realScrInPic.leftUp.x=zone.leftUp.x-leftUp.x;              /*��zone��������ϵת��*/
    realScrInPic.leftUp.y=zone.leftUp.y-leftUp.y;
    realScrInPic.rightDown.x=zone.rightDown.x-leftUp.x;
    realScrInPic.rightDown.y=zone.rightDown.y-leftUp.y;

    if(! publicZone(realPicZone,realScrInPic,&realPicZone))
    {
        return false;
    }
/*��������*/
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
            j=pixelsInALine/2;      /* ʵ�ʻ�ͼ��ȵ�һ�� */
            
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


void turn180(void)   /*��ת180��*/
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

   size=(long)(x2-x1+1)*(y2-y1+1)+2; /*��2��Ϊ����ǰ����Ԫ�����¼ͼƬ�Ŀ�͸�*/

   return size; 
}



Bool getimage(int x1,int y1,int x2,int y2,unsigned int * buf)    /*�����x,y��������Ļ�ڣ���ת�����������Ļ�����괦 */
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
   for(y=y1;y<=y2;y++)         /* һ��һ�еش洢 */
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

    /*1���Ƚ��к����Լ���,�Բ�����Ĳ���,���ش���*/
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

    /*2�������ѭ���ƶ�,��Ҫ���汻"�Ƴ�"��Ļ��ͼ��*/
    if(cycle)    
    {
        /*�Ӳ�ͬ���������Ӧ�����ڴ�Ĵ�С*/
        if(MOVRIGHT==direction || MOVLEFT==direction)
        {           
            backUpSize=dis*SCREEN_HEIGHT;
        }
        else if(MOVUP==direction || MOVDOWN==direction)
        {           
            backUpSize=dis*SCREEN_WIDTH;
        }
        else   /*�Ƿ�����*/
        {
            return false;
        }

        /*�����ڴ����*/
        backUp= (BYTE *) malloc( backUpSize*sizeof(BYTE) );
        if(!backUp)
        {
            return false;
        }

        /*�Ӳ�ͬ���,�������Ļ��ȡ���ݵ�����*/
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

        /*���ڿ��Զ�ȡ������"�Ƴ�"��Ļ��������*/
        for(i=0,sY=sStartY; sY<=sEndY; sY++)          /*��ס���Ǵ�������,�����Ҵ洢�ģ��ָ���ʱ��ҲҪ�������´�����ร�*/
        for(sX=sStartX; (sX<=sEndX) && (i<backUpSize); sX++,i++)
        {
            backUp[i]=videoBuffer[sY*SCREEN_WIDTH+sX];                  	
        }
    }

    /*3����ʼ�ƶ��ɣ���������*/
    switch(direction)
    {
        case MOVLEFT  :  for(sX=0; sX<=MAX_X-dis; sX++)   /*��������*/
                         for(sY=0;   sY<=MAX_Y  ; sY++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[sY*SCREEN_WIDTH+sX+dis];
                         }
                         break;
        case MOVRIGHT :  for(sX=MAX_X; sX>=dis; sX--)     /*��������*/
                         for(sY=0; sY<=MAX_Y  ; sY++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[sY*SCREEN_WIDTH+(sX-dis)];
                         }
                         break;
        case MOVUP    :  for(sY=0; sY<=MAX_Y-dis; sY++)   /*��������*/
                         for(sX=0; sX<=MAX_X; sX++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[(sY+dis)*SCREEN_WIDTH+sX];
                         }
                         break;
        case MOVDOWN  :  for(sY=MAX_Y; sY>=dis; sY--)     /*��������*/
                         for(sX=0;   sX<=MAX_X; sX++)
                         {
                             videoBuffer[sY*SCREEN_WIDTH+sX]=videoBuffer[(sY-dis)*SCREEN_WIDTH+sX];
                         }
                         break;
        default       :  return false;
                         break;
    }

    /*4�������ѭ���ƶ�����Ҫ�ǵð�"�Ƴ�"��Ļ�Ĳ��ֻ�ԭ�*/
    if(cycle)
    {       
        switch(direction)
        {
            case MOVLEFT   :    for(i=0,sY=0; sY<=MAX_Y; sY++)     /*�������¡���������*/
                             for(sX=MAX_X-dis+1; sX<=MAX_X; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            case MOVRIGHT  :    for(i=0,sY=0; sY<=MAX_Y; sY++)     /*�������¡���������*/
                             for(sX=0; sX<=dis-1; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            case MOVUP     :    for(i=0,sY=MAX_Y-dis+1; sY<=MAX_Y; sY++)     /*�������¡���������*/
                             for(sX=0;  sX<=MAX_X; sX++,i++)
                             {
                                 videoBuffer[sY*SCREEN_WIDTH+sX]=backUp[i];
                             }
                             break;
            case MOVDOWN   :    for(i=0,sY=0; sY<dis; sY++)     /*�������¡���������*/
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

    

	/* 1������ֻ����ʽ��ͼƬ�ļ� */
	handle=open( fileName, O_RDONLY|O_BINARY );
    if( -1==handle )
	{	
		return OPEN_FILE_FAILED;
	}


	/* 2����ȡ�ļ�ͷ����Ϣ��ͼƬͷ����Ϣ */
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
    
	/* 3�����ݶ�ȡ���ļ�ͷ����Ϣ��ͼƬͷ����Ϣ���ļ����м�⣬�Է�256ɫbmpͼƬ�����д��� */
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

	/* 4����ȡ��ɫ������ */
	for( i=0; i<256; i++ )
	{
		bytes = read( handle, &colorBoard[i], sizeof(RGBQUAD) );

		if( sizeof(RGBQUAD)!=bytes )
		{
			close(handle);
			return READ_FILE_FAILED;
		}
	}
 
	/* 5.1����ʹ��Ļ��ڣ��Ƚ�ͼ������ȫ��д���Դ���ٽ�ͼƬ�ĵ�ɫ������д��ϵͳ��ɫ��Ĵ����У���������ʹ�û���������ͼ�Ĺ��� */
	blackScreen(true);

	/* 5.2����ȡͼƬ������������Ϣ��д���Դ� */
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

		y=(startY+height-1)-i;   /* �����һ�п�ʼ��ͼ */
		
		if( y<0 || y>MAX_Y)
		{
			continue;
		}

		for( x=startX, j=0; x<(startX+width) && j<biWidth; x++, j++)  /* �������� */
		{
			if(x<0 || x>MAX_X)
			{
				continue;
			}
			videoBuffer[y*SCREEN_WIDTH+x]=buffer[j];
		}

	}

	/* �ļ���ȡ�������ر��ļ����� */
	free(buffer);
	close(handle);

	/* 5.3����ͼƬ�ĵ�ɫ������д���ɫ��Ĵ��� */
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
