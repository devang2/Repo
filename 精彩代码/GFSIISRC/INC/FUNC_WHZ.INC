/*******************************************************/
/* FILENAME:                                           */
/* DATE    :  1993.5.20                                */
/* AUTHOR  :  GuoGuanQi                                */
/* USED BY :  the main call modula                     */
/* FUCTION :  providing all the function declarations  */
/*            called by main calling modula.           */
/*******************************************************/
/* swap.c */
UC set_instant_swap(void);
UC set_delayed_swap(void);
UC query_swap_infor(void);

/* pcb.c */
UC pcb_control(void);

/* line.c */
UC line_query(void);
UC line_close(void);
UC line_open(void);

/* service.c */
UC look_for1(void);
UC look_for2(void);
UC look_for3(void);
UC look_for4(void);

/* trace.c */
UC trace_control(void);
UC DT_trace(void);
UC trace_disp(void);
UC print_control(void);