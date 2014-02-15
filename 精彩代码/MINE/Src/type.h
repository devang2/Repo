/*基本数据类型*/
typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef int           Bool;
typedef unsigned int  WORD;
typedef unsigned long DWORD;
typedef unsigned long LONG;
typedef int           MSG;   /*消息类型*/


/*基本常量*/
#define NULL  0
#define true  1
#define false 0




/*RGB三元色*/
typedef struct CRGBQUAD
{
    BYTE red;
    BYTE green;
    BYTE blue;
} RGB;



/*坐标*/
typedef struct Point
{
   long x,y;
} CPoint;



/*区域*/
typedef struct Zone
{
	CPoint leftUp;        /*左上角坐标*/
	CPoint rightDown;     /*右下角坐标*/
} CZone;



/*尺寸*/
typedef struct Size
{
    UINT width,height;
} CSize;




/*——bmp文件所用到的3个结构体——*/
typedef struct tagBITMAPFILEHEADER
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD
{
    BYTE  rgbBlue;
    BYTE  rgbGreen;
    BYTE  rgbRed;
    BYTE  rgbReserved;
} RGBQUAD;
/*————————————————*/