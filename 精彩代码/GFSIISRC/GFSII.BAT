deltree %2
md %2
md %2\gfsdata
xcopy %1\cbs.exe %2 
xcopy %1\config.sys %2 
xcopy %1\autoexec.bat %2 
xcopy %1\*.bgi %2 
xcopy %1\*.chr %2 
xcopy %1\gfsdata\*.lib %2\gfsdata 
xcopy %1\gfsdata\ntmenu.txt %2\gfsdata
xcopy %1\gfsdata\nthelp.txt %2\gfsdata
xcopy %1\gfsdata\ntmenu.idx %2\gfsdata
xcopy %1\gfsdata\lspy.dat %2\gfsdata
xcopy %1\gfsdata\lsdb.dat %2\gfsdata
xcopy %1\gfsdata\pyno.dat %2\gfsdata

rem multi serial ports
xcopy %1\4port.bat %2
xcopy %1\pccom.com %2
xcopy %1\setup.exe %2

rem  optional
xcopy %1\gfsdata\client.txt %2\gfsdata

if not "%3"=="" goto ARJ
xcopy %1\gfsdata\*.rat %2\gfsdata
goto End

:ARJ
copy %1\install.bat %3
copy %1\rate.arj %3
cd %2
arj a -r gfsii *.*
rem arj yf -je1 gfsii
copy \dos\arj.exe %3
copy gfsii.arj %3
rem copy gfsii.exe %3
label %3 gfsii30
:End

