/******************************************************************/
/*   FILENAME: DRAWSCR.C                                          */
/*   AUTHER :  ZHANG_RUI_JUN                                      */
/*   MODIFY :  Modified by Bob kong, 1993.4.9                     */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <string.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>
#include <menufunc.inc>

#include "menu_def.inc"

#define   HEAD_BK_CLR   9
#define   BOTM_BK_CLR   11


UI  	G_xad; 					/* The X addition of 1th polldown menu */
UI	G_bx;					/* The X coordation of 1th polldown menu */
UI      Z_bx;                   /* The X coordation of 2th polldown menu */
UI	Z_by;					/* The Y coordation of 2th polldown menu */
UI	Z_xad;					/* The X addition of 1th polldown menu */

UC	H_status = FALSE;	/* The 0th menu status: 0/1=non_active/active */
UC	Pop_down1 = FALSE;      /* The 1th polldown menu(3th menu) status:
				 TRUE = polldown, FALSE = not polldown */
UC	Pop_down2 = FALSE;	/* The 2th polldown menu(4th menu) status */
UC      Want_pop1 = FALSE;


HMENU *P_menu[MAX_LEVEL] = { NULL,NULL,NULL,NULL,NULL};
				/* records of every level menu */
UC Tmp_main_curr = 0;		/* Record the last main_menu, used for anti-flash */

/*extern UC Privi; */		/* main.c */
extern HMENU  *Menu_stru;	/* pointer to the menu structure date.(INIT_M.C)*/
extern UC *Menu[MAX_NODE];      /* pointer to the menu text date. (INIT_M.C)*/
extern UC *Help[MAX_NODE];      /* pointer to the menu text date. (INIT_M.C)*/
extern char patterns[][8];

/* ======================================================================= */
void echo_head_title(UC bk_color, UC color)
{
UC *tl;
UI x1 = T_EX+8;
UI x2 = MAX_X-8;
UI y1 = 6+2;
UI y2 = S_BY-6;

	tl = Menu[P_menu[0]->sub1+P_menu[0]->curr];
	setfillstyle(SOLID_FILL, bk_color);
	bar(x1, y1, x2, y2);
	hz16_disp( x1+(x2-x1-8*strlen(tl))/2, 10, tl, color );
}

static void get_zen_xad(void)
/* ------------------------------------------------------------------
	Calculate the X addition of 2th polldown menu.
	The addition is the maxium length of its submenus' text string
-------------------------------------------------------------------   */
{
UI j;

	Z_xad = 0;
	for(j=0; j<P_menu[4]->sub_number; j++)
	{
		if ( Z_xad<strlen(Menu[P_menu[4]->sub1+j]) )
			Z_xad = strlen(Menu[P_menu[4]->sub1+j]);
	}
	Z_xad = (Z_xad<<3) + 20;
}

static void get_zen_xpos(void)
/* ------------------------------------------------------------------
	Calculate the (X,Y) coordation of 2th polldown menu.
	The Coord. is obtained according to its father's Coord.
-------------------------------------------------------------------   */
{
	Z_by = G_BY + P_menu[3]->curr * G_YAD;
	Z_bx = G_bx + G_xad + 11;
	if ( (Z_bx+Z_xad+1)>(D_RIGHT-7) )
		Z_bx = G_bx - Z_xad - 17;
}

void init_zen_menu(void)
/* poll down the 2th polldown menu */
{
UI k;

	get_zen_xad();
	get_zen_xpos();

/* ----------------- if the Y > D_BOTTOM-3, extened up ------------------ */
	while((Z_by+P_menu[4]->sub_number*Z_YAD+10)>(D_BOTTOM-3))
	{
		Z_by = Z_by - G_YAD;
	}
/* ------------------------ draw the selections ------------------------- */

	draw_back(Z_bx-5, Z_by-4, Z_bx+Z_xad+5,
				Z_by+P_menu[4]->sub_number*Z_YAD+5, 7);
	for(k=0;k<P_menu[4]->sub_number;k++)
	{
		draw_key(Z_bx, Z_by+k*Z_YAD+1, Z_bx+Z_xad,
				Z_by+(k+1)*Z_YAD, Menu[P_menu[4]->sub1+k], 7, 0);
	}

/* --------------------- show the current choice --------------------- */
	k = P_menu[4]->curr;
	chang_key(Z_bx,Z_by+k*Z_YAD+1,Z_bx+Z_xad,
			Z_by+(k+1)*Z_YAD,Menu[P_menu[4]->sub1+k],15);

        /* ---- Draw Help window ---- */
	draw_back(0,D_BOTTOM+1,M_EX,479, BOTM_BK_CLR);
	disp_esc();
//        hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);

        /* ---- draw Exit window ---- */
        draw_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, BOTM_BK_CLR);
