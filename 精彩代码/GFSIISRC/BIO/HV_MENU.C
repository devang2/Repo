#include        <graphics.h>
#include        <string.h>
#include        <alloc.h>
#include        <stdlib.h>

#include        <DEF.INC>
#include        <KEY.INC>
#include        <FUNC.INC>
#include        <MENUFUNC.INC>

extern  UC Cursor_on;

#define BORDER			3
#define BACK_IN			16
#define KEY_DIST_X		16
#define KEY_DIST_Y		4
#define KEY_HIGH		26
#define TOPIC_HIGH		30
#define FIRST_LETTER_X	16
#define FIRST_LETTER_Y  16

static UC Menu_back_color;
static UC Menu_topic_color;
static UC Same_width;
static UC Esc_return;

static UC Flag_set_menu = FALSE;

#define	DRAW_CUR_KEY 	draw_key(key_sx[cur_choice], key_sy[cur_choice], \
						key_sx[cur_choice]+key_length[cur_choice%hnum], \
						key_sy[cur_choice]+KEY_HIGH, \
						selection[cur_choice], 7, 0)

#define CHANG_CUR_KEY	chang_key1(key_sx[cur_choice], key_sy[cur_choice], \
						key_sx[cur_choice]+key_length[cur_choice%hnum], \
						key_sy[cur_choice]+KEY_HIGH, \
						selection[cur_choice], 12)
#define	DRAW_CUR_KEY2 	draw_key(key_sx[cur_choice], key_sy[cur_choice], \
						key_sx[cur_choice]+key_length[cur_choice%hnum]-1, \
						key_sy[cur_choice]+KEY_HIGH-1, \
						selection[cur_choice], 10, 0)

#define CHANG_CUR_KEY2	chang_key3(key_sx[cur_choice], key_sy[cur_choice], \
						key_sx[cur_choice]+key_length[cur_choice%hnum]-1, \
						key_sy[cur_choice]+KEY_HIGH-1, \
						selection[cur_choice], 0)

#define RECOVER			setcolor(save_color);\
				setfillstyle(save_style.pattern,save_style.color);\
				Flag_set_menu = FALSE;\
				set_mouse_band(&mouse_band);\
				if ( old_cursor_stat )\
					cursor_on();\
				flash_continue();

/* ========================== set_menu() ============================ */
void set_menu(UC bkcolor, UC topic_color, UC same_width, UC esc_return)
{
	Menu_back_color = bkcolor;
	Menu_topic_color = topic_color;
	Same_width = same_width;
	Esc_return = esc_return;

	Flag_set_menu = TRUE;
}

