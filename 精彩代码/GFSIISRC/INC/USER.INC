UC usr_init(void);

UI usr_get_number(UI inner_code);
UC usr_get_data(UI inner_code, USER_STRUCT *user_data);
UI usr_get_innercode(UI user_number);

UC isUserNoEx(UI number);
UC isCodeUsed(UI code);
UC isAccEx(UI account);
void get_default(USER_STRUCT *us);

UC usr_chg_OutTab(UI code, UI number);
UC usr_chg_AccTab(UI code, UI account);
UC usr_chg_usdat(UI code, USER_STRUCT *us, UI nn);

UC usr_app_num(UI inner_code, UI user_number);
UC usr_append(UI inner_code, UI user_number, UI account);
UC usr_delete(UI number);
UC usr_del_number(UI number);
UC usr_chg_number(UI old_num, UI new_num);
UC usr_chg_OutTab(UI code, UI number);
UC usr_chg_data(UI inner_code, USER_STRUCT *user_data);

UI usr_get_account(UI user_number);
UI usr_findfirst(UI account_no);
UI usr_findnext(void);
UC usr_chg_account(UI code, UI account);
UC usr_chg_AccTab(UI code, UI account);
UC usr_load_tab(void);
void usr_unload_tab(void);
UI usr_findcode(UI number);
UC usr_add_account(UI account);
UC usr_del_account(UI account);
UC usr_chg_accdat(UI account, USER_ACC *accdat);
UC usr_chg_acc(UI account,USER_ACC *acc, UI nn);
UC usr_get_accdat(UI account, USER_ACC *accdat);

UC usr_app_pbx(UC pbxNo, UI *pbx, UI pbxnum);
UC usr_del_pbx(UC pbxNo);
UC usr_get_pbx(UC pbxNo, UI *pbx);
UC usr_chg_pbxdata(UC pbxNo, USER_STRUCT *usr_data);
UC usr_find_pno(UI code, UC tag);
UC usr_app_pos(UC posNo, UI *pos, UI *number);
UC usr_del_pos(UC posNo);
UC usr_get_pos(UC posNo, UI *pos);
UC usr_chg_posdata(UC posNo, USER_STRUCT *usr_data);
UC usr_find_posno(UI number);

UC usr_load_pabx(void);
UC usr_load_pos(void);

UC usr_add_line(UC no1, UC no2, UI code, UI number, UC tag);
UC usr_del_line(UC no1, UC no2, UC tag);

UC usr_load_sps(void);
UC usr_add_spline(UI code, UC spn, UI no);
UC usr_del_spline(UI code);
UC usr_get_sps(SP_STRUCT *sps);
UC usr_chg_sps(SP_STRUCT *sps);
void get_sp_default(SP_STRUCT *sps, UC spno);
UC get_ss_data(UC *spno, SP_STRUCT *ss_data);

UI  to_inn_num(UL number);
UC *to_out_num(UI number);

void usr_get_date(UC *date);