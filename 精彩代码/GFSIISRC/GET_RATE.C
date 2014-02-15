/**********************************************************************
 *                                                                                                                                *
 *          FILE NAME:    GETRATE.C                                   *
 *                                                                                                                                        *
 *          FUNCTION:     Search for a rate in the rate table         *
 *                        according to the input area number.         *
 *                                                                                                                                        *
 *          DATE:                 05/05/1993                                                              *
 *                                                                                                                *
 **********************************************************************/
 #include <string.h>
 #include <alloc.h>
 #include <io.h>
 #include <fcntl.h>

 #include <bio.inc>
 #include <menu.inc>
 #include <g_id.inc>

 #include "feedef.h"


 UI          Intel_len;
 UI          Naton_len;
 UI          Local_len;
 RATE_STRUCT huge *Intel_top;
 RATE_STRUCT huge *Naton_top;
 RATE_STRUCT huge *Local_top;

/*--------------------------------------------------------------
  ------------------------- get_rate() -------------------------
		Function:       Get a rate from rate data table accoding
				to the telepone number inputted.
		Calls:          rate_tbl_len,blocate_area,srch_rate
		Called by:
		Input:          tel_no -- telephone number
				mode -- 1:      international rate table
								2:  toll rate table
		Output:         rate -- rate
		Return:         TRUE -- geting is succesful.
				FALSE -- geting is failure.
  ---------------------------------------------------------------*/
 RATE_STRUCT huge *get_rate(UC *tel_no, UC mode)
 {
	RATE_STRUCT huge *location;
	RATE_STRUCT huge *rate;
	UC tel_no_str[9];

	if((mode!=IDD) && (mode!=DDD) && (mode!=LDD))
	    return(NULL);

	bcd_str(tel_no_str, tel_no, 8);   /* BCD code change to ASCII code   */

	location = blocate_rate(tel_no_str,mode);   /* LOCATE FIRST POSITION */
	if(location == NULL)
	    return(NULL);

	rate = srch_rate(tel_no_str,location,mode); /* SERACH BACK FOR RATE  */

	return(rate);
}

/*--------------------------------------------------------
  ------------------- rate_tbl_len() ---------------------
		Function:       Calculate the length of rate
				table.
		Calls:          Nnoe
		Called by:      get_rate()
		Input:          None
		Output:         None
		Return:         The length of rate table
 ---------------------------------------------------------*/
 UI rate_tbl_len(RATE_STRUCT huge *tbl)
 {
	UI tbllen = 0;

	while(tbl->length!=0 /* && tbllen<Max_len */)
	{
		tbllen++;
		tbl++;
	}

	return(tbllen);
 }

/*-----------------------------------------------------------
  -------------------- load_rate_tbl() ----------------------
		Function:       Load rate table from "intel.rat" and
				"toll.rat".
		Calls:          None
		Called by:
		Input:          None
		Output:         None
		Return:         TRUE -- loading is succesful.
				FALSE -- loading is failure.
  -----------------------------------------------------------*/
  UC load_rate_tab(void)
  {
	int         handle;
	long        filelen;
	FILE        *fptr;
	RATE_STRUCT huge *tbl;

	fptr = fopen(IDDRateFileName, "rb");
	if(!fptr)
	    return(FALSE);

	Intel_top = (RATE_STRUCT huge *)farcalloc(MAX_INTERN,sizeof(RATE_STRUCT));
	Intel_len = 0;
	tbl = Intel_top;
	while(fread(tbl,sizeof(RATE_STRUCT),1,fptr) == 1)
	{
	    tbl++;
	    Intel_len++;
	}
	fclose(fptr);

	handle = open(DDDRateFileName,O_RDONLY|O_BINARY);
	if(handle == -1)          /* file does not exist */
	{
	    farfree(Intel_top);
	    return(FALSE);
	}

	filelen = filelength(handle);
        close(handle);
	Naton_len = (UI) (filelen/sizeof(RATE_STRUCT));
	Naton_len += 2;

	fptr = fopen(DDDRateFileName, "rb");
	Naton_top = (RATE_STRUCT huge *)farcalloc(Naton_len,sizeof(RATE_STRUCT));
	Naton_len = 0;
	tbl = Naton_top;
	while(fread(tbl,sizeof(RATE_STRUCT),1,fptr) == 1)
	{
	    tbl++;
	    Naton_len++;
	}
        fclose(fptr);

	fptr = fopen(LDDRateFileName, "rb");
	if(!fptr)
	{
	    farfree(Naton_top);
	    farfree(Intel_top);
            return(FALSE);
	}

	Local_top = (RATE_STRUCT huge *)farcalloc(MAX_LOCAL,sizeof(RATE_STRUCT));
	Local_len = 0;
	tbl = Local_top;
	while(fread(tbl,sizeof(RATE_STRUCT),1,fptr) == 1)
	{
	    tbl++;
	    Local_len++;
	}
	fclose(fptr);

	return(TRUE);
 }

