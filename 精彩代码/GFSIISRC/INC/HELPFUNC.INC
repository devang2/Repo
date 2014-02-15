/* HELP.C */
void help(UI index);

/* INIT_H_M.C */
UC   init_help_menu(UC *struct_file, UC *text_file, UI *number);

/* DRAW_H.C */
void init_win_para(void);
void draw_expl_keys(void);
void draw_search_keys(void);
void clr_help_win(UC mode);
void press_expl_key(UC key_no);
void depress_expl_key(UC key_no);
void press_search_key(UC key_no);
void depress_search_key(UC key_no);
void press_level_key(UC key_no);
void depress_level_key(UC key_no);
UC   read_action0(void);
UC   read_action1(UI *mouse_x, UI *mouse_y);
void help_err(UC err_no);

/* EXPLAIN.C */
void help_err(UC err_type);
UC   locate_help(UI index);
UC   disp_one_page(void);
UC   explain_index(UI index, UI node_no);

/* H_PICK.C */
void disp_pos(void);
void disp_pos1(void);
void level_key_justify(void);
UI   h_pick(UI *start_node, UC *curr_node, UI total_nodes);
void list_one_page(void);
UI   pick(void);
void scroll_up(void);
void scroll_down(void);
void bar_up(void);
void bar_down(void);
void pg_up(void);
void pg_down(void);
void home(void);
void end(void);
void shift_up(void);
void shift_down(void);
void left(UC new_level);

/* KB_NUL.C */
void enable_help(void);
void disable_help(void);
void set_help(UI index);
