// 1993 (c) ALL RIGHTS RESERVED
// AUTHOR:  XuYongYong

/* --------------------------------------------------------
  FILEDIAL.CPP
  Defines type TFileDialog.  This defines the basic
  behavior of all file dialogs.
  -------------------------------------------------------- */
#include "yyxmain.h"
#include "filedial.h"
#include "msgbox.h"
#include <string.h>
#include <dir.h>
#include <errno.h>
/*
char _FAR * _CType _FARFUNC _fullpath( char _FAR *__buf,
                  const char _FAR *__path,
                  size_t __maxlen );
*/
/*
extern "C" {
	char far* _fullpath (char far *__buf,const char far *__path,size_t __maxlen);
}
*/
int  filedial_class::get_sub_files (char *path_name,char **sub_files)
{
//	return nums; -1:Error
  struct ffblk ffblk;
  int done;
  int file_counter=0;

	done = findfirst(path_name,&ffblk, (FA_HIDDEN| FA_RDONLY |
		   FA_SYSTEM | FA_LABEL |FA_ARCH ) );
	if (done==ENOENT) return -1;
	while (!done)
	{
		if ( !(ffblk.ff_attrib & FA_DIREC)) {
//			strcpy ( ((FILENAMESTRING *)sub_files )[file_counter],ffblk.ff_name );
			strcpy (sub_files[file_counter],ffblk.ff_name );
			file_counter ++;
		}
		done = findnext(&ffblk);
	}
	return file_counter;
}

int  filedial_class::get_sub_dires (char *dir_name,char **sub_dires)
{
//return nums; -1:Error  ADDING [A][B][C][D] etc;
  struct ffblk ffblk;
  int done ,i;
  int dir_counter =0;
  char path[100];
//  char * disk[10]={"A:","B:","C:","D:","E:","F:" };
  char * disk[10]={"A:","B:","C:","D:","E:","F:" };

	strcpy(path,dir_name); //E.G "\dos\"
	strcat(path,"*.*");
	done = findfirst(path,&ffblk, (FA_DIREC /*| FA_HIDDEN| FA_RDONLY |
		   FA_SYSTEM | FA_LABEL |FA_ARCH */ ) );
	if (done==ENOENT) return -1;
	while (!done)
	{
		if ((ffblk.ff_attrib & FA_DIREC) && (strcmp(ffblk.ff_name,"."))) {
			strcpy (sub_dires[dir_counter],ffblk.ff_name );
			dir_counter ++;
		}
		done = findnext(&ffblk);
	}
	for (i=0;i< /*int disknum=setdisk(getdisk())*/ 4 ; i++ ) {
		strcpy (sub_dires[dir_counter],disk[i]);
		dir_counter ++;
	}
	return dir_counter;
}

BOOL  filedial_class::haswildcards(char *pathname)
{ register i;
	for (i=strlen (pathname)-1; (i>=0) && pathname[i] !='\\'; i-- )
		if ((pathname[i] =='\*') || (pathname[i] =='\?')) return TRUE;
	return FALSE;
}

char * filedial_class::getfilename(char *pathname)
{
  char *p;

	p = strrchr(pathname, '\\');
	if ( !p )
		p = strrchr(pathname, ':');
	if ( !p )
		return pathname;
	else
		return p + 1;
}

int   filedial_class::analyse_edit_filename(char *buf)
{
  struct ffblk ffblk;
  int done;
// input:: edit_filename
// if part path ,connect with dirname
	if ((! strchr(edit_filename,':') ) && (edit_filename[0] !='\\' )){
		strcpy (buf,dirname);
		strcat (buf,edit_filename);
		strcpy (edit_filename,buf);
	}
////////////////////////////////////// !!!!!!!!!! BC3.0
	_fullpath(buf, edit_filename, MAXPATH);
// now I should realize fullpath ,as .. \. etc in tc1.00
//	strcpy (buf,edit_filename);
/*  Now c:\dos\ ----> c:\dos
	but c:      ----> c:\
*/
	if  ( buf[strlen(buf)-1] == '\\' )  buf[strlen(buf)-1] ='\0';
/*  Now c:\dos\ ----> c:\dos
	and c:\     ----> c:
*/
	if ( haswildcards (edit_filename ) == TRUE ) {
		strcpy (wildcards, getfilename  (buf));
		(getfilename(buf))[0] ='\0';
		if  ( buf[strlen(buf)-1] == '\\' )  buf[strlen(buf)-1] ='\0';
	}  else { // split wildcards     from buf
	// assume wildcards not contains '\';
		if (!strcmp (wildcards,"*.*")) 	strcpy (wildcards,"*.*" );
	}
	done = findfirst( buf ,&ffblk, ( FA_DIREC| FA_HIDDEN| FA_RDONLY |
		   FA_SYSTEM | FA_LABEL |FA_ARCH  ) );
//	perror (sys_errlist[errno]);
	if ( (done)&&(buf[strlen(buf)-1]!=':') ) {
	//		; no such file or directory      c:\dos
	// since root directory is the exception c:
  char buf1[100];
		strcpy (buf1,buf);
		getfilename(buf1)[0] ='\0';
		strcat (buf1,"*.*");
		done = findfirst( buf1 ,&ffblk, FA_DIREC );
		if (done ) return 0;     //error path
		else 	return 3;          //true dirname, no file
		;
	}   else {	// exists such file or directory
		if( !(ffblk.ff_attrib & FA_DIREC) && (buf[strlen(buf)-1]!=':') ) {
								//buf is a existed file // ??
			return 1;
		} else {				// is an existing directory
			return 2;
		}
	}
}

