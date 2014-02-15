/*������������*/
typedef unsigned char BYTE;
typedef unsigned int  UINT;
typedef int           Bool;
typedef unsigned int  WORD;
typedef unsigned long DWORD;
typedef unsigned long LONG;
typedef int           MSG;   /*��Ϣ����*/


/*��������*/
#define NULL  0
#define true  1
#define false 0




/*RGB��Ԫɫ*/
typedef struct CRGBQUAD
{
    BYTE red;
    BYTE green;
    BYTE blue;
} RGB;



/*����*/
typedef struct Point
{
   long x,y;
} CPoint;



/*����*/
typedef struct Zone
{
	CPoint leftUp;        /*���Ͻ�����*/
	CPoint rightDown;     /*���½�����*/
} CZone;



/*�ߴ�*/
typedef struct Size
{
    UINT width,height;
} CSize;




/*����bmp�ļ����õ���3���ṹ�塪��*/
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
/*��������������������������������*/