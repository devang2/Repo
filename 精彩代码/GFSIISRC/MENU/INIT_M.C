/******************************************************************/
/*   FILENAME: INIT_M.C                                           */
/*   LAST_MODI: 1993-3-4                                          */
/*   FUNCTION : init_menu(UC *struct_file, UC *text_file,         */
/*                        UC start_pos)                           */
/*   AUTHER :  ZHANG_RUI_JUN                                      */
/*   OTHERS :                                                     */
/******************************************************************/

#include <stdio.h>
#include <alloc.h>
#include <string.h>
#include <graphics.h>

#include <def.inc>
#include <func.inc>

#include "menu_def.inc"

HMENU *Menu_stru;
UC    *Menu[MAX_NODE];
UC    *Help[MAX_NODE];

UC init_menu1(UC *struct_file, UC *text_file, UC *help_file, UC start_pos)
{
FILE *fp_stru, *fp_text;	/* two file pointer */
UI 	 total_number;		/* the node number of the menu tree */
UI	 loopi;
UC 	 temp[60];		/* temp string for reading string */
UC	 str_len;

/* ---------------------- Open the structure files --------------- */
	fp_stru = fopen(struct_file,"rb");
	if(fp_stru == NULL)
		exit_scr(1, "\n\rINIT_M.C/init_menu(): File menu.str not exist!");
	fread(&total_number, sizeof(UI), 1, fp_stru);
	Menu_stru = (HMENU *)mem_alloc( sizeof(HMENU)*total_number );
	fread( Menu_stru, sizeof(HMENU), total_number, fp_stru );
	Menu_stru->curr = start_pos;
	fclose(fp_stru);

/* ------------------------ Open the text file ------------------------- */
	fp_text = fopen(text_file,"rt");
	if(fp_text == NULL)
		exit_scr(1, "\n\rINIT_M.C/init_menu(): File menu.txt not exist!");

	for(loopi=0; loopi<total_number; loopi++)
	{
		if (fgets(temp, 60, fp_text)==NULL)
		{
			printf("init_menu: FIle m_text.dat error!");
			return(FALSE);
		}
		str_len = strlen(temp) - 1; 	/* "-1" for get rid of \0D\0A */
		temp[str_len] = '\0';

		Menu[loopi] = (UC *)mem_alloc(str_len+1);
		strcpy(Menu[loopi],temp);
	}
	fclose(fp_text);

	fp_text = fopen(help_file,"rt");
	if(fp_text == NULL)
		exit_scr(1, "\n\rINIT_M.C/init_menu(): File menu.txt not exist!");

	for(loopi=0; loopi<total_number; loopi++)
	{
		if (fgets(temp, 60, fp_text)==NULL)
		{
			printf("init_menu: FIle m_text.dat error!");
			return(FALSE);
		}
		str_len = strlen(temp) - 1; 	/* "-1" for get rid of \0D\0A */
		temp[str_len] = '\0';

		Help[loopi] = (UC *)mem_alloc(str_len+1);
		strcpy(Help[loopi],temp);
	}
	fclose(fp_text);

	return(TRUE);
}

UC init_menu(UC *struct_file, UC *text_file, UC start_pos)
{
FILE *fp_stru, *fp_text;	/* two file pointer */
UI 	 total_number;		/* the node number of the menu tree */
UI	 loopi;
UC 	 temp[60];		/* temp string for reading string */
UC	 str_len;

/* ---------------------- Open the structure files --------------- */
	fp_stru = fopen(struct_file,"rb");
	if(fp_stru == NULL)
		exit_scr(1, "\n\rINIT_M.C/init_menu(): File menu.str not exist!");
	fread(&total_number, sizeof(UI), 1, fp_stru);
	Menu_stru = (HMENU *)mem_alloc( sizeof(HMENU)*total_number );
	fread( Menu_stru, sizeof(HMENU), total_number, fp_stru );
	Menu_stru->curr = start_pos;
	fclose(fp_stru);

/* ------------------------ Open the text file ------------------------- */
	fp_text = fopen(text_file,"rt");
	if(fp_text == NULL)
		exit_scr(1, "\n\rINIT_M.C/init_menu(): File menu.txt not exist!");

	for(loopi=0; loopi<total_number; loopi++)
	{
		if (fgets(temp, 60, fp_text)==NULL)
		{
			printf("init_menu: FIle m_text.dat error!");
			return(FALSE);
		}
		str_len = strlen(temp) - 1; 	/* "-1" for get rid of \0D\0A */
		temp[str_len] = '\0';

		Menu[loopi] = (UC *)mem_alloc(str_len+1);
		strcpy(Menu[loopi],temp);
	}
	fclose(fp_text);
/*
	fp_text = fopen(help_file,"rt");
	if(fp_text == NULL)
		exit_scr(1, "\n\rINIT_M.C/init_menu(): File menu.txt not exist!");

	for(loopi=0; loopi<total_number; loopi++)
	{
		if (fgets(temp, 60, fp_text)==NULL)
		{
			printf("init_menu: FIle m_text.dat error!");
			return(FALSE);
		}
		str_len = strlen(temp) - 1; 	/* "-1" for get rid of \0D\0A */
		temp[str_len] = '\0';

		Help[loopi] = (UC *)mem_alloc(str_len+1);
		strcpy(Help[loopi],temp);
	}
	fclose(fp_text);
*/
	return(TRUE);
}

