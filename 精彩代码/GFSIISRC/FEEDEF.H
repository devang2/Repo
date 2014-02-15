#ifndef  MAIN
#define  MAIN

#ifdef   MAIN_C
#define  EXTERN
#else
#define  EXTERN		extern
#endif

#include <dir.h>
#include <dos.h>
#include <stdio.h>

#define SC signed char
#define UC unsigned char

#define SI signed int
#define UI unsigned int

#define SL signed long int
#define UL unsigned long int

#define IRQNO	5
#define PORT_NUM 10
#define PhonesPerPort 64

#define MAX_LOCAL     600
#define MAX_INTERN    250
#define MAX_NATION    2250
#define MAX_LAN       60

#define MAX_USERS     (PhonesPerPort*PORT_NUM)
#define MAX_AUTHCD    1000
#define MAX_TRKNO     256
#define MAX_SPECIAL   50

#define SYS_PWD       1
#define DEL_PWD       2
#define CLR_PWD       3
#define CASH_PWD      4

#define LQ1600        1
#define LX800         2
#define INLIB         1
#define OUTLIB        2

#define COLOR         1
#define MONO          2

#define ACCOUNT       1
#define NOACCOUNT     2

#define W_WIDTH       1
#define N_WIDTH       2

#define HOTEL_USR     1
#define NORMAL_USR    2

#define COM1          1
#define COM2          2

#define MES_LEFT      92
#define MES_TOP       455
#define MES_RIGHT     548
#define MES_BOTTOM    479

#define MAIN_MES      0
#define PASS_ERR      1
#define RE_PASS       2
#define RE_PASS_ERR   3
#define NEW_PASS_EN   4
#define TIME_OUT      5
#define IO_ERR        6
#define NO_PAPER      7
#define PRN_ERR       8
#define PHO_FILE_ERR  9
#define SPRING_DAY    10
#define CONN_ERR      11
#define REC_TIMEOUT   12
#define RATE_FILE_ERR 13
#define NO_NOT_EXIST  14
#define CASH_FILE_ERR 15
#define USER_FILE_ERR 16
#define UNO_NOT_EXIST 17
#define TFEE_FILE_ERR 18
#define DTIME_FILE_ERR   19
#define CANCEL_INPUT     20
#define INVALID_INPUT    21
#define AUTHCD_FILE_ERR  22
#define AUTHCD_NOT_EXIST 23
#define ACCTIME_FILE_ERR 24
#define ACCFEE_FILE_ERR  25
#define BANACC_FILE_ERR  26
#define DAYACC_FILE_ERR  27
#define MONACC_FILE_ERR  28
#define YEARACC_FILE_ERR  29
#define HACCFEE_FILE_ERR  30
#define HBANACC_FILE_ERR  31
#define HDAYACC_FILE_ERR  32
#define HMONACC_FILE_ERR  33
#define HYEARACC_FILE_ERR 34
#define NULL_ACCOUNT_ERR  35
#define PCODE_FILE_ERR    36
#define PCODE_ERR         37
#define NULL_CASH_ERR     38

#define SIO_BUF_SIZE  512
#define CMD_BUF_SIZE  128

#define PORT          0

#define LINE          430
#define VERT          420
#define TOP           427
#define LEFT_TOP      421
#define LEFT_COR      425
#define RIGHT_TOP     422
#define RIGHT_COR     426
#define MID           429
#define RIGHT_BOT     424
#define LEFT_BOT      423
#define BOTTOM        428

#define OFF       0
#define ON        1

#define NO        0
#define YES       1

#define NOFRESH  0
#define REFRESH  1

#define ITBL_CLR 5
#define QTBL_CLR GREEN

#define LDD       1
#define DDD       2
#define IDD       3
#define HDD       4

#define IMMED     1
#define BATCH     2

#define INV       1
#define DETL      2

#define WHOLE     1
#define PHONE_NO  2
#define AUTH_CD   3
#define DEPT_NO   4

#define NORMAL    1
#define NONFEE    2
#define OUTRNG    3

#define SINGLE    1
#define MULTY     2

#define SET_PCODE 1
#define SET_IMPRN 2

#define SET_CASH   1
#define INQ_CASH   2
#define INQ_OVCASH 3

#define CASH_OFF   0
#define CASH_ON    1

#define SET_AUTH  0
#define INQ_AUTH  1
#define UNUSED_AUTH	255

#define BACKUP    1
#define INQUIRE   2
#define DELETE    3
#define PRINT     4

#define HIST_F     0
#define CRNT_F     1

#define TOTAL_FEE 1
#define PHONE_FEE 2
#define DEPT_FEE  3
#define AUTH_FEE  4

#define FEE_ACC   1
#define BAN_ACC   2
#define DAY_ACC   3
#define MON_ACC   4
#define YEAR_ACC  5
#define MON_NOACC 6
#define YEAR_NOAC 7
#define FEE_STA   8
#define HFEE_ACC  9
#define HBAN_ACC  10
#define HDAY_ACC  11
#define HMON_ACC  12
#define HYEAR_ACC 13

