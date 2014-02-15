#ifndef BIOINC
#define BIOINC

#define SC signed char
#define UC unsigned char
#define SI signed int
#define UI unsigned int
#define UL unsigned long int
#define SL signed long int

#define TRUE  0x01
#define FALSE 0

#define MAX_X	   	639		/* maxium screen resolution dots define */
#define MAX_Y		479

#define SD_LEFT		96		/* small dialogue window coordinates define */
#define SD_TOP		69
#define SD_RIGHT	634
#define SD_BOTTOM	443

#define LD_LEFT		5 		/* large dialogue window coordinates define */
#define LD_TOP		69
#define LD_RIGHT	634
#define LD_BOTTOM	443

#define  MENU_HELP 	0
#define  ESC_HELP	0
#define  PRIVI_HELP	0
#define  MAX_NODE_NUM	200

typedef struct
    {
		UI start_x,start_y;	/* 表格左上角点的屏幕坐标*/
		UC head_h;         	/* 表头高度 */
		UC unit_h;         	/* 表项高度 */
		UC unit_num;       	/* 表项个数 */
		UC item_num;       	/* 表栏个数 */
		UC item_w[20];     	/* 每个表栏的宽度 */
		UC line_color;     	/* 表格线的颜色   */
	}TABLE_STRUCT;

typedef struct
    {
		UI dot_sx,dot_sy;
		UI dot_ex,dot_ey;
		UC unit_x,unit_y;
	}UNIT_STRUCT;

typedef struct
	{
		UI sx, sy;
		UI ex, ey;
	}MOUSE_BAND_STRUCT;

extern TABLE_STRUCT;
extern UNIT_STRUCT;

void init_scr(void);
void exit_scr(UI exit_code, const UC *format, ...);

UI   M_amount(void);
UL   M_available(void);
void *mem_alloc(UL nbytes);
void mem_free(void far *ptr);

UI	 EM_amount(void);
UC	 EM_read(void far *destin, UL source, UI bytes);
UC	 EM_write(UL destin, void far *source, UI bytes);

void load_char(void);
void load_hzk(UC hz_style);
void init24(void);
void exit24(void);

void set_dot_mode(UC dot_color);
void reset_dot_mode(void);
void pixel(UI x, UI y);

void hz_16(UC qh, UC wh, UI x, UI y, UC color);
void finger(UI x, UI y, UC color);
void h_finger(UI x, UI y, UC color);
void char_16(UC asc_code, UI x, UI y, UC color);
void hz16_disp(UI x, UI y, UC hz_str[80], UC color);
void hz16_fill(UI x, UI y, UC *hz_str, UC fill_len, UC color);
void disp_str(UI x, UI y, UC hz_str[80], UC bk_color, UC color);
void disp_str1(UI x, UI y, UC hz_str[80], UC bk_color, UC color);
void topic_disp(UI x, UI y, UC hz_str[80], UC hz_color, UC char_color);
void disp_20d(UI x, UI y, UC *hz_str, UC bk_color, UC char_color);

void hz_24(UC qh, UC wh, UI x, UI y, UC color, UC hz_style);
void hz24_disp(UI x, UI y, UC hz_str[80], UC dist, UC color, UC hz_style);
void disp_str24(UI x, UI y, UC hz_str[80], UC dist,
					   UC bk_color, UC color, UC hz_style);

void load_timer(void);
void ring_on(void);
void ring_off(void);
void set_bell(UI freqence, UC cycle);
void reset_bell(void);
void set_flash(UI x, UI y, UC bk_color, UC finger_color, UC cycle);
void reset_flash(void);
void flash_pause(void);
void flash_continue(void);
void unload_timer(void);
void check_event_flag(void);
void delay_tick(UI nticks);
void delay_10ms(UI n_10ms);

void clr_keybuf(void);
UC	 kb_nul(void);
UC   kb_haskey(UI *key);
UC	 keybuf_nul(void);
UI 	 get_key0(void);
UI   get_key1(void);

void clr_kb(void);
UC	 kb_n(void);
UI 	 get_k0(void);
UI	 get_k1(void);

int  handler(int Err_code, int keep_ax, int keep_bp, int keep_si);

void disp_cursor(UI where_x, UI where_y, UC bk_color, UC cursor_color) ;
UC   set_get_color(UC bk_color, UC char_color, UC cursor_color,
				   UC return_bk_color, UC return_char_color);
UC   get_dec(UI x, UI y, UI high,  UI width, UI back_in,
			 UC max_len, UL *value, UC mode);
UC   get_pass(UI x, UI y, UC bk_color, UC char_color, UC *pass_buf, UC pass_len);
UC   get_no(UI x, UI y, UI high, UI width, UI back_in,
			 UC max_len, UC get_buf[], UC mode);
