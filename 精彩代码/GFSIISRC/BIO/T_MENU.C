#include 	<alloc.h>
#include        <graphics.h>
#include        <string.h>
#include        <stdlib.h>

#include        <DEF.INC>
#include        <KEY.INC>
#include        <FUNC.INC>
#include        <MENUFUNC.INC>

#define BORDER			3	/* border width */
#define BACK_IN			16	/* the distance from the first key to
							   the first character of topic */
#define KEY_DIST_X		16	/* the distance between two keys(default value). */
#define KEY_HIGH		26	/* the heigth of a key */
#define TOPIC_HIGH		30  /* the heigth of topic */
#define FIRST_LETTER_X	16  /* the distance from the first character of topic
							   to the left border */
#define FIRST_LETTER_Y  16	/* the distance from the topic to the top border */

#define WIN_X			240
#define WIN_Y			350
#define BK_COLOR		7
#define TOPIC_COLOR     12

static UC scr_buf[14500];

#define	DRAW_CUR_KEY 	draw_key(key_sx[cur_choice], key_sy, \
						key_sx[cur_choice]+key_length, \
						key_sy+KEY_HIGH, \
						selection[cur_choice], \
						7, 0)
#define CHANG_CUR_KEY	chang_key(key_sx[cur_choice], key_sy, \
						key_sx[cur_choice]+key_length, \
						key_sy+KEY_HIGH, \
						selection[cur_choice], \
						15)

static UI Win_x = WIN_X;
static UI Win_y = WIN_Y;
static UC Bk_color = BK_COLOR;
static UC Topic_color = TOPIC_COLOR;

/* --------------------------------------------------------------------
	FUNCTION:	set the error-window's position & color paramenters
				when DOS error occurred.
	CALLS:		None.
	CALLED BY:	This is an general-purposed routine.
	INPUT:		(x, y):	the left-top Coord. of the error-window.
				bk_color: the background color of the window.
				char_color: the topic color of the window.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void set_handler_mode(UI x, UI y, UC bk_color, UC topic_color)
{
	Win_x = x;
	Win_y = y;
	Bk_color = bk_color;
	Topic_color = topic_color;
}

/* --------------------------------------------------------------------
	FUNCTION:	reset the error-window's position & color paramenters.
	CALLS:		None.
	CALLED BY:	This is an general-purposed routine.
	INPUT:		None.
	OUTPUT:		None.
	RETURN:		None.
   -------------------------------------------------------------------- */
void reset_handler_mode(void)
{
	Win_x = WIN_X;
	Win_y = WIN_Y;
	Bk_color = BK_COLOR;
	Topic_color = TOPIC_COLOR;
}

UC t_menu(UC s_num, UC *topic, UC *selection[])
{
UI lengthx, lengthy;        /* size of menu */

UI key_sx[6], key_sy;
UI key_dist_x;	            /* distance between two keys */
UI key_length;				/* length of key. */
UC cur_choice;				/* current choice, <s_num */
UI back_in_add = 0;

UI i;                       /* loop index */
UI key;                     /* key value input */

UI tmp_var;					/* keep some temp-value */

UI save_color;              /* save line color */
struct fillsettingstype save_style;	/* save fill style */

	if (s_num<=1||s_num>6)	/* if the selections' number<=1, return 0 */
		return 0;
	if (*topic==0)
		return 0;
	/*-------save state of graphics -------*/
	save_color = getcolor();
	getfillsettings(&save_style);

	/* ------ calculate the key length -------*/
	key_length = 0;
	for (i=0; i<=s_num-1; i++)
		key_length = max(key_length, strlen(selection[i]) );

	key_length = (key_length<<3) + 2*5;
	tmp_var = key_length*s_num;

	/*----------count length of menu ---------*/
	lengthx = strlen(topic)<<3;
	if ( lengthx <= tmp_var + KEY_DIST_X*(s_num-1) + BACK_IN*2 )
	{
		lengthx = tmp_var + KEY_DIST_X*(s_num-1) + BACK_IN*2;
		key_dist_x = KEY_DIST_X;
	}
	else
	{
		key_dist_x = (lengthx-tmp_var-BACK_IN*2)/(s_num-1);
		if (key_dist_x>KEY_DIST_X*2)
		{
			back_in_add = ((key_dist_x-KEY_DIST_X*2)*(s_num-1))/2;
			key_dist_x = KEY_DIST_X*2;
		}
	}
	lengthx += FIRST_LETTER_X*2 + BORDER*2;
	lengthy = KEY_HIGH + FIRST_LETTER_Y*2 - 6 + BORDER*2 + TOPIC_HIGH;

	if ( (Win_x+lengthx>MAX_X) || (Win_y+lengthy>MAX_Y) )
		return 0;

/*
outf(100,100,1,12,"%d ", imagesize(Win_x, Win_y, Win_x+lengthx-1, Win_y+lengthy-1) );
*/
	getimage(Win_x, Win_y, Win_x+lengthx-1, Win_y+lengthy-1, scr_buf);

	draw_back(Win_x, Win_y, Win_x+lengthx-1, Win_y+lengthy-1, Bk_color);
	hz16_disp(Win_x+BORDER+FIRST_LETTER_X, Win_y+BORDER+FIRST_LETTER_Y,
				  topic, Topic_color);

	key_sx[0] = Win_x + BORDER + FIRST_LETTER_X + BACK_IN + back_in_add;
	key_sy = Win_y + BORDER + FIRST_LETTER_Y + TOPIC_HIGH;

	for (i=1; i<s_num; i++)
		key_sx[i] = key_sx[i-1] + key_length + key_dist_x;

	for (i=1; i<s_num; i++)
		draw_key(key_sx[i], key_sy, key_sx[i]+key_length,
				 key_sy+KEY_HIGH, selection[i], 7, 0);

	chang_key(key_sx[0], key_sy, key_sx[0]+key_length, key_sy+KEY_HIGH,
			  selection[0], 15);
	cur_choice = 0;
	s_num --;			/* use the subscribe */
	while (TRUE)
	{
		key = get_k1();
		switch (key)
		{
		case LEFT:
			DRAW_CUR_KEY;
			if (cur_choice==0)
				cur_choice = s_num;
			else
				cur_choice --;
			CHANG_CUR_KEY;
			break;

		case RIGHT:
			DRAW_CUR_KEY;
			if (cur_choice==s_num)
				cur_choice = 0;
			else
				cur_choice ++;
			CHANG_CUR_KEY;
			break;

		case ENTER:
				putimage(Win_x, Win_y, scr_buf, COPY_PUT);

				setcolor(save_color);
				setfillstyle(save_style.pattern,save_style.color);

				return(cur_choice+1);

		default:
			;
		}	/* end of switch(key) */
	}   	/* end of while (TRUE) */
}