#define HAX1001   1
#define HAX1002   2
#define HJD256    3
#define MD110     4
#define BH01      5
#define MECURY    6
#define NTM       7
#define JSQ31     8
#define METEL     9
#define HICOM     10

#define PHONE_CASH 0
#define AUTH_CASH  1

typedef struct
{
  fng_clr;
} COLOR_STRUCT;

typedef struct
{
//    UC pabx_m;    /* pabx model */
    UC prt_m;     /* 9 pin or 24 pin
		       1 for 24 pin
		       2 for 9  pin
		  */
    UC prt_w;     /* paper width
		       1 for 80 col
		       2 for 132 col
		  */
    UC prtflg;     /* DBCS librar flag
		       1 for inner DBCS lib
		  */
    UC com_m[PORT_NUM];     /* SIO port option:
		       1 for COM1 to PABX
		       2 for COM2 to PABX
		  */
//    UC com_cmd1;  /* COM1 data bits */
//    UC com_cmd2;  /* COM1 stop bits */
//    UC com_cmd3;  /* COM1 parity    */
//    UC com_cmd4;  /* COM1 transmite rate */
} SYS_MODE;

typedef struct
{
	UC area_no[9];      /* area No. */
	UC area_name[9];
	UC length;          /* No. len  */
	UC trunk_mark;
	UI first_rate;
	UI first_time;
	UI formal_rate;
	UI formal_time;
	UC add_fee;	    /* additional fee */
} RATE_STRUCT;

typedef struct
{
	UI add_percent;
	UI add_record;
	UI add_rate;
	UI add_time;
} ADD_STRUCT;

typedef struct
{
	UI delay_idd;
	UI delay_ddd;
	UI delay_ldd;
} DELAY_STRUCT;

typedef struct
{
       UC phone_no[9];      /* telephone number */
       UI user_no;          /* accountant No.   */
       UI code;             /* inner code                   */
       UC class;             /* ext telephone class          */
       UC feeflag[5];        /* 0 -- urban call fee flag     */
			     /* 1 -- local call fee flag     */
			     /* 2 -- information call fee flag    */
			     /* 3 -- domestic call fee flag       */
			     /* 4 -- international call fee flag  */
	UC    set_flag;          /* flag of download or not      */
	UC flag;	/* immediate print or not */
	UC max_min;     /* maximum minute permission for one call */

	UL intern_time;
	UL intern_charge;
	UI intern_count;
	UL nation_time;
	UL nation_charge;
	UI nation_count;
	UL local_time;       /* all the time of local call */
	UL local_charge;     /* all charge of local call   */
	UI local_count;      /* all times  of local call   */
	UL month_lease;
	UL addfee[5];        /* 0:new service,
				1:mannul long call,
				2:reparing,
				3:other fee,
				4:all the add fee for records
			      */
} PHONE_STRUCT;

typedef struct
{
    UL auth_code;            /* authority code            */
    UC auth_class;           /* authored class            */
    UC auth_usr_nam[11];     /* authored user name        */
    UC auth_set_flag;
    UI user_no;  	     /* accountant No.   */

    UL intern_time;
    UL intern_charge;
    UI intern_count;
    UL nation_time;
    UL nation_charge;
    UI nation_count;
    UL local_time;       /* all the time of local call */
    UL local_charge;     /* all charge of local call   */
    UI local_count;      /* all times  of local call   */

    UI month_lease;
    UL addfee;
} AUTHCD_STRUCT;

typedef struct
{
	UL intern_time;
	UL intern_charge;
	UL intern_count;
	UL nation_time;
	UL nation_charge;
	UL nation_count;
	UL local_time;       /* all the time of local call */
	UL local_charge;     /* all charge of local call   */
	UL local_count;      /* all times  of local call   */
	UL month_lease;
	UL addfee[5];        /* 0:new service,
				1:manuly long call,
				2:reparing,
				3:other fee,
				4:all the add fee for records
			      */
} FE_STAT_STRUCT;

typedef struct
{
    UC year;
    UC month;
    UC day;
    UC hour;
    UC min;
    UC sec;
}  SERAL_NO_STRUCT;

typedef struct
{
	UL intern_time;
	UL intern_charge;
	UL intern_count;
	UL nation_time;
	UL nation_charge;
	UL nation_count;
	UL local_time;       /* all the time of local call */
	UL local_charge;     /* all charge of local call   */
	UL local_count;      /* all times  of local call   */
	UL month_lease;      /* month lease                */
	UL addfee[5];        /* 0:new service,
				1:manly long call,
				2:reparing,
				3:other fee,
				4:all the add fee for records
			      */
	SERAL_NO_STRUCT seral_no;
	UL phone_no;
	UC flag;             /* 1:fee account,
				2:ban account,
				3:day account,
				4:month account,
				5:year account,
				6:month nonaccount,
				7:year nonaccount,
				8:fee statitcs
			      */
} ACC_STRUCT;