//        hz16_disp(MAX_X-S_XAD+2, D_BOTTOM+8, "07556404502", 0);
        draw_status_window(1);                /* reset the status' windows */
	help_mess(Help[P_menu[4]->sub1+k]);

}

void disp_esc(void)
{
    hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);
}


void hide_zen_menu(void)
/* Hide the 2th polldown menu */
{
/*
	get_zen_xad();
	get_zen_xpos();
*/
	while((Z_by+P_menu[4]->sub_number*Z_YAD+14)>(D_BOTTOM-3))
	{
		Z_by = Z_by - G_YAD;
	}
//	setfillstyle(SOLID_FILL,7);
	setfillpattern( &patterns[0][0], 15);

	bar(Z_bx-5, Z_by-4, Z_bx+Z_xad+5,
		Z_by+P_menu[4]->sub_number*Z_YAD+5);
	setfillstyle(SOLID_FILL,7);

}

static void get_gen_xad(void)
/* ------------------------------------------------------------------
	Calculate the X addition of 1th polldown menu.
	The addition is the maxium length of its submenus' text string
-------------------------------------------------------------------   */
{
UI j;

	G_xad = 0;
	for(j=0;j<P_menu[3]->sub_number;j++)
	{
		if ( G_xad<strlen(Menu[P_menu[3]->sub1+j]) )
			G_xad = strlen(Menu[P_menu[3]->sub1+j]);
	}
	G_xad = (G_xad<<3) + 20;
}

static void get_gen_xpos(void)
/* ------------------------------------------------------------------
	Calculate the X coordation of 1th polldown menu.
	The Coord. is obtained according to its father's Coord.
-------------------------------------------------------------------   */
{
	if(P_menu[2]->curr == 0)
		G_bx = 4 /*12*/ +G_BX;
	else
	{
		if(P_menu[2]->curr == 5)
			G_bx = D_RIGHT-G_xad-17;
		else
			G_bx = P_menu[2]->curr*S_XAD + G_BX;
	}
}

void init_gen_menu(void)
/* poll down the 1th polldown menu */
{
UI k;

	get_gen_xad();
	get_gen_xpos();

	draw_back(G_bx-5, G_BY-4, G_bx+G_xad+5,
		G_BY+P_menu[3]->sub_number*G_YAD+5, 7);
	for(k=0; k<P_menu[3]->sub_number; k++)
	{
		draw_key(G_bx, G_BY+k*G_YAD+1, G_bx+G_xad,
				G_BY+(k+1)*G_YAD, Menu[P_menu[3]->sub1+k], 7, 0);
	}
	k = P_menu[3]->curr;
	chang_key(G_bx, G_BY+k*G_YAD+1, G_bx+G_xad,
				G_BY+(k+1)*G_YAD, Menu[P_menu[3]->sub1+k], 15);

	/* ---- Draw Help window ---- */
	draw_back(0,D_BOTTOM+1,M_EX,479, BOTM_BK_CLR);
	disp_esc();
//        hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);

	/* ---- draw Exit window ---- */
	draw_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, BOTM_BK_CLR);
//        hz16_disp(MAX_X-S_XAD+2, D_BOTTOM+8, "07556404502", 0);
	draw_status_window(1);                /* reset the status' windows */
		help_mess(Help[P_menu[3]->sub1+k]);
}

void hide_gen_menu(void)
{
/*
	get_gen_xad();
	get_gen_xpos();
*/
//	setfillstyle(SOLID_FILL,7);
	setfillpattern( &patterns[0][0], 15);
	bar(G_bx-5, G_BY-4, G_bx+G_xad+5, G_BY+P_menu[3]->sub_number*G_YAD+5);
	setfillstyle(SOLID_FILL,7);
}

