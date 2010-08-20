rem @echo off
set PATH=..\tools;%PATH%

rem cd to bat file directory: need when run from explorer
cd /d %~dp0
set wxWidgetsDir=..\wxWidgets

copy /y "%wxWidgetsDir%\include\wx\msw\setup.h" "%wxWidgetsDir%\include\wx\msw\setup.h.bak"
copy /y setup.h "%wxWidgetsDir%\include\wx\msw\setup.h"
copy /y setup.h "%wxWidgetsDir%\include\wx\setup.h"

cd "%wxWidgetsDir%\build\msw\"
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
devenv "wx.sln" "Win32" /rebuild "Debug|x64"
devenv "wx.sln" /rebuild "Debug|Win32"
devenv "wx.sln" /rebuild "Release|x64"
devenv "wx.sln" /rebuild "Release|Win32"

mkdir "e:\tmp\My Dropbox\Public\flybot"
mklink /d "..\release" "e:\tmp\My Dropbox\Public\flybot"

pause
