.AUTODEPEND

#		*Translator Definitions*
CC = tcc +ICOEDIT.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = D:\TC\LIB
INCLUDEPATH = D:\TC\INCLUDE;..\TINCLUDE


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 icoedit.obj \
 event.obj \
 mouse.obj \
 ..\gui\gui.lib

#		*Explicit Rules*
icoedit.exe: icoedit.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/L$(LIBPATH) @&&|
c0l.obj+
icoedit.obj+
event.obj+
mouse.obj
icoedit
		# no map file
..\gui\gui.lib+
graphics.lib+
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
icoedit.obj: icoedit.cfg icoedit.cpp 

event.obj: icoedit.cfg ..\event\event.cpp 
	$(CC) -c ..\event\event.cpp

mouse.obj: icoedit.cfg ..\mouse\mouse.cpp 
	$(CC) -c ..\mouse\mouse.cpp

#		*Compiler Configuration File*
icoedit.cfg: icoedit.mak
  copy &&|
-ml
-v
-vi-
-w-ret
-w-nci
-w-inl
-wpin
-wamb
-wamp
-w-par
-wasm
-wcln
-w-cpt
-wdef
-w-dup
-w-pia
-wsig
-wnod
-w-ill
-w-sus
-wstv
-wucp
-wuse
-w-ext
-w-ias
-w-ibc
-w-pre
-w-nst
-I$(INCLUDEPATH)
-L$(LIBPATH)
| icoedit.cfg


