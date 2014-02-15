/******************************************************************/
/*   FILENAME:  SELECT.C                                          */
/*   FUNCTION :	select_menu(UI INPUT_CODE)                        */
/*   AUTHER :  ZHANG_RUI_JUN                                      */
/*   OTHERS :  modified by Bob kong, 1993/3/26                    */
/******************************************************************/

#include <mem.h>
#include <stdio.h>
#include <graphics.h>

#include <def.inc>
#include <key.inc>
#include <func.inc>
#include <menufunc.inc>

#include <menu_def.inc>

#define   HEAD_BK_CLR   9


UI  Menu_path;			    /* draw_scr.c */

/*extern UC Privi; */		    /* main.c 	  */

extern UI  G_xad;                   /* draw_scr.c */
extern UI  G_bx;                    /* draw_scr.c */
extern UI  Z_bx,Z_xad,Z_by;         /* draw_scr.c */
extern UC  H_status, Pop_down1, Pop_down2, Want_pop1;  	/* draw_scr.c */
extern HMENU *P_menu[MAX_LEVEL];    /* draw_scr.c */

extern HMENU  *Menu_stru;           /* init_m.c */
extern UC *Menu[MAX_NODE];          /* init_m.c */
extern UC *Help[MAX_NODE];          /* init_m.c */
extern UC  Tmp_main_curr;           /* draw_scr.c */
extern UC  Quit;       		    /* main.c     */

static UC  Return_code;		    /* if leaf node & Enter or Left_button */
				    /* pressed, Return_code = TRUE */
static UC  Left_button;		    /* if Left_button pressed, Left_button = TRUE, */
				    /* When this key is processed, reset it */
static UC  Tmp_head_curr;	    /* for keeping the old head-menu current choice */
static UC  hot_key = 0;		    /* when the mouse pressed at HELP or EXIT,*/
				    /* set hot_key==TRUE */
extern char patterns[][8];

void mouse_check(void);
void set_menu_path(void);
void down_handle(void);
void up_handle(void);
void left_handle(void);
void right_handle(void);
void esc_handle(void);
void enter_handle(void);
void hmenu_handle(UC ano);
void mmenu_handle(UC ano);
void smenu_handle(UC ano);
void gmenu_handle(UC ano);
void zmenu_handle(UC ano);

/* ======================== hmenu_handle() ========================== */
static void hmenu_handle(UC ano)
/* handle when head menu changed */
{
UI i,j;

	if(P_menu[0]->curr != ano)
	{
		i = P_menu[0]->curr/H_X_M;
		j = P_menu[0]->curr%H_X_M;
		draw_key(H_BX+j*H_XAD,H_BY+i*H_YAD,
				H_BX+(j+1)*H_XAD,H_BY+(i+1)*H_YAD,
					Menu[P_menu[0]->sub1+P_menu[0]->curr],7,0);
		i = ano/H_X_M;
		j = ano%H_X_M;
		P_menu[0]->curr = ano;
		chang_key(H_BX+j*H_XAD,H_BY+i*H_YAD,
				  H_BX+(j+1)*H_XAD,H_BY+(i+1)*H_YAD,
				  Menu[P_menu[0]->sub1+P_menu[0]->curr],15);
		help_mess(Help[P_menu[0]->sub1+P_menu[0]->curr]);
	}
}

