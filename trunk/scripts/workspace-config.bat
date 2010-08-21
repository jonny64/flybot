rem @echo off
set PATH=..\tools;%PATH%

rem cd to bat file directory: need when run from explorer
cd /d %~dp0
set wxWidgetsDir=..\wxWidgets

cd "%wxWidgetsDir%"
del .\build\msw\wx_adv.vcxproj
del .\build\msw\wx_base.vcxproj
del .\build\msw\wx_core.vcxproj
del .\build\msw\wx_wxregex.vcxproj
del .\build\msw\wx.sln
del .\include\wx\setup.h
rem any tool with name patch requires admin rights
..\tools\patsh -p0 < ..\scripts\wxWidgets.2.9.1.vc2010.patch

cd "%wxWidgetsDir%\build\msw\"
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
devenv "wx.sln" /rebuild "Debug|x64"
devenv "wx.sln" /rebuild "Debug|Win32"
devenv "wx.sln" /rebuild "Release|x64"
devenv "wx.sln" /rebuild "Release|Win32"

mkdir "e:\tmp\My Dropbox\Public\flybot"
mklink /d "..\release" "e:\tmp\My Dropbox\Public\flybot"

pause