typedef struct
{
    UC    phone_no[9];       /* telephone No or authrity no  */
    SL    cash;              /* cash preserved now           */
    UI    limit;             /* low limit for cash pledge    */
    UC    flag;
} CASH_PLEDGE;               /* ya jin */

typedef struct
{
    UC    PM_hour;
    UC    AM_hour;
    UC    sunday;
    UC    saturday;
} HALF_FEE;

typedef struct
{
    UC    mon;
    UC    day;
    UC    flag;
} HOLIDAY;

typedef struct
{
    UL caller1;              /* main caller 1                    */
    UL caller2;              /* main caller 2                    */
    UL auth_code;            /* authority code                   */
    UC callee[16];           /* telephone number of being called */
    UC trunk_no;             /* trunk number two BCD code        */
    UI year1;                /* year of start time               */
    UC mon1, day1;           /* month and day of start time      */
    UC hour1, min1, sec1;    /* hour:minute:second of start time */
    UI year2;                /* year of end time                 */
    UC mon2, day2;           /* month and day of end time        */
    UC hour2, min2, sec2;    /* hour:minute:second of end time   */
    UL time;                 /* calling time length in seconds   */
    UL charge;               /* calling fee in cents             */
    UL add;                  /* add fee for this record in cents */
    UC flag;                 /* stamped flag                     */
} RECORD_STRUCT;

typedef struct
{
    UI year;
    UC month;
    UC day;
    UC hour;
} TIME_STRUCT;

typedef struct
{
    UC qu;
    UC wei;
}   HZ_STRUCT;

typedef struct
{
    UI user_no;              /* user unit sequence number */
    UC user_name[25];        /* user unit name            */
    UI phones;
} USERS_STRUCT;

typedef struct
{
    UC phone_no[9];
} A_PHONE;

typedef struct {
	UC phone_no[7];
}SPECIAL_CALL;

/* global variables */
extern UI             Menu_path;
extern UL far 	*Tick_cnt_ptr;

EXTERN UC	      DEBUG;	/* run in debug mode */
EXTERN UC	      *IDDRateFileName;
EXTERN UC	      *DDDRateFileName;
EXTERN UC	      *LDDRateFileName;
EXTERN UC	      *TotTimeFileName;
EXTERN UC            *SIORecBuf[PORT_NUM], *CmdBuf[PORT_NUM];
EXTERN TIME_STRUCT    s_time, e_time;
EXTERN SYS_MODE       Sys_mode;
EXTERN UC             Page_len;
EXTERN UC             Usr_typ;
EXTERN RECORD_STRUCT  temp_list;
EXTERN UC             Authflg;
EXTERN PHONE_STRUCT  *Phone_top;
EXTERN UI             Oldlen, Newlen;
EXTERN UI             ANewlen;
EXTERN UI             AOldlen;
EXTERN AUTHCD_STRUCT *Auth_cd_top;
EXTERN UC	     *ChargeDataFile[2];
EXTERN CASH_PLEDGE   *Cash_top[2];
EXTERN UC	      *CashDataFile[2];
EXTERN UI             Cash_len[2];
EXTERN UC             Cashflg[2];
EXTERN CASH_PLEDGE   *temp_cash;
EXTERN SPECIAL_CALL  *Call_top[4];
EXTERN UC             Ldd_fflg;
EXTERN COLOR_STRUCT   Dsp_clr;
EXTERN UI  CmdCount[PORT_NUM], TRCount, IMTRCount, ERRCount, ErrCount;
EXTERN UI  SIO_INT[2], SIO_DATA[2], SIO_IER[2], SIO_IIR[2], SIO_LCR[2];
EXTERN UI  SIO_MCR[2], SIO_LSR[2], SIO_MSR[2];
EXTERN UI  SIORecTail[PORT_NUM], SIORecHead[PORT_NUM];
EXTERN UL  	      TimeOut;
EXTERN DELAY_STRUCT   Delay;
EXTERN UC  Intern_prn, Nation_prn, Local_prn;
EXTERN UC             Dsp_typ;
EXTERN UC             Usr_nam[31];
EXTERN UI             Tbllen;
EXTERN UI             Oldend;
EXTERN UC             Flag;
EXTERN RATE_STRUCT huge *Tbltop;
EXTERN int            Count;
EXTERN UI             Max_len;
EXTERN UC             Distr_name[9];
EXTERN int            semi_hour, semi_min, semi_sec;   /* half fee time             */
EXTERN int            hour, min, sec;                  /* all fee time              */
EXTERN HALF_FEE       half_fee;
EXTERN UC             week1, week2;                    /* week day of start and end */
EXTERN HOLIDAY        holiday[7];
EXTERN ADD_STRUCT     Intern_add, Honkon_add, Nation_add, Local_add;
EXTERN RATE_STRUCT    rate, rate1;
EXTERN UI             UNewlen;
EXTERN TIME_STRUCT    Ps_time, Pe_time;
EXTERN USERS_STRUCT  *User_top;

