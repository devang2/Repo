#include <dos.h>
#include <dir.h>
#include <string.h>
#include <conio.h>
#include <alloc.h>

#include "\vga\vga.h"
#include "text.h"

void asc16_24(VGABASE *A);
void ascbc(VGABASE *A);
void ascsl(VGABASE *A);
void hz16(VGABASE *A);
void hz24(VGABASE *A);
void hz32(VGABASE *A);
void hz40(VGABASE *A);
void hzuc(VGABASE *A,int i);

void dfont()
{
int n=12;
char *str[12]={	" ASCK16, ASCK24 (Path: \\VGA\\FONT) ",
				" *.CHR  Borland Font (Path: \\VGA\\FONT) ",
				" ASCKSL (Path: \\VGA\\FONT) ",
				" HZK16 (Path: \\VGA\\FONT) ",
				" HZK24S,H,K,F,T (Path: \\VGA\\FONT) ",
				" HZK32S,H,K,F,T (Path: \\VGA\\FONT) ",
				" HZK40S,H,K,F,T (Path: \\VGA\\FONT) ",
				" HZKSLSTJ (Path: C:\\UCDOS) ",
				" HZKSLHTJ (Path: C:\\UCDOS) ",
				" HZKSLKTJ (Path: C:\\UCDOS) ",
				" HZKSLFSJ (Path: C:\\UCDOS) ",
				" Return "};
int i;
int x0=20,y0=6;
unsigned colh=color(0,7,0,0),coll=color(7,0,0,0);
void *buf;
_640_480_16 A;
XMS::init();
buf=malloc(4200);

cclsall();
mmvprs(0,79,3," ASCII & CC Font ",color(2,0,0,0));
mmvprs(0,79,4,"ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ",color(2,0,0,0));
i=0;
while(1) {
	i=menu(n,i,x0,y0,coll,colh,str);
	gettext(1,1,80,25,buf);
	if( i==-1 || i==(n-1) )
		break;
	A.init();
	if(i==0)
		asc16_24(&A);
	else if(i==1)
		ascbc(&A);
	else if(i==2)
		ascsl(&A);
	else if(i==3)
		hz16(&A);
	else if(i==4)
		hz24(&A);
	else if(i==5)
		hz32(&A);
	else if(i==6)
		hz40(&A);
	else
		hzuc(&A,i-7);
	A.close();
	puttext(1,1,80,25,buf);
	}
free(buf);
cclsall();
}

void asc16_24(VGABASE *A)
{
int i,j,x,y;
unchar *c="  ";
ASC16 t1("\\vga\\font\\asck16",inMEM,A);
ASC24 t2("\\vga\\font\\asck24",inMEM,A);
A->setcolor(4);
t1.outtextxy(0,0,"16*8 dot ASCII Font");
A->setcolor(7);
x=0; y=20; j=0;
for(i=0;i<256;i++)
	{
	c[0]=i;
	t1.outchar(x,y,c);
	x+=10;
	j++;
	if(j>=64)
		{ j=0; x=0; y+=20; }
	}

y+=32;
A->setcolor(4);
t2.outtextxy(0,y,"24*12 dot ASCII Font");
A->setcolor(7);
y+=32;
x=0; j=0;
for(i=0;i<256;i++)
	{
	c[0]=i;
	t2.outchar(x,y,c);
	x+=16;
	j++;
	if(j>=40)
		{ j=0; x=0; y+=32; }
	}
getch();
}

void ascbc(VGABASE *A)
{
char *file[10]={"TRIP.CHR",
				"LITT.CHR",
				"SANS.CHR",
				"GOTH.CHR",
				"SCRI.CHR",
				"SIMP.CHR",
				"TSCR.CHR",
				"LCOM.CHR",
				"EURO.CHR",
				"BOLD.CHR"};
char *font[10]={"Triplex ",
				"Small ",
				"Sans Serif ",
				"Gothic ",
				"Script ",
				"Simplex ",
				"Triplex Scr ",
				"Complex ",
				"European ",
				"Blod "};
char *str;
str=(char *)malloc(81);
int i,y=0,dy;
char *buf;
buf=(char *)malloc(81);
getcwd(buf,80);
chdir("\\vga\\font");
ASCBC *t;
ASC16 tf("ASCK16",inMEM,A);
for(i=0;i<10;i++)
	{
	A->setcolor(7);
	tf.outtextxy(0,y,file[i]);
	strcpy(str,font[i]);
	if(i<8)
		strcat(str,"Stroked Font");
	t=new ASCBC(file[i],inMEM,A);
	t->outtextxy(100,y,str);
	dy=t->gettexthigh()+12;
	if(dy<28) dy=28;
	y+=dy;
	A->line(0,y-1,A->WIDE-1,y-1,A->setcolorto(4));
	delete t;
	}
chdir(buf);
free(buf); free(str);
getkey();
}

void ascsl(VGABASE *A)
{
int y=0,dy=64;
ASCSL t("\\vga\\font\\ASCKSL",inMEM,A);
A->setcolor(7);
t.setsize(48,48,0);
t.outtextxy(0,y,"ABCDEFGHIJKLMN"); y+=dy;
t.outtextxy(0,y,"OPQRSTUVWXYZ"); y+=dy;
t.outtextxy(0,y,"abcdefghijklmnopqrst"); y+=dy;
t.outtextxy(0,y,"uvwxyz"); y+=dy;
t.outtextxy(0,y,"1234567890"); y+=dy;
t.outtextxy(0,y,"~!@#$%^&*()_+-"); y+=dy;
t.outtextxy(0,y,"=|\\{}[]:;\"\'<>?,./"); y+=dy;
getkey();
}

