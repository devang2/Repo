#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <math.h>
#include <graphics.h>
float rh=40,th=0.7,ph=1,d=250,cx=140,cy=96,s1,c1,s2,c2,xt,yt,r1,r2;
float v[19][4]={{0,0,0,0},{0,5,7,-5},{0,5,7,5},{0,5,-7,5},{0,5,-7,-5},{0,-5,7,-5},
	  {0,-5,-7,-5},{0,-5,-7,5},{0,-5,7,5},{0,0,7,8},{0,0,-7,8},{0,-2,-7,-5},{0,-2,-9,-5},
	  {0,-5,-9,-5},{0,-5,-7,-5},{0,-2,-7,9},{0,-2,-9,9},{0,-5,-9,9},{0,-5,-7,9}};
float x1,y1,z1,sv[18][3],u1,u2,u3,v1,v2,v3,e1;
int nps[14]={0,5,6,5,6,5,5,5,5,5,5,5,5,5};
int s[14][7]={{0,0,0,0,0,0}, {0,1,2,3,4,1,0}, {0,1,5,8,9,2,1}, {0,5,6,7,8,5,0},
		{0,4,3,10,7,6,4}, {0,3,2,9,10,3,0}, {0,7,10,9,8,7,0},	{0,1,4,6,5,1,0},
		{0,11,12,13,14,11,0},
		{0,15,18,17,16,15,0},
		{0,11,14,18,15,11,0},{0,12,16,17,13,12,0},{0,11,15,16,12,11,0},
		{0,14,13,17,18,14,0}
		};
int n[14][4];
int xe,ye,ze,e2,j,k,l,p1,q,xh,yh,xv,yv;
int e[3][13][4];
int p[11];
int vi,eo;
int ob=1,n1=1,e2,j,k;
float vx,vy,vz;
void main(){
int i;
int gdriver=DETECT,gmode,errorcode;
initgraph(&gdriver,&gmode,"");
errorcode = graphresult();
if (errorcode!=grOk)
  {printf("graphices error:%s\n",grapherrormsg(errorcode));
  printf("press anykey to halt");
  getch();
  exit(1);}


s1=sin(th);
c1=cos(th);
s2=sin(ph);
c2=cos(ph);
for (i=1;i++;i<19){
  x1=-v[i][1]*s1+v[i][2]*c1;
  y1=-v[i][1]*c1*c2-v[i][2]*s1*c2+v[i][3]*s2;
  z1=-v[i][1]*s2*c1-v[i][2]*s2*s1-v[i][3]*c2+rh;
  sv[i][1]=d*(x1/z1)+cx;
  sv[i][2]=-d*(y1/z1)+cy;}
for (i=1;i++;i<14){
	   u1=v[s[i][2]][1]-v[s[i][1]][1];
	   u2=v[s[i][2]][2]-v[s[i][1]][2];
	   u3=v[s[i][2]][3]-v[s[i][1]][3];
	   v1=v[s[i][3]][1]-v[s[i][1]][1];
	   v2=v[s[i][3]][2]-v[s[i][1]][2];
	   v3=v[s[i][3]][3]-v[s[i][1]][3];
	   n[i][1]=u2*v3-v2*u3;
	   n[i][2]=u3*v1-v3*u1;
	   n[i][3]=u1*v2-v1*u2;}
       xe=rh*s2*c1;
   ye=rh*s2*s1;
   ze=rh*c2;


 for (i=1;i++;i<14){
	e2=s[i][1];
	vx=xe-v[e2][1];
	vy=ye-v[e2][2];
	vz=ze-v[e2][3];
  if(n[i][1]*vx+n[i][2]*vy+n[i][3]*vz<=0) goto  k920;
	e1=s[i][1];
	for (j=2;j++;j<=nps[i]){
	  e2=s[i][j];
	  for (k=1;k++;k<=n1){
		   if (e[ob][k][1]==e2 && e[ob][k][2]==e1){
			  e[ob][k][3]=2;
			  goto
			   k900;}}
		e[ob][n1][1]=e1;
		e[ob][n1][2]=e2;
		e[ob][n1][3]=1;
		n1++;
  k900:      e1=e2;}
 k920: if (i==7){
		   ob=2;
		   n1=1;
		   }
		   }

   if (ye>=-7) {
	p[1]=1;
	p[2]=2;}
	else
	  {p[1]=2;
	  p[2]=1;}
	  for (i=1; i++;i<=12){
		if (e[p[1]][i][3]=0) break;
		j=e[p[1]][i][1];
		k=e[p[1]][i][2];
		moveto(sv[j][1],sv[j][2]);
		lineto(sv[k][1],sv[k][2]);}

  for (eo=1;eo++;eo<=12){
	 if (e[p[2]][eo][3]=0) return 0;
	 i=e[p[2]][eo][1];
	 j=e[p[2]][eo][2];
	 if (sv[i][3]==0){
	   xt=sv[i][1];
	   yt=sv[i][2];
	   vi=-1;
	   for (l=1;l++;l<=12){
		 if (e[p[1]][l][3]!=1 ) break;
		 p1=e[p[1]][l][1];
		 q=e[p[1]][l][2];
		 r1=sv[q][1]-sv[p1][1];
		 r2=sv[q][2]-sv[p1][2];
		 u1=xt-sv[p1][1];
		 u2=yt-sv[p1][2];
		 if(u2*r1-u1*r2>=0) {
		 vi=1; exit(0);} }
		 sv[i][3]=vi;
					 }

	 if (sv[j][3]==0) {
	   xt=sv[j][1];
	   yt=sv[j][2];
	   vi=-1;
	   for (l=1;l++;l<=12){
		 if (e[p[1]][l][3]!=1 ) break;
		 p1=e[p[1]][l][1];
		 q=e[p[1]][l][2];
		 r1=sv[q][1]-sv[p1][1];
		 r2=sv[q][2]-sv[p1][2];
		 u1=xt-sv[p1][1];
		 u2=yt-sv[p1][2];
		 if(u2*r1-u1*r2>=0)  {
		 vi=1;exit(0);}}
		 sv[j][3]=vi;             }
		if (sv[i][3]+sv[j][3]==-2) break;
		if (sv[i][3]+sv[j][3]!=0 ){
		moveto(sv[i][1],sv[i][2]);
		lineto(sv[j][1],sv[j][2]);    }
   else {if (sv[i][3]==1) {
				xv=sv[i][1];
				yv=sv[i][2];
				xh=sv[j][1];
				yh=sv[j][2];}

			 if (sv[j][3]==1) {
				xv=sv[j][1];
				yv=sv[j][2];
				xh=sv[i][1];
				yh=sv[i][2];}

			v1=(xh-xv)/2;
			v2=(yh-yv)/2;
			xt=xv+v1;
			yt=yv+v2;
			for (i=2;i++;i<=7){
			 vi=-1;
		 for (l=1;l++;l<=12){
		 if (e[p[1]][l][3]!=1 ) break;
		 p1=e[p[1]][l][1];
		 q=e[p[1]][l][2];
		 r1=sv[q][1]-sv[p1][1];
		 r2=sv[q][2]-sv[p1][2];
		 u1=xt-sv[p1][1];
		 u2=yt-sv[p1][2];
		 if(u2*r1-u1*r2>=0) {
		 vi=1; exit(0);} }
		 v1=v1/2;
		 v2=v2/2;
		 xt=xt+vi*v1;
		 yt=yt+vi*v2;}
		 moveto(xv,yv);
		 lineto(xt,yt);
		 }} }





