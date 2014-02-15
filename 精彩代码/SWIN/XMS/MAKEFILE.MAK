.AUTODEPEND

#		*Translator Definitions*
CC = bcc +TESTXMS.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB
INCLUDEPATH = C:\BORLANDC\INCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 testxms.obj \
 xms.obj \
 xmsa.obj

#		*Explicit Rules*
testxms.exe: testxms.cfg $(EXE_dependencies)
  $(TLINK) /x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
testxms.obj+
xms.obj+
xmsa.obj
testxms
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
testxms.obj: testxms.cfg testxms.cpp 

xms.obj: testxms.cfg xms.cpp 

xmsa.obj: testxms.cfg xmsa.asm 
	$(TASM) /MX /ZI /O XMSA.ASM,XMSA.OBJ

#		*Compiler Configuration File*
testxms.cfg: makefile.mak
  copy &&|
-ml
-3
-v
-vi-
-wpro
-weas
-wpre
-I$(INCLUDEPATH)
-L$(LIBPATH)
| testxms.cfg