/* main.c */
void main(int argc, char *argv[]);
void main_menu_proc(void);

/*clr_rcd.c*/
void clr_rcd(void);
UC clr_pass(void);

/* set_data.c */
void set_data(void);
void set_data_proc(void);

/* rcd_proc.c */
void phone_record_proc(void);
void warn_mesg(UC *title, UC *warn);

/* deft_pro.c */
void deft_process(void);

/* pas_proc.c */
UC input_pass(void);
void modify_pass(void);
void modi_sys_pass(UC mode);

/* prt_cfg.c */
void get_sys_config(void);
void save_sys_config(void);
void set_prn_cfg(void);
void disp_prn_cfg(void);
void input_prn_cfg(void);

/* sio_cfg.c */
void set_sio_cfg(void);
void disp_sio_cfg(void);
void input_sio_cfg(void);

/* inq_tfe.c */
void stats_fee(void);
void inq_total_fee(void);
void disp_tfee_head(UC opflg);
void disp_tfee_detl1(void);
void disp_tfee_detl2(void);
void disp_tfee_detl3(void);
char *fdigt_tran(double betran);
char *ldigt_tran(UL betran);
void prt_total_fee(void);

/* inq_pfe.c*/
void process_fee_all(UC opflg2);
void process_fee_part(UC opflg1, UC opflg2);
void process_fee_one(UC opflg1, UC opflg2);
UC   inq_part_fee(PHONE_STRUCT *phone, UC *name, UC opflg);
void disp_pfee_detl1(PHONE_STRUCT *phone, UC *name, UC opflg);
void disp_pfee_detl2(PHONE_STRUCT *phone);
void disp_pfee_detl3(PHONE_STRUCT *phone);
UC   prt_phone_fee(PHONE_STRUCT *phone, UI num);
UC   prt_usr_fee(PHONE_STRUCT *phone, USERS_STRUCT usr, UI num);
void prtfee_part(UC opflg);
void prtfee_all(UC opflg);
UC   inv_one(PHONE_STRUCT *phone, UC *name, UC opflg1, UC opflg2, UC opflg3);
UC   htl_inv_one(PHONE_STRUCT *phone, UL phone_no, UC opflg1, UC opflg2);
//void htl_dsp_one(PHONE_STRUCT *phone, UL phone_no);
void htl_dsp_one(PHONE_STRUCT *phone, UL phone_no, UC isauthcd);
UC   set_dbl_char(void);
UC   rst_nomal_char(void);
UC   set_clos_tbl(void);
UC print_pfehd(void);
UC print_pfetbl(void);
UC print_pfedtal(void);
UC print_ufehd(void);
UC print_ufedtal(void);
UC print_invtbl1(void);
UC print_invdtl1(void);
UC print_invtbl2(void);
UC print_invtbl3(void);
UC print_invdtl2(void);
UC print_invtbl4(void);
UC print_invtbl5(void);
UC print_invdtl3(void);
UC print_invdtl4(void);
UC print_invtbl6(void);
UC print_invtbl7(void);
UC print_invtbl8(void);


/* inq_fee.c*/
void inq_fee_part(void);
void inq_ufee_part(void);
void inq_fee_all(void);
void inq_afee_all(void);
void inq_ufee_all(void);
void inq_afee_part(void);
void disp_inqfee_head(UC opflg);
void disp_a_usrfee(PHONE_STRUCT *phone, USERS_STRUCT usr, UC pos, UI num);
void disp_a_phnfee(PHONE_STRUCT *phone, UC pos, UI num);
void brush_tbl(UC xnum, UC ynum, UC bk_colr);
void brush_one_row(UC row, UC xnum, UC bk_colr);
void disp_a_ausfee(AUTHCD_STRUCT *authcd, UC pos, UI num);

/* inv_ufe.c */
void inv_one_usr(UC opflg3);
void inv_all_usr(void);
void inv_part_usr(void);
void inq_one_usr(void);
void inq_part_usr(void);
void cal_usr_fee(UI pnum, USERS_STRUCT *user, PHONE_STRUCT *phone);
void clr_ufc(PHONE_STRUCT *phone, UC opflg);
UC   input_user(UL *no, UC opflg);

/* inq_one.c */
void inquire_one_phone(void);
void print_one_phone(void);
void inquire_one_authcd(void);
void print_one_authcd(void);
void inquire_one_user(void);
void print_one_user(void);
void inquire_one_record(UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd);
void print_one_record(UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd);
UC   disp_a_rcd(UC mode, UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd, UC *position, UI *num);
UC   prn_a_rcd(UC mode, UL phone_no, USERS_STRUCT user, UC acflg, UC isauthcd, UI *num);

