#include <stdio.h>
#include <string.h>
#include "xms.hpp"

void main()
{
	char buff[]="DuDuDuDu........HaHaHaHa";

	printf("-------- Test XMB Function ------------\n");

	int xms_hd = xms.AllocXmb(100);
	if(xms_hd) {
		printf("Sccess alloc extended memory 64Kb block\n");

		if(xms.CmbMovetoXmb((char far *)buff,xms_hd,0,84000L)){
			printf("Sccess move conventional memory to extended memory\n");

			if(xms.XmbMovetoCmb(xms_hd,0,(char far *)buff,strlen(buff))){
				printf("Sccess move extended memory to conventional memory\n");
				printf("%s\n",buff);
			}
			else printf("XmbMovetoCmb :%s\n",xms.ErrorMess());
		}
		else printf("CmbMovetoXmb :%s\n",xms.ErrorMess());

		if(xms.FreeXmb(xms_hd))	printf("Sccess free extended memory\n");
		else printf("FreeXmb :%s\n",xms.ErrorMess());
	}
	else printf("AllocXmb:%s\n",xms.ErrorMess());
	printf("------- Test Upper Memory Function -------\n");
	char far * umb_hd = (char far *)xms.AllocUmb(1000);
	if(!umb_hd)
		printf("Alloc upper memory error.\n");
	else {
		printf("Alloced upper memory 1000x16 bytes");
		if(xms.FreeUmb(umb_hd))
			printf("Free upper memory sccess.\n");
	}
}