/* ========================== mmenu_handle() =========================== */
static void mmenu_handle(UC ano)
/* the 2th(horizon) menu handle when the 1th menu changed */
{

	if(P_menu[1]->curr != ano)
	{
		if (Pop_down2)
		{
			hide_zen_menu();
			Pop_down2 = FALSE;
		}
		if (Pop_down1)
		{
			hide_gen_menu();
			Pop_down1 = FALSE;
		}
		Tmp_main_curr = P_menu[1]->curr;		/* keep the old 1th menu choice */
		draw_key(M_BX, M_BY+Tmp_main_curr*M_YAD+1, M_EX,
				 M_BY+(Tmp_main_curr+1)*M_YAD,
				 Menu[P_menu[1]->sub1+Tmp_main_curr],7,0);
		P_menu[1]->curr = ano;
		chang_key(M_BX,M_BY+ano*M_YAD+1,M_EX,M_BY+(ano+1)*M_YAD,
					Menu[P_menu[1]->sub1+ano],15);
		help_mess(Help[P_menu[1]->sub1+ano]);

		P_menu[2] = Menu_stru + P_menu[1]->sub1+P_menu[1]->curr;
		if (P_menu[2]->sub_number==0)
		{
			P_menu[3] = NULL;
			P_menu[4] = NULL;
		}
		else
		{
			P_menu[3] = Menu_stru + P_menu[2]->sub1 + P_menu[2]->curr;
			if ( P_menu[3]->sub_number==0 )
				P_menu[4] = NULL;
			else
				P_menu[4] = Menu_stru + P_menu[3]->sub1 + P_menu[3]->curr;
		}
		init_sub_menu();
		Want_pop1 = FALSE;
	}
	if (Left_button)
		if (P_menu[2]->sub_number == 0)
			Return_code = TRUE;
}

/* ========================== smenu_handle() =========================== */
static void smenu_handle(UC ano)
/* handle when the 2th(horizon) menu changed */
{
UI k;

	if(P_menu[2]->curr != ano)
	{
		if (Pop_down2)
		{
			hide_zen_menu();
			Pop_down2 = FALSE;
		}
		if (Pop_down1)
		{
			hide_gen_menu();
			Pop_down1 = FALSE;
		}
		k = P_menu[2]->curr;
		draw_key(S_BX+k*S_XAD+1,S_BY,S_BX+(k+1)*S_XAD,S_EY,
					Menu[P_menu[2]->sub1+k],7,0);
		k = ano;
		P_menu[2]->curr = ano;
		chang_key(S_BX+k*S_XAD+1,S_BY,S_BX+(k+1)*S_XAD,S_EY,
					Menu[P_menu[2]->sub1+k],15);
		help_mess(Help[P_menu[2]->sub1+k]);

		P_menu[3] = Menu_stru + P_menu[2]->sub1 + P_menu[2]->curr;
		if ( P_menu[3]->sub_number==0 )
			P_menu[4] = NULL;
		else
			P_menu[4] = Menu_stru + P_menu[3]->sub1 + P_menu[3]->curr;
	}

	if ( P_menu[3]->sub_number == 0 )
	{
		if (Left_button)
		{
			Want_pop1 = TRUE;
			Return_code = TRUE;
		}
	}
	else
	{
		if (Left_button)
			Want_pop1 = TRUE;
		if (Want_pop1)
		{
			init_gen_menu();
			Pop_down1 = TRUE;
		}
	}
}

/* ============================ gmenu_handle() =========================== */
static void gmenu_handle(UC ano)
/* handle when the 2th (horizon) menu changed. */
{
UI k;

	if(P_menu[3]->curr != ano)
	{
		if (Pop_down2)
		{
			hide_zen_menu();
			Pop_down2 = FALSE;
		}
		k = P_menu[3]->curr;

		draw_key(G_bx, G_BY+k*G_YAD+1, G_bx+G_xad, G_BY+(k+1)*G_YAD,
					Menu[P_menu[3]->sub1+k],7,0);
		k = ano;
		P_menu[3]->curr = ano;
		chang_key(G_bx, G_BY+k*G_YAD+1, G_bx+G_xad, G_BY+(k+1)*G_YAD,
							Menu[P_menu[3]->sub1+k],15);
                help_mess(Help[P_menu[3]->sub1+k]);
		P_menu[4] = Menu_stru + P_menu[3]->sub1 + P_menu[3]->curr;
	}
	if(Left_button)
	{
		if (P_menu[4]->sub_number==0)
		{
			Return_code = TRUE;
			Pop_down2 = FALSE;
		}
		else
		{
			Pop_down2 = TRUE;
			init_zen_menu();
		}
	}
}