/* set_phon.c */
PHONE_STRUCT *binary_pcode(UC *input_no);
void send_pcode_tbl(void);
void set_phone_num(UC mode);
void disp_key_help(void);
void load_phone_tbl(void);
void unload_phone_tbl(void);
UI   phone_tbl_len(void);
void disp_phone_tbl(PHONE_STRUCT *tbl, UI num, UC mode);
UI   maxnum(UI num1, UI num2);
void input_phone_num(UC mode);
void modi_phone_tbl(UC mode);
void pgup_phone_tbl(UC mode);
void pgdn_phone_tbl(UC mode);
void save_phone_tbl(UC status, UC mode);
void sort_phone_tbl(PHONE_STRUCT *newitem);
void clr_phone_tbl(void);
void delete_num(UC mode);
void auto_add_one(UC mode);
UC select_1in3(void);
UC select_1in2(UC mode);

/* edt_rate.c */
UC   IDD_rate_set(void);
UC   DDD_rate_set(void);
UC   LDD_rate_set(void);
void set_rate(UC mode);
void sort_tbl(RATE_STRUCT huge *newitem);
void set_trunk(void);
void loc_tbl(void);
UI   tbl_len(void);
void save_tbl(UC mode,UC status);
void load_tbl(UC mode);
void unload_tbl(void);
void disp_tbl(RATE_STRUCT huge *tbl, int num);
void disp_head(UC mode);
void input_data(UC mode);
void modi_tbl(UC mode);
void pgup_tbl(void);
void pgdn_tbl(void);
void clr_tbl(void);
void delet_item(void);
void out_prompt(UC mode);
void loc_page(void);
UI   maxnum(UI num1, UI num2);
void search_area_no(void);
RATE_STRUCT huge *blocate_area(UC *input_no);
void copy_last_item(void);
void disp_one_row(UC row);
void clr_one_row(UC row);

/* rec_add.c */
void get_record_add(void);
void set_record_add(UC mode);

/* half_fee.c */
void get_half_fee(void);
void set_half_fee(void);
void disp_day(void);
void disp_spring_day(void);
void input_half_fee(void);
void input_spring_day(void);
void save_half_fee(void);

/* prn_rate.c */
void prn_rate(void);
UC print_rate(UC mode);
UC check_prn(void);
UC chk_prn(void);
UC print_a_rattbl(RATE_STRUCT huge *tbl);
UC print_ratrpt_head(UC isidd);
UC print_ratrpt_tail(void);
UC print_ratrpt_detail(RATE_STRUCT huge *tbl, UC isidd, UI num);
UC print_rattbl(void);


/* set_cash.c */
void set_cash_pledge(UC mode);
void set_cash_flag(void);
UC   check_maint_cash(UC mode);
UC   load_cash_tbl(UC mode);
void unload_cash_tbl(UC mode);
UC   maint_to_cash(UC mode);
UI   cash_tbl_len(UC mode);
void disp_cash_head(UC opflg, UC mode);
void disp_cash_help(UC opflg, UC mode);
void disp_cash_tbl(CASH_PLEDGE *tbl, int num, UC cmode, UC mode);
void input_cash_data(UC mode);
void modi_cash_tbl(UC mode);
void save_cash_tbl(UC mode);
void loc_cash_tbl(UC cmode);
void clr_cash_tbl(void);
void pgup_cash_tbl(UC cmode, UC mode);
void pgdn_cash_tbl(UC cmode, UC mode);
void loc_cash_page(UC cmode, UC mode);
CASH_PLEDGE *binary_cash(UC *input_no, UC mode);
UC cash_pass(void);
void indicator(UI x, UI y, UC ind_color);

/* setclass.c */
int send_phone_table(UC mode);
//void set_special(TABLE_STRUCT  phone_tbl);
void load_special_tbl(UC mode);
UI special_tbl_len(UC mode);
void input_special_num(UC mode);
void unload_special_tbl(UC mode);
void disp_special_tbl(SPECIAL_CALL *tbl, UI num);
void save_special_tbl(UC status, UC mode);
void modi_special_tbl(UC mode);
void sort_special_tbl(SPECIAL_CALL *newitem, UC mode);
void disp_special_key_help(void);
void delete_special_num(UC mode);


/* set_user.c */
void set_user_unit(void);
void disp_user_head(void);
void disp_user_help(void);
void load_user_tbl(void);
void unload_user_tbl(void);
UI   user_tbl_len(void);
void disp_user_tbl(USERS_STRUCT  *tbl, int num);
void clr_user_tbl(void);
void input_user_data(void);
void save_user_tbl(UC status);
void sort_user_tbl(USERS_STRUCT  *newitem);
void loc_user_tbl(void);
void modi_user_tbl(void);
void pgup_user_tbl(void);
void pgdn_user_tbl(void);
void delet_user_item(void);
void loc_user_page(void);
USERS_STRUCT *binary_user(UL locate_no);
void copy_last_user(void);
void disp_one_user(UC row);

