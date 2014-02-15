#define ESCAPE 0X1B
#define UP 0X48
#define DOWN 0X50
#define RETURN 0X0D
#define PGUP 0X49
#define PGDN 0X51
#define FALSE 0
#define TRUE 1
#define INSERT 0X52
#define LEFT 0X4B
#define RIGHT 0X4D
#define HOME 0X47
#define END 0X4F
#define DELETE 0X53
#define BACKSPACE 0X08
#define BELL '\a'
#define MAIN_OPT 7
#define PD_SUB 6
extern float insist,max;
extern int count,con1,con2;
extern int cs_1;
extern int max_num;
extern int mouse_test_sign;
struct pd_str{
  char *main;
  void (*m_help)();
  char *sub[PD_SUB];
  void (*fun[PD_SUB])();
  //void (*help[PD_SUB])();
  };
void bell(int num,int type);//,int cut);
void Initialize(void);
void help_0();  void help_1();  void help_2();
void help_3();  void help_4();  void help_5();
void help_6();
//void demo();
//int check(void);
int any_key();
void push_bar(int left,int top,int right,int bottom);
void pull_bar(int left,int top,int right,int bottom);
void bios_test(int c);
void mouse_test(int c);
void mouse_sub_test(int c);
int flag_test(); void clear_win();
void drw_bar(int left,int top,int right,int bottom,int color);
void cls_bar(int left,int top,int right,int bottom,int color);
void submenu(int left,int top,int right,int bottom,int color);
void disp_hz_str(int x,int y,char *s,int color);
void disp_shadow_str(int x,int y,char *s,int color);
//void ret_Main();
int Main_board(char *str[7]);
void print_2(int x1,int y1,int x2,int y2,int color1,int color2);
void print_win();
void gun_pop();
void exit_con();
void shell();
void win_board(int left,int top,int right,int bottom,int leavel,int bkcolor,int color);
void saveimage(char *fn,int left,int top,int right,int bottom);
int loadimage(char *fn,int left,int top,int right,int bottom);
int file_warn(int type);
void cls_left_scrn();
void cls_right_scrn();
void cls_all_scrn();
int print_test();
int handler(int errval,int ax,int bp,int si);
void cssz();
void print_win();
void dos_time();
void jp_clock();
//int brush();
void cont_sm();
void add_line(int add,int scro);
void sys_read();
void spara_read();
void spara_write();
int judge_print(void);