/*------------------------------------------------------------
  ------------------- unload_rate_tbl() ----------------------
		Function:       Free the space used by the rate table.
		Calls:          None
		Called by:
		Input:          None
		Output:         None
		Return:         None
  --------------------------------------------------------------*/
  void unload_rate_tab(void)
  {
	if(Local_top != NULL)
	    farfree(Local_top);
	if(Naton_top != NULL)
	    farfree(Naton_top);
	if(Intel_top != NULL)
            farfree(Intel_top);

        return;
  }

/*------------------------------------------------------
  --------------------- srch_rate() --------------------
			Function:       Search back for the area No.
			Calls:          None
			Called by;      get_rate()
			Input:          tel_no -- telephone number
					location -- the position locatted
			Output:         rate struct if found, otherwise
					return NULL
			Return:         TRUE -- searching is succesful.
					FALSE -- searching is failure.
  ------------------------------------------------------*/
 RATE_STRUCT huge *srch_rate(UC *tel_no,RATE_STRUCT huge *location,UC mode)
 {
	int         result;
	RATE_STRUCT huge *tbl_top;

	if(mode == IDD)
	    tbl_top = Intel_top;
	else if(mode == DDD)
	    tbl_top = Naton_top;
	else if(mode == LDD)
	    tbl_top = Local_top;

	do
	{
		result = memicmp(location->area_no, tel_no, location->length);

		if(result == 0)
		    return(location);

		if(location > tbl_top)
		    location--;

/*		if(location < tbl_top)
		    return(NULL);               NOT FOUND AND OUT OF RANGE
*/
	}while(location->trunk_mark != 1);     /* STOP SERACHING AT THE TRUNK ITEM */

	/*****  2Compare with the trunk item. *****/
	result = memicmp(location->area_no, tel_no, location->length);

	if(result != 0)
	    return(NULL);            /* NOT FOUND */

	return(location);
 }

