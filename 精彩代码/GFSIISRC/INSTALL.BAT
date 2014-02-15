@echo off
if "%3"=="" goto Usage

%2
md %3
cd %3
%1\arj x -y %1\gfsii
if not exist %2%3\gfsdata\ddd.rat %1\arj x %1\rate
copy c:\autoexec.bat c:\autoexec.gfs > nul
copy c:\config.sys c:\config.gfs >nul
copy %2%3\config.sys c:\  >nul
copy %2%3\autoexec.bat c:\ >nul
echo.
echo.
echo Please restart your computer!
echo.
goto End

:Usage
echo Usage: install source-drive destination-drive destination-path
echo        for example: install a:  c:  \gfsii

:End
echo on
