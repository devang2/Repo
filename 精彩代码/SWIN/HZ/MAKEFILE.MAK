.AUTODEPEND

#		*Translator Definitions*
CC = bcc +TESTHZ.CFG
TASM = TASM
TLIB = tlib
TLINK = tlink
LIBPATH = C:\BORLANDC\LIB
INCLUDEPATH = C:\BORLANDC\INCLUDE;..\XMS


#		*Implicit Rules*
.c.obj:
  $(CC) -c {$< }

.cpp.obj:
  $(CC) -c {$< }

#		*List Macros*


EXE_dependencies =  \
 testhz.obj \
 xmsa.obj \
 xms.obj \
 hzk.obj \
 hz.obj

#		*Explicit Rules*
testhz.exe: testhz.cfg $(EXE_dependencies)
  $(TLINK) /x/c/P-/L$(LIBPATH) @&&|
c0l.obj+
testhz.obj+
xmsa.obj+
xms.obj+
hzk.obj+
hz.obj
testhz
		# no map file
graphics.lib+
emu.lib+
mathl.lib+
cl.lib
|


#		*Individual File Dependencies*
testhz.obj: testhz.cfg testhz.cpp 

xmsa.obj: testhz.cfg ..\xms\xmsa.asm 
	$(TASM) /MX /ZI /O ..\XMS\XMSA.ASM,XMSA.OBJ

xms.obj: testhz.cfg ..\xms\xms.cpp 
	$(CC) -c ..\xms\xms.cpp

hzk.obj: testhz.cfg hzk.cpp 

hz.obj: testhz.cfg hz.cpp 

#		*Compiler Configuration File*
testhz.cfg: makefile.mak
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
| testhz.cfg