/* set_auth.c */
int ShakeHand(UC port);
int ShakeHandAll(void);
int DownloadCode(UC *tmp, UI len1, UI len2, UC ch, UC port);
void process_auth_code(UC mode);
void disp_authcd_head(UC mode);
void disp_authcd_help(UC mode);
UC   load_authcd_tbl(UC Alloc);
void load_afee_tbl(void);
void unload_authcd_tbl(void);
UI   authcd_tbl_len(void);
void disp_authcd_tbl(AUTHCD_STRUCT  *tbl, int num);
void clr_authcd_tbl(void);
void process_authcd_data(UC mode);
void save_authcd_tbl(UC status, UC send_flag);
void sort_authcd_tbl(AUTHCD_STRUCT  *newitem);
void loc_authcd_tbl(void);
void set_auth_head(void);
void modi_authcd_tbl(void);
void pgup_authcd_tbl(void);
void pgdn_authcd_tbl(void);
void delet_authcd_item(void);
void loc_authcd_page(void);
AUTHCD_STRUCT *binary_authcd(UL locate_no);
void copy_last_authcd(void);
void disp_one_authcd(UC row);

/* pho_user.c */
void set_phone_user(void);
void disp_pho_user_head(void);
void disp_pho_user_help(void);
void disp_pho_user_tbl(PHONE_STRUCT *tbl, UI num);
void input_pho_user_data(void);
void modi_pho_user_tbl(void);
void save_pho_user_tbl(void);
void loc_pho_user_tbl(void);
void clr_pho_user_tbl(void);
void pgup_pho_user_tbl(void);
void pgdn_pho_user_tbl(void);
void loc_pho_user_page(void);

/* user_ind.c */
void set_user_index(struct ffblk *user_fblk, struct ffblk *maint_fblk);

/* inq_user.c */
void inquire_user_unit(void);
UC   input_user_no(UL *no);
void search_user(UI input_no, UI *pnum, USERS_STRUCT *user);
void inq_user_head(USERS_STRUCT *user);
void disp_a_phone(UC *no, UI num);

/* prn_user.c */
void print_user_unit(void);
UC print_usrtbl(void);

/* get_a_re.c */
void InitPortAdd(void);
void new_com(UC port);
void SIORecInt(UC port);
void ToCPU(UC port, UC ch);
void SIOInit(void);
void SIOEnd(void);
UC   take_all_record(void);
void check_dat_tmp(void);
UC   ConnTermAndCPU(UC port);
UC   GetCmdFromBuf(UC port);
UC   take_record(void);
UC   tmp_to_dat(void);
UC   dat_to_tmp(void);
int  CheckPort(UC port);

/* ovr_cash.c */
void overflow_cash(UC opflg, UC mode, UC send, UC tout
);
void disp_over_head(UC mode);
void disp_over_cash(UI position,CASH_PLEDGE *cashe);
void clr_over_cash(void);
UC EndofCashFile(FILE *fp);
UC prt_over_cash(UI pos, CASH_PLEDGE *cashe);
UC print_ocsdtl(void);
UC print_ocstbl(void);


/* get_i_re.c */
void take_immediate_record(void);
void draw_rec_tbl(void);
void disp_rec_head(void);
void disp_rec_tbl(void);
void disp_rec_row(UC *cash_flag,RECORD_STRUCT *record,RATE_STRUCT *rec_rate,\
		  UC **a_name, UC row, UC pos);
void clr_rec_tbl(UC row);
void callee_uc(UC *callee,UC *tel_no);
void find_auth_usr(UC *auth_name);
void auth_usr_proc(UC *auth_name);

/* imm_prn.c */
void set_imm_print(void);
void disp_imm_prn(void);
void input_imm_prn(void);

/* fee_rec.c */
void Intern_charge(void);
void Nation_charge(void);
void Local_charge (void);
void all_record_transfer(void);
UC   imm_record_transfer(UC port);
UC   cbcd_uc(UC *c_bcd);
UC   delay_proc(UC mode);
UC   is_contry(void);
void update_cash(UC mode);

/* rechg_fee.c */
void recharge_all_record(void);
void recharge_a_kind_rcd(UC mode);

/* fee_base.c */
void just_date(void);
UC   is_cont_day(void);
UC   weekday(UI year, UC month, UC day);
void holiday_all_half(void);
void nation_same_day_hour(void);
void holiday_up_half(void);
void nation_two_day_up(void);
void holiday_dn_half(void);
void nation_two_day_dn(void);
void nation_same_day_charge(void);
void nation_cont_day_charge(void);
UC   nation_charge_hour(void);
void normal_same_day_charge(void);
void normal_cont_day_charge(void);
UC   normal_charge_hour(void);

/* get_rate.c */
RATE_STRUCT huge *get_rate(UC *tel_no, UC mode);
RATE_STRUCT huge *srch_rate(UC *tel_no,RATE_STRUCT huge *location,UC mode);
RATE_STRUCT huge *blocate_rate(UC *tel_no,UC mode);
UI   rate_tbl_len(RATE_STRUCT huge *tbl);
UC   load_rate_tab(void);
void unload_rate_tab(void);
RATE_STRUCT huge *binary_rate(UC *tel_no,UC mode);

