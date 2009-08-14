@echo off
if not exist "%PROGRAMFILES%\TortoiseSVN\bin\SubWCRev.exe" GOTO NOSVN:
cd "..\setup"
SubWCRev.exe ..\ verpatch.src verpatch.bat
set platformName = %1
call verpatch.bat platformName

echo packaging...
rem if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO NONSIS
rem "%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi
rem TODO: http://nsis.download3000.com/nsis-2.26
rem below is temporary solution
set botDir=..\setup\flybot-%VERSION%-x86_64
mkdir "%botDir%\Settings"
copy /y "..\bin\Chatbot.dll" "%botDir%"
copy /y "..\bin\Chatbot.mo" "%botDir%"
copy /y "..\Translation\Russian\flydict.ini" "%botDir%\Settings"
"c:\Program Files\7-Zip\7z" a -tzip ..\setup\flybot-%VERSION%-x86_64.zip %botDir%
pause

:NOSVN
echo You don't have Tortoise SVN installed. Aborting.
exit -1


:NONSIS
echo You don't have NSIS installed. Aborting.
exit -2