/* ============================ zmenu_handle() =========================== */
static void zmenu_handle(UC ano)
/* handle when the 3th menu (1th polldown menu) changed */
{
UI k;

	if(P_menu[4]->curr != ano)
	{
		k = P_menu[4]->curr;
		draw_key(Z_bx,Z_by+k*Z_YAD+1,Z_bx+Z_xad,Z_by+(k+1)*Z_YAD,
							Menu[P_menu[4]->sub1+k],7,0);
		k = ano;
		P_menu[4]->curr = ano;
		chang_key(Z_bx,Z_by+k*Z_YAD+1,Z_bx+Z_xad,Z_by+(k+1)*Z_YAD,
							Menu[P_menu[4]->sub1+k],15);
                help_mess(Help[P_menu[4]->sub1+k]);
	}
	if (Left_button)
		Return_code = TRUE;
}

/* =========================== esc_handle() ============================ */
static void esc_handle()
{
	if (H_status)
	{
		P_menu[0]->curr = Tmp_head_curr;
		cursor_off();

		clr_DialWin(0);
		Quit = escape(194, 190);
		if (!Quit)
		{
			recover_screen(0);
			cursor_on();
		}
	}
	else
	if(Pop_down2)
	{
		hide_zen_menu();
		Pop_down2 = FALSE;
	}
	else
	if(Pop_down1)
	{
		hide_gen_menu();
		Pop_down1 = FALSE;
		Want_pop1 = FALSE;
	}
	else
	{
		Tmp_head_curr = P_menu[0]->curr;
		draw_main_menu();      /* reprint the vertical menu frame */
		draw_sub_menu();       /* reprint horizontal without title*/
		init_head_menu();
		H_status = TRUE;
	}
}

/* ============================ enter_handle() =========================== */
static void enter_handle()
{
	UC flag;

	if(H_status)
	{
		echo_head_title(11, 0);    /* display head title on the right top */

		P_menu[1] = Menu_stru + P_menu[0]->sub1 + P_menu[0]->curr;

		if(P_menu[1]->sub_number == 0)		/* The 1th menu is leaf node */
		{
			Return_code = TRUE;
		}
		else
		{
			clr_DialWin(0);
			P_menu[2] = Menu_stru + P_menu[1]->sub1 + P_menu[1]->curr;
			if (P_menu[2]->sub_number!=0)
			{
				P_menu[3] = Menu_stru + P_menu[2]->sub1+P_menu[2]->curr;
				if ( P_menu[3]->sub_number!=0 )
					P_menu[4] = Menu_stru + P_menu[3]->sub1+P_menu[3]->curr;
			}


			if(P_menu[0]->curr==0) flag=input_pass();
			if(flag==FALSE)
			{
				recover_screen(0);
				cursor_on();
			}
			else
			{
				init_main_menu();
				draw_sub_menu();
				init_sub_menu();
				Pop_down1 = FALSE;
				Pop_down2 = FALSE;
				Want_pop1 = FALSE;
				H_status = FALSE;
			}
		}		/* end of if (sub_number!=0) */
	}			/* end of if (H_status) */
	else if (Pop_down2)
	{
		Return_code = TRUE;
	}
	else if (Pop_down1)
	{
		if ( P_menu[4]!=NULL && P_menu[4]->sub_number!=0 )
		{
			Pop_down2 =TRUE;
			init_zen_menu();
		}
		else
			Return_code = TRUE;
		return;
	}
	else
	if (P_menu[1]->sub_number!=0)
	{
		if ( P_menu[2]->sub_number!=0 )  		/*  The 1th menu not a leaf */
		{
			if (P_menu[3]->sub_number!=0)
			{
				Pop_down1 = TRUE;
				Want_pop1 = TRUE;
				init_gen_menu();
			}
			else
				Return_code = TRUE;
		}
		else
		Return_code = TRUE;
	}
}

