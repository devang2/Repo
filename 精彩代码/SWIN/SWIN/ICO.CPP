#include "gui.hpp"
#include "ico.hpp"

void ShowIco(int x,int y,char * icofile)
{
	FILE * icofp = fopen(icofile,"rb");
	if(icofp==NULL) return ;
	IcoHeader icoHeader;
	fread(&icoHeader,1,sizeof(IcoHeader),icofp);
	int size = icoHeader.width*icoHeader.height ;
	if(strcmp(icoHeader.icoId,"ICO")==0) {
		char * img = new char[size];
		if(img){
			fread(img,1,size,icofp);
			for(int i = 0 ; i < icoHeader.height ; i ++){
				for(int j=0;j< icoHeader.width;j++){
					int color = img[i*icoHeader.height+j];
					if(color) putpixel(x+j,y+i,color);
				}
			}
			delete img ;
		}
	}
	fclose(icofp);
}