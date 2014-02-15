.AUTODEPEND

#		*Translator Definitions*
CC = bcc +TESTEVET.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB
INCLUDEPATH = C:\BORLANDC\INCLUDE;..\TINCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 testevet.obj \
 event.obj \
 mouse.obj

#		*Explicit Rules*
testevet.exe: testevet.cfg $(EXE_dependencies)
  $(TLINK) /x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
testevet.obj+
event.obj+
mouse.obj
testevet
		# no map file
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
testevet.obj: testevet.cfg testevet.cpp 

event.obj: testevet.cfg event.cpp 

mouse.obj: testevet.cfg ..\mouse\mouse.cpp 
	$(CC) -c ..\mouse\mouse.cpp

#		*Compiler Configuration File*
testevet.cfg: testevet.mak
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
| testevet.cfg