void init_sub_menu(void)
/* draw the 2th menu -- horizon */
{
UI k;
UI left, top, right, bottom;
UC *string;

UC sub_num;
UC curr_sub;
UC draw_to;
HMENU *tmp;

/*	draw_sub_menu();*/
	sub_num = P_menu[2]->sub_number;
	curr_sub = P_menu[2]->curr;
	tmp = Menu_stru + P_menu[1]->sub1 + Tmp_main_curr;
	draw_to = max(sub_num, tmp->sub_number);

	if (sub_num==0)
	{
		for(k=0;k<draw_to;k++)
		{
			if  (k==tmp->curr)
				draw_key(S_BX+k*S_XAD+1, S_BY, S_BX+(k+1)*S_XAD, S_EY, "", 7, 0);
			else
			{
				setfillstyle(SOLID_FILL,7);
				bar(S_BX+k*S_XAD+6, S_BY+7, S_BX+(k+1)*S_XAD-5, S_EY-7);
			}
		}
	}
	else
	{
		setfillstyle(SOLID_FILL, 7);
		top = S_BY;
		bottom = S_EY;
		for(k=0; k<draw_to; k++)
		{
			if ( k!=curr_sub )
			{
				left = S_BX+k*S_XAD+1;
				right = S_BX+(k+1)*S_XAD;
				string = Menu[P_menu[2]->sub1+k];
				if ( k == tmp->curr)
				{
					if (k>=sub_num)
						draw_key(left,top,right,bottom,"",7,0);
					else
						draw_key(left,top,right,bottom,string,7,0);
				}
				else
				{
					bar(left+5,top+7,right-5,bottom-7);
					if (k<sub_num)
						hz16_disp( left + (( right-left-(strlen(string)<<3) )>>1),
							  ((top+bottom)>>1)-7, string, 0);
				}
			}
		}
		chang_key(S_BX+curr_sub*S_XAD+1, S_BY, S_BX+(curr_sub+1)*S_XAD,S_EY,
						Menu[P_menu[2]->sub1+curr_sub],15);
                /* ---- Draw Help window ---- */
                draw_back(0,D_BOTTOM+1,M_EX,479, BOTM_BK_CLR);
	        disp_esc();
//                hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);

                /* ---- draw Exit window ---- */
		draw_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, BOTM_BK_CLR);
//                hz16_disp(MAX_X-S_XAD+2, D_BOTTOM+8, "07556404502", 0);
		draw_status_window(1);                /* reset the status' windows */
		help_mess(Help[P_menu[2]->sub1+curr_sub]);
	}
}

void init_main_menu(void)
/* draw the 1th menu -- vertical */
{
UI k;

	draw_main_menu();
	if((P_menu[1]->sub_number != 0)&&(P_menu[1]!=NULL))
	{
		for(k=0;k<P_menu[1]->sub_number;k++)
		{

			write_key_data(M_BX,M_BY+k*M_YAD+1,M_EX,
				M_BY+(k+1)*M_YAD,Menu[P_menu[1]->sub1+k],0);
		}
		k = P_menu[1]->curr;
		chang_key(M_BX,M_BY+k*M_YAD+1,M_EX,M_BY+(k+1)*M_YAD,
				Menu[P_menu[1]->sub1+k],15);

                /* ---- Draw Help window ---- */
                draw_back(0,D_BOTTOM+1,M_EX,479, BOTM_BK_CLR);
	        disp_esc();
//                hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);

                /* ---- draw Exit window ---- */
                draw_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, BOTM_BK_CLR);
//                hz16_disp(MAX_X-S_XAD+2, D_BOTTOM+8, "07556404502", 0);
                draw_status_window(1);                /* reset the status' windows */
		help_mess(Help[P_menu[1]->sub1+k]);
	}
}

void init_head_menu(void)
/* draw  the 0th menu  Changed flag: Pop_down1, Pop_down2, H_status */
{
UI i,j,k;

	i = 0;
	j = 0;

	draw_head_menu();			/* reset the 0th menu */
	for(k=0;k<P_menu[0]->sub_number;k++)
	{
		i = k/H_X_M;
		j = k%H_X_M;
		write_key_data(H_BX+j*H_XAD+1, H_BY+i*H_YAD+1,
				H_BX+(j+1)*H_XAD, H_BY+(i+1)*H_YAD,
				Menu[P_menu[0]->sub1+k], 0);
	}
	i = P_menu[0]->curr/H_X_M;
	j = P_menu[0]->curr%H_X_M;
	k = P_menu[0]->curr;
	chang_key(H_BX+j*H_XAD+1,H_BY+i*H_YAD+1,
			H_BX+(j+1)*H_XAD,H_BY+(i+1)*H_YAD,
				Menu[P_menu[0]->sub1+k],15);
        /* ---- Draw Help window ---- */
        draw_back(0,D_BOTTOM+1,M_EX,479, BOTM_BK_CLR);
        disp_esc();
//        hz16_disp(12,D_BOTTOM+8,"Esc:退出",0);

        /* ---- draw Exit window ---- */
        draw_back(MAX_X-S_XAD, D_BOTTOM+1, MAX_X, MAX_Y, BOTM_BK_CLR);
//        hz16_disp(MAX_X-S_XAD+2, D_BOTTOM+8, "07556404502", 0);
	draw_status_window(1);                /* reset the status' windows */
	help_mess(Help[P_menu[0]->sub1+k]);
}

