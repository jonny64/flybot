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
echo OutFile ..\release\${VERSION}\flybot-${VERSION}.exe >> flybot.nsi
more flybot.nsi.template >> flybot.nsi
if not exist "..\release\%VERSION%" mkdir "..\release\%VERSION%"

echo packaging...
if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO :NONSIS
"%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi

"c:\Program Files\7-Zip\7z" a -tzip "..\release\%VERSION%\Chatbot.%VERSION%.pdb.zip" "..\bin\Chatbot.pdb"

:END
pause
exit

:NONSIS
echo post-build-release.bat : Tools error E0002 : You don't have NSIS installed. Aborting.
pause
exit -2

:ERROR
pause
exit %errorlevel%
