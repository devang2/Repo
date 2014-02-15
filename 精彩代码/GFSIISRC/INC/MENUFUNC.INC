#define SC signed char
#define UC unsigned char
#define SI signed int
#define UI unsigned int
#define UL unsigned long int
#define SL signed long int

/* INIT_M.C*/
UC init_menu1(UC *struct_file, UC *text_file, UC *help_file, UC start_pos);
UC  init_menu(UC *struct_file, UC *text_file, UC start_pos);

/* MOUSE.C */
UC   mouse_reset(void);
void set_mickey_rate(UI x, UI y);
void set_vertical_band(UI y1, UI y2);
void set_horizontal_band(UI x1, UI x2);
void set_mouse_band(MOUSE_BAND_STRUCT *mouse_band);
void get_mouse_band(MOUSE_BAND_STRUCT *mouse_band);
UC   init_mouse(void);
UC   get_pressed_button(void);
UC   right_button_pressed(void);
UC   right_pressed(void);
UC   left_button_pressed(void);
UC   left_pressed(void);
void cursor_on(void);
void cursor_off(void);
void set_mouse_position(UI x,UI y);
void get_mouse_position(UI *x,UI *y);

/* MBASIC.C */
void draw_key(UI left,UI top,UI right,UI bottom,UC *string,UC bc,UC cc);
void chang_key(UI left,UI top,UI right,UI bottom,UC *string,UC cc);
void chang_key_border(UI left,UI top,UI right,UI bottom,UC bd);
void write_key_data(UI left,UI top,UI right,UI bottom,UC *string,UC cc);
void draw_back(UI left,UI top,UI right,UI bottom,UC bkc);
void pop_back(UI left,UI top,UI right,UI bottom,UC bk_color);
void rid_pop(void);

/* DRAW_M.C */
void draw_status_window(UC wno);
void draw_one_status(UC ano,UC bc,UC cc,UC *str);
void disp_ht_title(UI x,UI y,UC *tl,UC bkc,UC chc);
void draw_sub_menu(void);
void draw_main_menu(void);
void draw_head_menu(void);
void pop_head(void);
void draw_menu(UC wno);

/* ECHO_DT.C */
void echo_time(void);
void echo_date(UC *date);

/* DRAW_SCR.C */
void init_zen_menu(void);
void hide_zen_menu(void);
void init_gen_menu(void);
void hide_gen_menu(void);
void init_sub_menu(void);
void init_main_menu(void);
void init_head_menu(void);
void draw_screen(UC stat_win_number);
void draw_screen1(UC stat_win_number);
void echo_head_title(UC bk_color, UC color);
void disp_esc(void);

/* ECHO.C */
void echo_privil(UC privill);
void echo_commstat(UC stat);
void echo_workstat(UC stat);
void echo_alarm(UC alarm_level);
UC   escape(UI x, UI y);

/* SELECT.C */
UC   select_menu(UI input_code);
void help_mess(UC *mess);

/* RECOVER.C */
void clr_DialWin(UC code);
void recover_screen(UC mode);

/* help.c */
void help(UI help_index);

/* each.c */
void default_process(void);
void set_global_data(void);
void get_global_data(void);
void def_procedure(void);