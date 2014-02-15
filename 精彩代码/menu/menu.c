#include <dos.h>
#include <bios.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define LEFT          0x4b00          /*    The key code of LEFT        */
#define RIGHT         0x4d00             /*    The key code of RIGHT        */
#define DOWN         0x5000          /*    The key code of DOWN        */
#define UP             0x4800            /*    The key code of UP        */
#define ENTER         0x1c0d             /*    The key code of ENTER        */
#define ALT_M         0x3200             /*    The key code of ALT_B        */
#define ALT_H         0x2300            /*    The key code of ALT_H        */

int key;                            /*    Variable to keep the key code          */
int textx,texty;                    /*    Variable to keep the location of text     */
struct menustruct                    /*    Structure is used to determine menu content                */
{
   char name[10];                    /*    Array is used to keep the names of menu                    */
   char str[10][20];                /*    Array is used to keep the sub menu item of every primary menu    */
   int n;                            /*    Number of sub menu item of every primary menu                */
    }ml[2];                            /*    Create three menu items                                */


char save[4096];                    /*    To save one section    */
char c[4096];                        /*    To save one section    */
int i,j;                            /*    Global Variable        */

void Menu();                        /*    Initialize menu        */
void Selectitem();                    /*    Define menu        */
void DrawSelectitem();                /*    Display main menu    */
void BlackText(int x,int y,char *z);/*    Choose one menu        */
void RedText(int x,int y,char *z);    /*                    */
void Run();                            /*    Determine action        */
void DrawMl(int n);                    /*    Display sub menu item    */
void MoveMl(int n,int x);            /*    */
void Enter(int m,int n);            /*    Call function as menu item*/
void Help();                         /*    Display Help Information */
void Ver();                         /*    Display Version Information */
void ClrScr();                        /*    Clear Screen               */
void DrawFrame(int left,int up,int right,int down,int textcolor,int backgroundcolor);    /*    Draw a rectangle    */

void main(void)
{
   Menu();
   Run();
}

void Menu()
{
   system("cls"); 
   textbackground(BLUE);
   window(1,1,25,80);
   clrscr();
   textx=3;
   texty=2;
   gotoxy(1,2);
   cprintf("%c",218);  /*©°   */
   for(i=0;i<78;i++)
   cprintf("%c",196);   /*£­  */
   cprintf("%c",191);   /*©´   */
   for(i=3;i<=23;i++)
   {
      gotoxy(1,i);
      cprintf("%c",179); /* |  */
      gotoxy(80,i);
      cprintf("%c",179);
   }
   cprintf("%c",192);    /*	|_   */
   for(i=0;i<78;i++)
      cprintf("%c",196);  
   cprintf("%c",217);    /*_|   */
   gotoxy(1,1);
   textcolor(7); 
   for(i=0;i<80;i++)
      cprintf("%c",219); /*¡ö   */
   Selectitem();  
   DrawSelectitem(); 
   gettext(2,3,78,23,c); 
}

void Selectitem()
{
   strcpy(ml[0].name,"Menu");
   strcpy(ml[0].str[0],"Item        ");
   strcpy(ml[0].str[1],"Exit        ");
   ml[0].n=2; 
   strcpy(ml[1].name,"Help");
   strcpy(ml[1].str[0],"System      ");
   strcpy(ml[1].str[1],"Ver         ");
   ml[1].n=2;
}

void DrawSelectitem()
{
   for(i=0;i<2;i++)
   RedText(i,1,ml[i].name); 
}

void RedText(int x,int y,char *z)
{
   textbackground(7); 
   gotoxy(3+x*20,y);
   for(j=0;z[j];j++)
   {
      if(j==0)
  textcolor(RED);
      else
  textcolor(BLACK);
      cprintf("%c",z[j]);
   }
}

void BlackText(int x,int y,char *z)
{
   textbackground(0); 
   textcolor(15); 
   gotoxy(3+20*x,y);
   cputs(z); 
}

