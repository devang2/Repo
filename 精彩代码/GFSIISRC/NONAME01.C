PHONE_STRUCT *binary_pcode(UC *input_no)
{
    PHONE_STRUCT *tbl, *tbl_top;
    UI  tbllen, top, bottom, middle;
    int result;

    tbl_top = Phone_top;
    tbl = Phone_top;
    if(Newlen == 0)
	tbllen = phone_tbl_len();
    else
	tbllen = Newlen;

    top = 0;
    bottom = tbllen-1;

    /***** Compare with the first item. *****/
    if(strcmp(    tbl->phone_no, input_no) < 0 && \
       strcmp((tbl+1)->phone_no, input_no) > 0)
	    return(NULL);

    if(strcmp(tbl->phone_no,input_no) > 0)
	    return(NULL);

    /***** Compare with the last item. *****/
    if(strcmp(tbl[bottom].phone_no,input_no) < 0)
	    return(NULL);

    /***** Compare and locate in the pcode table. *****/
    while(top <= bottom)
    {
	middle = (UI) (top+bottom)/2;
	tbl = &tbl_top[middle];
	result = strcmp(tbl->phone_no,input_no);

	if(!result)          /* find! */
	    return(tbl);

	else if(result < 0)       /* tbl->phone_no < input_no */
	    top = middle+1;
	else if(result > 0)       /* tbl->phone_no > input_no */
	    bottom = middle-1;
    }

    return(NULL);                 /* not find! */
}

