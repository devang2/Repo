#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int bfType ;					//always "BM"
	long bfsize ;					//file size
	int bfReserved1 ;				//must = 0
	int bfReserved2 ;				//must = 0
	long bfOffBits ;				//image data offset
}BITMAPFILEHEADER ;

typedef struct {
	long biSize ;					//size of BITMAPINFOHEADER
	long biWidth;					//image width
	long biHeight ;				//image height
	int biPlanes ;					//must be 1
	int biBitCount;				//bits per pixel,must be 1,4,8 or 24
	long biCompress;				//compress method
	long biSizeImage;				//image size
	long biXPeIsPerMeter;		//pixel per meter in herizontal direction
	long biYPeIsPerMeter;		//pixel per meter in vertical direction
	long biCIrUsed;				//color number used
	long biCIrImportant;			//important colors,if all colors are important,
										// it should be set 0
}BITMAPINFOHEADER;

typedef struct {
	char icoid[4] ;
	unsigned icoWidth ;
	unsigned icoHeight ;
	char * icoImage ;
}ICO;

struct RGB
{
	char red ;
	char green ;
	char blue ;
};

long bmpOff ,icoSize ;
RGB bmpPal[16] ;
RGB swinPal[16] ;
unsigned char ColorTable[16] ={ 0,4,2,14,1,5,3,8,7,4,2,14,1,5,3,15 };

void Bmp2Ico(FILE * bmpfp,ICO& ico);
void SaveIcoFile(FILE * icofp,ICO * ico);

#define abs(x) x<0?-x:x

int cmppal(int ci,int di)
{
	int redok,greenok,blueok;
	redok = greenok = blueok = 0 ;
	for(int i = 0 ; i < 16 ; i ++){
		redok = swinPal[i].red - bmpPal[ci].red;
		greenok = swinPal[i].green - bmpPal[ci].green;
		blueok = swinPal[i].blue - bmpPal[ci].blue;
		if((abs(redok) >= 0 && abs(redok) <= di)
			&&(abs(greenok) >= 0 && abs(greenok) <= di)
			&&(abs(blueok) >= 0 && abs(blueok) <= di))
		{
			return i ;
		}
	}
	return -1;
}

void ChangePal(char * palFile)
{
	FILE * palfp = fopen(palFile,"rb");
	if(palfp == NULL){
		printf("Error open palette file name\n");
		exit(1);
	}
	fread((char*)&swinPal,1,48,palfp);
	fclose(palfp);
	for(int ci = 0 ; ci <= 16 ; ci++){
		for(int di = 0 ; di <= 0xff ; di ++ ){
			int c = cmppal(ci,di) ;
			if(c!=-1){
				ColorTable[ci] = c ;
				break ;
			}
		}
	}
}




int main(int argc,char **argv)
{
	FILE * bmpfp,* icofp ;
	BITMAPFILEHEADER FileHeader ;
	BITMAPINFOHEADER InfoHeader ;
	ICO ico;

	if(argc != 3)
	{
		printf("Usage:%s <bmpfile> <icofile>\n",strupr(argv[0]));
		return 1;
	}

	if((bmpfp=fopen(argv[1],"rb")) == NULL)
	{
		printf("[%s] open error.\n",argv[1]);
		return 1;
	}
	if( fread((char *)&FileHeader,1,sizeof(BITMAPFILEHEADER),bmpfp)
		!= sizeof(BITMAPFILEHEADER) )
	{
		printf("[%s] read error.\n",argv[1]);
		return 1;
	}
	if( fread((char *)&InfoHeader,1,sizeof(BITMAPINFOHEADER),bmpfp)
		!= sizeof(BITMAPINFOHEADER) )
	{
		printf("[%s] read error.\n",argv[1]);
		return 1;
	}
	if(InfoHeader.biCompress != 0){
		printf("Only supports non_compressed BMP file.\n");
		return 1;
	}

	if(InfoHeader.biBitCount != 4){
		printf("[%s] is not a 16 color BMP file.\n",argv[1]);
		return 1;
	}

	memset((char*)&bmpPal,0,3*16);
	fseek(bmpfp,54,SEEK_SET);
	for(int i = 0 ; i < 16 ;i ++){
		bmpPal[i].red = (char)fgetc(bmpfp);
		bmpPal[i].green = (char)fgetc(bmpfp);
		bmpPal[i].blue = (char)fgetc(bmpfp);
		fgetc(bmpfp);
	}

//	ChangePal("swin.pal");

	if((icofp=fopen(argv[2],"wb"))==NULL){
		printf("[%s] open error\n",argv[2]);
		return 1;
	}

	bmpOff = (long)FileHeader.bfOffBits;

	ico.icoWidth = (unsigned)InfoHeader.biWidth ;
	ico.icoHeight = (unsigned)InfoHeader.biHeight ;
	ico.icoid[0] = 'I';
	ico.icoid[1] = 'C';
	ico.icoid[2] = 'O';
	ico.icoid[3] = 0 ;
	icoSize = (long)ico.icoWidth*ico.icoHeight;
	ico.icoImage = new char[icoSize] ;
	if(!ico.icoImage){
		printf("Memory not enough !\n");
		return 1;
	}
	Bmp2Ico(bmpfp,ico);
	SaveIcoFile(icofp,(ICO *)&ico);
	delete ico.icoImage;
	return 0 ;
}

void Bmp2Ico(FILE * bmpfp,ICO& ico)
{
	fseek(bmpfp,bmpOff,SEEK_SET);
	for(int h = ico.icoHeight-1 ; h >= 0 ; h--){
		for(int w = 0 ; w < ico.icoWidth ; w++){
			int bitchar = fgetc(bmpfp);
			ico.icoImage[h*ico.icoWidth + w] = ColorTable[bitchar>> 4];
//			ico.icoImage[h*ico.icoWidth + w] = bitchar>> 4;
			if(w < ico.icoWidth ){
				ico.icoImage[h*ico.icoWidth + (++w)] = ColorTable[bitchar&0x000f];
//				ico.icoImage[h*ico.icoWidth + (++w)] = bitchar&0x000f;
			}
		}
	}
	fclose(bmpfp);
}

void SaveIcoFile(FILE * icofp,ICO * ico)
{
	fwrite(ico,1,8,icofp);
	fwrite(ico->icoImage,1,icoSize,icofp);
	fclose(icofp);
}