/* ========================== right_handle() ============================= */
static void right_handle()
{
	if(H_status)
	{
		if((P_menu[0]->curr+1) < P_menu[0]->sub_number)
			hmenu_handle(P_menu[0]->curr+1);
		else
			hmenu_handle(0);
	}
	else
	{
		if (P_menu[2]->sub_number != 0)
		{
			if((P_menu[2]->curr+1) < P_menu[2]->sub_number)
				smenu_handle(P_menu[2]->curr+1);
			else
				smenu_handle(0);
		}
	}
}

/* =========================== left_handle() ============================ */
static void left_handle()
{
	if(H_status)
	{
		if(P_menu[0]->curr == 0)
			hmenu_handle(P_menu[0]->sub_number-1);
		else
			hmenu_handle(P_menu[0]->curr-1);
	}
	else
	{
		if (P_menu[2]->sub_number != 0)
		{
			if(P_menu[2]->curr == 0)
				smenu_handle(P_menu[2]->sub_number-1);
			else
				smenu_handle(P_menu[2]->curr-1);
		}
	}
}

/* =========================== up_handle() ============================= */
static void up_handle()
{
	if(H_status)
	{
		if(P_menu[0]->curr >= H_X_M)
			hmenu_handle(P_menu[0]->curr-H_X_M);
		else
			hmenu_handle(P_menu[0]->sub_number+P_menu[0]->curr-H_X_M);
	}
	else
	if(Pop_down1)
	{
		if(Pop_down2)
		{
			if (P_menu[4]->sub_number!=0)
			{	if(P_menu[4]->curr == 0)
					zmenu_handle(P_menu[4]->sub_number-1);
				else
					zmenu_handle(P_menu[4]->curr-1);
			}
		}
		else
		{
			if(P_menu[3]->sub_number!=0)
			{
				if(P_menu[3]->curr == 0)
					gmenu_handle(P_menu[3]->sub_number-1);
				else
					gmenu_handle(P_menu[3]->curr-1);
			}
		}
	}
	else
	{
	if(P_menu[1]->curr == 0)
			mmenu_handle(P_menu[1]->sub_number-1);
		else
			mmenu_handle(P_menu[1]->curr-1);
	}
}

/* =========================== down_handle() ============================ */
static void down_handle()
{
	if(H_status)
	{
		if((P_menu[0]->curr+H_X_M) < P_menu[0]->sub_number)
			hmenu_handle(P_menu[0]->curr+H_X_M);
		else
		if(P_menu[0]->curr >= P_menu[0]->sub_number-H_X_M)
			hmenu_handle(H_X_M-P_menu[0]->sub_number+P_menu[0]->curr);
	}
	else
	if(Pop_down1)
	{
		if(Pop_down2)
		{
			if(P_menu[4]->sub_number!=0)
			{
				if(P_menu[4]->curr == P_menu[4]->sub_number-1)
					zmenu_handle(0);
				else
					zmenu_handle(P_menu[4]->curr+1);
			}
		}
		else
		{
			if(P_menu[3]->sub_number!=0)
			{
				if(P_menu[3]->curr == P_menu[3]->sub_number-1)
					gmenu_handle(0);
				else
					gmenu_handle(P_menu[3]->curr+1);
			}
		}
	}
	else
	{
		if(P_menu[1]->curr == P_menu[1]->sub_number-1)
			mmenu_handle(0);
		else
			mmenu_handle(P_menu[1]->curr+1);
	}
}

/* ============================ select_menu() =========================== */
UC select_menu(UI input_code)
{
	Return_code = FALSE;

	switch(input_code)
	{
	case 0     :
		cursor_off();
		mouse_check();
		cursor_on();
		break;

	case ESC   :
		cursor_off();
		esc_handle();
		cursor_on();
		break;

	case LEFT  :
		cursor_off();
		left_handle();
		cursor_on();
		break;

	case RIGHT :
		cursor_off();
		right_handle();
		cursor_on();
		break;

	case UP    :
		cursor_off();
		up_handle();
		cursor_on();
		break;

	case DOWN  :
		cursor_off();
		down_handle();
		cursor_on();
		break;

	case ENTER :
		cursor_off();
		enter_handle();
		cursor_on();
		break;

	default   :
		sound_bell();
		break;
	}

	if(Return_code)
		set_menu_path();

	return(Return_code);
}

