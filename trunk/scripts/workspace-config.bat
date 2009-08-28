rem @echo off
set PATH=..\tools;%PATH%

rem cd to bat file directory: need when run from explorer
cd /d %~dp0
set wxWidgetsDir=e:\tmp\wxWidgets-2.8.10
set currDir=.

copy /y "%wxWidgetsDir%\include\wx\msw\setup.h" "%wxWidgetsDir%\include\wx\msw\setup.h.bak"
copy /y setup.h "%wxWidgetsDir%\include\wx\msw\setup.h"

mklink /d "..\include" "%wxWidgetsDir%\include"

rem wxWidgets library project (build\msw\wx.dsw) also should be modified
rem see http://wiki.wxwidgets.org/Supporting_x64_and_Win32_within_one_solution
mklink /d "..\lib" "%wxWidgetsDir%\lib"

rem cd %wxWidgetsDir%
rem call %currDir%\VC2005_MultiTargetSupport.bat

pause
