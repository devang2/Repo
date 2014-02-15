/* Copyright 1990-1993 by Gary R. Olhoeft */
/* showfont.c */
#include <dos.h>
#include <stdio.h>
#include <math.h>
#include <hpgl.h> /* Olhoeft's 32-bit protected mode extended graphics library */
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <3d.h>  
#include <mda.h>

int video_mode; /* both main() & vector() need access to video_mode */

void main(argc, argv)
int argc;
char *argv[];
  {
  int key,ko,jo,i,j,k;
  unsigned char str[80];
  char plot_out[80], *pos_per;
  if (argc<2)
    {
      printf("\n\n\n\nThe calling sequence for this program is:\n");
      printf("\n SHOWFONT font\n");
      printf("where 'font' is:\n");
      printf("  CYRILLIC   Russian\n");
      printf("  GOTHENGL   Gothic English\n");
      printf("  GOTHGERM   Gothic German\n");
      printf("  GOTHITAL   Gothic Italian\n");
      printf("  GRKCMPLX   Greek Complex\n");
      printf("  GRKCMPSM   Greek Complex Small\n");
      printf("  GRKSMPLX   Greek Simplex\n");
      printf("  ITLCMPLX   Italic Complex\n");
      printf("  ITLCMPSM   Italic Complex Small\n");
      printf("  ITLTRPLX   Italic Triplex\n");
      printf("  ROMSMPLX   Roman Simplex (no serifs, single lines)\n");
      printf("  ROMDUPLX   Roman Duplex  (no serifs, double lines)\n");
      printf("  ROMCMPSM   Roman Complex Small \n");
      printf("  ROMCMPLX   Roman Complex (serifs, double lines)\n");
      printf("  ROMTRPLX   Roman Triplex (serifs, triple lines)\n");
      printf("  SCRCMPLX   Script Complex \n");
      printf("  SCRSMPLX   Script Simplex\n");
      printf("  EQNSMPLX   Roman/Math/Greek Equation Simplex\n");
      printf("  EQNCMPLX   Roman/Math/Greek Equation Complex\n");
      printf("  EQNTRPLX   Roman/Math/Greek Equation Triplex\n");
      exit(0);
    }
  strcpy(plot_out, argv[1]);
  pos_per = strchr(plot_out, '.');
  if (pos_per == NULL) strcat(plot_out, ".PLT\0");
  else strncpy( pos_per+1, "PLT\0", 4);
  video_mode = plotter_is(56,"CRT");
  if (video_mode < 56) {plotter_off(); video_mode = plotter_is(1024,"CRT");}
  if (video_mode < 56) {plotter_off(); video_mode = plotter_is(261,"CRT");}
  if (video_mode < 56) {plotter_off(); video_mode = plotter_is(8514,"CRT");}
  full_screen();
  window(0., 133., 0., 100.); pen(15); frame();
  csize(1.); lorg(2); str[1] = 0;
  hpgl_select_font("romcmplx");
  pen(15); 
  for (j=0; j<256; j += 32)
    for (i=j; i<j+32; i++)
      {str[0] = i; label((double)(i-j)*4.15, (double)(j)/2.7+4., str);}
  hpgl_select_font(argv[1]);
  pen(14); 
  for (j=0; j<256; j += 32)
    for (i=j; i<j+32; i++)
      {str[0] = i; label((double)(i-j)*4.15, (double)(j)/2.7+11., str);}
  pen(13); csize(0.5);
  for (j=0; j<256; j += 32)
    for (i=j; i<j+32; i++)
      {
        if (i % 2 == 0) pen(13); else pen(12);
        sprintf(str, "%d", i); 
        label((double)(i-j)*4.15, (double)(j)/2.7+7.5, str); 
      }
  getch();
  plotter_off();
}