/* inq_loc.c */
void inquire_one_local(void);
UC   input_phone_no(UL *no, UC opflg);
void disp_record_head(void);
UC   is_in_range(void);
void disp_a_record(char *auth_name, UC pos, UI num);
void clr_record_tbl(void);
UC   EndofRecFile(FILE *fp);
void inquire_user_local(void);
void inquire_all_local(UC isauthcd);
UC   input_auth_cd(UL *auth_cd, UC opflg);
void inquire_auth_local(void);

/* inq_afe.c */
UC   inv_aone(AUTHCD_STRUCT *authcd, UC opflg2, UC opflg3);
void prtafee_all(void);
void prtafee_part(void);
UC   prt_authcd_fee(AUTHCD_STRUCT *authcd, UI num);
void disp_afee_detl3(AUTHCD_STRUCT *authcd);
void disp_afee_detl2(AUTHCD_STRUCT *authcd);
void disp_afee_detl1(AUTHCD_STRUCT *authcd);
UC   inq_apart_fee(AUTHCD_STRUCT *authcd, UC opflg);
void process_afee_one(UC opflg1, UC opflg2);
void process_afee_part(UC opflg1, UC opflg2);
void process_afee_all(UC opflg2);
UC print_afehd(void);
UC print_afetbl(void);
UC print_afedtal(void);
UC print_total(double total_f);


/* inq_long.c */
void inquire_one_long(void);
void inquire_user_long(void);
void inquire_all_long(UC isauthcd);
void inquire_auth_long(void);
UC  disp_long_rcd(UC mode, UC isphone, UC isauthcd, \
		  UL phone_no, UL auth_cd, USERS_STRUCT user);

/* inq_all.c */
void inquire_all_record(UC opflg);
UC  disp_a_kind_rcd(UC mode, UC opflg, TIME_STRUCT in_stime, TIME_STRUCT  in_etime);

/* prn_loc.c */
void print_one_local(void);
void print_user_local(void);
void print_auth_local(void);
void load_phone_no(UI pnum, USERS_STRUCT *user);
void unload_phone_no(void);
UC   binary_phone(USERS_STRUCT *user);
void print_all_local(UC isauthcd);
UC   print_a_record(char *auth_name, UI num);
UC print_rcdrpt_head(UC isidd);
UC print_rcdrpt_tail(void);
UC print_rcdrpt_detail(char *auth_name, UI num, UC isidd);
UC print_rcdtbl(void);
UC new_page(void);
UC  prn_long_rcd(UC mode, UC isphone, UC isauthcd, \
		    UL phone_no, UL auth_cd, USERS_STRUCT user);

/* prn_long.c */
void print_one_long(void);
void print_user_long(void);
void print_auth_long(void);
void print_all_long(UC isauthcd);

/* prn_all.c  */
void print_all(UC opflg);
UC  prn_a_kind_rcd(UC mode, UC opflg, TIME_STRUCT in_stime, TIME_STRUCT  in_etime);

/* del_all.c  */
void delete_all_record(UC isauthcd);
UC   set_del_time(TIME_STRUCT *del_stime,TIME_STRUCT *del_etime);
UC   input_del_time(TIME_STRUCT *del_stime,TIME_STRUCT *del_etime);
UC   in_time_range(TIME_STRUCT del_stime,TIME_STRUCT del_etime);
UC   valid_pass(UC mode);
void del_a_kind_file(UC mode, UC is_hst, UC isauthcd, TIME_STRUCT del_stime,\
		     TIME_STRUCT del_etime);

/* out_range.c */
void out_of_range(void);
UC   set_in_time(TIME_STRUCT *in_stime,TIME_STRUCT *in_etime);
UC   input_in_time(TIME_STRUCT *in_stime,TIME_STRUCT *in_etime);
void disp_num(void);
void disp_out_head(void);
void out_range_proc(UC mode,TIME_STRUCT in_stime,TIME_STRUCT in_etime);
void disp_out_record(char *auth_name, UC pos);
void clr_out_tbl(void);
UC   out_del_pass(void);
void file_refresh(UC mode,UC modi);
void mes_disp(UC back_in, UC *msg);

/* back_old.c */
void backup_old_rec(void);
UC   input_month(UC mode,UL *mon);
UC   rcd_file_filter(UC mode, UC month[3]);

/* inq_old.c */
void inquire_old_rec(UC isphone);
UC  disp_a_old_rcd(UC mode, UC isphone, UL mon, UL phone_no, UL input_cd);

/* set_time.c */
void get_default_time(void);
void set_sort_time(void);
void input_sort_time(void);

