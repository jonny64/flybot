rem @echo off
set PATH=..\tools;%PATH%

rem cd to bat file directory: need when run from explorer
cd /d %~dp0
set wxWidgetsDir=e:\Code\wxWidgets-2.9.1
set currDir=.

copy /y "%wxWidgetsDir%\include\wx\msw\setup.h" "%wxWidgetsDir%\include\wx\msw\setup.h.bak"
copy /y setup.h "%wxWidgetsDir%\include\wx\msw\setup.h"

mklink /d "..\include" "%wxWidgetsDir%\include"

rem wxWidgets library project (build\msw\wx.dsw) also should be modified
rem see http://wiki.wxwidgets.org/Supporting_x64_and_Win32_within_one_solution
rem or you can apply Scripts\tag-2.9.0-multiplatform-support.patch
mklink /d "..\lib" "%wxWidgetsDir%\lib"

rem cd %wxWidgetsDir%
rem call %currDir%\VC2005_MultiTargetSupport.bat

mkdir "e:\tmp\My Dropbox\Public\flybot"
mklink /d "..\release" "e:\tmp\My Dropbox\Public\flybot"

pause
