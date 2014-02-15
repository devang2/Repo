#include "event.hpp"
#include <stdio.h>
#include <conio.h>

void main()
{
	clrscr();
	while(1){
		while(!event.Present());
		if(event.type == KEYBD){
			gotoxy(1,1);
			printf("Your type key valuse:0x%04x",event.key);
			if(event.key==0x011b) break ;
		} else {
			gotoxy(1,2);
			mouse.Release(LEFT);

			if(mouse.count){
				printf("You mouse.count = %d    \n",mouse.count);
				printf("You mouse.curx = %d     \n",mouse.curx);
				printf("You mouse.cury = %d     \n",mouse.cury);
			}
		}
	}
}