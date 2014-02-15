/* 
   Example program for the NEO SDK, by Dong Kai.
   http://neosdk.91i.net
   ckerhome@yahoo.com.cn

   在13H下实现三维空间中旋转的文字
   由网友glvenlan的作品改写过来的
*/

#include <math.h>
#include "neo.h"

#define CLS() {unsigned int i; for(i=0;i<64000;i++)*(g_videoptr+i)=0;}
#define REF() movedata(FP_SEG(g_videoptr),FP_OFF(g_videoptr),0xa000,0,64000)

typedef struct
{
   int x, y;
}_2D;

typedef struct
{
   float x, y, z;
}_3D;

typedef struct
{
   _3D pos, t[4];
   _2D v[4], s[4];
   float ax, ay, az;
   int dx, dy, dz;
   int xmax, ymax;
   char far *buf;
}OBJ;

void triangle(_2D, _2D, _2D, int);

_3D p3d(float x,float y,float z)
{
   _3D t;
   t.x = x, t.y = y, t.z = z;
   return t;
}

_3D vec_sub(_3D a, _3D b)
{
   _3D t;
   t.x = a.x - b.x,
   t.y = a.y - b.y,
   t.z = a.z - b.z;
   return t;
}

float vec_dot(_3D a, _3D b)
{
   float t;
   t = a.x * b.x + a.y * b.y + a.z * b.z;
   return t;
}

OBJ gl;
_3D P,M,N;
float NP, MP, A, B, C, D;

main()
{
   neo_init();
   set_vbe_mode(0x13);
   g_videoptr = (char far *)farmalloc(64000);
   CLS();
   gl.buf = (char far *)farmalloc(64 * 8);
   gl.xmax = 64, gl.ymax = 8;
   gl.v[0].x = 0, gl.v[0].y = 0,
   gl.v[1].x = 0, gl.v[1].y = gl.ymax - 1;
   gl.v[2].x = gl.xmax-1, gl.v[2].y = gl.ymax - 1,
   gl.v[3].x = gl.xmax-1, gl.v[3].y = 0;
   gl.dx = -32, gl.dy = -4, gl.dz = -40;
   gl.ax = gl.ay = gl.az = 0;
   gl.pos.x = 0, gl.pos.y = 0, gl.pos.z = 100;
   printf("NEO-SDK");
   {
      int x, y;
      for (y = 0; y < gl.ymax; y++)
      for (x = 0; x < gl.xmax; x++) *(gl.buf + x + y * 64) = *((char far *)0xa0000000 + x + y * 320);
   }

   while (1)
   {
      int i;
      _3D t, tt;

      if (kbhit())
      {
         char k = getch();
         if (k == 0x1b) break;
         getch();
      }

      for(i = 0; i < 4; i++)
      {
         t.x = gl.v[i].x + gl.dx;
         t.y = gl.v[i].y + gl.dy; t.y *= 2;
         t.z = gl.dz;
         tt.x = t.x;
         tt.y = t.y * cos(gl.ax) - t.z * sin(gl.ax);
         tt.z = t.y * sin(gl.ax) + t.z * cos(gl.ax);
         t.x = tt.z * sin(gl.ay) + tt.x * cos(gl.ay);
         t.y = tt.y;
         t.z = tt.z * cos(gl.ay) - tt.x * sin(gl.ay);
         tt.x = t.x * cos(gl.az) - t.y * sin(gl.az);
         tt.y = t.x * sin(gl.az) + t.y * cos(gl.az);
         tt.z = t.z;
         gl.t[i].x = t.x = tt.x + gl.pos.x;
         gl.t[i].y = t.y = tt.y + gl.pos.y;
         gl.t[i].z = t.z = tt.z + gl.pos.z <= 0?1 : tt.z + gl.pos.z;
         gl.s[i].x = 160 + 200 * t.x / (t.z <= 0?1 : t.z);
         gl.s[i].y = 100 + 200 * t.y / (t.z <= 0?1 : t.z);
      }

      {
         float x1, x2, x3, y1, y2, y3, z1, z2, z3;
         x1 = gl.t[0].x, y1 = gl.t[0].y, z1 = gl.t[0].z;
         x2 = gl.t[1].x, y2 = gl.t[1].y, z2 = gl.t[1].z;
         x3 = gl.t[2].x, y3 = gl.t[2].y, z3 = gl.t[2].z;
         A = y1 * (z2 - z3) + y2 * (z3 - z1) + y3 * (z1 - z2);
         B = z1 * (x2 - x3) + z2 * (x3 - x1) + z3 * (x1 - x2);
         C = x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2);
         D = -x1 * (y2 * z3 - y3 * z2) - x2 * (y3 * z1 - y1 * z3) - x3 * (y1 * z2 - y2 * z1);
         P = gl.t[0], M = vec_sub(gl.t[3], P);
         N = vec_sub(gl.t[1], P);
         MP = vec_dot(M, M), NP = vec_dot(N, N);
      }
      triangle(gl.s[0], gl.s[1], gl.s[2], ((int)gl.ay&63)+32);
      triangle(gl.s[0], gl.s[2], gl.s[3], ((int)gl.ay&63)+32);
      REF();
      CLS();
      gl.ax += 0.01, gl.ay += 0.05, gl.az += 0.02;
   }
   farfree(g_videoptr);
   farfree(gl.buf);
}