/* mon_add.c */
void set_month_add(void);
void disp_fee_head(void);
void disp_fee_help(void);
void disp_fee_tbl(PHONE_STRUCT *tbl, UI num);
void input_fee_data(void);
void modi_fee_tbl(void);
void save_fee_tbl(void);
void loc_fee_tbl(void);
void clr_fee_tbl(void);
void pgup_fee_tbl(void);
void pgdn_fee_tbl(void);
void loc_fee_page(void);
PHONE_STRUCT *binary_search(UC *input_no);
PHONE_STRUCT *binary_sechac(UL locate_no);
void copy_last_afee(void);
void disp_one_afee(UC row);

/* cal_fee.c */
void cal_total_fee(void);
//void acount_one(void);
UC   cal_acc_fee(UC isacc);
void clr_acc_rcd(UC isacc);
//void clr_one_rcd(UL phone_no);
void fnd_stime(TIME_STRUCT *time1, TIME_STRUCT *time2);
void fnd_etime(TIME_STRUCT *time1, TIME_STRUCT *time2);
//void cal_one_fee(UL phone_no);
//void stamp_rcd(UC mode, UL phone_no);
void cal_one_fee(UL phone_no, UC isauthcd);
void clr_one_rcd(UL phone_no, UC isauthcd);
void acount_one(UC isauthcd);
void stamp_rcd(UC mode, UL phone_no, UC isauthcd);
UC IsSpecialCall(UC *callee, UI len, UC mode);

/* input_p.c */
UC   inp_phid_num(UL *no, UI *num, UC opflg1, UC opflg2);
void disp_phid_num(UC opflg1, UC opflg2);
UC   input_proc(UL *no, UI *num, UC opflg1);

/* inq_cash.c */
void inquire_cash_pledge(UC mode);
void inq_cash_help(void);
void inq_cash_data(UC mode);

/* message.c */
void message(int mode);
void message_win(UC back_in, UC *msg, UC echo_time);
void message_disp(UC back_in, UC *msg);
void message_end(void);

/* clr_scr.c */
void GOTOxy(char x, char y);
int  whereX(void);
int  whereY(void);
void clr_scr(UI left,UI top,UI right,UI bottom,UC fore_c,UC back_c);

/* calib_ti.c */
UC calib_time(void);
UC modi_time(UC date[3], UC time[3]);

/* swth_csh.c */
void set_csh_swtch(void);
void disp_csh_swtch(void);
void input_csh_swtch(void);

/* usr_cfg.c */
void set_usr_cfg(void);
void disp_usr_cfg(void);
void input_usr_cfg(void);

/* acc_fee.c */
UC print_accrpt_detail(ACC_STRUCT *acc, UI num, UC isacc);
UC print_accrpt_tail(UC isacc);
UC print_accrpt_head(UC isacc);
UC print_a_acc(ACC_STRUCT *acc, UC isacc);
void get_s_no(ACC_STRUCT *acc, UC *serial_no);
UC  EndofAccFile(FILE *fp);
void disp_a_accfee(ACC_STRUCT *acc, UC pos, UC isacc);
void disp_accfee_head(UC isacc);
UC prn_acc_fee(UC isacc);
UC inq_acc_fee(UC isacc);
UC print_afehd(void);
UC print_afetbl(void);
UC print_afedtal(void);
UC print_acctbl(void);
UC print_accdtal(UC *name);
FILE *open_acc_file(UC mode);
void acc_proc(UC isacc);

/* dsp_cfg.c */
void set_dsp_cfg(void);
void disp_dsp_cfg(void);
void input_dsp_cfg(void);

/* delay_f.c */
void set_delay(void);

/* setclass.c*/
void set_special_call(UC mode);

/*dbcs_prn.c*/
void getlib_char(int num, UC* p, UC* pc2);
void getlib_asc(int num, UC* p, UC* pa2);
void pch_16(UC *pp1, UC *qq1);
void pch_8(UC *pp1, UC *qq1);
void str_recon(UC *st, UI length, UC *p1, UC *p2_c, int *num1, UC *p2_a);
void rtn_recon(UC *st, UI length, UC *p1);
UC  prn_cs(int id, UC *st, UI length);
UC  pin9(int id, UC *fbuff, UI length);
UC prnf(const UC *fmt, ...);
UC prnfd(const UC *fmt, ...);
void getlib_24char(int num, UC* p, UC* pc2);
void getlib_24asc(int num, UC* p, UC* pa2);
void pch_24(UC *pp1, UC *qq1);
UC pin24(int id,UC *fbuff, UI length);
UC prn_24cs(int id,UC *st,UI length);
void buffree(UC *p1, UC *p2_a, UC *p2_c, UC *p3, UC *p4, int num_c, int j1);

/* pbx_cfg.c*/
/*void set_pbx_cfg(void);
void disp_pbx_cfg(void);
void input_pbx_cfg(void);
*/
/* sta_rpt.c */
void statistic(void);

/* Swth_ldd.c */
void set_ldd_cfg(void);
void disp_ldd_cfg(void);
void input_ldd_cfg(void);

/* comlib.c */
int comtxch(int,int,char);
int comrxch(int,int,char*);

#endif
