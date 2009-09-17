@echo off
if not exist "%PROGRAMFILES%\TortoiseSVN\bin\SubWCRev.exe" GOTO NOSVN:
cd "..\setup"
SubWCRev.exe ..\ verpatch.src verpatch.bat
set platformName = %1
call verpatch.bat platformName

echo generating packaging script...
echo !define VERSION %VERSION% > flybot.nsi
echo OutFile ..\release\${VERSION}\flybot-${VERSION}-x64.exe >> flybot.nsi
more flybot.x86_64.src >> flybot.nsi

echo packaging...
if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO NONSIS
"%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi

rem set botDir=..\setup\flybot-%VERSION%-x86_64
rem mkdir "%botDir%\Settings"
rem copy /y "..\bin\Chatbot.dll" "%botDir%"
rem copy /y "..\bin\Chatbot.mo" "%botDir%"
rem copy /y "..\Translation\Russian\flydict.ini" "%botDir%\Settings"
rem "c:\Program Files\7-Zip\7z" a -tzip "..\release\%VERSION%\flybot-%VERSION%-x86_64.zip" %botDir%

if not exist "..\release\%VERSION%" mkdir "..\release\%VERSION%"
"c:\Program Files\7-Zip\7z" a -tzip "..\release\%VERSION%\Chatbot.%VERSION%.x86_64.pdb.zip" "..\bin\Chatbot.pdb"
pause
exit

:NOSVN
echo You don't have Tortoise SVN installed. Aborting.
exit -1


:NONSIS
echo You don't have NSIS installed. Aborting.
exit -2