BOOL  filedial_class:: change_edit_filename()
{
  char buf[100];
  int i=analyse_edit_filename(buf);

	strcpy (edit_filename,wildcards);

	switch (i) {
		case 0:
		case 3:	return FALSE;					//other information
		case 1:	strcpy (pathname ,buf );		//existed file
			strcpy (dirname, buf);
			strcat (dirname,"\\");
			getfilename(buf)[0] ='\0';
			return TRUE;
		case 2:									//existed directory
			strcat (buf,"\\");
			strcpy (dirname		,buf );  //dirname  use for sub_dires' search
			strcpy (pathname 	,buf );  //pathname use for sub_files' search
			strcat (pathname 	,wildcards );
			return FALSE;
	}
// NO need Return A value
	return FALSE;
}

filedial_class::filedial_class(int ID,char *title,int left,int top,int width,int height,
		int atype, char *apath )
	:dialog_class(ID,title,left,top,width,height)
{
  int i;

	achFile =apath;     // a Path For the last time
	dial_type =atype;
	for (i=0;i<MAXFILES;i++ ) { files[i]=new FILENAMESTRING;
		if ( files[i]==NULL) error("Less memory");
	}
	for (i=0;i<MAXDIRES;i++ ) { dires[i]=new FILENAMESTRING;
		if ( dires[i]==NULL) error("Less memory");
	}

//	putch('\007');
	dirname[0]='\0';
	pathname[0]='\0';
	strcpy (edit_filename,apath);
	change_edit_filename();

//	putch('\007');
	if (!(filesnum=get_sub_files (pathname,files ))) filesnum=0;
	if (!(diresnum=get_sub_dires (dirname,dires ))) diresnum=0;
//	putch('\007');

	insert_control(	dires_listscrl =new Tlistscrl (DIRES_ID,"&Directories",216,80,150,100,
			diresnum,dires) );
	insert_control(	files_listscrl =new Tlistscrl (FILES_ID,"&Files",16,80,160,100,
			filesnum,files) );
	if ( dial_type == SD_FILESAVE ) {
			files_listscrl->status |=INVISIBLE;
			files_listscrl->status |=DISABLE;
	}
	insert_control(	file_tedit =new Ttedit (EDIT_ID,"File&name",16,32,128,22 ,
			edit_filename, 25 ) );
	insert_control(	dir_tstatic =new Tstatic (13,"Directory",
			206,30,150,20, dirname ,ALIGN_LEFT ) );
	canclose =FALSE;
}

filedial_class::~filedial_class ()
{ int i;
	for (i=0;i<MAXFILES;i++ ) if (files[i]!=NULL )delete files[i];
	for (i=0;i<MAXDIRES;i++ ) if (dires[i]!=NULL )delete dires[i];
}


void filedial_class::updatelists( )
{
	change_edit_filename();
	if (!(filesnum=get_sub_files (pathname,files ))) filesnum=0;
	if (!(diresnum=get_sub_dires (dirname,dires ))) diresnum=0;

	strcpy (file_tedit->text,edit_filename);
	file_tedit ->update_control();

	strcpy (dir_tstatic->text, dirname);
	dir_tstatic ->draw ();

	files_listscrl->listbox->max_value =filesnum -1;
	files_listscrl->listbox->current_value=-1;
	files_listscrl ->update_control();

	dires_listscrl->listbox->max_value =diresnum -1;
	dires_listscrl->listbox->current_value=-1;
	dires_listscrl ->update_control();
}

BOOL filedial_class::disk_ready (int disk_num ) // 0,1,2,3,
{
  struct dfree adfree;

Redo:
	getdfree(disk_num+1, &adfree);
	if ( diskerrorno != 0 ) {
		if (theprogram->exec_dialog(new Tmsgbox("Message",msgbuf,MB_RETRYCANCEL))
			!= Dlg_OK ) { diskerrorno =0; return FALSE; }
		else { diskerrorno =0; goto Redo ;}
	}
	if ( adfree.df_sclus  == 0xFFFF ) return FALSE; else return TRUE;
//	On error df_sclus in the dfree structure to 0xFFFF
}

