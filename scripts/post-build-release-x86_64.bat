@echo off
set PATH=..\tools;%PATH%
call "..\scripts\update-revision.bat"
if errorlevel 1 GOTO :END

cd "..\scripts"
set platformName=%1
call version-patch.bat platformName

echo generating packaging script...
call get-version-string.bat
cd "..\setup"
echo !define VERSION %VERSION% > flybot.nsi
rem %VERSION:"=% used to unquote var VERSION
set brandingText="%VERSION:"=% %platformName%"
echo BrandingText %brandingText% >> flybot.nsi
echo OutFile ..\release\${VERSION}\flybot-${VERSION}-x64.exe >> flybot.nsi
more flybot.x86_64.nsi.template >> flybot.nsi
if not exist "..\release\%VERSION%" mkdir "..\release\%VERSION%"


echo packaging...
if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO :NONSIS
"%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi

rem set botDir=..\setup\flybot-%VERSION%-x86_64
rem mkdir "%botDir%\Settings"
rem copy /y "..\bin\Chatbot.dll" "%botDir%"
rem copy /y "..\bin\Chatbot.mo" "%botDir%"
rem copy /y "..\Translation\Russian\flydict.ini" "%botDir%\Settings"
rem "c:\Program Files\7-Zip\7z" a -tzip "..\release\%VERSION%\flybot-%VERSION%-x86_64.zip" %botDir%

"c:\Program Files\7-Zip\7z" a -tzip "..\release\%VERSION%\Chatbot.%VERSION%.x86_64.pdb.zip" "..\bin\Chatbot.pdb"

:END
pause
exit

:NONSIS
echo post-build-release-x86_64.bat : Tools error E0002 : You don't have NSIS installed. Aborting.
pause
exit -2

:ERROR
pause
exit %errorlevel%
