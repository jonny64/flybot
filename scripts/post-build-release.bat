if not exist "%PROGRAMFILES%\TortoiseSVN\bin\SubWCRev.exe" goto NOSVN:
cd "..\setup"
SubWCRev.exe ..\ verpatch.src verpatch.bat
set platformName = %1
call verpatch.bat platformName

copy /y "..\Translation\Russian\*.mo" "..\bin"

echo packaging...
if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO NONSIS
"%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi
EXIT

:NOSVN
echo You don't have Tortoise SVN installed. Aborting.
exit -1


:NONSIS
echo You don't have NSIS installed. Aborting.
exit -2