int filedial_class::msg_handler (MSG& message)
{ int done ;
  char buf[100];

	switch (message.Action) {
		case EditInputedMSG:
			if (message.fptr==file_tedit) {
				strcpy (edit_filename,file_tedit->text);
				if (( analyse_edit_filename(buf) ==1 ) ||
					((analyse_edit_filename(buf) ==3 )&&(dial_type==SD_FILESAVE))
				   )	{
//					thequeue.SendMessage(ok_button->ID,
//						ButtonPushedMSG,ok_button);
// Note::EditInputedMSG--------->
					return TRUE;
				} else {
					updatelists ();
					return TRUE;
				}
			}
			break;
// after you selected sth
		case ListBoxItemSelectedMSG:
 //			if (message.ID==((FILES_ID <<8)|222)) {
			if((message.fptr==files_listscrl->listbox) &&
			   (files_listscrl->listbox->current_value>=files_listscrl->listbox->min_value)&&
			   (files_listscrl->listbox->current_value<=files_listscrl->listbox->max_value )
			  )	{
//	now selected one file
				thequeue.SendMessage(ok_button->ID,
						ButtonPushedMSG,ok_button);
				return TRUE;
			} else
//			if (message.ID==((DIRES_ID <<8)|222)) {
			if((message.fptr==dires_listscrl->listbox) &&
			   (dires_listscrl->listbox->current_value>=dires_listscrl->listbox->min_value)&&
			   (dires_listscrl->listbox->current_value<=dires_listscrl->listbox->max_value )
			  )	{
//  int disknum =setdisk(getdisk());
  int disknum =4;	//NOte::when many disk exits
  int real_dir_num =dires_listscrl->listbox->max_value -disknum+1;
				if (dires_listscrl->listbox->current_value >=
					dires_listscrl->listbox->max_value -disknum+1
				   )  {	// A: B: etc
  int new_disk=dires_listscrl->listbox->current_value-real_dir_num;
					while ( !disk_ready(new_disk) )
						if (theprogram->exec_dialog(new Tmsgbox
							("Message","Disk Not Ready",MB_RETRYCANCEL)) ==Dlg_CANCEL)
							return TRUE;
					strcpy (dirname ,"");
				}		else  ; //NORMAL DIRECTORY
				strcpy (edit_filename,dirname);
				strcat (edit_filename,dires_listscrl->listbox->string_list[
					dires_listscrl->listbox->current_value]);
				updatelists ();
				return TRUE;
			}
 // As you are selecting
 // <--- ---> ^V etc
		case ListBoxValueChangedMSG:
			if((message.fptr==files_listscrl->listbox) &&
			   (files_listscrl->listbox->current_value>=files_listscrl->listbox->min_value)&&
			   (files_listscrl->listbox->current_value<=files_listscrl->listbox->max_value )
			  )	{
				strcpy (file_tedit->text,files_listscrl->listbox->string_list[
					files_listscrl->listbox->current_value]);
				file_tedit ->update_control();
//				return TRUE;	//For listscrl use
				break;
			} else
// FROM dires_listscrl, changed one selection
			if((message.fptr==dires_listscrl->listbox) &&
			   (dires_listscrl->listbox->current_value>=dires_listscrl->listbox->min_value)&&
			   (dires_listscrl->listbox->current_value<=dires_listscrl->listbox->max_value )
			  )	{
				strcpy (file_tedit->text,dires_listscrl->listbox->string_list[
					dires_listscrl->listbox->current_value]);
				if (file_tedit->text[strlen(file_tedit->text)-1] !=':')
					strcat (file_tedit->text, "\\");
				strcat (file_tedit->text,wildcards);
				file_tedit ->update_control();
//				return TRUE;	//For listscrl use
				break;
			}
	}
	return dialog_class::msg_handler (message);

}


BOOL filedial_class::func_canclose()
{
  char buf[100];

	strcpy (edit_filename, file_tedit->text);
	switch ( dial_type ) {
	case SD_FILESAVE :
		if ( ( analyse_edit_filename(buf) ==1 ) || (analyse_edit_filename(buf) ==3 )
		   ) {
			strcpy (achFile, buf );
			return TRUE;
		}; break;
	case SD_FILEOPEN :
		if ( analyse_edit_filename(buf) ==1 ) {
			strcpy (achFile, buf );
			return TRUE;
		}; break;
	} // switch
	theprogram->exec_dialog(new Tmsgbox
		("Message","Sorry, Please Select a File",MB_OK));
		return FALSE;
}