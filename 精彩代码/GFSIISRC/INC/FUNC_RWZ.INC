/* MSG.C */
UC      look_messages(void);
/* REC.C */
UC      look_records(void);
void	add_one_record(UC *buffer);
/* SET_DT.C */
UC      set_datetime(void);
/* LOOK_VER.C */
UC      look_version(void);

/* COMM_HDL.C */
void echo_bills_number(UC *buffer);
void justify_time(UC *buffer);
void echo_init_commstate(void);
void echo_commstate(void);
void echo_message(UC *buffer);
void echo_load_request(UC tta);
void echo_warn_notice(UC *buffer);

/* LOGIN.C */
void login(void);
/* PWD.C */
UC	modify_password(void);
/* GET_OPT.C */
void get_operator(UC *opt_name);
/* PRIV.C */
void modify_privil(void);
/* OPT_MNG.C */
UC	operator_manage(void);

/* ECHOLOAD.C */
UC echo_load(void);
void disp_err_info(UI err_no);
void disp_load_info(UI file_no, UI frame_no);
