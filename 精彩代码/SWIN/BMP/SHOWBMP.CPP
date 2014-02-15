#include <stdio.h>
#include <dos.h>
#include <graphics.h>
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

unsigned char palette[48] ;

void SavePalette(char * p ,char * fname)
{
	FILE * fp = fopen(fname,"wb");
	if(fp == NULL) {
		printf("%s open error",fname);
		return ;
	}
	fwrite(p,1,48,fp);
	fclose(fp);
}

int show16bmp(int x,int y,const char * fname)
{
	FILE * bmpfp ;
	BITMAPFILEHEADER FileHeader ;
	BITMAPINFOHEADER InfoHeader ;

	if((bmpfp=fopen(fname,"rb")) == NULL)
	{
		printf("[%s] open error.\n",fname);
		return 1;
	}
	if( fread((char *)&FileHeader,1,sizeof(BITMAPFILEHEADER),bmpfp)
		!= sizeof(BITMAPFILEHEADER) )
	{
		printf("[%s] read error.\n",fname);
		return 1;
	}
	if( fread((char *)&InfoHeader,1,sizeof(BITMAPINFOHEADER),bmpfp)
		!= sizeof(BITMAPINFOHEADER) )
	{
		printf("[%s] read error.\n",fname);
		return 1;
	}
	if(InfoHeader.biCompress != 0){
		printf("Only supports non_compressed BMP file.\n");
		return 1;
	}

	if(InfoHeader.biBitCount != 4){
		printf("[%s] is not a 16 color BMP file.\n",fname);
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

//	SetPalette(palette);
	union REGS regs ;
	struct SREGS sregs ;
	for( i = 0 ; i< 16 ; i ++ )
	{
		regs.h.ah = 0x10 ;
		regs.h.al = 0x00 ;
		regs.h.bh = i ;
		regs.h.bl = i ;
		int86(0x10,&regs,&regs);
	}
	for( i = 0 ; i < 48 ; i ++ ){
		palette[i] = palette[i] >> 2 ;
		printf("%d ",palette[i]);
		if((i+1)%3==0) printf("\n");
	}

	regs.x.ax = 0x1012 ;
	regs.x.bx = 0 ;
	regs.x.cx = 16 ;
	regs.x.dx = FP_OFF(palette);
	sregs.es = FP_SEG(palette);
	int86x(0x10,&regs,&regs,&sregs);


	fseek(bmpfp,FileHeader.bfOffBits,SEEK_SET);

	for( i = InfoHeader.biHeight ; i > 0 ;  -- i ){
		for(int j = 0 ; j < InfoHeader.biWidth ; j ++ ){
			int pix = fgetc(bmpfp) ;
			putpixel(x+j,y+i,pix>>4);
			j ++ ;
			if(j >= InfoHeader.biWidth) break ;
			putpixel(x+j,y+i,pix&0x000f);
		}
	}
	fclose(bmpfp);
   return 0 ;
}

#include <graphics.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

int main(void)
{
	/* request auto detection */
	int gdriver = DETECT, gmode, errorcode;

	/* initialize graphics and local variables */
	initgraph(&gdriver, &gmode, "c:\\borlandc\\bgi");

	/* read result of initialization */
	errorcode = graphresult();
	if (errorcode != grOk)  /* an error occurred */
	{
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		exit(1); /* terminate with an error code */
	}

	show16bmp(0,0,"setup.bmp");
	FILE * fp = fopen("Dragon.pal","wb");
	if(fp == NULL) {
		printf("open pal error");
		return 0 ;
	}
	fwrite(palette,1,48,fp);
	fclose(fp);

	getch();

	/* clean up */
	closegraph();
	return 0;
}

