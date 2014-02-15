# Microsoft Visual C++ generated build script - Do not modify

PROJ = SAMPLE
DEBUG = 0
PROGTYPE = 6
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\WEIYON~1\MINIGUI\SAMPLE\
USEMFC = 1
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = DIALOG.C    
FIRSTCPP = HELPER.CPP  
RC = rc
CFLAGS_D_DEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /D "_DOS" /FR /Fd"GTDATA.PDB"
CFLAGS_R_DEXE = /nologo /Gs /G3 /FPi87 /W3 /AM /O1 /Ob1 /D "NDEBUG" /D "_DOS" 
LFLAGS_D_DEXE = /NOLOGO /NOI /STACK:5120 /ONERROR:NOEXE /CO 
LFLAGS_R_DEXE = /NOLOGO /NOE /NOI /STACK:5120 /ONERROR:NOEXE 
LIBS_D_DEXE = mafxcrd oldnames mlibce 
LIBS_R_DEXE = mafxcr oldnames mlibc7 
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_DEXE)
LFLAGS = $(LFLAGS_D_DEXE)
LIBS = $(LIBS_D_DEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_DEXE)
LFLAGS = $(LFLAGS_R_DEXE)
LIBS = $(LIBS_R_DEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = DIALOG.SBR \
		HELPER.SBR \
		SUPPORT.SBR \
		MAINGUI.SBR \
		WNDPANES.SBR \
		SAMPLE.SBR


DIALOG_DEP = e:\weiyon~1\minigui\sample\common.h \
	e:\weiyon~1\minigui\sample\support.h \
	e:\weiyon~1\minigui\sample\maingui.h \
	e:\weiyon~1\minigui\sample\dialog.h


HELPER_DEP = e:\weiyon~1\minigui\sample\common.h \
	e:\weiyon~1\minigui\sample\support.h \
	e:\weiyon~1\minigui\sample\maingui.h \
	e:\weiyon~1\minigui\sample\dialog.h \
	e:\weiyon~1\minigui\sample\wndpanes.h \
	e:\weiyon~1\minigui\sample\helper.h


SUPPORT_DEP = e:\weiyon~1\minigui\sample\common.h \
	e:\weiyon~1\minigui\sample\support.h \
	e:\weiyon~1\minigui\sample\maingui.h \
	e:\weiyon~1\minigui\sample\dialog.h


MAINGUI_DEP = e:\weiyon~1\minigui\sample\common.h \
	e:\weiyon~1\minigui\sample\support.h \
	e:\weiyon~1\minigui\sample\maingui.h \
	e:\weiyon~1\minigui\sample\dialog.h


WNDPANES_DEP = e:\weiyon~1\minigui\sample\common.h \
	e:\weiyon~1\minigui\sample\support.h \
	e:\weiyon~1\minigui\sample\maingui.h \
	e:\weiyon~1\minigui\sample\dialog.h \
	e:\weiyon~1\minigui\sample\wndpanes.h \
	e:\weiyon~1\minigui\sample\helper.h


SAMPLE_DEP = e:\weiyon~1\minigui\sample\common.h \
	e:\weiyon~1\minigui\sample\support.h \
	e:\weiyon~1\minigui\sample\maingui.h \
	e:\weiyon~1\minigui\sample\dialog.h \
	e:\weiyon~1\minigui\sample\sample.h \
	e:\weiyon~1\minigui\sample\sample.rc


all:	$(PROJ).EXE

DIALOG.OBJ:	DIALOG.C $(DIALOG_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c DIALOG.C

HELPER.OBJ:	HELPER.CPP $(HELPER_DEP)
	$(CPP) $(CFLAGS) $(CPPCREATEPCHFLAG) /c HELPER.CPP

SUPPORT.OBJ:	SUPPORT.C $(SUPPORT_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c SUPPORT.C

MAINGUI.OBJ:	MAINGUI.C $(MAINGUI_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c MAINGUI.C

WNDPANES.OBJ:	WNDPANES.CPP $(WNDPANES_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c WNDPANES.CPP

SAMPLE.OBJ:	SAMPLE.CPP $(SAMPLE_DEP)
	$(CPP) $(CFLAGS) $(CPPUSEPCHFLAG) /c SAMPLE.CPP

$(PROJ).EXE::	DIALOG.OBJ HELPER.OBJ SUPPORT.OBJ MAINGUI.OBJ WNDPANES.OBJ SAMPLE.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
DIALOG.OBJ +
HELPER.OBJ +
SUPPORT.OBJ +
MAINGUI.OBJ +
WNDPANES.OBJ +
SAMPLE.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
d:\msvc\lib\+
d:\msvc\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
