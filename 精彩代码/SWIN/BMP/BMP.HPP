#include <stdio.h>
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
	unsigned icoWidth ;
	unsigned icoHeight ;
	char * icoImage ;
}ICO;

long bmpOff ,icoSize ;
char palette[48];

void Bmp2Ico(FILE * bmpfp,ICO& ico);
void SaveIcoFile(FILE * icofp,ICO * ico);


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

	memset(palette,0,3*16);
	fseek(bmpfp,54,SEEK_SET);
	for(int i = 0 ; i < 48 ;i ++){
		palette[3*i+2] = (char)fgetc(bmpfp);
		palette[3*i+1] = (char)fgetc(bmpfp);
		palette[3*i] = (char)fgetc(bmpfp);
		fgetc(bmpfp);
	}

	if((icofp=fopen(argv[2],"wb"))==NULL){
		printf("[%s] open error\n",argv[2]);
		return 1;
	}

	bmpOff = (long)FileHeader.bfOffBits;

	ico.icoWidth = (unsigned)InfoHeader.biWidth ;
	ico.icoHeight = (unsigned)InfoHeader.biHeight ;
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
	for(int h = ico.icoHeight ; h > 0 ; -- h){
		char * tmpptr = ico.icoImage+h*ico.icoWidth ;
		for(int w = 0 ; w <ico.icoWidth ; w++){
			int bitchar = fgetc(bmpfp);
			*tmpptr = bitchar&0x000f;
			tmpptr ++ ;
			if(++w < ico.icoWidth ){
				bitchar = bitchar >> 4 ;
				* tmpptr = bitchar;
				tmpptr ++;
			}
		}
	}
	fclose(bmpfp);
}

void SaveIcoFile(FILE * icofp,ICO * ico)
{
	fwrite(ico,1,4,icofp);
	fwrite(ico->icoImage,1,icoSize,icofp);
	fclose(icofp);
}