UC hv_menu(UI x, UI y, UC hnum, UC total_num, UC *topic, UC *selection[])
{
UI lengthx, lengthy;        /* size of menu */
UC vnum;                    /* number of menu selection line */

UI key_sx[60], key_sy[60];  /* position of moving cursor, maxium 60 choices */
UI key_dist_x;	            /* step of moving cursor */
UI key_length[10];          /* length of cursor. */
							/* maxium 10 selections in 1 row */
UI back_in_add = 0;
UC cur_choice;				/* current choice, <total_num */
UI i,j;                     /* loop index */
UI key;                     /* key value input */

UI tmp_var;					/* keep some temp-value */

UI mx, my;
UC row, col;
UC last_mouse_stat = 0;	/* 0/1 = mouse unpressed/pressed */


MOUSE_BAND_STRUCT	mouse_band;
UC old_cursor_stat;
UI save_color;              /* save line color */
struct fillsettingstype save_style;                /* save fill style */
struct textsettingstype save_text;                /* save fill style */

printf("%lu ", farcoreleft());
	if (total_num<=1)
		exit_scr(1,"HV_MENU.C/hv_menu(): Argement 'total_num' must > 1.");
	if (hnum>total_num)
		hnum = total_num;

	/*  count vnum */
	if ( total_num%hnum == 0)
		vnum = total_num/hnum;
	else
		vnum = total_num/hnum+1;

	/*-------save state of graphics -------*/
	save_color = getcolor();
	getfillsettings(&save_style);
	flash_pause();
	old_cursor_stat = Cursor_on;
	get_mouse_band(&mouse_band);
	cursor_off();

	/*------- set_initial_paraments -------*/
	if (!Flag_set_menu)
	{
		Menu_back_color=2;
		Menu_topic_color=7;
		Same_width = FALSE;
		Esc_return = TRUE;
	}

	/*-------count length of cursor--------*/
	key_dist_x = 0;
	for (i=0; i<hnum; i++)
	{
		key_length[i] = 0;
		for (j=0; j<vnum; j++)
		{
			tmp_var = j*hnum+i;
			if ( tmp_var<total_num)
			{
				if ( key_length[i] < strlen(selection[tmp_var]) )
				{
					key_length[i] = strlen( selection[tmp_var] );
				}
			}
		}
		key_length[i] = (key_length[i]<<3)+10;
		key_dist_x += key_length[i];
	}

	if (Same_width)
	{
		for (i=1; i<hnum; i++)
			key_length[0] = max(key_length[0], key_length[i]);
		for (i=1; i<hnum; i++)
			key_length[i] = key_length[0];
		key_dist_x = hnum*key_length[0];
	}

	/*----------count length of menu ---------*/
	if (*topic)
	{
		lengthx = strlen(topic)<<3;
		if ( lengthx < key_dist_x + KEY_DIST_X*(hnum-1) + BACK_IN*2)
		{
			lengthx = key_dist_x + KEY_DIST_X*(hnum-1) + BACK_IN*2;
			key_dist_x = KEY_DIST_X;
		}
		else if ( hnum==1 )
		{
			back_in_add = (lengthx-key_dist_x-BACK_IN*2)>>1;
			back_in_add = (back_in_add>>4)<<4;
		}
		else
		{
			tmp_var = key_dist_x;
			key_dist_x = (lengthx-tmp_var-BACK_IN*2)/(hnum-1);
			if ( key_dist_x>KEY_DIST_X*2 )
			{
				back_in_add = (key_dist_x-2*KEY_DIST_X)/(hnum+1);
				back_in_add = (back_in_add>>4)<<4;

				key_dist_x = (lengthx-tmp_var-BACK_IN*2-back_in_add*2)/(hnum-1);
				tmp_var = ( (key_dist_x-KEY_DIST_X*2)*(hnum-1) )/hnum;
				key_dist_x = KEY_DIST_X*2;
				for (i=0; i<hnum; i++)
					key_length[i] += tmp_var;
			}
		}
	}
	else
	{
			lengthx = key_dist_x + BACK_IN*2 + KEY_DIST_X*(hnum-1);
			key_dist_x = KEY_DIST_X;
	}
	lengthx += FIRST_LETTER_X*2 + BORDER*2;

	lengthy = vnum*KEY_HIGH + (vnum-1)*KEY_DIST_Y + FIRST_LETTER_Y
			  + (FIRST_LETTER_Y-6) + BORDER*2;
	if (*topic)
	{
		lengthy += TOPIC_HIGH;
	}

	/*-----judge length of menu------*/
	if ( (x+lengthx>MAX_X) || (y+lengthy>MAX_Y) )
		exit_scr(1,"HV_MENU.C/hv_menu(): Window size is too large.");

	pop_back(x, y, x+lengthx-1, y+lengthy-1, Menu_back_color);
	if (*topic)
	{
		hz16_disp(x+BORDER+FIRST_LETTER_X,
				  y+BORDER+FIRST_LETTER_Y,
				  topic, Menu_topic_color);

	}

	key_sx[0] = x + BORDER + FIRST_LETTER_X + BACK_IN + back_in_add;
	key_sy[0] = y + BORDER + FIRST_LETTER_Y ;
	if (*topic)
		key_sy[0] += TOPIC_HIGH;

	for( i=1; i<total_num; i++)
	{
		if ( i%hnum == 0 )
			key_sx[i] = key_sx[0];
		else
			key_sx[i] = key_sx[i-1] + key_length[(i-1)%hnum] + key_dist_x;
		if (i<hnum )
			key_sy[i] = key_sy[0];
		else
			key_sy[i] = key_sy[i-hnum] + KEY_DIST_Y + KEY_HIGH;
	}

	for (i=1; i<total_num; i++)
		draw_key(key_sx[i], key_sy[i], key_sx[i]+key_length[i%hnum],
				 key_sy[i]+KEY_HIGH, selection[i], 7 /*12*/ , 0);

	chang_key1(key_sx[0], key_sy[0],
			  key_sx[0]+key_length[0], key_sy[0]+KEY_HIGH,
			  selection[0],
			  12);

	set_horizontal_band(key_sx[0]+1, key_sx[hnum-1]+key_length[hnum-1]-BORDER-2);
	set_vertical_band(key_sy[0]+1, key_sy[total_num-1]+KEY_HIGH-BORDER-1);
	cursor_on();
	cur_choice = 0;
	total_num --;			/* use the subscribe */

	gettextsettings(&save_text);
	settextstyle(3, HORIZ_DIR, 4);
	settextjustify(CENTER_TEXT, BOTTOM_TEXT);
	setcolor(6);

	outtextxy(x+23, y+38, "?");
	outtextxy(x+24, y+38, "?");
	setcolor(12);
	outtextxy(x+20, y+35, "?");
	outtextxy(x+21, y+35, "?");
	settextstyle(save_text.font, save_text.direction, save_text.charsize);
	settextjustify(save_text.horiz,save_text.vert);

	while (TRUE)
	{
		if ( !keybuf_nul() )
		{
			key = get_key0();
			switch (key)
			{
			case LEFT:
				cursor_off();
				DRAW_CUR_KEY;
				if (cur_choice==0)
					cur_choice = total_num;
				else
					cur_choice --;
				CHANG_CUR_KEY;
				cursor_on();
				break;

			case RIGHT:
				cursor_off();
				DRAW_CUR_KEY;
				if (cur_choice==total_num)
					cur_choice = 0;
				else
					cur_choice ++;
				CHANG_CUR_KEY;
				cursor_on();
				break;

			case UP:
				cursor_off();
				DRAW_CUR_KEY;
				if ( cur_choice>=hnum )
					cur_choice -= hnum;
				else
				{
					cur_choice += (vnum-1)*hnum;
					if ( cur_choice>total_num )
						cur_choice -= hnum;
				}
				CHANG_CUR_KEY;
				cursor_on();
				break;

			case DOWN:
				cursor_off();
				DRAW_CUR_KEY;
				cur_choice = cur_choice+hnum;
				if (cur_choice>total_num)
					cur_choice %= hnum;
				CHANG_CUR_KEY;
				cursor_on();
				break;

			case ENTER:
				cursor_off();
				rid_pop();
				RECOVER
				return(cur_choice+1);

			case ESC:
				if (Esc_return)
				{
					cursor_off();
					rid_pop();
					RECOVER
					return(0);
				}
				else
					sound_alarm();
				break;

			default:
				sound_alarm();
			}	/* end of switch(key) */
		}
		if ( left_button_pressed() )
			last_mouse_stat = 1;
		else if (last_mouse_stat==1)
		{
			last_mouse_stat = 0;
			get_mouse_position(&mx, &my);
			row = (my-key_sy[0])/(KEY_HIGH+KEY_DIST_Y);
			if (my>=key_sy[row*hnum]+KEY_HIGH-1)
				continue;

			col = 1;
			while ( col<hnum )
			{
				if (mx>key_sx[col])
					col++;
				else
					break;
			}
			col--;
			if (mx>key_sx[col]+key_length[col]-BORDER-2)
				continue;
			tmp_var =  row*hnum+col;
			if (tmp_var==cur_choice)
			{
				cursor_off();
				rid_pop();
				RECOVER
				return cur_choice+1;
			}
			else if (tmp_var<=total_num)
			{
				cursor_off();
				DRAW_CUR_KEY;
				cur_choice = tmp_var;
				CHANG_CUR_KEY;
				cursor_on();
			}
		}			/* end of else if (last_mouse_stat==1) */
	}   			/* end of while (TRUE) */
}




