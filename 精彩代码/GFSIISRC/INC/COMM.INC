/* INIT_MB.C */
void init_mail(void);
void exit_mail(void);

/* DATA_LIN.C */
UC   connect(UC fd);
UC   disconnect(UC fd);
void clr_data_buf(void);
UC   send(UC ta, UC * msg);
void receive(UI flag, UC * msg);

/* BUF_MANG.C */
void buffer_manage(void);


/* ESC_HL.C */
void set_receive_mode(UC esc, UC handle_check) ;
UC   get_handle(void);
UC   ptc_receive(UC fd, UC cmd, UC handle, UC *msg, UI delay);