void hz16(VGABASE *A)
{
HZ16 t("\\vga\\font\\hzk16",inXMS,A);
A->setcolor(7);
t.outtextxy(0,100,"£±£¶¡Á£±£¶µãÕóºº×Ö");
getkey();
}

void hz24(VGABASE *A)
{
int y=0,dy=48;
HZ24 ts("\\vga\\font\\hzk24s",inHD,A);
HZ24 th("\\vga\\font\\hzk24h",inHD,A);
HZ24 tk("\\vga\\font\\hzk24k",inHD,A);
HZ24 tf("\\vga\\font\\hzk24f",inHD,A);
TX24 tt("\\vga\\font\\hzk24t",inHD,A);
ts.join(&tt);
th.join(&tt);
tk.join(&tt);
tf.join(&tt);
A->setcolor(7);
ts.outtextxy(0,y,"£²£´¡Á£²£´µãÕóºº×Ö¡¡¡¡ËÎÌå"); y+=dy;
th.outtextxy(0,y,"£²£´¡Á£²£´µãÕóºº×Ö¡¡¡¡ºÚÌå"); y+=dy;
tk.outtextxy(0,y,"£²£´¡Á£²£´µãÕóºº×Ö¡¡¡¡¿¬Ìå"); y+=dy;
tf.outtextxy(0,y,"£²£´¡Á£²£´µãÕóºº×Ö¡¡¡¡·ÂËÎ"); y+=dy;
getkey();
}

void hz32(VGABASE *A)
{
int y=0,dy=56;
HZ32 ts("\\vga\\font\\hzk32s",inHD,A);
HZ32 th("\\vga\\font\\hzk32h",inHD,A);
HZ32 tk("\\vga\\font\\hzk32k",inHD,A);
HZ32 tf("\\vga\\font\\hzk32f",inHD,A);
TX32 tt("\\vga\\font\\hzk32t",inHD,A);
ts.join(&tt);
th.join(&tt);
tk.join(&tt);
tf.join(&tt);
A->setcolor(7);
ts.outtextxy(0,y,"£³£²¡Á£³£²µãÕóºº×Ö¡¡¡¡ËÎÌå"); y+=dy;
th.outtextxy(0,y,"£³£²¡Á£³£²µãÕóºº×Ö¡¡¡¡ºÚÌå"); y+=dy;
tk.outtextxy(0,y,"£³£²¡Á£³£²µãÕóºº×Ö¡¡¡¡¿¬Ìå"); y+=dy;
tf.outtextxy(0,y,"£³£²¡Á£³£²µãÕóºº×Ö¡¡¡¡·ÂËÎ"); y+=dy;
getkey();
}

void hz40(VGABASE *A)
{
int y=0,dy=64;
HZ40 ts("\\vga\\font\\hzk40s",inHD,A);
HZ40 th("\\vga\\font\\hzk40h",inHD,A);
HZ40 tk("\\vga\\font\\hzk40k",inHD,A);
HZ40 tf("\\vga\\font\\hzk40f",inHD,A);
TX40 tt("\\vga\\font\\hzk40t",inHD,A);
ts.join(&tt);
th.join(&tt);
tk.join(&tt);
tf.join(&tt);
A->setcolor(7);
ts.outtextxy(0,y,"£³£¶¡Á£´£°µãÕóºº×Ö¡¡¡¡ËÎÌå"); y+=dy;
th.outtextxy(0,y,"£³£¶¡Á£´£°µãÕóºº×Ö¡¡¡¡ºÚÌå"); y+=dy;
tk.outtextxy(0,y,"£³£¶¡Á£´£°µãÕóºº×Ö¡¡¡¡¿¬Ìå"); y+=dy;
tf.outtextxy(0,y,"£³£¶¡Á£´£°µãÕóºº×Ö¡¡¡¡·ÂËÎ"); y+=dy;
getkey();
}

void hzuc(VGABASE *B,int i)
{
VGA16 *A=(VGA16 *)B;
FONT *t;
if(i==0)
	t=new HZUC_S("\\vga\\font\\smlsls",inHD,A);
else if(i==1)
	t=new HZUC_S("\\vga\\font\\smlslh",inHD,A);
else if(i==2)
	t=new HZUCK_S("\\vga\\font\\smlslk",inHD,A);
else if(i==3)
	t=new HZUC_S("\\vga\\font\\smlslf",inHD,A);
A->setcolor(4);
t->outtextxy(0,0,"Ê§");
t->outtextxy(0,100,"Á¿");
t->outtextxy(0,200,"ºº");
t->outtextxy(0,300,"×Ö");
t->setsize(480,480);
A->setcolor(7);
int c7;
c7=A->getpalette(7);
A->setdac(c7,0,0,0);
if(i==0)
	t->outtextxy(100,0,"ËÎ");
else if(i==1)
	t->outtextxy(100,0,"ºÚ");
else if(i==2)
	t->outtextxy(100,0,"¿¬");
else if(i==3)
	t->outtextxy(100,0,"·Â");
int j;
for(j=16;j<136;j++)
	{
	A->setdac(c7,j,j,j);
	delay(32);
	}
getkey();
for(j=136;j>0;j--)
	{
	A->setdac(c7,j,j,j);
	delay(16);
	}
A->setdac(c7,128,128,128);
delete t;
}
