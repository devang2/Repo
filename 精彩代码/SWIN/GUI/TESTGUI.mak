.AUTODEPEND

#		*Translator Definitions*
CC = tcc +TESTGUI.CFG
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
 testgui.obj \
 gui.obj \
 event.obj \
 hzk.obj \
 hz.obj \
 mouse.obj \
 xms.obj \
 xmsa.obj

#		*Explicit Rules*
testgui.exe: testgui.cfg $(EXE_dependencies)
  $(TLINK) /v/x/c/L$(LIBPATH) @&&|
c0l.obj+
testgui.obj+
gui.obj+
event.obj+
hzk.obj+
hz.obj+
mouse.obj+
xms.obj+
xmsa.obj
testgui
		# no map file
graphics.lib+
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
testgui.obj: testgui.cfg testgui.cpp 

gui.obj: testgui.cfg gui.cpp 

event.obj: testgui.cfg ..\event\event.cpp 
	$(CC) -c ..\event\event.cpp

hzk.obj: testgui.cfg ..\hz\hzk.cpp 
	$(CC) -c ..\hz\hzk.cpp

hz.obj: testgui.cfg ..\hz\hz.cpp 
	$(CC) -c ..\hz\hz.cpp

mouse.obj: testgui.cfg ..\mouse\mouse.cpp 
	$(CC) -c ..\mouse\mouse.cpp

xms.obj: testgui.cfg ..\xms\xms.cpp 
	$(CC) -c ..\xms\xms.cpp

xmsa.obj: testgui.cfg ..\xms\xmsa.asm 
	$(TASM) /MX /ZI /O ..\XMS\XMSA.ASM,XMSA.OBJ

#		*Compiler Configuration File*
testgui.cfg: testgui.mak
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
| testgui.cfg


