#ifndef __FILEDIAL_H
#define __FILEDIAL_H

#include "dialog.h"
#include "dir.h"
#include "yyxctrl.h"
#include "tedit.h"
#include "tstatic.h"
#include "listscrl.h"

#define FILESPEC	MAXPATH + MAXEXT
#define MAXFILES 150
#define MAXDIRES 150

#define SD_FILEOPEN 0
#define SD_FILESAVE 1

#define EDIT_ID   11
#define FILES_ID  22
#define DIRES_ID  33

typedef class filedial_class Tfiledial;

typedef char FILENAMESTRING[40];

class filedial_class :public Tdialog {
public:
// controls
	Ttedit    *file_tedit;
	Tstatic   *dir_tstatic;
	Tlistscrl *files_listscrl;
	Tlistscrl *dires_listscrl;

	char *achFile;		// used at Quit From the Dialog, as the Path of File
// attributes
	int  dial_type; //SD_FILEOPEN or SD_FILESAVE

	int  filesnum;
	int  diresnum;

	char *files[MAXFILES];
	char *dires[MAXDIRES];

//	LPSTR FilePath;

	char pathname [150];
	char dirname  [150];

	char wildcards [150];
	char edit_filename [150];

//	char PathName[MAXPATH];
//  char Extension[MAXEXT];
//	char FileSpec[FILESPEC];

public:
	filedial_class(int ID,char *title,int left,int top,int width,int height,
		int atype, char * apath);
virtual	~filedial_class ();
virtual int  msg_handler (MSG& message);
virtual BOOL func_canclose();

protected:
// used for the string operation
	int   	analyse_edit_filename(char *buf);
	BOOL  	filedial_class::change_edit_filename();
	BOOL 	filedial_class::disk_ready (int disk_num );

	void 	filedial_class::updatelists();
	BOOL  	haswildcards(char *pathname);
	char * 	getfilename(char *pathname);

	int  filedial_class::get_sub_dires (char *dir_name,char **sub_dires) ;
	int  filedial_class::get_sub_files (char *path_name,char **sub_files);
};


/*

SD_FILEOPEN DIALOG DISCARDABLE LOADONCALL PURE MOVEABLE 20, 24, 202, 130
STYLE WS_TILED | WS_CAPTION | WS_SYSMENU | DS_SETFONT | DS_MODALFRAME
CAPTION "File Open"
FONT 8, "Helv"
BEGIN
  CONTROL "File&name:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 6, 8, 36, 10
  CONTROL "" ID_FNAME, "EDIT", ES_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 42, 6, 98, 12
  CONTROL "Directory:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 6, 20, 36, 10
  CONTROL "" ID_FPATH, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 42, 20, 98, 10
  CONTROL "&Files:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 6, 32, 64, 10
  CONTROL "" ID_FLIST, "LISTBOX", WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_STANDARD, 6, 44, 64, 82
  CONTROL "&Directories:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 76, 32, 64, 10
  CONTROL "" ID_DLIST, "LISTBOX", WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_STANDARD, 76, 44, 64, 82
  CONTROL "&OK" IDOK, "BUTTON", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 146, 5, 50, 14
  CONTROL "&Cancel" IDCANCEL, "BUTTON", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 146, 23, 50, 14
END


SD_FILESAVE DIALOG DISCARDABLE LOADONCALL PURE MOVEABLE 20, 24, 202, 130
STYLE WS_TILED | WS_CAPTION | WS_SYSMENU | DS_SETFONT | DS_MODALFRAME
CAPTION "File Save As"
FONT 8, "Helv"
BEGIN
  CONTROL "File&name:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 6, 8, 36, 10
  CONTROL "" ID_FNAME, "EDIT", ES_OEMCONVERT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL, 42, 6, 98, 12
  CONTROL "Directory:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 6, 20, 36, 10
  CONTROL "" ID_FPATH, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 42, 20, 98, 10
  CONTROL "&Directories:" -1, "STATIC", WS_CHILD | WS_VISIBLE | WS_GROUP, 6, 32, 64, 10
  CONTROL "" ID_DLIST, "LISTBOX", WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_STANDARD, 6, 44, 64, 82
  CONTROL "&OK" IDOK, "BUTTON", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 146, 5, 50, 14
  CONTROL "&Cancel" IDCANCEL, "BUTTON", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 146, 23, 50, 14
END

*/

/*	TFileDialog *ptFileOpen;

	strcpy (achFile,"*.bmp");
	ptFileOpen =new TFileDialog (this,SD_FILEOPEN, achFile, NULL );
	iRetVal =GetApplication() -> ExecDialog ( ptFileOpen );

	if (iRetVal != IDOK ) return ;
*/

/*
	TFileDialog *ptFileSaveAs;

	strcpy (achFile,"*.*");
	ptFileSaveAs =new TFileDialog (this ,SD_FILESAVE, achFile, NULL );
	iRetVal =GetApplication() -> ExecDialog ( ptFileSaveAs );
	if (iRetVal == IDOK ) {
		MessageBox( Msg.Receiver, achFile, "  User Pressed Ok ", MB_OK );
	} else {
		MessageBox( Msg.Receiver, achFile, "  User Pressed Cancel ", MB_OK );
	}
*/

/*
class _EXPORT TFileDialog : public TDialog
{
public:
	LPSTR FilePath;
	char PathName[MAXPATH];
	char Extension[MAXEXT];
	char FileSpec[FILESPEC];

	TFileDialog(PTWindowsObject AParent, int ResourceId, LPSTR AFilePath,
				PTModule AModule = NULL);
	virtual BOOL CanClose();
	void SelectFileName();
	void UpdateFileName();
	BOOL UpdateListBoxes();

	static PTStreamable build();

protected:
	virtual void SetupWindow();
	virtual void HandleFName(RTMessage Msg)
		  = [ID_FIRST + ID_FNAME];
	virtual void HandleFList(RTMessage Msg)
		  = [ID_FIRST + ID_FLIST];
	virtual void HandleDList(RTMessage Msg)
		  = [ID_FIRST + ID_DLIST];

	TFileDialog(StreamableInit) : TDialog(streamableInit) {};

private:
	virtual const Pchar streamableName() const
		{ return "TFileDialog"; }
};


*/

#endif
