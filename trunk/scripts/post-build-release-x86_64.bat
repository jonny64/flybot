if not exist "%PROGRAMFILES%\TortoiseSVN\bin\SubWCRev.exe" GOTO NOSVN:
cd "..\setup"
SubWCRev.exe ..\ verpatch.src verpatch.bat
set platformName = %1
call verpatch.bat platformName

echo packaging...
if not exist "%PROGRAMFILES%\NSIS\makensis.exe" GOTO NONSIS
"%PROGRAMFILES%\NSIS\makensis" /v2 flybot.nsi
exit

:NOSVN
echo You don't have Tortoise SVN installed. Aborting.
exit -1


:NONSIS
echo You don't have NSIS installed. Aborting.
exit -2