/* ============================ set_menu_path() ========================== */
static void set_menu_path()
{
UI i;

	switch(hot_key)
	{
	case 0:
		Menu_path = P_menu[0]->curr+1;
		for (i=1; i<5; i++)
		{
			if (P_menu[i]->sub_number==0)
				break;
			Menu_path = Menu_path*10 + P_menu[i]->curr;
		}
		break;
	default:
		hot_key = 0;
		Return_code = FALSE;
		return;
	}
}

/* =========================== mouse_check() ============================= */
static void mouse_check()
{
UC ano;
UI mx,my;

	Left_button = TRUE;
	get_mouse_position(&mx, &my);

	if ( my>D_BOTTOM+4 && my<MAX_Y-4)
	{
		if (mx>4&&mx<M_EX-4 )
		{
			hot_key = 1;				/* HELP is selected */
			Return_code = TRUE;
		}
		else if(mx>MAX_X-S_XAD+4&&mx<MAX_X-4)
		{
			hot_key = 2;				/* Escape is selected */
			Return_code = TRUE;
		}
		Left_button = FALSE;
		return;
	}

	if ( my>4 && my<S_BY-4 )
	{
		if((mx>T_EX+4) && (mx<MAX_X-4))
		{
			if (H_status)
			{
				if (P_menu[1]->sub_number==0)
					sound_alarm();
				else
				{
					P_menu[0]->curr = Tmp_head_curr;
					H_status = FALSE;
					recover_screen(0);
				}
			}
			else
			{
				Tmp_head_curr = P_menu[0]->curr;
				init_head_menu();
				H_status = TRUE;
			}
		}
		Left_button = FALSE;
		return;
	}

	if (H_status)
	{
		if (my>H_BY && my<H_BY+H_Y_M*H_YAD)
		{
			if((mx>H_BX)&&(mx<(H_BX+H_X_M*H_XAD)))
			{
				ano = (mx-H_BX)/H_XAD+H_X_M*((my-H_BY)/H_YAD);
				if (ano<P_menu[0]->sub_number)
				{
					hmenu_handle( ano );
					enter_handle();
				}
			}
		}
		Left_button = FALSE;
		return;
	}

	if (mx>M_BX && mx<M_EX )
	{
		if ( my>M_BY && my<M_BY+M_MENU*M_YAD )
		{
			ano = (my-M_BY)/M_YAD;
			if (ano<P_menu[1]->sub_number)
				mmenu_handle(ano);
		}
		Left_button = FALSE;
		return;
	}

	if ( my>S_BY && my<S_EY )
	{
		if ( mx>S_BX && mx<S_BX+S_MENU*S_XAD )
		{
			ano = (mx-S_BX)/S_XAD;
			if ( ano<P_menu[2]->sub_number)
				smenu_handle(ano);
		}
		Left_button = FALSE;
		return;
	}

	if ( Pop_down1 )
	{
		if ( my>G_BY && my<G_BY+P_menu[3]->sub_number*G_YAD )
		{
			if ( mx>G_bx && mx<G_bx+G_xad )
			{
				gmenu_handle((my-G_BY)/G_YAD);

				Left_button = FALSE;
				return;
			}
		}

		if (Pop_down2)
		{
			if ( my>Z_by && my<(Z_by+P_menu[4]->sub_number*Z_YAD) )
			{
				if ( mx>Z_bx )
					if ( mx<Z_bx+Z_xad )
					{
						zmenu_handle((my-Z_by)/Z_YAD);
						Left_button = FALSE;
						return;
					}
			}
		}
	}
	Left_button = FALSE;
}

void help_mess(UC *mess)
{
    setfillstyle(SOLID_FILL, 11);
    bar(92+4, 455+1, 92+8+445, 455+1+20);
    hz16_disp(92+4, 455+1, mess, 0);
    return;
}