void draw_screen(UC stat_win_number)
{
	draw_menu(stat_win_number);

	/* ---- Echo the whole menu's title ---- */
	topic_disp1(S_BX+(T_EX-S_BX-12*strlen(Menu[0]))/2-2,7-2,Menu[0],0,0);
	topic_disp1(S_BX+(T_EX-S_BX-12*strlen(Menu[0]))/2-1,7-1,Menu[0],15,15);
	topic_disp1(S_BX+(T_EX-S_BX-12*strlen(Menu[0]))/2+1,7+1,Menu[0],0,0);
	topic_disp1(S_BX+(T_EX-S_BX-12*strlen(Menu[0]))/2,7,Menu[0],7,7);

	P_menu[0] = Menu_stru;
	P_menu[1] = Menu_stru+P_menu[0]->sub1+P_menu[0]->curr;
	if( P_menu[0]->curr<P_menu[0]->sub_number && P_menu[1]->sub_number!=0 )
	{
		init_main_menu();
		P_menu[2] = Menu_stru + P_menu[1]->sub1 + P_menu[1]->curr;
		if (P_menu[2]->sub_number!=0)
		{
			P_menu[3] = Menu_stru + P_menu[2]->sub1 + P_menu[2]->curr;
			if ( P_menu[3]->sub_number!=0 )
				P_menu[4] = Menu_stru + P_menu[3]->sub1 + P_menu[3]->curr;
		}
		init_sub_menu();
		echo_head_title(HEAD_BK_CLR,14);  //(11, 0);
	}
	else
	{
		P_menu[0]->curr = 0;
//		clr_DialWin(1);
		clr_DialWin(0);
		H_status = TRUE;
	}
}

/* ------------------------------------------------------------------------
   FUNCTION:	The same as draw_screen(), but do not draw the head menu.
   ------------------------------------------------------------------------ */
void draw_screen1(UC stat_win_number)
{
	draw_menu(stat_win_number);
	/* ---- Echo the whole menu's title ---- */
	hz16_disp(S_BX+(T_EX-S_BX-8*strlen(Menu[0]))/2,8,Menu[0],3);
//        topic_disp1(S_BX+(T_EX-S_BX-8*strlen(Menu[0]))/2,4,Menu[0],3,3);

	P_menu[0] = Menu_stru;
	P_menu[1] = Menu_stru+P_menu[0]->sub1+P_menu[0]->curr;

	H_status = TRUE;
}


void hz_24c(int hz_No,UI pox,UI poy,UC color,UC *p3)
{

UC i,k;
UC dot_byte,count;
static UC bit_map[8]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

	set_dot_mode(color);
	count=3;

	for (i=0;i<72;i++)
	{
		dot_byte =p3[hz_No*72+i];
		for (k=0;k<8;k++)
		{
			if ( (dot_byte & bit_map[k]) )
			pixel(pox,poy);
			poy++;
		}
		if (--count==0)
		{
			pox++;
			poy-=24;
			count=3;
		}
	}
	reset_dot_mode();
}



void topic_disp1(UI x, UI y, UC *hz_str, UC hz_color , UC char_color)
{

FILE *fp1;
UC han1,han2,i;
UC *p1,*p2,*p3;
int i_c=0,m;
long int k;

	if((p1=malloc(200*sizeof(UC)))==NULL) return;
	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
//			i+=1;
			free(p1);
			return;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160)
			{
			     free(p1);
			     return;
			}
			p1[2*i_c]=han1;
			p1[2*i_c+1]=han2;
			i_c+=1;
			i+=1;
		}

	}

       if((fp1=fopen("gfsdata\\CCLIB2D.LIB","r+b"))==NULL)
	{
	    printf("Cann't open DBCS library file\n");
	    free(p1);
	    return;
	}

	if((p2=malloc(72*sizeof(UC)))==NULL)
	 {
	     free(p1);
	     fclose(fp1);
	     return;

	 }

	if((p3=malloc((i_c*72)*sizeof(UC)))==NULL)
	 {
	     free(p1);
	     free(p2);
	     fclose(fp1);
	     return;

	 }

  i=0;
  while(i<i_c)
  {
      m=(p1[2*i]-160-1)*94+(p1[2*i+1]-160-1);
      if( (p1[2*i]-160)>15)
       {
	      m=(p1[2*i]-160-1)*94+(p1[2*i+1]-160-1);
       }
//      m=(p1[2*i]-160-16)*94+(p1[2*i+1]-160-1);
      k=(long)m*(long)72;
      fseek(fp1,k*sizeof(UC),SEEK_SET);
      fread(p2,sizeof(UC),72,fp1);
      for( m=0; m<72; m++)
       {
		p3[i*72+m]=p2[m];
       }
     i+=1;
  }

   free(p1);
   free(p2);
   fclose(fp1);

	i_c=0;
	i=0;
	while ( (han1=hz_str[i]) != 0)
	{
		if (han1<160)
		{
			free(p3);
			return;
//			char_16(han1, x, y+1, char_color);
//			x += 8;
//			i++;
		}
		else
		{
			han2=hz_str[++i];
			if (han2<160)
			{
			     free(p3);
			     return;
			}
			hz_24c(i_c,x,y,hz_color,p3);
			x += 24;
			i_c+=1;
			i++;
		}
	}
     free(p3);
}