void fill_line(int lx,int rx,int i,int color)
{ 
   int x, y, c;
   float tx, ty, tz;
   _3D T_g;

   if (lx > 319)
      return;
   if (i < 0)
      return;
   if (i > 199)
      return;
   if (lx < 0)
      lx = 0;
   if (rx > 319)
      rx = 319;

   for(; lx <= rx; lx++)
   {
      x = lx - 160, y = i - 100;
      tz = -D / ( (A * x + B * y) / 200 + C ), tx = x * tz / 200, ty = y * tz / 200;
      T_g = vec_sub(p3d(tx, ty, tz), P );
      x = vec_dot(T_g, M) * gl.xmax / MP;
      if((unsigned)x >= gl.xmax)
         x=gl.xmax-1;
      y = vec_dot(T_g, N) * gl.ymax / NP;
      if ((unsigned)y >= gl.ymax)
         y=gl.ymax-1;
      c =* (gl.buf + x + y * gl.xmax);
      if(c)
         _dot(lx, i, color);
   }
}

void triangle(_2D a, _2D b, _2D c, int color)
{
   _2D p1, p2, p3;
   long dx1, dx2, dy1, dy2;
   int lx, rx, i;
   unsigned int p, q;

   if(a.y < b.y)
   {
      if(a.y < c.y)
      {
         p1 = a;
         if(b.y < c.y)
         p2 = b, p3 = c;
         else p2 = c, p3 = b;
      }
      else p1 = c, p2 = a, p3 = b;
   }
   else
   {
      if(b.y < c.y)
      {
         p1 = b;
         if(a.y < c.y)
            p2=a,p3=c;
         else
            p2=c,p3=a;
      }
      else
         p1=c, p2=b, p3=a;
   }
   dx1 = p2.x - p1.x, dy1 = p2.y - p1.y;
   dx2 = p3.x - p1.x, dy2 = p3.y - p1.y;
   if (dy1)
      for(i = p1.y; i <= p2.y; i++)
      {
         lx = p1.x + (i - p1.y) * dx1 / dy1;
         rx = p1.x + (i - p1.y) * dx2 / dy2;
         if (lx < rx)
            p = lx, q = rx;
         else
            p = rx, q = lx;
         fill_line(p, q, i, color);
      }
   dx1 = p3.x - p2.x, dy1 = p3.y - p2.y;
   if (dy1)
   {
      for (i = p2.y; i <= p3.y; i++)
      {
         lx = p2.x + (i - p2.y) * dx1 / dy1;
         rx = p1.x + (i - p1.y) * dx2 / dy2;
         if (lx < rx)
            p = lx, q = rx;
         else
            p = rx, q = lx;
         fill_line(p, q, i, color);
      }
   }
}