UC   get_decstr(UI x, UI y, UI high, UI width, UI back_in,
			 UC max_len, UC get_buf[], UC mode);
UC   get_account(UI x, UI y, UI high, UI width, UI back_in,
			 UC max_len, UC get_buf[], UC mode);
UC   get_money(UI x, UI y, UI high, UI width, UI back_in,
			 UC max_len, UL *money, UC mode);
UC   get_hz_str(UI x, UI y, UI high, UI width, UI back_in,
			 UC max_len, UC *get_buf);
UC   get_yn(UI x, UI y, UC bk_color, UC char_color, UC cursor_color);
UC   get_YN(void) ;

void sound_bell(void);
void sound_alarm(void);

void cursor(UI x1, UI y1, UI x2, UI y2) ;
void set_menu(UC bk_color, UC topic_color, UC same_width, UC esc_return);
UC   hv_menu(UI x, UI y, UC horiz_num, UC total_num, UC *topic, UC *selections[]);

void outf(UI x, UI y, UC bk_color, UC char_color,
		  const UC *format, ... ) ;

void set_prn_mode(UI x, UI y, UC bk_color, UC char_color);
void reset_prn_mode(void);
UC   init_prn(void);
UC   prn_ch(UC ch);
UC   prns(UC *buffer);
UC   prn_fs(UC *buffer, UI length);
UC   prnf(const UC *format, ...);
UC   clr_prnbuf(void);
UC   prn_crlf(void);

void set_msg_color(UC bk_color, UC title_color, /*UC border_color,*/
				   UC char_color);
void msg_win(UI x, UI y, UC *title, UC *msg, UC delay_time) ;
UI   scasw(UI *word_str, UI word_unit, UI range);

void set_handler_mode(UI x, UI y, UC bk_color, UC topic_color);
void reset_handler_mode(void);
UC   t_menu(UC s_num, UC *topic, UC *selections[]);
void set_init_mode(UI x, UI y, UC bk_color, UC char_color);
UC	 init_disk(UC driver);
UC	 del_disk(UC Drive);
void draw_disk(UI xl, UI yl, UC coff, UC disk_color, UC scr_color);

void set_fcopy_mode(UI x, UI y, UC bk_color, UC char_color);
UC   fcopy(UC *source_file, UC *dest_file ) ;

void set_mirror(UC *file1, UC *file2, UC *tmpfile);
UC	 check_mirror(void);
UC   init_mirror(void);
UC	 mirror_write(UL offset, void *buf, UI nbyte);
UC	 mirror_fwrite(UL offset, void *buf, UI size, UI nitems);

UI   cbcd_bcd(UC *bcd, UC *c_bcd, UI max_num);
UI   bcd_cbcd(UC *c_bcd, UC *bcd, UI max_num);
UI   bcd_str(UC *str, UC *bcd, UI max_num);
UI   cbcd_str(UC *str, UC *c_bcd, UI max_num);
UI   str_bcd(UC *bcd, UC *str, UI max_num);
UI   str_cbcd(UC *c_bcd, UC *str, UI max_num);
UI   bcd_hex(UL *hex, UC *bcd, UI max_num);
UI   cbcd_hex(UL *hex, UC *c_bcd, UI max_num);
UI   kex_bcd(UC *bcd, UL hex, UI max_num);
UI   hex_cbcd(UC *c_bcd, UL hex, UI max_num);

void scr_up (void);
void scr_down (void);
void set_window (UI x,UI y,UC win_col,UC win_row,UC color);
void get_window (UI *x,UI *y,UC *win_col,UC *win_row,UC *color);
void cls_window (UC color);
UC	 locate (UI x,UI y);
void crlf(void);
void coutf(UC back_color, UC fore_color, const UC *format, ...);

void draw_topic(UI x,UI y,UC *topic);
void draw_table(TABLE_STRUCT *table_data);
void locate_finger(UC unit_x,UC unit_y);
void move_finger(UC direction,UC step);
void hide_finger(void);
void echo_finger(void);
void get_current(UNIT_STRUCT *cur_unit);
void get_certain(UNIT_STRUCT *cer_unit);
void set_finger_color(UC color);

UC   *get_date(void);
UC   *get_time(void);
UC   set_date(UC date[3]);
UC   set_time(UC time[3]);
UC   week_day(UC *date);
UC   last_day(UC *now_date,UC *last_date);
UC   next_day(UC *now_date,UC *next_date);
UC   date_right(UC *date);
UC   time_right(UC *time);
UC   leap_year(UI year);
UC   start_time(UC *, UC *, UL);

void sound_alarm(void);

#endif