void Run()
{
   while(1)
   {
      gotoxy(texty,textx);
      key=bioskey(0);
      switch(key)
      {
        case ALT_M: DrawMl(0);break;
        case ALT_H: DrawMl(1); break;
        case UP:  break;
        case DOWN: break;
        case LEFT: break;
        case RIGHT: break;
        case ENTER: break;
        default : break;
      }
   }
}

void DrawFrame(int l,int u,int r,int d,int tcolor,int bcolor)
{
   textbackground(bcolor); 
   textcolor(bcolor);
   for(i=l;i<=r;i++) 
   {
      for(j=u;j<=d;j++)
      {
        gotoxy(i,j);
        cprintf("%c",219);
      }
   }
   textcolor(tcolor);
   for(i=u+1;i<d;i++) 
   {
      gotoxy(l,i);
      cprintf("%c",179);
      gotoxy(r,i);
      cprintf("%c",179);
   }
   for(i=l+1;i<r;i++)
   {
      gotoxy(i,u);
      cprintf("%c",196);
      gotoxy(i,d);
      cprintf("%c",196);
   }
   gotoxy(l,u);
   cprintf("%c",218);
   gotoxy(r,u);
   cprintf("%c",191);
   gotoxy(l,d);
   cprintf("%c",192);
   gotoxy(r,d);
   cprintf("%c",217);
}

void DrawMl(int n)
{
   gettext(1,1,80,25,save);
   BlackText(n,1,ml[n].name);
   DrawFrame(3+20*n-1,2,3+20*n+19,3+ml[n].n,0,7);
   for(i=3;i<3+ml[n].n;i++)
   {
      if(i==3)
        BlackText(n,i,ml[n].str[i-3]);
      else
        RedText(n,i,ml[n].str[i-3]);
   }
   gotoxy(79,1);
   MoveMl(n,3);
}

void MoveMl(int n,int x)
{
   int flag=1;
   while(flag)
   {
      gotoxy(79,1);
      key=bioskey(0);
      gotoxy(79,1);
      switch(key)
      {
        case LEFT:
            puttext(1,1,80,25,save);
            if(n==0)
                DrawMl(1);
            else
                DrawMl(n-1);
            flag=0;
            break;
        case RIGHT:
            puttext(1,1,80,25,save);
            if(n==1)
                DrawMl(0);
            else
                DrawMl(n+1);
            flag=0;
            break;
        case UP:
            RedText(n,x,ml[n].str[x-3]);
            if(x==3)
                x=3+ml[n].n-1;
            else
                x--;
            BlackText(n,x,ml[n].str[x-3]);
            flag=1;
            break;
        case DOWN:
            RedText(n,x,ml[n].str[x-3]);
            if(x==(3+ml[n].n-1))
                x=3;
            else
                x++;
            BlackText(n,x,ml[n].str[x-3]);
            flag=1;
            break;
        case ENTER:
            puttext(1,1,80,25,save);
            Enter(n,x-3);
            flag=0;
            break;
        }
    gotoxy(79,1);
   }
}

void Enter(int m,int n)
{
   switch(m)
   {
      case 0:switch(n) 
      {
        case 0: break;
        case 1:exit(0);break;
      } break;
     case 1:switch(n) 
     {
        case 0:Help();break;
        case 1:Ver(); break;
     }break;
   }
}


void ClrScr()
{
   int i,j;
   puttext(2,3,78,23,c);
   gotoxy(2,3);
}

void Help()
{
   ClrScr();
   DrawFrame(10,5,50,8,0,7); 
   gotoxy(15,6);
   cprintf("click menu to run each funtion");
   getch();
   ClrScr();
}

void Ver()
{
   ClrScr();
   DrawFrame(10,5,50,8,0,7);
   gotoxy(15,6);
   cprintf("Ver 1.0 finished by flying_bread");
   getch();
   ClrScr();
}  