/*------------------------------------------------------
  ------------------- blocate_area() -------------------
			Function:       Locate the first position.
			Calls:          None
			Called by:      get_rate()
			Input:          tel_no -- telephone number
			Output:         None
			Return:         The position of location.
  ------------------------------------------------------*/
 RATE_STRUCT huge *blocate_rate(UC *tel_no,UC mode)
 {
    RATE_STRUCT huge *tbl, huge *tbl_top;
    UC tel_len;
    UI tbllen,comp_len,comp_len1;
    UI position;
    UI top, bottom;
    int result;

    tel_len = strlen(tel_no);

    if(mode == IDD)
    {
	    tbl_top = Intel_top;
	    tbl = Intel_top;
/*	    tbllen = rate_tbl_len(Intel_top); */
	    tbllen = Intel_len;
    }
    else if(mode == DDD)
    {
            tbl_top = Naton_top;
            tbl = Naton_top;
/*	    tbllen = rate_tbl_len(Naton_top); */
	    tbllen = Naton_len;
    }
    else if(mode == LDD)
    {
            tbl_top = Local_top;
            tbl = Local_top;
/*	    tbllen = rate_tbl_len(Local_top); */
	    tbllen = Local_len;
    }

    top = 0;
    if(tbllen > 0)
	bottom = tbllen-1;
    else
	bottom = 0;

    /***** Compare with the first item. *****/
    comp_len  = maxnum(tbl->length, tel_len);
    comp_len1 = maxnum((tbl+1)->length, tel_len);
    if(memicmp(tbl->area_no,tel_no,comp_len)<0       &&    \
       memicmp((tbl+1)->area_no,tel_no,comp_len1)>0)
	    return(tbl);

    if(memicmp(tbl->area_no,tel_no,comp_len) > 0)
	    return(tbl);

    /***** Compare with the last item. *****/
    comp_len = maxnum(tbl[bottom].length, tel_len);
    if(memicmp(tbl[bottom].area_no,tel_no,comp_len) < 0)
            return(&tbl[bottom]);

    /***** Compare and locate in the rate table. *****/
    while(top <= bottom)
    {
        position = (UI) (top+bottom)/2;
	tbl = &tbl_top[position];
        comp_len = maxnum(tbl->length, tel_len);
	result = memicmp(tbl->area_no,tel_no,comp_len);

        if(!result)
            return(tbl);

        if(result < 0)
        {
            tbl++;
            comp_len = maxnum(tbl->length, tel_len);
	    result = memicmp(tbl->area_no, tel_no, comp_len);

	    if(result == 0)            /* found */
		return(tbl);
	    else if(result > 0)
		return(tbl-1);

            top = position;           /* SEARCH DOWN */
        }
	else
        {
            tbl--;

            comp_len = maxnum(tbl->length, tel_len);
	    result = memicmp(tbl->area_no, tel_no, comp_len);

            if(result == 0)
                return(tbl);        /* LOCATTED !!! */

            if(result < 0)
		return(tbl);

            bottom = position;        /* SEARCH UP */
        }
    }    /* END OF FOR */

    return(NULL);
}

RATE_STRUCT huge *binary_rate(UC *tel_no,UC mode)
{
    RATE_STRUCT huge *tbl, huge *tbl_top;
    UI tbllen;
    UI position;
    UI top, bottom;
    int result;

    if(mode == IDD)
    {
	    tbl_top = Intel_top;
	    tbl = Intel_top;
            tbllen = rate_tbl_len(Intel_top);
    }
    else if(mode == DDD)
    {
	    tbl_top = Naton_top;
            tbl = Naton_top;
            tbllen = rate_tbl_len(Naton_top);
    }
    else if(mode == LDD)
    {
            tbl_top = Local_top;
            tbl = Local_top;
            tbllen = rate_tbl_len(Local_top);
    }

    top = 0;
    bottom = tbllen-1;

    /***** Compare with the first item. *****/
    if(memicmp(tbl->area_no,tel_no,tbl->length) < 0       &&    \
       memicmp((tbl+1)->area_no,tel_no,(tbl+1)->length) > 0)
	    return(NULL);

    if(memicmp(tbl->area_no,tel_no,tbl->length) > 0)
	    return(NULL);

    /***** Compare with the last item. *****/
    if(memicmp(tbl[bottom].area_no,tel_no,tbl[bottom].length) < 0)
	    return(NULL);

    /***** Compare and locate in the rate table. *****/
    while(top <= bottom)
    {
        position = (UI) (top+bottom)/2;
        tbl = &tbl_top[position];
	result = memicmp(tbl->area_no,tel_no,tbl->length);

	if(!result)          /* found */
            return(tbl);

        if(result < 0)
        {
            tbl++;
	    result = memicmp(tbl->area_no, tel_no, tbl->length);

	    if(result == 0)
                return(tbl);
	    else if(result > 0)
		return(NULL);
	    else
		top = position+1;           /* SEARCH DOWN */
        }
        else
        {
            tbl--;

	    result = memicmp(tbl->area_no, tel_no, tbl->length);

            if(result == 0)
		return(tbl);                /* LOCATTED !!! */
	    else if(result < 0)
		return(NULL);
	    else
		bottom = position-1;        /* SEARCH UP    */
	}
    }    /* END OF FOR */

    return(NULL);
}
